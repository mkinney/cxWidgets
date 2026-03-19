#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/cxDate.h"
#include <string>

TEST_CASE("cxDate Constructors") {
    SUBCASE("Default constructor") {
        cxDate d1;
        // Just verify it doesn't crash and has some values
        CHECK(d1.getYear() > 0);
    }

    SUBCASE("Explicit constructor") {
        cxDate d2(2023, 10, 5);
        CHECK(d2.getYear() == 2023);
        CHECK(d2.getMonth() == 10);
        CHECK(d2.getDay() == 5);
    }

    SUBCASE("Julian date constructor") {
        cxDate d3(2460223L);
        CHECK(d3.getYear() == 2023);
        CHECK(d3.getMonth() == 10);
        CHECK(d3.getDay() == 5);
    }

    SUBCASE("String constructor") {
        cxDate d4("2023-12-25", YYYY_MM_DD, '-');
        CHECK(d4.getYear() == 2023);
        CHECK(d4.getMonth() == 12);
        CHECK(d4.getDay() == 25);
    }
}

TEST_CASE("cxDate Arithmetic") {
    cxDate d(2023, 1, 1);
    
    SUBCASE("addDays") {
        d.addDays(31);
        CHECK(d.getYear() == 2023);
        CHECK(d.getMonth() == 2);
        CHECK(d.getDay() == 1);
    }

    SUBCASE("subtractDays") {
        d.addDays(31); // 2023-02-01
        d.subtractDays(1);
        CHECK(d.getYear() == 2023);
        CHECK(d.getMonth() == 1);
        CHECK(d.getDay() == 31);
    }

    SUBCASE("addMonths") {
        d.addMonths(12);
        CHECK(d.getYear() == 2024);
        CHECK(d.getMonth() == 1);
        CHECK(d.getDay() == 1);
    }

    SUBCASE("Leap year") {
        cxDate leap(2024, 2, 28);
        leap.addDays(1);
        CHECK(leap.getMonth() == 2);
        CHECK(leap.getDay() == 29);
        leap.addDays(1);
        CHECK(leap.getMonth() == 3);
        CHECK(leap.getDay() == 1);
    }
}

TEST_CASE("cxDate Comparison") {
    cxDate d1(2023, 1, 1);
    cxDate d2(2023, 1, 1);
    cxDate d3(2023, 1, 2);
    cxDate d4(2024, 1, 1);

    CHECK(d1 == d2);
    CHECK(d1 != d3);
    CHECK(d3 > d1);
    CHECK(d4 > d3);
    CHECK(d1 < d3);
}
