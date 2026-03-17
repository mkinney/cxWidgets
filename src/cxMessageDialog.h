#ifndef __CXMESSAGEDIALOG_H__
#define __CXMESSAGEDIALOG_H__

// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxDialog.h"
#include "cxButton.h"
#include "cxMessageDialogStyles.h"
#include <string>
#include <memory>

#define OKBTN_HEIGHT 3
#define OKBTN_WIDTH 4
#define CANCELBTN_HEIGHT 3
#define CANCELBTN_WIDTH 8
#define YESBTN_HEIGHT 3
#define YESBTN_WIDTH 5
#define NOBTN_HEIGHT 3
#define NOBTN_WIDTH 4

/** \class cxMessageDialog
 * \brief Represents a dialog box with "OK" and/or "Cancel"
 * \brief  buttons (or "Yes" & "No")
 * \author Eric Oulashin
 * \author Mike Kinney
 * \version $Revision: 1.23 $
 * \date $Date: 2007/12/04 00:23:46 $
*/
class cxMessageDialog : public cxDialog
{

   public:
      /**
       * Default constructor
       * @param pParentWindow Pointer to parent window; defaults to nullptr
       * @param pRow Y location of upper-left corner of dialog; defaults to 0
       * @param pCol X location of upper-left corner of dialog; defaults to 0
       * @param pHeight Height of dialog; defaults to 24
       * @param pWidth Width of dialog; defaults to 80
       * @param pTitle Message box dialog title; defaults to blank
       * @param pMessage Message text; defaults to blank
       * @param pStyle Type of dialog from eMessageDialogStyles enumeration; defaults to cxOK|cxCancel
       * @param pStatus The status to display at the bottom of the window
       */
      explicit cxMessageDialog(cxWindow *pParentWindow = nullptr, int pRow = 0,
                               int pCol = 0, int pHeight = DEFAULT_HEIGHT,
                               int pWidth = DEFAULT_WIDTH,
                               const std::string& pTitle = "",
                               const std::string& pMessage = "",
                               long pStyle = cxOK|cxCANCEL,
                               const std::string& pStatus = "");

      /**
       * Simplified constructor - Displays a message dialog centered in the screen with a message and an OK button.
       * @param pParentWindow A pointer to the parent window
       * @param pTitle The title of the window
       * @param pMessage The message to be displayed in the dialog
       * @return
       */
      cxMessageDialog(cxWindow *pParentWindow, const std::string& pTitle, const std::string& pMessage);

      /**
       * Copy constructor
       * @param pThatDialog The other cxMessageDialog object to copy
       */
      cxMessageDialog(const cxMessageDialog& pThatDialog);

      /**
       * Destructor
       */
      virtual ~cxMessageDialog();

      /**
       * \brief Shows the dialog, waits for input, and returns the code
       * \brief corresponding to the user's input.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring this window to the top.
       *  Defaults to false.
       * @param pShowSubwindows Whether or not to show sub-windows also.
       *  Defaults to true.
       *
       * @return The code corresponding to the user's input
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = true,
                             bool pShowSubwindows = true) override;

      /**
       * Hides the dialog
       * @param pHideSubwindows Whether or not to also hide the subwindows
       *  (defaults to true)
       */
      virtual void hide(bool pHideSubwindows = true) override;

      /**
       * Un-hides the window.
       * @param pUnhideSubwindows Whether or not to also unhide the subwindows
       *    (defaults to true)
       */
      virtual void unhide(bool pUnhideSubwindows = true) override;

       /**
       * \brief Changes the window's position, based on a new upper-left corner
       * @param pNewRow The new topmost row of the form
       * @param pNewCol The new leftmost column of the form
       * @param pRefresh Whether or not to refresh the window (defaults to true)i
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

      /**
       * @brief get a pointer to the the OK/Yes button
       * @return cxButton pointer
       */
      virtual std::shared_ptr<cxButton> getOKButton() const { return(mOKBtn); }

      /**
       * @brief get a pointer to the the Cancel/NO button
       * @return cxButton pointer
       */
      virtual std::shared_ptr<cxButton> getCancelButton() const { return(mCancelBtn); }

      // Overridden from cxWindow so that this function doesn't get hidden
      virtual long setFocus(bool pShowSelf = true, bool pBringToTop = true, bool pShowSubwindows = true) override;

      /**
       * @brief set the focus on the cxButton
       * @param pButton the button to set the focus to
       * @param pShow show the buttons
       */
      void setFocus(const std::shared_ptr<cxButton>& pButton, bool pShow=true);

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

   protected:
      // Handles the input loop.  Returns the return code as a result of
      //  the user's interaction.
      long doInputLoop();

   private:
      //cxButton *mOKBtn = nullptr;         // OK/Yes button
      //cxButton *mCancelBtn = nullptr;     // Cancel/No button
      std::shared_ptr<cxButton> mOKBtn;     // OK/Yes button
      std::shared_ptr<cxButton> mCancelBtn; // Cancel/No button

      // Dis-allow show() for this class
      inline long show(bool pBringToTop = true, bool pShowSubwindows = true) override;

}; // end of class cxMessageDialog

#endif
