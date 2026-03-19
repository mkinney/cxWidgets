#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/cxValidators.h"

using namespace cxValidators;

TEST_CASE("isValidWholeNumStr") {
    CHECK(isValidWholeNumStr("123"));
    CHECK(isValidWholeNumStr("0"));
    CHECK(isValidWholeNumStr(""));
    CHECK(!isValidWholeNumStr("12a3"));
    CHECK(!isValidWholeNumStr("12.3"));
    CHECK(isValidWholeNumStr("-123"));
    CHECK(!isValidWholeNumStr("12-3"));
}

TEST_CASE("isValidFloatingPtNumStr") {
    CHECK(isValidFloatingPtNumStr("123"));
    CHECK(isValidFloatingPtNumStr("123.45"));
    CHECK(isValidFloatingPtNumStr("0.45"));
    CHECK(isValidFloatingPtNumStr(".45"));
    CHECK(isValidFloatingPtNumStr(""));
    CHECK(!isValidFloatingPtNumStr("12.34.56"));
    CHECK(!isValidFloatingPtNumStr("12a3"));
}
