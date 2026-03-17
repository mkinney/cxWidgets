// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxMultiForm.h"
#include "cxBase.h"
#include <map>
#include <utility>
using std::string;
using std::vector;
using std::map;
using std::pair;
using std::shared_ptr;
using std::make_shared;

cxMultiForm::cxMultiForm(cxWindow *pParentWindow, int pRow,
                         int pCol, int pHeight,
                         int pWidth, const string& pTitle,
                         eBorderStyle pBorderStyle,
                         cxWindow *pExtTitleWindow,
                         cxWindow *pExtStatusWindow, bool pAutoExit,
                         bool pStacked)
   : cxForm(pParentWindow, pRow, pCol, pHeight, pWidth, pTitle,
            pBorderStyle, pExtTitleWindow, pExtStatusWindow,
            pAutoExit, pStacked)
{
   setExitOnLeaveFirst(true);
}

cxMultiForm::cxMultiForm(const cxMultiForm& pThatMultiForm)
   : cxForm(pThatMultiForm.getParent(), pThatMultiForm.top(),
              pThatMultiForm.left(), pThatMultiForm.height(),
              pThatMultiForm.width(), pThatMultiForm.getTitle(),
              pThatMultiForm.getBorderStyle(),
              pThatMultiForm.getExtTitleWindow(),
              pThatMultiForm.getExtStatusWindow(),
              pThatMultiForm.getAutoExit(), pThatMultiForm.isStacked()),
     mCurrentForm(pThatMultiForm.mCurrentForm),
     mCycleForm(true),
     mAssumeMovingBackwards(pThatMultiForm.mAssumeMovingBackwards)
{
   copyCxMultiFormStuff(&pThatMultiForm);
   setIsModal(false); // This is done on purpose
} // Copy constructor

cxMultiForm::~cxMultiForm()
{
   // Free the memory used by the subform pointers.
   freeSubforms();
} // Destructor

cxMultiForm& cxMultiForm::operator =(const cxMultiForm& pThatMultiForm)
{
   if (&pThatMultiForm != this)
   {
      copyCxMultiFormStuff(&pThatMultiForm);
   }

   return(*this);
} // operator =

shared_ptr<cxForm> cxMultiForm::appendForm(int pRow, int pCol, int pHeight,
                                           int pWidth, const string& pTitle,
                                           eBorderStyle pBorderStyle, bool pStacked)
                                           {
   // Create the subform.  Set the form's autoExit to true so
   //  that it will exit its input loop when the user exits the last field.
   shared_ptr<cxForm> form = make_shared<cxForm>(nullptr, pRow+top(), pCol+left(), pHeight, pWidth,
                                                      pTitle, pBorderStyle, nullptr, nullptr, true, pStacked);
   // Make sure the subform will exit its input loop when the user
   //  leaves the first input going backwards (so that the user can
   //  navigate backwards through subforms).
   form->setExitOnLeaveFirst(true);
   // Also make sure the subform exits upon exit of the last field
   //  so the user can navigate forward through the subforms.
   form->setAutoExit(true);

   // Add the form function keys to the new form
   addFormFunctionKeysToSubform(form);
   // Add the quit keys & exit keys to the new form
   addQuitAndExitKeysToSubform(form);

   // Set this multiForm as the form's parent multiForm
   form->setParentMultiForm(this);

   // Add the form pointer to mForms
   mForms.push_back(form);

   return(form);
} // appendForm

bool cxMultiForm::appendForm(shared_ptr<cxForm>& pForm, int pRow, int pCol, bool* pMoved)
{
   // Don't append the form if it's nullptr, and don't
   // allow appending a cxMultiForm to itself.
   if ((pForm.get() == nullptr) || (pForm.get() == this))
   {
      return false;
   }

   bool appendedIt = false;

   // Only let them append the form if it isn't already in mForms.
   bool alreadyExists = false;
   for (const shared_ptr<cxForm>& form : mForms)
   {
      if (form == pForm)
      {
         alreadyExists = true;
         break;
      }
   }
   if (!alreadyExists)
   {
      // Move the subform to the proper place (relative to the form).
      //  Make sure it's not refreshed if it's hidden.
      bool subformMoved = pForm->move(top()+pRow, left()+pCol, !(pForm->isHidden()));
      if (pMoved != nullptr)
      {
         *pMoved = subformMoved;
      }

      // Make sure the form has no parent window
      pForm->setParent(nullptr);

      // Make sure the subform will exit its input loop when the user
      //  leaves the first input going backwards (so that the user can
      //  navigate backwards through subforms).
      pForm->setExitOnLeaveFirst(true);
      // Also make sure the subform exits upon exit of the last field
      //  so the user can navigate forward through the subforms.
      pForm->setAutoExit(true);

      // Add the form function keys to the form
      addFormFunctionKeysToSubform(pForm);
      // Add the quit keys & exit keys to the form
      addQuitAndExitKeysToSubform(pForm);

      // Set this multiForm as the form's parent multiForm
      pForm->setParentMultiForm(this);

      // Set the form's autoExit to true so that it will exit its
      //  input loop when the user exits the last field.
      pForm->setAutoExit(true);

      // Add the subform to mForms
      mForms.push_back(pForm);
      appendedIt = true;
   }

   return(appendedIt);
} // appendForm

bool cxMultiForm::appendForm(shared_ptr<cxForm>& pForm)
{
   // Don't append the form if it's nullptr, and don't
   // allow appending a cxMultiForm to itself.
   if ((pForm.get() == nullptr) || (pForm.get() == this))
   {
      return false;
   }

   bool appendedIt = false;
   // Only let them append the form if it isn't already in mForms.
   bool alreadyExists = false;
   formPtrContainer::const_iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      if ((*iter) == pForm)
      {
         alreadyExists = true;
         break;
      }
   }
   if (!alreadyExists)
   {
      // Make sure the form has no parent window
      pForm->setParent(nullptr);

      // Make sure the subform will exit its input loop when the user
      //  leaves the first input going backwards (so that the user can
      //  navigate backwards through subforms).
      pForm->setExitOnLeaveFirst(true);
      // Also make sure the subform exits upon exit of the last field
      //  so the user can navigate forward through the subforms.
      pForm->setAutoExit(true);

      // Add the form function keys to the form
      addFormFunctionKeysToSubform(pForm);
      // Add the quit keys & exit keys to the form
      addQuitAndExitKeysToSubform(pForm);

      // Set this multiForm as the form's parent multiForm
      pForm->setParentMultiForm(this);

      // Set the form's autoExit to true so that it will exit its
      //  input loop when the user exits the last field.
      pForm->setAutoExit(true);

      // Add the subform to mForms
      mForms.push_back(pForm);
      appendedIt = true;
   }

   return(appendedIt);
} // appendForm

