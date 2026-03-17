/*
 * cxBase.cpp - Contains cxWidgets utility functions.
 *
 * Copyright (c) 2005-2007 Michael H. Kinney
 *
 * \author $Author: erico $
 * \version $Revision: 1.101 $
 * \date $Date: 2007/12/04 00:23:45 $
 *
 * Date     User      Description
 * 08/21/07 erico     Added new versions of messageBox() that let you specify
 *                    colors for the different parts of the message box.  Also
 *                    added some default colors.
 */

#include "cxBase.h"
#include <signal.h> // For signal handling
#include <cstdlib>  // For getenv(), tolower(), and toupper()
#include <cctype>   // For toupper()
#include <set>
#include <fstream> // to write to file
#include "cxWindow.h" // For HOTKEY_CHAR
#include "cxMessageDialog.h"
#include "cxMenu.h"
#include "cxForm.h"
#include "cxStringUtils.h"
#include <vector>
#include <map>
#include <iostream>
using std::string;
using std::set;
using std::list;
using std::vector;
using std::map;
using std::ofstream;
using std::cerr;
using std::endl;
using cxStringUtils::toString;
using cxStringUtils::isPrintable;

// mInitialized is used to store whether cxBase::init() has been called.
//  It's declared here because the compiler gives a warning if it is declared
//  in cxBase.h.
namespace cxBase
{

   bool mInitialized = false;

   // cxWindow
   // These collections will replace the singular ones:
   set<attr_t> mMessageAttrs;
   set<attr_t> mTitleAttrs;
   set<attr_t> mStatusAttrs;
   set<attr_t> mBorderAttrs;
   set<attr_t> mHotkeyAttrs;
   set<attr_t> mMenuSelectionAttrs;
   set<attr_t> mLabelAttrs;
   set<attr_t> mDataReadonlyAttrs;
   set<attr_t> mDataEditableAttrs;

   // cxForm and cxMultiLineInput
   attr_t mLabelAttr=A_DIM;
   attr_t mDataEditableAttr=A_REVERSE;
   attr_t mDataReadonlyAttr=A_BOLD;
   int mMenuClearKeywordKey = KEY_F(10);

   // cxMenu
   attr_t mMenuSelectionAttr=A_REVERSE;

   // A set of keys that are designated as "navigation keys"
   set<int> mNavKeys;

   // Default colors for parts of windows
   e_cxColors mDefaultMessageColor = eGRAY_BLUE;
   e_cxColors mDefaultTitleColor = eWHITE_BLUE;
   e_cxColors mDefaultStatusColor = eYELLOW_BLUE;
   e_cxColors mDefaultBorderColor = eGRAY_BLUE;
}

