// Copyright (c) 2005-2007 Michael H. Kinney
//
// Note: When writing text to the subwindow, make sure
//  you turn scrollok off before writing, and then turn
//  scrollok back on after you're done writing.
//  (scrollok(mSubWindow, false), scrollok(mSubWindow, true))

#include "cxScrolledWindow.h"
#include "cxStringUtils.h"
#include "cxMultiLineInput.h"
#include "cxBase.h"
#include "cxValidators.h"
using cxStringUtils::strToLower;
using std::string;
using std::shared_ptr;

cxScrolledWindow::cxScrolledWindow(cxWindow *pParentWindow,
             int pRow, int pCol, int pHeight, int pWidth,
             const string& pTitle, const string& pMessage,
             const string& pStatus, eBorderStyle pBorderStyle,
             cxWindow *pExtTitleWindow, cxWindow *pExtStatusWindow,
             bool pHotkeyHighlighting)
   : cxWindow(pParentWindow, pRow, pCol, pHeight, pWidth,
              pTitle, pMessage, pStatus, pBorderStyle,
              pExtTitleWindow, pExtStatusWindow, pHotkeyHighlighting)
{
   // Note: cxWindow may change the height and width if they
   //  are <= 0..
   pHeight = height();
   pWidth = width();

   // Note: For some reason, this class' overridden init() doesn't
   //  get called by cxWindow, even though it's virtual in cxWindow,
   //  so we have to call it again here (this may not be really
   //  efficient..  we shouldn't have to call it here, but it seems
   //  that we do.)
   init(pRow, pCol, pHeight, pWidth, pTitle, pMessage, pStatus);
   reCreateSubWindow();
}

cxScrolledWindow::cxScrolledWindow(cxWindow *pParentWindow,
               int pRow, int pCol, const string& pTitle,
               const string& pMessage, const string& pStatus,
               cxWindow *pExtTitleWindow,
               cxWindow *pExtStatusWindow,
               bool pHotkeyHighlighting)
   : cxWindow(pParentWindow, pRow, pCol, pTitle, pMessage,
              pStatus, pExtTitleWindow, pExtStatusWindow,
              pHotkeyHighlighting)
{
   // Note: For some reason, this class' overridden init() doesn't
   //  get called by cxWindow, even though it's virtual in cxWindow,
   //  so we have to call it again here (this may not be really
   //  efficient..  we shouldn't have to call it here, but it seems
   //  that we do.)
   init(pRow, pCol, 0, 0, pTitle, pMessage, pStatus);
   reCreateSubWindow();
}

cxScrolledWindow::cxScrolledWindow(cxWindow *pParentWindow,
               const string& pTitle, const string& pMessage,
               const string& pStatus,
               cxWindow *pExtTitleWindow,
               cxWindow *pExtStatusWindow,
               bool pHotkeyHighlighting)
   : cxWindow(pParentWindow, pTitle, pMessage, pStatus,
              pExtTitleWindow, pExtStatusWindow, pHotkeyHighlighting)
{
   init(0, 0, 0, 0, pTitle, pMessage, pStatus);
   center();
   reCreateSubWindow();
}

cxScrolledWindow::cxScrolledWindow(cxWindow *pParentWindow,
               const string& pMessage, const string& pStatus,
               cxWindow *pExtTitleWindow,
               cxWindow *pExtStatusWindow,
               bool pHotkeyHighlighting)
   : cxWindow(pParentWindow, pMessage, pStatus, pExtTitleWindow,
              pExtStatusWindow, pHotkeyHighlighting)
{
   init(0, 0, 0, 0, "", pMessage, pStatus);
   center();
   reCreateSubWindow();
}

cxScrolledWindow::cxScrolledWindow(cxWindow *pParentWindow,
               const string& pMessage, cxWindow *pExtTitleWindow,
               cxWindow *pExtStatusWindow, bool pHotkeyHighlighting)
   : cxWindow(pParentWindow, pMessage, pExtTitleWindow, pExtStatusWindow,
              pHotkeyHighlighting)
{
   init(0, 0, 0, 0, "", pMessage, "");
   center();
   reCreateSubWindow();
}

