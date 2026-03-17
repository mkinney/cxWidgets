#ifndef __CXPANEL_H__
#define __CXPANEL_H__

// Copyright (c) 2006-2007 Michael H. Kinney

#include "cxWindow.h"
#include <string>
#include <vector>
#include <memory>

/**
 * \brief cxPanel is a window that contains other windows.  When showModal()
 * \brief is called, focus will cycle through all the subwindows in the order
 * \brief they were added.<br>
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = true);
 * These are the steps to add a window to a cxPanel:<br>
 * 1) Create it dynamically (i.e., with the 'new' operator)<br>
 * 2) Add it to the cxPanel with the append() method<br>
 * All windows added to a cxPanel must be created dynamically because
 * cxPanel's destructor will free the memory used by the windows it
 * contains.<br>
 * The windows added to a cxPanel via append() are managed in a
 * collection that is separate from the subwindow collection contained in
 * the base class (cxWindow).  This is because the subwindows cotnained by a
 * cxPanel are assumed to be created dynamically, but cxWindow does not
 * make this assumption with the subwindows.<br>
 * Notes:
 * <ul>
 * <li>Since all of the windows contained in a cxPanel are also added to
 * the list of subwindows inherited from cxWindow, it is not necessary to
 * overload show() in this class.
 * <li>If all the subwindows in your panel are full-screen, you may want to
 *     consider calling setShowPanelWindow(false) so that the panel window
 *     is not drawn before the subwindows.  This will avoid redrawing the
 *     entire screen when subwindows are shown, which will noticeably speed
 *     up screen redraws on slow terminals.  In addition, if all the
 *     subwindows in a panel are full-screen, showModal() should be called
 *     with the first argument false to avoid redrawing the whole screen.
 * </ul>
 */
class cxPanel : public cxWindow
{
   public:
      /**
       * \brief Default constructor.
       *
       * @param pParentWindow A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the window.
       * @param pWidth The width of the window.
       * @param pTitle The window title (useful if the panel will have
       *  borders).
       * @param pMessage The message to appear in the panel.
       * @param pStatus The status (useful if the panel will have borders).
       * @param pBorderStyle The border style - can be eBS_SINGLE_LINE for a
       *  single-line  border or eBS_NOBORDER for no border.  Defaults to
       *  eBS_NOBORDER.
       * @param pExtTitleWindow A pointer to another cxWindow in which to
       *  display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow in which to
       *  display the status.
       * @param pMessageUnderlines Whether or not to recognize & use
       *  underline characters when displaying the message text.
       */
      explicit cxPanel(cxWindow *pParentWindow = nullptr,
                       int pRow = 0, int pCol = 0,
                       int pHeight = DEFAULT_HEIGHT,
                       int pWidth = DEFAULT_WIDTH,
                       const std::string& pTitle = "",
                       const std::string& pMessage = "",
                       const std::string& pStatus = "",
                       eBorderStyle pBorderStyle = eBS_NOBORDER,
                       cxWindow *pExtTitleWindow = nullptr,
                       cxWindow *pExtStatusWindow = nullptr,
                       bool pMessageUnderlines = false);

      virtual ~cxPanel();

      /**
       * \brief Shows the panel and all of its subwindows and
       * \brief cycles input control through each of them.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *  to true.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults
       *  to true.  Note that the subwindows that belong to the panel are always
       *  drawn; this applies to windows that have the panel as a parent but
       *  are not actually "in" the panel.
       *
       * @return A return code based on user input.  Returns cxID_QUIT if the user
       *  presses ESC; otherwise, returns cxID_EXIT.
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = true) override;

      /**
       * \brief Returns whether or not the panel will exit focus
       * \brief when the last subwindow leaves focus.
       *
       * @return Whether or not the panel will exit focus
       *  when the last subwindow leaves focus.
       */
      virtual bool getExitOnLeaveLast() const;

      /**
       * \brief Sets whether the panel should exit focus
       * \brief when the last subwindow leaves focus.
       *
       * @param pExitOnLeaveLast Whether or not the panel
       *  should exit focus when the last subwindow leaves
       *  focus.
       */
      virtual void setExitOnLeaveLast(bool pExitOnLeaveLast);

