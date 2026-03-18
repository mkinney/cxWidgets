// Copyright (c) 2026 E. Oulashin
// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxForm.h"
#include "cxBase.h"
#include "cxMultiForm.h"
#include "cxFunctionKeyCases.h"
#include <set>
#include <iterator>
#include <typeinfo>
using std::string;
using std::set;
using std::map;
using std::pair;
using std::make_pair;
using std::vector;
using std::insert_iterator;
using std::shared_ptr;
using std::make_shared;
using std::dynamic_pointer_cast;
using cxBase::stringWithoutHotkeyChars;

cxForm::cxForm(cxWindow *pParentWindow, int pRow,
               int pCol, int pHeight,
               int pWidth, const string& pTitle,
               eBorderStyle pBorderStyle,
               cxWindow *pExtTitleWindow,
               cxWindow *pExtStatusWindow, bool pAutoExit,
               bool pStacked)
   : cxWindow(pParentWindow, pRow, pCol, pHeight, pWidth, pTitle, "", "",
              pBorderStyle, pExtTitleWindow, pExtStatusWindow),
     mStacked(pStacked),
     mAutoExit(pAutoExit),
     mInputsOnBorder(pBorderStyle == eBS_NOBORDER ? true : false)
{
   setExtendedHelpKey(KEY_F(1));
} // constructor

// Copy constructor
cxForm::cxForm(const cxForm& pThatForm)
   : cxWindow(nullptr, pThatForm.top(), pThatForm.left(), pThatForm.height(),
              pThatForm.width(), pThatForm.getTitle(), pThatForm.getMessage(),
              pThatForm.getStatus(), pThatForm.getBorderStyle(),
              pThatForm.getExtTitleWindow(), pThatForm.getExtStatusWindow()),
     mInputPositions(pThatForm.mInputPositions),
     mTrapNonAssignedFKeys(pThatForm.mTrapNonAssignedFKeys),
     mStartAtFirstInput(pThatForm.mStartAtFirstInput),
     mStacked(pThatForm.mStacked),
     mTotalInputHeight(pThatForm.mTotalInputHeight),
     mAutoExit(pThatForm.mAutoExit),
     mApplyAttrDefaults(pThatForm.mApplyAttrDefaults),
     mCustomStatus(pThatForm.mCustomStatus),
     mEditableInputsExist(pThatForm.mEditableInputsExist),
     mInputsOnBorder(pThatForm.mInputsOnBorder),
     mInputJumpKey(pThatForm.mInputJumpKey),
     mAllowQuit(pThatForm.mAllowQuit),
     mAllowExit(pThatForm.mAllowExit),
     mExitOnLeaveFirst(pThatForm.mExitOnLeaveFirst),
     mWaitForInputIfEmpty(pThatForm.mWaitForInputIfEmpty),
     mLoopStartFunction(pThatForm.mLoopStartFunction),
     mLoopEndFunction(pThatForm.mLoopEndFunction),
     mParentMultiForm(pThatForm.mParentMultiForm),
     mUseInputStatus(pThatForm.mUseInputStatus),
     mHasBeenShown(pThatForm.mHasBeenShown),
     mExtendedHelpKeys(pThatForm.mExtendedHelpKeys),
     mUseExtendedHelpKeys(pThatForm.mUseExtendedHelpKeys),
     mAllValidateOnJumpKey(pThatForm.mAllValidateOnJumpKey),
     mInputShowBools(pThatForm.mInputShowBools),
     mInputJumpIndexes(pThatForm.mInputJumpIndexes)
{
   // Copy the cxWindow stuff from the other form
   cxWindow::copyCxWinStuff(&pThatForm, false);

   // Copy the other form's inputs
   freeInputs();
   shared_ptr<cxMultiLineInput> newInput; // Used in creating the copied inputs
   for (const shared_ptr<cxMultiLineInput>& thatInput : pThatForm.mInputs)
   {
      // Copy the input as a cxComboBox or a cxMultiLineInput,
      // depending on the type of the input.
      if (thatInput->cxTypeStr() == "cxComboBox")
      {
         newInput = make_shared<cxComboBox>(*((cxComboBox*)thatInput.get()));
      }
      else
      {
         newInput = make_shared<cxMultiLineInput>(*thatInput);
      }
      newInput->setParentForm(this);
      newInput->setUseExtendedHelpKeys(mUseExtendedHelpKeys);
      newInput->setExtendedHelpKeys(mExtendedHelpKeys);
      mInputs.push_back(newInput);
   }
} // Copy ctor

cxForm::~cxForm()
{
   // Anything based on cxWindow should disappear from the screen when it is
   //  destroyed (via ncurses or the cxWindow destructor).  But I ran into a
   //  situation where a cxForm wasn't disappearing when it was destroyed for
   //  some reason..  So, hide it here.
   if (cxBase::cxInitialized())
   {
      hide();
   }
   freeInputs();
} // dtor

shared_ptr<cxMultiLineInput> cxForm::append(int pRow, int pCol, int pHeight, int pWidth,
                                            const string& pLabel, const string& pValidator,
                                            const string& pHelpString, eInputOptions pInputOption,
                                            const string& pName, string *pExtValue,
                                            int pRightLabelOffset, int pRightLabelHeight,
                                            int pRightLabelWidth, bool pShowRightLabel,
                                            eInputType pInputType)
                                            {
   shared_ptr<cxMultiLineInput> input; // Stores the return value

   // If stacking is turned on, then append the input on its own line (ignoring
   //  some of the parameters passed to this function).  Otherwise, append where
   //  the user wants the input to be.
   if (mStacked)
   {
      input = append(pHeight, pWidth, pLabel, pValidator, pHelpString, pInputOption, pName, pExtValue);
   }
   else
   {
      // If the width is too big for the form, then
      //  shorten it.
      if (left()+pCol+pWidth > right())
      {
         pWidth = width() - pCol;
      }

      // If the height is too big for the form, then
      //  shorten it.
      if (top()+pRow+pHeight > bottom())
      {
         pHeight = height() - pRow;
      }

      try
      {
         // Create the input
         switch (pInputType)
         {
            case eIT_CXCOMBOBOX:  // cxComboBox
               input = make_shared<cxComboBox>(nullptr, top()+pRow, left()+pCol, pHeight,
                                               pWidth, pLabel.substr(0, pWidth),
                                               eBS_NOBORDER, pInputOption,
                                               eINPUT_TYPE_TEXT, pExtValue, true,
                                               pRightLabelOffset, pRightLabelHeight,
                                               pRightLabelWidth, pShowRightLabel);
               break;
            case eIT_CXMULTILINEINPUT: // cxMultiLineInput (default)
            default:
               input = make_shared<cxMultiLineInput>(nullptr, top()+pRow, left()+pCol, pHeight,
                                                     pWidth, pLabel.substr(0, pWidth),
                                                     eBS_NOBORDER, pInputOption,
                                                     eINPUT_TYPE_TEXT, pExtValue,
                                                     pRightLabelOffset, pRightLabelHeight,
                                                     pRightLabelWidth, pShowRightLabel);
               break;
         }

         // Set up the extended help keys for the input
         input->setUseExtendedHelpKeys(mUseExtendedHelpKeys);
         set<int>::iterator setIter = mExtendedHelpKeys.begin();
         for (; setIter != mExtendedHelpKeys.end(); ++setIter)
         {
            input->addExtendedHelpKey(*setIter);
         }

         // Add the input's coordinates to mInputPositions
         mInputPositions.push_back(pair<int, int>(top()+pRow, left()+pCol));

         // If there is no border, let inputs be shown along the edges.
         if (getBorderStyle() == eBS_NOBORDER)
         {
            mInputsOnBorder = true;
         }
         else
         {
            // If one of the edges of the input is along one of the
            // form borders, set inputsOnBorder to true - let the
            // user put inputs on borders and behave accordingly
            // during scrolling, etc.
            if ((input->top() == top()) || (input->bottom() == bottom()))
            {
               mInputsOnBorder = true;
            }
         }

         // If mApplyAttrDefaults is true, apply an attribute
         // to the value of the input so that it stands out
         // from the rest of the form.
         if (mApplyAttrDefaults)
         {
            applyAttrDefaults(input);
         }

         // Apply all the other attributes that have been applied to the
         // form to the new input so it has the same attributes as all the
         // others.
         applyWinAttributes(input);

         // If the validator string (pValidator) is "m", turn on masking for
         // the input.  Otherwise, set the validator string using pValidator.
         if (pValidator == "m")
         {
            input->toggleMasking(true);
         }
         else
         {
            input->setValidatorStr(pValidator);
         }

         input->setStatus(pHelpString, false);

         // Set the name of the input.
         input->setName(pName);

         // Make sure the input doesn't trap non-assigned function keys.
         //input->trapNonAssignedFKeys(false);

         // Add all form function keys to the input's list of keys that exit
         // its input loop.  Note that this passes (pFunctionKey, false, true)
         // to the input - the false is so that the input does not run its
         // onLeave function when it exits, and the true is to make sure that
         // the exit key gets set.
         for (const pair<const int, shared_ptr<cxFunction> >& funcPair : mKeyFunctions)
         {
            input->addExitKey(funcPair.first, false, true);
         }

         // Add all the form's quit keys & exit keys to
         // the input.
         map<int, bool>::const_iterator keyIter = mQuitKeys.begin();
         for (; keyIter != mQuitKeys.end(); ++keyIter)
         {
            input->addQuitKey(keyIter->first, keyIter->second);
         }
         keyIter = mExitKeys.begin();
         for (; keyIter != mExitKeys.end(); ++keyIter)
         {
            input->addExitKey(keyIter->first, keyIter->second);
         }
         // Add the menu jump hotkey to the input as an exit key
         input->addExitKey(mInputJumpKey, false, true);

         // If mAllValidateOnJumpKey is false, then add mInputJumpKey to the
         //  input's list of keys that skip the validator function.
         if (!mAllValidateOnJumpKey)
         {
            input->addSkipValidatorKey(mInputJumpKey);
         }

         // Set this form as the input's parent form.
         input->setParentForm(this);

         // Add the input to mInputs, and also add
         //  a boolean to mInputShowBools (default
         //  to true to have it show).
         mInputs.push_back(input);
         mInputShowBools.push_back(true);

         // Update mEditableInputsExist
         lookForEditableInputs();
      }
      catch (const std::bad_alloc& e)
      {
         cxBase::messageBox("Warning: Not enough memory to add an input to the form");
      }
      catch (...)
      {
         cxBase::messageBox("Warning: Unknown error trying to add an input to the form");
      }
   }

   return(input);
} // append

shared_ptr<cxComboBox> cxForm::appendComboBox(int pRow, int pCol, int pHeight,
                                              int pWidth, const string& pLabel,
                                              const string& pValidator, const string& pHelpString,
                                              eInputOptions pInputOption, const string& pName,
                                              string *pExtValue,
                                              int pRightLabelOffset, int pRightLabelHeight,
                                              int pRightLabelWidth, bool pShowRightLabel)
                                              {
   shared_ptr<cxMultiLineInput> input = append(pRow, pCol, pHeight,
                                               pWidth, pLabel, pValidator, pHelpString,
                                               pInputOption, pName, pExtValue,
                                               pRightLabelOffset, pRightLabelHeight,
                                               pRightLabelWidth, pShowRightLabel,
                                               eIT_CXCOMBOBOX);
   return dynamic_pointer_cast<cxComboBox>(input);
} // appendComboBox

cxMLInputPtrPair cxForm::appendPair(int pRow, int pCol, int pHeight, int pWidth,
         const string& pLabel, const string& pValidator,
         const string& pHelpString, eInputOptions pInputOption,
         const string& pName, string *pExtValue1, string *pExtValue2,
         int pRightLabel1Offset, int pRightLabel1Height, int pRightLabel1Width,
         bool pShowRightLabel1, int pRightLabel2Offset, int pRightLabel2Height,
         int pRightLabel2Width, bool pShowRightLabel2, eInputType pInput1Type,
         eInputType pInput2Type)
         {
   cxMLInputPtrPair inputs = make_pair<shared_ptr<cxMultiLineInput>, shared_ptr<cxMultiLineInput> >(nullptr, nullptr);

   // Create the inputs (they will be either cxMultiLineInput or cxComboBox,
   //  according to pInput1Type and pInput2Type).
   switch (pInput1Type)
   {
      case eIT_CXMULTILINEINPUT:
         inputs.first = make_shared<cxMultiLineInput>(nullptr, 0, 0, pHeight, pWidth,
                                            pLabel, eBS_NOBORDER, pInputOption,
                                            eINPUT_TYPE_TEXT, pExtValue1,
                                            pRightLabel1Offset,
                                            pRightLabel1Height,
                                            pRightLabel1Width,
                                            pShowRightLabel1);
         break;
      case eIT_CXCOMBOBOX:
         inputs.first = make_shared<cxComboBox>(nullptr, 0, 0, pHeight, pWidth, pLabel,
                                 eBS_NOBORDER, pInputOption, eINPUT_TYPE_TEXT,
                                 pExtValue1, true, pRightLabel1Offset,
                                 pRightLabel1Height, pRightLabel1Width,
                                 pShowRightLabel1);
         break;
   }
   switch (pInput2Type)
   {
      case eIT_CXMULTILINEINPUT:
         inputs.second = make_shared<cxMultiLineInput>(nullptr, 0, 0, pHeight, pWidth,
                                            pLabel, eBS_NOBORDER, pInputOption,
                                            eINPUT_TYPE_TEXT, pExtValue2,
                                            pRightLabel2Offset,
                                            pRightLabel2Height,
                                            pRightLabel2Width,
                                            pShowRightLabel2);
         break;
      case eIT_CXCOMBOBOX:
         inputs.second = make_shared<cxComboBox>(nullptr, 0, 0, pHeight, pWidth, pLabel,
                                 eBS_NOBORDER, pInputOption, eINPUT_TYPE_TEXT,
                                 pExtValue2, true, pRightLabel2Offset,
                                 pRightLabel2Height, pRightLabel2Width,
                                 pShowRightLabel2);
         break;
   }
   // Set the other options for the inputs
   inputs.first->setValidatorStr(pValidator);
   inputs.first->setStatus(pHelpString);
   inputs.second->setValidatorStr(pValidator);
   inputs.second->setStatus(pHelpString);
   // Call the other appendPair() to append the inputs to the form
   appendPair(inputs.first, inputs.second, pRow, pCol, pName);

   return(inputs);
} // appendPair

cxComboBoxPtrPair cxForm::appendComboBoxPair(int pRow, int pCol, int pHeight,
         int pWidth, const string& pLabel, const string& pValidator,
         const string& pHelpString, eInputOptions pInputOption,
         const string& pName, string *pExtValue1, string *pExtValue2,
         int pRightLabel1Offset, int pRightLabel1Height, int pRightLabel1Width,
         bool pShowRightLabel1, int pRightLabel2Offset, int pRightLabel2Height,
         int pRightLabel2Width, bool pShowRightLabel2)
         {
   cxMLInputPtrPair inputs = appendPair(pRow, pCol, pHeight, pWidth, pLabel,
                     pValidator, pHelpString, pInputOption, pName, pExtValue1,
                     pExtValue2, pRightLabel1Offset, pRightLabel1Height,
                     pRightLabel1Width, pShowRightLabel1, pRightLabel2Offset,
                     pRightLabel2Height, pRightLabel2Width, pShowRightLabel2,
                     eIT_CXCOMBOBOX, eIT_CXCOMBOBOX);

   // Cast the return value pointers to cxComboBox pointers
   cxComboBoxPtrPair comboBoxes = make_pair<shared_ptr<cxComboBox>, shared_ptr<cxComboBox> >(nullptr, nullptr);
   if (inputs.first != nullptr)
   {
      try
      {
         comboBoxes.first = dynamic_pointer_cast<cxComboBox>(inputs.first);
      }
      catch (const std::bad_cast& e)
      {
         throw cxWidgetsException("cxForm::appendComboBoxPair() - Could not "
                                  "cast first input to a cxComboBox: " +
                                  string(e.what()));
      }
      catch (...)
      {
         throw cxWidgetsException("cxForm::appendComboBoxPair() - Unknown "
                                  "exception caught (trying to cast first "
                                  "input to a cxComboBox).");
      }
   }
   if (inputs.second != nullptr)
   {
      try
      {
         comboBoxes.second = dynamic_pointer_cast<cxComboBox>(inputs.second);
      }
      catch (const std::bad_cast& e)
      {
         throw cxWidgetsException("cxForm::appendComboBoxPair() - Could not "
                                  "cast second input to a cxComboBox: " +
                                  string(e.what()));
      }
      catch (...)
      {
         throw cxWidgetsException("cxForm::appendComboBoxPair() - Unknown "
                                  "exception caught (trying to cast second "
                                  "input to a cxComboBox).");
      }
   }

   return(comboBoxes);
} // appendComboBoxPair

void cxForm::appendPair(shared_ptr<cxMultiLineInput>& pInput1,
                        shared_ptr<cxMultiLineInput>& pInput2,
                        int pRow, int pCol, const string& pName)
                        {
   // Append the first input
   if (pInput1 != nullptr)
   {
      append(pInput1, pRow, pCol);

      if (pInput2 != nullptr)
      {
         // Add a non-editable input that just contains "/"
         int col2 = pCol + pInput1->width();
         shared_ptr<cxMultiLineInput> input = append(pRow, col2, 1, 1, "/", "", "", eINPUT_READONLY, "");
         input->setCanBeEditable(false);

         // Blank the label of the second input, and resize it down
         int labelWidth = (int)(pInput2->getLabel().length());
         pInput2->setLabel("");
         pInput2->resize(pInput2->height(), pInput2->width() - labelWidth,
                         false);
         // Check how much width there is available for the second input
         int widthAvailable = right() - pInput1->right()+1;
         if (hasBorder())
         {
            --widthAvailable;
         }
         if (widthAvailable > 0)
         {
            // If there is less width available than the input is wide, then
            //  shrink the input.
            if (widthAvailable < pInput2->width())
            {
               pInput2->resize(pInput2->height(), widthAvailable);
            }
            // Append the second input
            ++col2;
            append(pInput2, pRow, col2);

            // Use pName if it's not blank; otherwise, use the name of the
            //  first input for both inputs, and add "_start" and "_end" to the
            //  names.
            string name = pName;
            if (name == "")
            {
               name = pInput1->getName();
            }
            pInput1->setName(name + "_start");
            pInput2->setName(name + "_end");

            // If there is not enough room on the form for the right labels,
            //  then shrink them so they will fit.
            int rightmostColumn = right(); // The right column limit
            if (hasBorder())
            {
               --rightmostColumn;
            }
            // Figure out what the rightmost column of the right labels would
            //  be after they are moved
            col2 = pInput2->right() + pInput1->getRightLabelWidth()
                 + pInput2->getRightLabelWidth() + 3;
            // If col2 is greater than the rightmost column limit, then resize
            //  the right labels so that they will fit in the form.
            if (col2 > rightmostColumn)
            {
               widthAvailable = rightmostColumn - pInput2->right() - 3;
               pInput1->setRightLabelWidth(widthAvailable / 2, false);
               pInput2->setRightLabelWidth(widthAvailable -
                                           pInput1->getRightLabelWidth(),
                                           false);
            }

            // Move the right labels of the inputs to be to the right
            pInput1->setRightLabelOffset(pInput2->width()+2, false);
            pInput2->setRightLabelOffset(pInput1->getRightLabelWidth()+2, false);

            // Add a "/" between the right labels, if either of the right
            //  labels are enabled.
            if ((pInput1->rightLabelEnabled()) || (pInput2->rightLabelEnabled()))
            {
               col2 = pInput1->width() + pInput2->width()
                    + pInput1->getRightLabelWidth() + 3;
               input = append(pRow, col2, 1, 1, "/", "", "", eINPUT_READONLY, "");
               input->setCanBeEditable(false);
            }
         }
      }
   }
} // appendPair

