// Copyright (c) 2005-2007 Michael H. Kinney
//
// Note: When writing text to the subwindow, make sure
//  you turn scrollok off before writing, and then turn
//  scrollok back on after you're done writing.
//  (scrollok(mSubWindow, false), scrollok(mSubWindow, true))

// TODO: There seems to be a bug where if you disable hotkey highlighting
//  (with setHotkeyHighlighting(false);), the last character on the menu items
//  will be missing when the menu is drawn (but when the user selects one,
//  the whole thing will be available).  The issue may be in
//  cxWindow::writeWithHighlighting().

#include "cxMenu.h"
#include "cxBase.h"
#include "cxMultiLineInput.h"
#include "cxStringUtils.h"
#include <set>
#include <stdexcept> // For std::out_of_range exception
#include <cstdlib>   // For tolower()
using std::string;
using std::map;
using std::multimap;
using std::make_pair;
using std::set;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using cxBase::stringWithoutHotkeyChars;
using cxBase::messageBox;
using cxStringUtils::Find;
using cxStringUtils::toUpper;
using cxStringUtils::toString;
using cxStringUtils::stringTo;
using std::set;

cxMenu::cxMenu(cxWindow *pParentWindow, int pRow, int pCol, int pHeight,
               int pWidth, const string& pTitle, cxWindow *pExtTitleWindow,
               cxWindow *pExtStatusWindow, eBorderStyle pBorderStyle)
   : cxWindow(pParentWindow, pRow, pCol, pHeight, pWidth, pTitle, "", "",
              pBorderStyle, pExtTitleWindow, pExtStatusWindow),
     mSubWinHeight((getBorderStyle() == eBS_NOBORDER) ? height() : height()-2),
     mSubWinWidth((getBorderStyle() == eBS_NOBORDER) ? width() : width()-2)
{
   // Create the subwindow (for the list of items)
   if (getBorderStyle() == eBS_NOBORDER)
   {
      mSubWindow = derwin(mWindow, mSubWinHeight, mSubWinWidth, 0, 0);
      // If mSubWindow is nullptr, that means derwin() had an error..
      if (mSubWindow == nullptr)
      {
         // Free up the other memory used
         cxWindow::freeWindow();
         throw(cxWidgetsException("Couldn't create a new ncurses subwindow (constructing a new cxMenu)."));
      }
   }
   else
   {
      mSubWindow = derwin(mWindow, mSubWinHeight, mSubWinWidth, 1, 1);
      // If mSubWindow is nullptr, that means derwin() had an error..
      if (mSubWindow == nullptr)
      {
         // Free up the other memory used
         cxWindow::freeWindow();
         throw(cxWidgetsException("Couldn't create a new ncurses subwindow (constructing a new cxMenu)."));
      }
   }
   scrollok(mSubWindow, true);
   idlok(mSubWindow, true);

   if (pHeight > 2)
   {
      //resize(pHeight, pWidth, false);
   }

   // Use hotkey attributes with characters preceded by the ampersand (&)
   //  character.
   setHotkeyHighlighting(true);
} // Constructor

// Copy constructor
cxMenu::cxMenu(const cxMenu& pThatMenu)
   : cxWindow(nullptr, pThatMenu.top(), pThatMenu.left(),
              pThatMenu.height(), pThatMenu.width(), pThatMenu.getTitle(),
              pThatMenu.getMessage(), pThatMenu.getStatus(),
              pThatMenu.getBorderStyle(), pThatMenu.getExtTitleWindow(),
              pThatMenu.getExtStatusWindow()),
     mMenuSelectionAttrs(pThatMenu.mMenuSelectionAttrs),
     mSubWinHeight(pThatMenu.mSubWinHeight),
     mSubWinWidth(pThatMenu.mSubWinWidth),
     mCurrentMenuItem(pThatMenu.mCurrentMenuItem),
     mTopMenuItem(pThatMenu.mTopMenuItem),
     mAltPgUpKey(pThatMenu.mAltPgUpKey),
     mAltPgDownKey(pThatMenu.mAltPgDownKey),
     mSearchKey(pThatMenu.mSearchKey),
     mSearchText(pThatMenu.mSearchText),
     mLastSearchText(pThatMenu.mLastSearchText),
     mClearOnSearch(pThatMenu.mClearOnSearch),
     mCaseSensitiveSearch(pThatMenu.mCaseSensitiveSearch),
     mCustomStatus(pThatMenu.mCustomStatus),
     mAllowQuit(pThatMenu.mAllowQuit),
     mAllowExit(pThatMenu.mAllowExit),
     mSelectableItemExists(pThatMenu.mSelectableItemExists),
     mWrap(pThatMenu.mWrap),
     mWaitForInputIfEmpty(pThatMenu.mWaitForInputIfEmpty),
     mReturnCodes(pThatMenu.mReturnCodes),
     mHelpStrings(pThatMenu.mHelpStrings),
     mItemTypes(pThatMenu.mItemTypes),
     mAltItemText(pThatMenu.mAltItemText),
     mUnselectableItems(pThatMenu.mUnselectableItems),
     mItemHotkeyIndex(pThatMenu.mItemHotkeyIndex),
     mLastItemHotkey(pThatMenu.mLastItemHotkey),
     mExitWhenLeaveFirst(pThatMenu.mExitWhenLeaveFirst),
     mExitWhenLeaveLast(pThatMenu.mExitWhenLeaveLast),
     mRefreshItemsWhenModal(pThatMenu.mRefreshItemsWhenModal),
     // mNumParentMenus is set to 0 because this menu will not be added to
     //  any of the parent menus of the other cxMenu.
     mNumParentMenus(0),
     mLastInputWasMouseEvent(pThatMenu.mLastInputWasMouseEvent)
{
   copyCxMenuStuff(&pThatMenu);
} // Copy constructor

cxMenu::~cxMenu()
{
   // Free the memory used by the subwindow. (note: cxWindow frees the memory
   //  used by the main window.)
   freeSubWindow();
   // Free the memory used by mOnSelectItemFunction
   mOnSelectItemFunction.reset();
}

void cxMenu::append(const string& pDisplayText, long pReturnCode,
                    const string& pHelpString, cxMenuItemType pType,
                    bool pResize, const string& pItemText)
                    {
   // Add the display text to mMessageLines, and add the alternate item text to
   //  mAltItemText
   mMessageLines.push_back(pDisplayText);
   mAltItemText.push_back(pItemText);

   // Add the item's return code, help string, and type.
   mReturnCodes.push_back(pReturnCode);
   mHelpStrings.push_back(pHelpString);
   mItemTypes.push_back(pType);

   // Resize the window, if pResize is true
   if (pResize)
   {
      if (getBorderStyle() == eBS_NOBORDER)
      {
         resize((int)(mMessageLines.size()), width());
      }
      else
      {
         resize((int)(mMessageLines.size()+2), width());
      }
   }
   else
   {
      if (getBorderStyle() == eBS_NOBORDER)
      {
         if (height() < 1)
         {
            resize(1, width());
         }
      }
      else
      {
         if (height() < 3)
         {
            resize(3, width());
         }
      }
   }

   // Add a hotkey for this item
   addHotKey(pDisplayText);

   // If this is a normal item, then mSelectableItemExists
   //  should be true.
   if (pType == cxITEM_NORMAL)
   {
      mSelectableItemExists = true;
   }
   // If the menu item has a pull-right submenu, then
   //  insert a '>' in the text to appear next to the rightmost
   //  edge of the menu.
   else if (pType == cxITEM_SUBMENU)
   {
      string itemText = mMessageLines[mMessageLines.size()-1];
      itemText.append(mSubWinWidth - (int)itemText.length() - 1, ' ');
      itemText += ">";

      mMessageLines[mMessageLines.size()-1] = itemText;
   }
   else if (pType == cxITEM_UNSELECTABLE)
   {
      mUnselectableItems.insert(mMessageLines.size()-1);
   }

   // If the item type is not cxITEM_NORMAL, call lookForSelectableItem()
   //  to look for a selectable item on the menu and all of the submenus
   //  if there are any.
   if (pType != cxITEM_NORMAL)
   {
      lookForSelectableItem();
   }
} // append

void cxMenu::appendWithPullRight(const string& pDisplayText, cxMenu *pSubMenu,
                                 const string& pHelpString, bool pResize)
                                 {
   if (pSubMenu != nullptr)
   {
      // Add the item text to the menu
      append(pDisplayText, -1, pHelpString, cxITEM_SUBMENU, pResize, "");
      // Add the submenu pointer to mSubMenus.
      mSubMenus[mMessageLines.size()-1] = pSubMenu;
      // Increment the menu's mNumParentMenus member
      ++(pSubMenu->mNumParentMenus);
   }
} // appendWithPullRight

void cxMenu::appendWithPopUp(const string& pDisplayText, cxMenu *pSubMenu,
                             const string& pHelpString, bool pResize)
                             {
   if (pSubMenu != nullptr)
   {
      // Add the item text to the menu
      append(pDisplayText, -1, pHelpString, cxITEM_POPUPMENU, pResize, "");
      // Add the submenu pointer to mSubMenus.
      mSubMenus[mMessageLines.size()-1] = pSubMenu;
      // Increment the menu's mNumParentMenus member
      ++(pSubMenu->mNumParentMenus);
   }
} // appendWithPopUp

bool cxMenu::remove(unsigned pItemIndex, bool pResize, bool pRefresh)
{
   bool returnVal = false;

   if ((pItemIndex >= 0) && (pItemIndex < mMessageLines.size()))
   {
      // Remove all info about the item from the collections
      mMessageLines.erase(mMessageLines.begin() + pItemIndex);
      mReturnCodes.erase(mReturnCodes.begin() + pItemIndex);
      mHelpStrings.erase(mHelpStrings.begin() + pItemIndex);
      mItemTypes.erase(mItemTypes.begin() + pItemIndex);
      mAltItemText.erase(mAltItemText.begin() + pItemIndex);
      // If there is a submenu for this item index, decrement its
      //  mNumParentMenus member and remove it from mSubMenus.
      if (mSubMenus.find(pItemIndex) != mSubMenus.end())
      {
         --(mSubMenus[pItemIndex]->mNumParentMenus);
         mSubMenus.erase(pItemIndex);
      }
      mUnselectableItems.erase(pItemIndex);
      // If the item that has been removed happens to be
      //  mCurrentItem or mTopMenuItem, then decrement them.
      if (pItemIndex == (unsigned)mCurrentMenuItem)
      {
         --mCurrentMenuItem;
         if (mCurrentMenuItem < 0)
         {
            mCurrentMenuItem = 0;
         }
      }
      if (pItemIndex == (unsigned)mTopMenuItem)
      {
         --mTopMenuItem;
         if (mTopMenuItem < 0)
         {
            mTopMenuItem = 0;
         }
      }

      if (pResize)
      {
         if (getBorderStyle() == eBS_NOBORDER)
         {
            resize((int)(mMessageLines.size()), width(), false);
         }
         else
         {
            resize((int)(mMessageLines.size()+2), width(), false);
         }
      }

      // If mCurrentMenuItem is beyond the last menu item, then set it to the
      //  last menu item.
      int menuItemCount = (int)(numMenuItems());
      if (menuItemCount > 0)
      {
         int lastItemIndex = menuItemCount - 1;
         if (mCurrentMenuItem > lastItemIndex)
         {
            mCurrentMenuItem = lastItemIndex;
         }
      }
      else
      {
         mCurrentMenuItem = 0;
      }

      // Make sure mTopMenuItem is still the top item on the menu
      //  (since it was decremented).
      setTopItem(mTopMenuItem, pRefresh);

      returnVal = true;
   }

   return(returnVal);
} // remove

bool cxMenu::remove(const string& pItemText, bool pUseDisplayText, bool pResize,
                    bool pRefresh)
                    {
   bool returnVal = false;

   // If pUseDisplayText is true, then look at the display text for each item.
   //  Otherwise, look at the alternate item text for each item.
   if (pUseDisplayText)
   {
      unsigned numItems = mMessageLines.size();
      for (unsigned i = 0; i < numItems; ++i)
      {
         // Check the item text verbatim and the item text without the hotkey
         //  characters
         if ((mMessageLines[i] == pItemText) ||
             (stringWithoutHotkeyChars(mMessageLines[i]) == pItemText))
             {
            returnVal = remove(i, pResize, pRefresh);
            break;
         }
      }
   }
   else
   {
      unsigned numItems = mAltItemText.size();
      // pUseDisplayText is false - Look at the alternate item text for each item
      for (unsigned i = 0; i < numItems; ++i)
      {
         if (mAltItemText[i] == pItemText)
         {
            returnVal = remove(i, pResize, pRefresh);
            break;
         }
      }
   }

   return(returnVal);
} // remove

bool cxMenu::removeByReturnCode(long pReturnCode, bool pResize, bool pRefresh)
{
   bool removed = false;

   // Go through the menu items and find the index
   //  of the item to remove
   unsigned numItems = mReturnCodes.size();
   for (unsigned index = 0; index < numItems; ++index)
   {
      if (mReturnCodes[index] == pReturnCode)
      {
         removed = true;
         remove(index, pResize, pRefresh);
         break;
      }
   }

   return(removed);
} // removeByReturnCode

