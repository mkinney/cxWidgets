#ifndef __CXBASE_H__
#define __CXBASE_H__

/*
 * cxBase.h - Contains cxWidgets utility functions.
 * For the library to work, you will need to put
 * "cxBase::init();" at the beginning of your program and
 * "cxBase::cleanup();" at the end of your program.
 * This does the screen initialization/cleanup.
 *
 * Copyright (c) 2005-2007 Michael H. Kinney
 *
 * \author $Author: erico $
 * \version $Revision: 1.69 $
 * \date $Date: 2007/12/04 00:23:45 $
 *
 * Date     User      Description
 * 08/21/07 erico     Added new versions of messageBox() that let you specify
 *                    colors for the different parts of the message box.  Also
 *                    added some default colors.
 */


#include "cxWindow.h"
#include "cxColors.h"
#include "cxInputOptions.h"
#include "cxInputTypes.h"
#include "cxWidgetsException.h" // Included here so that everything gets it
#include "cxMessageDialogStyles.h"
#include <string>
#include <set>
#include <list>

/** \namespace cxBase
 * \brief Contains some miscellaneous cxWidgets helper functions.
 */
namespace cxBase
{

   /**
    * \brief Initializes the cxWidgets library.  This must be called before
    * \brief any screen-related cxWidgets library objects & functions are used.
    *
    * @param pEnableMouse Whether or not to enable mouse input.  Defaults to
    *  true.  Note that if the version of ncurses on the system doesn't support
    *  the mouse at the time when cxWidgets is compiled, the mouse will not be
    *  enabled, even if this parameter is true.
    */
   void init(bool pEnableMouse = true);

   /**
    * \brief Cleans up the cxWidgets library.  Must be called after all
    * \brief screen-related cxWidgets objects & functions are done being used.
    */
   void cleanup();

   /**
    * \brief Returns the top row # of the main screen.
    * @return The top row # of the main screen
    */
   int top();

   /**
    * \brief Returns the center row # of the main screen.
    * @return The center row # of the main screen
    */
   int centerRow();

   /**
    * \brief Returns the bottom row # of the main screen.
    * @return The bottom row # of the main screen
    */
   int bottom();

   /**
    * \brief Returns the leftmost column # of the main screen.
    * @return The leftmost column # of the main screen
    */
   int left();

   /**
    * \brief Returns the center column # of the main screen.
    * @return The center column # of the main screen
    */
   int centerCol();

   /**
    * \brief Returns the rightmost column # of the main screen.
    * @return The rightmost column # of the main screen
    */
   int right();

   /**
    * \brief Returns the width of the main screen.
    * @return The width of the main screen
    */
   int width();

   /**
    * \brief Returns the height of the main screen.
    * @return The height of the main screen
    */
   int height();

   /**
    * \brief Returns the terminal's current height & width.
    *
    * @param pHeight (OUT) This will store the terminal's current height.
    * @param pWidth (OUT) This will store the terminal's current width.
    */
   void getTermDimentions(int& pHeight, int& pWidth);

   /**
    * \brief Resizes the terminal screen.
    *
    * @param pNewHeight The new height (# of rows)
    * @param pNewWidth The new width (# of columns)
    *
    * @return true if successful or false if not
    */
   bool resizeTerm(int pNewHeight, int pNewWidth);

   /**
    * \brief Returns the default message color
    *
    * @return The default message color
    */
   e_cxColors getDefaultMessageColor();

   /**
    * \brief Sets the default message color
    *
    * @param pColor A member of the e_cxColors enumeration (see cxColors.h)
    */
   void setDefaultMessageColor(e_cxColors pColor);

   /**
    * \brief Returns the default title color
    *
    * @return The default title color
    */
   e_cxColors getDefaultTitleColor();

   /**
    * \brief Sets the default title color
    *
    * @param pColor A member of the e_cxColors enumeration (see cxColors.h)
    */
   void setDefaultTitleColor(e_cxColors pColor);

