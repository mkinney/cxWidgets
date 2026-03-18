// Copyright (c) 2026 E. Oulashin
// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxWindow.h"
#include "cxBase.h"
#include "cxStringUtils.h"
#include "cxPanel.h"
#include <algorithm>
#include <typeinfo> // For bad_cast
#include <iterator>
#include <utility>  // For make_pair
using std::copy;
using std::insert_iterator;
using std::front_insert_iterator;
using std::pair;
using std::make_pair;
using std::shared_ptr;
using std::make_shared;
using std::dynamic_pointer_cast;
using std::unique_ptr;
using std::make_unique;
using std::string;
using std::vector;
using std::set;
using std::list;
using std::deque;
using std::map;
using cxStringUtils::SplitStringRegex;
using cxStringUtils::toString;

cxWindow::cxWindow(cxWindow *pParentWindow,
                   int pRow, int pCol, int pHeight, int pWidth,
                   const string& pTitle, const string& pMessage,
                   const string& pStatus, eBorderStyle pBorderStyle,
                   cxWindow *pExtTitleWindow, cxWindow *pExtStatusWindow,
                   bool pHotkeyHighlighting)
   : mHotkeyHighlighting(pHotkeyHighlighting),
     mExtTitleWindow(pExtTitleWindow),
     mExtStatusWindow(pExtStatusWindow),
	  mParentWindow(pParentWindow != this ? pParentWindow : nullptr),
	  mBorderStyle(pBorderStyle)
{
   // If pHeight & pWidth are < 0, then use them to control the height of
   //  the window based on the parent window's height/width (or the screen's
   //  height & width).  i.e., if pHeight is -1, then this window will be
   //  1 row shorter than the parent window/screen.
   // Also, based on the window position (pCol and pRow) and the height & width
   //  of the main screen, make sure that the window height & width are not
   //  too big.
   const int maxHeight = (int)(cxBase::height()) - pRow;
   const int maxWidth = (int)(cxBase::width()) - pCol;
   if (pHeight < 0)
   {
      if (pParentWindow != nullptr)
      {
         pHeight = pParentWindow->height() + pHeight;
      }
      else
      {
         pHeight = cxBase::height() + pHeight;
      }
   }
   // If pHeight is more than the maximum possible window height that the
   //  screen can display, use the maximum height.
   else if ((pHeight > 0) && (pHeight > maxHeight))
   {
      pHeight = (int)(cxBase::height());
   }

   if (pWidth < 0)
   {
      if (pParentWindow != nullptr)
      {
         pWidth = pParentWindow->width() + pWidth;
      }
      else
      {
         pWidth = cxBase::width() + pWidth;
      }
   }
   else if ((pWidth > 0) && (pWidth > maxWidth))
   {
      pWidth = (int)(cxBase::width());
   }

   // Ensure that if pHeight or pWidth is 0, they are passed as 0 to init()
   // to trigger auto-sizing logic. Previous version might have had logic
   // that overridden 0 with full-screen dimensions.
   init(pRow, pCol, pHeight, pWidth, pTitle, pMessage, pStatus, pParentWindow);

   // If an external title window/external status window are specified,
   //  tell them to point to this window with their mTitleParent and
   //  mStatusParent (this way, they can let this window know when
   //  they go out of scope by setting mExtTitleWindow or mExtStatusWindow
   //  to nullptr).
   if (pExtTitleWindow != nullptr)
   {
      pExtTitleWindow->mTitleParent = this;
   }
   if (pExtStatusWindow != nullptr)
   {
      pExtStatusWindow->mStatusParent = this;
   }

   // Set mMouse's coordinates to (0, 0)
#ifdef NCURSES_MOUSE_VERSION
   mMouse.z = 0;
   mMouse.y = 0;
   mMouse.x = 0;
#endif
} // ctor

cxWindow::cxWindow(cxWindow *pParentWindow,
                   int pRow, int pCol, const string& pTitle,
                   const string& pMessage, const string& pStatus,
                   cxWindow *pExtTitleWindow, cxWindow *pExtStatusWindow,
                   bool pHotkeyHighlighting)
   : mWindow(nullptr),
     mMessageColorPair(eWHITE_BLUE),
     mTitleColorPair(eGRAY_BLUE),
     mStatusColorPair(eBROWN_BLUE),
     mBorderColorPair(eGRAY_BLUE),
     mHorizTitleAlignment(eHP_LEFT),
     mHorizMessageAlignment(eHP_LEFT),
     mHorizStatusAlignment(eHP_LEFT),
     mVerticalMessageAlignment(eVP_TOP),
     mDrawMessage(true),
     mDrawSpecialChars(true),
     mOnFocusFunction(nullptr),
     mOnLeaveFunction(nullptr),
     mIsModal(false),
     mLeaveNow(false),
     mHotkeyHighlighting(pHotkeyHighlighting),
     mPanel(nullptr),
     mExtTitleWindow(pExtTitleWindow),
     mExtStatusWindow(pExtStatusWindow),
     mTitleParent(nullptr),
     mStatusParent(nullptr),
     mParentWindow(pParentWindow != this ? pParentWindow : nullptr),
     mFocus(false),
     mBorderStyle(eBS_SINGLE_LINE),
     mEnabled(true),
     mDisableCursorOnShow(true),
     mLastKey(NOKEY),
     mChangeColorsOnFocus(false),
     mShowSubwinsForward(true),
     mShowSelfBeforeSubwins(true),
     mReturnCode(cxID_EXIT),
     mRunOnFocus(true),
     mRunOnLeave(true),
     mDrawBorderTop(true),
     mDrawBorderBottom(true),
     mDrawBorderLeft(true),
     mDrawBorderRight(true)
{
   init(pRow, pCol, 0, 0, pTitle, pMessage, pStatus, pParentWindow);

   // If an external title window/external status window are specified,
   //  tell them to point to this window with their mTitleParent and
   //  mStatusParent (this way, they can let this window know when
   //  they go out of scope by setting mExtTitleWindow or mExtStatusWindow
   //  to nullptr).
   if (pExtTitleWindow != nullptr)
   {
      pExtTitleWindow->mTitleParent = this;
   }
   if (pExtStatusWindow != nullptr)
   {
      pExtStatusWindow->mStatusParent = this;
   }

   // Set mMouse's coordinates to (0, 0)
#ifdef NCURSES_MOUSE_VERSION
   mMouse.z = 0;
   mMouse.y = 0;
   mMouse.x = 0;
#endif
} // ctor

cxWindow::cxWindow(cxWindow *pParentWindow,
                   const string& pTitle, const string& pMessage,
                   const string& pStatus, cxWindow *pExtTitleWindow,
                   cxWindow *pExtStatusWindow, bool pHotkeyHighlighting)
   : mWindow(nullptr),
     mMessageColorPair(eWHITE_BLUE),
     mTitleColorPair(eGRAY_BLUE),
     mStatusColorPair(eBROWN_BLUE),
     mBorderColorPair(eGRAY_BLUE),
     mHorizTitleAlignment(eHP_LEFT),
     mHorizMessageAlignment(eHP_LEFT),
     mHorizStatusAlignment(eHP_LEFT),
     mVerticalMessageAlignment(eVP_TOP),
     mDrawMessage(true),
     mDrawSpecialChars(true),
     mOnFocusFunction(nullptr),
     mOnLeaveFunction(nullptr),
     mIsModal(false),
     mLeaveNow(false),
     mHotkeyHighlighting(pHotkeyHighlighting),
     mPanel(nullptr),
     mExtTitleWindow(pExtTitleWindow),
     mExtStatusWindow(pExtStatusWindow),
     mTitleParent(nullptr),
     mStatusParent(nullptr),
     mParentWindow(pParentWindow != this ? pParentWindow : nullptr),
     mFocus(false),
     mBorderStyle(eBS_SINGLE_LINE),
     mEnabled(true),
     mDisableCursorOnShow(true),
     mLastKey(NOKEY),
     mChangeColorsOnFocus(false),
     mShowSubwinsForward(true),
     mShowSelfBeforeSubwins(true),
     mReturnCode(cxID_EXIT),
     mRunOnFocus(true),
     mRunOnLeave(true),
     mDrawBorderTop(true),
     mDrawBorderBottom(true),
     mDrawBorderLeft(true),
     mDrawBorderRight(true)
{
   init(0, 0, 0, 0, pTitle, pMessage, pStatus, pParentWindow);
   // Center the window on the screen, but don't draw it.
   center(false);

   // If an external title window/external status window are specified,
   //  tell them to point to this window with their mTitleParent and
   //  mStatusParent (this way, they can let this window know when
   //  they go out of scope by setting mExtTitleWindow or mExtStatusWindow
   //  to nullptr).
   if (pExtTitleWindow != nullptr)
   {
      pExtTitleWindow->mTitleParent = this;
   }
   if (pExtStatusWindow != nullptr)
   {
      pExtStatusWindow->mStatusParent = this;
   }

   // Set mMouse's coordinates to (0, 0)
#ifdef NCURSES_MOUSE_VERSION
   mMouse.z = 0;
   mMouse.y = 0;
   mMouse.x = 0;
#endif
} // ctor

cxWindow::cxWindow(cxWindow *pParentWindow,
               const string& pTitle, const string& pMessage,
               const string& pStatus,
               int pHeight, int pWidth,
               cxWindow *pExtTitleWindow,
               cxWindow *pExtStatusWindow,
               bool pHotkeyHighlighting)
   : mWindow(nullptr),
     mMessageColorPair(eWHITE_BLUE),
     mTitleColorPair(eGRAY_BLUE),
     mStatusColorPair(eBROWN_BLUE),
     mBorderColorPair(eGRAY_BLUE),
     mHorizTitleAlignment(eHP_LEFT),
     mHorizMessageAlignment(eHP_LEFT),
     mHorizStatusAlignment(eHP_LEFT),
     mVerticalMessageAlignment(eVP_TOP),
     mDrawMessage(true),
     mDrawSpecialChars(true),
     mOnFocusFunction(nullptr),
     mOnLeaveFunction(nullptr),
     mIsModal(false),
     mLeaveNow(false),
     mHotkeyHighlighting(pHotkeyHighlighting),
     mPanel(nullptr),
     mExtTitleWindow(pExtTitleWindow),
     mExtStatusWindow(pExtStatusWindow),
     mTitleParent(nullptr),
     mStatusParent(nullptr),
     mParentWindow(pParentWindow != this ? pParentWindow : nullptr),
     mFocus(false),
     mBorderStyle(eBS_SINGLE_LINE),
     mEnabled(true),
     mDisableCursorOnShow(true),
     mLastKey(NOKEY),
     mChangeColorsOnFocus(false),
     mShowSubwinsForward(true),
     mShowSelfBeforeSubwins(true),
     mReturnCode(cxID_EXIT),
     mRunOnFocus(true),
     mRunOnLeave(true),
     mDrawBorderTop(true),
     mDrawBorderBottom(true),
     mDrawBorderLeft(true),
     mDrawBorderRight(true)
{
   init(0, 0, pHeight, pWidth, pTitle, pMessage, pStatus, pParentWindow);
   // Center the window on the screen, but don't draw it.
   center(false);

   // If an external title window/external status window are specified,
   //  tell them to point to this window with their mTitleParent and
   //  mStatusParent (this way, they can let this window know when
   //  they go out of scope by setting mExtTitleWindow or mExtStatusWindow
   //  to nullptr).
   if (pExtTitleWindow != nullptr)
   {
      pExtTitleWindow->mTitleParent = this;
   }
   if (pExtStatusWindow != nullptr)
   {
      pExtStatusWindow->mStatusParent = this;
   }

   // Set mMouse's coordinates to (0, 0)
#ifdef NCURSES_MOUSE_VERSION
   mMouse.z = 0;
   mMouse.y = 0;
   mMouse.x = 0;
#endif
} // ctor

cxWindow::cxWindow(cxWindow *pParentWindow,
                   const string& pMessage, const string& pStatus,
                   cxWindow *pExtTitleWindow, cxWindow *pExtStatusWindow,
                   bool pHotkeyHighlighting)
   : mWindow(nullptr),
     mMessageColorPair(eWHITE_BLUE),
     mTitleColorPair(eGRAY_BLUE),
     mStatusColorPair(eBROWN_BLUE),
     mBorderColorPair(eGRAY_BLUE),
     mHorizTitleAlignment(eHP_LEFT),
     mHorizMessageAlignment(eHP_LEFT),
     mHorizStatusAlignment(eHP_LEFT),
     mVerticalMessageAlignment(eVP_TOP),
     mDrawMessage(true),
     mDrawSpecialChars(true),
     mOnFocusFunction(nullptr),
     mOnLeaveFunction(nullptr),
     mIsModal(false),
     mLeaveNow(false),
     mHotkeyHighlighting(pHotkeyHighlighting),
     mPanel(nullptr),
     mExtTitleWindow(pExtTitleWindow),
     mExtStatusWindow(pExtStatusWindow),
     mTitleParent(nullptr),
     mStatusParent(nullptr),
     mParentWindow(pParentWindow != this ? pParentWindow : nullptr),
     mFocus(false),
     mBorderStyle(eBS_SINGLE_LINE),
     mEnabled(true),
     mDisableCursorOnShow(true),
     mLastKey(NOKEY),
     mChangeColorsOnFocus(false),
     mShowSubwinsForward(true),
     mShowSelfBeforeSubwins(true),
     mReturnCode(cxID_EXIT),
     mRunOnFocus(true),
     mRunOnLeave(true),
     mDrawBorderTop(true),
     mDrawBorderBottom(true),
     mDrawBorderLeft(true),
     mDrawBorderRight(true)
{
   init(0, 0, 0, 0, "", pMessage, pStatus, pParentWindow);
   // Center the window on the screen, but don't draw it.
   center(false);

   // If an external title window/external status window are specified,
   //  tell them to point to this window with their mTitleParent and
   //  mStatusParent (this way, they can let this window know when
   //  they go out of scope by setting mExtTitleWindow or mExtStatusWindow
   //  to nullptr).
   if (pExtTitleWindow != nullptr)
   {
      pExtTitleWindow->mTitleParent = this;
   }
   if (pExtStatusWindow != nullptr)
   {
      pExtStatusWindow->mStatusParent = this;
   }

   // Set mMouse's coordinates to (0, 0)
#ifdef NCURSES_MOUSE_VERSION
   mMouse.z = 0;
   mMouse.y = 0;
   mMouse.x = 0;
#endif
} // ctor

cxWindow::cxWindow(cxWindow *pParentWindow,
               const string& pMessage, const string& pStatus,
               int pHeight, int pWidth,
               cxWindow *pExtTitleWindow,
               cxWindow *pExtStatusWindow,
               bool pHotkeyHighlighting)
   : mWindow(nullptr),
     mMessageColorPair(eWHITE_BLUE),
     mTitleColorPair(eGRAY_BLUE),
     mStatusColorPair(eBROWN_BLUE),
     mBorderColorPair(eGRAY_BLUE),
     mHorizTitleAlignment(eHP_LEFT),
     mHorizMessageAlignment(eHP_LEFT),
     mHorizStatusAlignment(eHP_LEFT),
     mVerticalMessageAlignment(eVP_TOP),
     mDrawMessage(true),
     mDrawSpecialChars(true),
     mOnFocusFunction(nullptr),
     mOnLeaveFunction(nullptr),
     mIsModal(false),
     mLeaveNow(false),
     mHotkeyHighlighting(pHotkeyHighlighting),
     mPanel(nullptr),
     mExtTitleWindow(pExtTitleWindow),
     mExtStatusWindow(pExtStatusWindow),
     mTitleParent(nullptr),
     mStatusParent(nullptr),
     mParentWindow(pParentWindow != this ? pParentWindow : nullptr),
     mFocus(false),
     mBorderStyle(eBS_SINGLE_LINE),
     mEnabled(true),
     mDisableCursorOnShow(true),
     mLastKey(NOKEY),
     mChangeColorsOnFocus(false),
     mShowSubwinsForward(true),
     mShowSelfBeforeSubwins(true),
     mReturnCode(cxID_EXIT),
     mRunOnFocus(true),
     mRunOnLeave(true),
     mDrawBorderTop(true),
     mDrawBorderBottom(true),
     mDrawBorderLeft(true),
     mDrawBorderRight(true)
{
   init(0, 0, pHeight, pWidth, "", pMessage, pStatus, pParentWindow);
   // Center the window on the screen, but don't draw it.
   center(false);

   // If an external title window/external status window are specified,
   //  tell them to point to this window with their mTitleParent and
   //  mStatusParent (this way, they can let this window know when
   //  they go out of scope by setting mExtTitleWindow or mExtStatusWindow
   //  to nullptr).
   if (pExtTitleWindow != nullptr)
   {
      pExtTitleWindow->mTitleParent = this;
   }
   if (pExtStatusWindow != nullptr)
   {
      pExtStatusWindow->mStatusParent = this;
   }

   // Set mMouse's coordinates to (0, 0)
#ifdef NCURSES_MOUSE_VERSION
   mMouse.z = 0;
   mMouse.y = 0;
   mMouse.x = 0;
#endif
} // ctor

cxWindow::cxWindow(cxWindow *pParentWindow, const string& pMessage,
                   cxWindow *pExtTitleWindow, cxWindow *pExtStatusWindow,
                   bool pHotkeyHighlighting)
   : mWindow(nullptr),
     mMessageColorPair(eWHITE_BLUE),
     mTitleColorPair(eGRAY_BLUE),
     mStatusColorPair(eBROWN_BLUE),
     mBorderColorPair(eGRAY_BLUE),
     mHorizTitleAlignment(eHP_LEFT),
     mHorizMessageAlignment(eHP_LEFT),
     mHorizStatusAlignment(eHP_LEFT),
     mVerticalMessageAlignment(eVP_TOP),
     mDrawMessage(true),
     mDrawSpecialChars(true),
     mOnFocusFunction(nullptr),
     mOnLeaveFunction(nullptr),
     mIsModal(false),
     mLeaveNow(false),
     mHotkeyHighlighting(pHotkeyHighlighting),
     mPanel(nullptr),
     mExtTitleWindow(pExtTitleWindow),
     mExtStatusWindow(pExtStatusWindow),
     mTitleParent(nullptr),
     mStatusParent(nullptr),
     mParentWindow(nullptr),
     mFocus(false),
     mBorderStyle(eBS_SINGLE_LINE),
     mEnabled(true),
     mDisableCursorOnShow(true),
     mLastKey(NOKEY),
     mChangeColorsOnFocus(false),
     mShowSubwinsForward(true),
     mShowSelfBeforeSubwins(true),
     mReturnCode(cxID_EXIT),
     mRunOnFocus(true),
     mRunOnLeave(true),
     mDrawBorderTop(true),
     mDrawBorderBottom(true),
     mDrawBorderLeft(true),
     mDrawBorderRight(true)
{
   init(0, 0, 0, 0, "", pMessage, "", pParentWindow);
   // Center the window on the screen, but don't draw it.
   center(false);

   // If an external title window/external status window are specified,
   //  tell them to point to this window with their mTitleParent and
   //  mStatusParent (this way, they can let this window know when
   //  they go out of scope by setting mExtTitleWindow or mExtStatusWindow
   //  to nullptr).
   if (pExtTitleWindow != nullptr)
   {
      pExtTitleWindow->mTitleParent = this;
   }
   if (pExtStatusWindow != nullptr)
   {
      pExtStatusWindow->mStatusParent = this;
   }

   // Set mMouse's coordinates to (0, 0)
#ifdef NCURSES_MOUSE_VERSION
   mMouse.z = 0;
   mMouse.y = 0;
   mMouse.x = 0;
#endif
} // ctor

cxWindow::cxWindow(cxWindow *pParentWindow, eHPosition pHPosition,
                   const string& pTitle, const string& pMessage,
                   const string& pStatus, cxWindow *pExtTitleWindow,
                   cxWindow *pExtStatusWindow, bool pHotkeyHighlighting)
   : mWindow(nullptr),
     mMessageColorPair(eWHITE_BLUE),
     mTitleColorPair(eGRAY_BLUE),
     mStatusColorPair(eBROWN_BLUE),
     mBorderColorPair(eGRAY_BLUE),
     mHorizTitleAlignment(eHP_LEFT),
     mHorizMessageAlignment(eHP_LEFT),
     mHorizStatusAlignment(eHP_LEFT),
     mVerticalMessageAlignment(eVP_TOP),
     mDrawMessage(true),
     mDrawSpecialChars(true),
     mOnFocusFunction(nullptr),
     mOnLeaveFunction(nullptr),
     mIsModal(false),
     mLeaveNow(false),
     mHotkeyHighlighting(pHotkeyHighlighting),
     mPanel(nullptr),
     mExtTitleWindow(pExtTitleWindow),
     mExtStatusWindow(pExtStatusWindow),
     mTitleParent(nullptr),
     mStatusParent(nullptr),
     mParentWindow(pParentWindow != this ? pParentWindow : nullptr),
     mFocus(false),
     mBorderStyle(eBS_SINGLE_LINE),
     mEnabled(true),
     mDisableCursorOnShow(true),
     mLastKey(NOKEY),
     mChangeColorsOnFocus(false),
     mShowSubwinsForward(true),
     mShowSelfBeforeSubwins(true),
     mReturnCode(cxID_EXIT),
     mRunOnFocus(true),
     mRunOnLeave(true),
     mDrawBorderTop(true),
     mDrawBorderBottom(true),
     mDrawBorderLeft(true),
     mDrawBorderRight(true)
{
   int newRow, newCol;

   getRowColBasedOn(pParentWindow, pHPosition, pTitle, pStatus, pMessage, newRow, newCol);

   init(newRow, newCol, 0, 0, pTitle, pMessage, pStatus, pParentWindow);

   // If an external title window/external status window are specified,
   //  tell them to point to this window with their mTitleParent and
   //  mStatusParent (this way, they can let this window know when
   //  they go out of scope by setting mExtTitleWindow or mExtStatusWindow
   //  to nullptr).
   if (pExtTitleWindow != nullptr)
   {
      pExtTitleWindow->mTitleParent = this;
   }
   if (pExtStatusWindow != nullptr)
   {
      pExtStatusWindow->mStatusParent = this;
   }

   // Set mMouse's coordinates to (0, 0)
#ifdef NCURSES_MOUSE_VERSION
   mMouse.z = 0;
   mMouse.y = 0;
   mMouse.x = 0;
#endif
} // ctor

