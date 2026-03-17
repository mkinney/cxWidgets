#ifndef __CXMENU_H__
#define __CXMENU_H__

// Copyright (c) 2005-2007 Michael H. Kinney
//
// This represents a pop-up (or pull-down) list of items,
//  one of which may be selected before the menu goes away.
//  The items are text, with the shortcut letter underlined.

#include "cxWindow.h"
#include "cxMenuItemType.h"
#include "cxFunction.h"

#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>

#define DEFAULT_CXMENU_SEARCH_KEY '/'     // Default search key
#define DEFAULT_CXMENU_ALT_PGUP_KEY '.'   // Default alternate PgUp key
#define DEFAULT_CXMENU_ALT_PGDOWN_KEY ',' // Default alternate PgDown key

/** \class cxMenu
 * \brief Represents a list of items, one of which may be selected
 * \brief before the menu goes away.
 * The items are one-line text entries.  An item can additionally have a
 * sub-menu associated with it.  The shortcut characters for the item can be
 * underlined via an ampersand (&) placed before a character when setting the
 * item text.<br>
 * Each item in a cxMenu has 2 pieces of text: The text that is displayed, and
 * alternate text.  When appending an item to the menu, both can be specified.
 * When retrieving item text from the menu, you can retrieve either the
 * displayed text or the item's alternate text.<br>
 * By default, the menu does not wrap around when the user scrolls beyond the
 * last or first item; however, this behavior can be changed with a call to
 * enableWrapping().<br>
 * The user can also search for a menu item - by default, the search key is
 * '/', but this can be changed via a call to setSearchKey().  By default, the
 * previous search text is used again for additional searches but can be
 * changed by the user.  If you don't want the form to use the previous search
 * text in additional searches, you can change this behavior with a call to
 * setClearOnSearch(), passing true as the parameter.  Also, item searching is
 * not case-sensitive by default, but this can be changed with a call to
 * setCaseSensitiveSearch().<br>
 * Also, cxMenu supports page scrolling with the pageUp and pageDown keys.  In
 * addition, cxMenu also uses alternate keys for doing a pageUp and pageDown,
 * which by default are '.' and ',', respectively.  These defaults can be
 * changed via setAltPgUpKey() and setAltPgDownKey().<br>
 * Note that the search key and the alternate pageUp and pageDown keys are
 * static; therefore, when these keys are changed, they are changed for all
 * instances of cxMenu.  setSearchKey(), setAltPgUpKey(), and setAltPgDownKey()
 * are static methods; therefore, they can be called without an instance of
 * cxMenu.
 *
 * \author $Author: erico $
 * \version $Revision: 1.88 $
 * \date $Date: 2007/12/04 00:23:46 $
 *
 */
class cxMenu : public cxWindow
{
   public:
      /**
       * Default constructor
       * @param pParentWindow Pointer to parent window; defaults to nullptr
       * @param pRow The y location of the upper-left corner of menu;
       *  defaults to 0
       * @param pCol the x location of the upper-left corner of menu;
       *  defaults to 0
       * @param pHeight The height of the menu; defaults to DEFAULT_HEIGHT
       * @param pWidth The width of the menu; defaults to DEFAULT_WIDTH
       * @param pTitle The title of the menu; blank by default
       * @param pExtTitleWindow A pointer to another cxWindow in which to
       *  display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow in which to
       *  display the status.
       * @param pBorderStyle The type of border to use - can be
       *  eBS_SINGLE_LINE for a single-line border or eBS_NOBORDER for
       *  no border.
       */
      explicit cxMenu(cxWindow *pParentWindow = nullptr, int pRow = 0, int pCol = 0,
                      int pHeight = DEFAULT_HEIGHT, int pWidth = DEFAULT_WIDTH,
                      const std::string& pTitle = "", cxWindow *pExtTitleWindow = nullptr,
                      cxWindow *pExtStatusWindow = nullptr,
                      eBorderStyle pBorderStyle = eBS_SINGLE_LINE);

      /**
       * \brief Copy constructor
       *
       * @param pThatMenu Another cxMenu to be copied
       */
      cxMenu(const cxMenu& pThatMenu);

      /**
       * Destructor
       */
      virtual ~cxMenu();

      /**
       * \brief Adds a string item to the end of the menu.
       *
       * @param pDisplayText The text to display for the item
       * @param pReturnCode The integer code that the the menu should return when show() exits
       * @param pHelpString A string to be displayed in the statusbar of the
       *  main window when the item is highlighted
       * @param pType The type of menu item - This defines whether it's a
       *  regular menu item (cxITEM_NORMAL), a pop-up menu, etc. (see the
       *  cxMenuItemType enumeration in cxMenuItemType.h)
       * @param pResize If true, the menu will resize itself so that it can
       *  show all the items.  Defaults to false.
       * @param pItemText Alternate text to use for the item
       */
      virtual void append(const std::string& pDisplayText, long pReturnCode,
                          const std::string& pHelpString = "",
                          cxMenuItemType pType = cxITEM_NORMAL,
                          bool pResize = false, const std::string& pItemText = "");

      /**
       * Adds an item with a pull-right submenu
       * @param pDisplayText The text to display for the item
       * @param pSubMenu A pointer to the submenu to display
       * @param pHelpString A string to be displayed in the statusbar of the main window
       * when the item is highlighted
       * @param pResize If true, the menu will resize itself so that it can show all the items.
       */
      virtual void appendWithPullRight(const std::string& pDisplayText, cxMenu *pSubMenu,
                                       const std::string& pHelpString = "",
                                       bool pResize = false);

