#ifndef __CXDATEPICKER_H__
#define __CXDATEPICKER_H__

// Copyright (c) 2026 Eric Oulashin
//
// cxDatePicker.h - A text-based date-picker dialog for the cxWidgets library.
//
// cxDatePicker presents a modal calendar view.  The layout (38 wide × 20 tall)
// is:
//
//  ┌─────────────────────────────────────┐  ← title " Select Date"
//  │ Mon, March 16, 2026                 │  ← selected-date header (row 1)
//  │─────────────────────────────────────│  ← separator (row 2)
//  │  [<]  March        [>]   2026       │  ← controls  (rows 3–5)
//  │                                     │
//  │  Su Mo Tu We Th Fr Sa               │  ← day headers (row 6)
//  │   1  2  3  4  5  6  7               │  ← calendar rows 7–12
//  │   …                                 │
//  │                                     │
//  │               [ OK ] [  Cancel  ]   │  ← buttons (rows 14–16)
//  └─────────────────────────────────────┘
//
// Navigation:
//  Tab / Shift-Tab  : cycle focus between month-combo → year-input → [<] →
//                     [>] → calendar → [Cancel] → [OK]
//  Arrow keys       : move day cursor within the calendar; left/right at
//                     month boundary navigates to the previous/next month
//  Enter            : confirm focused control / select focused day
//  Mouse click      : click a calendar day to focus it; click [OK]/[Cancel]
//                     to confirm/abort; click [<]/[>] to navigate months
//
// Return codes from showModal():
//  cxID_OK     : user pressed OK or double-clicked a day
//  cxID_CANCEL : user pressed Cancel or Escape
//
// Usage:
//   cxDatePicker picker(nullptr);   // centered on screen, defaults to today
//   if (picker.showModal() == cxID_OK) {
//       cxDate chosen = picker.getSelectedDate();
//   }

#include "cxWindow.h"
#include "cxButton.h"
#include "cxComboBox.h"
#include "cxInput.h"
#include "cxDate.h"
#include <memory>

class cxDatePicker : public cxWindow
{
public:
   // -----------------------------------------------------------------------
   // Layout constants
   // -----------------------------------------------------------------------
   static constexpr int PICKER_WIDTH  = 38;  // total window width
   static constexpr int PICKER_HEIGHT = 20;  // total window height

   // Row offsets within the ncurses WINDOW (0 = top border)
   static constexpr int ROW_DATE_HEADER  = 1;  // selected-date display
   static constexpr int ROW_SEPARATOR    = 2;  // ACS_HLINE separator
   static constexpr int ROW_CTRL_TOP     = 3;  // top border of nav buttons
   static constexpr int ROW_CTRL_MID     = 4;  // content of controls
   static constexpr int ROW_CTRL_BOT     = 5;  // bottom border of nav buttons
   static constexpr int ROW_DAY_HEADERS  = 6;  // "Su Mo Tu We Th Fr Sa"
   static constexpr int ROW_CAL_FIRST    = 7;  // first calendar week row
   static constexpr int ROW_CAL_LAST     = 12; // last calendar week row (6 weeks max)
   static constexpr int ROW_BTN_TOP      = 14; // top border of OK/Cancel
   static constexpr int ROW_BTN_MID      = 15; // button content
   static constexpr int ROW_BTN_BOT      = 16; // button bottom border

   // Column offsets within the ncurses WINDOW (0 = left border)
   // Control row layout: [<]  [MonthCombo]  [>]   [Year____]
   static constexpr int COL_CAL          = 2;  // left edge of calendar area
   static constexpr int COL_PREV_BTN     = 2;  // [<] button column
   static constexpr int NAV_BTN_W        = 4;  // nav button width (incl. border)
   static constexpr int NAV_BTN_H        = 3;  // nav button height (incl. border)
   static constexpr int COL_MONTH_COMBO  = 7;  // month combo box column
   static constexpr int MONTH_COMBO_W    = 13; // month combo box width
   static constexpr int MONTH_COMBO_H    = 10; // month combo box height (1 input + 9 menu)
   static constexpr int COL_NEXT_BTN     = 21; // [>] button column
   static constexpr int COL_YEAR_INPUT   = 27; // year input left edge
   static constexpr int YEAR_INPUT_W     = 6;  // year input width (4 digits + padding)
   static constexpr int COL_OK_BTN       = 20; // OK button column
   static constexpr int OK_BTN_W         = 5;  // OK button width
   static constexpr int COL_CANCEL_BTN   = 26; // Cancel button column
   static constexpr int CANCEL_BTN_W     = 10; // Cancel button width

   static constexpr int DAY_CELL_W       = 3;  // chars per day cell (" %2d")

   // -----------------------------------------------------------------------
   // Constructors / Destructor
   // -----------------------------------------------------------------------

   /**
    * @brief Constructs a cxDatePicker dialog centred on or near the given
    *        position.  Creates all child widgets (nav buttons, month combo,
    *        year input, OK, Cancel) and initialises the calendar to today's
    *        date.
    * @param pParentWindow  Parent window (nullptr = top-level)
    * @param pRow           Top row (-1 = center vertically)
    * @param pCol           Left column (-1 = center horizontally)
    * @param pDate          The date to show on the picker. Defaults to the current date.
    */
   explicit cxDatePicker(cxWindow *pParentWindow = nullptr,
                         int pRow = -1,
                         int pCol = -1,
                         const cxDate& pDate = cxDate::today());