      /**
       * \brief Returns whether the panel will exit focus
       * \brief when the first subwindow leaves focus when
       * \brief the user is navigating through them in
       * \brief reverse.
       *
       * @return Whether or not the panel will exit focus
       *  when the first subwindow leaves focus when
       *  the user is navigating through them in
       *  reverse.
       */
      virtual bool getExitOnLeaveFirst() const;

      /**
       * \brief Sets whether the panel should exit focus
       * \brief when the first subwindow leaves focus when
       * \brief the user is navigating through them in
       * \brief reverse.
       *
       * @param pExitOnLeaveFirst Whether or not the panel
       *  should exit focus when the first subwindow leaves
       *  focus when the user is navigating through them in
       *  reverse.
       */
      virtual void setExitOnLeaveFirst(bool pExitOnLeaveFirst);

      /**
       * \brief Returns the number of windows that are contained
       * \brief by the panel.
       *
       * @return The number of windows contained by the panel
       */
      virtual unsigned int numWindows() const;

      /**
       * \brief Gets a pointer to one of the subwindows (by index).
       * \brief Returns nullptr if the index is out of bounds.
       * \brief pIndex should be between 0 and 1 less than the
       * \brief number of subwindows.
       *
       * @param pIndex The index of the subwindow
       *
       * @return A pointer to one of the subwindows, or nullptr
       *  if pIndex is out of bounds.
       */
      virtual std::shared_ptr<cxWindow> getWindow(unsigned pIndex) const;

      /**
       * \brief Adds a window to the panel.  Note: The cxWindow
       * \brief object <b>must</b> be dynamically created (with
       * \brief the 'new' operator) because a cxPanel manages its
       * \brief windows via pointers and will free up all the
       * \brief memory used by the window pointers when the panel
       * \brief is destroyed.
       *
       * @param pWindow A pointer to a cxWindow object to add
       *  to the panel
       *
       * @return true if the window was appended, or false if not
       */
      virtual bool append(const std::shared_ptr<cxWindow>& pWindow);

      /**
       * \brief Adds a window to the panel with relative coordinates.
       * \brief The window will be moved to the coordinates
       * \brief specified by pRow and pCol, relative to the
       * \brief panel.  Note: The cxWindow object <b>must</b> be
       * \brief dynamically created (with the 'new' operator) because
       * \brief a cxPanel manages its windows via pointers and will
       * \brief free up all the memory used by the window pointers
       * \brief when the panel is destroyed.
       *
       * @param pWindow A pointer to a cxWindow object to add
       *  to the panel
       * @param pRow The new top row of the window (relative
       *  to the panel)
       * @param pCol The new left column of the window (relative
       *  to the panel)
       * @param pRefresh Whether or not to refresh the window after
       *  it is moved (defaults to false)
       *
       * @return true if the window was appended, or false if not
       */
      virtual bool append(const std::shared_ptr<cxWindow>& pWindow, int pRow, int pCol, bool pRefresh = false);

      /**
       * \brief Removes a window from the panel (by index) and frees the memory
       * \brief used by the window.  Does nothing if the index is out of bounds.
       * \brief Note: If you want to re-use the window, you can use
       * \brief removeWindow() instead.
       *
       * @param pIndex The index of the window to remove from the panel
       */
      virtual void delWindow(unsigned pIndex);

      /**
       * \brief Removes a window from the panel (by pointer), and frees the
       * \brief memory used by the window.  Does nothing if the window does
       * \brief not reside in the panel.  Note: If you want to re-use the
       * \brief window, you can use removeWindow() instead.
       *
       * @param pWindow A pointer to a window to remove from the panel
       */
      virtual void delWindow(const std::shared_ptr<cxWindow>& pWindow);

      /**
       * \brief Removes all windows from the panel.  Also frees the memory used
       * \brief by them.  For authors using cxPanel: This calls delWindow()
       * \brief with an index, so this method should not need to be overridden
       * \brief in derived classes.
       */
      virtual void delAllWindows();