       /**
       * Adds an item with a pop-up submenu
       * @param pDisplayText The text to display for the item
       * @param pSubMenu A pointer to the submenu to display
       * @param pHelpString A string to be displayed in the statusbar of the main
       * window when the item is highlighted
       * @param pResize If true, the menu will resize itself so that it can show all the items.
       */
      virtual void appendWithPopUp(const std::string& pDisplayText, cxMenu *pSubMenu,
                                   const std::string& pHelpString = "", bool pResize = false);

      /**
       * \brief Removes a menu item from the menu.  If the item has a submenu,
       * \brief the submenu will NOT be deleted and may be reused.
       *
       * @param pItemIndex Index of the item to delete
       * @param pResize If true, the menu will resize itself accordingly
       * @param pRefresh Whether or not to refresh the menu (defaults to false)
       *
       * @return True if the item was deleted; false if not.
       */
      virtual bool remove(unsigned pItemIndex, bool pResize = false,
                          bool pRefresh = false);

      /**
       * \brief Removes a menu item from the menu (by display text or alternate
       * \brief item text).  If the item has a submenu, the submenu will NOT be
       * \brief deleted and may be reused.
       *
       * @param pItemText The text of the item to delete
       * @param pUseDisplayText If true (default), then pItemText specifies the
       *  text displayed in the menu.  If false, then pItemText specifies the
       *  alternate item text.
       * @param pResize If true, the menu will resize itself accordingly
       * @param pRefresh Whether or not to refresh the menu (defaults to false)
       *
       * @return True if the item was deleted; false if not.
       */
      virtual bool remove(const std::string& pItemText, bool pUseDisplayText = true,
                          bool pResize = false, bool pRefresh = false);

      /**
       * \brief Removes a menu item based on its return code.
       *
       * @param pReturnCode The return code of the menu item
       * @param pResize If true, the menu will resize itself accordingly
       * @param pRefresh Whether or not to refresh the menu (defaults to false)
       *
       * @return True of the item was removed, or false if not.
       */
      virtual bool removeByReturnCode(long pReturnCode, bool pResize = false,
                                      bool pRefresh = false);

      /**
       * \brief Removes all items from the menu.
       *
       * @param pResize Whether or not to resize the menu when done (defaults to false).
       * @param pRefresh Whether or not to refresh the menu (defaults to false)
       */
      virtual void removeAllItems(bool pResize = false, bool pRefresh = false);

      /**
       * \brief Alias for removeAllItems() (this is here to overload the clear()
       * \brief inherited from cxWindow).  Does not resize the menu, and does
       * \brief not bring it to the top.
       *
       * @param pRefresh Whether or not to refresh the menu (defaults to false)
       */
      virtual void clear(bool pRefresh = false);

      /**
       * \brief Shows the menu, waits for input, and returns the menu item's return
       * \brief code.  Could also return cxID_QUIT if the user presses ESC.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring the window to the top.
       *  Defaults to false.
       * @param pShowSubwindows Whether or not to show sub-windows also.
       *  Defaults to true.
       *
       * @return The return code associated with the item selected (passed
       *  in with the append() function for the item).  Could also return cxID_QUIT if
       *  the user presses ESC.
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = true) override;

      /**
       * \brief Returns whether or not a call to showModal() will wait for a
       * \brief keypress from the user.  A cxmenu will wait for a keypress if
       * \brief it is enabled and it has at least 1 selectable menu item.  If
       * \brief it is enabled but does not have any selectable items, it may
       * \brief or may not wait for a keypress, depending on whether
       * \brief setWaitForInputIfEmpty() has been called.
       *
       * @return Whether or not a call to showModal() will wait for a keypress
       *  from the user.
       */
      virtual bool modalGetsKeypress() const override;

      /**
       * \brief Scrolls the menu item list by a certain amount.
       *
       * @param pScrollAmt The amount by which to scroll.  If this is positive,
       *  the menu will scroll down (the items will move upwards), and if this
       *  is negative, the menu will scroll up.
       * @param pRefresh Whether or not to refresh the menu (defaults to false).
       */
      virtual void scrollItems(int pScrollAmt, bool pRefresh = false);

      /**
       * \brief Scrolls the menu to the top.
       *
       * @param pRefresh Whether or not to refresh the menu (defaults to false).
       */
      virtual void scrollToTop(bool pRefresh = false);

      /**
       * \brief Scrolls the menu to the bottom.
       *
       * @param pRefresh Whether or not to refresh the menu (defaults to false).
       */
      virtual void scrollToBottom(bool pRefresh = false);

      /**
       * \brief Sets the item to appear at the top of the menu (by index).
       *
       * @param pTopItem The index of the item to appear at the top of the menu
       * @param pRefresh Whether or not to refresh the menu (defaults to false).
       */
      virtual void setTopItem(unsigned pTopItem, bool pRefresh = false);

      /**
       * \brief Sets the item to appear at the top of the menu (by item text).
       *
       * @param pItemtext The text of the item
       * @param pRefresh Whether or not to refresh the menu (defaults to false).
       */
      virtual void setTopItem(const std::string& pItemText, bool pRefresh = false);

