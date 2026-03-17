// Copyright (c) 2026 Eric Oulashin
//
// cxDatePicker.cpp - Implementation of the cxDatePicker dialog.

#include "cxDatePicker.h"
#include "cxKeyDefines.h"
#include "cxReturnCodes.h"
#include "cxBase.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

using std::string;
using std::vector;
using std::make_shared;

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

cxDatePicker::cxDatePicker(cxWindow *pParentWindow, int pRow, int pCol, const cxDate& pDate)
   : cxWindow(pParentWindow,
              (pRow == -1) ? (LINES  - PICKER_HEIGHT) / 2 : pRow,
              (pCol == -1) ? (COLS   - PICKER_WIDTH)  / 2 : pCol,
              PICKER_HEIGHT,
              PICKER_WIDTH,
              " Select Date",
              "",
              "",
              eBS_SINGLE_LINE),
     mSelectedDate(pDate),
     mDisplayDate(pDate),
     mFocusDay(pDate.getDay())
{
   initWidgets();
}

cxDatePicker::cxDatePicker(int pYear, int pMonth, int pDay,
                           cxWindow *pParentWindow, int pRow,
                           int pCol)
: cxWindow(pParentWindow,
              (pRow == -1) ? (LINES  - PICKER_HEIGHT) / 2 : pRow,
              (pCol == -1) ? (COLS   - PICKER_WIDTH)  / 2 : pCol,
              PICKER_HEIGHT,
              PICKER_WIDTH,
              " Select Date",
              "",
              "",
              eBS_SINGLE_LINE),
     mFocusDay(pDay)
{
   if (!mSelectedDate.setDate(pYear, pMonth, pDay))
   {
      mSelectedDate = cxDate::today();   // fall back to today if date is invalid
      mFocusDay = mSelectedDate.getDay();
   }
   mDisplayDate = mSelectedDate;
   initWidgets();
}

cxDatePicker::~cxDatePicker()
{
   // Child widgets are shared_ptr; they unregister themselves from the
   // parent automatically via cxWindow's destructor chain.
}

// ---------------------------------------------------------------------------
// cxWindow overrides
// ---------------------------------------------------------------------------

string cxDatePicker::cxTypeStr() const
{
   return "cxDatePicker";
}

void cxDatePicker::draw()
{
   cxWindow::draw();      // border, title
   // Restore the window's blue background colour so inner content matches
   wcolor_set(mWindow, mMessageColorPair, nullptr);
   drawContent();         // calendar, separator, header, month label
}

long cxDatePicker::showModal(bool pShowSelf, bool pBringToTop,
                             bool pShowSubwindows)
{
   if (!isEnabled()) return cxID_EXIT;

   mIsModal = true;
   long returnCode = cxID_CANCEL;

   if (!runOnFocusFunction() && !getLeaveNow())
   {
      // Sync year input and month combo to mDisplayDate
      std::ostringstream yearSS;
      yearSS << mDisplayDate.getYear();
      mYearInput->setValue(yearSS.str());
      if (mMonthCombo)
      {
         mMonthCombo->setValue(cxDate::monthName(mDisplayDate.getMonth()));
      }

      if (pShowSelf)
      {
         show(pBringToTop, pShowSubwindows);
         // These widgets are shown only when the user activates them
         if (mMonthCombo) mMonthCombo->hide();
         if (mYearInput)  mYearInput->hide();
      }

      // Set initial visual focus to calendar
      setFocusItem(mFocusItem);

      int prevCursor = curs_set(0);
      returnCode = doInputLoop();
      curs_set(prevCursor);

      hide();
      runOnLeaveFunction();
   }

   mLeaveNow = false;
   mIsModal  = false;
   return returnCode;
}

void cxDatePicker::hide(bool pHideSubwindows)
{
   cxWindow::hide(pHideSubwindows);
   if (mMonthCombo) mMonthCombo->hide(false);
   if (mYearInput)  mYearInput->hide(false);
   if (mPrevBtn)    mPrevBtn->hide(false);
   if (mNextBtn)    mNextBtn->hide(false);
   if (mCancelBtn)  mCancelBtn->hide(false);
   if (mOKBtn)      mOKBtn->hide(false);
}