void cxBase::init(bool pEnableMouse)
{
   WINDOW* tmpWindow=initscr();         // initialize the curses library
   if (tmpWindow == nullptr)
   {
      cerr << "Warning: initscr() returned a nullptr pointer" << endl;
      throw(cxWidgetsException("Warning: initscr() returned a nullptr pointer."));
   }
   //trace(TRACE_DISABLE);
   keypad(stdscr, TRUE);     // enable keyboard mapping
   nonl();                   // tell curses not to do NL->CR/NL on output
   cbreak();                 // take input chars one at a time, no wait for \n
   noecho();                 // do not echo characters
   intrflush(stdscr, FALSE); // flush on interrupt (Do we need/want this???)
   curs_set(0);              // Disable the cursor (it looks funky sometimes)
   ESCDELAY=0;               // change the ESCAPE key delay to 0

   //trace(TRACE_MAXIMUM); // turn on maximum trace debugging

   // Initialize the colors listed in cxColors.h
   if (has_colors())
   {
      start_color();
      // The following are the init_pair calls with the foreground
      //  color followed by the background color, as stated in
      //  the man page for init_pair.
      init_pair(eRED_BLACK,        COLOR_RED,     COLOR_BLACK);
      init_pair(eBRTRED_BLACK,     COLOR_RED,     COLOR_BLACK);
      init_pair(eGREEN_BLACK,      COLOR_GREEN,   COLOR_BLACK);
      init_pair(eBRTGREEN_BLACK,   COLOR_GREEN,   COLOR_BLACK);
      init_pair(eBROWN_BLACK,      COLOR_YELLOW,  COLOR_BLACK);
      init_pair(eYELLOW_BLACK,     COLOR_YELLOW,  COLOR_BLACK);
      init_pair(eBLUE_BLACK,       COLOR_BLUE,    COLOR_BLACK);
      init_pair(eBRTBLUE_BLACK,    COLOR_BLUE,    COLOR_BLACK);
      init_pair(eCYAN_BLACK,       COLOR_CYAN,    COLOR_BLACK);
      init_pair(eBRTCYAN_BLACK,    COLOR_CYAN,    COLOR_BLACK);
      init_pair(eMAGENTA_BLACK,    COLOR_MAGENTA, COLOR_BLACK);
      init_pair(eBRTMAGENTA_BLACK, COLOR_MAGENTA, COLOR_BLACK);
      init_pair(eGRAY_BLACK,       COLOR_WHITE,   COLOR_BLACK);
      init_pair(eWHITE_BLACK,      COLOR_WHITE,   COLOR_BLACK);
      init_pair(eBLACK_BLACK,      COLOR_BLACK,   COLOR_BLACK);

      init_pair(eRED_WHITE,        COLOR_RED,     COLOR_WHITE);
      init_pair(eBRTRED_WHITE,     COLOR_RED,     COLOR_WHITE);
      init_pair(eGREEN_WHITE,      COLOR_GREEN,   COLOR_WHITE);
      init_pair(eBRTGREEN_WHITE,   COLOR_GREEN,   COLOR_WHITE);
      init_pair(eBROWN_WHITE,      COLOR_YELLOW,  COLOR_WHITE);
      init_pair(eYELLOW_WHITE,     COLOR_YELLOW,  COLOR_WHITE);
      init_pair(eBLUE_WHITE,       COLOR_BLUE,    COLOR_WHITE);
      init_pair(eBRTBLUE_WHITE,    COLOR_BLUE,    COLOR_WHITE);
      init_pair(eCYAN_WHITE,       COLOR_CYAN,    COLOR_WHITE);
      init_pair(eBRTCYAN_WHITE,    COLOR_CYAN,    COLOR_WHITE);
      init_pair(eMAGENTA_WHITE,    COLOR_MAGENTA, COLOR_WHITE);
      init_pair(eBRTMAGENTA_WHITE, COLOR_MAGENTA, COLOR_WHITE);
      init_pair(eGRAY_WHITE,       COLOR_WHITE,   COLOR_WHITE);
      init_pair(eWHITE_WHITE,      COLOR_WHITE,   COLOR_WHITE);
      init_pair(eBLACK_WHITE,      COLOR_BLACK,   COLOR_WHITE);

      init_pair(eRED_BLUE,         COLOR_RED,     COLOR_BLUE);
      init_pair(eBRTRED_BLUE,      COLOR_RED,     COLOR_BLUE);
      init_pair(eGREEN_BLUE,       COLOR_GREEN,   COLOR_BLUE);
      init_pair(eBRTGREEN_BLUE,    COLOR_GREEN,   COLOR_BLUE);
      init_pair(eBROWN_BLUE,       COLOR_YELLOW,  COLOR_BLUE);
      init_pair(eYELLOW_BLUE,      COLOR_YELLOW,  COLOR_BLUE);
      init_pair(eBLUE_BLUE,        COLOR_BLUE,    COLOR_BLUE);
      init_pair(eBRTBLUE_BLUE,     COLOR_BLUE,    COLOR_BLUE);
      init_pair(eCYAN_BLUE,        COLOR_CYAN,    COLOR_BLUE);
      init_pair(eBRTCYAN_BLUE,     COLOR_CYAN,    COLOR_BLUE);
      init_pair(eMAGENTA_BLUE,     COLOR_MAGENTA, COLOR_BLUE);
      init_pair(eBRTMAGENTA_BLUE,  COLOR_MAGENTA, COLOR_BLUE);
      init_pair(eGRAY_BLUE,        COLOR_WHITE,   COLOR_BLUE);
      init_pair(eWHITE_BLUE,       COLOR_WHITE,   COLOR_BLUE);
      init_pair(eBLACK_BLUE,       COLOR_BLACK,   COLOR_BLUE);

      init_pair(eRED_GREEN,        COLOR_RED,     COLOR_GREEN);
      init_pair(eBRTRED_GREEN,     COLOR_RED,     COLOR_GREEN);
      init_pair(eBROWN_GREEN,      COLOR_YELLOW,  COLOR_GREEN);
      init_pair(eYELLOW_GREEN,     COLOR_YELLOW,  COLOR_GREEN);
      init_pair(eBLUE_GREEN,       COLOR_BLUE,    COLOR_GREEN);
      init_pair(eBRTBLUE_GREEN,    COLOR_BLUE,    COLOR_GREEN);
      init_pair(eCYAN_GREEN,       COLOR_CYAN,    COLOR_GREEN);
      init_pair(eBRTCYAN_GREEN,    COLOR_CYAN,    COLOR_GREEN);
      init_pair(eMAGENTA_GREEN,    COLOR_MAGENTA, COLOR_GREEN);
      init_pair(eBRTMAGENTA_GREEN, COLOR_MAGENTA, COLOR_GREEN);
      init_pair(eGRAY_GREEN,       COLOR_WHITE,   COLOR_GREEN);
      init_pair(eWHITE_GREEN,      COLOR_WHITE,   COLOR_GREEN);
      init_pair(eGREEN_GREEN,      COLOR_GREEN,   COLOR_GREEN);
      init_pair(eBRTGREEN_GREEN,   COLOR_GREEN,   COLOR_GREEN);
      init_pair(eBLACK_GREEN,      COLOR_BLACK,   COLOR_GREEN);

      init_pair(eRED_RED,          COLOR_RED,     COLOR_RED);
      init_pair(eBRTRED_RED,       COLOR_RED,     COLOR_RED);
      init_pair(eGREEN_RED,        COLOR_GREEN,   COLOR_RED);
      init_pair(eBRTGREEN_RED,     COLOR_GREEN,   COLOR_RED);
      init_pair(eBROWN_RED,        COLOR_YELLOW,  COLOR_RED);
      init_pair(eYELLOW_RED,       COLOR_YELLOW,  COLOR_RED);
      init_pair(eBLUE_RED,         COLOR_BLUE,    COLOR_RED);
      init_pair(eBRTBLUE_RED,      COLOR_BLUE,    COLOR_RED);
      init_pair(eCYAN_RED,         COLOR_CYAN,    COLOR_RED);
      init_pair(eBRTCYAN_RED,      COLOR_CYAN,    COLOR_RED);
      init_pair(eMAGENTA_RED,      COLOR_MAGENTA, COLOR_RED);
      init_pair(eBRTMAGENTA_RED,   COLOR_MAGENTA, COLOR_RED);
      init_pair(eGRAY_RED,         COLOR_WHITE,   COLOR_RED);
      init_pair(eWHITE_RED,        COLOR_WHITE,   COLOR_RED);
      init_pair(eBLACK_RED,        COLOR_BLACK,   COLOR_RED);

      init_pair(eRED_CYAN,         COLOR_CYAN,    COLOR_CYAN);
      init_pair(eBRTRED_CYAN,      COLOR_CYAN,    COLOR_CYAN);
      init_pair(eGREEN_CYAN,       COLOR_GREEN,   COLOR_CYAN);
      init_pair(eBRTGREEN_CYAN,    COLOR_GREEN,   COLOR_CYAN);
      init_pair(eBROWN_CYAN,       COLOR_YELLOW,  COLOR_CYAN);
      init_pair(eYELLOW_CYAN,      COLOR_YELLOW,  COLOR_CYAN);
      init_pair(eBLUE_CYAN,        COLOR_BLUE,    COLOR_CYAN);
      init_pair(eBRTBLUE_CYAN,     COLOR_BLUE,    COLOR_CYAN);
      init_pair(eCYAN_CYAN,        COLOR_CYAN,    COLOR_CYAN);
      init_pair(eBRTCYAN_CYAN,     COLOR_CYAN,    COLOR_CYAN);
      init_pair(eMAGENTA_CYAN,     COLOR_MAGENTA, COLOR_CYAN);
      init_pair(eBRTMAGENTA_CYAN,  COLOR_MAGENTA, COLOR_CYAN);
      init_pair(eGRAY_CYAN,        COLOR_WHITE,   COLOR_CYAN);
      init_pair(eWHITE_CYAN,       COLOR_WHITE,   COLOR_CYAN);
      init_pair(eBLACK_CYAN,       COLOR_BLACK,   COLOR_CYAN);
   }

   signal(SIGWINCH, cxBase::signalHandler);

   // Set up some default values for various cxWidgets items
   mMenuSelectionAttrs.insert(A_REVERSE); // Selected cxMenu items
   mHotkeyAttrs.insert(A_REVERSE); // Hotkeys

   // Set up the default navigation keys
   mNavKeys.insert(PAGE_UP);
   mNavKeys.insert(PAGE_DOWN);
   mNavKeys.insert(TAB);
   mNavKeys.insert(SHIFT_TAB);
   mNavKeys.insert(KEY_DOWN);
   mNavKeys.insert(KEY_UP);
   mNavKeys.insert(KEY_LEFT);
   mNavKeys.insert(KEY_RIGHT);

#ifdef NCURSES_MOUSE_VERSION
   // Initialize the mouse if pEnableMouse is true
   if (pEnableMouse)
   {
      mousemask(ALL_MOUSE_EVENTS, nullptr);
   }
#endif

   refresh();

   cxBase::mInitialized = true;
} // init

void cxBase::cleanup()
{
   // Only do this if cxWidgets is initialized, or else the program will
   //  segfault.
   if (cxInitialized())
   {
      curs_set(1); // Enable the cursor
      mvcur(0, COLS - 1, LINES - 1, 0); // Move the cursor to lower left
      erase();
      endwin();
      //delwin(stdscr);
      cxBase::mInitialized = false;
   }
} // cleanup

int cxBase::top()
{
   int row = 0, col = 0;
   getbegyx(stdscr, row, col);
   return(row);
} // top

int cxBase::centerRow()
{
   int row = 0, col = 0, center = 0;
   getmaxyx(stdscr, row, col);
   center=row/2;
   return(center);
} // centerRow

int cxBase::bottom()
{
   // getmaxyx() returns the screen size, but the column numbers are 0-based,
   //  so we need to subtract 1.
   int theHeight = 0, theWidth = 0;
   getmaxyx(stdscr, theHeight, theWidth);
   return(theHeight - 1);
} // bottom

int cxBase::left()
{
   int row = 0, col = 0;
   getbegyx(stdscr, row, col);
   (void)row;
   return(col);
} // left

int cxBase::centerCol()
{
   int theHeight = 0, theWidth = 0;
   getmaxyx(stdscr, theHeight, theWidth);
   (void)theHeight;
   return(theWidth / 2);
} // centerCol

