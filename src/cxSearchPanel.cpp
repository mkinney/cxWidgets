// Copyright (c) 2006-2007 Michael H. Kinney

#include "cxSearchPanel.h"
#include <memory>
using std::string;
using std::map;
using std::shared_ptr;
using std::make_shared;
using std::dynamic_pointer_cast;

cxSearchPanel::cxSearchPanel(cxWindow *pParentWindow, int pRow, int pCol,
                 int pHeight, int pWidth, const string& pFormTitle,
                 const string& pMenuTitle, eBorderStyle pFormBorderStyle,
                 eBorderStyle pMenuBorderStyle)
   : cxPanel(pParentWindow, pRow, pCol, pHeight, pWidth, "", "", "",
             eBS_NOBORDER, nullptr, nullptr, false)
{
   // As a screen optimization, don't show the panel window when showing the
   //  panel, since the cxForm and cxMenu will completely cover the panel
   //  anyway.
   setShowPanelWindow(false);

   // Exit when the user leaves the last window (which will be the menu).
   //setExitOnLeaveLast(true);
   // Make sure we don't exit when the user leaves the last window
   setExitOnLeaveLast(false);

   // Create the form with one line for the input.  If there are form borders,
   //  make the form be 3 lines tall; if there are no form borders, make the
   //  form be one line tall.
   int winHeight = 3;
   if (pFormBorderStyle == eBS_NOBORDER)
   {
      winHeight = 1;
   }
   int winWidth = width();
   mForm = make_shared<cxForm>(nullptr, 0, 0, winHeight, winWidth, pFormTitle,
                               pFormBorderStyle, nullptr, nullptr, true, false);
   // Have the enter key exit the form (to go onto the menu).
   mForm->addExitKey(ENTER, true, true);
   mForm->addExitKey(KEY_ENTER, true, true);
   // Set up formReverseNavigation()
   mForm->setKeyFunction(KEY_UP, cxSearchPanel::formReverseNavigation, this, nullptr, false,
                         false, true);
   mForm->setKeyFunction(SHIFT_TAB, cxSearchPanel::formReverseNavigation, this, nullptr, false,
                         false, true);
   append(mForm, 0, 0, false);

   // Create the cxMenu with the remaining height of the panel, and append it
   //  below the form
   winHeight = height() - winHeight;
   mMenu = make_shared<cxMenu>(nullptr, 0, 0, winHeight, winWidth, pMenuTitle, nullptr, nullptr,
                               pMenuBorderStyle);
   mMenu->setOnSelectItemFunction(cxSearchPanel::menuOnSelect, this, nullptr, true,
                                  true);
   // Have the menu refresh its item list when it gets focus.
   mMenu->setRefreshItemsWhenModal(true);
   // Have the menu exit focus when the user presses the up arrow on the first
   //  item.
   mMenu->setExitWhenLeaveFirst(true);
   int row = mForm->height();
   append(mMenu, row, 0, false);

   // ESC on the menu
   mMenu->setKeyFunction(ESCAPE, cxSearchPanel::menuOnESC, this, nullptr, false,
                         false, false);
   mMenu->setKeyFunction(ESC, cxSearchPanel::menuOnESC, this, nullptr, false,
                         false, false);
} // constructor

cxSearchPanel::cxSearchPanel(const cxSearchPanel& pPanel)
   : cxPanel(nullptr, pPanel.top(), pPanel.left(), pPanel.height(),
             pPanel.width(), pPanel.getTitle(), pPanel.getMessage(),
             pPanel.getStatus(), pPanel.getBorderStyle(),
             pPanel.getExtTitleWindow(), pPanel.getExtStatusWindow(),
             pPanel.getHotkeyHighlighting())
{
   mForm = make_shared<cxForm>(*(pPanel.mForm));
   mMenu = make_shared<cxMenu>(*(pPanel.mMenu));

   // Set up formReverseNavigation() for the form and append the form
   mForm->setKeyFunction(KEY_UP, cxSearchPanel::formReverseNavigation, this, nullptr, false,
                         false, true);
   mForm->setKeyFunction(SHIFT_TAB, cxSearchPanel::formReverseNavigation, this, nullptr, false,
                         false, true);
   // Have the enter key exit the form (to go onto the menu).
   mForm->addExitKey(ENTER, true, true);
   mForm->addExitKey(KEY_ENTER, true, true);
   append(mForm, 0, 0, false);
   // Append the menu
   int row = mForm->height();
   append(mMenu, row, 0, false);

   // Copy & check the event function pointers.  If any of their parameters
   //  point to the other cxSearchPanel's stuff, make them point to this one's
   //  stuff.
   checkEventFunctionPointers(pPanel);

   // ESC on the menu
   mMenu->setKeyFunction(ESCAPE, cxSearchPanel::menuOnESC, this, nullptr, false,
                         false, false);
   mMenu->setKeyFunction(ESC, cxSearchPanel::menuOnESC, this, nullptr, false,
                         false, false);
} // copy constructor

