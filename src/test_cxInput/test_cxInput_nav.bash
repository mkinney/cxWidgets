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
run_test 1 "A:" "a" "a"

# if the value of a cxInput is "set", then ensure
# that we get that value back out (changed from test #1)
run_test 2 "A:" "b" "b"

# test that if you set it with a blank that it truly is
# a blank
run_test 3 "A:" "" ""

# test what happens if you enter more characters than
# what's in the field
run_test 4 "A:" "1234" "124"

# test the HOME key
run_test 5 "A:" "#HOME#ABC" "ABC"

# test the HOME key (as first char)
run_test 6 "A:" "A#HOME#BC" "BC" "should be:BC: from:A#HOME#BC:"

# test the HOME key (as a sequence in the middle)
run_test 7 "A:" "111#HOME#ABC" "ABC" "should be:ABC: from:111#HOME#ABC:"

# test the BACKSPACE key 
run_test 8 "A:" "ABC#BACK#D" "AD" "should be:AD: from:ABC#BACK#D:"

# test the RIGHT key 
run_test 9 "A:" "ABC#HOME##RIGHT##RIGHT#D" "ABD" "should be:ABD: from:ABC#HOME##RIGHT##RIGHT#D:"

# test the BACKSPACE key 
run_test 10 "A:" "ABC#LEFT##BACK#" "BC" "should be:BC: from:ABC#LEFT##BACK#:"