int cxBase::right()
{
   // getmaxyx() returns the screen size, but the column numbers are 0-based,
   //  so we need to subtract 1.
   int theHeight = 0, theWidth = 0;
   getmaxyx(stdscr, theHeight, theWidth);
   (void)theHeight;
   return(theWidth - 1);
} // right

int cxBase::width()
{
   // getmaxyx() returns the screen size
   int theHeight = 0, theWidth = 0;
   getmaxyx(stdscr, theHeight, theWidth);
   (void)theHeight;
   return(theWidth);
} // width

int cxBase::height()
{
   int theHeight = 0, theWidth = 0;
   getmaxyx(stdscr, theHeight, theWidth);
   return(theHeight);
} // height

void cxBase::getTermDimentions(int& pHeight, int& pWidth)
{
   pHeight = cxBase::height();
   pWidth = cxBase::width();
} // getTermDimentions

bool cxBase::resizeTerm(int pNewHeight, int pNewWidth)
{
   return(resize_term(pNewHeight, pNewWidth) == OK);
} // resizeTerm

e_cxColors cxBase::getDefaultMessageColor()
{
   return(cxBase::mDefaultMessageColor);
} // getDefaultMessageColor

void cxBase::setDefaultMessageColor(e_cxColors pColor)
{
   cxBase::mDefaultMessageColor = pColor;
} // setDefaultMessageColor

e_cxColors cxBase::getDefaultTitleColor()
{
   return(cxBase::mDefaultTitleColor);
} // getDefaultTitleColor

void cxBase::setDefaultTitleColor(e_cxColors pColor)
{
   cxBase::mDefaultTitleColor = pColor;
} // setDefaultTitleColor

e_cxColors cxBase::getDefaultStatusColor()
{
   return(cxBase::mDefaultStatusColor);
} // getDefaultStatusColor

void cxBase::setDefaultStatusColor(e_cxColors pColor)
{
   cxBase::mDefaultStatusColor = pColor;
} // setDefaultStatusColor

e_cxColors cxBase::getDefaultBorderColor()
{
   return(cxBase::mDefaultBorderColor);
} // getDefaultBorderColor

void cxBase::setDefaultBorderColor(e_cxColors pColor)
{
   cxBase::mDefaultBorderColor = pColor;
} // setDefaultBorderColor

void cxBase::messageBox(int pRow, int pCol, int pHeight, int pWidth,
                        const string& pTitle, const string& pMessage,
                        const string& pStatus, eBorderStyle pBorderStyle)
                        {
   cxWindow aWindow(nullptr, pRow, pCol, pHeight, pWidth, pTitle, pMessage, pStatus, pBorderStyle);
   aWindow.showModal(true, true, false);
   aWindow.hide(); // Hide the window so it doesn't show anymore
} // messageBox

void cxBase::messageBox(int pRow, int pCol, const string& pTitle,
                        const string& pMessage,
                        const string& pStatus)
                        {
   cxWindow aWindow(nullptr, pRow, pCol, pTitle, pMessage, pStatus);
   aWindow.showModal(true, true, false);
   aWindow.hide(); // Hide the window so it doesn't show anymore
} // messageBox

void cxBase::messageBox(int pRow, int pCol, const string& pMessage)
{
   cxWindow aWindow(nullptr, pRow, pCol, "", pMessage, "");
   aWindow.showModal(true, true, false);
   aWindow.hide(); // Hide the window so it doesn't show anymore
} // messageBox

void cxBase::messageBox(const string& pTitle, const string& pMessage,
                        const string& pStatus)
                        {
   cxMessageDialog msgDlg(nullptr, pTitle, pMessage);
   msgDlg.setStatus(pStatus);
   msgDlg.showModal();
   msgDlg.hide(); // Hide the window so it doesn't show anymore
} // messageBox

void cxBase::messageBox(const string& pMessage, const string& pStatus)
{
   cxMessageDialog msgDlg(nullptr, "Info", pMessage);
   msgDlg.setStatus(pStatus);
   msgDlg.showModal();
   msgDlg.hide(); // Hide the window so it doesn't show anymore
} // messageBox

void cxBase::messageBox(const string& pMessage, e_cxColors pMessageColor,
                        attr_t pMessageAttr, e_cxColors pTitleColor,
                        e_cxColors pStatusColor, e_cxColors pBorderColor)
                        {
   cxMessageDialog msgDlg(nullptr, "Info", pMessage);
   msgDlg.setMessageColor(pMessageColor);
   msgDlg.setAttr(eMESSAGE, pMessageAttr);
   msgDlg.setTitleColor(pTitleColor);
   msgDlg.setStatusColor(pStatusColor);
   msgDlg.setBorderColor(pBorderColor);
   msgDlg.showModal();
} // messageBox

void cxBase::messageDialog(const string& pMessage)
{
   cxMessageDialog msgDlg(nullptr, "Info", pMessage);
   msgDlg.showModal();
   msgDlg.hide(); // Hide the window so it doesn't show anymore
} // messageDialog

void cxBase::messageDialog(const list<string>& pMessages)
{
   cxMessageDialog msgDlg(nullptr, 0, 0, height(), width(), "Info", "");
   msgDlg.addMessageLinesBelow(pMessages);
   msgDlg.center(false);
   msgDlg.showModal();
   msgDlg.hide(); // Hide the window so it doesn't show anymore
} // messageDialog

void cxBase::splash(const string& pTitle, const string& pMessage, int pSleep)
{
   cxWindow aWindow(nullptr, pTitle, pMessage, "");
   aWindow.show();
   sleep(pSleep);
   aWindow.hide();
} // splash

void cxBase::splash(const string& pMessage, int pSleep)
{
   cxWindow aWindow(nullptr, pMessage);
   aWindow.show();
   sleep(pSleep);
   aWindow.hide();
} // splash

// Erases the screen
void cxBase::eraseScreen()
{
   // Only do this if cxWidgets is initialized, or else the program will
   //  segfault.
   if (cxInitialized())
   {
      werase(stdscr);
      wrefresh(stdscr);
   }
} // eraseScreen

void cxBase::updateWindows()
{
   // Only do this if cxWidgets is initialized, or else the program will
   //  segfault.
   if (cxInitialized())
   {
      update_panels();
      doupdate();
   }
} // updateWindows

bool cxBase::isFunctionKey(int pKey)
{
   return((pKey == KEY_F(1)) ||
          (pKey == KEY_F(2)) ||
          (pKey == KEY_F(3)) ||
          (pKey == KEY_F(4)) ||
          (pKey == KEY_F(5)) ||
          (pKey == KEY_F(6)) ||
          (pKey == KEY_F(7)) ||
          (pKey == KEY_F(8)) ||
          (pKey == KEY_F(9)) ||
          (pKey == KEY_F(10)) ||
          (pKey == KEY_F(11)) ||
          (pKey == KEY_F(12)) ||
          (pKey == KEY_NPAGE) ||
          (pKey == KEY_PPAGE));
} // isFunctionKey

bool cxBase::toggleCursor(bool pToggleCursor)
{
   // If pToggleCursor is true, use 1; otherwise, use 0.
   int previous = curs_set(pToggleCursor ? 1 : 0);
   return(previous != 0); // Return the previous cursor state
} // toggleCursor