      /**
       * \brief Sets the item to appear at the bottom of the menu (by index).
       *
       * @param pItemIndex The index of the item to appear at the bottom of the menu
       * @param pRefresh Whether or not to refresh the menu (defaults to false).
       */
      virtual void setBottomItem(unsigned pItemIndex, bool pRefresh = false);

      /**
       * Changes the window's position (based on a new upper-left corner).
       * @param pNewRow The new topmost row of the form
       * @param pNewCol The new leftmost column of the form
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

      /**
       * Returns the number of menu items
       * @return The number of menu items
       */
      virtual unsigned numMenuItems() const;

      /**
       * Resizes the menu
       * @param pNewHeight New height of the menu
       * @param pNewWidth New width of the menu
       * @param pRefresh Whether or not to refresh the menu (defaults
       *  to false)
       */
      virtual void resize(int pNewHeight, int pNewWidth, bool pRefresh = false) override;

      /**
       * Outputs the menu item text lines
       */
      virtual void drawMessage() override;

      /**
       * \brief Shows the menu.
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *   to true.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults
       *   to true.
       * @return This method returns a long to support overriding in derived classes
       *   that might need to return a value.  For cxWindow, cxID_EXIT is
       *   always returned.
       */
      virtual long show(bool pBringToTop = true, bool pShowSubwindows = true) override;

      /**
       * Erases the window
       */
      virtual void erase(bool pEraseSubwindows = true) override;

      /**
       * Returns the width of the submenu
       * @return The width of the submenu
       */
      int getSubWinWidth() const;

      /**
       * Returns the height of the submenu
       * @return The height of the submenu
       */
      int getSubWinHeight() const;

      /**
       * \brief Returns the return code set by the last call to
       * \brief showModal() based on the user's interaction.  The
       * \brief return code is set to cxID_EXIT by default before
       * \brief the first call to showModal().
       *
       * @return The return code set by the last call to showModal()
       */
      virtual long getReturnCode() const override;

      /**
       * Returns the return code for a menu item at a specified index.
       * @param pIndex The index of the item whose return code to get
       * @return The item's return code (if index is within range).  If
       *  the index is not within range, -1 is returned.
       */
      virtual long getReturnCode(unsigned pIndex) const;

      /**
       * \brief Returns the return code for a menu item (by item text).
       *
       * @param pItemText The text of the menu item whose return code to get
       * @param pUseDisplayText If true (default), then this will return the text
       *  displayed on the menu for the item.  If false, then this will return
       *  the alternate text for the item.
       *
       * @return The item's return code (if index is within range).  If
       *  no item with the given text is not found, -1 is returned.
       */
      virtual long getReturnCode(const std::string& pItemText, bool pUseDisplayText = true) const;

      /**
       * \brief Returns the return code of the currently-highlited item.
       * @return The return code of the currently-highlighted item.
       */
      virtual long getCurrentItemReturnCode() const;

      /**
       * \brief Returns the text for a menu item.
       *
       * @param pIndex The index of the menu item
       * @param pGetDisplayText If true (default), then this will return the text
       *  displayed on the menu for the item.  If false, then this will return
       *  the alternate text for the item.
       *
       * @return The text at the specified index, or a blank string if the
       *  index is out of bounds.
       */
      virtual std::string getItemText(int pIndex, bool pGetDisplayText = true) const;

      /**
       * \brief Returns the text of the current selected menu item.  Could
       * \brief return a blank string if there is no current selected menu
       * \brief item.
       *
       * @param pGetDisplayText If true (default), then this will return the text
       *  displayed on the menu for the item.  If false, then this will return
       *  the alternate text for the item.
       *
       * @return The text of the current selected menu item, or a blank string
       *  if there is no current selected menu item.
       */
      virtual std::string getCurrentItemText(bool pGetDisplayText = true) const;

      /**
       * \brief Draws a border around the menu
       */
      virtual void drawBorder() override;

      /**
       * \brief Toggles selectability for an item (by index).  Selectability
       * \brief is currently not dealt with when showing a cxMenu.
       *
       * @param pIndex The index of the item
       * @param pSelectable If true, the item will be selectable.  If
       * true, the item will be selectable.  If false, the item
       * will not be selectable.
       */
      void toggleSelectability(unsigned pIndex, bool pSelectable);

      /**
       * \brief Toggles selectability for an item (by item text).
       * \brief Selectability is currently not dealt with when
       * \brief showing a cxMenu.
       *
       * @param pItemText The text of the item.
       * @param pSelectable If true, the item will be selectable.  If
       * true, the item will be selectable.  If false, the item
       * will not be selectable.
       */
      void toggleSelectability(const std::string& pItemText, bool pSelectable);

      /**
       * \brief Sets whether or not to clear the search text
       * \brief upon showing the search input box.
       *
       * @param pClearOnSearch If true, the previous search text will not be used
       *   in new searches.  If false, the previous search text will be used
       *   in new searches (but can still be changed by the user).
       */
      void setClearOnSearch(bool pClearOnSearch);

      /**
       * \brief Sets whether or not item searching should be case-sensitive.
       *
       * @param pCaseSensitiveSearch If true, item searching will be case-sensitive.
       *  If false, item searching will not be case-sensitive.
       */
      void setCaseSensitiveSearch(bool pCaseSensitiveSearch);

      /**
       * \brief Changes the key to be used for menu item searching (static)
       *
       * @param pSearchKey The new key to be used for searching
       */
      void setSearchKey(int pSearchKey);

