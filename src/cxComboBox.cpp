// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxComboBox.h"
#include "cxForm.h"
#include "cxBase.h"
#include <set>
using std::string;
using std::set;
using cxBase::stringWithoutHotkeyChars;

cxComboBox::cxComboBox(cxWindow *pParentWindow,
                       int pRow, int pCol,
                       int pHeight, int pWidth,
                       const string& pLabel,
                       eBorderStyle pBorderStyle,
                       eInputOptions pInputOption,
                       eInputTypes pInputType,
                       string *pExtValue, bool pMenuEnabled,
                       int pRightLabelOffset, int pRightLabelHeight,
                       int pRightLabelWidth, bool pShowRightLabel)
   : cxMultiLineInput(pParentWindow, pRow, pCol,
           // If the menu is to be enabled, ensure that the
           //  input portion is 1 line high.  Otherwise, use
           //  the given height for the input portion.
           (pMenuEnabled ?  (pBorderStyle == eBS_NOBORDER ? 1 : 3) : pHeight),
           pWidth, pLabel, pBorderStyle, pInputOption, pInputType, pExtValue,
           pRightLabelOffset, pRightLabelHeight, pRightLabelWidth,
           pShowRightLabel),
     // The location & width of mMenu may be slightly different,
     //  depending on whether there is a border or not.
     // New way - With a border, on the same row as the input portion
     mMenu(nullptr, (pBorderStyle == eBS_NOBORDER ? pRow : pRow+1),
            (pBorderStyle == eBS_NOBORDER ? pCol+(int)pLabel.length() :
                                pCol+(int)pLabel.length()+1),
            (pHeight > 1 ? pHeight-1 : 1),
            (pBorderStyle == eBS_NOBORDER ? pWidth-(int)pLabel.length() :
                pWidth-(int)pLabel.length()-2),
            "", nullptr, nullptr, eBS_SINGLE_LINE)
{
   // Bring the menu to the top, so that it (probably) won't have to be
   //  brought to the top when shown.
   mMenu.bringToTop(false);
   mMenu.hide(); // Make sure the menu is hidden, too

   // Make sure the parent class won't run its onFocus & onLeave functions
   //  in its input loop, so that they can be run at the proper time in
   //  this class' input loop.
   cxMultiLineInput::runFocusFunctions(false);
   // Also tell cxMultiLineInput not to run the validator function, because
   //  this class will run it.
   cxMultiLineInput::toggleValidatorFunction(false);

   // Enable/disable the menu according to pMenuEnabled.
   mMenu.setEnabled(pMenuEnabled);

   // Add the down arrow to mDropDownKeys
   mDropDownKeys.insert(KEY_DOWN);
} // Constructor

// Copy constructor
cxComboBox::cxComboBox(const cxComboBox& pThatComboBox)
   : cxMultiLineInput(nullptr, pThatComboBox.top(), pThatComboBox.left(),
           // If the menu is enabled, ensure that the input portion is 1 line
           //  high.  Otherwise, go ahead and just use the height of the other
           //  input.
           (pThatComboBox.mMenu.isEnabled() ? (pThatComboBox.getBorderStyle() == eBS_NOBORDER ? 1 : 3) : pThatComboBox.height()),
            pThatComboBox.width(), pThatComboBox.getLabel(),
            pThatComboBox.getBorderStyle(),
            (eInputOptions)pThatComboBox.getInputOption(),
            pThatComboBox.getInputType(), pThatComboBox.getExtValue()),
     mMenu(pThatComboBox.mMenu),
     mUseAltMenuItemText(pThatComboBox.mUseAltMenuItemText),
     mDropDownKeys(pThatComboBox.mDropDownKeys),
     mRunValidatorFunction(pThatComboBox.mRunValidatorFunction)
{
   // Make sure the parent class won't run its onFocus & onLeave functions
   //  in its input loop, so that they can be run at the proper time in
   //  this class' input loop.
   cxMultiLineInput::runFocusFunctions(false);
} // Copy constructor

cxComboBox::~cxComboBox()
{
} // Destructor

