#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/cxBase.h"
#include "../src/cxWindow.h"
#include "../src/cxInput.h"
#include "../src/cxMultiLineInput.h"
#include "../src/cxForm.h"
#include "../src/cxTextValidator.h"
#include <string>
#include <vector>

using namespace std;

TEST_CASE("cxWindow coordinates") {
    try {
        cxBase::init();
        
        SUBCASE("0 based") {
            cxWindow w(nullptr, 0, 0, 10, 20, "Test", "Test", "Test");
            CHECK(w.top()==0);
            CHECK(w.left()==0);
            CHECK(w.height()==10);
            CHECK(w.bottom()==9); 
            CHECK(w.right()==19);
            CHECK(w.width()==20);
            CHECK(w.centerRow()==4);
            CHECK(w.centerCol()==9);
        }

        SUBCASE("with offsets") {
            cxWindow w(nullptr, 1, 2, 10, 20, "Test", "Test", "Test");
            CHECK(w.top()==1);
            CHECK(w.left()==2);
            CHECK(w.height()==10);
            CHECK(w.bottom()==10);
            CHECK(w.width()==20);
            CHECK(w.right()==21);
            CHECK(w.centerRow()==5);
            CHECK(w.centerCol()==11);
        }

        cxBase::cleanup();
    } catch (const std::exception& e) {
        MESSAGE("Could not initialize ncurses or create window, skipping test: ", e.what());
    } catch (...) {
        MESSAGE("Could not initialize ncurses or create window, skipping test.");
    }
}

TEST_CASE("cxWindow auto centering") {
    try {
        cxBase::init();
        cxWindow w(nullptr, 0, 0, 24, 80, "Test", "Test", "Test");

        SUBCASE("parent centered") {
            int expectedCenterRow = (w.height() - 1) / 2;
            int expectedCenterCol = (w.width() - 1) / 2;
            CHECK(w.top()==0);
            CHECK(w.left()==0);
            CHECK(w.height()==24);
            CHECK(w.centerRow() == expectedCenterRow);
            CHECK(w.centerCol() == expectedCenterCol);
        }

        SUBCASE("auto centered subwindow") {
            cxWindow w2(&w, "X", "X", "X");
            int expectedTop = (w.height() - w2.height()) / 2 + w.top();
            int expectedLeft = (w.width() - w2.width()) / 2 + w.left();
            CHECK(w2.top() == expectedTop);
            CHECK(w2.left() == expectedLeft);
            CHECK(w2.height() == 3);
            CHECK(w2.width() == 3);
        }

        cxBase::cleanup();
    } catch (const std::exception& e) {
        MESSAGE("Could not initialize ncurses or create window, skipping test: ", e.what());
    } catch (...) {
        MESSAGE("Could not initialize ncurses or create window, skipping test.");
    }
}

TEST_CASE("cxInput basic") {
    try {
        cxBase::init();

        cxInput input1(nullptr, 0, 0, 10, "Name:");
        input1.setValue("This is a very long string.");
        CHECK(input1.getValue(false, false) == "This is a very long string.");
        cxBase::cleanup();
    } catch (const std::exception& e) {
        MESSAGE("Could not initialize ncurses or create window, skipping test: ", e.what());
    } catch (...) {
        MESSAGE("Could not initialize ncurses or create window, skipping test.");
    }
}

TEST_CASE("cxForm basic") {
    try {
        cxBase::init();
        cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
        aForm.append(1, 1, 1, 20, "Name:");
        aForm.append(2, 1, 1, 40, "City:");
        
        CHECK(aForm.numInputs() == 2);
        
        aForm.setValue("Name:", "Glen");
        CHECK(aForm.getValue("Name:") == "Glen");
        CHECK(aForm.getValue(0) == "Glen");

        aForm.remove("Name:");
        CHECK(aForm.numInputs() == 1);
        CHECK(aForm.inputLabel(0) == "City:");

        cxBase::cleanup();
    } catch (const std::exception& e) {
        MESSAGE("Could not initialize ncurses or create window, skipping test: ", e.what());
    } catch (...) {
        MESSAGE("Could not initialize ncurses or create window, skipping test.");
    }
}

TEST_CASE("cxTextValidator") {
    cxTextValidator iValidator;
    
    SUBCASE("digit validator") {
        iValidator.setTextStr("2");
        iValidator.setValidatorStr("d");
        CHECK(iValidator.textIsValid());
        
        iValidator.setValidatorStr("DD");
        CHECK_FALSE(iValidator.textIsValid());
    }

    SUBCASE("any number validator") {
        iValidator.setTextStr("2");
        iValidator.setValidatorStr("n");
        CHECK(iValidator.textIsValid());
    }

    SUBCASE("implied characters") {
        cxTextValidator v("5031234567", "(DDD) DDD-DDDD");
        CHECK(v.addImpliedChars() == "(503) 123-4567");
    }
}
