#ifndef __CXMULTILINEINPUT_H__
#define __CXMULTILINEINPUT_H__

/*
 * cxMultiLineInput.h - This class represents a multi-line input with a label.
 *  Logically, this is one big input, and the value will be spread across all
 *  lines.  Wrapping is handled as the user types to the end of each line.
 *  This class is made up of single-line cxInput objects; however, in
 *  retrospect, this seems rather janky; it would be good to just eliminate
 *  cxInput, rename this class to cxInput, and re-do this class so that it is
 *  just a single window and can handle scrolling.
 *
 * Copyright (C) 2005-2007 Michael H. Kinney
 *
 * \author $Author: erico $
 * \version $Revision: 1.155 $
 * \date $Date: 2007/12/04 00:23:46 $
 *
 * Date     User    Description
 * 08/23/07 erico   Added a built-in onKeypress function to do per-keypress
 *                  validation for numeric input types, and to allow deriving
 *                  classes to override it to add additional functionality.
 */

#include "cxWindow.h"
#include "cxInput.h"
#include "cxInputTypes.h"
#include "cxFunction.h"
#include <signal.h> // For sigaction
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>

enum eMLIF
{
   eMLIF_COMMA_SEP = 0,
   eMLIF_COMMA_SEP_WITH_DESC
}; // enum for cxMultiLineInputFormat

// eInputErrors is an enumeration that specifies an errors state for the
//  input.  If validation fails, the input's mErrorState will be set to
//  one of these errors.  If there is no error, it will be set to eNO_ERROR.
enum eInputErrors
{
   eNO_ERROR,
   eOUT_OF_RANGE,
   eINVALID_VALUE,
   eBLANK,
   eCUSTOM_VALIDATION_FAILED
}; // enum for input errors

class cxForm; // Pre-declaration to avoid a circular reference

/** \class cxMultiLineInput
 *  \brief This class represents an input box that can have a height
 *  \brief greater than 1.
 *  This class allows you to get text input from a user.  The method showModal()
 *  makes the input appear on the screen and wait for input.  showModal() also
 *  returns a return code, depending on the user's action.  If the user pressed
 *  ESC, cxID_QUIT is returned to signify that the user quit out of the input
 *  and didn't want to do anything.  If the user presses ENTER, cxID_EXIT is
 *  returned to signify that the value in the input should be used. getLastKey()
 *  can be called to get the last key typed by the user (see cxKeyDefines and
 *  the ncurses list of key definitions (man getch)).<br>
 *  By default, pressing the enter key anywhere in the input will exit the
 *  input, and when you call getValue() to get the value entered into the
 *  input, the text will all be concatenated together.  You can change this
 *  behavior by calling setEnterAlwaysExits() with a parameter of false.  In
 *  that situation, the enter key will go onto the next line, rather than
 *  exiting the input.  Also, getValue() will append newline characters after
 *  each line so that the value will represent the text how it was entered.
 *  This is done so that the string can be formatted properly when used in
 *  other applications.<br>
 *  There is also a set of functions that may be set, that will be run at
 *  various times.  The order of the functions is as follows:<br>
 *  <ol>
 *  <li> The "on focus" function is executed immediately at the start of
 *      setFocus() (see setOnFocusFunction() in cxWindow).
 *  <li> When certain hotkeys (set up with setKeyFunction()) are pressed,
 *      functions associated with those keys are run.  If there are no hotkey
 *      functions set up, the "on key" function is run after each keypress
 *      (see setOnKeyFunction()).
 *  <li> After the user is finished entering text, the validator function
 *      is run (see setValidatorFunction()).  The purpose of this function
 *      is to do any validation of the input that requires special logic.
 *  <li> Just before setFocus() exits, the "on leave" function is
 *      executed (see setOnLeaveFunction() in cxWindow).
 *  </ol>
 *  Note that the functions used must have the following signatures:<br>
 *  <ul>
 *  <li>onFocus function: string func(void*, void*, void*, void*)
 *  <li>onKey function: string func(void*, void*, void*, void)
 *  <li>"field" (hotkey) functions: string func(void*, void*, void*, void*) or string func(void*, void*)
 *  <li>Validator function: string func(void*, void*, void*, void)
 *  <li>onLeave function: string func(void*, void*, void*, void)
 *  </ul>
 *  Also, you can force an input never to be editable with the
 *  function setCanBeEditable().  If you call this with false, then
 *  the input can never be set editable, even with a call to
 *  setInputOption() with a parameter of eINPUT_EDITABLE.<br>
 *  cxMultiLineInput iInput(nullptr, 0, 0, 10, "Text:");<br>
 *  iInput.setValue("Some text for the input to hold");<br>
 *  iInput.getValue();  // Returns "Some text for the input to hold"<br>
 *  iInput.setFocus(); // Can only display "Some " in the input, since
 *  the width is 10<br>
 *  Setting a value longer than the input can display can cause some
 *  behavior that you may not expect, however.  If you set a value
 *  too long for the input to display and then call setFocus(), the
 *  input will leave focus as soon as the user types something, because
 *  the input is full.  However, being able to store more than the input
 *  can display can be useful if you are using a cxInput for display
 *  purposes only, and you still need to be able to store the entire value.<br>
 *  By default, when the user presses any function keys that aren't assigned
 *  to any functions, cxInput will exit its input loop so that calling code
 *  can process the function keys.  This can be changed with a call to
 *  trapNonAssignedFKeys().  However, when a cxMultiLineInput is used in
 *  a cxForm, it is recommended that the default not be changed, because a
 *  cxForm needs to be able to process function keys for its own lists of
 *  function pointers.<br>
 *  setTimeout() lets you set the amount of idle time (in seconds) that is
 *  allowed before the input quits out of its input loop and leaves focus.<br>
 *  cxMultiLineInput can also do some input validation.  Through the use of
 *  setInputType(), setRangeLowInt(), setRangeHighInt(), setRangeLowDouble(),
 *  setRangeHighDouble(), addValidOptions(), setValidOptions(), addValidOption(),
 *  setForceUpper(), and setAllowBlank(), you can specify how to validate the
 *  input.  cxMultiLineInput has a built-in validation function that performs
 *  this validation (this can be changed with setValidatorFunction() if
 *  that is really desired).  Alternatively, there is a function called
 *  inputValidator() that can be overridden in deriving classes to perform
 *  input validation.  The built-in validator can be toggled on or off
 *  using useBuiltInValidator().<br>
 *  cxMultiLineInput can also contain an "extended" help string and display
 *  it with a keypress, which defaults to F1 (that can be changed with
 *  setExtendedHelpkey()).  The extended help text can be set using
 *  setExtendedHelp().
 *
 *
 * Visual represenation:
 *  +-Form Title-----------------------------+
 *  |                                        |
 *  | Label:__________ RightLabel            |  (Note: When you press F1, a new message
 *  |                                        |         box pops up with info, aka
 *  +Status Line-----------------------------+         extended help.)
 *
 * \author $Author: erico $
 * \version $Revision: 1.155 $
 * \date $Date: 2007/12/04 00:23:46 $
 */
class cxMultiLineInput : public cxWindow
{
   public:
      /**
       * Default constructor
       * @param pParentWindow Pointer to parent window; defaults to nullptr
       * @param pRow Y location of input window (upper-left corner)
       * @param pCol X location of input window (upper-left corner)
       * @param pHeight Height of input window; defaults to 1
       * @param pWidth Width of input window; defaults to 80
       * @param pLabel Label of multi-line input; blank by default
       * @param pBorderStyle The type of border to use - defaults to
       *  eBS_NOBORDER
       * @param pInputOption Specifies the editability of the input.  Defaults
       *  to eINPUT_EDITABLE.  The enNputOptions enumeration is in the file
       *  cxInputOptions.h.
       * @param pInputType Specifies the type of input (text, numeric floating
       *  point, or numeric whole).  This defaults to eINPUT_TYPE_TEXT.
       * @param pExtValue Pointer to external input value; defaults to nullptr
       * @param pRightLabelOffset The distance between the right edge of the
       *  input and the right label.  Defaults to 0.
       * @param pRightLabelHeight The height of the label to the right of the
       *  input (defaults to 1)
       * @param pRightLabelHeight The width of the label to the right of the
       *  input (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel Whether or not to show the right label.
       *  Defaults to false.
       */
      explicit cxMultiLineInput(cxWindow *pParentWindow = nullptr, int pRow = 0,
                                int pCol = 0, int pHeight = 1,
                                int pWidth = DEFAULT_WIDTH,
                                const std::string& pLabel = "",
                                eBorderStyle pBorderStyle = eBS_NOBORDER,
                                eInputOptions pInputOption = eINPUT_EDITABLE,
                                eInputTypes pInputType = eINPUT_TYPE_TEXT,
                                std::string *pExtValue = nullptr,
                                int pRightLabelOffset = 0,
                                int pRightLabelHeight = 1,
                                int pRightLabelWidth = 7,
                                bool pShowRightLabel = false);

      /**
       * Copy constructor
       * @param pThatInput Another cxMultiLineInput object to copy
       */
      cxMultiLineInput(const cxMultiLineInput& pThatInput);

      /**
       * Destructor
       */
      virtual ~cxMultiLineInput();

