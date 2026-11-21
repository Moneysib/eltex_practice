#!/bin/bash

make -s || exit 1

passed=0
failed=0

check() {
    local actual
    actual=$(printf "$2" | ./queue 2>&1 | grep -oE -- "$3")
    if [ "$actual" == "$4" ]; then
        echo "PASS: $1"
        passed=$((passed + 1))
    else
        echo "FAIL: $1"
        echo "  expected: $4"
        echo "  actual:   $actual"
        failed=$((failed + 1))
    fi
}

check "pop_first returns highest priority" \
    '1\n10\nlow\n1\n200\nhigh\n2\n0\n' 'pop_[a-z_]+ -> .*' \
    'pop_first -> [200] high'

check "FIFO inside one priority" \
    '1\n50\nfirst\n1\n50\nsecond\n2\n2\n0\n' 'pop_[a-z_]+ -> .*' \
    $'pop_first -> [ 50] first\npop_first -> [ 50] second'

check "pop_priority found" \
    '1\n10\na\n1\n20\nb\n3\n10\n0\n' 'pop_[a-z_]+ -> .*' \
    'pop_priority -> [ 10] a'

check "pop_priority not found" \
    '1\n10\na\n3\n99\n0\n' 'pop_[a-z_]+ -> .*' \
    'pop_priority -> not found'

check "pop_min_priority found" \
    '1\n10\na\n1\n150\nb\n4\n100\n0\n' 'pop_[a-z_]+ -> .*' \
    'pop_min_priority -> [150] b'

check "pop_min_priority not found" \
    '1\n10\na\n4\n100\n0\n' 'pop_[a-z_]+ -> .*' \
    'pop_min_priority -> not found'

check "pop from empty queue" \
    '2\n0\n' 'pop_[a-z_]+ -> .*' \
    'pop_first -> not found'

check "bad priority rejected" \
    '1\n300\nbad\n5\n0\n' 'Bad priority .*|Queue empty!' \
    $'Bad priority 300 (must be 0..255)\nQueue empty!'

check "extracted element is removed" \
    '1\n10\na\n2\n2\n0\n' 'pop_[a-z_]+ -> .*' \
    $'pop_first -> [ 10] a\npop_first -> not found'

echo "Passed: $passed, failed: $failed"
[ $failed -eq 0 ]