// Appends a multi-line input to the form below the
//  last item currently in the form.
shared_ptr<cxMultiLineInput> cxForm::append(int pHeight, int pWidth,
                                            const string& pLabel,
                                            const string& pValidator,
                                            const string& pHelpString,
                                            eInputOptions pInputOption, const string& pName,
                                            string *pExtValue, int pRightLabelOffset,
                                            int pRightLabelHeight, int pRightLabelWidth,
                                            bool pShowRightLabel, eInputType pInputType)
                                            {
   // If the width is <= 0 or greater than the width of
   //  the form, or if it's only as wide as the label,
   //  then make it as wide as the form.
   if (getBorderStyle() != eBS_NOBORDER)
   {
      if ((pWidth <= 0) || (pWidth > width()-2) ||
          (pWidth == (int)pLabel.length()))
          {
         pWidth = width()-2;
      }
   }
   else
   {
      if ((pWidth <= 0) || (pWidth > width()) ||
          (pWidth == (int)pLabel.length()))
          {
         pWidth = width();
      }
   }

   // If pHeight is <= 0, then set it to 1.
   if (pHeight <= 0)
   {
      pHeight = 1;
   }

   int row = top();        // The starting row for the new input
   int col = left();       // The starting column for the new input
   int formBottom = bottom(); // Used in some tests
   bool showInput = true;  // To be added to mInputShowBools
   // If there is a border, then row needs to be 1 character
   //  down from the top, and col needs to be 1 character
   //  to the right of the left border.  Also, the width of
   //  the input needs to be 2 less.
   if (getBorderStyle() != eBS_NOBORDER)
   {
      ++row;
      ++col;
   }

   // Look for the bottom-most row of all inputs and set
   //  the row for the new input below it.
   unsigned numInputs = mInputPositions.size();
   for (unsigned i = 0; i != numInputs; ++i)
   {
      int newRow = mInputPositions[i].first + mInputs[i]->height();
      if (newRow > row)
      {
         row = newRow;
      }
   }

   shared_ptr<cxMultiLineInput> input; // Holds the return value
   try
   {
      // Create the input
      switch (pInputType)
      {
         case eIT_CXCOMBOBOX:  // cxComboBox
            input = make_shared<cxComboBox>(nullptr, row, col, pHeight, pWidth, pLabel,
                                   eBS_NOBORDER, pInputOption, eINPUT_TYPE_TEXT,
                                   pExtValue, true, pRightLabelOffset,
                                   pRightLabelHeight, pRightLabelWidth,
                                   pShowRightLabel);
            break;
         case eIT_CXMULTILINEINPUT: // cxMultiLineInput (default)
         default:
            input = make_shared<cxMultiLineInput>(nullptr, row, col, pHeight, pWidth, pLabel,
                                         eBS_NOBORDER, pInputOption,
                                         eINPUT_TYPE_TEXT, pExtValue,
                                         pRightLabelOffset, pRightLabelHeight,
                                         pRightLabelWidth, pShowRightLabel);
            break;
      }

      // Set up the extended help keys for the input
      input->setUseExtendedHelpKeys(mUseExtendedHelpKeys);
      set<int>::iterator setIter = mExtendedHelpKeys.begin();
      for (; setIter != mExtendedHelpKeys.end(); ++setIter)
      {
         input->addExtendedHelpKey(*setIter);
      }

      // Add the input's coordinates to mInputPositions
      mInputPositions.push_back(pair<int, int>(row, col));
      // If mApplyAttrDefaults is true, apply an attribute
      //  to the value of the input so that it stands out
      //  from the rest of the form.
      if (mApplyAttrDefaults)
      {
         applyAttrDefaults(input);
      }

      // Apply all the other attributes that have been applied to the
      //  form to the new input so it has the same attributes as all the
      //  others.
      applyWinAttributes(input);

      // If the top row of the input is below the visible part of
      //  the form, then hide it.
      if (getBorderStyle() != eBS_NOBORDER)
      {
         // There's a border, so hide the input if it
         //  overlaps the bottom row.
         if (row == formBottom)
         {
            input->hide();
            showInput = false;
         }
      }
      else
      {
         if (row == (formBottom + 1))
         {
            input->hide();
            showInput = false;
         }
      }

      // If the validator string (pValidator) is "m", turn on masking for
      //  the input.  Otherwise, set the validator string using pValidator.
      if (pValidator == "m")
      {
         input->toggleMasking(true);
      }
      else
      {
         input->setValidatorStr(pValidator);
      }

      input->setStatus(pHelpString, false);

      // Set the name of the input.
      input->setName(pName);

      // Make sure the input doesn't trap non-assigned function keys.
      input->trapNonAssignedFKeys(false);

      // Add all form function keys to the input's list of keys that exit
      // its input loop.  Note that this passes (pFunctionKey, false, true)
      // to the input - the false is so that the input does not run its
      // onLeave function when it exits, and the true is to make sure that
      // the exit key gets set.
      for (const pair<const int, shared_ptr<cxFunction> >& funcPair : mKeyFunctions)
      {
         input->addExitKey(funcPair.first, false, true);
      }

      // Add all the form's quit keys & exit keys to the input.
      map<int, bool>::const_iterator keyIter = mQuitKeys.begin();
      for (; keyIter != mQuitKeys.end(); ++keyIter)
      {
         input->addQuitKey(keyIter->first, keyIter->second);
      }
      keyIter = mExitKeys.begin();
      for (; keyIter != mExitKeys.end(); ++keyIter)
      {
         input->addExitKey(keyIter->first, keyIter->second);
      }
      // Add the menu jump hotkey to the input as an exit key
      input->addExitKey(mInputJumpKey, false, true);

      // If mAllValidateOnJumpKey is false, then add mInputJumpKey to the
      //  input's list of keys that skip the validator function.
      if (!mAllValidateOnJumpKey)
      {
         input->addSkipValidatorKey(mInputJumpKey);
      }

      // Set this form as the input's parent form.
      input->setParentForm(this);

      // Add the input to mInputs, and also add
      //  a boolean to mInputShowBools.
      mInputs.push_back(input);
      mInputShowBools.push_back(showInput);

      // Update mEditableInputsExist
      lookForEditableInputs();

      // If the form is in "stacked" mode, add the
      //  new input's height to mTotalInputHeight.
      if (mStacked)
      {
         mTotalInputHeight += (unsigned)(input->height());
      }
   }
   catch (const std::bad_alloc& e)
   {
      cxBase::messageBox("Warning: Not enough memory to add an input to the form");
   }
   catch (...)
   {
      cxBase::messageBox("Warning: Unknown error trying to add an input to the form");
   }

   return(input);
} // append

shared_ptr<cxComboBox> cxForm::appendComboBox(int pHeight, int pWidth, const string& pLabel,
                     const string& pValidator, const string& pHelpString,
                     eInputOptions pInputOption, const string& pName,
                     string *pExtValue, int pRightLabelOffset,
                     int pRightLabelHeight, int pRightLabelWidth,
                     bool pShowRightLabel)
                     {
   shared_ptr<cxMultiLineInput> input = append(pHeight, pWidth, pLabel,
                                               pValidator, pHelpString, pInputOption,
                                               pName, pExtValue, pRightLabelHeight,
                                               pRightLabelWidth, pShowRightLabel,
                                               eIT_CXCOMBOBOX);
   return dynamic_pointer_cast<cxComboBox>(input);
} // appendComboBox

shared_ptr<cxMultiLineInput> cxForm::append(const cxMultiLineInput& input)
{
   shared_ptr<cxMultiLineInput> iInput = make_shared<cxMultiLineInput>(input);

   // Set up the extended help keys for the input
   iInput->setUseExtendedHelpKeys(mUseExtendedHelpKeys);
   set<int>::iterator setIter = mExtendedHelpKeys.begin();
   for (; setIter != mExtendedHelpKeys.end(); ++setIter)
   {
      iInput->addExtendedHelpKey(*setIter);
   }

   // Make sure the input has no parent window
   iInput->setParent(nullptr);

   // Make sure the input doesn't trap non-assigned function keys.
   iInput->trapNonAssignedFKeys(false);

   // Add all form function keys to the input's list of
   // keys that stop its input loop.
   for (const pair<const int, shared_ptr<cxFunction> >& funcPair : mKeyFunctions)
   {
      iInput->addExitKey(funcPair.first, false, true);
   }

   // Add all form function keys to the input's list of keys that exit
   //  its input loop.  Note that this passes (pFunctionKey, false, true)
   //  to the input - the false is so that the input does not run its
   //  onLeave function when it exits, and the true is to make sure that
   //  the exit key gets set.
   map<int, bool>::const_iterator keyIter = mQuitKeys.begin();
   for (; keyIter != mQuitKeys.end(); ++keyIter)
   {
      iInput->addQuitKey(keyIter->first, keyIter->second);
   }
   keyIter = mExitKeys.begin();
   for (; keyIter != mExitKeys.end(); ++keyIter)
   {
      iInput->addExitKey(keyIter->first, keyIter->second);
   }
   // Add the menu jump hotkey to the input as an exit key
   iInput->addExitKey(mInputJumpKey, false, true);

   // If mAllValidateOnJumpKey is false, then add mInputJumpKey to the
   //  input's list of keys that skip the validator function.
   if (!mAllValidateOnJumpKey)
   {
      iInput->addSkipValidatorKey(mInputJumpKey);
   }

   // Set this form as the input's parent form.
   iInput->setParentForm(this);

   // If mApplyAttrDefaults is true, apply an attribute
   //  to the value of the input so that it stands out
   //  from the rest of the form.
   if (mApplyAttrDefaults)
   {
      applyAttrDefaults(iInput);
   }

   // Apply all the other attributes that have been applied to the
   //  form to the new input so it has the same attributes as all the
   //  others.
   applyWinAttributes(iInput);

   // Add the input to mInputs, and add the other information
   //  to the proper member variables.
   mInputs.push_back(iInput);
   mInputShowBools.push_back(true);
   mInputPositions.push_back(pair<int, int>(iInput->top(), iInput->left()));

   // Update mEditableInputs
   lookForEditableInputs();

   return(iInput);
} // append

void cxForm::append(shared_ptr<cxMultiLineInput>& pInput, int pRow, int pCol, bool* pMoved)
{
   if (pInput != nullptr)
   {
      // Only append the input if it isn't already in mInputs.
      bool alreadyExists = false;
      for (const auto& input : mInputs)
      {
         if (input == pInput)
         {
            alreadyExists = true;
            break;
         }
      }
      if (!alreadyExists)
      {
         // Set up the extended help keys for the input
         pInput->clearExtendedHelpKeys();
         pInput->setUseExtendedHelpKeys(mUseExtendedHelpKeys);
         set<int>::iterator setIter = mExtendedHelpKeys.begin();
         for (; setIter != mExtendedHelpKeys.end(); ++setIter)
         {
            pInput->addExtendedHelpKey(*setIter);
         }

         // Move the input to the proper place (relative to the form)
         bool inputMoved = pInput->move(top()+pRow, left()+pCol, false);
         if (pMoved != nullptr)
         {
            *pMoved = inputMoved;
         }

         // Make sure the input has no parent window
         pInput->setParent(nullptr);

         // Make sure the input doesn't trap non-assigned function keys.
         pInput->trapNonAssignedFKeys(false);

         // Add all form function keys to the input's list of keys that exit
         // its input loop.  Note that this passes (pFunctionKey, false, true)
         // to the input - the false is so that the input does not run its
         // onLeave function when it exits, and the true is to make sure that
         // the exit key gets set.
         for (const pair<const int, shared_ptr<cxFunction> >& funcPair : mKeyFunctions)
         {
            pInput->addExitKey(funcPair.first, false, true);
         }

         // Add all the form's quit keys & exit keys to the input.
         for (const pair<const int, bool>& keyPair : mQuitKeys)
         {
            pInput->addQuitKey(keyPair.first, keyPair.second);
         }
         for (const pair<const int, bool>& keyPair : mExitKeys)
         {
            pInput->addExitKey(keyPair.first, keyPair.second);
         }

         // Add the menu jump hotkey to the input as an exit key
         pInput->addExitKey(mInputJumpKey, false, true);

         // If mAllValidateOnJumpKey is false, then add mInputJumpKey to the
         //  input's list of keys that skip the validator function.
         if (!mAllValidateOnJumpKey)
         {
            pInput->addSkipValidatorKey(mInputJumpKey);
         }

         // Set this form as the input's parent form.
         pInput->setParentForm(this);

         // If mApplyAttrDefaults is true, apply an attribute
         //  to the value of the input so that it stands out
         //  from the rest of the form.
         if (mApplyAttrDefaults)
         {
            applyAttrDefaults(pInput);
         }

         // Apply all the other attributes that have been applied to the
         //  form to the new input so it has the same attributes as all the
         //  others.
         applyWinAttributes(pInput);

         // Add the input to mInputs
         mInputs.push_back(pInput);
         mInputShowBools.push_back(true);
         mInputPositions.push_back(pair<int, int>(pInput->top(), pInput->left()));

         // Update mEditableInputs
         lookForEditableInputs();
      }
   }
} // append

void cxForm::remove(unsigned int pIndex)
{
   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      // If the form is in "stacked" mode, subtract the
      //  input's height from mTotalInputHeight.
      if (mStacked)
      {
         mTotalInputHeight -= (unsigned)(mInputs[pIndex]->height());
      }

      // Remove the pointer from mInputs, and remove the corresponding
      // stuff from mInputShowBools & mInputPositions.
      mInputs.erase(mInputs.begin()+pIndex);
      mInputShowBools.erase(mInputShowBools.begin()+pIndex);
      mInputPositions.erase(mInputPositions.begin()+pIndex);
      // TODO: If this is a "stacked" form, move the rest
      // of the inputs up.

      // Update mEditableInputsExist
      lookForEditableInputs();

      // Make sure mCurrentInput is still within bounds.
      if (mCurrentInput >= (int)mInputs.size())
      {
         mCurrentInput = (int)mInputs.size() - 1;
         if (mCurrentInput < 0) { // In case there are no inputs
            mCurrentInput = 0;
         }
      }
   }
} // remove

void cxForm::remove(const string& pLabel, bool pIsLabel)
{
   bool inputFound = false;  // Whether or not the input was found

   unsigned numInputs = mInputs.size();
   for (unsigned i = 0; i < numInputs; ++i)
   {
      // If isLabel is true, match by label; otherwise, match by name.
      if (pIsLabel)
      {
         // Check the label text verbatim and the label text
         //  without the underlining ampersands
         if ((mInputs[i]->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(mInputs[i]->getLabel()) == pLabel))
             {
            inputFound = true;

            // If the form is in "stacked" mode, subtract the
            //  input's height from mTotalInputHeight.
            if (mStacked)
            {
               mTotalInputHeight -= (unsigned)(mInputs[i]->height());
            }

            // Erase the window from mMenuItems and all info
            // relating to it in mInputShowBools and
            // mInputPositions
            mInputs.erase(mInputs.begin()+i);
            mInputShowBools.erase(mInputShowBools.begin()+i);
            mInputPositions.erase(mInputPositions.begin()+i);
            // TODO: If this is a "stacked" form, move the rest
            //  of the inputs up.
            break;
         }
      }
      else
      {
         // Check the name text verbatim and the label text
         //  without the underlining ampersands
         if ((mInputs[i]->getName() == pLabel) ||
             (stringWithoutHotkeyChars(mInputs[i]->getLabel()) == pLabel))
             {
            inputFound = true;

            // If the form is in "stacked" mode, subtract the
            //  input's height from mTotalInputHeight.
            if (mStacked)
            {
               mTotalInputHeight -= (unsigned)(mInputs[i]->height());
            }

            // Erase the window from mMenuItems and all info
            // relating to it in mInputShowBools and
            // mInputPositions
            mInputs.erase(mInputs.begin()+i);
            mInputShowBools.erase(mInputShowBools.begin()+i);
            mInputPositions.erase(mInputPositions.begin()+i);
            break;
            // TODO: If this is a "stacked" form, move the rest
            //  of the inputs up.
         }
      }
   }

   if (inputFound)
   {
      // Update mEditableInputsExist
      lookForEditableInputs();

      // Make sure mCurrentInput is still within bounds.
      if (mCurrentInput >= (int)mInputs.size())
      {
         mCurrentInput = (int)mInputs.size() - 1;
         if (mCurrentInput < 0) { // In case there are no inputs
            mCurrentInput = 0;
         }
      }
   }
} // remove

void cxForm::remove(const shared_ptr<cxMultiLineInput>& pInput)
{
   remove(pInput.get());
} // remove

void cxForm::remove(cxMultiLineInput *pInput)
{
   bool inputFound = false;  // Whether or not the input was found

   unsigned numInputs = mInputs.size();
   for (unsigned i = 0; i < numInputs; ++i)
   {
      if (mInputs[i].get() == pInput)
      {
         inputFound = true;

         // If the form is in "stacked" mode, subtract the
         //  input's height from mTotalInputHeight.
         if (mStacked)
         {
            mTotalInputHeight -= (unsigned)(mInputs[i]->height());
         }

         // Erase the window from mMenuItems and all info
         // relating to it in mInputShowBools and
         // mInputPositions
         mInputs.erase(mInputs.begin()+i);
         mInputShowBools.erase(mInputShowBools.begin()+i);
         mInputPositions.erase(mInputPositions.begin()+i);
         // TODO: If this is a "stacked" form, move the rest
         //  of the inputs up.
         break;
      }
   }

   if (inputFound)
   {
      // Update mEditableInputsExist
      lookForEditableInputs();

      // Make sure mCurrentInput is still within bounds.
      if (mCurrentInput >= (int)mInputs.size())
      {
         mCurrentInput = (int)mInputs.size() - 1;
         if (mCurrentInput < 0) { // In case there are no inputs
            mCurrentInput = 0;
         }
      }
   }
} // remove

void cxForm::removeAll()
{
   freeInputs();
   mEditableInputsExist = false;
   mCurrentInput = 0;
} // removeAll

void cxForm::setFieldKeyFunction(const string& pLabel, int pFunctionKey,
                            const shared_ptr<cxFunction>& pFieldFunction,
                            bool pIsLabel)
                            {
   // If there are multiple inputs with the same label/name, all of them
   // will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel || stringWithoutHotkeyChars(input->getLabel()) == pLabel)
         {
            input->setKeyFunction(pFunctionKey, pFieldFunction);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setKeyFunction(pFunctionKey, pFieldFunction);
         }
      }
   }
} // setFieldKeyFunction

void cxForm::setFieldKeyFunction(const string& pLabel, int pFunctionKey,
                              funcPtr4 pFieldFunction, void *p1, void *p2,
                              void *p3, void *p4, bool pUseVal, bool pExitAfterRun,
                              bool pIsLabel)
                              {
   // If there are multiple inputs with the same label/name, all of them
   // will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel || (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
         {
            input->setKeyFunction(pFunctionKey, pFieldFunction, p1, p2, p3, p4,
                                      pUseVal, pExitAfterRun);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setKeyFunction(pFunctionKey, pFieldFunction, p1, p2, p3, p4,
                                      pUseVal, pExitAfterRun);
         }
      }
   }
} // setFieldKeyFunction

void cxForm::setFieldKeyFunction(unsigned int pIndex, int pFunctionKey,
                                 const shared_ptr<cxFunction>& pFieldFunction)
                                 {
   if (pIndex >= 0 && pIndex < mInputs.size())
   {
      mInputs[pIndex]->setKeyFunction(pFunctionKey, pFieldFunction);
   }
} // setFieldKeyFunction

void cxForm::setFieldKeyFunction(unsigned int pIndex, int pFunctionKey,
                              funcPtr4 pFieldFunction,
                              void *p1, void *p2, void *p3, void *p4,
                              bool pUseVal, bool pExitAfterRun)
                              {
   if (pIndex >= 0 && pIndex < mInputs.size())
   {
      mInputs[pIndex]->setKeyFunction(pFunctionKey, pFieldFunction, p1, p2, p3, p4, pUseVal, pExitAfterRun);
   }
} // setFieldKeyFunction

void cxForm::setFieldKeyFunction(const string& pLabel, int pFunctionKey,
                              funcPtr2 pFieldFunction, void *p1, void *p2,
                              bool pUseVal, bool pExitAfterRun,
                              bool pIsLabel)
                              {
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setKeyFunction(pFunctionKey, pFieldFunction, p1, p2,
                                      pUseVal, pExitAfterRun);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setKeyFunction(pFunctionKey, pFieldFunction, p1, p2,
                                      pUseVal, pExitAfterRun);
         }
      }
   }
} // setFieldKeyFunction

// Sets a function to be executed via a keypress for
//  one of the fields (by index).
void cxForm::setFieldKeyFunction(unsigned pIndex, int pFunctionKey, funcPtr2 pFieldFunction,
                              void *p1, void *p2, bool pUseVal, bool pExitAfterRun)
                              {
   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      mInputs[pIndex]->setKeyFunction(pFunctionKey, pFieldFunction, p1, p2, pUseVal, pExitAfterRun);
   }
}

void cxForm::setOnFocusFunction(const shared_ptr<cxFunction>& pFunction)
{
   cxWindow::setOnFocusFunction(pFunction);
}

void cxForm::setOnFocusFunction(const string& pLabel, const shared_ptr<cxFunction>& pFunction, bool pIsLabel)
{
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setOnFocusFunction(pFunction);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setOnFocusFunction(pFunction);
         }
      }
   }
} // setOnFocusFunction

void cxForm::setOnFocusFunction(unsigned pIndex, const shared_ptr<cxFunction>& pFunction)
{
   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      mInputs[pIndex]->setOnFocusFunction(pFunction);
   }
} // setOnFocusFunction

void cxForm::setOnFocusFunction(const string& pLabel, funcPtr4 pFunction, void *p1,
                                void *p2, void *p3, void *p4, bool pUseVal,
                                bool pIsLabel)
                                {
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setOnFocusFunction(pFunction, p1, p2, p3, p4, pUseVal);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setOnFocusFunction(pFunction, p1, p2, p3, p4, pUseVal);
         }
      }
   }
} // setOnFocusFunction

