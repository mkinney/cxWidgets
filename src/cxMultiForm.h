#ifndef __CXMULTIFORM_H__
#define __CXMULTIFORM_H__

// Copyright (c) 2005-2007 Michael H. Kinney
//
// This class represents a form that can contain
//  subforms (forms within the form).
//
// TODO: When the user presses ESC on one of
//  the subforms, if the subforms have any
//  onLeave functions, it will still run them..
//  ESC should quit directly out of the whole
//  multiForm.
//
// TODO: Add a way of telling the multiForm to
//  do subforms first on the first pass through
//  the input loop.  (Would have to add a private
//  boolean variable, mDoFormsFirst, and then
//  another boolean in doInputLoop(), firstPass,
//  and for each pass in the input loop, have it
//  do the inputs if firstPass is false or
//  mDoFormsFirst is false.)

#include "cxForm.h"
#include "cxFunction.h"
#include <string>
#include <vector>
#include <memory>

/** \class cxMultiForm
 * \brief Represents a form that can
 * \brief contain subforms (forms within the
 * \brief form).
 * Note that this class uses dynamic memory when
 * subforms are added and stores the pointers to
 * the subforms in member data.  getForm()
 * can be used to return a pointer to one of the
 * subforms so that you can perform operations on
 * one of the subforms; however, you should never
 * use the 'delete' operator on one of the pointers
 * returned by getForm() because the destructor
 * of this class takes care of freeing the memory
 * used by the subforms.
 */
class cxMultiForm : public cxForm
{
   public:
      /**
       * Default constructor.  All parameters have default values.
       * @param pParentWindow Pointer to parent window; default nullptr
       * @param pRow Y location of form; default 0
       * @param pCol X location of form; default 0
       * @param pHeight Height of form, in lines; defaults to 24
       * @param pWidth Width of form; defaults to 80
       * @param pTitle Title of form; defaults to empty string
       * @param pBorderStyle The type of border to use - can be
       *  eBS_SINGLE_LINE for a single-line border or eBS_NOBORDER
       *  for no border.
       * @param pExtTitleWindow A pointer to another cxWindow in which to
       *  display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow in which to
       *  display the status.
       * @param pAutoExit Whether or not to exit the form when the user
       *  tries to go to the next input from the last input. Defaults to
       *  false.
       * @param pStacked If true, the form will behave assuming each
       *  input is on its own line.
       */
      explicit cxMultiForm(cxWindow *pParentWindow = nullptr, int pRow = 0,
                         int pCol = 0, int pHeight = DEFAULT_HEIGHT,
                         int pWidth = DEFAULT_WIDTH, const std::string& pTitle = "",
                         eBorderStyle pBorderStyle = eBS_SINGLE_LINE,
                         cxWindow *pExtTitleWindow = nullptr,
                         cxWindow *pExtStatusWindow = nullptr,
                         bool pAutoExit = false, bool pStacked = false);

      /**
       * Copy constructor
       * @param pThatMultiForm Another cxForm object to be copied
       */
      cxMultiForm(const cxMultiForm& pThatMultiForm);

      virtual ~cxMultiForm();

      /**
       * \brief Assignment operator
       *
       * @param pThatMultiForm Another cxMultiForm to be copied
       *
       * @return
       */
      cxMultiForm& operator =(const cxMultiForm& pThatMultiForm);

      /**
       * \brief Appends a subform to the form.
       *
       * @param pRow The row of the subform (relative to the parent form)
       * @param pCol The column of the subform (relative to the parent form)
       * @param pHeight The height of the subform
       * @param pWidth The width of the subform
       * @param pBorderStyle The type of border for the subform (defaults
       *  to eBS_NOBORDER) - See cxBorderStyles.h for border types
       * @param pTitle The title for the subform (defaults to blank) - Note
       *  that this will only appear if the subform has a border.
       * @param pStacked Whether or not the subform should be in 'stacked'
       *  mode (with each input above the next).
       *
       * @return A pointer to the subform
       */
      virtual std::shared_ptr<cxForm> appendForm(int pRow, int pCol, int pHeight,
                          int pWidth, const std::string& pTitle = "",
                          eBorderStyle pBorderStyle = eBS_NOBORDER,
                          bool pStacked = false);