long cxComboBox::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   setReturnCode(cxID_EXIT);

   if (isEnabled())
   {
      mIsModal = true;

      // Run the onFocus function if we're set to run it.  If it returns true,
      //  that means we should exit.. so only do the input loop if it returns
      //  false or we aren't set to run the onFocus function.  Also, check to
      //  make sure that getLeaveNow() returns false, in case the onFocus
      // function called exitNow() or quitNow().
      bool continueOn = true;
      continueOn = !runOnFocusFunction() && !getLeaveNow();

      // Show the input window
      if (pShowSelf)
      {
         show(pBringToTop, pShowSubwindows);
      }
      if (continueOn)
      {
         // If the menu is enabled, show it if it has any items.
         // Note (08/23/07): This seems inconsistent with the way combo boxes
         //  usually work in other UI toolkits.
         /*
         if (mMenu.isEnabled()) {
            if (mMenu.numMenuItems() > 0) {
               mMenu.show(pBringToTop, false);
            }
         }
         */

         bool runOnLeaveFunc = true;
         if (inputLoopEnabled())
         {
            setReturnCode(doInputLoop(pShowSelf, runOnLeaveFunc));
            // If the validator function is enabled, then we need to do
            //  validation.
            if (mRunValidatorFunction)
            {
               // Look for the last keypress in the function keys
               //  of the parent form heirarchy..  If the key isn't
               //  found there, keep doing the input loop until
               //  the user enters something valid (using the
               //  validator function).
               int lastKey = cxMultiLineInput::getLastKey();
               bool doValidation = true;
               // If the user clicked outside the input, as well as the
               //  form the input is on (if the input is on a form), then
               //  don't do validation.
               if ((lastKeyWasMouseEvt() && !mouseEvtWasInWindow()))
               {
                  cxForm *parentForm = getParentForm();
                  if (parentForm != nullptr)
                  {
                     if (!(parentForm->pointIsInWindow(mMouse.y, mMouse.x)))
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
                  if (!getValidateOnReverse())
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
                        (runValidatorFunction() != ""))
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
                     setReturnCode(doInputLoop(pShowSelf, runOnLeaveFunc));
                     lastKey = getLastKey();
                     // If mValidateOnReverse is false, then check for
                     //  reverse navigation keys (shift-tab & up arrow).
                     //  If the user pressed those keys, then stop doing
                     //  doing validation.
                     if (!getValidateOnReverse())
                     {
                        if ((lastKey == SHIFT_TAB) || (lastKey == KEY_UP))
                        {
                           break;
                        }
                     }
                  }
               }
            }
         }

         // Make sure the menu is hidden before leaving
         mMenu.hide();

         mIsModal = false;

         // Run the onLeave function
         if (runOnLeaveFunc)
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

void cxComboBox::resize(int pNewHeight, int pNewWidth, bool pRefresh)
{
   // Don't bother resizing unless pNewHeight or pNewWidth are
   //  different than the current height/width.
   if ((pNewHeight != height()) || (pNewWidth != width()))
   {
      // If the menu is enabled, apply the width to both the input portion
      //  and the menu, and make the input portion 1 line high and use
      //  the rest of the height for the menu.  Otherwise, just resize
      //  the input portion (so the combo box acts just like a
      //  cxMultiLineInput).
      if (mMenu.isEnabled())
      {
         // pNewHeight should be at least 2 (or 4, if there are borders) to
         //  leave room for the input and the drop-down menu.
         int minHeight = 2;
         if (getBorderStyle() != eBS_NOBORDER)
         {
            minHeight = 4;
         }

         if (pNewHeight >= minHeight)
         {
            // Resize the input
            cxMultiLineInput::resize(1, pNewWidth, pRefresh);

            // Resize the menu
            // Get the new maximum length of the input - this
            //  will be the width of the drop-down menu.
            int newMenuWidth = maxValueLen();
            mMenu.resize(pNewHeight-1, newMenuWidth, pRefresh);
         }
      }
      else
      {
         cxMultiLineInput::resize(pNewHeight, pNewWidth, pRefresh);
      }
   }
} // resize

bool cxComboBox::move(int pNewRow, int pNewCol, bool pRefresh)
{
   // In case the move isn't completely successful
   int oldRow = top();
   int oldCol = left();

   // Move the input box
   bool moved = cxMultiLineInput::move(pNewRow, pNewCol, false);
   if (moved)
   {
      // Move the menu
      if (getBorderStyle() == eBS_NOBORDER)
      {
         moved = mMenu.move(pNewRow+1, pNewCol+(int)getLabel().length(), false);
      }
      else
      {
         moved = mMenu.move(pNewRow+2, pNewCol+(int)getLabel().length()+1, false);
      }

      // If the menu couldn't move, then move the input box back
      //  to where it was before.
      if (!moved)
      {
         cxMultiLineInput::move(oldRow, oldCol, false);
      }
   }

   // Refresh if the move was successful.
   if (moved && pRefresh)
   {
      show(false, false);
   }

   return(moved);
} // move

void cxComboBox::erase(bool pEraseSubwindows)
{
   cxMultiLineInput::erase(false);
   mMenu.erase();
} // erase

void cxComboBox::hide(bool pHideSubwindows)
{
   cxMultiLineInput::hide(pHideSubwindows);
   mMenu.hide(false);
} // hide

void cxComboBox::unhide(bool pUnhideSubwindows)
{
   if (isEnabled())
   {
      cxMultiLineInput::unhide(pUnhideSubwindows);
      // Don't un-hide the menu
   }
} // unhide

void cxComboBox::appendToMenu(const string& pDisplayText,
                              const string& pItemText, bool pRefresh,
                              bool pBringToTop)
                              {
   // Add the item to the menu.  Use the current # of menu items as the return
   //  code.
   mMenu.append(pDisplayText, mMenu.numMenuItems(), "", cxITEM_NORMAL, true,
                pItemText);
   if (pRefresh)
   {
      mMenu.show(pBringToTop, false);
   }
} // appendToMenu

bool cxComboBox::removeFromMenu(unsigned pItemIndex, bool pRefresh)
{
   return(mMenu.remove(pItemIndex, true, pRefresh));
} // removeFromMenu

bool cxComboBox::removeFromMenu(const string& pItemText, bool pDisplayText,
                                bool pRefresh)
                                {
   return(mMenu.remove(pItemText, pDisplayText, true, pRefresh));
} // removeFromMenu

void cxComboBox::removeAllItemsFromMenu()
{
   mMenu.removeAllItems(true, false);
   // Hide the menu, since it's empty
   mMenu.hide(false);
} // removeAllItemsFromMenu

void cxComboBox::showMenu(bool pBringToTop)
{
   mMenu.show(pBringToTop, false);
} // showMenu

void cxComboBox::hideMenu()
{
   mMenu.hide(false);
} // hideMenu

unsigned cxComboBox::numMenuItems() const
{
   return(mMenu.numMenuItems());
} // numMenuItems

string cxComboBox::getItemText(int pIndex) const
{
   return(mMenu.getItemText(pIndex));
} // getItemText

int cxComboBox::height() const
{
   int comboBoxHeight = cxMultiLineInput::height();

   // If the drop-down menu is enabled, add its
   //  height into the combo box height.
   if (mMenu.isEnabled())
   {
      comboBoxHeight += mMenu.height();
      // If there is a border, subtract 1 from the height
      //  (because the menu is located directly underneath
      //  the input line, which overlaps the bottom border by
      //  1 line).
      if (getBorderStyle() != eBS_NOBORDER)
      {
         --comboBoxHeight;
      }
   }

   return(comboBoxHeight);
} // height

int cxComboBox::width() const
{
   // The width of the input is the larger
   //  of the input and the menu's width.
   return(cxMultiLineInput::width());
} // width

int cxComboBox::inputHeight() const
{
   return(cxMultiLineInput::height());
} // inputHeight

int cxComboBox::inputWidth() const
{
   return(cxMultiLineInput::width());
} // inputWidth

int cxComboBox::menuHeight() const
{
   return(mMenu.height());
} // menuHeight

int cxComboBox::menuWidth() const
{
   return(mMenu.width());
} // menuWidth

void cxComboBox::setMenuHeight(int pNewHeight, bool pRefresh)
{
   mMenu.resize(pNewHeight, mMenu.width(), pRefresh);
} // changeMenuHeight

void cxComboBox::toggleMenu(bool pEnableMenu, bool pRefresh)
{
   if (pEnableMenu != mMenu.isEnabled())
   {
      mMenu.setEnabled(pEnableMenu);
      // If pEnableMenu is true, make sure the input portion is
      //  only 1 line high.  Also, make sure the menu is sized &
      //  positioned correctly, in case the combo box was resized
      //  while the menu was disabled.
      if (mMenu.isEnabled())
      {
         // The top row of the menu should be on the same row as the input.
         int menuTop = cxMultiLineInput::top();
         // The input & menu portions will have to be sized (& moved)
         //  properly, depending on whether or not there is a border.
         //  There should be 1 line for input, and the menu should be
         //  directly under the input line, just to the right of the
         //  label, and as wide as the input minus the label length.
         if (getBorderStyle() == eBS_NOBORDER)
         {
            cxMultiLineInput::resize(1, width(), pRefresh);
            mMenu.resize(mMenu.height(), width()-(int)(getLabel().length()),
                  false);
            mMenu.move(menuTop, left()+(int)(getLabel().length()), pRefresh);
         }
         else
         {
            cxMultiLineInput::resize(3, width(), pRefresh);
            mMenu.resize(mMenu.height(), width()-(int)(getLabel().length())-2,
                  false);
            ++menuTop;
            // This needs to be decremented if the input has a border
            mMenu.move(menuTop, left()+(int)(getLabel().length())+1, pRefresh);
         }
      }
      else
      {
         mMenu.hide();
      }
   }
} // toggleMenu

bool cxComboBox::menuIsEnabled() const
{
   return(mMenu.isEnabled());
} // menuIsEnabled

string cxComboBox::cxTypeStr() const
{
   return("cxComboBox");
} // cxTypeStr

cxMenu* cxComboBox::getMenu()
{
   return(&mMenu);
} // getmenu

void cxComboBox::setDropDownKey(int pKey)
{
   mDropDownKeys.clear();
   mDropDownKeys.insert(pKey);
} // setDropDownKey

void cxComboBox::addDropDownKey(int pKey)
{
   mDropDownKeys.insert(pKey);
} // addDropDownKey

void cxComboBox::setDropDownKeys(const set<int>& pKeys)
{
   mDropDownKeys = pKeys;
} // setDropDownKeys

void cxComboBox::removeDropDownKey(int pKey)
{
   mDropDownKeys.erase(pKey);
} // removeDropDownkey

bool cxComboBox::hasDropDownKey(int pKey) const
{
   return(mDropDownKeys.find(pKey) != mDropDownKeys.end());
} // hasDropDownKey

void cxComboBox::toggleValidatorFunction(bool pRunValidatorFunction)
{
   mRunValidatorFunction = pRunValidatorFunction;
} // toggleValidatorFunction

bool cxComboBox::getUseValidatorFunction() const
{
   return(mRunValidatorFunction);
} // getUseValidatorFunction

void cxComboBox::setUseAltMenuItemText(bool pUseAltMenuItemText)
{
   mUseAltMenuItemText = pUseAltMenuItemText;
} // setUseAltMenuItemText

bool cxComboBox::getUseAltMenuItemText() const
{
   return(mUseAltMenuItemText);
} // getUseAltMenuItemText

/////////////////////////
// Protected functions //
/////////////////////////

long cxComboBox::doInputLoop(bool pShowSelf, bool& pRunOnLeaveFunction)
{
   long returnCode = cxID_EXIT;
   //bool hideTheMenu = true; // Whether or not to hide the menu each time
   pRunOnLeaveFunction = true;
   bool continueOn = true;
   while (continueOn)
   {
      // Set focus to the input.  If the user
      //  pressed the down-arrow, let the user
      //  select something from the menu (if it
      //  has items), and set that value in
      //  the input.
      // Call the base class showModal(), since this
      //  function gets called from this class' showModal()
      returnCode = cxMultiLineInput::showModal(pShowSelf, true, false);
      if (returnCode == cxID_QUIT)
      {
         // The user decided to quit out of the input
         continueOn = false;
      }
      else
      {
         int lastKey = cxMultiLineInput::getLastKey();
         // If the input is not editable and the menu is enabled, then allow
         //  the user to choose an option from the menu.
         if (!isEditable() && mMenu.isEnabled())
         {
            returnCode = showMenuModal(pShowSelf, continueOn);
            continueOn = false;
         }
         // If the last key is set up as a quit key, then quit and return
         //  cxID_QUIT.  If the key isn't there, look for it in
         //  the exit keys (if it's there, quit and return cxID_EXIT).
         //  If not there either, handle the key in a switch.
         else if (hasQuitKey(lastKey))
         {
            returnCode = cxID_QUIT;
            continueOn = false;
         }
         else if (hasExitKey(lastKey))
         {
            returnCode = cxID_EXIT;
            continueOn = false;
         }
         // If the user pressed a drop-down menu key, show the menu modally.
         else if (hasDropDownKey(lastKey))
         {
            returnCode = showMenuModal(pShowSelf, continueOn);
         }
         else
         {
            switch (lastKey)
            {
               // If the user clicked the mouse outside the window, then
               //  don't continue on.
               case KEY_MOUSE:
                  continueOn = mouseEvtWasInWindow();
                  break;
               default:
                  continueOn = false;
                  // If the parent form (or one of its parent cxMultiForms)
                  //  has the key set up as a form function key, then
                  //  the onLeave function shouldn't be run.
                  pRunOnLeaveFunction = !searchParentFormsForFKey(lastKey, true);
                  break;
            }
         }
      }
   }

   // If pRunOnLeaveFunction is still true, then check mQuitKeys and mExitKeys
   //  to see whether the onLeave function should run.  If the last keypress
   //  is a quit key or an exit key, then if mQuitKeys or mExitKeys has a false
   //  for the last keypress, then don't run the onLeave function.
   if (pRunOnLeaveFunction)
   {
      int lastKey = getLastKey();
      if (mQuitKeys.find(lastKey) != mQuitKeys.end())
      {
         if (!mQuitKeys[lastKey])
         {
            pRunOnLeaveFunction = false;
         }
      }
      else if (mExitKeys.find(lastKey) != mExitKeys.end())
      {
         if (!mExitKeys[lastKey])
         {
            pRunOnLeaveFunction = false;
         }
      }
   }

   // mLeaveNow should be false
   mLeaveNow = false;

   return(returnCode);
} // doInputLoop

void cxComboBox::copyCxComboBoxStuff(const cxComboBox* pThatComboBox)
{
   if ((pThatComboBox != nullptr) && (pThatComboBox != this))
   {
      // Copy the stuff inherited from cxMultiLineInput, then copy
      //  this class' stuff.
      try
      {
         copyCxMultiLineInputStuff((const cxMultiLineInput*)pThatComboBox);
      }
      catch (const cxWidgetsException& exc)
      {
         throw(cxWidgetsException("Couldn't copy base cxMultiLineInput stuff (copying a cxComboBox)."));
      }

      mMenu = pThatComboBox->mMenu;
      mUseAltMenuItemText = pThatComboBox->mUseAltMenuItemText;
      mDropDownKeys = pThatComboBox->mDropDownKeys;
      mRunValidatorFunction = pThatComboBox->mRunValidatorFunction;
      setLastKey(pThatComboBox->getLastKey());
   }
} // copyCxComboBoxStuff

bool cxComboBox::menuIsModal() const
{
   return(mMenu.isModal());
} // menuIsModal

/////////////////////
// Private methods //
/////////////////////

long cxComboBox::showMenuModal(bool pShowMenu, bool& pContinueOn)
{
   pContinueOn = true;
   long returnCode = cxID_EXIT;

   // If the menu is enabled, show it modally so that the user can choose one
   //  of the menu items (if there is more than 1).  If the menu is not
   //  enabled, then the input loop should exit.
   if (mMenu.isEnabled())
   {
      bool hideTheMenu = true; // Whether or not to hide the menu
      if (mMenu.numMenuItems() > 0)
      {
         // If there is only 1 item in the list, go ahead
         //  and use it; otherwise, show the menu modally
         //  and let the user choose which one to use.
         if (mMenu.numMenuItems() == 1)
         {
            mMenu.show(true, false);
            setValue(mMenu.getItemText(0), !mUseAltMenuItemText);
            hideTheMenu = false;
         }
         else
         {
            long retval = mMenu.showModal(pShowMenu, true, false);
            setLastKey(mMenu.getLastKey());
            if (retval != cxID_QUIT)
            {
               // Set the menu item text as the value in the input.
               if (mMenu.getHotkeyHighlighting())
               {
                  setValue(stringWithoutHotkeyChars(
                           mMenu.getCurrentItemText()));
               }
               else
               {
                  setValue(mMenu.getCurrentItemText());
               }
            }
            hideTheMenu = true; // Hide the menu when we're done
         }
      }
      else
      {
         pContinueOn = false;
      }

      if (hideTheMenu)
      {
         mMenu.hide();
      }
   }
   else
   {
      returnCode = cxID_EXIT;
      pContinueOn = false;
   }

   return(returnCode);
} // showDropDownmenu
