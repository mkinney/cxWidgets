#ifndef __CXSEARCHPANEL_H__
#define __CXSEARCHPANEL_H__

// Copyright (c) 2006-2007 Michael H. Kinney
//
// This class is based on cxPanel, and it contains a cxForm at the
// top and a cxMenu below the form.

#include "cxPanel.h"
#include "cxForm.h"
#include "cxMenu.h"
#include <string>
#include <memory>

/**
  * \brief This class is based on cxPanel, and it contains a cxForm at the top
  * \brief and a cxMenu below the form.  This class was designed as something
  * \brief can be used to do database queries using user-inputted data from the
  * \brief form and show the results in the menu.
 */
class cxSearchPanel : public cxPanel
{
   public:
      /**
       * \brief Default constructor.
       *
       * @param pParentWindow A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the window.
       * @param pWidth The width of the window.
       * @param pFormTitle The title for the form
       * @param pMenuTitle The title for the menu
       * @param pFormBorderStyle The border style for the form
       * @param pMenuBorderStyle The border style for the menu
       */
      explicit cxSearchPanel(cxWindow *pParentWindow = nullptr,
                       int pRow = 0, int pCol = 0,
                       int pHeight = DEFAULT_HEIGHT,
                       int pWidth = DEFAULT_WIDTH,
                       const std::string& pFormTitle = "",
                       const std::string& pMenuTitle = "",
                       eBorderStyle pFormBorderStyle = eBS_SINGLE_LINE,
                       eBorderStyle pMenuBorderStyle = eBS_SINGLE_LINE);

      /**
       * \brief Copy constructor
       *
       * @param pPanel Another cxSearchPanel to copy
       */
      cxSearchPanel(const cxSearchPanel& pPanel);

      virtual ~cxSearchPanel();

      /**
       * \brief Returns a pointer to the cxForm on the panel.
       *
       * @return A pointer to the cxForm on the panel
       */
      const std::shared_ptr<cxForm>& getForm() const;

      /**
       * \brief Returns a pointer to the cxMenu on the panel.
       *
       * @return A pointer to the cxMenu on the panel
       */
      const std::shared_ptr<cxMenu>& getMenu() const;

      /**
       * \brief Adds an input to the form and returns a pointer to the new
       * \brief input.
       *
       * @param pRow Y location of the upper-left corner of the input, relative to the form
       * @param pCol X location of the upper-left corner of the input, relative to the form
       * @param pHeight The height of the input
       * @param pWidth The width of the input
       * @param pLabel The label to use with the input
       * @param pValidator String to use to validate input; see README.txt for details;
       *  blank by default
       * @param pHelpString A string to be displayed in the statusbar of the form when the
       *  input has focus; defaults to empty string
       * @param pInputKind Used to set the kind of input (i.e., normal, read-only, etc.);
       *  defaults to eINPUT_EDITABLE
       * @param pName A name for the input (can be used as an alternative way to
       *  identify the input)
       * @param pExtValue A pointer to a string to be linked to this input and used for its
       *  value.  Defaults to nullptr for none.
       *
       * @return A pointer to the new cxMultiLineInput object that is created.
       */
      virtual std::shared_ptr<cxMultiLineInput> appendToForm(int pRow, int pCol, int pHeight,
                                     int pWidth, const std::string& pLabel,
                                     const std::string& pValidator = "",
                                     const std::string& pHelpString = "",
                                     eInputOptions pInputKind = eINPUT_EDITABLE,
                                     const std::string& pName = "",
                                     std::string *pExtValue = nullptr);

