#include "cxStringUtils.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

using namespace std;

void test_TrimSpaces() {
    string s = "  hello  ";
    cxStringUtils::TrimSpaces(s);
    assert(s == "hello");

    s = "hello";
    cxStringUtils::TrimSpaces(s);
    assert(s == "hello");

    s = "   ";
    cxStringUtils::TrimSpaces(s);
    assert(s == "");

    s = "";
    cxStringUtils::TrimSpaces(s);
    assert(s == "");
    cout << "test_TrimSpaces passed!" << endl;
}

void test_strToUpper() {
    assert(cxStringUtils::strToUpper("hello") == "HELLO");
    assert(cxStringUtils::strToUpper("HELLO") == "HELLO");
    assert(cxStringUtils::strToUpper("HeLlO123") == "HELLO123");
    cout << "test_strToUpper passed!" << endl;
}

void test_strToLower() {
    assert(cxStringUtils::strToLower("HELLO") == "hello");
    assert(cxStringUtils::strToLower("hello") == "hello");
    assert(cxStringUtils::strToLower("HeLlO123") == "hello123");
    cout << "test_strToLower passed!" << endl;
}

void test_Replace() {
    string s = "hello world";
    cxStringUtils::Replace(s, "world", "universe");
    assert(s == "hello universe");

    s = "abc abc abc";
    cxStringUtils::Replace(s, "abc", "def");
    assert(s == "def abc abc"); // Only replaces first occurrence based on implementation usually
    cout << "test_Replace passed!" << endl;
}

int main() {
    cout << "Running cxStringUtils tests..." << endl;
    test_TrimSpaces();
    test_strToUpper();
    test_strToLower();
    test_Replace();
    cout << "All cxStringUtils tests passed!" << endl;
    return 0;
}
