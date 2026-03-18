// Copyright (c) 2026 E. Oulashin
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

   // Set mMaxInputLength to 0 (unlimited) by default, allowing text to
   // scroll horizontally when it exceeds the visible input width.
   mMaxInputLength = 0;
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
     mExitOnMouseOutside(pThatInput.mExitOnMouseOutside),
     mExitOnArrowAtBoundary(pThatInput.mExitOnArrowAtBoundary),
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
     mMaxInputLength(pThatInput.mMaxInputLength),
     mScrollOffset(pThatInput.mScrollOffset),
     mCursorPos(pThatInput.mCursorPos)
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
     mExitOnMouseOutside(pThatInput.mExitOnMouseOutside),
     mExitOnArrowAtBoundary(pThatInput.mExitOnArrowAtBoundary),
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
     mMaxInputLength(pThatInput.mMaxInputLength),
     mScrollOffset(pThatInput.mScrollOffset),
     mCursorPos(pThatInput.mCursorPos)
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
         //  screen, inclusive) for the user input display area.
         // With scrolling support, this is always based on the display width.
         int rightLimit = mInputStartX + mInputLen - 1;
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
   mCursorPos = 0;
   mScrollOffset = 0;
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
      // Update cursor position and scroll offset for the new value
      mCursorPos = (int)mValue.length();
      ensureCursorVisible();

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

bool cxInput::getExitOnMouseOutside() const
{
   return mExitOnMouseOutside;
} // getExitOnMouseOutside

void cxInput::setExitOnMouseOutside(bool pExitOnMouseOutside)
{
   mExitOnMouseOutside = pExitOnMouseOutside;
} // setExitOnMouseOutside

bool cxInput::getExitOnArrowAtBoundary() const
{
   return mExitOnArrowAtBoundary;
} // getExitOnArrowAtBoundary

void cxInput::setExitOnArrowAtBoundary(bool pExitOnArrowAtBoundary)
{
   mExitOnArrowAtBoundary = pExitOnArrowAtBoundary;
} // setExitOnArrowAtBoundary

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
   // pCursorX is the logical position within the value (relative to label start).
   // Update mCursorPos and scroll offset, then position the ncurses cursor.
   int row = 0;
   if (hasBorder())
   {
      row = 1;
   }

   // Treat pCursorX as a position relative to the start of the input area
   int logicalPos = pCursorX;
   if (logicalPos < 0)
      logicalPos = 0;
   if (logicalPos > (int)mValue.length())
      logicalPos = (int)mValue.length();

   mCursorPos = logicalPos;
   ensureCursorVisible();

   int screenX = mInputStartX + (mCursorPos - mScrollOffset);
   wmove(mWindow, row, screenX);
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

void cxInput::redrawVisibleValue()
{
   // Show the visible portion of mValue starting at mScrollOffset
   if (mMasked)
   {
      int totalLen = (int)mValue.length();
      int visibleStart = mScrollOffset;
      int visibleLen = totalLen - visibleStart;
      if (visibleLen > mInputLen)
         visibleLen = mInputLen;
      if (visibleLen < 0)
         visibleLen = 0;
      string maskedVisible(visibleLen, mMaskChar);
      int padLen = mInputLen - visibleLen;
      if (padLen > 0)
         maskedVisible.append(padLen, ' ');
      mvwaddnstr(mWindow, mYPos, mInputStartX, maskedVisible.c_str(), mInputLen);
   }
   else
   {
      string visibleText;
      if (mScrollOffset < (int)mValue.length())
         visibleText = mValue.substr(mScrollOffset, mInputLen);
      int padLen = mInputLen - (int)visibleText.length();
      if (padLen > 0)
         visibleText.append(padLen, ' ');
      mvwaddnstr(mWindow, mYPos, mInputStartX, visibleText.c_str(), mInputLen);
   }
}

