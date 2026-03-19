#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/cxMessageDialog.h"
#include "../src/cxBase.h"
#include <string>
#include <deque>

class TestMessageDialog : public cxMessageDialog {
public:
    TestMessageDialog(cxWindow *pParentWindow,
                      const std::string& pTitle,
                      const std::string& pMessage)
        : cxMessageDialog(pParentWindow, pTitle, pMessage) {}

    int getActualWidth() const { return width(); }
    int getActualHeight() const { return height(); }
    const std::deque<std::string>& getMessageLines() const { return mMessageLines; }
};

TEST_CASE("cxMessageDialog sizing") {
    cxBase::init();
    
    std::string title = "Centipede Game";
    std::string message = "Arrows: Move\nSpace: Shoot\n'q' or ESC: Quit\n\nPress any key to start!";

    TestMessageDialog dialog(nullptr, title, message);

    CHECK(dialog.getActualWidth() == 25);
    CHECK(dialog.getActualHeight() == 10);
    CHECK(dialog.getMessageLines().size() == 5);
    CHECK(dialog.getMessageLines()[0] == "Arrows: Move");
    CHECK(dialog.getMessageLines()[4] == "Press any key to start!");

    cxBase::cleanup();
}
