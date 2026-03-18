// Copyright (c) 2026 E. Oulashin
#ifndef __CXINPUT_H__
#define __CXINPUT_H__

// Copyright (c) 2005-2007 Michael H. Kinney
//
// This class represents an input box with
//  a label.

#include "cxWindow.h"
#include "cxFunction.h"
#include "cxTextValidator.h"
#include "cxInputOptions.h"
#include "cxColors.h"
#ifdef WANT_TIMEOUT
#include <sys/signal.h> // For sigaction
#endif
#include <string>
#include <map>
#include <set>
#include <memory>

// Forward declaration
class cxMultiLineInput;

/** \class cxInput
 *  \brief Represents a single-line input box with a label.  This class
 *  \brief generally should not be used directly, since cxMultiLineInput
 *  \brief uses this class, and cxMultiLineInput can do everything that
 *  \brief this class does, plus more.
 *  \brief
 *  This class allows you to get typed input from a user - the
 *  methods showModal() and setFocus() make the input appear on the screen
 *  and process input.  They also return a return code,
 *  depending on the user's action.  If the user pressed ESC,
 *  cxID_QUIT is returned to signify that the user quit out
 *  of the input and didn't want to do anything.  If the
 *  user presses ENTER, cxID_EXIT is returned to signify that
 *  the value in the input should be used. getLastKey() can
 *  be called to get the last key typed by the user (see
 *  cxKeyDefines and the ncurses list of key definitions (man
 *  getch)).<br>
 *  There is also a set of functions that may be set, that will
 *  be run at various times.  The order of the functions is
 *  as follows:<br>
 *  <ol>
 *  <li> The "on focus" function is executed immediately at the start of
 *       showModal() (see setOnFocusFunction() in cxWindow).
 *  <li>A function can be set up to fire after each keypress with
 *      setOnKeyFunction().  This function can be toggled to run or not with
*       toggleOnKeyFunction().
 *  <li> When certain hotkeys (set up with setKeyFunction()) are pressed,
 *       functions associated with those keys are run.
 *  <li> After the user is finished entering text, the validator function is
 *       run (see setValidatorFunction()).  The purpose of this function is to
 *       do any validation of the input that requires special logic.
 *  <li> Just before showModal() exits, the "on leave" function is executed
 *       (see setOnLeaveFunction() in cxWindow).
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
 *  Also, note that if you call setValue() to set a value that is
 *  too long to be displayed in the input, the input will only
 *  display what can fit inside the input, although getValue()
 *  will return the entire string that was set by setValue().
 *  For example:<br>
 *  cxInput iInput(nullptr, 0, 0, 10, "Text:");<br>
 *  iInput.setValue("Some text for the input to hold");<br>
 *  iInput.getValue();  // Returns "Some text for the input to hold"<br>
 *  iInput.setFocus(); // Can only display "Some " in the input, since the width is 10<br>
 *  Setting a value longer than the input can display can cause some
 *  behavior that you may not expect, however.  If you set a value
 *  too long for the input to display and then call setFocus(), the
 *  input will leave focus as soon as the user types something, because
 *  the input is full.  However, being able to store more than the input
 *  can display can be useful if you are using a cxInput for display
 *  purposes only, and you still need to be able to store the entire value.<br>
 *  This class has an overloaded setBorderStyle(), which changes the
 *  border style and re-calculates some of the internal data structures.
 *  One side-effect of this is that the acceptable length of input may increase.
 *  If you start out with a borderless input and turn on borders, the
 *  acceptable length of input will stay the same, but if you then turn the
 *  borders back off, the length of the input will increase because the
 *  borders are now gone, and there are 2 more available spaces for input;
 *  thus, the input will be 2 spaces wider than it originally started.<br>
 *  By default, when the user presses any function keys that aren't assigned
 *  to any functions, cxInput will exit its input loop so that calling code
 *  can process the function keys.  This can be changed with a call to
 *  trapNonAssignedFKeys().
 *
 *
 */