// Copy constructor
cxWindow::cxWindow(const cxWindow& pThatWindow)
   : mWindow(nullptr),
     mMessageLines(pThatWindow.mMessageLines),
     mSpecialChars(pThatWindow.mSpecialChars),
     mMessageAttrs(pThatWindow.mMessageAttrs),
     mTitleAttrs(pThatWindow.mTitleAttrs),
     mStatusAttrs(pThatWindow.mStatusAttrs),
     mBorderAttrs(pThatWindow.mBorderAttrs),
     mHotkeyAttrs(pThatWindow.mHotkeyAttrs),
     mSpecialCharAttrs(pThatWindow.mSpecialCharAttrs),
     mMessageColorPair(pThatWindow.mMessageColorPair),
     mTitleColorPair(pThatWindow.mTitleColorPair),
     mStatusColorPair(pThatWindow.mStatusColorPair),
     mBorderColorPair(pThatWindow.mBorderColorPair),
     mHorizTitleAlignment(pThatWindow.mHorizTitleAlignment),
     mHorizMessageAlignment(pThatWindow.mHorizMessageAlignment),
     mHorizStatusAlignment(pThatWindow.mHorizStatusAlignment),
     mDrawMessage(pThatWindow.mDrawMessage),
     mDrawSpecialChars(pThatWindow.mDrawSpecialChars),
     // Note: The onFocus and onLeave functions are copied straight across
     // here, but before we were using shared_ptr objects for them, we
     // called setFocusFunctions() to copy them from the other window.
     mOnFocusFunction(pThatWindow.mOnFocusFunction),
     mOnLeaveFunction(pThatWindow.mOnLeaveFunction),
     //mOnFocusFunction(nullptr),
     //mOnLeaveFunction(nullptr),
     mIsModal(false),
     mLeaveNow(false),
     // Note: mKeyFunctions and mMouseFunctions are copied straight across
     // here, but before we were using shared_ptr objects for them, we called
     // setKeyFunctions() and setMouseFunctions() to copy them from the other
     // window.
     mKeyFunctions(pThatWindow.mKeyFunctions),
     mMouseFunctions(pThatWindow.mMouseFunctions),
     mQuitKeys(pThatWindow.mQuitKeys),
     mExitKeys(pThatWindow.mExitKeys),
     mHotkeyHighlighting(pThatWindow.mHotkeyHighlighting),
     mMouse(pThatWindow.mMouse),
     mPanel(nullptr),
     mExtTitleWindow(pThatWindow.mExtTitleWindow),
     mExtStatusWindow(pThatWindow.mExtStatusWindow),
     mTitleParent(nullptr),
     mStatusParent(nullptr),
     mParentWindow(pThatWindow.mParentWindow != this ? pThatWindow.mParentWindow : nullptr),
     mFocus(false),
     mBorderStyle(pThatWindow.mBorderStyle),
     mEnabled(pThatWindow.mEnabled),
     mDisableCursorOnShow(pThatWindow.mDisableCursorOnShow),
     mLastKey(pThatWindow.mLastKey),
     mChangeColorsOnFocus(pThatWindow.mChangeColorsOnFocus),
     mShowSubwinsForward(pThatWindow.mShowSubwinsForward),
     mShowSelfBeforeSubwins(pThatWindow.mShowSelfBeforeSubwins),
     mReturnCode(pThatWindow.mReturnCode),
     mRunOnFocus(pThatWindow.mRunOnFocus),
     mRunOnLeave(pThatWindow.mRunOnLeave),
     mTitleStrings(pThatWindow.mTitleStrings),
     mStatusStrings(pThatWindow.mStatusStrings),
     mName(pThatWindow.mName),
     mDrawBorderTop(pThatWindow.mDrawBorderTop),
     mDrawBorderBottom(pThatWindow.mDrawBorderBottom),
     mDrawBorderLeft(pThatWindow.mDrawBorderLeft),
     mDrawBorderRight(pThatWindow.mDrawBorderRight)
{
   // Create mWindow and mPanel
   mWindow = newwin(pThatWindow.height(), pThatWindow.width(), pThatWindow.top(), pThatWindow.left());
   // If mWindow is nullptr, that means newwin() had an error..
   if (mWindow == nullptr)
   {
      throw(cxWidgetsException("Couldn't create a new ncurses window (copy-constructing another cxWindow)."));
   }
   reCreatePanel();
   keypad(mWindow, TRUE); // For each newwin() we have to set this..

   // Add this window to the other window's parent window
   if (mParentWindow != nullptr)
   {
      mParentWindow->addSubwindow(this);
   }
   // Note: If the other window's parent window is a cxPanel, we shouldn't
   // add this window to its other collection of windows (with addWindow()),
   // since it assumes the other windows are created dynamically and will
   // try to free the memory in its destructor.  Since we don't know if this
   // object was dynamically created, we shouldn't add it to a parent
   // cxPanel's window list.

   // Note: mKeyFunctions and mMouseFunctions are copied straight across
   // in the initializer list, but before we were using shared_ptrs for them,we called
   // we called setKeyFunctions() and setMouseFunctions() to copy them from the other
   // window.
   //setKeyFunctions(pThatWindow);
   //setFocusFunctions(pThatWindow);

   // If the other window is hidden, hide this one too.
   if (pThatWindow.isHidden())
   {
      hide();
   }

   // Copy the other window's mMouse information
#ifdef NCURSES_MOUSE_VERSION
   mMouse.y = pThatWindow.mMouse.y;
   mMouse.x = pThatWindow.mMouse.x;
   mMouse.z = pThatWindow.mMouse.z;
   mMouse.bstate = pThatWindow.mMouse.bstate;
   mMouse.id = pThatWindow.mMouse.id;
#endif
} // Copy constructor

cxWindow::~cxWindow()
{
   // Free the memory used by the key functions and mouse event functions
   clearKeyFunctions();
   clearMouseFunctions();

   // If other cxWindow objects are using this one
   //  as a title or status window, let them know
   //  that this window can no longer be used.
   if (mTitleParent != nullptr)
   {
      mTitleParent->mExtTitleWindow = nullptr;
   }
   if (mStatusParent != nullptr)
   {
      mStatusParent->mExtStatusWindow = nullptr;
   }

   // If other windows are being used for the
   //  title or status, then copy their text
   //  back into them.
   if (mExtTitleWindow != nullptr)
   {
      mExtTitleWindow->setMessage(mExtTitleTemp);
      if (!(mExtTitleWindow->isHidden()))
      {
         mExtTitleWindow->show();
      }
   }
   if (mExtStatusWindow != nullptr)
   {
      mExtStatusWindow->setMessage(mExtStatusTemp);
      if (!(mExtStatusWindow->isHidden()))
      {
         mExtStatusWindow->show();
      }
   }

   // Remove this window from the parent window's subwindow list.
   if (mParentWindow != nullptr)
   {
      // If the parent window is a cxPanel or something deriving from cxPanel,
      // remove this window from the cxPanel's mWindows.
      try
      {
         string parentType = mParentWindow->cxTypeStr();
         if ((parentType == "cxPanel") || (parentType == "cxSearchPanel"))
         {
            cxPanel *parentPanel = dynamic_cast<cxPanel*>(mParentWindow);
            cxPanel::cxWindowPtrCollection::iterator iter = parentPanel->mWindows.begin();
            for (; iter != parentPanel->mWindows.end(); ++iter)
            {
               if (iter->get() == this)
               {
                  // A pointer to this window was found in the
                  // panel's mWindows..  Remove it from its
                  // mWindows (note: there should be only 1 pointer
                  // to this window in a parent window).
                  parentPanel->mWindows.erase(iter);
                  break;
               }
            }
         }
      }
      catch (const std::bad_cast& e)
      {
            // A dynamic_cast failed
      }
      catch (const std::bad_typeid& e)
      {
         // A typeid failed
      }
      mParentWindow->removeSubWindow(this);
   }

   // Set all the subwindows' parent windows to nullptr (so that they don't try
   //  to do something with this window anymore).  Note: This is using a
   //  while loop surrounding the for loop, because each time we remove a
   //  window from mSubWindows, the iterator gets invalidated, so we need
   //  to make sure it's set correctly.
   while (anySubwinHasThisParent())
   {
      for (cxWindow*& subWin : mSubWindows)
      {
         if (subWin->mParentWindow == this)
         {
            //subWin->setParent(nullptr);
            subWin->mParentWindow = nullptr;
            break; // Exit the for loop; continue with the while loop
         }
      }
   }

   // Hide the window (to make sure it doesn't show anymore), and then free
   // the memory used by mWindow and mPanel.
   if (cxBase::cxInitialized())
   {
      hide();
   }
   // Update the physical screen
   //update_panels();

   freeWindow();
} // dtor

void cxWindow::centerHoriz(bool pRefresh)
{
   alignHoriz(eHP_CENTER, pRefresh);
} // centerHoriz

void cxWindow::alignHoriz(eHPosition pPosition, bool pRefresh)
{
   // Update the leftmost column coordinate and move the window.
   //  Align to the left, center or right in the parent window
   //   if there is one, or on the screen if there is no parent window.
   int leftCol = 0;
   if (mParentWindow != nullptr)
   {
      if (pPosition == eHP_CENTER)
      {
         leftCol = mParentWindow->centerCol() - (width() / 2);
      }
      else if (pPosition == eHP_LEFT)
      {
         leftCol = mParentWindow->left();
      }
      else if (pPosition == eHP_RIGHT)
      {
         leftCol = mParentWindow->right() - width();
      }
   }
   else
   {
      if (pPosition == eHP_CENTER)
      {
         leftCol = cxBase::centerCol() - (width() / 2);
      }
      else if (pPosition == eHP_LEFT)
      {
         leftCol = cxBase::left();
      }
      else if (pPosition == eHP_RIGHT)
      {
         leftCol = cxBase::right() - width() + 1;
      }
   }

   move(top(), leftCol, pRefresh);
} // alignHoriz

void cxWindow::centerVert(bool pRefresh)
{
   alignVert(eVP_CENTER, pRefresh);
} // centerVert

void cxWindow::alignVert(eVPosition pPosition, bool pRefresh)
{
   // Update the topmost row coordinate and move the window.
   //  Align in the parent window if there is one, or on the
   //  screen if there is no parent window.
   int topRow = 0;
   if (mParentWindow != nullptr)
   {
      if (pPosition == eVP_CENTER)
      {
         topRow = mParentWindow->centerRow() - (height() / 2);
      }
      else if (pPosition == eVP_TOP)
      {
         topRow = mParentWindow->top();
      }
      else if (pPosition == eVP_BOTTOM)
      {
         topRow = mParentWindow->bottom() - height();
      }
   }
   else
   {
      if (pPosition == eVP_CENTER)
      {
         topRow = cxBase::centerRow() - (height() / 2);
      }
      else if (pPosition == eVP_TOP)
      {
         topRow = cxBase::top();
      }
      else if (pPosition == eVP_BOTTOM)
      {
         topRow = cxBase::bottom() - height() + 1;
      }
   }

   move(topRow, left(), pRefresh);
} // centerVert

void cxWindow::center(bool pRefresh)
{
   alignHoriz(eHP_CENTER, false); // No need to refresh on the first call.
   alignVert(eVP_CENTER, pRefresh); // Only refresh if they wanna
} // center

string cxWindow::getTitle() const
{
   string title;

   if (mExtTitleWindow != nullptr)
   {
      title = mExtTitleWindow->getMessage();
   }
   else
   {
      // The main title is the one at horizontal position 1.
      title = getTitleString(1);
   }

   return(title);
} // getTitle

// Mutator for the window title
void cxWindow::setTitle(const string& pTitle, bool pRefreshTitle)
{
   // If an external cxWindow object is to hold the title,
   //  then set the title ther.e.  Otherwise, set the title
   //  in this window.
   if (mExtTitleWindow != nullptr)
   {
      // Set the message in the external window.
      // Note: calling the external window's setTitle
      //  could result in infinite recursion if there are
      //  circular links between cxWindow object as
      //  external status windows.
      // Ensure that the title will fit in the external window
      //  by taking a substring.  If the external window doesn't
      //  have a border, the length should be its with.  If the
      //  external window does have a border, the length should
      //  be its width - 2.
      if (mExtTitleWindow->mBorderStyle == eBS_NOBORDER)
      {
         mExtTitleWindow->setMessage(pTitle.substr(0, mExtTitleWindow->width()));
      }
      else
      {
         mExtTitleWindow->setMessage(pTitle.substr(0, mExtTitleWindow->width()-2));
      }

      if (pRefreshTitle)
      {
         if (!(mExtTitleWindow->isHidden()))
         {
            mExtTitleWindow->show();
         }
      }
   }
   else
   {
      // Update the title
      mTitleStrings[1] = pTitle.substr(0, width()-2);
      // If the window has a border, then refresh the
      //  title line if pRefreshTitle is true.
      if (hasBorder())
      {
         if (pRefreshTitle)
         {
            drawTitle();
         }
      }
   }
} // setTitle

// Accessor for the status
string cxWindow::getStatus() const
{
   string status;

   if (mExtStatusWindow != nullptr)
   {
      status = mExtStatusWindow->getMessage();
   }
   else
   {
      // The main status is the one at horizontal position 1.
      status = getStatusString(1);
   }

   return(status);
} // getStatus

// Mutator for the status text
void cxWindow::setStatus(const string& pStatus, bool pRefreshStatus)
{
   // If an external cxWindow object is to hold the status, then set the
   //  status there..  Otherwise, set the status in this window.
   if (mExtStatusWindow != nullptr)
   {
      // Set the message in the external window.
      // Note: calling the external window's setStatus
      //  could result in infinite recursion if there are
      //  circular links between cxWindow object as
      //  external status windows.
      // Ensure that the title will fit in the external window
      //  by taking a substring.  If the external window doesn't
      //  have a border, the length should be its with.  If the
      //  external window does have a border, the length should
      //  be its width - 2.
      if (mExtStatusWindow->mBorderStyle == eBS_NOBORDER)
      {
         mExtStatusWindow->setMessage(pStatus.substr(0, mExtStatusWindow->width()));
      }
      else
      {
         mExtStatusWindow->setMessage(pStatus.substr(0, mExtStatusWindow->width()-2));
      }

      if (pRefreshStatus)
      {
         if (!(mExtStatusWindow->isHidden()))
         {
            mExtStatusWindow->show();
         }
      }
   }
   else
   {
      // Update the status
      //mStatusStrings[1] = pStatus.substr(0, width()-2);
      mStatusStrings[1] = pStatus;
      // If the window has a border, then refresh the
      //  status line if pRefreshTitle is true.
      if (hasBorder())
      {
         if (pRefreshStatus)
         {
            drawStatus();
         }
      }
   }
} // setStatus

// Accessor for the message (concatenates all
//  message lines together)
string cxWindow::getMessage() const
{
   string returnVal;

   for (const string& msgLine : mMessageLines)
   {
      returnVal += msgLine;
   }

   return(returnVal);
} // getMessage

void cxWindow::setMessage(const string &pMessage)
{
   // If this is a single-line, borderless window, then
   // just truncate the message so that it will fit in
   // one line.  Otherwise, call init() again to re-initialize
   // the window based on the new message.
   if ((mBorderStyle == eBS_NOBORDER) && (height() == 1))
   {
      mMessageLines.clear();
      if ((int)pMessage.length() > width())
      {
         mMessageLines.push_back(pMessage.substr(0, width()));
      }
      else
      {
         mMessageLines.push_back(pMessage);
      }
   }
   else
   {
      init(top(), left(), height(), width(), getTitle(), pMessage, getStatus(), getParent());
   }
} // setMessage

void cxWindow::addMessageLineBelow(const string &pLine)
{
   mMessageLines.push_back(pLine);
} // addMessageLineBelow

void cxWindow::addMessageLinesBelow(const vector<string>& pLines)
{
   copy(pLines.begin(), pLines.end(),
        insert_iterator<messageLineContainer>(mMessageLines, mMessageLines.begin()));
} // addMessageLinesBelow

void cxWindow::addMessageLinesBelow(const list<string>& pLines)
{
   copy(pLines.begin(), pLines.end(),
        insert_iterator<messageLineContainer>(mMessageLines, mMessageLines.begin()));
} // addMessageLinesBelow

void cxWindow::addMessageLinesBelow(const deque<string>& pLines)
{
   copy(pLines.begin(), pLines.end(),
        insert_iterator<messageLineContainer>(mMessageLines, mMessageLines.begin()));
} // addMessageLinesBelow

void cxWindow::addMessageLineAbove(const string &pLine)
{
   mMessageLines.push_front(pLine);
} // addMessageLineAbove

void cxWindow::addMessageLinesAbove(const vector<string>& pLines)
{
   // Note: This uses copy(), and to preserve order, this uses reverse iterators.
   copy(pLines.rbegin(), pLines.rend(),
        front_insert_iterator<messageLineContainer>(mMessageLines));
} // addMessageLinesAbove

void cxWindow::addMessageLinesAbove(const list<string>& pLines)
{
   // Note: This uses copy(), and to preserve order, this uses reverse iterators.
   copy(pLines.rbegin(), pLines.rend(),
        front_insert_iterator<messageLineContainer>(mMessageLines));
} // addMessageLinesAbove

void cxWindow::addMessageLinesAbove(const deque<string>& pLines)
{
   // Note: This uses copy(), and to preserve order, this uses reverse iterators.
   copy(pLines.rbegin(), pLines.rend(),
        front_insert_iterator<messageLineContainer>(mMessageLines));
} // addMessageLinesAbove

void cxWindow::setMessageLines(const vector<string>& pLines)
{
   mMessageLines.clear();
   copy(pLines.begin(), pLines.end(),
        insert_iterator<messageLineContainer>(mMessageLines, mMessageLines.begin()));
} // setMessageLines

void cxWindow::setMessageLines(const list<string>& pLines)
{
   mMessageLines.clear();
   copy(pLines.begin(), pLines.end(),
        insert_iterator<messageLineContainer>(mMessageLines, mMessageLines.begin()));
} // setMessageLines

void cxWindow::setMessageLines(const deque<string>& pLines)
{
   mMessageLines = pLines;
} // setMessageLines

void cxWindow::removeMessageLine(unsigned pIndex)
{
   if ((pIndex >= 0) && (pIndex < mMessageLines.size()))
   {
      mMessageLines.erase(mMessageLines.begin() + pIndex);
   }
} // removeMessageLine

void cxWindow::setMessageLine(unsigned pIndex, const string& pLine)
{
   if ((pIndex >= 0) && (pIndex < mMessageLines.size()))
   {
      mMessageLines[pIndex] = pLine;
   }
} // setMessageLine

void cxWindow::setHorizTitleAlignment(eHPosition pHAlignment)
{
   mHorizTitleAlignment = pHAlignment;
   // If this window uses an external window for
   //  the title, then the title alignment is the
   //  message alignment in the other window.
   if (mExtTitleWindow != nullptr)
   {
      mExtTitleWindow->setHorizMessageAlignment(pHAlignment);
   }
} // setHorizTitleAlignment

eHPosition cxWindow::getHorizTitleAlignment() const
{
   // If this window uses an external window for
   //  the title, then the title alignment is the
   //  message alignment in the other window.
   if (mExtTitleWindow != nullptr)
   {
      return(mExtTitleWindow->mHorizMessageAlignment);
   }
   else
   {
      return(mHorizTitleAlignment);
   }
} // getHorizMessageAlignment

void cxWindow::setHorizMessageAlignment(eHPosition pHAlignment)
{
   mHorizMessageAlignment = pHAlignment;
} // setHorizStatusAlignment


eHPosition cxWindow::getHorizMessageAlignment() const
{
   return(mHorizMessageAlignment);
} // getHorizMessageAlignment

void cxWindow::setHorizStatusAlignment(eHPosition pHAlignment)
{
   mHorizStatusAlignment = pHAlignment;
   // If this window uses an external window for
   //  the title, then the status alignment is the
   //  message alignment in the other window.
   if (mExtStatusWindow != nullptr)
   {
      mExtStatusWindow->setHorizMessageAlignment(pHAlignment);
   }
} // setHorizStatusAlignment

eHPosition cxWindow::getHorizStatusAlignment() const
{
   // If this window uses an external window for
   //  the title, then the status alignment is the
   //  message alignment in the other window.
   if (mExtStatusWindow != nullptr)
   {
      return(mExtStatusWindow->mHorizMessageAlignment);
   }
   else
   {
      return(mHorizStatusAlignment);
   }
} // getHorizStatusAlignment

void cxWindow::setVerticalMessageAlignment(eVPosition pVAlignment)
{
   mVerticalMessageAlignment = pVAlignment;
} // setVerticalMessageAlignment

eVPosition cxWindow::getVerticalMessageAlignment() const
{
   return(mVerticalMessageAlignment);
} // getVerticalMessageAlignment

void cxWindow::setExtTitleWindow(cxWindow *pWindow, bool pRefreshTitle)
{
   // If pWindow is not nullptr, then move the title into the window
   //  (if there is any status text).
   if (pWindow != nullptr)
   {
      mExtTitleWindow = pWindow;

      // Store the external window's text so that it can be set
      //  back when this window doesn't need it anymore.
      mExtTitleTemp = pWindow->getMessage();

      string title = mTitleStrings[1];
      if (mExtTitleWindow->getBorderStyle() == eBS_NOBORDER)
      {
         mExtTitleWindow->setMessage(title.substr(0, mExtTitleWindow->width()));
         if (!(mExtTitleWindow->isHidden()))
         {
            mExtTitleWindow->show();
         }
      }
      else
      {
         mExtTitleWindow->setTitle(title.substr(0, mExtTitleWindow->width()-2));
         if (!(mExtTitleWindow->isHidden()))
         {
            mExtTitleWindow->drawTitle();
         }
      }

      if (pRefreshTitle)
      {
         drawTitle();
      }

      // Tell the external status window to point to this window with
      //  its mTitleParent (this way, they can let this window know when
      //  they go out of scope by setting mExtTitleWindow or mExtTitleWindow
      //  to nullptr).
      pWindow->mTitleParent = this;
   }
   else
   {
      // The external title window is being set to nullptr..  If
      //  there is currently an external title window set, then
      //  move its text back into the window and reset the title
      //  alignment in this window.
      if (mExtTitleWindow != nullptr)
      {
         mExtTitleWindow->setMessage(mExtTitleTemp);
         if (!(mExtTitleWindow->isHidden()))
         {
            mExtTitleWindow->show();
         }
         mExtTitleTemp = "";
         mExtTitleWindow->mTitleParent = nullptr;

         mHorizTitleAlignment = mExtTitleWindow->getHorizMessageAlignment();
      }

      mExtTitleWindow = pWindow;
   }
} // setExtTitleWindow

cxWindow* cxWindow::getExtTitleWindow() const
{
   return(mExtTitleWindow);
} // getExtTitleWindow

