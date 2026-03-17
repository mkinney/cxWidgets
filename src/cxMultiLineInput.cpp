// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxMultiLineInput.h"
#include "cxBase.h"
#include "cxForm.h"
#include "cxStringUtils.h"
#include "cxValidators.h"
#include <cstdlib>
using std::string;
using std::vector;
using std::make_pair;
using std::pair;
using std::set;
using std::map;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using cxStringUtils::stringToLong;
using cxStringUtils::stringToLongDouble;
using cxStringUtils::toString;
using cxStringUtils::Find;
using cxValidators::isValidFloatingPtNumStr;
using cxValidators::isValidWholeNumStr;

// TODO: For a single-character input, it looks like the onLeave function runs
//  even when the focus hasn't left the input.

cxMultiLineInput::cxMultiLineInput(cxWindow *pParentWindow, int pRow,
                 int pCol, int pHeight, int pWidth, const string& pLabel,
                 eBorderStyle pBorderStyle, eInputOptions pInputOption,
                 eInputTypes pInputType, string *pExtValue,
                 int pRightLabelOffset, int pRightLabelHeight,
                 int pRightLabelWidth, bool pShowRightLabel)
   : cxWindow(pParentWindow, pRow, pCol, (pHeight > 0) ? pHeight : 1, pWidth,
              "", "", "", pBorderStyle, nullptr, nullptr),
     mExtValue(pExtValue),
     mInputType(pInputType),
     mExtendedHelpColor(cxBase::getDefaultMessageColor()),
     /*
     mRightLabel(nullptr, (hasBorder() ? pRow + 1 : pRow),
                 right() + 1 + pRightLabelOffset, pRightLabelHeight,
                 pRightLabelWidth, "", "", "", eBS_NOBORDER, nullptr, nullptr, false),
     */
     mRightLabelOffset(pRightLabelOffset),
     mShowRightLabel(pShowRightLabel)
{
   mRightLabel = make_unique<cxWindow>(nullptr, (hasBorder() ? pRow + 1 : pRow),
                                       right() + 1 + pRightLabelOffset, pRightLabelHeight,
                                       pRightLabelWidth, "", "", "", eBS_NOBORDER, nullptr, nullptr, false);

   // The height must be at least 3 (with borders) or 1 (w/o borders)
   //  to add single-line inputs.
   // If there's a border, then the number of single-line inputs
   //  = height()-2.  Otherwise, the number of single-line inputs
   //  = height().
   if (getBorderStyle() != eBS_NOBORDER)
   {
      // Adjust pHeight if it's not valid
      if (pHeight < 3)
      {
         pHeight = 3;
      }

      int row = pRow+1; // Row # for the single-line inputs
      // Create the first input line (the label will be displayed
      //  with this one)
      shared_ptr<cxInput> input = make_shared<cxInput>(nullptr, row, pCol+1, pWidth-2, pLabel,
                                                       eBS_NOBORDER, pInputOption, true, nullptr);
      input->mParentMLInput = this;
      // The input should exit when a function key is pressed so that
      //  this multi-line input can catch it.
      input->setExitOnFKey(true);
      mInputs.push_back(input);

      // Create the rest of the inputs (with blank labels)
      for (row = pRow+2; row < pRow+pHeight-1; ++row)
      {
         input = make_shared<cxInput>(nullptr, row, pCol+1, pWidth-2, "", eBS_NOBORDER, pInputOption, true, nullptr);
         input->mParentMLInput = this;
         // The input should exit when a function key is pressed so that
         //  this multi-line input can catch it.
         input->setExitOnFKey(true);
         // The input should exit when the user presses backspace at the first
         //  input position so that this multi-line input can wrap back
         //  around to the previous input.
         input->setExitOnBackspaceAtFront(true);
         mInputs.push_back(input);
      }
   }
   else
   {
      // No border
      // Adjust pHeight if it's not valid
      if (pHeight < 1)
      {
         pHeight = 1;
      }

      int row = pRow; // Row # for the single-line inputs
      // Create the first input line (the label will be displayed
      //  with this one)
      shared_ptr<cxInput> input = make_shared<cxInput>(nullptr, row, pCol, pWidth, pLabel, eBS_NOBORDER, pInputOption, true, nullptr);
      input->mParentMLInput = this;
      input->setExitOnFKey(true);
      mInputs.push_back(input);

      // Create the rest of the inputs (with blank labels)
      for (row = pRow+1; row < pRow+pHeight; ++row)
      {
         input = make_shared<cxInput>(nullptr, row, pCol, pWidth, "", eBS_NOBORDER, pInputOption, true, nullptr);
         input->mParentMLInput = this;
         input->setExitOnFKey(true);
         // The input should exit when the user presses backspace at the first
         //  input position so that this multi-line input can wrap back
         //  around to the previous input.
         input->setExitOnBackspaceAtFront(true);
         mInputs.push_back(input);
      }
   }

   // If mExtValue is not nullptr, then update the value of
   //  this input.
   if (mExtValue != nullptr)
   {
      setValue(*mExtValue);
   }

   // If there are borders and the height is < 3, then resize the window
   //  with a height of 3.
   if ((getBorderStyle() != eBS_NOBORDER) && (height() < 3))
   {
      cxWindow::resize(3, width(), false);
   }

   // Don't draw the message text - This class keeps its own member for
   //  the text drawn in the window.
   toggleMessage(false);

   // Set up the default navigation keys
   mNavKeys.insert(PAGE_UP);
   mNavKeys.insert(PAGE_UP);
   mNavKeys.insert(PAGE_DOWN);
   mNavKeys.insert(TAB);
   mNavKeys.insert(SHIFT_TAB);
   mNavKeys.insert(KEY_DOWN);
   mNavKeys.insert(KEY_UP);
   mNavKeys.insert(KEY_LEFT);
   mNavKeys.insert(KEY_RIGHT);

   // Set up the built-in validator and onKeypress functions for the input
   setValidatorFunction(cxMultiLineInput::inputValidatorStatic, this, nullptr);
   setOnKeyFunction(cxMultiLineInput::onKeypressStatic, this, nullptr);
   toggleOnKeyFunction(true);

   // Enable/disable the right label window
   mRightLabel->setEnabled(pShowRightLabel);

   // Set up F1 as an extended help key by default
   addExtendedHelpKey(KEY_F(1));
} // Constructor

// Copy constructor
cxMultiLineInput::cxMultiLineInput(const cxMultiLineInput& pThatInput)
   : cxWindow(nullptr, pThatInput.top(), pThatInput.left(), pThatInput.height(),
     pThatInput.width(), pThatInput.getTitle(), pThatInput.getLabel(), pThatInput.getStatus(),
     pThatInput.getBorderStyle(), nullptr, nullptr),
     mRunFocusFunctions(pThatInput.mRunFocusFunctions),
     mExtValue(pThatInput.mExtValue),
     mCurrentInputLine(0),
     mEnterAlwaysExits(pThatInput.mEnterAlwaysExits),
     mReadOnlyOnLeave(pThatInput.mReadOnlyOnLeave),
     mExitOnFull(pThatInput.mExitOnFull),
     mDoInputLoop(pThatInput.mDoInputLoop),
     mSkipIfReadOnly(pThatInput.mSkipIfReadOnly),
     mRunValidatorFunction(pThatInput.mRunValidatorFunction),
     mUseClearKey(pThatInput.mUseClearKey),
     mValidateOnReverse(pThatInput.mValidateOnReverse),
     mApplyAttrDefaults(pThatInput.mApplyAttrDefaults),
     mSkipValidatorKeys(pThatInput.mSkipValidatorKeys),
     mNavKeys(pThatInput.mNavKeys),
     mInputType(pThatInput.mInputType),
     mValidOptionStrings(pThatInput.mValidOptionStrings),
     mAutoGenerateExtendedHelp(pThatInput.mAutoGenerateExtendedHelp),
     mRangeLowDouble(pThatInput.mRangeLowDouble),
     mRangeHighDouble(pThatInput.mRangeHighDouble),
     mRangeLowInt(pThatInput.mRangeLowInt),
     mRangeHighInt(pThatInput.mRangeHighInt),
     mValidatorFuncMessageBox(pThatInput.mValidatorFuncMessageBox),
     mAllowBlank(pThatInput.mAllowBlank),
     mExtendedHelp(pThatInput.mExtendedHelp),
     mExtendedHelpKeys(pThatInput.mExtendedHelpKeys),
     mUseExtendedHelpKeys(pThatInput.mUseExtendedHelpKeys),
     //mRightLabel(pThatInput.mRightLabel),
     mRightLabelOffset(pThatInput.mRightLabelOffset),
     mShowRightLabel(pThatInput.mShowRightLabel),
     mMaxInputLength(pThatInput.mMaxInputLength),
     mErrorState(pThatInput.mErrorState)
{
   mRightLabel = make_unique<cxWindow>(*pThatInput.mRightLabel);

   // Copy the other input's single-line inputs
   shared_ptr<cxInput> myInput;
   for (const auto& input : pThatInput.mInputs)
   {
      myInput = make_shared<cxInput>(*input);
      myInput->mParentMLInput = this;
      if (mUseExtendedHelpKeys)
      {
         for (int helpKey : mExtendedHelpKeys)
            myInput->addExitKey(helpKey, false, true);
         /*
         for (int helpKey : mExtendedHelpKey) {
            myInput->addExitKey(helpKey, false, true);
         }
         */
      }
      mInputs.push_back(myInput);
   }
   // Copy the other input's mValidatorFunction
   copyValidatorFunction(pThatInput);

   // If mExtValue is not nullptr, then update the value of
   //  this input.
   if (mExtValue != nullptr)
   {
      setValue(*mExtValue);
   }

   toggleMessage(pThatInput.messageWillDraw());

   mRightLabel->setEnabled(pThatInput.mRightLabel->isEnabled());
} // Copy constructor

cxMultiLineInput::~cxMultiLineInput()
{
   // Free the memory used by the cxInput pointers in mInputs
   freeInputs();
} // Destructor

cxMultiLineInput& cxMultiLineInput::operator =(const cxMultiLineInput& pThatInput)
{
   if (&pThatInput != this)
   {
      copyCxMultiLineInputStuff(&pThatInput);
   }

   return(*this);
} // operator =

long cxMultiLineInput::show(bool pBringToTop, bool pShowSubwindows)
{
   long returnVal = cxID_EXIT;
   // Only show the input if it's enabled.
   if (isEnabled())
   {
      // If the "external" user value pointer is not nullptr,
      //  update the input box value, just in case it
      //  has changed.
      if (mExtValue != nullptr)
      {
         setValue(*mExtValue);
      }

      // Show the subwindows now, if pShowSubwindows is true and
      //  mShowSelfBeforeSubwins is false.
      if (pShowSubwindows && !getShowSelfBeforeSubwins())
      {
         showSubwindows(pBringToTop, pShowSubwindows);
      }

      // If the window has a border, draw the border, title, & status.
      if (getBorderStyle() != eBS_NOBORDER)
      {
         drawBorder();
         drawTitle();
         drawStatus();

         if (pBringToTop)
         {
            bringToTop();
         }

         // Make sure the window isn't hidden
         unhide();
      }

      // Show all the single-line inputs
      for (const auto& input : mInputs)
      {
         returnVal = input->show(pBringToTop, false);
      }

      // Show the subwindows now, if pShowSubwindows is true and
      //  mShowSelfBeforeSubwins is true.
      if (pShowSubwindows && getShowSelfBeforeSubwins())
      {
         showSubwindows(pBringToTop, pShowSubwindows);
      }

      // Show the right label
      if (mShowRightLabel)
      {
         mRightLabel->show(pBringToTop, false);
      }
      else
      {
         mRightLabel->hide(false);
      }
   }
   else
   {
      hide(false);
      mRightLabel->hide(false);
   }

   return(returnVal);
} // show

long cxMultiLineInput::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   // Reset mRanFunctionAndShouldExit
   mRanFunctionAndShouldExit = false;

   setReturnCode(cxID_EXIT);
   // Only do this if the input window is active.
   if (isEnabled())
   {
      mIsModal = true;

      // If mAutoGenerateExtendedHelp is true and this is a text input, then
      //  generate mExtendedHelp with the valid strings and help text in
      //  mValidOptionStrings.  This is done here in case the user added,
      //  removed, or changed any of the valid inputs and/or their help strings
      //  since the last call to showModal().
      if (mAutoGenerateExtendedHelp && (mInputType == eINPUT_TYPE_TEXT))
      {
         generateExtendedHelp();
      }

      if (pBringToTop)
      {
         bringToTop(true);
      }

      // Run the onFocus function if we're set to run it.  If it returns true,
      //  that means we should exit.. so only do the input loop if it returns
      //  false or we aren't set to run the onFocus function.  The onFocus
      //  function should be run before the window is shown, in case its
      //  return value is to be set as the value of the input.  Also, check
      //  the value of getLeaveNow() in case the onFocus function called
      //  exitNow() or quitNow().
      bool continueOn = true;
      if (mRunFocusFunctions)
      {
         continueOn = !runOnFocusFunction() && !getLeaveNow();
      }

      // Show the window
      if (pShowSelf)
      {
         show(true, pShowSubwindows);
      }
      if (continueOn)
      {
         // Run the input loop if mDoInputLoop is true
         if (mDoInputLoop)
         {
            bool runOnLeaveFunc = true;
            // There should always be at least 1 single-line input,
            //  but check anyway, just in case..
            if (mInputs.size() > 0)
            {
               // If this input is read-only, don't do any
               //  input processing.  If mSkipIfReadOnly is
               //  false, wait for a keypress so that the user
               //  can see the input window.
               if (mInputs[0]->getInputOption() == eINPUT_READONLY)
               {
                  if (!mSkipIfReadOnly)
                  {
                     int lastKey = wgetch(mWindow);
                     setLastKey(lastKey);
                     if (lastKey == ESC)
                     {
                        setReturnCode(cxID_QUIT);
                     }
                     else
                     {
                        setReturnCode(cxID_EXIT);
                     }
                  }

                  return(getReturnCode());
               }

               // Start the input loop; have the single-line inputs show
               //  themselves if we didn't show this multi-line input.
               setReturnCode(doInputLoop(!pShowSelf, runOnLeaveFunc));
               int lastKey = getLastKey();
               // If mRunValidatorFunction is true, then we need to do
               //  validation.
               if (mRunValidatorFunction)
               {
                  bool doValidation = true;
                  // If the user clicked outside the input, as well as the
                  //  form the input is on (if the input is on a form), then
                  //  don't do validation.
                  if ((lastKeyWasMouseEvt() && !mouseEvtWasInWindow()))
                  {
                     if (mParentForm != nullptr)
                     {
                        if (!(mParentForm->pointIsInWindow(mMouse.y, mMouse.x)))
                        {
                           doValidation = false;
                        }
                     }
                     else
                     {
                        // mParentForm is nullptr..  just set doValidation to
                        //  false.
                        doValidation = false;
                     }
                  }
                  else
                  {
                     // If mValidateOnReverse is false and the user pressed a
                     //  key to navigate in reverse (i.e. shift-tab or the up
                     //  arrow), then don't run the validator function.
                     if (!mValidateOnReverse)
                     {
                        if ((lastKey == SHIFT_TAB) || (lastKey == KEY_UP))
                        {
                           doValidation = false;
                        }
                     }
                  }
                  if (doValidation && validatorFunctionIsSet())
                  {
                     // If the validator function is set, then keep running the
                     //  input loop while these conditions are true:
                     //  - The last key is not set up to skip the validator
                     //  - The last key is not set up in the parent form to
                     //    fire a function
                     //  - The validator function returns a non-blank string
                     // Note: When calling searchParentFormsForFKey(), we pass
                     //  a false so that it doesn't include navigational keys.
                     while (!hasSkipValidatorKey(lastKey) &&
                            !searchParentFormsForFKey(lastKey, false) &&
                            (runValidatorFunction()) != "")
                            {
                        // Since the input was invalid, blank it out.
                        setValue("", true);
                        // If the user decided to quit out of the input,
                        //  then break out of this loop.
                        if ((lastKey == ESC) || (getReturnCode() == cxID_QUIT))
                        {
                           break;
                        }
                        // Run the input loop again
                        setReturnCode(doInputLoop(!pShowSelf, runOnLeaveFunc));
                        lastKey = getLastKey();
                        // If mValidateOnReverse is false, then check for
                        //  reverse navigation keys (shift-tab & up arrow).
                        //  If the user pressed those keys, then stop doing
                        //  doing validation.
                        if (!mValidateOnReverse)
                        {
                           if ((lastKey == SHIFT_TAB) || (lastKey == KEY_UP))
                           {
                              break;
                           }
                        }
                     }
                  }
               }

               // Set mCurrentInputLine to the last line that has text on it
               //  (so that if someone calls showModal() again, the cursor will
               //  be where the user left off).
               int index = (int)mInputs.size() - 1;
               for (int j = 0; j < (int)mInputs.size(); ++j)
               {
                  if (mInputs[j]->getValue() == "")
                  {
                     index = j;
                     break;
                  }
               }
               --index; // To fix for off-by-one
               if (index >= 0)
               {
                  mCurrentInputLine = index;
               }
            }
            else
            {
               // There are no single-line inputs for some reason..  wait for a keypress.
               setLastKey(wgetch(mWindow));
            }

            // Do the rest of this stuff if the user didn't press a key
            //  that's in mQuitKeys or mExitKeys.
            if ((mQuitKeys.find(cxWindow::getLastKey()) == mQuitKeys.end()) &&
                (mExitKeys.find(cxWindow::getLastKey()) == mExitKeys.end()))
                {
               // If the user doesn't want to quit out of the input, run the
               //  "on leave" function (if it's set).
               if ((getReturnCode() != cxID_QUIT) && mRunFocusFunctions &&
                   runOnLeaveFunc)
                   {
                  runOnLeaveFunction();
               }
            }
         }
         else
         {
            // mDoInputLoop is false..  Just run the onLeave function.
            if (mRunFocusFunctions)
            {
               runOnLeaveFunction();
            }
         }

         // If mSetReadOnlyOnLeave is true, set the input read-only.
         if (mReadOnlyOnLeave)
         {
            setInputOption(eINPUT_READONLY);
         }

         // If the external value pointer is set, then update it.
         if (mExtValue != nullptr)
         {
            *mExtValue = getValue();
         }
      }

      mIsModal = false;
   }

   return(getReturnCode());
} // showModal