void cxMenu::removeAllItems(bool pResize, bool pRefresh)
{
   mMessageLines.clear();
   mReturnCodes.clear();
   mHelpStrings.clear();
   mItemTypes.clear();
   mUnselectableItems.clear();
   mAltItemText.clear();
   // Before clearing mSubMenus, go through it and decrement the submenus'
   //  mNumParentMenus members.
   map<int, cxMenu*>::iterator subMenuIter = mSubMenus.begin();
   for (; subMenuIter != mSubMenus.end(); ++ subMenuIter)
   {
      --(subMenuIter->second->mNumParentMenus);
   }
   mSubMenus.clear();

   // Reset the member variables relating to the current/top menu item, etc.
   mCurrentMenuItem = 0;
   mTopMenuItem = 0;

   if (pResize)
   {
      // No more items
      if (getBorderStyle() == eBS_NOBORDER)
      {
         resize(1, width(), false);
      }
      else
      {
         resize(2, width(), false);
      }
   }

   if (pRefresh)
   {
      show(false, false);
   }
} // removeAllItems

void cxMenu::clear(bool pRefresh)
{
   removeAllItems(false, pRefresh);
} // clear

long cxMenu::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   setReturnCode(cxID_EXIT);

   // Only do the input loop if the menu window is enabled.
   if (isEnabled())
   {
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
         if (mRefreshItemsWhenModal)
         {
            refreshMenuItems();
         }
         // Disable the cursor (saving the current cursor state)
         int prevCursorState = curs_set(0);
         mIsModal = true;
         bool runOnLeaveFunc = true;
         setReturnCode(doInputLoop(runOnLeaveFunc));
         mIsModal = false;
         // Set the cursor state back to what it was
         curs_set(prevCursorState);
         // Run the onLeave function
         if (runOnLeaveFunc)
         {
            runOnLeaveFunction();
         }
      }
   }

   return(getReturnCode());
} // showModal

bool cxMenu::modalGetsKeypress() const
{
   bool willGetKeypress = false;

   if (isEnabled())
   {
      if (mMessageLines.size() > 0)
      {
         if (mSelectableItemExists)
         {
            willGetKeypress = true;
         }
         else
         {
            willGetKeypress = mWaitForInputIfEmpty;
         }
      }
      else
      {
         willGetKeypress = mWaitForInputIfEmpty;
      }
   }

   return(willGetKeypress);
} // modalGetsKeypress

void cxMenu::scrollItems(int pScrollAmt, bool pRefresh)
{
   // Don't do anything if pScrollAmt is 0.
   if (pScrollAmt != 0)
   {
      // If scrolling by pScrollAmt would result in the last item being above
      //  the bottom row inside the window border, then adjust pScrollAmt so
      //  that the last item will appear on the bottom row inside the window
      //  border.
      int bottom = mTopMenuItem + mSubWinHeight + pScrollAmt;
      if (bottom > (int)mMessageLines.size() - 1)
      {
         pScrollAmt -= (bottom - (int)mMessageLines.size());
      }

      // Scroll the subwindow with wscrl, and adjust things if
      //  it succeeds.
      if (wscrl(mSubWindow, pScrollAmt) != ERR)
      {
         // Update mTopMenuItem by pScrollAmt
         mTopMenuItem += pScrollAmt;
         // If mTopMenuItem went out of bounds, adjust it.
         if (mTopMenuItem < 0)
         {
            mTopMenuItem = 0;
         }
         else if (mTopMenuItem >= (int)mMessageLines.size())
         {
            mTopMenuItem = (int)mMessageLines.size() - 1;
         }

         // If the current menu item is above the visible
         //  portion of the menu, then make the current
         //  item the topmost visible item.  Or, if the
         //  current menu item is below the visible
         //  portion of the menu, make the current item
         //  the bottommost visible item.
         if (mCurrentMenuItem < mTopMenuItem)
         {
            mCurrentMenuItem = mTopMenuItem;
         }
         else if (mCurrentMenuItem > (mTopMenuItem + (height()-3)))
         {
            mCurrentMenuItem = mTopMenuItem + (height()-3);
         }
      }
   }

   // Refresh the window if the user wanted to.
   if (pRefresh)
   {
      show(false, false);
   }
} // scrollItems

void cxMenu::scrollToTop(bool pRefresh)
{
   setTopItem(0, pRefresh);
} // scrollToTop

void cxMenu::scrollToBottom(bool pRefresh)
{
   if (numMenuItems() > 0)
   {
      setBottomItem(numMenuItems()-1, pRefresh);
   }
} // scrollToBottom

void cxMenu::setTopItem(unsigned pTopItem, bool pRefresh)
{
   // Find the greatest possible top item (the greatest-numbered one that
   //  can be a top item with the menu still full as possible).  For
   //  example, if there are 10 menu items and the subwindow is 8 lines
   //  high, the greatest possible top item number is item number 2
   //  (0-based).
   unsigned greatestPossibleTopItem = 0;
   if (mMessageLines.size() > (unsigned)mSubWinHeight)
   {
      greatestPossibleTopItem = mMessageLines.size() - (unsigned)mSubWinHeight;
      if (pTopItem > greatestPossibleTopItem)
      {
         pTopItem = greatestPossibleTopItem;
      }
   }

   if ((pTopItem >= 0) && (pTopItem < mMessageLines.size()))
   {
      if (mTopMenuItem != (int)pTopItem)
      {
         mTopMenuItem = (int)pTopItem;

         // If mCurrentMenuItem went out of bounds, adjust it.
         if (mCurrentMenuItem < mTopMenuItem)
         {
            mCurrentMenuItem = mTopMenuItem;
         }
         else if (mCurrentMenuItem >= mTopMenuItem + mSubWinHeight)
         {
            mCurrentMenuItem = mTopMenuItem + mSubWinHeight - 1;
         }
      }

      if (pRefresh)
      {
         // Update the menu item text in the subwindow and
         //  refresh it.
         drawMessage();
         wrefresh(mSubWindow);
      }
   }
} // setTopItem

void cxMenu::setTopItem(const string& pItemText, bool pRefresh)
{
   messageLineContainer::iterator iter = mMessageLines.begin();
   unsigned index = 0;
   for (; iter != mMessageLines.end(); ++iter)
   {
      if (stringWithoutHotkeyChars(*iter) == pItemText)
      {
         setTopItem(index, pRefresh);
         break;
      }
      else
      {
         ++index;
      }
   }
} // setTopItem

void cxMenu::setBottomItem(unsigned pItemIndex, bool pRefresh)
{
   if ((pItemIndex >= 0) && (pItemIndex < mMessageLines.size()))
   {
      // Calculate what the top item would be and set it, if
      //  pItemIndex is below the bottommost item in the menu.
      //  Otherwise, calculate the distance from pItemIndex to the item
      //  currently at the bottom of the menu and scroll by that amount.
      if ((int)pItemIndex > (mTopMenuItem + mSubWinHeight - 1))
      {
         setTopItem(pItemIndex - (unsigned)mSubWinHeight + 1, pRefresh);
      }
      else
      {
         int dist = (int)pItemIndex - (mTopMenuItem + mSubWinHeight - 1);
         scrollItems(dist, pRefresh);
      }
   }
} // setBottomItem

bool cxMenu::move(int pNewRow, int pNewCol, bool pRefresh)
{
   // When we move the menu window, we should be able to move the subwindow
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
   // If we destroy and re-create the subwindow as follows, it seems to look
   //  okay.
   freeSubWindow();
   bool moved = cxWindow::move(pNewRow, pNewCol, pRefresh);
   reCreateSubWindow();

   if (moved)
   {
      // Refresh the window if pRefresh is true.
      if (pRefresh)
      {
         show(false, false);
      }
   }

   return(moved);
} // move

unsigned cxMenu::numMenuItems() const
{
   return(mMessageLines.size());
}

// Resizes the menu.
void cxMenu::resize(int pNewHeight, int pNewWidth, bool pRefresh)
{
   if (((pNewHeight != height()) || (pNewWidth != width())) &&
       (pNewHeight > 0) && (pNewWidth > 0))
       {
      const int maxHeight = cxBase::height() - top();
      const int maxWidth = cxBase::width() - left();
      if ((pNewHeight <= maxHeight) && (pNewWidth <= maxWidth))
      {
         int leftSide = left();
         int topSide = top();

         // No borders: If the new height and the new width are < 1, this is
         //  probably unacceptable..  With borders, the height & width should
         //  be at least 3.
         if (getBorderStyle() == eBS_NOBORDER)
         {
            if (pNewHeight < 1)
            {
               pNewHeight = 1;
            }
            if (pNewWidth < 1)
            {
               pNewWidth = 1;
            }
         }
         else
         {
            if (pNewHeight < 3)
            {
               pNewHeight = 3;
            }
            if (pNewWidth < 3)
            {
               pNewWidth = 3;
            }
         }

         // Find the longest menu item in mMessageLines, and make sure
         //  pNewWidth is set to the greater of the longest menu item
         //  or pNewWidth.
         int longestMenuItem = 0;
         messageLineContainer::iterator iter = mMessageLines.begin();
         for (; iter != mMessageLines.end(); ++iter)
         {
            if ((int)(iter->length()) > longestMenuItem)
            {
               longestMenuItem = (int)(iter->length());
            }
         }
         if (longestMenuItem > pNewWidth)
         {
            pNewWidth = longestMenuItem;
         }
         // Change the width if it's smaller than the main title or status
         //  text.
         int titleLen = (int)(getTitle().length());
         int statusLen = (int)(getStatus().length());
         if ((pNewWidth < titleLen) || (pNewWidth < statusLen))
         {
            if (titleLen > statusLen)
            {
               pNewWidth = titleLen;
            }
            else
            {
               pNewWidth = statusLen;
            }
         }
         // Make sure it's not too wide.
         if (pNewWidth > maxWidth)
         {
            pNewWidth = maxWidth;
         }

         // Make sure it's not too tall.
         if (pNewHeight > maxHeight)
         {
            pNewHeight = maxHeight;
         }

         // If we have something realistic to display..
         if ((pNewWidth >= 1) && (pNewHeight >= 1))
         {
            // Free the subwindow and main window.  Note: An ncurses subwindow
            //  must be deleted before the main window is deleted.
            freeSubWindow();
            freeWindow();
            // Re-create mWindow
            mWindow = newwin(pNewHeight, pNewWidth, topSide, leftSide);
            if (mWindow != nullptr)
            {
               keypad(mWindow, TRUE);
               // Re-create the subwindow
               reCreateSubWindow();
               // Re-create the panel
               reCreatePanel();
               // Make sure the menu items fit inside the window.
               fitItemsToWidth();
            }
            else
            {
               // Uh oh, mWindow is nullptr..
               throw(cxWidgetsException("Couldn't re-create the curses window (cxMenu::resize()).  Height: " + toString(pNewHeight) + ", width: " + toString(pNewWidth) + ", row: " + toString(topSide) + ", col: " + toString(leftSide)));
            }
         }

         if (pRefresh)
         {
            show(false, false);
         }
      }
   }
} // resize

void cxMenu::drawMessage()
{
   if (mSubWindow != nullptr)
   {
      // Enable the message attributes
      enableAttrs(mSubWindow, eMESSAGE);
      wcolor_set(mSubWindow, mMessageColorPair, nullptr);

      int currentSubWinRow = 0;
      int minHeight = 0;
      // If there is a border, the minimum height is 2.
      if (getBorderStyle() != eBS_NOBORDER)
      {
         minHeight = 2;
      }
      if ((mMessageLines.size() > 0) && (height() > minHeight))
      {
         // bottomMenuItem is the index of the bottommost menu
         //  item to be displayed.
         int bottomMenuItem = getBottomItemIndex();
         for (int i = mTopMenuItem; i <= bottomMenuItem; ++i)
         {
            drawMenuItem(i, currentSubWinRow, false);

            ++currentSubWinRow;
         }
      }
      // Fill in the remainder of the text area with spaces
      //  (to fill in the background color and overwrite stuff
      //  that may have been output previously).
      int rowLimit = mSubWinHeight;
      scrollok(mSubWindow, false);
      for ( ; currentSubWinRow < rowLimit; ++currentSubWinRow)
      {
         // Output spaces to fill the current row in the subwindow
         std::ostringstream os;
         os << "%-" << mSubWinWidth << "s";
         mvwprintw(mSubWindow, currentSubWinRow, 0, (char*)os.str().c_str(), " ");
      }
      scrollok(mSubWindow, true);

      // Disable the message attributes
      disableAttrs(mSubWindow, eMESSAGE);
      wcolor_set(mSubWindow, 0, nullptr);
   }
} // drawMessage

long cxMenu::show(bool pBringToTop, bool pShowSubwindows)
{
   long returnCode = cxFIRST_AVAIL_RETURN_CODE;

   // Only do this if the menu window is enabled.
   if (isEnabled())
   {
      returnCode = cxWindow::show(pBringToTop, pShowSubwindows);
      // Refresh the subwindow
      wrefresh(mSubWindow);
      update_panels();
   }
   else
   {
      hide(false);
   }

   return(returnCode);
} // show

void cxMenu::erase(bool pEraseSubwindows)
{
   cxWindow::erase(pEraseSubwindows);
   if (pEraseSubwindows && mSubWindow != nullptr)
   {
      werase(mSubWindow);
      wrefresh(mSubWindow);
   }
} // erase

