#ifndef __CXNOTEBOOK_H__
#define __CXNOTEBOOK_H__

// Copyright (c) 2007 Michael H. Kinney

#include "cxPanel.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

/** \class cxNotebook
 * \brief This class represents a notebook control, which manages multiple
 * \brief cxPanels with associated tabs.  This derives from cxPanel; this
 * \brief class is similar to a cxPanel, except that this class does not
 * \brief automatically go to the next/previous window - the user has to click
 * \brief on the associated tab or press the pageUp/pageDown keys to go between
 * \brief tabs (and those keys may be changed).<br>
 * \brief <br>
 * \brief An alternate way to navigate between the tabs is with a keypress,
 * \brief which brings up a menu listing the tab labels, allowing the user to
 * \brief choose a tab.  By default, this key is F5; however, this can be
 * \brief changed with the setTabNavKey() method.<br>
 * \brief By default, the user is able to wrap between the first and last
 * \brief ediable tabs using the tab navigation keys.  This behavior can be
 * \brief changed with a call to setTabNavWrap().
 *
 * \author Mike Kinney
 * \author Eric Oulashin
 * \version $Revision: 1.31 $
 * \date $Date: 2007/12/04 00:23:46 $
*/
class cxNotebook: public cxPanel
{

   public:

      /**
       * \brief Constructor
       *
       * @param pParentWindow A pointer to the parent window.  Can be nullptr.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the window.
       * @param pWidth The width of the window.
       * @param pLabelsOnTop Whether or not the labels should be on top.  If
       *  false, the labels will be on the bottom.  This defaults to true
       *  (labels on top).
       * @param pLeftLabelSpace Whether or not to have a space to the left of
       *  label text.  Defaults to true.
       * @param pRightLabelSpace Whether or not to have a space to the right
       *  of label text.  Defaults to true.
       * @param pTabSpacing The number of spaces to use for horizontal
       *  alignment of the tabs (label windows).  0 would be sharing left/right
       *  borders, 1 would be right next to each other, etc.  This defaults to
       *  0.
       * @param pExtTitleWindow A pointer to another cxWindow in which to
       *  display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow in which to
       *  display the status.
       * @param pTabNavWrap Whether or not to enable the user to wrap between
       *  the first and last enabled tabs using the navigation keys.  Defaults
       *  to true.
       */
      explicit cxNotebook(cxWindow *pParentWindow = nullptr,
                          int pRow = 0, int pCol = 0,
                          int pHeight = DEFAULT_HEIGHT,
                          int pWidth = DEFAULT_WIDTH,
                          bool pLabelsOnTop = true,
                          bool pLeftLabelSpace = true,
                          bool pRightLabelSpace = true,
                          int pTabSpacing = 0,
                          cxWindow *pExtTitleWindow = nullptr,
                          cxWindow *pExtStatusWindow = nullptr,
                          bool pTabNavWrap = true);

      virtual ~cxNotebook();

      /**
       * \brief Toggles whether the labels should be on the top or bottom.
       *
       * @param pLabelsOnTop Boolean: If true, the labels should be on top.  If
       *  false, the labels should be on the bottom.
       * @param pRefresh Whether or not to refresh the screen.  Defaults to
       *  false.
       */
      void setLabelsOnTop(bool pLabelsOnTop, bool pRefresh = false);

      /**
       * \brief Returns whether the labels are set to appear on top.
       *
       * @return Boolean: true if the labels are set to appear on top, or
       *  false if they are to appear on the bottom.
       */
      bool getLabelsOnTop() const;

      /**
       * \brief Toggles the option to have a space to the left of label text.
       *
       * @param pLeftLabelSpace Boolean: If true, then new label text will have
       *  a space to the left.  If false, new label text will not.
       */
      void setLeftLabelSpace(bool pLeftLabelSpace);

      /**
       * \brief Returns the currently-set option of having a space to the left
       * \brief of label text.
       *
       * @return Boolean: true if it's enabled, false if not.
       */
      bool getLeftLabelSpace() const;

      /**
       * \brief Toggles the option to have a space to the right of label text.
       *
       * @param pRightLabelSpace Boolean: If true, then new label text will have
       *  a space to the right.  If false, new label text will not.
       */
      void setRightLabelSpace(bool pRightLabelSpace);

