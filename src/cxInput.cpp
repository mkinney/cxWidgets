// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxInput.h"
#include "cxBase.h"
#include "cxStringUtils.h"
#include "cxMultiLineInput.h"
#include <locale.h>
#include <exception> // for try/catch
#include <typeinfo> // for bad_cast
#include <cstdlib>
using std::string;
using std::set;
using cxStringUtils::indexOfLastCap;
using cxStringUtils::isPrintable;
using std::isupper;
using std::shared_ptr;
using std::make_shared;
using cxBase::visualStrLen;

// Default the input clear key to F10
int cxInput::inputClearKey = KEY_F(10);

cxInput::cxInput(cxWindow *pParentWindow, int pRow,
                 int pCol, int pWidth, const string& pLabel,
                 eBorderStyle pBorderStyle, eInputOptions pInputOption,
                 bool pExitOnFull, string *pExtVal)
   : cxWindow(pParentWindow, pRow, pCol, 1,
         // Truncate the label if it doesn't fit in the
         //  given width.
         pBorderStyle == eBS_NOBORDER ?
         (pWidth >= (int)visualStrLen(pLabel.substr(0, pWidth)) ? pWidth :
                  (int)visualStrLen(pLabel.substr(0, pWidth))) :
         (pWidth >= (int)visualStrLen(pLabel.substr(0, pWidth))+2 ? pWidth :
                  (int)visualStrLen(pLabel.substr(0, pWidth))+2),
         "", pLabel.substr(0, pWidth), "", pBorderStyle, nullptr, nullptr),
     mExtValue(pExtVal),
     mExitOnFull(pExitOnFull),
     mInputOption(pInputOption),
     mValueColorPair(mMessageColorPair)
{
   // Enable the use of underlines for the label
   cxWindow::setHotkeyHighlighting(true);
   // Resize the input to make sure it's only 1 line
   //  high (setHotkeyHighlighting doesn't know that
   //  it needs to be only 1 line)
   resize(1, pWidth, false);

   // Set up the X input cursor position
   if (mMessageLines.size() > 0)
   {
      mInputStartX = (int)(cxBase::visualStrLen(mMessageLines[0]));
   }

   // If there's a border, the X & Y positions
   //  will both be 1, and the input length will
   //  be 2 less.
   if (hasBorder())
   {
      ++mInputStartX;
      mYPos = 1;
   }

   // Figure out the length of input to accept (mInputLen)
   mRightMax = right();
   if (!hasBorder())
   {
      // There's no border, so the rightmost
      //  max. character position should be
      //  1 more.
      ++mRightMax;
   }
   mInputLen = width() - mInputStartX;
   // If there is a border, subtract 1 from mInputLen
   //  to account for the right border.
   if (hasBorder())
   {
      --mInputLen;
   }

   // Reserve enough characters for mValue so that it
   //  shouldn't have to allocate more memory as text is
   //  added to it.
   if (0 > mInputLen)
   {
      mInputLen = 0;
   }
   mValue.reserve(mInputLen);

   // If mExtValue is not nullptr, then update the value of
   //  this input.
   if (mExtValue != nullptr)
   {
      setValue(*mExtValue);
   }

#ifdef WANT_TIMEOUT
   // Set up mTimeoutSigaction
   sigemptyset(&mTimeoutSigaction.sa_mask);
   sigaddset(&mTimeoutSigaction.sa_mask, SIGALRM);
   mTimeoutSigaction.sa_flags = 0;
   mTimeoutSigaction.sa_handler = cxInput::idleTimeoutHandler;
   if (sigaction(SIGALRM, &mTimeoutSigaction, nullptr) < 0)
   {
   }
#endif

   // Set mMaxInputLength
   mMaxInputLength = mInputLen;
}

// Copy constructor
cxInput::cxInput(const cxInput& pThatInput)
   : cxWindow(nullptr, pThatInput.top(), pThatInput.left(), 1,
              pThatInput.width(), pThatInput.getTitle(), pThatInput.getLabel(),
              pThatInput.getStatus(), pThatInput.getBorderStyle(), nullptr, nullptr),
     mLabelAttrs(pThatInput.mLabelAttrs),
     mDataReadonlyAttrs(pThatInput.mDataReadonlyAttrs),
     mDataEditableAttrs(pThatInput.mDataEditableAttrs),
     mValue(pThatInput.mValue),
     mName(pThatInput.mName),
     mExtValue(pThatInput.mExtValue),
     mInputStartX(pThatInput.mInputStartX),
     mYPos(pThatInput.mYPos),
     mInputLen(pThatInput.mInputLen),
     mRightMax(pThatInput.mRightMax),
     mExitOnFull(pThatInput.mExitOnFull),
     mExitOnBackspaceAtFront(pThatInput.mExitOnBackspaceAtFront),
     mMustFill(pThatInput.mMustFill),
     mMasked(pThatInput.mMasked),
     mMaskChar(pThatInput.mMaskChar),
     mValidator(pThatInput.mValidator),
     mInputOption(pThatInput.mInputOption),
     mExitOnFunctionKey(pThatInput.mExitOnFunctionKey),
     mHasFocus(pThatInput.mHasFocus),
     mReadOnlyOnLeave(pThatInput.mReadOnlyOnLeave),
     mCanBeEditable(pThatInput.mCanBeEditable),
     mTrapNonAssignedFKeys(pThatInput.mTrapNonAssignedFKeys),
     mDoInputLoop(pThatInput.mDoInputLoop),
     mCursorAfterInput(pThatInput.mCursorAfterInput),
     mRunOnKeyFunction(pThatInput.mRunOnKeyFunction),
     mValueColorPair(pThatInput.mValueColorPair),
     mShowCursor(pThatInput.mShowCursor),
     mValidateOnReverse(pThatInput.mValidateOnReverse),
     mForceUpper(pThatInput.mForceUpper),
     mMaxInputLength(pThatInput.mMaxInputLength)
#ifdef WANT_TIMEOUT
     , mTimeout(pThatInput.mTimeout)
#endif
{
   // If mExtValue is not nullptr, then update the value of
   //  this input.
   if (mExtValue != nullptr)
   {
      setValue(*mExtValue);
   }

   // Copy the other input's mOnKeyFunction and mValidatorFunction
   copyCxFunction(pThatInput, mOnKeyFunction, pThatInput.mOnKeyFunction);
   copyCxFunction(pThatInput, mValidatorFunction, pThatInput.mValidatorFunction);
} // Copy constructor

// Second copy constructor
cxInput::cxInput(const cxInput& pThatInput, cxMultiLineInput *pParentMLInput)
   : cxWindow(nullptr, pThatInput.top(), pThatInput.left(), 1,
              pThatInput.width(), pThatInput.getTitle(), pThatInput.getLabel(),
              pThatInput.getStatus(), pThatInput.getBorderStyle(), nullptr, nullptr),
     mLabelAttrs(pThatInput.mLabelAttrs),
     mDataReadonlyAttrs(pThatInput.mDataReadonlyAttrs),
     mDataEditableAttrs(pThatInput.mDataEditableAttrs),
     mParentMLInput(pParentMLInput),
     mValue(pThatInput.mValue),
     mName(pThatInput.mName),
     mExtValue(pThatInput.mExtValue),
     mInputStartX(pThatInput.mInputStartX),
     mYPos(pThatInput.mYPos),
     mInputLen(pThatInput.mInputLen),
     mRightMax(pThatInput.mRightMax),
     mExitOnFull(pThatInput.mExitOnFull),
     mExitOnBackspaceAtFront(pThatInput.mExitOnBackspaceAtFront),
     mMustFill(pThatInput.mMustFill),
     mMasked(pThatInput.mMasked),
     mMaskChar(pThatInput.mMaskChar),
     mValidator(pThatInput.mValidator),
     mInputOption(pThatInput.mInputOption),
     mExitOnFunctionKey(pThatInput.mExitOnFunctionKey),
     mHasFocus(pThatInput.mHasFocus),
     mReadOnlyOnLeave(pThatInput.mReadOnlyOnLeave),
     mCanBeEditable(pThatInput.mCanBeEditable),
     mTrapNonAssignedFKeys(pThatInput.mTrapNonAssignedFKeys),
     mDoInputLoop(pThatInput.mDoInputLoop),
     mCursorAfterInput(pThatInput.mCursorAfterInput),
     mRunOnKeyFunction(pThatInput.mRunOnKeyFunction),
     mValueColorPair(pThatInput.mValueColorPair),
     mShowCursor(pThatInput.mShowCursor),
     mValidateOnReverse(pThatInput.mValidateOnReverse),
     mForceUpper(pThatInput.mForceUpper),
     mMaxInputLength(pThatInput.mMaxInputLength)
#ifdef WANT_TIMEOUT
     , mTimeout(pThatInput.mTimeout)
#endif
{
   // If mExtValue is not nullptr, then update the value of
   //  this input.
   if (mExtValue != nullptr)
   {
      setValue(*mExtValue);
   }

   // Copy the other input's mOnKeyFunction and mValidatorFunction
   copyCxFunction(pThatInput, mOnKeyFunction, pThatInput.mOnKeyFunction);
   copyCxFunction(pThatInput, mValidatorFunction, pThatInput.mValidatorFunction);
} // Second copy constructor

cxInput::~cxInput()
{
}

