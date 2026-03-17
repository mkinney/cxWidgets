#ifndef __CXSCROLLEDWINDOW_H__
#define __CXSCROLLEDWINDOW_H__
// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxWindow.h"
#include "cxFunction.h"
#include <string>

#define DEFAULT_CXSCROLLEDWINDOW_SEARCH_KEY '/'     // Default search key
#define DEFAULT_CXSCROLLEDWINDOW_GOTO_KEY CTRL_G    // Default key to go to a line#
#define DEFAULT_CXSCROLLEDWINDOW_ALT_PGUP_KEY '.'   // Default alternate PgUp key
#define DEFAULT_CXSCROLLEDWINDOW_ALT_PGDOWN_KEY ',' // Default alternate PgDown key

/**
 * \brief Represents a text window with scrolling.  This class is
 * \brief basically just like cxWindow, but it does scrolling, so
 * \brief that you can have a large message inside the window and
 * \brief be able to scroll it.
 * While the object is being displayed with showModal(), the user
 * can go to a specific line by pressing CTRL-G (this is the default;
 * it can be changed with setGoToKey()).  The user can also search
 * for text with the '/' key (this is the default; it can be changed
 * with setSearchKey()).  By default, the previous keyword is used
 * when searching again; this behavior can be changed with useLastKeyword().<br>
 * Also, cxFileViewer supports page scrolling with the pageUp and pageDown keys.
 * In addition, cxMenu also uses alternate keys for doing a pageUp and pageDown,
 * which by default are '.' and ',', respectively.  These defaults can be
 * changed via setAltPgUpKey() and setAltPgDownKey().<br>
 * cxScrolledWindow also lets you specify functions to be called at certain
 * points in & around the input loop (which is run when a call to
 * showModal() is made).  These functions are to aid in deriving a class
 * from cxScrolledWindow without having to overload showModal() and re-writing
 * an input loop.  The following methods set these functions:<br>
 * <ul>
 * <li>setOnFocusFunction() (inherited from cxWindow) - Sets a function to run
 *     when focus is gained.
 * <li>setLoopStartFunction() - Sets a function to run at the start of each
 * cycle through the input loop
 * <li>setLoopEndFunction() - Sets a function to be run at the end of each cycle
 * through the input loop
 * <li>setOnLeaveFunction() (inherited from cxWindow) - Sets a function to be
 *     run when focus is lost.
 * </ul>
 * Note that the functions you provide must have this signature:<br>
 * \verbatim
std::string someFunc(void*, void*, void*, void*)
\endverbatim
 *
 * \author $Author: erico $
 * \version $Revision: 1.21 $
 * \date $Date: 2007/12/04 00:23:46 $
 */
class cxScrolledWindow : public cxWindow
{
   public:
      /**
       * \brief Default constructor.
       * @param pParentWindow A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the window.
       * @param pWidth The width of the window.
       * @param pTitle The title to appear at the top of the window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pBorderStyle The type of border to use - can be
       *  eBS_SINGLE_LINE for a single-line border or eBS_NOBORDER for
       *  no border.
       * @param pExtTitleWindow A pointer to another cxWindow in which to
       *  display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow in which to
       *  display the status.
       * @param pMessageUnderlines Whether or not to recognize & use underline
       *  characters when displaying the message text.
       */
      explicit cxScrolledWindow(cxWindow *pParentWindow = nullptr,
                        int pRow = 0, int pCol = 0,
                        int pHeight = DEFAULT_HEIGHT,
                        int pWidth = DEFAULT_WIDTH,
                        const std::string& pTitle = "",
                        const std::string& pMessage = "",
                        const std::string& pStatus = "",
                        eBorderStyle pBorderStyle = eBS_SINGLE_LINE,
                        cxWindow *pExtTitleWindow = nullptr,
                        cxWindow *pExtStatusWindow = nullptr,
                        bool pMessageUnderlines = false);