      /**
       * \brief Assignment operator
       *
       * @param pThatInput Another cxMultiLineInput to copy
       *
       * @return The current cxMultiLineInput
       */
      cxMultiLineInput& operator =(const cxMultiLineInput& pThatInput);

      /**
       * Shows the window, along with the user's input
       * @param pBringToTop Whether or not to bring this window to the top.  Defaults to false.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Always uses false.
       * @return Returns cxID_EXIT
       */
      virtual long show(bool pBringToTop = false, bool pShowSubwindows = false) override;

      /**
       * \brief Enters a loop for user input.  Returns cxID_EXIT or cxID_QUIT,
       * \brief depending on whether the user exited or quit the input.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring the window to the top (always uses
       *  true).
       * @param pShowSubwindows Whether or not to show sub-windows also.  Always uses
       *  false.
       *
       * @return The return code, based on the user's input (returns cxID_QUIT if
       *  the user pressed ESC to quit out, or cxID_EXIT otherwise).
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = false) override;

      /**
       * Hides the input (the input still exists; it just won't be displayed).
       * @param pHideSubwindows Whether or not to also hide the subwindows
       *  (always uses false)
       */
      void hide(bool pHideSubwindows = false);

      /**
       * Un-hides the window.
       * @param pUnhideSubwindows Whether or not to also unhide the subwindows
       *    (always uses false)
       */
      void unhide(bool pUnhideSubwindows = false);

      /**
       * \brief Erases the window
       * @param pEraseSubwindows Whether or not to erase the subwindows (defaults to
       *    true)
       */
      virtual void erase(bool pEraseSubwindows = true) override;

      /**
       * Returns the label of the input
       * @return The label of the input
       */
      virtual std::string getLabel() const;

      /**
       * Sets the label for the input
       * @param pLabel The new label
       */
      virtual void setLabel(const std::string& pLabel);

      /**
       * \brief Clears the input value
       *
       * @param pRefresh Whether or not to refresh the input
       *  (defaults to false)
       */
      virtual void clearValue(bool pRefresh = false);

      /**
       * \brief Alias for clearValue() (this is here to overload the clear()
       * \brief inherited from cxWindow).
       *
       * @param pRefresh Whether or not to refresh the input
       *  (defaults to false)
       */
      virtual void clear(bool pRefresh = false) override;

      /**
       * Returns the user's input
       * @return The user's input
       */
      virtual std::string getValue() const;

      /**
       * \brief Sets the text in the input.
       *
       * @param pValue String to set as input text
       * @param pRefresh Whether or not to refresh the input (defaults to false)
       *
       * @return Boolean: true on success, or false on failure
       */
      virtual bool setValue(std::string pValue, bool pRefresh = false);

      /**
       * Returns the pointer to the external variable storing user input
       * @return Pointer to external variable storing user input
       */
      std::string *getExtValue() const { return(mExtValue); }

      /**
       * Mutator for the "external" user value variable pointer
       * @param pExtVal The new pointer to the external variable
       * @param pRefresh Whether or not to refresh the input
       */
      void setExtValue(std::string *pExtVal, bool pRefresh = false);

      /**
       * \brief Sets the validator string for this input.
       *
       * @param pValidator The new validator string.  See README.txt for format.
       */
      virtual void setValidatorStr(const std::string& pValidator);

      /**
       * \brief Returns the validator string
       *
       * @return The validator string
       */
      virtual std::string getValidatorStr() const;

      /**
       * \brief Returns whether the text entered into the input is valid.
       *
       * @return Whether or not the text entered is valid
       */
      virtual bool textIsValid() const;

      /**
       * \brief Returns whether some text is valid according to the input's validator.
       *
       * @param pText The text to be validated
       * @return Whether or not the text is valid
       */
      virtual bool textIsValid(const std::string& pText) const;

      /**
       * Returns whether or not input is masked
       * @return Whether or not input is masked
       */
      bool getMasked();

      /**
       * Enable/disable input masking
       * @param pMasking Whether to mask user input
       */
      void toggleMasking(bool pMasking);

      /**
       * Accessor for the masking character
       * @return The masking character
       */
      char getMaskChar() const;

      /**
       * Mutator for the masking character
       * @param pMaskChar The new masking character to use
       */
      void setMaskChar(char pMaskChar);

      /**
       * Changes the window's position, based on a new upper-left corner
       * @param pNewRow The new topmost row of the form
       * @param pNewCol The new leftmost column of the form
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

      /**
       * \brief Changes input's width and height.  The upper-left coordinate
       * \brief stays the same.
       * @param pNewHeight The new height
       * @param pNewWidth The new width
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void resize(int pNewHeight, int pNewWidth, bool pRefresh = true) override;

      /**
       * \brief Sets a function to be called when a key is pressed.
       *
       * @param pKey The key to use for the function
       * @param pFunction The function to call. This can be an instance of
       * one of the derived cxFunction classes as well.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, const std::shared_ptr<cxFunction>& pFunction) override;

      /**
       * \brief Adds a function to call when the user presses some key.
       * \brief This version adds a pointer to a function with this signature:
       * \brief string func(void*, void*, void*, void*).
       * \brief Note that functions added with this method take precedence
       * \brief over functions added with the next version of this method.
       *
       * @param pKey A keypress to fire the function
       * @param pFunction Function to point to - must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseVal Whether or not the function's return value should be
       *  set as the value in the field
       * @param pExitAfterRun Whether or not this field should exit from the
       *  input loop once the function is done
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr4 pFunction,
                                  void *p1, void *p2, void *p3, void *p4,
                                  bool pUseVal, bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true) override;

      /**
       * \brief Just like the above function, but also lets you specify whether
       * \brief to run the validator function, if the input should leave focus
       * \brief after the function runs.
       *
       * @param pKey A keypress to fire the function
       * @param pFunction Function to point to - must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseVal Whether or not the function's return value should be
       *  set as the value in the field
       * @param pExitAfterRun Whether or not this field should exit from the
       *  input loop once the function is done
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       * @param pRunValidator Whether or not to run the validator function
       *  after the function runs and the input exits its loop (useful if
       *  pExitAfterRun is true).
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr4 pFunction,
                                  void *p1, void *p2, void *p3, void *p4,
                                  bool pUseVal, bool pExitAfterRun,
                                  bool pRunOnLeaveFunction, bool pRunValidator);

      /**
       * \brief Adds a function to call when the user presses some key.
       *
       * @param pKey A keypress to fire the function
       * @param pFunction Function to point to - must have this signature:
       *  string (*fnctn)(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseVal Whether or not the function's return value should be
       *  set as the value in the field
       * @param pExitAfterRun Whether or not this field should exit from the
       *  input loop once the function is done
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr2 pFunction,
                                  void *p1, void *p2, bool pUseVal,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true);

      /**
       * \brief Just like the above function, but also lets you specify whether
       * \brief to run the validator function, if the input should leave focus
       * \brief after the function runs.
       *
       * @param pKey A keypress to fire the function
       * @param pFunction Function to point to - must have this signature:
       *  string (*fnctn)(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseVal Whether or not the function's return value should be
       *  set as the value in the field
       * @param pExitAfterRun Whether or not this field should exit from the
       *  input loop once the function is done
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       * @param pRunValidator Whether or not to run the validator function
       *  after the function runs and the input exits its loop (useful if
       *  pExitAfterRun is true).
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr2 pFunction,
                                  void *p1, void *p2, bool pUseVal,
                                  bool pExitAfterRun, bool pRunOnLeaveFunction,
                                  bool pRunValidator);

      /**
       * \brief Adds a function to call when the user presses some key.
       *
       * @param pKey A keypress to fire the function
       * @param pFunction Function to point to - must have this signature:
       *  string fnctn()
       * @param pUseVal Whether or not the function's return value should be
       *  set as the value in the field
       * @param pExitAfterRun Whether or not this field should exit from the
       *  input loop once the function is done
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr0 pFunction,
                                  bool pUseVal,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true);

      /**
       * \brief Just like the above function, but also lets you specify whether
       * \brief to run the validator function, if the input should leave focus
       * \brief after the function runs.
       *
       * @param pKey A keypress to fire the function
       * @param pFunction Function to point to - must have this signature:
       *  string fnctn()
       * @param pUseVal Whether or not the function's return value should be
       *  set as the value in the field
       * @param pExitAfterRun Whether or not this field should exit from the
       *  input loop once the function is done
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       * @param pRunValidator Whether or not to run the validator function
       *  after the function runs and the input exits its loop (useful if
       *  pExitAfterRun is true).
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr0 pFunction,
                                  bool pUseVal,
                                  bool pExitAfterRun, bool pRunOnLeaveFunction,
                                  bool pRunValidator);

      /**
       * \brief Adds a key to the list of keys that, when pressed, the input
       * \brief won't run the validator function.
       *
       * @param pKey A key to add to the list of keys
       */
      virtual void addSkipValidatorKey(int pKey);

      /**
       * \brief Removes a key from the list of keys that skip the validator
       * \brief function.
       *
       * @param pKey The key to remove
       */
      virtual void removeSkipValidatorKey(int pKey);

      /**
       * \brief Returns whether a key is in the list of keys that cause the
       * \brief input not to run the validator when it is pressed to run a
       * \brief function.
       *
       * @param pKey The key to look for
       *
       * @return Whether or not the key is in the list of keys that cause the
       *  input not to run the validator function when it is pressed to run a
       *  function.
       */
      virtual bool hasSkipValidatorKey(int pKey) const;