void cxMultiLineInput::hide(bool pHideSubwindows)
{
   // Hide the main window, as well as all the inputs.
   cxWindow::hide(false);

   for (const auto& input : mInputs)
   {
      input->hide(false);
   }
} // hide

void cxMultiLineInput::unhide(bool pUnhideSubwindows)
{
   // If the input is enabled, un-hide the main window,
   //  as well as all the inputs.
   if (isEnabled())
   {
      cxWindow::unhide(false);

      for (const auto& input : mInputs)
      {
         input->unhide(false);
      }
   }
} // unhide

void cxMultiLineInput::erase(bool pEraseSubwindows)
{
   // Erase the main window, as well as all the inputs.
   cxWindow::erase(pEraseSubwindows);

   for (const auto& input : mInputs)
   {
      input->erase(pEraseSubwindows);
   }
} // erase

// Returns the label of this input
string cxMultiLineInput::getLabel() const
{
   // The size of mInputs should always be > 0, but
   //  we check anyway, to be safe.
   if (mInputs.size() > 0)
   {
      return(mInputs[0]->getLabel());
   }
   else
   {
      return("");
   }
}

void cxMultiLineInput::setLabel(const string& pLabel)
{
   if (mInputs.size() > 0)
   {
      mInputs[0]->setLabel(pLabel);
   }
}

