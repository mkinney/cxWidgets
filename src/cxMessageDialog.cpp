// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxMessageDialog.h"
using std::string;
using std::make_pair;
using std::make_shared;

cxMessageDialog::cxMessageDialog(cxWindow *pParentWindow, int pRow,
                   int pCol, int pHeight,
                   int pWidth,
                   const string& pTitle,
                   const string& pMessage,
                   long pStyle,
                   const string& pStatus)
   : cxDialog(pParentWindow, pRow, pCol, pHeight, pWidth, pTitle, pMessage, pStatus)
{
   // Set variables for the starting column & row
   //  & total width of the buttons
   int theCol = left();
   int theRow = bottom() - OKBTN_HEIGHT; // All buttons should have the same height
   int totalButtonWidth = 0;
   // If the window has a border, the buttons should
   //  be created 1 column over.  If there is no border,
   //  the row should be incremented so the buttons are
   //  one row lower.
   if (getBorderStyle() != eBS_NOBORDER)
   {
      ++theCol;
   }
   else
   {
      ++theRow;
   }

   // Create the OK button if they've specified
   //  cxOK or cxYES
   if ((pStyle & cxOK) == cxOK)
   {
      mOKBtn = make_shared<cxButton>(this, theRow, theCol, OKBTN_HEIGHT, OKBTN_WIDTH, "&OK");
      mOKBtn->setHotkeyHighlighting(true);
      setFocus(mOKBtn, false);
      theCol += mOKBtn->width();
      totalButtonWidth += mOKBtn->width();
   }
   else if ((pStyle & cxYES) == cxYES)
   {
      mOKBtn = make_shared<cxButton>(this, theRow, theCol, YESBTN_HEIGHT, YESBTN_WIDTH, "&Yes");
      mOKBtn->setHotkeyHighlighting(true);
      setFocus(mOKBtn, false);
      theCol += mOKBtn->width();
      totalButtonWidth += mOKBtn->width();
   }

   // Create the cancel button if they've specified
   //  cxCANCEL or cxNO
   if ((pStyle & cxCANCEL) == cxCANCEL)
   {
      mCancelBtn = make_shared<cxButton>(this, theRow, theCol, CANCELBTN_HEIGHT, CANCELBTN_WIDTH, "&Cancel");
      mCancelBtn->setHotkeyHighlighting(true);
      totalButtonWidth += mCancelBtn->width();
      // If there is no OK button or if the cxNO_DEFAULT attribute was used,
      //  set focus to this button.
      if ((mOKBtn == nullptr) || ((pStyle & cxNO_DEFAULT) == cxNO_DEFAULT))
      {
         setFocus(mCancelBtn, false);
      }
   }
   else if ((pStyle & cxNO) == cxNO)
   {
      mCancelBtn = make_shared<cxButton>(this, theRow, theCol, NOBTN_HEIGHT, NOBTN_WIDTH, "&No");
      mCancelBtn->setHotkeyHighlighting(true);
      totalButtonWidth += mCancelBtn->width();
      // If there is no OK button or if the cxNO_DEFAULT attribute was used,
      //  set focus to this button.
      if ((mOKBtn == nullptr) || ((pStyle & cxNO_DEFAULT) == cxNO_DEFAULT))
      {
         setFocus(mCancelBtn, false);
      }
   }

   // Make sure the window is big enough to hold the buttons
   //  and message text
   int minWidth = totalButtonWidth;
   if (getBorderStyle() != eBS_NOBORDER)
   {
      minWidth += 2;
   }
   int minHeight = OKBTN_HEIGHT + mMessageLines.size();
   if (getBorderStyle() != eBS_NOBORDER)
   {
      minHeight += 2;
   }
   if ((width() < minWidth) || (height() < minHeight))
   {
      resize(minHeight, minWidth, false);
      // Make sure  the buttons are on the bottom of the
      //  form
      theRow = bottom() - OKBTN_HEIGHT;
      if (mOKBtn != nullptr)
      {
         mOKBtn->move(theRow, mOKBtn->left(), false);
      }
      if (mCancelBtn != nullptr)
      {
         mCancelBtn->move(theRow, mCancelBtn->left(), false);
      }
   }
   else
   {
      // The window is big enough to enclose everything..
      //  Make sure the buttons are horizontally centered
      //  in the window.
      if ((mOKBtn != nullptr) && (mCancelBtn != nullptr))
      {
         int okBtnCol = centerCol() - (totalButtonWidth/2);
         int cancelBtnCol = okBtnCol + mOKBtn->width();
         mOKBtn->move(mOKBtn->top(), okBtnCol, false);
         mCancelBtn->move(mCancelBtn->top(), cancelBtnCol, false);
      }
      else if ((mOKBtn != nullptr) && (mCancelBtn == nullptr))
      {
         mOKBtn->centerHoriz(false);
      }
      else if ((mOKBtn == nullptr) && (mCancelBtn != nullptr))
      {
         mCancelBtn->centerHoriz(false);
      }
   }
} // constructor