string cxBase::getTermType()
{
   // NOTE: Do not re-write this to be one line.
   // If you run a program from cron, you may not
   // have *ANY* environment variables "set".
   // If you do something like :
   //   return(string(getenv("TERM"));
   // You will get messages like this:
   //   terminate called after throwing an instance of 'std::logic_error'
   //   what():  basic_string::_S_construct nullptr not valid
   string retval;
   char *term =getenv("TERM");
   if (term != nullptr)
   {
      retval = term;
   }
   return(retval);
} // getTermType

bool cxBase::colorIsBright(e_cxColors pColor)
{
   return((pColor & cxCOLOR_BRIGHT) == cxCOLOR_BRIGHT);
} // colorIsBright

int cxBase::countHotkeys(const string& pStr)
{
   int hotkeyCount = 0;

   unsigned strLen = pStr.length();
   for (unsigned i = 0; i < strLen; ++i)
   {
      // Check for an ampersand (&) followed by
      //  a non-space
      if (i < (strLen-1))
      {
         if ((pStr[i] == HOTKEY_CHAR) && (pStr[i+1] != ' '))
         {
            ++hotkeyCount;
         }
      }
   }

   return(hotkeyCount);
} // countHotkeys

string cxBase::stringWithoutHotkeyChars(const string& pStr)
{
   string updatedStr(pStr);
   unsigned strLen = 0;
   while (countHotkeys(updatedStr) > 0)
   {
      strLen = updatedStr.length();
      for (unsigned i = 0; i < strLen; ++i)
      {
         // Check for an ampersand (&) followed by
         //  a non-space
         if (i < (strLen-1))
         {
            if ((updatedStr[i] == HOTKEY_CHAR) && (updatedStr[i+1] != ' '))
            {
               updatedStr.erase(i, 1);
               break;
            }
         }
      }
   }

   return(updatedStr);
} // stringWithoutHotkeyChars

unsigned cxBase::visualStrLen(const string& pStr)
{
   return(stringWithoutHotkeyChars(pStr).length());
} // visualStrLen

string cxBase::bringToTop(void *theWindow, void* unused)
{
   if (theWindow != nullptr)
   {
      cxWindow* pWindow = (cxWindow*)theWindow;
      pWindow->show(true, false);
   }

   return("");
} // bringToTop

string cxBase::cxFormShowModal(void* theForm, void* unused)
{
   string retval = toString(cxID_QUIT);

   if (nullptr != theForm)
   {
      cxForm *iForm = (cxForm*)theForm;
      retval = toString(iForm->showModal());
   }

   return(retval);
} // cxFormShowModal

string cxBase::cxMenuShowModal(void* theMenu, void* unused)
{
   string retval = toString(cxID_QUIT);

   if (nullptr != theMenu)
   {
      cxMenu *iMenu = (cxMenu*)theMenu;
      retval = toString(iMenu->showModal());
   }

   return(retval);
} // cxFormShowModal

string cxBase::noOp(void *p1, void *p2)
{
   return("");
} // noOp

string cxBase::noOp(void *p1, void *p2, void *p3, void *p4)
{
   return("");
} // noOp

int cxBase::runSystemCmd(const string& pCmd)
{
   cxBase::cleanup();
   int retval = system(pCmd.c_str());
   cxBase::init();

   return(retval);
} // runSystemCmd

void cxBase::writeText(int pRow, int pCol, const string& pText,
                       attr_t pAttr, e_cxColors pColor)
                       {
   // Only do this if cxWidgets is initialized, or else the program will
   //  segfault.
   if (cxInitialized())
   {
      // TODO: Implement the color stuff
      // TODO: Check to see whther the attribute & color are already used in
      //  stdscr, and if so, don't add/remove them.
      // TODO: Maybe this should store the current cursor position and reset
      //  the cursor position back to where it was at the end of this function?
      wattron(stdscr, pAttr);
      mvwprintw(stdscr, pRow, pCol, "%s", pText.c_str());
      wrefresh(stdscr);
      wattroff(stdscr, pAttr);
   }
} // writeText

void cxBase::getHotkeyChars(const string& pStr, set<char>& pHotkeys,
                            bool pConvertCase, bool pToUpper)
                            {
   pHotkeys.clear();

   unsigned strLen = pStr.length();
   for (unsigned i = 0; i < strLen; ++i)
   {
      // Add the hotkey if the ampersand is not the last character
      //  and the next character is not a space.
      if ((pStr[i] == HOTKEY_CHAR) && (i+1 < strLen) && (pStr[i+1] != ' '))
      {
         // Get the hotkey character
         char hotkey = pStr[i+1];
         // Convert its case if we need to.
         if (pConvertCase)
         {
            if (pToUpper)
            {
               hotkey = toupper(hotkey);
            }
            else
            {
               hotkey = tolower(hotkey);
            }
         }
         // Insert the hotkey character into pHotkeys (sets don't
         //  allow duplicates, so we don't have to worry about
         //  checking if it's already in pHotkeys).
         pHotkeys.insert(hotkey);
      }
   }
} // getHotkeyChars

void cxBase::showAllWindows(bool pBringToTop)
{
   // Only do this if cxWidgets is initialized, or else the program will
   //  segfault.
   if (cxInitialized())
   {
      // Get a pointer to the bottom panel in the stack
      PANEL *panel = panel_above(nullptr);
      // For each panel, get the cxWindow object it points
      //  to, and show it.
      cxWindow *window = nullptr;
      while (panel != nullptr)
      {
         window = (cxWindow*)panel_userptr(panel);
         // The pointer returned by panel_userptr for the
         //  panels shouldn't be nullptr, but check just in
         //  case.
         if (window != nullptr)
         {
            // Show the window.  Don't show its subwindows
            //  (because they'll be shown too, eventually).
            window->show(pBringToTop, false);
         }

         panel = panel_above(panel);
      }
   }
} // showAllWindows

cxWindow* cxBase::getTopWindow()
{
   cxWindow *topWindow = nullptr;

   // With a nullptr parameter, panel_below() returns a pointer to the topmost
   //  panel in the stack.
   PANEL *panel = panel_below(nullptr);
   if (nullptr != panel)
   {
      topWindow = const_cast<cxWindow*>(static_cast<const cxWindow*>(panel_userptr(panel)));
   }

   return(topWindow);
} // getTopWindow

cxWindow* cxBase::getBottomWindow()
{
   cxWindow *bottomWindow = nullptr;

   // With a nullptr parameter, panel_above() returns a pointer to the bottom
   //  panel in the stack.
   PANEL *panel = panel_above(nullptr);
   if (nullptr != panel)
   {
      bottomWindow = const_cast<cxWindow*>(static_cast<const cxWindow*>(panel_userptr(panel)));
   }

   return(bottomWindow);
} // getTopWindow

