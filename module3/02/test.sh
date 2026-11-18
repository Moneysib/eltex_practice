#!/bin/bash

DIR=$(cd "$(dirname "$0")" && pwd)
make -s -C "$DIR" || exit 1
BIN=$DIR/pubsub

WORK=$(mktemp -d)
cd "$WORK" || exit 1
cleanup() {
    kill $(jobs -p) 2>/dev/null
    rm -rf "$WORK"
}
trap cleanup EXIT

ok=0; bad=0
report() { if [ "$1" -eq 0 ]; then echo "[OK]   $2"; ok=$((ok+1)); else echo "[FAIL] $2"; bad=$((bad+1)); fi; }
has()  { grep -q -- "$2" "$1"; report $? "$3"; }
hasnt() { ! grep -q -- "$2" "$1"; report $? "$3"; }
alive() { kill -0 "$1" 2>/dev/null; }

echo "== Некорректные запуски =="
for args in "" "-x" "-b -b" "-p" "-p a b" "-s" "-p a,b" "-b тема"; do
    "$BIN" $args >/dev/null 2>&1; [ $? -eq 2 ]; report $? "аргументы '$args': код 2"
done

echo "== Без брокера =="
"$BIN" -p news 2>err.txt; rc=$?
[ $rc -ne 0 ] && [ -s err.txt ]; report $? "издатель без брокера: ошибка ($(cat err.txt))"
"$BIN" -s news 2>err.txt; rc=$?
[ $rc -ne 0 ] && [ -s err.txt ]; report $? "подписчик без брокера: ошибка"

echo "== Работа =="
"$BIN" -b > broker.log 2>&1 & BR=$!
sleep 1
"$BIN" -b > broker2.log 2>&1; rc=$?
[ $rc -ne 0 ] && [ -s broker2.log ]; report $? "второй брокер сообщает и завершается ($(cat broker2.log))"

"$BIN" -s news sport > s1.log 2>&1 & S1=$!
"$BIN" -s sport      > s2.log 2>&1 & S2=$!
sleep 1
"$BIN" -p news  > p1.log 2>&1 & P1=$!
"$BIN" -p sport > p2.log 2>&1 & P2=$!
sleep 3

has  s1.log "\[news\]"  "подписчик 1 получает тему news"
has  s1.log "\[sport\]" "подписчик 1 получает тему sport"
has  s2.log "\[sport\]" "подписчик 2 получает тему sport"
hasnt s2.log "\[news\]" "подписчик 2 не получает news"
has  broker.log "подписка $S1 на 'news'"  "брокер записал подписку на news"
has  broker.log "подписка $S1 на 'sport'" "брокер записал подписку на sport (несколько тем)"

kill -INT $S2; sleep 1
alive $S2; [ $? -ne 0 ]; report $? "подписчик 2 завершился по SIGINT"
has  broker.log "отписка $S2 от 'sport'" "брокер получил unsubscribe"
has  s2.log "завершение по сигналу" "подписчик 2 сообщил о завершении"

echo "== Завершение брокера =="
kill -INT $BR
for i in 1 2 3 4 5 6; do alive $BR || break; sleep 1; done
alive $BR; [ $? -ne 0 ]; report $? "брокер завершился"
sleep 1
alive $S1; [ $? -ne 0 ]; report $? "подписчик 1 завершился после SIGINT от брокера"
alive $P1; [ $? -ne 0 ]; report $? "издатель 1 завершился"
alive $P2; [ $? -ne 0 ]; report $? "издатель 2 завершился"
has  p1.log "завершение по сигналу" "издатель получил сигнал"
"$BIN" -p news >/dev/null 2>&1; [ $? -ne 0 ]; report $? "очередь удалена (новый издатель не подключается)"

echo "== Перезапуск брокера =="
"$BIN" -b > broker3.log 2>&1 & BR=$!
sleep 1
has broker3.log "Брокер запущен" "брокер стартует после удаления очереди"
kill -INT $BR; wait $BR 2>/dev/null

echo
echo "Пройдено: $ok, провалено: $bad"
[ "$bad" -eq 0 ]