// Returns the user's input
string cxMultiLineInput::getValue() const
{
   string value;

   // The user's input will be the concatenation of
   //  all the values from the single-line inputs.
   // If mEnterAlwaysExits is true, then the values
   //  of all the inputs can be concatenated as if
   //  this was one big input.  Otherwise (if
   //  mEnterAlwaysexits is false), add newline characters
   //  to the value after each input line.
   if (mEnterAlwaysExits)
   {
      for (const auto& input : mInputs)
      {
         string currentValue = input->getValue(false, false);
         if (currentValue != "")
         {
            value += currentValue;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         value += input->getValue(false, false) + "\n";
      }

      // Remove the trailing newline from value
      if (value.length() > 0)
      {
         if (value[value.length()-1] == '\n')
         {
            value.erase(value.length()-1, 1);
         }
      }
   }

   return(value);
} // getValue

// Clears the input value.
void cxMultiLineInput::clearValue(bool pRefresh)
{
   for (const auto& input : mInputs)
   {
      input->clearValue(pRefresh);
   }
} // clearValue

void cxMultiLineInput::clear(bool pRefresh)
{
   clearValue(pRefresh);
} // clear

bool cxMultiLineInput::setValue(string pValue, bool pRefresh)
{
   bool success = true;

   string oldValue = getValue();
   if (pValue != oldValue)
   {
      // If built-in validation is enabled, then validate the value based on
      //  mInputType, as inputValidator() would do.
      bool setIt = true;
      if (usingBuiltInValidator() && mRunValidatorFunction)
      {
         if (pValue == "")
         {
            setIt = mAllowBlank;
         }
         else
         {
            switch (mInputType)
            {
               case eINPUT_TYPE_TEXT:
                  setIt = checkValidOptionStrings(pValue);
                  break;
               case eINPUT_TYPE_NUMERIC_FLOATING_PT:
               case eINPUT_TYPE_NUMERIC_WHOLE:
                  // Note: valueInRange() checks whether the value contains
                  //  a valid number and is within range.
                  setIt = valueInRange(pValue);
                  break;
            }
         }
      }

      // If it's okay to set the value, then set it.
      if (setIt)
      {
         // If the external value pointer is set, then update it.
         if (mExtValue != nullptr)
         {
            *mExtValue = pValue;
         }

         // Split pValue into lines that can fit into
         //  the individual input lines (mInputs).
         cxInputPtrContainer::iterator iter = mInputs.begin();
         for (; (iter != mInputs.end()) && success; ++iter)
         {
            // Get the next length of string from pValue that can
            //  fit into the current input line and set that in
            //  the current input line.
            string value;
            if (iter != mInputs.end() - 1)
            {
               value = pValue.substr(0, (*iter)->getInputLen());
               pValue.erase(0, (*iter)->getInputLen());
            }
            else
            {
               value = pValue.substr(0);
               pValue = "";
            }
            success = (*iter)->setValue(value, false);
         }

         // If mRunValidatorFunction is true, then run the validator function, and
         //  if it returns a warning, then set the old value back.
         // Note: This uses getUseValidatorFunction() and
         //  toggleValidatorFunction() instead of setting mRunValidatorFunction
         //  because cxComboBox has its own mRunValidatorFunction.
         if (getUseValidatorFunction())
         {
            if (runValidatorFunction() != "")
            {
               // Temporarily disable the validator function to make sure we can
               //  set the old value back in the input..
               toggleValidatorFunction(false);
               setValue(oldValue, false);
               toggleValidatorFunction(true);
               success = false;
            }
         }
      }
      else
      {
         success = false;
      }
   }

   if (success)
   {
      mErrorState = eNO_ERROR;
      // Figure out which line the cursor should be on
      mCurrentInputLine = 0;
      for (const auto& input : mInputs)
      {
         if (input->isFull())
         {
            ++mCurrentInputLine;
         }
         else
         {
            break;
         }
      }
      // Fix mCurrentInputLine if it went out of bounds
      if (mCurrentInputLine >= (int)(mInputs.size()))
      {
         mCurrentInputLine = (int)(mInputs.size()) - 1;
      }

      // Refresh the screen if the user wanted to
      if (pRefresh)
      {
         refreshValue(pRefresh);
      }
      // If this input is on a cxForm, tell the form that it has been updated.
      if ((mParentForm != nullptr))
      {
         mParentForm->setChanged(true);
      }
   }

   return(success);
} // setValue

// Setter for the "external" user value variable pointer.
//  pExtval: The new pointer to the external variable.
//  pRefresh: Whether or not to refresh the input.
void cxMultiLineInput::setExtValue(string *pExtVal, bool pRefresh)
{
   mExtValue = pExtVal;
   if (mExtValue != nullptr)
   {
      // If pRefresh is true, then call show().  show() will
      //  update the value entered in this input box.
      //  Otherwise (if pRefresh is false), just update the
      //  value of this input.
      if (pRefresh)
      {
         show(false, false);
      }
      else
      {
         setValue(*mExtValue, pRefresh);
      }
   }
}

void cxMultiLineInput::setValidatorStr(const string& pValidator)
{
   // If the validator string is too long for the first input, then
   //  split it across each input.  Otherwise, set the validator
   //  only for the first input.
   cxInputPtrContainer::iterator iter = mInputs.begin();
   // length will hold the possible input length for each input window.
   unsigned length = (unsigned)(*iter)->width() - (*iter)->getLabel().length();
   if (pValidator.length() > length)
   {
      string validator;
      // lastIndex holds the last starting index to use for taking
      //  a substring of pValidator.
      unsigned lastIndex = 0;
      for (iter = mInputs.begin(); iter != mInputs.end(); ++iter)
      {
         // Update length.  We already know the length of input of the first input
         //  window, so if the current input is not the first input, then get
         //  the current input's length.
         if (iter != mInputs.begin())
         {
            length = (unsigned)(*iter)->width();
         }
         // Set the current input's validator only if there is something
         //  left in pValidator to set it to.
         if (lastIndex < pValidator.length())
         {
            validator = pValidator.substr(lastIndex, length);
            (*iter)->setValidator(validator);
            lastIndex += length;
            //length = pValidator.length() - validator.length();
         }
         else
         {
            break;
         }
      }
   }
   else
   {
      // The validator string (pValidator) can fit in the first input.
      (*iter)->setValidator(pValidator);
   }
} // setValidatorStr

string cxMultiLineInput::getValidatorStr() const
{
   // The validator is spread out across each input..  So
   //  it needs to be put back together.
   string validatorStr;
   for (const auto& input : mInputs)
   {
      validatorStr += input->getValidatorStr();
   }

   return(validatorStr);
} // getValidator

bool cxMultiLineInput::textIsValid() const
{
   bool isValid = true;

   // Keep checking each single-line input until
   //  we encounter one that is empty.
   for (const auto& input : mInputs)
   {
      if (input->getValue() != "")
      {
         isValid = input->textIsValid();
         if (!isValid)
         {
            break;
         }
      }
      else
      {
         break; // The current input has no value entered in it
      }
   }

   return(isValid);
} // textIsValid

bool cxMultiLineInput::textIsValid(const string& pText) const
{
   // Get the combined validator string from all
   //  single-line inputs
   string validatorStr = getValidatorStr();
   // Construct a cxTextValidator to see if the text string
   //  is valid
   cxTextValidator iValidator(pText, validatorStr);
   return (iValidator.textIsValid());
} // textIsValid

// Returns whether masking is enabled for this input.
bool cxMultiLineInput::getMasked()
{
   // Return true only if all inputs have masking
   //  enabled.
   bool allInputsMasked = true;

   //cxInputPtrContainer::iterator iter;
   for (const auto& input : mInputs)
   {
      // If this input has masking turned off, then
      //  set allInputsMasked false and break from the loop.
      if (!(input->getMasked()))
      {
         allInputsMasked = false;
         break;
      }
   }

   return(allInputsMasked);
}

// Enables/disables input masking.
void cxMultiLineInput::toggleMasking(bool pMasking)
{
   for (const auto& input : mInputs)
   {
      input->toggleMasking(pMasking);
   }
}

char cxMultiLineInput::getMaskChar() const
{
   char maskChar = '*';

   if (mInputs.size() > 0)
   {
      maskChar = (*(mInputs.begin()))->getMaskChar();
   }

   return(maskChar);
}

void cxMultiLineInput::setMaskChar(char pMaskChar)
{
   for (const auto& input : mInputs)
   {
      input->setMaskChar(pMaskChar);
   }
}

// Changes the window's position (based on
//  a new upper-left corner).  Also moves
//  the input windows.
bool cxMultiLineInput::move(int pNewRow, int pNewCol, bool pRefresh)
{
   // In case moving any of the single-line inputs fails..
   int oldRow = top();
   int oldCol = left();

   // Move this window
   bool moved = cxWindow::move(pNewRow, pNewCol, false);

   if (moved)
   {
      // Set up the new starting row & column for the
      //  input line windows.
      // Save the positions of the single-line inputs
      //  in case moving one of the fails, so we can
      //  move the rest back to where they were
      vector<pair<int, int> > oldInputPositions;

      int row = top();
      int col = left();
      // If there are borders, add 1 to both row & col.
      if (getBorderStyle() != eBS_NOBORDER)
      {
         ++row;
         ++col;
      }

      // Move all the input line windows
      for (const auto& input : mInputs)
      {
         // Store the input's position in
         //  oldInputPositions
         int inputTop = input->top();
         int inputLeft = input->left();
         oldInputPositions.push_back(pair<int, int>(inputTop, inputLeft));
         // Move the input
         moved = input->move(row, col, false);
         // If moving the input failed, then move all the previously-
         //  moved single-line inputs back to where they were.
         if (!moved)
         {
            unsigned vectorSize = oldInputPositions.size();
            for (unsigned i = 0; i < vectorSize; ++i)
            {
               mInputs[i]->move(oldInputPositions[i].first,
                                oldInputPositions[i].second, false);
            }
            break;
         }

         ++row;
      }

      // Move mRightLabel
      mRightLabel->move((hasBorder() ? top()+1 : top()),
                        right() + mRightLabelOffset + 1, false);
   }

   // If the move was successful, refresh the window
   //  if pRefresh is true; otherwise, move the
   //  window back to where it was before.
   if (moved)
   {
      if (pRefresh)
      {
         show(false, false);
      }
   }
   else
   {
      cxWindow::move(oldRow, oldCol, false);
   }

   return(moved);
} // move

void cxMultiLineInput::resize(int pNewHeight, int pNewWidth, bool pRefresh)
{
   // Don't bother resizing unless pNewHeight or pNewWidth are
   //  different than the current height/width.
   if ((pNewHeight != height()) || (pNewWidth != width()))
   {
      // Figure out the minimum & maximum height & width
      // The minimum height must be 1 without borders or 3 with borders
      int minHeight = 1;
      if (getBorderStyle() != eBS_NOBORDER)
      {
         minHeight = 3;
      }
      // Same with the width
      int minWidth = 1;
      if (getBorderStyle() != eBS_NOBORDER)
      {
         minWidth = 3;
      }
      int col = left();
      int row = top();
      const int maxHeight = (int)(cxBase::height()) - row;
      const int maxWidth = (int)(cxBase::width()) - col;

      // Adjust pNewHeight & pNewWidth if necessary
      if (pNewHeight < minHeight)
      {
         pNewHeight = minHeight;
      }
      else if (pNewHeight > maxHeight)
      {
         pNewHeight = maxHeight;
      }
      if (pNewWidth < minWidth)
      {
         pNewWidth = minWidth;
      }
      else if (pNewWidth > maxWidth)
      {
         pNewWidth = maxWidth;
      }

      // Keep track of the old height
      // Note: This must use the height() of this class, rather
      //  than the height of any derived class, in case the
      //  derived class' height() would report a different number.
      const int oldHeight = cxMultiLineInput::height();

      // Resize the window
      cxWindow::resize(pNewHeight, pNewWidth, pRefresh);

      // Get the current value
      const string theValue = getValue();

      // The width of the new inputs may have to be pNewWidth
      //  - 2 if there are borders.
      int inputWidth = pNewWidth;
      if (getBorderStyle() != eBS_NOBORDER)
      {
         inputWidth -= 2;
      }

      // Re-size all current inputs with the new width
      for (const auto& input : mInputs)
      {
         input->resize(1, inputWidth, false);
      }

      // If pNewHeight is greater than the old height, add more single-line
      //  inputs.  If pNewHeight is less than the current height, remove inputs
      //  (but always leave at least 1).
      if (pNewHeight > oldHeight)
      {
         int col = 0; // Column for the inputs
         if (getBorderStyle() != eBS_NOBORDER)
         {
            col = 1;
         }

         // Find the first row for the new inputs
         int row = mInputs[mInputs.size()-1]->bottom() + 1;

         // Use the same input kind for new inputs
         eInputOptions inputKind = (eInputOptions)mInputs[0]->getInputOption();

         for (int i = oldHeight; i < pNewHeight; ++i)
         {
            shared_ptr<cxInput> input = make_shared<cxInput>(nullptr, row, col, inputWidth, "",
                                                            eBS_NOBORDER, inputKind, true, nullptr);
            // Set the input's parent multi-line input to this input
            input->mParentMLInput = this;
            // Apply the attributes from the first input to the new input
            //input->setValueAttr(mInputs[0]->getValueAttr());
            input->mMessageAttrs = mInputs[0]->mMessageAttrs;
            input->mTitleAttrs = mInputs[0]->mTitleAttrs;
            input->mStatusAttrs = mInputs[0]->mStatusAttrs;
            input->mBorderAttrs = mInputs[0]->mBorderAttrs;
            input->mHotkeyAttrs = mInputs[0]->mHotkeyAttrs;
            input->mLabelAttrs = mInputs[0]->mLabelAttrs;
            input->mDataReadonlyAttrs = mInputs[0]->mDataReadonlyAttrs;
            input->mDataEditableAttrs = mInputs[0]->mDataEditableAttrs;
            mInputs.push_back(input);
            ++row;

            // Make sure the exit keys for the new input are
            //  the same as the other inputs
            input->mExitKeys = mInputs[0]->mExitKeys;

            // The input should exit when a function key is pressed so that
            //  this multi-line input can catch it.
            input->setExitOnFKey(true);
            // The input should exit when the user presses backspace at the first
            //  input position so that this multi-line input can wrap back
            //  around to the previous input.
            input->setExitOnBackspaceAtFront(true);
         }
      }
      else if (pNewHeight < oldHeight)
      {
         int numInputsToRemove = oldHeight - pNewHeight;

         // First we need to free the memory used by the extra input
         //  pointers, then remove the input pointers from mInputs.
         int numInputs = (int)mInputs.size();
         int startIndex = numInputs - numInputsToRemove;
         // Free the memory used by the input pointers
         for (int i = startIndex; i < numInputs; ++i)
         {
            mInputs[i].reset();
         }

         // Remove the input pointers from mInputs
         mInputs.erase(mInputs.begin()+startIndex, mInputs.end());
      }

      // Set the value back into the input
      setValue(theValue);

      // Set the maximum input length back
      setMaxInputLength(mMaxInputLength);
   }
} // resize

bool cxMultiLineInput::setKeyFunction(int pKey, const shared_ptr<cxFunction>& pFunction)
{
   bool setIt = cxWindow::setKeyFunction(pKey, pFunction);
   if (setIt)
   {
      // Add the key to each input's set of exit keys.
      for (const auto& input : mInputs)
      {
         input->addExitKey(pKey, false, true);
      }

      // Make sure the key doesn't exist in the quitKey and exitKey lists
      //  (for this multi-line input; not for the single-line inputs!)
      cxWindow::removeQuitKey(pKey);
      cxWindow::removeExitKey(pKey);
   }

   return(setIt);
} // setKeyFunction

bool cxMultiLineInput::setKeyFunction(int pFunctionKey,
                                funcPtr4 pFunction,
                                void *p1, void *p2, void *p3, void *p4,
                                bool pUseVal, bool pExitAfterRun,
                                bool pRunOnLeaveFunction)
                                {
   bool setIt = cxWindow::setKeyFunction(pFunctionKey, pFunction, p1,
                      p2, p3, p4, pUseVal, pExitAfterRun, pRunOnLeaveFunction);
   if (setIt)
   {
      // Add the key to each input's set of exit keys.
      for (const auto& input : mInputs)
      {
         input->addExitKey(pFunctionKey, false, true);
      }

      // Make sure the key doesn't exist in the quitKey and exitKey lists
      //  (for this multi-line input; not for the single-line inputs!)
      cxWindow::removeQuitKey(pFunctionKey);
      cxWindow::removeExitKey(pFunctionKey);
   }

   return(setIt);
} // setKeyFunction

bool cxMultiLineInput::setKeyFunction(int pFunctionKey, funcPtr4 pFunction,
                                  void *p1, void *p2, void *p3, void *p4,
                                  bool pUseVal, bool pExitAfterRun,
                                  bool pRunOnLeaveFunction, bool pRunValidator)
                                  {
   bool setIt = setKeyFunction(pFunctionKey, pFunction, p1, p2, p3, p4,
                               pUseVal, pExitAfterRun, pRunOnLeaveFunction);
   if (setIt && !pRunValidator)
   {
      mSkipValidatorKeys.insert(pFunctionKey);
   }

   return(setIt);
} // setKeyFunction

bool cxMultiLineInput::setKeyFunction(int pFunctionKey, funcPtr2 pFunction,
                                void *p1, void *p2, bool pUseVal, bool pExitAfterRun,
                                bool pRunOnLeaveFunction)
                                {
   bool setIt = cxWindow::setKeyFunction(pFunctionKey, pFunction, p1, p2,
                             pUseVal, pExitAfterRun, pRunOnLeaveFunction);
   if (setIt)
   {
      // Add the key to each input's set of exit keys.
      for (const auto& input : mInputs)
      {
         input->addExitKey(pFunctionKey, false, true);
      }

      // Make sure the key doesn't exist in the quitKey and exitKey lists.
      //  (for this multi-line input; not for the single-line inputs!)
      cxWindow::removeQuitKey(pFunctionKey);
      cxWindow::removeExitKey(pFunctionKey);
   }

   return(setIt);
} // setKeyFunction

bool cxMultiLineInput::setKeyFunction(int pFunctionKey, funcPtr2 pFunction,
                                  void *p1, void *p2, bool pUseVal,
                                  bool pExitAfterRun, bool pRunOnLeaveFunction,
                                  bool pRunValidator)
                                  {
   bool setIt = setKeyFunction(pFunctionKey, pFunction, p1, p2, pUseVal,
                               pExitAfterRun, pRunOnLeaveFunction);
   if (setIt && !pRunValidator)
   {
      mSkipValidatorKeys.insert(pFunctionKey);
   }

   return(setIt);
} // setKeyFunction

bool cxMultiLineInput::setKeyFunction(int pFunctionKey, funcPtr0 pFunction,
                                bool pUseVal, bool pExitAfterRun,
                                bool pRunOnLeaveFunction)
                                {
   bool setIt = cxWindow::setKeyFunction(pFunctionKey, pFunction,
                             pUseVal, pExitAfterRun, pRunOnLeaveFunction);
   if (setIt)
   {
      // Add the key to each input's set of exit keys.
      for (const auto& input : mInputs)
      {
         input->addExitKey(pFunctionKey, false, true);
      }

      // Make sure the key doesn't exist in the quitKey and exitKey lists.
      //  (for this multi-line input; not for the single-line inputs!)
      cxWindow::removeQuitKey(pFunctionKey);
      cxWindow::removeExitKey(pFunctionKey);
   }

   return(setIt);
} // setKeyFunction

bool cxMultiLineInput::setKeyFunction(int pFunctionKey, funcPtr0 pFunction,
                                  bool pUseVal,
                                  bool pExitAfterRun, bool pRunOnLeaveFunction,
                                  bool pRunValidator)
                                  {
   bool setIt = setKeyFunction(pFunctionKey, pFunction, pUseVal,
                               pExitAfterRun, pRunOnLeaveFunction);
   if (setIt && !pRunValidator)
   {
      mSkipValidatorKeys.insert(pFunctionKey);
   }

   return(setIt);
} // setKeyFunction

void cxMultiLineInput::addSkipValidatorKey(int pKey)
{
   mSkipValidatorKeys.insert(pKey);
} // addSkipValidatorKey

void cxMultiLineInput::removeSkipValidatorKey(int pKey)
{
   mSkipValidatorKeys.erase(pKey);
} // removeSkipValidatorKey

bool cxMultiLineInput::hasSkipValidatorKey(int pKey) const
{
   return(mSkipValidatorKeys.find(pKey) != mSkipValidatorKeys.end());
} // hasSkipValidatorKey

void cxMultiLineInput::clearKeyFunction(int pFunctionKey)
{
   cxWindow::clearKeyFunction(pFunctionKey);
   mSkipValidatorKeys.erase(pFunctionKey);
} // clearKeyFunction

void cxMultiLineInput::clearKeyFunctions()
{
   cxWindow::clearKeyFunctions();
   mSkipValidatorKeys.clear();
} // clearKeyFunctions

void cxMultiLineInput::setLabelColor(e_cxColors pColor)
{
   if (mInputs.size() > 0)
   {
      (*mInputs.begin())->setLabelColor(pColor);
   }
} // setLabelColor

e_cxColors cxMultiLineInput::getLabelColor() const
{
   // There should always be at least 1 input, but check anyways..
   if (mInputs.size() > 0)
   {
      return((*mInputs.begin())->getLabelColor());
   }
   else
   {
      // return a default value..
      return(eDEFAULT);
   }
} // getLabelColor

void cxMultiLineInput::setValueColor(e_cxColors pColor)
{
   // Set the value color of all inputs.
   for (const auto& input : mInputs)
   {
      input->setValueColor(pColor);
   }
} // setValueColor

e_cxColors cxMultiLineInput::getValueColor() const
{
   // There should always be at least 1 input, but check anyways..
   if (mInputs.size() > 0)
   {
      return((*mInputs.begin())->getValueColor());
   }
   else
   {
      return(eDEFAULT);
   }
} // getValueColor

void cxMultiLineInput::setColor(e_WidgetItems pItem, e_cxColors pColor)
{
   // Set the color on all the single-line inputs.
   for (const auto& input : mInputs)
   {
      input->setColor(pItem, pColor);
   }
} // setColor

int cxMultiLineInput::getInputOption() const
{
   int inputKind = eINPUT_READONLY;

   // Note: All inputs have the same kind.
   // If there are 0 inputs, for some reason,
   //  then the return type will be left as
   //   eINPUT_READONLY.
   if (mInputs.size() > 0)
   {
      inputKind = (*mInputs.begin())->getInputOption();
   }

   return(inputKind);
} // getKind

void cxMultiLineInput::setInputOption(eInputOptions pInputKind)
{
   for (const auto& input : mInputs)
   {
      input->setInputOption(pInputKind);
   }

   // If mApplyAttrDefaults is true, apply an attribute
   //  to the value of the input so that it stands out.
   if (mApplyAttrDefaults)
   {
      switch (pInputKind)
      {
         case eINPUT_EDITABLE:
            // Disable the read-only attributes & enable the editable
            //  attributes
            disableAttrs(mWindow, eDATA_READONLY);
            enableAttrs(mWindow, eDATA_EDITABLE);
            break;
         case eINPUT_READONLY:
            // Disable the editable attributes & enable the read-only
            //  attributes
            disableAttrs(mWindow, eDATA_EDITABLE);
            enableAttrs(mWindow, eDATA_READONLY);
            break;
      }
   }
} // setInputOption

void cxMultiLineInput::toggleCursor(bool pShowCursor)
{
   for (const auto& input : mInputs)
   {
      input->toggleCursor(pShowCursor);
   }
} // toggleCursor

string cxMultiLineInput::clearInput(void* theInput, void* unused)
{
   if (theInput != nullptr)
   {
      cxMultiLineInput *pInput = (cxMultiLineInput*)theInput;
      pInput->setValue("");
      // Refresh the input
      pInput->show(false, false);
   }

   return("");
} // clearInput

bool cxMultiLineInput::setValidatorFunction(const std::shared_ptr<cxFunction>& pFunction)
{
   bool setIt = false;
   // Free the memory used by mValidatorFunction, and then update it with the
   // given parameters.
   clearValidatorFunction();
   if (pFunction != nullptr)
   {
      mValidatorFunction = pFunction;
      setIt = (mValidatorFunction != nullptr);
   }
   else
   {
      setIt = true;
   }

   // Set mRunValidatorFunction - If mValidatorFunction is not nullptr, then
   // mRunValidatorFunction should be true; otherwise, it should be false.
   // Note: This uses toggleValidatorFunction() instead of setting
   // mRunValidatorFunction directly because cxComboBox has its own
   // mRunValidatorFunction.
   toggleValidatorFunction(mValidatorFunction != nullptr);

   return(setIt);
} // setValidatorFunction

bool cxMultiLineInput::setValidatorFunction(funcPtr4 pFunction, void *p1, void *p2,
                                            void *p3, void *p4)
                                            {
   bool setIt = false;
   // Free the memory used by mValidatorFunction, and then update it with the
   // given parameters.
   clearValidatorFunction();
   if (pFunction != nullptr)
   {
      mValidatorFunction = make_shared<cxFunction4>(pFunction, p1, p2, p3, p4, false, false, true);
      setIt = (mValidatorFunction != nullptr);
   }
   else
   {
      setIt = true;
   }

   // Set mRunValidatorFunction - If mValidatorFunction is not nullptr, then
   //  mRunValidatorFunction should be true; otherwise, it should be false.
   // Note: This uses toggleValidatorFunction() instead of setting
   //  mRunValidatorFunction directly because cxComboBox has its own
   //  mRunValidatorFunction.
   toggleValidatorFunction(mValidatorFunction != nullptr);

   return(setIt);
} // setValidatorFunction

bool cxMultiLineInput::setValidatorFunction(funcPtr2 pFunction, void *p1, void *p2)
{
   bool setIt = false;
   // Free the memory used by mValidatorFunction, and then update it with the
   //  given parameters.
   clearValidatorFunction();
   if (pFunction != nullptr)
   {
      mValidatorFunction = make_shared<cxFunction2>(pFunction, p1, p2, false, false, true);
      setIt = (mValidatorFunction != nullptr);
   }
   else
   {
      setIt = true;
   }

   // Set mRunValidatorFunction - If mValidatorFunction is not nullptr, then
   //  mRunValidatorFunction should be true; otherwise, it should be false.
   // Note: This uses toggleValidatorFunction() instead of setting
   //  mRunValidatorFunction directly because cxComboBox has its own
   //  mRunValidatorFunction.
   toggleValidatorFunction(mValidatorFunction != nullptr);

   return(setIt);
} // setValidatorFunction

bool cxMultiLineInput::setValidatorFunction(funcPtr0 pFunction)
{
   bool setIt = false;
   // Free the memory used by mValidatorFunction, and then update it with the
   //  given parameters.
   clearValidatorFunction();
   if (pFunction != nullptr)
   {
      mValidatorFunction = make_shared<cxFunction0>(pFunction, false, false, true);
      setIt = (mValidatorFunction != nullptr);
   }
   else
   {
      setIt = true;
   }

   // Set mRunValidatorFunction - If mValidatorFunction is not nullptr, then
   //  mRunValidatorFunction should be true; otherwise, it should be false.
   // Note: This uses toggleValidatorFunction() instead of setting
   //  mRunValidatorFunction directly because cxComboBox has its own
   //  mRunValidatorFunction.
   toggleValidatorFunction(mValidatorFunction != nullptr);

   return(setIt);
} // setValidatorFunction

void cxMultiLineInput::clearValidatorFunction()
{
   mValidatorFunction.reset();
} // clearValidatorFunction

bool cxMultiLineInput::isFull() const
{
   bool allFull = true;
   for (const auto& input : mInputs)
   {
      if ( !(input->isFull()) )
      {
         allFull = false;
         break;
      }
   }

   return(allFull);
} // isFull

void cxMultiLineInput::setOnKeyFunction(const shared_ptr<cxFunction>& pFunction)
{
   for (const auto& input : mInputs)
   {
      input->setOnKeyFunction(pFunction);
   }
} // setOnKeyFunction

void cxMultiLineInput::setOnKeyFunction(funcPtr4 pFunction, void *p1, void *p2,
                              void *p3, void *p4)
                              {
   // Set the onKeyFunction for all of the single-line inputs
   for (const auto& input : mInputs)
   {
      input->setOnKeyFunction(pFunction, p1, p2, p3, p4);
   }
} // setOnKeyFunction

void cxMultiLineInput::setOnKeyFunction(funcPtr2 pFunction, void *p1, void *p2)
{
   // Set the onKeyFunction for all of the single-line inputs
   for (const auto& input : mInputs)
   {
      input->setOnKeyFunction(pFunction, p1, p2);
   }
} // setOnKeyFunction

void cxMultiLineInput::setOnKeyFunction(funcPtr0 pFunction)
{
   // Set the onKeyFunction for all of the single-line inputs
   for (const auto& input : mInputs)
   {
      input->setOnKeyFunction(pFunction);
   }
} // setOnKeyFunction

void cxMultiLineInput::toggleOnKeyFunction(bool pRunOnKeyFunction)
{
   // Toggle the onKey function for all of the single-line inputs
   for (const auto& input : mInputs)
   {
      input->toggleOnKeyFunction(pRunOnKeyFunction);
   }
} // toggleOnKeyFunction

void cxMultiLineInput::clearOnKeyFunction()
{
   // Clear the onKey function for all of the single-line inputs
   for (const auto& input : mInputs)
   {
      input->clearOnKeyFunction();
   }
} // clearOnKeyFunction

bool cxMultiLineInput::onKeyFunctionEnabled() const
{
   // There should always be at least 1 single-line input, and
   //  mRunOnKeyFunction is the same for all of them.
   return(mInputs[0]->onKeyFunctionEnabled());
} // onKeyFunctionEnabled

void cxMultiLineInput::setEnterAlwaysExits(bool pEnterAlwaysExits)
{
   mEnterAlwaysExits = pEnterAlwaysExits;
} // setEnterAlwaysExits

bool cxMultiLineInput::enterAlwaysExits() const
{
   return(mEnterAlwaysExits);
} // enterAlwaysExits

int cxMultiLineInput::maxValueLen() const
{
   int maxLen = 0;

   for (const auto& input : mInputs)
   {
      maxLen += input->maxValueLen();
   }

   return(maxLen);
} // maxValueLen

bool cxMultiLineInput::hasFocus() const
{
   bool focus = false;

   // Go through mInputs, and if any of them has
   //  focus, that means this multiLineInput has
   //  focus.
   for (const auto& input : mInputs)
   {
      focus = input->hasFocus();
      if (focus)
      {
         break;
      }
   }

   return(focus);
} // hasFocus

void cxMultiLineInput::setReadOnlyOnLeave(bool pReadOnlyOnLeave)
{
   mReadOnlyOnLeave = pReadOnlyOnLeave;
} // setReadOnlyOnLeave

bool cxMultiLineInput::canBeEditable() const
{
   bool retval = false;

   // mInputs should always have at least 1 input,
   //  but check just in case..
   if (mInputs.size() > 0)
   {
      retval = mInputs[0]->canBeEditable();
   }

   return(retval);
} // canBeEditable

void cxMultiLineInput::setCanBeEditable(bool pCanBeEditable)
{
   for (const auto& input : mInputs)
   {
      input->setCanBeEditable(pCanBeEditable);
   }
} // setCanBeEditable

void cxMultiLineInput::setExitOnFull(bool pExitOnFull)
{
   mExitOnFull = pExitOnFull;
} // setExitOnFull

bool cxMultiLineInput::getExitOnFull() const
{
   return(mExitOnFull);
} // getExitOnFull

void cxMultiLineInput::setBorderStyle(eBorderStyle pBorderStyle)
{
   if (getBorderStyle() != pBorderStyle)
   {
      cxWindow::setBorderStyle(pBorderStyle);
      if (pBorderStyle == eBS_NOBORDER)
      {
         // Purposefully using the parent's resize() rather than
         //  this class' resize() (so we know we don't lose
         //  any inputs)
         cxWindow::resize(height()-2, width()-2, false);
         unsigned numInputs = mInputs.size();
         for (unsigned i = 0; i < numInputs; ++i)
         {
            mInputs[i]->move(top()+i, left(), false);
         }
      }
      else
      {
         // Purposefully using the parent's resize() rather than
         //  this class' resize() (so we know we don't gain
         //  any inputs)
         cxWindow::resize(height()+2, width()+2, false);
         unsigned numInputs = mInputs.size();
         for (unsigned i = 0; i < numInputs; ++i)
         {
            mInputs[i]->move(top()+i+1, left()+1, false);
         }
      }
   }
} // setBorderStyle

bool cxMultiLineInput::isAbove(const cxWindow& pThatWindow) const
{
   // Check to see if the first input's window is above
   //  pThatWindow (if the first input is, then all of the
   //  inputs should be).  mInputs should always have at
   //  least 1 input, but check just in case.
   if (mInputs.size() > 0)
   {
      return(mInputs[0]->isAbove(pThatWindow));
   }
   else
   {
      return(cxWindow::isAbove(pThatWindow));
   }
} // isAbove

bool cxMultiLineInput::isBelow(const cxWindow& pThatWindow) const
{
   // Check to see if the first input's window is below
   //  pThatWindow (if the first input is, then all of the
   //  inputs should be).  mInputs should always have at
   //  least 1 input, but check just in case.
   if (mInputs.size() > 0)
   {
      return(mInputs[0]->isBelow(pThatWindow));
   }
   else
   {
      return(cxWindow::isBelow(pThatWindow));
   }
} // isBelow

void cxMultiLineInput::bringToTop(bool pRefresh)
{
   // Bring the multi-line input window to the top
   cxWindow::bringToTop(false);
   // Bring the single-line inputs to the top
   for (const auto& input : mInputs)
   {
      input->bringToTop(false);
   }

   if (pRefresh)
   {
      cxBase::updateWindows();
   }
} // bringToTop

void cxMultiLineInput::refreshValue(bool pRefresh)
{
   for (const auto& input : mInputs)
   {
      input->refreshValue(pRefresh);
   }
} // refreshValue

void cxMultiLineInput::trapNonAssignedFKeys(bool pTrapNonAssignedFKeys)
{
   // Set this for all inputs.
   for (const auto& input : mInputs)
   {
      input->trapNonAssignedFKeys(pTrapNonAssignedFKeys);
   }
} // trapNonAssignedFKeys

bool cxMultiLineInput::runOnFocusFunction(string *pFunctionRetval)
{
   bool exitInputLoop = false;

   if (mOnFocusFunction != nullptr)
   {
      if (onFocusFunctionIsSet())
      {
         exitInputLoop = mOnFocusFunction->getExitAfterRun();
         string funcRetval = mOnFocusFunction->runFunction();

         if (mOnFocusFunction->getUseReturnVal())
         {
            setValue(funcRetval);
         }

         if (nullptr != pFunctionRetval)
         {
            *pFunctionRetval = funcRetval;
         }
      }
   }

   return(exitInputLoop);
} // runOnFocusFunction

void cxMultiLineInput::runFieldFunction(int pKey)
{
   if (mKeyFunctions.find(pKey) != mKeyFunctions.end())
   {
      // The cxFunction pointers in mKeyFunctions shouldn't be nullptr, but check
      //  anyway.
      if (mKeyFunctions[pKey] != nullptr)
      {
         if (mKeyFunctions[pKey]->functionIsSet()) {
            // Run the function
            string functionRetval = mKeyFunctions[pKey]->runFunction();

            // If the return value of the function is to be used,
            //  then use its return value as the value for this field.
            if (mKeyFunctions[pKey]->getUseReturnVal()) {
               setValue(functionRetval);
               // Refresh the window to show the new value.
               show(false, false);
            }
         }
      }
   }
} // runFieldFunction

int cxMultiLineInput::getInputLen() const
{
   int inputLen = 0;

   for (const auto& input : mInputs)
   {
      inputLen += input->getInputLen();
   }

   return(inputLen);
} // getInputLen

bool cxMultiLineInput::addQuitKey(int pKey, bool pRunOnLeaveFunction, bool pOverride)
{
   // Add the quit key to the single-line inputs'
   //  list of exit keys
   bool added = cxWindow::addQuitKey(pKey, pRunOnLeaveFunction, pOverride);
   if (added)
   {
      for (const auto& input : mInputs)
      {
         // Note: Purposefully passing false for the inputs' pRunOnLeaveFunction.
         input->addQuitKey(pKey, false, pOverride);
      }
   }

   return(added);
} // addQuitKey

void cxMultiLineInput::removeQuitKey(int pKey)
{
   mQuitKeys.erase(pKey);
   // Remove the key from each single-line input's
   //  list of exit keys.
   for (const auto& input : mInputs)
   {
      input->removeQuitKey(pKey);
   }
} // removeQuitKey

bool cxMultiLineInput::addExitKey(int pKey, bool pRunOnLeaveFunction,
                                  bool pOverride)
                                  {
   // Add the exit key to the single-line inputs' //  list of exit keys
   bool added = cxWindow::addExitKey(pKey, pRunOnLeaveFunction, pOverride);
   if (added)
   {
      for (const auto& input : mInputs)
      {
         // Note: Purposefully passing false for the inputs' pRunOnLeaveFunction.
         input->addExitKey(pKey, false, pOverride);
      }
   }

   return(added);
} // addExitKey

void cxMultiLineInput::removeExitKey(int pKey)
{
   mExitKeys.erase(pKey);
   // Remove the key from each single-line input's
   //  list of exit keys.
   for (const auto& input : mInputs)
   {
      input->removeExitKey(pKey);
   }
} // removeExitKey

void cxMultiLineInput::enableInputLoop(bool pDoInputLoop)
{
   mDoInputLoop = pDoInputLoop;
} // enableInputLoop

bool cxMultiLineInput::inputLoopEnabled() const
{
   return(mDoInputLoop);
} // inputLoopEnabled

void cxMultiLineInput::setEditable(bool pEditable)
{
   setInputOption(pEditable ? eINPUT_EDITABLE : eINPUT_READONLY);
} // setEditable

bool cxMultiLineInput::isEditable() const
{
   return((getInputOption() != eINPUT_READONLY) && isEnabled() &&
          inputLoopEnabled());
} // isEditable

bool cxMultiLineInput::getAutoWrapAtBeginning() const
{
   // There should always be at least 1 single-line input,
   //  so we should be safe accessing the first one in
   //  mInputs.
   // The return value should be the value of getCursorLeftAlign() for any of
   //  the single-line inputs.
   return(mInputs[0]->getCursorLeftAlign());
} // getAutoWrapAtBeginning

void cxMultiLineInput::setAutoWrapAtBeginning(bool pWrapAtBeginning)
{
   // Call setCursorLeftAlign() for each single-line input
   //  to set whether the cursor should be placed after the
   //  text in each single-line input.  This value should be
   //  the opposite of pWrapAtBeginning.
   for (const auto& input : mInputs)
   {
      input->setCursorLeftAlign(pWrapAtBeginning);
   }
} // setAutoWrapAtBeginning

bool cxMultiLineInput::getSkipIfReadOnly() const
{
   return(mSkipIfReadOnly);
} // getSkipIfReadOnly()

void cxMultiLineInput::setSkipIfReadOnly(bool pSkipIfReadOnly)
{
   mSkipIfReadOnly = pSkipIfReadOnly;
} // setSkipIfReadOnly

void cxMultiLineInput::setDisableCursorOnShow(bool pDisableCursorOnShow)
{
   cxWindow::setDisableCursorOnShow(pDisableCursorOnShow);
   for (const auto& input : mInputs)
   {
      input->setDisableCursorOnShow(pDisableCursorOnShow);
   }
} // setDisableCursorOnShow

cxForm* cxMultiLineInput::getParentForm() const
{
   return(mParentForm);
} // getParentForm

void cxMultiLineInput::setValidateOnReverse(bool pValidateOnReverse)
{
   mValidateOnReverse = pValidateOnReverse;
} // setValidateOnReverse

bool cxMultiLineInput::getValidateOnReverse() const
{
   return(mValidateOnReverse);
} // getValidateOnReverse

void cxMultiLineInput::setLastKey(int pLastKey)
{
   cxWindow::setLastKey(pLastKey);
   // If this input is on a cxForm, then the form's
   //  last key needs to be set also.
   if (mParentForm != nullptr)
   {
      mParentForm->setLastKey(pLastKey);
   }
} // setLastKey

bool cxMultiLineInput::modalGetsKeypress() const
{
   return(isEnabled() && (getInputOption() == eINPUT_EDITABLE));
} // modalGetsKeypress

void cxMultiLineInput::setEnabled(bool pEnabled)
{
   // Enable/disable this window, and enable/disable all the single-line
   //  inputs.
   cxWindow::setEnabled(pEnabled);
   for (const auto& input : mInputs)
   {
      input->setEnabled(pEnabled);
   }
   mDoInputLoop = pEnabled;

   // Enable/disable mRightLabel
   mRightLabel->setEnabled(pEnabled && mShowRightLabel);
} // setEnabled

void cxMultiLineInput::quitNow(bool pMoveForward)
{
   //cxWindow::quitNow();
   quitNow();
   // If this input is on a form, then tell the form to go onto the next
   //  input.
   if (nullptr != mParentForm)
   {
      // Get the index of this input in the form, and
      //  tell the form to go to the next input.
      int myIndex = mParentForm->getInputIndex(this);
      if (pMoveForward)
      {
         mParentForm->setCurrentInput(myIndex + 1);
      }
      else
      {
         mParentForm->setCurrentInput(myIndex - 1);
      }
   }
} // quitNow

void cxMultiLineInput::quitNow()
{
   cxWindow::quitNow();
   for (const auto& input : mInputs)
   {
      input->quitNow();
   }
} // quitNow

void cxMultiLineInput::exitNow(bool pMoveForward)
{
   //cxWindow::exitNow();
   exitNow();
   // If this input is on a form, then tell the form to go onto the next
   //  input.
   if (nullptr != mParentForm)
   {
      // Get the index of this input in the form, and
      //  tell the form to go to the next input.
      int myIndex = mParentForm->getInputIndex(this);
      if (pMoveForward)
      {
         mParentForm->setCurrentInput(myIndex + 1);
      }
      else
      {
         mParentForm->setCurrentInput(myIndex - 1);
      }
   }
} // exitNow

void cxMultiLineInput::exitNow()
{
   cxWindow::exitNow();
   for (const auto& input : mInputs)
   {
      input->exitNow();
   }
} // exitNow

void cxMultiLineInput::toggleValidatorFunction(bool pRunValidatorFunction)
{
   mRunValidatorFunction = pRunValidatorFunction;
} // toggleValidatorFunction

bool cxMultiLineInput::getUseValidatorFunction() const
{
   return(mRunValidatorFunction);
} // getUseValidatorFunction

cxWindow* cxMultiLineInput::getParent() const
{
   cxWindow *parentWindow = nullptr;

   if (mParentForm != nullptr)
   {
      parentWindow = mParentForm;
   }
   else
   {
      // mParentMultiForm is nullptr
      parentWindow = cxWindow::getParent();
   }

   return(parentWindow);
} // getParent

bool cxMultiLineInput::ranFunctionAndShouldExit() const
{
   return(mRanFunctionAndShouldExit);
} // ranFunctionAndShouldExit

void cxMultiLineInput::addAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // Add the attribute to all the single-line inputs.
   for (const auto& input : mInputs)
   {
      input->addAttr(pItem, pAttr);
   }
} // addAttr