   /**
    * \brief Returns the default status color
    *
    * @return The default status color
    */
   e_cxColors getDefaultStatusColor();

   /**
    * \brief Sets the default status color
    *
    * @param pColor A member of the e_cxColors enumeration (see cxColors.h)
    */
   void setDefaultStatusColor(e_cxColors pColor);

   /**
    * \brief Returns the default border color
    *
    * @return The default border color
    */
   e_cxColors getDefaultBorderColor();

   /**
    * \brief Sets the default border color
    *
    * @param pColor A member of the e_cxColors enumeration (see cxColors.h)
    */
   void setDefaultBorderColor(e_cxColors pColor);

   /**
    * \brief Shows a message box.
    * @param pRow row of upper-left corner
    * @param pCol column of upper-left corner
    * @param pHeight height of message box
    * @param pWidth width of message box
    * @param pTitle title of message box
    * @param pMessage message of message box
    * @param pStatus status of message box
    * @param pBorderStyle The type of border to use
    *  for the message box - Defaults to eBS_SINGLE_LINE
    */
   void messageBox(int pRow, int pCol, int pHeight, int pWidth, const std::string& pTitle,
                   const std::string& pMessage, const std::string& pStatus = "",
                   eBorderStyle pBorderStyle = eBS_SINGLE_LINE);

   /**
    * \brief Shows a message box automatically setting its height & width.
    * @param pRow row of upper-left corner
    * @param pCol column of upper-left corner
    * @param pTitle title of message box
    * @param pMessage message of message box
    * @param pStatus status of message box
    */
   void messageBox(int pRow, int pCol, const std::string& pTitle,
                   const std::string& pMessage, const std::string& pStatus);

   /**
    * \brief Shows a message box, automatically setting its height & width.
    *
    * @param pRow Row of upper-left corner
    * @param pCol Column of upper-left corner
    * @param pMessage The message to appear in the box
    */
   void messageBox(int pRow, int pCol, const std::string& pMessage);

   /**
    * \brief Shows a message box
    * @param pTitle title of message box
    * @param pMessage message of message box
    * @param pStatus status of message box
    */
   void messageBox(const std::string& pTitle, const std::string& pMessage,
                   const std::string& pStatus);

   /**
    * \brief Shows a message box
    * @param pMessage Message of message box
    * @param pStatus Status of message box.  Defaults to blank.
    */
   void messageBox(const std::string& pMessage, const std::string& pStatus);

   /**
    * \brief Shows a message box.
    *
    * @param pMessage The message for the message box
    * @param pMessageColor The color for the message portion
    * @param pMessageAttr The attribute for the message portion
    * @param pTitleColor The color for the title
    * @param pStatusColor The color for the status
    * @param pBorderColor The color for the border
    */
   void messageBox(const std::string& pMessage,
                   e_cxColors pMessageColor = getDefaultMessageColor(),
                   attr_t pMessageAttr = A_NORMAL,
                   e_cxColors pTitleColor = getDefaultTitleColor(),
                   e_cxColors pStatusColor = getDefaultStatusColor(),
                   e_cxColors pBorderColor = getDefaultBorderColor());

   /**
    * \brief Shows a message dialog (a message box with an OK button)
    * @param pMessage message of message dialog
    */
   void messageDialog(const std::string& pMessage);

   /**
    * \brief Shows a message dialog (a message box with an OK button)
    * @param pMessages message of message dialog
    */
   void messageDialog(const std::list<std::string>& pMessages);

   /**
    * \brief Shows a splash message
    * @param pTitle Title of splash message
    * @param pMessage Message of splash message
    * @param pSleep Duration in seconds of splash message
    */
   void splash(const std::string& pTitle, const std::string& pMessage, int pSleep);

   /**
    * \brief Shows a splash message
    * @param pMessage Message of splash message
    * @param pSleep Duration in seconds of splash message
    */
   void splash(const std::string& pMessage, int pSleep);

   /**
    * \brief Erases the screen
    */
   void eraseScreen();