void cxInput::ensureCursorVisible()
{
   // Ensure mCursorPos is within [mScrollOffset, mScrollOffset + mInputLen)
   if (mCursorPos < mScrollOffset)
      mScrollOffset = mCursorPos;
   if (mCursorPos >= mScrollOffset + mInputLen)
      mScrollOffset = mCursorPos - mInputLen + 1;
   // Scroll back to keep the display as full as possible: if the text
   // extends before the visible window but the visible portion isn't
   // filling the display width, reduce the scroll offset so more text
   // is shown (e.g., when backspacing erases characters at the end).
   int textLen = (int)mValue.length();
   if (mScrollOffset > 0 && textLen - mScrollOffset < mInputLen)
   {
      mScrollOffset = textLen - mInputLen;
   }
   if (mScrollOffset < 0)
      mScrollOffset = 0;
}

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
   // If mMaxInputLength is 0, the input supports unlimited text with
   // horizontal scrolling, so it is never "full".
   if (mMaxInputLength == 0)
      return false;
   return ((int)mValue.length() >= mMaxInputLength);
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
   // Display the visible portion of the value, accounting for scroll offset
   if (mMasked)
   {
      // Create a string of mask characters for the visible portion
      int totalMaskLen = (int)mValue.length();
      int visibleStart = mScrollOffset;
      int visibleLen = totalMaskLen - visibleStart;
      if (visibleLen > mInputLen)
         visibleLen = mInputLen;
      if (visibleLen < 0)
         visibleLen = 0;
      string maskedVisible(visibleLen, mMaskChar);
      // Pad with spaces to fill the input area
      int padLen = mInputLen - visibleLen;
      if (padLen > 0)
         maskedVisible.append(padLen, ' ');
      mvwaddnstr(mWindow, mYPos, mInputStartX, maskedVisible.c_str(), mInputLen);
   }
   else
   {
      // Show mValue starting from mScrollOffset, up to mInputLen chars
      string visibleText;
      if (mScrollOffset < (int)mValue.length())
         visibleText = mValue.substr(mScrollOffset, mInputLen);
      // Pad with spaces to fill the input area
      int padLen = mInputLen - (int)visibleText.length();
      if (padLen > 0)
         visibleText.append(padLen, ' ');
      mvwaddnstr(mWindow, mYPos, mInputStartX, visibleText.c_str(), mInputLen);
   }

   // If focus is set, place the cursor at the correct position
   if (mHasFocus)
   {
      if (mCursorAfterInput)
      {
         // Place cursor at the logical cursor position (relative to scroll)
         int screenCursorX = mInputStartX + (mCursorPos - mScrollOffset);
         if (screenCursorX > mInputStartX + mInputLen)
            screenCursorX = mInputStartX + mInputLen;
         wmove(mWindow, mYPos, screenCursorX);
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

#ifdef WANT_TIMEOUT
void cxInput::setTimeout(int pTimeout)
{
   mTimeout = pTimeout;
} // setTimeout

int cxInput::getTimeout() const
{
   return(mTimeout);
} // getTimeout
#endif

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

   // Enable the message attributes and turn on the value color
   enableValueAttrs();
   if (useColors)
   {
      wcolor_set(mWindow, mValueColorPair, nullptr);
   }

   // Only initialize cursor position when focus first starts.
   // When re-entering the input loop (e.g., from a grid re-showing
   // the same cell), preserve the existing cursor position.
   if (mJustStartedFocus)
   {
      if (mCursorAfterInput)
      {
         mCursorPos = (int)mValue.length();
      }
      else
      {
         mCursorPos = 0;
      }
   }
   // Clamp cursor position to valid range in case value changed externally
   if (mCursorPos > (int)mValue.length())
      mCursorPos = (int)mValue.length();
   ensureCursorVisible();

   // Start the input loop
   mLeaveNow = false;
   bool continueOn = true;
   while (continueOn && (getInputOption() != eINPUT_READONLY) && !getLeaveNow())
   {
      // Make sure the cursor is enabled
      curs_set(1);

      // Add implied characters from the validator
      if (mMasked)
      {
         mValidator.addImpliedChars(mValue);
      }
      else
      {
         mValidator.addImpliedChars(mValue);
      }

      // Redraw the visible portion of mValue and position the cursor
      ensureCursorVisible();
      redrawVisibleValue();

      // If focus just started and mCursorAfterInput is false, place cursor
      // at the start of the input area.
      if (mJustStartedFocus && !mCursorAfterInput)
      {
         mCursorPos = 0;
         mScrollOffset = 0;
      }

      // Place the screen cursor at the correct position
      int screenCursorX = mInputStartX + (mCursorPos - mScrollOffset);
      wmove(mWindow, mYPos, screenCursorX);

      // Handle special case: if validator string length == mValue length,
      // place cursor at last character rather than after it
      if ((mValidator.getValidatorStr().length() > 0) &&
          (mValue.length() == mValidator.getValidatorStr().length()) &&
          !mJustStartedFocus)
      {
         mCursorPos = (int)mValue.length() - 1;
         if (mCursorPos < 0) mCursorPos = 0;
         ensureCursorVisible();
         screenCursorX = mInputStartX + (mCursorPos - mScrollOffset);
         wmove(mWindow, mYPos, screenCursorX);
      }

      // Check if the input is full and should exit
      if (!mJustStartedFocus && isFull() && mExitOnFull)
      {
         setReturnCode(cxID_EXIT);
         break;
      }

#ifdef WANT_TIMEOUT
      alarm(mTimeout);
#endif
      int lastKey = wgetch(mWindow);
      if (mForceUpper)
      {
         lastKey = toupper(lastKey);
      }
#ifdef WANT_TIMEOUT
      alarm(0);
#endif
      setLastKey(lastKey);
      if (getLeaveNow())
      {
         return(getReturnCode());
      }

#ifdef NCURSES_MOUSE_VERSION
      if (lastKey == KEY_MOUSE)
      {
         if (getmouse(&mMouse) == OK)
         {
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
                     if (parentIsCxPanel() || (mParentMLInput != nullptr) ||
                         mExitOnMouseOutside)
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
            continue;
         }
      }
#endif

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
      else if (lastKey == cxInput::inputClearKey)
      {
         if (mParentMLInput != nullptr)
         {
            continueOn = false;
         }
         else
         {
            mValue = "";
            mCursorPos = 0;
            mScrollOffset = 0;
         }
      }
      else
      {
         switch (lastKey)
         {
            case ESCAPE:
               setReturnCode(cxID_QUIT);
               continueOn = false;
               break;
            case ENTER:
            case KEY_ENTER:
            case KEY_DOWN:
            case KEY_UP:
               if (lastKey == KEY_UP)
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
            case KEY_SEND:
               mCursorPos = 0;
               ensureCursorVisible();
               break;
            case KEY_END:
            case KEY_END2:
            case KEY_END3:
            case KEY_SHOME:
               mCursorPos = (int)mValue.length();
               ensureCursorVisible();
               break;
            case KEY_BACKSPACE:
            case BACKSPACE:
            case '\b':
               if (mCursorPos > 0)
               {
                  // Delete the character before the cursor
                  mValue.erase(mCursorPos - 1, 1);
                  --mCursorPos;
                  ensureCursorVisible();
               }
               else
               {
                  // Cursor at position 0
                  if (mExitOnBackspaceAtFront)
                  {
                     continueOn = false;
                  }
               }
               break;
            case KEY_DC:
               // Delete character at cursor position (forward delete)
               if (mCursorPos < (int)mValue.length())
               {
                  mValue.erase(mCursorPos, 1);
                  ensureCursorVisible();
               }
               break;
            case KEY_LEFT:
               if (cxBase::termType == "wy50")
               {
                  // Wyse50 terminal: backspace behavior
                  if (mCursorPos > 0)
                  {
                     mValue.erase(mCursorPos - 1, 1);
                     --mCursorPos;
                     ensureCursorVisible();
                  }
               }
               else if (mCursorPos > 0)
               {
                  if (!mMasked)
                  {
                     --mCursorPos;
                     ensureCursorVisible();
                  }
               }
               else if (mExitOnArrowAtBoundary)
               {
                  // Cursor at position 0 - exit so the container can
                  // navigate to the previous cell
                  setReturnCode(cxID_QUIT);
                  continueOn = false;
               }
               break;
            case KEY_RIGHT:
               if (!mMasked && mCursorPos < (int)mValue.length())
               {
                  ++mCursorPos;
                  ensureCursorVisible();
               }
               else if (mExitOnArrowAtBoundary && mCursorPos >= (int)mValue.length())
               {
                  // Cursor at end of text - exit so the container can
                  // navigate to the next cell
                  setReturnCode(cxID_EXIT);
                  continueOn = false;
               }
               break;
            case ERR:
               break;
            default:
            {
               // Process any functions tied to this keypress
               bool functionExists = false;
               continueOn = handleFunctionForLastKey(&functionExists);
               continueOn = (continueOn && !mLeaveNow);

               if (!functionExists)
               {
                  if (isPrintable(lastKey))
                  {
                     // Check if we can insert (max length check)
                     bool canInsert = true;
                     if (mMaxInputLength > 0 && (int)mValue.length() >= mMaxInputLength)
                        canInsert = false;

                     if (canInsert)
                     {
                        // Insert the character at mCursorPos
                        mValue.insert(mCursorPos, 1, (char)lastKey);
                        ++mCursorPos;

                        // Validate the new text
                        int lastCapIndex = indexOfLastCap(mValidator.getValidatorStr());
                        bool canValidate = true;
                        if (lastCapIndex > -1)
                        {
                           canValidate = (mValue.length() >= (unsigned)lastCapIndex + 1);
                        }
                        if (canValidate && !mValidator.textIsValid(mValue))
                        {
                           // Invalid - remove the character we just inserted
                           --mCursorPos;
                           mValue.erase(mCursorPos, 1);
                        }
                     }

                     ensureCursorVisible();
                     curs_set(1);
                  }

                  // Check parent multi-line input for auto-fill
                  if (mParentMLInput != nullptr)
                  {
                     mParentMLInput->autoFillFromValidOptions(true);
                     if (isFull())
                     {
                        continueOn = false;
                     }
                     else
                     {
                        mCursorPos = (int)mValue.length();
                        ensureCursorVisible();
                     }
                  }
               }
               break;
            }
         }

         // Run the onKey function
         if (mRunOnKeyFunction)
         {
            string oldValue = getValue();
            runOnKeyFunction();
            string newValue = getValue();
            // If onKey function changed the value, update cursor
            if (newValue != oldValue)
            {
               mCursorPos = (int)mValue.length();
               ensureCursorVisible();
            }
         }

         mJustStartedFocus = false;

         // Check if input is full and should exit (for non-bordered inputs)
         if (isFull() && mExitOnFull)
         {
            setReturnCode(cxID_EXIT);
            break;
         }
      }
   }

   // Update prevInput to reflect final mValue (for compatibility)
   prevInput = mValue;

   // Turn off the value color and disable the message attributes
   if (useColors)
   {
      wcolor_set(mWindow, 0, nullptr);
   }
   disableValueAttrs();

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