void cxMultiLineInput::setAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // Set the attribute to all the single-line inputs.
   for (const auto& input : mInputs)
   {
      input->setAttr(pItem, pAttr);
   }
} // setAttr

void cxMultiLineInput::removeAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // Remove the attribute from all the single-line inputs.
   for (const auto& input : mInputs)
   {
      input->removeAttr(pItem, pAttr);
   }
} // removeAttr

void cxMultiLineInput::removeAttrs(e_WidgetItems pItem)
{
   // Remove the attributes from all the single-line inputs.
   for (const auto& input : mInputs)
   {
      input->removeAttrs(pItem);
   }
} // removeAttrs

void cxMultiLineInput::getAttrs(e_WidgetItems pItem, set<attr_t>& pAttrs) const
{
   pAttrs.clear();

   // All the single-line inputs should have the same attributes, so get the
   //  attributes from the first one.
   if (mInputs.size() > 0) { // Should always be true, but check just in case..
      mInputs[0]->getAttrs(pItem, pAttrs);
   }
} // getAttrs

bool cxMultiLineInput::getCursorLeftAlign() const
{
   bool cursorLeftAlign = false;

   if (mInputs.size() > 0) { // Should always be true, but check just in case..
      cursorLeftAlign = mInputs[0]->getCursorLeftAlign();
   }

   return(cursorLeftAlign);
} // getCursorLeftAlign