shared_ptr<cxForm> cxMultiForm::getForm(unsigned pIndex) const
{
   shared_ptr<cxForm> form;

   if ((pIndex >= 0) && (pIndex < mForms.size()))
   {
      form = mForms[pIndex];
   }

   return(form);
} // getForm

shared_ptr<cxForm> cxMultiForm::getForm(const string& pTitle) const
{
   shared_ptr<cxForm> form;

   for (const shared_ptr<cxForm>& formInCollection : mForms)
   {
      if (formInCollection->getTitle() == pTitle)
      {
         form = formInCollection;
         break;
      }
   }

   return(form);
} // getForm

long cxMultiForm::show(bool pBringToTop, bool pShowSubwindows)
{
   long retval = cxFIRST_AVAIL_RETURN_CODE;
   // If the form is enabled, show the form (along with the
   //  inputs), and show all subforms.  Otherwise, hide the
   //  form and all subforms.
   if (isEnabled())
   {
      // Show this window before showing the subforms if
      //  getShowSelfBeforeSubwins() returns true
      if (getShowSelfBeforeSubwins())
      {
         retval = cxForm::show(pBringToTop, pShowSubwindows);
         // Set pBringToTop to true so that when the subforms are shown,
         //  they will appear above the multiForm window.
         pBringToTop = true;
      }

      // Show the subforms
      showAllSubforms(pBringToTop, false);

      // Show this window now if getShowSelfBeforeSubwins() returns false
      if (!getShowSelfBeforeSubwins())
      {
         retval = cxWindow::show(pBringToTop, pShowSubwindows);
      }
   }
   else
   {
      hide(false);
   }

   return(retval);
} // show

long cxMultiForm::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   setReturnCode(cxID_EXIT);

   // Do the input loop if the form is enabled.
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
         // Do the input loop; have the subforms show themselves if we didn't
         //  show the multiForm.
         setReturnCode(doInputLoop(!pShowSelf));
         // Run the onLeave function
         runOnLeaveFunction();
      }

      mIsModal = false;
   }

   return(getReturnCode());
} // showModal

void cxMultiForm::showAllSubforms(bool pBringToTop, bool pSkipCurrentForm) const
{
   unsigned numForms = mForms.size();
   for (unsigned i = 0; i < numForms; ++i)
   {
      // Don't show the current subform if pSkipCurrentForm is true.
      if (pSkipCurrentForm)
      {
         if ((unsigned)mCurrentForm == i)
         {
            continue;
         }
      }

      // If the subform is below this window, bring it to the top
      //  automatically.  Otherwise, honor pBringToTop.
      if (mForms[i]->isBelow(*this)) {
         mForms[i]->show(true, false);
      }
      else
      {
         mForms[i]->show(pBringToTop, false);
      }
   }
} // showAllSubforms

bool cxMultiForm::removeSubform(unsigned pIndex)
{
   bool removed = false;

   if ((pIndex >= 0) && (pIndex < mForms.size()))
   {
      mForms.erase(mForms.begin()+pIndex);
      removed = true;
   }

   return(removed);
} // removeSubform

string cxMultiForm::getValue(int pIndex) const
{
   return cxForm::getValue(pIndex);
} // getValue

string cxMultiForm::getValue(const string& pStr, bool pIsLabel) const
{
   return cxForm::getValue(pStr, pIsLabel);
} // getValue

string cxMultiForm::getValue(unsigned pFormIndex, int pInputIndex) const
{
   string retval;

   if ((pFormIndex >= 0) && (pFormIndex <= mForms.size()))
   {
      retval = mForms[pFormIndex]->getValue((int)pInputIndex);
   }

   return(retval);
} // getValue

string cxMultiForm::getValue(unsigned pFormIndex, const string& pLabel, bool pIsLabel) const
{
   string retval;

   if ((pFormIndex >= 0) && (pFormIndex <= mForms.size()))
   {
      retval = mForms[pFormIndex]->getValue(pLabel, pIsLabel);
   }

   return(retval);
} // getValue

string cxMultiForm::getValue(const string& pTitle, int pInputIndex) const
{
   string value;

   formPtrContainer::const_iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      if ((*iter)->getTitle() == pTitle)
      {
         value = (*iter)->getValue(pInputIndex);
         break;
      }
   }

   return(value);
} // getValue

string cxMultiForm::getValue(const string& pTitle, const string& pLabel, bool pIsLabel) const
{
   string value;

   formPtrContainer::const_iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      if ((*iter)->getTitle() == pTitle)
      {
         value = (*iter)->getValue(pLabel, pIsLabel);
         break;
      }
   }

   return(value);
} // getValue

bool cxMultiForm::setValue(unsigned pFormIndex, int pInputIndex, const string& pValue, bool pRefresh)
{
   bool valueWasSet = false;

   if ((pFormIndex >= 0) && (pFormIndex <= mForms.size()))
   {
      valueWasSet = mForms[pFormIndex]->setValue(pInputIndex, pValue, pRefresh);
   }

   return(valueWasSet);
} // setValue

bool cxMultiForm::setValue(unsigned pFormIndex, const string& pLabel, const string& pValue, bool pIsLabel, bool pRefresh)
{
   bool valueWasSet = false;

   if ((pFormIndex >= 0) && (pFormIndex <= mForms.size()))
   {
      valueWasSet = mForms[pFormIndex]->setValue(pLabel, pValue, pIsLabel, pRefresh);
   }

   return(valueWasSet);
} // setValue