// Simplified constructor
cxMessageDialog::cxMessageDialog(cxWindow *pParentWindow, const string& pTitle, const string& pMessage)
   : cxDialog(pParentWindow, pTitle, pMessage, ""),
     mOKBtn(nullptr),
     mCancelBtn(nullptr)
{
   resize(height()+OKBTN_HEIGHT, width(), false); // Make room for the OK button

   int useRow = bottom()-OKBTN_HEIGHT; // TODO: how to handle exact placement if the programmer wants to...
   mOKBtn = make_shared<cxButton>(this, useRow, centerCol()-1, OKBTN_HEIGHT, OKBTN_WIDTH, "&OK");
   mOKBtn->setHotkeyHighlighting(true);
   setFocus(mOKBtn, false);
} // simplified constructor

// Copy constructor
cxMessageDialog::cxMessageDialog(const cxMessageDialog& pThatDialog)
   : cxDialog(pThatDialog.getParent(), pThatDialog.top(), pThatDialog.left(),
              pThatDialog.height(), pThatDialog.width(), pThatDialog.getTitle(),
              pThatDialog.getMessage(), pThatDialog.getStatus())
{

   // Copy the OK and Cancel buttons
   if (pThatDialog.mOKBtn != nullptr)
   {
      mOKBtn = make_shared<cxButton>(this, pThatDialog.mOKBtn->top(), pThatDialog.mOKBtn->left(),
                            pThatDialog.mOKBtn->height(), pThatDialog.mOKBtn->width(),
                            pThatDialog.mOKBtn->getMessage());
      mOKBtn->setHotkeyHighlighting(pThatDialog.mOKBtn->getHotkeyHighlighting());
      if (pThatDialog.mOKBtn->hasFocus())
      {
         setFocus(mOKBtn, false);
      }
   }
   if (pThatDialog.mCancelBtn != nullptr)
   {
      mCancelBtn = make_shared<cxButton>(this, pThatDialog.mCancelBtn->top(),
                                pThatDialog.mCancelBtn->left(),
                                pThatDialog.mCancelBtn->height(),
                                pThatDialog.mCancelBtn->width(),
                                pThatDialog.mCancelBtn->getMessage());
      mCancelBtn->setHotkeyHighlighting(pThatDialog.mCancelBtn->getHotkeyHighlighting());
      if (pThatDialog.mCancelBtn->hasFocus())
      {
         setFocus(mCancelBtn, false);
      }
   }
} // copy constructor

cxMessageDialog::~cxMessageDialog()
{
} // destructor

long cxMessageDialog::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   setReturnCode(cxID_EXIT);

   // Only do the input loop if the dialog window is enabled.
   if (isEnabled())
   {
      mIsModal = true;

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
         // Disable the cursor (saving the current cursor state)
         int prevCursorState = curs_set(0);
         setReturnCode(doInputLoop());
         // Set the cursor state back to what it was before
         curs_set(prevCursorState);

         hide();

         // Run the onLeave function
         runOnLeaveFunction();
      }

      mIsModal = false;
   }

   return(getReturnCode());
} // showModal

void cxMessageDialog::hide(bool pHideSubwindows)
{
   // Hide the main window, as well as the buttons.
   cxWindow::hide(pHideSubwindows);

   if (mOKBtn != nullptr)
   {
      mOKBtn->hide(false);
   }
   if (mCancelBtn != nullptr)
   {
      mCancelBtn->hide(false);
   }
} // hide