void cxMultiLineInput::setCursorLeftAlign(bool pCursorLeftAlign)
{
   // Use pCursorLeftAlign on the first input, but use true on all the
   //  subsequent inputs, so that the cursor always moves properly through
   //  all the inputs like one would expect.
   if (mInputs.size() > 0) { // Should always be true, but check just in case..
      mInputs[0]->setCursorLeftAlign(pCursorLeftAlign);
      cxInputPtrContainer::iterator iter = mInputs.begin();
      ++iter; // Start on the 2nd input
      for (; iter != mInputs.end(); ++iter)
      {
         (*iter)->setCursorLeftAlign(true);
      }
   }
} // setCursorLeftAlign

bool cxMultiLineInput::getExitOnBackspaceAtFront() const
{
   bool exitOnBackspaceAtFront = false;

   if (mInputs.size() > 0) { // Should always be true, but check just in case..
      exitOnBackspaceAtFront = mInputs[0]->getExitOnBackspaceAtFront();
   }

   return(exitOnBackspaceAtFront);
} // getExitOnBackspaceAtFront

void cxMultiLineInput::setExitOnBackspaceAtFront(bool pExitOnBackspaceAtFront)
{
   // Only set the value in the first input, since that's all that really
   //  matters..
   if (mInputs.size() > 0) { // Should always be true, but check just in case..
      mInputs[0]->setExitOnBackspaceAtFront(pExitOnBackspaceAtFront);
   }
} // setExitOnBackspaceAtFront

bool cxMultiLineInput::hasNavKey(int pKey) const
{
   return(mNavKeys.find(pKey) != mNavKeys.end());
} // hasNavKey

void cxMultiLineInput::addNavKey(int pKey)
{
   mNavKeys.insert(pKey);
} // addNavKey

void cxMultiLineInput::removeNavKey(int pKey)
{
   mNavKeys.erase(pKey);
} // removeNavKey

void cxMultiLineInput::removeNavKeys()
{
   mNavKeys.clear();
} // removeNavKey

void cxMultiLineInput::getNavKeys(set<int>& pNavKeys) const
{
   pNavKeys.clear();
   pNavKeys = mNavKeys;
} // getNavKeys

void cxMultiLineInput::setTimeout(int pTimeout)
{
   for (const auto& input : mInputs)
      input->setTimeout(pTimeout);
   /*
   for (const auto& input : mInputs) {
      input->setTimeout(pTimeout);
   }
   */
} // setTimeout

int cxMultiLineInput::getTimeout() const
{
   int timeout = 0;

   if (mInputs.size() > 0)
   {
      timeout = mInputs[0]->getTimeout();
   }

   return(timeout);
} // getTimeout

string cxMultiLineInput::cxTypeStr() const
{
   return("cxMultiLineInput");
} // cxKeyStr

eInputTypes cxMultiLineInput::getInputType() const
{
   return(mInputType);
} // getInputType

void cxMultiLineInput::setInputType(eInputTypes pInputType)
{
   mInputType = pInputType;
} // setInputType

long double cxMultiLineInput::getRangeLowDouble() const
{
   return(mRangeLowDouble);
} // getRangeLowDouble

void cxMultiLineInput::setRangeLowDouble(const long double &pRangeLowDouble)
{
   mRangeLowDouble = pRangeLowDouble;
} // setRangeLowDouble

long double cxMultiLineInput::getRangeHighDouble() const
{
   return(mRangeHighDouble);
} // getRangeHighDouble

void cxMultiLineInput::setRangeHighDouble(const long double &pRangeHighDouble)
{
   mRangeHighDouble = pRangeHighDouble;
} // setRangeHighDouble

long int cxMultiLineInput::getRangeLowInt() const
{
   return(mRangeLowInt);
} // getRangeLowInt

void cxMultiLineInput::setRangeLowInt(const long int &pRangeLowInt)
{
   mRangeLowInt = pRangeLowInt;
} // setRangeLowInt

long int cxMultiLineInput::getRangeHighInt() const
{
   return(mRangeHighInt);
} // getRangeHighInt

void cxMultiLineInput::setRangeHighInt(const long int &pRangeHighInt)
{
   mRangeHighInt = pRangeHighInt;
} // setRangeHighInt

void cxMultiLineInput::getRangeDouble(long double &pLow, long double &pHigh) const
{
   pLow = mRangeLowDouble;
   pHigh = mRangeHighDouble;
} // getRangeDouble

void cxMultiLineInput::setRangeDouble(const long double &pLow, const long double &pHigh)
{
   mRangeLowDouble = pLow;
   mRangeHighDouble = pHigh;
} // setRangeDouble

void cxMultiLineInput::getRangeInt(long int &pLow, long int &pHigh)
{
   pLow = mRangeLowInt;
   pHigh = mRangeHighInt;
} // getRangeInt

void cxMultiLineInput::setRangeInt(const long int &pLow, const long int &pHigh)
{
   mRangeLowInt = pLow;
   mRangeHighInt = pHigh;
} // setRangeInt

string cxMultiLineInput::inputValidator()
{
   string retval;

   mErrorState = eNO_ERROR;
   // If we're not allowing blank input and the input is blank, then set an
   //  error.  Otherwise, validate based on the value and the input type.
   if (!mAllowBlank && (getValue() == ""))
   {
      retval = "The input cannot be blank.";
      mErrorState = eBLANK;
   }
   else
   {
      // Depending on mInputType, validate using mRangeLowDouble/mRangeHighDouble
      //  or mRangeLowInt/mRangeHighInt or mValidOptionStrings.
      switch (mInputType)
      {
         case eINPUT_TYPE_NUMERIC_FLOATING_PT:
         case eINPUT_TYPE_NUMERIC_WHOLE:
            if (!valueInRange(getValue()))
            {
               retval = "The value (" + getValue() + ") is outside the "
                        "range (";
               // Add the correct range values to retval
               switch (mInputType)
               {
                  case eINPUT_TYPE_NUMERIC_FLOATING_PT:
                     retval += toString(mRangeLowDouble) + " to "
                             + toString(mRangeHighDouble) + ")";
                     break;
                  case eINPUT_TYPE_NUMERIC_WHOLE:
                     retval += toString(mRangeLowInt) + " to "
                             + toString(mRangeHighInt) + ")";
                     break;
                  default:
                     break;
               }
            }
            break;
         case eINPUT_TYPE_TEXT:
            {
               string value = getValue();
               // If mValidOptionStrings has some strings in it, then make sure the
               //  value in the input is in mValidOptionStrings.  If it is, update
               //  the right label with the help text.  If not, then set retval
               //  with a warning.
               if (mValidOptionStrings.size() > 0)
               {
                  if (mValidOptionStrings.find(value) != mValidOptionStrings.end())
                  {
                     setRightLabel(mValidOptionStrings[value], true);
                  }
                  else
                  {
                     // The input's value isn't in mValidOptionStrings.
                     if (value != "")
                     {
                        mErrorState = eINVALID_VALUE;
                        retval = "The value is invalid.";
                     }
                     else
                     {
                        // The value is blank.  If mAllowBlank is false, then
                        //  the value is invalid.
                        if (!mAllowBlank)
                        {
                           mErrorState = eBLANK;
                           retval = "The value is invalid.";
                        }
                     }
                  }
               }
               else
               {
                  // There are no valid input strings set.  Check to see if
                  //  the value is blank and whether a blank is allowed.
                  if ((value == "") && !mAllowBlank)
                  {
                     mErrorState = eBLANK;
                     retval = "The value is invalid (blank is not allowed).";
                  }
               }
            }
            break;
         default:
            break;
      }
   }

   // If there is a problem with the input and mValidatorFuncMessageBox is
   //  true, then display a message box with the warning.
   if ((retval != "") && mValidatorFuncMessageBox)
   {
      cxBase::messageBox(retval + " (" + getValue() + ")");
   }

   return(retval);
} // inputValidator

string cxMultiLineInput::onKeypress()
{
   string retval;

   mErrorState = eNO_ERROR;
   // If the input is blank and blank is not allowed, then set the error state
   //  and retval.  Otherwise, do other validation.
   if ((getValue() == "") && !mAllowBlank)
   {
      mErrorState = eBLANK;
      retval = "Blank is not allowed.";
   }
   else
   {
      // For numeric inputs, validate using the appropriate validator.
      switch (mInputType)
      {
         case eINPUT_TYPE_NUMERIC_FLOATING_PT:
            retval = cxValidators::floatingPtOnKeyValidator(this, nullptr);
            break;
         case eINPUT_TYPE_NUMERIC_WHOLE:
            retval = cxValidators::intOnKeyValidator(this, nullptr);
            break;
         case eINPUT_TYPE_TEXT:
            // Nothing to do here
            break;
         default:
            break;
      }
   }

   return(retval);
} // onKeypress

void cxMultiLineInput::useBuiltInValidator()
{
   setValidatorFunction(cxMultiLineInput::inputValidatorStatic, this, nullptr);
} // useBuiltInValidator

bool cxMultiLineInput::usingBuiltInValidator() const
{
   bool builtInValidator = false;

   if (mValidatorFunction != nullptr)
   {
      if (mValidatorFunction->cxTypeStr() == "cxFunction2")
      {
         try
         {
            const cxFunction2 *function2 = dynamic_cast<cxFunction2*>(mValidatorFunction.get());
            if (function2 != nullptr)
            {
               builtInValidator = (function2->getFunction() ==
                                   cxMultiLineInput::inputValidatorStatic);
            }
         }
         catch (...)
         {
         }
      }
   }

   return(builtInValidator);
} // usingBuiltInValidator

void cxMultiLineInput::useBuiltInOnKeyFunction()
{
   setOnKeyFunction(cxMultiLineInput::onKeypressStatic, this, nullptr);
} // useBuiltInOnKeyFunction

bool cxMultiLineInput::usingBuiltInOnKeyFunction() const
{
   bool builtInOnKeypress = false;

   // Look at the first input's mOnKeyFunction (it should be the same for all
   // inputs)
   shared_ptr<cxFunction> onKeyFunc = mInputs[0]->mOnKeyFunction;
   if (onKeyFunc != nullptr)
   {
      if (onKeyFunc->cxTypeStr() == "cxFunction2")
      {
         try
         {
            const cxFunction2 *function2 = dynamic_cast<cxFunction2*>(onKeyFunc.get());
            if (function2 != nullptr)
            {
               builtInOnKeypress = (function2->getFunction() == cxMultiLineInput::onKeypressStatic);
            }
         }
         catch (...)
         {
         }
      }
   }

   return(builtInOnKeypress);
} // usingBuiltInOnKeyFunction

void cxMultiLineInput::validatorFuncMessageBox(bool pEnable)
{
   mValidatorFuncMessageBox = pEnable;
} // validatorFuncMessageBox

bool cxMultiLineInput::validatorFuncMessageBox() const
{
   return(mValidatorFuncMessageBox);
} // validatorFuncMessageBox