      /**
       * \brief Removes a hotkey from the window so that it will not
       * \brief fire a function.
       *
       * @param pFunctionKey The hotkey to remove
       */
      virtual void clearKeyFunction(int pFunctionKey);

      /**
       * \brief Clears the list of external functions fired by hotkeys.
       */
      virtual void clearKeyFunctions();

      /**
       * \brief Sets the label color
       *
       * @param pColor A member of the e_cxColors enumeration from cxColors.h
       */
      void setLabelColor(e_cxColors pColor);

      /**
       * Gets the label color
       *
       * @return An value from the e_cxColors enumeration representing the
       *  label color.
       */
      e_cxColors getLabelColor() const;

      /**
       * \brief Sets the color of the value typed into the input
       *
       * @param pColor A member of the e_cxColors enumeration from cxColors.h
       */
      void setValueColor(e_cxColors pColor);

      /**
       * Gets the value color
       *
       * @return An value from the e_cxColors enumeration representing the
       *  value color
       */
      e_cxColors getValueColor() const;

      /**
       * \brief Sets the color of one of the window items.
       *
       * @param pItem The item to set the color of (see the e_WidgetItems
       *  enumeration in cxWidgetItems.h).
       * @param pColor The color to set the item to (see the e_cxColors
       *  enumeration in cxColors.h)
       */
      virtual void setColor(e_WidgetItems pItem, e_cxColors pColor) override;

      /**
       * \brief Returns the input option for the input (i.e., eINPUT_EDITABLE,
       * \brief eINPUT_READONLY)
       *
       * @return The input kind
       */
      virtual int getInputOption() const;

      /**
       * \brief Sets the input option for the input (i.e., eINPUT_EDITABLE,
       * \brief eINPUT_READONLY) - See the eInputOptions enumeration in
       * \brief cxInput.h
       *
       * @param pInputKind The new input kind
       */
      virtual void setInputOption(eInputOptions pInputKind);

      /**
       * \brief Enables/disables the display of the cursor.
       *
       * @param pShowCursor Whether or not to show the cursor (true/false).
       */
      void toggleCursor(bool pShowCursor);

      /**
       * \brief Clears the value of a cxMultiLineInput
       *
       * @param theInput A pointer to a cxMultiLineInput object
       * @param unused Not used
       *
       * @return A blank string
       */
      static std::string clearInput(void* theInput, void* unused);

      /**
       * Sets a validator function to be run before focus is lost.
       * If the function returns a blank string, it is assumed that the
       * text is valid; if the function returns a non-blank string, it is
       * assumed that the text was incorrect.
       * @param pFunction Function pointer
       *
       * @return true if the function was set, or false if not
       */
      virtual bool setValidatorFunction(const std::shared_ptr<cxFunction>& pFunction);

      /**
       * Sets a validator function to be run before focus is lost.
       * If the function returns a blank string, it is assumed that the
       * text is valid; if the function returns a non-blank string, it is
       * assumed that the text was incorrect.
       * @param pFunction Function to point to--must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       *
       * @return true if the function was set, or false if not
       */
      bool setValidatorFunction(funcPtr4 pFunction, void *p1, void *p2,
                                void *p3, void *p4);

      /**
       * Sets a validator function to be run before focus is lost.
       * If the function returns a blank string, it is assumed that the
       * text is valid; if the function returns a non-blank string, it is
       * assumed that the text was incorrect.
       * @param pFunction Function to point to--must have signature string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       *
       * @return true if the function was set, or false if not
       */
      bool setValidatorFunction(funcPtr2 pFunction, void *p1, void *p2);

      /**
       * Sets a validator function to be run before focus is lost.
       * If the function returns a blank string, it is assumed that the
       * text is valid; if the function returns a non-blank string, it is
       * assumed that the text was incorrect.
       * @param pFunction Function to point to--must have signature string func()
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       *
       * @return true if the function was set, or false if not
       */
      bool setValidatorFunction(funcPtr0 pFunction);

      /**
       * \brief Un-sets the validator function.
       */
      void clearValidatorFunction();

      /**
       * \brief Returns whether the value in the input takes up the maximum possible space for the input.
       *
       * @return Whether or not the value in the input takes up the maximum possible space for the input.
       */
      bool isFull() const;

      /**
       * \brief Sets a function to be run whenever a key is pressed.
       * @param pFunction Function to point to--must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setOnKeyFunction(const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets a function to be run whenever a key is pressed.
       * @param pFunction Function to point to--must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setOnKeyFunction(funcPtr4 pFunction, void *p1, void *p2,
                                    void *p3, void *p4);

      /**
       * \brief Sets a function to be run whenever a key is pressed.
       * @param pFunction Function to point to--must have signature string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setOnKeyFunction(funcPtr2 pFunction, void *p1, void *p2);

      /**
       * \brief Sets a function to be run whenever a key is pressed.
       * @param pFunction Function to point to--must have signature string func()
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setOnKeyFunction(funcPtr0 pFunction);

      /**
       * \brief Sets whether the onKey function should be fired or not.
       *
       * @param pRunOnKeyFunction Whether or not the onKey function should be
       *  fired
       */
      virtual void toggleOnKeyFunction(bool pRunOnKeyFunction);

      /**
       * \brief Un-sets the onKey function.
       */
      virtual void clearOnKeyFunction();

      /**
       * \brief Returns whether or not the onKey function will be run.
       *
       * @return Whether or not the onKey function will be run
       */
      bool onKeyFunctionEnabled() const;

      /**
       * \brief Sets whether the enter key always causes the input
       * \brief to exit, rather than going onto the next line in
       * \brief the input.
       *
       * @param pEnterAlwaysExits If true, enter will always cause the input
       *     to exit.  If false, enter will cause the cursor to go to
       *     the next line in the input.
       */
      void setEnterAlwaysExits(bool pEnterAlwaysExits);

      /**
       * \brief Returns whether the enter key always causes the input
       * \brief to exit.
       *
       * @return Whether or not the enter key always causes the input
       *   to exit.
       */
      bool enterAlwaysExits() const;

      /**
       * \brief Returns the maximum length of the value that may be set.
       *
       * @return The maximum length of the value that may be set.
       */
      virtual int maxValueLen() const;

      /**
       * \brief Returns whether or not the input currently has focus.
       *
       * @return Whether or not the input currently has focus
       */
      virtual bool hasFocus() const override;

      /**
       * \brief Sets whether the input should go read-only when setFocus() exits.
       *
       * @param pReadOnlyOnLeave If true, the input will change to read-only when
       *        setFocus() exits.
       */
      virtual void setReadOnlyOnLeave(bool pReadOnlyOnLeave);

      /**
       * \brief Returns whether the input can be editable.
       *
       * @return Whether or not the input can be editable
       */
      bool canBeEditable() const;

      /**
       * \brief Sets whether the input can be set editable.  If this is called
       * \brief with false, then the input will be set read-only, and any calls
       * \brief to setInputOption(eINPUT_EDITABLE), which would normally set it
       * \brief editable, will not set it editable.
       *
       * @param pCanBeEditable If true, the input can be set editable; if false,
       *  the input will always be read-only.
       */
      void setCanBeEditable(bool pCanBeEditable);

      /**
       * \brief Sets whether the input should exit automatically when it's full.
       *
       * @param pExitOnFull Whether or not to exit automatically when full
       */
      void setExitOnFull(bool pExitOnFull);

      /**
       * \brief Returns whether the input will exit automatically when full
       *
       * @return Whether or not the input will exit automatically when full
       */
      bool getExitOnFull() const;

      /**
       * \brief Sets the border style
       *
       * @param pBorderStyle The type of border to use
       */
      virtual void setBorderStyle(eBorderStyle pBorderStyle) override;

      /**
       * \brief Returns whether the input is above another window.
       *
       * @param pThatWindow The other window
       *
       * @return True if the window is above pThatWindow; false if the window is below pThatWindow.
       */
      virtual bool isAbove(const cxWindow& pThatWindow) const override;

      /**
       * \brief Returns whether the input is below another window.
       *
       * @param pThatWindow The other window
       *
       * @return True if the window is below pThatWindow; false if the window is above pThatWindow.
       */
      virtual bool isBelow(const cxWindow& pThatWindow) const override;

      /**
       * \brief Brings the window to the top
       * @param pRefresh Whether to refresh the screen (defaults to true)
       */
      virtual void bringToTop(bool pRefresh = true) override;

      /**
       * \brief Refreshes just the value portion of the input.
       *
       * @param pRefresh Whether or not to refresh the window
       *  after re-drawing the value (defaults to true).
       */
      virtual void refreshValue(bool pRefresh = true);

      /**
       * \brief Sets whether non-assigned function keys should
       * \brief be trapped within the input loop and not cause
       * \brief the input to stop its input loop.
       *
       * @param pTrapNonAssignedFKeys If true, pressing function
       *  keys that are not assigned (when modal) will do nothing.
       *  If false, pressing non-assigned function keys while
       *  focus is set will cause the input to exit the input loop.
       */
      void trapNonAssignedFKeys(bool pTrapNonAssignedFKeys);