bool cxMultiForm::setValue(const string& pTitle, int pInputIndex, const string& pValue, bool pRefresh)
{
   bool valueWasSet = false;

   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      if ((*iter)->getTitle() == pTitle)
      {
         valueWasSet = (*iter)->setValue(pInputIndex, pValue, pRefresh);
         break;
      }
   }

   return(valueWasSet);
} // setValue

bool cxMultiForm::setValue(const string& pTitle, const string& pLabel, const string& pValue, bool pIsLabel, bool pRefresh)
{
   bool valueWasSet = false;

   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      if ((*iter)->getTitle() == pTitle)
      {
         valueWasSet = (*iter)->setValue(pLabel, pValue, pIsLabel, pRefresh);
         break;
      }
   }

   return(valueWasSet);
} // setValue

bool cxMultiForm::setValue(int pIndex, const string& pValue, bool pRefresh)
{
   return(cxForm::setValue(pIndex, pValue, pRefresh));
} // setValue

bool cxMultiForm::setValue(const string& pLabel, const string& pValue,
      bool pIsLabel, bool pRefresh)
      {
   return(cxForm::setValue(pLabel, pValue, pIsLabel, pRefresh));
}

size_t cxMultiForm::numSubforms() const
{
   return(mForms.size());
} // numSubforms

int cxMultiForm::getLastKey() const
{
   return(cxForm::getLastKey());
} // getLastKey

bool cxMultiForm::hasEditableSubforms() const
{
   bool editableSubformExists = false;

   formPtrContainer::const_iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      // If the subform is enabled and has editable inputs,
      //  then it is considered editable.
      if ((*iter)->isEnabled() && (*iter)->hasEditableInputs())
      {
         editableSubformExists = true;
         break;
      }
   }

   return (editableSubformExists);
} // hasEditableSubforms

bool cxMultiForm::move(int pNewRow, int pNewCol, bool pRefresh)
{
   // In case moving any of the subforms fails..
   int oldRow = top();
   int oldCol = left();

   // Calculate the relative amounts to move the subforms by
   int vOffset = pNewRow - top();
   int hOffset = pNewCol - left();

   // Move the main form
   bool moved = cxForm::move(pNewRow, pNewCol, false);
   // If the window move was successful, move the subforms.
   if (moved)
   {
      // Save the positions of the subforms in case
      //  moving one of the fails, so we can move
      //  the rest back to where they were
      vector<pair<int, int> > oldSubformPositions;

      formPtrContainer::iterator iter = mForms.begin();
      for (; iter != mForms.end(); ++iter)
      {
         // Store the subform's current position
         //  in oldSubformPositions
         int subformTop = (*iter)->top();
         int subformLeft = (*iter)->left();
         oldSubformPositions.push_back(pair<int, int>(subformTop, subformLeft));
         // Move the subform
         moved = (*iter)->moveRelative(vOffset, hOffset, false);
         // If moving the subform failed, then move all
         //  the previously-moved subforms back to where
         //  they were.
         if (!moved)
         {
            unsigned vectorSize = oldSubformPositions.size();
            for (unsigned i = 0; i < vectorSize; ++i)
            {
               mForms[i]->move(oldSubformPositions[i].first,
                                oldSubformPositions[i].second, false);
            }
            break;
         }
      }
   }

   // If the move was successful, refresh everything
   //  if pRefresh is true; otherwise, move the
   //  the multiForm to where it was before.
   if (moved)
   {
      if (pRefresh)
      {
         show(false, false);
      }
   }
   else
   {
      cxForm::move(oldRow, oldCol, false);
   }

   return(moved);
} // move

void cxMultiForm::hide(bool pHideSubwindows)
{
   // Hide the main window
   cxForm::hide(pHideSubwindows);
   // Hide the subforms
   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      (*iter)->hide(pHideSubwindows);
   }
} // hide

void cxMultiForm::unhide(bool pUnhideSubwindows)
{
   // If the form is enabled, un-hide the main form
   //  window and all subwindows.
   if (isEnabled())
   {
      cxForm::unhide(pUnhideSubwindows);
      // Un-hide the subforms
      formPtrContainer::iterator iter = mForms.begin();
      for (; iter != mForms.end(); ++iter)
      {
         (*iter)->unhide(pUnhideSubwindows);
      }
   }
} // unhide

bool cxMultiForm::setKeyFunction(int pKey, funcPtr4 pFunction,
                             void *p1, void *p2, void *p3, void *p4,
                             bool pUseReturnVal, bool pExitAfterRun,
                             bool pRunOnLeaveFunction)
                             {
   // Set the form function via the parent class' setKeyFunction, and set
   //  the form function in all the subforms.
   bool setIt = cxForm::setKeyFunction(pKey, pFunction, p1, p2,
                   p3, p4, pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction);
   if (setIt)
   {
      formPtrContainer::iterator iter = mForms.begin();
      for (; iter != mForms.end(); ++iter)
      {
         // Add the key to the subform to make it stop its input loop
         //  (so that the key can be handled here)
         // Note: if the function doesn't do anything, the subform will
         //  still quit out of its input loop, and the multiForm will go
         //  onto the next subform.
         (*iter)->addExitKey(pKey, true, true);
      }
   }

   return(setIt);
} // setKeyFunction

bool cxMultiForm::setKeyFunction(int pKey, funcPtr2 pFunction,
                             void *p1, void *p2, bool pUseReturnVal,
                             bool pExitAfterRun, bool pRunOnLeaveFunction)
                             {
   // Set the form function via the parent class' setKeyFunction, and set
   //  the form function in all the subforms.
   bool setIt = cxForm::setKeyFunction(pKey, pFunction, p1, p2,
                        pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction);
   if (setIt)
   {
      formPtrContainer::iterator iter = mForms.begin();
      for (; iter != mForms.end(); ++iter)
      {
         // Add the key to the subform to make it stop its input loop
         //  (so that the key can be handled here)
         // Note: if the function doesn't do anything, the subform will
         //  still quit out of its input loop, and the multiForm will go
         //  onto the next subform.
         (*iter)->addExitKey(pKey, true, true);
      }
   }

   return(setIt);
} // setKeyFunction