void cxForm::setOnFocusFunction(unsigned pIndex, funcPtr4 pFunction, void *p1, void *p2, void *p3, void *p4, bool pUseVal)
{
   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      mInputs[pIndex]->setOnFocusFunction(pFunction, p1, p2, p3, p4, pUseVal);
   }
} // setOnFocusFunction

void cxForm::setOnLeaveFunction(const shared_ptr<cxFunction>& pFunction)
{
   cxWindow::setOnLeaveFunction(pFunction);
} // setOnLeaveFunction

void cxForm::setOnLeaveFunction(const string& pLabel, const shared_ptr<cxFunction>& pFunction,
                                bool pIsLabel)
                                {
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setOnLeaveFunction(pFunction);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setOnLeaveFunction(pFunction);
         }
      }
   }
} // setOnLeaveFunction

void cxForm::setOnLeaveFunction(unsigned pIndex, const shared_ptr<cxFunction>& pFunction)
{
   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      mInputs[pIndex]->setOnLeaveFunction(pFunction);
   }
} // setOneLeaveFunction

void cxForm::setOnLeaveFunction(const string& pLabel, funcPtr4 pFunction, void *p1,
                                void *p2, void *p3, void *p4, bool pIsLabel)
                                {
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setOnLeaveFunction(pFunction, p1, p2, p3, p4);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setOnLeaveFunction(pFunction, p1, p2, p3, p4);
         }
      }
   }
} // setOnLeaveFunction

void cxForm::setOnLeaveFunction(unsigned pIndex, funcPtr4 pFunction, void *p1, void *p2, void *p3, void *p4)
{
   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      mInputs[pIndex]->setOnLeaveFunction(pFunction, p1, p2, p3, p4);
   }
} // setOnLeaveFunction

void cxForm::setFieldLabel(unsigned pIndex, const string& pLabel)
{
   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      mInputs[pIndex]->setLabel(pLabel);
   }
} // setFieldLabel

void cxForm::setFieldLabel(const string& pName, const string& pLabel)
{
   for (const auto& input : mInputs)
   {
      if (input->getName() == pName)
      {
         input->setLabel(pLabel);
      }
   }
} // setFieldLabel

void cxForm::setFieldName(unsigned pIndex, const string& pName)
{
   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      mInputs[pIndex]->setName(pName);
   }
} // setFieldName

void cxForm::setFieldName(const string& pLabel, const string& pName)
{
   for (const auto& input : mInputs)
   {
      if (input->getLabel() == pLabel)
      {
         input->setName(pName);
      }
   }
} // setFieldName

void cxForm::enableInputLoop(const string& pLabel, bool pDoInputLoop, bool pIsLabel)
{
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->enableInputLoop(pDoInputLoop);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->enableInputLoop(pDoInputLoop);
         }
      }
   }
} // enableInputLoop

void cxForm::enableInputLoop(unsigned pIndex, bool pDoInputLoop)
{
   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      mInputs[pIndex]->enableInputLoop(pDoInputLoop);
   }
} // enableInputLoop

void cxForm::setAutoExit(bool pAutoExit)
{
   mAutoExit = pAutoExit;
} // setAutoExit

bool cxForm::getAutoExit() const
{
   return(mAutoExit);
} // getAutoExit

void cxForm::setStartAtFirstInput(bool pStartAtFirstInput)
{
   mStartAtFirstInput = pStartAtFirstInput;
} // setStartAtFirstInput

bool cxForm::getStartAtFirstInput() const
{
   return(mStartAtFirstInput);
} // getStartAtFirstInput

void cxForm::setApplyAttrDefaults(bool pApplyAttrDefaults)
{
   mApplyAttrDefaults = pApplyAttrDefaults;
} // setApplyAttrDefaults

bool cxForm::getApplyAttrDefaults() const
{
   return(mApplyAttrDefaults);
} // getApplyAttrDefaults

long cxForm::show(bool pBringToTop, bool pShowSubwindows)
{
   long retval = cxID_EXIT;

   // If the form is enabled, show it & show all inputs.
   //  Otherwise, hide the window & all inputs.
   if (isEnabled())
   {
      // Show this window before showing the inputs if
      //  getShowSelfBeforeSubwins() returns true
      if (getShowSelfBeforeSubwins())
      {
         retval = cxWindow::show(pBringToTop, pShowSubwindows);
         // Set pBringToTop to true so that when the inputs are shown,
         //  they will appear above the cxForm window.
         pBringToTop = true;
      }

      // Show the inputs
      // TODO: experiment 10-24-06 billm
      //if (!pShowSubwindows) {
         showAllInputs(pBringToTop, false);
      //}

      // Show this window now if getShowSelfBeforeSubwins() returns false
      if (!getShowSelfBeforeSubwins())
      {
         retval = cxWindow::show(pBringToTop, pShowSubwindows);
      }

      // The form has now been shown.
      mHasBeenShown = true;
   }
   else
   {
      hide(false);
   }

   return(retval);
} // show

long cxForm::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   setReturnCode(cxID_EXIT);

   // Do the input loop if the form is enabled.
   if (isEnabled())
   {
      // Run the onFocus function.  If runOnFocusFunction() returns true, that
      //  means we should exit.. so only do the input loop if it returns false.
      //  Also, check to make sure that getLeaveNow() returns false, in case
      //  the onFocus function called exitNow() or quitNow().
      if (!runOnFocusFunction() && !getLeaveNow())
      {
         if (pShowSelf)
         {
            show(pBringToTop, pShowSubwindows);
         }
         bool runTheOnLeaveFunction = true;
         // Set mIsModal true here (this should be done after the onFocus
         //  function runs, in case it calls setCurrentInput()).
         mIsModal = true;
         setReturnCode(doInputLoop(runTheOnLeaveFunction));
         mIsModal = false;
         // Run the onLeave function, if runTheOnLeaveFunction is true
         if (runTheOnLeaveFunction)
         {
            runOnLeaveFunction();
         }
      }
      else
      {
         mIsModal = false;
      }
   }

   return(getReturnCode());
} // showModal

bool cxForm::modalGetsKeypress() const
{
   bool willGetKeypress = false;

   if (isEnabled())
   {
      if (hasEditableInputs())
      {
         willGetKeypress = true;
      }
      else
      {
         willGetKeypress = mWaitForInputIfEmpty;
      }
   }

   return(willGetKeypress);
} // modalGetsKeypress

void cxForm::hide(bool pHideSubwindows)
{
   // Hide the main window
   cxWindow::hide(pHideSubwindows);
   // Hide the inputs
   for (const auto& input : mInputs)
   {
      input->hide(false);
   }
} // hide

void cxForm::unhide(bool pUnhideSubwindows)
{
   // If the form is enabled, un-hide the form window,
   //  as well as all the inputs.
   if (isEnabled())
   {
      cxWindow::unhide(pUnhideSubwindows);

      for (const auto& input : mInputs)
      {
         // If the form hasn't been shown yet, then the inputs wouldn't have
         //  been shown..  So tell the input to show itself, to make sure that
         //  text has been written into the input window before it gets shown.
         if (!mHasBeenShown)
         {
            input->show(false, false);
         }
         // TODO:  experiment!!!
         else
         {
            input->unhide(pUnhideSubwindows);
         }
      }
   }
} // unhide

bool cxForm::move(int pNewRow, int pNewCol, bool pRefresh)
{
   // In case the moving any of the inputs fails..
   int oldRow = top();
   int oldCol = left();

   // Figure out the difference between the current
   //  top left corner and the new row & column
   int rowDiff = pNewRow - top();
   int colDiff = pNewCol - left();

   // Move the window
   bool moved = cxWindow::move(pNewRow, pNewCol, false);

   // If the window move was successful, move the input
   //  windows (and update the coordinates in mInputPositions too)
   if (moved)
   {
      // Save the positions of the single-line inputs
      //  in case moving one of the fails, so we can
      //  move the rest back to where they were
      vector<pair<int, int> > oldInputPositions;

      unsigned numInputs = mInputs.size();
      for (unsigned i = 0; i < numInputs; ++i)
      {
         // Store the input's position in
         //  oldInputPositions
         int inputTop = mInputs[i]->top();
         int inputLeft = mInputs[i]->left();
         oldInputPositions.push_back(pair<int, int>(inputTop, inputLeft));
         // Move the input
         moved = mInputs[i]->moveRelative(rowDiff, colDiff, false);
         // If the move was successful, update the coordinates in
         //  mInputPositions too.  If not, move all the previously-
         //  moved inputs back to where they were.
         if (moved)
         {
            mInputPositions[i].first = mInputs[i]->top();
            mInputPositions[i].second = mInputs[i]->left();
         }
         else
         {
            unsigned vectorSize = oldInputPositions.size();
            for (unsigned i = 0; i < vectorSize; ++i)
            {
               mInputs[i]->move(oldInputPositions[i].first,
                                oldInputPositions[i].second, false);
               mInputPositions[i].first = oldInputPositions[i].first;
               mInputPositions[i].second = oldInputPositions[i].second;
            }
            break;
         }
      }
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

size_t cxForm::numInputs() const
{
   return(mInputs.size());
}

// Returns the value associated with a particular
//  input (by index).
string cxForm::getValue(int pIndex) const
{
   if (pIndex >= 0 && pIndex < (int)mInputs.size())
   {
      return(mInputs[pIndex]->getValue());
   }
   else
   {
      return("");
   }
} // getValue

// Returns the value associated with a particular
//  input (by label or name).
string cxForm::getValue(const string& pStr, bool pIsLabel) const
{
   // Search through mInputs for the input with a
   //  matching label/name, and if found, return the
   //  input's value.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pStr) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pStr))
             {
            return(input->getValue());
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pStr)
         {
            return(input->getValue());
         }
      }
   }

   // No value was returned by this point..  That means
   //  no match was found.
   return("");
} // getValue

string cxForm::getLabel(int pIndex) const
{
   string label;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      label = mInputs[pIndex]->getLabel();
   }

   return(label);
} // getLabel

string cxForm::getName(int pIndex) const
{
   string name;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      name = mInputs[pIndex]->getName();
   }

   return(name);
} // getName

string cxForm::getName(const string& pLabel) const
{
   string name;

   for (const auto& input : mInputs)
   {
      if (input->getLabel() == pLabel)
      {
         name = input->getName();
         break;
      }
   }

   return(name);
} // getName

string cxForm::getStatus(int pIndex) const
{
   string status;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      status = mInputs[pIndex]->getStatus();
   }

   return(status);
} // getStatus

string cxForm::getStatus(const string& pLabel, bool pIsLabel) const
{
   string status;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            status = input->getStatus();
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            status = input->getStatus();
         }
      }
   }

   return(status);
} // getStatus

// Sets the value of an input (by index).  Returns
//  true if it succeeded or false if not.
bool cxForm::setValue(int pIndex, const string& pValue, bool pRefresh)
{
   bool returnVal = false;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      // Get the old value and compare it with the new value afterwards, and
      //  set mDataChanged true if it changed.
      string oldValue = mInputs[pIndex]->getValue();
      mInputs[pIndex]->setValue(pValue, pRefresh);
      if (oldValue != pValue)
      {
         mDataChanged = true;
      }
      returnVal = true;
   }

   return(returnVal);
} // setValue

// Sets the value of an input (by label/name).  Returns
//  true if it succeeded or false if not.
bool cxForm::setValue(const string& pLabel, const string& pValue, bool pIsLabel, bool pRefresh)
{
   bool returnVal = false;

   // If there are multiple inputs with the same label/name, all of them
   //  will be changed.
   int numInputs = (int)(mInputs.size());
   if (pIsLabel)
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if ((mInputs[i]->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(mInputs[i]->getLabel()) == pLabel))
             {
            setValue(i, pValue, pRefresh);
            returnVal = true;
         }
      }
   }
   else
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if (mInputs[i]->getName() == pLabel)
         {
            setValue(i, pValue, pRefresh);
            returnVal = true;
         }
      }
   }

   return(returnVal);
} // setValue

int cxForm::refreshInput(unsigned pIndex)
{
   int returnVal = 0;

   if ((pIndex >= 0) && (pIndex < mInputs.size()))
   {
      // Show the input (to refresh it) - And bring it
      //  to the top if the form window is above the
      //  input's window.
      mInputs[pIndex]->show(isAbove(*(mInputs[pIndex])), false);
   }

   return(returnVal);
} // refreshInput

int cxForm::refreshInput(const string& pStr, bool pIsLabel)
{
   int returnVal = 0;

   // Check the input labels or names, depending on the
   //  value of pIsLabel.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pStr ||
             (stringWithoutHotkeyChars(input->getLabel()) == pStr))
             {
            input->show(true, false);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pStr)
         {
            input->show(true, false);
         }
      }
   }


   return(returnVal);
} // refreshInput

bool cxForm::setMaskChar(int pIndex, char pMaskChar)
{
   bool returnVal = true;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->setMaskChar(pMaskChar);
   }
   else
   {
      returnVal = false;
   }

   return(returnVal);
} // setMaskChar

bool cxForm::setMaskChar(const string& pLabel, char pMaskChar, bool pIsLabel)
{
   bool returnVal = false;

   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setMaskChar(pMaskChar);
            returnVal = true;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setMaskChar(pMaskChar);
            returnVal = true;
         }
      }
   }

   return(returnVal);
} // setMaskChar

bool cxForm::getMasked(int pIndex)
{
   bool masked = false;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      masked = mInputs[pIndex]->getMasked();
   }

   return(masked);
} // getMasked

bool cxForm::getMasked(const string& pLabel, bool pIsLabel)
{
   bool masked = false;

   //inputPtrContainer::iterator iter;
   for (const auto& input : mInputs)
   {
      if ((input->getLabel() == pLabel) ||
          (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
          {
         masked = input->getMasked();
         break;
      }
   }

   return(masked);
} // getMasked

bool cxForm::toggleMasking(int pIndex, bool pMasking)
{
   bool returnVal = false;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->toggleMasking(pMasking);
      returnVal = true;
   }

   return(returnVal);
} // toggleMasking

bool cxForm::toggleMasking(const string& pLabel, bool pMasking, bool pIsLabel)
{
   bool returnVal = false;

   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel)
         {
            input->toggleMasking(pMasking);
            returnVal = true;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->toggleMasking(pMasking);
            returnVal = true;
         }
      }
   }

   return(returnVal);
} // toggleMasking

bool cxForm::setCurrentInput(int pIndex)
{
   bool retval = false;

   // Check to see that pIndex is within bounds before using it.
   if ((pIndex >= 0) && (pIndex < (int)(mInputs.size())))
   {
      // If the input is editable, go ahead and use it as the current input.
      if (mInputs[pIndex]->isEditable()) {
         mCurrentInput = pIndex;

         // If the form is modal, then don't increment/decrement
         //  mCurrentInput in the next iteration through the input loop.
         mCycleInput = !mIsModal;

         retval = true;
      }
   }

   return(retval);
} // setCurrentInput

bool cxForm::setCurrentInput(const string& pLabel, bool pIsLabel)
{
   bool retval = false;

   int numInputs = (int)mInputs.size();
   if (pIsLabel)
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if ((mInputs[i]->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(mInputs[i]->getLabel()) == pLabel))
             {
            // We found it!  But only set it as the current input if it's
            //  editable.
            if (mInputs[i]->isEditable()) {
               mCurrentInput = i;
               // If the form is modal, then don't increment/decrement
               //  mCurrentInput in the next iteration through the input loop.
               mCycleInput = !mIsModal;

               retval = true;
            }
            break;
         }
      }
   }
   else
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if (mInputs[i]->getName() == pLabel)
         {
            // We found it!  But only set it as the current input if it's
            //  editable.
            if (mInputs[i]->isEditable()) {
               mCurrentInput = i;
               // If the form is modal, then don't increment/decrement
               //  mCurrentInput in the next iteration through the input loop.
               mCycleInput = !mIsModal;

               retval = true;
            }
            break;
         }
      }
   }

   return(retval);
} // setCurrentInput

bool cxForm::setCurrentInputByPtr(const shared_ptr<cxMultiLineInput>& pInput)
{
   return setCurrentInputByPtr(pInput.get());
} // setCurrentInputByPtr

bool cxForm::setCurrentInputByPtr(const cxMultiLineInput* const pInput)
{
   bool retval = false;

   int numInputs = (int)mInputs.size();
   for (int i = 0; i < numInputs; ++i)
   {
      if (mInputs[i].get() == pInput)
      {
         mCurrentInput = i;
         // If the form is modal, then don't increment/decrement
         //  mCurrentInput in the next iteration through the input loop.
         mCycleInput = !mIsModal;
         retval = true;
         break;
      }
   }

   return(retval);
} // setCurrentInputByPtr

bool cxForm::setKeyFunction(int pKey, const shared_ptr<cxFunction>& pFunction)
{
   bool setIt = cxWindow::setKeyFunction(pKey, pFunction);
   if (setIt)
   {
      // Add the key to each input's list of keys that exit its input loop.
      // Note that this passes (pKey, false, true) to the input -
      // the false is so that the input does not run its onLeave function
      // when it exits, and the true is to make sure that the exit key gets
      // set.
      for (const auto& input : mInputs)
      {
         input->addExitKey(pKey, false, true);
      }
      // Make sure the key doesn't exist in the quitKey list. Also,
      // make sure it doesn't exist in this window's exit key list
      // (but we want to keep it in each single-line input's exit
      // key list). Don't call removeExitKey() on this window
      // because that will remove it from this window's exit key
      // list as well as each of the single-line inputs
      removeQuitKey(pKey);
      //removeExitKey(pKey);
      cxWindow::removeExitKey(pKey);
   }

   return(setIt);
} // setKeyFunction

void cxForm::clearKeyFunction(int pKey)
{
   cxWindow::clearKeyFunction(pKey);
   // Remove the key from each input's list of keys that exit its input loop,
   // as that wouldn't be needed anymore.
   for (const auto& input : mInputs)
   {
      input->removeExitKey(pKey);
   }
} // clearKeyFunction

