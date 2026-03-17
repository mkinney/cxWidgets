#ifndef __CXCOMBOBOX_H__
#define __CXCOMBOBOX_H__

#include "cxMultiLineInput.h"
#include "cxMenu.h"
#include <string>

/** \class cxComboBox
 *
 *   cxComboBox.h: This file defines the cxComboBox class
 *
 * Copyright (c) 2005-2007 Michael H. Kinney
 *
 * \brief Represents a combo box, which includes
 * \brief an input box with a drop-down menu of
 * \brief choices for the input.<br>
 * The menu is enabled by default; however, it can be disabled with a call to
 * toggleMenu().  When the menu portion is disabled, a cxComboBox acts just
 * like a cxMultiLineInput.<br>
 * Items can be appended to and removed from the menu with the appendToMenu()
 * and removeFromMenu() functions.  When appending items to the menu, you can
 * specify the text to be displayed and the alternate text for the item.  When
 * the user selects an item from the menu, by default, the displayed text will
 * be set in the input.  If you want to instead use the alternate text for the
 * menu items, you can call setUseAltMenuItemText(true).<br>
 * When the menu is enabled, the input portion of a cxComboBox is always 1 line
 * high, and the height of a cxComboBox includes the input height and the menu
 * height (i.e., if a cxComboBox is 15 lines high, the input portion is 1 line
 * high and the menu has 14 lines).  If you change the height of a cxComboBox,
 * and the menu is enabled, the input portion remains 1 line, and the menu is
 * resized.  If the menu is disabled, then the height of the input portion can
 * be changed, just as with cxMultiLineInput; however, when the menu is
 * re-enabled, the input portion will shrink back down to 1 line.
 *
 * \author $Author: erico $
 * \version $Revision: 1.30 $
 * \date $Date: 2007/12/04 00:23:46 $
 *
 */

#include <set>
using std::set;

class cxComboBox : public cxMultiLineInput
{
   public:
      /**
       * \brief Default constructor.
       * @param pParentWindow A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the window.  Note that the height of
       *  the input is always 1.  This parameter includes the input plus
       *  the height of the menu, which will appear below the input (i.e.,
       *  a height of 10 means the 1-line input and a 9-line menu).
       * @param pWidth The width of the window.
       * @param pLabel The label of the input
       * @param pBorderStyle The type of border to use - can be
       *  eBS_SINGLE_LINE for a single-line border or eBS_NOBORDER
       *  for no border.
       * @param pInputOption Specifies the editability of the input.  Defaults
       *  to eINPUT_EDITABLE.  The enNputOptions enumeration is in the file
       *  cxInputOptions.h.
       * @param pInputType Specifies the type of input (text, numeric floating
       *  point, or numeric whole).  This defaults to eINPUT_TYPE_TEXT.
       * @param pExtValue Pointer to external input value; defaults to nullptr
       * @param pMenuEnabled Whether or not the menu should be enabled (defaults
       *  to true)
       * @param pRightlabelOffset The distance between the right edge of the
       *  input and the right label.  Defaults to 0.
       * @param pRightLabelHeight The height of the label to the right of the
       *  input (defaults to 1)
       * @param pRightLabelHeight The width of the label to the right of the
       *  input (defaults to 5)
       * @param pShowRightLabel Whether or not to show the right label.
       *  Defaults to false.
       */
      explicit cxComboBox(cxWindow *pParentWindow = nullptr,
                       int pRow = 0, int pCol = 0,
                       int pHeight = DEFAULT_HEIGHT,
                       int pWidth = DEFAULT_WIDTH,
                       const std::string& pLabel = "",
                       eBorderStyle pBorderStyle = eBS_NOBORDER,
                       eInputOptions pInputOption = eINPUT_EDITABLE,
                       eInputTypes pInputType = eINPUT_TYPE_TEXT,
                       std::string *pExtValue = nullptr, bool pMenuEnabled = true,
                       int pRightLabelOffset = 0,
                       int pRightLabelHeight = 1,
                       int pRightLabelWidth = 5,
                       bool pShowRightLabel = false);

      // Copy constructor
      cxComboBox(const cxComboBox& pThatComboBox);

      virtual ~cxComboBox();