      /**
       * \brief Returns the currently-set option of having a space to the right
       * \brief of label text.
       *
       * @return Boolean: true if it's enabled, false if not.
       */
      bool getRightLabelSpace() const;

      /**
       * \brief Sets the number of spaces to use for horizontal alignment of
       * \brief the tabs (label windows).
       *
       * @param pTabSpacing The number of spaces to use for horizontal tab
       *  alignment.  0 would be sharing left/right borders, 1 would be right
       *  next to each other, 2 means there is 1 space between the tabs, etc.
       * @param pRefresh Whether or not to refresh the screen.  Defaults to
       *  false.
       *
       * @return Boolean: true if the tab spacing was set, or false if not.
       *  For example, this may not set the tab spacing if the new spacing
       *  would make any of the label windows go past the right edge of the
       *  notebook window.  If pTabSpacing is the same as what is already set,
       *  then this will return true.
       */
      bool setTabSpacing(int pTabSpacing, bool pRefresh = false);

      /**
       * \brief Returns the number of spaces being used for horizontal tab
       * \brief alignment.
       *
       * @return The number of spaces being used for horizontal tab alignment
       */
      int getTabSpacing() const;

      /**
       * \brief Sets whether or not the user will be able to wrap from the
       * \brief last enabled tab to the first, or from the first enabled tab
       * \brief to the last, when using the tab navigation keys.
       *
       * @param pTabNavWrap Whether or not to allow tab "wrapping" using the
       *  tab navigation keys
       */
      void setTabNavWrap(bool pTabNavWrap);

      /**
       * \brief Returns whether or not wrapping is allowed between the first
       * \brief and last enabled tabs using the navigation keys.
       *
       * @return Whether or not wrapping is allowed between the first and last
       *  enabled tabs using the navigation keys
       */
      bool getTabNavWrap() const;

      /**
       * \brief Shows the notebook.
       *
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *   to false.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults
       *   to true.
       * @return This method returns a long to support overriding in derived classes
       *   that might need to return a value.  For cxWindow, cxID_EXIT is
       *   always returned.
       */
      virtual long show(bool pBringToTop = false, bool pShowSubwindows = true) override;

      /**
       * \brief Shows the notebook and allows the user to interact with the
       * \brief pages.  Shows the current page.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *  to true.
       * @param pShowSubwindows
       *
       * @return A return code based on user input.  Returns cxID_QUIT if the user
       *  presses ESC; otherwise, returns cxID_EXIT.
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = true) override;

       /**
       * \brief Hides the notebook
       *
       * @param pHideSubwindows Whether or not to also hide the subwindows (defaults
       *    to true)
       */
      virtual void hide(bool pHideSubwindows = true) override;

      /**
       * \brief Un-hides the notebook
       *
       * @param pUnhideSubwindows Whether or not to also unhide the subwindows
       *    (defaults to true)
       */
      virtual void unhide(bool pUnhideSubwindows = true) override;

      /**
       * \brief Returns the class type as a string ("cxNotebook")
       *
       * @return The class type as a string ("cxNotebook")
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief Returns whether a window exists in the notebook.
       *
       * @param pWindow A pointer to a window to add to the notebook
       *
       * @return true if the window is in the notebook, or false if not
       */
      virtual bool windowIsInNotebook(cxWindow *pWindow) const;

      /**
       * \brief Appends a new panel & tab to the notebook.  Returns a pointer
       * \brief to the cxPanel that was added.  If the panel can't be added
       * \brief (i.e., if the label is too long for the tab to fit on the
       * \brief screen), this won't create the panel, and this will return
       * \brief nullptr.
       *
       * @param pLabel The label for the panel
       * @param pPanelName This can specify a name for the panel to allow
       *  identification later.  This is optional and defaults to a blank
       *  string.
       *
       * @return A smart pointer to the cxPanel that was added. If it couldn't be
       * added, the smart pointer's pointer will be null.
       */
      virtual std::shared_ptr<cxPanel> append(const std::string& pLabel, const std::string& pPanelName = "");