   /**
    * \brief Updates (refreshes) all windows on the screen
    */
   void updateWindows();

   /**
    * \brief Returns whether a key is a function key, pageDown, or pageUp
    * (these keys are tested by cxInput and cxMultiLineInput in their
    * input loops).
    *
    * @param pKey The key to be checked
    * @return Whether or not the key is a function key, pageDown, or pageUp
    */
   bool isFunctionKey(int pKey);

   /**
    * \brief Enables or disables the display of the cursor.
    *
    * @param pToggleCursor If true, the cursor will be enabled.
    * If false, the cursor will be disabled.
    *
    * @return The previous state of the cursor - true if it was enabled, or
    *  false if not.
    */
   bool toggleCursor(bool pToggleCursor);

   /**
    * \brief Returns a string representing the terminal type.  This is the
    * \brief value of the environment variable TERM.
    * @return A string representing the terminal type, i.e. "xterm", "wy50", etc.
    */
   std::string getTermType();

   /**
    * \brief Contains the terminal type string
    */
   static const std::string termType = getTermType();

   /**
    * \brief Returns whether a color has the brightness bit set.
    *
    * @param pColor The color to test
    *
    * @return Whether or not the color has the brightness bit set.
    */
   bool colorIsBright(e_cxColors pColor);

   /**
    * \brief Counts the number of hotkeys in a string.  This is useful when
    * working with cxMenu, which can contain items with hotkeys.
    *
    * @param pStr The string for which to count hotkeys
    *
    * @return The number of hotkeys in pStr
    */
   int countHotkeys(const std::string& pStr);

   /**
    * \brief Takes a string and returns a version without the & characters
    * \brief that are used for highlighting the next character.
    *
    * @param pStr The string to process
    *
    * @return A version of pStr without the highlighting characters.
    */
   std::string stringWithoutHotkeyChars(const std::string& pStr);

   /**
    * \brief Returns the length of a string without its hotkey characters -
    * \brief This is useful for counting the number of characters that
    * \brief will actually appear on the screen (not counting the
    * \brief characters that specify underlined characters).
    *
    * @param pStr The string to process
    *
    * @return The length of the string as it would appear on the screen
    */
   unsigned visualStrLen(const std::string& pStr);

   /**
    * \brief Brings a cxWindow to the top of the stack.  Doesn't refresh
    * \brief its subwindows.  This is designed to be used as an event
    * \brief function in cxWidgets (i.e., for when the user presses a certain
    * \brief key, etc.).
    *
    * @param theWindow A pointer to a cxWindow
    * @param unused Not used
    * @return A blank string
    */
   std::string bringToTop(void* theWindow, void* unused);

   /**
    * \brief Runs showModal() on a cxForm and returns the result.  This
    * \brief function was designed to be used as an event function in
    * \brief cxWidgets, which is why it returns the value as a string and
    * \brief takes 2 void pointers.
    *
    * @param theForm The cxForm to show modally
    * @param unused Not used
    *
    * @return The return value of the form's showModal(),
    *    as a string, or the string value of cxID_QUIT
    *    on error.
    */
   std::string cxFormShowModal(void* theForm, void* unused);

   /**
    * \brief Runs showModal() on a cxMenu and returns the result.  This
    * \brief function was designed to be used as an event function in
    * \brief cxWidgets, which is why it returns the value as a string and
    * \brief takes 2 void pointers.
    *
    * @param theMenu The cxMenu to show modally
    * @param unused Not used
    *
    * @return The return value of the form's showModal(),
    *    as a string, or the string value of cxID_QUIT
    *    on error.
    */
   std::string cxMenuShowModal(void* theMenu, void* unused);

   /**
    * \brief Does nothing.  Can be used, for example, as an event function
    * \brief on a cxForm when you want to have a keypress exit the form.
    *
    * @param p1 Not used
    * @param p2 Not used
    *
    * @return A blank string
    */
   std::string noOp(void *p1, void *p2);