cxScrolledWindow::cxScrolledWindow(cxWindow *pParentWindow, eHPosition pHPosition,
               const string& pTitle, const string& pMessage,
               const string& pStatus, cxWindow *pExtTitleWindow,
               cxWindow *pExtStatusWindow, bool pHotkeyHighlighting)
   : cxWindow(pParentWindow, pHPosition, pTitle, pMessage,
              pStatus, pExtTitleWindow, pExtStatusWindow, pHotkeyHighlighting)
{
   int newRow, newCol;

   getRowColBasedOn(pParentWindow, pHPosition, pTitle, pStatus, pMessage, newRow, newCol);

   init(newRow, newCol, 0, 0, pTitle, pMessage, pStatus);
   reCreateSubWindow();
}

cxScrolledWindow::cxScrolledWindow(const cxScrolledWindow& pThatWindow)
   : cxWindow(pThatWindow.getParent(), pThatWindow.top(), pThatWindow.left(),
              pThatWindow.height(), pThatWindow.width(), pThatWindow.getTitle(),
              "", pThatWindow.getStatus(), pThatWindow.getBorderStyle(),
              pThatWindow.getExtTitleWindow(), pThatWindow.getExtStatusWindow(),
              pThatWindow.getHotkeyHighlighting())
{
   init(pThatWindow.top(), pThatWindow.left(), pThatWindow.height(),
        pThatWindow.width(), pThatWindow.getTitle(), pThatWindow.getMessage(),
        pThatWindow.getStatus());
   reCreateSubWindow();
}

cxScrolledWindow::~cxScrolledWindow()
{
   if (mSubWindow != nullptr)
   {
      delwin(mSubWindow);
      mSubWindow = nullptr;
   }
}

long cxScrolledWindow::show(bool pBringToTop, bool pShowSubwindows)
{
   long retval = cxID_EXIT;

   // Only do this if the window is enabled.
   if (isEnabled())
   {
      drawBorder();
      drawTitle();
      drawStatus();
      // Note: The horizontal scroll arrows need to be added after drawStatus()
      //  because drawStatus() draws the whole bottom border.
      drawHorizontalScrollArrows();
      drawMessage();

      // Make sure this window is not hidden
      unhide();
      wrefresh(mSubWindow);
      if (pBringToTop)
      {
         bringToTop();
      }
      else
      {
         // Update the physical screen (bringToTop() calls doUpdate(), so that
         //  would've been done if pBringToTop was true).
         doupdate();
      }
   }
   else
   {
      hide(false);
   }

   return(retval);
} // show

long cxScrolledWindow::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   setReturnCode(cxID_EXIT);

   // Only do the input loop if the window is enabled.
   if (isEnabled())
   {
      mIsModal = true;

      // Run the onFocus function.  If runOnFocusFunction() returns true, that
      //  means we should exit.. so only do the input loop if it returns false.
      //  Also, check to make sure that getLeaveNow() returns false, in case
      //  the onFocus function called exitNow() or quitNow().
      if (!runOnFocusFunction() && !getLeaveNow())
      {
         if (pShowSelf)
         {
            show(pBringToTop, pShowSubwindows);
         }
         // Disable the cursor, and save the current cursor state so that it
         //  be set back later.
         int oldCursorState = curs_set(0);
         bool runOnLeaveFunc = true;
         setReturnCode(doInputLoop(runOnLeaveFunc));
         // Set the cursor state back to what it was
         if (oldCursorState != ERR)
         {
            curs_set(oldCursorState);
         }
         mIsModal = false;
         // Run the onLeave function
         if (runOnLeaveFunc)
         {
            runOnLeaveFunction();
         }
      }
      else
      {
         mIsModal = false;
      }
   }

   return(getReturnCode());
} // showModal