      /**
       * \brief Removes a window from the panel, without freeing its memory
       * \brief so that it may be used elsewhere.  Note: If you don't want to
       * \brief re-use the window, use delWindow() instead, as this frees up
       * \brief the window's memory using delete.
       *
       * @param pIndex The index of the window in the panel to remove
       *
       * @return A pointer to the window removed.  If pIndex is not a valid
       *  index, the return value will be nullptr.
       */
      virtual std::shared_ptr<cxWindow> removeWindow(unsigned int pIndex);

      /**
       * \brief Removes a window from the panel, without freeing its memory
       * \brief so that it may be used elsewhere.  Note: If you don't want to
       * \brief re-use the window, use delWindow() instead, as this frees up
       * \brief the window's memory using delete.
       *
       * @param pWindow A pointer to a window to be removed from the panel
       */
      virtual void removeWindow(const std::shared_ptr<cxWindow>& pWindow);
      virtual void removeWindow(cxWindow *pWindow);

      /**
       * \brief Returns whether a cxWindow object is contained
       * \brief in the panel
       *
       * @param pWindow A pointer to a cxWindow object
       *
       * @return Whether or not the cxWindow is contained in
       *  the panel
       */
      virtual bool windowIsInPanel(const std::shared_ptr<cxWindow>& pWindow) const;
      virtual bool windowIsInPanel(cxWindow *pWindow) const;

      /**
       * \brief Changes the panel's position, based on a new upper-left corner.
       * \brief Also moves all the contained windows accordingly.
       *
       * @param pNewRow The new topmost row for the panel
       * @param pNewCol The new leftmost column for the panel
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

      /**
       * \brief Sets which window will get focus in the next
       * \brief cycle through showModal() (by index)
       *
       * @param pIndex The index of the window that should
       *  get the focus
       * @return True if successful or false if not
       */
      virtual bool setCurrentWindow(unsigned pIndex);

      /**
       * \brief Sets which window will get focus in the next
       * \brief cycle through showModal() (by title or name)
       *
       * @param pID The title/name of the window that should get focus
       * @param pIsTitle If this is true (default), then pID will refer to the
       *  window's title.  If this is false, then pID will refer to the
       *  window's name.
       *
       * @return True if successful or false if not
       */
      virtual bool setCurrentWindow(const std::string& pID, bool pIsTitle = true);

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
      virtual bool setCurrentWindowByPtr(const std::shared_ptr<cxWindow>& pWindow);
      virtual bool setCurrentWindowByPtr(cxWindow *pWindow);

      /**
       * \brief Sets a function to be called when a key is pressed.
       *
       * @param pKey The key to use for the function
       * @param pFunction The function to call. This can be an instance of
       * one of the derived cxFunction classes as well.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, const std::shared_ptr<cxFunction>& pFunction) override;

      /**
       * \brief Removes a function pointer for a keypress for the panel so that it will no
       * \brief longer fire a function.
       *
       * @param pKey The hotkey for the function to remove
       */
      virtual void clearKeyFunction(int pKey) override;