void cxDatePicker::unhide(bool pUnhideSubwindows)
{
   if (!isEnabled()) return;
   cxWindow::unhide(pUnhideSubwindows);
   // mMonthCombo and mYearInput stay hidden until the user activates them
   if (mMonthCombo) mMonthCombo->hide(false);
   if (mYearInput)  mYearInput->hide(false);
   if (mPrevBtn)    mPrevBtn->unhide(false);
   if (mNextBtn)    mNextBtn->unhide(false);
   if (mCancelBtn)  mCancelBtn->unhide(false);
   if (mOKBtn)      mOKBtn->unhide(false);
}

bool cxDatePicker::move(int pNewRow, int pNewCol, bool pRefresh)
{
   int origTop  = top();
   int origLeft = left();
   bool moved = cxWindow::move(pNewRow, pNewCol, pRefresh);
   if (moved)
   {
      int dy = top()  - origTop;
      int dx = left() - origLeft;
      if (mMonthCombo) mMonthCombo->moveRelative(dy, dx, pRefresh);
      if (mYearInput)  mYearInput->moveRelative(dy, dx, pRefresh);
      if (mPrevBtn)    mPrevBtn->moveRelative(dy, dx, pRefresh);
      if (mNextBtn)    mNextBtn->moveRelative(dy, dx, pRefresh);
      if (mCancelBtn)  mCancelBtn->moveRelative(dy, dx, pRefresh);
      if (mOKBtn)      mOKBtn->moveRelative(dy, dx, pRefresh);
   }
   return moved;
}

// ---------------------------------------------------------------------------
// Public interface
// ---------------------------------------------------------------------------

void cxDatePicker::setDate(const cxDate& pDate)
{
   mSelectedDate = pDate;
   mDisplayDate  = pDate;
   mFocusDay     = pDate.getDay();
   if (mMonthCombo)
   {
      mMonthCombo->setValue(cxDate::monthName(pDate.getMonth()));
   }
}

// ---------------------------------------------------------------------------
// Private: widget creation
// ---------------------------------------------------------------------------

void cxDatePicker::initWidgets()
{
   // Month combo box (read-only drop-down, hidden until activated)
   mMonthCombo = make_shared<cxComboBox>(
      this,
      top() + ROW_CTRL_MID,
      left() + COL_MONTH_COMBO,
      MONTH_COMBO_H,
      MONTH_COMBO_W,
      "",              // no label
      eBS_NOBORDER,
      eINPUT_READONLY
   );
   for (int m = 1; m <= 12; ++m)
   {
      mMonthCombo->appendToMenu(cxDate::monthName(m));
   }
   mMonthCombo->setValue(cxDate::monthName(mDisplayDate.getMonth()));
   mMonthCombo->hide();

   // Year input (4 digits, no label, no border) — kept hidden until activated,
   // like mMonthCombo; the year value is drawn directly in the parent window.
   mYearInput = make_shared<cxInput>(
      this,
      top() + ROW_CTRL_MID,
      left() + COL_YEAR_INPUT,
      YEAR_INPUT_W,
      "",            // no label
      eBS_NOBORDER,
      eINPUT_EDITABLE,
      true           // exit on full (4 chars)
   );
   mYearInput->hide();

   // [<] Previous-month button
   mPrevBtn = make_shared<cxButton>(
      this,
      top() + ROW_CTRL_TOP,
      left() + COL_PREV_BTN,
      NAV_BTN_H,
      NAV_BTN_W,
      "<"
   );

   // [>] Next-month button
   mNextBtn = make_shared<cxButton>(
      this,
      top() + ROW_CTRL_TOP,
      left() + COL_NEXT_BTN,
      NAV_BTN_H,
      NAV_BTN_W,
      ">"
   );

   // [Cancel] button
   mCancelBtn = make_shared<cxButton>(
      this,
      top() + ROW_BTN_TOP,
      left() + COL_CANCEL_BTN,
      3,
      CANCEL_BTN_W,
      "&Cancel"
   );
   mCancelBtn->setHotkeyHighlighting(true);

   // [OK] button
   mOKBtn = make_shared<cxButton>(
      this,
      top() + ROW_BTN_TOP,
      left() + COL_OK_BTN,
      3,
      OK_BTN_W,
      "&OK"
   );
   mOKBtn->setHotkeyHighlighting(true);
}

// ---------------------------------------------------------------------------
// Private: drawing
// ---------------------------------------------------------------------------