long cxInput::show(bool pBringToTop, bool pShowSubwindows)
{
   long returnVal = cxFIRST_AVAIL_RETURN_CODE;
   // Only show the input if it's enabled; otherwise,
   //  hide the input.
   if (isEnabled())
   {
      // If the "external" user value pointer is not nullptr,
      //  update the input box value, just in case it
      //  has changed.
      if (mExtValue != nullptr)
      {
         setValue(*mExtValue);
      }

      draw();
      returnVal = cxWindow::show(pBringToTop, false);
      // Update this input's window
      cxBase::updateWindows();
   }
   else
   {
      hide(false);
   }

   return(returnVal);
} // show

long cxInput::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   setReturnCode(cxFIRST_AVAIL_RETURN_CODE);
   // Only do this if the input is enabled.
   if (isEnabled())
   {
      mIsModal = true;
      mHasFocus = true;
      mJustStartedFocus = true;

      // Run the onFocus function.  If it returns true, that means we should
      //  exit.. so only do the input loop if it returns false or we aren't
      //  set to run the onFocus function.  The onFocus function should be run
      //  before the window is shown, in case its return value is to be set as
      //  the value of the input.  Also, check the value of getLeaveNow() in
      //  case the onFocus function called exitNow() or quitNow().
      bool continueOn = !runOnFocusFunction() && !getLeaveNow();

      // Show the window
      if (pShowSelf)
      {
         show(true, pShowSubwindows);
      }
      else
      {
         // pShowSelf is false, so enable the value attributes here before the
         //  user starts typing. (they're normally enabled in show().)
         if (eINPUT_EDITABLE == mInputOption)
         {
            enableAttrs(mWindow, eDATA_EDITABLE);
         }
         else if (eINPUT_READONLY == mInputOption)
         {
            enableAttrs(mWindow, eDATA_READONLY);
         }
      }

      if (continueOn)
      {
         setReturnCode(cxID_EXIT);

         int prevCursorState = 0; // Previous cursor state
         if (mShowCursor)
         {
            prevCursorState = curs_set(1); // Enable the cursor
         }

         // If this input is read-only, don't do any input
         //  processing.
         if (mInputOption == eINPUT_READONLY)
         {
            // Disable the attributes & colors
            disableAttrs(mWindow, eDATA_READONLY);
            if (useColors)
            {
               wcolor_set(mWindow, 0, nullptr);
            }
            return(getReturnCode());
         }

         // Enable colors
         if (useColors)
         {
            wcolor_set(mWindow, mValueColorPair, nullptr);
         }

         // prevInput will hold input from the last iteration through the
         //  loop.  It's set to mValue in case mValue is already set (by
         //  a call to setValue, for example).
         string prevInput(mValue);
         bool updatePrevInput = true; // Whether or not to update prevInput
         int y = 0, x = 0; // Current character position
         getyx(mWindow, y, x);
         // rightLimit is the rightmost limit (highest column# on the
         //  screen, inclusive) for the user input.
         int rightLimit = mInputStartX;
         // If mMaxInputLength is less than the length that will fit inside the
         //  window, then use mMaxInputLength in calculating rightLimit;
         //  otherwise, use mInputLen.
         if (mMaxInputLength < mInputLen)
         {
            rightLimit = mInputStartX + mMaxInputLength - 1;
         }
         else
         {
            rightLimit = mInputStartX + mInputLen - 1;
         }
         // If the horizontal cursor position is beyond the
         //  right limit (after having written the value to
         //  the window), then set x to the right limit so
         //  that doInputLoop will still do its loop at least
         //  once.
         if (x > rightLimit)
         {
            wmove(mWindow, mYPos, rightLimit);
            x = rightLimit;
         }
         // Start the input loop
         setReturnCode(doInputLoop(x, y, rightLimit, updatePrevInput, prevInput));
         // If the user doesn't want to quit, then if there is a validator function
         //  set, continue doing the input loop until the input is valid.
         // Note: cxMultiLineInput has additional logic when it does this..  It
         //  has a list of keys that cause the input not to run the validator function
         //  when they are pressed (for keys that run a function).
         if ((validatorFunctionIsSet()) && (getReturnCode() != cxID_QUIT))
         {
            bool doValidation = true;
            // If mValidateOnReverse is false and the user pressed a
            //  key to navigate in reverse (i.e. shift-tab or the up
            //  arrow), then don't run the validator function.
            if (!mValidateOnReverse)
            {
               if ((getLastKey() == SHIFT_TAB) ||
                   (getLastKey() == KEY_UP))
                   {
                  doValidation = false;
               }
            }
            if (doValidation)
            {
               while ((runValidatorFunction() != "") &&
                      (getReturnCode() != cxID_QUIT))
                      {
                  setReturnCode(doInputLoop(x, y, rightLimit, updatePrevInput, prevInput));
                  // If mValidateOnReverse is false, then check for
                  //  reverse navigation keys (shift-tab & up arrow).
                  //  If the user pressed those keys, then stop doing
                  //  doing validation.
                  if (!mValidateOnReverse)
                  {
                     int lastKey = getLastKey();
                     if ((lastKey == SHIFT_TAB) || (lastKey == KEY_UP))
                     {
                        break;
                     }
                  }
               }
            }
         }

         mHasFocus = false;
         mIsModal = false;

         // Do the rest of this stuff if the user didn't press a key
         //  that's in mExitKeys.
         if (!hasExitKey(cxWindow::getLastKey()))
         {
            // If masking is enabled, then have mValidator add the implied characters
            //  to mValue now (since it wasn't being done during input).
            if (mMasked)
            {
               mValidator.addImpliedChars(mValue);
            }

            // Run the "on leave" function (if the user doesn't
            //  want to quit out of the input)
            if (getReturnCode() != cxID_QUIT)
            {
               runOnLeaveFunction();
            }
         }

         if (mShowCursor)
         {
            // Set the cursor mode back to the original state
            curs_set(prevCursorState);
         }

         // Disable the value attributes & color
         disableAttrs(mWindow, eDATA_EDITABLE);
         if (useColors)
         {
            wcolor_set(mWindow, 0, nullptr);
         }

         // If mSetReadOnlyOnLeave is true, set the input read-only.
         if (mReadOnlyOnLeave)
         {
            setInputOption(eINPUT_READONLY);
         }
      }
      else
      {
         mHasFocus = false;
         mIsModal = false;
         // Disable the attributes
         if (eINPUT_EDITABLE == mInputOption)
         {
            enableAttrs(mWindow, eDATA_EDITABLE);
         }
         else if (eINPUT_READONLY == mInputOption)
         {
            enableAttrs(mWindow, eDATA_READONLY);
         }
      }
   }

   return(getReturnCode());
} // showModal

// Returns the label for this input
string cxInput::getLabel() const
{
   if (mMessageLines.size() > 0)
   {
      return(*(mMessageLines.begin()));
   }
   else
   {
      return("");
   }
} // getLabel

void cxInput::setLabel(const string& pLabel)
{
   setMessage(pLabel);

   // Set up the X input cursor position, mInputStartX,
   //  and mInputLen again (see the constructor
   //  to see how they're initially set up).
   if (mMessageLines.size() > 0)
   {
      mInputStartX = (int)(cxBase::visualStrLen(mMessageLines[0]));
   }

   if (hasBorder())
   {
      ++mInputStartX;
   }

   mInputLen = width() - mInputStartX;
   if (hasBorder())
   {
      --mInputLen;
   }

   // Truncate mValue if it's now too long for the window.
   if (mValue.length() > (unsigned)mInputLen)
   {
      mValue = mValue.substr(0, mInputLen);
   }
} // setLabel

void cxInput::clearValue(bool pRefresh)
{
   mValue.erase();
   if (mExtValue != nullptr)
   {
      mExtValue->erase();
   }
   refreshValue(pRefresh);
} // clearValue

void cxInput::clear(bool pRefresh)
{
   clearValue(pRefresh);
} // clear

// Returns the user's input
string cxInput::getValue(bool pRemoveLeadingSpaces, bool pRemoveTrailingSpaces) const
{
   string theValue;

   if (mExtValue != nullptr)
   {
      theValue = *mExtValue;
   }
   else
   {
      theValue = mValue;
   }

   mValidator.addImpliedChars(theValue);

   // Remove leading spaces from mValue, if pRemoveLeadingSpaces is true.
   if (pRemoveLeadingSpaces)
   {
      unsigned valueLen = theValue.length();
      if (valueLen > 0)
      {
         if (mValue[0] == ' ')
         {
            unsigned i = 0;
            while ((i < valueLen) && (theValue[i] == ' '))
            {
               ++i;
            }
            theValue.erase(0, i);
         }
      }
   }

   // Remove trailing spaces from mValue, if pRemoveTrailingSpaces is true.
   if (pRemoveTrailingSpaces)
   {
      unsigned valueLen = theValue.length();
      if (valueLen > 0)
      {
         if (theValue[valueLen-1] == ' ')
         {
            unsigned i = valueLen - 1;
            while (i > 0 && theValue[i] == ' ')
            {
               --i;
            }
            if (theValue[i] == ' ')
               theValue.clear(); // entire string is spaces
            else
               theValue.erase(i + 1);
         }
      }
   }

   return(theValue);
} // getValue

// Sets the validator string
void cxInput::setValidator(const string& pVString)
{
   mValidator.setValidatorStr(pVString);
}

// Returns the validator string.
const string& cxInput::getValidatorStr() const
{
   return(mValidator.getValidatorStr());
} // getValidator

