#ifndef __CENTIPEDEGAME_H__
#define __CENTIPEDEGAME_H__

#include "../../src/cxWindow.h"
#include "../../src/cxBase.h"
#include "../../src/cxKeyDefines.h"
#include <vector>
#include <list>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

struct Point {
    int x, y;
};

struct CentipedeSegment {
    Point pos;
    int dir; // 1 for right, -1 for left
    bool descending;
};

class CentipedeGame : public cxWindow {
public:
    CentipedeGame(int pHeight, int pWidth) : cxWindow(nullptr, 0, 0, pHeight, pWidth) {
        setTitle("Centipede Game");
        center();
        mGameOver = false;
        mScore = 0;
        
        mPlayerPos = {pWidth / 2, pHeight - 2};
        
        // Initial mushrooms
        for (int i = 0; i < 30; ++i) {
            spawnMushroom();
        }
        
        spawnCentipede(10);
    }

    void spawnMushroom() {
        int w = width() - 2;
        int h = height() - 4; // leave room for player area
        Point p;
        p.x = (std::rand() % w) + 1;
        p.y = (std::rand() % h) + 1;
        mMushrooms.push_back(p);
    }

    void spawnCentipede(int length) {
        int startX = width() / 2;
        int startY = 1;
        for (int i = 0; i < length; ++i) {
            CentipedeSegment seg;
            seg.pos = {startX - i, startY};
            seg.dir = 1;
            seg.descending = false;
            mCentipede.push_back(seg);
        }
    }

    void draw() {
        wclear(mWindow);
        cxWindow::draw();

        writeText(0, (width() / 2) - 8, " CENTIPEDE GAME ", false);

        // Draw mushrooms
        for (const auto& p : mMushrooms) {
            writeText(p.y, p.x, "*", false);
        }
        
        // Draw bullets
        for (const auto& p : mBullets) {
            writeText(p.y, p.x, "|", false);
        }
        
        // Draw centipede
        for (const auto& seg : mCentipede) {
            writeText(seg.pos.y, seg.pos.x, "O", false);
        }
        
        // Draw player
        writeText(mPlayerPos.y, mPlayerPos.x, "^", false);
        
        // Draw score
        std::string scoreStr = " Score: " + std::to_string(mScore) + " ";
        setStatus(scoreStr);
        
        if (mGameOver) {
            writeText(height() / 2, (width() / 2) - 5, "GAME OVER", false);
        }
        
        cxBase::updateWindows();
    }

    bool isMushroomAt(int x, int y) {
        for (const auto& p : mMushrooms) {
            if (p.x == x && p.y == y) return true;
        }
        return false;
    }

    void removeMushroomAt(int x, int y) {
        for (auto it = mMushrooms.begin(); it != mMushrooms.end(); ++it) {
            if (it->x == x && it->y == y) {
                mMushrooms.erase(it);
                return;
            }
        }
    }

    void update() {
        if (mGameOver) return;

        // Update bullets
        for (auto it = mBullets.begin(); it != mBullets.end(); ) {
            it->y--;
            bool hit = false;
            
            // Check mushroom collision
            if (isMushroomAt(it->x, it->y)) {
                removeMushroomAt(it->x, it->y);
                mScore += 5;
                hit = true;
            }
            
            // Check centipede collision
            if (!hit) {
                for (auto cit = mCentipede.begin(); cit != mCentipede.end(); ++cit) {
                    if (cit->pos.x == it->x && cit->pos.y == it->y) {
                        // Hit! Add mushroom at this position
                        mMushrooms.push_back(cit->pos);
                        mCentipede.erase(cit);
                        mScore += 10;
                        hit = true;
                        break;
                    }
                }
            }

            if (hit || it->y <= 0) {
                it = mBullets.erase(it);
            } else {
                ++it;
            }
        }

        // Update centipede
        if (mCentipede.empty()) {
            spawnCentipede(12); // Respawn slightly longer
            return;
        }

        for (size_t i = 0; i < mCentipede.size(); ++i) {
            auto& seg = mCentipede[i];
            
            if (seg.descending) {
                seg.pos.y++;
                seg.descending = false;
                if (seg.pos.y >= height() - 1) {
                    mGameOver = true;
                }
            } else {
                int nextX = seg.pos.x + seg.dir;
                if (nextX <= 0 || nextX >= width() - 1 || isMushroomAt(nextX, seg.pos.y)) {
                    seg.dir *= -1;
                    seg.descending = true;
                } else {
                    seg.pos.x = nextX;
                }
            }
            
            // Player collision
            if (seg.pos.x == mPlayerPos.x && seg.pos.y == mPlayerPos.y) {
                mGameOver = true;
            }
        }
    }

    void run() {
        std::srand(std::time(NULL));
        keypad(mWindow, TRUE);
        wtimeout(mWindow, 50); // Faster than snake

        cxBase::messageBox("Centipede Game", "Arrows: Move\nSpace: Shoot\n'q' or ESC: Quit\n\nPress any key to start!");

        while (!mGameOver) {
            int ch = wgetch(mWindow);
            switch (ch) {
                case KEY_UP:    if (mPlayerPos.y > height() - 5) mPlayerPos.y--; break;
                case KEY_DOWN:  if (mPlayerPos.y < height() - 2) mPlayerPos.y++; break;
                case KEY_LEFT:  if (mPlayerPos.x > 1) mPlayerPos.x--; break;
                case KEY_RIGHT: if (mPlayerPos.x < width() - 2) mPlayerPos.x++; break;
                case ' ': {
                    Point bullet = {mPlayerPos.x, mPlayerPos.y - 1};
                    mBullets.push_back(bullet);
                    break;
                }
                case 'q': case ESC: mGameOver = true; break;
            }

            update();
            draw();
        }

        nodelay(mWindow, FALSE);
        wgetch(mWindow);
    }

private:
    Point mPlayerPos;
    std::list<Point> mMushrooms;
    std::list<Point> mBullets;
    std::vector<CentipedeSegment> mCentipede;
    bool mGameOver;
    int mScore;
};

#endif