cxSearchPanel::~cxSearchPanel()
{
} // destructor

const std::shared_ptr<cxForm>& cxSearchPanel::getForm() const
{
   return(mForm);
} // getForm

const std::shared_ptr<cxMenu>& cxSearchPanel::getMenu() const
{
   return(mMenu);
} // getMenu

shared_ptr<cxMultiLineInput> cxSearchPanel::appendToForm(int pRow, int pCol, int pHeight,
        int pWidth, const string& pLabel, const string& pValidator,
        const string& pHelpString, eInputOptions pInputKind, const string& pName,
        string *pExtValue)
        {
   return(mForm->append(pRow, pCol, pHeight, pWidth, pLabel, pValidator,
                        pHelpString, pInputKind, pName, pExtValue));
} // appendToForm

shared_ptr<cxComboBox> cxSearchPanel::appendComboBoxToForm(int pRow, int pCol, int pHeight,
        int pWidth, const string& pLabel, const string& pValidator,
        const string& pHelpString, eInputOptions pInputKind, const string& pName,
        string *pExtValue)
        {
   return(mForm->appendComboBox(pRow, pCol, pHeight, pWidth, pLabel,
                                pValidator, pHelpString, pInputKind, pName,
                                pExtValue));
} // appendComboBoxToForm

void cxSearchPanel::appendToMenu(const string& pDisplayText, long pReturnCode,
        const string& pHelpString, const string& pItemText,
        cxMenuItemType pType)
        {
   mMenu->append(pDisplayText, pReturnCode, pHelpString, pType, false, pItemText);
} // appendToMenu

void cxSearchPanel::appendToMenu(const string& pItem, const string& pHelpString,
                          const string& pItemText, cxMenuItemType pType)
                          {
   // Get the next available return code to be used for the item
   long returnCode = mMenu->getHighestReturnCode() + 1;
   // Append the menu item
   mMenu->append(pItem, returnCode, pHelpString, pType, false, pItemText);
} // appendToMenu

void cxSearchPanel::clearFormInputs(bool pRefresh, bool pOnlyEditable)
{
   mForm->clearInputs(pRefresh, pOnlyEditable);
} // clearFormInputs

void cxSearchPanel::clearMenu(bool pRefresh)
{
   mMenu->clear(pRefresh);
} // clearMenu

void cxSearchPanel::setFormOnFocusFunction(funcPtr4 pFuncPtr, void *p1, void *p2,
                        void *p3, void *p4, bool pUseVal, bool pExitAfterRun)
                        {
   mForm->setOnFocusFunction(pFuncPtr, p1, p2, p3, p4, pUseVal, pExitAfterRun);
} // setFormOnFocusFunction

void cxSearchPanel::setFormOnFocusFunction(funcPtr2 pFuncPtr, void *p1, void *p2,
                        bool pUseVal, bool pExitAfterRun)
                        {
   mForm->setOnFocusFunction(pFuncPtr, p1, p2, pUseVal, pExitAfterRun);
} // setFormOnFocusFunction

void cxSearchPanel::setFormOnFocusFunction(funcPtr0 pFuncPtr, bool pUseVal,
                                           bool pExitAfterRun)
                                           {
   mForm->setOnFocusFunction(pFuncPtr, pUseVal, pExitAfterRun);
} // setFormOnFocusFunction

void cxSearchPanel::setFormOnLeaveFunction(funcPtr4 pFuncPtr, void *p1, void *p2,
                           void *p3, void *p4)
                           {
   mForm->setOnLeaveFunction(pFuncPtr, p1, p2, p3, p4);
} // setFormOnLeaveFunction

void cxSearchPanel::setFormOnLeaveFunction(funcPtr2 pFuncPtr, void *p1, void *p2)
{
   mForm->setOnLeaveFunction(pFuncPtr, p1, p2);
} // setFormOnLeaveFunction

void cxSearchPanel::setFormOnLeaveFunction(funcPtr0 pFuncPtr)
{
   mForm->setOnLeaveFunction(pFuncPtr);
} // setFormOnLeaveFunction