void cxDatePicker::drawContent()
{
   if (!mWindow) return;

   // Use the window's blue background colour for all inner content so that
   // the header, separator, month label and calendar all match the border.
   wcolor_set(mWindow, mMessageColorPair, nullptr);

   const int year  = mDisplayDate.getYear();
   const int month = mDisplayDate.getMonth();

   // Row 1: selected-date header  e.g. "Mon, March 16, 2026"
   {
      cxDate header(year, month, mFocusDay);
      string dow = header.dayOfWeekStr().substr(0, 3); // "Mon"
      string mon = cxDate::monthName(month);
      std::ostringstream oss;
      oss << dow << ", " << mon << " " << mFocusDay << ", " << year;
      string hdr = oss.str();
      mvwhline(mWindow, ROW_DATE_HEADER, 1, ' ', width() - 2);
      mvwaddstr(mWindow, ROW_DATE_HEADER, COL_CAL, hdr.c_str());
   }

   // Row 2: horizontal separator
   mvwhline(mWindow, ROW_SEPARATOR, 1, ACS_HLINE, width() - 2);
   mvwaddch(mWindow, ROW_SEPARATOR, 0,          ACS_LTEE);
   mvwaddch(mWindow, ROW_SEPARATOR, width() - 1, ACS_RTEE);

   // Controls row (ROW_CTRL_MID): month name and year, drawn directly in the
   // parent window (both widgets stay hidden until the user activates them).
   // A_REVERSE highlights the entire field area when it has keyboard focus.
   {
      string monName = cxDate::monthName(month);
      if (mFocusItem == FocusItem::MONTH_COMBO)
      {
         wattron(mWindow, A_REVERSE);
      }
      // Fill the whole combo area (reverse or normal), then write name left-aligned
      mvwhline(mWindow, ROW_CTRL_MID, COL_MONTH_COMBO, ' ', MONTH_COMBO_W);
      mvwaddstr(mWindow, ROW_CTRL_MID, COL_MONTH_COMBO, monName.c_str());
      if (mFocusItem == FocusItem::MONTH_COMBO)
      {
         wattroff(mWindow, A_REVERSE);
      }
   }
   {
      std::ostringstream yearSS;
      yearSS << mDisplayDate.getYear();
      string yearStr = yearSS.str();
      if (mFocusItem == FocusItem::YEAR_INPUT)
      {
         wattron(mWindow, A_REVERSE);
      }
      mvwhline(mWindow, ROW_CTRL_MID, COL_YEAR_INPUT, ' ', YEAR_INPUT_W);
      mvwaddstr(mWindow, ROW_CTRL_MID, COL_YEAR_INPUT, yearStr.c_str());
      if (mFocusItem == FocusItem::YEAR_INPUT)
      {
         wattroff(mWindow, A_REVERSE);
      }
   }

   // Row 6: day-of-week headers
   mvwaddstr(mWindow, ROW_DAY_HEADERS, COL_CAL,
             " Su Mo Tu We Th Fr Sa");

   // Rows 7–12: calendar grid
   const int firstDow = static_cast<int>(
      cxDate::dayOfWeek(year, month, 1));  // 0=Sunday
   const int numDays  = cxDate::numMonthDays(month, year);

   // Clear the 6 calendar rows first
   for (int r = ROW_CAL_FIRST; r <= ROW_CAL_LAST; ++r)
   {
      mvwhline(mWindow, r, 1, ' ', width() - 2);
   }

   for (int day = 1; day <= numDays; ++day)
   {
      int pos     = day - 1 + firstDow;   // 0-based position in grid
      int winRow  = ROW_CAL_FIRST + pos / 7;
      int winCol  = COL_CAL + (pos % 7) * DAY_CELL_W;

      bool isFocused  = (day == mFocusDay);
      bool isSelected = (mDisplayDate.getYear()  == mSelectedDate.getYear()  &&
                         mDisplayDate.getMonth() == mSelectedDate.getMonth() &&
                         day == mSelectedDate.getDay());

      if (isFocused)
      {
         wattron(mWindow, A_REVERSE);
      }
      else if (isSelected)
      {
         wattron(mWindow, A_BOLD);
      }

      mvwprintw(mWindow, winRow, winCol, " %2d", day);

      if (isFocused)
      {
         wattroff(mWindow, A_REVERSE);
      }
      else if (isSelected)
      {
         wattroff(mWindow, A_BOLD);
      }
   }

   wnoutrefresh(mWindow);
}