bool cxForm::setKeyFunction(int pKey, funcPtr4 pFunction,
                            void *p1, void *p2, void *p3, void *p4,
                            bool pUseReturnVal, bool pExitAfterRun,
                            bool pRunOnLeaveFunction)
                            {
   bool setIt = cxWindow::setKeyFunction(pKey, pFunction, p1,
                           p2, p3, p4, pUseReturnVal, pExitAfterRun,
                           pRunOnLeaveFunction);
   if (setIt)
   {
      // Add the key to each input's list of keys that exit its input loop.
      // Note that this passes (pKey, false, true) to the input -
      // the false is so that the input does not run its onLeave function
      // when it exits, and the true is to make sure that the exit key gets
      // set.
      for (const auto& input : mInputs)
      {
         input->addExitKey(pKey, false, true);
      }
      // Make sure the key doesn't exist in the quitKey and exitKey lists.
      removeQuitKey(pKey);
      removeExitKey(pKey);
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(int pKey, funcPtr2 pFunction,
                            void *p1, void *p2, bool pUseReturnVal,
                            bool pExitAfterRun, bool pRunOnLeaveFunction)
                            {
   bool setIt = cxWindow::setKeyFunction(pKey, pFunction, p1,
                      p2, pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction);
   if (setIt)
   {
      // Add the key to each input's list of keys that exit its input loop.
      //  Note that this passes (pKey, false, true) to the input -
      //  the false is so that the input does not run its onLeave function
      //  when it exits, and the true is to make sure that the exit key gets
      //  set.
      for (const auto& input : mInputs)
      {
         input->addExitKey(pKey, false, true);
      }

      // Make sure the key doesn't exist in the quitKey and exitKey lists.
      removeQuitKey(pKey);
      removeExitKey(pKey);
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(int pKey, funcPtr0 pFunction,
                                  bool pUseReturnVal,
                                  bool pExitAfterRun,
                                  bool pRunOnLeaveFunction)
                                  {
   bool setIt = cxWindow::setKeyFunction(pKey, pFunction,
                           pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction);
   if (setIt)
   {
      // Add the key to each input's list of keys that exit its input loop.
      // Note that this passes (pKey, false, true) to the input -
      // the false is so that the input does not run its onLeave function
      // when it exits, and the true is to make sure that the exit key gets
      // set.
      for (const auto& input : mInputs)
      {
         input->addExitKey(pKey, false, true);
      }

      // Make sure the key doesn't exist in the quitKey and exitKey lists.
      removeQuitKey(pKey);
      removeExitKey(pKey);
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(int pIndex, int pKey, const shared_ptr<cxFunction>& pFunction)
{
   bool setIt = false;
   if (pIndex >= 0 && pIndex < (int)(mInputs.size()))
   {
      setIt = mInputs[pIndex]->setKeyFunction(pKey, pFunction);
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(const string& pLabel, int pKey, const shared_ptr<cxFunction>& pFunction, bool pIsLabel)
{
   bool setIt = false;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel)
         {
            input->setKeyFunction(pKey, pFunction);
            setIt = true;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setKeyFunction(pKey, pFunction);
            setIt = true;
         }
      }
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(int pIndex, int pKey,
                            funcPtr4 pFunction, void *p1, void *p2,
                            void *p3, void *p4, bool pUseVal,
                            bool pExitAfterRun, bool pRunOnLeaveFunction,
                            bool pRunValidator)
                            {
   bool setIt = false;

   if (pIndex >= 0 && pIndex < (int)(mInputs.size()))
   {
      setIt = mInputs[pIndex]->setKeyFunction(pKey, pFunction, p1, p2,
                                           p3, p4, pUseVal, pExitAfterRun,
                                           pRunOnLeaveFunction, pRunValidator);
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(const string& pLabel, int pKey,
                            funcPtr4 pFunction, void *p1, void *p2,
                            void *p3, void *p4, bool pUseVal,
                            bool pExitAfterRun, bool pRunOnLeaveFunction,
                            bool pRunValidator, bool pIsLabel)
                            {
   bool setIt = false;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel)
         {
            input->setKeyFunction(pKey, pFunction, p1, p2, p3, p4,
                                    pUseVal, pExitAfterRun, pRunOnLeaveFunction,
                                    pRunValidator);
            setIt = true;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setKeyFunction(pKey, pFunction, p1, p2, p3, p4,
                                    pUseVal, pExitAfterRun, pRunOnLeaveFunction,
                                    pRunValidator);
            setIt = true;
         }
      }
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(int pIndex, int pKey,
                            funcPtr2 pFunction, void *p1, void *p2,
                            bool pUseVal, bool pExitAfterRun,
                            bool pRunOnLeaveFunction, bool pRunValidator)
                            {
   bool setIt = false;

   if ((pIndex >= 0) && (pIndex < (int)(mInputs.size())))
   {
      setIt = mInputs[pIndex]->setKeyFunction(pKey, pFunction, p1, p2,
                                           pUseVal, pExitAfterRun,
                                           pRunOnLeaveFunction, pRunValidator);
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(const string& pLabel, int pKey,
                            funcPtr2 pFunction, void *p1, void *p2,
                            bool pUseVal, bool pExitAfterRun,
                            bool pRunOnLeaveFunction, bool pRunValidator,
                            bool pIsLabel)
                            {
   bool setIt = false;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel)
         {
            input->setKeyFunction(pKey, pFunction, p1, p2, pUseVal,
                                    pExitAfterRun, pRunOnLeaveFunction,
                                    pRunValidator);
            setIt = true;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setKeyFunction(pKey, pFunction, p1, p2, pUseVal,
                                    pExitAfterRun, pRunOnLeaveFunction,
                                    pRunValidator);
            setIt = true;
         }
      }
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(int pIndex, int pKey, funcPtr0 pFunction,
                            bool pUseVal, bool pExitAfterRun,
                            bool pRunOnLeaveFunction, bool pRunValidator)
                            {
   bool setIt = false;

   if ((pIndex >= 0) && (pIndex < (int)(mInputs.size())))
   {
      setIt = mInputs[pIndex]->setKeyFunction(pKey, pFunction,
                                           pUseVal, pExitAfterRun,
                                           pRunOnLeaveFunction, pRunValidator);
   }

   return(setIt);
} // setKeyFunction

bool cxForm::setKeyFunction(const string& pLabel, int pKey, funcPtr0 pFunction,
                            bool pUseVal, bool pExitAfterRun,
                            bool pRunOnLeaveFunction, bool pRunValidator,
                            bool pIsLabel)
                            {
   bool setIt = false;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel)
         {
            setIt = input->setKeyFunction(pKey, pFunction, pUseVal,
                                    pExitAfterRun, pRunOnLeaveFunction,
                                    pRunValidator);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            setIt = input->setKeyFunction(pKey, pFunction, pUseVal,
                                    pExitAfterRun, pRunOnLeaveFunction,
                                    pRunValidator);
         }
      }
   }

   return(setIt);
} // setKeyFunction

bool cxForm::allFieldsBlank() const
{
   bool allBlank = true;

   for (const auto& input : mInputs)
   {
      if (input->getValue() != "")
      {
         allBlank = false;
         break;
      }
   }

   return(allBlank);
} // allFieldsBlank

void cxForm::setLabelColor(const int& pIndex, e_cxColors pColor)
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->setLabelColor(pColor);
   }
} // setLabelColor

void cxForm::setLabelColor(const string& pLabel, e_cxColors pColor, bool pIsLabel)
{
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setLabelColor(pColor);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setLabelColor(pColor);
         }
      }
   }
} // setLabelColor

void cxForm::setAllLabelColor(e_cxColors pColor)
{
   for (const auto& input : mInputs)
   {
      input->setLabelColor(pColor);
   }
} // setAllLabelColor

int cxForm::getLabelColor(const int& pIndex) const
{
   int labelColor = eDEFAULT;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      labelColor = mInputs[pIndex]->getLabelColor();
   }

   return(labelColor);
} // getLabelColor

int cxForm::getLabelColor(const string& pLabel, bool pIsLabel) const
{
   int labelColor = eDEFAULT;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            labelColor = input->getLabelColor();
            break;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            labelColor = input->getLabelColor();
            break;
         }
      }
   }

   return(labelColor);
} // getLabelColor

void cxForm::setValueColor(const int& pIndex, e_cxColors pColor)
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->setValueColor(pColor);
   }
} // setValueColor

void cxForm::setValueColor(const string& pLabel, e_cxColors pColor, bool pIsLabel)
{
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setValueColor(pColor);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setValueColor(pColor);
         }
      }
   }
} // setValueColor

void cxForm::setAllValueColor(e_cxColors pColor)
{
   for (const auto& input : mInputs)
   {
      input->setValueColor(pColor);
   }
} // setAllValueColor

int cxForm::getValueColor(const int& pIndex) const
{
   int valueColor = eDEFAULT;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      valueColor = mInputs[pIndex]->getValueColor();
   }

   return(valueColor);
} // getValueColor

int cxForm::getValueColor(const string& pLabel, bool pIsLabel) const
{
   int valueColor = eDEFAULT;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            valueColor = input->getValueColor();
            break;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            valueColor = input->getValueColor();
            break;
         }
      }
   }

   return(valueColor);
} // getValueColor

void cxForm::setAllColors(e_cxColors pLabelColor, e_cxColors pValueColor)
{
   for (const auto& input : mInputs)
   {
      input->setColor(eLABEL, pLabelColor);
      input->setColor(eDATA, pValueColor);
   }
} // setAllColors

void cxForm::setColor(e_WidgetItems pItem, e_cxColors pColor)
{
   // For label & value colors, set those colors on all the inputs on the
   //  form; for any other colors, set those on the form.
   if ((pItem == eLABEL) || (pItem == eDATA_READONLY) ||
       (pItem == eDATA_EDITABLE) || (pItem == eDATA))
       {
      // Label & value colors - Set these on all inputs on the form
      for (const auto& input : mInputs)
      {
         input->setColor(pItem, pColor);
      }
   }
   else
   {
      cxWindow::setColor(pItem, pColor);
   }
} // setColor

int cxForm::inputTopRow(int pIndex) const
{
   int returnVal = -1;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      returnVal = mInputs[pIndex]->top();
   }

   return(returnVal);
} // inputTopRow

int cxForm::inputTopRow(const string& pLabel, bool pIsLabel) const
{
   int returnVal = -1;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            returnVal = input->top();
            break;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            returnVal = input->top();
            break;
         }
      }
   }

   return(returnVal);
} // inputTopRow

int cxForm::inputLeftCol(int pIndex) const
{
   int returnVal = -1;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      returnVal = mInputs[pIndex]->left();
   }

   return(returnVal);
} // inputLeftCol

int cxForm::inputLeftCol(const string& pLabel, bool pIsLabel) const
{
   int returnVal = -1;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            returnVal = input->left();
            break;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            returnVal = input->left();
            break;
         }
      }
   }

   return(returnVal);
} // inputLeftCol

int cxForm::inputHeight(int pIndex) const
{
   int returnVal = -1;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      returnVal = mInputs[pIndex]->height();
   }

   return(returnVal);
} // inputHeight

int cxForm::inputHeight(const string& pLabel, bool pIsLabel) const
{
   int returnVal = -1;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            returnVal = input->height();
            break;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            returnVal = input->height();
            break;
         }
      }
   }

   return(returnVal);
} // inputHeight

int cxForm::inputWidth(int pIndex) const
{
   int returnVal = -1;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      returnVal = mInputs[pIndex]->width();
   }

   return(returnVal);
} // inputWidth

int cxForm::inputWidth(const string& pLabel, bool pIsLabel) const
{
   int returnVal = -1;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if ((input->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            returnVal = input->width();
            break;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            returnVal = input->width();
            break;
         }
      }
   }

   return(returnVal);
} // inputWidth

int cxForm::maxInputLen(int pIndex) const
{
   int length = 0;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      length = mInputs[pIndex]->getInputLen();
   }

   return(length);
} // maxInputLen

int cxForm::maxInputLen(const string& pLabel, bool pIsLabel) const
{
   int length = 0;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            length = input->getInputLen();
            break;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            length = input->getInputLen();
            break;
         }
      }
   }

   return(length);
} // maxInputLen

string cxForm::inputLabel(int pIndex) const
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      return(mInputs[pIndex]->getLabel());
   }
   else
   {
      return("");
   }
} // inputLabel

string cxForm::inputLabel(const string& pName) const
{
   string retval;

   for (const auto& input : mInputs)
   {
      if (input->getName() == pName)
      {
         retval = input->getName();
         break;
      }
   }

   return(retval);
} // inputLabel

string cxForm::inputName(int pIndex) const
{
   string retval;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      retval = mInputs[pIndex]->getName();
   }

   return(retval);
} // inputName

string cxForm::inputName(const string& pLabel) const
{
   string retval;

   for (const auto& input : mInputs)
   {
      if (input->getLabel() == pLabel)
      {
         retval = input->getLabel();
         break;
      }
   }

   return(retval);
} // inputName

void cxForm::scrollInputs(int pVScrollAmt, int pHScrollAmt,
                          bool pRefresh, bool pBringToTop)
                          {
   // If there are no inputs, then no scrolling needs to be
   //  done.
   if (mInputs.size() == 0)
   {
      return;
   }

   // If pVScrollAmt and pHScrollAmt are both 0, no scrolling
   //  needs to be done.
   if ((pVScrollAmt == 0) && (pHScrollAmt == 0))
   {
      return;
   }

   // If we're to do any vertical scrolling:
   //  if we're to scroll up, but the first input is already
   //  at the top, don't do anything.  Also don't do anything
   //  if we're to scroll down but the last input is already
   //  at the bottom.
   if (pVScrollAmt > 0) { // Moving the inputs down
      if (getBorderStyle() != eBS_NOBORDER) { // With border
         int topBoundary = top();
         // If inputs aren't appearing on the border, then
         //  use top+1 as the upper boundary.
         if (!mInputsOnBorder)
         {
            ++topBoundary;
         }
         if (mInputPositions[0].first == topBoundary)
         {
            return;
         }
      }
      else { // No border
         if (mInputPositions[0].first == top())
         {
            return;
         }
      }
   }
   else if (pVScrollAmt < 0) { // Moving the inputs up
      if (getBorderStyle() != eBS_NOBORDER) { // With border
         int bottomBoundary = bottom();
         // If inputs aren't appearing on the border, then
         //  use bottom-1 as the lower boundary.
         if (!mInputsOnBorder)
         {
            --bottomBoundary;
         }
         if (mInputPositions[mInputPositions.size()-1].first == bottomBoundary)
         {
            return;
         }
      }
      else { // No border
         if (mInputPositions[mInputPositions.size()-1].first == bottom())
         {
            return;
         }
      }
   }

   int inputY = 0;
   int inputX = 0;
   unsigned numInputs = mInputs.size();
   for (unsigned i = 0; i < numInputs; ++i)
   {
      // Update the coordinates stored in
      //  mInputPositions
      inputY = mInputPositions[i].first + pVScrollAmt;
      inputX = mInputPositions[i].second + pHScrollAmt;
      mInputPositions[i].first = inputY;
      mInputPositions[i].second = inputX;

      // Move the input
      mInputs[i]->move(inputY, inputX, false);

      // If part of the input is outside the form,
      //  then hide it.
      if (getBorderStyle() != eBS_NOBORDER)
      {
         if ((inputY <= top()) || (mInputs[i]->bottom() >= bottom()))
         {
            mInputs[i]->hide();
            mInputShowBools[i] = false;
         }
         else
         {
            mInputs[i]->unhide();
            mInputShowBools[i] = true;
         }
      }
      else
      {
         if ((inputY < top()) || (mInputs[i]->bottom() > bottom()))
         {
            mInputs[i]->hide();
            mInputShowBools[i] = false;
         }
         else
         {
            mInputs[i]->unhide();
            mInputShowBools[i] = true;
         }
      }
   }

   if (pRefresh)
   {
      showAllInputs(pBringToTop);
   }
} // scrollInputs

void cxForm::toggleCursor(int pIndex, bool pShowCursor)
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->toggleCursor(pShowCursor);
   }
} // toggleCursor

void cxForm::toggleCursor(const string& pLabel, bool pShowCursor, bool pIsLabel)
{
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->toggleCursor(pShowCursor);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->toggleCursor(pShowCursor);
         }
      }
   }
} // toggleCursor

void cxForm::toggleCursor(bool pShowCursor)
{
   for (const auto& input : mInputs)
   {
      input->toggleCursor(pShowCursor);
   }
} // toggleCursor

shared_ptr<cxMultiLineInput> cxForm::getInput(int pIndex) const
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      return mInputs[pIndex];
   }
   else
   {
      return nullptr;
   }
} // getInput

shared_ptr<cxMultiLineInput> cxForm::getInput(const string& pLabel, bool pIsLabel) const
{
   shared_ptr<cxMultiLineInput> retval;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            retval = input;
            break;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            retval = input;
            break;
         }
      }
   }

   return retval;
} // getInput

shared_ptr<cxMultiLineInput> cxForm::getCurrentInput() const
{
   if (mInputs.size() > 0)
   {
      // Note: getCurrentInputIndex() does bounds checking and won't return
      // an invalid index.
      return(mInputs[getCurrentInputIndex()]);
   }
   else
   {
      return nullptr;
   }
} // getCurrentInput

int cxForm::getInputOption(int pIndex) const
{
   int kind = eINPUT_READONLY;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      kind = mInputs[pIndex]->getInputOption();
   }

   return(kind);
} // getKind

int cxForm::getInputOption(const string& pLabel, bool pIsLabel) const
{
   int kind = eINPUT_READONLY;

   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            kind = input->getInputOption();
            break;
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            kind = input->getInputOption();
            break;
         }
      }
   }

   return(kind);
} // getKind

void cxForm::setInputOption(int pIndex, eInputOptions pInputOption, bool pRefresh)
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      // Save the current input kind
      eInputOptions oldInputOption = (eInputOptions)mInputs[pIndex]->getInputOption();
      // Set the input kind
      mInputs[pIndex]->setInputOption(pInputOption);

      // Make sure mEditableInputsExist is up to date.
      int currentKind = mInputs[pIndex]->getInputOption();
      if ((currentKind == eINPUT_EDITABLE) && (mInputs[pIndex]->isEnabled())) {
         mEditableInputsExist = true;
      }
      else
      {
         lookForEditableInputs();
      }

      // If the default attributes are to be applied, then do it.
      if (mApplyAttrDefaults)
      {
         // Remove the input's current editable & read-only attributes..  The
         //  defaults will then be applied using applyAttrDefaults().
         mInputs[pIndex]->removeAttrs(eDATA_EDITABLE);
         mInputs[pIndex]->removeAttrs(eDATA_READONLY);
         applyAttrDefaults(mInputs[pIndex]);
      }

      // If the current input kind is different from the old input kind
      //  and pRefresh is true, refresh the input.
      if ((currentKind != oldInputOption) && pRefresh)
      {
         // Show the input (to refresh it) - And bring it
         //  to the top if the form window is above the
         //  input's window.
         mInputs[pIndex]->show(isAbove(*(mInputs[pIndex])), false);
      }
   }
} // setKind

void cxForm::setInputOption(const string& pLabel, eInputOptions pInputOption,
                     bool pIsLabel, bool pRefresh)
                     {
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   eInputOptions oldInputOption=eINPUT_EDITABLE;
   eInputOptions currentKind=eINPUT_EDITABLE;
   bool inputIsEnabled = false;
   if (pIsLabel)
   {
      for (auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            // Save the current input kind
            oldInputOption = (eInputOptions)(input->getInputOption());
            // Set the new input kind
            input->setInputOption(pInputOption);
            currentKind = (eInputOptions)input->getInputOption();

            // If the default attributes are to be applied,
            //  then for a 'normal' input, remove the bold
            //  attribute and apply the standout attribute.
            //  For a read-only input, remove the standout
            //  and apply the bold attribute.
            if (mApplyAttrDefaults)
            {
               // Remove the input's current editable & read-only attributes..  The
               //  defaults will then be applied using applyAttrDefaults().
               input->removeAttrs(eDATA_EDITABLE);
               input->removeAttrs(eDATA_READONLY);
               applyAttrDefaults(input);
            }

            // If the current input kind is different from the old input kind
            //  and pRefresh is true, refresh the input.
            if ((currentKind != oldInputOption) && pRefresh)
            {
               input->show(false, false);
            }

            // Update inputIsEnabled
            inputIsEnabled = input->isEnabled();
         }
      }
   }
   else
   {
      for (auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            // Save the current input kind
            oldInputOption = (eInputOptions)(input->getInputOption());
            // Set the new input kind
            input->setInputOption(pInputOption);
            currentKind = (eInputOptions)(input->getInputOption());

            // If the default attributes are to be applied,
            //  then for a 'normal' input, remove the bold
            //  attribute and apply the standout attribute.
            //  For a read-only input, remove the standout
            //  and apply the bold attribute.
            if (mApplyAttrDefaults)
            {
               // Remove the input's current editable & read-only attributes..  The
               //  defaults will then be applied using applyAttrDefaults().
               input->removeAttrs(eDATA_EDITABLE);
               input->removeAttrs(eDATA_READONLY);
               applyAttrDefaults(input);
            }

            // If the current input kind is different from the old input kind
            //  and pRefresh is true, refresh the input.
            if ((currentKind != oldInputOption) && pRefresh)
            {
               input->show(false, false);
            }

            // Update inputIsEnabled
            inputIsEnabled = input->isEnabled();
         }
      }
   }

   // Make sure mEditableInputsExist is up to date.
   if (currentKind == eINPUT_EDITABLE)
   {
      if (inputIsEnabled)
      {
         mEditableInputsExist = true;
      }
      else
      {
         lookForEditableInputs();
      }
   }
   else
   {
      lookForEditableInputs();
   }
} // setKind

void cxForm::setAllInputOption(eInputOptions pInputOption, bool pRefresh)
{
   eInputOptions inputKind;
   bool inputIsEnabled = false;
   for (auto& input : mInputs)
   {
      // Save the current input kind
      inputKind = (eInputOptions)(input->getInputOption());
      // Set the input's kind
      input->setInputOption(pInputOption);
      // If the default attributes are to be applied,
      //  then for a 'normal' input, remove the bold
      //  attribute and apply the standout attribute.
      //  For a read-only input, remove the standout
      //  and apply the bold attribute.
      if (mApplyAttrDefaults)
      {
         // Remove the input's current editable & read-only attributes..  The
         //  defaults will then be applied using applyAttrDefaults().
         input->removeAttrs(eDATA_EDITABLE);
         input->removeAttrs(eDATA_READONLY);
         applyAttrDefaults(input);
      }

      // If pInputOption is different from the old input kind and pRefresh is
      //  true, then refresh the input.
      if ((pInputOption != inputKind) && pRefresh)
      {
         input->show(false, false);
      }

      inputIsEnabled = input->isEnabled();
   }

   // Update mEditableInputsExist
   mEditableInputsExist = ((pInputOption == eINPUT_EDITABLE) && inputIsEnabled);
} // setAllKind

void cxForm::setStatus(const string& pStatus, bool pRefreshStatus)
{
   cxWindow::setStatus(pStatus, pRefreshStatus);
   mCustomStatus = true;
} // setStatus

void cxForm::disableCustomStatus(bool pRefreshStatus)
{
   cxWindow::setStatus("", pRefreshStatus);
   mCustomStatus = false;
   useInputStatusAsFormStatus(pRefreshStatus);
} // disableCustomStatus

void cxForm::setValidatorFunction(int pIndex, const shared_ptr<cxFunction>& pFunction)
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->setValidatorFunction(pFunction);
   }
} // setValidatorFunction

