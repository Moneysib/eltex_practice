#!/usr/bin/env bash
set -euo pipefail

BIN="./phonebook"
PASS=0
FAIL=0

make >/dev/null

fail() {
    echo "[FAIL] $1"
    echo "$2"
    FAIL=$((FAIL + 1))
}

pass() {
    echo "[PASS] $1"
    PASS=$((PASS + 1))
}

assert_contains() {
    local name="$1"
    local out="$2"
    local needle="$3"
    if ! grep -Fq -- "$needle" <<<"$out"; then
        fail "$name — не найдена строка: '$needle'" "$out"
        return 1
    fi
    return 0
}

assert_not_contains() {
    local name="$1"
    local out="$2"
    local bad="$3"
    if grep -Fq -- "$bad" <<<"$out"; then
        fail "$name — встречена лишняя строка: '$bad'" "$out"
        return 1
    fi
    return 0
}

clean_out() {
    sed 's/^> //'
}

out=$("$BIN" <<'EOF' | clean_out
2
4
1
3
1
0
EOF
)
if assert_contains "Пустое дерево и граничные индексы" "$out" "Список пуст" &&
   assert_contains "Пустое дерево и граничные индексы" "$out" "Ошибка: неверный номер или не хватило памяти" &&
   assert_contains "Пустое дерево и граничные индексы" "$out" "Нет такого контакта"; then
    pass "Пустое дерево, удаление и редактирование несуществующего контакта"
fi

out=$("$BIN" <<'EOF' | clean_out
1

Иван







1
Петров








2
0
EOF
)
if assert_contains "Обязательные поля" "$out" "Ошибка: фамилия и имя обязательны" &&
   assert_contains "Обязательные поля" "$out" "Список пуст" &&
   assert_not_contains "Обязательные поля" "$out" "Контакт добавлен"; then
    pass "Отказ при пустой фамилии или пустом имени"
fi

out=$("$BIN" <<'EOF' | clean_out
1
Смирнов
Алексей
Eltex
Инженер
+79001112233

a@example.com

@asmirnov

tg_a

1
Иванов
Борис






1
Иванов
Антон






1
Яковлев
Пётр






2
0
EOF
)
order=$(printf "%s\n" "$out" | awk '/^[0-9]+\. / { print }' | paste -sd '|' -)
expected="1. Иванов Антон|2. Иванов Борис|3. Смирнов Алексей|4. Яковлев Пётр"
if [[ "$order" == "$expected" ]]; then
    pass "Сортировка в дереве по фамилии и имени (in-order)"
else
    fail "Сортировка в дереве по фамилии и имени — получено: '$order', ожидалось: '$expected'" "$out"
fi

run_tree_scenario() {
    "$BIN" <<'EOF'
1
Абрамов
Сергей






1
Борисов
Олег






1
Волков
Денис






1
Громов
Игорь






1
Дроздов
Максим






1
Ежов
Пётр






1
Жуков
Роман






1
Зайцев
Лев






3
1
1
Янин
0
4
2
4
1
2
0
EOF
}

out=$(run_tree_scenario | clean_out)
after_reorder=$(printf "%s\n" "$out" | awk '/^[0-9]+\. / { n++; if (n >= 9 && n <= 16) print }' | paste -sd '|' -)
expected_after="1. Борисов Олег|2. Волков Денис|3. Громов Игорь|4. Дроздов Максим|5. Ежов Пётр|6. Жуков Роман|7. Зайцев Лев|8. Янин Сергей"
if [[ "$after_reorder" == "$expected_after" ]] &&
   grep -Fq -- "Контакт удалён" <<<"$out"; then
    pass "Добавление с периодической балансировкой, пересортировка и удаление"
else
    fail "Добавление с периодической балансировкой, пересортировка и удаление — порядок: '$after_reorder'" "$out"
fi

out=$("$BIN" <<'EOF' | clean_out
1
Козлов
Дмитрий
Завод
Мастер
+79990000001




3
1
3
-
5
+79990000002
+79990000003

0
2
0
EOF
)
final_view=$(printf "%s\n" "$out" | awk '
    /^1\. Козлов Дмитрий/ { block++; capture = (block == 2) }
    capture {
        if ($0 ~ /^1 - добавить/) exit
        print
    }
')
if assert_contains "Редактирование полей" "$final_view" "1. Козлов Дмитрий" &&
   assert_contains "Редактирование полей" "$final_view" "Телефон: +79990000002" &&
   assert_contains "Редактирование полей" "$final_view" "Телефон: +79990000003" &&
   assert_not_contains "Редактирование полей" "$final_view" "Работа: Завод"; then
    pass "Редактирование списков и очистка необязательного поля через '-'"
fi

if grep -q -- "-s" main.c contacts.h 2>/dev/null; then
    out_seed=$("$BIN" -s <<'EOF' | clean_out
2
0
EOF
)
    cnt_seed=$(printf "%s\n" "$out_seed" | grep -cE '^[0-9]+\. ' || true)
    if [[ "$cnt_seed" -ge 10 ]]; then
        pass "Запуск с флагом -s предзаполняет контакты ($cnt_seed шт.)"
    else
        fail "Запуск с флагом -s — слишком мало контактов: $cnt_seed" "$out_seed"
    fi
fi

echo ""
echo "Итого: passed=$PASS, failed=$FAIL"
[[ "$FAIL" -eq 0 ]]