   /**
    * \brief Does nothing.  Can be used, for example, as an event function
    * \brief on a cxForm when you want to have a keypress exit the form.
    *
    * @param p1 Not used
    * @param p2 Not used
    * @param p3 Not used
    * @param p4 Not used
    *
    * @return An empty string
    */
   std::string noOp(void *p1, void *p2, void *p3, void *p4);

   /**
    * \brief Runs a command on the system, calling the system() function.
    * \brief If your application has called cxBase::init(), this
    * \brief function should be called to run a system command.
    * \brief After running cxBase::init(), cxBase::cleanup() must be
    * \brief called before running system commands to get the
    * \brief terminal back to the state it was in before; then,
    * \brief cxBase::init() must be called again after the call to
    * \brief system() to restore cxWidgets graphical functions.
    * \brief This function does the wrapping for you.
    *
    * @param pCmd The command to run
    *
    * @return The return value of system()
    */
   int runSystemCmd(const std::string& pCmd);

   /**
    * \brief Writes text on the screen
    *
    * @param pRow The row at which to write the text
    * @param pCol The column at which to write the text
    * @param pText The text to be written
    * @param pAttr The text attribute (see the man page for wattron).  Defaults
    *  to normal.
    * @param pColor The text color (see e_cxColors enumeration in cxColors.h).
    *  Defaults to gray on black.
    */
   void writeText(int pRow, int pCol, const std::string& pText, attr_t pAttr = A_NORMAL, e_cxColors pColor = eGRAY_BLACK);

   /**
    * \brief Scans a string for hotkeys (characters preceeded by an ampersand)
    * \brief and adds the hotkey characters to a collection.
    *
    * @param pStr The string to scan
    * @param pHotkeys The collection to which to add hotkey characters
    * @param pConvertCase Whether or not to convert the case of the hotkeys
    *  (defaults to false)
    * @param ptoUpper If pConvertCase is true, this parameter specifies whether
    *  or not to convert the characters to upper case.  If true, the characters
    *  will be converted to upper-case; if false, they will be converted to
    *  lowercase (if pConvertCase is true).
    */
   void getHotkeyChars(const std::string& pStr, std::set<char>& pHotkeys,
                       bool pConvertCase = false, bool pToUpper = false);

   /**
    * \brief Shows all windows currently in use everywhere, from the
    * \brief bottom of the stack to the top.
    *
    * @param pBringToTop Whether or not to bring each window
    *  to the top as it's shown.
    */
   void showAllWindows(bool pBringToTop = false);

   /**
    * \brief Gets a pointer to the topmost cxWindow on the stack.  This could
    * \brief return nullptr, if it can't get to the topmost window.
    *
    * @return A pointer to the topmost cxWindow on the stack, or nullptr if there
    *  is none.
    */
   cxWindow* getTopWindow();

   /**
    * \brief Gets a pointer to the bottom-most cxWindow on the stack.  This could
    * \brief return nullptr, if it can't get to the topmost window.
    *
    * @return A pointer to the bottom-most cxWindow on the stack, or nullptr if
    *  there is none.
    */
   cxWindow* getBottomWindow();

   /**
    * \brief Returns a string version of a key.
    *
    * @param pKey The key to convert to a string
    *
    * @return A string representation of the key
    */
   std::string getKeyStr(int pKey);

   /**
    * \brief Returns a string version of a mouse state.
    *
    * @param pMouseState The mouse state to convert to a string
    *
    * @return A string representation of the mouse state
    */
   std::string getMouseStateStr(int pMouseState);

   /**
    * \brief Returns a string version of a return code.
    *
    * @param pReturnCode The return code to convert to a string
    *
    * @return A string representation of the return code
    */
   std::string getReturnCodeStr(long pReturnCode);

   /**
    * \brief Returns a string version of a value from the eInputOptions
    * \brief enumeration.
    *
    * @param pInputOption A member of the eInputOptions enumeration
    *
    * @return A string representation of pInputOption
    */
   std::string getInputOptionStr(eInputOptions pInputOption);