void cxSearchPanel::setMenuOnFocusFunction(funcPtr4 pFunction, void *p1, void *p2,
                      void *p3, void *p4, bool pUseVal, bool pExitAfterRun)
                      {
   mMenu->setOnFocusFunction(pFunction, p1, p2, p3, p4, pUseVal, pExitAfterRun);
} // setMenuOnFocusFunction

void cxSearchPanel::setMenuOnFocusFunction(funcPtr2 pFunction, void *p1, void *p2,
                                           bool pUseVal, bool pExitAfterRun)
                                           {
   mMenu->setOnFocusFunction(pFunction, p1, p2, pUseVal, pExitAfterRun);
} // setMenuOnFocusFunction

void cxSearchPanel::setMenuOnFocusFunction(funcPtr0 pFunction, bool pUseVal,
                                           bool pExitAfterRun)
                                           {
   mMenu->setOnFocusFunction(pFunction, pUseVal, pExitAfterRun);
} // setMenuOnFocusFunction

void cxSearchPanel::setMenuOnLeaveFunction(funcPtr4 pFunction, void *p1, void *p2,
                      void *p3, void *p4)
                      {
   mMenu->setOnLeaveFunction(pFunction, p1, p2, p3, p4);
} // setMenuOnLeaveFunction

void cxSearchPanel::setMenuOnLeaveFunction(funcPtr2 pFunction, void *p1, void *p2)
{
   mMenu->setOnLeaveFunction(pFunction, p1, p2);
} // setMenuOnLeaveFunction

void cxSearchPanel::setMenuOnLeaveFunction(funcPtr0 pFunction)
{
   mMenu->setOnLeaveFunction(pFunction);
} // setMenuOnLeaveFunction

void cxSearchPanel::resize(int pNewHeight, int pNewWidth, bool pRefresh)
{
   if ((pNewHeight != height()) || (pNewWidth != width()))
   {
      // Resize the panel window
      cxWindow::resize(pNewHeight, pNewWidth, pRefresh);
      // Resize the form with 1/4 of the panel height, and resize the menu to 3/4
      //  of the panel height.  We'll also need to move the menu so that it's
      //  still directly beneath the form.  The form always stays at position
      //  (0,0) relative to the panel.
      int winHeight = height() / 4;
      mForm->resize(winHeight, width(), pRefresh);
      winHeight = height() - winHeight;
      mMenu->resize(winHeight, width(), false);
      // Move the menu so that it's directly beneath the form
      mMenu->move(mForm->top()+mForm->height(), mMenu->left(), pRefresh);
   }
} // resize

void cxSearchPanel::setFormHeight(int pNewHeight, bool pRefresh)
{
   // Make sure pNewHeight is more than 0 and less than the panel height
   if ((pNewHeight > 0) && (pNewHeight < height()))
   {
      mForm->resize(pNewHeight, mForm->width(), pRefresh);
      // Resize the menu
      pNewHeight = height() - pNewHeight;
      mMenu->resize(pNewHeight, mMenu->width(), false);
      // Move the menu so that it's directly beneath the form
      mMenu->move(mForm->top()+mForm->height(), mMenu->left(), pRefresh);
   }
} // setFormHeight

void cxSearchPanel::setMenuHeight(int pNewHeight, bool pRefresh)
{
   // Make sure pNewHeight is more than 0 and less than the panel height
   if ((pNewHeight > 0) && (pNewHeight < height()))
   {
      mMenu->resize(pNewHeight, mMenu->width(), false);
      // Resize the form correspondingly
      pNewHeight = height() - pNewHeight;
      mForm->resize(pNewHeight, mForm->width(), pRefresh);
      // Move the menu so that it's directly beneath the form
      mMenu->move(mForm->top()+mForm->height(), mMenu->left(), pRefresh);
   }
} // setMenuHeight

string cxSearchPanel::cxTypeStr() const
{
   return("cxSearchPanel");
} // cxTypeStr

bool cxSearchPanel::setFormOnEnterFunction(funcPtr4 pFunction, void *p1,
                                           void *p2, void *p3, void *p4)
                                           {
   bool retval = false;

   if (mForm != nullptr)
   {
      retval = mForm->setKeyFunction(KEY_ENTER, pFunction, p1, p2, p3, p4,
                                     false, true, true) &&
               mForm->setKeyFunction(ENTER, pFunction, p1, p2, p3, p4, false,
                                     true, true);
   }

   return(retval);
} // setFormOnEnterFunction