bool cxMultiForm::setKeyFunction(int pKey, funcPtr0 pFunction,
                             bool pUseReturnVal, bool pExitAfterRun,
                             bool pRunOnLeaveFunction)
                             {
   // Set the form function via the parent class' setKeyFunction, and set
   //  the form function in all the subforms.
   bool setIt = cxForm::setKeyFunction(pKey, pFunction, pUseReturnVal,
                                       pExitAfterRun, pRunOnLeaveFunction);
   if (setIt)
   {
      formPtrContainer::iterator iter = mForms.begin();
      for (; iter != mForms.end(); ++iter)
      {
         // Add the key to the subform to make it stop its input loop
         //  (so that the key can be handled here)
         // Note: if the function doesn't do anything, the subform will
         //  still quit out of its input loop, and the multiForm will go
         //  onto the next subform.
         (*iter)->addExitKey(pKey, true, true);
      }
   }

   return(setIt);
} // setKeyFunction

bool cxMultiForm::removeSubform(const string& pTitle)
{
   bool removed = false;

   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      if ((*iter)->getTitle() == pTitle)
      {
         mForms.erase(iter);
         removed = true;
         break;
      }
   }

   return(removed);
} // removeSubform

bool cxMultiForm::setCurrentSubform(int pIndex)
{
   bool retval = false;

   if ((pIndex >= 0) && (pIndex < (int)mForms.size()))
   {
      retval = true;
      mCurrentForm = pIndex;
      // Make sure the form is enabled (if it's not
      //  enabled, the app will crash, for some reason)
      mForms[mCurrentForm]->setEnabled(true);

      // If the multiForm is modal, then don't increment/decrement
      //  mCurrentForm in the next iteration through the input loop.
      mCycleForm = !mIsModal;
   }

   return(retval);
} // setCurrentSubform

bool cxMultiForm::setCurrentSubform(const string& pTitle)
{
   bool succeeded = false;

   int numSubforms = (int)(mForms.size());
   for (int i = 0; i < numSubforms; ++i)
   {
      if (mForms[i]->getTitle() == pTitle)
      {
         succeeded = true;
         mCurrentForm = i;

         // If the multiForm is modal, then don't increment/decrement
         //  mCurrentForm in the next iteration through the input loop.
         mCycleForm = !mIsModal;

         break;
      }
   }

   return(succeeded);
} // setCurrentSubform

bool cxMultiForm::setCurrentSubformByPtr(const shared_ptr<cxForm>& pForm)
{
   return setCurrentSubformByPtr(pForm.get());
}

bool cxMultiForm::setCurrentSubformByPtr(cxForm *pForm)
{
   bool succeeded = false;

   if (pForm != nullptr)
   {
      int numSubforms = (int)(mForms.size());
      for (int i = 0; i < numSubforms; ++i)
      {
         if (mForms[i].get() == pForm)
         {
            succeeded = true;
            mCurrentForm = i;

            // If the multiForm is modal, then don't increment/decrement
            //  mCurrentForm in the next iteration through the input loop.
            mCycleForm = !mIsModal;

            break;
         }
      }
   }

   return(succeeded);
} // setCurrentSubformByPtr

int cxMultiForm::getCurrentForm() const
{
   return(mCurrentForm);
} // getCurrentForm

int cxMultiForm::getSubformIndex(const std::shared_ptr<cxForm>& pForm) const
{
   return getSubformIndex(pForm.get());
}

int cxMultiForm::getSubformIndex(cxForm *pForm) const
{
   int index = -1;

   int numSubforms = (int)mForms.size();
   for (int i = 0; i < numSubforms; ++i)
   {
      if (mForms[i].get() == pForm)
      {
         index = i;
         break;
      }
   }

   return(index);
} // getSubformIndex

bool cxMultiForm::hasChanged() const
{
   // See if any inputs changed on the form
   bool anythingChanged = cxForm::hasChanged();

   // If no inputs changed, see if any inputs
   //  on the subforms changed.
   if (!anythingChanged)
   {
      formPtrContainer::const_iterator iter = mForms.begin();
      for (; iter != mForms.end(); ++iter)
      {
         if ((*iter)->hasChanged())
         {
            anythingChanged = true;
            break;
         }
      }
   }

   return(anythingChanged);
} // hasChanged

void cxMultiForm::setChanged(bool pDataChanged)
{
   // Set pDataChanged on the main form
   cxForm::setChanged(pDataChanged);
   // Set pDataChanged on the subforms
   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      (*iter)->setChanged(pDataChanged);
   }
} // setChanged

void cxMultiForm::setSubformEnabled(unsigned pIndex, bool pEnabled)
{
   if ((pIndex >= 0) && (pIndex < mForms.size()))
   {
      mForms[pIndex]->setEnabled(pEnabled);
   }
} // setSubformEnabled

void cxMultiForm::setSubformEnabled(const string& pTitle, bool pEnabled)
{
   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      if ((*iter)->getTitle() == pTitle)
      {
         (*iter)->setEnabled(pEnabled);
         // Don't break here..  Apply pEnabled to
         //  all subforms with the same title.
      }
   }
} // setSubformEnabled

bool cxMultiForm::subformIsEnabled(unsigned pIndex) const
{
   bool isEnabled = false;

   if ((pIndex >= 0) && (pIndex < mForms.size()))
   {
      isEnabled = mForms[pIndex]->isEnabled();
   }

   return (isEnabled);
} // subformIsEnabled

bool cxMultiForm::addQuitKey(int pKey, bool pRunOnLeaveFunction, bool pOverride)
{
   // Add the key to the parent and all of the subforms.  Also, in case the
   //  key is set up as an exit key, remove it from the exit key list.
   bool added = cxForm::addQuitKey(pKey, pRunOnLeaveFunction, pOverride);
   if (added)
   {
      cxWindow::removeExitKey(pKey);

      formPtrContainer::iterator iter = mForms.begin();
      for (; iter != mForms.end(); ++iter)
      {
         (*iter)->addQuitKey(pKey, pRunOnLeaveFunction, pOverride);
         (*iter)->removeExitKey(pKey);
      }
   }

   return(added);
} // addQuitKey