int cxInput::getInputLen() const
{
   return(mInputLen);
} // getInputLen

bool cxInput::setValue(const string& pValue, bool pRefresh)
{
   bool success = true;

   if (pValue != getValue())
   {
      mValue = pValue;

      if (pRefresh)
      {
         show(false, false);
      }
   }

   return(success);
} // setValue

string* cxInput::getExtValue() const
{
   return(mExtValue);
}

// Setter for the "external" user value variable pointer.
//  pExtval: The new pointer to the external variable.
//  pRefresh: Whether or not to refresh the input.
void cxInput::setExtValue(string *pExtVal, bool pRefresh)
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
         setValue(*mExtValue);
      }
   }
} // setExtValue

bool cxInput::getExitOnFull() const
{
   return(mExitOnFull);
}

void cxInput::setExitOnFull(bool pExitOnFull)
{
   mExitOnFull = pExitOnFull;
}

bool cxInput::getExitOnBackspaceAtFront() const
{
   return(mExitOnBackspaceAtFront);
} // getExitOnBackspaceAtFront

void cxInput::setExitOnBackspaceAtFront(bool pExitOnBackspaceAtFront)
{
   mExitOnBackspaceAtFront = pExitOnBackspaceAtFront;
} // setExitOnBackspaceAtFront

bool cxInput::getMustFill() const
{
   return(mMustFill);
}

void cxInput::setMustFill(bool pMustFill)
{
   mMustFill = pMustFill;
} // setMustFill

int cxInput::getInputOption() const
{
   return(mInputOption);
} // getKind

void cxInput::setInputOption(eInputOptions pInputOption)
{
   // If pInputOption isn't eINPUT_READONLY, check to see
   //  that the input can be set editable before
   //  setting it.
   if (pInputOption != eINPUT_READONLY)
   {
      if (mCanBeEditable)
      {
         mInputOption = pInputOption;
      }
      else
      {
         mInputOption = eINPUT_READONLY;
      }
   }
   else
   {
      mInputOption = pInputOption;
   }
} // setInputOption

// Sets the horizontal (x) cursor position
void cxInput::setCursorX(int pCursorX)
{
   // If there is a border, the bounds
   //  are between mInputStartX and right()-1, and
   //  the row is 1.  If there is no
   //  border, the bounds are between mInputStartX and
   //  right(), and the row is 0.
   int rightmost = right();
   int row = 0;
   if (hasBorder())
   {
      --rightmost;
      row = 1;
   }

   if (pCursorX < mInputStartX)
   {
      wmove(mWindow, row, mInputStartX);
   }
   else if ((pCursorX >= mInputStartX) && (pCursorX < rightmost))
   {
      wmove(mWindow, row, pCursorX);
   }
} // setCursorX

bool cxInput::move(int pNewRow, int pNewCol, bool pRefresh)
{
   // False is passed for the refresh parameter to cxWindow::move()
   //  because mRightMax needs to be updated after the window
   //  is moved..  So the refresh is done later.
   bool moved = cxWindow::move(pNewRow, pNewCol, false);

   if (moved)
   {
      mRightMax = right();

      if (pRefresh)
      {
         show(false, false);
      }
   }

   return(moved);
} // move

void cxInput::resize(int pNewHeight, int pNewWidth, bool pRefresh)
{
   // Always use a height of 1 when re-sizing
   cxWindow::resize(1, pNewWidth, pRefresh);

   // Update the rightmost valid position & input length
   mRightMax = right();
   if (!hasBorder())
   {
      // There's no border, so the rightmost
      //  max. character position should be
      //  1 more.
      ++mRightMax;
   }
   mInputLen = width() - mInputStartX;
   // If there is a border, subtract 1 from mInputLen
   //  to account for the right border.
   if (hasBorder())
   {
      --mInputLen;
   }
} // resize

// Sets whether or not to exit upon function key press.
//  This was added for cxMultiLineInput so that it could
//  check function keys (so that checking function keys
//  wouldn't stop at this class).
void cxInput::setExitOnFKey(bool pExitOnFunctionKey)
{
   mExitOnFunctionKey = pExitOnFunctionKey;
}

bool cxInput::textIsValid() const
{
   return(mValidator.textIsValid(mValue));
} // textIsValid

bool cxInput::textIsValid(const string& pText) const
{
   return(mValidator.textIsValid(pText));
} // textIsValid

//// Private helper functions

void cxInput::getInputText(string& pValue, int pX, bool pCheckPrintable)
{
   bool getText = true;
   // See if we should check whether the last key pressed is a printable
   //  character
   if (pCheckPrintable)
   {
      getText = isPrintable(cxWindow::getLastKey());
   }
   if (getText)
   {
      pValue.erase();

      // mvwinchnstr() is used to extract the characters that
      //  the user entered from mWindow
      // If there is a border, the first character is at
      //  row 1, column mInputStartX.  If there is no border,
      //  the first character is at row 0 and colum mInputStartX.
      //chtype buffer[mInputLen+1];  // Holds the user input
      // When cross compiling, the compiler did not like the above
      //  declaration.  Using calloc instead:
      chtype *buffer = (chtype*)calloc(mInputLen+1,sizeof(chtype));
      if (buffer == nullptr) return;
      int numChars = 0;
      // Get the text in the window from mInputStartX up to the cursor
      //  position.  If there is a border, the input will be on row 1;
      //  without a border, the input will be on row 0.
      if (hasBorder())
      {
         numChars = mvwinchnstr(mWindow, 1, mInputStartX, buffer, pX - mInputStartX + 1);
      }
      else
      {
         numChars = mvwinchnstr(mWindow, 0, mInputStartX, buffer, pX - mInputStartX + 1);
      }
      // The ncurses data type 'chtype' contains both the
      //  character and attribute information.  In order to
      //  extract only the characters, we have to bitwise-AND
      //  each character with A_CHARTEXT before adding it.
      // If mvwinchnstr didn't return an error, we can go
      //  ahead and do this.
      if (numChars != ERR)
      {
         for (int i = 0; i < numChars; ++i)
         {
            pValue += (buffer[i] & A_CHARTEXT);
         }
      }

      // Free the memory allocated for the user input buffer
      free(buffer);
   }
} // getInputText

// Performs a 'backspace' behavior.  This was written as a facilitator for
//  showModal().
// Parameters:
//  y: The current vertical cursor position
//  x: The current horizontal cursor position
//  prevInput: The previous contents of the input
inline void cxInput::doBackspace(int y, int x, string& prevInput)
{
   if (x >= mInputStartX)
   {
      if (mValue.length() > 0)
      {
         mValue.erase(mValue.length()-1);
      }
      if (prevInput.length() > 0)
      {
         prevInput.erase(prevInput.length()-1);
      }

      // Blank out the last character in the input
      mvwaddch(mWindow, y, mInputStartX+(int)prevInput.length(), ' ');

      // Make sure the cursor is in the right place
      if (x > mInputStartX)
      {
         wmove(mWindow, y, x-1);
      }
      else
      {
         wmove(mWindow, y, mInputStartX);
      }
   }
} // doBackspace

void cxInput::setLabelColor(e_cxColors pColor)
{
   // Note: The window's message is the label.
   setMessageColor(pColor);
}

e_cxColors cxInput::getLabelColor() const
{
   // Note: The window's message is the label.
   return(getItemColor(eMESSAGE));
}

void cxInput::setValueColor(e_cxColors pColor)
{
   // Setting a color could require a bold attribute..   setElementColor
   //  will set the correct attribute for the color.
   attr_t tmpAttr = 0;
   setElementColor(mValueColorPair, tmpAttr, pColor);
   // Add the attribute to the editable & read-only attribute sets
   addAttr(eDATA_EDITABLE, tmpAttr);
   addAttr(eDATA_READONLY, tmpAttr);
} // setValueColor

e_cxColors cxInput::getValueColor() const
{
   e_cxColors color = eDEFAULT;

   if (mInputOption == eINPUT_EDITABLE)
   {
      color = getItemColor(eDATA_EDITABLE);
   }
   else if (mInputOption == eINPUT_READONLY)
   {
      color = getItemColor(eDATA_READONLY);
   }

   return(color);
} // getValueColor

void cxInput::setColor(e_WidgetItems pItem, e_cxColors pColor)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE) && (pItem != eDATA) && (pItem != eMESSAGE))
       {
      cxWindow::setColor(pItem, pColor);
   }
   else
   {
       switch(pItem)
       {
          case eLABEL:
             setLabelColor(pColor);
             break;
          case eDATA:
          case eDATA_READONLY:
          case eDATA_EDITABLE:
             setValueColor(pColor);
             break;
          // The next cases don't apply to cxInput, but they're here so that
          //  the compiler doesn't complain.
          case eMESSAGE:        // Message
          case eTITLE:          // Title
          case eSTATUS:         // Status
          case eBORDER:         // Borders
          case eHOTKEY:         // Hotkey
          case eMENU_SELECTION:
             break;
       }
   }
} // setColor

void cxInput::toggleCursor(bool pShowCursor)
{
   mShowCursor = pShowCursor;
}

string cxInput::clearInput(void* theInput, void* unused)
{
   if (theInput != nullptr)
   {
      cxInput *pInput = (cxInput*)theInput;
      pInput->setValue("");
      // Refresh the window
      pInput->show(false, false);
   }

   return("");
} // clearInput