void cxForm::setValidatorFunction(int pIndex, funcPtr4 pFunction, void *p1,
                     void *p2, void *p3, void *p4)
                     {
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->setValidatorFunction(pFunction, p1, p2, p3, p4);
   }
} // setValidatorFunction

void cxForm::setValidatorFunction(const string& pLabel, const shared_ptr<cxFunction>& pFunction,
                                  bool pIsLabel)
                                  {
   // If there are multiple inputs with the same label/name, all of them
   // will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel || (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
         {
            input->setValidatorFunction(pFunction);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setValidatorFunction(pFunction);
         }
      }
   }
} // setValidatorFunction

void cxForm::setValidatorFunction(const string& pLabel, funcPtr4 pFunction,
                     void *p1, void *p2, void *p3, void *p4, bool pIsLabel)
                     {
   // If there are multiple inputs with the same label/name, all of them
   // will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel || (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
         {
            input->setValidatorFunction(pFunction, p1, p2, p3, p4);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setValidatorFunction(pFunction, p1, p2, p3, p4);
         }
      }
   }
} // setValidatorFunction

void cxForm::showInputsOnBorder(bool pShowInputsOnBorder)
{
   // If the form has a border, let them change this; otherwise,
   //  it doesn't really make sense to have it set false.
   if (getBorderStyle() != eBS_NOBORDER)
   {
      mInputsOnBorder = pShowInputsOnBorder;
   }
   else
   {
      mInputsOnBorder = true;
   }
} // showInputsOnBorder

void cxForm::setOnKeyFunction(int pIndex, const shared_ptr<cxFunction>& pFunction)
{
   if (pIndex >= 0 && pIndex < (int)mInputs.size())
   {
      mInputs[pIndex]->setOnKeyFunction(pFunction);
   }
} // setOnKeyFunction

void cxForm::setOnKeyFunction(int pIndex, funcPtr4 pFunction, void *p1,
                              void *p2, void *p3, void *p4)
                              {
   if (pIndex >= 0 && pIndex < (int)mInputs.size())
   {
      mInputs[pIndex]->setOnKeyFunction(pFunction, p1, p2, p3, p4);
   }
} // setOnKeyFunction

void cxForm::setOnKeyFunction(const string& pLabel, const shared_ptr<cxFunction>& pFunction, bool pIsLabel)
{
   // If there are multiple inputs with the same label/name, all of them
   // will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel || (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
         {
            input->setOnKeyFunction(pFunction);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setOnKeyFunction(pFunction);
         }
      }
   }
} // setOnKeyFunction

void cxForm::setOnKeyFunction(const string& pLabel, funcPtr4 pFunction, void *p1,
                              void *p2, void *p3, void *p4, bool pIsLabel)
                              {
   // If there are multiple inputs with the same label/name, all of them
   // will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setOnKeyFunction(pFunction, p1, p2, p3, p4);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setOnKeyFunction(pFunction, p1, p2, p3, p4);
         }
      }
   }
} // setOnKeyFunction

void cxForm::setOnKeyFunction(int pIndex, funcPtr2 pFunction, void *p1,
                              void *p2)
                              {
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->setOnKeyFunction(pFunction, p1, p2);
   }
} // setOnKeyFunction

void cxForm::setOnKeyFunction(const string& pLabel, funcPtr2 pFunction, void *p1,
                              void *p2, bool pIsLabel)
                              {
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setOnKeyFunction(pFunction, p1, p2);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setOnKeyFunction(pFunction, p1, p2);
         }
      }
   }
} // setOnKeyFunction

void cxForm::setOnKeyFunction(int pIndex, funcPtr0 pFunction)
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->setOnKeyFunction(pFunction);
   }
} // setOnKeyFunction

void cxForm::setOnKeyFunction(const string& pLabel, funcPtr0 pFunction,
                              bool pIsLabel)
                              {
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   if (pIsLabel)
   {
      for (const auto& input : mInputs)
      {
         if (input->getLabel() == pLabel ||
             (stringWithoutHotkeyChars(input->getLabel()) == pLabel))
             {
            input->setOnKeyFunction(pFunction);
         }
      }
   }
   else
   {
      for (const auto& input : mInputs)
      {
         if (input->getName() == pLabel)
         {
            input->setOnKeyFunction(pFunction);
         }
      }
   }
} // setOnKeyFunction

void cxForm::setAllOnKeyFunction(const shared_ptr<cxFunction>& pFunction)
{
   for (const auto& input : mInputs)
   {
      input->setOnKeyFunction(pFunction);
   }
} // setAllOnKeyFunction

void cxForm::setAllOnKeyFunction(funcPtr4 pFunction, void *p1, void *p2, void *p3,
                                 void *p4)
                                 {
   for (const auto& input : mInputs)
   {
      input->setOnKeyFunction(pFunction, p1, p2, p3, p4);
   }
} // setAllOnKeyFunction

void cxForm::setAllOnKeyFunction(funcPtr2 pFunction, void *p1, void *p2)
{
   for (const auto& input : mInputs)
   {
      input->setOnKeyFunction(pFunction, p1, p2);
   }
} // setAllOnKeyFunction

void cxForm::setAllOnKeyFunction(funcPtr0 pFunction)
{
   inputPtrContainer::iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      (*iter)->setOnKeyFunction(pFunction);
   }
} // setAllOnKeyFunction

void cxForm::toggleOnKeyFunction(int pIndex, bool pRunOnKeyFunction)
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->toggleOnKeyFunction(pRunOnKeyFunction);
   }
} // toggleOnKeyFunction

void cxForm::toggleOnKeyFunction(const string& pLabel, bool pRunOnKeyFunction,
                               bool pIsLabel)
                               {
   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   inputPtrContainer::iterator iter = mInputs.begin();
   if (pIsLabel)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if (((*iter)->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars((*iter)->getLabel()) == pLabel))
             {
            (*iter)->toggleOnKeyFunction(pRunOnKeyFunction);
         }
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getName() == pLabel)
         {
            (*iter)->toggleOnKeyFunction(pRunOnKeyFunction);
         }
      }
   }
} // toggleOnKeyFunction

void cxForm::toggleAllOnKeyFunction(bool pRunOnKeyFunction)
{
   inputPtrContainer::iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      (*iter)->toggleOnKeyFunction(pRunOnKeyFunction);
   }
} // toggleAllOnKeyFunction

bool cxForm::onKeyFunctionEnabled(int pIndex) const
{
   bool enabled = false;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      enabled = mInputs[pIndex]->onKeyFunctionEnabled();
   }

   return(enabled);
} // onKeyFunctionEnabled

bool cxForm::onKeyFunctionEnabled(const string& pLabel, bool pIsLabel) const
{
   bool enabled = false;

   inputPtrContainer::const_iterator iter = mInputs.begin();
   if (pIsLabel)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if (((*iter)->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars((*iter)->getLabel()) == pLabel))
             {
            enabled = (*iter)->onKeyFunctionEnabled();
            break;
         }
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getName() == pLabel)
         {
            enabled = (*iter)->onKeyFunctionEnabled();
            break;
         }
      }
   }

   return(enabled);
} // onKeyFunctionEnabled

cxForm& cxForm::operator =(const cxForm& pThatForm)
{
   // Only try to copy pThatForm if it's a different instance.
   if (&pThatForm != this)
   {
      copyCxFormStuff(&pThatForm);
   }

   return(*this);
} // operator =

bool cxForm::addInputJumpKey(int pKey, int pIndex)
{
   bool retval = false;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      // If the key isn't already set up to fire a function, go ahead and add
      //  it as a jump key.
      if (mKeyFunctions.find(pKey) == mKeyFunctions.end())
      {
         retval = true;
         // Add the input index to mInputJumpIndexes
         mInputJumpIndexes.push_back(pIndex);
         // Set the form function to jump to the input
         setKeyFunction(pKey, cxForm::changeFocus, this,
                        &(mInputJumpIndexes[mInputJumpIndexes.size()-1]), false);
      }
   }

   return(retval);
} // addInputJumpkey

bool cxForm::addInputJumpKey(int pKey, const string& pLabel, bool pIsLabel)
{
   bool retval = false;

   // If there are multiple inputs with the same label/name, all of them
   //  will be affected.
   int numInputs = (int)mInputs.size();
   if (pIsLabel)
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if ((mInputs[i]->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(mInputs[i]->getLabel()) == pLabel))
             {
            retval = addInputJumpKey(pKey, i);
         }
      }
   }
   else
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if (mInputs[i]->getName() == pLabel)
         {
            retval = addInputJumpKey(pKey, i);
         }
      }
   }

   return(retval);
} // addInputJumpKey

int cxForm::getCurrentInputIndex() const
{
   // mCurrentInput should store a valid index, but check anyway.
   if (mCurrentInput < 0)
   {
      return(0);
   }
   else if ((unsigned)mCurrentInput >= mInputs.size())
   {
      return((int)mInputs.size() - 1);
   }
   else
   {
      return(mCurrentInput);
   }
} // getCurrentInputIndex

string cxForm::getCurrentInputLabel() const
{
   return(mInputs[getCurrentInputIndex()]->getLabel());
} // getCurrentInputLabel

string cxForm::getCurrentInputName() const
{
   return(mInputs[getCurrentInputIndex()]->getName());
} // getCurrentInputName

int cxForm::getInputIndex(const string& pLabel, bool pIsLabel) const
{
   int index = -1;

   int numInputs = (int)(mInputs.size());
   if (pIsLabel)
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if ((mInputs[i]->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars(mInputs[i]->getLabel()) == pLabel))
             {
            index = i;
            break;
         }
      }
   }
   else
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if (mInputs[i]->getName() == pLabel)
         {
            index = i;
            break;
         }
      }
   }

   return(index);
} // getInputIndex

int cxForm::getInputIndex(const shared_ptr<cxMultiLineInput>& pInput) const
{
   return getInputIndex(pInput.get());
} // getInputIndex

int cxForm::getInputIndex(const cxMultiLineInput *pInput) const
{
   int index = -1;

   int numInputs = (int)(mInputs.size());
   for (int i = 0; i < numInputs; ++i)
   {
      if (mInputs[i].get() == pInput)
      {
         index = i;
         break;
      }
   }

   return(index);
} // getInputIndex

void cxForm::setInputJumpKey(int pInputJumpKey)
{
   if (pInputJumpKey != mInputJumpKey)
   {
      // This map stores whether or not each input had the input jump key in
      //  its list of keys that skip the validator.  That way, we can set it
      //  back up the way it was before for each input.  The key is the index,
      //  and the value is whether or not the key was in the list.
      map<int, bool> hadSkipValidatorKey;

      int numInputs = (int)(mInputs.size());
      // Remove the current mInputJumpKey from each input's list of exit keys
      //  and list of keys that skip the validator
      for (int i = 0; i < numInputs; ++i)
      {
         mInputs[i]->removeExitKey(mInputJumpKey);
         hadSkipValidatorKey[i] = mInputs[i]->hasSkipValidatorKey(mInputJumpKey);
         mInputs[i]->removeSkipValidatorKey(mInputJumpKey);
      }
      // Update mInputJumpKey, and then add it to each input as an exit key.
      //  Also, if the input had the key set up to skip its validator, then
      //  add it.
      mInputJumpKey = pInputJumpKey;
      for (int i = 0; i < numInputs; ++i)
      {
         mInputs[i]->addExitKey(mInputJumpKey);
         if (hadSkipValidatorKey[i])
         {
            mInputs[i]->addSkipValidatorKey(mInputJumpKey);
         }
      }
   }
} // setInputJumpKey

int cxForm::getInputJumpKey() const
{
   return(mInputJumpKey);
} // getInputJumpKey

void cxForm::setCanBeEditable(int pIndex, bool pCanBeEditable)
{
   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      mInputs[pIndex]->setCanBeEditable(pCanBeEditable);
      if (pCanBeEditable)
      {
         setInputOption(pIndex, eINPUT_EDITABLE);
      }
      else
      {
         setInputOption(pIndex, eINPUT_READONLY);
      }
   }
} // setCanBeEditable

void cxForm::setCanBeEditable(const string& pLabel, bool pCanBeEditable, bool pIsLabel)
{
   // If there are multiple inputs with the same label/name, all of them will
   //  be affected.  Note that this uses indexes instead of iterators because
   //  this calls setCanbeEditable(), which takes an index.
   int numInputs = (int)(mInputs.size());
   if (pIsLabel)
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if (mInputs[i]->getLabel() == pLabel)
         {
            setCanBeEditable(i, pCanBeEditable);
         }
      }
   }
   else
   {
      for (int i = 0; i < numInputs; ++i)
      {
         if (mInputs[i]->getName() == pLabel)
         {
            setCanBeEditable(i, pCanBeEditable);
         }
      }
   }
} // setCanbeEditable

void cxForm::setAllCanBeEditable(bool pCanBeEditable)
{
   inputPtrContainer::iterator iter = mInputs.begin();
   bool inputIsEnabled = false;
   for (; iter != mInputs.end(); ++iter)
   {
      (*iter)->setCanBeEditable(pCanBeEditable);
      // Update inputIsEnabled
      if (!inputIsEnabled)
      {
         inputIsEnabled = (*iter)->isEnabled();
      }
   }

   // Update mEditableInputsExist
   mEditableInputsExist = (pCanBeEditable && inputIsEnabled);
} // setAllCanBeEditable

bool cxForm::canBeEditable(int pIndex) const
{
   bool retval = false;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      retval = mInputs[pIndex]->canBeEditable();
   }

   return(retval);
} // canBeEditable

bool cxForm::canBeEditable(const string& pLabel, bool pIsLabel) const
{
   bool retval = false;

   inputPtrContainer::const_iterator iter = mInputs.begin();
   if (pIsLabel)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if (((*iter)->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars((*iter)->getLabel()) == pLabel))
             {
            retval = (*iter)->canBeEditable();
            break;
         }
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getName() == pLabel)
         {
            retval = (*iter)->canBeEditable();
            break;
         }
      }
   }

   return(retval);
} // canBeEditable

bool cxForm::anyInputsCanBeEditable() const
{
   bool retval = false;

   inputPtrContainer::const_iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      if ((*iter)->canBeEditable() || (*iter)->isEnabled())
      {
         retval = true;
         break;
      }
   }

   return(retval);
} // anyInputsCanBeEditable

void cxForm::setAllowQuit(bool pAllowQuit)
{
   mAllowQuit = pAllowQuit;
} // setAllowQuit

bool cxForm::getAllowQuit() const
{
   return(mAllowQuit);
} // getAllowQuit

void cxForm::setAllowExit(bool pAllowExit)
{
   mAllowExit = pAllowExit;
} // setAllowExit

bool cxForm::getAllowExit() const
{
   return(mAllowExit);
} // getAllowExit

bool cxForm::addQuitKey(int pKey, bool pRunOnLeaveFunction, bool pOverride)
{
   // Add the key to the quit keys, then add the key to all inputs' quit keys.
   //  Also, in case the key is set up as an exit key, remove it from the exit
   //  key list.
   bool added = cxWindow::addQuitKey(pKey, pRunOnLeaveFunction, pOverride);
   if (added)
   {
      cxWindow::removeExitKey(pKey);

      inputPtrContainer::iterator iter = mInputs.begin();
      for (; iter != mInputs.end(); ++iter)
      {
         (*iter)->addQuitKey(pKey, pRunOnLeaveFunction, pOverride);
         (*iter)->removeExitKey(pKey);
      }
   }

   return(added);
} // addQuitKey

void cxForm::removeQuitKey(int pKey)
{
   mQuitKeys.erase(pKey);
   inputPtrContainer::iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      (*iter)->removeQuitKey(pKey);
   }
} // removeQuitKey

bool cxForm::addExitKey(int pKey, bool pRunOnLeaveFunction, bool pOverride)
{
   // Add the key to the exit keys, then add the key to all inputs' exit keys.
   //  Also, in case the key is set up as a quit key, remove it from the quit
   //  key list.
   bool added = cxWindow::addExitKey(pKey, pRunOnLeaveFunction, pOverride);
   if (added)
   {
      cxWindow::removeQuitKey(pKey);

      inputPtrContainer::iterator iter = mInputs.begin();
      for (; iter != mInputs.end(); ++iter)
      {
         (*iter)->addExitKey(pKey, pRunOnLeaveFunction, pOverride);
         (*iter)->removeQuitKey(pKey);
      }
   }

   return(added);
} // addExitKey

void cxForm::removeExitKey(int pKey)
{
   mExitKeys.erase(pKey);
   inputPtrContainer::iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      (*iter)->removeExitKey(pKey);
   }
} // removeExitKey

void cxForm::setOnFocusFunction(funcPtr4 pFuncPtr, void *p1, void *p2,
                           void *p3, void *p4, bool pUseVal,
                           bool pExitAfterRun)
                           {
   cxWindow::setOnFocusFunction(pFuncPtr, p1, p2, p3, p4, pUseVal,
                        pExitAfterRun);
} // setOnFocusFunction

void cxForm::setOnFocusFunction(funcPtr2 pFuncPtr, void *p1, void *p2,
                                bool pUseVal, bool pExitAfterRun)
                                {
   cxWindow::setOnFocusFunction(pFuncPtr, p1, p2, pUseVal, pExitAfterRun);
} // setOnFocusFunction

void cxForm::setOnFocusFunction(funcPtr0 pFuncPtr, bool pUseVal,
                                bool pExitAfterRun)
                                {
   cxWindow::setOnFocusFunction(pFuncPtr, pUseVal, pExitAfterRun);
} // setOnFocusFunction

void cxForm::setLoopStartFunction(const shared_ptr<cxFunction>& pFuncPtr)
{
   mLoopStartFunction = pFuncPtr;
} // setLoopStartFunction

void cxForm::setLoopEndFunction(const shared_ptr<cxFunction>& pFuncPtr)
{
   mLoopEndFunction = pFuncPtr;
} // setLoopEndFunction

void cxForm::setOnLeaveFunction(funcPtr4 pFuncPtr, void *p1,
                           void *p2, void *p3, void *p4)
                           {
   cxWindow::setOnLeaveFunction(pFuncPtr, p1, p2, p3, p4);
} // setOnLeaveFunction

void cxForm::setOnLeaveFunction(funcPtr2 pFuncPtr, void *p1, void *p2)
{
   cxWindow::setOnLeaveFunction(pFuncPtr, p1, p2);
} // setOnLeaveFunction

void cxForm::setOnLeaveFunction(funcPtr0 pFuncPtr)
{
   cxWindow::setOnLeaveFunction(pFuncPtr);
} // setOnLeaveFunction

void cxForm::showAllInputs(bool pBringToTop, bool pSkipCurrentInput) const
{
   int inputTop = 0;         // For the top coordinate of inputs
   int inputBottom = 0;      // For the bottom coordinate of inputs
   int inputLeft = 0;        // For the left coordinate of inputs
   int inputRight = 0;       // For the right coordinate of inputs
   int theTop = top();       // Top row of the window
   int theBottom = bottom(); // Bottom row of the window
   int theLeft = left();     // Leftmost column of the window
   int theRight = right();   // Rightmost column of the window
   unsigned numInputs = mInputs.size();
   for (unsigned i = 0; i < numInputs; ++i)
   {
      // Skip the current input if pSkipCurrentInput is true.
      if (pSkipCurrentInput)
      {
         if ((int)i == mCurrentInput)
         {
            continue;
         }
      }

      // Only show the item if it fits inside the window.
      inputTop = mInputPositions[i].first;
      inputLeft = mInputPositions[i].second;
      inputBottom = mInputs[i]->bottom();
      inputRight = mInputs[i]->right();
      // If we are to show inputs on borders, or there are no
      //  borders, then include the borders in the check.
      //  Otherwise, don't include the borders in the check.
      if ((mInputsOnBorder) || (getBorderStyle() == eBS_NOBORDER))
      {
         if ((inputTop >= theTop) && (inputBottom <= theBottom) &&
             (inputLeft >= theLeft) && (inputRight <= theRight))
             {
            // If the current input is meant to be shown,
            //  then show it.
            if (mInputShowBools[i])
            {
               // Bring the input to the top if the form window is
               //  above the input's window; otherwise, honor
               //  pBringToTop.
               if (isAbove(*(mInputs[i])))
               {
                  mInputs[i]->show(true, false);
               }
               else
               {
                  mInputs[i]->show(pBringToTop, false);
               }
            }
         }
         else
         {
            // The input is (at least) partially outside the
            //  form window, so hide it.
            mInputs[i]->hide();
         }
      }
      else
      {
         if ((inputTop > theTop) && (inputBottom < theBottom) &&
             (inputLeft > theLeft) && (inputRight < theRight))
             {
            // If the current input is meant to be shown,
            //  then show it.
            if (mInputShowBools[i])
            {
               // If the form window is above the input's window,
               //  then bring the input to the top.  Otherwise,
               //  honor pBringToTop.
               if (isAbove(*(mInputs[i])))
               {
                  mInputs[i]->show(isAbove(*(mInputs[i])), false);
               }
               else
               {
                  mInputs[i]->show(pBringToTop, false);
               }
            }
         }
         else
         {
            // Bring the input to the top if the form window is
            //  above the input's window; otherwise, honor
            //  pBringToTop.
            if (isAbove(*(mInputs[i])))
            {
               mInputs[i]->show(true, false);
            }
            else
            {
               mInputs[i]->show(pBringToTop, false);
            }
         }
      }
   }
} // showAllInputs