void cxWindow::setExtStatusWindow(cxWindow *pWindow, bool pRefreshStatus)
{
   // If pWindow is not nullptr, then move the status into the window
   //  (if there is any status text).
   if (pWindow != nullptr)
   {
      mExtStatusWindow = pWindow;

      // Store the external window's text so that it can be set
      //  back when this window doesn't need it anymore.
      mExtStatusTemp = pWindow->getMessage();

      // The main status string will be set in the external window.
      string mainStatusStr = mStatusStrings[1];

      if (mExtStatusWindow->getBorderStyle() == eBS_NOBORDER)
      {
         mExtStatusWindow->setMessage(mainStatusStr.substr(0, mExtStatusWindow->width()));
         if (!(mExtStatusWindow->isHidden()))
         {
            mExtStatusWindow->show();
         }
      }
      else
      {
         mExtStatusWindow->setStatus(mainStatusStr.substr(0, mExtStatusWindow->width()-2));
         if (!(mExtStatusWindow->isHidden()))
         {
            mExtStatusWindow->drawStatus();
         }
      }

      mStatusStrings[1] = "";

      if (pRefreshStatus)
      {
         drawStatus();
      }

      // Tell the external status window to point to this window with
      //  its mStatusParent (this way, they can let this window know when
      //  they go out of scope by setting mExtTitleWindow or mExtStatusWindow
      //  to nullptr).
      pWindow->mStatusParent = this;
   }
   else
   {
      // The external status window is being set to nullptr..  If
      //  there is currently an external status window set, then
      //  move its text back into the window.
      if (mExtStatusWindow != nullptr)
      {
         mExtStatusWindow->setMessage(mExtStatusTemp);
         if (!(mExtStatusWindow->isHidden()))
         {
            mExtStatusWindow->show();
         }
         mExtStatusTemp = "";
         mExtStatusWindow->mStatusParent = nullptr;
      }

      mExtStatusWindow = pWindow;
   }
} // setExtStatusWindow

cxWindow* cxWindow::getExtStatusWindow() const
{
   return(mExtStatusWindow);
} // getExtStatusWindow

bool cxWindow::isHidden() const
{
   if (mPanel == nullptr) return true;
   return(panel_hidden(mPanel) == TRUE);
} // isHidden

long cxWindow::show(bool pBringToTop, bool pShowSubwindows)
{
   // Only do this if mWindow and mPanel are both non-nullptr
   // and the window is enabled.
   if ((mWindow != nullptr) && (mPanel != nullptr))
   {
      if (mEnabled)
      {
         // Show the subwindows now, if pShowSubwindows is true and
         // mShowSelfBeforeSubwins is false.
         if (pShowSubwindows && !mShowSelfBeforeSubwins)
         {
            showSubwindows(pBringToTop, pShowSubwindows);
         }

         // Fill mWindow with the window text
         draw();

         // Call unhide() to make sure this window is not hidden.  unhide()
         // calls show_panel(mPanel) and update_pannels() to refresh the
         // window & update the panel).  Note that this is calling cxWindow's
         // unhide specifically; In testing, we found that because other
         // cxWindow-based classes override unhide() if they have other
         // windows to unhide, that could cause undesired screen repaints
         // when other derived class methods call show(), etc.
         cxWindow::unhide();

         // Bring this window to the top of the stack if
         //  pBringToTop is true and the window already
         //  isn't on top.
         if (pBringToTop)
         {
            if (!isOnTop())
            {
               top_panel(mPanel);
            }
         }

         // Show the subwindows now, if pShowSubwindows and
         //  mShowSelfBeforeSubwins are both true.
         if (pShowSubwindows && mShowSelfBeforeSubwins)
         {
            showSubwindows(pBringToTop, pShowSubwindows);
         }
      }
      else
      {
         hide(false);
      }
   }

   // Update the physical screen
   cxBase::updateWindows();
   return(cxID_EXIT);
} // show

long cxWindow::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   mReturnCode = cxID_EXIT;

   // for dragging windows around
   int pressedX=0;
   int pressedY=0;
   bool movingWindow=false;

   // Show the window and wait for a keypress (but only if the window
   //  is enabled).
   if (mEnabled)
   {
      mIsModal = true;

      // Run the onFocus function.  If runOnFocusFunction() returns true, that
      //  means we should exit.. so only do the input loop if it returns false.
      //  Also, check to make sure that getLeaveNow() returns false, in case
      //  the onFocus function called exitNow() or quitNow().
      if (!runOnFocusFunction() && !getLeaveNow())
      {
         if (mChangeColorsOnFocus)
         {
            // Enable the focus colors
            setFocusColors();
         }

         // Show the window
         if (pShowSelf)
         {
            show(pBringToTop, pShowSubwindows);
         }

         // Disable the cursor (saving the current cursor state)
         int prevCursorState = curs_set(0);
         // Do the input loop
         bool continueOn = true;
         bool runOnLeave = true; // Whether or not to run the onLeave function
         while (continueOn)
         {
            // Get a keypress from the user
            cxWindow::setLastKey(wgetch(mWindow));

            // Run the function for the last keypress if there is one.
            bool functionExists = false;
            continueOn = handleFunctionForLastKey(&functionExists, &runOnLeave);
            // If no function exists for the key, handle the key
            //  in a switch.
            if (!functionExists)
            {
               switch(mLastKey)
               {
                  case ESC:
                     mReturnCode = cxID_QUIT;
                     continueOn = false;
                     break;
#ifdef NCURSES_MOUSE_VERSION
                  case KEY_MOUSE: // Mouse event
                     if (getmouse(&mMouse) == OK)
                     {
                        // If this is just a cxWindow, then if the parent
                        //  window is a cxPanel, quit the input loop.  This
                        //  could allow the user to go to another window.
                        if (cxTypeStr() == "cxWindow")
                        {
                           if (parentIsCxPanel())
                           {
                              continueOn = false;
                           }
                           else
                           {
                              // Run a function that may exist for the mouse
                              //  state
                              bool mouseRunOnLeave = true;
                              continueOn = handleFunctionForLastMouseState(
                                                            &functionExists,
                                                            &mouseRunOnLeave);
                              runOnLeave = runOnLeave && mouseRunOnLeave;
                              // If there was no function for the mouse state,
                              //  process the mouse event here.
                              if (!functionExists)
                              {
                                 // see if they're trying to drag the window around
                                 if (mouseButton1Pressed())
                                 {
                                    if (mouseEvtWasInTitle())
                                    {
                                       movingWindow=true;
                                       // Looks like they're trying to drag the window...
                                       // Let's remember where the first started dragging from
                                       pressedX=mMouse.x;
                                       pressedY=mMouse.y;
                                    }
                                 }
                                 else if (mouseButton1Released())
                                 {
                                    if (movingWindow)
                                    {
                                       moveRelative(mMouse.y - pressedY, mMouse.x - pressedX, true);
                                    }
                                    movingWindow=false; // reset the moving of the window
                                 }
                              }
                           }
                        }
                        // If this is not actually a cxWindow (i.e., a class
                        //  derived from cxWindow), then quit out of this
                        //  input loop so that the derived object can process
                        //  mouse events.  Otherwise, do nothing (in a GUI, for
                        //  instance, if a user clicks arbitrarily in a window,
                        //  usually it does nothing).
                        else
                        {
                           continueOn = false;
                        }
                     }
                     break;
#endif
                  default:
                     // If the key is in the list of quit keys,
                     //  set the return code to cxID_QUIT; otherwise,
                     //  set it to cxID_EXIT.
                     if (hasQuitKey(getLastKey()))
                     {
                        mReturnCode = cxID_QUIT;
                     }
                     else
                     {
                        mReturnCode = cxID_EXIT;
                     }
                     continueOn = false;
                     break;
                  case NOKEY:  // No key - do nothing
                     break;
               }
            }
         }

         // Disable the focus colors and refresh the window
         if (mChangeColorsOnFocus)
         {
            unSetFocusColors();
            show(false, false);
         }

         // Set the cursor state back to what it was before
         curs_set(prevCursorState);

         mIsModal = false;

         // Run the onLeave function, if it's set and runOnLeave is still
         //  true, and if the user didn't press ESC.
         if (runOnLeave)
         {
            runOnLeave = runOnLeave && onLeaveFunctionIsSet() &&
                         (mLastKey != ESC);
            if (runOnLeave)
            {
               if (mOnLeaveFunction != nullptr)
               {
                  mOnLeaveFunction->runFunction();
               }
            }
         }
      }
      else
      {
         mIsModal = false;
      }
   }

   return(mReturnCode);
} // showModal

bool cxWindow::modalGetsKeypress() const
{
   // The default for cxWindow is simply whether or not the window is enabled;
   //  however, in deriving classes, this may be different.
   return(isEnabled());
} // moadlGetsKeypress

long cxWindow::setFocus(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   return(showModal(pShowSelf, pBringToTop, pShowSubwindows));
} // setFocus

void cxWindow::setFocusColors()
{
   // Add reverse to the message attributes so it stands out
   addAttr(eMESSAGE, A_REVERSE);
   mFocus = true;
} // setFocusColors

void cxWindow::unSetFocusColors(attr_t pAttr)
{
   // Remove reverse from the message attribute
   removeAttr(eMESSAGE, pAttr);
   mFocus = false;
} // unSetFocusColors

void cxWindow::setFocusColors(attr_t pAttr)
{
   // Add reverse to the message attributes so it stands out
   addAttr(eMESSAGE, pAttr);
   mFocus = true;
} // setFocusColors

void cxWindow::unSetFocusColors()
{
   // Remove reverse from the message attribute
   removeAttr(eMESSAGE, A_REVERSE);
   mFocus = false;
} // unSetFocusColors

// Erases the window
void cxWindow::erase(bool pEraseSubwindows)
{
   if (mWindow != nullptr)
   {
      werase(mWindow);
   }

   // Tell each subwindow to erase itself
   if (pEraseSubwindows)
   {
      for (cxWindow*& subWin : mSubWindows)
      {
         subWin->erase(pEraseSubwindows);
      }
   }

   // Update the physical screen
   cxBase::updateWindows();
} // erase

void cxWindow::bringToTop(bool pRefresh)
{
   if (mPanel == nullptr) return;
   top_panel(mPanel);
   if (pRefresh)
   {
      cxBase::updateWindows();
   }
} // bringToTop

void cxWindow::bringToBottom(bool pRefresh)
{
   if (mPanel == nullptr) return;
   bottom_panel(mPanel);
   if (pRefresh)
   {
      cxBase::updateWindows();
   }
} // bringToBottom

bool cxWindow::isOnTop() const
{
   if (mPanel == nullptr) return false;
   // panel_above will return nullptr if there is no
   //  other panel above mPanel.
   return(panel_above(mPanel) == nullptr);
} // isOnTop

bool cxWindow::isOnBottom() const
{
   if (mPanel == nullptr) return false;
   // panel_below will return nullptr if there is no
   //  other panel below mPanel.
   return(panel_below(mPanel) == nullptr);
} // isOnBottom

bool cxWindow::isAbove(const cxWindow& pThatWindow) const
{
   bool retval = false;

   // Follow the chain of windows up to see if this window
   //  is somewhere above pThatWindow.
   PANEL *panel = panel_above(pThatWindow.mPanel);
   cxWindow *window = nullptr;
   while (panel != nullptr)
   {
      // The user pointer of each window's panel should
      //  always point to their cxWindow.  If the panel's
      //  user pointer points to this window, then this
      //  window is on top of pThatWindow.
      window = (cxWindow*)panel_userptr(panel);
      if (window == this)
      {
         retval = true;
         break;
      }

      panel = panel_above(panel);
   }

   return(retval);
} // isAbove

bool cxWindow::isBelow(const cxWindow& pThatWindow) const
{
   bool retval = false;

   // Follow the chain of windows down to see if this window
   //  is somewhere below pThatWindow.
   PANEL *panel = panel_below(pThatWindow.mPanel);
   cxWindow *window = nullptr;
   while (panel != nullptr)
   {
      // The user pointer of each window's panel should
      //  always point to their cxWindow.  If the panel's
      //  user pointer points to this window, then this
      //  window is below pThatWindow.
      window = (cxWindow*)panel_userptr(panel);
      if (window == this)
      {
         retval = true;
         break;
      }

      panel = panel_below(panel);
   }

   return(retval);
} // isBelow

void cxWindow::hide(bool pHideSubwindows)
{
   // If not already hidden, hide the window.
   if (mPanel != nullptr && panel_hidden(mPanel) == FALSE)
   {
      hide_panel(mPanel);
   }

   // Tell each subwindow to hide itself
   if (pHideSubwindows)
   {
      for (cxWindow*& subWin : mSubWindows)
      {
         subWin->hide(pHideSubwindows);
      }
   }

   // Update the physical screen
   cxBase::updateWindows();
} // hide

void cxWindow::unhide(bool pUnhideSubwindows)
{
   // Only let the window be un-hidden if it's
   //  enabled.
   if (mEnabled)
   {
      show_panel(mPanel);

      // Tell each subwindow to unhide itself
      if (pUnhideSubwindows)
      {
         for (cxWindow*& subWin : mSubWindows)
         {
            subWin->unhide(pUnhideSubwindows);
         }
      }

      // Update the physical screen
      cxBase::updateWindows();
   }
} // unhide

void cxWindow::drawBorder(int pRow, int pCol, int pHeight, int pWidth,
      bool pTop, bool pBottom, bool pLeft, bool pRight)
      {
   // If there is a border, then draw it.
   if (hasBorder())
   {
      // Enable the attributes
      enableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, mBorderColorPair, nullptr);
      }

      // Draw the border differently, based on the border style.
      switch (mBorderStyle)
      {
         case eBS_SINGLE_LINE:
         default:
            // Draw a single-line border
            wborder(mWindow,
                    (pLeft ? ACS_VLINE : ' '),
                    (pRight ? ACS_VLINE : ' '),
                    (pTop ? ACS_HLINE : ' '),
                    (pBottom ? ACS_HLINE : ' '),
                    (pTop && pLeft ? ACS_ULCORNER : ' '),
                    (pTop && pRight ? ACS_URCORNER : ' '),
                    (pBottom && pLeft ? ACS_LLCORNER : ' '),
                    (pBottom && pRight ? ACS_LRCORNER : ' ')
                   );
            break;
         case eBS_DOUBLE_LINE:
            // Draw a double-line border (this only supported on terminals that
            // support extended ASCII/Codepage 437).  The numbers used here
            // came from Dan Gookin's book "Programmer's Guide to nCurses"
            // (1st edition?), page 173 (Chapter 14: "A Mixture of Stuff")
            wborder(mWindow,
                    (pLeft ? 0x000000ba : ' '),
                    (pRight ? 0x000000ba : ' '),
                    (pTop ? 0x000000cd : ' '),
                    (pBottom ? 0x000000cd : ' '),
                    (pTop && pLeft ? 0x000000c9 : ' '),
                    (pTop && pRight ? 0x000000bb : ' '),
                    (pBottom && pLeft ? 0x000000c8 : ' '),
                    (pBottom && pRight ? 0x000000bc : ' ')
                   );
            break;
         case eBS_DOUBLE_TOP_SINGLE_SIDES:
            wborder(mWindow,
                    (pLeft ? ACS_VLINE : ' '),
                    (pRight ? ACS_VLINE : ' '),
                    (pTop ? 0x000000cd : ' '),
                    (pBottom ? 0x000000cd : ' '),
                    (pTop && pLeft ? 0x000000d5 : ' '),
                    (pTop && pRight ? 0x000000b8 : ' '),
                    (pBottom && pLeft ? 0x000000d4 : ' '),
                    (pBottom && pRight ? 0x000000be : ' ')
                   );
            break;
         case eBS_SINGLE_TOP_DOUBLE_SIDES:
            wborder(mWindow,
                    (pLeft ? 0x000000ba : ' '),
                    (pRight ? 0x000000ba : ' '),
                    (pTop ? ACS_HLINE : ' '),
                    (pBottom ? ACS_HLINE : ' '),
                    (pTop && pLeft ? 0x000000d6 : ' '),
                    (pTop && pRight ? 0x000000b7 : ' '),
                    (pBottom && pLeft ? 0x000000d3 : ' '),
                    (pBottom && pRight ? 0x000000bd : ' ')
                   );
            break;
         case eBS_SPACE:
            // Draw the border using space(s)
            wborder(mWindow, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
            break;
      }

      // Disable the attributes
      disableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, 0, nullptr);
      }
   }
} // drawBorder

void cxWindow::drawBorder(int pHeight, int pWidth)
{
   drawBorder(0, 0, pHeight, pWidth, mDrawBorderTop, mDrawBorderBottom, mDrawBorderLeft, mDrawBorderRight);
} // drawBorder

void cxWindow::drawBorder()
{
   drawBorder(0, 0, height(), width(), mDrawBorderTop, mDrawBorderBottom, mDrawBorderLeft, mDrawBorderRight);
} // drawBorder

// returns the top of the window
int cxWindow::top() const
{
   int topRow = 0, leftCol = 0;
   getbegyx(mWindow, topRow, leftCol);

   return(topRow);
} // top

// returns the center row of the window
int cxWindow::centerRow() const
{
   return((top() + bottom())/2);
} // centerRow

// returns the bottom of the window
int cxWindow::bottom() const
{
   return(top()+height()-1);
} // bottom

// returns the left of the window
int cxWindow::left() const
{
   int topRow = 0, leftCol = 0;
   getbegyx(mWindow, topRow, leftCol);
   (void)topRow;
   return(leftCol);
} // left

// returns the center column of the window
int cxWindow::centerCol() const
{
   return((left() + right())/2);
} // centerCol

// returns the right of the window
int cxWindow::right() const
{
   return(left()+width()-1);
} // right

int cxWindow::width(int pWidth)
{
   return wresize(mWindow, height(), pWidth);
} // width

// Returns the width of the window
int cxWindow::width() const
{
   int theHeight = 0, theWidth = 0;
   getmaxyx(mWindow, theHeight, theWidth);
   (void)theHeight;
   return(theWidth);
} // width

int cxWindow::height(int pHeight)
{
   return wresize(mWindow, pHeight, width());
} // height

// Returns the height of the window
int cxWindow::height() const
{
   int theHeight = 0, theWidth = 0;
   getmaxyx(mWindow, theHeight, theWidth);

   return(theHeight);
} // height

void cxWindow::getRect(int& pTop, int& pLeft, int& pBottom, int& pRight)
{
   pTop = top();
   pLeft = left();
   pBottom = bottom();
   pRight = right();
} // getRect

bool cxWindow::pointIsInWindow(int pY, int pX) const
{
   return((pY >= top()) && (pY <= bottom()) &&
          (pX >= left()) && (pX <= right()));
} // pointIsInWindow

bool cxWindow::pointIsInTitle(int pY, int pX) const
{
   bool inTitle = false;

   // Check to see if there is a border before checking to see if the point
   //  is in the title area.
   if (hasBorder())
   {
      inTitle = ((pY == top()) && (pX >= left()) && (pX <= right()));
   }

   return(inTitle);
} // pointIsInTitle

void cxWindow::getSize(int& pHeight, int& pWidth)
{
   pHeight = height();
   pWidth = width();
} // getSize

bool cxWindow::hasFocus() const
{
   return(mFocus);
} // hasFocus

void cxWindow::setBorderStyle(eBorderStyle pBorderStyle)
{
   mBorderStyle = pBorderStyle;
} // setBorderStyle

eBorderStyle cxWindow::getBorderStyle() const
{
   return(mBorderStyle);
} // getBorderStyle

bool cxWindow::hasBorder() const
{
   return(getBorderStyle() != eBS_NOBORDER);
} // getBorderStyle

cxWindow* cxWindow::getParent() const
{
   return(mParentWindow);
} // getParent

// Changes the parent window pointer
void cxWindow::setParent(cxWindow *pNewParent)
{
   // Don't allow a cxWindow to be its own parent.
   // Also only set the parent if it's different from
   //  the current parent.
   if ((pNewParent != this) && (pNewParent != mParentWindow))
   {
      if (mParentWindow != nullptr)
      {
         // Make a copy of the current mParentWindow, because mParentWindow
         //  could be set to nullptr by the call to removeSubWindow().
         cxWindow *parentWindow = mParentWindow;
         // Tell the parent window to remove this window from its subwindow
         //  list.
         mParentWindow->removeSubWindow(this);

         // If this window has a cxPanel for a parent window,
         //  then remove this window from its mWindows.
         try
         {
            string parentType = parentWindow->cxTypeStr();
            if ((parentType == "cxPanel") || (parentType == "cxSearchPanel"))
            {
               cxPanel *parentPanel = dynamic_cast<cxPanel*>(parentWindow);
               cxPanel::cxWindowPtrCollection::iterator iter = parentPanel->mWindows.begin();
               for (; iter != parentPanel->mWindows.end(); ++iter)
               {
                  if (iter->get() == this)
                  {
                     parentPanel->mWindows.erase(iter);
                     break;
                  }
               }
            }
         }
         catch (const std::bad_cast& e)
         {
            // The dynamic_cast failed
         }
         catch (const std::bad_typeid& e)
         {
            // The typeid failed
         }
      }

      mParentWindow = pNewParent;

      // If the new mParentWindow is non-nullptr,
      // tell the new parent to add this
      // window to its subwindow list.
      if (mParentWindow != nullptr)
      {
         mParentWindow->addSubwindow(this);
      }
   }
} // setParent

// Returns a string containing all the characters
//  contained in this window.  The resulting window
//  'dump' will be placed into pResult.  Note that
//  each line is separated by a '\n' character.
void cxWindow::dump(string& pResult)
{
   pResult.erase();

   // If there is no border, go ahead and
   //  add everything line-by-line.  But if
   //  there is a border, it might be difficult
   //  to get the border characters (using
   //  mvwinchnstr() doesn't seem to extract
   //  the border characters properly).
   if (!hasBorder())
   {
      const int lineLength = width();
      unique_ptr line = make_unique<chtype[]>(lineLength);
      int bottomRow = bottom() - top() + 1;
      int numChars = 0; // # of chars read for each line
      for (int i = 0; i < bottomRow; ++i)
      {
         numChars = mvwinchnstr(mWindow, i, 0, line.get(), lineLength);
         if (numChars != ERR)
         {
            // We have to add each character 1 at a time to
            //  pResult, because in order to extract the characters
            //  from line, we have to AND each one with A_CHARTEXT.
            for (int i = 0; i < numChars; ++i)
            {
               //pResult += (line[i] & A_CHARTEXT);

               // Note: These tests don't seem to work..
               if (line[i] == ACS_ULCORNER)
               {
                  pResult += "┌";
               }
               else if (line[i] == ACS_URCORNER)
               {
                  pResult += "┐";
               }
               else if (line[i] == ACS_VLINE)
               {
                  pResult += "│";
               }
               else if (line[i] == ACS_HLINE)
               {
                  pResult += "─";
               }
               else
               {
                  pResult += (line[i] & A_CHARTEXT);
               }
            }
            pResult += '\n';
         }
      }
   }
   else
   {
      // We're using a border.
      // Add the top line:
      // Insert the upper-left corner character and
      //  the title.
      pResult += "┌";
      // TODO: This adds only the first title; update it to add all
      //  the titles in mTitleStrings
      pResult += getTitle();
      // Add horizontal line characters up to the right edge.
      size_t rightEdge = (size_t)right() - 1;
      for (size_t i = getTitle().length(); i < rightEdge; ++i)
      {
         pResult += "─";
      }
      // Add the upper-right corner character
      pResult += "┐\n";
      // For each line from top+1 to bottom-1, add
      //  the vertical line character ("│"), then
      //  add the text inside the box, then add
      //  another vertical line.
      const int lineLength = width()-2;
      unique_ptr line = make_unique<chtype[]>(lineLength);
      int bottomRow = bottom() - top();
      int numChars = 0; // # of chars read for each line
      for (int i = 1; i < bottomRow; ++i)
      {
         numChars = mvwinchnstr(mWindow, i, 1, line.get(), lineLength);
         if (numChars != ERR)
         {
            pResult += "│";
            // We have to add each character 1 at a time to
            //  pResult, because in order to extract the characters
            //  from line, we have to AND each one with A_CHARTEXT.
            for (int i = 0; i < numChars; ++i)
            {
               pResult += (line[i] & A_CHARTEXT);
            }
            pResult += "│\n";
         }
      }
      // Add the bottom-left character, status text, and
      //  the remainder of the bottom row.
      pResult += "└";
      // TODO: This adds only the first status string; update it to add all
      //  the strings in mStatusStrings
      pResult += getStatus();
      // Add horizontal line characters up to the right edge.
      for (size_t i = getStatus().length(); i < rightEdge; ++i)
      {
         pResult += "─";
      }
      pResult += "┘\n";
   }
} // dump