void cxScrolledWindow::scrollWin(int pVertScrollAmt, int pHorizScrollAmt, bool pRefresh)
{
   // Scroll vertically
   if (pVertScrollAmt != 0)
   {
      // Scroll by pVertScrollAmt
      mLineNumber += pVertScrollAmt;

      // If mLineNumber is negative, or if there are less lines in mMessageLines
      //  than can fill the subwindow, then mLineNumber should be 0 (don't scroll
      //  out of bounds).
      if ((mLineNumber < 0) || (mMessageLines.size() < (unsigned)mSubWinHeight))
      {
         mLineNumber = 0;
      }
      else
      {
         if (mMessageLines.size() >= (unsigned)mSubWinHeight)
         {
            // Only let them scroll down to the last page worth of the file.
            if (mLineNumber > ((int)mMessageLines.size() - mSubWinHeight))
            {
               mLineNumber = (int)mMessageLines.size() - mSubWinHeight;
            }
         }
      }
   }
   // Scroll horizontally
   if (pHorizScrollAmt != 0)
   {
      mHScrollOffset += pHorizScrollAmt;
      if (mHScrollOffset < 0)
      {
         mHScrollOffset = 0;
      }
   }

   // Refresh the window if there was any scrolling and the user wants to
   //  refresh.
   if ((pVertScrollAmt != 0) || (pHorizScrollAmt != 0))
   {
      drawMessage();
      if (pRefresh)
      {
         wrefresh(mSubWindow);
      }
   }
} // scrollWin

void cxScrolledWindow::drawMessage()
{
   // If the window is big enough, then do it.
   if ((mSubWinHeight >= 1) && (mSubWinWidth >= 1))
   {
      // Enable the message attributes and message color.
      enableAttrs(mSubWindow, eMESSAGE);
      if (useColors)
      {
         wcolor_set(mSubWindow, mMessageColorPair, nullptr);
      }

      // Note: In order for this to display the lines properly,
      //  scrolling must be disabled in mSubWindow while the lines
      //  are written.
      scrollok(mSubWindow, false);
      // Create a format string that will tell ncurses to use
      //  up to the width of the subwindow.
      std::ostringstream format;
      format << "%-" << mSubWinWidth << "s";
      // maxLineNumber is one past the last line# we can
      //  display in the window.
      const int maxLineNumber = mLineNumber + mSubWinHeight;
      int currentSubWinRow = 0;
      // Write each line in mMessageLines to the subwindow.
      for (int i = mLineNumber; i < maxLineNumber; ++i)
      {
         mvwprintw(mSubWindow, currentSubWinRow, 0, (char*)format.str().c_str(),
                   getLine(i).c_str());
         ++currentSubWinRow;
      }
      // If mMessageLines doesn't have enough lines to fill mSubWindow, then
      //  fill in the rest of mSubWindow with spaces (to fill in the color).
      if (mMessageLines.size() < (unsigned)mSubWinHeight)
      {
         for (int i = (int)mMessageLines.size(); i < mSubWinHeight; ++i)
         {
            mvwprintw(mSubWindow, i, 0, (char*)format.str().c_str(), " ");
         }
      }
      // Re-enable scrolling for the subwindow
      scrollok(mSubWindow, true);

      // Disable the message attributes and message color.
      disableAttrs(mSubWindow, eMESSAGE);
      if (useColors)
      {
         wcolor_set(mSubWindow, 0, nullptr);
      }
   }
} // drawMessage

bool cxScrolledWindow::move(int pNewRow, int pNewCol, bool pRefresh)
{
   // When we move the window, we should be able to move the subwindow
   //  by calling mvwin, as in the code below, but this seems to cause
   //  the menu to have refresh issues on some platforms.
   /*
   bool moved = cxWindow::move(pNewRow, pNewCol, false);
   // If the window moved, then move the subwindow using mvwin.
   if (moved) {
      // Find the new location of the subwindow, and then move it.
      int subwinY = top();
      int subwinX = left();
      if (hasBorder()) {
         ++subwinY;
         ++subwinX;
      }
      mvwin(mSubWindow, subwinY, subwinX);

      // Refresh the window if pRefresh is true.
      if (pRefresh) {
         show(false, false);
      }
   }
   */
   // If we re-create the subwindow after the move, as follows, it seems to
   //  look okay.
   bool moved = cxWindow::move(pNewRow, pNewCol, false);
   if (moved)
   {
      reCreateSubWindow();
      // Refresh the window if pRefresh is true.
      if (pRefresh)
      {
         show(false, false);
      }
   }

   return(moved);
} // move

