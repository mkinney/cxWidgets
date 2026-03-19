#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/cxStringUtils.h"
#include <string>
#include <vector>

using namespace std;

TEST_CASE("cxStringUtils::TrimSpaces") {
    string s = "  hello  ";
    cxStringUtils::TrimSpaces(s);
    CHECK(s == "hello");

    s = "hello";
    cxStringUtils::TrimSpaces(s);
    CHECK(s == "hello");

    s = "   ";
    cxStringUtils::TrimSpaces(s);
    CHECK(s == "");

    s = "";
    cxStringUtils::TrimSpaces(s);
    CHECK(s == "");
}

TEST_CASE("cxStringUtils::strToUpper") {
    CHECK(cxStringUtils::strToUpper("hello") == "HELLO");
    CHECK(cxStringUtils::strToUpper("HELLO") == "HELLO");
    CHECK(cxStringUtils::strToUpper("HeLlO123") == "HELLO123");
}

TEST_CASE("cxStringUtils::strToLower") {
    CHECK(cxStringUtils::strToLower("HELLO") == "hello");
    CHECK(cxStringUtils::strToLower("hello") == "hello");
    CHECK(cxStringUtils::strToLower("HeLlO123") == "hello123");
}

TEST_CASE("cxStringUtils::Replace") {
    string s = "hello world";
    cxStringUtils::Replace(s, "world", "universe");
    CHECK(s == "hello universe");

    s = "abc abc abc";
    cxStringUtils::Replace(s, "abc", "def");
    CHECK(s == "def abc abc"); 
}

TEST_CASE("cxStringUtils::wrapText (from test_wordwrap.cpp)") {
    auto wrapText = [](const string& message, int innerWidth) {
        vector<string> mMessageLines;
        if (message == "") return mMessageLines;

        vector<string> iMessageLines;
        cxStringUtils::SplitStringRegex(message, "\n", iMessageLines);
        if (iMessageLines.empty() && !message.empty()) {
            iMessageLines.push_back(message);
        }

        for (const string& msgLine : iMessageLines) {
            if ((int)msgLine.length() <= innerWidth) {
                mMessageLines.push_back(msgLine);
            } else {
                string currentLine;
                vector<string> words;
                cxStringUtils::SplitStringRegex(msgLine, " ", words);
                for (const string& word : words) {
                    if ((int)word.length() >= innerWidth) {
                        if (!currentLine.empty()) {
                            if (currentLine.back() == ' ') currentLine.pop_back();
                            mMessageLines.push_back(currentLine);
                            currentLine = "";
                        }
                        mMessageLines.push_back(word.substr(0, innerWidth));
                    } else {
                        if (currentLine.length() + word.length() + 1 <= (size_t)innerWidth) {
                            if (!currentLine.empty()) currentLine += " ";
                            currentLine += word;
                        } else {
                            if (!currentLine.empty()) {
                                if (currentLine.back() == ' ') currentLine.pop_back();
                                mMessageLines.push_back(currentLine);
                            }
                            currentLine = word;
                        }
                    }
                }
                if (!currentLine.empty()) {
                    if (currentLine.back() == ' ') currentLine.pop_back();
                    mMessageLines.push_back(currentLine);
                }
            }
        }
        return mMessageLines;
    };

    SUBCASE("Case 1: Wrapping long line") {
        string text1 = "A high-level, interpreted, general-purpose programming language.";
        int innerWidth = 56;
        auto lines = wrapText(text1, innerWidth);
        
        bool languageSplit = false;
        for (auto& l : lines) {
            if (l == "l") languageSplit = true;
            CHECK((int)l.length() <= innerWidth);
        }
        CHECK(!languageSplit);
    }

    SUBCASE("Case 2: Docker text") {
        string text2 = "A set of platform as a service products that use OS-level virtualization to deliver software in packages called containers.";
        int innerWidth = 56;
        auto lines = wrapText(text2, innerWidth);
        for (auto& l : lines) {
            CHECK((int)l.length() <= innerWidth);
        }
    }
}