      /**
       * \brief Overridden constructor
       * @param pParentWindow A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pTitle The title to appear at the top of the window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pMessageUnderlines Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxScrolledWindow(cxWindow *pParentWindow,
               int pRow, int pCol, const std::string& pTitle,
               const std::string& pMessage, const std::string& pStatus,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pMessageUnderlines = false);

      /**
       * \brief Overridden constructor - Centers the window in the middle of the screen.
       * @param pParentWindow A pointer to the parent window.
       * @param pTitle The title to appear at the top of the window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pMessageUnderlines Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxScrolledWindow(cxWindow *pParentWindow,
               const std::string& pTitle, const std::string& pMessage,
               const std::string& pStatus,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pMessageUnderlines = false);

      /**
       * \brief Overridden constructor - Centers the window in the middle of the screen.
       * @param pParentWindow A pointer to the parent window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pMessageUnderlines Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxScrolledWindow(cxWindow *pParentWindow,
               const std::string& pMessage, const std::string& pStatus,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pMessageUnderlines = false);

      /**
       * \brief Overridden constructor - Centers the window in the middle of the screen.
       * @param pParentWindow A pointer to the parent window.
       * @param pMessage The message to appear in the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pMessageUnderlines Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxScrolledWindow(cxWindow *pParentWindow, const std::string& pMessage,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pMessageUnderlines = false);

      /**
       * \brief Overridden constructor - Lets you specify the horizontal position
       * @param pParentWindow A pointer to the parent window.
       * @param pHPosition The horizontal position of the window - eHP_CENTER, eHP_LEFT, or eHP_RIGHT
       * @param pTitle The title to appear at the top of the window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pMessageUnderlines Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxScrolledWindow(cxWindow *pParentWindow, eHPosition pHPosition,
               const std::string& pTitle, const std::string& pMessage,
               const std::string& pStatus,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pMessageUnderlines = false);

      /**
       * \brief Copy constructor
       * @param pThatWindow Another cxWindow object to be copied
       */
      cxScrolledWindow(const cxScrolledWindow& pThatWindow);

      virtual ~cxScrolledWindow();

      /**
       * \brief Shows the window.
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *   to true.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults
       *   to true.
       * @return This method returns a long to support overriding in derived classes
       *   that might need to return a value.  For cxScrolledWindow, cxID_EXIT
       *   is always returned.
       */
      virtual long show(bool pBringToTop = true, bool pShowSubwindows = true) override;

      /**
       * \brief Shows the window, running an input loop allowing the user to
       * \brief scroll the window.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring the window to the top.
       *  Defaults to false.
       * @param pShowSubwindows Whether or not to show sub-windows also.
       *  Defaults to false.
       *
       * @return cxID_QUIT if the user pressed the escape key, or cxID_EXIT
       *  if the user pressed any other key.
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = false) override;

      /**
       * \brief Scrolls the window text by a certain amount.
       *
       * @param pVertScrollAmt The amount by which to scroll vertically
       * @param pHorizScrollAmt The amount by which to scroll horizontally
       * @param pRefresh Whether or not to refresh the menu (defaults to false).
       */
      virtual void scrollWin(int pVertScrollAmt, int pHorizScrollAmt,
                          bool pRefresh = false);

      /**
       * Draws the message to the window.
       */
      virtual void drawMessage() override;

      /**
       * Changes the position of the window, based on a new upper-left corner
       * @param pNewRow The new topmost row of the form
       * @param pNewCol The new leftmost column of the form
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

      /**
       * Resizes the window
       * @param pNewHeight New height of the window
       * @param pNewWidth New width of the window
       * @param pRefresh Whether or not to refresh the window.  Defaults to false.
       */
      virtual void resize(int pNewHeight, int pNewWidth, bool pRefresh = false) override;

      /**
       * \brief Assignment operator
       *
       * @param pThatWindow Another cxScrolledWindow to be copied
       *
       * @return The current cxScrolledWindow
       */
      cxScrolledWindow& operator =(const cxScrolledWindow& pThatWindow);

      /**
       * \brief Sets the key to use for searching
       *
       * @param pKey The key to use for searching
       */
      virtual void setSearchKey(int pKey);

      /**
       * \brief Sets the key to use for going to a specific line#
       *
       * @param pKey The key to use for going to a spefific line#
       */
      virtual void setGoToKey(int pKey);

      /**
       * \brief Sets whether to use the last keyword in new searches
       *
       * @param pUseLastKeyword Whether or not to use the last keyword in new searches
       */
      virtual void useLastKeyword(bool pUseLastKeyword);

      /**
       * \brief Sets the alternate pageUp key
       *
       * @param pPgUpKey The new alternate pageUp key
       */
      virtual void setAltPgUpKey(int pKey);

      /**
       * \brief Sets the new alternate pageDown key
       *
       * @param pPgDownKey The new alternate pageDown key
       */
      virtual void setAltPgDownKey(int pKey);

      /**
       * \brief Sets a function to be run at the start of each
       * \brief input loop.  The return value of the function is
       * \brief not used.
       *
       * @param pFuncPtr A pointer to the function
       */
      virtual void setLoopStartFunction(const std::shared_ptr<cxFunction>& pFuncPtr);