      /**
       * \brief Returns a pointer to one of the panels in the notebook (by
       * \brief index).
       *
       * @param pIndex The index of the panel to retrieve
       *
       * @return A pointer to the panel with the given index.  If there is
       * no panel for the given index, this will return nullptr.
       */
      virtual cxPanel* getPanel(unsigned int pIndex) const;

      /**
       * \brief Returns a pointer to one of the panels in the notebook (by
       * \brief label/name).
       *
       * @param pID The label/name of the panel
       * @param pIsLabel If true, this will treat pID as a label.  If false,
       *  this will treat pID as a name for a panel.  This defaults to true.
       *
       * @return A pointer to the panel with the given label/name.  If there is
       *  no panel for the given label/name, this will return nullptr.
       */
      virtual cxPanel* getPanel(const std::string& pID, bool pIsLabel = true) const;

      /**
       * \brief Returns a pointer to the current panel, or nullptr if there is
       * \brief no current panel for some reason (i.e., if the notebook has
       * \brief no panels).
       *
       * @return A pointer to the current panel, or nullptr if there is none
       */
      virtual cxPanel* getCurrentPanelPtr() const;

      /**
       * \brief Changes the notebok's position, based on a new upper-left
       * \brief corner. Also moves all the contained windows accordingly.
       *
       * @param pNewRow The new topmost row for the notebook
       * @param pNewCol The new leftmost column for the notebook
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

      /**
       * \brief Sets whether one of the panels in the notebook should show
       * \brief its window when it gets focus (by index).  This is for a
       * \brief screen refresh optimization.
       *
       * @param pIndex The index of the panel in the notebook
       * @param pShowPanelWindow Whether or not the panel should show its
       *  window when it gets focus.
       */
      virtual void setShowPanelWindow(unsigned int pIndex, bool pShowPanelWindow);

      /**
       * \brief Sets whether one of the panels in the notebook should show
       * \brief its window when it gets focus (by label/name).  This is for a
       * \brief screen refresh optimization.
       *
       * @param pID The label/name of the panel in the notebook
       * @param pShowPanelWindow Whether or not the panel should show its
       *  window when it gets focus.
       * @param pIsLabel If true, this will treat pID as a label.  If false,
       *  this will treat pID as a name for a panel.  This defaults to true.
       */
      virtual void setShowPanelWindow(const std::string& pID, bool pShowPanelWindow,
                                      bool pIsLabel = true);

      /**
       * \brief Returns whether or not the panel window will be shown.
       * \brief Overridden from cxPanel to prevent hiding.
       *
       * @return Whether or not the panel window will be shown
       */
      virtual bool getShowPanelWindow() const override;

      /**
       * \brief Returns whether or not one of the panels in the notebook will
       * \brief show its window when it gets focus (by index).
       *
       * @param pIndex The index of the panel
       *
       * @return Whether or not the panel in the notebook will show its window
       *  when it gets focus.
       */
      virtual bool getShowPanelWindow(unsigned int pIndex) const;

      /**
       * \brief Returns whether or not one of the panels in the notebook will
       * \brief show its window when it gets focus (by label/name).
       *
       * @param pID The label/name of the panel in the notebook
       * @param pShowPanelWindow Whether or not the panel should show its
       *  window when it gets focus.
       * @param pIsLabel If true, this will treat pID as a label.  If false,
       *  this will treat pID as a name for a panel.  This defaults to true.
       *
       * @return Whether or not the panel in the notebook will show its window
       *  when it gets focus.
       */
      virtual bool getShowPanelWindow(const std::string& pID, bool pIsLabel = true) const;

      /**
       * \brief Shows the subwindows for the notebook.  This is overidden here
       * \brief for screen optimization, since this method is called from
       * \brief show().  It doesn't show the panels inside the notebook, which
       * \brief are considered subwindows.  This method will only draw the
       * \brief subwindows that are outside the notebook.
       *
       * @param pBringToTop Whether or not to bring the subwindow to the top.
       *   Defaults to true.
       * @param pShowSubwindows Whether or not to show the subwindows for the
       *   subwindows.  Defaults to true.
       */
      virtual void showSubwindows(bool pBringToTop = true, bool pShowSubwindows = true) override;