class cxInput : public cxWindow
{
   public:
      /**
       * \brief Default constructor
       * @param pParentWindow Pointer to parent window; defaults to nullptr
       * @param pRow Y location of input window; defaults to 0
       * @param pCol X location of input window; defaults to 0
       * @param pWidth Width of input window; defaults to DEFAULT_WIDTH
       *  (defined as 80 in cxObject.h)
       * @param pLabel Label for input window; defaults to blank
       * @param pBorderStyle The type of border to use - Defaults to
       *  eBS_NOBORDER; See the enumeration in cxBorderStyles.h
       * @param pInputOption The editibility option - Defaults to
       *  eINPUT_EDITABLE.  See the enumeration in cxEditoptions.h
       * @param pExitOnFull Whether setFocus() should return when the input
       *  is full; defaults to false
       * @param pExtVal An external value that the input should be set
       *  to; defaults to nullptr
       */
      explicit cxInput(cxWindow *pParentWindow = nullptr, int pRow = 0,
                       int pCol = 0, int pWidth = DEFAULT_WIDTH,
                       const std::string& pLabel = "",
                       eBorderStyle pBorderStyle = eBS_NOBORDER,
                       eInputOptions pInputOption = eINPUT_EDITABLE,
                       bool pExitOnFull = false, std::string *pExtVal = nullptr);

      /**
       * \brief Copy constructor
       * @param pThatInput Another cxInput object to be copied
       */
      cxInput(const cxInput& pThatInput);

      /**
       * \brief Copy constructor that will be used by cxMultiLineInput when
       * \brief copying a cxMultiLineInput.  This was originally meant to be
       * \brief private (with cxMultiLineInput as a friend class), but when
       * \brief updating the code to use shared_ptr instead of raw
       * \brief dynamically-allocated pointers, it looked like make_shared()
       * \brief has trouble accessing private constructors.
       * @param pThatInput Another cxInput object to be copied
       */
      cxInput(const cxInput& pThatInput, cxMultiLineInput *pParentMLInput);

      /**
       * \brief Destructor
       */
      virtual ~cxInput();

      /**
       * \brief Shows the window, along with the user's input
       * @param pBringToTop Whether or not to bring this window to the top.  Defaults to false.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Always uses false.
       * @return The return value of cxWindow::show()
       */
      virtual long show(bool pBringToTop = false, bool pShowSubwindows = false) override;

      /**
       * \brief Handles user input for the form.
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
       * Returns the label for the input
       * @return The label associated with the input
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
       * @param pRemoveLeadingSpaces Whether or not to remove leading spaces.  Defaults to false.
       * @param pRemoveTrailingSpaces Whether or not to remove trailing spaces.  Defaults to false.
       * @return The user's input
       */
      virtual std::string getValue(bool pRemoveLeadingSpaces = false, bool pRemoveTrailingSpaces = false) const;

      /**
       * Sets the input text
       * @param pValue The text of the input
       * @param pRefresh Whether or not to refresh the input (defaults to false)
       * @return Whether or not the new value is valid.
       */
      virtual bool setValue(const std::string& pValue, bool pRefresh = false);

      /**
       * Returns the pointer to the "external" variable storing user input.
       * @return Pointer to location of input, if external input set
       */
      std::string* getExtValue() const;

      /**
       * Setter for the "external" user value variable pointer.
       * @param pExtVal The new pointer to the external variable
       * @param pRefresh Whether or not to refresh the input
       */
      void setExtValue(std::string *pExtVal, bool pRefresh = false);

      /**
       * Accessor for mExitOnFull
       * @return Whether setFocus() should return when the input is full
       */
      bool getExitOnFull() const;

      /**
       * \brief Sets whether the input should leave the input loop when it's
       * \brief full.
       *
       * @param pExitOnFull Whether setFocus() should return when the input is full
       */
      void setExitOnFull(bool pExitOnFull);

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
       * \brief Returns whether the input exits its input loop when a mouse
       * \brief click occurs outside the input window.
       *
       * @return true if the input exits on outside mouse clicks
       */
      bool getExitOnMouseOutside() const;

      /**
       * \brief Sets whether the input should exit its input loop when a
       * \brief mouse click occurs outside the input window.  This is useful
       * \brief for container widgets like cxGrid that need to redirect
       * \brief focus to a different cell when the user clicks elsewhere.
       *
       * @param pExitOnMouseOutside Whether to exit on outside mouse clicks
       */
      void setExitOnMouseOutside(bool pExitOnMouseOutside);

      /**
       * \brief Returns whether the input exits when the left arrow key is
       * \brief pressed at position 0 or the right arrow key is pressed at
       * \brief the end of the text.
       */
      bool getExitOnArrowAtBoundary() const;