void cxMultiLineInput::addValidOptions(const string& pValidOptions,
                                       bool pValidate)
                                       {
   // Add each character as a separate valid input string.
   for (const char& optionChar : pValidOptions)
   {
      mValidOptionStrings[string(1, optionChar)] = "";
   }

   if (pValidate)
   {
      // If the current value is not blank and is not valid against
      //  mValidOptionStrings, then blank the value in the input.
      const string value = getValue();
      if ((value != "") && (!checkValidOptionStrings(value)))
      {
         // Temporarily set mAllowBlank to true so that we can blank the value
         bool allowBlankBackup = mAllowBlank;
         mAllowBlank = true;
         setValue("", isModal()); // Refresh the screen if modal
         mAllowBlank = allowBlankBackup;
      }
   }
} // addValidOptions

void cxMultiLineInput::setValidOptions(const string& pValidOptions, bool pValidate)
{
   mValidOptionStrings.clear();
   addValidOptions(pValidOptions, pValidate);
} // setValidOptions

void cxMultiLineInput::setValidOptions(const map<string, string>& pValidOptions, bool pValidate)
{
   mValidOptionStrings = pValidOptions;

   if (pValidate)
   {
      // If the current value is not blank and is not valid against
      //  mValidOptionStrings, then blank the value in the input.
      if ((getValue() != "") && (!checkValidOptionStrings(getValue())))
      {
         // Temporarily set mAllowBlank to true so that we can blank the value
         bool allowBlankBackup = mAllowBlank;
         mAllowBlank = true;
         setValue("", isModal()); // Refresh the screen if modal
         mAllowBlank = allowBlankBackup;
      }
   }
} // setValidOptions

void cxMultiLineInput::addValidOption(const string& pValidOption,
                                      const string& pRightLabelText,
                                      bool pValidate)
                                      {
   mValidOptionStrings[pValidOption] = pRightLabelText;

   if (pValidate)
   {
      // If the current value is not blank and is not valid against
      //  mValidOptionStrings, then blank the value in the input.
      if ((getValue() != "") && (!checkValidOptionStrings(getValue())))
      {
         // Temporarily set mAllowBlank to true so that we can blank the value
         bool allowBlankBackup = mAllowBlank;
         mAllowBlank = true;
         setValue("", isModal()); // Refresh the screen if modal
         mAllowBlank = allowBlankBackup;
      }
   }
} // addValidOption

void cxMultiLineInput::getValidOptions(map<string, string>& pValidOptions) const
{
   pValidOptions = mValidOptionStrings;
} // getValidOptions

void cxMultiLineInput::getValidOptionStrings(set<string>& pValidOptionStrings) const
{
   pValidOptionStrings.clear();

   // Add just the valid input strings from mValidOptions
   for (const auto& optionStrPair : mValidOptionStrings)
   {
      pValidOptionStrings.insert(optionStrPair.first);
   }
} // getValidOptionStrings

string cxMultiLineInput::getValidOptionStrings(const eMLIF& pMLIF) const
{
   string retval;
   if (mValidOptionStrings.size() > 0)
   {
      for (const auto& optionStrPair : mValidOptionStrings)
      {
         retval += optionStrPair.first;
         if (pMLIF == eMLIF_COMMA_SEP_WITH_DESC)
         {
            retval += "=" + optionStrPair.second;
         }
         retval += ",";
         // add a space between the values
         if (pMLIF == eMLIF_COMMA_SEP_WITH_DESC)
         {
            retval += " ";
         }
      }
      // get rid of last comma
      if (pMLIF == eMLIF_COMMA_SEP_WITH_DESC)
      {
         // need to get rid of the 'comma' and the 'space'
         retval = retval.substr(0, retval.length()-2);
      }
      else
      {
         retval = retval.substr(0, retval.length()-1);
      }
   }
   return(retval);
} // getValidOptionStrings

bool cxMultiLineInput::hasValidOptionString(const string& pStr) const
{
   return(mValidOptionStrings.find(pStr) != mValidOptionStrings.end());
} // hasValidOptionString

string cxMultiLineInput::getValidOptionHelpText(const string& pInput) const
{
   string helpText;

   map<string, string>::const_iterator iter = mValidOptionStrings.find(pInput);
   if (iter != mValidOptionStrings.end())
   {
      helpText = iter->second;
   }

   return(helpText);
} // getValidOptionHelpText

void cxMultiLineInput::clearValidOptions()
{
   mValidOptionStrings.clear();
} // clearValidOptions

bool cxMultiLineInput::autoFillFromValidOptions(bool pRefresh)
{
   bool retval = true;

   if (mInputType == eINPUT_TYPE_TEXT)
   {
      // Look for a match in the valid values
      if (mValidOptionStrings.size() > 0)
      {
         retval = false;
         // We'll be looking for strings that start with the current text
         //  in the input.  First, we'll want to find how many matches there
         //  are.  If there is only 1 match, then use that entire value.
         //  If there is more than 1 match, then use the longest text that
         //  is found in all matches.
         string value = getValue();
         int numMatches = 0;
         for (const auto& optionStrPair : mValidOptionStrings)
         {
            if (Find(optionStrPair.first, "^" + value))
            {
               ++numMatches;
            }
            // If this is an exact match, then set retval to true.
            if (optionStrPair.first == value)
            {
               retval = true;
            }
         }

         for (const auto& optionStrPair : mValidOptionStrings)
         {
            if (Find(optionStrPair.first, "^" + value))
            {
               // If there is only one match, then use this one (this must be
               //  it).  But only if it was not a complete match (retval
               //  = false).
               if (numMatches == 1)
               {
                  if (!retval)
                  {
                     setValue(optionStrPair.first, pRefresh);
                  }
                  break;
               }
            }
         }

         // If the value in the input didn't completely match anything in
         //  mValidValueStrings, and we didn't find a complete match, then
         //  put "Invalid" in the right label.  Othewise, put the value's
         //  help text in the right label.
         if (!retval && (numMatches != 1))
         {
            setRightLabel("Invalid", pRefresh);
         }
         else
         {
            setRightLabel(mValidOptionStrings[getValue()], pRefresh);
         }
      }
   }

   return(retval);
} // autoFillFromValidOptions

void cxMultiLineInput::setForceUpper(bool pForceUpper)
{
   // Set it in all the single-line inputs.
   for (const auto& input : mInputs)
   {
      input->setForceUpper(pForceUpper);
   }
} // forceUpper

bool cxMultiLineInput::getForceUpper() const
{
   // Return the value from the first single-line input (they should all be the
   //  same).
   return(mInputs[0]->getForceUpper());
} // forceUpper

void cxMultiLineInput::setAllowBlank(bool pAllowBlank)
{
   mAllowBlank = pAllowBlank;
} // allowBlank

bool cxMultiLineInput::getAllowBlank() const
{
   return(mAllowBlank);
} // allowBlank

void cxMultiLineInput::setExtendedHelp(const string& pExtendedHelp)
{
   mExtendedHelp = pExtendedHelp;
} // setExtendedHelp

string cxMultiLineInput::getExtendedHelp() const
{
   return(mExtendedHelp);
} // getExtendedHelp

void cxMultiLineInput::setExtendedHelpColor(e_cxColors pColor)
{
   mExtendedHelpColor = pColor;
} // setExtendedHelpColor

e_cxColors cxMultiLineInput::getExtendedHelpColor() const
{
   return(mExtendedHelpColor);
} // getExtendedHelpColor

void cxMultiLineInput::setExtendedHelpAttribute(attr_t pAttr)
{
   mExtendedHelpAttribute = pAttr;
} // setExtendedHelpAttribute

attr_t cxMultiLineInput::getExtendedHelpAttribute() const
{
   return(mExtendedHelpAttribute);
} // getExtendedHelpAttribute

void cxMultiLineInput::autoGenerateExtendedHelp(bool pAutoGenerateExtendedHelp)
{
   mAutoGenerateExtendedHelp = pAutoGenerateExtendedHelp;
   // If mAutoGenerateExtendedHelp is now true and the input is currently
   //  modal, and this is a text input, then generate mExtendedHelp so that it
   //  is up to date.
   if (mAutoGenerateExtendedHelp && (mInputType == eINPUT_TYPE_TEXT) && mIsModal)
   {
      generateExtendedHelp();
   }
} // autoGenerateExtendedHelp

bool cxMultiLineInput::autoGenerateExtendeHelp() const
{
   return(mAutoGenerateExtendedHelp);
} // autoGenerateExtendedHelp

void cxMultiLineInput::setExtendedHelpKey(int pKey)
{
   // Clear the current extended help keys, and then add pKey as the help
   //  key.
   clearExtendedHelpKeys();
   addExtendedHelpKey(pKey);
} // setExtendedHelpKey

void cxMultiLineInput::addExtendedHelpKey(int pKey)
{
   mExtendedHelpKeys.insert(pKey);
   // If mUseExtendedHelpKeys is true, then add the key as an exit key to all
   //  the single-line inputs.
   if (mUseExtendedHelpKeys)
   {
      for (const auto& input : mInputs)
      {
         input->addExitKey(pKey, false, true);
      }
   }
} // addExtendedHelpKey

void cxMultiLineInput::setExtendedHelpKeys(const set<int>& pKeys)
{
   clearExtendedHelpKeys();
   mExtendedHelpKeys = pKeys;
   // If mUseExtendedHelpKeys is true, then add each key as an exit key to all
   //  the single-line inputs.
   if (mUseExtendedHelpKeys)
   {
      for (const auto& input : mInputs)
      {
         for (int helpKey : mExtendedHelpKeys)
         {
            input->addExitKey(helpKey, false, true);
         }
      }
   }
} // setExtendedHelpKeys

set<int> cxMultiLineInput::getExtendedHelpKeys() const
{
   return(mExtendedHelpKeys);
} // getExtendedHelpKeys

string cxMultiLineInput::getExtendedHelpKeyStrings() const
{
   string retval;

   for (int helpKey : mExtendedHelpKeys)
   {
      if (retval != "") { retval += ","; }
      retval += cxBase::getKeyStr(helpKey);
   }

   return(retval);
} // getExtendedHelpKeyStrings

void cxMultiLineInput::clearExtendedHelpKeys()
{
   // Remove all extended help keys from the single-line inputs' lists of exit
   // keys.
   for (const auto& input : mInputs)
   {
      for (int helpKey : mExtendedHelpKeys)
      {
         input->removeExitKey(helpKey);
      }
   }

   // Empty the mExtendedHelpKeys collection
   mExtendedHelpKeys.clear();
} // clearExtendedHelpKeys

void cxMultiLineInput::setUseExtendedHelpKeys(bool pUseExtendedHelpKeys)
{
   if (pUseExtendedHelpKeys != mUseExtendedHelpKeys)
   {
      mUseExtendedHelpKeys = pUseExtendedHelpKeys;
      // If mUseExtendedHelpKeys is true, then add all the extended help keys to
      //  each single-line input as an exit key.  If mUseExtendedHelpKeys is
      //  false, then remove the keys from each input's list of exit keys.
      set<int>::iterator setIter;
      if (mUseExtendedHelpKeys)
      {
         for (const auto& input : mInputs)
         {
            for (int helpKey : mExtendedHelpKeys)
            {
               input->addExitKey(helpKey);
            }
         }
      }
      else
      {
         for (const auto& input : mInputs)
         {
            for (int helpKey : mExtendedHelpKeys)
            {
               input->removeExitKey(helpKey);
            }
         }
      }
   }
} // useExtendedHelpKey

bool cxMultiLineInput::getUseExtendedHelpKeys() const
{
   return(mUseExtendedHelpKeys);
} // useExtendedHelpKey

bool cxMultiLineInput::keyIsExtendedHelpKey(int pKey) const
{
   return(mExtendedHelpKeys.find(pKey) != mExtendedHelpKeys.end());
} // keyIsExtendedHelpKey

void cxMultiLineInput::setShowRightLabel(bool pShowRightLabel)
{
   mShowRightLabel = pShowRightLabel;
   mRightLabel->setEnabled(mShowRightLabel);
} // setShowRightLabel

bool cxMultiLineInput::getShowRightLabel() const
{
   return(mShowRightLabel);
} // getShowRightLabel

void cxMultiLineInput::setRightLabelOffset(int pOffset, bool pRefresh)
{
   mRightLabelOffset = pOffset;
   mRightLabel->move(mRightLabel->top(), right() + 1 + mRightLabelOffset,
                     pRefresh);
} // setRightLabelOffset

int cxMultiLineInput::getRightLabelOffset() const
{
   return(mRightLabelOffset);
} // getRightLabelOffset

int cxMultiLineInput::getRightLabelTop() const
{
   return(mRightLabel->top());
} // getRightLabelTop

int cxMultiLineInput::getRightLabelLeft() const
{
   return(mRightLabel->left());
} // getRightLabelLeft()

int cxMultiLineInput::getRightLabelBottom() const
{
   return(mRightLabel->bottom());
} // getRightLabelBottom

int cxMultiLineInput::getRightLabelRight() const
{
   return(mRightLabel->right());
} // getRightLabelRight

int cxMultiLineInput::getRightLabelHeight() const
{
   return(mRightLabel->height());
} // getRightLabelHeight()

int cxMultiLineInput::getRightLabelWidth() const
{
   return(mRightLabel->width());
} // getRightLabelHeight()

void cxMultiLineInput::setRightLabelWidth(int pWidth, bool pRefresh)
{
   mRightLabel->resize(mRightLabel->height(), pWidth, pRefresh);
} // setRightlabelWidth

void cxMultiLineInput::setRightLabelHeight(int pHeight, bool pRefresh)
{
   mRightLabel->resize(pHeight, mRightLabel->width(), pRefresh);
} // setRightLabelHeight

void cxMultiLineInput::resizeRightLabel(int pHeight, int pWidth, bool pRefresh)
{
   mRightLabel->resize(pHeight, pWidth, pRefresh);
} // resizeRightLabel

void cxMultiLineInput::setRightLabel(const string& pText, bool pRefresh)
{
   mRightLabel->setMessage(pText);
   if (pRefresh)
   {
      mRightLabel->show(false, false);
   }
} // setRightLabelText

void cxMultiLineInput::setRightLabel(int pOffset, const string& pText, bool pRefresh)
{
   mRightLabel->setMessage(pText);
   mRightLabel->move(mRightLabel->top(), right() + 1 + pOffset, false);
   if (pRefresh)
   {
      mRightLabel->show(false, false);
   }
} // setRightLabelText

string cxMultiLineInput::getRightLabel() const
{
   return(mRightLabel->getMessage());
} // getRightLabelText

void cxMultiLineInput::getRightLabelSize(int& pHeight, int& pWidth)
{
   pHeight = mRightLabel->height();
   pWidth = mRightLabel->width();
} // getRightLabelSize

e_cxColors cxMultiLineInput::getRightLabelColor() const
{
   return(mRightLabel->getItemColor(eMESSAGE));
} // getRightLabelColor

void cxMultiLineInput::setRightLabelColor(e_cxColors pColor)
{
   mRightLabel->setMessageColor(pColor);
} // setRightLabelColor

void cxMultiLineInput::setRightLabelAttr(attr_t pAttr)
{
   mRightLabel->setAttr(eMESSAGE, pAttr);
} // setRightLabelAttr

