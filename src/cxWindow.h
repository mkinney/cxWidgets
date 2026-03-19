// Copyright (c) 2026 E. Oulashin
#ifndef __CXWINDOW_H__
#define __CXWINDOW_H__

// Copyright (c) 2005-2007 Michael H. Kinney

// The cxWindow class is used to draw windows/boxes on the screen.
// Note: Quite a few objects are derived from this class.
// Note: If cxWindow is updated so that it might not wait for a keypress in
//  its input loop, modalGetsKeypress() will need to be updated to reflect
//  this.
//
// Maybe in the future we can add a window ID member variable (and
//  parameters to the constructors, making sure it's consistent with
//  wxWidgets classes).  This way, as in the wxWidgets framework, we
//  can identify what command came from which window, etc.).

#include "cxObject.h"
#include "cxPositions.h"
#include "cxReturnCodes.h"
#include "cxBorderStyles.h"
#include "cxMiscDefines.h"
#include "cxFunction.h"
#include "cxWidgetItems.h"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>
#include <utility>
#include <memory>

class cxWindow;
typedef std::vector<cxWindow*> cxWindowPtrContainer;

// If the version of ncurses on the current machine doesn't support the mouse,
//  then define some of ncurses' mouse event macros so that code using them
//  will still compile.
#ifndef NCURSES_MOUSE_VERSION
   #define BUTTON1_PRESSED 2
   #define BUTTON1_RELEASED 1
   #define BUTTON1_CLICKED 4
   #define BUTTON1_DOUBLE_CLICKED 8
   #define BUTTON1_TRIPLE_CLICKED 16
   #define BUTTON2_PRESSED 128
   #define BUTTON2_RELEASED 64
   #define BUTTON2_CLICKED 256
   #define BUTTON2_DOUBLE_CLICKED 512
   #define BUTTON2_TRIPLE_CLICKED 1024
   #define BUTTON3_PRESSED 8192
   #define BUTTON3_RELEASED 4096
   #define BUTTON3_CLICKED 16384
   #define BUTTON3_DOUBLE_CLICKED 32768
   #define BUTTON3_TRIPLE_CLICKED 65536
   #define BUTTON4_PRESSED 524288
   #define BUTTON4_RELEASED 262144
   #define BUTTON4_CLICKED 1048576
   #define BUTTON4_DOUBLE_CLICKED 2097152
   #define BUTTON4_TRIPLE_CLICKED 4194304
#endif

#define DEFAULT_HEIGHT 24
#define DEFAULT_WIDTH  80

typedef std::deque<std::string> messageLineContainer;