void cxDatePicker::redrawDay(int pDay)
{
   if (!mWindow || pDay < 1) return;
   const int numDays = cxDate::numMonthDays(
      mDisplayDate.getMonth(), mDisplayDate.getYear());
   if (pDay > numDays) return;

   int winRow, winCol;
   getDayWinPos(pDay, winRow, winCol);

   wcolor_set(mWindow, mMessageColorPair, nullptr);

   bool isFocused  = (pDay == mFocusDay);
   bool isSelected = (mDisplayDate.getYear()  == mSelectedDate.getYear()  &&
                      mDisplayDate.getMonth() == mSelectedDate.getMonth() &&
                      pDay == mSelectedDate.getDay());

   if (isFocused)
   {
      wattron(mWindow, A_REVERSE);
   }
   else if (isSelected)
   {
      wattron(mWindow, A_BOLD);
   }

   mvwprintw(mWindow, winRow, winCol, " %2d", pDay);

   if (isFocused)       wattroff(mWindow, A_REVERSE);
   else if (isSelected) wattroff(mWindow, A_BOLD);

   wnoutrefresh(mWindow);
}

void cxDatePicker::getDayWinPos(int pDay, int& pWinRow, int& pWinCol) const
{
   const int firstDow = static_cast<int>(
      cxDate::dayOfWeek(mDisplayDate.getYear(), mDisplayDate.getMonth(), 1));
   int pos     = pDay - 1 + firstDow;
   pWinRow     = ROW_CAL_FIRST + pos / 7;
   pWinCol     = COL_CAL + (pos % 7) * DAY_CELL_W;
}

int cxDatePicker::getDayAtScreenPos(int pScreenRow, int pScreenCol) const
{
   // Check within calendar row band
   int firstCalScreenRow = top() + ROW_CAL_FIRST;
   int lastCalScreenRow  = top() + ROW_CAL_LAST;
   if (pScreenRow < firstCalScreenRow || pScreenRow > lastCalScreenRow) return 0;

   // Check within calendar column band
   int calLeftScreen  = left() + COL_CAL;
   int calRightScreen = left() + COL_CAL + 7 * DAY_CELL_W;
   if (pScreenCol < calLeftScreen || pScreenCol >= calRightScreen) return 0;

   int week    = pScreenRow - firstCalScreenRow;
   int dayCol  = (pScreenCol - calLeftScreen) / DAY_CELL_W;
   int firstDow = static_cast<int>(
      cxDate::dayOfWeek(mDisplayDate.getYear(), mDisplayDate.getMonth(), 1));
   int day = week * 7 + dayCol - firstDow + 1;

   int numDays = cxDate::numMonthDays(
      mDisplayDate.getMonth(), mDisplayDate.getYear());
   if (day < 1 || day > numDays) return 0;

   return day;
}

// ---------------------------------------------------------------------------
// Private: focus management
// ---------------------------------------------------------------------------

void cxDatePicker::highlightButton(std::shared_ptr<cxButton>& pBtn, bool pOn)
{
   if (!pBtn) return;
   attr_t attr = cxBase::hasAttr(eHOTKEY, A_STANDOUT) ? A_REVERSE : A_STANDOUT;
   if (pOn)
   {
      pBtn->setFocusColors(attr);
   }
   else
   {
      pBtn->unSetFocusColors(attr);
   }
   pBtn->show(false, false);
}

void cxDatePicker::setFocusItem(FocusItem pItem)
{
   // Remove highlight from all buttons
   highlightButton(mPrevBtn,   false);
   highlightButton(mNextBtn,   false);
   highlightButton(mCancelBtn, false);
   highlightButton(mOKBtn,     false);

   mFocusItem = pItem;

   switch (pItem)
   {
      case FocusItem::PREV_BTN:   highlightButton(mPrevBtn,   true); break;
      case FocusItem::NEXT_BTN:   highlightButton(mNextBtn,   true); break;
      case FocusItem::CANCEL_BTN: highlightButton(mCancelBtn, true); break;
      case FocusItem::OK_BTN:     highlightButton(mOKBtn,     true); break;
      default: break;
   }

   // Ensure all buttons are above the picker in z-order after show() calls
   // may have shuffled the panel stack. Then redraw content and composite.
   if (mPrevBtn)    mPrevBtn->bringToTop(false);
   if (mNextBtn)    mNextBtn->bringToTop(false);
   if (mCancelBtn)  mCancelBtn->bringToTop(false);
   if (mOKBtn)      mOKBtn->bringToTop(false);

   // Redraw content so the month-label highlight reflects the new focus state
   drawContent();
   update_panels();
   doupdate();
}