      /**
       * \brief Sets a function to be called when a key is pressed.  Note that
       * \brief when a key function is set on a cxPanel, the key will be
       * \brief added to the panel's subwindows as exit keys - so if you need
       * \brief to disable a function key for a subwindow of a cxPanel (but
       * \brief not disable that key in the whole panel), you can call
       * \brief removeExitKey() on that subwindow.
       *
       * @param pKey The key to use for the function
       * @param pFunction The function to call - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 The 1st parameter to the function (pointer)
       * @param p2 The 2nd parameter to the function (pointer)
       * @param p3 The 3rd parameter to the function (pointer)
       * @param p4 The 4th parameter to the function (pointer)
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxWindow, but could be useful for deriving
       *  classes).
       * @param pExitAfterRun Whether or not to exit the input loop
       *  after the function is run (not used by cxWindow, but could be
       *  useful for deriving classes).
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr4 pFunction,
                                  void *p1, void *p2, void *p3, void *p4,
                                  bool pUseReturnVal = false,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true) override;

      /**
       * \brief Sets a function to be called when a key is pressed.  Note that
       * \brief when a key function is set on a cxPanel, the key will be
       * \brief added to the panel's subwindows as exit keys - so if you need
       * \brief to disable a function key for a subwindow of a cxPanel (but
       * \brief not disable that key in the whole panel), you can call
       * \brief removeExitKey() on that subwindow.
       *
       * @param pKey The key to use for the function
       * @param pFunction The function to call - Must have this signature:
       *  string func(void*, void*)
       * @param p1 The 1st parameter to the function (pointer)
       * @param p2 The 2nd parameter to the function (pointer)
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxWindow, but could be useful for deriving
       *  classes).
       * @param pExitAfterRun Whether or not to exit the input loop
       *  after the function is run (not used by cxWindow, but could be
       *  useful for deriving classes).
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr2 pFunction,
                                  void *p1, void *p2,
                                  bool pUseReturnVal = false,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true) override;

      /**
       * \brief Sets a function to be called when a key is pressed.  Note that
       * \brief when a key function is set on a cxPanel, the key will be
       * \brief added to the panel's subwindows as exit keys - so if you need
       * \brief to disable a function key for a subwindow of a cxPanel (but
       * \brief not disable that key in the whole panel), you can call
       * \brief removeExitKey() on that subwindow.
       *
       * @param pKey The key to use for the function
       * @param pFunction The function to call - Must have this signature:
       *  string func()
       * @param p1 The 1st parameter to the function (pointer)
       * @param p2 The 2nd parameter to the function (pointer)
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxWindow, but could be useful for deriving
       *  classes).
       * @param pExitAfterRun Whether or not to exit the input loop
       *  after the function is run (not used by cxWindow, but could be
       *  useful for deriving classes).
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, funcPtr0 pFunction,
                                  bool pUseReturnVal = false,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true) override;

      /**
       * \brief Returns the index of the current window.
       * \brief Could return -1 if there are no windows
       * \brief in the panel.
       *
       * @return The index of the current window
       */
      virtual int getCurrentWindowIndex() const;

      /**
       * \brief Given a cxWindow pointer, this function will return the index
       * \brief of the window in the panel, or -1 if the window is not in the
       * \brief panel.
       *
       * @param pWindow A pointer to the window whose index to get
       *
       * @return The index of the window, or -1 if the window is not in the
       *  panel.
       */
      virtual int getWindowIndex(const std::shared_ptr<cxWindow>& pWindow) const;
      virtual int getWindowIndex(cxWindow *pWindow) const;

      /**
       * \brief Returns a pointer to the current window.
       * \brief Could return nullptr if there are no windows
       * \brief in the panel.
       *
       * @return A pointer to the current window
       */
      virtual const std::shared_ptr<cxWindow>& getCurrentWindowPtr() const;

      /**
       * \brief Shows the panel.  If setShowPanelWindow(false) has been
       * \brief called, the panel window won't be shown, only the windows
       * \brief contained in the panel will be shown.
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
       * \brief Hides the panel, along with the windows in the panel.
       *
       * @param pHideSubwindows This is here so that this function is
       *  overridden from cxWindow.
       */
      virtual void hide(bool pHideSubwindows = true) override;

      /**
       * \brief Un-hides the panel, along with the windows in the panel.
       *
       * @param pUnhideSubwindows This is here so that this function is
       *  overridden from cxWindow.
       */
      virtual void unhide(bool pUnhideSubwindows = true) override;

      /**
       * \brief Returns whether or not the panel window will be
       * \brief shown along with the windows in the panel.
       *
       * @return Whether or not the panel window will be shown along
       *  with the windows in the panel.
       */
      virtual bool getShowPanelWindow() const;

      /**
       * \brief Sets whether or not the panel window should be shown
       * \brief when showing the panel.  The windows in the panel
       * \brief will still be shown.
       *
       * @param pShowPanelWindow Whether or not to have the panel
       *  window show when showing the panel.
       */
      virtual void setShowPanelWindow(bool pShowPanelWindow);