void cxScrolledWindow::resize(int pNewHeight, int pNewWidth, bool pRefresh)
{
   cxWindow::resize(pNewHeight, pNewWidth, pRefresh);

   init(top(), left(), pNewHeight, pNewWidth, getTitle(), getMessage(),
        getStatus(), nullptr, false);

   // Re-create mSubWindow
   reCreateSubWindow();

   if (pRefresh)
   {
      show(false, false);
   }
} // resize

cxScrolledWindow& cxScrolledWindow::operator =(const cxScrolledWindow& pThatWindow)
{
   // Only try to copy pThatWindow if it's a different instance.
   if (&pThatWindow != this)
   {
      copyCxScrolledWindowStuff(&pThatWindow);
   }

   return(*this);
} // operator =

void cxScrolledWindow::setSearchKey(int pKey)
{
   mSearchKey = pKey;
} // setSearchKey

void cxScrolledWindow::setGoToKey(int pKey)
{
   mGoToKey = pKey;
} // setGoToKey

void cxScrolledWindow::useLastKeyword(bool pUseLastKeyword)
{
   mUseLastKeyword = pUseLastKeyword;
} // useLastKeyword

void cxScrolledWindow::setAltPgUpKey(int pKey)
{
   mAltPgUpKey = pKey;
} // setAltPgUpKey

void cxScrolledWindow::setAltPgDownKey(int pKey)
{
   mAltPgDownKey = pKey;
} // setAltPgDownKey

void cxScrolledWindow::setLoopStartFunction(const shared_ptr<cxFunction>& pFuncPtr)
{
   mLoopStartFunction = pFuncPtr;
} // setLoopStartFunction

void cxScrolledWindow::setLoopEndFunction(const shared_ptr<cxFunction>& pFuncPtr)
{
   mLoopEndFunction = pFuncPtr;
} // setLoopEndFunction

string cxScrolledWindow::cxTypeStr() const
{
   return("cxScrolledWindow");
} // cxTypeStr

void cxScrolledWindow::drawBorder()
{
   cxWindow::drawBorder();

   // On the right border, display up & down arrows directly under the top
   //  border and directly over the bottom border to indicate that vertical
   //  scrolling is allowed.  Note: The horizontal arrows on the bottom border
   //  can't be added here because drawStatus() is called after this method,
   //  which puts border characters across the entire bottom border.
   // Make sure the window has a border and there is enough space to draw
   //  the arrows.
   if (hasBorder() && (height() > 4))
   {
      // Enable the border attributes
      enableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, mBorderColorPair, nullptr);
      }

      mvwaddch(mWindow, 1, width()-1, ACS_UARROW);          // Up arrow
      mvwaddch(mWindow, height()-2, width()-1, ACS_DARROW); // Down arrow

      // Disable the border attributes
      disableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, 0, nullptr);
      }
   }
} // drawBorder

//// Protected functions

void cxScrolledWindow::reCreateSubWindow()
{
   if (mSubWindow != nullptr)
   {
      //werase(mSubWindow);
      //wclear(mSubWindow);
      delwin(mSubWindow);
      mSubWindow = nullptr;
   }

   mSubWinHeight = height();
   mSubWinWidth = width();
   if (getBorderStyle() == eBS_NOBORDER)
   {
      mSubWindow = derwin(mWindow, mSubWinHeight, mSubWinWidth, 0, 0);
      // If mSubWindow is nullptr, that means derwin() had an error..
      if (mSubWindow == nullptr)
      {
         // Free up the other memory used
         cxWindow::freeWindow();
         throw(cxWidgetsException("Couldn't re-create the ncurses subwindow (cxMenu)."));
      }
   }
   else
   {
      mSubWinHeight -= 2;
      mSubWinWidth -= 2;
      mSubWindow = derwin(mWindow, mSubWinHeight, mSubWinWidth, 1, 1);
      if (mSubWindow == nullptr)
      {
         // Free up the other memory used
         cxWindow::freeWindow();
         throw(cxWidgetsException("Couldn't re-create the ncurses subwindow (cxMenu)."));
      }
   }
   scrollok(mSubWindow, true);
   idlok(mSubWindow, true);
} // reCreateSubWindow