      /**
       * \brief Shows the window and waits for input.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *   to true.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults
       *   to true.
       *
       * @return A return code based on user input.  Returns cxID_QUIT if the user
       *  presses ESC; otherwise, returns cxID_EXIT.
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = true) override;

      /**
       * \brief Changes window's width and height.  The window's upper-left
       * \brief coordinate stays the same.  Note that the height must
       * \brief be at least 2 if the combo box doesn't have borders, or 4
       * \brief if it has borders, to leave room for both the input and
       * \brief the drop-down menu.
       *
       * @param pNewHeight The new height
       * @param pNewWidth The new width
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void resize(int pNewHeight, int pNewWidth, bool pRefresh = true) override;

      /**
       * \brief Changes the window's position, based on a new upper-left corner
       *
       * @param pNewRow The new topmost row of the form
       * @param pNewCol The new leftmost column of the form
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

      /**
       * \brief Erases the window
       * @param pEraseSubwindows Whether or not to erase the subwindows (defaults to
       *    true)
       */
      virtual void erase(bool pEraseSubwindows = true) override;

      /**
       * \brief Hides the window
       * @param pHideSubwindows Whether or not to also hide the subwindows (defaults
       *    to true)
       */
      virtual void hide(bool pHideSubwindows = true) override;

      /**
       * \brief Un-hides the window
       * @param pUnhideSubwindows Whether or not to also unhide the subwindows
       *    (defaults to true)
       */
      virtual void unhide(bool pUnhideSubwindows = true) override;

      /**
       * \brief Adds a string item to the end of the menu.
       *
       * @param pDisplayText The text to display for the item
       * @param pItemText Alternate text to use for the item
       * @param pShowMenu Whether or not to refresh the menu after the
       *  item has been added (defaults to false).
       * @param pBringToTop Whether or not to bring the menu to the top if
       *  it's being shown (defaults to false).
       */
      virtual void appendToMenu(const std::string& pDisplayText,
                                const std::string& pItemText = "",
                                bool pRefresh = false, bool pBringToTop = false);

      /**
       * \brief Removes a menu item from the menu.
       *
       * @param pItemIndex Index of the item to delete
       * @param pRefresh Whether or not to refresh the menu (defaults to false)
       *
       * @return True if the item was deleted; false if not.
       */
      virtual bool removeFromMenu(unsigned pItemIndex, bool pRefresh = false);

      /**
       * \brief Removes a menu item from the menu.
       *
       * @param pItemText The text of the item to delete
       * @param pDisplayText If true (default), then pItemText specifies the
       *  text displayed on the menu.  If false, then pItemText specifies the
       *  alternate item text.
       * @param pRefresh Whether or not to refresh the menu (defaults to false)
       *
       * @return True if the item was deleted; false if not.
       */
      virtual bool removeFromMenu(const std::string& pItemText,
                                  bool pDisplayText = true,
                                  bool pRefresh = false);

      /**
       * \brief Removes all items from the menu.
       */
      virtual void removeAllItemsFromMenu();

      /**
       * \brief Shows the drop-down menu
       *
       * @param pBringToTop Whether or not to bring the menu to the top
       *  (defaults to false)
       */
      virtual void showMenu(bool pBringToTop = false);

      /**
       * \brief Hides the drop-down menu
       */
      virtual void hideMenu();

      /**
       * \brief Returns the number of items on the menu
       *
       * @return The number of items on the menu
       */
      virtual unsigned numMenuItems() const;

      /**
       * Returns the text for a menu item.
       * @param pIndex The index of the menu item
       * @return The menu item text at the specified index, or a blank string if the index is out of bounds.
       */
      virtual std::string getItemText(int pIndex) const;

      /**
       * \brief Returns the total height of the combo box, including
       * \brief the input and the menu.
       *
       * @return The total height of the combo box
       */
      virtual int height() const;

      /**
       * \brief Returns the width of the combo box
       *
       * @return The width of the combo box
       */
      virtual int width() const;

      /**
       * \brief Returns the height of the input portion of
       * \brief the combo box.
       *
       * @return The height of the input portion
       */
      virtual int inputHeight() const;

      /**
       * \brief Returns the width of the input portion of
       * \brief the combo box.
       *
       * @return The width of the input portion
       */
      virtual int inputWidth() const;

      /**
       * \brief Returns the height of the menu portion of
       * \brief the combo box.
       *
       * @return The height of the menu portion
       */
      virtual int menuHeight() const;

      /**
       * \brief Returns the width of the menu portion of
       * \brief the combo box.
       *
       * @return The width of the menu portion
       */
      virtual int menuWidth() const;

      /**
       * \brief Allows the height of the menu portion to be changed.
       *
       * @param pNewHeight The new height of the menu portion
       * @param pRefresh Whether or not to refresh the form (defaults
       *  to false)
       */
      virtual void setMenuHeight(int pNewHeight, bool pRefresh = false);

      /**
       * \brief Enables or disables the menu.
       *
       * @param pEnableMenu If true, the drop-down menu will be enabled;
       *  if false, the drop-down menu will be disabled.
       * @param pRefresh Whether or not to refresh the combo box, in case
       *  the input portion needs to resize(defaults to true)
       */
      virtual void toggleMenu(bool pEnableMenu, bool pRefresh = true);