bool cxSearchPanel::setFormOnEnterFunction(funcPtr2 pFunction, void *p1,
                                           void *p2)
                                           {
   bool retval = false;

   if (mForm != nullptr)
   {
      retval = mForm->setKeyFunction(KEY_ENTER, pFunction, p1, p2, false, true,
                                     true) &&
               mForm->setKeyFunction(ENTER, pFunction, p1, p2, false, true,
                                     true);
   }

   return(retval);
} // setFormOnEnterFunction

bool cxSearchPanel::setFormOnEnterFunction(funcPtr0 pFunction)
{
   bool retval = false;

   if (mForm != nullptr)
   {
      retval = mForm->setKeyFunction(KEY_ENTER, pFunction, false, true, true)
               && mForm->setKeyFunction(ENTER, pFunction, false, true, true);
   }

   return(retval);
} // setFormOnEnterFunction

void cxSearchPanel::clearFormOnEnterFunction()
{
   if (mForm != nullptr)
   {
      mForm->clearKeyFunction(KEY_ENTER);
      mForm->clearKeyFunction(ENTER);
      // Make sure the form still exits focus when the user presses the
      //  enter key.
      mForm->addExitKey(ENTER, true, true);
      mForm->addExitKey(KEY_ENTER, true, true);
   }
} // clearFormOnEnterFunction

bool cxSearchPanel::setFormKeyFunction(int pFunctionKey, funcPtr4 pFunction,
                                       void *p1, void *p2, void *p3, void *p4,
                                       bool pUseReturnVal,
                                       bool pExitAfterRun,
                                       bool pRunOnLeaveFunction)
                                       {
   bool retval = false;

   if (mForm != nullptr)
   {
      retval = mForm->setKeyFunction(pFunctionKey, pFunction, p1, p2, p3, p4,
                                     false, true, true);
   }

   return(retval);
} // setFormKeyFunction

bool cxSearchPanel::setFormKeyFunction(int pFunctionKey, funcPtr2 pFunction,
                                       void *p1, void *p2,
                                       bool pUseReturnVal,
                                       bool pExitAfterRun,
                                       bool pRunOnLeaveFunction)
                                       {
   bool retval = false;

   if (mForm != nullptr)
   {
      retval = mForm->setKeyFunction(pFunctionKey, pFunction, p1, p2, false, true,
                                     true);
   }

   return(retval);
} // setFormKeyFunction

bool cxSearchPanel::setFormKeyFunction(int pFunctionKey, funcPtr0 pFunction,
                                       bool pUseReturnVal,
                                       bool pExitAfterRun,
                                       bool pRunOnLeaveFunction)
                                       {
   bool retval = false;

   if (mForm != nullptr)
   {
      retval = mForm->setKeyFunction(pFunctionKey, pFunction, false, true,
                                     true);
   }

   return(retval);
} // setFormKeyFunction

void cxSearchPanel::clearFormKeyFunction(int pFunctionKey)
{
   if (mForm != nullptr)
   {
      mForm->clearKeyFunction(pFunctionKey);
   }
} // clearFormKeyFunction

void cxSearchPanel::clearFormKeyFunctions()
{
   if (mForm != nullptr)
   {
      mForm->clearKeyFunctions();
   }
} // clearFormKeyFunctions

bool cxSearchPanel::formHasKeyFunction(int pFunctionKey) const
{
   bool retval = false;

   if (mForm != nullptr)
   {
      retval = mForm->hasKeyFunction(pFunctionKey);
   }

   return(retval);
} // formHasKeyFunction

bool cxSearchPanel::setMenuKeyFunction(int pFunctionKey, funcPtr4 pFunction,
                                       void *p1, void *p2, void *p3, void *p4,
                                       bool pUseReturnVal, bool pExitAfterRun,
                                       bool pRunOnLeaveFunction)
                                       {
   bool retval = false;

   if (mMenu != nullptr)
   {
      retval = mMenu->setKeyFunction(pFunctionKey, pFunction, p1, p2, p3, p4,
                                     false, true, true);
   }

   return(retval);
} // setMenuKeyFunction

bool cxSearchPanel::setMenuKeyFunction(int pFunctionKey, funcPtr2 pFunction,
                                       void *p1, void *p2, bool pUseReturnVal,
                                       bool pExitAfterRun,
                                       bool pRunOnLeaveFunction)
                                       {
   bool retval = false;

   if (mMenu != nullptr)
   {
      retval = mMenu->setKeyFunction(pFunctionKey, pFunction, p1, p2, false, true,
                                     true);
   }

   return(retval);
} // setMenuKeyFunction