      /**
       * \brief Sets a function to be run at the end of each
       * \brief cycle through the input loop.  The return value
       * \brief of the function is not used.
       *
       * @param pFuncPtr A pointer to the function
       */
      virtual void setLoopEndFunction(const std::shared_ptr<cxFunction>& pFuncPtr);

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief Draws a border around the window
       */
      virtual void drawBorder() override;

   protected:
      /**
       * \brief Re-creates the scrollable subwindow.
       */
      void reCreateSubWindow();

      /**
       * \brief Initializes the window parameters and sets up the internal
       * \brief data members.
       *
       * @param pRow The topmost row of the window
       * @param pCol The leftmost column of the window
       * @param pHeight The height of the window
       * @param pWidth The width of the window
       * @param pTitle The title to use for the window
       * @param pMessage The message to display inside the window
       * @param pStatus The status text to use
       * @param pParentWindow A pointer to the parent cxWindow (defaults to nullptr)
       * @param pResizeVertically Whether or not to resize vertically so that
       *  all the message text shows (defaults to false).
       */
      virtual void init(int pRow, int pCol, int pHeight, int pWidth,
                        const std::string& pTitle, const std::string& pMessage,
                        const std::string& pStatus,
                        cxWindow *pParentWindow = nullptr,
                        bool pResizeVertically = false) override;

      /**
       * \brief Makes a copy of a cxScrolledWindow's member variables
       *
       * @param pThatWindow A pointer to another cxScrolledWindow whose members to copy
       */
      void copyCxScrolledWindowStuff(const cxScrolledWindow* pThatWindow);

      /**
       * \brief Returns the number of the line currently being displayed
       * \brief at the top of the window.
       *
       * @return The number of the line currently being displayed at the top of the window
       */
      int getLineNumber() const;

      /**
       * \brief Returns the height of the scrollable subwindow.
       *
       * @return The height of the scrollable subwindow
       */
      int getSubWinHeight() const;

      /**
       * \brief Returns the width of the scrollable subwindow.
       *
       * @return The width of the scrollable subwindow
       */
      int getSubWinWidth() const;

   private:
      WINDOW *mSubWindow = nullptr; // For scrolling
      int mSubWinHeight = 0;        // The height of the subwindow
      int mSubWinWidth = 0;         // The width of the subwindow
      int mLineNumber = 0;          // The topmost line# being shown from mMessageLines
      int mHScrollOffset = 0;       // Horizontal scrolling offset
      int mSearchKey = DEFAULT_CXSCROLLEDWINDOW_SEARCH_KEY;        // Key to use for searching
      int mGoToKey = DEFAULT_CXSCROLLEDWINDOW_GOTO_KEY;            // Key to use to go to a specific line#
      int mAltPgUpKey = DEFAULT_CXSCROLLEDWINDOW_ALT_PGUP_KEY;     // Alternate key for a pageUp
      int mAltPgDownKey = DEFAULT_CXSCROLLEDWINDOW_ALT_PGDOWN_KEY; // Alternate key for a pageDown
      std::string mSearchKeyword;   // Previous keyword used for searching
      bool mUseLastKeyword = true;  // Whether or not to use mSearchKeyword for searching

      // Functions to be run at various points in the input loop
      std::shared_ptr<cxFunction> mLoopStartFunction; // At the start of each cycle
      std::shared_ptr<cxFunction> mLoopEndFunction;   // At the end of each cycle

      // Handles the input loop.  Returns a return code based on the
      //  user's input.
      //
      // Parameters:
      //  pRunOnLeaveFunction (OUT): This will store whether or not the onLeave
      //   function should run.
      long doInputLoop(bool& pRunOnLeaveFunction);

      /**
       *  Returns a line from mMessageLines, truncating it
       *  (if necessary) so that it will fit in mSubWindow.
       *  Makes use of hScrollOffset for trimming.
       * @param pLineNumber The line number of the desired line
       * @return The line
       */
      std::string getLine(int pLineNumber);

      // Scrolls to a specific line in mMessageLines.
      // Parameters:
      //  pLineNum: The line # to which to scroll (0-based)
      inline void goToLine(int pLineNum);

      // Prompts the user for a line number and scrolls to
      //  that line.
      void doGoToLine();

      // Prompts the user for a search keyword & scrolls to
      //  the line with that keyword.
      void doSearch();

      // Puts the horizontal scroll arrows in place on the bottom border.
      void drawHorizontalScrollArrows();
};

#endif