      /**
       * \brief Appends a subform to the form via a pointer.
       * \brief The pointer that is passed in <b>must point to a
       * \brief dynamically-created cxForm object created with
       * \brief the 'new' operator</b>, because cxMultiForm cleans
       * \brief up its subform pointers in the destructor with
       * \brief the 'delete' operator.
       *
       * @param pForm A pointer to a cxForm (or a deriving object)
       * @param pRow The row on the form where you want the subform
       *  to be located
       * @param pCol The column on the form where you want the subform
       *  to be located
       * @param pMoved If non-null, the boolean pointed to by this
       *  variable will store whether or not the subform was able
       *  to be moved.
       *
       * @return Whether or not the subform got appended (true/false)
       */
      virtual bool appendForm(std::shared_ptr<cxForm>& pForm, int pRow, int pCol, bool* pMoved = nullptr);

      /**
       * \brief Appends a subform to the form via a pointer (without
       * \brief moving it).  The pointer that is passed in <b>must
       * \brief point to a dynamically-created cxForm object created
       * \brief with the 'new' operator</b>, because cxMultiForm
       * \brief cleans up its subform pointers in the destructor with
       * \brief the 'delete' operator.
       *
       * @param pForm A pointer to a cxForm (or a deriving object)
       *
       * @return Whether or not the subform got appended (true/false)
       */
      virtual bool appendForm(std::shared_ptr<cxForm>& pForm);

      /**
       * \brief Returns a pointer to one of the subforms (by index), or
       * \brief nullptr if the index is out of bounds.
       *
       * @param pIndex The index of the subform
       *
       * @return A pointer to the subform, or nullptr if pIndex is out of bounds.
       *  The return value should always be checked against nullptr before it's
       *  used.
       */
      virtual std::shared_ptr<cxForm> getForm(unsigned pIndex) const;

      /**
       * \brief Returns a pointer to one of the subforms (by title), or
       * \brief nullptr if there is no subform with the given title.
       *
       * @param pTitle The title of the subform
       *
       * @return A pointer to the subform, or nullptr if there is no subform with
       *  the given title.  The return value should always be checked against
       *  nullptr before it's used.
       */
      virtual std::shared_ptr<cxForm> getForm(const std::string& pTitle) const;

      /**
       * Shows the form
       * @param pBringToTop Whether or not to bring this window to the top.  Defaults to
       *  false.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults to true.
       *  Note that the subforms are considered subwindows, so if this parameter is
       *  false and the first parameter is true, the subforms won't show.
       * @return The return value of cxForm::show()
       */
      virtual long show(bool pBringToTop = false, bool pShowSubwindows = true) override;