      /**
       * \brief Returns the label for one of the panels (by index).
       *
       * @param pIndex The index of the panel
       *
       * @return The label of the panel.  If pIndex is out of bounds, this will
       *  return a blank string.
       */
      virtual std::string getLabel(unsigned int pIndex) const;

      /**
       * \brief Returns the label forone of the panels (by panel name).
       *
       * @param pName The name of the panel
       *
       * @return The label of the panel.  If there is no panel with the given
       *  name, this will return a blank string.
       */
      virtual std::string getLabel(const std::string& pName) const;

      /**
       * \brief Sets the label for one of the panels (by index).  If the new
       * \brief label would result in the label windows being too large to
       * \brief fit on the screen, however, the label will not be updated.
       *
       * @param pIndex The index of the panel
       * @param pLabel The new label
       * @param pRefresh Whether or not to refresh the screen.  Defaults to false.
       *
       * @return true if the label was set, or false if not.
       */
      virtual bool setLabel(unsigned int pIndex, const std::string& pLabel, bool pRefresh = false);

      /**
       * \brief Sets the label for one of the panels (by label/name).  If the
       * \brief new label would result in the label windows being too large to
       * \brief fit on the screen, however, the label will not be updated.
       *
       * @param pID The label/name of the panel
       * @param pLabel The new label
       * @param pIsLabel Boolean: true if pID is a label, or false if it's a
       *  panel name.
       * @param pRefresh Whether or not to refresh the screen.  Defaults to false.
       *
       * @return true if the label was set, or false if not.
       */
      virtual bool setLabel(const std::string& pID, const std::string& pLabel,
                            bool pIsLabel = true, bool pRefresh = false);

      /**
       * \brief Enables or disables the notebook.
       *
       * @param pEnabled Whether the notebook should be enabled or not (true/false)
       */
      virtual void setEnabled(bool pEnabled) override;

      /**
       * \brief Enables or disables a window in the notebook (by index).  (This
       * \brief should be inherited from cxPanel, but the compiler doesn't see
       * \brief it).
       *
       * @param pIndex The index of the window in the panel
       * @param pEnabled Boolean: If true, the window will be enabled, and if
       *  false, the window will be disabled.
       */
      virtual void setEnabled(unsigned int pIndex, bool pEnabled) override;

      /**
       * \brief Enables or disables a window in the panel (by title/name).
       * \brief Enables or disables a window in the notebook (by title/name).
       * \brief (This should be inherited from cxPanel, but the compiler
       * \brief doesn't see it).
       *
       * @param pID The title/name of the window in the panel
       * @param pEnabled Boolean: If true, the window will be enabled, and if
       *  false, the window will be disabled.
       * @param pIsTitle If this is true (default), then pID will refer to the
       *  window's title.  If this is false, then pID will refer to the
       *  window's name.
       */
      virtual void setEnabled(const std::string& pID, bool pEnabled, bool pIsTitle = true) override;

      /**
       * \brief Sets the key to be used for navigation to the next tab
       *
       * @param pKey A key to use to navigate to the next tab
       */
      void setNextTabKey(int pKey);

      /**
       * \brief Returns the key that is used for navigation to the next tab
       *
       * @return The key that is used for navigation to the next tab
       */
      int getNextTabKey() const;

      /**
       * \brief Sets the key to be used for navigation to the previous tab
       *
       * @param pKey A key to use to navigate to the previous tab
       */
      void setPrevTabKey(int pKey);

      /**
       * \brief Returns the key that is used for navigation to the previous tab
       *
       * @return The key that is used for navigation to the previous tab
       */
      int getPrevTabKey() const;

      /**
       * \brief Sets the key that is used for bringing up a tab selection menu
       *
       * @param pKey A key to use to bring up the tab selection menu
       */
      void setTabNavKey(int pKey);

      /**
       * \brief Returns the key currently used for bringing up the tab
       * \brief selection menu
       *
       * @return The key currently used for bringing up the tab selection menu
       */
      int getTabNavKey() const;