      /**
       * \brief Returns whether or not the menu is enabled.
       *
       * @return Whether or not the menu is enabled (true/false)
       */
      virtual bool menuIsEnabled() const;

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief Returns a pointer to the drop-down menu.
       *
       * @return A pointer to the drop-down menu
       */
      cxMenu* getMenu();

      /**
       * \brief Sets a single key that will show the drop-down menu.
       *
       * @param pKey A key to set as the drop-down menu key
       */
      virtual void setDropDownKey(int pKey);

      /**
       * \brief Adds a key that will show the drop-down menu.
       *
       * @param pKey A key to add
       */
      virtual void addDropDownKey(int pKey);

      /**
       * \brief Sets the keys that will show the drop-down menu.
       *
       * @param pKeys A set of keys
       */
      virtual void setDropDownKeys(const set<int>& pKeys);

      /**
       * \brief Removes a key from the list of keys that will show the
       * \brief drop-down menu.
       *
       * @param pKey A key to remove
       */
      virtual void removeDropDownKey(int pKey);

      /**
       * \brief Returns whether or not a key is in the list of drop-down keys
       *
       * @param pKey A key to check
       *
       * @return true if the key is in the list of drop-down keys, or false if
       *  not
       */
      virtual bool hasDropDownKey(int pKey) const;

      /**
       * \brief Sets whether or not the validator function should be used.
       *
       * @param pRunValidatorFunction Whether or not the validator function
       *  should be used
       */
      virtual void toggleValidatorFunction(bool pRunValidatorFunction);

      /**
       * \brief Returns whether or not the validator function should be used.
       *
       * @return Whether or not the validator function will be used
       */
      virtual bool getUseValidatorFunction() const;

      /**
       * \brief Sets whether or not to use the alternate text for all items on
       * \brief the drop-down menu.  If this is set true, then when the user
       * \brief selects an item, the item's alternate text will get set in the
       * \brief input, rather than the item's displayed text.
       *
       * @param pUseAltMenuItemText Whether or not to use the alternate text
       *  for all items on the drop-down menu
       */
      virtual void setUseAltMenuItemText(bool pUseAltMenuItemText);

      /**
       * \brief Returns whether or not the alternate text of the drop-down menu
       * \brief items will be used.
       *
       * @return Boolean: Whether or not the alternate text of the drop-down
       *  menu items will be used
       */
      virtual bool getUseAltMenuItemText() const;

   protected:
      /**
       * \brief Handles the input loop.  Returns cxID_QUIT or cxID_EXIT,
       * \brief depending on the user's input.  This function is meant
       * \brief not to be virtual, since it is specifically for
       * \brief cxComboBox.
       *
       * @param pShowSelf Whether or not to show the input window
       *  before doing the input loop.
       * @param pRunOnLeaveFunction This will contain a boolean value
       *  that will signify whether or not to run the onLeave function
       *  (which will be set false if the user presses a function key
       *  not set in the cxMultiLineInput object that the parent cxForm
       *  or any of its parent cxMultiForm objects have set).
       *
       * @return A return code (cxID_QUIT or cxID_EXIT, depending on the
       *  user's input).
       */
      long doInputLoop(bool pShowSelf, bool& pRunOnLeaveFunction);

      /**
       * \brief Makes a copy of a cxComboBox's member variables
       *
       * @param pThatComboBox A pointer to another cxComboBox to copy
       */
      void copyCxComboBoxStuff(const cxComboBox* pThatComboBox);

      /**
       * \brief Returns whether or not the drop-down menu is modal.
       *
       * @return true if the drop-down is modal, or false if not
       */
      bool menuIsModal() const;

   private:
      cxMenu mMenu;             // The drop-down menu
      bool mUseAltMenuItemText = false; // Whether or not to use the alternate text for menu items
      set<int> mDropDownKeys;   // Keys that show the drop-down menu
      // mRunValidatorFunction controls whether or not the validator function
      //  should run.  Note that cxMultiLineInput has this same bool too;
      //  however, cxComboBox will disable that one because cxComboBox will
      //  run the validator.
      bool mRunValidatorFunction = true;

      // This is a helper for doInputLoop().  This shows the drop-down menu if
      //  it's enabled.  This will return a return code to be used for the
      //  input loop and also set whether or not the input loop should
      //  continue.
      //
      // Parameters:
      //  pShowMenu: Whether or not to have the menu show its window when
      //   showModal() is called for it (for a screen optimization)
      //  pContinueOn (OUT): This will contain whether or not the input loop
      //   should continue.
      //
      // Return: The return code for use with the input loop & showModal()
      long showMenuModal(bool pShowMenu, bool& pContinueOn);
};

#endif