void cxScrolledWindow::init(int pRow, int pCol, int pHeight, int pWidth,
                        const string& pTitle, const string& pMessage,
                        const string& pStatus, cxWindow *pParentWindow,
                        bool pResizeVertically)
                        {
   cxWindow::init(pRow, pCol, pHeight, pWidth, pTitle, pMessage,
                  pStatus, pParentWindow, pResizeVertically);
} // init

void cxScrolledWindow::copyCxScrolledWindowStuff(const cxScrolledWindow* pThatWindow)
{
   if ((pThatWindow != nullptr) && (pThatWindow != this))
   {
      if (mSubWindow != nullptr)
      {
         delwin(mSubWindow);
         mSubWindow = nullptr;
      }

      // Copy the parent cxWindow stuff, then the stuff for this class.
      try
      {
         copyCxWinStuff((const cxWindow*)pThatWindow);
      }
      catch (const cxWidgetsException& exc)
      {
         // Free up the other memory used
         cxWindow::freeWindow();
         throw(cxWidgetsException("Couldn't copy base cxWindow stuff (copying a cxScrolledWindow)."));
      }

      //init(pThatWindow->top(), pThatWindow->left(), pThatWindow->height(),
      //     pThatWindow->width(), pThatWindow->getTitle(), pThatWindow->getMessage(),
      //     pThatWindow->getStatus());

      mSubWinHeight = pThatWindow->mSubWinHeight;
      mSubWinWidth = pThatWindow->mSubWinWidth;
      if (getBorderStyle() == eBS_NOBORDER)
      {
         mSubWindow = derwin(mWindow, mSubWinHeight, mSubWinWidth, 0, 0);
         // If mSubWindow is nullptr, that means derwin() had an error..
         if (mSubWindow == nullptr)
         {
            // Free up the other memory used
            cxWindow::freeWindow();
            throw(cxWidgetsException("Couldn't re-create the ncurses subwindow (cxMenu)."));
         }
      }
      else
      {
         mSubWindow = derwin(mWindow, mSubWinHeight, mSubWinWidth, 1, 1);
         if (mSubWindow == nullptr)
         {
            // Free up the other memory used
            cxWindow::freeWindow();
            throw(cxWidgetsException("Couldn't re-create the ncurses subwindow (cxMenu)."));
         }
      }
      scrollok(mSubWindow, true);
      idlok(mSubWindow, true);

      mLineNumber = pThatWindow->mLineNumber;
      mHScrollOffset = pThatWindow->mHScrollOffset;
   }
} // copyCxScrolledWindowStuff

int cxScrolledWindow::getLineNumber() const
{
   return(mLineNumber);
} // getLineNumber

int cxScrolledWindow::getSubWinHeight() const
{
   return(mSubWinHeight);
} // getSubWinHeight

int cxScrolledWindow::getSubWinWidth() const
{
   return(mSubWinWidth);
} // getSubWinWidth()

//// Private functions