int cxMenu::getSubWinWidth() const
{
   return (mSubWinWidth);
} // getSubwinWidth

int cxMenu::getSubWinHeight() const
{
   return (mSubWinHeight);
} // getSubwinHeight

long cxMenu::getReturnCode() const
{
   return(cxWindow::getReturnCode());
} // getReturnCode

long cxMenu::getReturnCode(unsigned pIndex) const
{
   if ((pIndex >= 0) && (pIndex < mReturnCodes.size()))
   {
      return(mReturnCodes[pIndex]);
   }
   else
   {
      return(-1);
   }
} // getReturnCode

long cxMenu::getReturnCode(const string& pItemText, bool pUseDisplayText) const
{
   int returnCode = -1;

   // If pUseDisplayText is true, then look at the display text for each item.
   //  Otherwise, look at the alternate item text for each item.
   if (pUseDisplayText)
   {
      unsigned numItems = mMessageLines.size();
      for (unsigned i = 0; i < numItems; ++i)
      {
         // Check the item text verbatim and the item text without the hotkey
         //  characters
         if ((mMessageLines[i] == pItemText) ||
             (stringWithoutHotkeyChars(mMessageLines[i]) == pItemText))
             {
            returnCode = mReturnCodes[i];
            break;
         }
      }
   }
   else
   {
      unsigned numItems = mAltItemText.size();
      for (unsigned i = 0; i < numItems; ++i)
      {
         if (mAltItemText[i] == pItemText)
         {
            returnCode = mReturnCodes[i];
            break;
         }
      }
   }

   return(returnCode);
} // getReturnCode

long cxMenu::getCurrentItemReturnCode() const
{
   return(getReturnCode(getCurrentMenuItem()));
} // getCurrentItemReturnCode

string cxMenu::getItemText(int pIndex, bool pGetDisplayText) const
{
   string itemText;

   if (pGetDisplayText)
   {
      if ((pIndex >= 0) && (pIndex < (int)(mMessageLines.size())))
      {
         itemText = mMessageLines[pIndex];
      }
   }
   else
   {
      if ((pIndex >= 0) && (pIndex < (int)(mAltItemText.size())))
      {
         itemText = mAltItemText[pIndex];
      }
   }

   return(itemText);
} // getItemText()

string cxMenu::getCurrentItemText(bool pGetDisplayText) const
{
   string itemText;

   // 1) Get the current menu item index
   // 2) Get the text associated with that index
   int itemIndex = getCurrentMenuItem();
   if ((itemIndex >= 0) && (itemIndex < (int)numMenuItems()))
   {
      itemText = getItemText(itemIndex, pGetDisplayText);
   }

   return(itemText);
} // getCurrentItemText

void cxMenu::drawBorder()
{
   cxWindow::drawBorder();

   // On the right border, display up & down arrows directly under the top
   //  border and directly over the bottom border to indicate that scrolling
   //  is allowed.
   // Make sure the window has a border and there is enough space to draw the
   //  arrows.
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

void cxMenu::toggleSelectability(unsigned pIndex, bool pSelectable)
{
   if ((pIndex >= 0) && (pIndex < mMessageLines.size()))
   {
      if (pSelectable)
      {
         mUnselectableItems.erase(pIndex);
      }
      else
      {
         mUnselectableItems.insert(pIndex);
      }

      // Update mSelectableItemExists
      lookForSelectableItem();
   }
} // toggleSelectability

void cxMenu::toggleSelectability(const string& pItemText, bool pSelectable)
{
   unsigned numItems = mMessageLines.size();
   for (unsigned index = 0; index < numItems; ++index)
   {
      if (mMessageLines[index] == pItemText)
      {
         toggleSelectability(index, pSelectable);
         break;
      }
   }
} // toggleSelectability

void cxMenu::setClearOnSearch(bool pClearOnSearch)
{
   mClearOnSearch = pClearOnSearch;
} // setClearOnSearch

void cxMenu::setCaseSensitiveSearch(bool pCaseSensitiveSearch)
{
   mCaseSensitiveSearch = pCaseSensitiveSearch;
} // setCaseSensitiveSearch

void cxMenu::setSearchKey(int pSearchKey)
{
   mSearchKey = pSearchKey;
} // setSearchKey

void cxMenu::setAltPgUpKey(int pPgUpKey)
{
   mAltPgUpKey = pPgUpKey;
} // setAltPgUpkey

int cxMenu::getAltPgUpKey() const
{
   return(mAltPgUpKey);
} // getAltPgUpKey

void cxMenu::setAltPgDownKey(int pPgDownKey)
{
   mAltPgDownKey = pPgDownKey;
} // setAltPgDownKey

int cxMenu::getAltPgDownKey() const
{
   return(mAltPgDownKey);
} // getAltPgDownKey

void cxMenu::setStatus(const string& pStatus, bool pRefreshStatus)
{
   cxWindow::setStatus(pStatus, pRefreshStatus);
   mCustomStatus = true;
} // setStatus

void cxMenu::disableCustomStatus(bool pRefreshStatus)
{
   cxWindow::setStatus("", pRefreshStatus);
   mCustomStatus = false;
   useHelpAsStatus(pRefreshStatus);
} // disableCustomStatus

cxMenu& cxMenu::operator =(const cxMenu& pThatMenu)
{
   // Only try to copy pThatMenu if it's a different
   //  instance.
   if (&pThatMenu != this)
   {
      copyCxMenuStuff(&pThatMenu);
   }

   return(*this);
} // operator =

void cxMenu::refreshMenuItems()
{
   drawMessage();
   wrefresh(mSubWindow);
} // refreshMenuItems

bool cxMenu::setItemTextByReturnCode(long pReturnCode, const string& pItemText,
      bool pSetAll, bool pRefresh)
      {
   bool foundIt = false; // Whether or not the return code was found

   // Look for indexes in mReturnCodes with a matching return code
   unsigned numItems = mMessageLines.size();
   unsigned i = 0;
   for (; i < numItems; ++i)
   {
      if (mReturnCodes[i] == pReturnCode)
      {
         foundIt = true; // We found it!
         // If pItemText is longer than the width of the subwindow, then
         //  truncate it before adding it to mMessageLines.
         if (pItemText.length() > (unsigned)mSubWinWidth)
         {
            mMessageLines[i] = pItemText.substr(0, (unsigned)mSubWinWidth-1);
         }
         else
         {
            mMessageLines[i] = pItemText; // Set the item text
         }

         if (pRefresh)
         {
            // If the item is currently in the window, refresh it.
            if ((i >= (unsigned)mTopMenuItem) &&
                (i < ((unsigned)mTopMenuItem+(unsigned)mSubWinHeight)))
                {
               int subwinRow = (int)i - mTopMenuItem;
               // If pIndex is the current menu item and this menu is modal,
               //  highlight it when it's refreshed.
               if ((i == (unsigned)mCurrentMenuItem) && (isModal()))
               {
                  drawMenuItem((int)i, subwinRow, true, true);
               }
               else
               {
                  drawMenuItem((int)i, subwinRow, true, false);
               }
            }
         }

         // If we aren't to set all items with the matching return code,
         //  then quit out of the loop.
         if (!pSetAll)
         {
            break;
         }
      }
   }

   return(foundIt);
} // setItemTextByReturnCode

bool cxMenu::setItemTextByIndex(int pIndex, const string& pItemText,
      bool pRefresh)
      {
   bool retval = false;

   if ((pIndex >= 0) && (pIndex < (int)mMessageLines.size()))
   {
      retval = true;

      // If pItemText is longer than the width of the subwindow, then
      //  truncate it before adding it to mMessageLines.
      if (pItemText.length() > (unsigned)mSubWinWidth)
      {
         mMessageLines[pIndex] = pItemText.substr(0, (unsigned)mSubWinWidth-1);
      }
      else
      {
         mMessageLines[pIndex] = pItemText; // Set the item text
      }

      if (pRefresh)
      {
         // If the item is currently in the window, refresh it.
         if ((pIndex >= mTopMenuItem) && (pIndex < (mTopMenuItem+mSubWinHeight)))
         {
            int subwinRow = pIndex - mTopMenuItem;
            // If pIndex is the current menu item, highlight it
            //  when it's refreshed.
            if (pIndex == mCurrentMenuItem)
            {
               drawMenuItem(pIndex, subwinRow, true, true);
            }
            else
            {
               drawMenuItem(pIndex, subwinRow, true, false);
            }
         }
      }
   }

   return(retval);
} // setItemTextByIndex

void cxMenu::setBorderStyle(eBorderStyle pBorderStyle)
{
   if (getBorderStyle() != pBorderStyle)
   {
      cxWindow::setBorderStyle(pBorderStyle);
      freeSubWindow();
      reCreateSubWindow();
   }
} // setBorderStyle

bool cxMenu::itemExists(const string& pItemText, bool pUseDisplayText) const
{
   bool exists = false;

   if (pUseDisplayText)
   {
      messageLineContainer::const_iterator iter = mMessageLines.begin();
      for (; iter != mMessageLines.end(); ++iter)
      {
         // Check the item text verbatim and the item text without the hotkey
         //  characters
         if ((*iter == pItemText) ||
             (stringWithoutHotkeyChars(*iter) == pItemText))
             {
            exists = true;
            break;
         }
      }
   }
   else
   {
      // pUseDisplayText is false - Check using the alternate item texts.
      vector<string>::const_iterator iter = mAltItemText.begin();
      for (; iter != mAltItemText.end(); ++iter)
      {
         if (*iter == pItemText)
         {
            exists = true;
            break;
         }
      }
   }

   return(exists);
} // itemExists

void cxMenu::setAllowQuit(bool pAllowQuit)
{
   mAllowQuit = pAllowQuit;
} // setAllowQuit

bool cxMenu::getAllowQuit() const
{
   return(mAllowQuit);
} // getAllowQuit

void cxMenu::setAllowExit(bool pAllowExit)
{
   mAllowExit = pAllowExit;
} // setAllowExit

bool cxMenu::getAllowExit() const
{
   return(mAllowExit);
} // getAllowExit

int cxMenu::getIndexByReturnCode(long pReturnCode) const
{
   int index = -1;

   int i = 0;
   vector<long>::const_iterator iter = mReturnCodes.begin();
   for (; iter != mReturnCodes.end(); ++iter)
   {
      if ((*iter) == pReturnCode)
      {
         index = i;
         break;
      }

      ++i;
   }

   return(index);
} // getIndexByReturnCode

string cxMenu::getItemTextByReturnCode(long pReturnCode, bool pGetDisplayText)
{
   string itemText;

   // Get the item's index, and if the index is valid, get the item text.
   int itemIndex = getIndexByReturnCode(pReturnCode);
   if (itemIndex != -1)
   {
      itemText = getItemText(itemIndex, pGetDisplayText);
   }

   return(itemText);
} // getItemTextByReturnCode

void cxMenu::setLoopStartFunction(const shared_ptr<cxFunction>& pFuncPtr)
{
   mLoopStartFunction= pFuncPtr;
} // setLoopStartFunction

void cxMenu::setLoopEndFunction(const shared_ptr<cxFunction>& pFuncPtr)
{
   mLoopEndFunction = pFuncPtr;
} // setLoopEndFunction

bool cxMenu::runLoopStartFunction()
{
   bool exitAfterRun = false;

   if (mLoopStartFunction != nullptr && mLoopStartFunction->functionIsSet())
   {
      exitAfterRun = mLoopStartFunction->getExitAfterRun();
      mLoopStartFunction->runFunction();
   }

   return(exitAfterRun);
} // runLoopStartFunction

bool cxMenu::runLoopEndFunction()
{
   bool exitAfterRun = false;

   if (mLoopEndFunction != nullptr && mLoopEndFunction->functionIsSet())
   {
      exitAfterRun = mLoopEndFunction->getExitAfterRun();
      mLoopEndFunction->runFunction();
   }

   return(exitAfterRun);
} // runLoopEndFunction

cxMenuItemType cxMenu::getItemType(unsigned pIndex) const
{
   cxMenuItemType itemType = (cxMenuItemType)(-1);

   if ((pIndex >= 0) && (pIndex < mItemTypes.size()))
   {
      itemType = mItemTypes[pIndex];
   }

   return(itemType);
} // getItemType

cxMenuItemType cxMenu::getItemType(const string& pItemText) const
{
   cxMenuItemType itemType = (cxMenuItemType)(-1);

   // Look through mMessageLines for the item text, and if it is found,
   //  get the type of the item.
   unsigned numItems = numMenuItems();
   for (unsigned i = 0; i < numItems; ++i)
   {
      if (mMessageLines[i] == pItemText)
      {
         itemType = getItemType(i);
         break;
      }
   }

   return(itemType);
} // getItemType

void cxMenu::setWrapping(bool pWrap)
{
   mWrap = pWrap;
} // setWrapping

bool cxMenu::getWrapping() const
{
   return(mWrap);
} // getWrapping

void cxMenu::setWaitForInputIfEmpty(bool pWaitForInputIfEmpty)
{
   mWaitForInputIfEmpty = pWaitForInputIfEmpty;
} // setWaitForInputIfEmpty

bool cxMenu::getWaitForInputIfEmpty() const
{
   return(mWaitForInputIfEmpty);
} // getWaitForInputIfEmpty

int cxMenu::getCurrentMenuItem() const
{
   return(mCurrentMenuItem);
} // getCurrentMenuItem

void cxMenu::setCurrentMenuItem(int pItemIndex, bool pRefresh)
{
   // Only do this if pItemIndex is within bounds
   if ((pItemIndex >= 0) && (pItemIndex < (int)(mMessageLines.size())))
   {
      mCurrentMenuItem = pItemIndex;
      // If the current menu item is off the menu, then set it as the top
      //  menu item.
      if (mCurrentMenuItem < mTopMenuItem)
      {
         setTopItem((unsigned)mCurrentMenuItem, false);
      }

      if (pRefresh)
      {
         cxMenu::show(false, false);
      }
   }
} // setCurrentMenuItem

void cxMenu::setExitWhenLeaveFirst(bool pExitWhenLeaveFirst)
{
   mExitWhenLeaveFirst = pExitWhenLeaveFirst;
} // setExitWhenLeaveFirst

bool cxMenu::getExitWhenLeaveFirst() const
{
   return(mExitWhenLeaveFirst);
} // getExitWhenLeaveFirst

void cxMenu::setExitWhenLeaveLast(bool pExitWhenLeaveLast)
{
   mExitWhenLeaveLast = pExitWhenLeaveLast;
} // setExitWhenLeaveLast

bool cxMenu::getExitWhenLeaveLast() const
{
   return(mExitWhenLeaveLast);
} // getExitWhenLeaveLast

void cxMenu::setRefreshItemsWhenModal(bool pRefreshItemsWhenModal)
{
   mRefreshItemsWhenModal = pRefreshItemsWhenModal;
} // setRefreshItemsWhenmodal

bool cxMenu::getRefreshItemsWhenModal() const
{
   return(mRefreshItemsWhenModal);
} // getRefreshItemsWhenModal

void cxMenu::addAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if (pItem != eMENU_SELECTION)
   {
      cxWindow::addAttr(pItem, pAttr);
      cxWindow::addAttr(pItem, pAttr);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eMENU_SELECTION: // Menu selection
            attrSet = &mMenuSelectionAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eLABEL:          // Labels
         case eDATA:           // Data items (general)
         case eDATA_READONLY:  // Read-only data items
         case eDATA_EDITABLE:  // Editable data items
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Insert the attribute, if attrSet was set.
      if (nullptr != attrSet)
      {
         attrSet->insert(pAttr);
      }
   }
} // addAttr

