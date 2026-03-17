#include "cxDate.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace std;

void test_Constructors() {
    // Default constructor (today)
    cxDate d1;
    cout << "Today is: " << d1.getYear() << "-" << d1.getMonth() << "-" << d1.getDay() << endl;

    // Explicit constructor
    cxDate d2(2023, 10, 5);
    assert(d2.getYear() == 2023);
    assert(d2.getMonth() == 10);
    assert(d2.getDay() == 5);

    // Julian date constructor (2460223 is 2023-10-05)
    cxDate d3(2460223L);
    assert(d3.getYear() == 2023);
    assert(d3.getMonth() == 10);
    assert(d3.getDay() == 5);

    // String constructor
    cxDate d4("2023-12-25", YYYY_MM_DD, '-');
    assert(d4.getYear() == 2023);
    assert(d4.getMonth() == 12);
    assert(d4.getDay() == 25);

    cout << "test_Constructors passed!" << endl;
}

void test_Arithmetic() {
    cxDate d(2023, 1, 1);
    
    d.addDays(31);
    assert(d.getYear() == 2023);
    assert(d.getMonth() == 2);
    assert(d.getDay() == 1);

    d.subtractDays(1);
    assert(d.getYear() == 2023);
    assert(d.getMonth() == 1);
    assert(d.getDay() == 31);

    d.addMonths(12);
    assert(d.getYear() == 2024);
    assert(d.getMonth() == 1);
    assert(d.getDay() == 31);

    // Leap year test
    cxDate leap(2024, 2, 28);
    leap.addDays(1);
    assert(leap.getMonth() == 2);
    assert(leap.getDay() == 29);
    leap.addDays(1);
    assert(leap.getMonth() == 3);
    assert(leap.getDay() == 1);

    cout << "test_Arithmetic passed!" << endl;
}

void test_Comparison() {
    cxDate d1(2023, 1, 1);
    cxDate d2(2023, 1, 1);
    cxDate d3(2023, 1, 2);
    cxDate d4(2024, 1, 1);

    assert(d1 == d2);
    assert(d1 != d3);
    assert(d3 > d1);
    assert(d4 > d3);
    assert(d1 < d3);

    cout << "test_Comparison passed!" << endl;
}

void test_Formatting() {
    cxDate d(2023, 10, 5, YYYY_MM_DD, '/');
    // We need to see how toString() is implemented or what it returns.
    // Based on header, it has toString()
}

int main() {
    cout << "Running cxDate tests..." << endl;
    test_Constructors();
    test_Arithmetic();
    test_Comparison();
    cout << "All cxDate tests passed!" << endl;
    return 0;
}
