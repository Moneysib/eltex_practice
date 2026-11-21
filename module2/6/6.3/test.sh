#!/bin/bash

make -s || exit 1

passed=0
failed=0

check() {
    local name="$1"
    local input="$2"
    local pattern="$3"
    local expected="$4"
    local args="${5:-}"

    local actual
    actual=$(printf -- "$input" | ./calc $args 2>&1 | grep -oE -- "$pattern")
    if [ "$actual" == "$expected" ]; then
        echo "PASS: $name"
        passed=$((passed + 1))
    else
        echo "FAIL: $name"
        echo "  expected: $expected"
        echo "  actual:   $actual"
        failed=$((failed + 1))
    fi
}

check "addition" \
    '2 + 3\nq\n' 'Результат: .*' \
    'Результат: 5.00000'

check "subtraction" \
    '10 - 4.5\nq\n' 'Результат: .*' \
    'Результат: 5.50000'

check "multiplication" \
    '2.5 * 4\nq\n' 'Результат: .*' \
    'Результат: 10.00000'

check "division" \
    '7 / 2\nq\n' 'Результат: .*' \
    'Результат: 3.50000'

check "multiple expressions and spaces" \
    '   1 + 2  \n  3 * 4 \nq\n' 'Результат: .*' \
    $'Результат: 3.00000\nРезультат: 12.00000'

check "negative and float numbers" \
    '-2.5 + 1.25\nq\n' 'Результат: .*' \
    'Результат: -1.25000'

check "division by zero" \
    '5 / 0\nq\n' 'Ошибка: .*' \
    'Ошибка: операция не выполнена (деление на ноль?)'

check "unknown operation" \
    '2 ^ 3\nq\n' 'Неизвестная операция .*' \
    "Неизвестная операция '^'"

check "invalid input: text instead of numbers" \
    'abc + def\nq\n' 'Ошибка: .*' \
    'Ошибка: нужно число, знак операции и число, например 2 + 3'

check "invalid input: trailing garbage" \
    '2 + 3 extra\nq\n' 'Ошибка: .*' \
    'Ошибка: нужно число, знак операции и число, например 2 + 3'

check "empty line ignored" \
    '\n   \n4 + 5\nq\n' 'Результат: .*' \
    'Результат: 9.00000'

check "missing plugins directory" \
    '' 'Операции не найдены' \
    'Операции не найдены' \
    'non_existent_dir'

tmp_dir=$(mktemp -d)
cp plugins/*.so "$tmp_dir/"
cat << 'EOF' > "$tmp_dir/pow.c"
#include "operation.h"
static int my_pow(double a, double b, double *res) {
    double r = 1.0;
    for (int i = 0; i < (int)b; i++) r *= a;
    *res = r;
    return 0;
}
const Operation operation = { '^', "возведение в степень", my_pow };
EOF
gcc -Wall -Wextra -std=c11 -I. -fPIC -shared "$tmp_dir/pow.c" -o "$tmp_dir/libpow.so"

check "custom plugin loaded from custom dir" \
    '2 ^ 5\nq\n' 'Результат: .*' \
    'Результат: 32.00000' \
    "$tmp_dir"

rm -rf "$tmp_dir"

echo "Passed: $passed, failed: $failed"
[ $failed -eq 0 ]