void cxWindow::showSubwindows(bool pBringToTop, bool pShowSubwindows)
{
   // Tell each subwindow to show itself.
   // If mShowSubwinsForward is true, then show them in forward order;
   //  otherwise, show them in reverse order.
   if (mShowSubwinsForward)
   {
      for (cxWindow*& subWin : mSubWindows)
      {
         subWin->show(pBringToTop, pShowSubwindows);
      }
   }
   else
   {
      cxWindowPtrContainer::reverse_iterator iter = mSubWindows.rbegin();
      for (; iter != mSubWindows.rend(); ++iter)
      {
         (*iter)->show(pBringToTop, pShowSubwindows);
      }
   }
} // showSubwindows

cxWindow& cxWindow::operator =(const cxWindow& pThatWindow)
{
   // Only try to copy pThatWindow if it's a different instance.
   if (&pThatWindow != this)
   {
      copyCxWinStuff(&pThatWindow);
   }

   return(*this);
} // operator =

void cxWindow::writeText(int pRow, int pCol, const string& pText, bool pRefresh)
{
   // Enable the attributes
   enableAttrs(mWindow, eMESSAGE);
   // Write the text
   mvwprintw(mWindow, pRow, pCol, "%s", pText.c_str());
   // Disable the attributes
   disableAttrs(mWindow, eMESSAGE);
   if (pRefresh)
   {
      wrefresh(mWindow);
   }
} // writeText

void cxWindow::setHotkeyHighlighting(bool pHotkeyHighlighting)
{
   // If pHotkeyHighlighting is different from mHotkeyHighlighting,
   //  the window & window message will need to be re-initialized
   //  in case the message text will have to be shifted due to
   //  underline characters in the window message.
   bool reInit = (pHotkeyHighlighting != mHotkeyHighlighting);

   // Set mHotkeyHighlighting
   mHotkeyHighlighting = pHotkeyHighlighting;

   if (reInit)
   {
      // Re-combine the message lines into a single line (for calling
      //  init() again)
      string message;
      combineMessageLines(message);
      init(top(), left(), height(), width(), getTitle(), message, getStatus(), getParent());
   }
} // setHotkeyHighlighting

bool cxWindow::getHotkeyHighlighting() const
{
   return(mHotkeyHighlighting);
} // getHotkeyHighlighting

bool cxWindow::isEnabled() const
{
   return(mEnabled);
} // isEnabled

void cxWindow::setEnabled(bool pEnabled)
{
   mEnabled = pEnabled;
   // If mEnabled is false, hide the window.
   if (!mEnabled)
   {
      hide(false);
   }
} // setEnabled

void cxWindow::setDisableCursorOnShow(bool pDisableCursorOnShow)
{
   mDisableCursorOnShow = pDisableCursorOnShow;
} // disableCursorOnShow

bool cxWindow::getDisableCursorOnShow() const
{
   return(mDisableCursorOnShow);
} // getDisableCursorOnShow

int cxWindow::getLastKey() const
{
   return(mLastKey);
} // getLastKey

void cxWindow::setLastKey(int pLastKey)
{
   mLastKey = pLastKey;
   // If this window has a parent window, then
   //  the parent window's last key should be
   //  set too.
   if (mParentWindow != nullptr)
   {
      mParentWindow->setLastKey(pLastKey);
   }
} // setLastKey

bool cxWindow::lastKeyWasMouseEvt() const
{
   bool retval = false;
#ifdef NCURSES_MOUSE_VERSION
   retval = (mLastKey == KEY_MOUSE);
#endif
   return(retval);
} // lastKeyWasMouseEvt

void cxWindow::getLastMouseEvtCoords(int& pY, int& pX)
{
   pY = 0;
   pX = 0;
#ifdef NCURSES_MOUSE_VERSION
   pY = mMouse.y;
   pX = mMouse.x;
#endif
} // getLastMouseEvtCoords

bool cxWindow::getChangeColorsOnFocus() const
{
   return(mChangeColorsOnFocus);
} // getChangeColorsOnFocus

void cxWindow::setChangeColorsOnFocus(bool pChangeColorsOnFocus)
{
   mChangeColorsOnFocus = pChangeColorsOnFocus;
} // setChangeColorsOnFocus

void cxWindow::setOnFocusFunction(const std::shared_ptr<cxFunction>& pFunction)
{
   mOnFocusFunction = pFunction;
} // setOnFocusFunction

void cxWindow::setOnFocusFunction(funcPtr4 pFunction, void *p1, void *p2,
                                  void *p3, void *p4, bool pUseVal,
                                  bool pExitAfterRun)
                                  {
   shared_ptr<cxFunction4> funcPtr = make_shared<cxFunction4>(pFunction, p1, p2, p3, p4, pUseVal,
                                                              pExitAfterRun, false);
   if (funcPtr != nullptr)
   {
      mOnFocusFunction = dynamic_pointer_cast<cxFunction>(funcPtr);
   }
   else
   {
      mOnFocusFunction = nullptr;
   }
} // setOnFocusFunction

void cxWindow::setOnFocusFunction(funcPtr2 pFunction, void *p1, void *p2,
                                 bool pUseVal, bool pExitAfterRun)
                                 {
   shared_ptr<cxFunction2> funcPtr = make_shared<cxFunction2>(pFunction, p1, p2, pUseVal,
                                                              pExitAfterRun, false);
   if (funcPtr != nullptr)
   {
      mOnFocusFunction = dynamic_pointer_cast<cxFunction>(funcPtr);
   }
   else
   {
      mOnFocusFunction = nullptr;
   }
} // setOnFocusFunction

void cxWindow::setOnFocusFunction(funcPtr0 pFunction, bool pUseVal,
                                  bool pExitAfterRun)
                                  {
   shared_ptr<cxFunction0> funcPtr = make_shared<cxFunction0>(pFunction, pUseVal,
                                                              pExitAfterRun, false);
    if (funcPtr != nullptr)
    {
        mOnFocusFunction = dynamic_pointer_cast<cxFunction>(funcPtr);
    }
    else
    {
      mOnFocusFunction = nullptr;
    }
} // setOnFocusFunction

void cxWindow::setOnLeaveFunction(const std::shared_ptr<cxFunction>& pFunction)
{
   mOnLeaveFunction = pFunction;
} // setOnLeaveFunction

void cxWindow::setOnLeaveFunction(funcPtr4 pFunction, void *p1, void *p2,
                                  void *p3, void *p4)
                                  {
   shared_ptr<cxFunction4> funcPtr = make_shared<cxFunction4>(pFunction, p1, p2, p3, p4, false,
                                                              false, false);
   if (funcPtr != nullptr)
   {
      mOnLeaveFunction = dynamic_pointer_cast<cxFunction>(funcPtr);
   }
   else
   {
      mOnLeaveFunction = nullptr;
   }
} // setOnLeaveFunction

void cxWindow::setOnLeaveFunction(funcPtr2 pFunction, void *p1, void *p2)
{
  shared_ptr<cxFunction2> funcPtr = make_shared<cxFunction2>(pFunction, p1, p2, false,
                                                              false, false);
   if (funcPtr != nullptr)
   {
      mOnLeaveFunction = dynamic_pointer_cast<cxFunction>(funcPtr);
   }
   else
   {
      mOnLeaveFunction = nullptr;
   }
} // setOnLeaveFunction

void cxWindow::setOnLeaveFunction(funcPtr0 pFunction)
{
   shared_ptr<cxFunction0> funcPtr = make_shared<cxFunction0>(pFunction, false, false, false);
   if (funcPtr != nullptr)
   {
      mOnLeaveFunction = dynamic_pointer_cast<cxFunction>(funcPtr);
   }
   else
   {
      mOnLeaveFunction = nullptr;
   }
} // setOnLeaveFunction

bool cxWindow::runOnFocusFunction(string *pFunctionRetval)
{
   bool exitAfterRun = false;

   if (mOnFocusFunction != nullptr)
   {
      if (onFocusFunctionIsSet() && mRunOnFocus)
      {
         exitAfterRun = mOnFocusFunction->getExitAfterRun();
         string funcRetval = mOnFocusFunction->runFunction();
         if (nullptr != pFunctionRetval)
         {
            *pFunctionRetval = funcRetval;
         }
      }
   }

   return(exitAfterRun);
} // runOnFocusFunction

bool cxWindow::runOnLeaveFunction(string *pFunctionRetval)
{
   bool exitAfterRun = false;

   if (mOnLeaveFunction != nullptr)
   {
      // Don't do anything if the last keypress is in mQuitKeys or mExitKeys and
      // the boolean for the key is false.
      // Check to see whether the onLeave function should be run, based on
      //  the last keypress:
      //  - If the last keypress is in mQuitKeys or mExitKeys and the boolean
      //    for that key in mQuitKeys/mExitKeys is false, don't run the onLeave
      //    function.
      //  - If the last keypress is in mKeyFunctions, and the
      //    the function's runOnLeaveFunction parameter is set false, don't run
      //    the onLeave function.
      bool iRunOnLeave = true;
      int lastKey = getLastKey();
      // Note: The last key shouldn't be in both mQuitKeys and mExitKeys, but
      //  check both anyways.
      if (mQuitKeys.find(lastKey) != mQuitKeys.end())
      {
         iRunOnLeave = mQuitKeys[lastKey];
      }
      if (mExitKeys.find(lastKey) != mExitKeys.end())
      {
         iRunOnLeave = iRunOnLeave && mExitKeys[lastKey];
      }

      // Check mKeyFunctions
      if (mKeyFunctions.find(lastKey) != mKeyFunctions.end())
      {
         // The cxFunction pointers in mKeyFunctions shouldn't be nullptr,
         //  but check anyway.
         if (mKeyFunctions[lastKey] != nullptr)
         {
            iRunOnLeave = iRunOnLeave && mKeyFunctions[lastKey]->getRunOnLeaveFunction();
         }
      }

      if (mOnLeaveFunction->functionIsSet() && iRunOnLeave && mRunOnLeave)
      {
         exitAfterRun = mOnLeaveFunction->getExitAfterRun();
         string funcRetval = mOnLeaveFunction->runFunction();
         if (nullptr != pFunctionRetval)
         {
            *pFunctionRetval = funcRetval;
         }
      }
   }

   return(exitAfterRun);
} // runOnLeaveFunction

void cxWindow::toggleOnFocusFunction(bool pRunOnFocus)
{
   mRunOnFocus = pRunOnFocus;
} // toggleOnFocusFunction

void cxWindow::toggleOnLeaveFunction(bool pRunOnLeave)
{
   mRunOnLeave = pRunOnLeave;
} // toggleOnLeaveFunction

bool cxWindow::onFocusFunctionEnabled() const
{
   return(mRunOnFocus);
} // onFocusFunctionEnabled

bool cxWindow::onLeaveFunctionEnabled() const
{
   return(mRunOnLeave);
} // onLeaveFunctionEnabled

const shared_ptr<cxFunction>& cxWindow::getOnFocusFunction() const
{
   return(mOnFocusFunction);
} // getOnFocusFunction

const shared_ptr<cxFunction>& cxWindow::getOnLeaveFunction() const
{
   return(mOnLeaveFunction);
} // getOnLeaveFunction

bool cxWindow::isModal() const
{
   return(mIsModal);
} // isModal

bool cxWindow::setKeyFunction(int pKey, const shared_ptr<cxFunction>& pFunction)
{
   if (pFunction != nullptr)
   {
      shared_ptr<cxFunction> func = dynamic_pointer_cast<cxFunction>(pFunction);
      if (func != nullptr)
      {
         mKeyFunctions[pKey] = func;
         // Remove the key from the lists of exit keys and quit keys so that we
         // can be sure that the key won't make the window leave its input loop
         // before the function can fire
         removeExitKey(pKey);
         removeQuitKey(pKey);
      }
      else
      {
         // The dynamic_pointer_cast failed, so remove the key from the map.
         mKeyFunctions.erase(pKey);
      }
   }
   else
   {
      mKeyFunctions.erase(pKey);
   }

   return true;
} // setKeyFunction

bool cxWindow::setKeyFunction(int pKey, funcPtr4 pFunction, void *p1, void *p2,
                            void *p3, void *p4, bool pUseReturnVal,
                            bool pExitAfterRun, bool pRunOnLeaveFunction)
                            {
   bool setIt = false;

   // If pFunction is nullptr, then remove the function from mKeyFunctions if
   //  it exists; otherwise, do an add.
   if (pFunction == nullptr)
   {
      // Remove it
      clearKeyFunction(pKey);
      // setIt should be true if it doesn't exist
      setIt = !hasKeyFunction(pKey);
   }
   else
   {
       // pFunction isn't nullptr.  Add it.
      shared_ptr<cxFunction4> func4Ptr = make_shared<cxFunction4>(pFunction, p1, p2, p3, p4,
                                                                  pUseReturnVal, pExitAfterRun,
                                                                  pRunOnLeaveFunction);
      if (func4Ptr != nullptr)
      {
         shared_ptr<cxFunction> func = dynamic_pointer_cast<cxFunction>(func4Ptr);
         if (func != nullptr)
         {
            mKeyFunctions[pKey] = func;
            setIt = true;
         }
      }
      else
      {
         // We couldn't allocate any memory, so remove it from the map.
         mKeyFunctions.erase(pKey);
      }
   }

   // If the key was successfully set, remove it from the lists of exit keys
   // and quit keys (so that we can be sure that the key won't make the
   // window leave its input loop before the function can fire).
   if (setIt)
   {
      removeExitKey(pKey);
      removeQuitKey(pKey);
   }

   return(setIt);
} // setKeyFunction

bool cxWindow::setKeyFunction(int pKey, funcPtr2 pFunction, void *p1,
                            void *p2, bool pUseReturnVal, bool pExitAfterRun,
                            bool pRunOnLeaveFunction)
                            {
   bool setIt = false;

   // If pFunction is nullptr, then remove the function from mKeyFunctions if
   //  it exists; otherwise, do an add.
   if (pFunction == nullptr)
   {
      // Remove it
      clearKeyFunction(pKey);
      // setIt should be true if it doesn't exist
      setIt = !hasKeyFunction(pKey);
   }
   else
   {
      // pFunction isn't nullptr.  Add it.
      shared_ptr<cxFunction2> func2Ptr = make_shared<cxFunction2>(pFunction, p1, p2,
                                                                  pUseReturnVal, pExitAfterRun,
                                                                  pRunOnLeaveFunction);
      if (func2Ptr != nullptr)
      {
         shared_ptr<cxFunction> func = dynamic_pointer_cast<cxFunction>(func2Ptr);
         if (func != nullptr)
         {
            mKeyFunctions[pKey] = func;
            setIt = true;
         }
      }
      else
      {
         // We couldn't allocate any memory, so remove it from the map.
         mKeyFunctions.erase(pKey);
      }
   }

   // If the key was successfully set, remove it from the lists of exit keys
   // and quit keys (so that we can be sure that the key won't make the
   // window leave its input loop before the function can fire).
   if (setIt)
   {
      removeExitKey(pKey);
      removeQuitKey(pKey);
   }

   return(setIt);
} // setKeyFunction

bool cxWindow::setKeyFunction(int pKey, funcPtr0 pFunction, bool pUseReturnVal,
                              bool pExitAfterRun, bool pRunOnLeaveFunction)
                              {
   bool setIt = false;

   // If pFunction is nullptr, then remove the function from mKeyFunctions if
   //  it exists; otherwise, do an add.
   if (pFunction == nullptr)
   {
      // Remove it
      clearKeyFunction(pKey);
      // setIt should be true if it doesn't exist
      setIt = !hasKeyFunction(pKey);
   }
   else
   {
      // pFunction isn't nullptr.  Add it.
      shared_ptr<cxFunction0> func0Ptr = make_shared<cxFunction0>(pFunction, pUseReturnVal, pExitAfterRun,
                                                                  pRunOnLeaveFunction);
      if (func0Ptr != nullptr)
      {
         shared_ptr<cxFunction> func = dynamic_pointer_cast<cxFunction>(func0Ptr);
         if (func != nullptr)
         {
            mKeyFunctions[pKey] = func;
            setIt = true;
         }
      }
      else
      {
         // We couldn't allocate any memory, so remove it from the map.
         mKeyFunctions.erase(pKey);
      }
   }

   // If the key was successfully set, remove it from the lists of exit keys
   // and quit keys (so that we can be sure that the key won't make the
   // window leave its input loop before the function can fire).
   if (setIt)
   {
      removeExitKey(pKey);
      removeQuitKey(pKey);
   }

   return(setIt);
} // setKeyFunction

void cxWindow::clearKeyFunction(int pKey)
{
   // If the function exists, free the memory used by it and remove it
   // from mKeyFunctions.
   if (mKeyFunctions.find(pKey) != mKeyFunctions.end())
   {
      mKeyFunctions.erase(pKey);
   }

   // If the key no longer exists as a function key, then check to
   //  see if this window is in a cxPanel.  If so, then if the key
   //  is in the panel's function keys, then we need to add it to
   //  this window as an exit key so that it can trickle up to the
   //  cxPanel to be processed.
   if (!hasKeyFunction(pKey))
   {
      if (mParentWindow != nullptr)
      {
         string parentType = mParentWindow->cxTypeStr();
         if ((parentType == "cxPanel") || (parentType == "cxSearchPanel"))
         {
            cxPanel *parentPanel = dynamic_cast<cxPanel*>(mParentWindow);
            if (parentPanel != nullptr)
            {
               if (parentPanel->hasKeyFunction(pKey))
               {
                  addExitKey(pKey);
               }
            }
         }
      }
   }
} // clearKeyFunction

void cxWindow::clearKeyFunctionByPtr(funcPtr4 pFunction)
{
   // Create a set of keys that fire the function, and then call
   //  the other clearKeyFunction() for each key.
   set<int> keys;
   for (const pair<const int, shared_ptr<cxFunction> >& funcPair : mKeyFunctions)
   {
      try
      {
         shared_ptr<cxFunction4> func4 = dynamic_pointer_cast<cxFunction4>(funcPair.second);
         if (func4 != nullptr && func4->getFunction() == pFunction)
         {
            keys.insert(funcPair.first);
         }
      }
      catch (const std::bad_cast& e)
      {
      }
      catch (...)
      {
      }
   }

   // Call the other clearKeyFunction() for each key.
   for (const int& key : keys)
   {
      clearKeyFunction(key);
   }
} // clearKeyFunction

void cxWindow::clearKeyFunctionByPtr(funcPtr2 pFunction)
{
   // Create a set of keys that fire the function, and then call
   //  the other clearKeyFunction() for each key.
   set<int> keys;
   for (const pair<const int, std::shared_ptr<cxFunction> >& funcPair : mKeyFunctions)
   {
      try
      {
         shared_ptr<cxFunction2> func2 = dynamic_pointer_cast<cxFunction2>(funcPair.second);
         if (func2 != nullptr && func2->getFunction() == pFunction)
         {
            keys.insert(funcPair.first);
         }
      }
      catch (const std::bad_cast& e)
      {
      }
      catch (...)
      {
      }
   }

   // Call the other clearKeyFunction() for each key.
   for (const int& key : keys)
   {
      clearKeyFunction(key);
   }
} // clearKeyFunction

void cxWindow::clearKeyFunctionByPtr(funcPtr0 pFunction)
{
   // Create a set of keys that fire the function, and then call
   //  the other clearKeyFunction() for each key.
   set<int> keys;
   for (const pair<const int, std::shared_ptr<cxFunction> >& funcPair : mKeyFunctions)
   {
      try
      {
         shared_ptr<cxFunction0> func0 = dynamic_pointer_cast<cxFunction0>(funcPair.second);
         if (func0 != nullptr && func0->getFunction() == pFunction)
         {
            keys.insert(funcPair.first);
         }
      }
      catch (const std::bad_cast& e)
      {
      }
      catch (...)
      {
      }
   }

   // Call the other clearKeyFunction() for each key.
   for (const int& key : keys)
   {
      clearKeyFunction(key);
   }
} // clearKeyFunction

void cxWindow::clearKeyFunctions()
{
   // Clear the map
   mKeyFunctions.clear();

   // Check to see if this window is in a cxPanel.  If so, then
   //  we need to add the panel's function keys to this window as exit
   //  keys so they can trickle up to the cxPanel to be processed.
   if (mParentWindow != nullptr)
   {
      try
      {
         string parentType = mParentWindow->cxTypeStr();
         if ((parentType == "cxPanel") || (parentType == "cxSearchPanel"))
         {
            cxPanel *parentPanel = dynamic_cast<cxPanel*>(mParentWindow);
            vector<int> functionKeys;
            parentPanel->getFunctionKeys(functionKeys);
            for (const int& key : functionKeys)
            {
               addExitKey(key);
            }
         }
      }
      catch (const std::bad_cast& e)
      {
            // A dynamic_cast failed
      }
      catch (const std::bad_typeid& e)
      {
         // A typeid failed
      }
  }
} // clearKeyFunctions

bool cxWindow::hasKeyFunction(int pKey) const
{
   bool exists = false;

   map<int, shared_ptr<cxFunction> >::const_iterator iter = mKeyFunctions.find(pKey);
   if (iter != mKeyFunctions.end())
   {
      // Check to make sure that the cxFunction pointer is not nullptr
      exists = (iter->second != nullptr);
   }

   return(exists);
} // hasKeyFunction

bool cxWindow::setMouseFunction(int pMouseState, const shared_ptr<cxFunction>& pFunction)
{
   mMouseFunctions[pMouseState] = pFunction;
   return true;
} // setMouseFunction