string cxBase::getKeyStr(int pKey)
{
   string keyStr;
   switch(pKey)
   {
      case KEY_F(1):
         keyStr = "F1";
         break;
      case KEY_F(2):
         keyStr = "F2";
         break;
      case KEY_F(3):
         keyStr = "F3";
         break;
      case KEY_F(4):
         keyStr = "F4";
         break;
      case KEY_F(5):
         keyStr = "F5";
         break;
      case KEY_F(6):
         keyStr = "F6";
         break;
      case KEY_F(7):
         keyStr = "F7";
         break;
      case KEY_F(8):
         keyStr = "F8";
         break;
      case KEY_F(9):
         keyStr = "F9";
         break;
      case KEY_F(10):
         keyStr = "F10";
         break;
      case KEY_F(11):
         keyStr = "F11";
         break;
      case KEY_F(12):
         keyStr = "F12";
         break;
      case SHIFT_F1:
         keyStr = "sF1";
         break;
      case SHIFT_F2:
         keyStr = "sF2";
         break;
      case SHIFT_F3:
         keyStr = "sF3";
         break;
      case SHIFT_F4:
         keyStr = "sF4";
         break;
      case SHIFT_F5:
      case SHIFT_F52:
         keyStr = "sF5";
         break;
      case SHIFT_F6:
      case SHIFT_F62:
         keyStr = "sF6";
         break;
      case SHIFT_F7:
         keyStr = "sF7";
         break;
      case SHIFT_F8:
         keyStr = "sF8";
         break;
      case SHIFT_F9:
         keyStr = "sF9";
         break;
      case SHIFT_F10:
         keyStr = "sF10";
         break;
      case SHIFT_F11:
         keyStr = "sF11";
         break;
      case SHIFT_F12:
         keyStr = "sF12";
         break;
      case CTRL_F1:
         keyStr = "cF1";
         break;
      case CTRL_F2:
         keyStr = "cF2";
         break;
      case CTRL_F3:
         keyStr = "cF3";
         break;
      case CTRL_F4:
         keyStr = "cF4";
         break;
      case CTRL_F5:
         keyStr = "cF5";
         break;
      case CTRL_F6:
         keyStr = "cF6";
         break;
      case CTRL_F7:
         keyStr = "cF7";
         break;
      case CTRL_F8:
         keyStr = "cF8";
         break;
      case CTRL_F9:
         keyStr = "cF9";
         break;
      case CTRL_F10:
         keyStr = "cF10";
         break;
      case CTRL_F11:
         keyStr = "cF11";
         break;
      case CTRL_F12:
         keyStr = "cF12";
         break;
      case CTRL_SHIFT_F1:
         keyStr = "csF1";
         break;
      case CTRL_SHIFT_F2:
         keyStr = "csF2";
         break;
      case CTRL_SHIFT_F3:
         keyStr = "csF3";
         break;
      case CTRL_SHIFT_F4:
         keyStr = "csF4";
         break;
      case CTRL_SHIFT_F5:
         keyStr = "csF5";
         break;
      case CTRL_SHIFT_F6:
         keyStr = "csF6";
         break;
      case CTRL_SHIFT_F7:
         keyStr = "csF7";
         break;
      case CTRL_SHIFT_F8:
         keyStr = "csF8";
         break;
      case CTRL_SHIFT_F9:
         keyStr = "csF9";
         break;
      case CTRL_SHIFT_F10:
         keyStr = "csF10";
         break;
      case CTRL_SHIFT_F11:
         keyStr = "csF11";
         break;
      case CTRL_SHIFT_F12:
         keyStr = "csF12";
         break;
      case KEY_UP:
         keyStr = "Up";
         break;
      case KEY_DOWN:
         keyStr = "Dn";
         break;
      case KEY_RIGHT:
         keyStr = "Rt";
         break;
      case KEY_LEFT:
         keyStr = "Lt";
         break;
      case PAGE_DOWN:
         keyStr = "PgDn";
         break;
      case PAGE_UP:
         keyStr = "PgUp";
         break;
      case KEY_HOME:
      case KEY_SEND:  // sEND=HOME
         keyStr = "Home";
         break;
      case KEY_END:
      case KEY_END2:
      case KEY_END3:
      case KEY_SHOME: // sHOME=END
         keyStr = "End";
         break;
      case ENTER:
      case KEY_ENTER:
         keyStr = "Enter";
         break;
      case TAB:
         keyStr = "Tab";
         break;
      case SHIFT_TAB:
         keyStr = "sTab";
         break;
      case KEY_BACKSPACE:
      case BACKSPACE:
         keyStr = "Backspace";
         break;
      case ESC:
         keyStr = "ESC";
         break;
      case CTRL_G:
         keyStr = "ctrl-G";
         break;
      case NOKEY:
         keyStr = "NOKEY";
         break;
      case KEY_DC:
         keyStr = "Delete";
         break;
      case KEY_DL:
         keyStr = "DeleteLine";
         break;
      case KEY_IC:
         keyStr = "Insert";
         break;
#ifdef NCURSES_MOUSE_VERSION
      case KEY_MOUSE:
         keyStr = "Mouse";
         break;
#endif
      default:
         if (isPrintable(pKey))
         {
            keyStr = "";
            keyStr += pKey;
         }
         else
         {
            keyStr = cxStringUtils::toString(pKey);
         }
         break;
   }

   return(keyStr);
} // getKeyStr

string cxBase::getMouseStateStr(int pMouseState)
{
   string mouseStateStr;
   switch(pMouseState)
   {
#ifdef NCURSES_MOUSE_VERSION
      case BUTTON1_PRESSED:
         mouseStateStr = "Button 1 down";
         break;
      case BUTTON1_RELEASED:
         mouseStateStr = "Button 1 up";
         break;
      case BUTTON1_CLICKED:
         mouseStateStr = "Button 1 clicked";
         break;
      case BUTTON1_DOUBLE_CLICKED:
         mouseStateStr = "Button 1 double clicked";
         break;
      case BUTTON1_TRIPLE_CLICKED:
         mouseStateStr = "Button 1 triple clicked";
         break;
      case BUTTON2_PRESSED:
         mouseStateStr = "Button 2 down";
         break;
      case BUTTON2_RELEASED:
         mouseStateStr = "Button 2 up";
         break;
      case BUTTON2_CLICKED:
         mouseStateStr = "Button 2 clicked";
         break;
      case BUTTON2_DOUBLE_CLICKED:
         mouseStateStr = "Button 2 double clicked";
         break;
      case BUTTON2_TRIPLE_CLICKED:
         mouseStateStr = "Button 2 triple clicked";
         break;
      case BUTTON3_PRESSED:
         mouseStateStr = "Button 3 down";
         break;
      case BUTTON3_RELEASED:
         mouseStateStr = "Button 3 up";
         break;
      case BUTTON3_CLICKED:
         mouseStateStr = "Button 3 clicked";
         break;
      case BUTTON3_DOUBLE_CLICKED:
         mouseStateStr = "Button 3 double clicked";
         break;
      case BUTTON3_TRIPLE_CLICKED:
         mouseStateStr = "Button 3 triple clicked";
         break;
      case BUTTON4_PRESSED:
         mouseStateStr = "Button 4 down";
         break;
      case BUTTON4_RELEASED:
         mouseStateStr = "Button 4 up";
         break;
      case BUTTON4_CLICKED:
         mouseStateStr = "Button 4 clicked";
         break;
      case BUTTON4_DOUBLE_CLICKED:
         mouseStateStr = "Button 4 double clicked";
         break;
      case BUTTON4_TRIPLE_CLICKED:
         mouseStateStr = "Button 4 triple clicked";
         break;
#endif
      default:
         mouseStateStr = "Unknown (" + cxStringUtils::toString(pMouseState)
                       + ")";
         break;
   }

   return(mouseStateStr);
} // getMouseStateStr

string cxBase::getReturnCodeStr(long pReturnCode)
{
   string retval;

   switch(pReturnCode)
   {
      case cxID_OK:
         retval = "cxID_OK";
         break;
      case cxID_CANCEL:
         retval = "cxID_CANCEL";
         break;
      case cxID_EXIT:
         retval = "cxID_EXIT";
         break;
      case cxID_QUIT:
         retval = "cxID_QUIT";
         break;
      case cxFIRST_AVAIL_RETURN_CODE:
         retval = "cxFIRST_AVAIL_RETURN_CODE";
         break;
      default:
         retval = cxStringUtils::toString(pReturnCode);
         break;
   }

   return(retval);
} // getReturnCodeStr