bool cxSearchPanel::setMenuKeyFunction(int pFunctionKey, funcPtr0 pFunction,
                                       bool pUseReturnVal, bool pExitAfterRun,
                                       bool pRunOnLeaveFunction)
                                       {
   bool retval = false;

   if (mMenu != nullptr)
   {
      retval = mMenu->setKeyFunction(pFunctionKey, pFunction, false, true,
                                     true);
   }

   return(retval);
} // setMenuKeyFunction

void cxSearchPanel::clearMenuKeyFunction(int pFunctionKey)
{
   if (mMenu != nullptr)
   {
      mMenu->clearKeyFunction(pFunctionKey);
   }
} // clearMenuKeyFunction

void cxSearchPanel::clearMenuKeyFunctions()
{
   if (mMenu != nullptr)
   {
      mMenu->clearKeyFunctions();
   }
} // clearMenuKeyFunctions

bool cxSearchPanel::menuHasKeyFunction(int pFunctionKey) const
{
   bool retval = false;

   if (mMenu != nullptr)
   {
      retval = mMenu->hasKeyFunction(pFunctionKey);
   }

   return(retval);
} // menuHasKeyFunction

bool cxSearchPanel::setMenuOnSelectItemFunction(funcPtr4 pFunction, void *p1,
                                                void *p2, void *p3, void *p4,
                                                bool pExitAfterRun,
                                                bool pRunOnLeaveFunction)
                                                {
   bool funcWasSet = false;

   if (mMenu != nullptr)
   {
      funcWasSet = mMenu->setOnSelectItemFunction(pFunction, p1, p2, p3, p4,
                                     pExitAfterRun, pRunOnLeaveFunction);
   }

   return(funcWasSet);
} // setMenuOnSelectItemFunction

bool cxSearchPanel::setMenuOnSelectItemFunction(funcPtr2 pFunction, void *p1,
                                                void *p2, bool pExitAfterRun,
                                                bool pRunOnLeaveFunction)
                                                {
   bool funcWasSet = false;

   if (mMenu != nullptr)
   {
      funcWasSet = mMenu->setOnSelectItemFunction(pFunction, p1, p2,
                                         pExitAfterRun, pRunOnLeaveFunction);
   }

   return(funcWasSet);
} // setMenuOnSelectItemFunction

bool cxSearchPanel::setMenuOnSelectItemFunction(funcPtr0 pFunction,
                                                bool pExitAfterRun,
                                                bool pRunOnLeaveFunction)
                                                {
   bool funcWasSet = false;

   if (mMenu != nullptr)
   {
      funcWasSet = mMenu->setOnSelectItemFunction(pFunction, pExitAfterRun,
                                                  pRunOnLeaveFunction);
   }

   return(funcWasSet);
} // setMenuOnSelectItemFunction

bool cxSearchPanel::itemWasSelected() const
{
   bool itemSelected = false;

   if (mMenu != nullptr)
   {
      itemSelected = mMenu->itemWasSelected();
   }

   return(itemSelected);
} // itemWasSelected

string cxSearchPanel::getSelectedItem(bool pDisplayText) const
{
   string retval;

   if (mMenu != nullptr)
   {
      if (mMenu->itemWasSelected())
      {
         retval = mMenu->getCurrentItemText(pDisplayText);
      }
   }

   return(retval);
} // getSelectedItem

bool cxSearchPanel::setFormCurrent()
{
   return(setCurrentWindow(0));
} // setFormCurrent

bool cxSearchPanel::setMenuCurrent()
{
   return(setCurrentWindow(1));
} // setMenuCurrent

//// Protected functions

void cxSearchPanel::copyCxSearchPanelStuff(const cxSearchPanel* pThatPanel)
{
   if ((pThatPanel != nullptr) && (pThatPanel != this))
   {
      // Copy the cxPanel stuff inherited from the parent, then copy this
      //  class' stuff.  (Note: cxPanel doesn't have a copyCxPanelStuff(),
      //  so we're calling copyCxWinStuff()).
      try
      {
         copyCxWinStuff((const cxWindow*)pThatPanel);
      }
      catch (const cxWidgetsException& e)
      {
         throw(cxWidgetsException("Couldn't copy base cxWindow stuff (copying a cxSearchPanel)."));
      }

      // Re-size the panel & set other parameters to match the other panel
      resize(pThatPanel->height(), pThatPanel->width(), false);
      // Note: Purposefully not changing the parent window pointer.  Otherwise,
      //  segfaults could ensue.
      //setParent(pThatPanel->getParent());
      move(pThatPanel->top(), pThatPanel->left(), false);
      setTitle(pThatPanel->getTitle());
      setMessage(pThatPanel->getMessage());
      setStatus(pThatPanel->getStatus());
      setBorderStyle(pThatPanel->getBorderStyle());
      setExtTitleWindow(pThatPanel->getExtTitleWindow());
      setExtStatusWindow(pThatPanel->getExtStatusWindow());
      setHotkeyHighlighting(pThatPanel->getHotkeyHighlighting());
      // Copy the menu & form
      *mForm = *(pThatPanel->mForm);
      *mMenu = *(pThatPanel->mMenu);
      // Copy & check the event function pointers: If any of their parameters
      //  point to the other cxSearchPanel's stuff, make them point to this
      //  one's stuff.
      checkEventFunctionPointers(*pThatPanel);
   }
} // copyCxSearchPanelStuff