bool cxWindow::setMouseFunction(int pMouseState, funcPtr4 pFunction, void *p1,
                            void *p2, void *p3, void *p4, bool pUseReturnVal,
                            bool pExitAfterRun, bool pRunOnLeaveFunction)
                            {
   bool setIt = false;

   // If pFunction is nullptr, then remove the function from mKeyFunctions if
   //  it exists; otherwise, do an add.
   if (pFunction == nullptr)
   {
      // Remove it
      clearMouseFunction(pMouseState);
      // setIt should be true if it doesn't exist
      setIt = !hasMouseFunction(pMouseState);
   }
   else
   {
      // pFunction isn't nullptr.  Add it.
      shared_ptr<cxFunction4> func4Ptr = make_shared<cxFunction4>(pFunction, p1, p2, p3, p4,
                                                                  pUseReturnVal, pExitAfterRun,
                                                                  pRunOnLeaveFunction);
      if (func4Ptr != nullptr)
      {
         mMouseFunctions[pMouseState] = dynamic_pointer_cast<cxFunction>(func4Ptr);
         setIt = true;
      }
      else
      {
         // We couldn't allocate any memory, so remove it from the map.
         mMouseFunctions.erase(pMouseState);
      }
   }

   return(setIt);
} // setMouseFunction

bool cxWindow::setMouseFunction(int pMouseState, funcPtr2 pFunction, void *p1,
                            void *p2, bool pUseReturnVal, bool pExitAfterRun,
                            bool pRunOnLeaveFunction)
                            {
   bool setIt = false;

   // If pFunction is nullptr, then remove the function from mKeyFunctions if
   //  it exists; otherwise, do an add.
   if (pFunction == nullptr)
   {
      // Remove it
      clearMouseFunction(pMouseState);
      // setIt should be true if it doesn't exist
      setIt = !hasMouseFunction(pMouseState);
   }
   else
   {
      // pFunction isn't nullptr.  Add it.
      shared_ptr<cxFunction2> func2Ptr = make_shared<cxFunction2>(pFunction, p1, p2,
                                                                  pUseReturnVal, pExitAfterRun,
                                                                  pRunOnLeaveFunction);
      if (func2Ptr != nullptr)
      {
         mMouseFunctions[pMouseState] = dynamic_pointer_cast<cxFunction>(func2Ptr);
         setIt = true;
      }
      else
      {
         // We couldn't allocate any memory, so remove it from the map.
         mMouseFunctions.erase(pMouseState);
      }
   }

   return(setIt);
} // setMouseFunction

bool cxWindow::setMouseFunction(int pMouseState, funcPtr0 pFunction,
                            bool pUseReturnVal, bool pExitAfterRun,
                            bool pRunOnLeaveFunction)
                            {
   bool setIt = false;

   // If pFunction is nullptr, then remove the function from mKeyFunctions if
   //  it exists; otherwise, do an add.
   if (pFunction == nullptr)
   {
      // Remove it
      clearMouseFunction(pMouseState);
      // setIt should be true if it doesn't exist
      setIt = !hasMouseFunction(pMouseState);
   }
   else
   {
      // pFunction isn't nullptr.  Add it.
      shared_ptr<cxFunction0> func0Ptr = make_shared<cxFunction0>(pFunction,
                                                                  pUseReturnVal, pExitAfterRun,
                                                                  pRunOnLeaveFunction);
      if (func0Ptr != nullptr)
      {
         mMouseFunctions[pMouseState] = dynamic_pointer_cast<cxFunction>(func0Ptr);
         setIt = true;
      }
      else
      {
         // We couldn't allocate any memory, so remove it from the map.
         mMouseFunctions.erase(pMouseState);
      }
   }

   return(setIt);
} // setMouseFunction

void cxWindow::clearMouseFunction(int pMouseState)
{
   // If the mouse event exists, free the memory used by it and remove it
   //  from mMouseFunctions.
   if (mMouseFunctions.find(pMouseState) != mMouseFunctions.end())
   {
      mMouseFunctions.erase(pMouseState);
   }
} // clearMouseFunction

void cxWindow::clearMouseFunctions()
{
   // Clear the map
   mMouseFunctions.clear();
} // clearMouseFunctions

bool cxWindow::hasMouseFunction(int pMouseState) const
{
   bool exists = false;

   map<int, shared_ptr<cxFunction> >::const_iterator iter = mMouseFunctions.find(pMouseState);
   if (iter != mMouseFunctions.end())
   {
      // Check to make sure that the cxFunction pointer is not nullptr
      exists = (iter->second != nullptr);
   }

   return(exists);
} // hasMouseFunction

void cxWindow::setShowSubwinsForward(bool pShowSubwinsForward)
{
   mShowSubwinsForward = pShowSubwinsForward;
} // setShowSubwinsForward

bool cxWindow::getShowSubwinsForward() const
{
   return(mShowSubwinsForward);
} // getShowSubwinsForward

void cxWindow::setShowSelfBeforeSubwins(bool pShowSelfBeforeSubwins)
{
   mShowSelfBeforeSubwins = pShowSelfBeforeSubwins;
} // setShowSelfBeforeSubwins

bool cxWindow::getShowSelfBeforeSubwins() const
{
   return(mShowSelfBeforeSubwins);
} // getShowSelfBeforeSubwins

long cxWindow::getReturnCode() const
{
   return(mReturnCode);
} // getReturnCode

bool cxWindow::addTitleString(int pHPos, const string& pStr,
                              bool pRefreshTitle)
                              {
   bool addedIt = false;

   // Make sure pHPos is within the left & right borders
   if ((pHPos > 0) && (pHPos < width()))
   {
      // Calculate the maximum width of the string, based on pHPos and
      //  the window width, and truncate pStr before adding it.
      int innerWidth = width()-2;
      size_t maxLen = (size_t)(innerWidth - pHPos + 1);
      string truncatedStr = pStr.substr(0, maxLen);
      mTitleStrings[pHPos] = truncatedStr;
      addedIt = (mTitleStrings.find(pHPos) != mTitleStrings.end());
      if (addedIt && pRefreshTitle)
      {
         drawTitle();
      }
   }

   return(addedIt);
} // addTitleString

string cxWindow::getTitleString(int pHPos) const
{
   string titleString;

   // First see if pHPos exists as a key in mTitleStrings, and if it does,
   //  we can go ahead and get the title string at that location (indexing
   //  into an STL map automatically adds a value if it doesn't exist).
   map<int, string>::const_iterator iter = mTitleStrings.find(pHPos);
   if (iter != mTitleStrings.end())
   {
      titleString = iter->second;
   }

   return(titleString);
} // getTitleString

map<int, string> cxWindow::getTitleStrings() const
{
   return(mTitleStrings);
} // getTitleStrings

void cxWindow::getTitleStrings(map<int, string>& pTitleStrings) const
{
   pTitleStrings = mTitleStrings;
} // getTitleStrings

void cxWindow::setTitleStrings(const map<int, string>& pTitleStrings)
{
   mTitleStrings = pTitleStrings;
   // Make sure there is a title string at horizontal position 1 (for
   //  the main title string)
   if (mTitleStrings.find(1) == mTitleStrings.end())
   {
      //mTitleStrings.insert(std::make_pair(1, ""));
      string blankStr;  // to get this to compile on AIX..
      mTitleStrings.insert(std::make_pair(1, blankStr));
   }
} // setTitleStrings

void cxWindow::removeTitleString(int pHPos)
{
   map<int, string>::iterator pos = mTitleStrings.find(pHPos);
   if (pos != mTitleStrings.end())
   {
      mTitleStrings.erase(pos);
   }

   // The main title string is at position 1..  If the user removed this
   //  string, make sure there's still a blank string there.
   if (pHPos == 1)
   {
      mTitleStrings[1] = "";
   }
} // removeTitleString

void cxWindow::removeTitleStrings()
{
   mTitleStrings.clear();

   // Make sure there's still a string at position 1 (this is the main title
   //  string).
   mTitleStrings[1] = "";
} // removeTitleStrings

bool cxWindow::addStatusString(int pHPos, const string& pStr,
                               bool pRefreshStatus)
                               {
   bool addedIt = false;

   // Make sure pHPos is within the left & right borders
   if ((pHPos > 0) && (pHPos < width()))
   {
      // Calculate the maximum width of the string, based on pHPos and
      //  the window width, and truncate pStr before adding it.
      int innerWidth = width()-2;
      size_t maxLen = (size_t)(innerWidth - pHPos + 1);
      string truncatedStr = pStr.substr(0, maxLen);
      mStatusStrings[pHPos] = truncatedStr;
      addedIt = (mStatusStrings.find(pHPos) != mStatusStrings.end());
      if (addedIt && pRefreshStatus)
      {
         drawStatus();
      }
   }

   return(addedIt);
} // addStatusString

string cxWindow::getStatusString(int pHPos) const
{
   string statusString;

   // First see if pHPos exists as a key in mStatusStrings, and if it does,
   //  we can go ahead and get the status string at that location (indexing
   //  into an STL map automatically adds a value if it doesn't exist).
   map<int, string>::const_iterator iter = mStatusStrings.find(pHPos);
   if (iter != mStatusStrings.end())
   {
      statusString = iter->second;
   }

   return(statusString);
} // getStatusString

map<int, string> cxWindow::getStatusStrings() const
{
   return(mStatusStrings);
} // getStatusStrings

void cxWindow::getStatusStrings(map<int, string>& pStatusStrings) const
{
   pStatusStrings = mStatusStrings;
} // getStatusStrings

void cxWindow::setStatusStrings(const map<int, string>& pStatusStrings)
{
   mStatusStrings = pStatusStrings;
   // Make sure there is a status string at horizontal position 1 (for
   //  the main status string)
   if (mStatusStrings.find(1) == mStatusStrings.end())
   {
      //mStatusStrings.insert(std::make_pair(1, ""));
      //string blankString; // to get this to compile on AIX..
      mStatusStrings.insert(std::make_pair(1, string("")));
   }
} // setStatusStrings

void cxWindow::removeStatusString(int pHPos)
{
   map<int, string>::iterator pos = mStatusStrings.find(pHPos);
   if (pos != mStatusStrings.end())
   {
      mStatusStrings.erase(pos);
   }

   // The main title string is at position 1..  If the user removed this
   //  string, make sure there's still a blank string there.
   if (pHPos == 1)
   {
      mStatusStrings[1] = "";
   }
} // removeStatusString

void cxWindow::removeStatusStrings()
{
   mStatusStrings.clear();

   // Make sure there's still a string at position 1 (this is the main status
   //  string).
   mStatusStrings[1] = "";
} // removeTitleStrings

bool cxWindow::addQuitKey(int pKey, bool pRunOnLeaveFunction, bool pOverride)
{
   bool added = false;

   // Add the quit key if it doesn't exist as a function key or if pOverride
   //  is true.
   if (!hasKeyFunction(pKey) || pOverride)
   {
      mQuitKeys[pKey] = pRunOnLeaveFunction;
      added = (mExitKeys.find(pKey) != mExitKeys.end());
   }

   // If the key was added as a quit key, remove it as an exit key.
   if (added)
   {
      mExitKeys.erase(pKey);
   }

   return(added);
} // addQuitKey

void cxWindow::removeQuitKey(int pKey)
{
   mQuitKeys.erase(pKey);
} // removeQuitKey

void cxWindow::removeQuitKeys()
{
   mQuitKeys.clear();
} // removeQuitKeys

bool cxWindow::addExitKey(int pKey, bool pRunOnLeaveFunction, bool pOverride)
{
   bool added = false;
   // Add the exit key if it doesn't exist as a function key or if pOverride
   //  is true.
   if (!hasKeyFunction(pKey) || pOverride)
   {
      mExitKeys[pKey] = pRunOnLeaveFunction;
      added = (mExitKeys.find(pKey) != mExitKeys.end());
   }

   // If the key was added as an exit key, remove it as a quit key.
   if (added)
   {
      mQuitKeys.erase(pKey);
   }

   return(added);
} // addExitKey

void cxWindow::removeExitKey(int pKey)
{
   mExitKeys.erase(pKey);
} // removeExitKey

void cxWindow::removeExitKeys()
{
   mExitKeys.clear();
} // removeExitKeys

bool cxWindow::hasExitKey(int pKey) const
{
   return(mExitKeys.find(pKey) != mExitKeys.end());
} // hasExitKey

bool cxWindow::hasQuitKey(int pKey) const
{
   return(mQuitKeys.find(pKey) != mQuitKeys.end());
} // hasQuitKey

void cxWindow::clear(bool pRefresh)
{
   setMessage("");
   if (pRefresh)
   {
      show(false, false);
   }
} // clear

void cxWindow::quitNow()
{
   // Only set mLeaveNow true if the window is modal.  Otherwise, other widgets
   //  could end up in infinite loop land.
   if (isModal())
   {
      mLeaveNow = true;
      mReturnCode = cxID_QUIT;
   }
   else
   {
      mLeaveNow = false;
   }
} // quitNow

void cxWindow::exitNow()
{
   // Only set mLeaveNow true if the window is modal.  Otherwise, other widgets
   //  could end up in infinite loop land.
   if (isModal())
   {
      mLeaveNow = true;
      mReturnCode = cxID_EXIT;
   }
   else
   {
      mLeaveNow = false;
   }
} // exitNow

void cxWindow::getQuitKeyStrings(vector<string>& pKeys) const
{
   pKeys.clear();

   for (const auto& mapPair : mQuitKeys)
   {
      pKeys.push_back(cxBase::getKeyStr(mapPair.first));
   }
} // getQuitKeyStrings

string cxWindow::getQuitKeyListString() const
{
   string quitKeyList;

   for (const auto mapPair : mQuitKeys )
   {
      if (quitKeyList != "") { quitKeyList += ","; }
      quitKeyList += cxBase::getKeyStr(mapPair.first);
   }

   return(quitKeyList);
} // getQuitKeyListString

void cxWindow::getExitKeyStrings(vector<string>& pKeys) const
{
   pKeys.clear();

   for (const auto& exitKeyPair : mExitKeys)
   {
      pKeys.push_back(cxBase::getKeyStr(exitKeyPair.first));
   }
} // getExitKeyStrings

string cxWindow::getExitKeyListString() const
{
   string exitKeyList;

   for (const auto& exitKeyPair : mExitKeys)
   {
      if (exitKeyList != "") { exitKeyList += ","; }
      exitKeyList += cxBase::getKeyStr(exitKeyPair.first);
   }

   return(exitKeyList);
} // getExitKeyListString

map<int, bool> cxWindow::getQuitKeys() const
{
   return(mQuitKeys);
} // getQuitKeys

map<int, bool> cxWindow::getExitKeys() const
{
   return(mExitKeys);
} // getExitKeys

void cxWindow::toggleMessage(bool pDrawMessage)
{
   mDrawMessage = pDrawMessage;
} // toggleMessage

void cxWindow::toggleTitle(bool pDrawTitle)
{
   mDrawTitle = pDrawTitle;
} // toggleTitle

void cxWindow::toggleStatus(bool pDrawStatus)
{
   mDrawStatus = pDrawStatus;
} // toggleStatus

bool cxWindow::messageWillDraw() const
{
   return(mDrawMessage);
} // messageWillDraw

bool cxWindow::titleWillDraw() const
{
   return(mDrawTitle);
} // titleWillDraw

bool cxWindow::statusWillDraw() const
{
   return(mDrawStatus);
} // statusWillDraw

void cxWindow::toggleSpecialChars(bool pDrawSpecialChars)
{
   mDrawSpecialChars = pDrawSpecialChars;
} // toggleSpecialChars

bool cxWindow::specialCharsWillDraw() const
{
   return(mDrawSpecialChars);
} // specialCharsWillDraw

void cxWindow::drawTitle()
{
   // If an external window is being used
   //  for the title, then refresh it.
   //  Otherwise, draw the title in the
   //  top border.
   if (mExtTitleWindow != nullptr)
   {
      mExtTitleWindow->show();
   }
   else
   {
      // This only makes sense to do if mWindow is not nullptr and
      //  the window has a border.
      if ((mWindow != nullptr) && hasBorder())
      {
         writeBorderStrings(mTitleStrings, 0, eTITLE, mTitleColorPair);
      }
   }
} // drawTitle

// Writes the status line
void cxWindow::drawStatus()
{
   // If an external window is being used
   //  for the status, then refresh it.
   //  Otherwise, draw the status in the
   //  bottom border.
   if (mExtStatusWindow != nullptr)
   {
      mExtStatusWindow->show();
   }
   else
   {
      // This only makes sense to do if mWindow is not nullptr and
      //  the window has a border.
      if ((mWindow != nullptr) && hasBorder())
      {
         writeBorderStrings(mStatusStrings, height()-1, eSTATUS, mStatusColorPair);
      }
   }
} // drawStatus

// Outputs the lines in mMessageLines
void cxWindow::drawMessage()
{
   // Enable the message attributes and message color.
   enableAttrs(mWindow, eMESSAGE);
   if (useColors)
   {
      wcolor_set(mWindow, mMessageColorPair, nullptr);
   }
   if (height() > 0)
   {
      // Figure out the width of the message area
      int innerWidth = width();
      int innerHeight = height();
      if (hasBorder())
      {
         innerWidth -= 2;
         innerHeight -= 2;
      }

      // Start at row 0 & column 0 if there are no borders, or row 1 & column
      //  1 if there are borders.
      int currentRowInWindow = 0;
      int leftmostCol = 0;
      if (hasBorder())
      {
         leftmostCol = 1;
         currentRowInWindow = 1;
      }
      // TODO: not sure if need this or not
      // wmove(mWindow, currentRowInWindow, leftmostCol);

      // Adjust starting row based on vertical alignment
      int numMessageLines = (int)mMessageLines.size();
      switch (mVerticalMessageAlignment)
      {
         case eVP_CENTER:
            currentRowInWindow += (innerHeight - numMessageLines) / 2;
            break;
         case eVP_BOTTOM:
            currentRowInWindow += (innerHeight - numMessageLines);
            break;
         case eVP_TOP:
         default:
            break;
      }
      if (currentRowInWindow < (hasBorder() ? 1 : 0))
      {
         currentRowInWindow = (hasBorder() ? 1 : 0);
      }

      wmove(mWindow, currentRowInWindow, leftmostCol);

   // Don't display more rows than the window can hold.
   int rowLimit = height();
   if (hasBorder())
   {
      --rowLimit;
   }

   for (const string& msgLine : mMessageLines)
   {
      if (currentRowInWindow < rowLimit)
      {
         // Figure out the horizontal starting position (based
         //  on horizontal alignment)
         int currentStartX = leftmostCol;
         switch(mHorizMessageAlignment)
         {
            case eHP_RIGHT:
               currentStartX = leftmostCol + innerWidth - (int)msgLine.length();
               break;
            case eHP_CENTER:
               currentStartX = leftmostCol + (innerWidth / 2) - ((int)msgLine.length() / 2);
               break;
            case eHP_LEFT:
            default:
               currentStartX = leftmostCol;
               break;
         }
         if (currentStartX < leftmostCol) currentStartX = leftmostCol;

         // If we're to use underlines when writing the message, then
         //  use writeWithHighlighting().  Otherwise, write the message
         //  line verbatim.
         if (mHotkeyHighlighting)
         {
            writeWithHighlighting(mWindow, msgLine, currentRowInWindow, currentStartX,
                                innerWidth);
            // Fill in the rest of the line with spaces
            int visualLen = (int)cxBase::visualStrLen(msgLine);
            for (int i = currentStartX + visualLen; i < leftmostCol + innerWidth; ++i)
            {
               mvwaddch(mWindow, currentRowInWindow, i, ' ');
            }
         }
         else
         {
            // Clear the line first to ensure background color is consistent
            wmove(mWindow, currentRowInWindow, leftmostCol);
            for(int i=0; i<innerWidth; ++i) waddch(mWindow, ' ');
            mvwaddstr(mWindow, currentRowInWindow, currentStartX, msgLine.c_str());
         }

         ++currentRowInWindow;
      }
   }
   // Fill in the remainder of the text area with spaces
   //  (to fill in the background color)
   if (hasBorder())
   {
      rowLimit = height() - 1;
   }
   else
   {
      rowLimit = height();
   }
      for ( ; currentRowInWindow < rowLimit; ++currentRowInWindow)
      {
         std::ostringstream os;
         if (!hasBorder())
         {
            os << "%-" << width() << "s";
            mvwprintw(mWindow, currentRowInWindow, 0, (char*)os.str().c_str(), " ");
         }
         else
         {
            os << "%-" << width()-2 << "s";
            mvwprintw(mWindow, currentRowInWindow, 1, (char*)os.str().c_str(), " ");
         }
      }
   }

   // Disable the message attributes and color.
   disableAttrs(mWindow, eMESSAGE);
   if (useColors)
   {
      wcolor_set(mWindow, 0, nullptr);
   }
} // drawMessage

void cxWindow::drawSpecialChars()
{
   if (mSpecialChars.size() > 0)
   {
      // Enable the message color
      if (useColors)
      {
         wcolor_set(mWindow, mMessageColorPair, nullptr);
      }

      // Write the special characters
      pair<int, int> location = make_pair(0, 0);
      attr_t attribute = A_NORMAL;
      for (const auto& charPair : mSpecialChars)
      {
         location = charPair.first;
         // Set the attribute to use.  There should be an attribute in
         //  mSpecialCharAttrs for all the characters in mSpecialChars, but
         //  check for it anyway just to be sure.
         if (mSpecialCharAttrs.find(location) != mSpecialCharAttrs.end())
         {
            attribute = mSpecialCharAttrs[location];
         }
         else
         {
            attribute = A_NORMAL;
         }

         // Enable the attribute, write the character, then disable the attribute.
         wattron(mWindow, attribute);
         mvwaddch(mWindow, location.first, location.second, charPair.second);
         wattroff(mWindow, attribute);
      }

      // Disable the message color
      if (useColors)
      {
         wcolor_set(mWindow, 0, nullptr);
      }
   }
} // drawSpecialChars

void cxWindow::setReturnCode(const long& pReturnCode)
{
   mReturnCode = pReturnCode;
} // setReturnCode

void cxWindow::setMessageColor(e_cxColors pColor)
{
   // Setting a color could require a bold attribute..   setElementColor
   //  will set the correct attribute for the color.
   attr_t tmpAttr = 0;
   setElementColor(mMessageColorPair, tmpAttr, pColor);
   // Add the attribute to the message attribute set
   addAttr(eMESSAGE, tmpAttr);
} // setMessageColor

void cxWindow::setTitleColor(e_cxColors pColor)
{
   // Setting a color could require a bold attribute..   setElementColor
   //  will set the correct attribute for the color.
   attr_t tmpAttr = 0;
   setElementColor(mTitleColorPair, tmpAttr, pColor);
   // Add the attribute to the title attribute set
   addAttr(eTITLE, tmpAttr);
} // setTitleColor

void cxWindow::setStatusColor(e_cxColors pColor)
{
   // Setting a color could require a bold attribute..   setElementColor
   //  will set the correct attribute for the color.
   attr_t tmpAttr = 0;
   setElementColor(mStatusColorPair, tmpAttr, pColor);
   // Add the attribute to the status attribute set
   cxBase::addAttr(eSTATUS, tmpAttr);
} // setStatusColor

void cxWindow::setBorderColor(e_cxColors pColor)
{
   // Setting a color could require a bold attribute..   setElementColor
   //  will set the correct attribute for the color.
   attr_t tmpAttr = 0;
   setElementColor(mBorderColorPair, tmpAttr, pColor);
   // Set the attribute in the border attribute set
   setAttr(eBORDER, tmpAttr);
} // getBorderColor