bool cxInput::setValidatorFunction(funcPtr4 pFunction, void *p1, void *p2,
                                   void *p3, void *p4)
                                   {
   bool setIt = false;
   // Free the memory used by mValidatorFunction, and then update it with the
   //  given parameters.
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

   return(setIt);
} // setValidatorFunction

bool cxInput::setValidatorFunction(funcPtr2 pFunction, void *p1, void *p2)
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

   return(setIt);
} // setValidatorFunction

bool cxInput::setValidatorFunction(funcPtr0 pFunction)
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

   return(setIt);
} // setValidatorFunction

void cxInput::clearValidatorFunction()
{
   mValidatorFunction.reset();
} // clearValidatorFunction

bool cxInput::setOnKeyFunction(const shared_ptr<cxFunction>& pFunction)
{
   clearOnKeyFunction();
   mOnKeyFunction = pFunction;
   return true;
} // setOnKeyFunction

bool cxInput::setOnKeyFunction(funcPtr4 pFunction, void *p1, void *p2,
                               void *p3, void *p4)
                               {
   bool setIt = false;
   // Free the memory used by mOnKeyFunction, and then update it with the
   //  given parameters.
   clearOnKeyFunction();
   if (pFunction != nullptr)
   {
      mOnKeyFunction = make_shared<cxFunction4>(pFunction, p1, p2, p3, p4, false, false, true);
      setIt = (mOnKeyFunction != nullptr);
   }

   return(setIt);
} // setOnKeyFunction

bool cxInput::setOnKeyFunction(funcPtr2 pFunction, void *p1, void *p2)
{
   bool setIt = false;
   // Free the memory used by mOnKeyFunction, and then update it with the
   //  given parameters.
   clearOnKeyFunction();
   if (pFunction != nullptr)
   {
      mOnKeyFunction = make_shared<cxFunction2>(pFunction, p1, p2, false, false, true);
      setIt = (mOnKeyFunction != nullptr);
   }

   return(setIt);
} // setOnKeyFunction

bool cxInput::setOnKeyFunction(funcPtr0 pFunction)
{
   bool setIt = false;
   // Free the memory used by mOnKeyFunction, and then update it with the
   //  given parameters.
   clearOnKeyFunction();
   if (pFunction != nullptr)
   {
      mOnKeyFunction = make_shared<cxFunction0>(pFunction, false, false, true);
      setIt = (mOnKeyFunction != nullptr);
   }

   return(setIt);
} // setOnKeyFunction

void cxInput::toggleOnKeyFunction(bool pRunOnKeyFunction)
{
   mRunOnKeyFunction = pRunOnKeyFunction;
} // toggleOnKeyFunction

void cxInput::clearOnKeyFunction()
{
   mOnKeyFunction.reset();
} // clearOnKeyFunction

bool cxInput::onKeyFunctionEnabled() const
{
   return(mRunOnKeyFunction);
} // onKeyFunctionEnabled

bool cxInput::isFull() const
{
   return(((int)mValue.length() >= mInputLen) ||
          ((int)mValue.length() >= mMaxInputLength));
} // isFull

int cxInput::maxValueLen() const
{
   return(mInputLen);
} // maxValueLen

bool cxInput::hasFocus() const
{
   return(mHasFocus);
} // hasFocus

void cxInput::setReadOnlyOnLeave(bool pReadOnlyOnLeave)
{
   mReadOnlyOnLeave = pReadOnlyOnLeave;
} // setReadOnlyOnLeave

bool cxInput::canBeEditable() const
{
   return(mCanBeEditable);
} // canBeEditable

void cxInput::setCanBeEditable(bool pCanBeEditable)
{
   mCanBeEditable = pCanBeEditable;

   // If mCanbeEditable is now false, set the kind
   //  to eINPUT_READONLY.
   if (!mCanBeEditable)
   {
      setInputOption(eINPUT_READONLY);
   }
} // setCanBeEditable

void cxInput::setBorderStyle(eBorderStyle pBorderStyle)
{
   if (getBorderStyle() != pBorderStyle)
   {
      cxWindow::setBorderStyle(pBorderStyle);

      // Make sure the window is still wide enough to hold the
      //  label, input (keep mInputLen the same), and borders.
      int newWidth = width();
      if (hasBorder())
      {
         if (newWidth < (int)visualStrLen(getLabel()) + mInputLen + 2)
         {
            newWidth = (int)visualStrLen(getLabel()) + mInputLen + 2;
         }
         mYPos = 1;
         ++mInputStartX;
         resize(3, newWidth, false);
      }
      else
      {
         if (newWidth < (int)visualStrLen(getLabel()) + mInputLen)
         {
            newWidth = (int)visualStrLen(getLabel()) + mInputLen;
         }
         mYPos = 0;
         --mInputStartX;
         resize(1, newWidth, false);
      }
   }
} // setBorderStyle

void cxInput::refreshValue(bool pRefresh)
{
   // TODO: Grab the text out of the window and compare it to mValue.  If they
   //  differ, then go ahead and refresh the value in the window.

   // Enable the value attributes and the color.
   if (eINPUT_EDITABLE == mInputOption)
   {
      enableAttrs(mWindow, eDATA_EDITABLE);
   }
   else if (eINPUT_READONLY == mInputOption)
   {
      enableAttrs(mWindow, eDATA_READONLY);
   }
   if (useColors)
   {
      wcolor_set(mWindow, mValueColorPair, nullptr);
   }

   // Place the cursor after the label
   wmove(mWindow, mYPos, mInputStartX);
   // If mMasked is true, display asterisks rather
   //  than what the user is actually typing.
   // We use mvwprintw here to fill the entire space
   //  so that the value color is used throughout
   //  the entire space.
   if (mMasked)
   {
      // Create a string full of mask characters
      //  and display it.
      string maskedValue(mValue.length(), mMaskChar);
      std::ostringstream os;
      os << "%-" << mInputLen << "s";
      mvwprintw(mWindow, mYPos, mInputStartX, (char*)os.str().c_str(), maskedValue.c_str());
   }
   else
   {
      std::ostringstream os;
      os << "%-" << mInputLen << "s";
      mvwprintw(mWindow, mYPos, mInputStartX, (char*)os.str().c_str(), mValue.c_str());
   }

   // If focus is set, place the cursor one position past the input
   //  (so that when showModal() is called, it will be ready to
   //  accept input).  Otherwise, disable the main value attribute,
   //  extra value attributes, and value color, if focus is not set
   if (mHasFocus)
   {
      if (mCursorAfterInput)
      {
         wmove(mWindow, mYPos, mInputStartX+(int)mValue.length());
      }
      else
      {
         // Place the cursor after the label
         wmove(mWindow, mYPos, mInputStartX);
      }
   }
   else
   {
      // Disable the value attributes
      if (eINPUT_EDITABLE == mInputOption)
      {
         enableAttrs(mWindow, eDATA_EDITABLE);
      }
      else if (eINPUT_READONLY == mInputOption)
      {
         enableAttrs(mWindow, eDATA_READONLY);
      }
      if (useColors)
      {
         wcolor_set(mWindow, 0, nullptr);
      }
   }

   // If pRefresh is true, update this input's window
   if (pRefresh)
   {
      cxBase::updateWindows();
   }
} // refreshValue

void cxInput::trapNonAssignedFKeys(bool pTrapNonAssignedFKeys)
{
   mTrapNonAssignedFKeys = pTrapNonAssignedFKeys;
} // trapNonAssignedFKeys

void cxInput::enableInputLoop(bool pDoInputLoop)
{
   mDoInputLoop = pDoInputLoop;
} // enableInputLoop

bool cxInput::inputLoopEnabled() const
{
   return(mDoInputLoop);
} // inputLoopEnabled

bool cxInput::isEditable() const
{
   return((getInputOption() != eINPUT_READONLY) && isEnabled() &&
          inputLoopEnabled());
} // isEditable

bool cxInput::getCursorLeftAlign() const
{
   return(!mCursorAfterInput);
} // getCursorAfterInput

void cxInput::setCursorLeftAlign(bool pCursorLeftAlign)
{
   // mCursorAfterInput should be the opposite of what pCursorLeftAlign is.
   mCursorAfterInput = !pCursorLeftAlign;
} // setCursorAfterInput

void cxInput::setValidateOnReverse(bool pValidateOnReverse)
{
   mValidateOnReverse = pValidateOnReverse;
} // setValidateOnReverse

bool cxInput::getValidateOnReverse() const
{
   return(mValidateOnReverse);
} // getValidateOnReverse

void cxInput::setLastKey(int pLastKey)
{
   cxWindow::setLastKey(pLastKey);
   // If this input is in a cxMultiLineInput, its last key
   //  needs to be set too.
   if (mParentMLInput != nullptr)
   {
      mParentMLInput->setLastKey(pLastKey);
   }
} // setLastKey

bool cxInput::modalGetsKeypress() const
{
   return(isEnabled() && (getInputOption() == eINPUT_EDITABLE));
} // modalGetsKeypress

bool cxInput::runOnFocusFunction(string *pFunctionRetval)
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

cxWindow* cxInput::getParent() const
{
   cxWindow *parentWindow = nullptr;

   if (mParentMLInput != nullptr)
   {
      parentWindow = mParentMLInput;
   }
   else
   {
      // mParentMultiForm is nullptr
      parentWindow = cxWindow::getParent();
   }

   return(parentWindow);
} // getParent

