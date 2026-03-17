#ifndef __FLASHCARDAPP_H__
#define __FLASHCARDAPP_H__

#include "../../src/cxWindow.h"
#include "../../src/cxBase.h"
#include "../../src/cxKeyDefines.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

struct Card {
    std::string front;
    std::string back;
};

class FlashcardApp : public cxWindow {
public:
    FlashcardApp(int pHeight, int pWidth) : cxWindow(nullptr, 0, 0, pHeight, pWidth) {
        setTitle("Flashcard Example");
        center();
        mCurrentCardIdx = 0;
        mShowBack = false;
        loadCards("cards.txt");
        updateStatus();
    }

    void loadCards(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            cxBase::messageBox("Error", "Could not open " + filename);
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            size_t pos = line.find('|');
            if (pos != std::string::npos) {
                Card card;
                card.front = line.substr(0, pos);
                // Trim leading/trailing spaces
                card.front.erase(0, card.front.find_first_not_of(" "));
                card.front.erase(card.front.find_last_not_of(" ") + 1);
                
                card.back = line.substr(pos + 1);
                card.back.erase(0, card.back.find_first_not_of(" "));
                card.back.erase(card.back.find_last_not_of(" ") + 1);
                
                mCards.push_back(card);
            }
        }
        file.close();
    }

    void updateStatus() {
        if (mCards.empty()) {
            setStatus(" No cards loaded ");
        } else {
            std::string status = " Card " + std::to_string(mCurrentCardIdx + 1) + " of " + std::to_string(mCards.size()) + " ";
            status += " (?/h=help)";
            setStatus(status);
        }
    }

    void draw() {
        wclear(mWindow);
        cxWindow::draw();

        if (mCards.empty()) {
            writeText(height() / 2, (width() / 2) - 10, "No cards available.", false);
        } else {
            const Card& card = mCards[mCurrentCardIdx];
            std::string side = mShowBack ? "[ BACK ]" : "[ FRONT ]";
            std::string text = mShowBack ? card.back : card.front;

            writeText(2, (width() / 2) - (side.length() / 2), side, false);
            
            // Basic word wrap or just centering for now
            // If text is long, we might need more complex drawing, but for examples simple is fine
            if (text.length() < (size_t)width() - 4) {
                writeText(height() / 2, (width() / 2) - (text.length() / 2), text, false);
            } else {
                // Simple split if too long (very basic)
                writeText(height() / 2, 2, text.substr(0, width() - 4), false);
                if (text.length() > (size_t)width() - 4) {
                    writeText(height() / 2 + 1, 2, text.substr(width() - 4), false);
                }
            }
        }
        
        cxBase::updateWindows();
    }

    void showHelp() {
        cxBase::messageBox("Flashcard Help", 
            "Right Arrow : Next Card\n"
            "Left Arrow  : Previous Card\n"
            "Space       : Flip Card\n"
            "h or ?      : Show this help\n"
            "q or ESC    : Quit\n\n"
            "All Keys:\n"
            "- RIGHT     : Move to next card\n"
            "- LEFT      : Move to previous card\n"
            "- SPACE     : Toggle front/back\n"
            "- h / ?     : Show this help screen\n"
            "- q / ESC   : Exit the application");
    }

    void run() {
        keypad(mWindow, TRUE);
        
        if (mCards.empty()) {
            cxBase::messageBox("Flashcard", "No cards found in cards.txt.\nExiting.");
            return;
        }

        bool quit = false;
        while (!quit) {
            draw();
            int ch = wgetch(mWindow);
            switch (ch) {
                case KEY_RIGHT:
                    if (mCurrentCardIdx < mCards.size() - 1) {
                        mCurrentCardIdx++;
                        mShowBack = false;
                        updateStatus();
                    }
                    break;
                case KEY_LEFT:
                    if (mCurrentCardIdx > 0) {
                        mCurrentCardIdx--;
                        mShowBack = false;
                        updateStatus();
                    }
                    break;
                case ' ':
                    mShowBack = !mShowBack;
                    break;
                case 'h':
                case '?':
                    showHelp();
                    break;
                case 'q':
                case ESC:
                    quit = true;
                    break;
            }
        }
    }

private:
    std::vector<Card> mCards;
    size_t mCurrentCardIdx;
    bool mShowBack;
};

#endif