void cxWindow::setColor(e_WidgetItems pItem, e_cxColors pColor)
{
   switch(pItem)
   {
      case eMESSAGE:        // Message
         setMessageColor(pColor);
         break;
      case eTITLE:          // Title
         setTitleColor(pColor);
         break;
      case eSTATUS:         // Status
         setStatusColor(pColor);
         break;
      case eBORDER:         // Borders
         setBorderColor(pColor);
         break;
      case eHOTKEY:         // Hotkey
         // There is no setHotkeyColor()..
         break;
      // The next 4 cases don't apply to cxWindow, but they're here so that
      //  the compiler doesn't complain.
      case eMENU_SELECTION:
      case eLABEL:
      case eDATA:
      case eDATA_READONLY:
      case eDATA_EDITABLE:
         break;
   }
} // setColor

bool cxWindow::hasAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // Get the attributes
   set<attr_t> attrs;
   getAttrs(pItem, attrs);
   // Return whether or not the attribute exists
   return(attrs.find(pAttr) != attrs.end());
} // hasAttr

string cxWindow::cxTypeStr() const
{
   return("cxWindow");
} // cxTypeStr

bool cxWindow::getLeaveNow() const
{
   return(mLeaveNow);
} // getLeaveNow

string cxWindow::getName() const
{
   return(mName);
} // getName

void cxWindow::setName(const string& pName)
{
   mName = pName;
} // setName

bool cxWindow::mouseEvtWasInWindow() const
{
#ifdef NCURSES_MOUSE_VERSION
   return(mouseEvtInRect(mMouse, top(), left(), bottom(), right()));
#else
   return(false);
#endif
} // mouseEvtInWindow

bool cxWindow::mouseEvtWasInTitle() const
{
   bool inTitle = false;
#ifdef NCURSES_MOUSE_VERSION
   // Note: Can only click in the title if there *is* a border
   if (hasBorder())
   {
      inTitle = mouseEvtInRect(mMouse, top(), left(), top(), right());
   }
#endif
   return(inTitle);
} // mouseEvtInTitle

void cxWindow::doMouseBehavior()
{
} // doMouseBehavior

shared_ptr<cxFunction> cxWindow::getKeyFunction(int pKey) const
{
   map<int, shared_ptr<cxFunction> >::const_iterator iter = mKeyFunctions.find(pKey);
   if (iter != mKeyFunctions.end())
   {
      return(iter->second);
   }
   else
   {
      return(nullptr);
   }
}

shared_ptr<cxFunction0> cxWindow::getKeyFunctionAsFunction0(int pKey) const
{
   shared_ptr<cxFunction0> funcToReturn = nullptr;
   shared_ptr<cxFunction> funcPtr = getKeyFunction(pKey);
   if (funcPtr != nullptr)
   {
      if (funcPtr->cxTypeStr() == "cxFunction0")
      {
         try
         {
            funcToReturn = dynamic_pointer_cast<cxFunction0>(funcPtr);
         }
         catch (...)
         {
         }
      }
   }
   return funcToReturn;
} // getKeyFunction2

std::shared_ptr<cxFunction2> cxWindow::getKeyFunctionAsFunction2(int pKey) const
{
   shared_ptr<cxFunction2> funcToReturn = nullptr;
   shared_ptr<cxFunction> funcPtr = getKeyFunction(pKey);
   if (funcPtr != nullptr)
   {
      if (funcPtr->cxTypeStr() == "cxFunction2")
      {
         try
         {
            funcToReturn = dynamic_pointer_cast<cxFunction2>(funcPtr);
         }
         catch (...)
         {
         }
      }
   }
   return funcToReturn;
} // getKeyFunction2

std::shared_ptr<cxFunction4> cxWindow::getKeyFunctionAsFunction4(int pKey) const
{
   shared_ptr<cxFunction4> funcToReturn = nullptr;
   shared_ptr<cxFunction> funcPtr = getKeyFunction(pKey);
   if (funcPtr != nullptr)
   {
      if (funcPtr->cxTypeStr() == "cxFunction4")
      {
         try
         {
            funcToReturn = dynamic_pointer_cast<cxFunction4>(funcPtr);
         }
         catch (...)
         {
         }
      }
   }
   return funcToReturn;
} // getKeyFunction4

bool cxWindow::mouseButton1Pressed() const
{
   bool retval = false;

#ifdef NCURSES_MOUSE_VERSION
   retval = (mMouse.bstate == BUTTON1_PRESSED);
#endif

   return(retval);
} // mouseButton1Pressed

bool cxWindow::mouseButton1Released() const
{
   bool retval = false;

#ifdef NCURSES_MOUSE_VERSION
   retval = (mMouse.bstate == BUTTON1_RELEASED);
#endif

   return(retval);
} // mouseButton1Released

bool cxWindow::mouseButton1Clicked() const
{
   bool retval = false;

#ifdef NCURSES_MOUSE_VERSION
   retval = (mMouse.bstate == BUTTON1_CLICKED);
#endif

   return(retval);
} // mouseButton1Clicked

bool cxWindow::mouseButton1DoubleClicked() const
{
   bool retval = false;

#ifdef NCURSES_MOUSE_VERSION
   retval = (mMouse.bstate == BUTTON1_DOUBLE_CLICKED);
#endif

   return(retval);
} // mouseButton1DoubleClicked

bool cxWindow::mouseButton2Clicked() const
{
   bool retval = false;

#ifdef NCURSES_MOUSE_VERSION
   retval = (mMouse.bstate == BUTTON2_CLICKED);
#endif

   return(retval);
} // mouseButton2Clicked

bool cxWindow::mouseButton2DoubleClicked() const
{
   bool retval = false;

#ifdef NCURSES_MOUSE_VERSION
   retval = (mMouse.bstate == BUTTON2_DOUBLE_CLICKED);
#endif

   return(retval);
} // mouseButton2DoubleClicked

bool cxWindow::mouseButton3Clicked() const
{
   bool retval = false;

#ifdef NCURSES_MOUSE_VERSION
   retval = (mMouse.bstate == BUTTON3_CLICKED);
#endif

   return(retval);
} // mouseButton3Clicked

void cxWindow::mouseButton1Click(int pY, int pX)
{
#ifdef NCURSES_MOUSE_VERSION
   mMouse.y = pY;
   mMouse.x = pX;
   mMouse.bstate = BUTTON1_CLICKED;
#endif
} // mouseButton1Click

void cxWindow::mouseButton2Click(int pY, int pX)
{
#ifdef NCURSES_MOUSE_VERSION
   mMouse.y = pY;
   mMouse.x = pX;
   mMouse.bstate = BUTTON2_CLICKED;
#endif
} // mouseButton2Click

void cxWindow::mouseButton3Click(int pY, int pX)
{
#ifdef NCURSES_MOUSE_VERSION
   mMouse.y = pY;
   mMouse.x = pX;
   mMouse.bstate = BUTTON3_CLICKED;
#endif
} // mouseButton3Click

#ifdef NCURSES_MOUSE_VERSION
MEVENT cxWindow::getMouseEvent() const
{
   return(mMouse);
} // getMouseEvent
#endif

string cxWindow::getString(int pRow, int pCol, int pNumber)
{
   return(cxBase::getString(pRow, pCol, pNumber, mWindow));
} // getString

void cxWindow::info()
{
   cxWindow iWindow(nullptr, 0, 0, 20, 30, "Info", "", "Press any key");
   iWindow.addMessageLineBelow("Entire Screen:");
   iWindow.addMessageLineBelow("top():" + toString(cxBase::top()) + ":");
   iWindow.addMessageLineBelow("left():" + toString(cxBase::left()) + ":");
   iWindow.addMessageLineBelow("right():" + toString(cxBase::right()) + ":");
   iWindow.addMessageLineBelow("bottom():" + toString(cxBase::bottom()) + ":");
   iWindow.addMessageLineBelow("height():" + toString(cxBase::height()) + ":");
   iWindow.addMessageLineBelow("width():" + toString(cxBase::width()) + ":");
   iWindow.addMessageLineBelow("");
   iWindow.addMessageLineBelow("This window:");
   iWindow.addMessageLineBelow("top():" + toString(top()) + ":");
   iWindow.addMessageLineBelow("left():" + toString(left()) + ":");
   iWindow.addMessageLineBelow("right():" + toString(right()) + ":");
   iWindow.addMessageLineBelow("bottom():" + toString(bottom()) + ":");
   iWindow.addMessageLineBelow("height():" + toString(height()) + ":");
   iWindow.addMessageLineBelow("width():" + toString(width()) + ":");
   iWindow.addMessageLineBelow("");
   iWindow.addMessageLineBelow("hasBorder():" + string(hasBorder() ? "true" : "false") + ":");
   iWindow.center();
   iWindow.showModal(true, true, false);
   iWindow.hide();
} // info

void cxWindow::toggleBorderTop(bool pToggle)
{
   mDrawBorderTop = pToggle;
} // toggleBorderTop

void cxWindow::toggleBorderBottom(bool pToggle)
{
   mDrawBorderBottom = pToggle;
} // toggleBorderBottom

void cxWindow::toggleBorderLeft(bool pToggle)
{
   mDrawBorderLeft = pToggle;
} // toggleBorderLeft

void cxWindow::toggleBorderRight(bool pToggle)
{
   mDrawBorderRight = pToggle;
} // toggleBorderRight

void cxWindow::addSpecialChar(int pY, int pX, chtype pChar,
                              attr_t pAttr)
                              {
   pair<int, int> location = make_pair(pY, pX);
   mSpecialChars[location] = pChar;
   mSpecialCharAttrs[location] = pAttr;
} // addSpecialChar

void cxWindow::removeSpecialChar(int pY, int pX)
{
   pair<int, int> location = make_pair(pY, pX);
   mSpecialChars.erase(location);
   mSpecialCharAttrs.erase(location);
} // removeSpecialChar

void cxWindow::clearSpecialChars()
{
   mSpecialChars.clear();
   mSpecialCharAttrs.clear();
} // clearSpecialChars

bool cxWindow::functionExistsForLastMouseState() const
{
#ifdef NCURSES_MOUSE_VERSION
   return(mMouseFunctions.find(mMouse.bstate) != mMouseFunctions.end());
#else
   return(false);
#endif
} // functionExistsForLastMouseState

int cxWindow::getMouseState() const
{
#ifdef NCURSES_MOUSE_VERSION
   return(mMouseFunctions.find(mMouse.bstate) != mMouseFunctions.end());
#else
   return(false);
#endif
} // getMouseState

string cxWindow::getMouseStateStr() const
{
#ifdef NCURSES_MOUSE_VERSION
   return(cxBase::getMouseStateStr(mMouse.bstate));
#else
   return("");
#endif
} // getMouseStateStr

void cxWindow::doMouseBehavior(int pMouseY, int pMouseX)
{
   // Set mMouse's X and Y location, and then call the other doMouseBehavior().
#ifdef NCURSES_MOUSE_VERSION
   mMouse.y = pMouseY;
   mMouse.x = pMouseX;
#endif
   doMouseBehavior();
} // doMouseBehavior

e_cxColors cxWindow::getItemColor(e_WidgetItems pItem) const
{
   // Default to something if pItem is not something used by cxWindow..
   if ((pItem == eMENU_SELECTION) || (pItem == eLABEL) ||
       (pItem == eDATA_READONLY) || (pItem == eDATA_EDITABLE))
       {
      return(eDEFAULT);
   }

   // Figure out which member color pair & attribute set to check
   short colorPair = 0;
   set<attr_t> attrSet;
   switch(pItem)
   {
      case eMESSAGE:        // Message
         colorPair = mMessageColorPair;
         attrSet = mMessageAttrs;
         break;
      case eTITLE:          // Title
         colorPair = mTitleColorPair;
         attrSet = mTitleAttrs;
         break;
      case eSTATUS:         // Status
         colorPair = mStatusColorPair;
         attrSet = mStatusAttrs;
         break;
      case eBORDER:         // Borders
         colorPair = mBorderColorPair;
         attrSet = mBorderAttrs;
         break;
      case eHOTKEY:         // Hotkey
         attrSet = mHotkeyAttrs;
         break;
      // The following aren't handled in cxWindow:
      case eMENU_SELECTION: // Selected menu items
      case eLABEL:          // Input labels
      case eDATA:
      case eDATA_READONLY:  // Read-only data items
      case eDATA_EDITABLE:  // Editable data items
         break;
   }

   // Now, determine the actual color that should be returned.
   e_cxColors color = (e_cxColors)colorPair; // This will be returned
   // If the attribute set contains A_BOLD, then check the color (if brown,
   //  then the actual color is yellow, etc.).
   if (attrSet.find(A_BOLD) != attrSet.end())
   {
      switch(colorPair)
      {
         case eBROWN_BLACK:
            color = eYELLOW_BLACK;
            break;
         case eBROWN_BLUE:
            color = eYELLOW_BLUE;
            break;
         case eGRAY_BLACK:
            color = eWHITE_BLACK;
            break;
         case eGRAY_BLUE:
            color = eWHITE_BLUE;
            break;
      }
   }

   return(color);
} // getItemColor

map<int, shared_ptr<cxFunction> >::iterator cxWindow::keyFunctions_begin()
{
   return(mKeyFunctions.begin());
} // keyFunctions_begin

map<int, shared_ptr<cxFunction> >::iterator cxWindow::keyFunctions_end()
{
   return(mKeyFunctions.end());
} // keyFunctions_end

void cxWindow::getFunctionKeyStrings(vector<string>& pKeys) const
{
   pKeys.clear();

   for (const auto& keyFuncPair : mKeyFunctions)
   {
      pKeys.emplace_back(cxBase::getKeyStr(keyFuncPair.first));
   }
} // getFunctionKeyStrings

void cxWindow::getFunctionKeys(vector<int>& pKeys) const
{
   pKeys.clear();

   for (const auto& keyFuncPair : mKeyFunctions)
   {
      pKeys.emplace_back(keyFuncPair.first);
   }
} // getFunctionKeys

void cxWindow::addAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &mBorderAttrs;
         break;
      // The next 4 cases don't apply to cxWindow, but they're here so that
      //  the compiler doesn't complain.
      case eMENU_SELECTION:
      case eLABEL:
      case eDATA:
      case eDATA_READONLY:
      case eDATA_EDITABLE:
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &mHotkeyAttrs;
         break;
   }

   // Insert the attribute, if attrSet was set.
   if (nullptr != attrSet)
   {
      attrSet->insert(pAttr);
   }
} // addAttr

void cxWindow::setAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &mBorderAttrs;
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &mHotkeyAttrs;
         break;
      // The next 4 cases don't apply to cxWindow, but they're here so that
      //  the compiler doesn't complain.
      case eMENU_SELECTION:
      case eLABEL:
      case eDATA:
      case eDATA_READONLY:
      case eDATA_EDITABLE:
         break;
   }

   // Set the attribute, if attrSet was set.
   if (nullptr != attrSet)
   {
      attrSet->clear();
      attrSet->insert(pAttr);
   }
} // setAttr

void cxWindow::removeAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &mBorderAttrs;
         break;
      // The next 4 cases don't apply to cxWindow, but they're here so that
      //  the compiler doesn't complain.
      case eMENU_SELECTION:
      case eLABEL:
      case eDATA:
      case eDATA_READONLY:
      case eDATA_EDITABLE:
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &mHotkeyAttrs;
         break;
   }

   // Remove the attribute, if attrSet was set.
   if (nullptr != attrSet)
   {
      attrSet->erase(pAttr);
   }
} // removeAttr

void cxWindow::removeAttrs(e_WidgetItems pItem)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &mBorderAttrs;
         break;
      // The next 4 cases don't apply to cxWindow, but they're here so that
      //  the compiler doesn't complain.
      case eMENU_SELECTION:
      case eLABEL:
      case eDATA:
      case eDATA_READONLY:
      case eDATA_EDITABLE:
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &mHotkeyAttrs;
         break;
   }

   // Remove the attributes, if attrSet was set.
   if (nullptr != attrSet)
   {
      attrSet->clear();
   }
} // removeAttrs

void cxWindow::getAttrs(e_WidgetItems pItem, set<attr_t>& pAttrs) const
{
   pAttrs.clear();

   switch(pItem)
   {
      case eMESSAGE:        // Message
         pAttrs = mMessageAttrs;
         break;
      case eTITLE:          // Title
         pAttrs = mTitleAttrs;
         break;
      case eSTATUS:         // Status
         pAttrs = mStatusAttrs;
         break;
      case eBORDER:         // Borders
         pAttrs = mBorderAttrs;
         break;
      // The next 4 cases don't apply to cxWindow, but they're here so that
      //  the compiler doesn't complain.
      case eMENU_SELECTION:
      case eLABEL:
      case eDATA:
      case eDATA_READONLY:
      case eDATA_EDITABLE:
         break;
      case eHOTKEY:         // Hotkey
         pAttrs = mHotkeyAttrs;
         break;
   }
} // getAttrs

// Changes the window's height & width.
//  (The window will stay at the same starting
//  row & col.)
void cxWindow::resize(int pNewHeight, int pNewWidth, bool pRefresh)
{
   if ((pNewHeight != height()) || (pNewWidth != width()))
   {
      int col = left();
      int row = top();

      // Make sure the window won't get resized too small or too big.
      const int maxHeight = cxBase::height() - row;
      const int maxWidth = cxBase::width() - col;
      if (pNewHeight <= 0)
      {
         pNewHeight = 1; // Minimal, but init() will probably change this..
      }
      else if (pNewHeight > maxHeight)
      {
         pNewHeight = maxHeight;
      }
      if (pNewWidth <= 0)
      {
         pNewWidth = 1; // Minimal, but init() will probably change this..
      }
      else if (pNewWidth > maxWidth)
      {
         pNewWidth = maxWidth;
      }

      // Re-combine the window message into a single line (for calling init() again)
      string message;
      combineMessageLines(message);
      init(row, col, pNewHeight, pNewWidth, getTitle(), message, getStatus(),
           mParentWindow);

      // Refresh the window
      if (pRefresh)
      {
         show(false, false);
      }
   }
} // resize

bool cxWindow::move(int pNewRow, int pNewCol, bool pRefresh)
{
   // Move the panel and store the return value
   int moveReturnCode = move_panel(mPanel, pNewRow, pNewCol);
   bool windowMoved = (moveReturnCode != ERR);

   if (windowMoved)
   {
      if (pRefresh)
      {
         show(false, false);
      }
   }

   return (windowMoved);
} // move

bool cxWindow::moveRelative(int pVerticalOffset, int pHorizOffset, bool pRefresh)
{
   bool moved = false;

   if ((pVerticalOffset != 0) || (pHorizOffset != 0))
   {
      moved = move(top()+pVerticalOffset, left()+pHorizOffset, pRefresh);
   }

   return(moved);
} // moveRelative

unsigned cxWindow::numSubWindows()
{
   return(mSubWindows.size());
} // numSubWindows

void cxWindow::setSubWinMessage(unsigned pIndex, const string& pMessage)
{
   if ((pIndex > 0) && (pIndex < mSubWindows.size()))
   {
      mSubWindows[pIndex]->setMessage(pMessage);
   }
} // setSubWinMessage

void cxWindow::setSubWinMessage(const string& pTitle, const string& pMessage)
{
   for (cxWindow*& subWin : mSubWindows)
   {
      if (subWin->getTitle() == pTitle)
      {
         subWin->setMessage(pMessage);
         break;
      }
   }
} // setSubWinMessage