      /**
       * \brief Adds a combo box to the form.  Returns a pointer to the new input.
       *
       * @param pRow Y location of the upper-left corner of the input, relative to the form
       * @param pCol X location of the upper-left corner of the input, relative to the form
       * @param pHeight The height of the input
       * @param pWidth The width of the input
       * @param pLabel The label to use with the input
       * @param pValidator String to use to validate input; see README.txt for details;
       *  blank by default
       * @param pHelpString A string to be displayed in the statusbar of the form when the
       *  input has focus; defaults to empty string
       * @param pInputKind Used to set the kind of input (i.e., normal, read-only, etc.);
       *  defaults to eINPUT_EDITABLE
       * @param pName A name for the input (can be used as an alternative way to
       *  identify the input)
       * @param pExtValue A pointer to a string to be linked to this input and used for its
       *  value.  Defaults to nullptr for none.
       * @return A pointer to the new cxMultiLineInput object that is created.
       */
      virtual std::shared_ptr<cxComboBox> appendComboBoxToForm(int pRow, int pCol, int pHeight,
                                     int pWidth, const std::string& pLabel,
                                     const std::string& pValidator = "",
                                     const std::string& pHelpString = "",
                                     eInputOptions pInputKind = eINPUT_EDITABLE,
                                     const std::string& pName = "",
                                     std::string *pExtValue = nullptr);

      /**
       * \brief Adds a string item to the end of the menu.
       *
       * @param pDisplayText The text to display for the item
       * @param pReturnCode The integer code that the the menu should return when show() exits
       * @param pHelpString A string to be displayed in the statusbar of the
       *  main window when the item is highlighted
       * @param pItemText Alternate text to use for the item
       * @param pType The kind of menu item (from the cxMenuItemType
       *  enumeration in cxMenuItemType.h)
       */
      virtual void appendToMenu(const std::string& pDisplayText, long pReturnCode,
                          const std::string& pHelpString = "",
                          const std::string& pItemText = "",
                          cxMenuItemType pType = cxITEM_NORMAL);

      /**
       * \brief Adds a string item to the end of the menu, defaulting the
       * \brief item's return code to the next one available.
       *
       * @param pDisplayText The text to display for the item
       * @param pHelpString A string to be displayed in the statusbar of the
       *  main window when the item is highlighted
       * @param pItemText Alternate text to use for the item
       * @param pType The kind of menu item (from the cxMenuItemType
       *  enumeration in cxMenuItemType.h)
       */
      virtual void appendToMenu(const std::string& pDisplayText, const std::string& pHelpString = "",
                                const std::string& pItemText = "",
                                cxMenuItemType pType = cxITEM_NORMAL);

      /**
       * \brief Clears the inputs on the form.
       *
       * @param pRefresh Whether or not to refresh the screen.  Defaults to false.
       * @param pOnlyEditable Whether or not to clear only the editable inputs.
       *  Defaults to false.
       */
      virtual void clearFormInputs(bool pRefresh = false, bool pOnlyEditable = false);

      /**
       * \brief Removes the menu items.
       *
       * @param pRefresh Whether or not to refresh the screen.  Defaults to false.
       */
      virtual void clearMenu(bool pRefresh = false);

      /**
       * \brief Sets the onFocus funtion for the form.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc(void*, void*, void*, void*).
       * @param p1 A pointer to the first argument
       * @param p2 A pointer to the 2nd argument
       * @param p3 A pointer to the 3rd argument
       * @param p4 A pointer to the 4th argument
       * @param pUseVal Whether or not the function's return value should be set as
       *  the value in the field (defaults to false)
       * @param pExitAfterRun Whether or not the form should exit after the
       *  function runs (defaults to false; if true, the input loop won't be
       *  started).
       */
      void setFormOnFocusFunction(funcPtr4 pFuncPtr, void *p1, void *p2,
                           void *p3, void *p4, bool pUseVal = false,
                           bool pExitAfterRun = false);

      /**
       * \brief Sets the onFocus funtion for the form.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc(void*, void*).
       * @param p1 A pointer to the first argument
       * @param p2 A pointer to the 2nd argument
       * @param pUseVal Whether or not the function's return value should be set as
       *  the value in the field (defaults to false)
       * @param pExitAfterRun Whether or not the form should exit after the
       *  function runs (defaults to false; if true, the input loop won't be
       *  started).
       */
      void setFormOnFocusFunction(funcPtr2 pFuncPtr, void *p1, void *p2,
                                  bool pUseVal = false,
                                  bool pExitAfterRun = false);