void cxDatePicker::moveFocusDay(int pDelta)
{
   const int numDays = cxDate::numMonthDays(
      mDisplayDate.getMonth(), mDisplayDate.getYear());
   int oldDay = mFocusDay;
   int newDay = mFocusDay + pDelta;

   if (newDay < 1)
   {
      // Navigate to previous month, land on last day
      navigateMonth(-1);
      mFocusDay = cxDate::numMonthDays(
         mDisplayDate.getMonth(), mDisplayDate.getYear());
      refreshCalendar();
      return;
   }
   if (newDay > numDays)
   {
      // Navigate to next month, land on first day
      navigateMonth(1);
      mFocusDay = 1;
      refreshCalendar();
      return;
   }

   mFocusDay = newDay;
   redrawDay(oldDay);
   redrawDay(newDay);
   drawContent(); // refresh header + month label too
   update_panels();
   doupdate();
}

void cxDatePicker::navigateMonth(int pDelta)
{
   mDisplayDate.addMonths(pDelta);
   // Clamp mFocusDay to the new month's day count
   int numDays = cxDate::numMonthDays(
      mDisplayDate.getMonth(), mDisplayDate.getYear());
   if (mFocusDay > numDays) mFocusDay = numDays;

   // Sync year input in case year changed (e.g. Dec → Jan)
   std::ostringstream yearSS;
   yearSS << mDisplayDate.getYear();
   mYearInput->setValue(yearSS.str());

   // Sync month combo
   if (mMonthCombo)
   {
      mMonthCombo->setValue(cxDate::monthName(mDisplayDate.getMonth()));
   }

   refreshCalendar();
}

void cxDatePicker::applyYearInput()
{
   string yearStr = mYearInput->getValue(true, true);
   if (yearStr.empty()) return;
   try
   {
      int yr = std::stoi(yearStr);
      if (yr >= CXDATE_YEAR_MIN && yr <= CXDATE_YEAR_MAX)
      {
         mDisplayDate.setYear(yr);
         // Clamp day
         int numDays = cxDate::numMonthDays(
            mDisplayDate.getMonth(), mDisplayDate.getYear());
         if (mFocusDay > numDays) mFocusDay = numDays;
         refreshCalendar();
      }
   }
   catch (...)
   {
      /* invalid year text – ignore */
   }
}

void cxDatePicker::applyMonthCombo()
{
   if (!mMonthCombo) return;
   string sel = mMonthCombo->getValue();
   for (int m = 1; m <= 12; ++m)
   {
      if (cxDate::monthName(m) == sel)
      {
         mDisplayDate.setMonth(m);
         int numDays = cxDate::numMonthDays(m, mDisplayDate.getYear());
         if (mFocusDay > numDays) mFocusDay = numDays;
         break;
      }
   }
}

void cxDatePicker::refreshCalendar()
{
   drawContent();
   update_panels();
   doupdate();
}

// ---------------------------------------------------------------------------
// Private: input loop
// ---------------------------------------------------------------------------

