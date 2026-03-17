#ifndef __CXFRAME_H__
#define __CXFRAME_H__

/*
 * cxFrame.h -frame work
 *
 * Copyright (C) 2005-2007 Michael H. Kinney
 *
 * \author $Author: erico $
 * \version $Revision: 1.12 $
 * \date $Date: 2007/12/04 00:23:46 $
 *
 */

#include "cxWindow.h"
#include "cxMenu.h"

/**
 * \brief Similar to wxFrame and other frame classes in GUI frameworks, this
 * \brief class represents a window that contains a title, possibly a menu bar,
 * \brief a status bar, and contains another window for the view area.
 * \brief A cxFrame should contain a cxPanel if you want something that contains
 * \brief both a menu bar (which is handled in this class) and more than 1
 * \brief cxWindow-based object.
 * \brief <b>This class is not finished yet and currently shouldn't be used.</b>
 */
class cxFrame : public cxWindow
{
   // TODO: Write this class
   public:
      /**
       * \brief Default constructor.
       * @param pParentWindow A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the window.
       * @param pWidth The width of the window.
       * @param pTitle The window title (appears in the top border).
       * @param pMessage The message to appear in the window.
       * @param pStatus The status (appears in the bottom border).
       * @param pBorderStyle The type of border to use - can be
       *  eBS_SINGLE_LINE for a single-line border or eBS_NOBORDER for
       *  no border.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the window title.
       * @param pExtStatusWindow A pointer to another cxWindow in which to display the status.
       * @param pMessageUnderlines Whether or not to recognize & use underline characters when
       *  displaying the message text.
       */
      explicit cxFrame(cxWindow *pParentWindow = nullptr,
                       int pRow = 0, int pCol = 0,
                       int pHeight = DEFAULT_HEIGHT,
                       int pWidth = DEFAULT_WIDTH,
                       const std::string& pTitle = "",
                       const std::string& pMessage = "",
                       const std::string& pStatus = "",
                       eBorderStyle pBorderStyle = eBS_SINGLE_LINE,
                       cxWindow *pExtTitleWindow = nullptr,
                       cxWindow *pExtStatusWindow = nullptr,
                       bool pMessageUnderlines = false);

      virtual ~cxFrame();

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

   private:
      cxMenu mMenuBar;
      cxWindow *mViewWindow; // The window to be displayed inside the frame
};

#endif