      /**
       * \brief Returns whether the user is allowed to exit out
       * \brief of the cxPanel (with a return code of cxID_EXIT)
       *
       * @return Whether the user is allowed to exit out of the panel
       */
      virtual bool getAllowExit() const;

      /**
       * \brief Sets whether the user is allowed to exit out of
       * \brief the panel (with a return code of cxID_EXIT)
       *
       * @param pAllowExit Whether or not the user is allowed to
       *  exit out of the panel
       */
      virtual void setAllowExit(bool pAllowExit);

      /**
       * \brief Returns whether the user is allowed to quit out
       * \brief of the cxPanel (with a return code of cxID_QUIT)
       *
       * @return Whether the user is allowed to quit out of the panel
       */
      virtual bool getAllowQuit() const;

      /**
       * \brief Sets whether the user is allowed to quit out of
       * \brief the panel (with a return code of cxID_QUIT))
       *
       * @param pAllowExit Whether or not the user is allowed to
       *  quit out of the panel
       */
      virtual void setAllowQuit(bool pAllowQuit);

      /**
       * \brief Returns whether or not one of the windows in the panel
       * \brief is enabled (by index).
       *
       * @param pIndex The index of the window in the panel
       *
       * @return Whethre or not the window is enabled
       */
      virtual bool windowIsEnabled(unsigned pIndex) const;

      /**
       * \brief Enables/disables one of the windows in the panel (by index).
       *
       * @param pIndex The index of the window
       * @param pEnabled Whether or not the window should be enabled
       *  (defaults to true)
       */
      virtual void enableWindow(unsigned pIndex, bool pEnabled = true);

      /**
       * \brief Clears all the window in the panel.
       *
       * @param pRefresh Whether or not to refresh the windows as they are
       *  cleared (defaults to false).
       */
      virtual void clear(bool pRefresh = false) override;

      /**
       * \brief Adds a key that will cause the window to quit its input loop
       * \brief  and return cxID_EXIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave functions
       *  when the panel exits.  Defaults to true.  Note that this affects
       *  both the onLeave function for the panel, as well as the current
       *  window when the panel is modal.
       * @param pOverride If true, then the key will be added as an exit key
       *  regardless if the key exists as a function key.  This defaults to
       *  false.
       *
       * @return Whether or not the key was added
       */
      virtual bool addQuitKey(int pKey, bool pRunOnLeaveFunction = true,
                              bool pOverride = false) override;

      /**
       * \brief Removes a quit key
       *
       * @param pKey The key to remove from the list of quit keys
       */
      virtual void removeQuitKey(int pKey) override;

      /**
       * \brief Adds a key that will cause the window to quit its input loop
       * \brief and return cxID_EXIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave functions
       *  when the panel exits.  Defaults to true.  Note that this affects
       *  both the onLeave function for the panel, as well as the current
       *  window when the panel is modal.
       * @param pOverride If true, then the key will be added as an exit key
       *  regardless if the key exists as a function key.  This defaults to
       *  false.
       *
       * @return Whether or not the key was added
       */
      virtual bool addExitKey(int pKey, bool pRunOnLeaveFunction = true,
                              bool pOverride = false) override;

      /**
       * \brief Removes an exit key
       *
       * @param pKey The key to remove from the list of exit keys
       */
      virtual void removeExitKey(int pKey) override;

      /**
       * \brief Enables or disables the panel.  Disabling/enabling a cxPanel
       * \brief consequently also enables/disables the windows in the panel.
       *
       * @param pEnabled Whether the panel should be enabled or not (true/false)
       */
      virtual void setEnabled(bool pEnabled) override;

      /**
       * \brief Enables or disables a window in the panel (by index).
       *
       * @param pIndex The index of the window in the panel
       * @param pEnabled Boolean: If true, the window will be enabled, and if
       *  false, the window will be disabled.
       */
      virtual void setEnabled(unsigned pIndex, bool pEnabled);

      /**
       * \brief Enables or disables a window in the panel (by title/name).
       *
       * @param pID The title/name of the window in the panel
       * @param pEnabled Boolean: If true, the window will be enabled, and if
       *  false, the window will be disabled.
       * @param pIsTitle If this is true (default), then pID will refer to the
       *  window's title.  If this is false, then pID will refer to the
       *  window's name.
       */
      virtual void setEnabled(const std::string& pID, bool pEnabled, bool pIsTitle = true);