void cxMessageDialog::unhide(bool pUnhideSubwindows)
{
   // If the window is active, unhide it, as well as the buttons.
   if (isEnabled())
   {
      cxWindow::unhide(pUnhideSubwindows);

      if (mOKBtn != nullptr)
      {
         mOKBtn->unhide(false);
      }
      if (mCancelBtn != nullptr)
      {
         mCancelBtn->unhide(false);
      }
   }
} // unhide

bool cxMessageDialog::move(int pNewRow, int pNewCol, bool pRefresh)
{
   // originalTop and originalLeft are used for calculating the y and x offsets
   //  for moving the buttons.
   int originalTop = top();
   int originalLeft = left();
   bool moved = cxWindow::move(pNewRow, pNewCol, pRefresh);
   if (moved)
   {
      int y_relative = top() - originalTop;
      int x_relative = left() - originalLeft;
      if (mOKBtn != nullptr)
      {
         mOKBtn->moveRelative(y_relative, x_relative, pRefresh);
      }
      if (mCancelBtn != nullptr)
      {
         mCancelBtn->moveRelative(y_relative, x_relative, pRefresh);
      }
   }
   else
   {
      // Sometimes, if the user wanted to move the dialog box off the screen,
      //  it wouldn't move (which is normal), but the buttons would be hidden
      //  for some reason.  So, this makes sure that the buttons will still
      //  show.
      if (pRefresh)
      {
         show(false, true);
      }
   }

   return(moved);
} // move

long cxMessageDialog::setFocus(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   return cxWindow::setFocus(pShowSelf, pBringToTop, pShowSubwindows);
}

void cxMessageDialog::setFocus(const std::shared_ptr<cxButton>& pButton, bool pShow)
{
   if (pButton != nullptr)
   {

      attr_t focusAttr=cxBase::hasAttr(eHOTKEY, A_STANDOUT) ? A_REVERSE : A_STANDOUT ;

      if (mOKBtn == pButton)
      {
         mOKBtn->setFocusColors(focusAttr);
         if (pShow)
         {
            mOKBtn->show(true, true);
         }

         if (mCancelBtn != nullptr)
         {
            mCancelBtn->unSetFocusColors(focusAttr);
            if (pShow)
            {
               mCancelBtn->show(true, true);
            }
         }
      }
      else if (mCancelBtn == pButton)
      {
         mCancelBtn->setFocusColors(focusAttr);
         if (pShow)
         {
            mCancelBtn->show(true, true);
         }

         if (mOKBtn != nullptr)
         {
            mOKBtn->unSetFocusColors(focusAttr);
            if (pShow)
            {
               mOKBtn->show(true, true);
            }
         }
      }
   }
} // setFocus

string cxMessageDialog::cxTypeStr() const
{
   return("cxMessageDialog");
} // cxKeyStr

//// Protected methods