      /**
       * \brief Sets the message to be displayed when all the panels in the
       * \brief notebook are disabled.
       *
       * @param pAllDisabledMsg A new message to display
       */
      void setAllDisabledMsg(const std::string& pAllDisabledMsg);

      /**
       * \brief Returns the message that is displayed when all the panels in
       * \brief the notebook are disabled.
       *
       * @return The message that is displayed when all the panels in the
       *  notebook are disabled
       */
      const std::string& getAllDisabledMsg() const;

      /**
       * \brief Sets the message to be displayed when there are no panels in
       * \brief the notebook.
       *
       * @param pNoWindowsMsg A new message to display
       */
      void setNoWindowsMsg(const std::string& pNoWindowsMsg);

      /**
       * \brief Returns the message that is displayed when one of the panels in
       * \brief the notebook is disabled.
       *
       * @return The message that is displayed when one of the panels in the
       *  notebook is disabled.
       */
      const std::string& getWindowDisabledMsg() const;

      /**
       * \brief Sets the message to be displayed when a window is disabled.
       *
       * @param pMsg A new message
       */
      void setWindowDisabledMsg(const std::string& pMsg);

      /**
       * \brief Returns the message that is displayed when there are no panels
       * \brief in the notebook.
       *
       * @return The message that is displayed when there are no panels in the
       *  notebook
       */
      const std::string& getNoWindowsMsg() const;

      /**
       * \brief Returns whether the current mouse information's coordinates
       * \brief are within the top row of the notebook.
       *
       * @return Whether or not the last mouse event occurred in the top row
       *  of the notebook
       */
      virtual bool mouseEvtWasInTitle() const override;

      /**
       * \brief Removes a panel from the notebook.  This returns a cxWindow
       * \brief pointer rather than a cxPanel pointer so that this method
       * \brief will override the version from cxPanel.
       *
       * @param pIndex The index of the window in the panel to remove
       *
       * @return A pointer to the panel removed.  If pIndex is not a valid
       * index, the smart pointer returned will have a null value for its pointer.
       */
      virtual std::shared_ptr<cxWindow> removeWindow(unsigned int pIndex) override;

      /**
       * \brief Removes a panel from the notebook, without freeing its memory
       * \brief so that it may be used elsewhere.  Note: If you don't want to
       * \brief re-use the window, use delWindow() instead, as this frees up
       * \brief the window's memory using delete.  This takes a cxWindow*
       * \brief rather than a cxPanel* so that this method will override the
       * \brief version from cxPanel.
       *
       * @param pWindow A pointer to a panel to be removed from the notebook
       */
      virtual void removeWindow(cxWindow *pWindow) override;

      /**
       * \brief Removes a panel from the notebook (by shared_ptr), without
       * \brief freeing its memory.  Overridden from cxPanel to prevent hiding.
       *
       * @param pWindow A shared_ptr to a panel to be removed from the notebook
       */
      virtual void removeWindow(const std::shared_ptr<cxWindow>& pWindow) override;

      /**
       * \brief Removes a panel from the notebook (by index) and frees up its
       * \brief memory.  Does nothing if the index is out of bounds.  Note:
       * \brief If you want to re-use the window, you can use removeWindow()
       * \brief instead.
       *
       * @param pIndex The index of the panel to remove from the notebook
       */
      virtual void delWindow(unsigned int pIndex) override;

      /**
       * \brief Removes a panel from the notebook (by pointer).  Does nothing
       * \brief if the panel does not reside in the notebook.  Note: If you
       * \brief want to re-use the panel, you can use removeWindow() instead.
       *
       * @param pWindow A pointer to a panel to remove from the notebook
       */
      virtual void delWindow(cxWindow *pWindow);

      /**
       * \brief Removes a panel from the notebook (by shared_ptr).
       * \brief Overridden from cxPanel to prevent hiding.
       *
       * @param pWindow A shared_ptr to a panel to remove from the notebook
       */
      virtual void delWindow(const std::shared_ptr<cxWindow>& pWindow) override;

      /**
       * \brief Sets which panel will get focus in the next call to
       * \brief showModal() (by index).
       *
       * @param pIndex The index of the window that should
       *  get the focus
       *
       * @return True if successful or false if not
       */
      virtual bool setCurrentWindow(unsigned int pIndex) override;