      /**
       * \brief Sets the alternate pageUp key
       *
       * @param pPgUpKey The new alternate pageUp key
       */
      void setAltPgUpKey(int pPgUpKey);

      /**
       * \brief Returns the key that is set as the alternate pageUp key
       *
       * @return The key that is set as the alternate pageUp key
       */
      int getAltPgUpKey() const;

      /**
       * \brief Sets the new alternate pageDown key
       *
       * @param pPgDownKey The new alternate pageDown key
       */
      void setAltPgDownKey(int pPgDownKey);

      /**
       * \brief Returns the key that is set as the alternate pageDown key
       *
       * @return The key that is set as the alternate pageDown key
       */
      int getAltPgDownKey() const;

      /**
       * \brief Mutator for the status line text.  After this method
       * \brief is called, the status provided with this method will
       * \brief be used instead of the menu item help messages.
       *
       * @param pStatus The new status message for window
       * @param pRefreshStatus Whether or not to refresh the status area (defaults to true)
       */
      virtual void setStatus(const std::string& pStatus, bool pRefreshStatus = true) override;

      /**
       * \brief Turns off the custom status text set with setStatus.
       * \brief After a call to this function, the current status
       * \brief text will be cleared, and the form will go back to
       * \brief using the help messages from the inputs in the status line.
       *
       * @param pRefreshStatus Whether or not to refresh the status area (defaults to true)
       *
       */
      void disableCustomStatus(bool pRefreshStatus = true);

      /**
       * \brief Assignment operator
       *
       * @param pThatMenu Another cxMenu to be copied
       *
       * @return The current cxMenu
       */
      cxMenu& operator =(const cxMenu& pThatMenu);

      /**
       * \brief Refreshes the list of menu items.
       */
      virtual void refreshMenuItems();

      /**
       * \brief Sets the text of an item (by its return code).
       *
       * @param pReturnCode The return code of the item
       * @param pItemText The new text for the item
       * @param pSetAll If true, will change the text of all items with the
       *                   given return code (defaults to false).
       * @param pRefresh Whether or not to refresh the menu (defaults to false).
       *
       * @return If true, the set was successful; if false, the set was unsuccessful
       *   (i.e., if there are no items with the given return code, etc.)
       */
      virtual bool setItemTextByReturnCode(long pReturnCode, const std::string& pItemText,
                                 bool pSetAll = false, bool pRefresh = false);

      /**
       * \brief Sets the text of an item (by index).
       *
       * @param pIndex The index of the item
       * @param pItemText The new text for the item
       * @param pRefresh Whether or not to refresh the menu (defaults to false).
       *
       * @return If true, the set was successful; if false, the set was unsuccessful
       *   (i.e., if pIndex is an invalid index, etc.)
       */
      virtual bool setItemTextByIndex(int pIndex, const std::string& pItemText,
                                 bool pRefresh = false);

      /**
       * \brief Sets the border style
       *
       * @param pBorderStyle A eBorderStyle enumeration value (see cxBorderStyles.h)
       */
      virtual void setBorderStyle(eBorderStyle pBorderStyle) override;

      /**
       * \brief Returns whether or not an item exists in the menu.
       *
       * @param pItemText The text of the item for which to search
       * @param pUseDisplayText If true (default), then pItemText specifies the
       *  text displayed in the menu.  If false, then pItemText specifies the
       *  alternate item text.
       *
       * @return Whether or not the item exists in the menu
       */
      virtual bool itemExists(const std::string& pItemText, bool pUseDisplayText = true) const;

      /**
       * \brief Sets whether the form should allow the user to quit.
       *
       * @param pAllowQuit If true, the user will be able to quit out of the form; if
       *  false, the user won't be able to quit.
       */
      void setAllowQuit(bool pAllowQuit);

      /**
       * \brief Returns whether the user is able to quit out of the form.
       *
       * @return Whether or not the use is able to quit out of the form
       */
      bool getAllowQuit() const;

      /**
       * \brief Sets whether the form should allow the user to exit.
       *
       * @param pAllowExit If true, the user will be able to exit out of the form; if
       *  false, the user won't be able to exit.
       */
      void setAllowExit(bool pAllowExit);

      /**
       * \brief Returns whether the user is able to exit out of the form.
       *
       * @return Whether or not the use is able to exit out of the form
       */
      bool getAllowExit() const;

      /**
       * \brief Returns the index of a menu item by its return code.
       *
       * @param pReturnCode The return code of the menu item
       *
       * @return The index of the item, or -1 if it's not found.
       */
      int getIndexByReturnCode(long pReturnCode) const;

      /**
       * \brief Returns an item's text by its return code.  If there is no
       * \brief item with the given return code, a blank string is returned.
       *
       * @param pReturnCode The return code to look for
       * @param pGetDisplayText If true (default), then this will return the text
       *  displayed on the menu for the item.  If false, then this will return
       *  the alternate text for the item.
       *
       * @return The item text for the given return code, or a blank string
       *  if there is no item with the given return code.
       */
      std::string getItemTextByReturnCode(long pReturnCode, bool pGetDisplayText = true);

      /**
       * \brief Sets a function to be run at the start of each
       * \brief input loop.  The return value of the function is
       * \brief not used.
       *
       * @param pFuncPtr A pointer to the function
       */
      void setLoopStartFunction(const std::shared_ptr<cxFunction>& pFuncPtr);