void cxInput::addAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE) && (pItem != eMESSAGE))
       {
      cxWindow::addAttr(pItem, pAttr);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eLABEL:          // Labels
            attrSet = &mLabelAttrs;
            break;
         case eDATA:
         case eDATA_EDITABLE:  // Editable data
            attrSet = &mDataEditableAttrs;
            break;
         case eDATA_READONLY:  // Read-only data
            attrSet = &mDataReadonlyAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eMENU_SELECTION: // Menu selection
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Insert the attribute, if attrSet was set.
      if (nullptr != attrSet)
      {
         attrSet->insert(pAttr);
      }
   }
} // addAttr

void cxInput::setAttr(e_WidgetItems pItem, attr_t pAttr)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE) && (pItem != eMESSAGE))
       {
      cxWindow::setAttr(pItem, pAttr);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eLABEL:          // Labels
            attrSet = &mLabelAttrs;
            break;
         case eDATA:
         case eDATA_EDITABLE:  // Editable data
            attrSet = &mDataEditableAttrs;
            break;
         case eDATA_READONLY:  // Read-only data
            attrSet = &mDataReadonlyAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eMENU_SELECTION: // Menu selection
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Set the attribute, if attrSet was set.
      if (nullptr != attrSet)
      {
         attrSet->clear();
         attrSet->insert(pAttr);
      }
   }
} // setAttr

void cxInput::removeAttr(e_WidgetItems pItem, attr_t pAttr)
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
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eLABEL:          // Labels
            attrSet = &mLabelAttrs;
            break;
         case eDATA:
         case eDATA_EDITABLE:  // Editable data
            attrSet = &mDataEditableAttrs;
            break;
         case eDATA_READONLY:  // Read-only data
            attrSet = &mDataReadonlyAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eMENU_SELECTION: // Menu selection
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Remove the attribute, if attrSet was set.
      if (nullptr != attrSet)
      {
         attrSet->erase(pAttr);
      }
   }
} // removeAttr

void cxInput::removeAttrs(e_WidgetItems pItem)
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
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;
      switch(pItem)
      {
         case eLABEL:          // Labels
            attrSet = &mLabelAttrs;
            break;
         case eDATA:
         case eDATA_EDITABLE:  // Editable data
            attrSet = &mDataEditableAttrs;
            break;
         case eDATA_READONLY:  // Read-only data
            attrSet = &mDataReadonlyAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eMENU_SELECTION: // Menu selection
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Remove the attributes, if attrSet was set.
      if (nullptr != attrSet)
      {
         attrSet->clear();
      }
   }
} // removeAttrs

void cxInput::getAttrs(e_WidgetItems pItem, set<attr_t>& pAttrs) const
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
      switch(pItem)
      {
         case eLABEL:          // Labels
            pAttrs = mLabelAttrs;
            break;
         case eDATA:
         case eDATA_EDITABLE:  // Editable data
            pAttrs = mDataEditableAttrs;
            break;
         case eDATA_READONLY:  // Read-only data
            pAttrs = mDataReadonlyAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eMENU_SELECTION: // Menu selection
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }
   }
} // getAttrs

string cxInput::runValidatorFunction() const
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

void cxInput::setTimeout(int pTimeout)
{
   mTimeout = pTimeout;
} // setTimeout

int cxInput::getTimeout() const
{
   return(mTimeout);
} // getTimeout

string cxInput::cxTypeStr() const
{
   return("cxInput");
} // cxTypeStr

void cxInput::setForceUpper(bool pForceUpper)
{
   mForceUpper = pForceUpper;
} // forceUpper

bool cxInput::getForceUpper() const
{
   return(mForceUpper);
} // forceUpper

void cxInput::setMaxInputLength(int pLength)
{
   if (pLength >= 0)
   {
      mMaxInputLength = pLength;
   }
} // setMaxInputLength

int cxInput::getMaxInputLength() const
{
   return(mMaxInputLength);
} // getMaxInputLength

//// Protected functions

bool cxInput::handleFunctionForLastKey(bool *pFunctionExists,
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

   int lastKey = cxWindow::getLastKey();
   if (mKeyFunctions.find(lastKey) != mKeyFunctions.end())
   {
      shared_ptr<cxFunction> iFunc = mKeyFunctions[lastKey];
      // The cxFunction pointers in mKeyFunctions shouldn't be nullptr, but
      //  check just in case.
      if (iFunc != nullptr)
      {
         // If pFunctionExists is non-nullptr, set it to true, since
         //  a function was found for the last keypress.
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
            mValue = functionRetval;
            // Refresh the new value in this input.
            mvwaddnstr(mWindow, mYPos, mInputStartX, mValue.c_str(),
                       (int)mValue.length());
            // Make sure the cursor is at the end of the value.
            wmove(mWindow, mYPos, mInputStartX+(int)mValue.length()-1);
         }

         // If the 'mExitAfterRun' value this field function is true,
         //  then exit the input loop.
         if (iFunc->getExitAfterRun())
         {
            continueOn = false;
         }

         if (mExitOnFunctionKey)
         {
            continueOn = false;
         }
      }
   }

   return(continueOn);
} // handleFunctionForLastKey

void cxInput::enableAttrs(WINDOW *pWin, e_WidgetItems pItem)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE))
       {
      cxWindow::enableAttrs(pWin, pItem);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;

      switch(pItem)
      {
         case eLABEL:          // Labels
            attrSet = &mLabelAttrs;
            break;
         case eDATA:
         case eDATA_EDITABLE:  // Editable data
            attrSet = &mDataEditableAttrs;
            break;
         case eDATA_READONLY:  // Read-only data
            attrSet = &mDataReadonlyAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eMENU_SELECTION: // Menu selection
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Enable the attributes, if attrSet was set.
      if (nullptr != attrSet)
      {
         // If attrSet has at least 1 attribute, disable them; otherwise, disable
         //  the cxBase attributes.
         if (attrSet->size() > 0)
         {
            set<attr_t>::const_iterator iter = attrSet->begin();
            for (; iter != attrSet->end(); ++iter)
            {
               wattron(pWin, *iter);
            }
         }
         else
         {
            cxBase::enableAttrs(pWin, pItem);
         }
      }
      else
      {
         cxBase::enableAttrs(pWin, pItem);
      }
   }
} // enableAttrs

void cxInput::disableAttrs(WINDOW *pWin, e_WidgetItems pItem)
{
   // If pItem is not one of the ones this class handles, just have
   //  the parent class (cxWindow) handle it.
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE))
       {
      cxWindow::disableAttrs(pWin, pItem);
   }
   else
   {
      // attrSet is a pointer that will be set to point to the correct attribute
      //  set, depending on the value of pItem.
      set<attr_t>* attrSet = nullptr;

      switch(pItem)
      {
         case eLABEL:          // Labels
            attrSet = &mLabelAttrs;
            break;
         case eDATA:
         case eDATA_EDITABLE:  // Editable data
            attrSet = &mDataEditableAttrs;
            break;
         case eDATA_READONLY:  // Read-only data
            attrSet = &mDataReadonlyAttrs;
            break;
         // This class doesn't deal with these ones:
         case eHOTKEY:         // Hotkey
         case eMENU_SELECTION: // Menu selection
         case eMESSAGE:        // Message
         case eTITLE:          // Title
         case eSTATUS:         // Status
         case eBORDER:         // Borders
            break;
      }

      // Disable the attributes, if attrSet was set.
      if (nullptr != attrSet)
      {
         // If attrSet has at least 1 attribute, disable them; otherwise, disable
         //  the cxBase attributes.
         if (attrSet->size() > 0)
         {
            set<attr_t>::const_iterator iter = attrSet->begin();
            for (; iter != attrSet->end(); ++iter)
            {
               wattroff(pWin, *iter);
            }
         }
         else
         {
            cxBase::disableAttrs(pWin, pItem);
         }
      }
      else
      {
         cxBase::disableAttrs(pWin, pItem);
      }
   }
} // disableAttrs

e_cxColors cxInput::getItemColor(e_WidgetItems pItem) const
{
   // If pItem is not something specifically used by this class, return
   //  the value of cxWindow::getItemColor().
   if ((pItem != eLABEL) && (pItem != eDATA_READONLY) &&
       (pItem != eDATA_EDITABLE))
       {
      return(cxWindow::getItemColor(pItem));
   }

   // Figure out which member color pair & attribute set to check
   short colorPair = 0;
   set<attr_t> attrSet;
   switch(pItem)
   {
      case eLABEL:          // Input labels
         attrSet = mLabelAttrs;
         break;
      case eDATA:
      case eDATA_EDITABLE:  // Editable data items
         attrSet = mDataEditableAttrs;
         break;
      case eDATA_READONLY:  // Read-only data items
         attrSet = mDataReadonlyAttrs;
         break;
      // The following aren't handled by this class:
      case eMESSAGE:        // Message
      case eTITLE:          // Title
      case eSTATUS:         // Status
      case eBORDER:         // Borders
      case eHOTKEY:         // Hotkey
      case eMENU_SELECTION: // Selected menu items
         break;
   }

   // Now, determine the actual color that should be returned.
   e_cxColors color = (e_cxColors)colorPair; // This will be returned
   // If the attribute set contains A_BOLD, then check the color (if brown,
   //  then the actual color is yellow, etc.).
   if (attrSet.find(A_BOLD) != attrSet.end())
   {
      switch(colorPair)
      {
         case eBROWN_BLACK:
            color = eYELLOW_BLACK;
            break;
         case eBROWN_BLUE:
            color = eYELLOW_BLUE;
            break;
         case eGRAY_BLACK:
            color = eWHITE_BLACK;
            break;
         case eGRAY_BLUE:
            color = eWHITE_BLUE;
            break;
      }
   }

   return(color);
} // getItemColor