      /**
       * \brief Sets which panel will get focus in the next call to
       * \brief showModal() (by title).
       *
       * @param pTitle The title of the window that should
       *  get the focus
       *
       * @return True if successful or false if not
       */
      virtual bool setCurrentWindow(const std::string& pTitle);

      /**
       * \brief Sets which window will get focus in the next call to
       * \brief showModal() (by label or name).
       * \brief This is separate from the previous setCurrentWindow() because
       * \brief the previous setCurrentWindow() must be here to override
       * \brief the one inherited from cxPanel.  This method also sets by
       * \brief label/name, whereas the other sets by the title.
       *
       * @param pTitle The label/name of the window that should get the focus
       *
       * @return True if successful or false if not
       */
      virtual bool setCurrentWindow(const std::string& pID, bool pIsLabel) override;

      /**
       * \brief Sets which window will get focus in the next
       * \brief cycle through showModal() (by pointer).  This
       * \brief has a different function name so as not to
       * \brief be ambiguous to the compiler.
       *
       * @param pWindow A pointer to a cxWindow (if it exists
       *  in the panel, it will then be the current window).
       * @return True if successful or false if not
       */
      virtual bool setCurrentWindowByPtr(cxWindow *pWindow) override;

      /**
       * \brief Sets which window will get focus in the next
       * \brief cycle through showModal() (by shared_ptr).
       * \brief Overridden from cxPanel to prevent hiding.
       *
       * @param pWindow A shared_ptr to a cxWindow (if it exists
       *  in the panel, it will then be the current window).
       * @return True if successful or false if not
       */
      virtual bool setCurrentWindowByPtr(const std::shared_ptr<cxWindow>& pWindow) override;

      /**
       * \brief Returns whether a cxWindow object is contained in the notebook.
       * \brief Overridden from cxPanel to prevent hiding.
       *
       * @param pWindow A shared_ptr to a cxWindow object
       *
       * @return Whether or not the cxWindow is contained in the notebook
       */
      virtual bool windowIsInPanel(const std::shared_ptr<cxWindow>& pWindow) const override;

      /**
       * \brief Returns the index of the tab (label window) that was clicked
       * \brief on.  If the user didn't click on a tab window, then this will
       * \brief return -1.
       *
       * @return The index of the tab that was clicked on, or -1 if the user
       *  didn't click on a tab window.
       */
      virtual int lastClickTabIndex() const;

      /**
       * \brief Swaps the order of 2 panels (by index).  If both indexes are
       * \brief the same, or one or both is out of bounds, nothing will be
       * \brief changed.  This overrides the swap() inherited from cxPanel.
       * \brief This does not refresh the screen.
       *
       * @param pWindow1Index The index of the first window
       * @param pWindow2Index The index of the second window
       *
       * @return Whether or not the windows got swapped
       */
      virtual bool swap(int pWindow1Index, int pWindow2Index) override;

      /**
       * \brief Swaps the order of 2 panels (by pointer).  If both pointers are
       * \brief the same, or one or both don't exist in the panel, nothing will
       * \brief be changed.  This overrides the swap() inherited from cxPanel.
       * \brief This does not refresh the screen.
       *
       * @param pWindow1 A pointer to the first window
       * @param pWindow2 A pointer to the second window
       *
       * @return Whether or not the windows got swapped
       */
      virtual bool swap(const std::shared_ptr<cxWindow>& pWindow1, const std::shared_ptr<cxWindow>& pWindow2) override;
      virtual bool swap(cxWindow *pWindow1, cxWindow *pWindow2) override;

      /**
       * \brief Selects the next available (enabled) window (moving forward),
       * \brief if there is one.  Returns whether or not there was an enabled
       * \brief window.
       *
       * @return true if there is an enabled window, or false if not.
       */
      bool selectNextWin();

      /**
       * \brief Selects the previous available (enabled) window (moving
       * \brief backward), if there is one.  Returns whether or not there was
       * \brief an enabled window.
       *
       * @return true if there is an enabled window, or false if not.
       */
      bool selectPrevWin();