      /**
       * \brief Runs the onFocus function, if it's set.  If the
       * \brief onFocus function's mUseReturnVal is true, the return
       * \brief value of the onFocus function will be set in the
       * \brief input.
       *
       * @param pFunctionRetval If not nullptr, the string that this points to
       *  will contain the return value of the onFocus function.  Defaults to
       *  nullptr.
       *
       * @return Whether the input loop should exit (returns the value of
       *  the onFocus function's mExitAfterRun, or false if the onFocus
       *  function isn't set).
       */
      virtual bool runOnFocusFunction(std::string *pFunctionRetval = nullptr) override;

      /**
       * \brief Runs the function that was set with setKeyFunction for this key
       * @param the key to run the function for
       */
      void runFieldFunction(int pKey);

      /**
       * Returns the maximum length of input that can be accepted
       * @return Maximum length of input that can be accepted by this input line
       */
      int getInputLen() const;

      /**
       * \brief Adds a key that will cause the input to quit and return
       * \brief cxID_QUIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave function
       *  when the input exits.  Defaults to true.
       * @param pOverride If true, then the key will be added as an exit key
       *  regardless if the key exists as a function key.  This defaults to
       *  false.
       *
       * @return Whether or not the key was added
       */
      virtual bool addQuitKey(int pKey, bool pRunOnLeaveFunction = true,
                              bool pOverride = false);

      /**
       * \brief Removes a quit key (but doesn't work for ESC)
       *
       * @param pKey The key to remove from the list of quit keys
       */
      virtual void removeQuitKey(int pKey);

      /**
       * \brief Adds a key that will cause the input to quit and return
       * \brief cxID_EXIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave function
       *  when the input exits.  Defaults to true.
       * @param pOverride If true, then the key will be added as an exit key
       *  regardless if the key exists as a function key.  This defaults to
       *  false.
       *
       * @return Whether or not the key was added
       */
      virtual bool addExitKey(int pKey, bool pRunOnLeaveFunction = true,
                              bool pOverride = false);

      /**
       * \brief Removes an exit key
       *
       * @param pKey The key to remove from the list of exit keys
       */
      virtual void removeExitKey(int pKey);

      /**
       * \brief Enables or disables the input loop.
       * \brief If the input loop is disabled, you can still
       * \brief call setFocus() and showModal() on the
       * \brief input - the onFocus and onLeave functions
       * \brief will still run, but the input won't
       * \brief wait for user input.
       *
       * @param pDoInputLoop Whether to enable or disable
       *  the input loop (true/false)
       */
      virtual void enableInputLoop(bool pDoInputLoop);

      /**
       * \brief Returns whether the input loop is enabled
       *
       * @return
       */
      virtual bool inputLoopEnabled() const;

      /**
       * \brief Enables or disables editing of the input.  If disabled, the
       * \brief user won't be able to type in the input.  This calls
       * \brief setInputOption() with either eINPUT_EDITABLE or
       * \brief eINPUT_READONLY, depending on the value of pEditable.
       *
       * @param pEditable Whether or not to enable editing (true/false)
       */
      virtual void setEditable(bool pEditable);

      /**
       * \brief Returns whether or not the input is editable.
       * An input is editable if it passes the 3
       * following criteria:<br>
       * <ul>
       * <li>Its kind is not eINPUT_READONLY
       * <li>It is enabled (see cxWindow::setEnabled() and cxWindow::isEnabled())
       * <li>Its input loop is enabled
       * </ul>
       *
       * @return Whether or not the input is editable
       */
      virtual bool isEditable() const;

      /**
       * \brief Returns whether or not auto-wrapping of text
       * \brief moves the cursor to the beginning of the
       * \brief next line.
       *
       * @return Whether or not auto-wrapping of text moves
       *  the cursor to the beginning of the next line
       */
      bool getAutoWrapAtBeginning() const;

      /**
       * \brief Sets whether auto-wrapping of text
       * \brief should move the cursor to the beginning
       * \brief of the next line.
       *
       * @param pWrapAtBeginning Whether or not auto-wrapping
       *  of text should move the cursor to the beginning of
       *  the next line.  If true, then as the user is typing
       *  text, when one line fills up, the cursor will move
       *  to the beginning of the next line.  If false, the
       *  cursor will move to the end of the text in the
       *  next line.
       */
      void setAutoWrapAtBeginning(bool pWrapAtBeginning);

      /**
       * \brief Returns whether the input will skip all
       * \brief input processing in showModal() or
       * \brief setFocus().
       *
       * @return Whether or not the input will skip all
       *  input processing in showModal() or setFocus()
       */
      bool getSkipIfReadOnly() const;

      /**
       * \brief Sets whether the input should skip
       * \brief all input processing in showModal()
       * \brief or setFocus().  By default, this is
       * \brief true when a cxMultiLineInput is created.
       *
       * @param pSkipIfReadOnly Whether or not the
       *  input should skip all input processing in
       *  showModal() or setFocus().
       */
      void setSkipIfReadOnly(bool pSkipIfReadOnly);

      /**
       * \brief Sets whether the window should disable the cursor
       * \brief when it's shown (this is the default behavior).
       *
       * @param pDisableCursorOnShow Whether or not to disable
       *  the cursor when shown
       */
      virtual void setDisableCursorOnShow(bool pDisableCursorOnShow) override;

      /**
       * \brief Returns the parent cxForm pointer.
       *
       * @return The parent cxForm pointer
       */
      cxForm* getParentForm() const;

      /**
       * \brief Sets whether or not to run the validator function
       * \brief when the user wants to move in reverse (i.e., by
       * \brief pressing shift-tab or the up arrow to exit the
       * \brief input).  The default behavior is true when a
       * \brief cxMultiLineInput is created.
       *
       * @param pValidateOnReverse Whether or not to run the validator
       *  function when the user wants to move in reverse
       */
      virtual void setValidateOnReverse(bool pValidateOnReverse);

      /**
       * \brief Returns whether or not the validator function will
       * \brief run when the user wants to move in reverse.
       *
       * @return Whether or not the validator funtion will run when
       *  the user wants to move in reverse
       */
      virtual bool getValidateOnReverse() const;

      /**
       * \brief Sets the last keypress
       *
       * @param pLastKey A key (represented by an int)
       */
      virtual void setLastKey(int pLastKey) override;

      /**
       * \brief Returns whether or not a call to showModal() will wait for a
       * \brief keypress from the user.  This will be true if the input
       * \brief is enabled and the input type is eINPUT_EDITABLE; it will be
       * \brief false otherwise.
       *
       * @return Whether or not a call to showModal() will wait for a keypress
       *  from the user.
       */
      virtual bool modalGetsKeypress() const override;

      /**
       * \brief Enables or disables the input.
       *
       * @param pEnabled Whether the input should be enabled or not (true/false)
       */
      virtual void setEnabled(bool pEnabled) override;

      /**
       * \brief After this is called, the code returned by showModal will
       * \brief be cxID_QUIT.  Also, if the input is on a cxForm, the form
       * \brief will go onto the next or previous input, depending on the
       * \brief value of pMoveForward (via a call to setCurrentInput()).
       *
       * @param pMoveForward If this is true, and the input is on a cxForm,
       *  the form will be told to go onto its next input; otherwise, the
       *  form will be told to go onto its previous input.
       */
      virtual void quitNow(bool pMoveForward);

      /**
       * \brief Tells the input to exit the input loop now, with a return value
       * \brief of cxID_QUIT.
       */
      virtual void quitNow() override;

      /**
       * \brief After this is called, the code returned by showModal will
       * \brief be cxID_EXIT.  Also, if the input is on a cxForm, the form
       * \brief will go onto the next or previous input, depending on the
       * \brief value of pMoveForward (via a call to setCurrentInput()).
       *
       * @param pMoveForward If this is true, and the input is on a cxForm,
       *  the form will be told to go onto its next input; otherwise, the
       *  form will be told to go onto its previous input.
       */
      virtual void exitNow(bool pMoveForward);

      /**
       * \brief Tells the input to exit the input loop now, with a return value
       * \brief of cxID_EXIT.
       */
      virtual void exitNow() override;

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
       * \brief Returns a pointer to the parent window.  If the cxMultiLineInput
       * \brief has a parent cxForm, a pointer to that will be
       * \brief returned; otherwise, a pointer to the parent cxWindow
       * \brief will be returned.
       *
       * @return A pointer to the parent window
       */
      virtual cxWindow* getParent() const override;

      /**
       * \brief Returns whether the input ran a key function that was set up
       * \brief to have the input exit when it was done.
       *
       * @return
       */
      virtual bool ranFunctionAndShouldExit() const;

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
       * \brief Returns whether the cursor will be left-aligned when the
       * \brief input is shown modally.
       *
       * @return Whether or not the cursor will be left-aligned when the
       *  input is shown modally.
       */
      virtual bool getCursorLeftAlign() const;

      /**
       * \brief Sets whether the cursor should be left-aligned when the
       * \brief input is shown modally.
       *
       * @param pCursorLeftAlign Whether or not the cursor should be
       *  left-aligned when the input is shown modally.
       */
      virtual void setCursorLeftAlign(bool pCursorLeftAlign);

      /**
       * \brief Returns whether the input will exit the input loop when the
       * \brief user presses backspace in the first input position.
       *
       * @return Whether or not the input will exit the input loop when the
       *  user presses backspace in the first input position.
       */
      bool getExitOnBackspaceAtFront() const;