void cxMultiForm::removeQuitKey(int pKey)
{
   cxForm::removeQuitKey(pKey);
   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      (*iter)->removeQuitKey(pKey);
   }
} // removeQuitKey

bool cxMultiForm::addExitKey(int pKey, bool pRunOnLeaveFunction, bool pOverride)
{
   // Add the key to the parent and all of the subforms.  Also, in case the
   //  key is set up as a quit key, remove it from the quit key list.
   bool added = cxForm::addExitKey(pKey, pRunOnLeaveFunction, pOverride);
   if (added)
   {
      cxWindow::removeQuitKey(pKey);

      formPtrContainer::iterator iter = mForms.begin();
      for (; iter != mForms.end(); ++iter)
      {
         (*iter)->addExitKey(pKey, pRunOnLeaveFunction, pOverride);
         (*iter)->removeQuitKey(pKey);
      }
   }

   return(added);
} // addExitKey

void cxMultiForm::removeExitKey(int pKey)
{
   cxForm::removeExitKey(pKey);
   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      (*iter)->removeExitKey(pKey);
   }
} // removeExitKey

void cxMultiForm::setDisableCursorOnShow(bool pDisableCursorOnShow)
{
   cxForm::setDisableCursorOnShow(pDisableCursorOnShow);
   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      (*iter)->setDisableCursorOnShow(pDisableCursorOnShow);
   }
} // setDisableCursorOnShow

void cxMultiForm::clear(bool pRefresh)
{
   // Clear the parent form
   cxForm::clear(pRefresh);
   // And clear all the subforms
   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      (*iter)->clear(pRefresh);
   }
} // clear

void cxMultiForm::setAssumeMovingBackwards(bool pAssumeMovingBackwards)
{
   mAssumeMovingBackwards = pAssumeMovingBackwards;
} // setAssumeMovingBackwards

bool cxMultiForm::getAssumeMovingBackwards() const
{
   return(mAssumeMovingBackwards);
} // getAssumeMovingBackwards

void cxMultiForm::setEnabled(bool pEnabled)
{
   // Enable/disable the multiForm window, then enable/disable the subforms.
   cxForm::setEnabled(pEnabled);
   formPtrContainer::iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      (*iter)->setEnabled(pEnabled);
   }
} // setEnabled

string cxMultiForm::cxTypeStr() const
{
   return("cxMultiForm");
} // cxTypeStr

bool cxMultiForm::subformIsEnabled(const string& pTitle) const
{
   bool isEnabled = false;

   formPtrContainer::const_iterator iter = mForms.begin();
   for (; iter != mForms.end(); ++iter)
   {
      if ((*iter)->getTitle() == pTitle)
      {
         isEnabled = (*iter)->isEnabled();
         break;
      }
   }

   return (isEnabled);
} // subformIsEnabled

//// Protected functions

void cxMultiForm::copyCxMultiFormStuff(const cxMultiForm* pThatMultiForm)
{
   if ((pThatMultiForm != nullptr) && (pThatMultiForm != this))
   {
      freeSubforms();

      // Copy the parent cxForm stuff, then copy the stuff for this class.
      try
      {
         copyCxFormStuff((const cxForm*)pThatMultiForm);
      }
      catch (const cxWidgetsException& esc)
      {
         // Free up the other memory used
         cxWindow::freeWindow();
         throw(cxWidgetsException("Couldn't copy base cxForm stuff (copying another cxMultiForm)."));
      }

      // Copy the basic member data
      mCurrentForm = pThatMultiForm->mCurrentForm;
      cxForm::setLastKey(pThatMultiForm->getLastKey());
      mCycleForm = true;
      mAssumeMovingBackwards = pThatMultiForm->mAssumeMovingBackwards;
      setIsModal(false);

      // Copy the subforms
      formPtrContainer::const_iterator iter = pThatMultiForm->mForms.begin();
      for (; iter != pThatMultiForm->mForms.end(); ++iter)
      {
         mForms.push_back(make_shared<cxForm>(**iter));
      }
   }
} // copyCxMultiFormStuff

long cxMultiForm::doInputLoop(bool pShowSubforms)
{
   long returnCode = cxID_EXIT;
   mLeaveNow = false;

   // Run the onFocus function.  If it
   //  returns true, that means we should exit
   //  after the function runs.
   if (runOnFocusFunction())
   {
      return(returnCode);
   }

   bool continueOn = true;
   while (continueOn)
   {
      // Run the loop start function, and break from
      //  the input loop if its mExitAfterRun is true.
      if (runLoopStartFunction())
      {
         break;
      }

      // If there are no editable inputs and no enabled/editable
      //  subforms on the form, show a message to the user.
      if (!hasEditableInputs() && !hasEditableSubforms())
      {
         if (!getWaitForInputIfEmpty())
         {
            cxBase::messageBox("This multiForm has no editable inputs and no enabled/editable subforms.");
         }
         else
         {
            setLastKey(wgetch(mWindow));
#ifdef NCURSES_MOUSE_VERSION
            // Handle mouse events.
            // If mouse button 1 was clicked, then:
            //  - If it was clicked outside the window, then if the parent
            //    window is a cxPanel or another cxMultiForm, then exit the
            //    input loop.  This would allow the user to go to another
            //    window.
            if (getLastKey() == KEY_MOUSE)
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
                     if (mouseButton1Clicked())
                     {
                        if (!mouseEvtWasInWindow())
                        {
                           if (parentIsCxPanel() || (mParentMultiForm != nullptr))
                           {
                              continueOn = false;
                           }
                        }
                     }
                  }
               }
            }
            else
            {
               continueOn = false;
            }
#endif
         }

         // If ncurses does not support the mouse (NCURSES_MOUSE_VERSION is not
         // defined), then just exit the input loop.
#ifndef NCURSES_MOUSE_VERSION
         continueOn = false;