bool cxForm::formKeyIsSet(int pKey) const
{
   // First check against mInputJumpKey
   bool keyIsSet = (pKey == mInputJumpKey);

   // If the key isn't mInputJumpKey, look in mKeyFunctions
   if (!keyIsSet)
   {
      for (const pair<const int, shared_ptr<cxFunction> >& funcPair : mKeyFunctions)
      {
         if (funcPair.first == pKey)
         {
            keyIsSet = true;
            break;
         }
      }
   }

   return(keyIsSet);
} // formKeyIsSet

bool cxForm::isStacked() const
{
   return(mStacked);
} // isStacked

bool cxForm::getExitOnLeaveFirst() const
{
   return(mExitOnLeaveFirst);
} // getExitOnLeaveFirst

void cxForm::setExitOnLeaveFirst(bool pExitOnLeaveFirst)
{
   mExitOnLeaveFirst = pExitOnLeaveFirst;
} // setExitOnLeaveFirst

bool cxForm::hasEditableInputs() const
{
   //return(mEditableInputsExist);
   bool editableInputsExist = false;

   // For inputs to be editable, consider ones that
   //  are not read-only and are enabled.
   inputPtrContainer::const_iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      if (((*iter)->getInputOption() != eINPUT_READONLY) &&
          ((*iter)->isEnabled()))
          {
         editableInputsExist = true;
         break;
      }
   }

   return(editableInputsExist);
} // hasEditableInputs

int cxForm::firstEditableInput() const
{
   int index = -1;

   int numInputs = (int)mInputs.size();
   for (int i = 0; i < numInputs; ++i)
   {
      if (mInputs[i]->isEditable()) {
         index = i;
         break;
      }
   }

   return(index);
} // firstEditableInput

int cxForm::lastEditableInput() const
{
   int index = -1;

   for (int i = (int)mInputs.size() - 1; i >= 0; --i)
   {
      if (mInputs[i]->isEditable()) {
         index = i;
         break;
      }
   }

   return(index);
} // lastEditableInput

void cxForm::bringToTop(bool pRefresh)
{
   // Bring the form window to the top
   cxWindow::bringToTop(false);
   // Bring all the inputs to the top
   inputPtrContainer::iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      (*iter)->bringToTop(false);
   }

   if (pRefresh)
   {
      cxBase::updateWindows();
   }
} // bringToTop

bool cxForm::hasChanged() const
{
   return(mDataChanged);
} // getChanged

void cxForm::setChanged(bool pDataChanged)
{
   mDataChanged = pDataChanged;
} // setChanged

void cxForm::trapNonAssignedFKeys(bool pTrapNonAssignedFKeys)
{
   mTrapNonAssignedFKeys = pTrapNonAssignedFKeys;
} // trapNonAssignedFKeys

cxMultiForm* cxForm::getParentMultiForm() const
{
   return(mParentMultiForm);
} // getParentMultiForm

void cxForm::setParentMultiForm(cxMultiForm *pParentMultiForm)
{
   mParentMultiForm = pParentMultiForm;
} // setParentMultiForm

bool cxForm::runLoopStartFunction()
{
   bool exitAfterRun = false;

   if (mLoopStartFunction != nullptr && mLoopStartFunction->functionIsSet())
   {
      exitAfterRun = mLoopStartFunction->getExitAfterRun();
      mLoopStartFunction->runFunction();
   }

   return(exitAfterRun);
} // runLoopStartFunction

bool cxForm::runLoopEndFunction()
{
   bool exitAfterRun = false;

   if (mLoopEndFunction != nullptr && mLoopEndFunction->functionIsSet())
   {
      exitAfterRun = mLoopEndFunction->getExitAfterRun();
      mLoopEndFunction->runFunction();
   }

   return(exitAfterRun);
} // runLoopEndFunction

int cxForm::maxValueLen(int pIndex) const
{
   int maxLen = 0;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      maxLen = mInputs[pIndex]->maxValueLen();
   }

   return(maxLen);
} // maxValueLen

int cxForm::maxValueLen(const string& pLabel, bool pIsLabel)
{
   int maxLen = 0;

   inputPtrContainer::iterator iter = mInputs.begin();
   if (pIsLabel)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if (((*iter)->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars((*iter)->getLabel()) == pLabel))
             {
            maxLen = (*iter)->maxValueLen();
            break;
         }
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getName() == pLabel)
         {
            maxLen = (*iter)->maxValueLen();
            break;
         }
      }
   }

   return(maxLen);
} // maxValueLen

bool cxForm::isEditable(int pIndex) const
{
   bool editable = false;

   if ((pIndex >= 0) && (pIndex < (int)mInputs.size()))
   {
      editable = mInputs[pIndex]->isEditable();
   }

   return(editable);
} // isEditable

bool cxForm::isEditable(const string& pLabel, bool pIsLabel) const
{
   bool editable = false;

   inputPtrContainer::const_iterator iter = mInputs.begin();
   if (pIsLabel)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if (((*iter)->getLabel() == pLabel) ||
             (stringWithoutHotkeyChars((*iter)->getLabel()) == pLabel))
             {
            editable = (*iter)->isEditable();
            break;
         }
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getName() == pLabel)
         {
            editable = (*iter)->isEditable();
            break;
         }
      }
   }

   return(editable);
} // isEditable

void cxForm::setDisableCursorOnShow(bool pDisableCursorOnShow)
{
   cxWindow::setDisableCursorOnShow(pDisableCursorOnShow);

   inputPtrContainer::iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      (*iter)->setDisableCursorOnShow(pDisableCursorOnShow);
   }
} // setDisableCursorOnShow

void cxForm::setWaitForInputIfEmpty(bool pWaitForInputIfEmpty)
{
   mWaitForInputIfEmpty = pWaitForInputIfEmpty;
} // setWaitForInputIfEmpty

bool cxForm::getWaitForInputIfEmpty() const
{
   return(mWaitForInputIfEmpty);
} // getWaitForInputIfEmpty

void cxForm::clearInputs(bool pRefresh, bool pOnlyEditable)
{
   inputPtrContainer::iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      // If pOnlyEditable is true, then only clear the input if it's editable.
      if (pOnlyEditable)
      {
         if ((*iter)->isEditable())
         {
            (*iter)->clearValue(pRefresh);
         }
      }
      else
      {
         (*iter)->clearValue(pRefresh);
      }
   }
} // clearInputs

void cxForm::getValues(map<string, string>& pValues, bool pUseLabels,
                       bool pSkipBlankIDs) const
                       {
   pValues.clear();

   inputPtrContainer::const_iterator iter = mInputs.begin();
   if (pUseLabels)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         // If pSkipBlankIDs is true and the label is blank, then skip this
         //  input.
         if (pSkipBlankIDs && ((*iter)->getLabel() == ""))
         {
            continue;
         }
         pValues[(*iter)->getLabel()] = (*iter)->getValue();
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         // If pSkipBlankIDs is true and the name is blank, then skip this
         //  input.
         if (pSkipBlankIDs && ((*iter)->getName() == ""))
         {
            continue;
         }
         pValues[(*iter)->getName()] = (*iter)->getValue();
      }
   }
} // getValues

void cxForm::setValues(const map<string, string>& pValues, bool pUseLabels,
                       bool pRefresh, bool pDisableValidatorFunctions)
                       {
   int inputIndex = 0;
   shared_ptr<cxMultiLineInput> iInput;
   bool useValidatorBackup = false; // For each input

   map<string, string>::const_iterator iter = pValues.begin();
   for (; iter != pValues.end(); ++iter)
   {
      // Get the index of the input.  If the input is on the form (inputIndex
      //  should be >= 0), then go ahead and set the value.
      inputIndex = getInputIndex(iter->first, pUseLabels);
      if (inputIndex >= 0)
      {
         // If they want to temporarily disable the validator function, then back
         //   up the input's useValidatorFunction setting and disable it.
         if (pDisableValidatorFunctions)
         {
            iInput = getInput(inputIndex);
            useValidatorBackup = iInput->getUseValidatorFunction();
            iInput->toggleValidatorFunction(false);
         }

         setValue(inputIndex, iter->second, pRefresh);

         // If they wanted to temporarily disable the validator function, then
         //  set it back to what it was originally.
         if (pDisableValidatorFunctions)
         {
            iInput->toggleValidatorFunction(useValidatorBackup);
         }
      }
   }
} // setValues

void cxForm::setEnabled(bool pEnabled)
{
   // Enable/disable the form window, then enable/disable the input windows.
   cxWindow::setEnabled(pEnabled);
   inputPtrContainer::iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      (*iter)->setEnabled(pEnabled);
   }
} // setEnabled

void cxForm::useInputStatus(bool pUseInputStatus)
{
   mUseInputStatus = pUseInputStatus;
   // If the window is enabled and borders are turned on, then draw the
   //  current input's status in the statusbar of the menu or a border
   //  line, depending on the value of pUseInputStatus.
   if ((getBorderStyle() != eBS_NOBORDER) && (isEnabled()) && (!isHidden()))
   {
      if (pUseInputStatus)
      {
         useInputStatusAsFormStatus(true);
      }
      else
      {
         mvwhline(mWindow, height()-1, 1, ACS_HLINE, width()-2);
         wrefresh(mWindow);
      }
   }
} // useInputStatus

bool cxForm::hasInput(const string& pStr, bool pIsLabel) const
{
   bool inputExists = false;

   inputPtrContainer::const_iterator iter = mInputs.begin();
   if (pIsLabel)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getLabel() == pStr)
         {
            inputExists = true;
            break;
         }
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getName() == pStr)
         {
            inputExists = true;
            break;
         }
      }
   }

   return(inputExists);
} // hasInput

cxWindow* cxForm::getParent() const
{
   cxWindow *parentWindow = nullptr;

   if (mParentMultiForm != nullptr)
   {
      parentWindow = mParentMultiForm;
   }
   else
   {
      // mParentMultiForm is nullptr
      parentWindow = cxWindow::getParent();
   }

   return(parentWindow);
} // getParent

// Note: This is commented out because, for some reason, with this in
//  the code, it seems to screw up all the forms in cprime so that they
//  only have 1 input, even if this method is not used anywhere..
/*
int cxForm::nextEditableInput(int pIndex) const {
   int nextEditableInput = -1;

   int numInputs = (int)(mInputs.size());
   if ((pIndex >= 0) && (pIndex < numInputs)) {
      // Look for another editable input from pIndex+1 to the last input.
      for (int i = pIndex+1; i < numInputs; ++i) {
         if (mInputs[i]->isEditable()) {
            nextEditableInput = i;
            break;
         }
         // If we haven't found another editable input yet, loop around and
         //  go from the first input up to pIndex.
         if (nextEditableInput == -1) {
            for (int i = 0; i <= pIndex; ++i) {
               if (mInputs[i]->isEditable()) {
                  nextEditableInput = i;
                  break;
               }
            }
         }
      }
   }

   return(nextEditableInput);
} // nextEditableInput
*/

void cxForm::addAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE))
       {
      cxWindow::addAttr(pItem, pAttr);
   }
   else
   {
      // Add the attribute to all inputs on the form.
      inputPtrContainer::iterator iter = mInputs.begin();
      for (; iter != mInputs.end(); ++iter)
      {
         (*iter)->addAttr(pItem, pAttr);
      }
   }
} // addAttr

void cxForm::setAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE))
       {
      cxWindow::setAttr(pItem, pAttr);
   }
   else
   {
      // Set the attribute on all inputs on the form.
      inputPtrContainer::iterator iter = mInputs.begin();
      for (; iter != mInputs.end(); ++iter)
      {
         (*iter)->setAttr(pItem, pAttr);
      }
   }
} // setAttr

void cxForm::removeAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE))
       {
      cxWindow::removeAttr(pItem, pAttr);
   }
   else
   {
      // Remove the attribute from all inputs on the form.
      inputPtrContainer::iterator iter = mInputs.begin();
      for (; iter != mInputs.end(); ++iter)
      {
         (*iter)->removeAttr(pItem, pAttr);
      }
   }
} // removeAttr

void cxForm::removeAttrs(e_WidgetItems pItem)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE))
       {
      cxWindow::removeAttrs(pItem);
   }
   else
   {
      // Remove the attributes from all inputs on the form.
      inputPtrContainer::iterator iter = mInputs.begin();
      for (; iter != mInputs.end(); ++iter)
      {
         (*iter)->removeAttrs(pItem);
      }
   }
} // removeAttrs

void cxForm::getAttrs(e_WidgetItems pItem, set<attr_t>& pAttrs) const
{
   pAttrs.clear();

   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE))
       {
      cxWindow::getAttrs(pItem, pAttrs);
   }
   else
   {
      // All the inputs should have the same eLABEL, eDATA_READONLY, and
      //  eDATA_EDITABLE attributes, so we can call getAttrs() on just the
      //  first input.
      if (mInputs.size() > 0)
      {
         mInputs[0]->getAttrs(pItem, pAttrs);
      }
   }
} // getAttrs

void cxForm::getNavKeys(set<int>& pNavKeys) const
{
   pNavKeys.clear();

   set<int> inputNavKeys; // For each input
   // Get he navigational keys from each input and append them to pNavKeys
   inputPtrContainer::const_iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      (*iter)->getNavKeys(inputNavKeys);
      copy(inputNavKeys.begin(), inputNavKeys.end(),
           insert_iterator<set<int> >(pNavKeys, pNavKeys.end()));
   }
} // getNavKeys

void cxForm::getNavKeyStrings(set<string>& pNavKeyStrings) const
{
   pNavKeyStrings.clear();

   set<int> navKeys;
   getNavKeys(navKeys);
   set<int>::const_iterator iter = navKeys.begin();
   for (; iter != navKeys.end(); ++iter)
   {
      pNavKeyStrings.insert(cxBase::getKeyStr(*iter));
   }
} // getNavKeyStrings

bool cxForm::hasNavKey(int pKey) const
{
   bool foundIt = false;

   inputPtrContainer::const_iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      if ((*iter)->hasNavKey(pKey))
      {
         foundIt = true;
         break;
      }
   }

   return(foundIt);
} // hasNavKey

string cxForm::cxTypeStr() const
{
   return("cxForm");
} // cxTypeStr

void cxForm::setLastKey(int pLastKey)
{
   cxWindow::setLastKey(pLastKey);
   // If this form is on a multiForm, then
   //  its last key needs to be set also.
   if (mParentMultiForm != nullptr)
   {
      mParentMultiForm->setLastKey(pLastKey);
   }
} // setLastKey

void cxForm::quitNow()
{
   // Tell the form to quit, as well as the current input.
   cxWindow::quitNow();
   shared_ptr<cxMultiLineInput> input = getInput(getCurrentInputIndex());
   if (nullptr != input)
   {
      input->quitNow();
   }
} // quitNow

void cxForm::exitNow()
{
   // Tell the form to exit, as well as the current input.
   cxWindow::exitNow();
   shared_ptr<cxMultiLineInput> input = getInput(getCurrentInputIndex());
   if (nullptr != input)
   {
      input->exitNow();
   }
} // exitNow

void cxForm::setExtendedHelpKey(int pKey)
{
   // Clear the current extended help keys, and then add pKey as the help key.
   clearExtendedHelpKeys();
   addExtendedHelpKey(pKey);
} // setExtendedHelpKey

void cxForm::addExtendedHelpKey(int pKey)
{
   mExtendedHelpKeys.insert(pKey);
   // Add the key to all the inputs
   for (shared_ptr<cxMultiLineInput>& input : mInputs)
   {
      input->addExtendedHelpKey(pKey);
   }
} // addExtendedHelpKey

set<int> cxForm::getExtendedHelpKeys() const
{
   return(mExtendedHelpKeys);
} // getExtendedHelpKey

string cxForm::getExtendedHelpKeyStrings() const
{
   string retval;

   for (int helpKey : mExtendedHelpKeys)
   {
      if (retval != "") { retval += ","; }
      retval += cxBase::getKeyStr(helpKey);
   }

   return(retval);
} // getExtendedHelpKeyStrings

void cxForm::clearExtendedHelpKeys()
{
   mExtendedHelpKeys.clear();
   for (shared_ptr<cxMultiLineInput>& input : mInputs)
   {
      input->clearExtendedHelpKeys();
   }
} // clearExtendedHelpKeys

void cxForm::setUseExtendedHelpKeys(bool pUseExtendedHelpKeys)
{
   mUseExtendedHelpKeys = pUseExtendedHelpKeys;
   // Set it for all inputs
   for (shared_ptr<cxMultiLineInput>& input : mInputs)
   {
      input->setUseExtendedHelpKeys(mUseExtendedHelpKeys);
   }
} // setUseExtendedHelpKey

bool cxForm::getUseExtendedHelpKeys() const
{
   return(mUseExtendedHelpKeys);
} // getUseExtendedHelpKey

string cxForm::getExtendedHelp(int pIndex) const
{
   string retval;

   if ((pIndex >= 0) && (pIndex < (int)(mInputs.size())))
   {
      retval = mInputs[pIndex]->getExtendedHelp();
   }

   return(retval);
} // getExtendedHelp

string cxForm::getExtendedHelp(const string& pIDStr, bool pIsLabel) const
{
   string retval;

   inputPtrContainer::const_iterator iter = mInputs.begin();
   if (pIsLabel)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if (((*iter)->getLabel() == pIDStr) ||
             (stringWithoutHotkeyChars((*iter)->getLabel()) == pIDStr))
             {
            retval = (*iter)->getExtendedHelp();
            break;
         }
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getName() == pIDStr)
         {
            retval = (*iter)->getExtendedHelp();
            break;
         }
      }
   }

   return(retval);
} // getExtendedHelp

void cxForm::setExtendedHelp(int pIndex, const string& pExtendedHelp)
{
   if ((pIndex >= 0) && (pIndex < (int)(mInputs.size())))
   {
      mInputs[pIndex]->setExtendedHelp(pExtendedHelp);
   }
} // setExtendedHelp

void cxForm::setExtendedHelp(const string& pIDStr, const string& pExtendedHelp,
                             bool pIsLabel)
                             {
   inputPtrContainer::const_iterator iter = mInputs.begin();
   if (pIsLabel)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if (((*iter)->getLabel() == pIDStr) ||
             (stringWithoutHotkeyChars((*iter)->getLabel()) == pIDStr))
             {
            (*iter)->setExtendedHelp(pExtendedHelp);
            break;
         }
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getName() == pIDStr)
         {
            (*iter)->setExtendedHelp(pExtendedHelp);
            break;
         }
      }
   }
} // setExtendedHelp

void cxForm::validateOnJumpKey(int pIndex, bool pValidate)
{
   if ((pIndex >= 0) && (pIndex < (int)(mInputs.size())))
   {
      // If pValidate is true, then remove the jump key from the input's
      //  "skip validator" keys.  Otherwise, add the jump key to the input's
      //  "skip validator" keys.
      if (pValidate)
      {
         mInputs[pIndex]->removeSkipValidatorKey(mInputJumpKey);
      }
      else
      {
         mInputs[pIndex]->addSkipValidatorKey(mInputJumpKey);
      }
   }
} // validateOnJumpKey

void cxForm::validateOnJumpKey(const string& pID, bool pValidate, bool pIsLabel)
{
   inputPtrContainer::iterator iter = mInputs.begin();
   if (pIsLabel)
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getLabel() == pID)
         {
            // If pValidate is true, then remove the jump key from the input's
            //  "skip validator" keys.  Otherwise, add the jump key to the input's
            //  "skip validator" keys.
            if (pValidate)
            {
               (*iter)->removeSkipValidatorKey(mInputJumpKey);
            }
            else
            {
               (*iter)->addSkipValidatorKey(mInputJumpKey);
            }
         }
      }
   }
   else
   {
      for (; iter != mInputs.end(); ++iter)
      {
         if ((*iter)->getName() == pID)
         {
            // If pValidate is true, then remove the jump key from the input's
            //  "skip validator" keys.  Otherwise, add the jump key to the input's
            //  "skip validator" keys.
            if (pValidate)
            {
               (*iter)->removeSkipValidatorKey(mInputJumpKey);
            }
            else
            {
               (*iter)->addSkipValidatorKey(mInputJumpKey);
            }
         }
      }
   }
} // validateOnJumpKey