      /**
       * \brief Sets whether the input should exit the input loop when the
       * \brief user presses backspace in the first input position.
       *
       * @param pExitOnBackspaceAtFront Whether or not the input should
       *  exit the input loop when the user presses backspace in the first
       *  input position.
       */
      void setExitOnBackspaceAtFront(bool pExitOnBackspaceAtFront);

      /**
       * \brief Runs the input's validator function and returns its return
       * \brief value.  If the validator function is not set, this returns
       * \brief a blank string.
       *
       * @return The return value of the validator function, or a blank string
       *  if the validator function is not set.
       */
      virtual std::string runValidatorFunction() const;

      /**
       * \brief Returns whether a key is in the input's list of what it
       * \brief considers to be "navigational" keys.  Normally, when the
       * \brief user presses a key, a cxMultiLineInput will check its parent
       * \brief window (if it's on a cxForm or cxPanel) to see if it has a
       * \brief function associated with that key, and if so, the input
       * \brief will exit its input loop without running its validator.
       * \brief However, navigational keys are an exception, because usually
       * \brief you would want to run the validator function to run before
       * \brief the user leaves the input.  The default navigational
       * \brief keys are pageUp, pageDown, arrow keys, tab, and shift-tab.
       *
       * @param pKey The key to test
       *
       * @return Whether or not the key is in the input's list of navigational
       *  keys
       */
      virtual bool hasNavKey(int pKey) const;

      /**
       * \brief Add a key to be considered a "navigational" key.  The key will
       * \brief be added to the input's set of navigational keys (see the
       * \brief description for hasNavKey()).
       *
       * @param pKey The key to be added
       */
      virtual void addNavKey(int pKey);

      /**
       * \brief Removes a key from the input's list of "navigational" keys (see
       * \brief the description for hasNavKey()).
       *
       * @param pKey The key to be removed
       */
      virtual void removeNavKey(int pKey);

      /**
       * \brief Removes all keys that the input considers "navigational" keys
       */
      virtual void removeNavKeys();

      /**
       * \brief Returns the input's list of what it considers "navigational"
       * \brief keys.
       *
       * @param pNavKeys This will contain the navigational keys for the input.
       */
      virtual void getNavKeys(std::set<int>& pNavKeys) const;

      /**
       * \brief Sets the amount of time (in seconds) that the input should
       * \brief wait when idle before exiting the input loop (0 = no timeout).
       *
       * @param pTimeout The amount of time (in seconds) that the input should
       *  wait when idle before exiting the input loop (0 = no timeout).
       */
      virtual void setTimeout(int pTimeout);

      /**
       * \brief Returns the amount of time that the input will wait before
       * \brief exiting the input loop (0 = no timeout).
       *
       * @return The amount of time that the input will wait before exiting
       *  the input loop (0 = no timeout).
       */
      int getTimeout() const;

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief Returns the input type.  This is a member of the cxInputTypes
       * \brief enumeration (defines in cxInputTypes.h) and can be
       * \brief eINPUT_TYPE_TEXT, eINPUT_TYPE_NUMERIC_FLOATING_PT, or
       * \brief eINPUT_TYPE_NUMERIC_WHOLE.
       *
       * @return The input type set in the iput
       */
      eInputTypes getInputType() const;

      /**
       * \brief Sets the input type.  This is a member of the cxInputTypes
       * \brief enumeration (defined in cxInputTypes.h) and can be
       * \brief eINPUT_TYPE_TEXT, eINPUT_TYPE_NUMERIC_FLOATING_PT, or
       * \brief eINPUT_TYPE_NUMERIC_WHOLE.  Note that if the type is set to
       * \brief a numeric type, then one of the validators from cxValidators.h
       * \brief will be set up to validate the input for every keypress.
       *
       * @param pInputType The input type
       */
      virtual void setInputType(eInputTypes pInputType);

      /**
       * \brief Returns the low end of the floating-point numeric range for
       * \brief the input.
       *
       * @return The low end of the floating-point numeric range for the input
       */
      long double getRangeLowDouble() const;

      /**
       * \brief Sets the low end of the floating-point numeric range for the
       * \brief input.
       *
       * @param pRangeLowDouble A value for the low end of the floating-point
       *  range
       */
      void setRangeLowDouble(const long double &pRangeLowDouble);

      /**
       * \brief Returns the high end of the floating-point numeric range for
       * \brief the input.
       *
       * @return The high end of the floating-point numeric range for the input
       */
      long double getRangeHighDouble() const;

      /**
       * \brief Sets the high end of the floating-point numeric range for the
       * \brief input.
       *
       * @param pRangeLowDouble A value for the high end of the floating-point
       *  range
       */
      void setRangeHighDouble(const long double &pRangeHighDouble);

      /**
       * \brief Returns the low end of the whole numeric range for the input.
       *
       * @return The low end of the whole numeric range for the input
       */
      long int getRangeLowInt() const;

      /**
       * \brief Sets the low end of the whole numeric range for the input.
       *
       * @param pRangeLowInt A value for the low whole numeric range
       */
      void setRangeLowInt(const long int &pRangeLowInt);

      /**
       * \brief Returns the high end of the whole numeric range for the input.
       *
       * @return The high end of the whole numeric range for the input
       */
      long int getRangeHighInt() const;

      /**
       * \brief Sets the high end of the whole numeric range for the input.
       *
       * @param pRangeLowInt A value for the high whole numeric range
       */
      void setRangeHighInt(const long int &pRangeHighInt);

      /**
       * \brief Returns the floating-point numeric range for in the input.
       *
       * @param pLow (OUT) This will be set to the low end of the range
       * @param pHigh (OUT) This will be set to the high end of the range
       */
      void getRangeDouble(long double &pLow, long double &pHigh) const;

      /**
       * \brief Sets the floating-point numeric range for the input.
       *
       * @param pLow The value for the low end
       * @param pHigh The value for the high end
       */
      void setRangeDouble(const long double &pLow, const long double &pHigh);

      /**
       * \brief Returns the whole number range for the input.
       *
       * @param pLow (OUT) This will be set to the low end of the range
       * @param pHigh (OUT) This will be set to the high end of the range
       */
      void getRangeInt(long int &pLow, long int &pHigh);

      /**
       * \brief Sets the whole number range for the input.
       *
       * @param pLow The value for the low end
       * @param pHigh The value for the high end
       */
      void setRangeInt(const long int &pLow, const long int &pHigh);

      /**
       * \brief onLeave validator function - This runs just before the user
       * \brief leaves the input to validate the value.  If the input is a
       * \brief numeric input, this will make sure the value in the input is
       * \brief within the set range.  For a text input, this will make sure
       * \brief the value is valid according to mValidOptionStrings.  This
       * \brief function can be overridden in derived classes to add additional
       * \brief validation.
       *
       * @return Blank string if the value is valid, or a warning if not
       */
      virtual std::string inputValidator();

      /**
       * \brief This runs each time the user presses a key.  If built-in
       * \brief validation is used, this function will be automatically set up
       * \brief to fire.  If the input is a whole numeric input, this runs
       * \brief cxValidators::intOnKeyValidator() to only allow whole numbers.
       * \brief If the input is a floating-point numeric input, this runs
       * \brief cxValidators::floatingPtOnKeyValidator() to only allow
       * \brief floating-point numbers.  This function can be overridden in
       * \brief derived classes to add additional per-key validation or other
       * \brief functionality.
       *
       * @return Blank string on success, or a warning on failure (i.e., if the
       *  input is invalid).
       */
      virtual std::string onKeypress();

      /**
       * \brief Sets up the input's built-in validator function for the input.
       * \brief After this is called, inputValidator() will be called just
       * \brief before the user leaves the input to validate the input.
       * \brief inputValidator() can be overridden in derived classes to add
       * \brief additional validation.
       * \brief The built-in validator function is set up by default when a
       * \brief cxMultiLineInput is created.
       */
      virtual void useBuiltInValidator();

      /**
       * \brief Returns whether or not the built-in validator function is set
       * \brief up for the input.
       *
       * @return Whether or not the built-in validator is set up for the input
       */
      virtual bool usingBuiltInValidator() const;

      /**
       * \brief Sets up the input's built-in onKey function for the input.
       * \brief After this is called, onKeypress() will be called whenever
       * \brief the user presses a key.  onKeypress() provides per-keypress
       * \brief validation, depending on the type of input, and it can be
       * \brief overridden in derived classes to add additional per-key
       * \brief validation or other onKeypress functionality.
       */
      virtual void useBuiltInOnKeyFunction();

      /**
       * \brief Returns whether or not the built-in onKeypress function is
       * \brief set up for the input.
       *
       * @return Whether or not the built-in onKeypress function is set up
       *  for the input
       */
      virtual bool usingBuiltInOnKeyFunction() const;

      /**
       * \brief Sets whether or not a message box should be shown in the
       * \brief built-in validator function if the input is invalid.
       *
       * @param pEnable Whether or not to enable showing a message box for
       *  errors in the input validator
       */
      void validatorFuncMessageBox(bool pEnable);

      /**
       * \brief Returns whether or not the message box for errors is enabled
       * \brief in the input validator.
       *
       * @return Whether or not the message box for errors is enabled in the
       *  input validator
       */
      bool validatorFuncMessageBox() const;

