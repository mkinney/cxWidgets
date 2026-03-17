#ifndef __CXDIALOG_H__
#define __CXDIALOG_H__

// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxBase.h"
#include "cxWindow.h"
#include <string>

/** \class cxDialog
 * \brief Represents a dialog window on the screen (i.e., something to be shown
 * \brief  modally).
 *  Can contain a title, status, and message to appear within the window.
 *
 * \author $Author: erico $
 * \version $Revision: 1.13 $
 * \date $Date: 2007/12/04 00:23:46 $
*/
class cxDialog : public cxWindow
{
   public:
      /**
       * Default constructor--all parameters have default values available
       * @param pParentWindow Pointer to parent window
       * @param pRow The row of the upper-left corner
       * @param pCol The column of the upper-left corner
       * @param pHeight The height of the dialog
       * @param pWidth The width of the dialog
       * @param pTitle The window title (appears in the top border).
       * @param pMessage The message to appear with the dialog
       * @param pStatus The status (appears in the bottom border).
       */
      explicit cxDialog(cxWindow *pParentWindow = nullptr, int pRow = 0,
                        int pCol = 0, int pHeight = DEFAULT_HEIGHT,
                        int pWidth = DEFAULT_WIDTH,
                        const std::string& pTitle = "",
                        const std::string& pMessage = "",
                        const std::string& pStatus = "");

      /**
       * Constructor overload
       * @param pParentWindow Pointer to parent window
       * @param pRow The row of the upper-left corner
       * @param pCol The column of the upper-left corner
       * @param pTitle The title to appear at the top of the dialog
       * @param pMessage The message to appear with the dialog
       * @param pStatus The status to display at the bottom of the window
       */
      cxDialog(cxWindow *pParentWindow,
               int pRow, int pCol, const std::string& pTitle,
               const std::string& pMessage, const std::string& pStatus);
      /**
       * Constructor overload
       * @param pParentWindow Pointer to parent window
       * @param pTitle The title to appear at the top of the dialog
       * @param pMessage The message to appear with the dialog
       * @param pStatus The status to display at the bottom of the window
       */
      cxDialog(cxWindow *pParentWindow,
               const std::string& pTitle, const std::string& pMessage,
               const std::string& pStatus);
      /**
       * Constructor overload
       * @param pParentWindow Pointer to parent window
       * @param pMessage The message to appear with the dialog
       * @param pStatus The status to display at the bottom of the window
       */
      cxDialog(cxWindow *pParentWindow,
               const std::string& pMessage, const std::string& pStatus);
      /**
       * Constructor overload
       * @param pParentWindow Pointer to parent window
       * @param pMessage The message to appear with the dialog
       */
      cxDialog(cxWindow *pParentWindow, const std::string& pMessage);

      /**
       * Destructor
       */
      virtual ~cxDialog();

      /**
       * \brief Shows the dialog, waits for input, and
       * \brief returns the code corresponding to the
       * \brief user's input.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring this window to the top.
       *  Defaults to true.
       * @param pShowSubwindows Whether or not to show the dialog's subwindows.
       *  Defaults to true.
       * @return A code corresponding to user's input (cxID_OK or cxID_QUIT)
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = true) override;

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

   private:
};

#endif