      /**
       * \brief Swaps the order of 2 subwindows (by index).  If both indexes
       * \brief are the same, or one or both is out of bounds, nothing
       * \brief will be changed.  This does not refresh the screen.
       *
       * @param pWindow1Index The index of the first window
       * @param pWindow2Index The index of the second window
       *
       * @return Whether or not the windows got swapped
       */
      virtual bool swap(int pWindow1Index, int pWindow2Index);

      /**
       * \brief Swaps the order of 2 subwindows (by pointer).  If both pointers
       * \brief are the same, or one or both don't exist in the panel, nothing
       * \brief will be changed.  This does not refresh the screen.
       *
       * @param pWindow1 A pointer to the first window
       * @param pWindow2 A pointer to the second window
       *
       * @return Whether or not the windows got swapped
       */
      virtual bool swap(cxWindow *pWindow1, cxWindow *pWindow2);
      virtual bool swap(const std::shared_ptr<cxWindow>& pWindow1, const std::shared_ptr<cxWindow>& pWindow2);

      /**
       * \brief Sets the color of one of the window items.
       *
       * @param pItem The item to set the color of (see the e_WidgetItems
       *  enumeration in cxWidgetItems.h).
       * @param pColor The color to set the item to (see the e_cxColors
       *  enumeration in cxColors.h)
       */
      virtual void setColor(e_WidgetItems pItem, e_cxColors pColor) override;

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief This causes the panel to stop its input loop with a code of
       * \brief cxID_QUIT.
       */
      virtual void quitNow() override;

      /**
       * \brief This causes the panel to stop its input loop with a code of
       * \brief cxID_EXIT.
       */
      virtual void exitNow() override;

      /**
       * \brief Returns a pointer to the last window in the panel that was
       * \brief visited in the input loop (during a showModal()).  Could
       * \brief return nullptr if there was no last window visited (i.e., when a
       * \brief cxPanel is first created).  If the cxPanel has been shown
       * \brief previously, then the next time it is shown, the last window
       * \brief will still be set to the window that the focus was on the
       * \brief last time the cxPanel was shown.
       *
       * @return A pointer to the last window visited during the input loop,
       *  or nullptr if there was no last window visited.
       */
      const std::shared_ptr<cxWindow>& getLastWindow() const;

      /**
       * \brief Sets the name of one of the windows in the panel (by index).
       *
       * @param pIndex The index of the window in the panel
       * @param pName The new name for the window
       */
      virtual void setName(unsigned pIndex, const std::string& pName);

      /**
       * \brief Sets the name of one of the windows in the panel (by title/name).
       *
       * @param pID The title or current name of the window
       * @param pName The new name for the window
       * @param pIsTitle If true (default), then pID refers to the title of
       *  the window.  If false, then pID refers to the current name of the
       *  window.
       */
      virtual void setName(const std::string& pID, const std::string& pName, bool pIsTitle = true);

      /**
       * \brief Sets the name of the panel.  The name is an alternative means
       * \brief of identifying the panel.  This should be inherited from
       * \brief cxWindow, but the compiler doesn't seem to see it.
       *
       * @param pName The new name for the panel
       */
      virtual void setName(const std::string& pName);

      /**
       * \brief Returns the top row of the window
       * @return The top row of the window
       */
      virtual int top() const override;

      /**
       * \brief Returns the bottom row of the window
       * @return The bottom row of the window
       */
      virtual int bottom() const override;

   protected:
      typedef std::vector<std::shared_ptr<cxWindow> > cxWindowPtrCollection;
      cxWindowPtrCollection mWindows;        // The windows in the panel to cycle between
      bool mCycleWin = true;                 // Whether or not to cycle to the next/previous
                                             //  window in the input loop (this is set false
                                             //  in setCurrentWindow()).
      std::shared_ptr<cxWindow> mLastWindow; // The last window visited during the input loop