long cxScrolledWindow::doInputLoop(bool& pRunOnLeaveFunction)
{
   pRunOnLeaveFunction = true;
   long returnCode = cxID_EXIT;

   // For dragging the window around with the mouse
   int pressedX = 0;
   int pressedY = 0;
   bool movingWin = false;

   int lastKey = NOKEY;
   bool continueOn = true;
   while (continueOn)
   {
      // Run the loop start function, if it's set
      if (mLoopStartFunction != nullptr && mLoopStartFunction->functionIsSet())
      {
         mLoopStartFunction->runFunction();
         if (mLoopStartFunction->getExitAfterRun())
         {
            break;
         }
      }

      // Make sure the cursor is disabled (in case the loop start or loop end
      //  function enabled it)
      curs_set(0);

      lastKey = wgetch(mWindow);
      cxWindow::setLastKey(lastKey);
      // Handle mouse events first.
#ifdef NCURSES_MOUSE_VERSION
      // If there was a mouse event, then process it.  If there was
      //  a mouse button event, then:
      //   - If button 1 was clicked inside the window, then do item
      //     selection or scrolling.
      //   - If the mouse button event was outside the window, then:
      //     - If the parent window is a cxPanel, then exit.
      //     - If this menu is in at least 1 parent menu, then exit.
      if (lastKey == KEY_MOUSE)
      {
         if (getmouse(&mMouse) == OK)
         {
            // Run a function that may exist for the mouse state.  If
            //  no function exists for the mouse state, then process
            //  it here.
            bool mouseFuncExists = false;
            continueOn = handleFunctionForLastMouseState(&mouseFuncExists,
                                                        &pRunOnLeaveFunction);
            if (!mouseFuncExists)
            {
               // If the mouse event was inside the window, then handle it.
               if (mouseEvtWasInWindow())
               {
                  switch (mMouse.bstate)
                  {
                     case BUTTON1_CLICKED:
                        // If there are borders, handle scrolling.
                        if (hasBorder())
                        {
                           // Do up/down scrolling if the user clicked on the
                           //  right spot on the right border
                           if (mMouse.x == right())
                           {
                              if (mMouse.y == top()+1)
                              {
                                 // Scroll up by 1 line
                                 scrollWin(-1, 0, true);
                              }
                              else if (mMouse.y == bottom()-1)
                              {
                                 // Scroll down by 1 line
                                 scrollWin(1, 0, true);
                              }
                              else
                              {
                                 // Do pagewise scrolling (note: the window should
                                 //  be at least 6 characters high to have room
                                 //  for the areas to click in on the border).
                                 if (height() >= 6)
                                 {
                                    // Find the vertical boundaries of the click
                                    //  areas
                                    int upperHalfTop = top() + 2;
                                    int upperHalfBottom = top() + (height()/2);
                                    int bottomHalfTop = bottom() - (height()/2);
                                    int bottomHalfBottom = bottom() - 2;
                                    // If the mouse was clicked in the top half,
                                    //  do a page up.  If the mouse was clicked in
                                    //  the bottom half, do a page down.
                                    if ((mMouse.y >= upperHalfTop) &&
                                          (mMouse.y <= upperHalfBottom))
                                          {
                                       // Page up
                                       scrollWin(-mSubWinHeight, 0, true);
                                    }
                                    else if ((mMouse.y >= bottomHalfTop) &&
                                          (mMouse.y <= bottomHalfBottom))
                                          {
                                       // Page down
                                       scrollWin(mSubWinHeight, 0, true);
                                    }
                                 }
                              }
                           }
                           // Do left/right scrolling if the user clicked on the
                           //  right spot on the bottom border
                           else if (mMouse.y == bottom())
                           {
                              if (mMouse.x == left()+1)
                              {
                                 // Scroll left by 1 character
                                 scrollWin(0, -1, true);
                              }
                              else if (mMouse.x == right()-1)
                              {
                                 // Scroll right by 1 character
                                 scrollWin(0, 1, true);
                              }
                              else
                              {
                                 // Do pagewise left-right scrolling.  (Note: the
                                 //  window should be at least 6 characters wide
                                 //  to have room for the areas to click on the
                                 //  border.)
                                 if (width() >= 6)
                                 {
                                    // Find the horizontal boundaries of the click
                                    //  areas
                                    int leftHalfLeft = left() + 2;
                                    int leftHalfRight = left() + (width()/2);
                                    int rightHalfLeft = right() - (width()/2);
                                    int rightHalfRight = right() - 2;
                                    // If the mouse was clicked in the left half,
                                    //  do a page left.  If the mouse was clicked
                                    //  in the right half, do a page right.
                                    if ((mMouse.x >= leftHalfLeft) &&
                                          (mMouse.x <= leftHalfRight))
                                          {
                                       // Do a page left
                                       scrollWin(0, -mSubWinWidth, true);
                                    }
                                    else if ((mMouse.x >= rightHalfLeft) &&
                                          (mMouse.x <= rightHalfRight))
                                          {
                                       // Do a page right
                                       scrollWin(0, mSubWinWidth, true);
                                    }
                                 }
                              }
                           }
                        }
                        break;
                        // TODO: Clicking and dragging the window doesn't seem to work all the time.
                     case BUTTON1_PRESSED:
                        // Update pressedX and pressedY
                        pressedX = mMouse.x;
                        pressedY = mMouse.y;
                        // If they clicked in th title, then let them move the
                        //  window.
                        movingWin = mouseEvtWasInTitle();
                        break;
                     case BUTTON1_RELEASED:
                        // If the user is moving the window, then go ahead and
                        //  move it.
                        if (movingWin)
                        {
                           moveRelative(mMouse.y - pressedY, mMouse.x - pressedX, true);
                           movingWin = false; // reset the moving of the window
                        }
                        break;
                  }
               }
               else
               {
                  // The mouse button event was outside the window.  If the parent
                  //  window is a cxPanel, then quit the input loop.  This allows
                  //  the user to go to another window in the panel.
                  if (parentIsCxPanel())
                  {
                     returnCode = cxID_EXIT;
                     continueOn = false;
                  }
               }
            }
         }
      }
#else
      // This is defined for versions of ncurses without mouse support.
      // This is here because the next block starts with "else if".  The
      //  code will go onto the next block because of the false.
      if (false)
      {
      }
#endif
      // If the last key is a quit key, then quit and return
      //  cxID_QUIT.  If the key isn't there, see if it's an exit key
      //  (if it is, quit and return cxID_EXIT).  If not there either,
      //  handle the key normally.
      else if (hasQuitKey(lastKey))
      {
         returnCode = cxID_QUIT;
         continueOn = false;
      }
      else if (hasExitKey(lastKey))
      {
         returnCode = cxID_EXIT;
         continueOn = false;
      }
      else
      {
         // Run a function that may be associated with the last
         //  keypress.
         bool functionExists = false;
         continueOn = handleFunctionForLastKey(&functionExists,
                                               &pRunOnLeaveFunction);
         // Quit the input loop if continueOn was set false or if
         //  mLeaveNow was set true
         if (!continueOn || mLeaveNow)
         {
            break;
         }

         if (!functionExists)
         {
            switch (lastKey)
            {
               case KEY_UP: // Scroll up by 1 line
                  scrollWin(-1, 0, true);
                  break;
               case KEY_DOWN: // Scroll down by 1 line
                  scrollWin(1, 0, true);
                  break;
               case KEY_LEFT: // Scroll left by 1 character
                  scrollWin(0, -1, true);
                  break;
               case KEY_RIGHT: // Scroll right by 1 character
                  scrollWin(0, 1, true);
                  break;
               case KEY_PPAGE:  // Page up
                  scrollWin(-mSubWinHeight, 0, true);
                  break;
               case KEY_NPAGE:  // Page down
                  scrollWin(mSubWinHeight, 0, true);
                  break;
               case KEY_HOME:
               case KEY_SEND:  // sEND=HOME
                  scrollWin(-mLineNumber, 0, true);
                  break;
               case KEY_END:
               case KEY_END2:
               case KEY_END3:
               case KEY_SHOME: // sHOME=END
                  // Go to the last page
                  scrollWin(((int)mMessageLines.size() - mSubWinHeight) - mLineNumber,
                            0, true);
                  break;
               case ESC:
                  returnCode = cxID_QUIT;
                  continueOn = false;
                  break;
               case ERR:  // Error getting a key
                  // What can we do in this case?
                  break;
               default:
                  if (lastKey == mGoToKey)
                  {
                     doGoToLine();
                  }
                  else if (lastKey == mSearchKey)
                  {
                     doSearch();
                  }
                  else if (lastKey == mAltPgUpKey)
                  {
                     scrollWin(-mSubWinHeight, 0, true);
                  }
                  else if (lastKey == mAltPgDownKey)
                  {
                     scrollWin(mSubWinHeight, 0, true);
                  }
                  else
                  {
                     continueOn = false;
                  }
                  break;
            } // switch
         }
      }

      // Run the end loop function, if it's set
      if (mLoopEndFunction != nullptr && mLoopEndFunction->functionIsSet())
      {
         mLoopEndFunction->runFunction();
         continueOn = (continueOn && !(mLoopEndFunction->getExitAfterRun()));
      }
   } // while

   // mLeaveNow should be false
   mLeaveNow = false;

   return(returnCode);
} // doInputLoop