string cxBase::getInputOptionStr(eInputOptions pInputOption)
{
   string retval;

   switch(pInputOption)
   {
      case eINPUT_EDITABLE:
         retval = "eINPUT_EDITABLE";
         break;
      case eINPUT_READONLY:
         retval = "eINPUT_READONLY";
         break;
      default:
         retval = cxStringUtils::toString(pInputOption);
         break;
   }

   return(retval);
} // getInputKindStr

string cxBase::getBorderStyleStr(eBorderStyle pBorderStyle)
{
   string retval;

   switch(pBorderStyle)
   {
      case eBS_NOBORDER:
         retval = "eBS_NOBORDER";
         break;
      case eBS_SINGLE_LINE:
         retval = "eBS_SINGLE_LINE";
         break;
      case eBS_SPACE:
         retval = "eBS_SPACE";
         break;
      default:
         retval = cxStringUtils::toString(pBorderStyle);
         break;
   }

   return(retval);
} // getBorderStyleStr

string cxBase::getInputTypeStr(eInputTypes pInputType)
{
   string retval;

   switch (pInputType)
   {
      case eINPUT_TYPE_TEXT:
         retval = "eINPUT_TYPE_TEXT";
         break;
      case eINPUT_TYPE_NUMERIC_FLOATING_PT:
         retval = "eINPUT_TYPE_NUMERIC_FLOATING_PT";
         break;
      case eINPUT_TYPE_NUMERIC_WHOLE:
         retval = "eINPUT_TYPE_NUMERIC_WHOLE";
         break;
      default:
         retval = cxStringUtils::toString((int)pInputType);
         break;
   }

   return(retval);
} // getInputTypeStr

void cxBase::signalHandler(int pSignal)
{
   switch (pSignal)
   {
      case SIGWINCH:  // Window attributes changed
         break;
   }
} // signalHandler

// TODO: There's a problem with this method.  If the user presses ESC instead
//  of choosing yes or no, this will return false, as if the user chose "No".
bool cxBase::promptYesNo(const string& pMessage, const string& pTitle,
                         long pButtons)
                         {
   bool retval = false;
   // Only do this if cxWidgets is initialized, or else the program will
   //  segfault.
   string title = pTitle;
   if (pTitle == "")
   {
      title = "Question";
   }
   if (cxInitialized())
   {
      cxMessageDialog iDlg(nullptr, 0, 0, 8, 25, title, pMessage,
            (eMessageDialogStyles)pButtons);
      iDlg.center(false);
      retval = (iDlg.showModal() == cxID_OK);
   }

   return(retval);
} // promptYesNo

bool cxBase::cxInitialized()
{
   return(cxBase::mInitialized);
} // cxInitialized

void cxBase::addAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &cxBase::mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &cxBase::mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &cxBase::mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &cxBase::mBorderAttrs;
         break;
      case eMENU_SELECTION: // Menu selection
         attrSet = &cxBase::mMenuSelectionAttrs;
         break;
      case eLABEL:          // Labels
         attrSet = &cxBase::mLabelAttrs;
         break;
      case eDATA:           // Data items (general)
      case eDATA_READONLY:  // Read-only data items
         attrSet = &cxBase::mDataReadonlyAttrs;
         break;
      case eDATA_EDITABLE:  // Editable data items
         attrSet = &cxBase::mDataEditableAttrs;
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &cxBase::mHotkeyAttrs;
         break;
   }

   // Insert the attribute, if attrSet was set.
   if (nullptr != attrSet)
   {
      attrSet->insert(pAttr);
   }
} // addAttr

void cxBase::setAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &cxBase::mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &cxBase::mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &cxBase::mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &cxBase::mBorderAttrs;
         break;
      case eMENU_SELECTION: // Menu selection
         attrSet = &cxBase::mMenuSelectionAttrs;
         break;
      case eLABEL:          // Labels
         attrSet = &cxBase::mLabelAttrs;
         break;
      case eDATA_READONLY:  // Read-only data items
         attrSet = &cxBase::mDataReadonlyAttrs;
         break;
      case eDATA:           // Data items (general)
      case eDATA_EDITABLE:  // Editable data items
         attrSet = &cxBase::mDataEditableAttrs;
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &cxBase::mHotkeyAttrs;
         break;
   }

   // Set the attribute, if attrSet was set.
   if (nullptr != attrSet)
   {
      attrSet->clear();
      attrSet->insert(pAttr);
   }
} // setAttr

void cxBase::removeAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &cxBase::mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &cxBase::mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &cxBase::mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &cxBase::mBorderAttrs;
         break;
      case eMENU_SELECTION: // Menu selection
         attrSet = &cxBase::mMenuSelectionAttrs;
         break;
      case eLABEL:          // Labels
         attrSet = &cxBase::mLabelAttrs;
         break;
      case eDATA:           // Data items (general)
      case eDATA_READONLY:  // Read-only data items
         attrSet = &cxBase::mDataReadonlyAttrs;
         break;
      case eDATA_EDITABLE:  // Editable data items
         attrSet = &cxBase::mDataEditableAttrs;
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &cxBase::mHotkeyAttrs;
         break;
   }

   // Remove the attribute, if attrSet was set.
   if (nullptr != attrSet)
   {
      attrSet->erase(pAttr);
   }
} // removeAttr

void cxBase::removeAttrs(e_WidgetItems pItem)
{
   // attrSet is a pointer that will be set to point to the correct attribute
   //  set, depending on the value of pItem.
   set<attr_t>* attrSet = nullptr;

   switch(pItem)
   {
      case eMESSAGE:        // Message
         attrSet = &cxBase::mMessageAttrs;
         break;
      case eTITLE:          // Title
         attrSet = &cxBase::mTitleAttrs;
         break;
      case eSTATUS:         // Status
         attrSet = &cxBase::mStatusAttrs;
         break;
      case eBORDER:         // Borders
         attrSet = &cxBase::mBorderAttrs;
         break;
      case eMENU_SELECTION: // Menu selection
         attrSet = &cxBase::mMenuSelectionAttrs;
         break;
      case eLABEL:          // Labels
         attrSet = &cxBase::mLabelAttrs;
         break;
      case eDATA:           // Data items (general)
      case eDATA_READONLY:  // Read-only data items
         attrSet = &cxBase::mDataReadonlyAttrs;
         break;
      case eDATA_EDITABLE:  // Editable data items
         attrSet = &cxBase::mDataEditableAttrs;
         break;
      case eHOTKEY:         // Hotkey
         attrSet = &cxBase::mHotkeyAttrs;
         break;
   }

   // Remove all of the attributes, if attrSet was set.
   if (nullptr != attrSet)
   {
      attrSet->clear();
   }
} // removeAttrs

void cxBase::getAttrs(e_WidgetItems pItem, set<attr_t>& pAttrs)
{
   pAttrs.clear();

   switch(pItem)
   {
      case eMESSAGE:        // Message
         pAttrs = cxBase::mMessageAttrs;
         break;
      case eTITLE:          // Title
         pAttrs = cxBase::mTitleAttrs;
         break;
      case eSTATUS:         // Status
         pAttrs = cxBase::mStatusAttrs;
         break;
      case eBORDER:         // Borders
         pAttrs = cxBase::mBorderAttrs;
         break;
      case eMENU_SELECTION: // Menu selection
         pAttrs = cxBase::mMenuSelectionAttrs;
         break;
      case eLABEL:          // Labels
         pAttrs = cxBase::mLabelAttrs;
         break;
      case eDATA:           // Data items (general)
      case eDATA_READONLY:  // Read-only data items
         pAttrs = cxBase::mDataReadonlyAttrs;
         break;
      case eDATA_EDITABLE:  // Editable data items
         pAttrs = cxBase::mDataEditableAttrs;
         break;
      case eHOTKEY:         // Hotkey
         pAttrs = cxBase::mHotkeyAttrs;
         break;
   }
} // getAttrs