      /**
       * \brief Adds single-character valid input strings to the input.
       * \brief Each character in the string will be added as a separate valid
       * \brief string that can be typed into the input.  This type of
       * \brief validation is used when the input type is set to
       * \brief eINPUT_TYPE_TEXT.
       *
       * @param pValidOptions A string containing characters that are valid
       *  inputs
       * @param pValidate Whether or not to validate the text currently in the
       *  input against the valid option strings.  Defaults to true.
       */
      virtual void addValidOptions(const std::string& pValidOptions,
                                   bool pValidate = true);

      /**
       * \brief Sets single-character valid input strings to the input.  This
       * \brief is the same as addValidOptions(), except that this clears the
       * \brief valid input collection first.
       *
       * @param pValidOptions A string containing characters that are valid
       *  inputs
       * @param pValidate Whether or not to validate the text currently in the
       *  input against the valid option strings.  Defaults to true.
       */
      virtual void setValidOptions(const std::string& pValidOptions,
                                   bool pValidate = true);

      /**
       * \brief Sets all the strings that may be typed into the input.
       *
       * @param pValidOptions A map where each key is a valid string that may
       *  be typed into the input, and the value is help text that goes along
       *  with the valid string.
       * @param pValidate Whether or not to validate the text currently in the
       *  input against the valid option strings.  Defaults to true.
       */
      virtual void setValidOptions(const std::map<std::string, std::string>& pValidOptions,
                                   bool pValidate = true);

      /**
       * \brief Adds a string to the set of valid strings that can be typed
       * \brief into the input.
       *
       * @param pValidOption The string to add to the set of valid strings
       * @param pRightLabelText The text to use for the label to the right of
       *  the input.  Defaults to a blank string.
       * @param pValidate Whether or not to validate the text currently in the
       *  input against the valid option strings.  Defaults to true.
       */
      virtual void addValidOption(const std::string& pValidOption,
                                  const std::string& pRightLabelText = "",
                                  bool pValidate = true);

      /**
       * \brief Returns the collection of valid input strings that can be typed
       * \brief into the input.  It is a map, where each key is the valid text,
       * \brief and the value is the help text.
       *
       * @param pValidOptions (OUT) This will contain the map of valid strings
       *  for the input & help text.
       */
      virtual void getValidOptions(std::map<std::string, std::string>& pValidOptions) const;

      /**
       * \brief Get the valid input strings formatted
       * @param pMLIF (format enum)
       *     eMLIF_COMMA_SEP=Comma separated list of inputs,
       *     eMLIF_COMMA_SEP_WITH_DESC=Comma Separated list of inputs with the description
       * @return formatted string of the valid inputs
       */
      std::string getValidOptionStrings(const eMLIF& pMLIF=eMLIF_COMMA_SEP) const;

      /**
       * \brief Returns a set of all the valid input strings currently set.
       *
       * @param pValidOptionStrings (OUT) This will hold the valid input strings
       *  currently set in the input.
       */
      void getValidOptionStrings(std::set<std::string>& pValidOptionStrings) const;

      /**
       * \brief Returns whether or not a string exists in the input's set of
       * \brief valid input strings.
       *
       * @param pStr The string to look for
       *
       * @return true if the string exists in the set of valid input strings,
       *  or false if not.
       */
      virtual bool hasValidOptionString(const std::string& pStr) const;

      /**
       * \brief Returns the help text for one of the valid input strings.  If
       * \brief the given input string is not in the collection of valid input
       * \brief strings, then this will return a blank string.
       *
       * @param pInput A valid input string to look for
       *
       * @return The help text for the valid input string, or blank if the
       *  given string is not found in the collection.
       */
      virtual std::string getValidOptionHelpText(const std::string& pInput) const;

      /**
       * \brief Clears the set of valid strings that can be typed into the
       * \brief input.
       */
      virtual void clearValidOptions();

      /**
       * \brief For a text input, this will try to auto-fill the input based
       * \brief on the value currently set in the input and the valid input
       * \brief strings set in the input.  If a match is found, the input will
       * \brief be filled with that value.  If more than one match is found,
       * \brief the input will be filled with the longest string that matches
       * \brief all of them.
       * \brief This returns whether or not the text in the input completely
       * \brief matched any of the valid input strings.  If there are none,
       * \brief this will return true.
       *
       * @param pRefresh Whether or not to refresh the input (defaults to false)
       *
       * @return true if the value in the input exactly matched any of the

       */
      virtual bool autoFillFromValidOptions(bool pRefresh = false);

      /**
       * \brief Toggles the option to force text to be upper-case.
       *
       * @param pForceUpper true if text should be upper-case, or false if
       *  not.
       */
      virtual void setForceUpper(bool pForceUpper);

      /**
       * \brief Returns the option for forcing text to upper-case.
       *
       * @return Whether or not upper-casing text is enabled
       */
      virtual bool getForceUpper() const;

      /**
       * \brief Sets whether or not to allow a blank value.
       *
       * @param pAllowBlank Whether or not to allow a blank value (true/false)
       */
      virtual void setAllowBlank(bool pAllowBlank);

      /**
       * \brief Returns the option for allowing a blank value.
       *
       * @return The option for allowing a blank value (true/false)
       */
      virtual bool getAllowBlank() const;

      /**
       * \brief Sets the "extended" help text for the input (this can be as
       * \brief long as desired).
       *
       * @param pExtendedHelp New extended help
       */
      virtual void setExtendedHelp(const std::string& pExtendedHelp);

      /**
       * \brief Returns the extended help set in the input.
       *
       * @return The extended help of the input
       */
      virtual std::string getExtendedHelp() const;

      /**
       * \brief Sets the color to use for the extended help message.
       *
       * @param pColor A value of the e_cxColors enumeration (see cxColors.h)
       */
      virtual void setExtendedHelpColor(e_cxColors pColor);

      /**
       * \brief Returns the color used for the extended help message.
       *
       * @return The color used for the extended help message - This is a value
       *  of the e_cxColors enumeration (see cxColors.h)
       */
      virtual e_cxColors getExtendedHelpColor() const;

      /**
       * \brief Sets the attribute to use for the extended help message.
       *
       * @param pAttr An ncurses attribute (see the man page for wattron for
       *  a list of attributes)
       */
      virtual void setExtendedHelpAttribute(attr_t pAttr);

      /**
       * \brief Returns the attribute used for the extended help message
       *
       * @return The attribute used for the extended help message
       */
      virtual attr_t getExtendedHelpAttribute() const;

      /**
       * \brief Enables or disables automatic generation of the extended help
       * \brief text based on the collection of valid input strings.
       *
       * @param pAutoGenerateExtendedHelp Whether or not to automatically
       *  generate the extended help text
       */
      virtual void autoGenerateExtendedHelp(bool pAutoGenerateExtendedHelp);

      /**
       * \brief Returns whether or not the extended help text will be
       * \brief automatically generated from the collection of valid input
       * \brief strings.
       *
       * @return Whether or not the extended help text will be automatically
       *  generated from the collection of valid input strings (true/false)
       */
      virtual bool autoGenerateExtendeHelp() const;

      /**
       * \brief Sets a single key to be used to display the extended help.
       *
       * @param pKey A key to be used to display the extended help
       */
      virtual void setExtendedHelpKey(int pKey);

      /**
       * \brief Adds an additional key to be used to display the extended help.
       *
       * @param pKey A key to be used to display the extended help
       */
      virtual void addExtendedHelpKey(int pKey);

      /**
       * \brief Sets the keys to be used to display the extended help.
       *
       * @param pKeys A set of keys to use to display the extended help
       */
      virtual void setExtendedHelpKeys(const std::set<int>& pKeys);

      /**
       * \brief Returns the keys currently set up to display extended help.
       *
       * @return The keys that are currently set up to dispplay extended help
       */
      virtual std::set<int> getExtendedHelpKeys() const;

      /**
       * \brief Returns a comma-separated list of strings representing the
       * \brief extended help keys for the input.
       *
       * @return A comma-separated list of strings representing the extended
       *  help keys for the input.
       */
      virtual std::string getExtendedHelpKeyStrings() const;

      /**
       * \brief Removes all extended help keys.
       */
      virtual void clearExtendedHelpKeys();

      /**
       * \brief Toggles whether or not to use the extended help keys
       *
       * @param pUseExtendedHelpKey true if the extended help keys should be
       *  used, or false if not.
       */
      virtual void setUseExtendedHelpKeys(bool pUseExtendedHelpKey);

      /**
       * \brief Returns whether the extended help keys are set to be used
       *
       * @return Whether or not the extended help keys are set to be used
       */
      virtual bool getUseExtendedHelpKeys() const;

      /**
       * \brief Returns whether or not a key is set up as an extended help key.
       *
       * @param pKey A key to check for in the set of extended help keys
       *
       * @return true if the key is set up as an extended help key, or false
       *  if not
       */
      virtual bool keyIsExtendedHelpKey(int pKey) const;

      /**
       * \brief Sets whether or not to show the label to the right of the input.
       *
       * @param pShowRightLabel Whether or not to show the label to the right
       *  of the input (true/false)
       */
      virtual void setShowRightLabel(bool pShowRightLabel);