string cxScrolledWindow::getLine(int pLineNumber)
{
   if (pLineNumber >= 0 && (unsigned)pLineNumber < mMessageLines.size())
   {
      if (mHScrollOffset < (int)mMessageLines[pLineNumber].length())
      {
         return(mMessageLines[pLineNumber].substr(mHScrollOffset, mSubWinWidth));
      }
      else
      {
         return("");
      }
   }
   else
   {
      return("");
   }
} // getLine

inline void cxScrolledWindow::goToLine(int pLineNum)
{
   if (pLineNum != mLineNumber)
   {
      scrollWin(pLineNum - mLineNumber, 0, true);
   }
} // goToLine

void cxScrolledWindow::doGoToLine()
{
   // Prompt the user to go to a specific line
   cxMultiLineInput goToInput(this, 1, 1, 1, 15, "Line #:", eBS_SINGLE_LINE);
   goToInput.setTitle("Go To Line", false);
   goToInput.center(false);
   goToInput.setBorderColor(eYELLOW_BLUE);
   goToInput.setLabelColor(eBRTCYAN_BLUE);
   // Ensure that the user can only type integers.
   goToInput.setOnKeyFunction(cxValidators::intOnKeyValidator, &goToInput, nullptr);
   if (goToInput.showModal() != ESC)
   {
      // Subtract 1 from the user's line# (because the index to
      //  mMessageLines is 0-based).
      int lineNo = cxStringUtils::stringTo(goToInput.getValue()) - 1;
      goToLine(lineNo);
   }
   goToInput.hide();
} // doGoToLine