      /**
       * \brief Sets the onFocus funtion for the form.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc().
       * @param pUseVal Whether or not the function's return value should be set as
       *  the value in the field (defaults to false)
       * @param pExitAfterRun Whether or not the form should exit after the
       *  function runs (defaults to false; if true, the input loop won't be
       *  started).
       */
      void setFormOnFocusFunction(funcPtr0 pFuncPtr, bool pUseVal = false,
                                  bool pExitAfterRun = false);

      /**
       * \brief Sets the onLeave function for the form.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc(void*, void*, void*, void*).
       * @param p1 A pointer to the first argument
       * @param p2 A pointer to the 2nd argument
       * @param p3 A pointer to the 3rd argument
       * @param p4 A pointer to the 4th argument
       */
      void setFormOnLeaveFunction(funcPtr4 pFuncPtr, void *p1, void *p2,
                                  void *p3, void *p4);

      /**
       * \brief Sets the onLeave function for the form.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc(void*, void*).
       * @param p1 A pointer to the first argument
       * @param p2 A pointer to the 2nd argument
       */
      void setFormOnLeaveFunction(funcPtr2 pFuncPtr, void *p1, void *p2);

      /**
       * \brief Sets the onLeave function for the form.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc(void*, void*).
       */
      void setFormOnLeaveFunction(funcPtr0 pFuncPtr);

      /**
       * \brief Sets the onFocus function for the menu.
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseVal
       * @param pExitAfterRun Whether or not the menu should exit after the function
       *  finishes running (defaults to false).
       */
      virtual void setMenuOnFocusFunction(funcPtr4 pFunction, void *p1, void *p2,
                              void *p3, void *p4, bool pUseVal = false,
                              bool pExitAfterRun = false);

      /**
       * \brief Sets the onFocus function for the menu.
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseVal
       * @param pExitAfterRun Whether or not the menu should exit after the function
       *  finishes running (defaults to false).
       */
      virtual void setMenuOnFocusFunction(funcPtr2 pFunction, void *p1, void *p2,
                              bool pUseVal = false, bool pExitAfterRun = false);

      /**
       * \brief Sets the onFocus function for the menu.
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func()
       * @param pUseVal
       * @param pExitAfterRun Whether or not the menu should exit after the function
       *  finishes running (defaults to false).
       */
      virtual void setMenuOnFocusFunction(funcPtr0 pFunction,
                             bool pUseVal = false, bool pExitAfterRun = false);

      /**
       * \brief Sets the onLeave function for the menu.
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setMenuOnLeaveFunction(funcPtr4 pFunction, void *p1, void *p2,
                              void *p3, void *p4);

      /**
       * \brief Sets the onLeave function for the menu.
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       */
      virtual void setMenuOnLeaveFunction(funcPtr2 pFunction, void *p1, void *p2);

      /**
       * \brief Sets the onLeave function for the menu.
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func()
       */
      virtual void setMenuOnLeaveFunction(funcPtr0 pFunction);

      /**
       * \brief Changes the width and height, and re-sizes the form and menu
       * \brief accordingly.
       *
       * @param pNewHeight The new height
       * @param pNewWidth The new width
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void resize(int pNewHeight, int pNewWidth, bool pRefresh = true) override;

      /**
       * \brief Sets the form's height.  The menu will also be re-sized accordingly
       * \brief to appear directly underneath the form.
       *
       * @param pNewHeight The new height for the form
       * @param pRefresh Whether or not to refresh the screen.  Defaults to true.
       */
      virtual void setFormHeight(int pNewHeight, bool pRefresh = true);

      /**
       * \brief Sets the menu's height.  The form will also be re-sized accordingly
       * \brief to appear directly above the menu.
       *
       * @param pNewHeight The new height for the menu
       * @param pRefresh Whether or not to refresh the screen.  Defaults to true.
       */
      virtual void setMenuHeight(int pNewHeight, bool pRefresh = true);