      /**
       * \brief Sets whether the input should exit its input loop when the
       * \brief left arrow key is pressed at position 0 or the right arrow
       * \brief key is pressed at the end of the text.  Useful for grid cells
       * \brief where arrow keys should navigate between cells at text boundaries.
       */
      void setExitOnArrowAtBoundary(bool pExitOnArrowAtBoundary);

      /**
       * Accessor for mMustFill
       * @return Returns whether mMustFill is set
       */
      bool getMustFill() const;

      /**
       * Mutator for mMustFill
       * @param pMustFill New value for mMustFill; boolean
       */
      void setMustFill(bool pMustFill);

      /**
       * Accessor for mMasked
       * @return Whether to mask input (display asterisks, not text, in input field)
       */
      bool getMasked() const { return(mMasked); }

      /**
       * Enables/disables masking
       * @param pMasking Whether to mask input (display asterisks, not text, in input field)
       */
      void toggleMasking(bool pMasking) { mMasked = pMasking; }

      /**
       * Accessor for the masking character
       * @return The masking character
       */
      char getMaskChar() const { return(mMaskChar); }

      /**
       * Mutator for the masking character
       * @param pMaskChar The new masking character to use
       */
      void setMaskChar(char pMaskChar) { mMaskChar = pMaskChar; }

      /**
       * \brief Accessor for mInputOption; Returns an int representing the
       * \brief editability option for the input - See the eInputOptions
       * \brief enumeration in cxInputOptions.h
       * @return The edit option for this input (i.e., eINPUT_EDITABLE, eINPUT_READONLY)
       */
      int getInputOption() const;

      /**
       * \brief Sets the input editability (i.e., eINPUT_EDITABLE, eINPUT_READONLY) -
       * \brief See the eInputOptions enumeration in cxInputOptions.h
       *
       * @param pInputOption The new editability option
       */
      void setInputOption(eInputOptions pInputOption);

      /**
       * Sets the validator string
       * @param pVString The new validator string
       */
      void setValidator(const std::string& pVString);

      /**
       * Returns the validator string
       * @return The validator string
       */
      const std::string& getValidatorStr() const;

      /**
       * Returns the maximum length of input that can be accepted
       * @return Maximum lenght of input that can be accepted by this input line
       */
      int getInputLen() const;

      /**
       * Sets the horizontal (x) cursor position
       * @param pCursorX Horizontal cursor position
       */
      void setCursorX(int pCursorX);

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
       * \brief Changes the input's width.  The upper-left coordinate stays
       * \brief the same.  Note that pNewHeight is not used; that parameter is
       * \brief only here so that this method will override cxWindow's resize()
       * \brief method.  The height is always 1.
       * @param pNewHeight Not used (always uses a height of 1)
       * @param pNewWidth The new width
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void resize(int pNewHeight, int pNewWidth, bool pRefresh = true) override;

       /**
       * Sets whether or not to exit upon use of a function key.
       * @param pExitOnFunctionKey Whether to exit on function key press
       */
      void setExitOnFKey(bool pExitOnFunctionKey);

      /**
       * Returns whether the text entered into the input is valid.
       *
       * @return Whether or not the text entered is valid
       */
      virtual bool textIsValid() const;

      /**
       * Returns whether some text is valid according to the input's validator.
       * @param pText The text to be validated
       * @return Whether or not the text is valid
       */
      virtual bool textIsValid(const std::string& pText) const;

      /**
       * \brief Sets the label color
       *
       * @param pColor A member of the e_cxColors enumeration from cxColors.h
       */
      void setLabelColor(e_cxColors pColor);

      /**
       * Gets the label color
       *
       * @return A value from the e_cxColors enumeration representing the label
       *  color
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
       * @return A value from the e_cxColors enumeration representing the label
       *  color
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
       * \brief Enables/disables the display of the cursor.
       *
       * @param pShowCursor Whether or not to show the cursor (true/false).
       */
      void toggleCursor(bool pShowCursor);

      /**
       * \brief Clears the value of a cxInput
       *
       * @param theInput A pointer to a cxInput object
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
       * @param pFunction Function to point to--must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       *
       * @return true if the function was set, or false if not.
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
       *
       * @return true if the function was set, or false if not.
       */
      bool setValidatorFunction(funcPtr2 pFunction, void *p1, void *p2);