//// Private helper functions

inline void cxInput::enableValueAttrs()
{
   if (eINPUT_EDITABLE == mInputOption)
   {
      enableAttrs(mWindow, eDATA_EDITABLE);
   }
   else if (eINPUT_READONLY == mInputOption)
   {
      enableAttrs(mWindow, eDATA_READONLY);
   }
} // enableValueAttrs

inline void cxInput::disableValueAttrs()
{
   if (eINPUT_EDITABLE == mInputOption)
   {
      disableAttrs(mWindow, eDATA_EDITABLE);
   }
   else if (eINPUT_READONLY == mInputOption)
   {
      disableAttrs(mWindow, eDATA_READONLY);
   }
} // disableValueAttrs

long cxInput::doInputLoop(int x, int y, int rightLimit, bool updatePrevInput,
                          string& prevInput)
                          {
   setReturnCode(cxID_EXIT);

   // Enable the message attributes and turn on the
   //  value color
   enableValueAttrs();
   if (useColors)
   {
      wcolor_set(mWindow, mValueColorPair, nullptr);
   }

   // moveCursor keeps track of whether or not the cursor should be positioned
   //  after the text each iteration through the input loop.  Normally, the
   //  cursor is moved after the text is written to the window to ensure that
   //  the cursor is in the proper place, but sometimes we won't want this to
   //  happen (i.e., if the user presses the left or right arrow keys to go
   //  sideways).
   bool moveCursor = true;

   // Start the input loop
   mLeaveNow = false;
   bool continueOn = true;
   // writeValue specifies whether or not to write the text value to the input
   //  each time through the loop.
   bool writeValue = true;
   // highestX stores The highest horizontal position reached + 1 (to help
   //  in getting the final input text once the input loop is finished).
   int highestX = mInputStartX + (int)(mValue.length()) - 1;
   while((x <= rightLimit) && continueOn && (getInputOption() != eINPUT_READONLY && !getLeaveNow()))
   {
      // Make sure the cursor is enabled (sometimes it would disappear for
      //  some reason).
      curs_set(1);
      // Add implied characters to the input.  If masking is to be done,
      //  then add the implied characters to mValue and output masking
      //  characters in the window.  If no masking is to be done, output
      //  the value with no masking.
      if (mMasked)
      {
         mValidator.addImpliedChars(mValue);

         wmove(mWindow, y, mInputStartX);
         unsigned valueLen = mValue.length();
         for (unsigned i = 0; i < valueLen; ++i)
         {
            waddch(mWindow, mMaskChar);
         }
      }
      else
      {
         // Add the implied characters from the validator to prevInput,
         //  but only if updatePrevInput is true (it could have been
         //  set false in the last iteration if the user pressed a
         //  backspace).
         if (updatePrevInput)
         {
            mValidator.addImpliedChars(prevInput);
         }

         // Write prevInput to the window
         // Get the current cursor position (curY, curX), in case moveCursor
         //  is false.
         if (writeValue)
         {
            int curY = 0, curX = 0;
            getyx(mWindow, curY, curX);
            mvwaddstr(mWindow, y, mInputStartX, prevInput.c_str());
            // If moveCursor is false, move the cursor back to the
            //  start of the input value.
            if (!moveCursor)
            {
               wmove(mWindow, curY, curX);
            }
         }
      }

      // If focus just started and mCursorAfterInput is
      //  false, place the cursor after the label (rather
      //  than leave it after the input text).
      if (mJustStartedFocus && !mCursorAfterInput)
      {
         // Place the cursor after the label
         if (moveCursor)
         {
            wmove(mWindow, mYPos, mInputStartX);
         }
      }
      else
      {
         // If mValue has the same length as the validator string,
         //  place the cursor at the last character of the
         //  input in the window.  Otherwise, place the cursor
         //  after the last character of the text in the window.
         //  This may be redundant, since this is also done in show(),
         //  but it needs to always be done after the value is written
         //  to the window.
         if ((mValidator.getValidatorStr().length() > 0) &&
             (mValue.length() == mValidator.getValidatorStr().length()))
             {
            prevInput = mValue; // prevInput needs to be updated here
            if (moveCursor)
            {
               wmove(mWindow, mYPos, mInputStartX+(int)mValue.length()-1);
            }
         }
         else
         {
            if (moveCursor)
            {
               wmove(mWindow, mYPos, mInputStartX+(int)mValue.length());
            }
         }
      }

      // Update y and x (since something has been written to the window)
      getyx(mWindow, y, x);
      // If the horizontal position of the cursor is now beyond the
      //  right limit, then we may (or may not) need to exit the
      //  window:
      //  If the focus has just started, don't exit.
      //  Otherwise, if we're to exit when full, go ahead & exit.
      //     Otherwise, don't exit.
      if (hasBorder())
      {
         if (x > rightLimit)
         {
            if (mJustStartedFocus)
            {
               if (moveCursor)
               {
                  wmove(mWindow, mYPos, rightLimit);
                  getyx(mWindow, y, x);
               }
            }
            else
            {
               if (mExitOnFull)
               {
                  setReturnCode(cxID_EXIT);
                  break;
               }
               else
               {
                  if (moveCursor)
                  {
                     wmove(mWindow, mYPos, rightLimit);
                     getyx(mWindow, y, x);
                  }
               }
            }
         }
      }

      // updatePrevInput should now default to true unless the user
      //  presses a backspace.
      updatePrevInput = true;

      // Don't let the user move the cursor into the label
      if (x < mInputStartX)
      {
         wmove(mWindow, y, mInputStartX);
         getyx(mWindow, y, x);
      }

#ifdef WANT_TIMEOUT
      // Each time we get a keypress (using wgetch), we need to enable and
      //  disable the alarm so that every time the user presses a key, the
      //  idle timer is reset.
      alarm(mTimeout);
#endif
      int lastKey = wgetch(mWindow);
      // Upper-case the character if mForceUpper is true
      if (mForceUpper)
      {
         lastKey = toupper(lastKey);
      }
#ifdef WANT_TIMEOUT
      alarm(0);
#endif
      setLastKey(lastKey);
      // If mLeaveNow was set (i.e., from a call to exitNow() or quitNow(),
      //  then return now.
      if (getLeaveNow())
      {
         return(getReturnCode());
      }
#ifdef NCURSES_MOUSE_VERSION
      // For mouse button events, we want to do the following:
      //  - If there is an external function set up for the mouse state, then
      //    run it.  Otherwise, do the following:
      //  - If the user clicked inside the window, continue onto the next
      //    iteration of the input loop (to simulate nothing happening).
      //  - If the user clicked outside the window, then:
      //    - If the parent window is a cxPanel or cxMultiLineInput, quit
      //      out of the input loop (allowing the user to go to another
      //      window or input inside of a cxMultiLineInput).
      //      Otherwise, continue on (to simulate nothing happening).
      if (lastKey == KEY_MOUSE)
      {
         if (getmouse(&mMouse) == OK)
         {
            // Run a function that may exist for the mouse state.  If
            //  no function exists for the mouse state, then process
            //  it here.
            bool mouseFuncExists = false;
            continueOn = handleFunctionForLastMouseState(&mouseFuncExists);
            if (!mouseFuncExists)
            {
               if (mouseEvtWasButtonEvt())
               {
                  if (mouseEvtWasInWindow())
                  {
                     continue;
                  }
                  else
                  {
                     if (parentIsCxPanel() || (mParentMLInput != nullptr))
                     {
                        setReturnCode(cxID_QUIT);
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
         else
         {
            // getmouse failed..  just continue to the next iteration of the
            //  input loop.
            continue;
         }
      }
#endif
      // If the last key is in the quit keys, then quit and return
      //  cxID_QUIT.  If the key isn't there, look for it in
      //  the exit keys (if it's there, quit and return cxID_EXIT).
      //  If not there either, handle the key in a switch.
      if (hasQuitKey(lastKey))
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
      else if (lastKey == cxInput::inputClearKey)
      {
         // If this input is part of a multi-line input, then
         //  exit the input loop so that the multi-line input can
         //  catch the key and clear all of its inputs.  Otherwise,
         //  go ahead and clear the value of this input.
         if (mParentMLInput != nullptr)
         {
            continueOn = false;
         }
         else
         {
            prevInput = "";
            setValue("", true);
         }
      }
      else
      {
         // Perform some action based on the last key pressed
         switch(lastKey)
         {
            case ESCAPE: // Defined in cxKeyDefines.h
               setReturnCode(cxID_QUIT);
               continueOn = false;
               break;
            case ENTER:
            case KEY_ENTER:
            case KEY_DOWN:
            case KEY_UP:
               // Interpret the up key as quitting out
               //  of the input.  In some situations,
               //  there might be a validator function for
               //  the current input that depends on the
               //  value of another input - an input won't
               //  let the cursor leave if the value is
               //  invalid, but if this returns cxID_QUIT,
               //  you'd still be able to go back to the
               //  previous input in a cxForm.
               // For enter or down arrow, interpret that
               //  as a successful exit.
               if (lastKey)
               {
                  setReturnCode(cxID_QUIT);
               }
               else
               {
                  setReturnCode(cxID_EXIT);
               }
               continueOn = false;
               break;
            case TAB:
            case SHIFT_TAB:
               setReturnCode(cxID_EXIT);
               continueOn = false;
               break;
            case KEY_HOME:
            case KEY_SEND:  // sEND=HOME
               // Move the cursor to the start of the text
               wmove(mWindow, y, mInputStartX);
               // Don't place the cursor after the text next time
               //  through the input loop
               moveCursor = false;
               break;
            case KEY_END:
            case KEY_END2:
            case KEY_END3:
            case KEY_SHOME: // sHOME=END
               // This used to move to the end of the input:
               //wmove(mWindow, y, mInputStartX+mInputLen-1);
               // But if the input is not full, that might not make sense..
               //  So we should move the cursor to the highest horizontal
               //  position we've seen so far (but add 1 because highestX
               //  is the highest horizontal positon - 1):
               wmove(mWindow, y, highestX+1);

               // Don't place the cursor after the text next time
               //  through the input loop
               moveCursor = false;
               break;
            case KEY_BACKSPACE:
            case BACKSPACE: // Defined in cxKeyDefines.h
            case KEY_DC:    // Delete character
            case '\b':      // Backspace character
               // If the cursor was in the first position
               if (x == mInputStartX)
               {
                  // The cursor was in the first input position

                  // If the value currently has only 1 character, then remove
                  //  it (that's probably what the user intended).  Check
                  //  prevInput rather than mValue because it's more up to date.
                  if (prevInput.length() == 1)
                  {
                     prevInput = "";
                     mValue = "";
                     // Don't update prevInput later, because we just did now.
                     updatePrevInput = false;
                     // Because we have 1 less character, decrement highestX if
                     //  it's still greater than the starting X position-1.
                     if (highestX > mInputStartX-1)
                     {
                        --highestX;
                     }
                     // Write a space to clear out the input
                     mvwaddch(mWindow, y, x, ' ');
                     // Make sure the cursor is placed where it should be.
                     wmove(mWindow, y, x);
                     // Don't write the input value next time through the loop
                     writeValue = false;
                  }

                  // If mExitOnBackspaceAtFront is true, then don't continue
                  //  the input loop.
                  if (mExitOnBackspaceAtFront)
                  {
                     continueOn = false;
                  }
               }
               else if (x >= highestX)
               {
                  // The cursor was at the end of the value.
                  doBackspace(y, x, prevInput);
                  // Decrement highestX if it's still greater than the
                  //  starting X position-1
                  if (highestX > mInputStartX-1)
                  {
                     --highestX;
                  }
                  // Don't update prevInput
                  updatePrevInput = false;

                  wmove(mWindow, y, x-1);
                  moveCursor = false;
               }
               else
               {
                  // The cursor was somewhere in the middle.  Delete the
                  //  character at the cursor position from the value.
                  int pos = x - mInputStartX - 1;
                  prevInput = cxStringUtils::removeChar(prevInput, pos);
                  // Don't update prevInput later, because we just did now.
                  updatePrevInput = false;

                  // Because we have 1 less character, decrement highestX if
                  //  it's still greater than the starting X position-1.
                  if (highestX > mInputStartX-1)
                  {
                     --highestX;
                  }

                  // Write the new value to the window
                  // Pad the value with spaces on the right so that it will
                  //  write over anything that may have been there before.
                  string inputPadded = prevInput;
                  int numSpaces = mInputLen - (int)(prevInput.length());
                  inputPadded.append(numSpaces, ' ');
                  mvwaddnstr(mWindow, y, mInputStartX, inputPadded.c_str(), mInputLen);
                  // Don't write the input value next time through the loop
                  writeValue = false;
                  // Move the cursor where it should be
                  wmove(mWindow, y, x-1);
                  // Don't move the cursor next time, because we just did.
                  moveCursor = false;
               }
               break;
            // For left & right keys, only allow the
            //  user to move left/right if masking is
            //  not enabled (so we don't have to worry
            //  about characters changed in the middle).
            case KEY_LEFT:
               // If the user is using a Wyse50 terminal, then
               //  do a backspace.  Otherwise, let the user move
               //  the cursor over without deleting.
               if (cxBase::termType == "wy50")
               {
                  doBackspace(y, x, prevInput);
                  // Don't update prevInput
                  updatePrevInput = false;
               }
               else
               {
                  if (!mMasked)
                  {
                     wmove(mWindow, y, x-1);
                  }
               }
               // Don't move the cursor next time around
               moveCursor = false;
               break;
            case KEY_RIGHT:
               // Allow the user to move right, if the input text is not
               //  masked, and if the cursor is not beyond the text in the
               //  input.
               // Note: x - mInputStartX is the position of the cursor where
               //  a horizontal value of 0 is the position after the label.
               if (!mMasked && ((x - mInputStartX) < (int)(cxInput::getValue().length())))
               {
                  wmove(mWindow, y, x+1);
               }
               // Don't move the cursor next time around
               moveCursor = false;
               break;
            case ERR:   // Error getting a key
               break;
            // Process any other key
            default:
               // Update highestX if appropriate.
               if (isPrintable(lastKey) && (x > highestX))
               {
                  highestX = x;
               }

               {
                  // Process any functions that may be tied to
                  //  the last keypress.
                  bool functionExists = false;
                  continueOn = handleFunctionForLastKey(&functionExists);
                  // If mLeaveNow was set true, then leave the input loop.
                  continueOn = (continueOn && !mLeaveNow);

                  // If no function exists for the current key, then treat it
                  //  as another keypress from the user.
                  if (!functionExists)
                  {
                     if (isPrintable(lastKey))
                     {
                        // Deal with input masking
                        if (mMasked)
                        {
                           mValue += char(lastKey);
                           waddch(mWindow, mMaskChar);
                        }
                        else
                        {
                           // Add the key to the window and input if the user
                           //  typed a printable character.
                           waddch(mWindow, char(lastKey));
                           // Save the current mValue, in case the user moved
                           //  the cursor home, etc. and mValue has characters
                           //  past the current horizontal position
                           //string valueBackup = mValue;
                           getInputText(mValue, x, true);
                           wmove(mWindow, y, x); // because getInput moves the cursor.
                        }
                     }
                     // If the input is not valid, then deal with it.
                     // Look for the last capital letter in the validator string..
                     //  If there is one, then only validate the string if it
                     //  has enough characters.
                     int lastCapIndex = indexOfLastCap(mValidator.getValidatorStr());
                     bool canValidate = true;
                     if (lastCapIndex > -1)
                     {
                        canValidate = (mValue.length() >= (unsigned)lastCapIndex + 1);
                     }
                     if (canValidate)
                     {
                        if (mValidator.textIsValid(mValue))
                        {
                           // If the length of the input matches
                           //  the length of the validator string,
                           //  place the cursor at the last
                           //  character in the window.
                           if ((unsigned)(x - mInputStartX + 1) ==
                               mValidator.getValidatorStr().length())
                               {
                              wmove(mWindow, y, x-1);
                           }
                        }
                        else
                        {
                           // The text is not valid at this point, so we have
                           //  to deal with it.
                           // Remove the invalid character from mValue and prevInput.
                           if (mValue.length() > 0)
                           {
                              mValue.erase(mValue.length()-1);
                           }
                           if (!mMasked)
                           {
                              // No masking - Remove the invalid character from
                              //  prevInput
                              if (prevInput.length() > 0)
                              {
                                 // Updated 2/26
                                 //prevInput.erase(prevInput.length()-1);
                              }
                           }

                           // Since the current character is invalid, erase it.
                           wmove(mWindow, y, x);
                           // Replace the invalid character with the previous
                           //  character at this position, or a space if there
                           //  was no previous character at this location.  Also,
                           //  this only makes sense if masking is not being done.
                           if (((int)prevInput.length() > x - mInputStartX) && (!mMasked))
                           {
                              waddch(mWindow, prevInput[x - mInputStartX]);
                           }
                           else
                           {
                              waddch(mWindow, ' ');
                           }
                           // Move the cursor back 1 position to the left,
                           //  but only if the cursor position is not at the
                           //  rightmost edge (this is an edge case that
                           //  seemed to be erasing the last character if it
                           //  was valid).
                           if (x < (mRightMax - left()))
                           {
                              wmove(mWindow, y, x-1);
                           }

                           updatePrevInput = false;
                        }
                     }

                     // Make sure the cursor is still enabled
                     curs_set(1);
                  }

                  // Check the parent multi-line input to see if what the user
                  //  has typed is in its list of valid values, and if so, try
                  //  to fill in the rest.
                  if (mParentMLInput != nullptr)
                  {
                     mParentMLInput->autoFillFromValidOptions(true);
                     // If this input is full, then stop.
                     if (isFull())
                     {
                        continueOn = false;
                     }
                     else
                     {
                        // x needs to be updated to reflect where the cursor
                        //  is.
                        x = mInputStartX + (int)(mValue.length()) - 1;
                        // Also update highestX if we need to.
                        if (x > highestX)
                        {
                           highestX = x;
                        }
                        updatePrevInput = true;
                     }
                  }
               }
               break;
         }

         // Run the onKey function
         // In case the onKey function will remove characters from the
         //  input value, keep track of whether the value was shortened,
         //  in which case the focus should stay on the input.
         bool valueShortened = false;
         if (mRunOnKeyFunction)
         {
            // Keep track of the input value before & after the onKey function
            //  runs so that we can tell if it was shortened.
            string oldValue = getValue();
            runOnKeyFunction();
            string newValue = getValue();
            if (newValue.length() < oldValue.length())
            {
               valueShortened = true;
            }
         }

         // Update the current vertical & horizontal position in the window
         getyx(mWindow, y, x);

         // June 11, 2007: The following doesn't seem necessary.  It was
         //  causing a bug where if you type some stuff, back up with the left
         //  arrow, and then type a character, it would not keep the text after
         //  the character:
         /*
         // If the last keypress was not a printable character, then set
         //  highestX to the end of the text in the input - This is needed
         //  so that when getInputText() is called with highestText(), the
         //  value it gets is correct.  There are situations when the cursor
         //  might not be at the last value of the text in the input, in which
         //  case getInputText() wouldn't get all of the text.
         if (!isPrintable(getLastKey())) {
            highestX = (int)(mValue.length()) + mInputStartX - 1;
         }
         */

         // Update prevInput for this iteration (if no masking)
         if (!mMasked)
         {
            if (updatePrevInput)
            {
               getInputText(prevInput, highestX, true);
               wmove(mWindow, y, x); // because getInputText() moves the cursor.
            }
         }

         mJustStartedFocus = false;

         // Decide whether focus should leave this input.  Note: for a
         //  borderless input window, we can never tell if the cursor went
         //  beyond the limit because it never will..  So if the cursor
         //  reaches the limit at the end of this loop, then exit if
         //  mExitOnFull is true, and if the input was not shortened in the
         //  process of the user typing (i.e., by an onKey function).
         if (!hasBorder())
         {
            if ((x == rightLimit) && mExitOnFull && !valueShortened)
            {
               setReturnCode(cxID_EXIT);
               break;
            }
         }

         moveCursor = isPrintable(lastKey);
         /*
         // If the user didn't want to go sideways or home/end, or press
         //  backspace, then let the cursor be moved next time around.
         if ((lastKey != KEY_LEFT) && (lastKey != KEY_RIGHT) &&
             (lastKey != KEY_HOME) && (lastKey != KEY_END) &&
             (lastKey != KEY_SHOME) && (lastKey != KEY_SEND) &&
             (lastKey != KEY_END2) && (lastKey != KEY_BACKSPACE) &&
             (lastKey != BACKSPACE) && (lastKey != KEY_DC) &&
             (lastKey != '\b')) {
            moveCursor = true;
         }
         */
      }
   }
   // If masking is not being used, make sure mValue is updated by grabbing
   //  the text from the input between mInputStartX and highestX.
   if (!mMasked)
   {
      getInputText(mValue, highestX, false);
   }

   // Turn off the value color and disable the message attributes
   if (useColors)
   {
      wcolor_set(mWindow, 0, nullptr);
   }
   disableValueAttrs();

   // mLeaveNow should be false
   mLeaveNow = false;

   return(getReturnCode());
} // doInputLoop

// This is private so that it can't be called from the
//  outside.
void cxInput::setHotkeyHighlighting(bool pMessageUnderlines)
{
   cxWindow::setHotkeyHighlighting(pMessageUnderlines);
} // setHotkeyHighlighting

void cxInput::draw()
{
   // Make sure the data attributes are disabled before writing the
   //  label
   disableAttrs(mWindow, eDATA_EDITABLE);
   disableAttrs(mWindow, eDATA_READONLY);
   // Enable the label attributes
   enableAttrs(mWindow, eLABEL);
   // Draw in the window, using the parent's draw() method
   cxWindow::draw();
   // Disable the label attributes and enable either the editable or
   //  the read-only attributes, depending on whether this input is
   //  editable or read-only.
   disableAttrs(mWindow, eLABEL);
   if (eINPUT_EDITABLE == mInputOption)
   {
      enableAttrs(mWindow, eDATA_EDITABLE);
   }
   else if (eINPUT_READONLY == mInputOption)
   {
      enableAttrs(mWindow, eDATA_READONLY);
   }
   // Draw the value portion of the input, but don't refresh it on the
   //  screen.
   refreshValue(false);
   // If the input is not being shown modally, disable the data attributes
   //  here (otherwise, they will be disabled in showModal()).
   if (!mIsModal)
   {
      if (eINPUT_EDITABLE == mInputOption)
      {
         disableAttrs(mWindow, eDATA_EDITABLE);
      }
      else if (eINPUT_READONLY == mInputOption)
      {
         disableAttrs(mWindow, eDATA_READONLY);
      }
   }
} // draw

#ifdef WANT_TIMEOUT
void cxInput::idleTimeoutHandler(int pSignal)
{
   // A window that has focus should be the topmost window on the stack.
   cxWindow *topWindow = cxBase::getTopWindow();
   if (topWindow != nullptr)
   {
      try
      {
         cxInput *iInput = dynamic_cast<cxInput*>(topWindow);
         if (iInput != nullptr)
         {
            // If the input's parent multi-line input is not nullptr, call exitNow()
            //  on it.  Otherwise, call exitNow() on the single-line input.
            if (iInput->mParentMLInput != nullptr)
            {
               iInput->mParentMLInput->quitNow();
            }
            else
            {
               topWindow->quitNow();
            }
         }
      }
      catch (const std::bad_cast& e)
      {
         // The dynamic_cast failed
      }
   }
} // idleTimeoutHandler
#endif

string cxInput::runOnKeyFunction() const
{
   string retval;

   if (mOnKeyFunction != nullptr)
   {
      retval = mOnKeyFunction->runFunction();
   }

   return(retval);
} // runOnKeyFunction

void cxInput::copyCxFunction(const cxInput& pThatInput, /*OUT*/std::shared_ptr<cxFunction>& pDestFunc,
                             /*IN*/const std::shared_ptr<cxFunction>& pSrcFunc)
                             {
   // This function is for copying cxInput objects and is intended for use with
   //  mOnKeyFunction and mValidatorFunction.
   // If pDestFunc is not nullptr, then free the memory used by it.
   if (pDestFunc != nullptr)
   {
      pDestFunc.reset();
   }

   // Copy the other cxFunction pointer.
   if (pSrcFunc.get() != nullptr)
   {
      // We'll need to check what type of cxFunction it is so we can
      // create it properly.
      if (pSrcFunc->cxTypeStr() == "cxFunction0")
      {
         const cxFunction0 *iFunc0 = dynamic_cast<cxFunction0*>(pSrcFunc.get());
         if (iFunc0 != nullptr)
         {
            pDestFunc = make_shared<cxFunction0>(iFunc0->getFunction(),
                                        iFunc0->getUseReturnVal(),
                                        iFunc0->getExitAfterRun(),
                                        iFunc0->getRunOnLeaveFunction());
         }
      }
      else if (pSrcFunc->cxTypeStr() == "cxFunction2")
      {
         const cxFunction2 *iFunc2 = dynamic_cast<cxFunction2*>(pSrcFunc.get());
         if (iFunc2 != nullptr)
         {
            // When creating mOnKeyFunction, if any of its parameters point to
            // pThatInput, have them point to this one instead.  Or if they
            // point to the other input's parent multi-line input, have them
            // point to this one's parent multi-line input instead.
            void* params[2] = { iFunc2->getParam1(), iFunc2->getParam2() };
            for (int i = 0; i < 2; ++i)
            {
               if (params[i] == &pThatInput)
               {
                  params[i] = this;
               }
               else
               {
                  if (pThatInput.mParentMLInput != nullptr)
                  {
                     if (params[i] == pThatInput.mParentMLInput)
                     {
                        params[i] = mParentMLInput;
                     }
                  }
               }
            }
            pDestFunc = make_shared<cxFunction2>(iFunc2->getFunction(), params[0],
                                        params[1], iFunc2->getUseReturnVal(),
                                        iFunc2->getExitAfterRun(),
                                        iFunc2->getRunOnLeaveFunction());
         }
      }
      else if (pSrcFunc->cxTypeStr() == "cxFunction4")
      {
         const cxFunction4 *iFunc4 = dynamic_cast<cxFunction4*>(pSrcFunc.get());
         if (iFunc4 != nullptr)
         {
            // When creating mOnKeyFunction, if any of its parameters point to
            // pThatInput, have them point to this one instead.  Or if they
            // point to the other input's parent multi-line input, have them
            // point to this one's parent multi-line input instead.
            void* params[4] = { iFunc4->getParam1(), iFunc4->getParam2(),
                                iFunc4->getParam3(), iFunc4->getParam4()  };
            for (int i = 0; i < 4; ++i)
            {
               if (params[i] == &pThatInput)
               {
                  params[i] = this;
               }
               else
               {
                  if (pThatInput.mParentMLInput != nullptr)
                  {
                     if (params[i] == pThatInput.mParentMLInput)
                     {
                        params[i] = mParentMLInput;
                     }
                  }
               }
            }
            pDestFunc = make_shared<cxFunction4>(iFunc4->getFunction(), params[0],
                                        params[1], params[2], params[3],
                                        iFunc4->getUseReturnVal(),
                                        iFunc4->getExitAfterRun(),
                                        iFunc4->getRunOnLeaveFunction());
         }
      }
   }
} // copyCxFunction

bool cxInput::validatorFunctionIsSet() const
{
   bool isSet = false;

   if (mValidatorFunction != nullptr)
   {
      isSet = (mValidatorFunction->functionIsSet());
   }

   return(isSet);
} // validatorFunctionIsSet