void cxMenu::setAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if (pItem != eMENU_SELECTION)
   {
      cxWindow::setAttr(pItem, pAttr);
      cxWindow::setAttr(pItem, pAttr);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eMENU_SELECTION: // Menu selection
            attrSet = &mMenuSelectionAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkeys (handled in cxWindow)
         case eLABEL:          // Labels
         case eDATA:           // Data items (general)
         case eDATA_READONLY:  // Read-only data items
         case eDATA_EDITABLE:  // Editable data items
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Set the attribute, if attrSet was set.
      if (nullptr != attrSet)
      {
         attrSet->clear();
         attrSet->insert(pAttr);
      }
   }
} // setAttr

void cxMenu::removeAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if (pItem != eMENU_SELECTION)
   {
      cxWindow::removeAttr(pItem, pAttr);
      cxWindow::removeAttr(pItem, pAttr);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eMENU_SELECTION: // Menu selection
            attrSet = &mMenuSelectionAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eLABEL:          // Labels
         case eDATA:           // Data items (general)
         case eDATA_READONLY:  // Read-only data items
         case eDATA_EDITABLE:  // Editable data items
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Remove the attribute, if attrSet was set.
      if (nullptr != attrSet)
      {
         attrSet->erase(pAttr);
      }
   }
} // removeAttr

void cxMenu::removeAttrs(e_WidgetItems pItem)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if (pItem != eMENU_SELECTION)
   {
      cxWindow::removeAttrs(pItem);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eMENU_SELECTION: // Menu selection
            attrSet = &mMenuSelectionAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eLABEL:          // Labels
         case eDATA:           // Data items (general)
         case eDATA_READONLY:  // Read-only data items
         case eDATA_EDITABLE:  // Editable data items
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Remove the attributes, if attrSet was set.
      if (nullptr != attrSet)
      {
         attrSet->clear();
      }
   }
} // removeAttrs

void cxMenu::getAttrs(e_WidgetItems pItem, set<attr_t>& pAttrs) const
{
   pAttrs.clear();

   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if (pItem != eMENU_SELECTION)
   {
      cxWindow::getAttrs(pItem, pAttrs);
   }
   else
   {
      switch(pItem)
      {
         case eMENU_SELECTION: // Menu selection
            pAttrs = mMenuSelectionAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eLABEL:          // Labels
         case eDATA:           // Data items (general)
         case eDATA_READONLY:  // Read-only data items
         case eDATA_EDITABLE:  // Editable data items
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }
   }
} // getAttrs

void cxMenu::setHotkeyHighlighting(bool pHotkeyHighlighting)
{
   mHotkeyHighlighting = pHotkeyHighlighting;
} // setHotkeyHighlighting

string cxMenu::cxTypeStr() const
{
   return("cxMenu");
} // cxKeyStr

void cxMenu::sortByReturnCode(bool pRefresh)
{
   // This method sorts by removing all items and re-inserting them in the
   //  order of their return codes.

   // Make a backup of the members that store item information, in case
   //  something goes wrong.
   messageLineContainer messageLinesBackup = mMessageLines;
   vector<long> returnCodesBackup = mReturnCodes;
   vector<string> helpStringsBackup = mHelpStrings;
   vector<cxMenuItemType> itemTypesBackup = mItemTypes;
   map<int, cxMenu*> submenusBackup = mSubMenus;
   set<unsigned> unselectableItemsBackup = mUnselectableItems;
   try
   {
      // Store the current return code so that the same item can be
      //  highlighted after the menu is sorted.
      long selectedReturnCode = getReturnCode(getCurrentMenuItem());

      // Store backups of the items and their attributes by return code.
      //  Note: This has the side-effect that if the same return code exists
      //  for more than 1 item, then the attributes (sub-menu, selectability,
      //  etc.) will be applied to each item that had that return code, whether
      //  it had the attribute originally or not.
      map<long, string> codesAndItems;
      map<long, string> codesAndHelpStrings;
      map<long, cxMenuItemType> codesAndItemTypes;
      map<long, cxMenu*> codesAndSubMenus;
      set<long> unselectableCodes;
      long returnCode = 0;
      int numItems = (int)(numMenuItems());
      for (int i = 0; i < numItems; ++i)
      {
         returnCode = getReturnCode(i);
         codesAndItems[returnCode] = getItemText(i);
         codesAndHelpStrings[returnCode] = mHelpStrings[i];
         codesAndItemTypes[returnCode] = mItemTypes[i];
         if (mSubMenus.find(i) != mSubMenus.end())
         {
            codesAndSubMenus[returnCode] = mSubMenus[i];
         }
         if (mUnselectableItems.find((unsigned)i) != mUnselectableItems.end())
         {
            unselectableCodes.insert(returnCode);
         }
      }
      // Remove the items, and add them back in order of return code
      removeAllItems(false, false);
      int lastItemIndex = 0;
      map<long, string>::iterator iter = codesAndItems.begin();
      for (; iter != codesAndItems.end(); ++iter)
      {
         append(iter->second, iter->first, codesAndHelpStrings[iter->first],
                codesAndItemTypes[iter->first]);
         lastItemIndex = (int)(mMessageLines.size()) - 1;
         // If there is a submenu or unselectability flag for the item, then
         //  add it.
         if (codesAndSubMenus.find(iter->first) != codesAndSubMenus.end())
         {
            mSubMenus[lastItemIndex] = codesAndSubMenus[iter->first];
         }
         if (unselectableCodes.find(iter->first) != unselectableCodes.end())
         {
            mUnselectableItems.insert((unsigned)lastItemIndex);
         }
      }

      // Select the same item that was selected when the menu was sorted.
      int index = getIndexByReturnCode(selectedReturnCode);
      setCurrentMenuItem(index, false);

      // If pRefresh is true, then refresh the menu.
      if (pRefresh)
      {
         drawMessage();
         wrefresh(mSubWindow);
      }
   }
   catch (const std::out_of_range& e)
   {
      // Retore the backups
      mMessageLines = messageLinesBackup;
      mReturnCodes = returnCodesBackup;
      mHelpStrings = helpStringsBackup;
      mItemTypes = itemTypesBackup;
      mSubMenus = submenusBackup;
      mUnselectableItems = unselectableItemsBackup;
   }
   catch (...)
   {
      // Retore the backups
      mMessageLines = messageLinesBackup;
      mReturnCodes = returnCodesBackup;
      mHelpStrings = helpStringsBackup;
      mItemTypes = itemTypesBackup;
      mSubMenus = submenusBackup;
      mUnselectableItems = unselectableItemsBackup;
   }
} // sortByReturnCode

long cxMenu::getHighestReturnCode() const
{
   long highestReturnCode = 0;

   vector<long>::const_iterator iter = mReturnCodes.begin();
   for (; iter != mReturnCodes.end(); ++iter)
   {
      if (*iter > highestReturnCode)
      {
         highestReturnCode = *iter;
      }
   }

   return(highestReturnCode);
} // getHighestReturnCode

bool cxMenu::itemWasSelected() const
{
   bool retval = false;

   // If the return code isn't cxID_QUIT or cxID_EXIT, then if an item was
   //  selected, the last keypress should have been enter or a mouse click,
   //  and the return code should be within mReturnCodes.  If the current
   //  item is a submenu, then use that menu's itemWasSelected().
   long returnCode = getReturnCode();
   if ((returnCode != cxID_QUIT) && (returnCode != cxID_EXIT))
   {
      int lastKey = getLastKey();
      bool mouseClickedInWindow = false;
      if (lastKeyWasMouseEvt())
      {
         mouseClickedInWindow = ((mouseButton1Clicked() ||
                                  mouseButton1DoubleClicked()) &&
                                 mouseEvtWasInWindow());
      }
      if ((lastKey == ENTER) || (lastKey == KEY_ENTER) ||
          mouseClickedInWindow)
          {
         // Try to get the menu item index by the return code (this may be more
         //  accurate, since an event function might set the current menu item).
         //  If it is less than 0, then revert to mCurrentMenuItem.
         int menuItem = getIndexByReturnCode(returnCode);
         if (menuItem < 0)
         {
            menuItem = mCurrentMenuItem;
         }
         if (menuItem >= 0)
         {
            // Since we were able to get a menu item from the return code,
            //  then retval should be true, because the return code exists
            //  in mReturnCodes.
            retval = true;
            // If the currently-selected item has a submenu, then use that
            //  menu's itemWasSelected().
            if ((mItemTypes[menuItem] == cxITEM_SUBMENU) ||
                (mItemTypes[menuItem] == cxITEM_POPUPMENU))
                {
               // Since the item is a submenu, mSubMenus should have an entry
               //  for menuItem, but check anyway, just in case.
               map<int, cxMenu*>::const_iterator iter = mSubMenus.find(menuItem);
               if (iter != mSubMenus.end())
               {
                  retval = iter->second->itemWasSelected();
               }
            }
         }
      }
   }

   return(retval);
} // itemWasSelected

bool cxMenu::mouseEvtWasInItemArea(int& pItemIndex) const
{
   bool retval = false;
   pItemIndex = -1;

   // If the mouse event was in the menu window, see if it was within the
   //  borders (if there are borders).  If so, then see if there's a menu
   //  item there, and if so, set pItemIndex.
   if (mouseEvtWasInWindow())
   {
      // If there are borders, see if the mouse event is within the borders.
      if (getBorderStyle() != eBS_NOBORDER)
      {
         retval = ((mMouse.y > top()) && (mMouse.y < bottom()) &&
                   (mMouse.x > left()) && (mMouse.x < right()));
      }
      else
      {
         // No borders
         retval = true;
      }

      if (retval) { // If the user clicked in the menu item area..
         // If there are some menu items, then see if the user clicked on one.
         int numItems = (int)(cxMenu::numMenuItems());
         if (numItems > 0)
         {
            // In case there are less items than will fill the menu, set minimum
            //  and maximum line #s in the menu that contain items.  These will be
            //  relative to the screen, just like mMouse.y and mMouse.x.
            int topItemLocation = top();                   // Assuming no borders
            int bottomItemLocation = top() + height() - 1; // Assuming no borders
            // If there are borders, fix topItemLocation and bottomItemLocation.
            if (getBorderStyle() != eBS_NOBORDER)
            {
               ++topItemLocation;
               --bottomItemLocation;
            }
            // If there are less items than will fill the menu, then fix
            //  bottomItemLocation.  (There is one item per row.)
            if (numItems < mSubWinHeight)
            {
               bottomItemLocation = topItemLocation + numItems - 1;
            }

            // If the mouse event was in the list of menu items, then determine
            //  which item was clicked on and set pItemIndex.
            if ((mMouse.y >= topItemLocation) && (mMouse.y <= bottomItemLocation))
            {
               // Find the distance from the top row, and set pItemIndex based
               //  on mTopItemIndex and the distance from the top.
               int distFromTop = mMouse.y - topItemLocation;
               pItemIndex = mTopMenuItem + distFromTop;
            }
         }
      }
   }

   return(retval);
} // mouseEvtWasInItemArea