// Initializes the window parameters and mWindow
void cxWindow::init(int pRow, int pCol, int pHeight, int pWidth,
                    const string& pTitle, const string& pMessage,
                    const string& pStatus, cxWindow *pParentWindow,
                    bool pResizeVertically)
                    {
   // If pHeight and pWidth are <= 0, we'll try to calculate them below
   // unless they remain <= 0 after calculation.
   int newWidth = pWidth;   // if we have to override the width
   int newHeight = pHeight; // if we have to override the height
   const int maxHeight = (int)(cxBase::height()) - pRow;
   const int maxWidth = (int)(cxBase::width()) - pCol;

   // Split message on newlines (\n) (if it has newlines,
   // the message won't display correctly)
   //string message = cxStringUtils::stripNewlines(pMessage);
   string message = pMessage;
   // Find the length of the longest line in the message
   int maxMsgLineLen = 0;
   vector<string> initialLines;
   cxStringUtils::SplitStringRegex(message, "\n", initialLines);
   for (const string& line : initialLines)
   {
      int currentLineLen = (mHotkeyHighlighting ? cxBase::visualStrLen(line) : line.length());
      if (currentLineLen > maxMsgLineLen)
      {
         maxMsgLineLen = currentLineLen;
      }
   }

   if (pWidth <= 0)
   {
      newWidth = maxMsgLineLen + (hasBorder() ? 2 : 0);
      if (newWidth > maxWidth)
      {
         newWidth = maxWidth;
      }
   }
   // If pWidth is greater than newWidth, use pWidth instead.
   if (pWidth > 0 && pWidth > newWidth)
   {
      newWidth = pWidth;
   }

   // we might have to override the width of the window
   // if the title+2 is longer than the message
   // (2 for the borders)
   string title = pTitle;
   string status = pStatus;
   if (hasBorder())
   {
      if (((int)(title.length())+2) <= maxWidth)
      {
         if (((int)(title.length())+2) > newWidth)
         {
            // Yeah, we fit! Override the width
            newWidth = title.length()+2;
         }
      }
      else
      {
         // Crap. Need to shorten the title
         title = title.substr(0,newWidth-2);
      }

      // Widen the window (if needed) based on
      // the status text
      if (status.length() > title.length())
      {
         if (((int)(status.length())+2) <= maxWidth)
         {
            if (((int)(status.length())+2) > newWidth)
            {
               // Yeah, we fit! Override the width
               newWidth = status.length()+2;
            }
         }
         else
         {
            // Crap. Need to shorten the title
            status = status.substr(0,newWidth-2);
         }
      }

      // Make sure the width is at least 2 (for borders).
      if (newWidth < 2)
      {
        newWidth = 2;
      }

      // Make sure the height is at least 2 (for the borders
      //  - it will grow if the message text is non-blank).
      if (newHeight < 2)
      {
         newHeight = 2;
      }
   }

   // Make sure newWidth and newHeight aren't too big.
   if (newWidth > maxWidth)
   {
      newWidth = maxWidth;
   }
   if (newHeight > maxHeight)
   {
      newHeight = maxHeight;
   }

   mMessageLines.clear();
   // Calculate the height based on the message, and also
   // find where we need to split the message based on
   // the width of the window.
   if (message != "")
   {
      int innerWidth; // Inner window width
      if (hasBorder())
      {
         innerWidth = newWidth - 2;
      }
      else
      {
         innerWidth = newWidth;
      }

      // Split message on newlines (\n)
      unsigned stringLen = 0;
      vector<string> iMessageLines;
      cxStringUtils::SplitStringRegex(message, "\n", iMessageLines);
      if (iMessageLines.empty() && !message.empty())
      {
         iMessageLines.push_back(message);
      }
      for (const string& msgLine : iMessageLines)
      {
         // If the current line is short enough to fit inside the window by
         // itself, then just add it to mMessageLines.  Otherwise, split the
         // message line on spaces and add lines to mMessageLines long enough
         // to fit in the window.
         // If mHotkeyHighlighting is true, don't count message underline
         // characters as part of the string length.
         if (mHotkeyHighlighting)
         {
            stringLen = cxBase::visualStrLen(msgLine);
         }
         else
         {
            stringLen = msgLine.length();
         }
         if (stringLen <= (unsigned)innerWidth)
         {
            mMessageLines.push_back(msgLine);
         }
         else
         {
            string currentLine;  // Will be added to mMessageLines
            vector<string> words;
            cxStringUtils::SplitStringRegex(msgLine, " ", words);
            for (string& word : words)
            {
               // If the current word is >= the inner width, set the
               // window width to the width of the current word (if
               // possible..  if not, shorten the word).
               if (mHotkeyHighlighting)
               {
                  stringLen = cxBase::visualStrLen(word);
               }
               else
               {
                  stringLen = word.length();
               }
               if (stringLen >= (unsigned)innerWidth)
               {
                  // The current word will have to go on a line by
                  // itself..  So add the current line to
                  // mMessageLines (if it's not blank).
                  if (currentLine != "")
                  {
                     if (currentLine[currentLine.length()-1] == ' ')
                     {
                        currentLine = currentLine.substr(0, currentLine.length()-1);
                     }
                     mMessageLines.push_back(currentLine);
                     currentLine = "";
                  }

                  // Check the length of the current word against
                  // the inner width.
                  if (pResizeVertically)
                  {
                     if (hasBorder())
                     {
                        if ((int)(word.length()) <= maxWidth-2)
                        {
                           innerWidth = (int)(word.length());
                           newWidth = innerWidth + 2;
                        }
                        else
                        {
                           // Truncate the word..  (a better way might
                           // be to split it across 2 lines?)
                           word = word.substr(0, innerWidth);
                        }
                     }
                     else
                     {
                        if ((int)(word.length()) <= maxWidth)
                        {
                           innerWidth = (int)(word.length());
                           newWidth = innerWidth;
                        }
                        else
                        {
                           // Truncate the word..  (a better way might
                           //  be to split it across 2 lines?)
                           word = word.substr(0, innerWidth);
                        }
                     }
                  }
                  else
                  {
                     // If we're not resizing vertically, we probably shouldn't
                     // resize horizontally either.  Truncate the word.
                     word = word.substr(0, innerWidth);
                  }

                  if (word != "")
                  {
                     mMessageLines.push_back(word);
                  }
               }
               else
               {
                  // Add the current word to currentLine.  Update the height
                  //  of the window and update mMessageLines if currentLine
                  //  is long enough to fill the width of the window.
                  if ((int)(currentLine.length() + word.length() + (currentLine.empty() ? 0 : 1)) > innerWidth)
                  {
                     if (currentLine != "")
                     {
                        if (currentLine[currentLine.length()-1] == ' ')
                        {
                           currentLine = currentLine.substr(0, currentLine.length()-1);
                        }
                        mMessageLines.push_back(currentLine);
                     }
                     currentLine = word + " ";
                  }
                  else
                  {
                     if (!currentLine.empty() && currentLine.back() != ' ')
                     {
                        currentLine += " ";
                     }
                     currentLine += word + " ";
                  }
               }
            }

            // We may need to add the current line, if
            //  we didn't add it in the above loop.
            if (currentLine != "")
            {
               if (currentLine[currentLine.length()-1] == ' ')
               {
                  currentLine = currentLine.substr(0, currentLine.length()-1);
               }
               mMessageLines.push_back(currentLine);
            }
         }
      }
   }

   // Make sure the height of this window is enough for the
   //  message text, if pResizeVertically is true.
   if (pResizeVertically)
   {
      int minHeight = (int)mMessageLines.size();
      if (hasBorder())
      {
         minHeight += 2;
      }
      if (newHeight < minHeight)
      {
         newHeight = minHeight;
      }
   }
   else if (pHeight <= 0)
   {
      // If pHeight was 0 (auto-size), but pResizeVertically is false,
      // we still need a height based on the message.
      newHeight = (int)mMessageLines.size() + (hasBorder() ? 2 : 0);
   }

   // if we have something realistic to display...
   if ((newWidth >= 1) && (newHeight >= 1))
   {
      setMessageColor(cxBase::getDefaultMessageColor());
      setTitleColor(cxBase::getDefaultTitleColor());
      setStatusColor(cxBase::getDefaultStatusColor());
      setBorderColor(cxBase::getDefaultBorderColor());

      // Free the memory used by mWindow and mPanel (just in case), and
      //  (re-)create them.
      freeWindow();
      mWindow = newwin(newHeight, newWidth, pRow, pCol);
      // If mWindow is nullptr, that means newwin() had an error..
      if (mWindow == nullptr)
      {
         throw(cxWidgetsException("Couldn't create a new ncurses window (constructing a new cxWindow)."));
      }
      reCreatePanel();
      keypad(mWindow, TRUE); // for each newwin() we have to set this...
      hide();

      // Add this window to the parent window, if it's not nullptr
      addToParentWindow(pParentWindow);
   }
   else
   {
      throw(cxWidgetsException("Warning: There was no width OR no height to a window. [in cxWindow::init(row:" +
            cxStringUtils::toString(pRow) + " pCol:" + cxStringUtils::toString(pCol) + " pHeight:" +
            cxStringUtils::toString(pHeight) + " pWidth:" + cxStringUtils::toString(pWidth) +
            " pTitle:" + pTitle + ": pMessage:" + pMessage + ": pStatus:" + pStatus + ":) newWidth:" +
            cxStringUtils::toString(newWidth) + " newHeight:" + cxStringUtils::toString(newHeight) + " ]"));
   }

   // Set the title & status
   setTitle(title, false);
   setStatus(status, false);
} // init

//// Helper functions

// Writes text at a row & column
void cxWindow::drawLabel(int pRow, int pCol, const string& pLabel)
{
   if (pLabel != "")
   {
      mvwaddstr(mWindow, pRow, pCol, pLabel.c_str());
   }
} // drawLabel

// Removes all sub window pointers
void cxWindow::removeAllSubwindows()
{
   // Set all the subwindows' parent windows to nullptr
   //  (so that they don't try to do something with
   //  this window anymore).
   while (anySubwinHasThisParent())
   {
      for (cxWindow*& subWin : mSubWindows)
      {
         if (subWin->mParentWindow == this)
         {
            subWin->setParent(nullptr);
            //subWin->mParentWindow = nullptr;
            break; // Exit the for loop; continue with the while loop
         }
      }
   }
   mSubWindows.clear();
} // removeAllSubwindows

void cxWindow::addSubwindow(cxWindow *pSubWindow)
{
   // Only add the subwindow if it doesn't already exist
   //  in mSubWindows.
   if (!subWindowExists(pSubWindow))
   {
      mSubWindows.push_back(pSubWindow);
   }
} // addSubwindow

void cxWindow::removeSubWindow(const cxWindow *pSubWindow)
{
   // Erase all instances of the subwindow pointer from mSubWindows. (A
   //  subwindow should only be in there once though.)
   cxWindowPtrContainer::iterator iter =
             find(mSubWindows.begin(), mSubWindows.end(), pSubWindow);
   while (iter != mSubWindows.end())
   {
      if ((*iter)->mParentWindow == this)
      {
         (*iter)->mParentWindow = nullptr;
      }
      mSubWindows.erase(iter);
      iter = find(mSubWindows.begin(), mSubWindows.end(), pSubWindow);
   }
} // removeSubWindow

// Sets the color (and attribute, if neccessary) of a
//  color element.
void cxWindow::setElementColor(short& pColorPair, attr_t& pElementAttr, e_cxColors pColor)
{
   pColorPair = pColor;

   // If the color's brightness bit is set, then the attribute needs to be bold.
   if (cxBase::colorIsBright(pColor))
   {
      pElementAttr = A_BOLD;
   }
   else
   {
      pElementAttr = A_NORMAL;
   }
} // setElementColor

void cxWindow::enableAttrs(WINDOW *pWin, e_WidgetItems pItem)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &mBorderAttrs;
         break;
      // The next 4 cases don't apply to cxWindow, but they're here so that
      //  the compiler doesn't complain.
      case eMENU_SELECTION:
      case eLABEL:
      case eDATA:
      case eDATA_READONLY:
      case eDATA_EDITABLE:
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &mHotkeyAttrs;
         break;
   }

   // Enable the attributes, if attrSet was set.
   if (nullptr != attrSet)
   {
      // If attrSet has at least 1 attribute, enable them; otherwise, enable
      //  the cxBase attributes.
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
} // enableAttrs

void cxWindow::disableAttrs(WINDOW *pWin, e_WidgetItems pItem)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &mBorderAttrs;
         break;
      // The next 4 cases don't apply to cxWindow, but they're here so that
      //  the compiler doesn't complain.
      case eMENU_SELECTION:
      case eLABEL:
      case eDATA:
      case eDATA_READONLY:
      case eDATA_EDITABLE:
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &mHotkeyAttrs;
         break;
   }

   // Disable the attributes, if attrSet was set.
   if (nullptr != attrSet)
   {
      // If attrSet has at least 1 attribute, disable them; otherwise, disable
      //  the cxBase attributes.
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
} // disableAttrs

void cxWindow::writeWithHighlighting(WINDOW* pWindow, const string& pText,
                     const int& pY, const int& pStartX, const int& pEndX,
                     int pNormalTextItem)
                     {
   if (mHotkeyHighlighting)
   {
      int textLen = (int)pText.length();
      int xPos = pStartX; // Current horizontal position
      for (int i = 0; i < textLen; ++i)
      {
         // Stop writing text when the horizontal position
         //  reaches the end boundary (pEndX).
         if (xPos == pEndX)
         {
            break;
         }

         // If the current character in pText is a HOTKEY_CHAR
         //  and the next character is not a space, write the
         //  next character with the hotkey attributes.
         if ((pText[i] == HOTKEY_CHAR) && (i+1 < textLen) &&
               (pText[i+1] != ' '))
               {
            ++i;
            // Make the hotkey stand out by applying the hotkey attributes.
            enableAttrs(pWindow, eHOTKEY);
            mvwaddch(pWindow, pY, xPos, pText[i]);
            disableAttrs(pWindow, eHOTKEY);
            // If pNormalTextItem is a valid value, apply it so that the normal
            //  text appears the way they want it to appear.
            if ((pNormalTextItem >= eMESSAGE) && (pNormalTextItem <= eHOTKEY))
            {
               enableAttrs(pWindow, (e_WidgetItems)pNormalTextItem);
            }
         }
         else
         {
            mvwaddch(pWindow, pY, xPos, pText[i]);
         }

         ++xPos; // Update the horizontal position
      }
   }
   else
   {
      int maxStrLength = pEndX - pStartX;
      mvwaddnstr(pWindow, pY, pStartX, pText.c_str(), maxStrLength);
   }
} // writeWithHighlighting

void cxWindow::copyCxWinStuff(const cxWindow* pThatWindow, bool pRecreateWin)
{
   if ((pThatWindow != nullptr) && (pThatWindow != this))
   {
      if (pRecreateWin)
      {
         // This method is needed because we have dynamic memory
         //  (mWindow and mPanel)
         int winHeight = pThatWindow->height();
         int winWidth = pThatWindow->width();
         int winTop = pThatWindow->top();
         int winLeft = pThatWindow->left();
         freeWindow();
         mWindow = newwin(winHeight, winWidth, winTop, winLeft);
         // If mWindow is nullptr, that means newwin() had an error..
         if (mWindow == nullptr)
         {
            throw(cxWidgetsException("Couldn't create a new ncurses window (copying a cxWindow)."));
         }
         reCreatePanel();
         keypad(mWindow, TRUE); // for each newwin() we have to set this...
      }

      mMessageLines = pThatWindow->mMessageLines;
      mSpecialChars = pThatWindow->mSpecialChars;
      mMessageAttrs = pThatWindow->mMessageAttrs;
      mStatusAttrs = pThatWindow->mStatusAttrs;
      mBorderAttrs = pThatWindow->mBorderAttrs;
      mHotkeyAttrs = pThatWindow->mHotkeyAttrs;
      mSpecialCharAttrs = pThatWindow->mSpecialCharAttrs;
      mMessageColorPair = pThatWindow->mMessageColorPair;
      mTitleColorPair = pThatWindow->mTitleColorPair;
      mBorderColorPair = pThatWindow->mBorderColorPair;
      mHorizTitleAlignment = pThatWindow->mHorizTitleAlignment;
      mHorizMessageAlignment = pThatWindow->mHorizMessageAlignment;
      mHorizStatusAlignment = pThatWindow->mHorizStatusAlignment;
      mDrawMessage = pThatWindow->mDrawMessage;
      mDrawSpecialChars = pThatWindow->mDrawSpecialChars;
      mIsModal = false;
      mLeaveNow = false;
      // Copy the key functions
      setKeyFunctions(*pThatWindow);
      // Copy the focus functions
      setFocusFunctions(*pThatWindow);
      mQuitKeys = pThatWindow->mQuitKeys;
      mExitKeys = pThatWindow->mExitKeys;
      mMessageAttrs = pThatWindow->mMessageAttrs;
      mExtTitleWindow = pThatWindow->mExtTitleWindow;
      mExtStatusWindow = pThatWindow->mExtStatusWindow;
      mTitleParent = pThatWindow->mTitleParent;
      mStatusParent = pThatWindow->mStatusParent;
      mExtTitleTemp = pThatWindow->mExtTitleTemp;
      mExtStatusTemp = pThatWindow->mExtStatusTemp;
      // Note: Purposefully not changing mParentWindow.  Otherwise, segfaults
      //  could ensue.
      // Note: Purposefully not copying the other window's
      //  mSubWindows..  Maybe in the future we should add
      //  a feature to let a window have multiple parents?
      mFocus = pThatWindow->mFocus;
      mBorderStyle = pThatWindow->mBorderStyle;
      mHotkeyHighlighting = pThatWindow->mHotkeyHighlighting;
      mMouse = pThatWindow->mMouse;
      mEnabled = pThatWindow->mEnabled;
      mDisableCursorOnShow = pThatWindow->mDisableCursorOnShow;
      mLastKey = pThatWindow->mLastKey;
      mChangeColorsOnFocus = pThatWindow->mChangeColorsOnFocus;
      mShowSubwinsForward = pThatWindow->mShowSubwinsForward;
      mShowSelfBeforeSubwins = pThatWindow->mShowSelfBeforeSubwins;
      mReturnCode = pThatWindow->mReturnCode;
      mRunOnFocus = pThatWindow->mRunOnFocus;
      mRunOnLeave = pThatWindow->mRunOnLeave;
      mTitleStrings = pThatWindow->mTitleStrings;
      mStatusStrings = pThatWindow->mStatusStrings;
      mName = pThatWindow->mName;
      mDrawBorderTop = pThatWindow->mDrawBorderTop;
      mDrawBorderBottom = pThatWindow->mDrawBorderBottom;
      mDrawBorderLeft = pThatWindow->mDrawBorderLeft;
      mDrawBorderRight = pThatWindow->mDrawBorderRight;

      // Copy the other window's mMouse information
#ifdef NCURSES_MOUSE_VERSION
      mMouse.y = pThatWindow->mMouse.y;
      mMouse.x = pThatWindow->mMouse.x;
      mMouse.z = pThatWindow->mMouse.z;
      mMouse.bstate = pThatWindow->mMouse.bstate;
      mMouse.id = pThatWindow->mMouse.id;
#endif

      // If the other window is hidden, hide this one too.
      if (pThatWindow->isHidden())
      {
         hide();
      }
   }
} // copyCxWinStuff

void cxWindow::getRowColBasedOn(cxWindow *pParentWindow, eHPosition pPosition,
                                const string& pTitle, const string& pStatus,
                                const string& pMessage,
                                int& pRow, int& pCol)
                                {
   pRow = 0;
   pCol = 0;
   int width=0;

   // If the window is to be centered, figure out the width based on
   //  the message length and an arbitrary place to split it if it's big.
   if (pPosition == eHP_CENTER)
   {
      const int someWidth = (cxBase::width() >= 30 ? 30 : cxBase::width());
      if (pMessage.length() >= 15)
      {
         if (!hasBorder())
         {
            if ((int)pMessage.length() < someWidth)
            {
               width = pMessage.length();
            }
            else
            {
               width = someWidth;
            }
         }
         else
         {
            if ((int)pMessage.length() < someWidth)
            {
               width = pMessage.length() + 2;
            }
            else
            {
               width = someWidth + 2;
            }
         }
      }
   }
   else
   {
      if (pTitle.length()+2 > pMessage.length())
      {
         width=pTitle.length()+2;
      }
      else
      {
         width=pMessage.length()+2;
      }
   }

   if (pStatus.length() + 2 > (unsigned)width)
   {
      width = pStatus.length() + 2;
   }

   switch (pPosition)
   {
      case eHP_LEFT:
         pCol = 0;
         break;
      case eHP_RIGHT:
         if (pParentWindow != nullptr)
         {
            pCol = pParentWindow->right() - width + 1;
            pCol = pParentWindow->right() - width + 1;
         }
         else
         {
            pCol = cxBase::right() - width + 1;
         }
         break;
      case eHP_CENTER: // This is the default
      default:
         if (pParentWindow != nullptr)
         {
            pCol=((pParentWindow->right() - width) / 2);
         }
         else
         {
            pCol=((cxBase::width() - width) / 2);
         }
         break;
   }

   if (pParentWindow != nullptr)
   {
      pRow=(pParentWindow->centerRow() - 1);
   }
   else
   {
      pRow=(cxBase::centerRow() - 1);
   }

   if (pRow < 0) { pRow=0; }
   if (pCol < 0) { pCol=0; }
} // getRowColBasedOn

bool cxWindow::handleFunctionForLastKey(bool *pFunctionExists,
                                        bool *pRunOnLeaveFunction)
                                        {
   // This function returns whether or not the input loop should continue.
   bool continueOn = true;

   // If pFunctionExists or pRunOnLeaveFunction are not nullptr, then default
   //  them.
   if (pFunctionExists != nullptr)
   {
      *pFunctionExists = false;
   }
   if (pRunOnLeaveFunction != nullptr)
   {
      *pRunOnLeaveFunction = true;
   }

   int lastKey = cxWindow::getLastKey();
   if (mKeyFunctions.find(lastKey) != mKeyFunctions.end())
   {
      shared_ptr<cxFunction> iFunc = mKeyFunctions[lastKey];
      // The cxFunction pointers in mkeyFunctions shouldn't be nullptr, but check
      // just in case.
      if (iFunc != nullptr)
      {
         if (iFunc->functionIsSet())
         {
            iFunc->runFunction();
            continueOn = !(iFunc->getExitAfterRun());

            if (pFunctionExists != nullptr)
            {
               *pFunctionExists = true;
            }
            if (pRunOnLeaveFunction != nullptr)
            {
               *pRunOnLeaveFunction = iFunc->getRunOnLeaveFunction();
            }
         }
      }
   }

   return(continueOn);
} // handleFunctionForLastKey

bool cxWindow::handleFunctionForLastMouseState(bool *pFunctionExists,
                                               bool *pRunOnLeaveFunction)
                                               {
   // This function returns whether or not the input loop should continue.
   bool continueOn = true;

   // If pFunctionExists or pRunOnLeaveFunction are not nullptr, then default
   // them.
   if (pFunctionExists != nullptr)
   {
      *pFunctionExists = false;
   }
   if (pRunOnLeaveFunction != nullptr)
   {
      *pRunOnLeaveFunction = true;
   }

   if (functionExistsForLastMouseState())
   {
      shared_ptr<cxFunction> iFunc = mMouseFunctions[mMouse.bstate];
      // The cxFunction pointers in mkeyFunctions shouldn't be nullptr, but check
      // just in case.
      if (iFunc != nullptr)
      {
         if (iFunc->functionIsSet())
         {
            iFunc->runFunction();
            continueOn = !(iFunc->getExitAfterRun());

            if (pFunctionExists != nullptr)
            {
               *pFunctionExists = true;
            }
            // If pRunOnLeaveFunction is not nullptr, then:
            // If the window should exit after the function runs, then set
            // pRunOnLeaveFunction to what's set in the cxFunction.
            if (pRunOnLeaveFunction != nullptr)
            {
               if (iFunc->getExitAfterRun())
               {
                  *pRunOnLeaveFunction = iFunc->getRunOnLeaveFunction();
               }
            }
         }
      }
   }

   return(continueOn);
} // handleFunctionForLastMouseState

// Returns the largest subwindow height (or 0 if there are none).
inline int cxWindow::maxSubwindowHeight() const
{
   int maxSubwinHeight = 0;

   for (const cxWindow*const& subWin : mSubWindows)
   {
      if (subWin->height() > maxSubwinHeight)
      {
         maxSubwinHeight = subWin->height();
      }
   }

   return(maxSubwinHeight);
} // maxSubwindowHeight

void cxWindow::reCreatePanel()
{
   if (mPanel != nullptr)
   {
      del_panel(mPanel);
      mPanel = nullptr;
   }
   if (mWindow == nullptr)
   {
      throw cxWidgetsException("cxWindow::reCreatePanel mWindow should not be nullptr.");
   }
   mPanel = new_panel(mWindow);
   if (mPanel == nullptr)
   {
      throw cxWidgetsException("cxWindow::reCreatePanel could not run new_panel().");
   }
   set_panel_userptr(mPanel, this);
} // reCreatePanel

// Frees the memory used by mWindow
void cxWindow::freeWindow()
{
   if (mPanel != nullptr)
   {
      del_panel(mPanel);
      mPanel = nullptr;
   }
   if (mWindow != nullptr)
   {
      delwin(mWindow);
      mWindow = nullptr;
   }
} // freeWindow

// Combines all the messages lines into one
//  string.
void cxWindow::combineMessageLines(string& message)
{
   message.erase();
   for (const string& msgLine : mMessageLines)
   {
      if (!message.empty())
      {
         message += "\n";
      }
      message += msgLine;
   }
} // combineMessageLines

inline bool cxWindow::subWindowExists(cxWindow *pWindow) const
{
   bool exists = false;

   for (const cxWindow*const& subWin : mSubWindows)
   {
      if (subWin == pWindow)
      {
         exists = true;
         break;
      }
   }

   return(exists);
} // subWindowExists