void cxScrolledWindow::doSearch()
{
   // Prompt the user for text
   int winWidth = (cxBase::width() > 45 ? 45 : cxBase::width());
   cxMultiLineInput searchInput(this, 1, 1, 1, winWidth, "Keyword:", eBS_SINGLE_LINE);
   searchInput.setTitle("Search", false);
   if (mUseLastKeyword)
   {
      // Set the current keyword in the search input
      searchInput.setValue(mSearchKeyword);
   }
   searchInput.center(false);
   searchInput.setBorderColor(eYELLOW_BLUE);
   searchInput.setLabelColor(eBRTCYAN_BLUE);
   if (searchInput.showModal() != ESC)
   {
      // Get the keyword entered into the input
      string keyword = searchInput.getValue();
      // The search should start from line 0; however, if
      //  mUseLastKeyword is true, then check to see if
      //  we should do a new search or repeat the last
      //  search.
      int startLine = 0;
      if (mUseLastKeyword)
      {
         // Start searching from one past the current line number;
         //  however, if the user enters a new keyword, start from
         //  the first line.
         if (keyword == mSearchKeyword)
         {
            startLine = mLineNumber + 1;
         }
         mSearchKeyword = keyword;
      }

      // Go through mMessageLines looking for the user's keyword
      int numLines = (int)mMessageLines.size();
      bool found = false;
      // make sure we are case-insensitive
      keyword=strToLower(keyword);
      string tmp;
      for (int i = startLine; i < numLines; ++i)
      {
         // make sure we are case-insensitive
         tmp = strToLower(mMessageLines[i]);
         if (tmp.find(keyword, 0) != string::npos)
         {
            // If found, scroll to this line
            goToLine(i);
            found = true;
            break;
         }
      }
      if (!found)
      {
         cxBase::messageBox("Not found");
      }
   }
   searchInput.hide();
} // doSearch

void cxScrolledWindow::drawHorizontalScrollArrows()
{
   // Make sure the window has a border and there is enough space
   //  to draw the arrows.
   if (hasBorder() && (width() > 4))
   {
      // Enable the border attributes
      enableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, mBorderColorPair, nullptr);
      }

      mvwaddch(mWindow, height()-1, 1, ACS_LARROW);         // Left arrow
      mvwaddch(mWindow, height()-1, width()-2, ACS_RARROW); // Right arrow

      // Disable the border attributes
      disableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, 0, nullptr);
      }
   }
} // drawHorizontalScrollArrows