long cxDatePicker::doInputLoop()
{
   long returnCode = cxID_CANCEL;
   bool continueOn = true;

   while (continueOn)
   {
      int key = wgetch(mWindow);
      setLastKey(key);

      // ----- Tab / Shift-Tab: cycle focus -----
      if (key == TAB || key == SHIFT_TAB)
      {
         static const FocusItem order[] = {
            FocusItem::PREV_BTN,
            FocusItem::MONTH_COMBO,
            FocusItem::NEXT_BTN,
            FocusItem::YEAR_INPUT,
            FocusItem::CALENDAR,
            FocusItem::OK_BTN,
            FocusItem::CANCEL_BTN
         };
         constexpr int N = static_cast<int>(sizeof(order)/sizeof(order[0]));
         int cur = 0;
         for (int i = 0; i < N; ++i)
         {
            if (order[i] == mFocusItem)
            {
               cur = i; break;
            }
         }
         int next = (key == TAB) ? (cur + 1) % N : (cur + N - 1) % N;
         setFocusItem(order[next]);
         continue;
      }

      // ----- Escape: cancel -----
      if (key == ESCAPE)
      {
         returnCode = cxID_CANCEL;
         continueOn = false;
         break;
      }

      // ----- Keys depend on focused control -----
      switch (mFocusItem)
      {

         // ------- Month combo box -------
         case FocusItem::MONTH_COMBO:
            if (key == ENTER || key == KEY_ENTER || key == ' ' || key == KEY_DOWN)
            {
               // Show the combo modally; hide it afterwards and refresh
               if (mMonthCombo)
               {
                  mMonthCombo->setValue(cxDate::monthName(mDisplayDate.getMonth()));
                  long rc = mMonthCombo->showModal(true, true, true);
                  if (rc != cxID_QUIT)
                  {
                     applyMonthCombo();
                  }
                  mMonthCombo->hide();
                  refreshCalendar();
               }
               setFocusItem(FocusItem::MONTH_COMBO);
            }
            break;

         // ------- Previous-month button -------
         case FocusItem::PREV_BTN:
            if (key == ENTER || key == KEY_ENTER || key == ' ')
            {
               navigateMonth(-1);
            }
            break;

         // ------- Next-month button -------
         case FocusItem::NEXT_BTN:
            if (key == ENTER || key == KEY_ENTER || key == ' ')
            {
               navigateMonth(1);
            }
            break;

         // ------- Year input -------
         case FocusItem::YEAR_INPUT:
            if (key == ENTER || key == KEY_ENTER || key == ' ')
            {
               if (mYearInput)
               {
                  // Pre-populate with the currently displayed year, then
                  // let cxInput handle text entry (it blocks until done)
                  std::ostringstream yearSS;
                  yearSS << mDisplayDate.getYear();
                  mYearInput->setValue(yearSS.str());
                  mYearInput->showModal(true, false, false);
                  applyYearInput();
                  mYearInput->hide();
                  refreshCalendar();
               }
               setFocusItem(FocusItem::YEAR_INPUT);
            }
            break;

         // ------- Calendar grid -------
         case FocusItem::CALENDAR:
            switch (key)
            {
               case KEY_LEFT:
                  moveFocusDay(-1);
                  break;
               case KEY_RIGHT:
                  moveFocusDay(1);
                  break;
               case KEY_UP:
                  moveFocusDay(-7);
                  break;
               case KEY_DOWN:
                  moveFocusDay(7);
                  break;
               case ENTER:
               case KEY_ENTER:
                  mSelectedDate = cxDate(
                     mDisplayDate.getYear(),
                     mDisplayDate.getMonth(),
                     mFocusDay);
                  returnCode  = cxID_OK;
                  continueOn  = false;
                  break;
               default:
                  break;
            }
            break;

         // ------- Cancel button -------
         case FocusItem::CANCEL_BTN:
            if (key == ENTER || key == KEY_ENTER || key == ' ')
            {
               returnCode = cxID_CANCEL;
               continueOn = false;
            }
            break;

         // ------- OK button -------
         case FocusItem::OK_BTN:
            if (key == ENTER || key == KEY_ENTER || key == ' ')
            {
               mSelectedDate = cxDate(
                  mDisplayDate.getYear(),
                  mDisplayDate.getMonth(),
                  mFocusDay);
               returnCode = cxID_OK;
               continueOn = false;
            }
            break;

      } // end switch(mFocusItem)

      // ----- Global shortcuts -----
      if (continueOn)
      {
         switch (key)
         {
            case 'o': case 'O': // OK shortcut
               mSelectedDate = cxDate(
                  mDisplayDate.getYear(),
                  mDisplayDate.getMonth(),
                  mFocusDay);
               returnCode = cxID_OK;
               continueOn = false;
               break;
            case 'c': case 'C': // Cancel shortcut
               returnCode = cxID_CANCEL;
               continueOn = false;
               break;
            default: break;
         }
      }

      // ----- Mouse events -----
#ifdef NCURSES_MOUSE_VERSION
      if (continueOn && key == KEY_MOUSE)
      {
         if (getmouse(&mMouse) == OK)
         {
            int my = mMouse.y;
            int mx = mMouse.x;

            if (mMouse.bstate & (BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED))
            {

               // Calendar day click
               int clickedDay = getDayAtScreenPos(my, mx);
               if (clickedDay > 0)
               {
                  int oldDay = mFocusDay;
                  mFocusDay  = clickedDay;
                  setFocusItem(FocusItem::CALENDAR);
                  redrawDay(oldDay);
                  redrawDay(mFocusDay);
                  drawContent();
                  show(false, false);

                  // Double-click confirms the day
                  if (mMouse.bstate & BUTTON1_DOUBLE_CLICKED)
                  {
                     mSelectedDate = cxDate(
                        mDisplayDate.getYear(),
                        mDisplayDate.getMonth(),
                        mFocusDay);
                     returnCode = cxID_OK;
                     continueOn = false;
                  }
               }

               // OK button
               if (continueOn && mOKBtn &&
                   mouseEvtInRect(mMouse,
                      mOKBtn->top(), mOKBtn->left(),
                      mOKBtn->bottom(), mOKBtn->right()))
               {
                  mSelectedDate = cxDate(
                     mDisplayDate.getYear(),
                     mDisplayDate.getMonth(),
                     mFocusDay);
                  setFocusItem(FocusItem::OK_BTN);
                  returnCode = cxID_OK;
                  continueOn = false;
               }

               // Cancel button
               if (continueOn && mCancelBtn &&
                   mouseEvtInRect(mMouse,
                      mCancelBtn->top(), mCancelBtn->left(),
                      mCancelBtn->bottom(), mCancelBtn->right()))
               {
                  setFocusItem(FocusItem::CANCEL_BTN);
                  returnCode = cxID_CANCEL;
                  continueOn = false;
               }

               // [<] Previous button
               if (continueOn && mPrevBtn &&
                   mouseEvtInRect(mMouse,
                      mPrevBtn->top(), mPrevBtn->left(),
                      mPrevBtn->bottom(), mPrevBtn->right()))
               {
                  setFocusItem(FocusItem::PREV_BTN);
                  navigateMonth(-1);
               }

               // [>] Next button
               if (continueOn && mNextBtn &&
                   mouseEvtInRect(mMouse,
                      mNextBtn->top(), mNextBtn->left(),
                      mNextBtn->bottom(), mNextBtn->right()))
               {
                  setFocusItem(FocusItem::NEXT_BTN);
                  navigateMonth(1);
               }

               // Month combo click (clicks on the month label area in the nav row)
               if (continueOn && mMonthCombo)
               {
                  int comboScreenRow = top() + ROW_CTRL_MID;
                  int comboScreenCol = left() + COL_MONTH_COMBO;
                  if (my == comboScreenRow &&
                      mx >= comboScreenCol &&
                      mx < comboScreenCol + MONTH_COMBO_W)
                  {
                     setFocusItem(FocusItem::MONTH_COMBO);
                     mMonthCombo->setValue(cxDate::monthName(mDisplayDate.getMonth()));
                     long rc = mMonthCombo->showModal(true, true, true);
                     if (rc != cxID_QUIT)
                     {
                        applyMonthCombo();
                     }
                     mMonthCombo->hide();
                     refreshCalendar();
                     setFocusItem(FocusItem::MONTH_COMBO);
                  }
               }

               // Year input click (detect by screen coordinates of the drawn area)
               {
                  int yearScreenRow = top() + ROW_CTRL_MID;
                  int yearScreenCol = left() + COL_YEAR_INPUT;
                  if (continueOn && mYearInput &&
                      my == yearScreenRow &&
                      mx >= yearScreenCol && mx < yearScreenCol + YEAR_INPUT_W)
                  {
                     setFocusItem(FocusItem::YEAR_INPUT);
                     std::ostringstream yearSS;
                     yearSS << mDisplayDate.getYear();
                     mYearInput->setValue(yearSS.str());
                     mYearInput->showModal(true, false, false);
                     applyYearInput();
                     mYearInput->hide();
                     refreshCalendar();
                     setFocusItem(FocusItem::YEAR_INPUT);
                  }
               }

            } // BUTTON1_CLICKED

            // Window drag (title bar)
            if (mMouse.bstate & BUTTON1_PRESSED)
            {
               if (mouseEvtWasInTitle())
               {
                  mMouse.x = mx; mMouse.y = my;
               }
            }
         } // getmouse OK
      } // KEY_MOUSE
#endif

   } // while (continueOn)

   mLeaveNow = false;
   return returnCode;
}