void cxWindow::addToParentWindow(cxWindow *pParentWindow)
{
   if (pParentWindow != nullptr)
   {
      // Add this window to the parent window.
      pParentWindow->addSubwindow(this);
      // It would be nice if there was a way to detect whether
      //  an object is created dynamically (i.e., with the 'new'
      //  operator)..  If so, then if the parent window is a
      //  cxPanel, we could go ahead and add a pointer to this
      //  window to the parent panel by calling its addWindow().
      //  But I don't know if there's a way in C++ to tell if an
      //  object was created dynamically
      /*
      // If the parent window is a cxPanel, then add this window
      //  to it using its addWindow(); otherwise, add this window
      //  using its addSubwindow().
      try {
         string parentType = mParentWindow->cxTypeStr();
         if ((parentType == "cxPanel") || (parentType == "cxSearchPanel")) {
            cxPanel *parentPanel = dynamic_cast<cxPanel*>(pParentWindow);
            // Add this window to the panel.  Use the addWindow() that
            //  takes a row & col so that this window will be positioned
            //  relative to the panel.
            parentPanel->addWindow(this, top(), left(), false);
         }
         else {
            pParentWindow->addSubwindow(this);
         }
      }
      catch (const std::bad_cast& e) {
         // The dynamic_cast failed..  Just call addSubwindow.
         pParentWindow->addSubwindow(this);
      }
      catch (const std::bad_typeid& e) {
         // The typeid failed..  Just call addSubwindow.
         pParentWindow->addSubwindow(this);
      }
      */
   }
} // addToParentWindow

void cxWindow::writeBorderStrings(const map<int, string>& pStrings, int pVPos,
                                  e_WidgetItems pItem, short pTextColorPair)
                                  {
   // Enable the attributes for the window
   enableAttrs(mWindow, pItem);

   int endX = width()-1; // Absolute end boundary for all title text
   // For drawing border lines between the title strings
   int lineStart = 0;
   int lineEnd = 0;
   int lineLength = 0;
   map<int, string>::const_iterator iter2; // Temporary iterator
   // Note: An STL map is always sorted by its keys, so we don't have
   //  to worry about the horizontal positions (the key value) being
   //  out of order.
   map<int, string>::const_iterator lastIter = pStrings.end();
   if (pStrings.empty())
   {
      return;
   }
   --lastIter;
   map<int, string>::const_iterator iter = pStrings.begin();
   for (; iter != pStrings.end(); ++iter)
   {
      // iter->first: The horizontal position for the text
      // iter->second: The string to write
      // Enable the text attributes and color
      enableAttrs(mWindow, pItem);
      if (useColors)
      {
         wcolor_set(mWindow, pTextColorPair, nullptr);
      }
      // Write the string to the window
      writeWithHighlighting(mWindow, iter->second, pVPos, iter->first, endX);
      // Disable the text attributes and color
      disableAttrs(mWindow, pItem);
      if (useColors)
      {
         wcolor_set(mWindow, 0, nullptr);
      }
      // Get the line starting & ending positions - The
      //  line should start after the current string and
      //  end before the next string (or the right edge of
      //  the window).
      lineStart = iter->first + iter->second.length();
      if (iter != lastIter)
      {
      //if (iter != pStrings.end()) {
         iter2 = iter;
         ++iter2;
         lineEnd = iter2->first;
      }
      else
      {
         lineEnd = width()-1;
      }

      // Draw the line between this and the next string
      enableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, mBorderColorPair, nullptr);
      }
      lineLength = lineEnd - lineStart;
      switch (mBorderStyle)
      {
         case eBS_SINGLE_LINE:
         case eBS_SINGLE_TOP_DOUBLE_SIDES:
         default:
            if (pItem == eTITLE)
            {
               mvwhline(mWindow, pVPos, lineStart, (mDrawBorderTop ? ACS_HLINE : ' '), lineLength);
            }
            else
            {
               mvwhline(mWindow, pVPos, lineStart, (mDrawBorderBottom ? ACS_HLINE : ' '), lineLength);
            }
            break;
         case eBS_DOUBLE_LINE:
         case eBS_DOUBLE_TOP_SINGLE_SIDES:
            if (pItem == eTITLE)
            {
               mvwhline(mWindow, pVPos, lineStart, (mDrawBorderTop ? 0x000000cd : ' '), lineLength);
            }
            else
            {
               mvwhline(mWindow, pVPos, lineStart, (mDrawBorderBottom ? 0x000000cd : ' '), lineLength);
            }
            break;
         case eBS_SPACE:
            mvwhline(mWindow, pVPos, lineStart, ' ', lineLength);
            break;
      }

      // With this code, I was trying to add additional special characters
      //  that may exist in the line segment between this & the next string.
      //  This would be used if the special characters are drawn before the
      //  title & status strings so as not to stomp on the strings.  I couldn't
      //  get it working quite right..
      /*
      // Check for additional special characters along the line segment
      //  and write them if there are any.
      pair<int, int> location(make_pair(pVPos, 0));
      for (int x = lineStart; x <= (lineStart + lineLength); ++x) {
         location.second = x; // horizontal component
         if (mSpecialChars.find(location) != mSpecialChars.end()) {
            // Enable the special char's attribute, if there is one
            if (mSpecialCharAttrs.find(location) != mSpecialCharAttrs.end()) {
               wattron(mWindow, mSpecialCharAttrs[location]);
            }
            // Write the special char
            mvwaddch(mWindow, location.first, location.second,
                     mSpecialChars[location]);
            // Disable the special char's attribute, if there is one
            if (mSpecialCharAttrs.find(location) != mSpecialCharAttrs.end()) {
               wattroff(mWindow, mSpecialCharAttrs[location]);
            }
         }
      }
      */

      disableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, 0, nullptr);
      }
   }
   // End case: Write the horizontal border line from the last string to the
   // edge of the window (to clear any text that may have been there if the
   // new text is shorter)
   if (pStrings.size() > 0)
   {
      --iter; // To get to the last element in pStrings
      lineStart = iter->first + (int)(iter->second.length());

      // Enable the attributes & color
      enableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, mBorderColorPair, nullptr);
      }
      int lineLength = endX - lineStart;
      switch (mBorderStyle)
      {
         case eBS_SINGLE_LINE:
         case eBS_SINGLE_TOP_DOUBLE_SIDES:
         default:
            if (pItem == eTITLE)
            {
               mvwhline(mWindow, pVPos, lineStart, (mDrawBorderTop ? ACS_HLINE : ' '), lineLength);
            }
            else
            {
               mvwhline(mWindow, pVPos, lineStart, (mDrawBorderBottom ? ACS_HLINE : ' '), lineLength);
            }
            break;
         case eBS_DOUBLE_LINE:
         case eBS_DOUBLE_TOP_SINGLE_SIDES:
            if (pItem == eTITLE)
            {
               mvwhline(mWindow, pVPos, lineStart, (mDrawBorderTop ? 0x000000cd : ' '), lineLength);
            }
            else
            {
               mvwhline(mWindow, pVPos, lineStart, (mDrawBorderBottom ? 0x000000cd : ' '), lineLength);
            }
            break;
         case eBS_SPACE:
            mvwhline(mWindow, pVPos, lineStart, ' ', lineLength);
            break;
      }
      // Disable the attributes & color
      disableAttrs(mWindow, eBORDER);
      if (useColors)
      {
         wcolor_set(mWindow, 0, nullptr);
      }
   }
} // writeBorderStrings

inline bool cxWindow::anySubwinHasThisParent()
{
   bool retval = false;

   for (const cxWindow*const& subWin : mSubWindows)
   {
      if (subWin->mParentWindow == this)
      {
         retval = true;
         break;
      }
   }

   return(retval);
} // anySubwindowHasThisParent

void cxWindow::draw()
{
   // Only do this if mWindow and mPanel are both non-nullptr.
   if ((mWindow != nullptr) && (mPanel != nullptr))
   {
      // If the window has a border, draw the border characters
      if (hasBorder())
      {
         drawBorder();
      }
      if (mDrawMessage)
      {
         drawMessage();
      }
      // If the window has a border, draw the title & status strings
      if (hasBorder())
      {
         if (mDrawTitle)
         {
            drawTitle();
         }
         if (mDrawStatus)
         {
            drawStatus();
         }
      }
      // Draw the additional special characters (these are drawn before the
      //  title & status on purpose).
      if (mDrawSpecialChars)
      {
         drawSpecialChars();
      }
   }
} // draw

#ifdef NCURSES_MOUSE_VERSION
bool cxWindow::mouseEvtInRect(const MEVENT& pMouse, int pTop, int pLeft,
                              int pBottom, int pRight)
                              {
   return((pMouse.x >= pLeft) && (pMouse.x <= pRight) &&
          (pMouse.y >= pTop) && (pMouse.y <= pBottom));
} // mouseEventInrect
#endif

bool cxWindow::mouseEvtWasButtonEvt() const
{
   bool retval = false;

#ifdef NCURSES_MOUSE_VERSION
   switch (mMouse.bstate)
   {
      // These are all button events.
      case BUTTON1_PRESSED:
      case BUTTON1_RELEASED:
      case BUTTON1_CLICKED:
      case BUTTON1_DOUBLE_CLICKED:
      case BUTTON1_TRIPLE_CLICKED:
      case BUTTON2_PRESSED:
      case BUTTON2_RELEASED:
      case BUTTON2_CLICKED:
      case BUTTON2_DOUBLE_CLICKED:
      case BUTTON2_TRIPLE_CLICKED:
      case BUTTON3_PRESSED:
      case BUTTON3_RELEASED:
      case BUTTON3_CLICKED:
      case BUTTON3_DOUBLE_CLICKED:
      case BUTTON3_TRIPLE_CLICKED:
      case BUTTON4_PRESSED:
      case BUTTON4_RELEASED:
      case BUTTON4_CLICKED:
      case BUTTON4_DOUBLE_CLICKED:
      case BUTTON4_TRIPLE_CLICKED:
         retval = true;
         break;
      default:
         break;
   }
#endif

   return(retval);
} // mouseEvtWasButtonEvt

void cxWindow::setKeyFunctions(const cxWindow& pWindow)
{
   // Make sure the other window is a different window than this one
   if (&pWindow == this)
   {
      return;
   }

   // Clear mKeyFunctions and copy the ones from pWindow
   mKeyFunctions.clear();
   shared_ptr<cxFunction> iFunc = nullptr;
   for (const auto& funcMapPair : mKeyFunctions)
   {
      iFunc = funcMapPair.second;
      if (iFunc != nullptr)
      {
         // See if it's a cxFunction0, cxFunction2, or cxFunction 4, and copy it as such.
         if (iFunc->cxTypeStr() == "cxFunction0")
         {
            try
            {
               shared_ptr<cxFunction0> iFunc0 = dynamic_pointer_cast<cxFunction0>(iFunc);
               if (iFunc0 != nullptr)
               {
                  setKeyFunction(funcMapPair.first, iFunc0->getFunction(),
                                 iFunc0->getUseReturnVal(),
                                 iFunc0->getExitAfterRun(),
                                 iFunc0->getRunOnLeaveFunction());
               }
            }
            catch (...)
            {
            }
         }
         else if (iFunc->cxTypeStr() == "cxFunction2")
         {
            try
            {
               shared_ptr<cxFunction2> iFunc2 = dynamic_pointer_cast<cxFunction2>(iFunc);
               if (iFunc2 != nullptr)
               {
                  // If either parameter points to the other window, have it
                  //  point to this one instead.
                  void *param1 = iFunc2->getParam1();
                  void *param2 = iFunc2->getParam2();
                  if (param1 == (void*)(&pWindow))
                  {
                     param1 = (void*)this;
                  }
                  if (param2 == (void*)(&pWindow))
                  {
                     param2 = (void*)this;
                  }
                  setKeyFunction(funcMapPair.first, iFunc2->getFunction(),
                                 param1, param2, iFunc2->getUseReturnVal(),
                                 iFunc2->getExitAfterRun(),
                                 iFunc2->getRunOnLeaveFunction());
               }
            }
            catch (...)
            {
            }
         }
         else if (iFunc->cxTypeStr() == "cxFunction4")
         {
            try
            {
               shared_ptr<cxFunction4> iFunc4 = dynamic_pointer_cast<cxFunction4>(iFunc);
               if (iFunc4 != nullptr)
               {
                  // If any of the parameters point to the other window, have
                  //  them point to this one instead.
                  void *param1 = iFunc4->getParam1();
                  void *param2 = iFunc4->getParam2();
                  void *param3 = iFunc4->getParam3();
                  void *param4 = iFunc4->getParam4();
                  if (param1 == (void*)(&pWindow))
                  {
                     param1 = (void*)this;
                  }
                  if (param2 == (void*)(&pWindow))
                  {
                     param2 = (void*)this;
                  }
                  if (param3 == (void*)(&pWindow))
                  {
                     param3 = (void*)this;
                  }
                  if (param4 == (void*)(&pWindow))
                  {
                     param4 = (void*)this;
                  }
                  setKeyFunction(funcMapPair.first, iFunc4->getFunction(),
                                 param1, param2, param3, param4,
                                 iFunc4->getUseReturnVal(),
                                 iFunc4->getExitAfterRun(),
                                 iFunc4->getRunOnLeaveFunction());
               }
            }
            catch (...)
            {
            }
         }
      }
   }

   // Do the same with mMouseFunctions
   mMouseFunctions.clear();
   iFunc = nullptr;
   for (const auto& funcMapPair : mMouseFunctions)
   {
      iFunc = funcMapPair.second;
      if (iFunc != nullptr)
      {
         // See if it's a cxFunction0, cxFunction2, or cxFunction 4, and copy it as such.
         if (iFunc->cxTypeStr() == "cxFunction0")
         {
            try
            {
               shared_ptr<cxFunction0> iFunc0 = dynamic_pointer_cast<cxFunction0>(iFunc);
               if (iFunc0 != nullptr)
               {
                  setMouseFunction(funcMapPair.first, iFunc0->getFunction(),
                                 iFunc0->getUseReturnVal(),
                                 iFunc0->getExitAfterRun(),
                                 iFunc0->getRunOnLeaveFunction());
               }
            }
            catch (...)
            {
            }
         }
         else if (iFunc->cxTypeStr() == "cxFunction2")
         {
            try
            {
               shared_ptr<cxFunction2> iFunc2 = dynamic_pointer_cast<cxFunction2>(iFunc);
               if (iFunc2 != nullptr)
               {
                  // If either parameter points to the other window, have it
                  //  point to this one instead.
                  void *param1 = iFunc2->getParam1();
                  void *param2 = iFunc2->getParam2();
                  if (param1 == (void*)(&pWindow))
                  {
                     param1 = (void*)this;
                  }
                  if (param2 == (void*)(&pWindow))
                  {
                     param2 = (void*)this;
                  }
                  setMouseFunction(funcMapPair.first, iFunc2->getFunction(),
                                 param1, param2, iFunc2->getUseReturnVal(),
                                 iFunc2->getExitAfterRun(),
                                 iFunc2->getRunOnLeaveFunction());
               }
            }
            catch (...)
            {
            }
         }
         else if (iFunc->cxTypeStr() == "cxFunction4")
         {
            try
            {
               shared_ptr<cxFunction4> iFunc4 = dynamic_pointer_cast<cxFunction4>(iFunc);
               if (iFunc4 != nullptr)
               {
                  // If any of the parameters point to the other window, have
                  //  them point to this one instead.
                  void *param1 = iFunc4->getParam1();
                  void *param2 = iFunc4->getParam2();
                  void *param3 = iFunc4->getParam3();
                  void *param4 = iFunc4->getParam4();
                  if (param1 == (void*)(&pWindow))
                  {
                     param1 = (void*)this;
                  }
                  if (param2 == (void*)(&pWindow))
                  {
                     param2 = (void*)this;
                  }
                  if (param3 == (void*)(&pWindow))
                  {
                     param3 = (void*)this;
                  }
                  if (param4 == (void*)(&pWindow))
                  {
                     param4 = (void*)this;
                  }
                  setMouseFunction(funcMapPair.first, iFunc4->getFunction(),
                                 param1, param2, param3, param4,
                                 iFunc4->getUseReturnVal(),
                                 iFunc4->getExitAfterRun(),
                                 iFunc4->getRunOnLeaveFunction());
               }
            }
            catch (...)
            {
            }
         }
      }
   }
} // setKeyFunctions

void cxWindow::setFocusFunctions(const cxWindow& pWindow)
{
   // Free the memory currently used by the functions
   if (mOnFocusFunction != nullptr)
   {
      mOnFocusFunction = nullptr;
   }
   if (mOnLeaveFunction != nullptr)
   {
      mOnLeaveFunction = nullptr;
   }

   // Copy the other window's onFocus function
   if (pWindow.mOnFocusFunction != nullptr)
   {
      // See if it's a cxFunction0, cxFunction2, or cxFunction 4, and copy it as such.
      if (pWindow.mOnFocusFunction->cxTypeStr() == "cxFunction0")
      {
         try
         {
            shared_ptr<cxFunction0> iFunc0 = dynamic_pointer_cast<cxFunction0>(pWindow.mOnFocusFunction);
            if (iFunc0 != nullptr)
            {
               setOnFocusFunction(iFunc0->getFunction(),
                                  iFunc0->getUseReturnVal(),
                                  iFunc0->getExitAfterRun());
            }
         }
         catch (...)
         {
         }
      }
      else if (pWindow.mOnFocusFunction->cxTypeStr() == "cxFunction2")
      {
         try
         {
            shared_ptr<cxFunction2> iFunc2 = dynamic_pointer_cast<cxFunction2>(pWindow.mOnFocusFunction);
            if (iFunc2 != nullptr)
            {
               // If either parameter points to the other window, have it
               //  point to this one instead.
               void* params[] = { iFunc2->getParam1(), iFunc2->getParam2() };
               for (int i = 0; i < 2; ++i)
               {
                  if (params[i] == (void*)(&pWindow))
                  {
                     params[i] = (void*)this;
                  }
               }
               setOnFocusFunction(iFunc2->getFunction(), params[0], params[1],
                                  iFunc2->getUseReturnVal(),
                                  iFunc2->getExitAfterRun());
            }
         }
         catch (...)
         {
         }
      }
      else if (pWindow.mOnFocusFunction->cxTypeStr() == "cxFunction4")
      {
         try
         {
            shared_ptr<cxFunction4> iFunc4 = dynamic_pointer_cast<cxFunction4>(pWindow.mOnFocusFunction);
            if (iFunc4 != nullptr)
            {
               // If either parameter points to the other window, have it
               //  point to this one instead.
               void* params[] = { iFunc4->getParam1(), iFunc4->getParam2(),
                                  iFunc4->getParam3(), iFunc4->getParam4()  };
               for (int i = 0; i < 4; ++i)
               {
                  if (params[i] == (void*)(&pWindow))
                  {
                     params[i] = (void*)this;
                  }
               }
               setOnFocusFunction(iFunc4->getFunction(), params[0], params[1],
                                  params[2], params[3],
                                  iFunc4->getUseReturnVal(),
                                  iFunc4->getExitAfterRun());
            }
         }
         catch (...)
         {
         }
      }
   }

   // Do the same for the onLeave function
   if (pWindow.mOnLeaveFunction != nullptr)
   {
      // See if it's a cxFunction0, cxFunction2, or cxFunction 4, and copy it as such.
      if (pWindow.mOnLeaveFunction->cxTypeStr() == "cxFunction0")
      {
         try
         {
            shared_ptr<cxFunction0> iFunc0 = dynamic_pointer_cast<cxFunction0>(pWindow.mOnLeaveFunction);
            if (iFunc0 != nullptr)
            {
               setOnLeaveFunction(iFunc0->getFunction());
            }
         }
         catch (...)
         {
         }
      }
      else if (pWindow.mOnLeaveFunction->cxTypeStr() == "cxFunction2")
      {
         try
         {
            shared_ptr<cxFunction2> iFunc2 = dynamic_pointer_cast<cxFunction2>(pWindow.mOnLeaveFunction);
            if (iFunc2 != nullptr)
            {
               // If either parameter points to the other window, have it
               //  point to this one instead.
               void* params[] = { iFunc2->getParam1(), iFunc2->getParam2() };
               for (int i = 0; i < 2; ++i)
               {
                  if (params[i] == (void*)(&pWindow))
                  {
                     params[i] = (void*)this;
                  }
               }
               setOnLeaveFunction(iFunc2->getFunction(), params[0], params[1]);
            }
         }
         catch (...)
         {
         }
      }
      else if (pWindow.mOnLeaveFunction->cxTypeStr() == "cxFunction4")
      {
         try
         {
            shared_ptr<cxFunction4> iFunc4 = dynamic_pointer_cast<cxFunction4>(pWindow.mOnLeaveFunction);
            if (iFunc4 != nullptr)
            {
               // If either parameter points to the other window, have it
               //  point to this one instead.
               void* params[] = { iFunc4->getParam1(), iFunc4->getParam2(),
                                  iFunc4->getParam3(), iFunc4->getParam4() };
               for (int i = 0; i < 4; ++i)
               {
                  if (params[i] == (void*)(&pWindow))
                  {
                     params[i] = (void*)this;
                  }
               }
               setOnLeaveFunction(iFunc4->getFunction(), params[0], params[1],
                                  params[2], params[3]);
            }
         }
         catch (...)
         {
         }
      }
   }
} // setFocusFunctions

bool cxWindow::parentIsCxPanel() const
{
   bool isPanel = false;

   if (mParentWindow != nullptr)
   {
      isPanel = (mParentWindow->cxTypeStr() == "cxPanel");
   }

   return(isPanel);
} // parentIsCxPanel

bool cxWindow::parentIsCxForm() const
{
   bool isForm = false;

   if (mParentWindow != nullptr)
   {
      isForm = (mParentWindow->cxTypeStr() == "cxForm");
   }

   return(isForm);
} // parentIsCxForm

bool cxWindow::parentIsCxNotebook() const
{
   bool isNotebook = false;

   if (mParentWindow != nullptr)
   {
      isNotebook = (mParentWindow->cxTypeStr() == "cxNotebook");
   }

   return(isNotebook);
} // parentIsCxNotebook

cxWindowPtrContainer::const_iterator cxWindow::subWindows_begin() const
{
   return(mSubWindows.begin());
} // subWindows_begin

cxWindowPtrContainer::const_iterator cxWindow::subWindows_end() const
{
   return(mSubWindows.end());
} // subWindows_end

cxWindowPtrContainer::const_reverse_iterator cxWindow::subWindows_rbegin() const
{
   return(mSubWindows.rbegin());
} // subWindows_rbegin

cxWindowPtrContainer::const_reverse_iterator cxWindow::subWindows_rend() const
{
   return(mSubWindows.rend());
} // subWindows_end

bool cxWindow::onFocusFunctionIsSet() const
{
   bool funcIsSet = false;

   if (mOnFocusFunction != nullptr)
   {
      funcIsSet = mOnFocusFunction->functionIsSet();
   }

   return(funcIsSet);
} // onLeaveFunctionIsSet

bool cxWindow::onLeaveFunctionIsSet() const
{
   bool funcIsSet = false;

   if (mOnLeaveFunction != nullptr)
   {
      funcIsSet = mOnLeaveFunction->functionIsSet();
   }

   return(funcIsSet);
} // onLeaveFunctionIsSet