      /**
       * \brief Sets a function to be run at the end of each
       * \brief cycle through the input loop.  The return value
       * \brief of the function is not used.
       *
       * @param pFuncPtr A pointer to the function
       */
      void setLoopEndFunction(const std::shared_ptr<cxFunction>& pFuncPtr);

      /**
       * \brief Runs the loop start function, if it's set.
       *
       * @return The value of the loop start function's mExitAfterRun, or
       *  false if the loop start function isn't set.
       */
      virtual bool runLoopStartFunction();

      /**
       * \brief Runs the loop end function, if it's set.
       *
       * @return The value of the loop end function's mExitAfterRun, or
       *  false if the loop start end isn't set.
       */
      virtual bool runLoopEndFunction();

      /**
       * \brief Returns the type of a menu item (by index), or -1 if the given
       * \brief index is not valid.
       *
       * @param pIndex The index of the menu item
       *
       * @return The type of the menu item
       */
      cxMenuItemType getItemType(unsigned pIndex) const;

      /**
       * \brief Returns the type of a menu item (by item text), or -1 if the
       * \brief given item textis not valid.
       *
       * @param pItemText The text of the menu item
       *
       * @return The type of the menu item
       */
      cxMenuItemType getItemType(const std::string& pItemText) const;

      /**
       * \brief Enables or disables wrapping when trying to scroll beyond
       * \brief the last or first item.
       *
       * @param pWrap Whether or not to enable wrapping
       */
      virtual void setWrapping(bool pWrap);

      /**
       * \brief Returns whether scroll wrapping is enabled.
       *
       * @return Whether or not scroll wrapping is enabled
       */
      virtual bool getWrapping() const;

      /**
       * \brief Toggles whether or not the menu should display a message
       * \brief and wait for user input if there are no menu items.  This
       * \brief is the default behavior.
       *
       * @param pWaitForInputIfEmpty Whether or not to display a message and
       *  wait for user input if there are no menu items.
       */
      virtual void setWaitForInputIfEmpty(bool pWaitForInputIfEmpty);

      /**
       * \brief Returns whether or not the menu will display a message
       * \brief and wait for user input if there are no menu items.
       *
       * @return Whether or not the menu will display a message and wait for
       *  user input if there are no menu items.
       */
      virtual bool getWaitForInputIfEmpty() const;

      /**
       * \brief Returns the index of the current menu item.
       *
       * @return The index of the current menu item
       */
      virtual int getCurrentMenuItem() const;

      /**
       * \brief Sets the current menu item (by index)
       *
       * @param pItemIndex The index of the new menu item
       * @param pRefresh Whether or not to refresh the menu after setting
       *  the menu item (defaults to false)
       */
      virtual void setCurrentMenuItem(int pItemIndex, bool pRefresh = false);

      /**
       * \brief Changes the behavior of exiting the input loop when the user
       * \brief presses the up arrow on the first menu item.
       *
       * @param pExitWhenLeaveFirst If true, then when the user presses the up
       *  arrow on the first menu item, the menu will exit its input loop.
       */
      void setExitWhenLeaveFirst(bool pExitWhenLeaveFirst);

      /**
       * \brief Returns whether the menu will exit its input loop when the
       * \brief user presses the up arrow on the first menu item.
       *
       * @return Whether or not the menu will exit its input loop when the
       *  user prsses the up arrow on the first menu item.
       */
      bool getExitWhenLeaveFirst() const;

      /**
       * \brief Changes the behavior of exiting the input loop when the user
       * \brief presses the down arrow on the last menu item.
       *
       * @param pExitWhenLeaveFirst If true, then when the user presses the
       *  down arrow on the last menu item, the menu will exit its input loop.
       */
      void setExitWhenLeaveLast(bool pExitWhenLeaveLast);

      /**
       * \brief Returns whether the menu will exit its input loop when the
       * \brief user presses the down arrow on the last menu item.
       *
       * @return Whether or not the menu will exit its input loop when the
       *  user prsses the down arrow on the last menu item.
       */
      bool getExitWhenLeaveLast() const;

      /**
       * \brief Sets whether the list of items should be refreshed when the
       * \brief menu is shown modally.
       *
       * @param pRefreshItemsWhenModal Whether or not the list of htems should
       *  be refreshed when the menu is shown modally
       */
      void setRefreshItemsWhenModal(bool pRefreshItemsWhenModal);

      /**
       * \brief Returns whether or not the list of items will be refreshed when
       * \brief the menu is shown modally.
       *
       * @return true if the list of items will be refreshed when the menu is
       *  shown modally, or false if not.
       */
      bool getRefreshItemsWhenModal() const;

      /**
       * \brief Adds an ncurses attribute to use for one of the items in the
       * \brief window (see the e_WidgetItems enumeration).
       *
       * @param pItem The item to add the attribute for (see the e_WidgetItems
       *  enumeration).
       * @param pAttr The ncurses atribute to add.
       */
      virtual void addAttr(e_WidgetItems pItem, attr_t pAttr) override;

      /**
       * \brief Sets the ncurses attribute to use for one of the items in the
       * \brief window (see the e_WidgetItems enumeration).  Clears the current
       * \brief set of attributes for the item and inserts the given attribute
       * \brief into the set.
       *
       * @param pItem The item to apply the attribute for (see the e_WidgetItems
       *  enumeration).
       * @param pAttr The ncurses atribute to apply.
       */
      virtual void setAttr(e_WidgetItems pItem, attr_t pAttr) override;