      /**
       * \brief Returns the name of the cxWidgets class ("cxSearchPanel").
       *
       * @return The name of the cxWidgets class ("cxSearchPanel")
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief Sets a function to be called when the enter key is pressed on
       * \brief the form.
       *
       * @param pFunction The function to call - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 The 1st parameter to the function (pointer)
       * @param p2 The 2nd parameter to the function (pointer)
       * @param p3 The 3rd parameter to the function (pointer)
       * @param p4 The 4th parameter to the function (pointer)
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setFormOnEnterFunction(funcPtr4 pFunction, void *p1, void *p2,
                                          void *p3, void *p4);

      /**
       * \brief Sets a function to be called when the enter key is pressed on
       * \brief the form.
       *
       * @param pFunction The function to call - Must have this signature:
       *  string func(void*, void*)
       * @param p1 The 1st parameter to the function (pointer)
       * @param p2 The 2nd parameter to the function (pointer)
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setFormOnEnterFunction(funcPtr2 pFunction, void *p1, void *p2);

      /**
       * \brief Sets a function to be called when the enter key is pressed on
       * \brief the form.
       *
       * @param pFunction The function to call - Must have this signature:
       *  string func()
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setFormOnEnterFunction(funcPtr0 pFunction);

      /**
       * \brief Removes the onEnter function from the form.
       */
      virtual void clearFormOnEnterFunction();

      /**
       * \brief Adds a function to the form to call when the user presses some
       * \brief key.  Note that functions added with this method take precedence
       * \brief over functions with 2 void parameters.
       *
       * @param pFunctionKey An int representing a function key
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running
       *  the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setFormKeyFunction(int pFunctionKey, funcPtr4 pFunction,
                                      void *p1, void *p2, void *p3, void *p4,
                                     bool pUseReturnVal = false,
                                     bool pExitAfterRun = false,
                                     bool pRunOnLeaveFunction = true);

      /**
       * \brief Adds a function to the form to call when the user presses some
       * \brief key.
       *
       * @param pFunctionKey An int representing a function key
       * @param pFunction The function to be run - Must have signature
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running the
       *  function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setFormKeyFunction(int pFunctionKey, funcPtr2 pFunction,
                                      void *p1, void *p2,
                                      bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true);

      /**
       * \brief Adds a function to the form to call when the user presses some
       * \brief key.
       *
       * @param pFunctionKey An int representing a function key
       * @param pFunction The function to be run - Must have signature
       *  string func()
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running the
       *  function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setFormKeyFunction(int pFunctionKey, funcPtr0 pFunction,
                                      bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true);

      /**
       * \brief Removes a hotkey from the form so that it will not
       * \brief fire a function.
       *
       * @param pFunctionKey The hotkey to remove
       */
      virtual void clearFormKeyFunction(int pFunctionKey);

      /**
       * \brief Clears the form's list of external functions fired by hotkeys.
       */
      virtual void clearFormKeyFunctions();

      /**
       * \brief Returns whether the form has a function hotkey set up to
       * \brief run a function.
       *
       * @param pFunctionKey The key to look for
       *
       * @return Whether or not the form has pFunctionKey set up to
       *  run a function.
       */
      virtual bool formHasKeyFunction(int pFunctionKey) const;

      /**
       * \brief Adds a function to the menu to call when the user presses some
       * \brief key.  Note that functions added with this method take precedence
       * \brief over functions with 2 void parameters.
       *
       * @param pFunctionKey An int representing a function key
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxMenu, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the menu after running
       *  the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setMenuKeyFunction(int pFunctionKey, funcPtr4 pFunction,
                                      void *p1, void *p2, void *p3, void *p4,
                                     bool pUseReturnVal = false,
                                     bool pExitAfterRun = false,
                                     bool pRunOnLeaveFunction = true);

      /**
       * \brief Adds a function to the menu to call when the user presses some
       * \brief key.
       *
       * @param pFunctionKey An int representing a function key
       * @param pFunction The function to be run - Must have signature
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxMenu, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the menu after running the
       *  function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setMenuKeyFunction(int pFunctionKey, funcPtr2 pFunction,
                                      void *p1, void *p2,
                                      bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true);

      /**
       * \brief Adds a function to the menu to call when the user presses some
       * \brief key.
       *
       * @param pFunctionKey An int representing a function key
       * @param pFunction The function to be run - Must have signature
       *  string func()
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxMenu, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the menu after running the
       *  function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setMenuKeyFunction(int pFunctionKey, funcPtr0 pFunction,
                                      bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true);

      /**
       * \brief Removes a hotkey from the menu so that it will not
       * \brief fire a function.
       *
       * @param pFunctionKey The hotkey to remove
       */
      virtual void clearMenuKeyFunction(int pFunctionKey);