bool cxMenu::setOnSelectItemFunction(funcPtr4 pFunction, void *p1, void *p2,
                                     void *p3, void *p4, bool pExitAfterRun,
                                     bool pRunOnLeaveFunction)
                                     {
   // Free the memory used by the current mOnSelectItemFunction, and then
   //  create it with the new options.
   freeOnSelectItemFunction();
   mOnSelectItemFunction = make_shared<cxFunction4>(pFunction, p1, p2, p3, p4, false,
                                           pExitAfterRun, pRunOnLeaveFunction);
   return(mOnSelectItemFunction != nullptr);
} // setOnSelectItemFunction

bool cxMenu::setOnSelectItemFunction(funcPtr2 pFunction, void *p1, void *p2,
                                     bool pExitAfterRun,
                                     bool pRunOnLeaveFunction)
                                     {
   // Free the memory used by the current mOnSelectItemFunction, and then
   //  create it with the new options.
   freeOnSelectItemFunction();
   mOnSelectItemFunction = make_shared<cxFunction2>(pFunction, p1, p2, false,
                                           pExitAfterRun, pRunOnLeaveFunction);
   return(mOnSelectItemFunction != nullptr);
} // setOnSelectItemFunction

bool cxMenu::setOnSelectItemFunction(funcPtr0 pFunction, bool pExitAfterRun,
                                     bool pRunOnLeaveFunction)
                                     {
   // Free the memory used by the current mOnSelectItemFunction, and then
   //  create it with the new options.
   freeOnSelectItemFunction();
   mOnSelectItemFunction = make_shared<cxFunction0>(pFunction, false, pExitAfterRun,
                                           pRunOnLeaveFunction);
   return(mOnSelectItemFunction != nullptr);
} // setOnSelectItemFunction

shared_ptr<cxFunction> cxMenu::getOnSelectItemFunction() const
{
   return(mOnSelectItemFunction);
} // getOnSelectItemFunction

//// Protected functions

void cxMenu::copyCxMenuStuff(const cxMenu* pThatMenu)
{
   if ((pThatMenu != nullptr) && (pThatMenu != this))
   {
      freeSubWindow();
      // Copy the cxWindow stuff inherited from the parent, then copy
      //  this class' stuff
      try
      {
         copyCxWinStuff((const cxWindow*)pThatMenu);
      }
      catch (const cxWidgetsException& exc)
      {
         throw(cxWidgetsException("Couldn't copy base cxWindow stuff (copying a cxMenu)."));
      }

      mMenuSelectionAttrs = pThatMenu->mMenuSelectionAttrs;

      mSubWinHeight = pThatMenu->mSubWinHeight;
      mSubWinWidth = pThatMenu->mSubWinWidth;

      reCreateSubWindow();

      mCurrentMenuItem = pThatMenu->mCurrentMenuItem;
      mTopMenuItem = pThatMenu->mTopMenuItem;
      mAltPgUpKey = pThatMenu->mAltPgUpKey;
      mAltPgDownKey = pThatMenu->mAltPgDownKey;
      mSearchKey = pThatMenu->mSearchKey;
      mSearchText = pThatMenu->mSearchText;
      mLastSearchText = pThatMenu->mLastSearchText;
      mClearOnSearch = pThatMenu->mClearOnSearch;
      mCaseSensitiveSearch = pThatMenu->mCaseSensitiveSearch;
      mCustomStatus = pThatMenu->mCustomStatus;
      mAllowQuit = pThatMenu->mAllowQuit;
      mAllowExit = pThatMenu->mAllowExit;
      mSelectableItemExists = pThatMenu->mSelectableItemExists;
      mWrap = pThatMenu->mWrap;
      mWaitForInputIfEmpty = pThatMenu->mWaitForInputIfEmpty;
      mReturnCodes = pThatMenu->mReturnCodes;
      mHelpStrings = pThatMenu->mHelpStrings;
      mItemTypes = pThatMenu->mItemTypes;
      mAltItemText = pThatMenu->mAltItemText;
      mSubMenus = pThatMenu->mSubMenus;
      mUnselectableItems = pThatMenu->mUnselectableItems;
      mItemHotkeyIndex = pThatMenu->mItemHotkeyIndex;
      mLastItemHotkey = pThatMenu->mLastItemHotkey;
      mExitWhenLeaveFirst = pThatMenu->mExitWhenLeaveFirst;
      mExitWhenLeaveLast = pThatMenu->mExitWhenLeaveLast;
      mRefreshItemsWhenModal = pThatMenu->mRefreshItemsWhenModal;
      setHotkeyHighlighting(pThatMenu->getHotkeyHighlighting());
   }
} // copyCxMenuStuff

int cxMenu::subWinTop() const
{
   int topRow, leftCol;
   getbegyx(mSubWindow, topRow, leftCol);

   return(topRow);
} // subWinTop

int cxMenu::subWinLeft() const
{
   int topRow, leftCol;
   getbegyx(mSubWindow, topRow, leftCol);
   (void)topRow;
   return(leftCol);
} // subWinLeft