      /**
       * \brief Removes an ncurses attribute from one of the item lists.
       *
       * @param pItem The item to remove the attribute for (see the
       *  e_WidgetItems enumeration).
       * @param pAttr The ncurses attribute to remove
       */
      virtual void removeAttr(e_WidgetItems pItem, attr_t pAttr) override;

      /**
       * \brief Removes all attributes for a given window item.
       *
       * @param pItem The item to remove attributes for (see the e_WidgetItems
       *  enumeration).
       */
      virtual void removeAttrs(e_WidgetItems pItem) override;

      /**
       * \brief Returns the set of ncurses attributes for a given item.
       *
       * @param pItem The item to retrieve attributes for (see the e_WidgetItems
       *  enumeration).
       * @param pAttrs This will contain the attributes for the item.
       */
      virtual void getAttrs(e_WidgetItems pItem, std::set<attr_t>& pAttrs) const override;

      /**
       * \brief Enables/disables the use of hotkey attributes.
       * \brief If true, wherever there's an ampersand (&) before a character in
       * \brief the message, that character will have hotkey attributes
       * \brief applied to it.
       *
       * @param pHotkeyHighlighting Whether or not to use hotkey attributes.
       */
      virtual void setHotkeyHighlighting(bool pHotkeyHighlighting) override;

      /**
       * \brief Returns the name of the cxWidgets class, "cxMenu".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxMenu").
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief Sorts the menu items by their return code.  Note: If the same
       * \brief return code exists for more than 1 item, then other attributes
       * \brief for those menu items (i.e., a sub-menu, unselectability, help
       * \brief text, etc.) will be duplicated too.  Use with care!
       *
       * @param pRefresh Whether or not to refresh the menu.  Defaults to
       *  false.
       */
      virtual void sortByReturnCode(bool pRefresh = false);

      /**
       * \brief Returns the highest return code of the menu items.
       *
       * @return The highest return code of the menu items
       */
      long getHighestReturnCode() const;

      /**
       * \brief Returns whether an item was selected the last time the menu was
       * \brief shown (either from the menu or from a submenu).
       *
       * @return true if an item was selected, or false if not.
       */
      virtual bool itemWasSelected() const;

      /**
       * \brief Returns whether the last mouse event occurred in the area where
       * \brief the menu items are listed.  If so, then this will also get the
       * \brief index of the menu item that is located there.  If there is no
       * \brief item at that location or if the mouse event didn't occur in
       * \brief the item, the index this gets will be -1.
       * \brief Note that the return code of the item can be retrieved using
       * \brief the version of getReturnCode() that takes an item index.
       *
       * @param pItemIndex (OUT) This will be set to the index of the item
       *  that was clicked on, or -1 if the mouse event didn't occur at one
       *  of the menu items.
       *
       * @return true if the last mouse event occurred in the item area, or
       *  false if not.
       */
      bool mouseEvtWasInItemArea(int& pItemIndex) const;

      /**
       * \brief Sets a function to run when the user selects an item.
       *
       * @param pFunction A pointer to the function to run.  Must have this
       *  signature: string func(void*, void*, void*, void*)
       * @param p1 A pointer to the first parameter to pass to the function
       * @param p2 A pointer to the second parameter to pass to the function
       * @param p3 A pointer to the 3rd parameter to pass to the function
       * @param p4 A pointer to the 4th parameter to pass to the function
       * @param pExitAfterRun Whether or not the menu should exit after the
       *  function runs
       * @param pRunOnLeaveFunction Whether or not the function should run
       *  its onLeave function if it exits after the function runs
       *
       * @return true if the function was set, or false if not
       */
      virtual bool setOnSelectItemFunction(funcPtr4 pFunction, void *p1,
                                           void *p2, void *p3, void *p4,
                                           bool pExitAfterRun,
                                           bool pRunOnLeaveFunction);

      /**
       * \brief Sets a function to run when the user selects an item.
       *
       * @param pFunction A pointer to the function to run.  Must have this
       *  signature: string func(void*, void*)
       * @param p1 A pointer to the first parameter to pass to the function
       * @param p2 A pointer to the second parameter to pass to the function
       * @param pExitAfterRun Whether or not the menu should exit after the
       *  function runs
       * @param pRunOnLeaveFunction Whether or not the function should run
       *  its onLeave function if it exits after the function runs
       *
       * @return true if the function was set, or false if not
       */
      virtual bool setOnSelectItemFunction(funcPtr2 pFunction, void *p1,
                                           void *p2, bool pExitAfterRun,
                                           bool pRunOnLeaveFunction);

      /**
       * \brief Sets a function to run when the user selects an item.
       *
       * @param pFunction A pointer to the function to run.  Must have this
       *  signature: string func()
       * @param pExitAfterRun Whether or not the menu should exit after the
       *  function runs
       * @param pRunOnLeaveFunction Whether or not the function should run
       *  its onLeave function if it exits after the function runs
       *
       * @return true if the function was set, or false if not
       */
      virtual bool setOnSelectItemFunction(funcPtr0 pFunction,
                                           bool pExitAfterRun,
                                           bool pRunOnLeaveFunction);

      /**
       * \brief Returns the pointer to the onSelectMenuItem function.
       *
       * @return The pointer to the onSelectMenuItem function
       */
      std::shared_ptr<cxFunction> getOnSelectItemFunction() const;