   /**
    * \brief Returns a string version of an eBorderStyle value.
    *
    * @param pBorderStyle A member of the eBorderStyle enumeration
    *
    * @return A string representation of the eBorderStyle value
    */
   std::string getBorderStyleStr(eBorderStyle pBorderStyle);

   /**
    * \brief Returns a string version of an eInputType value.
    *
    * @param pInputType A member of the eInputType enumeration
    *
    * @return A string representation of the value
    */
   std::string getInputTypeStr(eInputTypes pInputType);

   /**
    * \brief Handler for system signals
    *
    * @param pSignal The signal to handle
    */
   void signalHandler(int pSignal);

   /**
    * \brief Prompts the user with a cxMessageDialog for a yes/no choice.
    * \brief Returns their choice as a boolean.
    *
    * @param pMessage The message to put in the message dialog
    * @param pTitle The title for the message dialog.  Defaults to a blank string.
    * @param pButtons Specifies the buttons to appear on the dialog.  Defaults
    *  to cxYES_NO (to display yes & no buttons).  Note that you can use
    *  multiple values listed in cxMessageDialogStyles.h ORed together.  For
    *  example, cxYES_NO | cxNO_DEFAULT would display Yes and No buttons and
    *  default to No.
    *
    * @return True if the user chose yes, or false otherwise.
    */
   bool promptYesNo(const std::string& pMessage, const std::string& pTitle = "",
                    long pButtons = cxYES_NO);

   /**
    * \brief Returns whether or not cxWidgets has been initialized.
    *
    * @return Whether or not cxWidgets has been initialized.
    */
   bool cxInitialized();

   /**
    * \brief Adds an ncurses attribute to use for one of the items in the
    * \brief window (see the e_WidgetItems enumeration).
    *
    * @param pItem The item type to add the attribute for (see the e_WidgetItems
    *  enumeration).
    * @param pAttr The ncurses atribute to add.
    */
   void addAttr(e_WidgetItems pItem, attr_t pAttr);

   /**
    * \brief Sets the ncurses attribute to use for one of the items in the
    * \brief window (see the e_WidgetItems enumeration).  Clears the current
    * \brief set of attributes for the item and inserts the given attribute
    * \brief into the set.
    *
    * @param pItem The item type to apply the attribute for (see the e_WidgetItems
    *  enumeration).
    * @param pAttr The ncurses atribute to apply.
    */
   void setAttr(e_WidgetItems pItem, attr_t pAttr);

   /**
    * \brief Removes an ncurses attribute from one of the item lists.
    *
    * @param pItem The item type to remove the attribute for (see the
    *  e_WidgetItems enumeration).
    * @param pAttr The ncurses attribute to remove
    */
   void removeAttr(e_WidgetItems pItem, attr_t pAttr);

   /**
    * \brief Removes all attributes for a given window item.
    *
    * @param pItem The item type to remove attributes for (see the e_WidgetItems
    *  enumeration).
    */
   void removeAttrs(e_WidgetItems pItem);

   /**
    * \brief Returns the set of ncurses attributes for a given item.
    *
    * @param pItem The item type to retrieve attributes for (see the e_WidgetItems
    *  enumeration).
    * @param pAttrs This will contain the attributes for the item.
    */
   void getAttrs(e_WidgetItems pItem, std::set<attr_t>& pAttrs);


   /**
    * @brief Determine if the widget has an attribute "set"
    *
    * @param pItem The item type to check (see e_WidgetItems)
    * @param pAttr The attribute. (i.e., A_NORMAL, etc.)
    *
    * @return true if the widget has that attribute
    */
   bool hasAttr(e_WidgetItems pItem, attr_t pAttr);

   /**
    * \brief Enables all of a set of attributes for a given item type on an
    * \brief ncurses window.
    *
    * @param pWindow A pointer to the ncurses window for which to enable the
    *  attributes.
    * @param pItem The item type to retrieve attributes for (see the
    *  e_cxWidgetItems enumeration).
    */
   void enableAttrs(WINDOW *pWindow, e_WidgetItems pItem);