      /**
       * \brief Returns whether or not the label to the right of the input
       * \brief will be shown.
       *
       * @return Whether or not the label to the right of the input will be
       *  shown (true/false)
       */
      virtual bool getShowRightLabel() const;

      /**
       * \brief Sets the horizontal distance of the right label from the right
       * \brief edge of the input.  For instance, an offset of 1 means the
       * \brief right label will be one space to the right of the input window.
       *
       * @param pOffset The offset of the right label
       * @param pRefresh Whether or not to refresh the right label on the
       *  screen (defaults to false)
       */
      virtual void setRightLabelOffset(int pOffset, bool pRefresh = false);

      /**
       * \brief Returns the offset of the right label from the right edge of
       * \brief the input.
       *
       * @return The offset of the right label from the right edge of the input
       */
      virtual int getRightLabelOffset() const;

      /**
       * \brief Returns the top row of the right label window.
       *
       * @return The top row of the right label window
       */
      virtual int getRightLabelTop() const;

      /**
       * \brief Returns the leftmost column of the right label window.
       *
       * @return The leftmost column of the right label window
       */
      virtual int getRightLabelLeft() const;

      /**
       * \brief Returns the bottom row of the right label window.
       *
       * @return The bottom row of the right label window
       */
      virtual int getRightLabelBottom() const;

      /**
       * \brief Returns the rightmost column of the right label window
       *
       * @return The rightmost column of the right label window
       */
      virtual int getRightLabelRight() const;

      /**
       * \brief Returns the height of the right label window
       *
       * @return The height of the right label window
       */
      virtual int getRightLabelHeight() const;

      /**
       * \brief Returns the width of the right label window
       *
       * @return The width of the right label widow
       */
      virtual int getRightLabelWidth() const;

      /**
       * \brief Sets the width of the right label
       *
       * @param pWidth A width for the right label
       * @param pRefresh Whether or not to refresh the right label on the
       *  screen (defaults to false)
       */
      virtual void setRightLabelWidth(int pWidth, bool pRefresh = false);

      /**
       * \brief Sets the height of the right label
       *
       * @param pHeight A height for the right label
       * @param pRefresh Whether or not to refresh the right label on the
       *  screen (defaults to false)
       */
      virtual void setRightLabelHeight(int pHeight, bool pRefresh = false);

      /**
       * \brief Re-sizes the right label
       *
       * @param pHeight A height for the right label
       * @param pWidth A width for the right label
       * @param pRefresh Whether or not to refresh the right label on the
       *  screen (defaults to false)
       */
      virtual void resizeRightLabel(int pHeight, int pWidth, bool pRefresh = false);

      /**
       * \brief Sets the text of the right label.
       *
       * @param pText The text for the right label
       * @param pRefresh Whether or not to refresh the right label on the
       *  screen (defaults to false)
       */
      virtual void setRightLabel(const std::string& pText, bool pRefresh = false);

      /**
       * \brief Sets the offset & the text of the right label.
       *
       * @param pOffset The offset from the right side of the input
       * @param pText The text for the right label
       * @param pRefresh Whether or not to refresh the right label on the
       *  screen (defaults to false)
       */
      virtual void setRightLabel(int pOffset, const std::string& pText, bool pRefresh = false);

      /**
       * \brief Returns the text of the right label
       *
       * @return The text of the right label
       */
      virtual std::string getRightLabel() const;

      /**
       * \brief Returns the size of the right label
       *
       * @param pHeight (OUT) This will contain the height of the right label
       * @param pWidth (OUT) This will contain the width of the right label
       */
      virtual void getRightLabelSize(int& pHeight, int& pWidth);

      /**
       * \brief Returns the color used for the right label.
       *
       * @return The color used for the right label
       */
      virtual e_cxColors getRightLabelColor() const;

      /**
       * \brief Sets the color to use for the right label.
       *
       * @param pColor A member of the e_cxColors enumeration (see cxColors.h)
       */
      virtual void setRightLabelColor(e_cxColors pColor);

      /**
       * \brief Sets the ncurses attribute for the right label message.
       *
       * @param pAttr The ncurses atribute to apply.
       */
      virtual void setRightLabelAttr(attr_t pAttr);

      /**
       * \brief Sets the maximum text length that can be accepted by the input.
       *
       * @param pLength The maximum text length that can be accepted by the
       *  input
       */
      virtual void setMaxInputLength(int pLength);

      /**
       * \brief Returns the currently-set maximum input length.
       *
       * @return The currently-set maximum input length
       */
      virtual int getMaxInputLength() const;

      /**
       * \brief Returns the current error state of the input.  This can be
       * \brief one of the following values:<br>
       * \brief eNO_ERROR (no error)<br>
       * \brief eOUT_OF_RANGE (numeric out of range error)<br>
       * \brief eINVALID_VALUE (value is not in the list of valid strings)<br>
       * \brief eBLANK (the input is blank, and blank is not allowed)<br>
       * \brief eCUSTOM_VALIDATION_FAILED (custom validation failed - This is
       * \brief a generic error that can be used by deriving classes if
       * \brief any additional validation fails)
       *
       * @return The current error state of the input
       */
      virtual eInputErrors getErrorState() const;

      /**
       * \brief Sets the error state for the input.  This can be one of the
       * \brief following values:<br>
       * \brief eNO_ERROR (no error)<br>
       * \brief eOUT_OF_RANGE (numeric out of range error)<br>
       * \brief eINVALID_VALUE (value is not in the list of valid strings)<br>
       * \brief eBLANK (the input is blank, and blank is not allowed)<br>
       * \brief eCUSTOM_VALIDATION_FAILED (custom validation failed - This is
       * \brief a generic error that can be used by deriving classes if
       * \brief any additional validation fails)
       *
       * @param pErrorState The error state to set in the input
       */
      virtual void setErrorState(eInputErrors pErrorState);

      /**
       * \brief Returns whether or not the clear key will be used.
       *
       * @return Boolean: Whether or not the clear key will be used
       */
      virtual bool getUseClearKey() const;

      /**
       * \brief Sets whether or not the clear key should be used.
       *
       * @param pUseClearKey Boolean: Whether or not the clear key should be used
       */
      virtual void setUseClearKey(bool pUseClearKey);

      /**
       * \brief This function is called when the user clears the input.  This
       * \brief does nothing in cxMultiLineInput, but it can be overridden in
       * \brief derived classes to add additional functionality when the user
       * \brief clears the input.
       */
      virtual void additionalOnClear();

      /**
       * \brief Returns whether or not the right label window is enabled.
       *
       * @return Boolean: Whethre or not the right label window is enabled
       */
      virtual bool rightLabelEnabled() const;

      /**
       * \brief Returns whether or not the validator function is set (if
       * \brief mValidatorFunction is not nullptr and if its function pointer is set)
       *
       * @return true if the validator function is set, or false if not
       */
      bool validatorFunctionIsSet() const;

   protected:
      /**
       * \brief This is the function to be run to validate
       * \brief the text bfore focus is lost.
       */
      std::shared_ptr<cxFunction> mValidatorFunction;

      /**
       * \brief Whether or not to run the onFocus and onLeave functions.
       */
      bool mRunFocusFunctions = true;

      /**
       * \brief Copies another cxMultiLineInput's member variables
       *
       * @param pThatInput A pointer to another cxMultiLineInput to be copied
       */
      void copyCxMultiLineInputStuff(const cxMultiLineInput *pThatInput);

      /**
       * \brief Handles the input loop.  Returns cxID_QUIT or cxID_EXIT,
       * \brief depending on the user's input.  This function is meant
       * \brief not to be virtual, since it is specifically for
       * \brief cxMultiLineInput.
       *
       * @param pShowInputs Whether or not to have the inputs show themselves
       *  before they do their input loops.
       *
       * @param pRunOnLeaveFunction This will contain a boolean value
       *  that will signify whether or not to run the onLeave function
       *  (which will be set false if the user presses a function key
       *  not set in the cxMultiLineInput object that the parent cxForm
       *  or any of its parent cxMultiForm objects have set).
       *
       * @return A return code (cxID_QUIT or cxID_EXIT, depending on the
       *  user's input).
       */
      long doInputLoop(bool pShowInputs, bool& pRunOnLeaveFunction);

      /**
       * \brief Returns whether the onFocus and onLeave functions will be run.
       *
       * @return Whether or not the onFocus and onLeave functions will be run.
       */
      bool focusFunctionsWillRun() const;

      /**
       * \brief Enables or disables the onFocus and onLeave functions.
       *
       * @param pRunFocusFunctions Whether or not to run the onFocus and onLeave
       *  functions (true/false).
       */
      void runFocusFunctions(bool pRunFocusFunctions);

      /**
       * \brief Sets the parent cxForm pointer
       *
       * @param pParentForm The new parent cxForm pointer
       */
      void setParentForm(cxForm *pParentForm);

      /**
       * \brief Searches the parent cxForm (if mParentForm isn't nullptr)
       * \brief for a key that runs a function.  Returns true if it is found
       * \brief or false if not.
       *
       * @param pFunctionKey The function key to look for
       * @param pIncludeNavigationalKeys If this is true, navigational keys
       *  (such as pageUp, pageDown, arrow keys, tab, and shift-tab) will
       *  be included in looking for keys that run a function.  This defaults
       *  to false.
       *
       * @return true if the key is set up in the parent
       *  form (or its parent cxMultiForm), or false if not.
       */
      bool searchParentFormsForFKey(int pFunctionKey,
                                    bool pIncludeNavigationalKeys = false);