#endif
      }
      else
      {
         // doTheInputs is used for controlling whether to cycle through the
         //  inputs for each pass through the input loop.
         bool doTheInputs = true;

         // Cycle through all inputs on this form, if there are any, before
         //  cycling through the subforms.
         if (doTheInputs && hasEditableInputs())
         {
            returnCode = doInputs(continueOn);
            if (continueOn)
            {
               // If the user wants to quit, and we allow
               //  this, then let the user quit.
               if (((returnCode == cxID_QUIT) && getAllowQuit()))
               {
                  continueOn = false;
                  break;
               }
               // Update continueOn in case mLeaveNow has changed
               continueOn = (continueOn && !mLeaveNow);
            }
         }

         // Reset doTheInputs for the next iteration (in case it was set false
         //  last time)
         doTheInputs = true;

         if (continueOn)
         {
            // Cycle through all the subforms, if there are any that have
            //  editable inputs.
            if (hasEditableSubforms())
            {
               bool clickedInput = false;
               returnCode = doSubforms(pShowSubforms, continueOn, clickedInput);
               if (continueOn)
               {
                  // If the user wants to quit, and we allow
                  //  this, then let the user quit.
                  if (((returnCode == cxID_QUIT) && getAllowQuit()))
                  {
                     continueOn = false;
                     break;
                  }
                  // If the user clicked an input, then set continueOn to true
                  //  so that we can go back up and cycle through the inputs
                  //  again.  Otherwise, update continueOn based on other
                  //  factors.
                  if (clickedInput)
                  {
                     continueOn = true;
                  }
                  else
                  {
                     // Update continueOn based on:
                     //  - The current value of continueOn AND
                     //  - The value of mLeaveNow (in case it changed via
                     //    an event function)
                     //  - Whether or not autoExit is enabled (if not, then
                     //    we should continue on if the others are all true)
                     continueOn = (continueOn && !mLeaveNow && !getAutoExit());
                  }
               }
            }
         }
      }

      // Run the end loop function.  Update continueOn
      //  based on its current value, the value of
      //  the end loop function's mExitAfterRun, and whether
      //  we should exit after the last form (the opposite
      //  of getAutoExit()).
      continueOn = (continueOn && !runLoopEndFunction() && !getAutoExit());
   }

   // mLeaveNow should be false
   mLeaveNow = false;

   return(returnCode);
} // doInputLoop

int cxMultiForm::lowestSubformRow(unsigned pIndex) const
{
   int lowestRow = -1;

   if ((pIndex >= 0) && (pIndex < mForms.size()) && mForms.size() > 0)
   {
      formPtrContainer::const_iterator iter = mForms.begin() + pIndex;
      int row = 0;
      for (; iter != mForms.end(); ++iter)
      {
         row = (*iter)->top() - top() > lowestRow;
         if (row > lowestRow)
         {
            lowestRow = row;
         }
      }
   }

   return(lowestRow);
} // lowestSubformRow

//// Private functions

inline void cxMultiForm::freeSubforms()
{
   for (shared_ptr<cxForm>& form : mForms)
   {
      form.reset(); // TODO: Is this really necessary here?
   }
   mForms.clear();
} // freeSubforms

