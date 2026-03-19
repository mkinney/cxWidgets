#!/usr/bin/bash
# This tests HOME, END, LEFT and RIGHT keys in a cxInput (input text field)

run_test() {
    local test_num=$1
    local label=$2
    local input=$3
    local expected=$4
    local msg=$5

    ./test_cxInput.pl "$label" "$input"
    local value=`cat test_cxInput.log 2>/dev/null`
    if [ "$value" != "$expected" ]; then
        if [ -n "$msg" ]; then
            echo "Test $test_num - Failed. ($msg was:$value:)"
        else
            echo "Test $test_num - Failed. (value:$value:)"
        fi
        return 1
    else
        echo "Test $test_num - Passed."
        return 0
    fi
}

# if the value of a cxInput is "set", then ensure
# that we get that value back out
run_test 1 "A:" "a" "a" || exit 1

# if the value of a cxInput is "set", then ensure
# that we get that value back out (changed from test #1)
run_test 2 "A:" "b" "b" || exit 1

# test that if you set it with a blank that it truly is
# a blank
run_test 3 "A:" "" "" || exit 1

# test what happens if you enter more characters than
# what's in the field
# Old test that started failing with cxInput scrolling update:
#run_test 4 "A:" "1234" "124" || exit 1
# New test:
run_test 4 "A:" "1234" "1234" || exit 1

# test the HOME key
run_test 5 "A:" "#HOME#ABC" "ABC" || exit 1

# test the HOME key (as first char)
# Old test that was failing - Assumed it's in overwrite mode:
#run_test 6 "A:" "A#HOME#BC" "BC" "should be:BC: from:A#HOME#BC:" || exit 1
# Current test - The default is insert mode
run_test 6 "A:" "A#HOME#BC" "BCA" "should be:BCA: from:A#HOME#BC:" || exit 1

# test the HOME key (as a sequence in the middle)
# Old test that was failing - Assumed it's in overwrite mode:
#run_test 7 "A:" "111#HOME#ABC" "ABC" "should be:ABC: from:111#HOME#ABC:" || exit 1
# Current test - The default is insert mode
run_test 7 "A:" "111#HOME#ABC" "ABC111" "should be:ABC111: from:111#HOME#ABC:" || exit 1

# test the BACKSPACE key 
run_test 8 "A:" "ABC#BACK#D" "ABD" "should be:ABD: from:ABC#BACK#D:" || exit 1

# test the RIGHT key 
# Old test that was failing - Assumed it's in overwrite mode:
#run_test 9 "A:" "ABC#HOME##RIGHT##RIGHT#D" "ABD" "should be:ABD: from:ABC#HOME##RIGHT##RIGHT#D:" || exit 1
# Current test - The default is insert mode
run_test 9 "A:" "ABC#HOME##RIGHT##RIGHT#D" "ABDC" "should be:ABDC: from:ABC#HOME##RIGHT##RIGHT#D:" || exit 1

# test the BACKSPACE key 
run_test 10 "A:" "ABC#LEFT##BACK#" "AC" "should be:AC: from:ABC#LEFT##BACK#:" || exit 1
