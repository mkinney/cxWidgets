#ifndef __CXBUTTON_H__
#define __CXBUTTON_H__

/** \class cxButton
 *
 *    Copyright (C) 2005-2007 Michael H. Kinney
 *
 * \brief Represents a button.  An external function may be set up for the
 * \brief button to run, either by single-clicking on it with the main mouse
 * \brief button or by pressing enter when the button has focus.  The function
 * \brief to be executed by the button is called using setOnClickFunction().<br>
 * \brief <br>
 * \brief Note that the main mouse button click behavior can't be set using
 * \brief setMouseFunction() with BUTTON1_CLICKED; however, external mouse
 * \brief functions can be set for other mouse buttons if desired.
 *
 * \author Eric Oulashin
 * \author Mike Kinney
 * \version $Revision: 1.13 $
 * \date $Date: 2007/12/04 00:23:46 $
*/

#include "cxWindow.h"
#include "cxFunction.h"
#include <string>
#include <memory>

class cxButton : public cxWindow
{
   public:
      /**
       * \brief Default constructor
       *
       * @param pParentWindowPtr A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the button.
       * @param pWidth The width of the button.
       * @param pLabel The text to appear on the button.
       * @param pBorderStyle The border style - can be eBS_SINGLE_LINE for
       *  a single-line border or eBS_NOBORDER for no border.  Defaults to
       *  eBS_SINGLE_LINE.
       */
      explicit cxButton(cxWindow *pParentWindowPtr = nullptr, int pRow = 0,
                        int pCol = 0, int pHeight = 3, int pWidth = 5,
                        const std::string& pLabel = "",
                        eBorderStyle pBorderStyle = eBS_SINGLE_LINE);

      /**
       * \brief Constructor that sets up the button with a 4-parameter function.
       *
       * @param pParentWindowPtr A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the button.
       * @param pWidth The width of the button.
       * @param pLabel The text to appear on the button.
       * @param pBorderStyle The border style - can be eBS_SINGLE_LINE for
       *  a single-line border or eBS_NOBORDER for no border.  Defaults to
       *  eBS_SINGLE_LINE.
       * @param pFunction A pointer to a function to be run when the user
       *  presses the enter key on the button.  Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter for the onClick function.
       *  Defaults to nullptr.
       * @param p2 Pointer to the second parameter for the onClick function.
       *  Defaults to nullptr.
       * @param p3 Pointer to the 3rd parameter for the onClick function.
       *  Defaults to nullptr.
       * @param p4 Pointer to the 4th parameter for the onClick function.
       *  Defaults to nullptr.
       */
      cxButton(cxWindow *pParentWindowPtr, int pRow, int pCol, int pHeight,
               int pWidth, const std::string& pLabel, eBorderStyle pBorderStyle,
               funcPtr4 pFunction, void *p1 = nullptr, void *p2 = nullptr,
               void *p3 = nullptr, void *p4 = nullptr);

      /**
       * \brief Constructor that sets up the button with a 2-parameter function.
       *
       * @param pParentWindowPtr A pointer to the parent window.
       * @param pRow The row of the upper-left corner.
       * @param pCol The column of the upper-left corner.
       * @param pHeight The height of the button.
       * @param pWidth The width of the button.
       * @param pLabel The text to appear on the button.
       * @param pBorderStyle The border style - can be eBS_SINGLE_LINE for
       *  a single-line border or eBS_NOBORDER for no border.  Defaults to
       *  eBS_SINGLE_LINE.
       * @param pFunction A pointer to a function to be run when the user
       *  presses the enter key on the button.  Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter for the onClick function.
       *  Defaults to nullptr.
       * @param p2 Pointer to the second parameter for the onClick function.
       *  Defaults to nullptr.
       */
      cxButton(cxWindow *pParentWindowPtr, int pRow, int pCol, int pHeight,
               int pWidth, const std::string& pLabel, eBorderStyle pBorderStyle,
               funcPtr2 pFunction, void *p1 = nullptr, void *p2 = nullptr);

      /**
       * \brief Copy constructor
       *
       * @param pButton Another cxButton to copy
       */
      cxButton(const cxButton& pButton);

      /**
       * Destructor
       */
      virtual ~cxButton();

      /**
       * \brief Assignment operator
       *
       * @param pButton Another button to copy
       *
       * @return The current button
       */
      cxButton& operator =(const cxButton& pButton);

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief Shows the button and waits for input.  Fires the button's set
       * \brief event if the user presses the enter key.
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
                             bool pShowSubwindows = true) override;

      /**
       * \brief Sets the "on click" function pointer for the button.  When
       * \brief the user presses the enter key on the button, this function
       * \brief will run after the button loses focus.
       *
       * @param pFunction The function to run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setOnClickFunction(funcPtr4 pFunction, void *p1, void *p2,
                                      void *p3, void *p4);

      /**
       * \brief Sets the "on click" function pointer for the button.  When
       * \brief the user presses the enter key on the button, this function
       * \brief will run after the button loses focus.
       *
       * @param pFunction The function to run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       */
      virtual void setOnClickFunction(funcPtr2 pFunction, void *p1, void *p2);

      /**
       * \brief Sets the "on click" function pointer for the button.  When
       * \brief the user presses the enter key on the button, this function
       * \brief will run after the button loses focus.
       *
       * @param pFunction The function to run - Must have this signature:
       *  string func()
       */
      virtual void setOnClickFunction(funcPtr0 pFunction);

      /**
       * \brief Runs the onClick function, if it's set.
       *
       * @return The return value of the onClick function
       */
      virtual std::string runOnClickFunction();

      /**
       * \brief If the last mouse event occurred in the button, this will run
       * \brief the button's event function.  Otherwise, this will call
       * \brief quitNow() on the button so that it will quit out of an input
       * \brief loop if it's in one.
       */
      virtual void doMouseBehavior() override;

   private:
      // mOnClickFunction is a pointer to a function that should fire when
      //  the user presses the enter key (if the mouse was supported, this would
      //  also run when the user clicks the button).
      std::shared_ptr<cxFunction> mOnClickFunction;

      // Copies the onClick function from another cxButton.  This is used by
      //  the copy constructor and the = operator.
      void copyOnClickFunction(const cxButton& pButton);
};

#endif