   protected:
      /**
       * \brief Menu selection attributes
       */
      std::set<attr_t> mMenuSelectionAttrs;

      /**
       * \brief Makes a copy of a cxMenu's member variables
       *
       * @param pThatMenu A pointer to another cxMenu whose members to copy
       */
      void copyCxMenuStuff(const cxMenu* pThatMenu);

      /**
       * \brief Returns the top row of the subwindow
       *
       * @return The top row of the subwindow
       */
      int subWinTop() const;

      /**
       * \brief Returns the left column of the subwindow
       *
       * @return The left column of the subwindow
       */
      int subWinLeft() const;

      /**
       * \brief Handles the input loop and returns the menu item's return
       * \brief code.  Could also return cxID_QUIT if the user presses ESC.
       *
       * @param pRunOnLeavFunction (OUT) Whether or not the onLeave function
       *  should be run
       *
       * @return The return code associated with the item selected (passed
       *  in with the append() function for the item).  Could also return
       *  cxID_QUIT if the user presses ESC.
       */
      long doInputLoop(bool& pRunOnLeaveFunction);

      /**
       * \brief Enables the attributes for one of the m*Attrs sets for an ncurses window.
       * \brief If the m*Attrs collection is empty, this will enable the cxBase attributes
       * \brief instead.
       *
       * @param pWin A pointer to the ncurses window for which to enable the
       *  attributes (will usually be mWindow).
       * @param pItem The item type to retrieve attributes for - see the
       *  e_cxWidgetItems enumeration (defined in cxWidgetItems.h).
       */
      virtual void enableAttrs(WINDOW *pWin, e_WidgetItems pItem) override;

      /**
       * \brief Disables the attributes for one of the m*Attrs sets for an ncurses window.
       * \brief  If the m*Attrs collection is empty, this will disable the cxBase attributes
       * \brief  instead.
       *
       * @param pWin A pointer to the ncurses window for which to disable the
       *  attributes (will usually be mWindow).
       * @param pItem The item type to retrieve attributes for - see the
       *  e_cxWidgetItems enumeration (defined in cxWidgetItems.h).
       */
      virtual void disableAttrs(WINDOW *pWin, e_WidgetItems pItem) override;

   private:
      WINDOW *mSubWindow = nullptr;                      // For scrolling
      int mSubWinHeight;                                 // Height of the subwindow
      int mSubWinWidth;                                  // Width of the subwindow
      int mCurrentMenuItem = 0;                          // Keeps track of the currently highlighted item
      int mTopMenuItem = 0;                              // The index of the topmost menu item shown
      int mAltPgUpKey = DEFAULT_CXMENU_ALT_PGUP_KEY;     // Alternate key for doing a pageUp
      int mAltPgDownKey = DEFAULT_CXMENU_ALT_PGDOWN_KEY; // Alternate key for doing a pageDown
      int mSearchKey = DEFAULT_CXMENU_SEARCH_KEY;        // Key to use for item searching
      std::string mSearchText;                           // For item searching
      std::string mLastSearchText;                       // For item searching
      bool mClearOnSearch = false;                       // Whether or not to clear the search text
      bool mCaseSensitiveSearch = false;                 // Whether or not searches are case-sensitive
      // mCustomStatus will be true if we're using custom
      //  status text (not the help text for the menu items)
      bool mCustomStatus = false;
      bool mAllowQuit = true;                            // Whether or not to allow the user to quit
      bool mAllowExit = true;                            // Whether or not to allow the user to exit
      bool mSelectableItemExists = false;                // Whether or not a selectable item exists
      bool mWrap = true;                                 // Whether or not to wrap around when scrolling
      bool mWaitForInputIfEmpty = true;                  // Whether or not to get input in showModal()
                                                         // if there are no menu items
      std::vector<long> mReturnCodes;                    // Return codes for the menu items
      std::vector<std::string> mHelpStrings;             // Help strings for the menu items
      std::vector<cxMenuItemType> mItemTypes;            // Stores the type of each menu item
      std::vector<std::string> mAltItemText;             // Alternate text for each item
      // mSubMenus stores submenus for menu items.  The keys to this
      // map are the indexes in mMessageLines that the submenus belong
      // to.
      std::map<int, cxMenu*> mSubMenus;
      std::set<unsigned> mUnselectableItems; // Contains indexes of unselectable items

      // Functions to be run at various points in the input loop
      std::shared_ptr<cxFunction> mLoopStartFunction; // At the start of each input loop iteration
      std::shared_ptr<cxFunction> mLoopEndFunction;   // At the end of each input loop iteration

      // mOnSelectItemFunction is a function that can be fired when the user
      // selects an item.
      std::shared_ptr<cxFunction> mOnSelectItemFunction;