long cxMessageDialog::doInputLoop()
{
   long returnCode = cxID_EXIT;

   // for dragging the window around
   int pressedX = 0;
   int pressedY = 0;
   bool movingWin = false;

   bool continueOn = true;
   while (continueOn)
   {
      int key = wgetch(mWindow);
      setLastKey(key);
      // This class does not check the quit keys & exit keys because we need
      //  a definitive yes/no response from the user.

      switch(key)
      {
         case ESCAPE: // Defined in cxKeydefines.h
            returnCode = cxID_QUIT;
            continueOn=false;
            break;
         case KEY_LEFT:
         case KEY_UP:
            if ((mOKBtn != nullptr) && (mCancelBtn != nullptr))
            {
               if (!(mOKBtn->hasFocus()))
               {
                  setFocus(mOKBtn);
               }
            }
            break;
         case KEY_RIGHT:
         case KEY_DOWN:
         case TAB:
            if ((mOKBtn != nullptr) && (mCancelBtn != nullptr))
            {
               if (mOKBtn->hasFocus())
               {
                  setFocus(mCancelBtn);
               }
            }
            break;
         case ENTER:
         case KEY_ENTER:
         case ' ':
            if ((mOKBtn != nullptr) && (mOKBtn->hasFocus()))
            {
               returnCode=cxID_OK;
            }
            if ((mCancelBtn != nullptr) && (mCancelBtn->hasFocus()))
            {
               returnCode=cxID_CANCEL;
            }
            continueOn=false;
            break;
         case 'o':
         case 'O':  // OK
            // If mOKBtn isn't nullptr and has a value
            //  of "OK", set focus to it, stop the
            //  input loop, and return cxID_OK.
            if ((mOKBtn != nullptr) && (mOKBtn->getMessage() == "&OK"))
            {
               setFocus(mOKBtn);
               returnCode=cxID_OK;
               continueOn = false;
            }
            break;
         case 'c':
         case 'C':  // Cancel
            // If mCancelBtn isn't nullptr and has a value
            //  of "Cancel", set focus to it, stop the
            //  input loop, and return cxID_CANCEL.
            if ((mCancelBtn != nullptr) &&
                (mCancelBtn->getMessage() == "&Cancel"))
                {
               setFocus(mCancelBtn);
               returnCode=cxID_CANCEL;
               continueOn = false;
            }
            break;
         case 'y':
         case 'Y':  // Yes
            // If mOKBtn isn't nullptr and has a value
            //  of "Yes", set focus to it, stop the
            //  input loop, and return cxID_OK.
            if ((mOKBtn != nullptr) && (mOKBtn->getMessage() == "&Yes"))
            {
               setFocus(mOKBtn);
               returnCode = cxID_OK;
               continueOn = false;
            }
            break;
         case 'n':
         case 'N':  // No
            // If mCancelBtn isn't nullptr and has a value
            //  of "No", set focus to it, stop the
            //  input loop, and return cxID_CANCEL.
            if ((mCancelBtn != nullptr) && (mCancelBtn->getMessage() == "&No"))
            {
               setFocus(mCancelBtn);
               returnCode = cxID_CANCEL;
               continueOn = false;
            }
            break;
#ifdef NCURSES_MOUSE_VERSION
         case KEY_MOUSE: // Mouse event
            if (getmouse(&mMouse) == OK)
            {
               switch (mMouse.bstate)
               {
                  // For normal mouse clicks, check to see if the user clicked
                  //  in the OK or Cancel button, and take appropriate action.
                  case BUTTON1_CLICKED:
                     // If the mouse event occurred in the OK button, then return
                     //  cxID_OK.  If the mosue event occurred in the cancel button,
                     //  return cxID_CANCEL.
                     if (mOKBtn != nullptr)
                     {
                        if (mouseEvtInRect(mMouse, mOKBtn->top(), mOKBtn->left(),
                                 mOKBtn->bottom(), mOKBtn->right()))
                                 {
                           setFocus(mOKBtn);
                           returnCode = cxID_OK;
                           continueOn = false;
                        }
                     }
                     if (continueOn) { // If it wasn't in the OK button..
                        if (mCancelBtn != nullptr)
                        {
                           if (mouseEvtInRect(mMouse, mCancelBtn->top(), mCancelBtn->left(),
                                    mCancelBtn->bottom(), mCancelBtn->right()))
                                    {
                              setFocus(mCancelBtn);
                              returnCode = cxID_CANCEL;
                              continueOn = false;
                           }
                        }
                     }
                     break;
                  case BUTTON1_PRESSED:
                     // Looks like they're trying to drag the window...
                     // Let's remember where the first started dragging from
                     pressedX=mMouse.x;
                     pressedY=mMouse.y;
                     // If they clicked in the title, then let them move the
                     //  window.
                     movingWin=mouseEvtWasInTitle();
                     break;
                  case BUTTON1_RELEASED:
                     // If the user is moving the window, then go ahead and
                     //  move it.
                     if (movingWin)
                     {
                        moveRelative(mMouse.y - pressedY, mMouse.x - pressedX, true);
                        movingWin=false; // reset the moving of the window
                     }
                     break;
               }
            }
            break;
#endif
         default:
            break;
      }
   } // while

   // mLeaveNow should be false
   mLeaveNow = false;

   return(returnCode);
} // doInputLoop

///// Private methods

// The show() is private in this class to prevent it from being called from the
//  outside, but it is defined here just in case it is called within this class.
inline long cxMessageDialog::show(bool pBringToTop, bool pShowSubwindows)
{
   return(cxWindow::show(pBringToTop, pShowSubwindows));
} // show
