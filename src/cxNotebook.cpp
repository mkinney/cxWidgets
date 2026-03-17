// Copyright (c) 2007 Michael H. Kinney

#include "cxNotebook.h"
#include "cxMenu.h"
#include "cxBase.h"
#include "cxStringUtils.h"
#include <exception>
#include <typeinfo> // for try/catch
#include <list>
using std::string;
using std::list;
using std::vector;
using std::map;
using cxStringUtils::TrimSpaces;
using cxStringUtils::Find;
using std::list;
using std::shared_ptr;
using std::make_shared;

cxNotebook::cxNotebook(cxWindow *pParentWindow, int pRow, int pCol, int pHeight,
                       int pWidth, bool pLabelsOnTop, bool pLeftLabelSpace,
                       bool pRightLabelSpace, int pTabSpacing,
                       cxWindow *pExtTitleWindow, cxWindow *pExtStatusWindow,
                       bool pTabNavWrap)
   : cxPanel(pParentWindow, pRow, pCol, pHeight, pWidth, "", "", "",
             eBS_NOBORDER, pExtTitleWindow, pExtStatusWindow, false),
     mLabelsOnTop(pLabelsOnTop),
     mLeftLabelSpace(pLeftLabelSpace),
     mRightLabelSpace(pRightLabelSpace),
     mTabNavKey(KEY_F(5)),
     mAllDisabledMsg("All windows in this notebook are disabled."),
     mNoWindowsMsg("This notebook has no windows."),
     mWindowDisabledMsg("That window is currently disabled."),
     mTabSpacing(pTabSpacing),
     mTabNavWrap(pTabNavWrap)
{
   // For a screen optimization, don't show the notebook's window when showing
   //  the notebook.
   setShowPanelWindow(false);
} // ctor

cxNotebook::~cxNotebook()
{
} // dtor

void cxNotebook::setLabelsOnTop(bool pLabelsOnTop, bool pRefresh)
{
   if (mLabelsOnTop != pLabelsOnTop)
   {
      mLabelsOnTop = pLabelsOnTop;
      // Now, we have to re-arrange the windows.
      // Set the new top row for the label windows, and the new top row for
      //  the panel windows.
      int labelWinTop = top();
      int panelTop = top();
      if (mLabelsOnTop)
      {
         // The panels should start 2 lines under the label window tops (the
         //  label windows are 3 lines high).
         panelTop += 2;
      }
      else
      {
         // The label windows are 3 lines high, so set labelWinTop so that the
         //  label windows' bottom row lines up with the notebook window's
         //  bottom row.
         labelWinTop = bottom() - 2;
      }

      // Move all the label windows into place.  Also, toggle their top/bottom
      // border options, and set up their special border characters so that
      // they all flow into each other.  Also, clear the panels' title &
      // status spaces.
      // This loop uses the bracket operator with an index with mLabels, since
      //  we need an index for setLabelWinSpecialChars() anyway.
      unsigned numWins = mLabels.size();
      for (unsigned index = 0; index < numWins; ++index)
      {
         // The cxWindow pointers shouldn't be nullptr, but check anyway, just to
         //  be sure.
         if (mLabels[index] != nullptr)
         {
            // Move the window
            mLabels[index]->move(labelWinTop, mLabels[index]->left(), false);
            // Toggle its top/bottom border options
            mLabels[index]->toggleBorderTop(mLabelsOnTop);
            mLabels[index]->toggleBorderBottom(!mLabelsOnTop);
            // Set its special border characters
            setLabelWinSpecialChars(index);
            setupLabelBorder(index, true);
         }
      }
      // Move all the panel windows into place
      for (shared_ptr<cxWindow>& window : mWindows)
      {
         // The cxWindow pointers shouldn't be nullptr, but check anyway, just to
         // be sure.
         if (window != nullptr)
         {
            window->move(panelTop, window->left(), false);
         }
      }

      // Refresh the notebook on the screen if they wanted to
      if (pRefresh)
      {
         show(false, false);
      }
   }
} // setLabelsOnTop

bool cxNotebook::getLabelsOnTop() const
{
   return(mLabelsOnTop);
} // getLabelsOnTop

void cxNotebook::setLeftLabelSpace(bool pLeftLabelSpace)
{
   mLeftLabelSpace = pLeftLabelSpace;
} // setLeftLabelSpace

bool cxNotebook::getLeftLabelSpace() const
{
   return(mLeftLabelSpace);
} // getLeftLabelSpace

void cxNotebook::setRightLabelSpace(bool pRightLabelSpace)
{
   mRightLabelSpace = pRightLabelSpace;
} // setRightLabelSpace

bool cxNotebook::getRightLabelSpace() const
{
   return(mRightLabelSpace);
} // getRightLabelSpace

bool cxNotebook::setTabSpacing(int pTabSpacing, bool pRefresh)
{
   bool retval = false;

   if (pTabSpacing != mTabSpacing)
   {
      // Before setting mTabSpacing, verify that the new tab spacing would not
      //  push the label windows past the right edge of the notebook window.
      int rightmostEdge = left();
      if (mLabels.size() > 0)
      {
         // Add the width of the first window.  Then, for each additional
         //  window, add pTabSpacing and the width of the window.  This is
         //  because pTabSpacing refers to the number of spaces between the
         //  windows.  We also have to subtract 2 (1 because the window columns
         //  are 0-based and 1 because the tab spacing is 0-based, i.e., a tab
         //  spacing of 0 means that the label windows share left & right
         //  borders).
         labelWinContainer::iterator iter = mLabels.begin();
         rightmostEdge += (*iter)->width();
         ++iter;
         for (; iter != mLabels.end(); ++iter)
         {
            rightmostEdge += (pTabSpacing + (*iter)->width() - 2);
         }

         // If rightmostEdge is no greater than the right edge of the notebook
         //  window, then retval should be true.
         retval = (rightmostEdge <= right());
      }
      else
      {
         retval = true;
      }

      // If we can set mTabSpacing, then set it and re-align the label windows.
      if (retval)
      {
         mTabSpacing = pTabSpacing;
         alignLabelWindows(pRefresh);
      }
   }
   else
   {
      // pTabSpacing is the same as mTabSpacing, so it's valid.
      retval = true;
   }

   return(retval);
} // setTabSpacing

int cxNotebook::getTabSpacing() const
{
   return(mTabSpacing);
} // getTabSpacing

void cxNotebook::setTabNavWrap(bool pTabNavWrap)
{
   mTabNavWrap = pTabNavWrap;
} // setTabNavWrap

bool cxNotebook::getTabNavWrap() const
{
   return(mTabNavWrap);
} // getTabNavWrap

long cxNotebook::show(bool pBringToTop, bool pShowSubwindows)
{
   long retval = cxID_EXIT;

   // Only show the noteobok if it's enabled.
   if (isEnabled())
   {
      // For consistency with other cxWindow objects, show the subwindows now
      //  if we should (note: these subwindows are not the panels, but windows
      //  that may be set up to show when this window shows.
      //  See cxWindow::addSubwindow()).
      if (pShowSubwindows && !getShowSelfBeforeSubwins())
      {
         showSubwindows(pBringToTop, pShowSubwindows);
      }

      // Set up the special border characters for the label windows and the
      //  current panel so that it looks good.
      setAllLabelWinSpecialChars();
      setupLabelBorder(mCurrentPanelIndex, false);

      // Show the label windows
      labelWinContainer::iterator labelIter = mLabels.begin();
      for (; labelIter != mLabels.end(); ++labelIter)
      {
         // The cxWindow pointer shouldn't be nullptr, but check just in case.
         if ((*labelIter) != nullptr)
         {
            (*labelIter)->show(pBringToTop, false);
         }
      }
      const std::shared_ptr<cxWindow>& iWindow = getWindow(mCurrentPanelIndex);
      if (iWindow != nullptr)
      {
         retval = iWindow->show(pBringToTop, false);
      }

      // For consistency with other cxWindow objects, show the subwindows now
      //  if we should (note: these subwindows are not the panels, but windows
      //  that may be set up to show when this window shows.
      //  See cxWindow::addSubwindow()).
      if (pShowSubwindows && getShowSelfBeforeSubwins())
      {
         showSubwindows(pBringToTop, pShowSubwindows);
      }
   }
   else
   {
      // The noteobok isn't enabled..  Make sure it's hidden.
      hide(false);
   }

   return(retval);
} // show