void cxForm::allValidateOnJumpKey(bool pValidate)
{
   mAllValidateOnJumpKey = pValidate;

   // For each input, if pValidate is true, then remove mInputJumpKey from
   //  the input's list of keys to skip the validator.  Otherwise, add
   //  it to the input's list of keys to skip the validator.
   inputPtrContainer::iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      if (pValidate)
      {
         (*iter)->removeSkipValidatorKey(mInputJumpKey);
      }
      else
      {
         (*iter)->addSkipValidatorKey(mInputJumpKey);
      }
   }
} // allValidateOnJumpKey

bool cxForm::swap(int pInput1Index, int pInput2Index)
{
   bool swapped = false;

   // Make sure pInput1Index and pInput2Index are different and are not out of
   //  bounds.
   if ((pInput1Index == pInput2Index) || (pInput1Index < 0) ||
       (pInput2Index < 0) || (pInput1Index >= (int)(mInputs.size())) ||
       (pInput2Index >= (int)(mInputs.size())))
       {
      swapped = false;
   }
   else
   {
      // Swap the cxWindow pointers
      mInputs[pInput1Index].swap(mInputs[pInput2Index]);
      swapped = true;
   }

   return(swapped);
} // swap

bool cxForm::swap(shared_ptr<cxWindow>& pInput1, shared_ptr<cxWindow>& pInput2)
{
   bool swapped = false;

   // Don't do anything if pInput1 and pInput2 point to the same input.
   if (pInput1 != pInput2)
   {
      // Look for pInput1 and pInput2 in mInputs.
      inputPtrContainer::iterator iter1 = mInputs.begin();
      inputPtrContainer::iterator iter2 = mInputs.begin();
      for (; iter1 != mInputs.end(); ++iter1)
      {
         if (*iter1 == pInput1)
         {
            break;
         }
      }
      for (; iter2 != mInputs.end(); ++iter2)
      {
         if (*iter2 == pInput2)
         {
            break;
         }
      }

      // Swap the cxMultiLineInput pointers, but only if we found them in
      // mInputs.
      if ((iter1 != mInputs.end()) && (iter2 != mInputs.end()))
      {
         iter1->swap(*iter2);
         swapped = true;
      }
   }

   return(swapped);
} // swap

//// Protected functions

void cxForm::copyCxFormStuff(const cxForm* pThatForm)
{
   if ((pThatForm != nullptr) && (pThatForm != this))
   {
      // Free the memory used by the inputs
      freeInputs();

      // Copy the stuff inherited from cxWindow, then copy this
      //  class' stuff.
      try
      {
         copyCxWinStuff((const cxWindow*)pThatForm);
      }
      catch (const cxWidgetsException& exc)
      {
         // Free up the other memory used
         cxWindow::freeWindow();
         throw(cxWidgetsException("Couldn't copy base cxWindow stuff (copying a cxForm)."));
      }

      move(pThatForm->top(), pThatForm->left(), false);
      setTitle(pThatForm->getTitle());
      setMessage(pThatForm->getMessage());
      setStatus(pThatForm->getStatus());
      setBorderStyle(pThatForm->getBorderStyle());
      setExtTitleWindow(pThatForm->getExtTitleWindow());
      setExtStatusWindow(pThatForm->getExtStatusWindow());
      mCurrentInput = pThatForm->mCurrentInput;
      mTrapNonAssignedFKeys = pThatForm->mTrapNonAssignedFKeys;
      mStartAtFirstInput = pThatForm->mStartAtFirstInput;
      mInputPositions = pThatForm->mInputPositions;
      mStacked = pThatForm->mStacked;
      mTotalInputHeight = pThatForm->mTotalInputHeight;
      mAutoExit = pThatForm->mAutoExit;
      mApplyAttrDefaults = pThatForm->mApplyAttrDefaults;
      mCustomStatus = pThatForm->mCustomStatus;
      mEditableInputsExist = pThatForm->mEditableInputsExist;
      mInputsOnBorder = pThatForm->mInputsOnBorder;
      mAllowQuit = pThatForm->mAllowQuit;
      mAllowExit = pThatForm->mAllowExit;
      mExitOnLeaveFirst = pThatForm->mExitOnLeaveFirst;
      mWaitForInputIfEmpty = pThatForm->mWaitForInputIfEmpty;
      mDataChanged = pThatForm->mDataChanged;
      mParentMultiForm = pThatForm->mParentMultiForm;
      mCycleInput = true;
      mLoopStartFunction = pThatForm->mLoopStartFunction;
      mLoopEndFunction = pThatForm->mLoopEndFunction;
      mInputShowBools = pThatForm->mInputShowBools;
      mInputJumpIndexes = pThatForm->mInputJumpIndexes;
      mUseInputStatus = pThatForm->mUseInputStatus;
      mHasBeenShown = pThatForm->mHasBeenShown;
      mExtendedHelpKeys = pThatForm->mExtendedHelpKeys;
      mUseExtendedHelpKeys = pThatForm->mUseExtendedHelpKeys;
      mAllValidateOnJumpKey = pThatForm->mAllValidateOnJumpKey;

      // Copy the other form's inputs
      shared_ptr<cxMultiLineInput> newInput; // Used in creating the copied inputs
      inputPtrContainer::const_iterator iter = pThatForm->mInputs.begin();
      for (; iter != pThatForm->mInputs.end(); ++iter)
      {
         // Copy the input as a cxComboBox or a cxMultiLineInput,
         //  depending on the type of the input.
         if ((*iter)->cxTypeStr() == "cxComboBox")
         {
            newInput = dynamic_pointer_cast<cxComboBox>(*iter);
         }
         else
         {
            newInput = make_shared<cxMultiLineInput>(**iter);
         }
         newInput->setParentForm(this);
         newInput->setUseExtendedHelpKeys(mUseExtendedHelpKeys);
         newInput->setExtendedHelpKeys(mExtendedHelpKeys);
         mInputs.push_back(newInput);
      }
   }
} // copyCxFormStuff

void cxForm::lookForEditableInputs()
{
   mEditableInputsExist = false;
   inputPtrContainer::const_iterator iter = mInputs.begin();
   for (; iter != mInputs.end(); ++iter)
   {
      if (((*iter)->getInputOption() != eINPUT_READONLY) &&
          ((*iter)->isEnabled()))
          {
         mEditableInputsExist = true;
         break;
      }
   }
} // lookForEditableInputs

//// Private functions

// Makes sure this window is exactly the size needed to
//  surround the inputs.
void cxForm::fitToInputs()
{
   // Go through the input windows and look for the
   //  smallest left & top values and the largest
   //  right & bottom values, then set the width
   //  of this form accordingly (as long as it's
   //  not bigger than the main screen).

   int smallestLeft = 9999;
   int largestRight = 0;
   int smallestTop = 9999;
   int largestBottom = 0;
   inputPtrContainer::iterator iter;
   for (iter = mInputs.begin(); iter != mInputs.end(); ++iter)
   {
      if ((*iter)->left() < smallestLeft)
      {
         smallestLeft = (*iter)->left();
      }
      if ((*iter)->right() > largestRight)
      {
          largestRight = (*iter)->right();
      }
      if ((*iter)->top() < smallestTop)
      {
         smallestTop = (*iter)->top();
      }
      if ((*iter)->bottom() > largestBottom)
      {
        largestBottom = (*iter)->bottom();
      }
   }

   // Move & resize this window if it will fit inside
   //  the main screen.
   if ((smallestLeft >= cxBase::left()) && (smallestTop >= cxBase::top()) &&
       (largestRight <= cxBase::right()) && (largestBottom <= cxBase::bottom()))
       {
      move(smallestTop, smallestLeft);
      resize(largestBottom-smallestTop+1, largestRight-smallestLeft+1);
   }
} // fitToInputs

inline void cxForm::useInputStatusAsFormStatus(bool pRefreshStatus)
{
   // Only do this if a custom status message isn't set.
   if (!mCustomStatus)
   {
      int currentInput = getCurrentInputIndex();
      if (currentInput >= 0 && currentInput < (int)mInputs.size())
      {
         // The status message of the inputs are their help messages.
         // Use the base class' setStatus() instead of this
         //  class' setStatus() so that mCustomStatus doesn't get
         //  set to true.
         cxWindow::setStatus(mInputs[currentInput]->getStatus(), pRefreshStatus);
      }
   }
} // useInputStatusAsFormStatus

bool cxForm::selectNextInput()
{
   // This function returns whether the current input was the last
   //  input in mInputs.
   bool lastInput = false;

   if (mEditableInputsExist)
   {
      int previousInput = mCurrentInput;
      if (mCurrentInput >= 0 && mCurrentInput < (int)mInputs.size())
      {
         // If the last character in the current input is a space,
         //  then remove it.  (In testing, it was found that when
         //  the user hits the TAB key to go to the next input, it
         //  was getting an extra space for the current input.  This
         //  is a kludge, but it works..)
         clearInputIfSpace();

         // Select the next input
         ++mCurrentInput;
         // If mCurrentInput is now bigger than the last index
         //  of mInputs, that means mCurrentInput was the index
         //  of the last input - so wrap around to the first.
         //  If mStacked is true,  make sure all the inputs are
         //  moved back to their original positions.
         if (mCurrentInput >= (int)mInputs.size())
         {
            mCurrentInput = 0;
            lastInput = true;

            // Move the inputs to their proper places
            int row = top();
            int col = left();
            if (getBorderStyle() != eBS_NOBORDER)
            {
               ++row;
               ++col;
            }

            int numInputs = (int)mInputs.size();
            for (int i = 0; i < numInputs; ++i)
            {
               if (mStacked)
               {
                  // Move the input
                  mInputs[i]->move(row, col, false);

                  // Update mInputPositions
                  mInputPositions[i].first = row;
                  mInputPositions[i].second = col;
               }

               // Only show the item if it fits inside the window.
               // If we are to show inputs on borders, or there are no
               //  borders, then include the borders in the check.
               //  Otherwise, don't include the borders in the check.
               if ((mInputsOnBorder) || (getBorderStyle() == eBS_NOBORDER))
               {
                  if ((mInputs[i]->top() >= top()) && (mInputs[i]->bottom() <= bottom()) &&
                      (mInputs[i]->left() >= left()) && (mInputs[i]->right() <= right()))
                      {
                     // Show the input if it's not being shown
                     if ((mInputShowBools[i] == false) || (mInputs[i]->isHidden())) {
                        mInputShowBools[i] = true;
                        // Show the input (to refresh it) - And if the
                        //  form window is above the input's window,
                        //  bring the input to the top.
                        mInputs[i]->show(isAbove(*(mInputs[i])), false);
                     }
                  }
                  else
                  {
                     // The input is (at least) partially outside the
                     //  form window, so hide it.
                     mInputShowBools[i] = false;
                     mInputs[i]->hide();
                  }
               }
               else
               {
                  if ((mInputs[i]->top() > top()) && (mInputs[i]->bottom() < bottom()) &&
                      (mInputs[i]->left() > left()) && (mInputs[i]->right() < right()))
                      {
                     // Show the input if it's not being shown
                     if ((mInputShowBools[i] == false) || (mInputs[i]->isHidden())) {
                        mInputShowBools[i] = true;
                        // Show the input (to refresh it) - And if the
                        //  form window is above the input's window,
                        //  bring the input to the top.
                        mInputs[i]->show(isAbove(*(mInputs[i])), false);
                     }
                  }
                  else
                  {
                     // The input is (at least) partially outside the
                     //  form window, so hide it.
                     mInputShowBools[i] = false;
                     mInputs[i]->hide();
                  }
               }

               if (mStacked)
               {
                  ++row;
               }
            }
         }
         // Look for an editable input
         int currentInput = getCurrentInputIndex();
         // While the current input is read-only or disabled..
         //while (!(mInputs[currentInput]->isEditable())) {
         while ((mInputs[currentInput]->getInputOption() == eINPUT_READONLY) ||
                !(mInputs[currentInput]->isEnabled())) {
            // We still want to run its onFocus and onLeave
            //  methods if it is enabled
            //if (mInputs[currentInput]->isEnabled()) {
            //   mInputs[currentInput]->runOnFocusFunction();
            //   mInputs[currentInput]->runOnLeaveFunction();
            //}
            // Go to the next input
            ++currentInput;
            if (currentInput >= (int)mInputs.size())
            {
               currentInput = 0;
               lastInput = true;
            }
         }
         mCurrentInput = currentInput;
      }
      else
      {
         mCurrentInput = 0;
      }

      // Scroll the inputs if part of the current input
      //  is outside the form
      int currentInput = getCurrentInputIndex();
      int inputTop = mInputs[currentInput]->top();
      int inputBottom = mInputs[currentInput]->bottom();
      if (mInputsOnBorder)
      {
         if ((inputTop > bottom()) || (inputBottom < top()))
         {
            int yDiff = mInputs[previousInput]->top() - mInputs[currentInput]->top();
            int xDiff = mInputs[previousInput]->left() - mInputs[currentInput]->left();
            scrollInputs(yDiff, xDiff, true, false);
         }
      }
      else
      {
         if ((inputTop >= bottom()) || (inputBottom <= top()))
         {
            int yDiff = mInputs[previousInput]->top() - mInputs[currentInput]->top();
            int xDiff = mInputs[previousInput]->left() - mInputs[currentInput]->left();
            scrollInputs(yDiff, xDiff, true, false);
         }
      }
   }

   return(lastInput);
} // selectNextInput

bool cxForm::selectPrevInput()
{
   // This function returns whether the current input was the first
   //  input in mInputs.
   bool firstInput = false;

   if (mEditableInputsExist)
   {
      // movedToLast = whether or not we wrapped
      //  around to the last input
      bool movedToLast = false;

      int previousInput = mCurrentInput;
      if (mCurrentInput >= 0 && mCurrentInput < (int)mInputs.size())
      {
         // If the last character in the current input is a space,
         //  then remove it.  (In testing, it was found that when
         //  the user hits the TAB key to go to the next input, it
         //  was getting an extra space for the current input.  This
         //  is a kludge, but it works..)
         clearInputIfSpace();

         // Select the previous editable input
         --mCurrentInput;
         if (mCurrentInput < 0)
         {
            mCurrentInput = (int)mInputs.size()-1;
            firstInput = true;
            movedToLast = true;
         }
         // Look for an editable input
         int currentInput = getCurrentInputIndex();
         // While the current input is read-only or disabled..
         //while (!(mInputs[currentInput]->isEditable())) {
         while ((mInputs[currentInput]->getInputOption() == eINPUT_READONLY) ||
                !(mInputs[currentInput]->isEnabled())) {
            // We still want to run its onFocus and onLeave
            //  methods if it is enabled
            //if (mInputs[currentInput]->isEnabled()) {
            //   mInputs[currentInput]->runOnFocusFunction();
            //   mInputs[currentInput]->runOnLeaveFunction();
            //}
            // Go to the previous input
            --currentInput;
            if (currentInput < 0)
            {
               currentInput = (int)mInputs.size()-1;
               firstInput = true;
               movedToLast = true;
            }
         }
         mCurrentInput = currentInput;
      }
      else
      {
         mCurrentInput = (int)mInputs.size()-1;
         firstInput = true;
         movedToLast = true;
      }

      // Scroll the inputs if part of the current input
      //  is outside the form (but only if in 'stacked' mode).
      if (mStacked)
      {
         int currentInput = getCurrentInputIndex();
         int inputTop = mInputs[currentInput]->top();
         int inputBottom = mInputs[currentInput]->bottom();
         if ((inputTop >= bottom()) || (inputBottom <= top()))
         {
            // Figure out how far to scroll the inputs
            int yDiff = 0;
            int xDiff = mInputPositions[previousInput].second - mInputs[currentInput]->left();
            if (movedToLast)
            {
               if (getBorderStyle() != eBS_NOBORDER)
               {
                  yDiff = bottom() - mInputPositions[currentInput].first - 1;
               }
               else
               {
                  yDiff = bottom() - mInputPositions[currentInput].first;
               }
            }
            else
            {
               yDiff = mInputPositions[previousInput].first - mInputs[currentInput]->top();
            }

            scrollInputs(yDiff, xDiff, true, false);
         }
      }
   }

   return(firstInput);
} // selectPrevInput

// Returns the greatest left side of all the inputs.
inline int cxForm::greatestLeftSide() const
{
   int greatestLeft = 0;

   inputPtrContainer::const_iterator iter;
   for (iter = mInputs.begin(); iter != mInputs.end(); ++iter)
   {
      if ((*iter)->left() > greatestLeft)
      {
         greatestLeft = (*iter)->left();
      }
   }

   return(greatestLeft);
} // greatestLeftSide

// Returns the lowest left side of all the inputs.
inline int cxForm::lowestLeftSide() const
{
   int lowestLeft = 9999;

   inputPtrContainer::const_iterator iter;
   for (iter = mInputs.begin(); iter != mInputs.end(); ++iter)
   {
      if ((*iter)->left() < lowestLeft)
      {
         lowestLeft = (*iter)->left();
      }
   }

   return(lowestLeft);
} // lowestLeftSide

// Returns the greatest top side of all the inputs.
inline int cxForm::greatestTopSide() const
{
   int greatestTop = 0;

   inputPtrContainer::const_iterator iter;
   for (iter = mInputs.begin(); iter != mInputs.end(); ++iter)
   {
      if ((*iter)->top() > greatestTop)
      {
         greatestTop = (*iter)->top();
      }
   }

   return(greatestTop);
} // greatestTopSide

// Returns the lowest top side of all the inputs.
inline int cxForm::lowestTopSide() const
{
   int lowestTop = 9999;

   inputPtrContainer::const_iterator iter;
   for (iter = mInputs.begin(); iter != mInputs.end(); ++iter)
   {
      if ((*iter)->top() < lowestTop)
      {
         lowestTop = (*iter)->top();
      }
   }

   return(lowestTop);
} // lowestTopSide

// Returns the index in mInputs that has the next lowest
//  top side of an input, or -1 if one isn't found.
inline int cxForm::inputNextLowestTop(int topSide)
{
   int nextLowestTop = 9999;

   int index = 0;
   int numInputs = (int)mInputs.size();
   if (mInputs.size() > 0)
   {
      for (index = 0; index < numInputs; ++index)
      {
         if ((mInputs[index]->top() > topSide) &&
             (mInputs[index]->top() < nextLowestTop))
             {
            nextLowestTop = mInputs[index]->top();
         }
      }
   }

   // If index == numInputs, that means we didn't
   //  find a next lowest top, so return -1.
   if (index == numInputs)
   {
      return(-1);
   }
   else
   {
      return(index);
   }
} // inputNextLowestTop

// Returns the index in mInputs that has the next lowest
//  left side of an input, or -1 if one isn't found.
inline int cxForm::inputNextLowestLeft(int leftSide)
{
   int nextLowestLeft = 9999;

   int index = 0;
   int numInputs = (int)mInputs.size();
   if (mInputs.size() > 0)
   {
      for (index = 0; index < numInputs; ++index)
      {
         if ((mInputs[index]->left() > leftSide) &&
             (mInputs[index]->left() < nextLowestLeft))
             {
            nextLowestLeft = mInputs[index]->top();
         }
      }
   }

   // If index == numInputs, that means we didn't
   //  find a next lowest left, so return -1.
   if (index == numInputs)
   {
      return(-1);
   }
   else
   {
      return(index);
   }
} // inputNextLowestLeft

inline void cxForm::clearInputIfSpace()
{
   int currentInput = getCurrentInputIndex();
   string inputVal = mInputs[currentInput]->getValue();
   if (inputVal.length() > 0)
   {
      string::iterator iter = inputVal.end() - 1;
      if (*iter == ' ')
      {
         inputVal.erase(iter);
         mInputs[currentInput]->setValue(inputVal);
      }
   }
} // clearInputIfSpace

inline void cxForm::freeInputs()
{
   for (shared_ptr<cxMultiLineInput>& input : mInputs)
   {
      input.reset(); // TODO: Is this needed here?
   }
   mInputs.clear();
} // freeInputs