   /**
    * \brief Disables all of a set of attributes for a given item type on an
    * \brief ncurses window.
    *
    * @param pWindow A pointer to the ncurses window for which to disable the
    *  attributes.
    * @param pItem The item type to retrieve attributes for (see the
    *  e_cxWidgetItems enumeration).
    */
   void disableAttrs(WINDOW *pWindow, e_WidgetItems pItem);

   /**
    * \brief Figure out the appropriate ncurses attr_t from a "string"
    * \brief see "man curs_attr"
    *
    * @param pAttr "NORMAL", "BOLD", etc..
    *
    * @return attr_t that corresponds with that string
    */
   attr_t getAttr(const std::string& pAttr);

   /**
    * \brief Returns a string version of an ncurses attribute.
    *
    * @param pAttr An ncurses attribute
    *
    * @return A descriptive string version of the attribute
    */
   std::string getAttrStr(attr_t pAttr);

   /**
    * \brief Returns a string version of an e_WidgetItems value.
    *
    * @param pItem A member of the e_WidgetItems enumeration (see
    *  cxWidgetItems.h)
    *
    * @return The string version of the item
    */
   std::string getWidgetItemStr(e_WidgetItems pItem);

   /**
    * \brief Sets the key that should be used to clear the "Keyword" input
    * \brief for cxMenu item searching.
    *
    * @param pKey The key that should be used to clear the "Keyword" input
    *  for cxMenu item searching.
    */
   void setMenuClearKeywordKey(int pKey);

   /**
    * \brief Returns the key that is set to clear the "Keyword" input for
    * \brief cxMenu item searching.
    *
    * @return The key that is set to clear the "Keyword" input for cxMenu item
    *  searching.
    */
   int getMenuClearKeywordKey();

   /**
    * @brief dump the contents to the screen to a file
    *
    * @param pFancy - print fields that have underlines
    *     as underscores '_'.
    * @param pFilename to write to
    *
    * @return string "" on success otherwise message
    */
   std::string dump(bool pFancy=false, const std::string& pFilename="");

   /**
    * \brief Returns whether a key is a navigational key, according to
    * \brief cxBase's list of navigational keys.  The defaults are the arrow
    * \brief keys, pageUp, pageDown, tab, and shift-tab.  cxMultiLineInput
    * \brief has its own list of navigational keys, but this may be useful
    * \brief for other things.
    *
    * @param pKey The key to test
    *
    * @return Whether or not the key is a navigational key
    */
   bool isNavKey(int pKey);

   /**
    * \brief Add a key to be considered a "navigational" key.  The key will
    * \brief be added to cxBase's set of navigational keys.
    *
    * @param pKey The key to be added
    */
   void addNavKey(int pKey);

   /**
    * \brief Removes a key from cxBase's list of "navigational" keys.
    *
    * @param pKey The key to be removed
    */
   void removeNavKey(int pKey);

   /**
    * @brief Get the contents of the screen (for debugging/testing)
    *
    * @param pRow the starting row
    * @param pCol the starting column
    * @param pNumber number of characters to get
    * @param pWin A pointer to an ncurses window structure to get contents
    *  from.  If this is nullptr, curscr will be used.
    *
    * @return the "string" value of the screen
    */
   std::string getString(int pRow, int pCol, int pNumber, WINDOW *pWin = nullptr);

   /**
    * @brief Look for a string on the screen (for debugging/testing)
    *
    * @param pSearch string to search for
    * @param pRow the row it was found on
    * @param pCol the column it was found on
    * @param pWin A pointer to an ncurses window structure to get contents
    *  from.  If this is nullptr, curscr will be used.
    *
    * @return bool true if we were able to find the string, false otherwise
    */
   bool findString(const std::string& pSearch, int& pRow, int& pCol, WINDOW *pWin = nullptr);

} // end of namespace cxBase

#endif