long cxMultiForm::doInputs(bool& pContinueOn)
{
   long returnCode = cxID_EXIT;
   pContinueOn = true;

   if ((numInputs() > 0) && hasEditableInputs())
   {
      // Go through all the inputs.  Don't worry about
      //  processing function keys, because that's
      //  done in cxForm.
      bool runOnLeaveFunction = true;
      bool functionExists = false;
      while(doCurrentInput(returnCode, runOnLeaveFunction, functionExists,
                           true) && !mLeaveNow && pContinueOn)
                           {
#ifdef NCURSES_MOUSE_VERSION
         if (cxForm::getLastKey() == KEY_MOUSE)
         {
            // Run a function that may exist for the mouse state.  If
            //  no function exists for the mouse state, then process
            //  it here.
            bool mouseFuncExists = false;
            pContinueOn = handleFunctionForLastMouseState(&mouseFuncExists);
            if (!mouseFuncExists)
            {
               if (mouseButton1Clicked())
               {
                  // Loop through the member inputs and see if the user clicked
                  //  on another input.  If so, then set that input as the current
                  //  input.  If the user clicked outside all inputs, then exit
                  //  the while loop.
                  bool clickedAnotherInput = false;
                  int index = 0;
                  for (; index < (int)(mInputs.size()); ++index)
                  {
                     // If the mouse event location is inside the input, then
                     //  set it as the current input and go onto the next
                     //  iteration of the while loop.
                     if (mInputs[index]->pointIsInWindow(mMouse.y, mMouse.x))
                     {
                        clickedAnotherInput = true;
                        setCurrentInput(index);
                     }
                  }
                  if (clickedAnotherInput)
                  {
                     continue;
                  }
                  else
                  {
                     // The user didn't click another input.  See if they clicked
                     //  in a subform, and if so, select it.
                     bool clickedASubform = false;
                     index = 0;
                     for (; index < (int)(mForms.size()); ++index)
                     {
                        // If the mouse event location is inside the form, then
                        //  set it as the current form.
                        if (mForms[index]->pointIsInWindow(mMouse.y, mMouse.x))
                        {
                           setCurrentSubform(index);
                           clickedASubform = true;
                        }
                     }

                     // If the user didn't click a subform, then check to see
                     //  if the user clicked outside the multiForm window.  If
                     //  so, then if the parent window is a cxPanel or another
                     //  cxForm/cxMultiForm, then set pContinueOn to false so
                     //  that the main input loop with exit.
                     if (!clickedASubform && !mouseEvtWasInWindow())
                     {
                        if (parentIsCxPanel() || (mParentMultiForm != nullptr))
                        {
                           pContinueOn = false;
                        }
                        else
                        {
                           // Continue onto the next iteration of the while
                           //  loop.  The user will stay in the current input.
                           continue;
                        }
                     }
                     else
                     {
                        // Exit out of the while loop for inputs
                        returnCode = cxID_EXIT;
                        break;
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
         // If the last key is a quit key, then quit and return
         //  cxID_QUIT.  If the key isn't there, look for it in
         //  the exit keys (if it's there, quit and return cxID_EXIT).
         else if (hasQuitKey(cxForm::getLastKey()))
         {
            returnCode = cxID_QUIT;
            pContinueOn = false;
         }
         else if (hasExitKey(cxForm::getLastKey()))
         {
            returnCode = cxID_EXIT;
            pContinueOn = false;
         }
      }
      // If pContinueOn was set false, then return now.
      if (!pContinueOn)
      {
         return(returnCode);
      }

      // Depending on the user's last key and
      //  return code, make sure we go to the
      //  first or last subform.
      // Note: The parent class (cxForm)'s doCurrentInput()
      //  handles running functions for the last keypress,
      //  so we don't have to do that here.
      switch (cxForm::getLastKey())
      {
         case TAB:
         case ENTER:
         case KEY_ENTER:
         case KEY_DOWN:
            mCurrentForm = 0;
            break;
         case SHIFT_TAB:
         case KEY_UP:
            mCurrentForm = (int)mForms.size() - 1;
            if (mCurrentForm < 0)
            {
               mCurrentForm = 0;
            }
            break;
         default:
            // Do nothing (Note: Don't process function keys
            //  here, because that's done in cxForm.)
            break;
      }
   }

   return(returnCode);
} // doInputs

long cxMultiForm::doSubforms(bool pShowSubforms, bool& pContinueOn, bool& pClickedInput)
{
   long returnCode = cxID_EXIT;
   pContinueOn = true;
   pClickedInput = false;

   if ((mForms.size() > 0) && hasEditableSubforms())
   {
      // If the last keypress was a mouse event, then run a function that may
      //  exist for the mouse state.
#ifdef NCURSES_MOUSE_VERSION
      if (cxForm::getLastKey() == KEY_MOUSE)
      {
         if (getmouse(&mMouse) == OK)
         {
            pContinueOn = handleFunctionForLastMouseState();
         }
      }
      else
      {
#endif
      // If the current form is the last form and mAssumeMovingBackwards is
      //  true, assume that we're navigating backwards and set the current
      //  input on the last form to its last input.
         if ((mCurrentForm == (int)mForms.size() - 1) && mAssumeMovingBackwards)
         {
            int lastInput = mForms[mCurrentForm]->numInputs() - 1;
            if (lastInput < 0)
            {
               lastInput = 0;
            }
            mForms[mCurrentForm]->setCurrentInput(lastInput);
         }
#ifdef NCURSES_MOUSE_VERSION
      }
#endif

      shared_ptr<cxForm> currentForm = mForms[mCurrentForm];
      while (pContinueOn)
      {
         // Show the current form modally but don't bring it
         //  to the top, because bringing it to the top will
         //  cause it to also have to re-draw all of its
         //  inputs, which is noticeable on slow terminals.
         returnCode = currentForm->showModal(pShowSubforms, false, false);
         // Set the last key and check for quit/exit functions
         if (currentForm->modalGetsKeypress())
         {
            cxForm::setLastKey(currentForm->getLastKey());
#ifdef NCURSES_MOUSE_VERSION
            if (cxForm::getLastKey() == KEY_MOUSE)
            {
               // If the input captured a mouse event, then set mMouse to the
               //  form's mMouse.  Note: Not using getmouse() here because if
               //  an onLeave function for the input was fired and the user
               //  used the mouse there, then getmouse() would get that mouse
               //  event rather than the one from the form, which is not what
               //  we would want.
               mMouse = currentForm->getMouseEvent();
               // Run a function that may exist for the mouse state.  If
               //  no function exists for the mouse state, then process
               //  it here.
               bool mouseFuncExists = false;
               pContinueOn = handleFunctionForLastMouseState(&mouseFuncExists);
               if (!mouseFuncExists)
               {
                  if (mouseButton1Clicked())
                  {
                     // Loop through the subforms and see if the user clicked on
                     //  another form.  If so, then set that form as the current
                     //  subform.
                     bool clickedAnotherForm = false;
                     int index = 0;
                     for (; index < (int)(mForms.size()); ++index)
                     {
                        // If the mouse event location is inside the form, then
                        //  set it as the current input and go onto the next
                        //  iteration of the while loop.
                        if (mForms[index]->pointIsInWindow(mMouse.y, mMouse.x))
                        {
                           clickedAnotherForm = true;
                           setCurrentSubform(index);
                           currentForm = mForms[index];
                        }
                     }
                     if (clickedAnotherForm)
                     {
                        continue;
                     }
                     else
                     {
                        // The user didn't click another subform.  Look to see
                        //  if they clicked in an input, and if so, select it.
                        index = 0;
                        for (; index < (int)(mInputs.size()); ++index)
                        {
                           // If the mouse event location is inside the input, then
                           //  set it as the current input and go onto the next
                           //  iteration of the while loop.
                           if (mInputs[index]->pointIsInWindow(mMouse.y, mMouse.x))
                           {
                              pClickedInput = true;
                              setCurrentInput(index);
                           }
                        }

                        // If the user didn't click an input, then check to see
                        //  if the user clicked outside the multiForm window.  If
                        //  so, then if the parent window is a cxPanel or another
                        //  cxForm/cxMultiForm, then set pContinueOn to false so
                        //  that the main input loop with exit.
                        if (!pClickedInput && !mouseEvtWasInWindow())
                        {
                           if (parentIsCxPanel() || (mParentMultiForm != nullptr))
                           {
                              pContinueOn = false;
                           }
                           else
                           {
                              // Continue onto the next iteration of the while
                              //  loop.  The user will stay in the current form.
                              continue;
                           }
                        }
                        else
                        {
                           // Exit the while loop for the subforms
                           returnCode = cxID_EXIT;
                           break;
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
            // If the last key is a quit key, then quit and return
            //  cxID_QUIT.  If the key isn't there, look for it in
            //  the exit keys (if it's there, quit and return cxID_EXIT).
            //  If not there either, handle the key normally.
            else if (hasQuitKey(cxForm::getLastKey()))
            {
               returnCode = cxID_QUIT;
               pContinueOn = false;
            }
            else if (hasExitKey(cxForm::getLastKey()))
            {
               returnCode = cxID_EXIT;
               pContinueOn = false;
            }
         }

         // If pContinueOn was set false, then return now.
         if (!pContinueOn)
         {
            return(returnCode);
         }

         // If the user wants to quit, and we allow this,
         //  then let the user quit (break out of the input
         //  loop).  Otherwise, handle the last keypress.
         if ((returnCode == cxID_QUIT) && getAllowQuit())
         {
            pContinueOn = false;
            break;
         }
         else
         {
            // If we got a new keypress, run a function that may be associated
            //  with the last keypress.  If no function exists for the key,
            //  then go on to the next/previous subform.
            bool functionExists = false;
            if (currentForm->modalGetsKeypress())
            {
               pContinueOn = handleFunctionForLastKey(&functionExists);
            }
            if (!pContinueOn)
            {
               break;
            }

            if (!functionExists)
            {
               // Based on the last key typed by the user, go to the next
               //  or previous form (if pContinueOn is still true).
               switch (cxForm::getLastKey())
               {
                  // Shift-tab & up arrow: Go to the previous form
                  case SHIFT_TAB:
                  case KEY_UP:
                     // Set the current form's input to its first input, but
                     //  only if it's allowed to cycle its current input.
                     if (currentForm->mCycleInput)
                     {
                        currentForm->setCurrentInput(0);
                     }
                     currentForm->mCycleInput = true;
                     // Go to the previous form.  If the current form
                     //  was the last form, then don't continue.
                     if (mCycleForm)
                     {
                        if (selectPrevForm())
                        {
                           pContinueOn = false;
                           // If we go back to the inputs/ make sure we
                           //  go to the last one.
                           mCurrentInput = (int)numInputs() - 1;
                           if (mCurrentInput < 0)
                           {
                              mCurrentInput = 0;
                           }
                        }
                     }
                     break;
                  // Tab, enter, & down (default): Go to the
                  //  next form
                  case TAB:
                  case ENTER:
                  case KEY_ENTER:
                  case KEY_DOWN:
                     // Set the current form's input to its last input, but
                     //  only if it's allowed to cycle its current input.
                     if (currentForm->mCycleInput)
                     {
                        currentForm->setCurrentInput(currentForm->numInputs()-1);
                     }
                     currentForm->mCycleInput = true;
                     // Go to the next form.  If the current form
                     //  was the last form, then don't continue.
                     if (mCycleForm)
                     {
                        // Go to the next form
                        if (selectNextForm())
                        {
                           pContinueOn = false;
                           // If we go back to the inputs, make sure
                           //  we go to the first one.
                           mCurrentInput = 0;
                        }
                        // Set focus to the first editable input in the form.
                        int inputIndex = mForms[mCurrentForm]->firstEditableInput();
                        mForms[mCurrentForm]->setCurrentInput(inputIndex);
                     }
                     break;
                  default:
                     {
                        bool functionExists = false;
                        // Run any function that may be associated with the
                        //  last keypress.  Capture the return value
                        //  (handleFunctionForLastKey() returns whether or
                        //  not to continue the input loop).
                        // TODO: This is handled above the switch..  so do
                        //  we still need to handle it here?
                        //bool shouldContinue = handleFunctionForLastKey(&functionExists);
                        // If the function exists, pay attention to the
                        //  return value of handleFunctionForLastKey().
                        if (functionExists)
                        {
                           //pContinueOn = shouldContinue;
                        }
                        // Go to the next form if there wasn't a function
                        //  for the last keypress.
                        else
                        {
                           if (mCycleForm)
                           {
                              if (selectNextForm())
                              {
                                 pContinueOn = false;
                                 // If we go back to the inputs, make sure
                                 //  we go to the first one.
                                 mCurrentInput = 0;
                              }
                              else
                              {
                                 pContinueOn = true;
                              }
                           }
                        }
                     }
                     break;
               }
            }
         }

         // Update the currentForm pointer
         currentForm = mForms[mCurrentForm];

         // Make sure we can cycle through the subforms the
         //  next time around.
         mCycleForm = true;

         // Update pContinueOn in case mLeaveNow has changed
         pContinueOn = (pContinueOn && !mLeaveNow);
      }
   }

   return(returnCode);
} // doSubforms

bool cxMultiForm::selectNextForm()
{
   bool wasOnLastForm = false;

   // Go to the next form.
   ++mCurrentForm;
   // Fix mCurrentForm if it went out of bounds.
   if ((unsigned)mCurrentForm >= mForms.size())
   {
      mCurrentForm = 0;
      wasOnLastForm = true;
   }

   return(wasOnLastForm);
} // selectNextForm

bool cxMultiForm::selectPrevForm()
{
   bool wasOnFirstForm = false;

   // Go to the previous form
   --mCurrentForm;
   // Fix mCurrentForm if it went out of bounds
   if (mCurrentForm < 0)
   {
      // Wrap around to the last subform
      if (mForms.size() > 0)
      {
         mCurrentForm = (int)(mForms.size()) - 1;
      }
      else
      {
         mCurrentForm = 0;
      }
   }

   return(wasOnFirstForm);
} // selectPrevForm

void cxMultiForm::addFormFunctionKeysToSubform(std::shared_ptr<cxForm>& pForm)
{
   for (const pair<const int, shared_ptr<cxFunction> >& funcPair : mKeyFunctions)
   {
      pForm->addExitKey(funcPair.first, false, true);
   }
} // addFormFunctionKeysToSubform

void cxMultiForm::addQuitAndExitKeysToSubform(std::shared_ptr<cxForm>& pForm)
{
   for (const pair<const int, bool>& quitKeyPair : mQuitKeys)
   {
      pForm->addQuitKey(quitKeyPair.first, false, true);
   }
   for (const pair<const int, bool>& exitKeyPair : mExitKeys)
   {
      pForm->addQuitKey(exitKeyPair.first, false, true);
   }
} // addQuitAndExitKeysToSubform

inline bool cxMultiForm::subformIsEnabledAndEditable(unsigned int pIndex)
{
   return(mForms[pIndex]->isEnabled() &&
          mForms[pIndex]->hasEditableInputs());
} // subformIsEnabledAndEditable