long cxMenu::doInputLoop(bool& pRunOnLeaveFunction)
{
   pRunOnLeaveFunction = true;
   // Set the default return code to cxID_QUIT to represent
   //  the user quitting out of the menu with no selection.
   long returnCode = cxID_EXIT;

   // For dragging the window around
   int pressedX = 0;
   int pressedY = 0;
   bool movingWin = false;

   // If there is at least 1 menu item, then allow the user to make a
   //  selection.
   if (mMessageLines.size() > 0)
   {
      // If there is at least 1 selectable menu item,
      //  then we can do the input loop.  Otherwise,
      //  just wait for a keypress.
      if (mSelectableItemExists)
      {
         // Go to the next selectable item
         goToSelectableItem(true, false);
         string searchStr;
         searchStr += mSearchKey;

         mLeaveNow = false;
         bool continueOn = true;
         bool selectItem = false; // Whether or not to do the "select item" behavior
         // Continue the input loop while continueOn is true, there is still
         //  at least 1 selectable item, and mLeaveNow is false.
         while (continueOn && mSelectableItemExists && !mLeaveNow)
         {
            selectItem = false;

            // Run the loop start function, and break from
            //  the input loop if its mExitAfterRun is true.
            if (runLoopStartFunction())
            {
               break;
            }

            // Use the currently-selected menu item's help as
            //  the status for this window.
            useHelpAsStatus();
            drawStatus();

            // Get the return code from the most recently shown
            //  menu item.
            returnCode = mReturnCodes[mCurrentMenuItem];

            // Highlight the current menu item
            int subwinRow = mCurrentMenuItem - mTopMenuItem;
            drawMenuItem(mCurrentMenuItem, subwinRow, true, true);

            // Get the key typed by the user.
            int lastKey = wgetch(mWindow);
            cxWindow::setLastKey(lastKey);

            // If the key is different from mLastItemHotkey, then reset
            //  the item hotkey info (so that if the key is a hotkey for
            //  an item, it will start at the beginning if more than 1
            //  item has the hotkey).
            if (lastKey != mLastItemHotkey)
            {
               mItemHotkeyIndex = 0;
               mLastItemHotkey = NOKEY;
            }

            // Handle mouse events first.
            mLastInputWasMouseEvent = false;
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
               mLastInputWasMouseEvent = true;
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
                     if (mouseEvtWasButtonEvt()) { // If it was a mouse button event
                        if (mouseEvtWasInWindow())
                        {
                           switch (mMouse.bstate)
                           {
                              case BUTTON1_CLICKED:
                              case BUTTON1_DOUBLE_CLICKED:
                                 {
                                    int itemIndex = -1;
                                    if (mouseEvtWasInItemArea(itemIndex))
                                    {
                                       if (itemIndex > -1)
                                       {
                                          // Set the current item
                                          setCurrentMenuItem(itemIndex, true);
                                          // Set selectItem to true so that the
                                          //  item gets "selected".
                                          selectItem = true;
                                       }
                                    }
                                    else
                                    {
                                       // If there are borders, then handle scrolling.
                                       // If the user clicked the top or bottom arrow in
                                       //  the right border, then scroll up or down by 1.
                                       //  If the user clicked between the arrows, then
                                       //  scroll up or down by a page.
                                       if (hasBorder())
                                       {
                                          // Do scrolling up/down by 1 if the user
                                          //  clicked in the right border just above
                                          //  the bottom or just below the top.
                                          if (mMouse.x == right())
                                          {
                                             if (mMouse.y == top()+1)
                                             {
                                                // Scroll up by 1
                                                scrollUpOne(continueOn);
                                             }
                                             else if (mMouse.y == bottom()-1)
                                             {
                                                // Scroll down by 1
                                                scrollDownOne(continueOn);
                                             }
                                             else
                                             {
                                                // Do pagewise scrolling (note: the menu
                                                //  should be at least 6 characters high
                                                //  to have room for the areas to click
                                                //  in on the border).
                                                if (height() >= 6)
                                                {
                                                   // Find the vertical boundaries of
                                                   //  the click areas
                                                   int upperHalfTop = top() + 2;
                                                   int upperHalfBottom = top()
                                                                + (height()/2);
                                                   int bottomHalfTop = bottom()
                                                                - (height()/2);
                                                   int bottomHalfBottom = bottom() - 2;
                                                   // If the mouse was clicked in the
                                                   //  top half, do a page up.  If the
                                                   //  mouse was clicked in the bottom
                                                   //  half, do a page down.
                                                   if ((mMouse.y >= upperHalfTop) &&
                                                         (mMouse.y <= upperHalfBottom))
                                                         {
                                                      doPgUp();
                                                   }
                                                   else if ((mMouse.y >= bottomHalfTop) &&
                                                         (mMouse.y <= bottomHalfBottom))
                                                         {
                                                      doPgDown();
                                                   }
                                                }
                                             }
                                          }
                                       }
                                    }
                                 }
                                 break;
                                 // TODO: CLicking and dragging the window doesn't seem to work all the time.
                              case BUTTON1_PRESSED:
                                 // Update pressedX and pressedY
                                 pressedX = mMouse.x;
                                 pressedY = mMouse.y;
                                 // If they clicked in the title, then let them
                                 //  move the window.
                                 movingWin = mouseEvtWasInTitle();
                                 break;
                              case BUTTON1_RELEASED:
                                 // If the user is moving the window, then go ahead and
                                 //  move it.
                                 if (movingWin)
                                 {
                                    moveRelative(mMouse.y - pressedY, mMouse.x - pressedX, true);
                                    movingWin = false; // Reset the moving of the window
                                 }
                                 break;
                           }
                        }
                        else
                        {
                           // The mouse button event was outside the menu.  If
                           //  the parent window is a cxPanel, or if this menu
                           //  has at least 1 parent menu, then quit the input
                           //  loop.  This can allow the user to go to another
                           //  window (in a cxPanel) or go back up to the parent
                           //  menu.
                           if (parentIsCxPanel() || (mNumParentMenus > 0))
                           {
                              returnCode = cxID_EXIT;
                              continueOn = false;
                           }
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
            // If the last key is in the quit keys, then quit and return
            //  cxID_QUIT.  If the key isn't there, look for it in
            //  the exit keys (if it's there, quit and return cxID_EXIT).
            //  If not there either, handle the key normally.
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

            // If continueOn was set to false, then break out of the
            //  while loop.
            if (!continueOn)
            {
               break;
            }
            // If the user pressed enter or tab, and if the menu item is not
            //  a submenu, then get the return code before the hotkey function
            //  runs so that the return code is available to the hotkey function.
            bool gotReturnCodeAlready = false;
            if ((lastKey == ENTER) || (lastKey == KEY_ENTER) ||
                (lastKey == TAB) || (lastKey == SHIFT_TAB))
                {
               if (mItemTypes[mCurrentMenuItem] != cxITEM_SUBMENU)
               {
                  returnCode = getCurrentItemReturnCode(lastKey, continueOn);
                  setReturnCode(returnCode);
                  gotReturnCodeAlready = true;
               }
            }
            // Run a function that may be associated with the last
            //  keypress.  If no function exists for the key, then
            //  handle the keypress here.
            bool functionExists = false;
            continueOn = handleFunctionForLastKey(&functionExists);
            // Update mSelectableItemExists (in case there was a function
            //  for the last keypress that changed the selectability of
            //  any of the items).
            lookForSelectableItem();
            // If mLeaveNow was set true, then stop the input loop.
            if (mLeaveNow)
            {
               // Update returnCode before leaving (exitNow() and quitNow()
               //  set it).
               returnCode = getReturnCode();
               continueOn = false;
               break;
            }
            // Stop the input loop if continueOn was set false.
            if (!continueOn)
            {
               // Set the return code to cxID_QUIT or cxID_EXIT, but not
               //  if the user pressed enter or tab or if the key is in
               //  the list of exit keys for the window.
               if (lastKey == ESCAPE)
               {
                  returnCode = cxID_QUIT;
               }
               else
               {
                  if (((lastKey != ENTER) && (lastKey != KEY_ENTER) &&
                      (lastKey != TAB)) || hasExitKey(lastKey))
                      {
                     returnCode = cxID_EXIT;
                  }
               }
               break;
            }

            if (!functionExists)
            {
               //  Ideally, we would handle all keys in the
               //  switch statement, but C++ doesn't allow the
               //  use of a variable as a case of a switch
               //  statement, so we have to handle some keys
               //  in separate if tests.
               if (lastKey == mSearchKey)
               {
                  doSearch();
               }
               else if (lastKey == mAltPgUpKey)
               {
                  doPgUp();
               }
               else if (lastKey == mAltPgDownKey)
               {
                  doPgDown();
               }
               else
               {
                  switch(lastKey)
                  {
                     case ESCAPE: // Defined in cxKeyDefines.h
                        if (mAllowQuit)
                        {
                           returnCode = cxID_QUIT;
                           continueOn = false;
                        }
                        break;
                     case KEY_UP:
                        scrollUpOne(continueOn);
                        break;
                     case KEY_DOWN:
                        scrollDownOne(continueOn);
                        break;
                     case KEY_LEFT:
                        // Left arrow means the user wants to exit out of the menu
                        if (mAllowQuit)
                        {
                           returnCode = cxID_QUIT;
                           continueOn = false;
                        }
                        break;
                     case KEY_RIGHT:
                        // If the currently selected menu item is a submenu
                        //  type, set selectItem to true so that the submenu
                        //  will be shown.
                        if (mItemTypes[mCurrentMenuItem] == cxITEM_SUBMENU)
                        {
                           selectItem = true;
                        }
                        break;
                     // pageUp and pageDown should scroll by a height's worth
                     //  of items.  Also use ',' and '.' for pageUp and
                     //  pageDown, respectively.
                     case PAGE_DOWN:
                        doPgDown();
                        break;
                     case PAGE_UP:
                        doPgUp();
                        break;
                     case KEY_HOME:
                     case KEY_SEND:  // sEND=HOME
                        doHome();
                        break;
                     case KEY_END:
                     case KEY_END2:
                     case KEY_END3:
                     case KEY_SHOME: // sHOME=END
                        doEnd();
                        break;
                     // Enter: Select the current item
                     case ENTER:
                     case KEY_ENTER:
                        selectItem = true;
                        break;
                     case TAB:
                        break;
                     case SHIFT_TAB:
                        if (mAllowExit)
                        {
                           returnCode = cxID_EXIT;
                           continueOn = false;
                        }
                        break;
                     case ' ':
                        break;
                     case ERR: // Error getting a keypress
                        // Should something special be done if there
                        //  was an error getting a keypress?
                        break;
                     default:
                        {
                           // Look for the key pressed in mItemHotkeys.  If there
                           //  are any entries in mItemHotkeys for it, then go to
                           //  the menu item specified by the index in
                           //  mItemHotkeys.  If it has a submenu, then show it.
                           //  If not, then return its return code.  If there
                           //  is more than 1 entry in mItemHotkeys for the hotkey,
                           //  then the item will only be highlighted and
                           //  not automatically selected.
                           char hotkey = tolower((char)lastKey);
                           unsigned numHotKeyEntries = mItemHotkeys.count(hotkey);
                           if (numHotKeyEntries > 0)
                           {
                              // mItemHotkeyIndex will need to be reset to 0
                              //  if it is >= the number of entries in
                              //  mItemHotkeys for the hotkey that was pressed,
                              //  or if the user pressed a different hotkey
                              //  than last time.
                              if ((mItemHotkeyIndex >= numHotKeyEntries) ||
                                  (lastKey != mLastItemHotkey))
                                  {
                                 mItemHotkeyIndex = 0;
                              }
                              // Update mLastItemHotkey so we can test if
                              //  the next hotkey the user pressed is
                              //  different
                              mLastItemHotkey = lastKey;

                              multimap<char, int>::iterator iter =
                                     mItemHotkeys.lower_bound(hotkey);
                              // Increment the iterator based on mItemHotkeyIndex
                              //  so we're looking at the correct entry in
                              //  mItemHotkeys
                              for (unsigned i = 0; i < mItemHotkeyIndex; ++i)
                              {
                                 ++iter;
                              }
                              // Un-highlight the current menu item and
                              //  highlight the new menu item.
                              if (mCurrentMenuItem != iter->second)
                              {
                                 // Un-highlight the current item
                                 int currentItemRow = mCurrentMenuItem - mTopMenuItem;
                                 drawMenuItem(mCurrentMenuItem, currentItemRow, false, false);

                                 // Figure out where the newly-selected item is and
                                 //  highlight it
                                 int selectedItem = iter->second;
                                 int diff = selectedItem - currentItemRow;
                                 int selectedItemRow = currentItemRow + diff - mTopMenuItem;
                                 drawMenuItem(iter->second, selectedItemRow, true, true);
                              }

                              mCurrentMenuItem = iter->second;

                              // If the current menu item is outside the visible
                              //  area, then scroll the menu items so that it
                              //  is visible.
                              int bottomItem = mTopMenuItem + mSubWinHeight - 1;
                              if (mCurrentMenuItem < mTopMenuItem)
                              {
                                 // The current menu item is above the top
                                 //  item - Scroll up so that the item is
                                 //  visible
                                 int scrollAmt = mTopMenuItem - mCurrentMenuItem;
                                 scrollItems(-scrollAmt, true);
                              }
                              else if (mCurrentMenuItem > bottomItem)
                              {
                                 // The current menu item is below the bottom
                                 //  item  - Scroll down
                                 int scrollAmt = mCurrentMenuItem - bottomItem;
                                 scrollItems(scrollAmt, true);
                              }

                              // If there is only 1 entry in mItemHotkeys for the
                              //  hotkey that was pressed, then automatically
                              //  select it.
                              if (numHotKeyEntries == 1)
                              {
                                 // If the menu item is a submenu type, show its submenu.
                                 if ((mItemTypes[mCurrentMenuItem] == cxITEM_SUBMENU) ||
                                     (mItemTypes[mCurrentMenuItem] == cxITEM_POPUPMENU))
                                     {
                                    returnCode = doSubmenu();

                                    // Return the return code of the submenu if the user didn't
                                    //  exit or quit.
                                    if ((returnCode != cxID_EXIT) &&
                                        (returnCode != cxID_QUIT))
                                        {
                                       return(returnCode);
                                    }
                                 }
                                 else
                                 {
                                    // The menu item doesn't have a submenu..  Just
                                    //  return its return code.
                                    returnCode = mReturnCodes[mCurrentMenuItem];
                                    continueOn = false;
                                 }
                              }
                              else
                              {
                                 // There is more than 1 entry in mItemHotkeys
                                 //  for the hotkey that was pressed..
                                 //  Increment mItemHotkeyIndex so that we go
                                 //  onto the next entry next time.
                                 ++mItemHotkeyIndex;
                              }
                           }
                        }
                        break;
                  } // switch
               }
            }

            // If the user chose to select the item (via pressing the enter
            //  key or clicking on it), then if it's a submenu, show the
            //  submenu, but if not, select the item.
            if (selectItem)
            {
               // If the currently selected menu item is a
               //  submenu type, tell it to show its submenu.
               if (mItemTypes[mCurrentMenuItem] == cxITEM_SUBMENU)
               {
                  returnCode = doSubmenu();

                  // Return the return code of the submenu if the user didn't
                  //  exit or quit.
                  if ((returnCode != cxID_EXIT) && (returnCode != cxID_QUIT))
                  {
                     // Run the onSelectMenuItem function
                     bool exitAfterRun = false;
                     runOnSelectItemFunction(exitAfterRun, pRunOnLeaveFunction);
                     return(returnCode);
                  }
                  else
                  {
                     // If the submenu's last input was a mouse event, then
                     //  update mMouse, and see if the user clicked on an item
                     //  in this menu.  If so, highlight it.
                     if (mSubMenus.find(mCurrentMenuItem) != mSubMenus.end())
                     {
                        if (mSubMenus[mCurrentMenuItem]->mLastInputWasMouseEvent) {
#ifdef NCURSES_MOUSE_VERSION
                           mMouse = mSubMenus[mCurrentMenuItem]->getMouseEvent();
                           int itemIndex = -1;
                           if (mouseEvtWasInItemArea(itemIndex))
                           {
                              if (itemIndex > -1)
                              {
                                 highlightItem(itemIndex);
                                 //selectItem = true;
                                 // Set the current item, and set selectItem to
                                 //  true so that it gets "selected".
                                 setCurrentMenuItem(itemIndex, true);
                              }
                           }
#endif
                        }
                     }
                  }
               }
               else
               {
                  // For this case, we want to get the return code.
                  //  This is handled earlier so that the return code is
                  //  available to the hotkey functions.  If we already
                  //  got the return code, don't continue on; otherwise,
                  //  get the return code.
                  if (gotReturnCodeAlready)
                  {
                     continueOn = false;
                  }
                  else
                  {
                     returnCode = getCurrentItemReturnCode(lastKey, continueOn);
                  }
               }

               // Run the onSelectMenuItem function
               bool exitAfterRun = false;
               runOnSelectItemFunction(exitAfterRun, pRunOnLeaveFunction);
               continueOn = continueOn && !exitAfterRun;
            }

            // Run the end loop function.  Update continueOn
            //  based on its current value and the value of
            //  the end loop function's mExitAfterRun.
            continueOn = (continueOn && !runLoopEndFunction());
         } // while

         // Un-highlight the current menu item
         int subwinRow = mCurrentMenuItem - mTopMenuItem;
         drawMenuItem(mCurrentMenuItem, subwinRow, true, false);
      }
      else
      {
         // There are no selectable items in the menu.
         if (mWaitForInputIfEmpty)
         {
            cxWindow::setLastKey(wgetch(mWindow));
            handleFunctionForLastKey();
         }
      }
   } // if (mMessageLines.size() > 0)
   else
   {
      // There are no items in the menu, but at least show something and pause
      //  for user input.
      if (mWaitForInputIfEmpty)
      {
         cxWindow messageWindow(nullptr, top()+2, left()+2,
                     "Message", "This menu contains no items.", "");
         messageWindow.show();
         int lastKey = wgetch(mWindow);
         cxWindow::setLastKey(lastKey);
         messageWindow.hide();
         // If the last keypress was a mouse event, then run an external
         //  function that may be set up for it.
#ifdef NCURSES_MOUSE_VERSION
         if (lastKey == KEY_MOUSE)
         {
            mLastInputWasMouseEvent = true;
            if (getmouse(&mMouse) == OK)
            {
               // Run a function that may exist for the mouse state.
               handleFunctionForLastMouseState(nullptr, &pRunOnLeaveFunction);
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
         else if (lastKey == ESC)
         {
            returnCode = cxID_QUIT;
         }
         else
         {
            returnCode = cxID_EXIT;
         }
      }
   }

   // mLeaveNow should be false
   mLeaveNow = false;

   return(returnCode);
} // doInputLoop

void cxMenu::enableAttrs(WINDOW *pWin, e_WidgetItems pItem)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if (pItem != eMENU_SELECTION)
   {
      cxWindow::enableAttrs(pWin, pItem);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eMENU_SELECTION: // Menu selection
            attrSet = &mMenuSelectionAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eLABEL:          // Labels
         case eDATA:           // Data items (general)
         case eDATA_READONLY:  // Read-only data items
         case eDATA_EDITABLE:  // Editable data items
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Enable the attributes, if attrSet was set.
      if (nullptr != attrSet)
      {
         if (attrSet->size() > 0)
         {
            set<attr_t>::const_iterator iter = attrSet->begin();
            for (; iter != attrSet->end(); ++iter)
            {
               wattron(pWin, *iter);
            }
         }
         else
         {
            cxBase::enableAttrs(pWin, pItem);
         }
      }
      else
      {
         cxBase::enableAttrs(pWin, pItem);
      }
   }
} // enableAttrs

void cxMenu::disableAttrs(WINDOW *pWin, e_WidgetItems pItem)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if (pItem != eMENU_SELECTION)
   {
      cxWindow::disableAttrs(pWin, pItem);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eMENU_SELECTION: // Menu selection
            attrSet = &mMenuSelectionAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eLABEL:          // Labels
         case eDATA:           // Data items (general)
         case eDATA_READONLY:  // Read-only data items
         case eDATA_EDITABLE:  // Editable data items
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Enable the attributes, if attrSet was set.
      if (nullptr != attrSet)
      {
         if (attrSet->size() > 0)
         {
            set<attr_t>::const_iterator iter = attrSet->begin();
            for (; iter != attrSet->end(); ++iter)
            {
               wattroff(pWin, *iter);
            }
         }
         else
         {
            cxBase::disableAttrs(pWin, pItem);
         }
      }
      else
      {
         cxBase::disableAttrs(pWin, pItem);
      }
   }
} // disableAttrs

///// Private functions

// Returns the index of the bottommost item to show in the
//  subwindow.
inline int cxMenu::getBottomItemIndex()
{
   int bottomMenuItem = mTopMenuItem + mSubWinHeight - 1;
   // Make sure the index is within bounds.
   if (bottomMenuItem >= (int)mMessageLines.size())
   {
      bottomMenuItem = (int)mMessageLines.size() - 1;
   }

   return(bottomMenuItem);
}

// Shows the submenu for the current menu item and returns the
//  return code returned by the submenu.  Returns -1 if the
//  current menu item isn't a submenu type.
inline long cxMenu::doSubmenu()
{
   long returnCode = cxID_QUIT;
   cxMenu *subMenu = nullptr;

   if ((unsigned)mCurrentMenuItem < mItemTypes.size())
   {
      // If a menu exists for this menu item, then check if
      //  this item is supposed to have one.  If it's a submenu,
      //  get the pointer to it and move the submenu so it appears
      //  to the right of this menu and aligned with the menu item.
      //  If it's a pop-up menu, just get the pointer to it.
      if (mItemTypes[mCurrentMenuItem] == cxITEM_SUBMENU)
      {
         if (mSubMenus.find(mCurrentMenuItem) != mSubMenus.end())
         {
            // Move the item's submenu so that it's to the right
            //  of this menu.
            // itemRow is the row (relative to the menu) where the
            //  item resides.
            int itemRow = mCurrentMenuItem - mTopMenuItem + 1;

            subMenu = mSubMenus[mCurrentMenuItem];
            if (subMenu != nullptr)
            {
               subMenu->move(itemRow, right()+1, false);
            }
         }
      }
      else if (mItemTypes[mCurrentMenuItem] == cxITEM_POPUPMENU)
      {
         if (mSubMenus.find(mCurrentMenuItem) != mSubMenus.end())
         {
            subMenu = mSubMenus[mCurrentMenuItem];
         }
      }
   }

   // If we got a submenu for this item, show it and get
   //  its return code, etc.
   if (subMenu != nullptr)
   {
      returnCode = subMenu->showModal(true, true, true);
      subMenu->hide();

      // Re-draw the window (in case there were any submenus that
      //  appeared on top of the border).  This class is designed
      //  so that submenus share the right boder line with this
      //  menu, so the border should be all we'd have to re-draw -
      //  but from testing, it seems we have to re-draw everything..
      if ((returnCode == cxID_QUIT) || (returnCode == cxID_EXIT))
      {
         drawBorder();
         drawTitle();
         drawStatus();
         wrefresh(mWindow);
         drawMessage();
         wrefresh(mSubWindow);
      }
   }

   return(returnCode);
}

// Writes a menu item line (from mMessageLines) to the subwindow.
//  Checks for hotkeys (ampersands), and pads with spaces on the
//  right so it fills the entire row in the subwindow.
// pItemIndex: The index of the item (in mMessageLines) to write
// pSubWinRow: The row in the subwindow to write on
// pRefreshSubwindow: Whether or not to refresh the subwindow
// pHighlight: Whether or not to highlight the item
void cxMenu::drawMenuItem(int pItemIndex, int pSubWinRow, bool pRefreshSubwindow, bool pHighlight)
{
   string itemStr;
   // If pItemIndex is between 0 and the number of menu items, then use the
   //  string in mMessageLines at the given index.  Otherwise, use a string
   //  full of spaces that will fill the subwindow.
   if ((pItemIndex >= 0) && ((unsigned)pItemIndex < numMenuItems()))
   {
      itemStr = mMessageLines[pItemIndex].substr(0, mSubWinWidth);
   }
   else
   {
      itemStr.assign(mSubWinWidth, ' ');
   }

   wcolor_set(mSubWindow, mMessageColorPair, nullptr);
   scrollok(mSubWindow, false);
   // If highlighting is to be used, enable the menu selection attributes (i.e., A_REVERSE)
   e_WidgetItems item = eMESSAGE;
   if (pHighlight)
   {
      item = eMENU_SELECTION;
      enableAttrs(mSubWindow, item);
   }
   // Write the menu item to the subwindow
   writeWithHighlighting(mSubWindow, itemStr, pSubWinRow, 0, mSubWinWidth, item);

   //scrollok(mSubWindow, false);
   // currentCol is the current horizontal position in the window
   int currentCol = (int)cxBase::stringWithoutHotkeyChars(itemStr).length();

   // Fill the rest of the line with spaces (to fill
   //  in the background color & overwrite stuff that may
   //  have been output previously).
   int endIndex = mSubWinWidth;
   for (; currentCol < endIndex; ++currentCol)
   {
      mvwaddch(mSubWindow, pSubWinRow, currentCol, ' ');
   }
   scrollok(mSubWindow, true);

   // Turn off the menu selection attributes
   if (pHighlight)
   {
      disableAttrs(mSubWindow, eMENU_SELECTION);
   }

   // Refresh the subwindow (if needed).
   if (pRefreshSubwindow)
   {
      wrefresh(mSubWindow);
   }
} // drawMenuItem

// Scans a string for hotkeys and adds the hotkeys to mItemHotkeys, if the
//  string contains one.
void cxMenu::addHotKey(const string& pItemText)
{
   set<char> hotkeys;
   // Get the hotkey characters from the text, converting their
   //  case to lowercase.
   cxBase::getHotkeyChars(pItemText, hotkeys, true, false);
   set<char>::const_iterator iter = hotkeys.begin();
   for (; iter != hotkeys.end(); ++iter)
   {
      if (mMessageLines.size() > 0)
      {
         mItemHotkeys.insert(make_pair(*iter, mMessageLines.size()-1));
      }
   }
} // addHotKey

// Copies the current menu item's help string to the
//  status string, and optionally updates the status.
inline void cxMenu::useHelpAsStatus(bool pRefreshStatus)
{
   // Only do this if a custom status message isn't set.
   if (!mCustomStatus)
   {
      if (mCurrentMenuItem >= 0 && mCurrentMenuItem < (int)mHelpStrings.size())
      {
         string helpText = mHelpStrings[mCurrentMenuItem];
         // Truncate the help text if it's too long..  (Maybe we should
         //  resize the window instead, since a menu will likely be
         //  fairly narrow..  But we wouldn't want it to be too wide either..)
         if ((int)helpText.length() > width()-2)
         {
            helpText = helpText.substr(0, width()-2);
         }

         if (getStatus() != helpText)
         {
            // Use the base class' setStatus() instead of this
            //  class' setStatus() so that mCustomStatus doesn't get
            //  set to true.
            cxWindow::setStatus(helpText, pRefreshStatus);
         }
      }
   }
} // useHelpAsStatus

// Goes through the menu items and resizes their text
//  to make sure they're all exactly the width of the inside of
//  this window.
void cxMenu::fitItemsToWidth()
{
   unsigned index = 0;  // Index for mItemTypes, etc.
   messageLineContainer::iterator iter = mMessageLines.begin();
   for (; iter != mMessageLines.end(); ++iter)
   {
      // Shorten the menu item text if it's too long
      if ((int)iter->length() > mSubWinWidth)
      {
         *iter = iter->substr(0, mSubWinWidth);
      }
      ++index;
   }
} // fitItemsToWidth

void cxMenu::goToSelectableItem(bool pForward, bool pBringToTop)
{
   // Do this if there is at least 1 menu item and not all menu items
   //  are unselectable.
   if ((mMessageLines.size() > 0) &&
       (mUnselectableItems.size() < mMessageLines.size()))
       {
      // If mCurrentMenuItem is beyond the last or before the first
      //  item, then fix it.
      if (mCurrentMenuItem >= (int)numMenuItems())
      {
         if (mWrap)
         {
            if (pForward)
            {
               mCurrentMenuItem = 0;
            }
            else
            {
               mCurrentMenuItem = (int)numMenuItems() - 1;
            }
            goToSelectableItem(pForward, pBringToTop);
         }
         else
         {
            mCurrentMenuItem = (int)numMenuItems()-1;
            goToSelectableItem(pForward, pBringToTop);
         }
      }
      else if (mCurrentMenuItem < 0)
      {
         if (mWrap)
         {
            if (pForward)
            {
               mCurrentMenuItem = 0;
            }
            else
            {
               mCurrentMenuItem = (int)numMenuItems() - 1;
            }
            goToSelectableItem(pForward, pBringToTop);
         }
         else
         {
            mCurrentMenuItem = 0;
            goToSelectableItem(pForward, pBringToTop);
         }
      }
      else
      {
         int currentItem = mCurrentMenuItem;
         if (pForward)
         {
            while (mUnselectableItems.find((unsigned)currentItem) !=
                   mUnselectableItems.end())
                   {
               ++currentItem;
               // Wrap around if mCurrentMenuItem becomes too large and
               //  we're allowed to wrap.
               if ((currentItem >= (int)mMessageLines.size()) && mWrap)
               {
                  currentItem = 0;
                  while (mUnselectableItems.find((unsigned)currentItem) !=
                         mUnselectableItems.end())
                         {
                     ++currentItem;
                  }
               }
            }
         }
         else
         {
            while (mUnselectableItems.find((unsigned)mCurrentMenuItem) !=
                   mUnselectableItems.end())
                   {
               --mCurrentMenuItem;
               // Wrap around if mCurrentMenuItem becomes negative
               if (mCurrentMenuItem < 0)
               {
                  mCurrentMenuItem = (int)mMessageLines.size() - 1;
               }
            }
         }

         // If the current menu item is outside the current viewable
         //  area, then adjust the viewable area.
         if ((mCurrentMenuItem < mTopMenuItem) ||
             (mCurrentMenuItem >= (mTopMenuItem+mSubWinHeight)))
             {
            setTopItem(mCurrentMenuItem, true);
         }

         if (pBringToTop)
         {
            bringToTop(false);
         }
      }
   }
} // goToSelectableItem

void cxMenu::doSearch()
{
   // newSearch tells us whether to do a new
   //  search or repeat the last search
   bool newSearch = false;

   // winWidth will be used as the width of the
   //  input box and error window used in this
   //  method.
   int winWidth = (cxBase::width() > 25 ? 25 : cxBase::width());
   cxMultiLineInput searchInput(this, 1, 1, 1, winWidth, "Keyword:", eBS_SINGLE_LINE);
   searchInput.setTitle("Search", false);
   searchInput.center(false);
   searchInput.setBorderColor(eYELLOW_BLUE);
   searchInput.setLabelColor(eBRTCYAN_BLUE);
   // If mClearOnSearch is false, use mSearchText
   //  as the initial value for the input box.
   if (!mClearOnSearch)
   {
      searchInput.setValue(mSearchText);
   }
   // Set up a key for the input to clear the keyword.  This uses the return
   //  value of cxBase::noOp(), which does nothing and returns a blank string.
   searchInput.setKeyFunction(cxBase::getMenuClearKeywordKey(), cxBase::noOp,
                              nullptr, nullptr, true, false, true);

   if (searchInput.showModal() != ESC)
   {
      mSearchText = searchInput.getValue();
      // If mSearchText differs from mLastSearchText,
      //  we should do a new search.
      if (mSearchText != mLastSearchText)
      {
         newSearch = true;
      }
      // Update mLastSearchText
      mLastSearchText = searchInput.getValue();

      // If mSearchText isn't blank, do the search.
      if (mSearchText != "")
      {
         int item = 0;
         // If we're to do a new search, start from
         //  the first item.  Otherwise, start from
         //  the currently-selected item.
         if (newSearch)
         {
            item = 0;
         }
         else
         {
            item = mCurrentMenuItem+1;
         }

         int numMenuItems = (int)mMessageLines.size();
         bool found = false;
         for(; item < numMenuItems; ++item)
         {
            // If we should do a case-sensitive search,
            //  leave the case alone.  Otherwise, convert
            //  both the menu item & search text to upper-case
            //  before searching.
            if (mCaseSensitiveSearch)
            {
               found = Find(stringWithoutHotkeyChars(mMessageLines[item]), mSearchText);
            }
            else
            {
               string menuItemUpper = stringWithoutHotkeyChars(mMessageLines[item]);
               string searchTextUpper = mSearchText;
               toUpper(menuItemUpper);
               toUpper(searchTextUpper);
               found = Find(menuItemUpper, searchTextUpper);
            }

            if (found)
            {
               if (item != mCurrentMenuItem)
               {
                  // Un-highlight the current menu item
                  int subwinRow = mCurrentMenuItem - mTopMenuItem;
                  drawMenuItem(mCurrentMenuItem, subwinRow, false, false);

                  // Set & highlight the new item
                  mCurrentMenuItem = item;
                  // If the item is above the top menu item
                  //  or below the bottom menu item, then
                  //  scroll the items so that it's in view.
                  if ((item < mTopMenuItem) ||
                      (item >= mTopMenuItem+mSubWinHeight))
                      {
                     setTopItem(item, true);
                  }
               }
               break;
            }
         }

         // Show a message if the item wasn't found.
         if (!found)
         {
            cxWindow messageWin(this, 1, 1, 3, winWidth, "", "Not found");
            messageWin.center(true);
            messageWin.setBorderColor(eYELLOW_BLUE);
            messageWin.setMessageColor(eBRTCYAN_BLUE);
            messageWin.show();
            wgetch(mWindow);
         }
      }
   }
} // doSearch

void cxMenu::scrollUpOne(bool& pContinueOn)
{
   // Un-highlight the current menu item
   int subwinRow = mCurrentMenuItem - mTopMenuItem;
   drawMenuItem(mCurrentMenuItem, subwinRow, false, false);

   // Decrement mCurrentMenuItem to go to the previous
   //  item.  If it is below the first selectable item,
   //  then:
   //     If mExitWhenLeaveFirst is true, don't continue
   //     the input loop.  Otherwise, call
   //     goToSelectableItem() to skip past unselectable
   //     items.
   --mCurrentMenuItem;
   if (mCurrentMenuItem < firstSelectableItem())
   {
      if (mExitWhenLeaveFirst)
      {
         pContinueOn = false;
         // Increment mCurrentMenuItem so that it is
         //  on a selectable item for the next time
         //  the menu is shown.
         ++mCurrentMenuItem;
      }
      else
      {
         goToSelectableItem(false, false);
      }
   }

   // Scroll the menu up as needed
   if (mCurrentMenuItem < mTopMenuItem)
   {
      setTopItem((unsigned)mCurrentMenuItem, true);
   }
} // scrollUpOne

void cxMenu::scrollDownOne(bool& pContinueOn)
{
   // Un-highlight the current menu item
   int subwinRow = mCurrentMenuItem - mTopMenuItem;
   drawMenuItem(mCurrentMenuItem, subwinRow, false, false);

   // Increment mCurrentMenuItem to go to the next
   //  item.  If it is beyond the last selectable item,
   //  then:
   //     If mExitWhenLeaveLast is true, don't continue
   //     the input loop.  Otherwise, call
   //     goToSelectableItem() to skip past unselectable
   //     items.
   ++mCurrentMenuItem;
   if (mCurrentMenuItem > lastSelectableItem())
   {
      if (mExitWhenLeaveLast)
      {
         pContinueOn = false;
         // Decrement mCurrentMenuItem so that it is
         //  on a selectable item for the next time
         //  the menu is shown.
         --mCurrentMenuItem;
      }
      else
      {
         goToSelectableItem(true, false);
      }
   }

   // Scroll the menu down if needed
   if (mCurrentMenuItem > (mTopMenuItem + mSubWinHeight - 1))
   {
      setBottomItem((unsigned)mCurrentMenuItem, true);
   }
} // scrollDownOne

inline void cxMenu::doPgUp()
{
   // If the list is already at the top page and the user
   //  presses pageUp again, make the first item the
   //  current item.
   if (mTopMenuItem == 0)
   {
      mCurrentMenuItem = 0;
   }
   // Scroll up a page
   scrollItems(-mSubWinHeight, true);
} // doPgUp

// Scrolls down one page
inline void cxMenu::doPgDown()
{
   // If there is more than a pagefull of menu items, then
   //  we will need to scroll.
   if ((int)mMessageLines.size() > mSubWinHeight)
   {
      // If the list is already on the last page and the user
      //  presses pageDown again, make the last item the
      //  current item.
      if (mTopMenuItem == (int)mMessageLines.size() - mSubWinHeight)
      {
         mCurrentMenuItem = (int)mMessageLines.size() - 1;
      }
      // Scroll down a page
      scrollItems(mSubWinHeight, true);
   }
   else
   {
      // There is less than a pagefull of menu items..  Go to
      //  the last menu item.
      // Un-highlight the current menu item
      int subwinRow = mCurrentMenuItem - mTopMenuItem;
      drawMenuItem(mCurrentMenuItem, subwinRow, true, false);
      // Set the current menu item to be the last one
      mCurrentMenuItem = (int)mMessageLines.size() - 1;
      // Highlight the current menu item
      subwinRow = mCurrentMenuItem - mTopMenuItem;
      drawMenuItem(mCurrentMenuItem, subwinRow, true, true);
   }
} // doPgDown

inline void cxMenu::reCreateSubWindow()
{
   int menuHeight = height();
   int menuWidth = width();

   freeSubWindow();
   // These subwindow settings are for when the menu has no borders.
   mSubWinHeight = menuHeight;
   mSubWinWidth = menuWidth;
   int topRow = 0;
   int leftCol = 0;
   // If there is a border, adjust the subwindow height & width and top row &
   //  left column.
   if (getBorderStyle() != eBS_NOBORDER)
   {
      mSubWinHeight = menuHeight-2;
      mSubWinWidth = menuWidth-2;
      topRow = 1;
      leftCol = 1;
   }
   // Re-create the subwindow.  If derwin() can't create it, it will return
   //  nullptr, and we have a problem.
   mSubWindow = derwin(mWindow, mSubWinHeight, mSubWinWidth, topRow, leftCol);
   if (mSubWindow == nullptr)
   {
      // Free up the other memory used
      cxWindow::freeWindow();
      throw(cxWidgetsException("Couldn't re-create the ncurses subwindow (cxMenu::reCreateSubWindow()).  Subwindow height & width: " + toString(mSubWinHeight) + ", " + toString(mSubWinWidth) + "; menu height & width: " + toString(menuHeight) + ", " + toString(menuWidth) + "; begin X & Y: " + toString(topRow) + ", " + toString(leftCol)));
   }
   keypad(mSubWindow, TRUE);
   scrollok(mSubWindow, true);
   idlok(mSubWindow, true);
} // reCreateSubWindow

inline void cxMenu::freeSubWindow()
{
   if (mSubWindow != nullptr)
   {
      delwin(mSubWindow);
      mSubWindow = nullptr;
   }
} // freeSubWindow

bool cxMenu::lookForSelectableItem()
{
   // If there are no items, then there are no
   //  selectable menu items.
   if (numMenuItems() == 0)
   {
      mSelectableItemExists = false;
      // Make sure mUnselectableItems is empty.
      mUnselectableItems.clear();
   }
   // If there are less unselectable items than there
   //  are items, then there must be at least one selectable
   //  item.
   else if (mUnselectableItems.size() < numMenuItems())
   {
      mSelectableItemExists = true;
   }
   else
   {
      // If there is any item index that doesn't exist in
      //  mUnselectableItems, then there is at least one
      //  selectable item.
      mSelectableItemExists = false;
      unsigned numItems = cxMenu::numMenuItems();
      for (unsigned i = 0; i < numItems; ++i)
      {
         if (mUnselectableItems.find(i) == mUnselectableItems.end())
         {
            mSelectableItemExists = true;
            break;
         }
      }
   }

   // If mSelectableItemExists is false at this point, look in all
   //  submenus to see if they have any selectable item.  If a submenu
   //  has a selectable item, the user should still be able to get to
   //  it.
   if (!mSelectableItemExists)
   {
      map<int, cxMenu*>::iterator iter = mSubMenus.begin();
      for (; iter != mSubMenus.end(); ++iter)
      {
         mSelectableItemExists = iter->second->lookForSelectableItem();
         // Break out of this loop if mSelectableItems has been set true.
         if (mSelectableItemExists)
         {
            // Remove the index of the submenu item from mUnselectableItems.
            mUnselectableItems.erase(iter->first);
            break;
         }
      }
   }

   return(mSelectableItemExists);
} // lookForSelectableItem

void cxMenu::doHome()
{
   // Make the first item the current item,
   //  and scroll up to the top.
   if (mCurrentMenuItem != 0)
   {
      // Un-highlight the current menu item
      int subwinRow = mCurrentMenuItem - mTopMenuItem;
      drawMenuItem(mCurrentMenuItem, subwinRow, true, false);
      // The new current menu item should be the first one
      setTopItem(0, true);
      mCurrentMenuItem = 0;
      // But that item might not be selectable, so go to
      //  the first selectable item from this point.
      goToSelectableItem(true, false);
   }
} // doHome

void cxMenu::doEnd()
{
   // Make the last item the current item, and
   //  scroll down to the bottom page.
   if (mCurrentMenuItem != (int)mMessageLines.size()-1)
   {
      // Un-highlight the current menu item
      int subwinRow = mCurrentMenuItem - mTopMenuItem;
      drawMenuItem(mCurrentMenuItem, subwinRow, true, false);
      // The new current menu item should be the last one
      setBottomItem((int)numMenuItems()-1, true);
      mCurrentMenuItem = (int)numMenuItems()-1;
      // But that item might not be selectable, so go to
      //  the first selectable item from this point.
      goToSelectableItem(true, false);
   }
} // doEnd

long cxMenu::getCurrentItemReturnCode(const int& pLastKey, bool& pContinueOn)
{
   long returnCode = 0L;

   // If the current menu item is a submenu type, then show
   //  its submenu and process the return code.  Otherwise,
   //  stop the input loop.
   if ((mItemTypes[mCurrentMenuItem] == cxITEM_SUBMENU) ||
       (mItemTypes[mCurrentMenuItem] == cxITEM_POPUPMENU))
       {
      returnCode = doSubmenu();

      // Return the return code of the submenu if the user didn't
      //  exit or quit.
      if ((returnCode != cxID_EXIT) && (returnCode != cxID_QUIT))
      {
         return(returnCode);
      }
   }
   else
   {
      // The current menu item is not a submenu type.
      // If the last keypress was a tab, then exit (if
      //  mAllowExit is true).  Otherwise, return the
      //  return code of the current menu item.
      if (pLastKey == TAB)
      {
         if (mAllowExit)
         {
            returnCode = cxID_EXIT;
            pContinueOn = false;
         }
      }
      else
      {
         returnCode = mReturnCodes[mCurrentMenuItem];
         pContinueOn = false;
      }
   }

   return(returnCode);
} // getCurrentItemReturnCode

int cxMenu::firstSelectableItem() const
{
   int itemIndex = -1;

   // Go forward from the first item, and the first one that we find that is
   //  not in mUnselectableItems is the first selectable item.
   unsigned numMenuItems = mMessageLines.size();
   if (numMenuItems > 0)
   {
      for (unsigned i = 0; i < numMenuItems; ++i)
      {
         if (mUnselectableItems.find(i) == mUnselectableItems.end())
         {
            itemIndex = (int)i;
            break;
         }
      }
   }

   return(itemIndex);
} // firstSelectableItem

int cxMenu::lastSelectableItem() const
{
   int itemIndex = -1;

   // Go backward from the last item, and the first one that we find that is
   //  not in mUnselectableItems is the last selectable item.
   unsigned numMenuItems = mMessageLines.size();
   if (numMenuItems > 0)
   {
      for (unsigned i = numMenuItems - 1; i >= 0; --i)
      {
         if (mUnselectableItems.find(i) == mUnselectableItems.end())
         {
            itemIndex = (int)i;
            break;
         }
      }
   }

   return(itemIndex);
} // lastSelectableItem

void cxMenu::highlightItem(int pItemIndex)
{
   if (pItemIndex > 0)
   {
      // Highlight the item in the window
      int winRow = mMouse.y - top() - 1;
      drawMenuItem(pItemIndex, winRow, true, true);
      // Use the item's help as the menu status
      useHelpAsStatus(true);
   }
} // highlightItem

void cxMenu::freeOnSelectItemFunction()
{
   mOnSelectItemFunction.reset();
} // freeOnSelectItemFunction

void cxMenu::runOnSelectItemFunction(bool& pExitAfterRun,
                                     bool& pRunOnLeaveFunction)
                                     {
   pExitAfterRun = false;
   pRunOnLeaveFunction = true;

   // If mOnSelectItemFunction is not nullptr, then run it.
   if (mOnSelectItemFunction != nullptr)
   {
      mOnSelectItemFunction->runFunction();
      pExitAfterRun = mOnSelectItemFunction->getExitAfterRun();
      pRunOnLeaveFunction = mOnSelectItemFunction->getRunOnLeaveFunction();
   }
} // runOnSelectItemFunction

void cxMenu::checkEventFunctionPointers(const cxMenu& pMenu)
{
   shared_ptr<cxFunction> onSelectItemFunc = pMenu.mOnSelectItemFunction;
   if (onSelectItemFunc != nullptr)
   {
      if (onSelectItemFunc->cxTypeStr() == "cxFunction0")
      {
         const cxFunction0* iFunc0 = dynamic_cast<cxFunction0*>(onSelectItemFunc.get());
         if (iFunc0 != nullptr)
         {
            setOnSelectItemFunction(iFunc0->getFunction(),
                                    iFunc0->getExitAfterRun(),
                                    iFunc0->getRunOnLeaveFunction());
         }
      }
      else if (onSelectItemFunc->cxTypeStr() == "cxFunction2")
      {
         const cxFunction2* iFunc2 = dynamic_cast<cxFunction2*>(onSelectItemFunc.get());
         if (iFunc2 != nullptr)
         {
            void* params[] = { iFunc2->getParam1(), iFunc2->getParam2() };
            for (int i = 0; i < 2; ++i)
            {
               if (params[i] == (void*)(&pMenu))
               {
                  params[i] = (void*)this;
               }
            }
            setOnSelectItemFunction(iFunc2->getFunction(), params[0], params[1],
                                    iFunc2->getExitAfterRun(),
                                    iFunc2->getRunOnLeaveFunction());
         }
      }
      else if (onSelectItemFunc->cxTypeStr() == "cxFunction4")
      {
         const cxFunction4* iFunc4 = dynamic_cast<cxFunction4*>(onSelectItemFunc.get());
         if (iFunc4 != nullptr)
         {
            void* params[] = { iFunc4->getParam1(), iFunc4->getParam2(),
                               iFunc4->getParam3(), iFunc4->getParam4() };
            for (int i = 0; i < 4; ++i)
            {
               if (params[i] == (void*)(&pMenu))
               {
                  params[i] = (void*)this;
               }
            }
            setOnSelectItemFunction(iFunc4->getFunction(), params[0],
                                    params[1], params[2], params[3],
                                    iFunc4->getExitAfterRun(),
                                    iFunc4->getRunOnLeaveFunction());
         }
      }
   }
} // checkEventFunctionPointers
