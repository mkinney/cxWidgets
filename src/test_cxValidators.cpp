#include "cxValidators.h"
#include <iostream>
#include <cassert>

using namespace std;
using namespace cxValidators;

void test_isValidWholeNumStr() {
    assert(isValidWholeNumStr("123"));
    assert(isValidWholeNumStr("0"));
    assert(isValidWholeNumStr(""));
    assert(!isValidWholeNumStr("12a3"));
    assert(!isValidWholeNumStr("12.3"));
    assert(isValidWholeNumStr("-123"));
    assert(!isValidWholeNumStr("12-3"));
    cout << "test_isValidWholeNumStr passed!" << endl;
}

void test_isValidFloatingPtNumStr() {
    assert(isValidFloatingPtNumStr("123"));
    assert(isValidFloatingPtNumStr("123.45"));
    assert(isValidFloatingPtNumStr("0.45"));
    assert(isValidFloatingPtNumStr(".45"));
    assert(isValidFloatingPtNumStr(""));
    assert(!isValidFloatingPtNumStr("12.34.56"));
    assert(!isValidFloatingPtNumStr("12a3"));
    cout << "test_isValidFloatingPtNumStr passed!" << endl;
}

int main() {
    cout << "Running cxValidators tests..." << endl;
    test_isValidWholeNumStr();
    test_isValidFloatingPtNumStr();
    cout << "All cxValidators tests passed!" << endl;
    return 0;
}