void cxMultiLineInput::setMaxInputLength(int pLength)
{
   if (pLength >= 0)
   {
      mMaxInputLength = pLength;
      // Spread the length across all the single-line inputs
      int length = pLength;
      for (const auto& input : mInputs)
      {
         if (length >= input->maxValueLen())
         {
            input->setMaxInputLength(input->maxValueLen());
            length -= input->maxValueLen();
         }
         else
         {
            input->setMaxInputLength(length);
            length = 0; // We've used up all the length
         }
      }
   }
} // setMaxInputLength

int cxMultiLineInput::getMaxInputLength() const
{
   return(mMaxInputLength);
} // getMaxInputLength

eInputErrors cxMultiLineInput::getErrorState() const
{
   return(mErrorState);
} // getErrorState

void cxMultiLineInput::setErrorState(eInputErrors pErrorState)
{
   mErrorState = pErrorState;
} // setErrorState

bool cxMultiLineInput::getUseClearKey() const
{
   return(mUseClearKey);
} // getUseClearKey

void cxMultiLineInput::setUseClearKey(bool pUseClearKey)
{
   mUseClearKey = pUseClearKey;
} // setUseClearKey

void cxMultiLineInput::additionalOnClear()
{
   // This intentionally does nothing.  This function is meant to be overridden
   //  by derived classes to add functionality when the user clears the input.
} // additionalOnClear

bool cxMultiLineInput::rightLabelEnabled() const
{
   return(mRightLabel->isEnabled());
} // rightLabelEnabled

bool cxMultiLineInput::validatorFunctionIsSet() const
{
   bool isSet = false;

   if (mValidatorFunction != nullptr)
   {
      isSet = (mValidatorFunction->functionIsSet());
   }

   return(isSet);
} // validatorFunctionIsSet

/////////////////////////
// Protected functions //
/////////////////////////

void cxMultiLineInput::copyCxMultiLineInputStuff(const cxMultiLineInput *pThatInput)
{
   if ((pThatInput != nullptr) && (pThatInput != this))
   {
      // Remove the single-line inputs in mInputs (they will be re-created
      //  later).
      freeInputs();

      // Copy the stuff inherited from cxWindow, then copy this class' stuff.
      try
      {
         copyCxWinStuff((const cxWindow*)pThatInput);
      }
      catch (const cxWidgetsException& exc)
      {
         // Free up the other memory used
         cxWindow::freeWindow();
         throw(cxWidgetsException("Couldn't copy base cxWindow stuff (copying a cxMultiLineInput)."));
      }

      move(pThatInput->top(), pThatInput->left(), false);
      setLabel(pThatInput->getLabel());
      copyValidatorFunction(*pThatInput);
      mExtValue = pThatInput->mExtValue;
      mCurrentInputLine = 0;
      mEnterAlwaysExits = pThatInput->mEnterAlwaysExits;
      mReadOnlyOnLeave = pThatInput->mReadOnlyOnLeave;
      mExitOnFull = pThatInput->mExitOnFull;
      mDoInputLoop = pThatInput->mDoInputLoop;
      mSkipIfReadOnly = pThatInput->mSkipIfReadOnly;
      mRunValidatorFunction = pThatInput->mRunValidatorFunction;
      mUseClearKey = pThatInput->mUseClearKey;
      mRunFocusFunctions = pThatInput->mRunFocusFunctions;
      mValidateOnReverse = pThatInput->mValidateOnReverse;
      // Note: Purposefully not copying mParentForm.  Otherwise, segfaults
      // could ensue.
      //mParentForm = pThatInput->mParentForm;
      mApplyAttrDefaults = pThatInput->mApplyAttrDefaults;
      mRanFunctionAndShouldExit = false;
      mSkipValidatorKeys = pThatInput->mSkipValidatorKeys;
      mNavKeys = pThatInput->mNavKeys;
      mValidOptionStrings = pThatInput->mValidOptionStrings;
      mAutoGenerateExtendedHelp = pThatInput->mAutoGenerateExtendedHelp;
      mInputType = pThatInput->mInputType;
      mRangeLowDouble = pThatInput->mRangeLowDouble;
      mRangeHighDouble = pThatInput->mRangeHighDouble;
      mRangeLowInt = pThatInput->mRangeLowInt;
      mRangeHighInt = pThatInput->mRangeHighInt;
      mValidatorFuncMessageBox = pThatInput->mValidatorFuncMessageBox;
      mAllowBlank = pThatInput->mAllowBlank;
      mExtendedHelp = pThatInput->mExtendedHelp;
      mExtendedHelpKeys = pThatInput->mExtendedHelpKeys;
      mUseExtendedHelpKeys = pThatInput->mUseExtendedHelpKeys;
      //mRightLabel = pThatInput->mRightLabel;
      mRightLabel = make_unique<cxWindow>(*(pThatInput->mRightLabel));
      mRightLabelOffset = pThatInput->mRightLabelOffset;
      mShowRightLabel = pThatInput->mShowRightLabel;
      mErrorState = pThatInput->mErrorState;
      mRightLabel->setEnabled(pThatInput->mRightLabel->isEnabled());

      // Copy the other input's single-line inputs
      for (auto& input : mInputs)
      {
         shared_ptr<cxInput> myInput = make_shared<cxInput>(*input, this);
         if (mUseExtendedHelpKeys)
         {
            for (int helpKey : mExtendedHelpKeys)
            {
               myInput->addExitKey(helpKey, false, true);
            }
         }
         mInputs.push_back(myInput);
      }

      // If mExtValue is not nullptr, then update the value of
      //  this input.
      if (mExtValue != nullptr)
      {
         setValue(*mExtValue);
      }
   }
} // copyCxMultiLineInputStuff

long cxMultiLineInput::doInputLoop(bool pShowInputs, bool& pRunOnLeaveFunction)
{
   setReturnCode(cxID_EXIT);

   pRunOnLeaveFunction = true;
   mCurrentInputLine = 0;
   // If the cursor is to be right-aligned, start on the last single-line
   //  input that has text in it.
   const bool cursorLeftAlign = getCursorLeftAlign();
   if (!cursorLeftAlign)
   {
      int iCurrentInputLine = (int)(mInputs.size()) - 1;
      while ((iCurrentInputLine >= 0) &&
            (mInputs[iCurrentInputLine]->getValue() == ""))
            {
         --iCurrentInputLine;
      }
      if (iCurrentInputLine < 0)
      {
         iCurrentInputLine = 0;
      }
      mCurrentInputLine = iCurrentInputLine;
   }

   mLeaveNow = false;
   bool continueOn = true;
   while(continueOn && (getInputOption() != eINPUT_READONLY) && !getLeaveNow())
   {
      // Get the last key (returned by the current input line)
      //  and take action based on the key.
      setReturnCode(mInputs[mCurrentInputLine]->showModal(pShowInputs));
      int lastKey = mInputs[mCurrentInputLine]->getLastKey();
      setLastKey(lastKey);
#ifdef NCURSES_MOUSE_VERSION
      // For mouse button events, we want to do the following:
      //  - If an external function exists for the mouse state, then run it.
      //    Otherwise, do the following:
      //  - If the user clicked inside the window, continue onto the next
      //    iteration of the input loop (to simulate nothing happening).
      //  - If the user clicked outside the window, then:
      //    - If the parent window is a cxPanel or if the input has a parent
      //      form, quit out of the input loop (allowing the user to go to
      //      another input or another window).
      //      Otherwise, continue on (to simulate nothing happening).
      if (lastKey == KEY_MOUSE)
      {
         if (getmouse(&mMouse) == OK)
         {
            // If an external function exists for the mouse event, run it.
            //  Otherwise, process the event here.
            bool mouseFunctionExists = false;
            continueOn = handleFunctionForLastMouseState(&mouseFunctionExists);
            if (!mouseFunctionExists)
            {
               if (mouseEvtWasButtonEvt())
               {
                  if (mouseEvtWasInWindow())
                  {
                     continue;
                  }
                  else
                  {
                     if (parentIsCxPanel() || (mParentForm != nullptr))
                     {
                        setReturnCode(cxID_EXIT);
                        continueOn = false;
                        break;
                     }
                     else
                     {
                        continue;
                     }
                  }
               }
            }
         }
      }
#else
      // This is defined for versions of ncurses without mouse support.
      // This is here because the next block starts with "else if".  The
      //  code will go onto the next block because of the false.
      if (false)
      {
      }
#endif
      // If the last key is in mQuitKeys, then quit and return
      //  cxID_QUIT.  If the key isn't there, look for it in
      //  mExitKeys (if it's there, quit and return cxID_EXIT).
      //  If not there either, handle the key in a switch.
      else if (hasQuitKey(lastKey))
      {
         setReturnCode(cxID_QUIT);
         continueOn = false;
      }
      else if (hasExitKey(lastKey))
      {
         setReturnCode(cxID_EXIT);
         continueOn = false;
      }
      // If the key is the input clear key, then clear the
      //  input value.  (Ideally, we'd handle this in the switch, but
      //  you must use const values in a switch).
      else if ((lastKey == cxInput::inputClearKey) && mUseClearKey)
      {
         setValue("", true);
         additionalOnClear();
      }
      // If the user pressed an extended help key and mUseExtendedHelpKeys is
      //  true, then display the extended help.
      else if (keyIsExtendedHelpKey(lastKey) && mUseExtendedHelpKeys)
      {
         if (mExtendedHelp != "")
         {
            cxBase::messageBox(mExtendedHelp, mExtendedHelpColor,
                               mExtendedHelpAttribute);
         }
         else
         {
            cxBase::messageBox("Extended help is not available for this input.",
                               mExtendedHelpColor, mExtendedHelpAttribute);
         }
      }
      else
      {
         // If there is a function associated with the last keypress,
         //  then run it.
         bool functionExists = false;
         bool continueAfterFunc = handleFunctionForLastKey(&functionExists);

         switch(lastKey)
         {
            case KEY_BACKSPACE:
            case BACKSPACE:
            case KEY_UP:
               if (mCurrentInputLine > 0)
               {
                  mInputs[mCurrentInputLine]->show(true, false);
                  --mCurrentInputLine;
                  // Make sure the cursor is at the end of the input
                  int valueLen = (int)(mInputs[mCurrentInputLine]->getValue().length());
                  int labelLen = (int)(mInputs[mCurrentInputLine]->getLabel().length());
                  int xPos = labelLen + valueLen - 1;
                  mInputs[mCurrentInputLine]->setCursorX(xPos);
                  // Set the cursor left alignment false in the input to
                  //  ensure that the cursor position is not reset by the
                  //  input. The cursor left alignment value will get set back
                  //  to what it was before at the end of this method.
                  mInputs[mCurrentInputLine]->setCursorLeftAlign(false);
               }
               else
               {
                  setReturnCode(cxID_EXIT);
                  continueOn = false;
               }
               break;
            case TAB:  // Exit on tab
            case SHIFT_TAB:
               setReturnCode(cxID_EXIT);
               continueOn = false;
               break;
            case KEY_DOWN:
            case ENTER:
            case KEY_ENTER:
               // If the current input line isn't full, then
               //  if mEnterAlwaysExits is true, stop the input
               //  loop.  Otherwise, go onto the next input
               //  line.
               if (!(mInputs[mCurrentInputLine]->isFull())) {
                  if (mEnterAlwaysExits)
                  {
                     setReturnCode(cxID_EXIT);
                     continueOn = false;
                  }
                  else
                  {
                     ++mCurrentInputLine;
                     // If we just left the last input line,
                     //  then don't continue the input loop.
                     if (mCurrentInputLine >= (int)mInputs.size())
                     {
                        setReturnCode(cxID_EXIT);
                        continueOn = false;
                     }
                  }
               }
               else
               {
                  ++mCurrentInputLine;
                  // If we just left the last input line,
                  //  then don't continue the input loop.
                  if (mCurrentInputLine >= (int)mInputs.size())
                  {
                     setReturnCode(cxID_EXIT);
                     continueOn = false;
                  }
               }
               break;
            case ESC:
               setReturnCode(cxID_QUIT);
               continueOn = false;
               break;
            case ERR:   // Error getting a key
               break;
            default:
               // If mLeaveNow was set true, then leave the input loop.
               continueOn = (continueOn && !mLeaveNow && continueAfterFunc);
               if (continueOn)
               {
                  // If no function exists for the current key, treat it as
                  //  a regular key that the user has typed, and go to the
                  //  next input.
                  if (!functionExists)
                  {
                     if (mCurrentInputLine < (int)mInputs.size())
                     {
                        mInputs[mCurrentInputLine]->show(true, false);
                        // If we're on the last input and the input is full,
                        //  then if mExitOnFull is true, exit.
                        if (mCurrentInputLine == (int)mInputs.size()-1)
                        {
                           if (mInputs[mCurrentInputLine]->isFull() && mExitOnFull)
                           {
                              setReturnCode(cxID_EXIT);
                              continueOn = false;
                           }
                        }
                        else
                        {
                           // This isn't the last input line.. Go
                           //  onto the next line.
                           ++mCurrentInputLine;
                        }
                     }
                     else
                     {
                        // mCurrentInputLine somehow is >= mInputs.size()..
                        if (mExitOnFull)
                        {
                           continueOn = false;
                        }
                     }
                  }
               }
               else
               {
                  if (functionExists)
                  {
                     // continueOn is false and functionExists is true,
                     //  which means there was a function that ran and
                     //  this input should exit the input loop.  If this
                     //  input is on a form, then tell the form to go
                     //  onto the next input.
                     if (nullptr != mParentForm)
                     {
                        // Get the index of this input in the form, and
                        //  tell the form to go to the next input.
                        int myIndex = mParentForm->getInputIndex(this);
                        mParentForm->setCurrentInput(myIndex + 1);
                     }
                  }
               }
               break;
         } // switch (lastKey)
      } // else
   } // while (continueOn)

   // mLeaveNow should be false
   mLeaveNow = false;

   // Set the original cursor alignment flag back.  Sometimes, this would
   //  change for one of the single-line inputs if the user presses backspace
   //  and goes back to the previous inputs because the cursor needs to be at
   //  the last position in the input in that case.
   setCursorLeftAlign(cursorLeftAlign);

   return(getReturnCode());
} // doInputLoop

bool cxMultiLineInput::focusFunctionsWillRun() const
{
   return(mRunFocusFunctions);
} // focusFunctionsWillRun

void cxMultiLineInput::runFocusFunctions(bool pRunFocusFunctions)
{
   mRunFocusFunctions = pRunFocusFunctions;
} // runFocusFunctions

void cxMultiLineInput::setParentForm(cxForm *pParentForm)
{
   mParentForm = pParentForm;
} // setParentForm

//// Private helper functions

// This is private so that it can't be called from the
//  outside.
void cxMultiLineInput::setHotkeyHighlighting(bool pHotkeyHighlighting)
{
   cxWindow::setHotkeyHighlighting(pHotkeyHighlighting);
} // setHotkeyHighlighting