      /**
       * \brief Clears the menu's list of external functions fired by hotkeys.
       */
      virtual void clearMenuKeyFunctions();

      /**
       * \brief Returns whether the menu has a function hotkey set up to
       * \brief run a function.
       *
       * @param pFunctionKey The key to look for
       *
       * @return Whether or not the menu has pFunctionKey set up to
       *  run a function.
       */
      virtual bool menuHasKeyFunction(int pFunctionKey) const;

      /**
       * \brief Sets a function to run when the user selects an item from the
       * \brief menu.
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
      virtual bool setMenuOnSelectItemFunction(funcPtr4 pFunction, void *p1,
                                               void *p2, void *p3, void *p4,
                                               bool pExitAfterRun,
                                               bool pRunOnLeaveFunction);

      /**
       * \brief Sets a function to run when the user selects an item from the
       * \brief menu.
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
      virtual bool setMenuOnSelectItemFunction(funcPtr2 pFunction, void *p1,
                                               void *p2, bool pExitAfterRun,
                                               bool pRunOnLeaveFunction);

      /**
       * \brief Sets a function to run when the user selects an item from the
       * \brief menu.
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
      virtual bool setMenuOnSelectItemFunction(funcPtr0 pFunction,
                                               bool pExitAfterRun,
                                               bool pRunOnLeaveFunction);

      /**
       * \brief Returns whether or not an item was selected from the menu.
       *
       * @return Whether or not an item was selected from the menu
       */
      virtual bool itemWasSelected() const;

      /**
       * \brief Returns text of the item selected from the menu, if there was
       * \brief one.  If not, this will return a blank string.
       *
       * @param pDisplayText If true (default), this will return the text
       *  displayed on the menu for the item.  If false, this will return
       *  the alternate text for the item.
       *
       * @return The text of the item selected from the menu
       */
      virtual std::string getSelectedItem(bool pDisplayText = true) const;

      /**
       * \brief Makes the form be the current window
       *
       * @return true if it succeeded, or false if not
       */
      virtual bool setFormCurrent();

      /**
       * \brief Makes the menu be the current window
       *
       * @return true if it succeeded, or false if not
       */
      virtual bool setMenuCurrent();

   protected:
      /**
       * \brief Makes a copy of another cxSearchPanel
       *
       * @param pThatPanel A pointer to another cxSearchPanel to be copied
       */
      void copyCxSearchPanelStuff(const cxSearchPanel* pThatPanel);

   private:
      std::shared_ptr<cxForm> mForm; // Pointer to the form on the panel
      std::shared_ptr<cxMenu> mMenu; // Pointer to the menu on the panel

      // This function is used when copying another cxSearchPanel.  It checks
      //  event function pointers (onFocus, onLeave, mFormOnEnterFunction) to
      //  see if any of their parameters point to another cxSearchPanel or its
      //  form or menu, and if so, has them point to this one or mForm or
      //  mMenu.
      //
      // Parameters:
      //  pPanel: Another cxSearchPanel being copied.  Its event function
      //   parameters will be checked when a cxSearchPanel is being copied.
      void checkEventFunctionPointers(const cxSearchPanel& pPanel);

      // Don't allow assignment with = (this causes segfaults)
      cxSearchPanel& operator =(const cxSearchPanel& pPanel);

      // This is an event function for when the user presses the up arrow or
      //  shift-tab in the cxForm.  If they are on the first input, it will go
      //  to the last input on the form.  Otherise, it will go to the previous
      //  input as usual.
      static std::string formReverseNavigation(void *theSearchPanel, void *unused);

      // This is an event function for when the user presses the ESCAPE in
      //  the cxMenu.  It will clear the form fields, the menu items, and
      //  apply focus to the first field on the form.
      static std::string menuOnESC(void *theSearchPanel, void *unused);

      // This is an event function for when the user selects an item in the
      //  menu.
      static std::string menuOnSelect(void *theSearchPanel, void *unused);
};

#endif
