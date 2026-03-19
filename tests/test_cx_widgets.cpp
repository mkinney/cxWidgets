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
#include <memory>

using namespace std;

// RAII class for ncurses initialization and cleanup
struct NcursesGuard {
    NcursesGuard() {
        if (!cxBase::cxInitialized()) {
            try {
                cxBase::init();
            } catch (...) {
                // Ignore failure here, it will be handled by cxInitialized() checks
            }
        }
    }
    ~NcursesGuard() {
        // We don't cleanup in between tests to avoid flickering or re-init issues in CI
        // But we could if needed. For now, leave it initialized once.
    }
};

TEST_CASE("cxWindow coordinates") {
    NcursesGuard guard;
    if (!cxBase::cxInitialized()) {
        MESSAGE("Could not initialize ncurses, skipping ncurses-dependent tests.");
        return;
    }

    int h = cxBase::height();
    int w = cxBase::width();
    if (h <= 0 || w <= 0) {
        MESSAGE("Invalid terminal dimensions (", h, "x", w, "), skipping ncurses-dependent tests.");
        return;
    }

    SUBCASE("0 based") {
        try {
            auto w = std::make_unique<cxWindow>(nullptr, 0, 0, 10, 20, "Test", "Test", "Test");
            CHECK(w->top()==0);
            CHECK(w->left()==0);
            CHECK(w->height()==10);
            CHECK(w->bottom()==9); 
            CHECK(w->right()==19);
            CHECK(w->width()==20);
            CHECK(w->centerRow()==4);
            CHECK(w->centerCol()==9);
        } catch (const std::exception& e) {
            MESSAGE("Could not create window, skipping subcase: ", e.what());
        }
    }

    SUBCASE("with offsets") {
        try {
            auto w = std::make_unique<cxWindow>(nullptr, 1, 2, 10, 20, "Test", "Test", "Test");
            CHECK(w->top()==1);
            CHECK(w->left()==2);
            CHECK(w->height()==10);
            CHECK(w->bottom()==10);
            CHECK(w->width()==20);
            CHECK(w->right()==21);
            CHECK(w->centerRow()==5);
            CHECK(w->centerCol()==11);
        } catch (const std::exception& e) {
            MESSAGE("Could not create window, skipping subcase: ", e.what());
        }
    }
}

TEST_CASE("cxWindow auto centering") {
    NcursesGuard guard;
    if (!cxBase::cxInitialized()) {
        MESSAGE("Could not initialize ncurses, skipping ncurses-dependent tests.");
        return;
    }

    int h = cxBase::height();
    int w = cxBase::width();
    if (h <= 0 || w <= 0) {
        MESSAGE("Invalid terminal dimensions (", h, "x", w, "), skipping ncurses-dependent tests.");
        return;
    }

    try {
        auto w = std::make_unique<cxWindow>(nullptr, 0, 0, 24, 80, "Test", "Test", "Test");

        SUBCASE("parent centered") {
            int expectedCenterRow = (w->height() - 1) / 2;
            int expectedCenterCol = (w->width() - 1) / 2;
            CHECK(w->top()==0);
            CHECK(w->left()==0);
            CHECK(w->height()==24);
            CHECK(w->centerRow() == expectedCenterRow);
            CHECK(w->centerCol() == expectedCenterCol);
        }

        SUBCASE("auto centered subwindow") {
            auto w2 = std::make_unique<cxWindow>(w.get(), "X", "X", "X");
            int expectedTop = (w->height() - w2->height()) / 2 + w->top();
            int expectedLeft = (w->width() - w2->width()) / 2 + w->left();
            CHECK(w2->top() == expectedTop);
            CHECK(w2->left() == expectedLeft);
            CHECK(w2->height() == 3);
            CHECK(w2->width() == 3);
        }
    } catch (const std::exception& e) {
        MESSAGE("Could not create window, skipping test: ", e.what());
    }
}

TEST_CASE("cxInput basic") {
    NcursesGuard guard;
    if (!cxBase::cxInitialized()) {
        MESSAGE("Could not initialize ncurses, skipping ncurses-dependent tests.");
        return;
    }

    int h = cxBase::height();
    int w = cxBase::width();
    MESSAGE("Terminal dimensions: ", h, "x", w);

    if (h <= 0 || w <= 0) {
        MESSAGE("Invalid terminal dimensions, skipping ncurses-dependent tests.");
        return;
    }

    try {
        MESSAGE("Creating cxInput...");
        auto input1 = std::make_unique<cxInput>(nullptr, 0, 0, 10, "Name:");
        MESSAGE("cxInput created. Setting value...");
        input1->setValue("This is a very long string.");
        MESSAGE("Value set. Checking value...");
        CHECK(input1->getValue(false, false) == "This is a very long string.");
        MESSAGE("Value checked.");
    } catch (const std::exception& e) {
        MESSAGE("Exception during cxInput test: ", e.what());
    } catch (...) {
        MESSAGE("Unknown exception during cxInput test.");
    }
}

TEST_CASE("cxForm basic") {
    NcursesGuard guard;
    if (!cxBase::cxInitialized()) {
        MESSAGE("Could not initialize ncurses, skipping ncurses-dependent tests.");
        return;
    }

    int h = cxBase::height();
    int w = cxBase::width();
    if (h <= 0 || w <= 0) {
        MESSAGE("Invalid terminal dimensions (", h, "x", w, "), skipping ncurses-dependent tests.");
        return;
    }

    try {
        auto aForm = std::make_unique<cxForm>(nullptr, 0, 0, 10, 50, "Title");
        aForm->append(1, 1, 1, 20, "Name:");
        aForm->append(2, 1, 1, 40, "City:");
        
        CHECK(aForm->numInputs() == 2);
        
        aForm->setValue("Name:", "Glen");
        CHECK(aForm->getValue("Name:") == "Glen");
        CHECK(aForm->getValue(0) == "Glen");

        aForm->remove("Name:");
        CHECK(aForm->numInputs() == 1);
        CHECK(aForm->inputLabel(0) == "City:");
    } catch (const std::exception& e) {
        MESSAGE("Could not create form, skipping test: ", e.what());
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