   private:
      typedef std::vector<std::shared_ptr<cxWindow> > labelWinContainer;
      labelWinContainer mLabels;      // These act as labels for the tabs
      bool mLabelsOnTop;              // Can determine if the labels are on top or bottom
      bool mLeftLabelSpace;           // Whether to have a space to the left of label text
      bool mRightLabelSpace;          // Whether to have a space to the right of label text
      int mCurrentPanelIndex = 0;     // Index of the current panel
      int mNextTabKey = PAGE_DOWN;    // The key to use to go to the next tab
      int mPrevTabKey = PAGE_UP;      // The key to use to go to the previous tab
      int mTabNavKey;                 // The key to use to bring up a tab navigation menu
      std::string mAllDisabledMsg;    // Message to display when all windows are disabled
      std::string mNoWindowsMsg;      // Message to display when the notebook has no windows
      std::string mWindowDisabledMsg; // Message to display when a window is disabled
      int mTabSpacing;                // # of spaces between the tabs
      int mLastClickTabIndex = -1;    // Index of the window that was clicked on
      // mTabNavWrap controls whether "wrapping" occurs if user wants to
      //  navigate to the next/previous tab with the keyboard and they're
      //  on the last/first enabled tab.
      bool mTabNavWrap;

      // Runs the input loop
      //
      // Parameters:
      //  pRunOnLeaveFunction (OUT): This will store whether or not the onLeave
      //   function should run.
      long doInputLoop(bool& pRunOnLeaveFunction);

      // Shows the current panel, and hides the others
      //
      // Parameters:
      //  pApplylabelAttr: Whether or not to apply the label window attribute.
      //   Defaults to true.
      //  pBringToTop: Whether or not to bring the current panel to the top of
      //   the stack.  Defaults to true.
      void showCurrentWindow(bool pApplyLabelAttr = true, bool pBringToTop = true);

      // Sets up the special border characters for a label window.  This is a
      //  helper that is called when appending a panel and when the user
      //  changes one of the labels.
      //
      // Parameters:
      //  pIndex: The index of the label window.  The label windows to the left
      //   and to the right of the window will also have their border characters
      //   set.
      void setLabelWinSpecialChars(unsigned int pIndex);

      // Sets the special border characters for all label windows
      inline void setAllLabelWinSpecialChars();

      // Makes sure that all the label windows are where they should be.  Also
      //  makes sure they have the proper speical border characters.
      //
      // Parameters:
      //  pRefresh: Whether or not to refresh the screen.  Defaults to false.
      void alignLabelWindows(bool pRefresh = false);

      // Dis-allow some of cxPanel's methods from being called by users of
      //  this class
      bool getExitOnLeaveLast() const override;
      void setExitOnLeaveLast(bool pExitOnLeaveLast) override;
      bool getExitOnLeaveFirst() const override;
      void setExitOnLeaveFirst(bool pExitOnLeaveFirst) override;
      bool windowIsInPanel(cxWindow *pWindow) const override;
      bool append(const std::shared_ptr<cxWindow>& pWindow) override;
      bool append(const std::shared_ptr<cxWindow>& pWindow, int pRow, int pCol, bool pRefresh = false) override;
      void setShowPanelWindow(bool pShowPanelWindow) override;

      // Don't allow copy construction or assignment
      cxNotebook(const cxNotebook& pNotebook);
      cxNotebook& operator =(const cxNotebook& pNotebook);

      /**
       * \brief Sets up spaces in the border for a panel so that it looks
       * \brief like the label window "opens up" into the panel.  (either
       * \brief the top/title line or the bottom/status line)
       *
       * @param pIndex The index of the panel to setup
       * @param pClear If this is true, then the special border characters will
       *  be cleared, and no space strings will be set in the title/status for
       *  the window.
       */
      void setupLabelBorder(int pIndex, bool pClear = false);

      // This is a helper for setupLabelBorder().  It removes all strings from
      //  a title/status map that are all spaces.
      static void removeSpaceStrings(std::map<int, std::string>& pStringMap);

      // Shows a tab navigation menu, allowing the user to choose another tab
      //  to go to.
      void showNavMenu();

}; // end of class cxNotebook

#endif