bool cxMultiLineInput::searchParentFormsForFKey(int pKey,
                              bool pIncludeNavigationalKeys)
                              {
   bool foundIt = false;

   // If pIncludeNavigationalKeys is false and pKey is a navigational
   //  key, then we won't want to include navigational keys when searching the
   //  parents' keys.
   bool searchParents = true;
   if (!pIncludeNavigationalKeys && hasNavKey(pKey))
   {
      searchParents = false;
   }

   // If pIncludeNavigationalKeys was false and pKey is a navigational key,
   //  then we wouldn't search the parents for this key.
   if (searchParents)
   {
      // Use mParentForm if it's not nullptr; otherwise, use the parent
      //  cxWindow.
      if (mParentForm != nullptr)
      {
         cxForm *parentForm = mParentForm;
         while (parentForm != nullptr)
         {
            // Test to see if pKey is set in the parent
            //  form and it is not the parent form's jump hotkey
            //  (we want to ignore the jump hotkey, because otherwise
            //  the onLeave function wouldn't run)
            if (parentForm->formKeyIsSet(pKey) &&
                (pKey != parentForm->getInputJumpKey()))
                {
               foundIt = true;
               break;
            }
            parentForm = (cxForm*)(parentForm->getParentMultiForm());
         }
         // If we haven't found the key yet, it's possible that the form could
         //  be on a cxPanel rather than a cxMultiForm, so look there too.
         if (!foundIt)
         {
            // The form could be on a cxPanel
            cxWindow *parentWin = mParentForm->getParent();
            while (parentWin != nullptr)
            {
               if (parentWin->hasKeyFunction(pKey))
               {
                  foundIt = true;
                  break;
               }
               parentWin = parentWin->getParent();
            }
         }
      }
      else
      {
         // The input could be on a cxPanel if not a cxForm
         cxWindow *parentWin = getParent();
         while (parentWin != nullptr)
         {
            if (parentWin->hasKeyFunction(pKey))
            {
               foundIt = true;
               break;
            }
            parentWin = parentWin->getParent();
         }
      }
   }

   return(foundIt);
} // searchParentFormsForFKey

void cxMultiLineInput::freeInputs()
{
   for (auto& input : mInputs)
      input.reset();
   mInputs.clear();
} // freeInputs

bool cxMultiLineInput::handleFunctionForLastKey(bool *pFunctionExists,
                                                bool *pRunOnLeaveFunction)
                                                {
   // continueOn is whether or not to continue the input loop.
   bool continueOn = true;

   // If pFunctionExists or pRunOnLeaveFunction are non-nullptr, then default
   //  them.
   if (pFunctionExists != nullptr)
   {
      *pFunctionExists = false;
   }
   if (pRunOnLeaveFunction != nullptr)
   {
      *pRunOnLeaveFunction = true;
   }

   int lastKey = getLastKey();
   if (mKeyFunctions.find(lastKey) != mKeyFunctions.end())
   {
      shared_ptr<cxFunction> iFunc = mKeyFunctions[lastKey];
      // The cxFunction pointers in mKeyFunctions shouldn't be nullptr, but check
      //  anyway.
      if (iFunc != nullptr)
      {
         if (pFunctionExists != nullptr)
         {
            *pFunctionExists = true;
         }
         if (pRunOnLeaveFunction != nullptr)
         {
            *pRunOnLeaveFunction = iFunc->getRunOnLeaveFunction();
         }

         // Run the function
         string functionRetval = iFunc->runFunction();

         // If the return value of the function is to be used,
         //  then use its return value as the value for this field.
         if (iFunc->getUseReturnVal())
         {
            setValue(functionRetval);
            // Refresh the window to show the new value.
            refreshValue(true);
         }

         // If this input should exit after the function runs, store it
         //  in mRanFunctionAndShouldExit.
         mRanFunctionAndShouldExit = iFunc->getExitAfterRun();

         // If the 'mExitAfterRun' value this field function is true,
         //  then the input loop shouldn't continue.
         continueOn = !(iFunc->getExitAfterRun());
      }
   }

   return(continueOn);
} // handleFunctionForLastKey

void cxMultiLineInput::enableAttrs(WINDOW *pWin, e_WidgetItems pItem)
{
   // Enable the attributes on all single-line inputs.
   for (const auto& input : mInputs)
   {
      input->enableAttrs(input->mWindow, pItem);
   }
} // enableAttrs

void cxMultiLineInput::disableAttrs(WINDOW *pWin, e_WidgetItems pItem)
{
   // Disable the attributes on all single-line inputs.
   for (const auto& input : mInputs)
   {
      input->disableAttrs(input->mWindow, pItem);
   }
} // disableAttrs

string cxMultiLineInput::runValidatorFunction() const
{
   string retval;

   if (mValidatorFunction != nullptr)
   {
      if (mValidatorFunction->functionIsSet())
      {
         retval = mValidatorFunction->runFunction();
      }
   }

   return(retval);
} // runValidatorFunction

void cxMultiLineInput::draw()
{
   // If the window has a border, draw the border, title, & status.
   if (getBorderStyle() != eBS_NOBORDER)
   {
      drawBorder();
      drawTitle();
      drawStatus();
   }

   // Draw all the single-line inputs
   for (const auto& input : mInputs)
   {
      input->draw();
   }
} // draw

bool cxMultiLineInput::scrDiff()
{
   bool difference = false;

   // Draw the input text (but don't show it yet)
   draw();
   // Grab what's on the screen and compare it to what's drawn in the
   // input window..  If there is a difference, then show what is drawn
   // in this input.
   // Populate a collection of lines currently on the physical screen.  Each
   // element is a pair, storing the number of characters and the line.


   vector<pair<int, unique_ptr<chtype[]>>> screenLines;
   for (int line = 0; line < height(); ++line)
   {
      unique_ptr<chtype[]> buffer = make_unique<chtype[]>(width()+1);
      int numChars = mvwinchnstr(stdscr, top()+line, left(), buffer.get(), width());
      screenLines.push_back(make_pair(numChars, std::move(buffer)));
   }
   // Populate a collection of lines in the member window
   vector<pair<int, unique_ptr<chtype[]> > > memberLines;
   int inputHeight = height();
   for (int line = 0; line < inputHeight; ++line)
   {
      unique_ptr<chtype[]> buffer = make_unique<chtype[]>(width()+1);
      int numChars = mvwinchnstr(mWindow, line, 0, buffer.get(), width());
      // We also have to deal with the fact that each single-line input is
      // its own separate window..  If there is a border, then we'll have to
      // separately grab its characters, between the borders; If there are
      // no borders, it will be a little easier.
      if (getBorderStyle() != eBS_NOBORDER)
      {
         // There is a border.  If the current line# is within the top & bottom
         // borders, then grab the text of the single-line input at that
         // point.  Otherwise, grab the text in the multiLineInput window at
         // that point (which will be a border line).
         if ((line > 0) && (line < inputHeight-1))
         {
            unique_ptr buffer2 = make_unique<chtype[]>(mInputs[line-1]->width()+1);
            int inputChars = mvwinchnstr(mInputs[line-1]->mWindow, 0, 0, buffer2.get(), mInputs[line-1]->width());
            for (int ch = 0; ch < inputChars; ++ch)
            {
               buffer[ch+1] = buffer2[ch];
            }
         }
         else
         {
            numChars = mvwinchnstr(mWindow, line, 0, buffer.get(), width());
         }
      }
      else
      {
         // There is no border - Grab the text of the single-line input at
         //  this location.
         numChars = mvwinchnstr(mInputs[line]->mWindow, 0, 0, buffer.get(), width());
      }
      memberLines.push_back(make_pair(numChars, std::move(buffer)));
   }
   // See if there's a difference in the member window vs. screen
   if (screenLines.size() == memberLines.size())
   {
      unsigned numLines = screenLines.size();
      chtype *screenBuffer = nullptr;
      chtype *memberBuffer = nullptr;
      for (unsigned line = 0; line < numLines; ++line)
      {
         // Compare each character if the number of characters in each
         //  line is the same
         if (screenLines[line].first == memberLines[line].first)
         {
            screenBuffer = screenLines[line].second.get();
            memberBuffer = memberLines[line].second.get();
            int lineLength = screenLines[line].first;
            for (int ch = 0; ch < lineLength; ++ch)
            {
               if (screenBuffer[ch] != memberBuffer[ch])
               {
                  difference = true;
                  break;
               }
            }
         }
         else
         {
            difference = true;
         }

         if (difference)
         {
            break;
         }
      }
   }
   else
   {
      difference = true;
   }

   return(difference);
} // scrDiff

void cxMultiLineInput::copyValidatorFunction(const cxMultiLineInput& pThatInput)
{
   mValidatorFunction.reset();

   // Copy the other input's mValidatorFunction.
   if (pThatInput.mValidatorFunction.get() != nullptr)
   {
      // We'll need to check what type of cxFunction it is so we can
      //  create it properly.
      if (pThatInput.mValidatorFunction->cxTypeStr() == "cxFunction0")
      {
         const cxFunction0 *iFunc0 = dynamic_cast<cxFunction0*>(pThatInput.mValidatorFunction.get());
         if (iFunc0 != nullptr)
         {
            mValidatorFunction = make_shared<cxFunction0>(iFunc0->getFunction(),
                                          iFunc0->getUseReturnVal(),
                                          iFunc0->getExitAfterRun(),
                                          iFunc0->getRunOnLeaveFunction());
         }
      }
      else if (pThatInput.mValidatorFunction->cxTypeStr() == "cxFunction2")
      {
         const cxFunction2 *iFunc2 = dynamic_cast<cxFunction2*>(pThatInput.mValidatorFunction.get());
         if (iFunc2 != nullptr)
         {
            // When creating mOnKeyFunction, if any of its parameters point to
            //  pThatInput, have them point to this one instead.  Or if they
            //  point to the other input's parent multi-line input, have them
            //  point to this one's parent multi-line input instead.
            void* params[2] = { iFunc2->getParam1(), iFunc2->getParam2() };
            for (int i = 0; i < 2; ++i)
            {
               if (params[i] == &pThatInput)
               {
                  params[i] = this;
               }
            }
            mValidatorFunction = make_shared<cxFunction2>(iFunc2->getFunction(), params[0],
                                          params[1], iFunc2->getUseReturnVal(),
                                          iFunc2->getExitAfterRun(),
                                          iFunc2->getRunOnLeaveFunction());
         }
      }
      else if (pThatInput.mValidatorFunction->cxTypeStr() == "cxFunction4")
      {
         const cxFunction4 *iFunc4 = dynamic_cast<cxFunction4*>(pThatInput.mValidatorFunction.get());
         if (iFunc4 != nullptr)
         {
            // When creating mOnKeyFunction, if any of its parameters point to
            //  pThatInput, have them point to this one instead.  Or if they
            //  point to the other input's parent multi-line input, have them
            //  point to this one's parent multi-line input instead.
            void* params[4] = { iFunc4->getParam1(), iFunc4->getParam2(),
                                iFunc4->getParam3(), iFunc4->getParam4()  };
            for (int i = 0; i < 4; ++i)
            {
               if (params[i] == &pThatInput)
               {
                  params[i] = this;
               }
            }
            mValidatorFunction = make_shared<cxFunction4>(iFunc4->getFunction(), params[0],
                                             params[1], params[2], params[3],
                                             iFunc4->getUseReturnVal(),
                                             iFunc4->getExitAfterRun(),
                                             iFunc4->getRunOnLeaveFunction());
         }
      }
   }
} // copyValidatorFunction

bool cxMultiLineInput::usingOnKeyNumericValidator() const
{
   bool retval = false;

   // The onKey function is set up for each of the single-line inputs that make
   //  up this input.
   // mInputs should have at least 1 input, but check just in case..
   if (mInputs.size() > 0)
   {
      if (mInputs[0]->mOnKeyFunction != nullptr)
      {
         if (mInputs[0]->mOnKeyFunction->cxTypeStr() == "cxFunction2")
         {
            const cxFunction2 *iFunc2 = dynamic_cast<cxFunction2*>(mInputs[0]->mOnKeyFunction.get());
            if (iFunc2 != nullptr)
            {
               funcPtr2 funcPtr = iFunc2->getFunction();
               if ((funcPtr == cxValidators::intOnKeyValidator) ||
                   (funcPtr == cxValidators::floatingPtOnKeyValidator))
                   {
                  retval = true;
               }
            }
         }
      }
   }

   return(retval);
} // usingOnKeyNumericValidator

string cxMultiLineInput::inputValidatorStatic(void *theInput, void *unused)
{
   if (theInput == nullptr) { return(""); }

   cxMultiLineInput *pInput = static_cast<cxMultiLineInput*>(theInput);
   return(pInput->inputValidator());
} // inputValidatorStatic

string cxMultiLineInput::onKeypressStatic(void *theInput, void *unused)
{
   if (theInput == nullptr) { return(""); }

   cxMultiLineInput *pInput = static_cast<cxMultiLineInput*>(theInput);
   return(pInput->onKeypress());
} // onKeypressStatic

void cxMultiLineInput::generateExtendedHelp()
{
   if (mValidOptionStrings.size() > 0)
   {
      mExtendedHelp = "";
      for (const auto& strPair : mValidOptionStrings)
      {
         if (mExtendedHelp != "")
         {
            mExtendedHelp += ", ";
         }
         mExtendedHelp = strPair.first + ": " + strPair.second;
      }
   }
} // generateExtendedHelp

bool cxMultiLineInput::checkValidOptionStrings(const string& pValue)
{
   bool retval = true;

   // If mValidOptionStrings isn't empty, then if pValue isn't in it, return
   //  false.
   if (mValidOptionStrings.size() > 0)
   {
      retval = (mValidOptionStrings.find(pValue) != mValidOptionStrings.end());
   }

   return(retval);
} // checkValidOptionStrings

bool cxMultiLineInput::valueInRange(const string& pValue) const
{
   bool retval = true;

   switch (mInputType)
   {
      case eINPUT_TYPE_NUMERIC_FLOATING_PT:
         if (pValue != "")
         {
            if (isValidFloatingPtNumStr(pValue))
            {
               if ((mRangeLowDouble != 0.0) || (mRangeHighDouble != 0.0))
               {
                  // Make sure we know for sure which numbers are the smaller and
                  //  the larger of the two, in case they're backwards for some
                  //  reason
                  long double low = mRangeLowDouble;
                  long double high = mRangeHighDouble;
                  // If low is more than high, then swap them
                  if (low > high)
                  {
                     long double temp = low;
                     low = high;
                     high = temp;
                  }

                  // See if the value is within range
                  long double value = stringToLongDouble(pValue);
                  retval = ((value >= low) && (value <= high));
               }
            }
            else
            {
               // pValue does not contain a valid floating-point number
               retval = false;
            }
         }
         break;
      case eINPUT_TYPE_NUMERIC_WHOLE:
         if (pValue != "")
         {
            if (isValidWholeNumStr(pValue))
            {
               if ((mRangeLowInt != 0.0) || (mRangeHighInt != 0.0))
               {
                  // Make sure we know for sure which numbers are the smaller and
                  //  the larger of the two, in case they're backwards for some
                  //  reason
                  long int low = mRangeLowInt;
                  long int high = mRangeHighInt;
                  // If low is more than high, then swap them
                  if (low > high)
                  {
                     long int temp = low;
                     low = high;
                     high = temp;
                  }

                  // See if the value is within range
                  long int value = stringToLong(pValue);
                  retval = ((value >= low) && (value <= high));
               }
            }
            else
            {
               // pValue does not contain a valid whole number
               retval = false;
            }
         }
         break;
      default:
         // The input is not numeric.
         break;
   }

   return(retval);
} // valueInRange