bool cxBase::hasAttr(e_WidgetItems pItem, attr_t pAttr)
{
   bool retval=false;
   set<attr_t> attrs;
   getAttrs(pItem, attrs);
   retval = (attrs.find(pAttr) != attrs.end());
   //messageBox("hassAttr:" + string(retval ? "true" : "false"));
   return(retval);
} // hasAttr

void cxBase::enableAttrs(WINDOW *pWindow, e_WidgetItems pItem)
{
   if (nullptr != pWindow)
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;

      switch(pItem)
      {
         case eMESSAGE:        // Message
            attrSet = &cxBase::mMessageAttrs;
            break;
         case eTITLE:          // Title
            attrSet = &cxBase::mTitleAttrs;
            break;
         case eSTATUS:         // Status
            attrSet = &cxBase::mStatusAttrs;
            break;
         case eBORDER:         // Borders
            attrSet = &cxBase::mBorderAttrs;
            break;
         case eMENU_SELECTION: // Menu selection
            attrSet = &cxBase::mMenuSelectionAttrs;
            break;
         case eLABEL:          // Labels
            attrSet = &cxBase::mLabelAttrs;
            break;
         case eDATA:           // Data items (general)
         case eDATA_READONLY:  // Read-only data items
            attrSet = &cxBase::mDataReadonlyAttrs;
            break;
         case eDATA_EDITABLE:  // Editable data items
            attrSet = &cxBase::mDataEditableAttrs;
            break;
         case eHOTKEY:         // Hotkey
            attrSet = &cxBase::mHotkeyAttrs;
            break;
      }

      // Enable the attributes, if attrSet was set.
      if (nullptr != attrSet)
      {
         set<attr_t>::const_iterator iter = attrSet->begin();
         for (; iter != attrSet->end(); ++iter)
         {
            wattron(pWindow, *iter);
         }
      }
   }
} // enableAttrs

void cxBase::disableAttrs(WINDOW *pWindow, e_WidgetItems pItem)
{
   if (nullptr != pWindow)
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;

      switch(pItem)
      {
         case eMESSAGE:        // Message
            attrSet = &cxBase::mMessageAttrs;
            break;
         case eTITLE:          // Title
            attrSet = &cxBase::mTitleAttrs;
            break;
         case eSTATUS:         // Status
            attrSet = &cxBase::mStatusAttrs;
            break;
         case eBORDER:         // Borders
            attrSet = &cxBase::mBorderAttrs;
            break;
         case eMENU_SELECTION: // Menu selection
            attrSet = &cxBase::mMenuSelectionAttrs;
            break;
         case eLABEL:          // Labels
            attrSet = &cxBase::mLabelAttrs;
            break;
         case eDATA:           // Data items (general)
         case eDATA_READONLY:  // Read-only data items
            attrSet = &cxBase::mDataReadonlyAttrs;
            break;
         case eDATA_EDITABLE:  // Editable data items
            attrSet = &cxBase::mDataEditableAttrs;
            break;
         case eHOTKEY:         // Hotkey
            attrSet = &cxBase::mHotkeyAttrs;
            break;
      }

      // Disable the attributes, if attrSet was set.
      if (nullptr != attrSet)
      {
         set<attr_t>::const_iterator iter = attrSet->begin();
         for (; iter != attrSet->end(); ++iter)
         {
            wattroff(pWindow, *iter);
         }
      }
   }
} // disableAttrs

attr_t cxBase::getAttr(const string& pAttr)
{
   attr_t retval=A_NORMAL;
   if (pAttr == "NORMAL")
   {
      retval=A_NORMAL;
   }
   else if (pAttr == "STANDOUT")
   {
      retval=A_STANDOUT;
   }
   else if (pAttr == "UNDERLINE")
   {
      retval=A_UNDERLINE;
   }
   else if (pAttr == "REVERSE")
   {
      retval=A_REVERSE;
   }
   else if (pAttr == "DIM")
   {
      retval=A_DIM;
   }
   else if (pAttr == "BOLD")
   {
      retval=A_BOLD;
   }
   return(retval);
} // getAttr

string cxBase::getAttrStr(attr_t pAttr)
{
   string attrStr = "Unknown";

   switch (pAttr)
   {
      case A_NORMAL:
         attrStr = "NORMAL";
         break;
      case A_STANDOUT:
         attrStr = "STANDOUT";
         break;
      case A_UNDERLINE:
         attrStr = "UNDERLINE";
         break;
      case A_REVERSE:
         attrStr = "REVERSE";
         break;
      case A_DIM:
         attrStr = "DIM";
         break;
      case A_BOLD:
         attrStr = "BOLD";
         break;
      default:
         break;
   }

   return(attrStr);
} // getAttrStr

string cxBase::getWidgetItemStr(e_WidgetItems pItem)
{
   string itemStr;

   switch (pItem)
   {
      case eMESSAGE:        // Message
         itemStr = "eMESSAGE";
         break;
      case eTITLE:          // Title
         itemStr = "eTITLE";
         break;
      case eSTATUS:         // Status
         itemStr = "eSTATUS";
         break;
      case eBORDER:         // Borders
         itemStr = "eBORDER";
         break;
      case eMENU_SELECTION: // Selected menu items
         itemStr = "eMENU_SELECTION";
         break;
      case eLABEL:          // Input labels
         itemStr = "eLABEL";
         break;
      case eDATA_READONLY:  // Read-only data items
         itemStr = "eDATA_READONLY";
         break;
      case eDATA_EDITABLE:  // Editable data items
         itemStr = "eDATA_EDITABLE";
         break;
      case eDATA:           // Data items (general)
         itemStr = "eDATA";
         break;
      case eHOTKEY:         // Hotkey
         itemStr = "eHOTKEY";
         break;
   }

   return(itemStr);
} // getWidgetItemStr

void cxBase::setMenuClearKeywordKey(int pKey)
{
   cxBase::mMenuClearKeywordKey = pKey;
} // setMenuClearKeywordKey

int cxBase::getMenuClearKeywordKey()
{
   return(cxBase::mMenuClearKeywordKey);
} // getMenuClearKeywordKey