//// Private functions

cxSearchPanel& cxSearchPanel::operator =(const cxSearchPanel& pPanel)
{
   copyCxSearchPanelStuff(&pPanel);

   return(*this);
} // operator =

void cxSearchPanel::checkEventFunctionPointers(const cxSearchPanel& pPanel)
{
   // Set the keypress functions and onFocus and onLeave functions
   setKeyFunctions(pPanel);
   setFocusFunctions(pPanel);

   // Look at the onFocus and onLeave function for the form.  If any of the
   //  parameters point to the other searchPanel or its form or menu, then
   //  have them use this one instead.
   shared_ptr<cxFunction> onFocusFunc = mForm->getOnFocusFunction();
   if (onFocusFunc != nullptr)
   {
      if (onFocusFunc->cxTypeStr() == "cxFunction2")
      {
         shared_ptr<cxFunction2> iFunc2 = std::dynamic_pointer_cast<cxFunction2>(onFocusFunc);
         if (iFunc2 != nullptr)
         {
            void* params[] = { iFunc2->getParam1(), iFunc2->getParam2() };
            for (int i = 0; i < 2; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mForm->setOnFocusFunction(iFunc2->getFunction(), params[0],
                                    params[1], iFunc2->getUseReturnVal(),
                                    iFunc2->getExitAfterRun());
         }
      }
      else if (onFocusFunc->cxTypeStr() == "cxFunction4")
      {
         shared_ptr<cxFunction4> iFunc4 = std::dynamic_pointer_cast<cxFunction4>(onFocusFunc);
         if (iFunc4 != nullptr)
         {
            void* params[] = { iFunc4->getParam1(), iFunc4->getParam2(),
                               iFunc4->getParam3(), iFunc4->getParam4() };
            for (int i = 0; i < 4; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mForm->setOnFocusFunction(iFunc4->getFunction(), params[0],
                        params[1], params[2], params[3],
                        iFunc4->getUseReturnVal(), iFunc4->getExitAfterRun());
         }
      }
   }
   shared_ptr<cxFunction> onLeaveFunc = mForm->getOnLeaveFunction();
   if (onLeaveFunc != nullptr)
   {
      if (onLeaveFunc->cxTypeStr() == "cxFunction2")
      {
         shared_ptr<cxFunction2> iFunc2 = std::dynamic_pointer_cast<cxFunction2>(onLeaveFunc);
         if (iFunc2 != nullptr)
         {
            void* params[] = { iFunc2->getParam1(), iFunc2->getParam2() };
            for (int i = 0; i < 2; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mForm->setOnLeaveFunction(iFunc2->getFunction(), params[0],
                                      params[1]);
         }
      }
      else if (onLeaveFunc->cxTypeStr() == "cxFunction4")
      {
         shared_ptr<cxFunction4> iFunc4 = std::dynamic_pointer_cast<cxFunction4>(onLeaveFunc);
         if (iFunc4 != nullptr)
         {
            void* params[] = { iFunc4->getParam1(), iFunc4->getParam2(),
                               iFunc4->getParam3(), iFunc4->getParam4() };
            for (int i = 0; i < 4; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mForm->setOnLeaveFunction(iFunc4->getFunction(), params[0],
                                       params[1], params[2], params[3]);
         }
      }
   }

   // Do the same for the menu's onFocus and onLeave functions
   onFocusFunc = mMenu->getOnFocusFunction();
   if (onFocusFunc != nullptr)
   {
      if (onFocusFunc->cxTypeStr() == "cxFunction2")
      {
         shared_ptr<cxFunction2> iFunc2 = std::dynamic_pointer_cast<cxFunction2>(onFocusFunc);
         if (iFunc2 != nullptr)
         {
            void* params[] = { iFunc2->getParam1(), iFunc2->getParam2() };
            for (int i = 0; i < 2; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mMenu->setOnFocusFunction(iFunc2->getFunction(), params[0],
                        params[1],
                        iFunc2->getUseReturnVal(), iFunc2->getExitAfterRun());
         }
      }
      else if (onFocusFunc->cxTypeStr() == "cxFunction4")
      {
         shared_ptr<cxFunction4> iFunc4 = std::dynamic_pointer_cast<cxFunction4>(onFocusFunc);
         if (iFunc4 != nullptr)
         {
            void* params[] = { iFunc4->getParam1(), iFunc4->getParam2(),
                               iFunc4->getParam3(), iFunc4->getParam4() };
            for (int i = 0; i < 4; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mMenu->setOnFocusFunction(iFunc4->getFunction(), params[0],
                  params[1], params[2], params[3],
                  iFunc4->getUseReturnVal(), iFunc4->getExitAfterRun());
         }
      }
   }
   onLeaveFunc = mMenu->getOnLeaveFunction();
   if (onLeaveFunc != nullptr)
   {
      if (onLeaveFunc->cxTypeStr() == "cxFunction2")
      {
         shared_ptr<cxFunction2> iFunc2 = std::dynamic_pointer_cast<cxFunction2>(onLeaveFunc);
         if (iFunc2 != nullptr)
         {
            void* params[] = { iFunc2->getParam1(), iFunc2->getParam2() };
            for (int i = 0; i < 2; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mMenu->setOnLeaveFunction(iFunc2->getFunction(), params[0],
                                      params[1]);
         }
      }
      else if (onLeaveFunc->cxTypeStr() == "cxFunction4")
      {
         shared_ptr<cxFunction4> iFunc4 = std::dynamic_pointer_cast<cxFunction4>(onLeaveFunc);
         if (iFunc4 != nullptr)
         {
            void* params[] = { iFunc4->getParam1(), iFunc4->getParam2(),
                               iFunc4->getParam3(), iFunc4->getParam4() };
            for (int i = 0; i < 4; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mMenu->setOnLeaveFunction(iFunc4->getFunction(), params[0],
                                       params[1], params[2], params[3]);
         }
      }
   }

   // Same with the menu's onSelectMenuItem function
   shared_ptr<cxFunction> onSelectItemFunc = mMenu->getOnSelectItemFunction();
   if (onSelectItemFunc->cxTypeStr() == "cxFunction2")
   {
      const cxFunction2 *iFunc2 = dynamic_cast<cxFunction2*>(onSelectItemFunc.get());
      if (iFunc2 != nullptr)
      {
         void* params[] = { iFunc2->getParam1(), iFunc2->getParam2() };
         for (int i = 0; i < 2; ++i)
         {
            if (params[i] == (void*)(&pPanel))
            {
               params[i] = (void*)this;
            }
         }
         mMenu->setOnSelectItemFunction(iFunc2->getFunction(), params[0],
                                        params[1], iFunc2->getExitAfterRun(),
                                        iFunc2->getRunOnLeaveFunction());
      }
   }
   else if (onSelectItemFunc->cxTypeStr() == "cxFunction2")
   {
      const cxFunction4 *iFunc4 = dynamic_cast<cxFunction4*>(onSelectItemFunc.get());
      if (iFunc4 != nullptr)
      {
         void* params[] = { iFunc4->getParam1(), iFunc4->getParam2(),
                            iFunc4->getParam3(), iFunc4->getParam4() };
         for (int i = 0; i < 4; ++i)
         {
            if (params[i] == (void*)(&pPanel))
            {
               params[i] = (void*)this;
            }
         }
         mMenu->setOnSelectItemFunction(iFunc4->getFunction(), params[0],
                                        params[1], params[2], params[3],
                                        iFunc4->getExitAfterRun(),
                                        iFunc4->getRunOnLeaveFunction());
      }
   }

   // Same with the form & menu key functions
   map<int, shared_ptr<cxFunction> >::iterator funcIter = mForm->keyFunctions_begin();
   for (; funcIter != mForm->keyFunctions_end(); ++funcIter)
   {
      // We'll need to check 2 parameters if it's a cxFunction2, or 4
      // parameters if it's a cxFunction4.
      shared_ptr<cxFunction4> iFunction4 = getKeyFunctionAsFunction4(funcIter->first);
      if (iFunction4 != nullptr)
      {
         void* params[] = { iFunction4->getParam1(), iFunction4->getParam2(),
                            iFunction4->getParam3(), iFunction4->getParam4() };
         for (int i = 0; i < 4; ++i)
         {
            if (params[i] == (void*)(&pPanel))
            {
               params[i] = (void*)this;
            }
         }
         mForm->setKeyFunction(funcIter->first, iFunction4->getFunction(),
               params[0], params[1], params[2], params[3],
               iFunction4->getUseReturnVal(), iFunction4->getExitAfterRun(),
               iFunction4->getRunOnLeaveFunction());
      }
      else
      {
         shared_ptr<cxFunction2> iFunction2 = getKeyFunctionAsFunction2(funcIter->first);
         if (iFunction2 != nullptr)
         {
            void* params[] = { iFunction2->getParam1(), iFunction2->getParam2() };
            for (int i = 0; i < 2; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mForm->setKeyFunction(funcIter->first, iFunction2->getFunction(),
                           params[0], params[1], iFunction2->getUseReturnVal(),
                           iFunction2->getExitAfterRun(),
                           iFunction2->getRunOnLeaveFunction());
         }
      }
   }
   funcIter = mMenu->keyFunctions_begin();
   for (; funcIter != mMenu->keyFunctions_end(); ++funcIter)
   {
      // We'll need to check 2 parameters if it's a cxFunction2, or 4
      // parameters if it's a cxFunction4.
      shared_ptr<cxFunction4> iFunction4 = getKeyFunctionAsFunction4(funcIter->first);
      if (iFunction4 != nullptr)
      {
         void* params[] = { iFunction4->getParam1(), iFunction4->getParam2(),
                            iFunction4->getParam3(), iFunction4->getParam4() };
         for (int i = 0; i < 4; ++i)
         {
            if (params[i] == (void*)(&pPanel))
            {
               params[i] = (void*)this;
            }
         }
         mMenu->setKeyFunction(funcIter->first, iFunction4->getFunction(),
               params[0], params[1], params[2], params[3],
               iFunction4->getUseReturnVal(), iFunction4->getExitAfterRun(),
               iFunction4->getRunOnLeaveFunction());
      }
      else
      {
         shared_ptr<cxFunction2> iFunction2 = getKeyFunctionAsFunction2(funcIter->first);
         if (iFunction2 != nullptr)
         {
            void* params[] = { iFunction2->getParam1(), iFunction2->getParam2() };
            for (int i = 0; i < 2; ++i)
            {
               if (params[i] == (void*)(&pPanel))
               {
                  params[i] = (void*)this;
               }
            }
            mMenu->setKeyFunction(funcIter->first, iFunction2->getFunction(),
                           params[0], params[2], iFunction2->getUseReturnVal(),
                           iFunction2->getExitAfterRun(),
                           iFunction2->getRunOnLeaveFunction());
         }
      }
   }
} // checkEventFunctionPointers

string cxSearchPanel::formReverseNavigation(void *theSearchPanel, void *unused)
{
   if (theSearchPanel == nullptr) { return(""); }

   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   shared_ptr<cxForm> mForm = pSearchPanel->mForm;
   if (mForm->numInputs() > 0)
   {
      // If they're on the first input, go to the last input.  Otherwise, go to
      //  the previous input.
      int curInputIndex = mForm->getCurrentInputIndex();
      if (curInputIndex == 0)
      {
         mForm->setCurrentInput((int)(mForm->numInputs())-1);
      }
      else
      {
         mForm->setCurrentInput(curInputIndex-1);
      }
   }

   return("");
} // formReverseNavigation

string cxSearchPanel::menuOnESC(void *theSearchPanel, void *unused)
{
   if (theSearchPanel == nullptr) { return(""); }

   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   shared_ptr<cxForm> mForm = pSearchPanel->mForm;
   shared_ptr<cxMenu> mMenu = pSearchPanel->mMenu;

   // clear form fields
   mForm->clearInputs(true);

   // clear menu
   mMenu->clear(true);

   // Set the current window to the form
   pSearchPanel->setCurrentWindow(0);

   // go to first input field
   mForm->setCurrentInput(0);

   return("");
} // menuOnESC

string cxSearchPanel::menuOnSelect(void *theSearchPanel, void *unused)
{
   if (theSearchPanel == nullptr) { return(""); }

   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   // Tell the search panel to exit now
   pSearchPanel->exitNow();
   // Manually set the menu's return code to the current item's return code.
   //  (This is because exitNow() will set its return code to cxID_EXIT.)
   if (pSearchPanel->mMenu != nullptr)
   {
      long itemReturnCode = pSearchPanel->mMenu->getCurrentItemReturnCode();
      pSearchPanel->mMenu->setReturnCode(itemReturnCode);
   }

   return("");
} // menuOnSelect
