#!/bin/bash

DIR=$(cd "$(dirname "$0")" && pwd)
make -s -C "$DIR" || exit 1
BIN=$DIR/filecopy

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
cd "$WORK" || exit 1

ok=0; bad=0
report()
{
    if [ "$1" -eq 0 ]; then echo "[OK]   $2"; ok=$((ok+1))
    else echo "[FAIL] $2"; bad=$((bad+1)); fi
}

expect_rc()
{
    local want=$1 desc=$2; shift 2
    "$BIN" "$@" >/dev/null 2>err.txt
    local rc=$?
    [ "$rc" -eq "$want" ]; report $? "$desc (код $rc)"
}

same() { cmp -s "$1" "$1.copy"; report $? "копия '$1' совпадает"; }

echo "hello" > small.txt
: > empty.txt
head -c 4096    /dev/urandom > block.bin
head -c 4097    /dev/urandom > block1.bin
head -c 1000000 /dev/urandom > big.bin
echo "x" > "with space.txt"
mkdir dir

echo "== Неименованный канал =="
expect_rc 0 "несколько файлов" small.txt empty.txt block.bin block1.bin big.bin "with space.txt"
for f in small.txt empty.txt block.bin block1.bin big.bin "with space.txt"; do same "$f"; done
rm -f *.copy

echo "== Именованный канал (-p) =="
expect_rc 0 "-p mypipe" -p mypipe small.txt big.bin block1.bin
for f in small.txt big.bin block1.bin; do same "$f"; done
[ ! -e mypipe ] && [ ! -e mypipe.ack ]; report $? "FIFO удалены после работы"
rm -f *.copy

mkfifo mypipe mypipe.ack
expect_rc 0 "-p с уже существующими FIFO" -p mypipe small.txt
same small.txt
rm -f *.copy

echo "== Повторный запуск =="
echo "old" > small.txt.copy
expect_rc 0 "перезапись существующей копии" small.txt
same small.txt
rm -f *.copy

echo "== Ошибки: файлы =="
expect_rc 1 "несуществующий файл" nofile.txt
[ -s err.txt ]; report $? "сообщение в stderr: $(cat err.txt)"
[ ! -e nofile.txt.copy ]; report $? "копия несуществующего файла не создана"

expect_rc 1 "несуществующий файл в середине списка" small.txt nofile.txt block.bin
same small.txt; same block.bin
rm -f *.copy

expect_rc 1 "каталог вместо файла" dir
[ -s err.txt ]; report $? "сообщение в stderr: $(cat err.txt)"

if [ "$(id -u)" -ne 0 ]; then
    echo secret > noperm.txt; chmod 000 noperm.txt
    expect_rc 1 "нет прав на чтение" noperm.txt
fi

echo "== Ошибки: аргументы =="
expect_rc 2 "без аргументов"
expect_rc 2 "-p без имени" -p
expect_rc 2 "-p без файлов" -p mypipe
expect_rc 2 "неизвестный ключ" -z small.txt
touch notfifo
expect_rc 1 "-p указывает на обычный файл" -p notfifo small.txt

echo
echo "Пройдено: $ok, провалено: $bad"
[ "$bad" -eq 0 ]