      // mItemHotkeys keeps track of which keys are mapped to which menu items.
      std::multimap<char, int> mItemHotkeys;
      // If there is a hotkey that goes to more than 1 menu item, we
      //  want the user to be able to continue pressing the hotkey to
      //  go to the next item, rather than automatically selecting the
      //  item, which is the default behavior.  mHotkeyIndex keeps
      //  track of the "index" into mItemHotkeys of the current element with
      //  the hotkey that should be checked when the user presses a
      //  hotkey.
      unsigned int mItemHotkeyIndex = 0;
      // Keeps track of the last hotkey that was pressed If different,
      // mItemHotkeyIndex will need to be reset to 0)
      int mLastItemHotkey = NOKEY;
      // mExitWhenLeaveFirst and mExitWhenLeaveLast control whether the menu
      //  will exit the input loop when the user is scrolling through the menu
      //  items and leaves the first or  last menu item, respectively.
      bool mExitWhenLeaveFirst = false;
      bool mExitWhenLeaveLast = false;
      // mRefreshItemsWhenModal controls whether or not the menu items will
      //  be refreshed when showModal() is called.
      bool mRefreshItemsWhenModal = false;
      // mNumParentMenus is incremented when a cxMenu is added to another menu
      //  as a pull-right or pop-up menu.  It is decremented when the menu is
      //  removed.
      int mNumParentMenus = 0;
      // mLastInputWasMouseEvent will be set true if the last input
      //  was a mouse event in the input loop.
      bool mLastInputWasMouseEvent = false;

      // Returns the index of the bottommost item to show in the subwindow.
      //  Note: If there are less items than will fill the menu, this returns
      //  the index of the last item.
      inline int getBottomItemIndex();

      // Shows the submenu for the current menu item and returns the
      //  return code returned by the submenu.  Returns -1 if the
      //  current menu item isn't a submenu type.
      inline long doSubmenu();

      // Writes a menu item line (from mMessageLines) to the subwindow.
      //  Checks for hotkeys (ampersands), and pads with spaces on the
      //  right so it fills the entire row in the subwindow.
      // pItemIndex: The index of the item (in mMessageLines) to write
      // pSubWinRow: The row in the subwindow to write on
      // pRefreshSubwindow: Whether or not to refresh the subwindow
      // pHighlight: Whether or not to highlight the item
      void drawMenuItem(int pItemIndex, int pSubWinRow, bool pRefreshSubwindow,
                        bool pHighlight = false);

      // Scans a string for a hotkey and adds it hotkey to mItemHotkeys, if the
      //  string contains one.
      void addHotKey(const std::string& pItemText);

      // Makes sure this window is exactly the size needed to
      //  surround the menu items.
      //void fitToItems();

      // Copies the current menu item's help string to the
      //  status string, and optionally updates the status.
      inline void useHelpAsStatus(bool pRefreshStatus = false);

      // Goes through the menu items and resizes their text
      //  to make sure they're all exactly the width of the inside of
      //  this window.
      void fitItemsToWidth();

      // Makes sure mCurrentItem is a selectable menu item.
      // Parameters:
      //  pForward: If true, go downward.  If false, go upward.
      //  pBringToTop: Whether or not to bring the item to the top
      void goToSelectableItem(bool pForward, bool pBringToTop);

      // Handles item searching
      void doSearch();

      // Scrolls up one menu item (helper for doInputLoop()).
      //
      // Parameters:
      //  pContinueOn (INOUT): This is the continueOn boolean that is used
      //   in the input loop.
      void scrollUpOne(bool& pContinueOn);

      // Scrolls down one menu item (helper for doInputLoop()).
      //
      // Parameters:
      //  pContinueOn (INOUT): This is the continueOn boolean that is used
      //   in the input loop.
      void scrollDownOne(bool& pContinueOn);

      // Scrolls up one page
      inline void doPgUp();

      // Scrolls down one page
      inline void doPgDown();

      // Re-creates the subwindow
      inline void reCreateSubWindow();

      // Frees the memory used by the subwindow
      inline void freeSubWindow();

      // Updates mSelectableItemExists.  Returns the value of
      //  mSelectableItemExists (to support recursive calls).
      bool lookForSelectableItem();

      // For scrolling - Goes to the first menu item
      void doHome();

      // For scrolling - Goes to the last menu item
      void doEnd();

      // Returns a return code for the currently-selected menu item.  For use
      //  in the input loop when the user presses enter or tab.
      // Parameters:
      //  pLastKey: The last key pressed by the user
      //  pContinueOn: This variable gets set to whether the input loop
      //     should continue on.
      long getCurrentItemReturnCode(const int& pLastKey, bool& pContinueOn);

      // Returns the index of the first selectable item.  Returns -1 if there
      //  are no selectable items.
      int firstSelectableItem() const;

      // Returns the index of the last selectable item.  Returns -1 if there
      //  are no selectable items.
      int lastSelectableItem() const;

      // Makes an item highlighted.  Also uses its help as the status, etc.
      //
      // Parameters:
      //  pItemIndex: The index of the item to highlight
      void highlightItem(int pItemIndex);

      // Frees the memory used by mOnSelectItemFunction and sets it to nullptr.
      void freeOnSelectItemFunction();

      // Runs the function for when the user selects an item, if one is set.
      //
      // Parameters:
      //  pExitAfterRun (OUT): This will store whether or not the menu should
      //   exit after the function runs.
      // pRunOnLeaveFunction (OUT): This will store whether or not the onLeave
      //  function should run.
      void runOnSelectItemFunction(bool& pExitAfterRun,
                                   bool& pRunOnLeaveFunction);

      // This function is used when copying another cxMenu.  It checks the
      //  parameters of the event functions (i.e., mOnSelectItemFunction), and
      //  if any of the parameters of the other menu's event functions point
      //  to the other menu, they will be changed to point to this one.
      //
      // Parameters:
      //  pMenu: Another cxMenu being copied
      void checkEventFunctionPointers(const cxMenu& pMenu);
};

#endif