   /**
    * @brief Constructs a cxDatePicker dialog centred on or near the given
    *        position.  Creates all child widgets (nav buttons, month combo,
    *        year input, OK, Cancel) and initialises the calendar to today's
    *        date.
    * @param pYear          The year to use for the date
    * @param pMonth         The month to use for the date
    * @param pDay           The day of the month to use for the date
    * @param pParentWindow  Parent window (nullptr = top-level)
    * @param pRow           Top row (-1 = center vertically)
    * @param pCol           Left column (-1 = center horizontally)
    */
   explicit cxDatePicker(int pYear, int pMonth, int pDay,
                         cxWindow *pParentWindow = nullptr,
                         int pRow = -1,
                         int pCol = -1);

   virtual ~cxDatePicker();

   // -----------------------------------------------------------------------
   // cxWindow overrides
   // -----------------------------------------------------------------------

   /** @return The type name string "cxDatePicker". */
   virtual std::string cxTypeStr() const override;

   /**
    * @brief Shows the date picker and enters the interactive input loop.
    * @param pShowSelf        If true, make the picker window itself visible.
    * @param pBringToTop      If true, raise the window to the top of the Z-order.
    * @param pShowSubwindows  If true, show all child widgets as well.
    * @return cxID_OK if the user confirmed a date, cxID_CANCEL otherwise.
    */
   virtual long showModal(bool pShowSelf = true,
                          bool pBringToTop = true,
                          bool pShowSubwindows = true) override;

   /**
    * @brief Hides the date picker and all its child widgets.
    * @param pHideSubwindows  If true, hide all child widgets as well.
    */
   virtual void hide(bool pHideSubwindows = true) override;

   /**
    * @brief Shows the date picker and its child widgets.
    *        Note: the month combo drop-down list stays hidden until the
    *        user activates it; only the combo's input field is shown.
    * @param pUnhideSubwindows  If true, unhide all child widgets as well.
    */
   virtual void unhide(bool pUnhideSubwindows = true) override;

   /**
    * @brief Moves the date picker window and repositions all child widgets.
    * @param pNewRow    New top row for the window.
    * @param pNewCol    New left column for the window.
    * @param pRefresh   If true, redraw immediately after moving.
    * @return true on success, false if the move could not be performed.
    */
   virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

   /** @brief Draws the window border and calendar content. */
   virtual void draw() override;

   // -----------------------------------------------------------------------
   // Date picker interface
   // -----------------------------------------------------------------------

   /**
    * @brief Returns the date the user confirmed.
    *        Only meaningful after showModal() returned cxID_OK.
    * @return A const reference to the selected cxDate.
    */
   [[nodiscard]] const cxDate& getSelectedDate() const noexcept
   {
      return mSelectedDate;
   }

   /**
    * @brief Pre-sets the date shown when the picker opens.
    * @param pDate  The date to display and select initially.
    */
   void setDate(const cxDate& pDate);

private:
   // -----------------------------------------------------------------------
   // Internal focus tracker
   // -----------------------------------------------------------------------
   enum class FocusItem
   {
      MONTH_COMBO,
      YEAR_INPUT,
      PREV_BTN,
      NEXT_BTN,
      CALENDAR,
      CANCEL_BTN,
      OK_BTN
   };

   // -----------------------------------------------------------------------
   // State
   // -----------------------------------------------------------------------
   cxDate    mSelectedDate;                        // Date confirmed by the user (valid on cxID_OK)
   cxDate    mDisplayDate;                         // Month/year currently shown in the calendar
   int       mFocusDay  = 1;                       // Which day (1-based) has the cursor in the calendar
   FocusItem mFocusItem = FocusItem::CALENDAR;

   // -----------------------------------------------------------------------
   // Child widgets
   // -----------------------------------------------------------------------
   std::shared_ptr<cxComboBox> mMonthCombo;
   std::shared_ptr<cxInput>   mYearInput;
   std::shared_ptr<cxButton>  mPrevBtn;
   std::shared_ptr<cxButton>  mNextBtn;
   std::shared_ptr<cxButton>  mCancelBtn;
   std::shared_ptr<cxButton>  mOKBtn;

   // -----------------------------------------------------------------------
   // Private helpers
   // -----------------------------------------------------------------------

   /// Creates all child widgets at the correct absolute screen positions.
   void initWidgets();

   /// Draws the picker content into mWindow (called from draw()).
   void drawContent();

   /// Redraws a single calendar day cell (used to update the highlight without
   /// redrawing the entire window). @param pDay 1-based day number.
   void redrawDay(int pDay);

   /// Returns the ncurses WINDOW row/col (0-based within mWindow) of a day cell.
   void getDayWinPos(int pDay, int& pWinRow, int& pWinCol) const;

   /// Returns the day (1-based) at ncurses screen coordinates, or 0 if none.
   int getDayAtScreenPos(int pScreenRow, int pScreenCol) const;

   /// Moves focus to a new control, updating the visual highlight.
   void setFocusItem(FocusItem pItem);

   /// Moves the day cursor by @p pDelta days, clamped to the current month.
   void moveFocusDay(int pDelta);

   /// Navigates to the previous or next month. @param pDelta -1 or +1.
   void navigateMonth(int pDelta);

   /// Applies or removes the A_REVERSE highlight on a nav/OK/Cancel button.
   void highlightButton(std::shared_ptr<cxButton>& pBtn, bool pOn);

   /// Reads the year from mYearInput and updates mDisplayDate if valid.
   void applyYearInput();

   /// Reads the month from mMonthCombo and updates mDisplayDate.
   void applyMonthCombo();

   /// Main interactive input loop; returns cxID_OK or cxID_CANCEL.
   long doInputLoop();

   /// Refreshes just the calendar grid area and the date-header line.
   void refreshCalendar();
};

#endif // __CXDATEPICKER_H__