/** \class cxWindow
 *  \brief Represents a text-based window on the screen.
 *  Can contain a title, status, and a message to appear
 *  within the window.  A cxWindow can have a border or
 *  be borderless.  A cxWindow may also have a parent and
 *  may also have subwindows.  When setting up a cxWindow
 *  to have a parent, it will tell the parent to also add
 *  the window as one of its subwindows.  Also, when a
 *  cxWindow is destroyed, it will be removed from its
 *  parent's subwindow list (if it has a parent);
 *  additionally, when a cxWindow is destroyed, it will
 *  orphan all of its subwindows (so they have no parent).<br>
 *  Note to authors deriving from cxWindow or a cxWindow-based class:
 *  In the destructor, if you do anything that would affect the screen,
 *  you should first check to see if cxWidgets is initialized (via a call
 *  to cxBase::cxInitialized()), or else the application may segfault.
 *  This might happen if someone writes a simple program using cxWidgets
 *  objects in their main() method.  In that situation, cxBase::cleanup()
 *  would get called before the cxWidgets object destructors get called
 *  when the objects go out of scope.<br><br>
 *  The parent/child cxWindow relationship has the following
 *  effects:<br>
 * <ul>
 * <li>Showing a cxWindow (via show()) also shows the
 *     subwindows (unless you tell show() not to show the
 *     subwindows).
 * <li>If a cxWindow has a parent window, then centerHoriz(),
 *     centerVert(), and center() will center the window
 *     within the parent window; otherwise, the window will
 *     be centered on the entire screen.
 * </ul>
 *  Note: When deriving a window from cxWindow (or other
 *  cxWindow-derived window classes) that contain other
 *  other windows or controls, one method that's important
 *  to override is move(), so that the "child" windows move
 *  properly, in addition to the main window.  You do not
 *  normally need to override centerHoriz(), centerVert(),
 *  or center() however, because these methods end up calling
 *  move() anyway.<br>
 *  Also, if you are overriding show() and/or showModal(), it
 *  is important to honor isEnabled() (for consistency) unless
 *  you have a good reason to do otherwise.  show() and
 *  showModal() in all the windows in the cxWindows API will do
 *  nothing if they are not enabled (more specifically, they will
 *  make sure that the window remains hidden).<br>
 *  In the cxWindow-based classes in the cxWidgets library, by the time
 *  the onLeave function is called, the window will no longer be modal
 *  (i.e., a call to isModal() will return false).<br>
 *  A cxWindow can also be enabled or disabled with a call
 *  brief to setEnabled().  When a cxWindow is disabled, it
 *  will hide and remain hidden (even with a call to show() or
 *  unhide()) until it is enabled again.  Also, a cxWindow
 *  that is hidden won't process user input.<br>
 *  Note that when deriving a class from cxWindow, the
 *  ncurses WINDOW structure mWindow is available to derived
 *  classes, in case derived classes need to do any special
 *  drawing with it or anything special (i.e., setting up a
 *  derived window for scrolling).  Note also that a cxWindow
 *  also has a data member of type PANEL, which comes from
 *  the panel library and is used to manage stacked windows.
 *  This member is not available to derived classes, however.
 *  If you derive a class from cxWindow and need to re-create
 *  mWindow and the panel, you may re-create the window by
 *  first calling the ncurses function delwin() with mWindow
 *  as the parameter and then re-creating it with newwin.
 *  The panel must be re-created, however, by calling
 *  reCreatePanel(), which is a protected member method in
 *  this class.<br>
 *  The constructors of this class will throw a
 *  cxWidgetsException if the underlying ncurses window cannot
 *  be created.  So, at the very least, your main program should
 *  include its code in a try/catch and should catch a const
 *  cxWidgetsException&.  The error message can be retreived via
 *  the getErrorMsg() method.<br>
 *  Note: If a cxWindow or any of its child classes are located
 *  on a cxPanel, you can get a pointer to the cxPanel by using
 *  getParent() and casting the return value to a cxPanel
 *  pointer if you are 100% sure it's a cxPanel.  If you are
 *  unsure, the best way to determine the parent window type is by calling
 *  cxTypeStr(), as folows:
\verbatim
   cxWindow *parent = getParent();
   if (parent != nullptr) {
      if (parent->cxTypeStr() == "cxPanel") {
         // The parent is a cxPanel
      }
   }
\endverbatim
 *  Normally, when showModal() is called, a cxWindow will show
 *  itself and wait for the user to press a key.  To provide an
 *  additional clue that the window has focus, you can call
 *  setChangeColorsOnFocus() and pass it true.<br>
 *  The clear() method will likely need to be implemented in derived
 *  classes that need special behavior.<br>
 *  <br>
 *  Another thing that cxWindow and cxWindow-derived classes do is
 *  maintain sets of attributes for various elements of a cxWindow.  Some
 *  maintain different sets than others.  Methods dealing with adding, setting,
 *  and removing attributes include:
 *  <ul>
 *  <li>addAttr()
 *  <li>setAttr()
 *  <li>removeAttr()
 *  <li>removeAttrs()
 *  <li>getAttrs()
 *  </ul>
 * There is an enumeration for item types, defined in cxWidgetItems.h, which
 * defines these item types:
 *  <ul>
 *  <li>eMESSAGE,        // Message (text within the borders)
 *  <li>eTITLE,          // Title text
 *  <li>eSTATUS,         // Status text
 *  <li>eBORDER,         // Borders
 *  <li>eMENU_SELECTION, // Selected menu items (for cxMenu)
 *  <li>eLABEL,          // Input labels (for cxMultiLineInput & cxInput)
 *  <li>eDATA_READONLY,  // Read-only data items (for cxMultiLineInput & cxInput)
 *  <li>eDATA_EDITABLE,  // Editable data items (for cxMultiLineInput & cxInput)
 *  <li>eHOTKEY          // Hotkey (for cxMenu and cxWindow)
 *  </ul>
 *
*/
class cxWindow : public cxObject
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
       * @param pTitle The window title (appears in the top border).
       * @param pMessage The message to appear in the window.
       * @param pStatus The status (appears in the bottom border).
       * @param pBorderStyle The border style - can be eBS_SINGLE_LINE for
       *  a single-line border or eBS_NOBORDER for no border.
       * @param pExtTitleWindow A pointer to another cxWindow in which to
       *  display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow in which to
       *  display the status.
       * @param pHotkeyHighlighting Whether or not to recognize & use
       *  underline characters when displaying the message text.
       */
      explicit cxWindow(cxWindow *pParentWindow = nullptr,
                        int pRow = 0, int pCol = 0,
                        int pHeight = DEFAULT_HEIGHT,
                        int pWidth = DEFAULT_WIDTH,
                        const std::string& pTitle = "",
                        const std::string& pMessage = "",
                        const std::string& pStatus = "",
                        eBorderStyle pBorderStyle = eBS_SINGLE_LINE,
                        cxWindow *pExtTitleWindow = nullptr,
                        cxWindow *pExtStatusWindow = nullptr,
                        bool pHotkeyHighlighting = false);

      /**
       * \brief Overloaded constructor
       * @param pParentWindow A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pTitle The title to appear at the top of the window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pHotkeyHighlighting Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxWindow(cxWindow *pParentWindow,
               int pRow, int pCol, const std::string& pTitle,
               const std::string& pMessage, const std::string& pStatus,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pHotkeyHighlighting = false);
      /**
       * \brief Overloaded constructor - Centers the window in the middle of the screen.
       * @param pParentWindow A pointer to the parent window.
       * @param pTitle The title to appear at the top of the window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pHotkeyHighlighting Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxWindow(cxWindow *pParentWindow,
               const std::string& pTitle, const std::string& pMessage,
               const std::string& pStatus,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pHotkeyHighlighting = false);

      /**
       * \brief Overloaded constructor - Centers the window in the middle of the screen.
       * @param pParentWindow A pointer to the parent window.
       * @param pTitle The title to appear at the top of the window.
       * @param pMessage The message to appear in the window.
       * @param pHeight The height for the window
       * @param pWidth The width for the window
       * @param pStatus The status to display on the bottom row of the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pHotkeyHighlighting Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxWindow(cxWindow *pParentWindow,
               const std::string& pTitle, const std::string& pMessage,
               const std::string& pStatus,
               int pHeight, int pWidth,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pHotkeyHighlighting = false);

      /**
       * \brief Overloaded constructor - Centers the window in the middle of the screen.
       * @param pParentWindow A pointer to the parent window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pHotkeyHighlighting Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxWindow(cxWindow *pParentWindow,
               const std::string& pMessage, const std::string& pStatus,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pHotkeyHighlighting = false);

      /**
       * \brief Overloaded constructor - Centers the window in the middle of the screen.
       * @param pParentWindow A pointer to the parent window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pHeight The height for the window
       * @param pWidth The width for the window
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pHotkeyHighlighting Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxWindow(cxWindow *pParentWindow,
               const std::string& pMessage, const std::string& pStatus,
               int pHeight, int pWidth,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pHotkeyHighlighting = false);

      /**
       * \brief Overloaded constructor - Centers the window in the middle of the screen.
       * @param pParentWindow A pointer to the parent window.
       * @param pMessage The message to appear in the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pHotkeyHighlighting Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxWindow(cxWindow *pParentWindow, const std::string& pMessage,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pHotkeyHighlighting = false);
      /**
       * \brief Overloaded constructor - Lets you specify the horizontal position
       * @param pParentWindow A pointer to the parent window.
       * @param pHPosition The horizontal position of the window - eHP_CENTER, eHP_LEFT, or eHP_RIGHT
       * @param pTitle The title to appear at the top of the window.
       * @param pMessage The message to appear in the window.
       * @param pStatus The status to display on the bottom row of the window.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow to use as an external status window.
       * @param pHotkeyHighlighting Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      cxWindow(cxWindow *pParentWindow, eHPosition pHPosition,
               const std::string& pTitle, const std::string& pMessage,
               const std::string& pStatus,
               cxWindow *pExtTitleWindow = nullptr,
               cxWindow *pExtStatusWindow = nullptr,
               bool pHotkeyHighlighting = false);

      /**
       * \brief Copy constructor
       * @param pThatWindow Another cxWindow object to be copied
       */
      cxWindow(const cxWindow& pThatWindow);


      /**
       * Destructor
       */
      virtual ~cxWindow();

      /**
       * \brief Centers the window horizontally on the screen
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void centerHoriz(bool pRefresh = true);

      /**
       * \brief Aligns the window horizontally on the screen
       * @param pPosition The alignment (e.g. eHP_CENTER, eHP_LEFT, or eHP_RIGHT)
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void alignHoriz(eHPosition pPosition, bool pRefresh = true);

      /**
       * \brief Centers the window vertically on the screen
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void centerVert(bool pRefresh = true);

      /**
       * \brief Aligns the window vertically on the screen
       * @param pPosition The alignment (e.g. eVP_CENTER, eVP_TOP, eVP_BOTTOM)
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void alignVert(eVPosition pPosition, bool pRefresh = true);

      /**
       * \brief Centers the window on the screen
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void center(bool pRefresh = true);

      /**
       * \brief Accessor for title text - Returns the first string in the
       * \brief set of title strings.
       *
       * @return The first string in the set of title strings
       */
      virtual std::string getTitle() const;

      /**
       * \brief Setter for the title text - This sets the first string in the
       * \brief set of title strings.
       *
       * @param pTitle The new title for the window
       * @param pRefreshTitle Whether or not to refresh the title area (defaults to true)
       */
      virtual void setTitle(const std::string& pTitle, bool pRefreshTitle = true);

      /**
       * \brief Accessor for the status line text
       * @return The status line from the bottom row of window
       */
      std::string getStatus() const;

      /**
       * \brief Mutator for the status line text
       * @param pStatus The new status message for window
       * @param pRefreshStatus Whether or not to refresh the status area (defaults to true)
       */
      virtual void setStatus(const std::string& pStatus, bool pRefreshStatus = true);

      /**
       * \brief Returns the message that appears in the window, concatenating all message lines together
       * @return The message that appears in the window.
       */
      std::string getMessage() const;

      /**
       * \brief Sets the text that appears within the window.
       @param pMessage The new message for the window.
       */
      virtual void setMessage(const std::string &pMessage);

      /**
       * \brief Adds a line to the message to be displayed in the window,
       * \brief below the current message text.
       *
       * @param pLine The line to add
       */
      virtual void addMessageLineBelow(const std::string &pLine);

      /**
       * \brief Adds lines to the message below the current message text.
       *
       * @param pLines The lines to add
       */
      virtual void addMessageLinesBelow(const std::vector<std::string>& pLines);

      /**
       * \brief Adds lines to the message below the current message text.
       *
       * @param pLines The lines to add
       */
      virtual void addMessageLinesBelow(const std::list<std::string>& pLines);

      /**
       * \brief Adds lines to the message below the current message text.
       *
       * @param pLines The lines to add
       */
      virtual void addMessageLinesBelow(const std::deque<std::string>& pLines);

      /**
       * \brief Adds a line to the message to be displayed in the window,
       * \brief above the current message text.
       *
       * @param pLine The line to add
       */
      virtual void addMessageLineAbove(const std::string& pLine);

      /**
       * \brief Adds lines to the message above the current message text.
       *
       * @param pLines The lines to add
       */
      virtual void addMessageLinesAbove(const std::vector<std::string>& pLines);

      /**
       * \brief Adds lines to the message above the current message text.
       *
       * @param pLines The lines to add
       */
      virtual void addMessageLinesAbove(const std::list<std::string>& pLines);

      /**
       * \brief Adds lines to the message above the current message text.
       *
       * @param pLines The lines to add
       */
      virtual void addMessageLinesAbove(const std::deque<std::string>& pLines);

      /**
       * \brief Sets the message lines in the window
       *
       * @param pLines A collection of message lines
       */
      virtual void setMessageLines(const std::vector<std::string>& pLines);

      /**
       * \brief Sets the message lines in the window
       *
       * @param pLines A collection of message lines
       */
      virtual void setMessageLines(const std::list<std::string>& pLines);

      /**
       * \brief Sets the message lines in the window
       *
       * @param pLines A collection of message lines
       */
      virtual void setMessageLines(const std::deque<std::string>& pLines);

      /**
       * \brief Removes a line from the text inside the window (by index).
       *
       * @param pIndex The index of the line to remove
       */
      virtual void removeMessageLine(unsigned pIndex);

      /**
       * \brief Modifies one of the lines displayed in the window.
       *
       * @param pIndex The index of the line
       * @param pLine The new line text
       */
      virtual void setMessageLine(unsigned pIndex, const std::string& pLine);

      /**
       * \brief Sets the horizontal alignment of the title text.
       * \brief Note: This may be deprecated due to cxWindow's
       * \brief more recent feature of being able to set strings
       * \brief to display at different points in the titlebar.
       *
       * @param pHAlignment The horizontal alignment - Can be eHP_CENTER,
       *  eHP_LEFT, or eHP_RIGHT
       */
      void setHorizTitleAlignment(eHPosition pHAlignment);

      /**
       * \brief Accessor for the title text alignment
       * @return The horizontal title alignment - Can be eHP_CENTER, eHP_LEFT, or eHP_RIGHT
       */
      eHPosition getHorizTitleAlignment() const;

      /**
       * \brief Sets the horizontal alignment of the message text
       * @param pHAlignment The horizontal alignment - Can be eHP_CENTER, eHP_LEFT, or eHP_RIGHT
       */
      void setHorizMessageAlignment(eHPosition pHAlignment);

      /**
       * \brief Accessor for the horizontal message text alignment
       * @return The horizontal message alignment - Can be eHP_CENTER, eHP_LEFT, or eHP_RIGHT
       */
      eHPosition getHorizMessageAlignment() const;

      /**
       * \brief Sets the horizontal alignment of the status text
       * @param pHAlignment The horizontal alignment - Can be eHP_CENTER, eHP_LEFT, or eHP_RIGHT
       */
      void setHorizStatusAlignment(eHPosition pHAlignment);

      /**
       * \brief Accessor for the horizontal status text alignment
       * @return The horizontal status alignment - Can be eHP_CENTER, eHP_LEFT, or eHP_RIGHT
       */
      eHPosition getHorizStatusAlignment() const;

      /**
       * \brief Sets the vertical alignment of the message text
       * @param pVAlignment The vertical alignment - Can be eVP_CENTER, eVP_TOP, or eVP_BOTTOM
       */
      void setVerticalMessageAlignment(eVPosition pVAlignment);

      /**
       * \brief Accessor for the vertical message text alignment
       * @return The vertical message alignment - Can be eVP_CENTER, eVP_TOP, or eVP_BOTTOM
       */
      eVPosition getVerticalMessageAlignment() const;

      /**
       * \brief Sets an external cxWindow object to be used for the title, rather
       * \brief than the title line of the window.
       * @param pWindow A pointer to the external window to use for the title.
       * @param pRefreshTitle Whether or not to refresh the title (defsults to true)
       */
      void setExtTitleWindow(cxWindow *pWindow, bool pRefreshTitle = true);

      /**
       * \brief Accessor for the external title window pointer
       * @return The external title window pointer
       */
      cxWindow* getExtTitleWindow() const;

      /**
       * \brief Sets an external cxWindow object to be used for status, rather
       * \brief than the status line of the window.
       * @param pWindow A pointer to the external window to use for status.
       * @param pRefreshStatus Whether or not to refresh the status (defsults to true)
       */
      void setExtStatusWindow(cxWindow *pWindow, bool pRefreshStatus = true);

      /**
       * \brief Accessor for the external status window pointer
       * @return The external status window pointer
       */
      cxWindow* getExtStatusWindow() const;

      /**
       * \brief Returns whether or not the window is hidden.
       * @return Whether or not the window is hidden.
       */
      bool isHidden() const;

      /**
       * \brief Shows the window.
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *   to false.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults
       *   to true.
       * @return This method returns a long to support overriding in derived classes
       *   that might need to return a value.  For cxWindow, cxID_EXIT is
       *   always returned.
       */
      virtual long show(bool pBringToTop = false, bool pShowSubwindows = true);

      /**
       * \brief Shows the window and waits for input.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *   to true.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults
       *   to true.
       *
       * @return A return code based on user input.  Returns cxID_QUIT if the user
       *  presses ESC; otherwise, returns cxID_EXIT.
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = true);

      /**
       * \brief Returns whether or not a call to showModal() will wait for a
       * \brief keypress from the user.  (Some windows may be set not do wait
       * \brief for a keypress in some situations).
       *
       * @return Whether or not a call to showModal() will wait for a keypress
       *  from the user.
       */
      virtual bool modalGetsKeypress() const;

      /**
       * \brief Alias for showModal()
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *   to true.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults
       *    to true.
       *
       * @return See showModal()
       */
      virtual long setFocus(bool pShowSelf = true, bool pBringToTop = true,
                            bool pShowSubwindows = true);

      /**
       * \brief Turns on the ncurses attribute A_REVERSE for the window to
       * \brief make things stand out.
       */
      virtual void setFocusColors();

      /**
       * \brief Turns on the ncurses attribute pAttr for the window to
       * \brief make things stand out.
       */
      virtual void setFocusColors(attr_t pAttr);

      /**
       * \brief Disables focus for this window and unhighlights the message
       */
      virtual void unSetFocusColors();

      /**
       * \brief Disables focus for this window and un-highlights the message (using pAttr)
       */
      virtual void unSetFocusColors(attr_t pAttr);

      /**
       * \brief Erases the window
       * @param pEraseSubwindows Whether or not to erase the subwindows (defaults to
       *    true)
       */
      virtual void erase(bool pEraseSubwindows = true);

      /**
       * \brief Brings the window to the top
       * @param pRefresh Whether to refresh the screen (defaults to true)
       */
      virtual void bringToTop(bool pRefresh = true);

      /**
       * \brief Puts the window on the bottom of all other windows.
       * @param pRefresh Whether to refresh the screen (defaults to false)
       */
      virtual void bringToBottom(bool pRefresh = false);

      /**
       * \brief Returns whether the window is on top of all other windows.
       *
       * @return True if the window is on top of all other windows, or false if not.
       */
      virtual bool isOnTop() const;

      /**
       * \brief Returns whether the window is on the bottom of all other windows.
       *
       * @return True if the window is on the bottom of all other windows, or false if not.
       */
      virtual bool isOnBottom() const;

      /**
       * \brief Returns whether the window is above another window.
       *
       * @param pThatWindow The other window
       *
       * @return True if the window is above pThatWindow; false if the window is below pThatWindow.
       */
      virtual bool isAbove(const cxWindow& pThatWindow) const;

      /**
       * \brief Returns whether a window is below another window.
       *
       * @param pThatWindow The other window
       *
       * @return True if the window is below pThatWindow; false if the window is above pThatWindow.
       */
      virtual bool isBelow(const cxWindow& pThatWindow) const;

      /**
       * \brief Hides the window
       *
       * @param pHideSubwindows Whether or not to also hide the subwindows (defaults
       *    to true)
       */
      virtual void hide(bool pHideSubwindows = true);

      /**
       * \brief Un-hides the window
       *
       * @param pUnhideSubwindows Whether or not to also unhide the subwindows
       *    (defaults to true)
       */
      virtual void unhide(bool pUnhideSubwindows = true);

      /**
       * \brief Draws the border (based on the border style)
       *
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the window.
       * @param pWidth The width of the window.
       * @param pTop draw the top border
       * @param pBottom draw the bottom border
       * @param pLeft draw the left border
       * @param pRight draw the right border
       */
      virtual void drawBorder(int pRow, int pCol, int pHeight, int pWidth,
                              bool pTop=true, bool pBottom=true,
                              bool pLeft=true, bool pRight=true);

      /**
       * \brief Draws a box in the window
       * @param pHeight The height of the window.
       * @param pWidth The width of the window.
       */
      virtual void drawBorder(int pHeight, int pWidth);

      /**
       * \brief Draws a box for the window border
       */
      virtual void drawBorder();

      /**
       * \brief Returns the top row of the window
       * @return The top row of the window
       */
      virtual int top() const;

      /**
       * \brief Returns the center row of the window
       * @return The center row of the window
       */
      virtual int centerRow() const;

      /**
       * \brief Returns the bottom row of the window
       * @return The bottom row of the window
       */
      virtual int bottom() const;

      /**
       * \brief Returns the left column of the window
       * @return The leftmost column of the window
       */
      virtual int left() const;

      /**
       * \brief Returns the center column of the window
       * @return The center column of window
       */
      virtual int centerCol() const;

      /**
       * \brief Returns the right column of the window
       * @return The rightmost column of window
       */
      virtual int right() const;

      /**
       * \brief Changes the width of the window.
       *
       * @param pWidth The new width for the window
       *
       * @return The nCurses return/error code as a result of changing the window size
       */
      virtual int width(int pWidth);

      /**
       * \brief Returns the width of the window
       * @return The width of the window
       */
      virtual int width() const;

      /**
       * \brief Changes the height of the window.
       *
       * @param pHeight The new height for the window
       *
       * @return The nCurses return/error code as a result of changing the window size
       */
      virtual int height(int pHeight);

      /**
       * \brief Returns the height of the window
       * @return The height of the window
       */
      virtual int height() const;

      /**
       * \brief Returns the coordinates of the rectangle defining the window.
       *
       * @param pTop (OUT) The top row
       * @param pLeft (OUT) The left column
       * @param pBottom (OUT) The bottom row
       * @param pRight (OUT) The right column
       */
      virtual void getRect(int& pTop, int& pLeft, int& pBottom, int& pRight);

      /**
       * \brief Returns whether a screen location is in the window (including
       * \brief the borders).
       *
       * @param pY A vertical character position
       * @param pX A horizontal character position
       *
       * @return true if the coordinate is in the window, or false if not.
       */
      virtual bool pointIsInWindow(int pY, int pX) const;

      /**
       * \brief Returns whether a screen location is in the title of the window
       *
       * @param pY A vertical character position
       * @param pX A horizontal character position
       *
       * @return true if the coordinate is in the title of a window, or false if not.
       */
      virtual bool pointIsInTitle(int pY, int pX) const;

      /**
       * \brief Returns the height & width of the window.
       *
       * @param pHeight (OUT) The window's height
       * @param pWidth (OUT) The window's width
       */
      virtual void getSize(int& pHeight, int& pWidth);

      /**
       * \brief Adds an ncurses attribute to use for one of the items in the
       * \brief window (see the e_WidgetItems enumeration).
       *
       * @param pItem The item to add the attribute for (see the e_WidgetItems
       *  enumeration).
       * @param pAttr The ncurses atribute to add.
       */
      virtual void addAttr(e_WidgetItems pItem, attr_t pAttr);

      /**
       * \brief Sets the ncurses attribute to use for one of the items in the
       * \brief window (see the e_WidgetItems enumeration).  Clears the current
       * \brief set of attributes for the item and inserts the given attribute
       * \brief into the set.
       *
       * @param pItem The item to apply the attribute for (see the e_WidgetItems
       *  enumeration).
       * @param pAttr The ncurses atribute to apply.
       */
      virtual void setAttr(e_WidgetItems pItem, attr_t pAttr);

      /**
       * \brief Removes an ncurses attribute from one of the item lists.
       *
       * @param pItem The item to remove the attribute for (see the
       *  e_WidgetItems enumeration).
       * @param pAttr The ncurses attribute to remove
       */
      virtual void removeAttr(e_WidgetItems pItem, attr_t pAttr);

      /**
       * \brief Removes all attributes for a given window item.
       *
       * @param pItem The item to remove attributes for (see the e_WidgetItems
       *  enumeration).
       */
      virtual void removeAttrs(e_WidgetItems pItem);

      /**
       * \brief Returns the set of ncurses attributes for a given item.
       *
       * @param pItem The item to retrieve attributes for (see the e_WidgetItems
       *  enumeration).
       * @param pAttrs This will contain the attributes for the item.
       */
      virtual void getAttrs(e_WidgetItems pItem, std::set<attr_t>& pAttrs) const;

      /**
       * \brief Changes the window's width and height.  The window's upper-left
       * \brief coordinate stays the same.
       * @param pNewHeight The new height
       * @param pNewWidth The new width
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       */
      virtual void resize(int pNewHeight, int pNewWidth, bool pRefresh = true);

      /**
       * \brief Changes the window's position, based on a new upper-left corner
       * @param pNewRow The new topmost row of the form
       * @param pNewCol The new leftmost column of the form
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true);

      /**
       * \brief Changes the window's position via vertical & horizontal offsets.
       * \brief Note that this method doesn't need to be overriden in deriving classes,
       * \brief since it calls move() with the proper values; thus, for moving, it is
       * \brief only important that move() be overridden in derived classes if anything
       * \brief special needs to be done when moving the window.
       *
       * @param pVerticalOffset The vertical offset by which to move the window
       * @param pHorizOffset The horizontal offset by which to move the window
       * @param pRefresh Whether or not to refresh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool moveRelative(int pVerticalOffset, int pHorizOffset, bool pRefresh = true);

      /**
       * \brief Returns the number of subwindows in this window.
       * @return An unsigned int representing the number of subwindows
       */
      unsigned numSubWindows();

      /**
       * \brief Sets the message of one of the subwindows (by index).
       * @param pIndex The index of the subwindow
       * @param pMessage The new message to set in the subwindow
       */
      void setSubWinMessage(unsigned pIndex, const std::string& pMessage);

      /**
       * \brief Sets the message of one of the subwindows (by title).
       * @param pTitle The title of the subwindow
       * @param pMessage The new message to set in the subwindow
       */
      void setSubWinMessage(const std::string& pTitle, const std::string& pMessage);

      /**
       * \brief Returns true if window has focus, false otherwise
       * @return A bool indicating whether or not the window has focus
       */
      virtual bool hasFocus() const;

      /**
       * \brief Sets the border style
       *
       * @param pBorderStyle A eBorderStyle enumeration value (defined in
       *  cxBorderStyles.h):<br>
       *  eBS_NOBORDER: No border<br>
       *  eBS_SINGLE_LINE: Single line<br>
       *  eBS_DOUBLE_LINE: Double line (only supported on terminals that
       *   support high ASCII/Codepage 437)<br>
       *  eBS_SPACE: Spaces <br>
       */
      virtual void setBorderStyle(eBorderStyle pBorderStyle);

      /**
       * \brief Returns the border style
       * @return The border style - can be eBS_NOBORDER or eBS_SINGLE_LINE
       */
      eBorderStyle getBorderStyle() const;

      /**
       * @brief Does this window have a border?
       * @return bool true if it does have a border, false otherwise
       */
      bool hasBorder() const;

      /**
       * \brief Returns a pointer to the parent window
       * @return A pointer to the parent window
       */
      virtual cxWindow* getParent() const;

       /**
       * \brief Changes the pointer to the parent window; changes the parent window
       * @param pNewParent Pointer to the new parent window
       */
      void setParent(cxWindow *pNewParent);

      /**
       * \brief Returns a string containing all the characters
       * \brief contained in this window.  The resulting window
       * \brief dump will be placed into pResult.  Note that
       * \brief each line is separated by a '\\n' character.
       *
       * @param pResult This string will contain all characters contained in
       *  the window.
       */
      virtual void dump(std::string& pResult);

      /**
       * \brief Shows the subwindows for the window.
       * @param pBringToTop Whether or not to bring the subwindow to the top.
       *   Defaults to true.
       * @param pShowSubwindows Whether or not to show the subwindows for the
       *   subwindows.  Defaults to true.
       */
      virtual void showSubwindows(bool pBringToTop = true, bool pShowSubwindows = true);

      /**
       * \brief Assignment operator
       *
       * @param pThatWindow Another cxWindow to be copied
       *
       * @return The current window
       */
      cxWindow& operator =(const cxWindow& pThatWindow);

      /**
       * \brief Writes text on the window.  Note that the text is not permanant, so
       * \brief any subsequent calls to show() or setFocus() will draw over the
       * \brief text written with this method.
       * \brief Also, for the text to show up, this method should be called <u>after</u>
       * \brief a call to show().
       *
       * @param pRow The row in the window at which to write the text
       * @param pCol The column in the window at which to write the text
       * @param pText The text to write in the window
       * @param pRefresh Whether or not to refresh the window after writing the text
       */
      virtual void writeText(int pRow, int pCol, const std::string& pText, bool pRefresh = true);

      /**
       * \brief Enables/disables the use of hotkey attributes when writing the message.
       * \brief If true, wherever there's an ampersand (&) before a character in
       * \brief the message, that character will be underlined.  Otherwise, the
       * \brief message will be written verbatim.
       *
       * @param pHotkeyHighlighting Whether or not to use hotkey attributes when writing
       *        the message.
       */
      virtual void setHotkeyHighlighting(bool pHotkeyHighlighting);

      /**
       * \brief Returns whether hotkey attributes are used when writing the message.
       *
       * @return Whether or not hotkey attributes are used when writing the message.
       */
      bool getHotkeyHighlighting() const;

      /**
       * \brief Returns whether or not the window is enabled
       *
       * @return Whether or not the window is enabled (true/false)
       */
      virtual bool isEnabled() const;

      /**
       * \brief Enables or disables the window.  If the window is
       * \brief disabled, it won't show or process input (it will
       * \brief remain hidden, even after a call to show() or
       * \brief unhide()).  Note that if you are deriving a class
       * \brief from cxWindow, the 'enabled'-ness should be
       * \brief honored if the class has any methods that process
       * \brief user input (i.e., showModal() or setFocus()).
       *
       * @param pEnabled Whether the window should be enabled or not (true/false)
       */
      virtual void setEnabled(bool pEnabled);

      /**
       * \brief Sets whether the window should disable the cursor
       * \brief when it's shown (this is the default behavior).
       *
       * @param pDisableCursorOnShow Whether or not to disable
       *  the cursor when shown
       */
      virtual void setDisableCursorOnShow(bool pDisableCursorOnShow);

      /**
       * \brief Returns whether or not the window will disable
       * \brief the cursor when shown.
       *
       * @return Whether or not the window will disable the cursor
       *  when shown
       */
      virtual bool getDisableCursorOnShow() const;

      /**
       * \brief Returns whether the window exits its modal input loop when
       * \brief a mouse click occurs outside the window.
       *
       * @return true if the window exits on outside mouse clicks
       */
      virtual bool getExitOnMouseOutside() const;

      /**
       * \brief Sets whether the window should exit its modal input loop
       * \brief when a mouse click occurs outside the window.  Useful for
       * \brief container widgets like cxGrid that manage focus across
       * \brief multiple child widgets.
       *
       * @param pExitOnMouseOutside Whether to exit on outside mouse clicks
       */
      virtual void setExitOnMouseOutside(bool pExitOnMouseOutside);

      /**
       * \brief Returns the last key pressed by the user (for showModal(), etc.)
       *
       * @return The last key pressed by the user
       */
      virtual int getLastKey() const;

      /**
       * \brief Sets the last keypress
       *
       * @param pLastKey A key (represented by an int)
       */
      virtual void setLastKey(int pLastKey);

      /**
       * \brief Returns whether or not the last keypress was a mouse event.
       *
       * @return Boolean: true if the last keypress was a mouse event, or false
       *  if not.
       */
      virtual bool lastKeyWasMouseEvt() const;

      /**
       * \brief Returns the coordinates of the last mouse event in the window.
       * \brief If there has not yet been a mouse event or if the mouse is not
       * \brief supported by the computer's version of ncurses, this will
       * \brief return (0, 0).
       *
       * @param pY (OUT) The vertical coordinate
       * @param pX (OUT) The horizontal coordinate
       */
      virtual void getLastMouseEvtCoords(int& pY, int& pX);

      /**
       * \brief Returns whether the window will change
       * \brief colors when focus is set.
       *
       * @return Whether or not the window will change
       *  colors when focus is set.
       */
      virtual bool getChangeColorsOnFocus() const;

      /**
       * \brief Sets whether colors should change when focus
       * \brief is set via showModal() (to visually show
       * \brief that focus is set).  The default behavior is
       * \brief false.
       *
       * @param pChangeColorsOnFocus Whether or not the window's
       *  colors should change when focus is set.
       */
      virtual void setChangeColorsOnFocus(bool pChangeColorsOnFocus);

      /**
       * \brief Sets the window's "on focus" function
       *
       * @param pFunction The function to be run
       */
      virtual void setOnFocusFunction(const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets the "on focus" function pointer
       * \brief The function must have this signature: string func(void*, void*, void*, void*).
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseVal Whether or not the function's return value should be
       *  used.  This can be useful in some deriving classes.
       * @param pExitAfterRun Whether or not to exit after the function
       *  finishes running (defaults to false).
       */
      virtual void setOnFocusFunction(funcPtr4 pFunction, void *p1, void *p2,
                              void *p3, void *p4, bool pUseVal = false,
                              bool pExitAfterRun = false);

      /**
       * \brief Sets the "on focus" function pointer
       * \brief The function must have this signature: string func(void*, void*).
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseVal Whether or not the function's return value should be
       *  used.  This can be useful in some deriving classes.
       * @param pExitAfterRun Whether or not to exit after the function
       *  finishes running (defaults to false).
       */
      virtual void setOnFocusFunction(funcPtr2 pFunction, void *p1, void *p2,
                              bool pUseVal = false, bool pExitAfterRun = false);

      /**
       * \brief Sets the "on focus" function pointer
       * \brief The function must have this signature: string func().
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func()
       * @param pUseVal Whether or not the function's return value should be
       *  used.  This can be useful in some deriving classes.
       * @param pExitAfterRun Whether or not to exit after the function
       *  finishes running (defaults to false).
       */
      virtual void setOnFocusFunction(funcPtr0 pFunction, bool pUseVal = false,
                                      bool pExitAfterRun = false);

      /**
       * \brief Sets the window's "on leave" function
       *
       * @param pFunction The function to be run
       */
      virtual void setOnLeaveFunction(const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets the "on leave" function pointer
       * \brief The function must have this signature: string func(void*, void*, void*, void*).
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setOnLeaveFunction(funcPtr4 pFunction, void *p1, void *p2, void *p3,
                                      void *p4);

      /**
       * \brief Sets the "on focus" function pointer
       * \brief The function must have this signature: string func(void*, void*).
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       */
      virtual void setOnLeaveFunction(funcPtr2 pFunction, void *p1, void *p2);

      /**
       * \brief Sets the "on focus" function pointer
       * \brief The function must have this signature: string func().
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func()
       */
      virtual void setOnLeaveFunction(funcPtr0 pFunction);

      /**
       * \brief Runs the onFocus function, if it's set.
       *
       * @param pFunctionRetval If not nullptr, the string that this points to
       *  will contain the return value of the onFocus function.  Defaults to
       *  nullptr.
       *
       * @return Whether the input loop should exit (returns the value of
       *  the onFocus function's mExitAfterRun, or false if the onFocus
       *  function isn't set).
       */
      virtual bool runOnFocusFunction(std::string *pFunctionRetval = nullptr);

      /**
       * \brief Runs the onLeave function, if it's set.  However, if the last
       * \brief keypress is in mExitKeys or mQuitKeys and the boolean for that
       * \brief key is false, the onLeave function won't be run.
       *
       * @param pFunctionRetval If not nullptr, the string that this points to
       *  will contain the return value of the onLeave function.  Defaults to
       *  nullptr.
       *
       * @return Whether the input loop should exit (returns the value of
       *  the onLeave function's mExitAfterRun, or false if the onLeave
       *  function isn't set).
       */
      virtual bool runOnLeaveFunction(std::string *pFunctionRetval = nullptr);

      /**
       * \brief Sets whether or not the onFocus function should run.
       *
       * @param pRunOnFocus If true, the onFocus function will run when
       *  the window gains focus; if false, it won't run.
       */
      virtual void toggleOnFocusFunction(bool pRunOnFocus);

      /**
       * \brief Sets whether or not the onLeave function should run.
       *
       * @param pRunOnFocus If true, the onLeave function will run when
       *  the window loses focus; if false, it won't run.
       */
      virtual void toggleOnLeaveFunction(bool pRunOnLeave);

      /**
       * \brief Returns whether or not the onFocus function is set to run.
       *
       * @return Whether or nto the onFocus function is set to run
       */
      virtual bool onFocusFunctionEnabled() const;

      /**
       * \brief Returns whether or not the onLeave function is set to run.
       *
       * @return Whether or nto the onLeave function is set to run
       */
      virtual bool onLeaveFunctionEnabled() const;

      /**
       * \brief Accessor for the onFocus function
       *
       * @return The onFocus function
       */
      virtual const std::shared_ptr<cxFunction>& getOnFocusFunction() const;

      /**
       * \brief Accessor for the onLeave function
       *
       * @return The onLeave function
       */
      virtual const std::shared_ptr<cxFunction>& getOnLeaveFunction() const;

      /**
       * \brief Returns whether or not the window is currently
       * \brief being shown modally.
       *
       * @return Whether or not the window is currently being
       *  shown modally
       */
      virtual bool isModal() const;

      /**
       * \brief Sets a function to be called when a key is pressed.
       *
       * @param pKey The key to use for the function
       * @param pFunction The function to call. This can be an instance of
       * one of the derived cxFunction classes as well.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pKey, const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets a function to be called when a key is pressed.
       *
       * @param pKey The key to use for the function
       * @param pFunction The function to call
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
                                  bool pRunOnLeaveFunction = true);

      /**
       * \brief Sets a function to be called when a key is pressed.
       *
       * @param pKey The key to use for the function
       * @param pFunction The function to call
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
                                  bool pRunOnLeaveFunction = true);

      /**
       * \brief Sets a function to be called when a key is pressed.  This
       * \brief one uses a function that takes no parameters and returns
       * \brief a string, as in the following signature:<br>
       * \brief string someFunc()
       *
       * @param pKey The key to use for the function
       * @param pFunction The function to call
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
                                  bool pRunOnLeaveFunction = true);

      /**
       * \brief Removes a function pointer for a keypress so that it will no
       * \brief longer fire a function.
       *
       * @param pKey The hotkey for the function to remove
       */
      virtual void clearKeyFunction(int pKey);

      /**
       * \brief Removes a keypress function pointer.
       *
       * @param pFunction A pointer to the function to be removed
       */
      virtual void clearKeyFunctionByPtr(funcPtr4 pFunction);

      /**
       * \brief Removes a keypress function pointer.
       *
       * @param pFunction A pointer to the function to be removed
       */
      virtual void clearKeyFunctionByPtr(funcPtr2 pFunction);

      /**
       * \brief Removes a keypress function pointer.
       *
       * @param pFunction A pointer to the function to be removed
       */
      virtual void clearKeyFunctionByPtr(funcPtr0 pFunction);

      /**
       * \brief Clears the list of external functions fired by hotkeys.
       */
      virtual void clearKeyFunctions();

      /**
       * \brief Returns whether an external function exists for a keypress.
       *
       * @param pKey The key to look for
       *
       * @return Whether or not the window has an external function pointer
       *  set up for the key
       */
      virtual bool hasKeyFunction(int pKey) const;

      /**
       * \brief Sets a function to be called for a certain mouse state.
       *
       * @param pMouseState The mouse state to fire the function.  This is
       *  a list of mouse states:<br>
       *  BUTTON1_PRESSED          Mouse button 1 down<br>
       *  BUTTON1_RELEASED         Mouse button 1 up<br>
       *  BUTTON1_CLICKED          Mouse button 1 clicked<br>
       *  BUTTON1_DOUBLE_CLICKED   Mouse button 1 double clicked<br>
       *  BUTTON1_TRIPLE_CLICKED   Mouse button 1 triple clicked<br>
       *  BUTTON2_PRESSED          Mouse button 2 down<br>
       *  BUTTON2_RELEASED         Mouse button 2 up<br>
       *  BUTTON2_CLICKED          Mouse button 2 clicked<br>
       *  BUTTON2_DOUBLE_CLICKED   Mouse button 2 double clicked<br>
       *  BUTTON2_TRIPLE_CLICKED   Mouse button 2 triple clicked<br>
       *  BUTTON3_PRESSED          Mouse button 3 down<br>
       *  BUTTON3_RELEASED         Mouse button 3 up<br>
       *  BUTTON3_CLICKED          Mouse button 3 clicked<br>
       *  BUTTON3_DOUBLE_CLICKED   Mouse button 3 double clicked<br>
       *  BUTTON3_TRIPLE_CLICKED   Mouse button 3 triple clicked<br>
       *  BUTTON4_PRESSED          Mouse button 4 down<br>
       *  BUTTON4_RELEASED         Mouse button 4 up<br>
       *  BUTTON4_CLICKED          Mouse button 4 clicked<br>
       *  BUTTON4_DOUBLE_CLICKED   Mouse button 4 double clicked<br>
       *  BUTTON4_TRIPLE_CLICKED   Mouse button 4 triple clicked<br>
       * @param pFunction The function to call. This can be an instance of
       * one of the derived cxFunction classes as well.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setMouseFunction(int pMouseState, const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets a function to be called for a certain mouse state.
       *
       * @param pMouseState The mouse state to fire the function.  This is
       *  a list of mouse states:<br>
       *  BUTTON1_PRESSED          Mouse button 1 down<br>
       *  BUTTON1_RELEASED         Mouse button 1 up<br>
       *  BUTTON1_CLICKED          Mouse button 1 clicked<br>
       *  BUTTON1_DOUBLE_CLICKED   Mouse button 1 double clicked<br>
       *  BUTTON1_TRIPLE_CLICKED   Mouse button 1 triple clicked<br>
       *  BUTTON2_PRESSED          Mouse button 2 down<br>
       *  BUTTON2_RELEASED         Mouse button 2 up<br>
       *  BUTTON2_CLICKED          Mouse button 2 clicked<br>
       *  BUTTON2_DOUBLE_CLICKED   Mouse button 2 double clicked<br>
       *  BUTTON2_TRIPLE_CLICKED   Mouse button 2 triple clicked<br>
       *  BUTTON3_PRESSED          Mouse button 3 down<br>
       *  BUTTON3_RELEASED         Mouse button 3 up<br>
       *  BUTTON3_CLICKED          Mouse button 3 clicked<br>
       *  BUTTON3_DOUBLE_CLICKED   Mouse button 3 double clicked<br>
       *  BUTTON3_TRIPLE_CLICKED   Mouse button 3 triple clicked<br>
       *  BUTTON4_PRESSED          Mouse button 4 down<br>
       *  BUTTON4_RELEASED         Mouse button 4 up<br>
       *  BUTTON4_CLICKED          Mouse button 4 clicked<br>
       *  BUTTON4_DOUBLE_CLICKED   Mouse button 4 double clicked<br>
       *  BUTTON4_TRIPLE_CLICKED   Mouse button 4 triple clicked<br>
       * @param pFunction The function to call
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
      virtual bool setMouseFunction(int pMouseState, funcPtr4 pFunction,
                                  void *p1, void *p2, void *p3, void *p4,
                                  bool pUseReturnVal = false,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true);

      /**
       * \brief Sets a function to be called for a certain mouse state.
       *
       * @param pMouseState The mouse state to fire the function.  This is
       *  a list of mouse states:<br>
       *  BUTTON1_PRESSED          Mouse button 1 down<br>
       *  BUTTON1_RELEASED         Mouse button 1 up<br>
       *  BUTTON1_CLICKED          Mouse button 1 clicked<br>
       *  BUTTON1_DOUBLE_CLICKED   Mouse button 1 double clicked<br>
       *  BUTTON1_TRIPLE_CLICKED   Mouse button 1 triple clicked<br>
       *  BUTTON2_PRESSED          Mouse button 2 down<br>
       *  BUTTON2_RELEASED         Mouse button 2 up<br>
       *  BUTTON2_CLICKED          Mouse button 2 clicked<br>
       *  BUTTON2_DOUBLE_CLICKED   Mouse button 2 double clicked<br>
       *  BUTTON2_TRIPLE_CLICKED   Mouse button 2 triple clicked<br>
       *  BUTTON3_PRESSED          Mouse button 3 down<br>
       *  BUTTON3_RELEASED         Mouse button 3 up<br>
       *  BUTTON3_CLICKED          Mouse button 3 clicked<br>
       *  BUTTON3_DOUBLE_CLICKED   Mouse button 3 double clicked<br>
       *  BUTTON3_TRIPLE_CLICKED   Mouse button 3 triple clicked<br>
       *  BUTTON4_PRESSED          Mouse button 4 down<br>
       *  BUTTON4_RELEASED         Mouse button 4 up<br>
       *  BUTTON4_CLICKED          Mouse button 4 clicked<br>
       *  BUTTON4_DOUBLE_CLICKED   Mouse button 4 double clicked<br>
       *  BUTTON4_TRIPLE_CLICKED   Mouse button 4 triple clicked<br>
       * @param pMouseState The mouse state to fire the function
       * @param pFunction The function to call
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
      virtual bool setMouseFunction(int pMouseState, funcPtr2 pFunction,
                                  void *p1, void *p2,
                                  bool pUseReturnVal = false,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true);

      /**
       * \brief Sets a function to be called for a certain mouse state.  This
       * \brief one uses a function that takes no parameters and returns a
       * \brief string.
       *
       * @param pMouseState The mouse state to fire the function.  This is
       *  a list of mouse states:<br>
       *  BUTTON1_PRESSED          Mouse button 1 down<br>
       *  BUTTON1_RELEASED         Mouse button 1 up<br>
       *  BUTTON1_CLICKED          Mouse button 1 clicked<br>
       *  BUTTON1_DOUBLE_CLICKED   Mouse button 1 double clicked<br>
       *  BUTTON1_TRIPLE_CLICKED   Mouse button 1 triple clicked<br>
       *  BUTTON2_PRESSED          Mouse button 2 down<br>
       *  BUTTON2_RELEASED         Mouse button 2 up<br>
       *  BUTTON2_CLICKED          Mouse button 2 clicked<br>
       *  BUTTON2_DOUBLE_CLICKED   Mouse button 2 double clicked<br>
       *  BUTTON2_TRIPLE_CLICKED   Mouse button 2 triple clicked<br>
       *  BUTTON3_PRESSED          Mouse button 3 down<br>
       *  BUTTON3_RELEASED         Mouse button 3 up<br>
       *  BUTTON3_CLICKED          Mouse button 3 clicked<br>
       *  BUTTON3_DOUBLE_CLICKED   Mouse button 3 double clicked<br>
       *  BUTTON3_TRIPLE_CLICKED   Mouse button 3 triple clicked<br>
       *  BUTTON4_PRESSED          Mouse button 4 down<br>
       *  BUTTON4_RELEASED         Mouse button 4 up<br>
       *  BUTTON4_CLICKED          Mouse button 4 clicked<br>
       *  BUTTON4_DOUBLE_CLICKED   Mouse button 4 double clicked<br>
       *  BUTTON4_TRIPLE_CLICKED   Mouse button 4 triple clicked<br>
       * @param pMouseState The mouse state to fire the function
       * @param pFunction The function to call
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
      virtual bool setMouseFunction(int pMouseState, funcPtr0 pFunction,
                                  bool pUseReturnVal = false,
                                  bool pExitAfterRun = false,
                                  bool pRunOnLeaveFunction = true);

      /**
       * \brief Removes a function pointer for a mouse event so that it will
       * \brief no longer fire a function.
       *
       * @param pMouseState The mouse state for the function to remove
       */
      virtual void clearMouseFunction(int pMouseState);

      /**
       * \brief Clears the list of external functions fired by mouse events.
       */
      virtual void clearMouseFunctions();

      /**
       * \brief Returns whether an external function exists for a mouse event.
       *
       * @param pMouseState A mouse state t o look for
       *
       * @return Whethre or not the window has an external function pointer set
       *  up for the mouse event
       */
      virtual bool hasMouseFunction(int pMouseState) const;

      /**
       * \brief Returns a begin iterator to the map of cxFunction pointers.
       * \brief The map key is the keypress, and the value is a pointer to
       * \brief a cxFunction, which calls an external function.  The object
       * \brief pointed to will actually be either a cxFunction2 or a
       * \brief cxFunction4.
       *
       * @return A begin iterator to the map of key functions
       */
      std::map<int, std::shared_ptr<cxFunction> >::iterator keyFunctions_begin();

      /**
       * \brief Returns an end iterator to the map of cxFunction pointers.
       * \brief The map key is the keypress, and the value is a pointer to
       * \brief a cxFunction, which calls an external function.  The object
       * \brief pointed to will actually be either a cxFunction2 or a
       * \brief cxFunction4.
       *
       * @return An end iterator to the map of key functions
       */
      std::map<int, std::shared_ptr<cxFunction> >::iterator keyFunctions_end();

      /**
       * \brief Populates a collection of strings representing the keys
       * \brief that are tied to function pointers.
       *
       * @param pKeys A collection that will contain strings representing the
       *  keys that are associated with functions (i.e., F1-F12 for function
       *  keys, individual characters representing different keys, etc.).
       */
      virtual void getFunctionKeyStrings(std::vector<std::string>& pKeys) const;

      /**
       * \brief Populates a collection with integers representing the keys
       * \brief that are tied to function pointers.
       *
       * @param pKeys A collection that will contain integers representing the
       *  keys that are associated with functions.
       */
      virtual void getFunctionKeys(std::vector<int>& pKeys) const;

      /**
       * \brief Toggles the order in which subwindows will be shown.
       *
       * @param pShowSubwinsForward If true, the subwindows will be shown
       *  in forward order.  If false, the subwindows will be shown in
       *  reverse order.
       */
      virtual void setShowSubwinsForward(bool pShowSubwinsForward);

      /**
       * \brief Returns whether or not the subwindows will be shown
       * \brief forward (if false, that means the subwindows will
       * \brief be shown in reverse order).
       *
       * @return Whether or not the subwindows will be shown in forward order
       */
      virtual bool getShowSubwinsForward() const;

      /**
       * \brief Sets whether the window will show itself before
       * \brief showing its subwindows.
       *
       * @param pShowSelfBeforeSubwins If true, the window will show
       *  itself before showing its subwindows.  If false, the window
       *  will show itself <b>after</b> showing its subwindows.
       */
      virtual void setShowSelfBeforeSubwins(bool pShowSelfBeforeSubwins);

      /**
       * \brief Returns whether or not the window will show itself before
       * \brief showing its subwindows.
       *
       * @return Whether or not the window will show itself before showing
       *  its subwindows
       */
      virtual bool getShowSelfBeforeSubwins() const;

      /**
       * \brief Returns the return code set by the last call to
       * \brief showModal() based on the user's interaction.  The
       * \brief return code is set to cxID_EXIT by default before
       * \brief the first call to showModal().
       *
       * @return The return code set by the last call to showModal()
       */
      virtual long getReturnCode() const;

      /**
       * \brief Adds/sets a string to appear in the title at
       * \brief a given horizontal position in the top row.
       * \brief If there is already a string set at the given
       * \brief horizontal position, it will be changed to
       * \brief the new string passed in.  Also, the string will
       * \brief be truncated to fit within the top row, if needed.
       *
       * @param pHPos The horizontal position for the string.  This
       *  must be within the borders (>= 1 and less than width()).
       * @param pStr The string to use in the top row
       * @param pRefreshTitle Whether or not to refresh the title line
       *  (defaults to false)
       *
       * @return True if pStr was added or false if not.
       */
      virtual bool addTitleString(int pHPos, const std::string& pStr,
                                  bool pRefreshTitle = false);

      /**
       * \brief Returns the title string set at a given horizontal position.
       * \brief Will return a blank string if there is no title string
       * \brief at the given horizontal position.
       *
       * @param pHPos The horizontal position of the title string.  Note that
       *  the main title string is at position 1 (not 0) because it is
       *  positioned to fit within the borders.
       *
       * @return The title string at that position, or a blank string if there
       *  is no string there.
       */
      virtual std::string getTitleString(int pHPos) const;

      /**
       * \brief Returns the map of title strings for the window.
       * \brief The key is the horizontal position in the title line,
       * \brief and the value is the string at that location.
       *
       * @return The map of title strings for the window
       */
      virtual std::map<int, std::string> getTitleStrings() const;

      /**
       * \brief Returns the map of title strings for the window (using an
       * \brief out parameter rather than the return value).  The key is the
       * \brief horizontal position in the title line, and the value is the
       * \brief string at that location.
       *
       * @param pTitleStrings (OUT) This will contain the title strings.
       */
      virtual void getTitleStrings(std::map<int, std::string>& pTitleStrings) const;

      /**
       * \brief Sets all the title strings via a map of horizontal
       * \brief locations and strings.
       *
       * @param pTitleStrings A map containing the title strings with
       *  the horizontal position as the key.
       */
      virtual void setTitleStrings(const std::map<int, std::string>& pTitleStrings);

      /**
       * \brief Removes a title string at a given horizontal position.
       *
       * @param pHPos The horizontal position of the title string to remove
       */
      virtual void removeTitleString(int pHPos);

      /**
       * \brief Removes all title strings.
       */
      virtual void removeTitleStrings();

      /**
       * \brief Adds/sets a string to appear in the status line at
       * \brief a given horizontal position in the bottom row.
       * \brief If there is already a string set at the given
       * \brief horizontal position, it will be changed to
       * \brief the new string passed in.  Also, the string will
       * \brief be truncated to fit within the bottom row, if needed.
       *
       * @param pHPos The horizontal position for the string.  This
       *  must be within the borders (>= 1 and less than width()).
       * @param pStr The string to use in the bottom row
       * @param pRefreshStatus Whether or not to refresh the status line
       *  (defaults to false)
       *
       * @return True if pStr was added or false if not.
       */
      virtual bool addStatusString(int pHPos, const std::string& pStr,
                                   bool pRefreshStatus = false);

      /**
       * \brief Returns the status string set at a given horizontal position.
       * \brief Will return a blank string if there is no string at the given
       * \brief horizontal position.
       *
       * @param pHPos The horizontal position of the status string.  Note that
       *  the main status string is at position 1 (not 0) because it is
       *  positioned to fit within the borders.
       *
       * @return The status string at that position, or a blank string if there
       *  is no string there.
       */
      virtual std::string getStatusString(int pHPos) const;

      /**
       * \brief Returns the map of status strings for the window.
       * \brief The key is the horizontal position in the title line,
       * \brief and the value is the string at that location.
       *
       * @return The map of status strings for the window
       */
      virtual std::map<int, std::string> getStatusStrings() const;

      /**
       * \brief Returns the map of status strings for the window (using an
       * \brief out parameter rather than the return value).  The key is the
       * \brief horizontal position in the title line, and the value is the
       * \brief string at that location.
       *
       * @param pStatusStrings (OUT) This will contain the status strings.
       */
      virtual void getStatusStrings(std::map<int, std::string>& pStatusStrings) const;

      /**
       * \brief Sets all the status strings via a map of horizontal
       * \brief locations and strings.
       *
       * @param pStatusStrings A map containing the status strings with
       *  the horizontal position as the key.
       */
      virtual void setStatusStrings(const std::map<int, std::string>& pStatusStrings);

      /**
       * \brief Removes a status string at a given horizontal position.
       *
       * @param pHPos The horizontal position of the status string to remove
       */
      virtual void removeStatusString(int pHPos);

      /**
       * \brief Removes all status strings.
       */
      virtual void removeStatusStrings();

      /**
       * \brief Adds a key that will cause the window to quit its input loop
       * \brief and return cxID_QUIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave function
       *  when the window exits.  Defaults to true.
       * @param pOverride If true, then the key will be added as an exit key
       *  regardless if the key exists as a function key.  This defaults to
       *  false.
       *
       * @return Whether or not the key was added
       */
      virtual bool addQuitKey(int pKey, bool pRunOnLeaveFunction = true,
                              bool pOverride = false);

      /**
       * \brief Removes a quit key
       *
       * @param pKey The key to remove from the list of quit keys
       */
      virtual void removeQuitKey(int pKey);

      /**
       * \brief Removes all quit keys.
       */
      virtual void removeQuitKeys();

      /**
       * \brief Adds a key that will cause the window to quit its input loop
       * \brief and return cxID_EXIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave function
       *  when the window exits.  Defaults to true.
       * @param pOverride If true, then the key will be added as an exit key
       *  regardless if the key exists as a function key.  This defaults to
       *  false.
       *
       * @return Whether or not the key was added
       */
      virtual bool addExitKey(int pKey, bool pRunOnLeaveFunction = true,
                              bool pOverride = false);

      /**
       * \brief Removes an exit key
       *
       * @param pKey The key to remove from the list of exit keys
       */
      virtual void removeExitKey(int pKey);

      /**
       * \brief Removes all exit keys.
       */
      virtual void removeExitKeys();

      /**
       * \brief Returns whether or not a key exists in the
       * \brief set of exit keys for the window.
       *
       * @param pKey The key to look for in the set of exit keys
       *
       * @return Whether or not the key exists in the set of exit
       *  keys for the window
       */
      virtual bool hasExitKey(int pKey) const;

      /**
       * \brief Returns whether or not a key exists in the
       * \brief set of quit keys for the window.
       *
       * @param pKey The key to look for in the set of quit keys
       *
       * @return Whether or not the key exists in the set of quit
       *  keys for the window
       */
      virtual bool hasQuitKey(int pKey) const;

      /**
       * \brief Clears the text from the window.
       *
       * @param pRefresh Whether or not to refresh the window
       *  after clearing it (defaults to false)
       */
      virtual void clear(bool pRefresh = false);

      /**
       * \brief This is mainly for deriving classes.  This causes the object
       * \brief to stop its input loop with a code of cxID_QUIT.
       */
      virtual void quitNow();

      /**
       * \brief This is mainly for deriving classes.  It tells the object
       * \brief to stop its input loop with a code of cxID_EXIT.
       */
      virtual void exitNow();

      /**
       * \brief Populates a collection of strings representing the keys
       * \brief that will make the window quit its input loop.
       *
       * @param pKeys A collection that will contain strings representing the
       *  keys that will make the window quit its input loop
       */
      virtual void getQuitKeyStrings(std::vector<std::string>& pKeys) const;

      /**
       * \brief Returns a string containing a comma-separated list of the
       * \brief quit keys for the window.
       *
       * @return A string containing a comma-separated list of the quit keys
       *  for the window.
       */
      virtual std::string getQuitKeyListString() const;

      /**
       * \brief Populates a collection of strings representing the keys
       * \brief that will make the window exit its input loop.
       *
       * @param pKeys A collection that will contain strings representing the
       *  keys that will make the window quit its input loop
       */
      virtual void getExitKeyStrings(std::vector<std::string>& pKeys) const;

      /**
       * \brief Returns a string containing a comma-separated list of the
       * \brief exit keys for the window.
       *
       * @return A string containing a comma-separated list of the exit keys
       *  for the window.
       */
      virtual std::string getExitKeyListString() const;

      /**
       * \brief Returns the keys that will make the window quit its input
       * \brief loop.
       *
       * @return A map containing all the keys that will make the window quit
       *  its input loop.  The boolean for each key specifies whether the
       *  onLeave function will be run for that key.
       */
      virtual std::map<int, bool> getQuitKeys() const;

      /**
       * \brief Returns the keys that will make the window exit is input
       * \brief loop.
       *
       * @return A map containing all the keys that will make the window exit
       *  its input loop.  The boolean for each key specifies whether the
       *  onLeave function will be run for that key.
       */
      virtual std::map<int, bool> getExitKeys() const;

      /**
       * \brief Sets whether or not to draw the message area (the area within
       * \brief the borders)
       *
       * @param pDrawMessage Whether or not to draw the message area
       */
      void toggleMessage(bool pDrawMessage);

      /**
       * \brief Sets whether or not to draw the title
       *
       * @param pDrawTitle Whether or not to draw the title
       */
      void toggleTitle(bool pDrawTitle);

      /**
       * \brief Sets whether or not to draw the status line
       *
       * @param pDrawStatus Whether or not to draw the status line
       */
      void toggleStatus(bool pDrawStatus);

      /**
       * \brief Returns whether the message will draw or not.
       *
       * @return true if the message will draw, or false if not
       */
      bool messageWillDraw() const;

      /**
       * \brief Returns whether the title will draw or not.
       *
       * @return true if the title will draw, or false if not
       */
      bool titleWillDraw() const;

      /**
       * \brief Returns whether the status line will draw or not.
       *
       * @return true if the status line will draw, or false if not
       */
      bool statusWillDraw() const;

      /**
       * \brief Sets whether or not to draw the "special" characters (these are
       * \brief written to the window after everything else is written).
       *
       * @param pDrawSpecialChars Whether or not to draw the "special"
       *  characters
       */
      void toggleSpecialChars(bool pDrawSpecialChars);

      /**
       * \brief Returns whether the "special" characters will be drawn or
       * \brief not. (They are written to the window after everything else
       * \brief is written.)
       *
       * @return true if the special characters will draw, or false if not
       */
      bool specialCharsWillDraw() const;

      /**
       * \brief Writes the title line
       */
      virtual void drawTitle();

      /**
       * \brief Writes the status line
       */
      virtual void drawStatus();

      /**
       * \brief Writes the message text (in the area within the borders)
       */
      virtual void drawMessage();

      /**
       * \brief Writes the set of "special" characters to the window
       */
      virtual void drawSpecialChars();

      /**
       * \brief Explicitly sets the member return code for the window
       * \brief window (may be useful in some situations).
       *
       * @param pReturnCode The return code to set
       */
      virtual void setReturnCode(const long& pReturnCode);

      /**
       * \brief Returns the message color
       * @param pColor A cxWidgets color - See cxColors.h
       */
      virtual void setMessageColor(e_cxColors pColor);

      /**
       * \brief Sets the title color
       * @param pColor A cxWidgets color - See cxColors.h
       */
      virtual void setTitleColor(e_cxColors pColor);

      /**
       * \brief Sets the color of the status line at bottom of window
       * @param pColor A cxWidgets color - See cxColors.h
       */
      virtual void setStatusColor(e_cxColors pColor);

      /**
       * \brief Sets the color of the window border
       * @param pColor A cxWidgets color - See cxColors.h
       */
      virtual void setBorderColor(e_cxColors pColor);

      /**
       * \brief Sets the color of one of the window items.
       *
       * @param pItem The item to set the color of (see the e_WidgetItems
       *  enumeration in cxWidgetItems.h).
       * @param pColor The color to set the item to (see the e_cxColors
       *  enumeration in cxColors.h)
       */
      virtual void setColor(e_WidgetItems pItem, e_cxColors pColor);

      /**
       * \brief Returns whether an attribute is set for a widget item.
       *
       * @param pItem The item type to check (see the e_WidgetItems
       *  enumeration)
       * @param pAttr The ncurses attribute to check
       *
       * @return Whether or not the attribute is set for the given widget item.
       */
      virtual bool hasAttr(e_WidgetItems pItem, attr_t pAttr);

      /**
       * \brief Returns the name of the cxWidgets class.  This is overridden
       * \brief in cxWindow-based classes.  This can be used to determine
       * \brief the type of cxWidgets object that deriving classes derive from
       * \brief in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief Returns whether or not something told the window to leave now.
       * \brief If something called exitNow() or quitNow() on the window, this
       * \brief would be true.
       *
       * @return The value of the mLeaveNow member
       */
      bool getLeaveNow() const;

      /**
       * Accessor for the name for this input
       * @return The alternate label for this input
       */
      std::string getName() const;

      /**
       * \brief Sets the name of the window.  The name is an alternative means
       * \brief of identifying the window.
       *
       * @param pName The new name for the window
       */
      virtual void setName(const std::string& pName);

      /**
       * \brief Returns whether the current mouse information stored in the
       * \brief cxWindow has coordinates within the window's boundaries.
       * \brief For developers: This is affected by a call to getmouse()
       * \brief (i.e., getmouse(&mMouse)).
       *
       * @return Whether or not the last mouse event (stored in mMouse)
       *  occurred within the window's boundaries (true/false).
       */
      bool mouseEvtWasInWindow() const;

      /**
       * \brief Returns whether the current mouse information stored in the
       * \brief cxWindow has coordinates within the window's title.
       * \brief For developers: This is affected by a call to getmouse()
       * \brief (i.e., getmouse(&mMouse)).
       *
       * @return Whether or not the last mouse event (stored in mMouse)
       *  occurred within the window's boundaries (true/false).
       */
      virtual bool mouseEvtWasInTitle() const;

      /**
       * \brief Triggers the window's mouse event behavior.  This method can
       * \brief be overridden in deriving classes to deal with mouse behavior.
       */
      virtual void doMouseBehavior();

      /**
       * \brief Triggers the window's mouse event behavior.  Also sets the
       * \brief member mouse location.  In most cases, this method shouldn't
       * \brief need to be overridden in deriving classes - The other
       * \brief doMouseBehavior() method (with no parameters) should be
       * \brief overridden instead, since this method calls that version after
       * \brief setting the mouse coordinates.
       *
       * @param pMouseY The vertical mouse location
       * @param pMouseX The horizontal mouse location
       */
      virtual void doMouseBehavior(int pMouseY, int pMouseX);

      /**
       * \brief Returns a cxFunction pointer for a key.
       * \brief If the key does not exist in mKeyFunctions, or if the
       * \brief pointer is not a cxFunction2 pointer (i.e., its cxTypeStr()
       * \brief doesn't return "cxFunction2"),, this will return nullptr.
       *
       * @param pKey The key to look for in mKeyFunctions
       *
       * @return A cxFunction2 pointer for the key, or nullptr if there is no
       *  function for the key.
       */
      std::shared_ptr<cxFunction> getKeyFunction(int pKey) const;

      /**
       * \brief Returns a cxFunction pointer for a key, casted to a cxFunction0
       * \brief pointer.  If the key does not exist in mKeyFunctions, or if the
       * \brief pointer is not a cxFunction2 pointer (i.e., its cxTypeStr()
       * \brief doesn't return "cxFunction2"),, this will return nullptr.
       *
       * @param pKey The key to look for in mKeyFunctions
       *
       * @return A cxFunction2 pointer for the key, or nullptr if there is no
       *  cxFunction2 for the key.
       */
      std::shared_ptr<cxFunction0> getKeyFunctionAsFunction0(int pKey) const;

      /**
       * \brief Returns a cxFunction pointer for a key, casted to a cxFunction2
       * \brief pointer.  If the key does not exist in mKeyFunctions, or if the
       * \brief pointer is not a cxFunction2 pointer (i.e., its cxTypeStr()
       * \brief doesn't return "cxFunction2"),, this will return nullptr.
       *
       * @param pKey The key to look for in mKeyFunctions
       *
       * @return A cxFunction2 pointer for the key, or nullptr if there is no
       *  cxFunction2 for the key.
       */
      std::shared_ptr<cxFunction2> getKeyFunctionAsFunction2(int pKey) const;

      /**
       * \brief Returns a cxFunction pointer for a key, casted to a cxFunction4
       * \brief pointer.  If the key does not exist in mKeyFunctions, or if the
       * \brief pointer is not a cxFunction4 pointer (i.e., its cxTypeStr()
       * \brief doesn't return "cxFunction4"),, this will return nullptr.
       *
       * @param pKey The key to look for in mKeyFunctions
       *
       * @return A cxFunction4 pointer for the key, or nullptr if there is no
       *  cxFunction4 for the key.
       */
      std::shared_ptr<cxFunction4> getKeyFunctionAsFunction4(int pKey) const;

      /**
       * \brief Returns whether or not the mouse event read in mMouse was
       * \brief a button press, click, double click.
       *
       * @return true if the mouse event read in mMouse was a button press,
       *  click, or double click.
       */
      bool mouseEvtWasButtonEvt() const;

      /**
       * \brief Returns whether or not the first mouse button was pressed
       * \brief and held down, according to the internal mouse data structure.
       * \brief NOTE: pressed != clicked
       *
       * @return true if the first mouse button was pressed, or false if not.
       */
      bool mouseButton1Pressed() const;

      /**
       * \brief Returns whether or not the first mouse button was released
       * \brief according to the internal mouse data structure.
       * \brief NOTE: Typically what happens after the user releases the
       * \brief button after a 'pressed' event.
       *
       * @return true if the first mouse button was pressed, or false if not.
       */
      bool mouseButton1Released() const;

      /**
       * \brief Returns whether or not the first mouse button was clicked
       * \brief according to the internal mouse data structure.
       *
       * @return true if the first mouse button was clicked, or false if not.
       */
      bool mouseButton1Clicked() const;

      /**
       * \brief Returns whether or not the first mouse button was double-clicked
       * \brief according to the internal mouse data structure.
       *
       * @return true if the first mouse button was clicked, or false if not.
       */
      bool mouseButton1DoubleClicked() const;

      /**
       * \brief Returns whether or not the second mouse button was clicked
       * \brief according to the internal mouse data structure.
       *
       * @return true if the second mouse button was clicked, or false if not.
       */
      bool mouseButton2Clicked() const;

      /**
       * \brief Returns whether or not the second mouse button was double-clicked
       * \brief according to the internal mouse data structure.
       *
       * @return true if the second mouse button was clicked, or false if not.
       */
      bool mouseButton2DoubleClicked() const;

      /**
       * \brief Returns whether or not the second mouse button was clicked
       * \brief according to the internal mouse data structure.
       *
       * @return true if the second mouse button was clicked, or false if not.
       */
      bool mouseButton3Clicked() const;

      /**
       * \brief Tells the window that mouse button 1 was clicked at a certain
       * \brief location.  The window can then react to the correct information
       * \brief in its doMouseBehavior() function.
       *
       * @param pY The vertical mouse location
       * @param pX The horizontal mouse location
       */
      virtual void mouseButton1Click(int pY, int pX);

      /**
       * \brief Tells the window that mouse button 2 was clicked at a certain
       * \brief location.  The window can then react to the correct information
       * \brief in its doMouseBehavior() function.
       *
       * @param pY The vertical mouse location
       * @param pX The horizontal mouse location
       */
      virtual void mouseButton2Click(int pY, int pX);

      /**
       * \brief Tells the window that mouse button 3 was clicked at a certain
       * \brief location.  The window can then react to the correct information
       * \brief in its doMouseBehavior() function.
       *
       * @param pY The vertical mouse location
       * @param pX The horizontal mouse location
       */
      virtual void mouseButton3Click(int pY, int pX);

#ifdef NCURSES_MOUSE_VERSION
      /**
       * \brief Returns the window's mouse event structure (stored as an
       * \brief ncurses MEVENT structure).  A cxWindow's input loop normally
       * \brief captures mouse events in this structure, so this should be up
       * \brief to date after showModal() is called.
       *
       * @return The window's mouse event structure
       */
      MEVENT getMouseEvent() const;
#endif

      /**
       * @brief Get the contents of a portion of the window. (for debugging/testing)
       *
       * @param pRow the starting row
       * @param pCol the starting column
       * @param pNumber number of characters to get
       *
       * @return the "string" value of the
       */
      std::string getString(int pRow, int pCol, int pNumber);

      /**
       * @brief Display information about a window (for debugging/testing)
       * @return information about a window
       */
      void info();

      /**
       * \brief Sets whether or not to display the top border
       * \brief (Note: Only applicable if mBorderStyle != eBS_NOBORDER.)
       * @param pToggle If true, then the top border will be displayed
       */
      virtual void toggleBorderTop(bool pToggle);

      /**
       * \brief Sets whether or not to display the bottom border
       * \brief (Note: Only applicable if mBorderStyle != eBS_NOBORDER.)
       * @param pToggle If true, then the bottom border will be displayed
       */
      virtual void toggleBorderBottom(bool pToggle);

      /**
       * \brief Sets whether or not to display the left border
       * \brief (Note: Only applicable if mBorderStyle != eBS_NOBORDER.)
       * @param pToggle If true, then the left border will be displayed
       */
      virtual void toggleBorderLeft(bool pToggle);

      /**
       * \brief Sets whether or not to display the right border
       * \brief (Note: Only applicable if mBorderStyle != eBS_NOBORDER.)
       * @param pToggle If true, then the right border will be displayed
       */
      virtual void toggleBorderRight(bool pToggle);

      /**
       * \brief Adds a character to be written somewhere in the window.  These
       * \brief characters are written after everything else.
       *
       * @param pY The vertical location in the window
       * @param pX The horizontal location in the window
       * @param pChar The character to add.  This is a chtype, which is used
       *  by ncurses.  This can be any regular character, as well as ncurses
       *  defined characters (see the man page for addch for a list of ncurses
       *  defined characters).
       * @param pAttr An ncurses attribute to apply to the character.  See
       *  the man page for attron for a list of ncurses attributes.  This
       *  defaults to A_NORMAL.
       */
      virtual void addSpecialChar(int pY, int pX, chtype pChar,
                                  attr_t pAttr = A_NORMAL);

      /**
       * \brief Removes a character from the set of special characters (by
       * \brief location).
       *
       * @param pY Vertical position of the character
       * @param pX Horizontal position of the character
       */
      virtual void removeSpecialChar(int pY, int pX);

      /**
       * \brief Clears the collection of special characters
       */
      virtual void clearSpecialChars();

      /**
       * \brief Returns whether an external function pointer exists for the
       * \brief the last mouse state captured by the window.
       *
       * @return true if one exists, or false if not
       */
      virtual bool functionExistsForLastMouseState() const;

      /**
       * \brief Returns the current mouse state as it was last captured by the
       * \brief window.  If the version of ncurses used when compiling
       * \brief cxWidgets doesn't support the mouse, this will return 0.
       *
       * @return The mouse state last captured by the window
       */
      int getMouseState() const;

      /**
       * \brief Returns a string version of the current mouse state as it was
       * \brief last captured by the window.  If the version of ncurses used
       * \brief when compiling cxWidgets doesn't support the mouse, this will
       * \brief return a blank string.
       *
       * @return A string version of the mouse state last captured by the
       *  window
       */
      std::string getMouseStateStr() const;

      /**
       * \brief Returns the color of one of the items in a window.
       *
       * @param pItem The item type (a member of the e_WidgetItems enumeration
       *  - see cxWidgetItems.h)
       *
       * @return A member of the e_cxColors enumeration representing the color
       *  of the item (see cxColors.h)
       */
      virtual e_cxColors getItemColor(e_WidgetItems pItem) const;

   protected:

      /**
       * \brief Everything in the window is written here; this is an nCurses window structure
       */
      WINDOW *mWindow = nullptr;

      /**
       * \brief Contains the text to be shown inside the window (note: the
       * \brief length of each line should be <= the inside width of the
       * \brief window).
       */
      messageLineContainer mMessageLines;

      /**
       * \brief Contains "special" characters which are added to the window
       * \brief after all other stuff is added.  The key is a pair of
       * \brief integers for the location (Y, X), and the value is the
       * \brief character to add at that location.
       */
      std::map<std::pair<int, int>, chtype> mSpecialChars;

      /**
       * \brief Message attributes
       */
      std::set<attr_t> mMessageAttrs;
      /**
       * \brief Title attributes
       */
      std::set<attr_t> mTitleAttrs;
      /**
       * \brief Status attributes
       */
      std::set<attr_t> mStatusAttrs;
      /**
       * \brief Border attributes
       */
      std::set<attr_t> mBorderAttrs;
      /**
       * \brief Hotkey attributes
       */
      std::set<attr_t> mHotkeyAttrs;
      /**
       * \brief Attributes for the "special" characters (which are added to
       * \brief the window after all other stuff is added).  The key is a pair
       * \brief of integers for the location (Y, X), and the value is the
       * \brief attribute to apply for the special character.
       */
      std::map<std::pair<int, int>, attr_t> mSpecialCharAttrs;

      // Colors for the various elements
      short mMessageColorPair = eWHITE_BLUE;
      short mTitleColorPair = eGRAY_BLUE;
      short mStatusColorPair = eBROWN_BLUE;
      short mBorderColorPair = eGRAY_BLUE;

      eHPosition mHorizTitleAlignment = eHP_LEFT;   // Horizontal title alignment
      eHPosition mHorizMessageAlignment = eHP_LEFT; // Horizontal message alignment
      eHPosition mHorizStatusAlignment = eHP_LEFT;  // Horizontal status alignment
      eVPosition mVerticalMessageAlignment = eVP_TOP; // Vertical message alignment

      bool mDrawMessage = true;      // Whether or not to draw the message
      bool mDrawTitle = true;        // Whether or not to draw the title
      bool mDrawStatus = true;       // Whether or not to draw the status
      bool mDrawSpecialChars = true; // Whether or not to draw the "special" chars

      std::shared_ptr<cxFunction> mOnFocusFunction = nullptr; // Function for when focus is gained
      std::shared_ptr<cxFunction> mOnLeaveFunction = nullptr; // Function for when focus is lost
      bool mIsModal = false;   // Whether or not the window is being shown modally
      // mLeaveNow can be set true by exitNow() or quitNow() if an external
      // function wants the window object to leave its input loop.
      bool mLeaveNow = false;
      // This map contains pointers to functions to be called for various
      // keypresses.
      std::map<int, std::shared_ptr<cxFunction> > mKeyFunctions;
      // This map contains pointers to functions to be called for various
      // mouse states.
      std::map<int, std::shared_ptr<cxFunction> > mMouseFunctions;
      // mQuitKeys and mExitKeys contain additional keys to cause the window
      // to quit the input loop & return cxID_QUIT and cxID_EXIT,
      // respectively.  The key to the maps is the keypress, and the boolean
      // tells whether or not to run the onLeave function before leaving.
      std::map<int, bool> mQuitKeys;
      std::map<int, bool> mExitKeys;
      // mHotkeyHighlighting specifies whether or not to use hotkey
      // attributes in the message.
      bool mHotkeyHighlighting;
#ifdef NCURSES_MOUSE_VERSION
      MEVENT mMouse; // Stores data for mouse events
#endif

      /**
       * \brief Creates mPanel (by calling new_panel()), then sets up
       * \brief the panel's user pointer to point to the cxWindow.  The
       * \brief panel's user pointer needs to point to the cxWindow in
       * \brief order for isAbove() and isBelow() to work properly.  So,
       * \brief in any derived classes, if you need to re-create the
       * \brief panel, this function should be used.
       */
      virtual void reCreatePanel();

      /**
       * \brief Frees the memory used by the ncurses window and
       * \brief panel used in the cxWindow object.
       */
      void freeWindow();

      /**
       * \brief Initializes the window parameters and sets up the internal
       * \brief data members.
       *
       * @param pRow The topmost row of the window
       * @param pCol The leftmost column of the window
       * @param pHeight The height of the window
       * @param pWidth The width of the window
       * @param pTitle The title to use for the window
       * @param pMessage The message to display inside the window
       * @param pStatus The status text to use
       * @param pParentWindow A pointer to the parent cxWindow (defaults to nullptr)
       * @param pResizeVertically Whether or not to resize vertically so that
       *  all the message text shows (defaults to true).
       */
      virtual void init(int pRow, int pCol, int pHeight, int pWidth,
                        const std::string& pTitle, const std::string& pMessage,
                        const std::string& pStatus, cxWindow *pParentWindow = nullptr,
                        bool pResizeVertically = true);

      /**
       * \brief Removes all subwindow pointers
       */
      virtual void removeAllSubwindows();

      /**
       * \brief Adds a subwindow to the window.  Subwindows of a
       * \brief will be displayed when a window is displayed.
       *
       * @param pSubWindow A pointer to another cxWindow to be added as a subwindow.
       */
      virtual void addSubwindow(cxWindow *pSubWindow);

      /**
       * \brief Removes a window from the subwindow list
       *
       * @param pSubWindow A pointer to the window to remove
       */
      virtual void removeSubWindow(const cxWindow *pSubWindow);

      /**
       * \brief Sets the color (and attribute, if necessary) of a color element.
       *
       * @param pColorPair (OUT) This will be set to the correct ncurses color
       *  pair to use.
       * @param pElementAttr (OUT) This will be set to the correct ncurses
       *  element to use.
       * @param pColor The color to set - See the e_cxColors enumeration (defined
       *  in cxColors.h)
       */
      virtual void setElementColor(short& pColorPair, attr_t& pElementAttr, e_cxColors pColor);

      /**
       * \brief Enables the attributes for one of the m*Attrs sets for an ncurses window.
       * \brief If the m*Attrs collection is empty, this will enable the cxBase attributes
       * \brief instead.
       *
       * @param pWin A pointer to the ncurses window for which to enable the
       *  attributes (will usually be mWindow).
       * @param pItem The item type to retrieve attributes for - see the
       *  e_cxWidgetItems enumeration (defined in cxWidgetItems.h).
       */
      virtual void enableAttrs(WINDOW *pWin, e_WidgetItems pItem);

      /**
       * \brief Disables the attributes for one of the m*Attrs sets for an ncurses window.
       * \brief  If the m*Attrs collection is empty, this will disable the cxBase attributes
       * \brief  instead.
       *
       * @param pWin A pointer to the ncurses window for which to disable the
       *  attributes (will usually be mWindow).
       * @param pItem The item type to retrieve attributes for - see the
       *  e_cxWidgetItems enumeration (defined in cxWidgetItems.h).
       */
      virtual void disableAttrs(WINDOW *pWin, e_WidgetItems pItem);

      /**
       * \brief Writes text to an ncurses WINDOW, highlighting any characters
       * \brief preceeded by ampersands (&).
       *
       * @param pWindow A pointer to the ncurses WINDOW object in which to write text
       * @param pText The text to write
       * @param pY The vertical position of the text
       * @param pStartX The starting horizontal position of the text
       * @param pEndX The ending boundary for text (one location past the last
       *              character to be written)
       * @param pNormalTextItem A value from the e_WidgetItems enumeration (see
       *  cxWidgetItems.h) representing the attributes to use for the normal text.
       *  If -1, this won't be used.  This was added for situations where an
       *  attribute for normal text is also a hotkey attribute (used for highlighted
       *  text).  In that case, the attribute would be disabled for normal text
       *  when it shouldn't.
       */
      virtual void writeWithHighlighting(WINDOW* pWindow, const std::string& pText,
                        const int& pY, const int& pStartX, const int& pEndX,
                        int pNormalTextItem = -1);

      /**
       * \brief Makes a copy of a cxWindow's member variables
       *
       * @param pThatWindow A pointer to another cxWindow to copy
       * @param pRecreateWin If true (default), this will re-create the mWindow
       *  member in the size of the other cxWindow.
       */
      virtual void copyCxWinStuff(const cxWindow* pThatWindow, bool pRecreateWin = true);

      /**
       * \brief Figures out how to align text based on a title & message
       *
       * @param pParentWindow A pointer to the parent window
       * @param pPosition The horizontal positioning
       * @param pTitle The title text
       * @param pStatus The status text
       * @param pMessage The message text
       * @param pRow The topmost row
       * @param pCol The leftmost column
       */
      virtual void getRowColBasedOn(cxWindow *pParentWindow, eHPosition pPosition,
                            const std::string& pTitle, const std::string& pStatus,
                            const std::string& pMessage, int& pRow, int& pCol);

      /**
       * \brief Looks for a function tied to the last keypress and
       * \brief runs it, if one exists.
       *
       * @param pFunctionExists A pointer to a bool: If non-null, it will
       *  store whether or not a function existed for the last keypress.
       *  Defaults to nullptr.
       * @param pRunOnLeaveFunction A pointer to a bool: If non-NUL, it will
       *  store whether or not to run the onLeave function when the window
       *  exits (this is an option for cxFunction).
       *
       * @return Whether or not the input loop should continue
       */
      virtual bool handleFunctionForLastKey(bool *pFunctionExists = nullptr,
                                            bool *pRunOnLeaveFunction = nullptr);

      /**
       * \brief Looks for a function tied to the last mouse state captured
       * \brief by the window and runs it, if one exists.
       *
       * @param pFunctionExists A pointer to a bool: If non-null,
       *  it will store whether or not a function existed for the last mouse
       *  event.  Defaults to nullptr.
       * @param pRunOnLeaveFunction A pointer to a bool: If non-NUL, it will
       *  store whether or not to run the onLeave function when the window
       *  exits (this is an option for cxFunction).
       *
       * @return Whether or not the input loop should continue
       */
      virtual bool handleFunctionForLastMouseState(bool *pFunctionExists = nullptr,
                                            bool *pRunOnLeaveFunction = nullptr);

      /**
       * \brief Fills the member ncurses window structure with the current
       * \brief window text, but does not actually show it on the screen.
       * \brief This is useful for optimization purposes.
       */
      virtual void draw();

      /**
       * \brief Returns whether or not a mouse event location is within
       * \brief a window.
       *
       * @param pMouse The mouse event object
       * @param pTop The window's top row
       * @param pLeft The window's left side
       * @param pBottom The window's bottom row
       * @param pRight The window's right side
       *
       * @return Whether or not the mouse event is within the given window
       *  boundaries (true/false).
       */
#ifdef NCURSES_MOUSE_VERSION
      static bool mouseEvtInRect(const MEVENT& pMouse, int pTop, int pLeft,
                                 int pBottom, int pRight);
#endif

      /**
       * \brief Re-sets the key functions from another cxWindow.  This is
       * \brief a helper for copying windows.
       *
       * @param pWindow Another cxWindow
       */
      void setKeyFunctions(const cxWindow& pWindow);

      /**
       * \brief Re-sets the onFocus and onLeave functions from another
       * \brief cxWindow.  This is a helper for copying windows.
       *
       * @param pWindow Another cxWindow
       */
      void setFocusFunctions(const cxWindow& pWindow);

      /**
       * \brief Returns whether or not the parent window, if there is one, is
       * \brief a cxPanel.
       *
       * @return true if the parent window is a cxPanel, or false if not.
       */
      bool parentIsCxPanel() const;

      /**
       * \brief Returns whether or not the parent window, if there is one, is
       * \brief a cxForm.
       *
       * @return true if the parent window is a cxForm, or false if not.
       */
      bool parentIsCxForm() const;

      /**
       * \brief Returns whether or not the parent window, if there is one, is
       * \brief a cxNotebook.
       *
       * @return true if the parent window is a cxNotebook, or false if not.
       */
      bool parentIsCxNotebook() const;

      /**
       * \brief Returns a const iterator to the first subwindow.
       *
       * @return A const iterator to the first subwindow.
       */
      cxWindowPtrContainer::const_iterator subWindows_begin() const;

      /**
       * \brief Returns a const iterator to the one past the last subwindow.
       *
       * @return A const iterator to one past the last subwindow.
       */
      cxWindowPtrContainer::const_iterator subWindows_end() const;

      /**
       * \brief Returns a const reverse begin iterator for the subwindow collection.
       *
       * @return A const reverse begin iterator for the subwindow collection
       */
      cxWindowPtrContainer::const_reverse_iterator subWindows_rbegin() const;

      /**
       * \brief Returns a const reverse end iterator for the subwindow collection.
       *
       * @return A const reverse end iterator for the subwindow collection
       */
      cxWindowPtrContainer::const_reverse_iterator subWindows_rend() const;

      /**
       * \brief Returns whether or not the onFocus function is set (if
       * \brief mOnFocusFunction is not nullptr and if its function pointer is
       * \brief set)
       *
       * @return true if the onFocus function is set, or false if not
       */
      bool onFocusFunctionIsSet() const;

      /**
       * \brief Returns whether or not the onLeave function is set (if
       * \brief mOnLeaveFunction is not nullptr and if its function pointer is
       * \brief set)
       *
       * @return true if the onLeave function is set, or false if not
       */
      bool onLeaveFunctionIsSet() const;

   private:
      PANEL *mPanel = nullptr;            // For layered window management
      cxWindow *mExtTitleWindow;          // Optionally used for the title instead of the title line.
      cxWindow *mExtStatusWindow;         // Optionally used for status instead
                                          //  of the status line.
      cxWindow *mTitleParent = nullptr;   // Pointer to a window using me for its title
      cxWindow *mStatusParent = nullptr;  // Pointer to a window using me for its status
      std::string mExtTitleTemp;          // Keeps track of old external title window text
      std::string mExtStatusTemp;         // Keeps track of old external status window text
      cxWindow *mParentWindow;            // Pointer to the parent window (can be nullptr for none)
      cxWindowPtrContainer mSubWindows;   // Contains subwindows for this window
      bool mFocus = false;                // Whether or not the window has focus
      eBorderStyle mBorderStyle;          // Border style
      bool mEnabled = true;               // Whether or not the window is enabled
      bool mDisableCursorOnShow = true;   // Whether or not to disable the cursor in show()
      bool mExitOnMouseOutside = false;  // Exit modal input loop on mouse click outside window
      int mLastKey = NOKEY;               // The last key typed by the user during an input loop
      bool mChangeColorsOnFocus = false;  // Whether or not to apply colors upon setting focus
      bool mShowSubwinsForward = true;    // If true, the subwindows will be shown in
                                          //  forward order; if false, they will be shown
                                          //  in reverse order.
      bool mShowSelfBeforeSubwins = true; // Whether or not to show this window before
                                          //  showing the subwindows
      long mReturnCode = cxID_EXIT;       // Keeps track of the return code set by showModal()
      bool mRunOnFocus = true;            // Whether or not to run the onFocus function
      bool mRunOnLeave = true;            // Whether or not to run the onLeave function
      // mTitleStrings and mStatusStrings keep track of horizontal positions
      // and strings to write for the title and status lines, respectively.
      std::map<int, std::string> mTitleStrings;
      std::map<int, std::string> mStatusStrings;
      // mName provides an alternate way to identify a window.
      std::string mName;
      bool mDrawBorderTop = true;    // Display the top border (only applicable if borderstyle != eBS_NOBORDER)
      bool mDrawBorderBottom = true; // Display the bottom border (only applicable if borderstyle != eBS_NOBORDER)
      bool mDrawBorderLeft = true;   // Display the left border (only applicable if borderstyle != eBS_NOBORDER)
      bool mDrawBorderRight = true;  // Display the top border (only applicable if borderstyle != eBS_NOBORDER)

      //// Helper functions

      // Writes text at a row & column
      void drawLabel(int pRow, int pCol, const std::string& pLabel);

      // Returns the largest subwindow height (or 0 if there are none).
      inline int maxSubwindowHeight() const;

      // Combines all the messages lines into one
      //  string.
      void combineMessageLines(std::string& message);

      // Returns whether or not a cxWindow exists in mSubWindows.
      // Parameters:
      //  pWindow: A cxWindow pointer to see if exists in mSubWindows
      inline bool subWindowExists(cxWindow *pWindow) const;

      // Helper method for the constructors - Checks the parent
      //  window pointer and adds this window to the parent (might
      //  have to call a different method if the parent window is
      //  a cxPanel)
      void addToParentWindow(cxWindow *pParentWindow);

      // Writes strings to a border, where the strings are mapped to
      //  horizontal positions.  This is a helper for writing multiple
      //  strings in the top/bottom border.
      // Parameters:
      //  pStrings: This maps strings to their horizontal positions
      //  pVPos: This is the vertical position in the window of where
      //         the strings will be written (this will probably be
      //         the top or bottom border position).
      //  pItem: The item type of the text, for applying the attributes.
      //  pTextColorPair: The color to apply to the text
      void writeBorderStrings(const std::map<int, std::string>& pStrings, int pVPos,
                              e_WidgetItems pItem, short pTextColorPair);

      // This is a helper for the destructor - It returns whether or not any
      //  of the subwindows has this window as its parent.
      inline bool anySubwinHasThisParent();

}; // end of class cxWindow

#endif
