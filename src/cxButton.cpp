// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxButton.h"
#include "cxKeyDefines.h"
using std::string;
using std::make_shared;

cxButton::cxButton(cxWindow *pParentWindowPtr, int pRow, int pCol, int pHeight,
                   int pWidth, const string& pLabel, eBorderStyle pBorderStyle)
   : cxWindow(pParentWindowPtr, pRow, pCol, pHeight, pWidth, "", pLabel, "",
              pBorderStyle),
     mOnClickFunction(nullptr)
{
} // constructor

cxButton::cxButton(cxWindow *pParentWindowPtr, int pRow, int pCol, int pHeight,
                   int pWidth, const string& pLabel, eBorderStyle pBorderStyle,
                   funcPtr4 pFunction, void *p1, void *p2, void *p3, void *p4)
   : cxWindow(pParentWindowPtr, pRow, pCol, pHeight, pWidth, "", pLabel, "",
              pBorderStyle),
     mOnClickFunction(nullptr)
{
	mOnClickFunction = make_shared<cxFunction4>(pFunction, p1, p2, p3, p4, false, false, false);
} // constructor

cxButton::cxButton(cxWindow *pParentWindowPtr, int pRow, int pCol, int pHeight,
                   int pWidth, const string& pLabel, eBorderStyle pBorderStyle,
                   funcPtr2 pFunction, void *p1, void *p2)
   : cxWindow(pParentWindowPtr, pRow, pCol, pHeight, pWidth, "", pLabel, "",
              pBorderStyle),
     mOnClickFunction(new cxFunction2(pFunction, p1, p2, false, false, false))
{
	mOnClickFunction = make_shared<cxFunction2>(pFunction, p1, p2, false, false, false);
} // constructor

cxButton::cxButton(const cxButton& pButton)
   : cxWindow(nullptr, pButton.top(), pButton.left(), pButton.height(),
              pButton.width(), pButton.getTitle(), pButton.getMessage(),
              pButton.getStatus(), pButton.getBorderStyle()),
     mOnClickFunction(nullptr)
{
   copyOnClickFunction(pButton);
} // copy constructor

cxButton::~cxButton()
{
} // destructor

cxButton& cxButton::operator =(const cxButton& pButton)
{
   if (&pButton != this)
   {
      copyCxWinStuff(&pButton, true);
      copyOnClickFunction(pButton);
   }
   return(*this);
} // operator =

string cxButton::cxTypeStr() const
{
   return("cxButton");
} // cxTypeStr

long cxButton::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   if (isEnabled())
   {
      // Run the onFocus function.  If runOnFocusFunction() returns true, that
      //  means we should exit.. so only do the input loop if it returns false.
      //  Also, check to make sure that getLeaveNow() returns false, in case
      //  the onFocus function called exitNow() or quitNow().
      if (!runOnFocusFunction() && !getLeaveNow())
      {
         // This is a loop that will stay in here as long as the user keeps pressing
         //  enter, which should run the button's event function rather than let
         //  the button lose focus.  Also, check mLeaveNow - this could be set false
         //  by a call to exitNow() or quitNow().
         bool doShowModal = true;
         while (doShowModal && !getLeaveNow())
         {
            // Visual cue: Before showing the button, apply reverse video to the message
            //  area (inside the borders), and after showing the button, remove the
            //  reverse video and update the message area to refresh it.
            cxWindow::addAttr(eMESSAGE, A_REVERSE);
            // Refresh on screen if needed
            if (!pShowSelf) { drawMessage(); unhide(false); }
            cxWindow::showModal(pShowSelf, pBringToTop, pShowSubwindows);
            cxWindow::removeAttr(eMESSAGE, A_REVERSE);
            drawMessage(); unhide(false); // Refresh on screen

            // If the user pressed the enter key or clicked the mouse on the
            //  button, then fire the button's event function.
            bool mouseEvent = false;
            // mouseFunctionExists will be set true if a function was fired for
            //  a mouse event other than a main mouse button click.
            bool mouseFunctionExists = false;
            bool mouseClickOnButton = false;
            int lastKey = cxWindow::getLastKey();
#ifdef NCURSES_MOUSE_VERSION
            if (lastKey == KEY_MOUSE)
            {
               if (getmouse(&mMouse) == OK)
               {
                  mouseEvent = true;
                  // For normal mouse clicks (left click), check to see if the
                  //  user clicked in the button.  For other clicks, just keep
                  //  track that the user clicked a mouse button (if so, then
                  //  we'll want to stay focused in the button).
                  // hmm..  Rather than having to deal with all the ncurses mouse
                  //  events like this, I'm wondering if there's a better way to
                  //  deal with this.  Maybe push this switch up to cxWindow and
                  //  have it call overridable event functions?  (not sure if that
                  //  would make things easier or harder for developers..  will
                  //  have to play with it some more first.)
                  switch (mMouse.bstate)
                  {
                     case BUTTON1_CLICKED:  // Normal mouse click
                        mouseClickOnButton = mouseEvtWasInWindow();
                        break;
                     // For all other mouse states, run
                     //  handleFunctionForLastMouseState().
                     default:
                        handleFunctionForLastMouseState(&mouseFunctionExists);
                        break;
                  }
               }
            }
#endif
            if ((lastKey == ENTER) || (lastKey == KEY_ENTER) || mouseClickOnButton)
            {
               doShowModal = true;
               runOnClickFunction();
            }
            else
            {
               // If there was a mouse event, and no external function existed
               //  for the mouse event, then if this button has a cxPanel for a
               //  parent, and if the mouse event was outside the button
               //  window, then quit.
               if (mouseEvent && !mouseFunctionExists)
               {
                  if (parentIsCxPanel())
                  {
                     if (!mouseEvtWasInWindow())
                     {
                        doShowModal = false;
                     }
                  }
               }
               else
               {
                  // For all other keys other than enter and mouse clicking within
                  //  the window, quit.
                  doShowModal = false;
               }
            }
         }
      }
   }

   return(getReturnCode());
} // showModal

