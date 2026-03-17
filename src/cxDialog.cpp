// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxDialog.h"
using std::string;

cxDialog::cxDialog(cxWindow *pParentWindow, int pRow,
                   int pCol, int pHeight,
                   int pWidth,
                   const string& pTitle,
                   const string& pMessage,
                   const string& pStatus)
   : cxWindow(pParentWindow, pRow, pCol, pHeight, pWidth, pTitle, pMessage, pStatus)
{
}

cxDialog::cxDialog(cxWindow *pParentWindow,
                   int pRow, int pCol, const string& pTitle,
                   const string& pMessage, const string& pStatus)
   : cxWindow(pParentWindow, pRow, pCol, pTitle, pMessage, pStatus)
{
}

cxDialog::cxDialog(cxWindow *pParentWindow,
                   const string& pTitle, const string& pMessage,
                   const string& pStatus)
   : cxWindow(pParentWindow, pTitle, pMessage, pStatus)
{
}

cxDialog::cxDialog(cxWindow *pParentWindow,
                   const string& pMessage, const string& pStatus)
   : cxWindow(pParentWindow, pMessage, pStatus)
{
}

cxDialog::cxDialog(cxWindow *pParentWindow, const string& pMessage)
   : cxWindow(pParentWindow, pMessage)
{
}

cxDialog::~cxDialog()
{
}

long cxDialog::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   // Only do the input loop if the dialog is enabled.
   if (isEnabled())
   {
      mIsModal = true;

      if (pShowSelf)
      {
         show(pBringToTop, pShowSubwindows);
      }
      setReturnCode(cxID_QUIT);

      // For dragging the window around
      int pressedX = 0;
      int pressedY = 0;
      bool movingWin = false;

      // Do the input loop
      bool continueOn = true;
      while (continueOn)
      {
         int c = wgetch(mWindow);

         // Note: Cannot use switch statement due to mKeyCancel
         // not being a "const int". Blech!
         switch(c)
         {
            case ESCAPE: // Defined in cxKeyDefines.h
               setReturnCode(cxID_QUIT);
               continueOn=false;
               break;
            case ENTER:  // Defined in cxKeyDefines.h
            case KEY_ENTER:
               setReturnCode(cxID_EXIT);
               continueOn=false;
               break;
#ifdef NCURSES_MOUSE_VERSION
            case KEY_MOUSE:
               if (getmouse(&mMouse) == OK)
               {
                  // Run a function that may exist for the mouse state.  If
                  //  no function exists for the mouse state, then process
                  //  it here.
                  bool mouseFuncExists = false;
                  continueOn = handleFunctionForLastMouseState(&mouseFuncExists);
                  if (!mouseFuncExists)
                  {
                     switch (mMouse.bstate)
                     {
                        case BUTTON1_PRESSED:
                           // Update pressedX and pressedY
                           pressedX = mMouse.x;
                           pressedY = mMouse.y;
                           // If the user clicked in the title, let them move
                           //  the window.
                           movingWin = mouseEvtWasInTitle();
                           break;
                        case BUTTON1_RELEASED:
                           // If the user is moving the window, then go ahead
                           //  and move it.
                           if (movingWin)
                           {
                              moveRelative(mMouse.y - pressedY, mMouse.x
                                           - pressedX, true);
                              movingWin=false; // reset the moving of the window
                           }
                           break;
                     }
                  }
               }
               break;
#endif
            default:
               //mvwprintw(stdscr, 23, 60, "cxDialog P:%03d:", c);
               //wrefresh(stdscr);
               break;
         }
      }

      mIsModal = false;
   }

   return(getReturnCode());
} // showModal

string cxDialog::cxTypeStr() const
{
   return("cxDialog");
} // cxTypeStr