      /**
       * \brief Shows the form and waits for input.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring this window to the top.  Defaults to
       *  false.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults to true.
       *  Note that the subforms are considered subwindows, so if this parameter is
       *  false and the first parameter is true, the subforms won't show.
       *
       * @return A return code based on user input; see the eReturnCodes enumeration in
       *  cxReturnCodes.h
        */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = false,
                             bool pShowSubwindows = true) override;

      /**
       * \brief Shows (refreshes) all inputs
       *
       * @param pBringToTop Whether or not to bring the inputs to the top (defaults
       *  to true)
       * @param pSkipCurrentForm Whether or not to skip the current form (defaults
       *  to false)
       */
      virtual void showAllSubforms(bool pBringToTop = true, bool pSkipCurrentForm = false) const;

      /**
       * \brief Removes a subform (by index).
       *
       * @param pIndex The index of the subform
       *
       * @return True if the remove succeeded, or false if not (i.e., if
       *  pIndex is out of bounds).
       */
      virtual bool removeSubform(unsigned pIndex);

      /**
       * \brief Removes a subform (by title).
       *
       * @param pTitle The title of the subform
       *
       * @return True if the remove succeeded, or false if not (i.e., if
       *  there were no subforms with the given title).
       */
      virtual bool removeSubform(const std::string& pTitle);

      /**
       * \brief Gets the value of an input on one of the subforms (by indexes)
       *
       * @param pFormIndex The subform index
       * @param pInputIndex The index of the input on the subform
       *
       * @return The value stored in the input, or a blank string if
       *  the indexes are out of bounds.
       */
      virtual std::string getValue(unsigned pFormIndex, int pInputIndex) const;

      /**
       * \brief Returns the value associated with a particular input (by index).
       * \brief Overridden from cxForm to prevent hiding.
       */
      virtual std::string getValue(int pIndex) const override;

      /**
       * \brief Returns the value associated with a particular input (by label or name).
       * \brief Overridden from cxForm to prevent hiding.
       */
      virtual std::string getValue(const std::string& pStr, bool pIsLabel = true) const override;

      /**
       * \brief Gets the value of an input on one of the subforms (by
       * \brief index & label/name)
       *
       * @param pFormIndex The subform index
       * @param pLabel The label/name of the input on the subform
       * @param pIsLabel If true (default), pLabel specifies the label of
       *  the input on the form.  If false, pLabel specifies the name of
       *  the input.
       *
       * @return The value stored in the input, or a blank string if
       *  pFormIndex is out of bounds or there is no input on the subform
       *  with the given label/name.
       */
      virtual std::string getValue(unsigned pFormIndex, const std::string& pLabel,
                              bool pIsLabel = true) const;

      /**
       * \brief Gets the value of an input on one of the subforms (by
       * \brief title & input index)
       *
       * @param pTitle The title of the subform
       * @param pInputIndex The index of the input on the subform
       *
       * @return The value stored in the input, or a blank string
       *  if there is no subform with the given title or pInputIndex
       *  is out of bounds.
       */
      virtual std::string getValue(const std::string& pTitle, int pInputIndex) const;

      /**
       * \brief Gets the value of an input on one of the subforms (by
       * \brief title & label/name).
       *
       * @param pTitle The title of the subform
       * @param pLabel The label/name of the input on the subfrom
       * @param pIsLabel If true (default), pLabel specifies the label of the input.
       *   If false, pLabel specifies the name of the input.
       *
       * @return The value stored in the input, or a blank string if
       *  there is no subform with the given title or if there is no
       *  input on the subform with the given label/name.
       */
      virtual std::string getValue(const std::string& pTitle, const std::string& pLabel,
                              bool pIsLabel = true) const;

      /**
       * \brief Sets a value of an input on one of the subforms.
       *
       * @param pFormIndex The index of the subform
       * @param pInputIndex The index of the input on the subform
       * @param pValue The value to set in the input
       * @param pRefresh Whether or not to refresh the input after
       *  the value is set
       *
       * @return true if successful or false if not
       */
      virtual bool setValue(unsigned pFormIndex, int pInputIndex, const std::string& pValue,
                            bool pRefresh = false);


      /**
       * \brief Sets a value of an input on one of the subforms.
       *
       * @param pFormIndex The index of the subform
       * @param pLabel The label/name of the input on the subform
       * @param pValue The value to set in the input
       * @param pIsLabel If true (default), pLabel specifies the label of the input.
       *   If false, pLabel specifies the name of the input.
       * @param pRefresh Whether or not to refresh the input (defaults to false)
       *
       * @return true if successful or false if not
       */
      virtual bool setValue(unsigned pFormIndex, const std::string& pLabel,
                            const std::string& pValue, bool pIsLabel = true,
                            bool pRefresh = false);

      /**
       * \brief Sets a value of an input on one of the subforms.
       *
       * @param pTitle The title of the subform
       * @param pInputIndex The index of the input on the subform
       * @param pValue The value to set in the input
       * @param pRefresh Whether or not to refresh the input after
       *  the value is set
       *
       * @return true if successful or false if not
       */
      virtual bool setValue(const std::string& pTitle, int pInputIndex,
                            const std::string& pValue, bool pRefresh = false);

      /**
       * \brief Sets a value of an input on one of the subforms.
       *
       * @param pTitle The title of the subform
       * @param pLabel The label/name of the input on the subform
       * @param pValue The value to set in the input
       * @param pIsLabel If true (default), pLabel specifies the label of the input.
       *   If false, pLabel specifies the name of the input.
       * @param pRefresh Whether or not to refresh the input (defaults to false)
       *
       * @return true if successful or false if not
       */
      virtual bool setValue(const std::string& pTitle, const std::string& pLabel,
                            const std::string& pValue, bool pIsLabel = true,
                            bool pRefresh = false);

      /**
       * Sets the value of one of the inputs on the form (by index).
       * @param pIndex The index of the input
       * @param pValue The value to be placed at that index.
       * @param pRefresh Whether or not to refresh the input (defaults to false)
       * @return True if successful or false if not
       */
      virtual bool setValue(int pIndex, const std::string& pValue, bool pRefresh = false) override;

      /**
       * Sets the value of one of the inputs on the form (by label/name).
       * @param pLabel The label of the input
       * @param pValue The new string value for the input
       * @param pIsLabel If true (default), pLabel specifies the label of the input.
       *   If false, pLabel specifies the name of the input.
       * @param pRefresh Whether or not to refresh the input (defaults to false)
       * @return True if successful or false if not
       */
      virtual bool setValue(const std::string& pLabel, const std::string& pValue, bool pIsLabel = true, bool pRefresh = false) override;

      /**
       * \brief Returns the number of subforms on the form.
       *
       * @return The number of subforms on the form
       */
      size_t numSubforms() const;

      /**
       * \brief Returns the last key typed by the user.
       *
       * @return The last key typed by the user
       */
      virtual int getLastKey() const;

      /**
       * \brief Returns whether any of the subforms contain
       * \brief inputs that are currently editable.
       *
       * @return Whether or not any of the subforms contain
       *  inputs that are currently editable.
       */
      bool hasEditableSubforms() const;

      /**
       * Changes the window's position (based on a new upper-left corner).  Also moves the input windows.
       * @param pNewRow The new topmost row of the form
       * @param pNewCol The new leftmost column of the form
       * @param pRefresh Whether or not to refrseh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

      /**
       * \brief Hides the form
       * @param pHideSubwindows Whether or not to also hide the subwindows (defaults
       *    to true)
       */
      virtual void hide(bool pHideSubwindows = true) override;

      /**
       * \brief Un-hides the form
       * @param pUnhideSubwindows Whether or not to also unhide the subwindows
       *    (defaults to true)
       */
      virtual void unhide(bool pUnhideSubwindows = true) override;

      /**
       * Adds a function to call when the user presses some key.
       * Note that the function key is expected to be F1-F10
       * (the setFocus() function only expects these keys for
       * running functions).
       * Note that functions added with this method take precedence over functions with 2 void parameters.
       * @param pKey An int representing a function key
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function        * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxForm will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr4 pFunction,
                                  void *p1, void *p2, void *p3, void *p4,
                                  bool pUseReturnVal = false,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true) override;

      /**
       * Adds a function to call when the user presses some key.
       * Note that the function key is expected to be F1-F10
       * (the setFocus() function only expects these keys for
       * running functions).
       * @param pKey An int representing a function key
       * @param pFunction The function to be run - Must have signature string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxForm will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr2 pFunction,
                                  void *p1, void *p2,
                                  bool pUseReturnVal = false,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true);

      /**
       * Adds a function to call when the user presses some key.
       * Note that the function key is expected to be F1-F10
       * (the setFocus() function only expects these keys for
       * running functions).
       * @param pKey An int representing a function key
       * @param pFunction The function to be run - Must have signature string func()
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxForm will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr0 pFunction,
                                  bool pUseReturnVal = false,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true);

      /**
       * \brief Changes which subform is to have the focus (by index).
       *
       * @param pIndex The index of the subform that should next get focus
       *
       * @return True if successful or false if not
       */
      virtual bool setCurrentSubform(int pIndex);

      /**
       * \brief Changes which subform is to have the focus (by title).
       *
       * @param pTitle The title of the subform that should next get focus
       *
       * @return True if successful or false if not
       */
      virtual bool setCurrentSubform(const std::string& pTitle);

      /**
       * \brief Changes which subform is to have the focus (by pointer).
       * \brief This has a different function name so as not to be
       * \brief ambiguous to the compiler.
       *
       * @param pForm A pointer to a cxForm (if it exists
       *  in the multiForm, it will then be the current window).
       *
       * @return True if successful or false if not
       */
      virtual bool setCurrentSubformByPtr(const std::shared_ptr<cxForm>& pForm);
      virtual bool setCurrentSubformByPtr(cxForm *pForm);

      /**
       * \brief Returns the index of the current form.
       *
       * @return The index of the current form
       */
      virtual int getCurrentForm() const;

      /**
       * \brief Returns the index of a subform on the
       * \brief form, given a cxForm pointer.
       *
       * @param pForm A pointer to a cxForm
       *
       * @return The index of the subform, or -1 if
       *  the given cxForm pointer isn't a subform on
       *  this form.
       */
      virtual int getSubformIndex(const std::shared_ptr<cxForm>& pForm) const;
      virtual int getSubformIndex(cxForm *pForm) const;

      /**
       * \brief Returns whether or not any inputs on the form,
       * \brief or any inputs on any of the subforms, have changed.
       *
       * @return true if the values of any inputs on the form or on
       *  any of the subforms have changed; returns false if nothing
       *  has changed.
       */
      virtual bool hasChanged() const override;

      /**
       * \brief Setter for whether or not data should be considered changed.
       *
       * @param pDataChanged Whether or not data should be considered changed
       *  (true/false)
       */
      virtual void setChanged(bool pDataChanged) override;

      /**
       * \brief Enables or disables one of the subforms (by index).
       *
       * @param pIndex The index of the subform
       * @param pEnabled Whether the subform should be enabled or not (true/false)
       */
      virtual void setSubformEnabled(unsigned pIndex, bool pEnabled);

      /**
       * \brief Enables or disables one of the subforms (by title).
       *
       * @param pTitle The title of the subform
       * @param pEnabled Whether the subform should be enabled or not (true/false)
       */
      virtual void setSubformEnabled(const std::string& pTitle, bool pEnabled);

      /**
       * \brief Returns whether one of the subforms is
       * \brief enabled (by index).
       *
       * @param pIndex The index of the subform
       *
       * @return Whether the subform is enabled or not (true/false).
       *  Returns false if pIndex is out of bounds.
       */
      virtual bool subformIsEnabled(unsigned pIndex) const;

      /**
       * \brief Returns whether one of the subforms is
       * \brief enabled (by title).
       *
       * @param pTitle The title of the subform
       *
       * @return Whether the subform is enabled or not (true/false).
       *  Returns false if there is no subform with the given title.
       */
      virtual bool subformIsEnabled(const std::string& pTitle) const;

      /**
       * \brief Adds a key that will cause the form to quit and
       * \brief return cxID_QUIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave functions
       *  when the multiForm exits.  Defaults to true.  Note that this affects
       *  both the onLeave function for the multiForm, as well as the current
       *  subform when the multiForm is modal.
       * @param pOverride If true, then the key will be added as an exit key
       *  regardless if the key exists as a function key.  This defaults to
       *  false.
       *
       * @return Whether or not the key was added
       */
      virtual bool addQuitKey(int pKey, bool pRunOnLeaveFunction = true,
                              bool pOverride = false) override;

      /**
       * \brief Removes a quit key (but doesn't work for ESC)
       *
       * @param pKey The key to remove from the list of quit keys
       */
      virtual void removeQuitKey(int pKey) override;

      /**
       * \brief Adds a key that will cause the form to quit and
       * \brief return cxID_EXIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave functions
       *  when the multiForm exits.  Defaults to true.  Note that this affects
       *  both the onLeave function for the multiForm, as well as the current
       *  subform when the multiForm is modal.
       * @param pOverride If true, then the key will be added as an exit key
       *  regardless if the key exists as a function key.  This defaults to
       *  false.
       *
       * @return Whether or not the key was added
       */
      virtual bool addExitKey(int pKey, bool pRunOnLeaveFunction = true,
                              bool pOverride = false) override;

      /**
       * \brief Removes an exit key
       *
       * @param pKey The key to remove from the list of exit keys
       */
      virtual void removeExitKey(int pKey) override;

      /**
       * \brief Sets whether the window should disable the cursor
       * \brief when it's shown (this is the default behavior).
       *
       * @param pDisableCursorOnShow Whether or not to disable
       *  the cursor when shown
       */
      virtual void setDisableCursorOnShow(bool pDisableCursorOnShow) override;

      /**
       * \brief Clears the multiForm and all the subforms.
       *
       * @param pRefresh Whether or not to refresh the multiForm and the
       *  subforms as they are cleared (defaults to false).
       */
      virtual void clear(bool pRefresh = false);

      /**
       * \brief Normally, if the cursor is on the last form when a cxMultiForm
       * \brief gains focus, the cursor will be set to the last input on that
       * \brief form, assuming the user is moving backwards through the
       * \brief subforms.  This method can turn that off.
       *
       * @param pAssumeMovingBackwards Whether or not to assume the user is
       *  moving backwards and to set the cursor on the last input of the
       *  last subform.
       */
      void setAssumeMovingBackwards(bool pAssumeMovingBackwards);

      /**
       * \brief Returns whether it will be assumed that the user is moving
       * \brief backwards through the subforms when the cxMultiForm gains
       * \brief focus (see setAssumeMovingBackwards())
       *
       * @return Whether or not to assume the user is moving backwards and to
       *  set the cursor on the last input of the last subform.
       */
      bool getAssumeMovingBackwards() const;

      /**
       * \brief Enables or disables the multiForm.  Enabling/disabling a
       * \brief cxMultiForm consequently also enables/disables all the inputs
       * \brief and subforms.
       *
       * @param pEnabled Whether the multiForm should be enabled or not (true/false)
       */
      virtual void setEnabled(bool pEnabled) override;

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

   protected:
      /**
       * \brief Makes a copy of a cxMultiForm's member variables
       *
       * @param pThatMultiForm A pointer to another cxMultiForm whose members to copy
       */
      void copyCxMultiFormStuff(const cxMultiForm* pThatMultiForm);

      /**
       * \brief Handles the input loop.  Returns cxID_QUIT or cxID_EXIT,
       * \brief depending on the user's input.  This function is meant
       * \brief not to be virtual, since it is specifically for
       * \brief cxMultiForm.
       *
       * @param  pShowSubforms Whether or not to have the subforms show themselves
       *  before running their input loops.
       *
       * @return A return code (cxID_QUIT or cxID_EXIT, depending on the
       *  user's input).
       */
      long doInputLoop(bool pShowSubforms);

      /**
       * \brief Returns the lowest row of any subform on the form, starting at
       * \brief a given index.  Returns -1 if there are no subforms or if
       * \brief the given index is out of bounds.
       *
       * @param pIndex The index at which to start.  Defaults to 0.
       *
       * @return The lowest row (relative to the form) of the subforms, or
       *  -1 if there are no inputs or pIndex is out of bounds.
       */
      int lowestSubformRow(unsigned pIndex = 0) const;

   private:
      typedef std::vector<std::shared_ptr<cxForm> > formPtrContainer;
      formPtrContainer mForms; // Contains pointers to the subforms
      int mCurrentForm = 0;    // The index of the subform that currently has focus
      bool mCycleForm = true;  // Whether or not to cycle to the next or
                               // previous form in the input loop (this
                               // is set to false when setCurrentSubform()
                               // is called)
      // If this is set true, then when focus is set to the multiForm and the
      //  cursor is on the last form, then the cursor on the last form will be
      //  set to the last input on that form (motion is assumed to be moving
      //  backwards).
      bool mAssumeMovingBackwards = true;

      // Frees the memory used by the subforms and clears
      //  mForms.
      inline void freeSubforms();

      // Runs an input loop for the inputs.  Also sets a boolean for
      //  whether or not to continue the input loop.
      long doInputs(bool& pContinueOn);

      // Runs an input loop for the subforms.
      // Parameters:
      //  pShowSubforms: Whether or not to have the subforms show themselves
      //   before running their input loops.
      //  pContinueOn: This will store a boolean for whether or not to
      //   continue the input loop.
      //  pClickedInput: This will store a boolean for whether or not the user
      //   clicked on an input in the multiForm.  This is used by the input
      //   loop to determine whether to go back to looping through the inputs.
      long doSubforms(bool pShowSubforms, bool& pContinueOn, bool& pClickedInput);

      // Selects the next form with editable inputs.
      //  Returns whether or not the current form was
      //  the last form in mForms.
      bool selectNextForm();

      // Selects the previous form with editable inputs.
      //  Returns whether or not the current form was
      //  the first form in mForms.
      bool selectPrevForm();

      // Adds this multiForm's function keys to a subform.
      void addFormFunctionKeysToSubform(std::shared_ptr<cxForm>& pForm);

      // Adds this multiForm's quit keys and exit keys to a subform.
      void addQuitAndExitKeysToSubform(std::shared_ptr<cxForm>& pForm);

      // Returns whether a subform is enabled and editable (by index).
      //  Note that no bounds checking is done (for optimal
      //  efficiency).
      inline bool subformIsEnabledAndEditable(unsigned int pIndex);
};

#endif