long cxNotebook::showModal(bool pShowSelf, bool pBringToTop, bool pShowSubwindows)
{
   setReturnCode(cxID_EXIT);

   if (isEnabled())
   {
      // Run the onFocus function.  If runOnFocusFunction() returns true, that
      //  means we should exit.. so only do the input loop if it returns false.
      //  Also, check to make sure that getLeaveNow() returns false, in case
      //  the onFocus function called exitNow() or quitNow().
      if (!runOnFocusFunction() && !getLeaveNow())
      {
         // Set mIsModal to true here (this should be done after the onFocus
         //  function runs).
         mIsModal = true;
         // Don't bring the notebook to the top, unless the parent of this
         //  notebook is a cxPanel, cxSearchPanel, or another cxNotebook.
         bool bringToTop = false;
         if (getParent() != nullptr)
         {
            string parentType = getParent()->cxTypeStr();
            if ((parentType == "cxPanel") || (parentType == "cxSearchPanel") ||
                (parentType == "cxNotebook"))
                {
               bringToTop = pBringToTop;
            }
         }

         // Show the notebook if pShowSelf is true, and do the input loop.
         if (pShowSelf)
         {
            show(bringToTop, pShowSubwindows);
         }
         bool runOnLeaveFunc = true;
         setReturnCode(doInputLoop(runOnLeaveFunc));
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

void cxNotebook::hide(bool pHideSubwindows)
{
   // Hide the panel
   cxPanel::hide(pHideSubwindows);
   // Hide the label windows
   labelWinContainer::iterator iter = mLabels.begin();
   for (; iter != mLabels.end(); ++iter)
   {
      // The cxWindow pointers shouldn't be nullptr, but check just in case.
      if ((*iter) != nullptr)
      {
         (*iter)->hide(false);
      }
   }
} // hide

void cxNotebook::unhide(bool pUnhideSubwindows)
{
   // Unhide the panel
   cxPanel::unhide(pUnhideSubwindows);
   // Unhide the label windows
   labelWinContainer::iterator iter = mLabels.begin();
   for (; iter != mLabels.end(); ++iter)
   {
      // The cxWindow pointers shouldn't be nullptr, but check just in case.
      if ((*iter) != nullptr)
      {
         (*iter)->unhide(false);
      }
   }
} // unhide

string cxNotebook::cxTypeStr() const
{
   return("cxNotebook");
} // cxTypeStr

bool cxNotebook::windowIsInNotebook(cxWindow *pWindow) const
{
   return(cxPanel::windowIsInPanel(pWindow));
} // windowIsInNotebook

shared_ptr<cxPanel> cxNotebook::append(const string& pLabel, const string& pPanelName)
{
   shared_ptr<cxPanel> iPanel;

   // See if a new tab window with pLabel will fit on the screen.  If so, we
   //  can go ahead and add the panel & tab window.
   // Figure out the width of the new label window, based on its label and
   //  label spacing.
   int labelWinWidth = (int)(pLabel.length());
   if (mLeftLabelSpace)
   {
      ++labelWinWidth;
   }
   if (mRightLabelSpace)
   {
      ++labelWinWidth;
   }
   // Add 2 to labelWinWidth to account for borders.
   labelWinWidth += 2;
   // Look at the last label window..  The new label will appear to the right
   //  of this window.
   int newLabelWinLeft = left();
   if (mLabels.size() > 0)
   {
      // The cxWindow pointer shouldn't be nullptr, but check just in case.
      if (mLabels[mLabels.size()-1] != nullptr)
      {
         newLabelWinLeft = mLabels[mLabels.size()-1]->right() + mTabSpacing;
      }
   }

   // If the new label window will fit in the notebook, then go ahead and append
   //  the label window and panel.
   if (newLabelWinLeft + labelWinWidth <= right())
   {
      // Determine the top row of the label window based on the top row of the
      //  cxNotebook and mLabelsOnTop.
      int newLabelWinTop = top();
      if (!mLabelsOnTop)
      {
         // It should be bottom() - 2 because the label window's top border
         //  will be on the same row as the bottom row of the panels.
         newLabelWinTop = bottom() - 2;
      }

      // Create the label window and add it to mLabels.
      string label;
      if (mLeftLabelSpace) { label = " "; }
      label += pLabel;
      if (mRightLabelSpace) { label += " "; }
      try
      {
         shared_ptr<cxWindow> labelWin = make_shared<cxWindow>(nullptr, newLabelWinTop, newLabelWinLeft, 3,
                                                               labelWinWidth, "", label, "", eBS_SINGLE_LINE);

         // If we were able to create the label window, then add it to mLabels
         // and create the panel.
         if (labelWin != nullptr)
         {
            // determine if we need to 'go topless' or 'go bottomless'!
            if (mLabelsOnTop)
            {
               labelWin->toggleBorderBottom(false);
            }
            else
            {
               labelWin->toggleBorderTop(false);
            }
            // Add the label window to mLabels
            mLabels.push_back(labelWin);
            // Set up the special border characters for the label window so that
            //  it blends in with the others
            setLabelWinSpecialChars(mLabels.size()-1);

            // Append a new panel
            int panelRow = top();
            int panelCol = left();
            if (mLabelsOnTop)
            {
               // The top row of the panel should be 2 rows down because the
               //  bottom row of the label window windows will be on the same
               //  row as the top row of the panels.
               panelRow += 2;
            }
            try
            {
               iPanel = make_shared<cxPanel>(nullptr, panelRow, panelCol, height()-2, width(),
                                             "", "", "", eBS_SINGLE_LINE);
            }
            catch (const std::bad_alloc& e)
            {
               // Couldn't allocate memory for the new label window
               iPanel = nullptr;
            }

            // If we were able to create the panel, we finally get to append it!
            if (iPanel != nullptr)
            {
               if (append(iPanel))
               {
                  // Set pPanelName on the panel as well as the label window so that
                  //  we can identify it from either window.
                  iPanel->setName(pPanelName);
                  labelWin->setName(pPanelName);

                  // Set up mNextTabKey, mPrevTabkey, and mTabNavKey on the panel
                  //  as exit keys so that the keypress can bubble up to this
                  //  class.
                  iPanel->addExitKey(mNextTabKey, true, true);
                  iPanel->addExitKey(mPrevTabKey, true, true);
                  iPanel->addExitKey(mTabNavKey, true, true);
               }
               else
               {
                  // Uh oh, couldn't append the panel window..  Remove the label
                  // window.
                  // Remove the cxWindow pointer from mLabels
                  labelWinContainer::iterator iter = mLabels.end();
                  --iter; // So it the iterator points to the last element
                  mLabels.erase(iter, mLabels.end());
               }
            }
            else
            {
               // Uh-oh, iPanel is nullptr..  Remove the label window that we created
               // for it.
               // Remove the cxWindow pointer from mLabels
               labelWinContainer::iterator iter = mLabels.end();
               --iter; // So it the iterator points to the last element
               mLabels.erase(iter, mLabels.end());
            }
         }
      }
      catch (...)
      {
      }
   }

   // The number of panels should be the same as the number of label windows.
   //  If for some reason they are not, throw an exception.
   if (numWindows() != mLabels.size())
   {
      throw cxWidgetsException("cxNotebook::append() - The number of panels "
                               "and labels is not the same.");
   }

   return(iPanel);
} // append

cxPanel* cxNotebook::getPanel(unsigned int pIndex) const
{
   cxPanel* iPanel = nullptr;

   // Check to make sure that pIndex is within bounds
   if ((pIndex >= 0) && (pIndex < numWindows()))
   {
      // Get the cxWindow pointer and cast it to a cxPanel pointer (cxNotebook
      // only stores cxPanels).
      const std::shared_ptr<cxWindow>& window = getWindow(pIndex);
      if (window != nullptr)
      {
         try
         {
            iPanel = dynamic_cast<cxPanel*>(window.get());
         }
         catch (const std::bad_cast& e)
         {
         }
         catch (const std::exception& e)
         {
         }
         catch (...)
         {
         }
      }
   }

   return(iPanel);
} // getPanel

cxPanel* cxNotebook::getPanel(const string& pID, bool pIsLabel) const
{
   cxPanel* iPanel = nullptr;

   if (pIsLabel)
   {
      // Note: The labels have the same index as the panel they go to.
      unsigned numLabels = mLabels.size();
      for (unsigned index = 0; index < numLabels; ++index)
      {
         // The cxWindow pointer shouldn't be nullptr, but check just in case.
         if (mLabels[index] != nullptr)
         {
            // Note: The window message has the label text.
            if (mLabels[index]->getMessage() == pID)
            {
               iPanel = getPanel(index);
               break;
            }
         }
      }
   }
   else
   {
      // pIsLabel is false.  Identifying the panel by name.
      // Note: mWindows is inherited from cxPanel.
      unsigned numWindows = mWindows.size();
      for (unsigned index = 0; index < numWindows; ++index)
      {
         // The cxWindow pointer shouldn't be nullptr, but check just in case.
         if (mWindows[index] != nullptr)
         {
            if (mWindows[index]->getName() == pID)
            {
               iPanel = getPanel(index);
               break;
            }
         }
      }
   }

   return(iPanel);
} // getPanel

cxPanel* cxNotebook::getCurrentPanelPtr() const
{
   cxPanel *currentPanel = nullptr;

   const std::shared_ptr<cxWindow>& currentWin = cxPanel::getCurrentWindowPtr();
   if (currentWin != nullptr)
   {
      currentPanel = dynamic_cast<cxPanel*>(currentWin.get());
   }

   return(currentPanel);
} // getCurrentPanelPtr

bool cxNotebook::move(int pNewRow, int pNewCol, bool pRefresh)
{
   bool retval = cxPanel::move(pNewRow, pNewCol, pRefresh);
   // Move the label windows
   alignLabelWindows(pRefresh);
   return(retval);
} // move

void cxNotebook::setShowPanelWindow(unsigned int pIndex, bool pShowPanelWindow)
{
   // Note: getPanel() does bounds checking and has a try/catch.
   cxPanel *iPanel = getPanel(pIndex);
   if (iPanel != nullptr)
   {
      iPanel->setShowPanelWindow(pShowPanelWindow);
   }
} // setShowPanelWindow

void cxNotebook::setShowPanelWindow(const string& pID, bool pShowPanelWindow,
                                    bool pIsLabel)
                                    {
   if (pIsLabel)
   {
      // Note: The labels have the same index as the panel they go to.
      unsigned numLabels = mLabels.size();
      for (unsigned index = 0; index < numLabels; ++index)
      {
         // The cxWindow pointer shouldn't be nullptr, but check just in case.
         if (mLabels[index] != nullptr)
         {
            // Note: The window message has the label text.
            if (mLabels[index]->getMessage() == pID)
            {
               // Note: getPanel() does bounds checking and has a try/catch.
               cxPanel *iPanel = getPanel(index);
               if (iPanel != nullptr)
               {
                  iPanel->setShowPanelWindow(pShowPanelWindow);
               }
               break;
            }
         }
      }
   }
   else
   {
      // pIsLabel is false.  Identifying the panel by name.
      // Note: mWindows is inherited from cxPanel.
      unsigned numWindows = mWindows.size();
      for (unsigned index = 0; index < numWindows; ++index)
      {
         // The cxWindow pointer shouldn't be nullptr, but check just in case.
         if (mWindows[index] != nullptr)
         {
            if (mWindows[index]->getName() == pID)
            {
               // Note: getPanel() does bounds checking and has a try/catch.
               cxPanel *iPanel = getPanel(index);
               if (iPanel != nullptr)
               {
                  iPanel->setShowPanelWindow(pShowPanelWindow);
               }
               break;
            }
         }
      }
   }
} // setShowPanelWindow

bool cxNotebook::getShowPanelWindow() const
{
   return cxPanel::getShowPanelWindow();
} // getShowPanelWindow

bool cxNotebook::getShowPanelWindow(unsigned int pIndex) const
{
   bool retval = false;

   // Note: We can't seem to use getPanel() here because the compiler
   //  complains about constness.

   // Do bounds checking
   if ((pIndex >= 0) && (pIndex < mWindows.size()))
   {
      const std::shared_ptr<cxWindow>& window = getWindow(pIndex);
      if (window != nullptr)
      {
         try
         {
            cxPanel *iPanel = dynamic_cast<cxPanel*>(window.get());
            if (iPanel != nullptr)
            {
               retval = iPanel->getShowPanelWindow();
            }
         }
         catch (const std::bad_cast& e)
         {
         }
         catch (const std::exception& e)
         {
         }
         catch (...)
         {
         }
      }
   }

   return(retval);
} // getShowPanelWindow

bool cxNotebook::getShowPanelWindow(const string& pID, bool pIsLabel) const
{
   bool retval = false;

   if (pIsLabel)
   {
      // Note: The labels have the same index as the panel they go to.
      unsigned numLabels = mLabels.size();
      for (unsigned index = 0; index < numLabels; ++index)
      {
         // The cxWindow pointer shouldn't be nullptr, but check just in case.
         if (mLabels[index] != nullptr)
         {
            // Note: The window message has the label text.
            if (mLabels[index]->getMessage() == pID)
            {
               // Note: getPanel() does bounds checking and has a try/catch.
               cxPanel *iPanel = getPanel(index);
               if (iPanel != nullptr)
               {
                  retval = iPanel->getShowPanelWindow();
               }
               break;
            }
         }
      }
   }
   else
   {
      // pIsLabel is false.  Identifying the panel by name.
      // Note: mWindows is inherited from cxPanel.
      unsigned numWindows = mWindows.size();
      for (unsigned index = 0; index < numWindows; ++index)
      {
         // The cxWindow pointer shouldn't be nullptr, but check just in case.
         if (mWindows[index] != nullptr)
         {
            if (mWindows[index]->getName() == pID)
            {
               // Note: getPanel() does bounds checking and has a try/catch.
               cxPanel *iPanel = getPanel(index);
               if (iPanel != nullptr)
               {
                  retval = iPanel->getShowPanelWindow();
               }
               break;
            }
         }
      }
   }

   return(retval);
} // getShowPanelWindow

void cxNotebook::showSubwindows(bool pBringToTop, bool pShowSubwindows)
{
   if (getShowSubwinsForward())
   {
      cxWindowPtrContainer::const_iterator iter = subWindows_begin();
      for (; iter != subWindows_end(); ++iter)
      {
         // Make sure the window is not one of the panels in the notebook
         if (!windowIsInPanel(*iter))
         {
            (*iter)->show(pBringToTop, pShowSubwindows);
         }
      }
   }
   else
   {
      cxWindowPtrContainer::const_reverse_iterator iter = subWindows_rbegin();
      for (; iter != subWindows_rend(); ++iter)
      {
         // Make sure the window is not one of the panels in the notebook
         if (!windowIsInPanel(*iter))
         {
            (*iter)->show(pBringToTop, pShowSubwindows);
         }
      }
   }
} // showSubwindows

string cxNotebook::getLabel(unsigned int pIndex) const
{
   string label;

   if ((pIndex >= 0) && (pIndex < mLabels.size()))
   {
      // The cxWindow pointers in mLabels shouldn't be nullptr, but check
      //  anyway, just to make sure..
      if (mLabels[pIndex] != nullptr)
      {
         label = mLabels[pIndex]->getMessage();
         // Remove leading & trailing spaces, in case left/right spacing
         //  options were used.
         TrimSpaces(label);
      }
   }

   return(label);
} // getLabel

string cxNotebook::getLabel(const string& pName) const
{
   string label;

   labelWinContainer::const_iterator iter = mLabels.begin();
   for (; iter != mLabels.end(); ++iter)
   {
      // The cxWindow pointers in mLabels shouldn't be nullptr, but check
      //  anyway, just to make sure..
      if ((*iter) != nullptr)
      {
         // Note: The panel names are set in the label windows too.
         if ((*iter)->getName() == pName)
         {
            // Note: The message text is the label text.
            label = (*iter)->getMessage();
            // Remove leading & trailing spaces, in case left/right spacing
            //  options were used.
            TrimSpaces(label);
         }
      }
   }

   return(label);
} // getlabel

bool cxNotebook::setLabel(unsigned int pIndex, const string& pLabel, bool pRefresh)
{
   bool labelWasSet = false;

   if ((pIndex >= 0) && (pIndex < mLabels.size()))
   {
      // The cxWindow pointers in mLabels shouldn't be nullptr, but check the
      //  one at pIndex anyway, just to be sure, and return if it's nullptr.
      if (mLabels[pIndex] == nullptr)
      {
         return(false);
      }

      // Based on how wide the label window would be, see if we would need
      //  to push the other label windows to the right: If they would need
      //  to be moved too far (i.e., off the screen), then we won't want to
      //  change the label.
      // Figure out the width of the label window, based on its label and
      //  label spacing.
      int labelWinWidth = (int)(pLabel.length());
      if (mLeftLabelSpace)
      {
         ++labelWinWidth;
      }
      if (mRightLabelSpace)
      {
         ++labelWinWidth;
      }
      // Add 2 to labelWinWidth to account for borders.
      labelWinWidth += 2;
      // Figure out what the new rightmost edge of all the label windows would
      // be - If it would be off the screen, then we don't want to change the
      // label.
      int changeInWidth = labelWinWidth - mLabels[pIndex]->width();
      int rightmostEdge = mLabels[mLabels.size()-1]->right() + changeInWidth;

      // If the rightmost edge will not go beyond the right side of the
      //  notebook window, then go ahead and change the label.
      if (rightmostEdge <= right())
      {
         // Set the new label in the window, and resize it (to make sure it
         //  still has a height of 3)
         string label;
         if (mLeftLabelSpace) { label = " "; }
         label += pLabel;
         if (mRightLabelSpace) { label += " "; }
         mLabels[pIndex]->setMessage(label);
         mLabels[pIndex]->resize(3, labelWinWidth, pRefresh);
         // Set up the special border characters for the label window so that
         //  it blends in with the others
         setLabelWinSpecialChars(pIndex);
         // Move the other windows so that they're all beside each other
         //  where they should be.
         alignLabelWindows(pRefresh);

         labelWasSet = true;
      }
   }

   return(labelWasSet);
} // setLabel

bool cxNotebook::setLabel(const string& pID, const string& pLabel,
                          bool pIsLabel, bool pRefresh)
                          {
   bool labelWasSet = false;

   bool setIt = false;
   unsigned index = 0;
   labelWinContainer::iterator iter = mLabels.begin();
   if (pIsLabel)
   {
      string winMessage;
      for (; iter != mLabels.end(); ++iter)
      {
         // The cxWindow pointers in mLabels shouldn't be nullptr, but check
         //  anyway, just to make sure..
         if ((*iter) != nullptr)
         {
            winMessage = (*iter)->getMessage();
            TrimSpaces(winMessage);
            if (winMessage == pID)
            {
               setIt = true;
               break;
            }
         }

         ++index;
      }
   }
   else
   {
      for (; iter != mLabels.end(); ++iter)
      {
         // The cxWindow pointers in mLabels shouldn't be nullptr, but check
         //  anyway, just to make sure..
         if ((*iter) != nullptr)
         {
            // Note: The panel names are also set in the label windows.
            if ((*iter)->getName() == pID)
            {
               setIt = true;
               break;
            }
         }

         ++index;
      }
   }

   if (setIt)
   {
      labelWasSet = setLabel(index, pLabel, pRefresh);
   }

   return(labelWasSet);
} // setLabel

void cxNotebook::setEnabled(bool pEnabled)
{
   // Call the parent setEnabled() (which will handle it for the notebook
   //  window and all the panels in the notebook)
   cxPanel::setEnabled(pEnabled);
   //  Call setEnabled() for all the label windows (which aren't in the
   //  notebook collection of windows).
   labelWinContainer::iterator iter = mLabels.begin();
   for (; iter != mLabels.end(); ++iter)
   {
      // The cxWindow pointers in mLabels shouldn't be nullptr, but check anyway,
      //  just to be sure.
      if ((*iter) != nullptr)
      {
         (*iter)->setEnabled(pEnabled);
      }
   }
} // setEnabled

void cxNotebook::setEnabled(unsigned int pIndex, bool pEnabled)
{
   cxPanel::setEnabled(pIndex, pEnabled);
} // setEnabled

void cxNotebook::setEnabled(const string& pID, bool pEnabled, bool pIsTitle)
{
   cxPanel::setEnabled(pID, pEnabled, pIsTitle);
} // setEnabled

void cxNotebook::setNextTabKey(int pKey)
{
   if (pKey != mNextTabKey)
   {
      // Remove the current next-tab key from all windows' lists of exit keys
      cxWindowPtrCollection::iterator iter = mWindows.begin();
      for (; iter != mWindows.end(); ++iter)
      {
         (*iter)->removeExitKey(mNextTabKey);
      }

      // Set the new key, and add it as an exit key to all the windows
      mNextTabKey = pKey;
      for (iter = mWindows.begin(); iter != mWindows.end(); ++iter)
      {
         (*iter)->addExitKey(mNextTabKey, true, true);
      }
   }
} // setNextTabKey

int cxNotebook::getNextTabKey() const
{
   return(mNextTabKey);
} // getNextTabKey

void cxNotebook::setPrevTabKey(int pKey)
{
   if (pKey != mPrevTabKey)
   {
      // Remove the current previous-tab key from all windows' lists of exit
      // keys
      cxWindowPtrCollection::iterator iter = mWindows.begin();
      for (; iter != mWindows.end(); ++iter)
      {
         (*iter)->removeExitKey(mPrevTabKey);
      }

      // Set the new key, and add it as an exit key to all the windows
      mPrevTabKey = pKey;
      for (iter = mWindows.begin(); iter != mWindows.end(); ++iter)
      {
         (*iter)->addExitKey(mPrevTabKey, true, true);
      }
   }
} // setPrevTabkey

int cxNotebook::getPrevTabKey() const
{
   return(mPrevTabKey);
} // getPrevTabkey

void cxNotebook::setTabNavKey(int pKey)
{
   if (pKey != mTabNavKey)
   {
      // Remove the current tab nav key from all windows' lists of exit keys
      cxWindowPtrCollection::iterator iter = mWindows.begin();
      for (; iter != mWindows.end(); ++iter)
      {
         (*iter)->removeExitKey(mTabNavKey);
      }

      // Set the new key, and add it as an exit key to all the windows
      mTabNavKey = pKey;
      for (iter = mWindows.begin(); iter != mWindows.end(); ++iter)
      {
         (*iter)->addExitKey(mTabNavKey, true, true);
      }
   }
} // setTabNavKey

int cxNotebook::getTabNavKey() const
{
   return(mTabNavKey);
} // getTabNavKey

void cxNotebook::setAllDisabledMsg(const string& pMsg)
{
   mAllDisabledMsg = pMsg;
} // setAlLDisabledMsg

const string& cxNotebook::getAllDisabledMsg() const
{
   return(mAllDisabledMsg);
} // getAllDisabledMsg

void cxNotebook::setNoWindowsMsg(const string& pMsg)
{
   mNoWindowsMsg = pMsg;
} // setNoWindowsMsg

const string& cxNotebook::getWindowDisabledMsg() const
{
   return(mWindowDisabledMsg);
} // getWindowDisabledMsg

void cxNotebook::setWindowDisabledMsg(const string& pMsg)
{
   mWindowDisabledMsg = pMsg;
} // setWindowDisabledMsg

const string& cxNotebook::getNoWindowsMsg() const
{
   return(mNoWindowsMsg);
} // getNoWindowsMsg

bool cxNotebook::mouseEvtWasInTitle() const
{
   bool inTitle = false;
#ifdef NCURSES_MOUSE_VERSION
   // This is regardless of whether the notebook has a border or not.
   inTitle = mouseEvtInRect(mMouse, top(), left(), top(), right());
#endif
   return(inTitle);
} // mouseEvtWasInTitle

shared_ptr<cxWindow> cxNotebook::removeWindow(unsigned int pIndex)
{
   shared_ptr<cxWindow> removedWindow;

   if ((pIndex >= 0) && (pIndex < numWindows()))
   {
      removedWindow = cxPanel::removeWindow(pIndex);

      // If the panel was removed (removedWindow will not be null), then remove the
      //  label window too.
      if (removedWindow != nullptr)
      {
         // Remove the label window pointer from mLabels
         labelWinContainer::iterator iter = mLabels.begin() + pIndex;
         mLabels.erase(iter);
      }

      // Now that there is a "hole" in the label window arrangement, fix it.
      //  Make sure that all the label windows are where they should be.
      alignLabelWindows(false);
   }

   return removedWindow;
} // removeWindow

void cxNotebook::removeWindow(cxWindow *pWindow)
{
   // Call the other removeWindow that takes an index
   removeWindow(getWindowIndex(pWindow));
} // removeWindow

void cxNotebook::removeWindow(const std::shared_ptr<cxWindow>& pWindow)
{
   cxPanel::removeWindow(pWindow);
} // removeWindow

void cxNotebook::delWindow(unsigned int pIndex)
{
   // TODO: This is simply a duplicate of removeWindow() now that we don't need to manually
   // free the memory here, due to the update to use std::shared_ptr
   removeWindow(pIndex);
} // delWindow

void cxNotebook::delWindow(cxWindow *pWindow)
{
   // TODO: This is simply a duplicate of removeWindow() now that we don't need to manually
   // free the memory here, due to the update to use std::shared_ptr
   // Call the other removeWindow that takes an index
   removeWindow(getWindowIndex(pWindow));
} // delWindow

void cxNotebook::delWindow(const std::shared_ptr<cxWindow>& pWindow)
{
   cxPanel::delWindow(pWindow);
} // delWindow

bool cxNotebook::setCurrentWindow(unsigned int pIndex)
{
   bool retval = false;

   if ((pIndex >= 0) && (pIndex < numWindows()))
   {
      retval = cxPanel::setCurrentWindow(pIndex);

      // If found, then update mCurrentPanelIndex.
      if (retval)
      {
         mCurrentPanelIndex = (int)pIndex;
      }
   }

   return(retval);
} // setCurrentWindow

bool cxNotebook::setCurrentWindow(const string& pTitle)
{
   bool retval = false;

   // Look for a window with the given title, and if found, call
   //  setCurrentWindow() with the index.
   int numWins = (int)(mWindows.size());
   for (int i = 0; i < numWins; ++i)
   {
      if (mWindows[i]->getTitle() == pTitle)
      {
         retval = setCurrentWindow(i);
      }
   }

   return(retval);
} // setCurrentWindow

bool cxNotebook::setCurrentWindow(const string& pID, bool pIsLabel)
{
   bool retval = false;

   // Look for a window with the given label.  If found, this will call
   //  setCurrentWindow() with the index.
   if (pIsLabel)
   {
      // Go through mLabels and look for a window with the given label
      string winMessage;
      int numWins = (int)(mLabels.size());
      for (int i = 0; i < numWins; ++i)
      {
         winMessage = mLabels[i]->getMessage();
         TrimSpaces(winMessage);
         if (winMessage == pID)
         {
            retval = setCurrentWindow(i);
            break;
         }
      }
   }
   else
   {
      int numWins = (int)(mWindows.size());
      for (int i = 0; i < numWins; ++i)
      {
         if (mWindows[i]->getName() == pID)
         {
            retval = setCurrentWindow(i);
            break;
         }
      }
   }

   return(retval);
} // setCurrentWindow

bool cxNotebook::setCurrentWindowByPtr(cxWindow *pWindow)
{
   bool retval = false;

   // Look through mWindows with a matching pointer, and if found, call
   //  setCurrentWindow() with the index.
   int numWins = (int)(mWindows.size());
   for (int i = 0; i < numWins; ++i)
   {
      if (mWindows[i].get() == pWindow)
      {
         retval = setCurrentWindow(i);
         break;
      }
   }

   return(retval);
} // setCurrentWindowByPtr

bool cxNotebook::setCurrentWindowByPtr(const std::shared_ptr<cxWindow>& pWindow)
{
   return cxPanel::setCurrentWindowByPtr(pWindow);
} // setCurrentWindowByPtr

int cxNotebook::lastClickTabIndex() const
{
   return(mLastClickTabIndex);
} // lastClickTabIndex

bool cxNotebook::swap(int pWindow1Index, int pWindow2Index)
{
   bool retval = false;

   // Only do this if pWindow1Index and pWindow2Index are within bounds and
   //  are different.
   if ((pWindow1Index != pWindow2Index) && (pWindow1Index >= 0) &&
       (pWindow1Index < (int)numWindows()) && (pWindow2Index >= 0) &&
       (pWindow2Index < (int)numWindows()))
       {
      // Call cxPanel's swap(), and if that succeeded, swap the label windows
      // too.
      retval = cxPanel::swap(pWindow1Index, pWindow2Index);
      if (retval)
      {
         mLabels[pWindow1Index].swap(mLabels[pWindow2Index]);
         // Move the label windows where they should be
         alignLabelWindows(false);
      }
   }

   return(retval);
} // swap

bool cxNotebook::selectNextWin()
{
   bool retval = false;

   if (numWindows() > 0)
   {
      // Start from mCurrentPanelIndex+1 and go to the end.  Then wrap around and
      //  go from 0 to mCurrentPanelIndex, if we couldn't select another window.
      shared_ptr<cxWindow> iWindow;
      for (int i = mCurrentPanelIndex + 1; i < (int)(numWindows()); ++i)
      {
         iWindow = getWindow(i);
         // The window pointer shouldn't be nullptr, but check anyway to be sure.
         if (iWindow != nullptr)
         {
            if (iWindow->isEnabled())
            {
               mCurrentPanelIndex = i;
               retval = true;
               break;
            }
         }
      }
      // If we didn't select another window yet, then wrap around.
      if (!retval)
      {
         for (int i = 0; i < mCurrentPanelIndex; ++i)
         {
            iWindow = getWindow(i);
            // The window pointer shouldn't be nullptr, but check anyway to be sure.
            if (iWindow != nullptr)
            {
               if (iWindow->isEnabled())
               {
                  mCurrentPanelIndex = i;
                  retval = true;
                  break;
               }
            }
         }
      }
   }

   // If we found another window, then call setCurrentWindow() to make sure
   //  the data in the parent class (cxPanel) is set.
   if (retval)
   {
      setCurrentWindow(mCurrentPanelIndex);
   }

   return(retval);
} // selectNextWin

bool cxNotebook::selectPrevWin()
{
   bool retval = false;

   if (numWindows() > 0)
   {
      // Start from mCurrentPanelIndex-1 and go to 0.  Then wrap around and
      //  go from the last one to mCurrentPanelIndex, if we couldn't select
      //  another window.
      shared_ptr<cxWindow> iWindow;
      for (int i = mCurrentPanelIndex - 1; i >= 0; --i)
      {
         iWindow = getWindow(i);
         // The window pointer shouldn't be nullptr, but check anyway to be sure.
         if (iWindow != nullptr)
         {
            if (iWindow->isEnabled())
            {
               mCurrentPanelIndex = i;
               retval = true;
               break;
            }
         }
      }
      // If we didn't select another window yet, then wrap around.
      if (!retval)
      {
         for (int i = (int)(numWindows()-1); i > mCurrentPanelIndex; --i)
         {
            iWindow = getWindow(i);
            // The window pointer shouldn't be nullptr, but check anyway to be sure.
            if (iWindow != nullptr)
            {
               if (iWindow->isEnabled())
               {
                  mCurrentPanelIndex = i;
                  retval = true;
                  break;
               }
            }
         }
      }
   }

   // If we found another window, then call setCurrentWindow() to make sure
   //  the data in the parent class (cxPanel) is set.
   if (retval)
   {
      setCurrentWindow(mCurrentPanelIndex);
   }

   return(retval);
} // selectPrevWin

bool cxNotebook::swap(const std::shared_ptr<cxWindow>& pWindow1, const std::shared_ptr<cxWindow>& pWindow2)
{
   return swap(pWindow1.get(), pWindow2.get());
} // swap

bool cxNotebook::swap(cxWindow *pWindow1, cxWindow *pWindow2)
{
   bool retval = false;

   // Look for the indexes of the 2 windows and call the other swap().
   int winIndex1 = -1;
   int winIndex2 = -1;
   int numWins = (int)numWindows();
   cxWindow *winPtr = nullptr;
   for (int i = 0; i < numWins; ++i)
   {
      winPtr = mWindows[i].get();
      if (winPtr == pWindow1)
      {
         winIndex1 = i;
      }
      if (winPtr == pWindow2)
      {
         winIndex2 = i;
      }

      // If both have been set, then stop looping.
      if ((winIndex1 > -1) && (winIndex2 > -1))
      {
         break;
      }
   }

   // If both indexes have been set, then call the other swap() with the
   //  indexes.
   if ((winIndex1 > -1) && (winIndex2 > -1))
   {
      retval = swap(winIndex1, winIndex2);
   }

   return(retval);
} // swap

//// Private methods

long cxNotebook::doInputLoop(bool& pRunOnLeaveFunction)
{
   pRunOnLeaveFunction = true;
   long returnCode = cxID_EXIT;

   // Do this if there is at least 1 panel.
   if (numWindows() > 0)
   {
      // If none of the windows are enabled, then output a message box to
      //  let the user know, and don't do the input loop.
      if (!anyEnabledWindows())
      {
         cxBase::messageBox(mAllDisabledMsg);
         return(cxID_EXIT);
      }

      // If mLeaveNow is set to true while the panel is modal (via a hotkey
      //  function, etc.), then the input loop will quit.
      mLeaveNow = false;

      // For dragging the window around
      int pressedX = 0;
      int pressedY = 0;
      bool movingWin = false;

      int lastKey = NOKEY;
      // iWindow will be a pointer to the current cxPanel (it's a cxWindow*
      //  so that it's more generic, in case we ever want to change this class
      //  to store more than just cxPanels)
      shared_ptr<cxWindow> iWindow;
      bool continueOn = true;
      while (continueOn && !mLeaveNow)
      {
         // If mCurrentPanelIndex is out of bounds for some reason, then
         //  reset it.
         if ((mCurrentPanelIndex < 0) || (mCurrentPanelIndex >= (int)(numWindows())))
         {
            mCurrentPanelIndex = 0;
            // If the first window is disabled, then select the next available
            //  one.  If there are no other available windows, then notify the
            //  user and exit this loop.
            iWindow = getWindow(mCurrentPanelIndex);
            if (iWindow != nullptr)
            {
               if (!(iWindow->isEnabled()))
               {
                  if (!selectNextWin())
                  {
                     cxBase::messageBox(mAllDisabledMsg);
                     return(cxID_EXIT);
                  }
               }
            }
         }
         // Update the current window information in the parent class (so that
         //  calls to getCurrentWindowIndex(), etc. will return the correct
         //  value).  Preserve the value of mCycleWin (inherited from cxPanel),
         //  because setCurrentWindow() might change it.
         const bool cycleWinBackup = mCycleWin;
         cxPanel::setCurrentWindow(mCurrentPanelIndex);
         mCycleWin = cycleWinBackup;
         // mLastWindow is inherited from cxPanel
         mLastWindow = getWindow(mCurrentPanelIndex);

         // ensure the top (or bottom) line shows the lines correctly
         setupLabelBorder(mCurrentPanelIndex, false);

         // Call showCurrentWindow() to update the current window's border
         //  characters and tab attributes on the screen
         showCurrentWindow(true, true);
         // Show the current window modally
         iWindow = getWindow(mCurrentPanelIndex);
         if (iWindow != nullptr)
         {
            iWindow->showModal();
            lastKey = iWindow->getLastKey();
            setLastKey(lastKey);
            // Handle mouse events first
#ifdef NCURSES_MOUSE_VERSION
            if (lastKey == KEY_MOUSE)
            {
               mMouse = iWindow->getMouseEvent();
               // Run a function that may exist for the mouse state.  If
               //  no function exists for the mouse state, then process
               //  it here.
               bool mouseFuncExists = false;
               continueOn = handleFunctionForLastMouseState(&mouseFuncExists,
                                                       &pRunOnLeaveFunction);
               if (!mouseFuncExists)
               {
                  switch (mMouse.bstate)
                  {
                     // If the user clicked button 1 inside the notebook window,
                     //  then look to see if they clicked the mouse in one of the
                     //  tab windows.  If so, go to that tab.
                     case BUTTON1_CLICKED:
                        if (mouseEvtWasInWindow())
                        {
                           mLastClickTabIndex = -1;
                           for (int i = 0; i < (int)(mLabels.size()); ++i)
                           {
                              // The cxWindow pointers shouldn't be nullptr, but check
                              //  anyway, just to be sure.
                              if (mLabels[i] != nullptr)
                              {
                                 if (mLabels[i]->pointIsInWindow(mMouse.y, mMouse.x))
                                 {
                                    // Only select the panel if it's enabled.
                                    shared_ptr<cxWindow> tmpWindow = getWindow(i);
                                    if (tmpWindow != nullptr)
                                    {
                                       if (tmpWindow->isEnabled())
                                       {
                                          mCurrentPanelIndex = i;
                                          mLastClickTabIndex = i;
                                       }
                                       else
                                       {
                                          // The window is disabled - Show a
                                          //  message to the user.
                                          cxBase::messageBox(mWindowDisabledMsg);
                                       }
                                    }
                                    break;
                                 }
                              }
                           }
                        }
                        else
                        {
                           mLastClickTabIndex = -1;
                        }
                        break;
                        // Note: Dragging the window doesn't seem to work.
                     case BUTTON1_PRESSED:
                        // Update pressedX and pressedY
                        pressedX = mMouse.x;
                        pressedY = mMouse.y;
                        // If they clicked in the title, then let them
                        //  move the window.
                        movingWin = mouseEvtWasInTitle();
                        break;
                     case BUTTON1_RELEASED:
                        // If the user is moving the window, then go ahead and
                        //  move it.
                        if (movingWin)
                        {
                           moveRelative(mMouse.y - pressedY, mMouse.x - pressedX, true);
                           movingWin = false; // Reset the moving of the window
                        }
                        break;
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
            //  If not there either, handle the key normally.
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
            // If there was no function for the last keypress, then see if
            //  the user pressed one of the navigation keys, and if so,
            //  go onto the next/previous panel.  We can't do this inside
            //  of a switch, because case statements need to use const
            //  variables or literals.
            else if (lastKey == mNextTabKey)
            {
               bool goToNextWin = true;

               // If the user is on the last enabled window, then only let them
               //  go to the next one if mTabNavWrap is true.
               if (lastEnabledWindow(mCurrentPanelIndex))
               {
                  goToNextWin = mTabNavWrap;
               }

               if (goToNextWin)
               {
                  selectNextWin();
               }
            }
            else if (lastKey == mPrevTabKey)
            {
               bool goToPrevWin = true;

               // If the user is on the first enabled window, then only let them
               //  go to the previous one if mTabNavWrap is true.
               if (firstEnabledWindow(mCurrentPanelIndex))
               {
                  goToPrevWin = mTabNavWrap;
               }

               if (goToPrevWin)
               {
                  selectPrevWin();
               }
            }
            // If the last key is mTabNavKey, then bring up the tab navigation
            //  menu.
            else if (lastKey == mTabNavKey)
            {
               showNavMenu();
            }
            else
            {
               // Run a function that may be associated with the last
               //  keypress.  If no function exists for the key, then
               //  go on to the next/previous window.
               bool functionExists = false;
               continueOn = continueOn && handleFunctionForLastKey(&functionExists);
               // Quit out of the input loop if continueOn was set false.
               if (!continueOn)
               {
                  break;
               }

               // Quit out of the input loop if mLeaveNow was set true by an
               //  external hotkey function or other event function, and if
               //  we're allowed to quit.
               if (mLeaveNow)
               {
                  // Depending on the return value (possibly set by exitNow()
                  //  or quitNow()), check to see if we're allowed to exit/quit
                  //  before exiting.
                  bool canQuit = true;
                  if (getReturnCode() == cxID_QUIT)
                  {
                     canQuit = getAllowQuit();
                  }
                  else
                  {
                     canQuit = getAllowExit();
                  }

                  if (canQuit)
                  {
                     break;
                  }
               }

               // If no function exists for the last keypress, then check the
               //  last keypress.
               if (!functionExists)
               {
                  switch (lastKey)
                  {
                     case ESC:
                        if (getAllowQuit())
                        {
                           returnCode = cxID_QUIT;
                           continueOn = false;
                        }
                        break;
                     default:
                        break;
                  }
               }
            }
         }
         else
         {
            throw cxWidgetsException("cxNotebook::doInputLoop() - nullptr panel pointer");
         }

         // Make sure there is still at least one enabled panel (it's possible
         //  that an event function could disable a panel).  Don't continue on
         //  if there are no more enabled windows.
         if (continueOn && !anyEnabledWindows())
         {
            continueOn = false;
            cxBase::messageBox(mAllDisabledMsg);
         }
      }
   }
   else
   {
      cxBase::messageBox(mNoWindowsMsg);
   }

   // mLeaveNow should be false.
   mLeaveNow = false;

   return(returnCode);
} // doInputLoop

void cxNotebook::showCurrentWindow(bool pApplyLabelAttr, bool pBringToTop)
{
   // Show the current panel and hide the others.

   // What is our label highlight attribute going to be?
   attr_t labelHighlight=A_BOLD;

   int numWins = (int)(numWindows());
   for (int i = 0; i < numWins; ++i)
   {
      // The cxWindow pointer shouldn't be nullptr, but check anyway.
      if (mWindows[i] != nullptr)
      {
         // The window to show
         if (i == mCurrentPanelIndex)
         {
            if (mWindows[mCurrentPanelIndex] != nullptr)
            {
               // Make the label attribute be "different"
               if (pApplyLabelAttr)
               {
                  mLabels[mCurrentPanelIndex]->setAttr(eMESSAGE, labelHighlight);
               }

               // Show the label and the panel
               mLabels[mCurrentPanelIndex]->show(pBringToTop, false);
               mWindows[mCurrentPanelIndex]->show(pBringToTop, false);
            }
         }
         else
         {
            // Hide the panel
            mWindows[i]->hide(false);

            // if the label is not already NORMAL, set it to normal and re-draw it
            // We check first so we do not re-paint all of the labels if we do
            // not have to. (screen redrawing optimization)
            if (pApplyLabelAttr)
            {
               if (mLabels[i]->hasAttr(eMESSAGE, labelHighlight))
               {
                  // Apply the normal message attribute to the label
                  mLabels[i]->setAttr(eMESSAGE, A_NORMAL);
               }
            }
            mLabels[i]->show(pBringToTop, false);
         }
      }
   }
} // showCurrentWindow

void cxNotebook::setLabelWinSpecialChars(unsigned int pIndex)
{
   if ((pIndex >= 0) && (pIndex < mLabels.size()))
   {
      // The cxWindow pointers in mLabels shouldn't be nullptr, but check anyway
      //  just to be sure.
      if (mLabels[pIndex] != nullptr)
      {
         // Clear the window's special characters
         mLabels[pIndex]->clearSpecialChars();
         // If the tabs tyle is having them together (sharing left & right
         //  borders), then add tee characters to the windows so that they
         //  flow together.
         // If the tab spacing is 0 (so that they share left/right borders),
         //  then add tee characters to the windows so that they all flow
         //  together.
         unsigned lastLabelWindowIndex = mLabels.size() - 1;
         if (mTabSpacing == 0)
         {
            // If there is a label window to the right, there should be a tee
            //  character on the right side of the label window.  Also, the
            //  window to the right should have a tee character on its left.
            //  Check mLabelsOnTop to see if it should go on the top or bottom.
            if (pIndex < lastLabelWindowIndex)
            {
               // Add tee characters to the current window
               if (mLabelsOnTop)
               {
                  mLabels[pIndex]->addSpecialChar(0, mLabels[pIndex]->width()-1, ACS_TTEE);
               }
               else
               {
                  mLabels[pIndex]->addSpecialChar(mLabels[pIndex]->height()-1,
                        mLabels[pIndex]->width()-1, ACS_BTEE);
               }
               // Add tee characters to the window on the right
               shared_ptr<cxWindow> rightLabelWin(mLabels[pIndex+1]);
               if (rightLabelWin != nullptr)
               {
                  if (mLabelsOnTop)
                  {
                     rightLabelWin->addSpecialChar(0, 0, ACS_TTEE);
                  }
                  else
                  {
                     rightLabelWin->addSpecialChar(rightLabelWin->height()-1, 0,
                           ACS_BTEE);
                  }
               }
            }
            // If there is a label window to the left, there should be a tee
            //  character on the left side of the label window.  Also, the window
            //  to the left should have a tee character on its right.  Check
            //  mLabelsOnTop to see if it should go on the top or bottom.
            if (pIndex > 0)
            {
               if (mLabelsOnTop)
               {
                  mLabels[pIndex]->addSpecialChar(0, 0, ACS_TTEE);
               }
               else
               {
                  mLabels[pIndex]->addSpecialChar(mLabels[pIndex]->height()-1, 0, ACS_BTEE);
               }
               // Add tee characters to the window on the left
               shared_ptr<cxWindow> leftLabelWin(mLabels[pIndex-1]);
               if (leftLabelWin != nullptr)
               {
                  if (mLabelsOnTop)
                  {
                     leftLabelWin->addSpecialChar(0, leftLabelWin->width()-1,
                           ACS_TTEE);
                  }
                  else
                  {
                     leftLabelWin->addSpecialChar(leftLabelWin->height()-1,
                           leftLabelWin->width()-1,
                           ACS_BTEE);
                  }
               }
            }
         }
         else
         {
            // Tab spacing is not 0.  If there are label windows to the left &
            //  right, remove their special characters.  (Special characters
            //  have already been removed from the current label window.)
            if (pIndex < lastLabelWindowIndex)
            {
               // There is a label window to the right
               if (mLabels[pIndex+1] != nullptr)
               {
                  mLabels[pIndex+1]->clearSpecialChars();
               }
            }
            if (pIndex > 0)
            {
               // There is a label window to the left
               if (mLabels[pIndex-1] != nullptr)
               {
                  mLabels[pIndex-1]->clearSpecialChars();
               }
            }
         }
      }
   }
} // setLabelWinSpecialChars

inline void cxNotebook::setAllLabelWinSpecialChars()
{
   // Call setLabelWinSpecialChars() for each label window.
   unsigned numLabels = mLabels.size();
   for (unsigned i = 0; i < numLabels; ++i)
   {
      setLabelWinSpecialChars(i);
   }
} // setAllLabelWinSpecialChars

void cxNotebook::alignLabelWindows(bool pRefresh)
{
   int winRow = top();
   int winCol = left();
   if (!mLabelsOnTop)
   {
      winRow = bottom() - 2;
   }
   unsigned numLabels = mLabels.size();
   for (unsigned i = 0; i < numLabels; ++i)
   {
      // Set up the special border characters for the window.  (Note: It's
      //  important that this be done first so that if pRefresh is true,
      //  the window will show up on the screen with the correct special
      //  border characters, or none at all if it shouldn't have any.)
      setLabelWinSpecialChars(i);
      // Move the window (and optionally refresh the screen)
      // The cxWindow pointers in mLabels shouldn't be nullptr, but check just
      //  in case.
      if (mLabels[i] != nullptr)
      {
         mLabels[i]->move(winRow, winCol, pRefresh);
         // Update winCol
         winCol = mLabels[i]->right() + mTabSpacing;
      }
   }

   // If pRefresh is true, then show the current panel so that the panel's
   //  special characters appear on the screen.
   if (pRefresh)
   {
      setupLabelBorder(mCurrentPanelIndex, false); // Make sure the panel has its special chars
      shared_ptr<cxWindow> iWindow = getWindow(mCurrentPanelIndex);
      if (iWindow != nullptr)
      {
         iWindow->show(false, false);
      }
   }
} // alignLabelWindows

// These methods are private so that they can't be called from outside this
//  class.  They are inherited from cxPanel.

bool cxNotebook::getExitOnLeaveLast() const
{
   return(cxPanel::getExitOnLeaveLast());
} // getExitOnLeaveLast

void cxNotebook::setExitOnLeaveLast(bool pExitOnLeaveLast)
{
   cxPanel::setExitOnLeaveLast(pExitOnLeaveLast);
} // setExitOnLeaveLast

bool cxNotebook::getExitOnLeaveFirst() const
{
   return(cxPanel::getExitOnLeaveFirst());
} // getExitOnLeaveFirst

void cxNotebook::setExitOnLeaveFirst(bool pExitOnLeaveFirst)
{
   cxPanel::setExitOnLeaveFirst(pExitOnLeaveFirst);
} // setExitOnLeaveFirst

bool cxNotebook::windowIsInPanel(cxWindow *pWindow) const
{
   return(cxPanel::windowIsInPanel(pWindow));
} // windowIsInPanel

bool cxNotebook::windowIsInPanel(const std::shared_ptr<cxWindow>& pWindow) const
{
   return cxPanel::windowIsInPanel(pWindow);
} // windowIsInPanel

bool cxNotebook::append(const shared_ptr<cxWindow>& pWindow)
{
   return(cxPanel::append(pWindow));
} // append

bool cxNotebook::append(const shared_ptr<cxWindow>& pWindow, int pRow, int pCol, bool pRefresh)
{
   return(cxPanel::append(pWindow, pRow, pCol, pRefresh));
} // append

void cxNotebook::setShowPanelWindow(bool pShowPanelWindow)
{
   cxPanel::setShowPanelWindow(pShowPanelWindow);
} // setShowPanelWindow

// The following 2 functions are private to dis-allow copy construction and
//  assignment.

cxNotebook::cxNotebook(const cxNotebook& pNotebook)
{
}

cxNotebook& cxNotebook::operator =(const cxNotebook& pNotebook)
{
   return(*this);
}

//// Other private methods

void cxNotebook::setupLabelBorder(int pIndex, bool pClear)
{
   // If pIndex is out of bounds, then just return.
   if ((pIndex < 0) || (pIndex >= (int)(mWindows.size())))
   {
      return;
   }

   // iWindow is the panel inside the notebook.  This uses a cxWindow pointer
   //  to be more generic (this doesn't do any cxPanel-specific stuff).
   std::shared_ptr<cxWindow> iWindow = getWindow(pIndex);
   if (iWindow != nullptr)
   {
      // If pClear is true, then remove the special border characters.
      //  Otherwise, add the spaces & border characters.
      if (pClear)
      {
         map<int, string> strMap;
         // Remove the space strings from the title
         iWindow->getTitleStrings(strMap);
         removeSpaceStrings(strMap);
         iWindow->setTitleStrings(strMap);
         // Remove space strings from the status
         iWindow->getStatusStrings(strMap);
         removeSpaceStrings(strMap);
         iWindow->setStatusStrings(strMap);
         // Remove the special border characters
         iWindow->clearSpecialChars();
      }
      else
      {
         // Figure out the left & right boundaries of where to add spaces in the
         //  title/status.  This is based on the left/right column of the label
         //  window minus the left edge of the notebook window.
         int leftPos = mLabels[pIndex]->left() - left();
         int rightPos = mLabels[pIndex]->right() - left();

         // Construct a string full of spaces and add it to a map to be set in
         //  the title/status line.
         string spaces;
         spaces.append(rightPos - leftPos - 1, ' ');

         // Depending on the value of mLabelsOnTop, add the spaces, and also add
         //  special border characters on the top or bottom so that the label
         //  windows and the panel blend together.
         iWindow->clearSpecialChars(); // Reset the special characters
         // Set up the left & right corner characters & row to add them to for the
         //  blank space in the panel border.  Default these as if the label
         //  windows are on top, but change them if the label windows are on the
         //  bottom.
         int row = 0;
         chtype leftCornerChar = ACS_LRCORNER;
         chtype rightCornerChar = ACS_LLCORNER;
         chtype horizontalTee = ACS_BTEE;
         if (mLabelsOnTop)
         {
            // Get the current title strings, look for strings that are all
            //  spaces, and remove them.  Then add the new spaces at the new
            //  location, and set the title string map back in the window.
            //  This preserves any other title strings that are set in the
            //  window.
            map<int, string> titleStrings;
            iWindow->getTitleStrings(titleStrings);
            removeSpaceStrings(titleStrings);
            // Set the title strings back in the window
            titleStrings[leftPos+1] = spaces;
            iWindow->setTitleStrings(titleStrings);
         }
         else
         {
            // Get the current status strings, look for strings that are all
            //  spaces, and remove them.  Then add the new spaces at the new
            //  location, and set the status string map back in the window.
            //  This preserves any other status strings that are set in the
            //  window.
            map<int, string> statusStrings;
            iWindow->getStatusStrings(statusStrings);
            removeSpaceStrings(statusStrings);
            // Set the status strings back in the window
            statusStrings[leftPos+1] = spaces;
            iWindow->setStatusStrings(statusStrings);

            // Set the row & border characters to what they should be
            row = iWindow->height()-1;
            leftCornerChar = ACS_URCORNER;
            rightCornerChar = ACS_ULCORNER;
            horizontalTee = ACS_TTEE;
         }

         // Add the special characters to the panel window
         // Special case for the left side: Add a vertical line
         if (leftPos == 0)
         {
            iWindow->addSpecialChar(row, 0, ACS_VLINE);
         }
         else
         {
            iWindow->addSpecialChar(row, 0, ACS_LTEE);
            iWindow->addSpecialChar(row, leftPos, leftCornerChar);
         }

         // Special case for the right side: Add a vertical line
         if (rightPos == iWindow->width() - 1)
         {
            iWindow->addSpecialChar(0, iWindow->width()-1, ACS_VLINE);
         }
         else
         {
            // If there is a label window whose right edge lines up with the
            //  panel's right edge, then add a vertical line there.
            if (rightPos == iWindow->width()-1)
            {
               iWindow->addSpecialChar(row, rightPos, ACS_VLINE);
            }
            else
            {
               iWindow->addSpecialChar(row, rightPos, rightCornerChar);
               // If the last label window's right edge is the same right edge of
               //  the notebook, then add a right tee character on the right edge.
               if (mLabels[mLabels.size()-1]->right() == right())
               {
                  iWindow->addSpecialChar(row, iWindow->width()-1, ACS_RTEE);
               }
            }
         }

         // Go through the other label windows and see if we need to add
         //  horizontal tee characters to the panel so that the label windows
         //  blend in with the panel window.
         bool addLeft = true;   // Whether or not to add the left character
         bool addRight = true;  // Whether or not to add the right character
         // Store copies of the original leftPos & rightPos for comparisons
         int originalLeftPos = leftPos;
         int originalRightPos = rightPos;
         int numLabels = (int)(mLabels.size());
         for (int i = 0; i < numLabels; ++i)
         {
            // Skip the current label..  We already did that one.
            if (i == pIndex) { continue; }

            leftPos = mLabels[i]->left() - left();
            rightPos = mLabels[i]->right() - left();
            // Add the left chracter if:
            //  - leftPos is not on the left border
            //  - leftPos is neither originalLeftPos nor originalRightPos
            // Add the right character if:
            //  - rightPos is not on the right border
            //  - rightPos is neither originalRightPos nor originalLeftPos
            addLeft = ((leftPos != 0) && (leftPos != originalLeftPos) &&
                       (leftPos != originalRightPos));
            addRight = ((rightPos != iWindow->width()-1) &&
                        (rightPos != originalRightPos) &&
                        (rightPos != originalLeftPos));

            if (addLeft)
            {
               iWindow->addSpecialChar(row, leftPos, horizontalTee);
            }
            if (addRight)
            {
               iWindow->addSpecialChar(row, rightPos, horizontalTee);
            }
         }
      }
   }
} // setupLabelBorder

void cxNotebook::removeSpaceStrings(map<int, string>& pStringMap)
{
   // This uses a list of integers to keep track of all status strings
   //  that are all spaces.
   list<int> spaceStrLocations;
   map<int, string>::iterator iter = pStringMap.begin();
   for (; iter != pStringMap.end(); ++iter)
   {
      if (Find(iter->second, "^ +$", true))
      {
         spaceStrLocations.push_back(iter->first);
      }
   }
   // Remove the all-space status strings
   list<int>::iterator locIter = spaceStrLocations.begin();
   for (; locIter != spaceStrLocations.end(); ++locIter)
   {
      pStringMap.erase(*locIter);
   }
} // removeSpaceStrings

void cxNotebook::showNavMenu()
{
   // Make sure there are some labels in the notebook first
   if (mLabels.size() > 0)
   {
      cxMenu iMenu(this, 0, 0, 2, 13, "Select page", nullptr, nullptr,
                   eBS_SINGLE_LINE);
      // Add a menu item for each tab, with a "&" at the beginning of the text
      //  to enable hotkeys.  Each menu item's return code will be the index
      //  for the window.
      string labelText;
      long numLabels = (long)(mLabels.size());
      for (long i = 0; i < numLabels; ++i)
      {
         labelText = mLabels[i]->getMessage();
         TrimSpaces(labelText); // Remove any leading/trailing spaces
         iMenu.append("&" + labelText, i, "", cxITEM_NORMAL, true);
      }
      // Center the menu in the notebook window (and don't refresh it)
      iMenu.center(false);
      // The current menu item should be the current tab
      iMenu.setCurrentMenuItem(getCurrentWindowIndex(), false);
      // Show the menu, and if the user selected a tab (other than the current
      //  tab), then go to it.
      long retval = iMenu.showModal(true, true, false);
      if (iMenu.itemWasSelected())
      {
         if (getCurrentWindowIndex() != (int)retval)
         {
            // Only go to the window if the window is enabled.
            if (getWindow((unsigned)retval)->isEnabled())
            {
               cxPanel *currentPanel = getCurrentPanelPtr();
               currentPanel->exitNow();
               setCurrentWindow((unsigned)retval);
            }
            else
            {
               // The window is disabled - Show a message to the user.
               cxBase::messageBox(mWindowDisabledMsg);
            }
         }
      }
   }
} // showNavMenu