// Note: This works, but prints even HIDDEN panel's border characters.
string cxBase::dump(bool pFancy, const string& pFilename)
{
   string retval;

   string filename="temp.scn";
   if (pFilename != "")
   {
      filename=pFilename;
   }

   cxBase::updateWindows();

   const int mainHeight=height();
   const int mainWidth=width();
   //char theScreen[mainHeight][mainWidth];
   vector<vector <char> > theScreen;
   vector<char> arow;

   // set all of the screen characters to spaces
   arow.assign(mainWidth, ' '); // Give the row mainWidth spaces
   for (int row=0; row < mainHeight; ++row)
   {
      theScreen.push_back(arow);
   }

   ofstream theFile;
   theFile.open(filename.c_str());

   char c;
   attr_t u;
   bool hasBorder=false;
   bool doBorder=true;

   if (theFile.good())
   {
      // Allocate an array to store a line from the screen, with the longest
      //  length we'd need (+1 to account for the nullptr termination character).
      chtype *line = new chtype[width() + 1];
      if (nullptr != line)
      {
         // Get a pointer to the bottom panel in the stack
         cxWindow* cxWindowPtr = nullptr;
         // For each panel, get the cxWindow object it points
         //  to, and show it.
         PANEL *panel = panel_above(nullptr);
         while (panel != nullptr)
         {
            cxWindowPtr = (cxWindow*)panel_userptr(panel);
            // If the window isn't hidden, grab each character from it and update
            //  the corresponding character in theScreen (which represents the
            //  whole screen)
            if (!cxWindowPtr->isHidden())
            {
               try
               {
                  WINDOW *windowPtr=panel_window(panel);
                  hasBorder = cxWindowPtr->hasBorder();

                  const int lineLength = cxWindowPtr->width();
                  if (nullptr != line)
                  {
                     int bottomRow = cxWindowPtr->height();
                     int numChars = 0; // # of chars read for each line

                     for (int row = 0; row < bottomRow; ++row)
                     {
                        numChars = mvwinchnstr(windowPtr, row, 0, line, lineLength);
                        if (numChars != ERR)
                        {
                           // in order to extract the characters from line, we have to AND each one with A_CHARTEXT.
                           for (int col = 0; col < numChars; ++col)
                           {

                              c=line[col] & A_CHARTEXT;

                              // If pFancy and the attribute is "underline" and the value
                              // is a space ' ', then set it to an underline
                              if (pFancy)
                              {
                                 u=line[col] & A_ATTRIBUTES;
                                 if (c == ' ')
                                 {
                                    if (u & A_UNDERLINE)
                                    {
                                       c='_';
                                    }
                                 }
                              }

                              if ((hasBorder) && (doBorder))
                              {
                                 if (col == 0)
                                 {
                                    if (row == 0)
                                    {
                                       c = '+' ; // upper left
                                    }
                                    else
                                    {
                                       if (row == bottomRow-1)
                                       {
                                          c = '+' ; // lower left
                                       }
                                       else
                                       {
                                          c = '|'; // left border
                                       }
                                    }
                                 }
                                 else if (col == lineLength-1)
                                 {
                                    if (row == 0)
                                    {
                                       c = '+' ; // upper right
                                    }
                                    else
                                    {
                                       if (row == bottomRow-1)
                                       {
                                          c = '+' ; // bottom right
                                       }
                                       else
                                       {
                                          c = '|'; // right border
                                       }
                                    }
                                 }
                                 else
                                 {
                                    if ((row == 0) || (row == bottomRow-1))
                                    {
                                       c = '-' ; // horizontal character
                                    }
                                 }
                              }
                              theScreen[cxWindowPtr->top()+row][cxWindowPtr->left()+col]=c;
                           }
                        }
                     }
                  }

                  // re-do the tile/status
                  if (hasBorder)
                  {

                     // title line
                     map<int, string>titles = cxWindowPtr->getTitleStrings();
                     map<int, string>::iterator titleIter=titles.begin();
                     for (; titleIter != titles.end(); ++titleIter)
                     {
                        string tmp=titleIter->second;
                        for (unsigned i=0; i < tmp.length(); i++)
                        {
                           theScreen[cxWindowPtr->top()][cxWindowPtr->left()+titleIter->first+i]=tmp[i];
                        }
                     }

                     // status line
                     map<int, string>statii = cxWindowPtr->getStatusStrings();
                     map<int, string>::iterator statusIter=statii.begin();
                     for (; statusIter != statii.end(); ++statusIter)
                     {
                        string tmp=statusIter->second;
                        for (unsigned i=0; i < tmp.length(); i++)
                        {
                           theScreen[cxWindowPtr->bottom()][cxWindowPtr->left()+statusIter->first+i]=tmp[i];
                        }
                     }
                  }
               }
               catch (const std::bad_alloc& e)
               {
                  // Couldn't dynamically allocate some memory..
               }
               catch (const std::exception& e)
               {
                  // Oh No!
               }
            }

            // get the next panel
            panel = panel_above(panel);
         }
         // Free the memory used  by line
         delete [] line;
         line = nullptr;
      }

      // dump theScreen to the file
      for (int row=0; row < mainHeight; ++row)
      {
         for (int col=0; col < mainWidth; ++col)
         {
            theFile << theScreen[row][col];
         }
         theFile << endl;
      }
      theFile.close();
   }
   else
   {
      retval="Warning: Could not write to file(" + filename + ")";
   }

   return(retval);
} // dump

/*
// NOTE: This works, but does not print border characters.
string cxBase::dump(const string& pFilename) {
   string retval;

   string filename="temp.scn";
   if (pFilename != "") {
      filename=pFilename;
   }

   ofstream theFile;
   theFile.open(filename.c_str());

   char c;
   chtype ch;

   if (theFile.good()) {

      const int lineLength = width();
      chtype line[lineLength];  // To hold each line
      int bottomRow = height();
      int numChars = 0; // # of chars read for each line

      for (int row = 0; row < bottomRow; ++row) {

         numChars = mvwinchnstr(curscr, row, 0, line, lineLength);
         if (numChars != ERR) {
            // in order to extract the characters from line, we have to AND each one with A_CHARTEXT.
            for (int col = 0; col < numChars; ++col) {
               c=line[col] & A_CHARTEXT;
               theFile << c;
            }
            theFile << endl;
         }
      }

      theFile.close();
   }
   else {
      retval="Warning: Could not write to file(" + filename + ")";
   }

   return(retval);
} // dump
*/

bool cxBase::isNavKey(int pKey)
{
   return(mNavKeys.find(pKey) != mNavKeys.end());
} // isNavigationalKey

void cxBase::addNavKey(int pKey)
{
   mNavKeys.insert(pKey);
} // addNavKey

void cxBase::removeNavKey(int pKey)
{
   mNavKeys.erase(pKey);
} // removeNavKey

string cxBase::getString(int pRow, int pCol, int pNumber, WINDOW *pWin)
{
   string retval;

   if (pWin == nullptr)
   {
      pWin = curscr;
   }

   // Ensure we don't go looking for info outside of our own window
   if ((pRow >= 0) && (pCol >= 0))
   {

      // Ensure they want *some* characters
      if (pNumber > 0)
      {
         chtype * line = new chtype[pNumber + 1]; // nullptr terminated
         int numChars = mvwinchnstr(pWin, pRow, pCol, line, pNumber);
         if (numChars == pNumber)
         {
            for (int i=0; i < pNumber; ++i) { // eliminate the nullptr at the end
               retval.push_back((line[i] & A_CHARTEXT));
            }
         }
         delete [] line;
      }
   }

   return(retval);
} // getString

bool cxBase::findString(const string& pSearch, int& pRow, int& pCol, WINDOW *pWin)
{
   bool retval=false;
   pRow=-1;
   pCol=-1;

   if (pWin == nullptr)
   {
      pWin = curscr;
   }

   chtype * line = new chtype[width() + 1]; // nullptr terminated
   size_t pos = 0;
   string lineString;

   for (int row=0; row < height(); ++row)
   {

      // reset the line
      lineString="";

      // go get the characters from the screen
      (void)mvwinchnstr(pWin, row, 0, line, width());

      // move them into a std::string
      for (int i=0; i < width(); ++i) { // eliminate the nullptr at the end
         lineString.push_back((line[i] & A_CHARTEXT)); // only get the text value
      }

      // search
      pos=lineString.find(pSearch);
      if (pos != string::npos)
      {
         // found!
         pRow=row;
         pCol=pos;
         retval=true;
      }

   }
   delete [] line;

   return(retval);
} // findString