void cxButton::setOnClickFunction(funcPtr4 pFunction, void *p1, void *p2,
                                  void *p3, void *p4)
                                  {
   mOnClickFunction.reset();
   mOnClickFunction = make_shared<cxFunction4>(pFunction, p1, p2, p3, p4, false, false,
                                      false);
} // setOnClickFunction

void cxButton::setOnClickFunction(funcPtr2 pFunction, void *p1, void *p2)
{
   mOnClickFunction.reset();
   mOnClickFunction = make_shared<cxFunction2>(pFunction, p1, p2, false, false, false);
} // setOnClickFunction

void cxButton::setOnClickFunction(funcPtr0 pFunction)
{
   mOnClickFunction.reset();
   mOnClickFunction = make_shared<cxFunction0>(pFunction, false, false, false);
} // setOnClickFunction

string cxButton::runOnClickFunction()
{
   string retval;

   // If the onClick function is set, run it.
   if (mOnClickFunction != nullptr)
   {
      if (mOnClickFunction->functionIsSet())
      {
         retval = mOnClickFunction->runFunction();
      }
   }

   return(retval);
} // runOnClickFunction

void cxButton::doMouseBehavior()
{
#ifdef NCURSES_MOUSE_VERSION
   // If mouse button 1 was clicked inside the window, then run the onClick
   //  function, if it's set.
   if (mouseEvtWasInWindow() && mouseButton1Clicked())
   {
      runOnClickFunction();
   }
#endif
} // doMouseBehavior

///////////////////////
// Private functions //
///////////////////////

void cxButton::copyOnClickFunction(const cxButton& pButton)
{
   mOnClickFunction.reset();
   const cxFunction *rawPtr = pButton.mOnClickFunction.get();
   if (rawPtr != nullptr)
   {
      const string funcType = pButton.mOnClickFunction->cxTypeStr();
      if (funcType == "cxFunction0")
      {
         try
         {
            const cxFunction0* func0 = dynamic_cast<const cxFunction0*>(rawPtr);
            if (func0 != nullptr)
            {
               mOnClickFunction = make_shared<cxFunction0>(func0->getFunction(),
                                              func0->getUseReturnVal(),
                                              func0->getExitAfterRun(),
                                              func0->getRunOnLeaveFunction());
            }
         }
         catch (...)
         {
         }
      }
      else if (funcType == "cxFunction2")
      {
         try
         {
            const cxFunction2* func2 = dynamic_cast<const cxFunction2*>(rawPtr);
            if (func2 != nullptr)
            {
               void* params[] = { func2->getParam1(), func2->getParam2() };
               for (int i = 0; i < 2; ++i)
               {
                  if (params[i] == (void*)(&pButton))
                  {
                     params[i] = (void*)this;
                  }
               }
               mOnClickFunction = make_shared<cxFunction2>(func2->getFunction(),
                                              params[0], params[1],
                                              func2->getUseReturnVal(),
                                              func2->getExitAfterRun(),
                                              func2->getRunOnLeaveFunction());
            }
         }
         catch (...)
         {
         }
      }
      else if (funcType == "cxFunction4")
      {
         try
         {
            const cxFunction4* func4 = dynamic_cast<const cxFunction4*>(rawPtr);
            if (func4 != nullptr)
            {
               void* params[] = { func4->getParam1(), func4->getParam2(),
                                  func4->getParam3(), func4->getParam4() };
               for (int i = 0; i < 4; ++i)
               {
                  if (params[i] == (void*)(&pButton))
                  {
                     params[i] = (void*)this;
                  }
               }
               mOnClickFunction = make_shared<cxFunction4>(func4->getFunction(),
                                              params[0], params[1], params[2],
                                              params[3],
                                              func4->getUseReturnVal(),
                                              func4->getExitAfterRun(),
                                              func4->getRunOnLeaveFunction());
            }
         }
         catch (...)
         {
         }
      }
   }
} // copyOnClickFunction