      /**
       * \brief Returns whether any of the windows contained in mWindows are
       * \brief enabled.
       *
       * @return true if there is at least 1 window that is enabled, or false
       *  if not.
       */
      bool anyEnabledWindows() const;

      /**
       * \brief Returns whether or not a window in the panel with a given index
       * \brief is the last enabled window.  If the index is out of range, this
       * \brief will return false.
       *
       * @param pIndex The index of a window in the panel
       *
       * @return true if the window is the last enabled window in the panel,
       *  or false if not.
       */
      bool lastEnabledWindow(int pIndex) const;

      /**
       * \brief Returns whether or not a window in the panel with a given index
       * \brief is the first enabled window.  If the index is out of range, this
       * \brief will return false.
       *
       * @param pIndex The index of a window in the panel
       *
       * @return true if the window is the first enabled window in the panel,
       *  or false if not.
       */
      bool firstEnabledWindow(int pIndex) const;

   private:
      friend class cxWindow;

      cxWindowPtrCollection::iterator mWindowIter; // To keep track of current window
      bool mExitOnLeaveLast = false;   // Exit the input loop when leaving last window?
      bool mExitOnLeaveFirst = false;  // Exit the input loop when leaving first window?
      bool mAllowExit = true;         // Whether or not the user is allowed to exit
      bool mAllowQuit = true;         // Whether or not the user is allowed to quit
      // If mShowPanelWindow is true (default), the panel window
      // will be shown (in addition to the subwindows and the windows
      // in the panel).  If it is false, only the windows in the
      // panel will be shown.
      bool mShowPanelWindow = true;

      // Adds a cxWindow pointer to mWindows.  Also checks to see
      // if the cxWindow already has a cxPanel parent, and if so,
      // will remove the window from the other cxPanel.
      //
      // Parameters:
      //  pWindow: A cxWindow pointer to be added to mWindows.
      //
      // Return: true if the window was added, or false if not.
      bool addWindowPtr(const std::shared_ptr<cxWindow>& pWindow);

      // Does the input loop.  Returns the return code
      //  according to the user's input.
      // Parameters:
      //  pSubwinShow: Whether or not to have the subwindows show
      //   themselves before doing their input loops.
      //  pRunOnLeaveFunction (OUT): This will store whether or not the onLeave
      //   function should run.
      long doInputLoop(bool pSubwinShow, bool& pRunOnLeaveFunction);

      // This adds an exit key to one of the windows in the panel.  If the
      //  window is a cxMenu and if the key is one of the menu's up/down
      //  navigational keys, however, the exit key will not be added to the
      //  menu.
      //
      // Parameters:
      //  pWindow: A pointer to the cxWindow to add the exit key to
      //  pKey: The key to add as the exit key
      //  pRunOnLeaveFunction: Whether or not the window should run its
      //   onLeave function before leaving focus
      //  pOverride: Whether or not the key should override a function call
      //   for the key, if there is one set up
      //
      // Note: This is inline for speed (this function will get called
      //  inside of a loop).
      inline void addExitKeyToWindow(std::shared_ptr<cxWindow>& pWindow, int pKey,
                              bool pRunOnLeaveFunction, bool pOverride);

      // This adds a quit key to one of the windows in the panel.  If the
      //  window is a cxMenu and if the key is one of the menu's up/down
      //  navigational keys, however, the quit key will not be added to the
      //  menu.
      //
      // Parameters:
      //  pWindow: A pointer to the cxWindow to add the quit key to
      //  pKey: The key to add as the quit key
      //  pRunOnLeaveFunction: Whether or not the window should run its
      //   onLeave function before leaving focus
      //  pOverride: Whether or not the key should override a function call
      //   for the key, if there is one set up
      //
      // Note: This is inline for speed (this function will get called
      //  inside of a loop).
      inline void addQuitKeyToWindow(std::shared_ptr<cxWindow>& pWindow, int pKey,
                              bool pRunOnLeaveFunction, bool pOverride);

      // Don't allow copy construction or assignment
      cxPanel(const cxPanel& pThatPanel);
      cxPanel& operator =(const cxPanel& pThatPanel);
};

#endif