      /**
       * Sets a validator function to be run before focus is lost.
       * If the function returns a blank string, it is assumed that the
       * text is valid; if the function returns a non-blank string, it is
       * assumed that the text was incorrect.
       * @param pFunction Function to point to--must have signature string func()
       *
       * @return true if the function was set, or false if not.
       */
      bool setValidatorFunction(funcPtr0 pFunction);

      /**
       * \brief Un-sets the validator function.
       */
      void clearValidatorFunction();

      /**
       * \brief Sets a function to be run whenever a key is pressed (referred
       * \brief to as the "onKey" function).
       * @param pFunction Function to point to
       *
       * @return true if the function was set, or false if not
       */
      virtual bool setOnKeyFunction(const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets a function to be run whenever a key is pressed (referred
       * \brief to as the "onKey" function).
       * @param pFunction Function to point to--must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       *
       * @return true if the function was set, or false if not
       */
      virtual bool setOnKeyFunction(funcPtr4 pFunction, void *p1, void *p2,
                            void *p3, void *p4);

      /**
       * \brief Sets a function to be run whenever a key is pressed (referred
       * \brief to as the "onKey" function).
       * @param pFunction Function to point to--must have signature string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       *
       * @return true if the function was set, or false if not
       */
      virtual bool setOnKeyFunction(funcPtr2 pFunction, void *p1, void *p2);

      /**
       * \brief Sets a function to be run whenever a key is pressed (referred
       * \brief to as the "onKey" function).
       * @param pFunction Function to point to--must have signature string func()
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       *
       * @return true if the function was set, or false if not
       */
      virtual bool setOnKeyFunction(funcPtr0 pFunction);

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
      virtual bool onKeyFunctionEnabled() const;

      /**
       * \brief Returns whether the value in the input takes up the maximum possible space in the input.
       *
       * @return Whether or not the value in the input takes up the maximum possible space
       */
      virtual bool isFull() const;

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
       * \brief Sets whether the input can be set editable.
       *
       * @param pCanBeEditable If true, the input can be set editable; if false,
       *  the input will always be read-only.
       */
      void setCanBeEditable(bool pCanBeEditable);

      /**
       * \brief Sets the border style
       *
       * @param pBorderStyle A eBorderStyle enumeration value (see cxBorderStyles.h)
       */
      virtual void setBorderStyle(eBorderStyle pBorderStyle) override;

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
       * \brief Enables or disables the input loop.
       * \brief If the input loop is disabled, you can still
       * \brief call setFocus() and showModal() on the
       * \brief input - the onFocus and onLeave functions
       * \brief will still run, but the input won't
       * \brief wait for user input.
       *
       * @param pDoInputLoop Whether to enable or disable
       *  brief the input loop (true/false)
       */
      virtual void enableInputLoop(bool pDoInputLoop);

      /**
       * \brief Returns whether the input loop is enabled
       *
       * @return
       */
      virtual bool inputLoopEnabled() const;

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
      bool isEditable() const;

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
       * \brief Returns a pointer to the parent window.  If the cxInput
       * \brief has a parent cxMultiLineInput, a pointer to that will be
       * \brief returned; otherwise, a pointer to the parent cxWindow
       * \brief will be returned.
       *
       * @return A pointer to the parent window
       */
      virtual cxWindow* getParent() const override;

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
       * \brief Runs the input's validator function and returns its return
       * \brief value.  If the validator function is not set, this returns
       * \brief a blank string.
       *
       * @return The return value of the validator function, or a blank string
       *  if the validator function is not set.
       */
      virtual std::string runValidatorFunction() const;

      /**
       * \brief Sets the amount of time (in seconds) that the input should
       * \brief wait when idle before exiting the input loop (0 = no timeout).
       *
       * @param pTimeout The amount of time (in seconds) that the input should
       *  wait when idle before exiting the input loop (0 = no timeout).
       */
      virtual void setTimeout(int pTimeout);

      /**
       * \brief Returns the amount of time that the
       *
       * @return
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
       * \brief Toggles the option to force text to be upper-case.
       *
       * @param pForceUpper true if text should be upper-case, or false if
       *  not.
       */
      void setForceUpper(bool pForceUpper);

      /**
       * \brief Returns the option for forcing text to upper-case.
       *
       * @return Whether or not upper-casing text is enabled
       */
      bool getForceUpper() const;

      /**
       * \brief Sets the maximum text length that can be accepted by the input.
       *
       * @param pLength The maximum text length that can be accepted by the
       *  input
       */
      void setMaxInputLength(int pLength);

      /**
       * \brief Returns the currently-set maximum input length.
       *
       * @return The currently-set maximum input length
       */
      int getMaxInputLength() const;

      /**
       * \brief inputClearKey is a default key to be used to clear inputs.
       */
      static int inputClearKey;

   protected:
      // Attributes for various element (in addition to the ones provided
      //  in cxWindow)
      /**
       * \brief Label attributes
       */
      std::set<attr_t> mLabelAttrs;
      /**
       * \brief Read-only data attributes
       */
      std::set<attr_t> mDataReadonlyAttrs;
      /**
       * \brief Editable data attributes
       */
      std::set<attr_t> mDataEditableAttrs;

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

      /**
       * \brief Returns the color of one of the items in a window.
       *
       * @param pItem The item type (a member of the e_WidgetItems enumeration
       *  - see cxWidgetItems.h)
       *
       * @return A member of the e_cxColors enumeration representing the color
       *  of the item (see cxColors.h)
       */
      virtual e_cxColors getItemColor(e_WidgetItems pItem) const override;

   private:
      friend class cxMultiLineInput;  // Because that class uses this class

      // The following member is set if this input is in a cxMultiLineInput.
      cxMultiLineInput *mParentMLInput = nullptr;
      std::string mValue; // Holds the user's input
      // mName is an alternative means of identifying the input (by name).
      std::string mName;
      // mExtValue points to an "external" location to load/store the user's input
      // (basically, it will be used for user input storage as well as mValue,
      // but the value in this input will always be the what mExtValue contains).
      // If nullptr, then it won't be used.
      std::string *mExtValue;
      int mInputStartX = 0;                 // Starting X cursor position for text input
      int mYPos = 0;                        // Y cursor position for input
      int mInputLen = 0;                    // Valid length of input
      int mRightMax = 0;                    // Rightmost input boundary
      bool mExitOnFull;                     // Whether the input loop should stop when the input is full
      // mExitOnBackspaceAtFront specifies whether the input loop should stop when
      //  backspace is pressed in the first input position.
      bool mExitOnBackspaceAtFront = false;
      bool mExitOnMouseOutside = false;   // Exit input loop on mouse click outside this window
      bool mExitOnArrowAtBoundary = false; // Exit on left arrow at pos 0 or right arrow at end of text
      bool mMustFill = false;               // Whether or not the user must fill the entire field
      bool mMasked = false;                 // Whether or not the field is masked (i.e. for a password)
      char mMaskChar = '*';                 // The character to display for character masking
      cxTextValidator mValidator;           // For use with text validation
      eInputOptions mInputOption;           // Specifies normal, read-only, etc.
      bool mExitOnFunctionKey = false;      // Whether or not to exit on function key press.
      bool mHasFocus = false;               // Whether or not focus is set
      // mReadOnlyOnLeave stores whether or not the input should go read-only
      //  when setFocus() finishes.
      bool mReadOnlyOnLeave = false;
      bool mCanBeEditable = true;           // Whether or not the input can be editable
      bool mJustStartedFocus = false;       // True immediately when focus is set
      // If mTrapNonAssignedFKeys is true, function keys that aren't
      //  assigned to anything won't cause the input to exit its
      //  input loop.
      bool mTrapNonAssignedFKeys = false;
      bool mDoInputLoop = true; // Whether or not to run the input loop on focus
      // mCursorAfterInput specifies whether to place the cursor after the
      //  input text when showing the input.
      bool mCursorAfterInput = true;

      // Function to be run when a key is pressed
      std::shared_ptr<cxFunction> mOnKeyFunction; // Function to be run when a key is pressed
      bool mRunOnKeyFunction = true;              // Whether or not to run the onKey function
      // Function to be run to validate the text before focus is lost
      std::shared_ptr<cxFunction> mValidatorFunction;
      short mValueColorPair;
      bool mShowCursor = true;              // Whether or not to show the cursor
      // Whether or not to run the validator function when the user is
      //  navigating in reverse (i.e., with the up arrow or shift-tab).
      bool mValidateOnReverse = true;
      bool mForceUpper = false;             // Convert letters to upper-case
      int mMaxInputLength = 0;              // Maximum length of text input (0 = unlimited)
      int mScrollOffset = 0;               // Index of first visible character in mValue (for scrolling)
      int mCursorPos = 0;                  // Cursor position within mValue (0-based index)

      // Returns the text entered in the window, from mInputStartX (inclusive)
      //  to a given horizontal position (exclusive), as a string.
      //
      // Parameters:
      //  pValue: This is where the text will be stored.
      //  pX: The current horizontal position of the cursor.
      //  pCheckPrintable: Whether or not to check whether the last key
      //   pressed is a printable character before grabbing the value.
      //   Defaults to true.
      void getInputText(std::string& pValue, int pX, bool pCheckPrintable = true);

      // Performs a 'backspace' behavior.  This was written as a facilitator for
      //  setFocus().
      // Parameters:
      //  y: The current vertical cursor position
      //  x: The current horizontal cursor position
      //  prevInput: The last contents of the input
      inline void doBackspace(int y, int x, std::string& prevInput);

      // Redraws the visible portion of mValue in the ncurses window,
      // accounting for mScrollOffset. Pads with spaces to fill mInputLen.
      void redrawVisibleValue();

      // Adjusts mScrollOffset so that mCursorPos is visible within the
      // display area (mInputLen characters wide).
      void ensureCursorVisible();

      // Enables the attributes in mValueAttrs.
      inline void enableValueAttrs();

      // Disables the attributes in mValueAttrs.
      inline void disableValueAttrs();

      // Handles the input loop.  Returns cxID_QUIT or cxID_EXIT,
      //  depending on what happens when the user exits the input.
      // Parameters:
      //  x: The current horizontal position of the cursor
      //  y: The current vertical position of the cursor
      //  rightLimit: The rightmost limit of the cursor position (inclusive)
      //  updatePrevInput: Whether or not to update the previous input value
      //  prevInput: The previous input value
      long doInputLoop(int x, int y, int rightLimit,
                       bool updatePrevInput, std::string& prevInput);

      // Adds a key to the list of keys that will stop the input
      //  loop.
      void addStopKey(int pKey);

      // Disable the use of setHotkeyHighlighting from the
      //  outside.
      virtual void setHotkeyHighlighting(bool pMessageUnderlines) override;

      // Fills mWindow with the current window text, but does not actually
      //  show it.
      virtual void draw() override;

#ifdef WANT_TIMEOUT
      // mTimeout is the length of time (in seconds) for the input to be idle
      //  before exiting the input loop.  mTimeoutSigaction is a struct that
      //  specifies the timeout function to call.
      int mTimeout = 0;
      struct sigaction mTimeoutSigaction;

      // This function will get called upon idle timeout.  (This function will
      //  get set up for the SIGALRM signal; pSignal is there because signal
      //  functions need to have that particular signature (void return type,
      //  and an int parameter)).  This function will call exitNow() on the
      //  input.
      static void idleTimeoutHandler(int pSignal);
#endif

      // Runs the function pointed to by mOnKeyFunction, and returns the return
      //  value of the function.
      std::string runOnKeyFunction() const;

      // This is for copying a cxInput - This copies one of the cxFunction
      //  pointers (mOnKeyFunction, mValidatorFunction) from the other input.
      //
      // Parameters:
      //  pThatInput: The cxInput being copied
      //  pDestFunc: The cxFunction pointer being copied into (mOnKeyFunction
      //   or mValidatorFunction) - This is a reference to a cxFunction*,
      //   so mOnKeyFunction or mValidatorFunction can be passed to this
      //   method without any funky syntax.
      //  pSrcFunc: The cxFunction pointer being copied from
      //   (pThatInput.mOnKeyFunction or pThatInput.mValidatorFunction)
      //
      // An example function call:
      //  copyCxFunction(pThatInput, mOnKeyFunction, pThatInput.mOnKeyFunction);
      void copyCxFunction(const cxInput& pThatInput, /*OUT*/std::shared_ptr<cxFunction>& pDestFunc,
                          /*IN*/const std::shared_ptr<cxFunction>& pSrcFunc);

      // Returns whether or not the validator function is set (if
      //  mValidatorFunction is not nullptr and if its function pointer is set)
      bool validatorFunctionIsSet() const;

};

#endif