long cxForm::doInputLoop(bool& pRunOnLeaveFunction)
{
   long returnCode = cxID_EXIT;

   mEditableInputsExist = hasEditableInputs();
   if (mEditableInputsExist)
   {
      // If mStartAtFirstInput is true, then set mCurrentInput to
      //  the first editable input.
      if (mStartAtFirstInput)
      {
         mCurrentInput = firstEditableInput();
      }

      mLeaveNow = false;
      bool continueOn = true;
      bool functionExists = false; // Whether a function exists for a keypress
      bool mouseEvtInForm = false; // Whether there was a mouse event in the form
      // Keep going while continueOn is true and there are editable inputs
      //  (in case any field function or othe function sets all inputs
      //  read-only) and mLeaveNow is false.

      // for dragging forms around
      int pressedX=0;
      int pressedY=0;
      bool movingForm=false;

      while (continueOn && mEditableInputsExist && !mLeaveNow)
      {
         mouseEvtInForm = false;

         // Run the loop start function, and break from
         //  the input loop if its mExitAfterRun is true.
         if (runLoopStartFunction())
         {
            break;
         }

         // If there are still any editable inputs on the form, get input
         //  from the current input.
         if (hasEditableInputs())
         {
            continueOn = doCurrentInput(returnCode, pRunOnLeaveFunction,
                                        functionExists, false);
#ifdef NCURSES_MOUSE_VERSION
            // If there was a mouse button event outside the form window,
            //  and if the parent window is a cxPanel or a cxMultiForm, then
            //  exit out of the input loop.  This could allow the user to go to
            //  another window.
            if (getLastKey() == KEY_MOUSE)
            {
               // Note: doCurrentInput() updates mMouse, so we don't need to do
               //  that here.
               // Run a function that may exist for the mouse state.  If
               //  no function exists for the mouse state, then process
               //  it here.
               bool mouseFuncExists = false;
               continueOn = handleFunctionForLastMouseState(&mouseFuncExists,
                                                         &pRunOnLeaveFunction);
               if (!mouseFuncExists)
               {
                  if (mouseEvtWasButtonEvt()) { // If it was a mouse button event
                     if (mouseEvtWasInWindow())
                     {
                        mouseEvtInForm = true;
                        if (mouseButton1Clicked()) { // Mouse button 1 clicked
                           // The mouse button event was inside the form.  See if
                           //  the user clicked on another input in the form, and
                           //  if so, go to it.
                           int index = 0;
                           for (; index < (int)(mInputs.size()); ++index)
                           {
                              if (mInputs[index]->pointIsInWindow(mMouse.y, mMouse.x))
                              {
                                 // Set the input only if it can be editable.
                                 if (mInputs[index]->canBeEditable()) {
                                    // Set the input editable, and then set it to
                                    //  have focus for the next iteration of the
                                    //  input loop.
                                    mInputs[index]->setInputOption(eINPUT_EDITABLE);
                                    setCurrentInput(index);
                                 }
                              }
                           }
                        }
                        else if (mouseButton1Pressed())
                        {
                           if (mouseEvtWasInTitle())
                           {
                              movingForm=true;
                              // Looks like they're trying to drag the window...
                              // Let's remember where the first started dragging from
                              pressedX=mMouse.x;
                              pressedY=mMouse.y;
                           }
                        }
                        else if (mouseButton1Released())
                        {
                           if (movingForm)
                           {
                              // TODO: cannot figure out why we cannot move the window UP
                              moveRelative(mMouse.y - pressedY, mMouse.x - pressedX, true);
                           }
                           movingForm=false; // reset the moving of the form
                        }
                     }
                     else
                     {
                        // The mouse button event was outside the form.  If the
                        //  parent window is a cxPanel or a cxMultiForm, then
                        //  quit the input loop.  This can allow the user to go
                        //  to another window.
                        if (parentIsCxPanel() || (mParentMultiForm != nullptr))
                        {
                           continueOn = false;
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
            else
            {
               // If the last key is a quit key, then quit and return
               //  cxID_QUIT.  If the key isn't there, look for it in
               //  the exit keys (if it's there, quit and return cxID_EXIT).
               //  If not there either, handle the key normally.
               if (hasQuitKey(getLastKey()))
               {
                  returnCode = cxID_QUIT;
                  continueOn = false;
               }
               else if (hasExitKey(getLastKey()))
               {
                  returnCode = cxID_EXIT;
                  continueOn = false;
               }
            }
         }
         else
         {
            cxBase::messageBox("This form has no editable fields.");
            continueOn = false;
         }

         // If there was no mouse event in the form, then run the end loop
         //  function, and update continueOn based on its current value and the
         //  value of the end loop function's mExitAfterRun.
         if (!mouseEvtInForm)
         {
            continueOn = (continueOn && !runLoopEndFunction());
            // Make sure we can cycle through the inputs next time through the
            //  loop.
            mCycleInput = true;

            // Update mEditableInputsExist
            mEditableInputsExist = hasEditableInputs();
         }
      }

      // If using the inputs' status as the form status, then clear the
      //  status line before we leave.
      if (mUseInputStatus && !mCustomStatus)
      {
         setStatus("");
         // setStatus() will set mCustomStatus true..  Set it back.
         mCustomStatus = false;
         // Make sure mUseInputStatus is also still true.
         mUseInputStatus = true;
      }
   }
   else
   {
      // No editable inputs - just wait for a keypress (if mWaitForInputIfEmpty
      //  is true).

      //  The loop start & end functions should still be run.
      if (mLoopStartFunction != nullptr && mLoopStartFunction->functionIsSet())
      {
         mLoopStartFunction->runFunction();
      }

      // Wait for input from the user if mWaitForInputIfEmpty
      //  is true.
      if (mWaitForInputIfEmpty)
      {
         // Disable the cursor (storing the previous cursor state)
         bool prevCursorState = cxBase::toggleCursor(false);

         // We have a loop here in case the user presses ESC but
         //  we're not allowed to quit (in that case, we'll keep on
         //  getting keypresses).
         bool continueOn = true;
         while (continueOn)
         {
            setLastKey(wgetch(mWindow));
#ifdef NCURSES_MOUSE_VERSION
            // If there was a mouse button event outside the form window,
            //  and if the parent window is a cxPanel or a cxMultiForm, then
            //  exit out of the input loop (allowing the user to go to another
            //  window).
            if (getLastKey() == KEY_MOUSE)
            {
               if (getmouse(&mMouse) == OK)
               {
                  // Run a function that may exist for the mouse state.  If
                  //  no function exists for the mouse state, then process
                  //  it here.
                  bool mouseFuncExists = false;
                  continueOn = handleFunctionForLastMouseState(&mouseFuncExists,
                                             &pRunOnLeaveFunction);
                  if (!mouseFuncExists)
                  {
                     if (mouseEvtWasButtonEvt() && !mouseEvtWasInWindow())
                     {
                        if (parentIsCxPanel() || (mParentMultiForm != nullptr))
                        {
                           continueOn = false;
                        }
                     }
                  }
               }
            }
#endif
            if (continueOn)
            {
               continueOn = handleFunctionForLastKey();
            }
            // Stop now if continueOn was set to false.
            if (!continueOn)
            {
               break;
            }

            // If the user pressed ESC, or the key is found in mQuitKeys,
            //  then quit if we're allowed to.  Otherwise exit.
            if ((getLastKey() == ESC) ||
                (mQuitKeys.find(getLastKey()) != mQuitKeys.end()))
                {
               if (mAllowQuit)
               {
                  returnCode = cxID_QUIT;
                  continueOn = false;
                  // The user wanted to just quit; don't run the onLeave
                  //  function.
                  pRunOnLeaveFunction = false;
               }
            }
            else if (getLastKey() == NOKEY)
            {
               // No key - Do nothing
            }
            else
            {
               // Don't check mAllowExit here..  If mAllowExit and mAllowQuit
               //  are both false, then a cxForm with no inputs would never
               //  allow the user to get out.
               returnCode = cxID_EXIT;
               continueOn = false;
            }
         }

         // Set the previous cursor state back
         cxBase::toggleCursor(prevCursorState);
      }

      if (mLoopEndFunction != nullptr && mLoopEndFunction->functionIsSet())
      {
         mLoopEndFunction->runFunction();
      }
   }

   // mLeaveNow should be false
   mLeaveNow = false;

   return(returnCode);
} // doInputLoop

bool cxForm::doCurrentInput(long& pReturnCode, bool& pRunOnLeaveFunction,
                            bool& pFunctionExists, bool pExitOverride)
                            {
   // continueOn signifies whether or not the input loop should
   //  continue.
   bool continueOn = true;

   // If there are no editable inputs, then
   //  just wait for a key.  Otherwise, get
   //  a key from the current input.
   if (!mEditableInputsExist)
   {
      // Should we tell the user that no editable inputs exist,
      //  or just wait for a keypress?  (If there are no editable
      //  inputs, the user might still want to press F4 to go
      //  to an input, which will make it editable so the user
      //  can edit it).
      setLastKey(wgetch(mWindow));
#ifdef NCURSES_MOUSE_VERSION
      // If the key was a mouse event, then capture it in mMouse.
      if (getLastKey() == KEY_MOUSE)
      {
         getmouse(&mMouse);
      }
#endif
      continueOn = false;
   }
   else
   {
      // If the current input is read-only, go to the next
      //  editable input.
      if (mInputs[getCurrentInputIndex()]->getInputOption() == eINPUT_READONLY)
      {
         selectNextInput();
      }

      if (mUseInputStatus)
      {
         // Use the current input's status text as the status for the form
         //  window.
         useInputStatusAsFormStatus(true);
      }

      // Store the current value of the input so we can see if
      //  any data changed.
      int currentInput = getCurrentInputIndex();
      string oldValue = mInputs[currentInput]->getValue();
      // Set focus to the current input and take appropriate
      //  action based on the last keystroke.
      pReturnCode = mInputs[currentInput]->showModal();
      setLastKey(mInputs[currentInput]->getLastKey());
#ifdef NCURSES_MOUSE_VERSION
      // If the input captured a mouse event, then set mMouse to the input's
      //  mMouse.  Note: Not using getmouse() here because if an onLeave
      //  function for the input was fired and the user used the mouse there,
      //  then getmouse() would get that mouse event rather than the one from
      // the input, which is not what we would want.
      if (getLastKey() == KEY_MOUSE)
      {
         mMouse = mInputs[currentInput]->getMouseEvent();
      }
#endif
      // Update mDataChanged if oldValue is different from the
      //  current value in the input.
      if (mInputs[currentInput]->getValue() != oldValue)
      {
         mDataChanged = true;
      }
      // In case the current input went to read-only mode after
      //  its showModal(), update mEditableInputsExist.
      if (mInputs[currentInput]->getInputOption() == eINPUT_READONLY)
      {
         lookForEditableInputs();
      }
   }

   // If the last key is in mQuitKeys, then quit and return
   //  cxID_QUIT, if we're allowed to.  If the key isn't
   //  there, look for it in the exit keys (if it's there, quit
   //  and return cxID_EXIT).  If not there either, handle
   //  they key in the switch.
   if (hasQuitKey(getLastKey()))
   {
      if (mAllowQuit || pExitOverride)
      {
         pReturnCode = cxID_QUIT;
         continueOn = false;
      }
   }
   else if (hasExitKey(getLastKey()))
   {
      if (mAllowExit || pExitOverride)
      {
         pReturnCode = cxID_EXIT;
         continueOn = false;
      }
   }
   else
   {
      int lastKey = getLastKey();
      switch(lastKey)
      {
         // NOKEY is for no key pressed (can happen
         //  with read-only inputs, etc.)
         case NOKEY:
         // Navigation keys for going between inputs
         case TAB:
         case SHIFT_TAB:
         case ENTER:
         case KEY_ENTER:
         case KEY_DOWN:
         case KEY_UP:
            // If there are no editable inputs, then
            //  just exit the form.
            if (!mEditableInputsExist)
            {
               pReturnCode = cxID_EXIT;
               continueOn = false;
            }
            else
            {
               // See if there are any functions to run.
               continueOn = handleFunctionForLastKey(&pFunctionExists);
               // If a function existed here for the last keypress, then look
               //  in mKeyFunctions to see if we should run the onLeave
               //  function after the function was fired.
               if (pFunctionExists)
               {
                  if (mKeyFunctions.find(lastKey) != mKeyFunctions.end())
                  {
                     // The cxFunction pointers in mKeyFunctions shouldn't be
                     //  nullptr, but check anyway, just to be sure.
                     if (mKeyFunctions[lastKey] != nullptr)
                     {
                        pRunOnLeaveFunction = pRunOnLeaveFunction &&
                                           mKeyFunctions[lastKey]->getRunOnLeaveFunction();
                     }
                  }
               }
               else
               {
                  // No function exists for the key..  Go to the next/previous
                  //  input.
                  // cxInput returns cxID_QUIT for an up arrow..
                  //  Change the return code back to cxID_EXIT.
                  pReturnCode = cxID_EXIT;
                  // Un-set focus for the current input and
                  //  refresh it.
                  int currentInput = getCurrentInputIndex();
                  mInputs[currentInput]->unSetFocusColors();
                  // Show the input (to refresh it) - And bring it
                  //  to the top if the form window is above the
                  //  input's window.
                  mInputs[currentInput]->show(isAbove(*(mInputs[currentInput])), false);

                  // If the last keypress was a tab, enter, or down arrow,
                  //  go to the next input.  Otherwise, go to
                  //  the previous input.  But only if mCycleInput is true.
                  if (mCycleInput)
                  {
                     if ((lastKey == TAB) || (lastKey == ENTER) ||
                         (lastKey == KEY_ENTER) || (lastKey == KEY_DOWN) ||
                         (lastKey == NOKEY))
                         {
                        // If the current input was the last input, and
                        //  mAutoExit is true, then quit out of the loop.
                        bool wasLastInput = selectNextInput();
                        continueOn = !(wasLastInput && (mAutoExit || pExitOverride));
                     }
                     else
                     {
                        // If the user is allowed to leave the form when
                        //  exiting the first input going backwards, then
                        //  if the current input was the first input, and
                        //  mAutoExit or pExitOverride are true, then
                        //  don't continue on.
                        bool wasFirstInput = selectPrevInput();
                        if (mExitOnLeaveFirst)
                        {
                           continueOn = !(wasFirstInput && (mAutoExit || pExitOverride));
                        }
                     }
                  }
               }
            }
            break;
         case ESCAPE: // Defined in cxKeyDefines.h
            // In case there's a function set up for the escape key, look for
            //  one and run it if there is one.  If there isn't a function set
            //  up for the escape key, then the default behavior is to exit the
            //  form if we are allowed to.
            continueOn = handleFunctionForLastKey(&pFunctionExists);
            if (!pFunctionExists)
            {
               if (mAllowQuit || pExitOverride)
               {
                  continueOn = false;
                  pReturnCode = cxID_QUIT;
               }
            }
            break;
         case ERR:    // Error getting input
            break;
         default:
            // See if there are any functions to run.
            continueOn = handleFunctionForLastKey(&pFunctionExists);
            if (pFunctionExists)
            {
               if (!continueOn)
               {
                  pReturnCode = cxID_EXIT;
               }
            }
            else
            {
               // No function exists for the last keypress.
               // If the user pressed the hotkey to jump to another input,
               //  then do that.  Otherwise, see if there are any
               //  functions to run for the keypress.
               if (lastKey == mInputJumpKey)
               {
                  showJumpMenu();
               }
               else
               {
                  // If mTrapNonAssignedFKeys is false, then:
                  //  If we're on the last editable input and mAutoExit is
                  //  true, and we're allowed to exit, then don't continue
                  //  on.
                  if (!mTrapNonAssignedFKeys)
                  {
                     int lastEditableInput = highestEditableInput();
                     if (lastEditableInput >= 0)
                     {
                        if ((mCurrentInput == lastEditableInput) &&
                            mAutoExit && (mAllowExit || pExitOverride))
                            {
                           continueOn = false;
                        }
                     }
                     else
                     {
                        if (mAllowExit || pExitOverride)
                        {
                           continueOn = false;
                        }
                     }
                  }
               }
            }
            break;
      }
   }

   return(continueOn);
} // doCurrentInput

void cxForm::setIsModal(bool pIsModal)
{
   mIsModal = pIsModal;
} // setIsModal

string cxForm::changeFocus(void *theForm, void *theIndex)
{
   if ((nullptr == theForm) || (nullptr == theIndex))
   {
      return("");
   }

   cxForm *pForm = (cxForm*)theForm;
   int *pIndex = (int*)theIndex;

   // See if the input is a read-only input
   shared_ptr<cxMultiLineInput> theInput = pForm->getInput(*pIndex);
   bool wasReadOnly = (theInput->getInputOption() == eINPUT_READONLY);
   // Set the input to normal (not read-only)
   theInput->setInputOption(eINPUT_EDITABLE);
   // If the input was originally read-only, tell it to go
   //  back to read-only upon exit.
   theInput->setReadOnlyOnLeave(wasReadOnly);

   // Tell the form that the input at pIndex should be the
   //  next input to get focus
   pForm->setCurrentInput(*pIndex);

   return("");
} // changeFocus

void cxForm::addJumpKeys(const string& pItemText, int pIndex)
{
   set<char> hotkeys;
   // Get the hotkeys from the string
   cxBase::getHotkeyChars(pItemText, hotkeys, true, false);
   // Add the hotkeys as input jump keys
   set<char>::const_iterator iter = hotkeys.begin();
   for (; iter != hotkeys.end(); ++iter)
   {
      addInputJumpKey(*iter, pIndex);
   }
} // addJumpKey

void cxForm::showJumpMenu()
{
   if (anyInputsCanBeEditable())
   {
      // Instantiate the menu
      cxMenu jumpMenu(this, 1, 1, 8, 40, "Choose an input");
      jumpMenu.center(false);
      jumpMenu.setTitleColor(eBRTCYAN_BLUE);
      jumpMenu.setBorderColor(eYELLOW_BLUE);
      jumpMenu.setStatus("Home=Top End=Bottom PgUp PgDn /=Search", true);
      // Add the input labels/names to the menu
      int numInputs = (int)mInputs.size();
      for (int i = 0; i < numInputs; ++i)
      {
         // If the input can be set editable, add it to the menu.
         if (mInputs[i]->canBeEditable()) {
            // Check the label first - If it's not blank, use it;
            //  otherwise, use the input name (hopefully both
            //  aren't blank!)
            string menuItem = mInputs[i]->getLabel();
            if (menuItem == "")
            {
               menuItem = mInputs[i]->getName();
            }

            jumpMenu.append(menuItem, i, "", cxITEM_NORMAL, false);
         }
      }

      // Show the menu
      long returnCode = jumpMenu.showModal();
      // Continue showing the menu until the user selects an
      //  input that's editable (they can still type ESC to
      //  quit out of the menu).
      while ((returnCode != cxID_QUIT) &&
            !(getInput((int)returnCode)->canBeEditable()))
            {
         cxBase::messageBox("Sorry, that field cannot be edited.");
         returnCode = jumpMenu.showModal();
      }

      if (returnCode != cxID_QUIT)
      {
         // The return code of the jump menu is the index to
         //  which input should get the focus.

         // Set the input editable
         setInputOption((int)returnCode, eINPUT_EDITABLE);
         // Just in case, check to see if the input did
         //  get set editable.  If so, then update
         //  mEditableInputsExist, have focus go to the
         //  chosen input next in the loop, etc.
         if (getInputOption((int)returnCode) == eINPUT_EDITABLE)
         {
            mEditableInputsExist = true;
            // Have the new input get the next focus.
            setCurrentInput((int)returnCode);
         }
         else
         {
            cxBase::messageBox("Sorry, that field cannot be edited.");
         }
      }

      jumpMenu.hide();
   }
   else
   {
      cxWindow iWindow(this, 0, 0, 1, 1, "", "There are no editable fields on this form.");
      iWindow.center(false);
      iWindow.setBorderColor(eYELLOW_BLUE);
      iWindow.setMessageColor(eBRTCYAN_BLUE);
      iWindow.show();
      wgetch(mWindow);
      iWindow.hide();
   }
} // showJumpMenu

int cxForm::highestEditableInput() const
{
   int index = -1;

   if (mInputs.size() > 0)
   {
      index = (int)mInputs.size() - 1;
      while ((index >= 0) && (mInputs[index]->getInputOption() == eINPUT_READONLY))
      {
         --index;
      }
   }

   return(index);
} // highestEditableInput

void cxForm::applyAttrDefaults(shared_ptr<cxMultiLineInput>& pInput)
{
   // item will store which cxWidgets item to apply defaults to.
   e_WidgetItems item = eDATA_EDITABLE;
   if (pInput->getInputOption() != eINPUT_EDITABLE)
   {
      // If the input is not eINPUT_EDITABLE, assume it's a read-only input.
      item = eDATA_READONLY;
   }

   // Remove the current attributes from the input
   pInput->removeAttrs(item);

   // Get the form's attributes for the item, and if there are none, use
   //  cxBase's attributes for the item.
   set<attr_t> attrs;
   getAttrs(item, attrs);
   if (attrs.size() == 0)
   {
      cxBase::getAttrs(item, attrs);
   }

   // Apply the attributes from cxBase
   set<attr_t>::const_iterator iter = attrs.begin();
   for (; iter != attrs.end(); ++iter)
   {
      pInput->addAttr(item, *iter);
   }
} // applyAttrDefaults

void cxForm::applyWinAttributes(shared_ptr<cxMultiLineInput>& pInput)
{
   // Loop through each e_WidgetItems value, and get the attributes and
   //  apply the attributes to the input.
   set<attr_t> attrs;
   set<attr_t>::const_iterator attrIter;
   int item = eMESSAGE; // First e_WidgetItems value (eHOTKEY is the last)
   for (; item <= eHOTKEY; item++)
   {
      getAttrs((e_WidgetItems)item, attrs);
      for (attrIter = attrs.begin(); attrIter != attrs.end(); ++attrIter)
      {
         pInput->setAttr((e_WidgetItems)item, *attrIter);
      }
   }
} // applyWinAttributes