      /**
       * \brief Frees the memory used by the single-line inputs.
       */
      void freeInputs();

      /**
       * \brief Looks for a function tied to the last keypress and
       * \brief runs it, if one exists.  If one exists, the return
       * \brief value of the function may be set in the input if its
       * \brief mUseReturnValue is true.
       *
       * @param pFunctionExists A pointer to a bool (if non-null,
       *  it will store whether or not a function existed for the key).
       *  Defaults to nullptr.
       * @param pRunOnLeaveFunction A pointer to a bool: If non-NUL, it will
       *  store whether or not to run the onLeave function when the window
       *  exits (this is an option for cxFunction).
       *
       * @return Whether or not the input loop should continue
       */
      virtual bool handleFunctionForLastKey(bool *pFunctionExists = nullptr,
                                            bool *pRunOnLeaveFunction = nullptr) override;

      /**
       * \brief Enables the attributes for one of the m*Attrs sets for an ncurses window.
       * \brief If the m*Attrs collection is empty, this will enable the cxBase attributes
       * \brief instead.
       *
       * @param pWin A pointer to the ncurses window for which to enable the
       *  attributes (will usually be mWindow).  Note that this parameter is ignored,
       *  and each single-line input's mWindow is used instead.  This parameter is
       *  here to override enableAttrs() from cxWindow.
       * @param pItem The item type to retrieve attributes for - see the
       *  e_cxWidgetItems enumeration (defined in cxWidgetItems.h).
       */
      virtual void enableAttrs(WINDOW *pWin, e_WidgetItems pItem) override;

      /**
       * \brief Disables the attributes for one of the m*Attrs sets for an ncurses window.
       * \brief  If the m*Attrs collection is empty, this will disable the cxBase attributes
       * \brief  instead.
       *
       * @param pWindow A pointer to the ncurses window for which to disable the
       *  attributes (will usually be mWindow).
       * @param pItem The item type to retrieve attributes for - see the
       *  e_cxWidgetItems enumeration (defined in cxWidgetItems.h).
       */
      virtual void disableAttrs(WINDOW *pWin, e_WidgetItems pItem) override;

   private:
      friend class cxForm; // Because that class uses this class
      friend class cxInput;

      typedef std::vector<std::shared_ptr<cxInput> > cxInputPtrContainer;
      cxInputPtrContainer mInputs; // The collection of single-line inputs
      // mExtValue points to an "external" location to load/store the user's
      // input (basically, it will be used for user input storage as well as
      // mValue, but the value in this input will always be the what mExtValue
      // contains).  If nullptr, then it won't be used.
      std::string *mExtValue;
      int mCurrentInputLine = 0; // The index of the current single-line input
      // If mEnterAlwaysExits is true, enter will exit the input loop if
      // the cursor is in the middle of one of the input lines; if
      // false, enter will go to the next input line and stay in
      // the input loop until the user presses enter on the last line.
      bool mEnterAlwaysExits = true;
      bool mReadOnlyOnLeave = false;     // Make the input read-only when leaving showModal()?
      bool mExitOnFull = true;           // Whether to exit automatically when full
      bool mDoInputLoop = true;          // Whether to run the input loop on focus
      bool mSkipIfReadOnly = true;       // Don't wait for input if read-only?
      bool mRunValidatorFunction = true; // Controls whether or not the validator function should run
      bool mUseClearKey = true;          // Use built-in clear key?

      // Whether or not to run the validator function when the user is
      //  navigating in reverse (i.e., with the up arrow or shift-tab).
      bool mValidateOnReverse = true;

      // mParentForm is a pointer to a cxForm, in case the input appears on a
      //  cxForm.  This can be null.
      cxForm *mParentForm = nullptr;

      // If mApplyAttrDefaults is true, some attributes will be applied to
      //  the input when its kind is set.
      bool mApplyAttrDefaults = true;

      // mRanFunctionAndShouldExit keeps track of whether the input ran a key
      //  function that was set up to have the input exit when it was done.
      bool mRanFunctionAndShouldExit = false;

      // mSkipValidatorKeys keeps track of keys used with key functions where
      //  we don't want to run the validator function.
      std::set<int> mSkipValidatorKeys;

      // A set of keys that are designated as "navigation keys" -
      //  Normally, when the user presses a key, a cxMultiLineInput will check
      //  its parent window (if it's on a cxForm or cxPanel) to see if it has
      //  a function associated with that key, and if so, the input will exit
      //  its input loop without running its validator.  However, navigational
      //  keys are an exception, because usually you would want to run the
      //  validator function to run before the user leaves the input.  The
      //  default navigational keys are pageUp, pageDown, arrow keys, tab, and
      //  shift-tab.
      std::set<int> mNavKeys;

      // mInputType defines the type of input (text, numeric floating point, or
      //  numeric whole).  This specifies which type of validation should be
      //  used.
      eInputTypes mInputType;

      // mValidOptionStrings contains strings enumerating all the possible
      //  values for the input that are valid.  If this is empty, that means
      //  anything is valid.  This is a map - The key is the valid string, and
      //  the value is help text that goes along with it.
      std::map<std::string, std::string> mValidOptionStrings;
      // mAutoGenerateExtendedHelp specifies whether or not the extended help
      //  should be auto-generated from the list of valid input strings and
      //  their help texts.
      bool mAutoGenerateExtendedHelp = false;

      // mRangeLowDouble and mRangeHighDouble define low & high numeric values
      //  for floating-point numeric range validation.  If both are 0, that
      //  means any number is valid.
      long double mRangeLowDouble = 0.0;
      long double mRangeHighDouble = 0.0;
      // mRagneLowInt and mRangeLowHigh define low & high numeric values for
      //  whole numeric range validation.  If both are 0, that means any number
      //  is valid.
      long int mRangeLowInt = 0;
      long int mRangeHighInt = 0;

      // This bool specifies whether or not the validator function should
      //  display a message box when the input is invalid.
      bool mValidatorFuncMessageBox = false;

      bool mAllowBlank = true; // Allow a blank value?

      // mExtendedHelp can contain long or more verbose help text.  You can
      //  also set the color & attribute for the extended help text.
      std::string mExtendedHelp;
      e_cxColors mExtendedHelpColor;
      attr_t mExtendedHelpAttribute = A_NORMAL;
      // mExtendedHelpKeys specifies keys that can be pressed to display the
      //  extended help.  mUseExtendedHelpKeys specifies whether or not to
      //  use them.
      std::set<int> mExtendedHelpKeys;
      bool mUseExtendedHelpKeys = true;

      // mRightLabel is a window that will display an additional label to
      //  the right of the input.  mRightLabelOffset is the distance between
      //  the input and mRightLabel.
      //cxWindow mRightLabel;
      std::unique_ptr<cxWindow> mRightLabel;
      int mRightLabelOffset;
      bool mShowRightLabel; // Whether or not to display the right label

      int mMaxInputLength = 32000;  // The maximum length of input acceptable

      // mErrorState specifies a current error for the input (i.e., if
      //  validation failed).
      eInputErrors mErrorState = eNO_ERROR;

      // Disable the use of setHotkeyHighlighting from the
      //  outside.
      void setHotkeyHighlighting(bool pMessageUnderlines);

      // Fills mWindow with the current window text, but does not actually
      //  show it.
      virtual void draw() override;

      // Returns whether or not what's on the screen differs from the member
      //  data.
      bool scrDiff();

      // This is for copying a cxMultiLineInput - This copies
      //  cxValidatorFunction.
      void copyValidatorFunction(const cxMultiLineInput& pThatInput);

      // This is a helper for setInputType() - This Returns whether
      //  intOnKeyValidator() or floatingPtOnKeyValidator() from cxValidators
      //  are set up as onKey functions for the input.
      bool usingOnKeyNumericValidator() const;

      // Static cxWidgets function for validation - This calls inputValidator()
      //  for a cxMultiLineInput.
      //
      // Parameters:
      //  theInput: A pointer to the cxMultiLineInput
      //  unused: Not used
      static std::string inputValidatorStatic(void *theInput, void *unused);

      // Static cxWidgets onKeypress function - This calls onKeypress() for
      //  a cxMultiLineInput.
      //
      // Parameters:
      //  theInput: A pointer to the cxMultiLineInput
      //  unused: Not used
      static std::string onKeypressStatic(void *theInput, void *unused);

      // This will generate mExtendedHelp based on mValidOptionStrings, if
      //  mValidOptionStrings has anything in it.
      void generateExtendedHelp();

      // Validates a string against mValidOptionStrings.  If mValidOptionStrings
      //  is empty, or if the string is in it, this will return true.  If
      //  mValidOptionStrings is not empty and the string isn't in it, this
      //  will return false.
      bool checkValidOptionStrings(const std::string& pValue);

      // This is a helper for validation.  For a numeric input type, this
      //  returns whether or not a value is within the set valid range.  If
      //  both the low & high range values are 0, this will assume that any
      //  number is valid and will return true.  For a non-numeric input type,
      //  this will return true.  If the given value does not contain a valid
      //  number, this will return false.
      //
      // Parameters:
      //  pValue: A value to check
      bool valueInRange(const std::string& pValue) const;
};

#endif
