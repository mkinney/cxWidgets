#ifndef __CXFORM_H__
#define __CXFORM_H__

/*
 * cxForm.h - This is a form that contains text inputs.
 *
 * Copyright (C) 2005-2007 Michael H. Kinney
 *
 * \author $Author: erico $
 * \version $Revision: 1.187 $
 * \date $Date: 2007/12/04 00:23:46 $
 *
 * Date     User    Description
 */

#include "cxWindow.h"
#include "cxMultiLineInput.h"
#include "cxComboBox.h"
#include "cxMenu.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>

#define DEFAULT_CXFORM_JUMPMENU_KEY KEY_F(4)  // Default hotkey for input jumping

// This is used in append() for specifying the type of input to append, either
//  a cxMultiLineInput or a cxComboBox.
enum eInputType
{
   eIT_CXMULTILINEINPUT,
   eIT_CXCOMBOBOX
};

class cxMultiForm; // Pre-declaration to avoid a circular reference

// These typedefs are for appendPair() and appendComboBoxPair().
typedef std::pair<std::shared_ptr<cxMultiLineInput>, std::shared_ptr<cxMultiLineInput> > cxMLInputPtrPair;
typedef std::pair<std::shared_ptr<cxComboBox>, std::shared_ptr<cxComboBox> > cxComboBoxPtrPair;

/** \class cxForm
 * \brief Represents a form that contains text inputs.
 * The user can move forward between the inputs on the form using the TAB and
 * down arrow, and backward using SHIFT-TAB or the up arrow.  The tab order is
 * is based on the order in which the inputs are appended to the form.  The
 * input indexes also match the order in which the inputs are appended to the
 * form, and they are zero-based.<br>
 * It is a good idea to use unique labels for the inputs or unique names,
 * since many of cxForm's methods access the fields the fields by label/name;
 * in particular, getValues() populates a map of field labels/names and values,
 * and if there are duplicate labels or names, it would not return the values
 * of all the fields on the form.<br>
 * The function getInput() can be used to get a pointer to one of the inputs,
 * but one caveat of cxForm is that some operations shouldn't be performed on
 * inputs added to a cxForm:<br>
 * <ul>
 * <li>Don't free the memory used by the input (via delete/free()) -
 *     cxForm does that for you in its destructor.
 * <li>Don't call setFocus() or showModal() on an individual input that's
 *     on a cxForm - cxForm keeps track of which input is
 *     supposed to have focus and manages that in its
 *     input loop.  If you want to change which input has
 *     focus in the input loop, use setCurrentInput().
 * </ul>
 * A cxForm also has the ability to let the user jump to a specific input while
 * the form is being shown modally.  This is done with a hotkey, which is F4 by
 * default, but the hotkey can be changed with setInputJumpKey().
 * When the user presses this key, a cxMenu is used to display a list of
 * choices containing each input label, or the input name if the label is
 * blank.  When the user chooses an input, the input will be set editable, and
 * it will get the focus.  Note that inputs that can't be set editable won't
 * appear in the menu.<br>
 * Note: Many functions provided by this class that do work on one of the
 * inputs provide access to the input by index or by label/name.  Many of them
 * are just pass-through methods that call the appropriate method in
 * cxMultiLineInput.<br>
 * By default, function keys that don't have any function pointer associated
 * with them will cause a cxForm's input loop to exit if the user is on the
 * last input, if they're allowed to exit.  This behavior can be changed with a
 * call to trapNonAssignedFKeys().
 *
 */
class cxForm : public cxWindow
{
   public:
      /**
       * Default constructor.  All parameters have default values.
       * @param pParentWindow Pointer to parent window; default nullptr
       * @param pRow Y location of form; default 0
       * @param pCol X location of form; default 0
       * @param pHeight Height of form, in lines; defaults to 24
       * @param pWidth Width of form; defaults to 80
       * @param pTitle Title of form; defaults to empty string
       * @param pBorderStyle The type of border to use - can be
       *  eBS_SINGLE_LINE for a single-line border or eBS_NOBORDER
       *  for no border.
       * @param pExtTitleWindow A pointer to another cxWindow in which to display the
       *  window title.
       * @param pExtStatusWindow A pointer to another cxWindow in which to display the status.
       * @param pAutoExit Whether or not to exit the form when the user tries to go to the
       *  next input from the last input. Defaults to false.
       * @param pStacked If true, the form will behave assuming each input is on its
       *  own line.
       */
      explicit cxForm(cxWindow *pParentWindow = nullptr, int pRow = 0,
                      int pCol = 0, int pHeight = DEFAULT_HEIGHT,
                      int pWidth = DEFAULT_WIDTH, const std::string& pTitle = "",
                      eBorderStyle pBorderStyle = eBS_SINGLE_LINE,
                      cxWindow *pExtTitleWindow = nullptr,
                      cxWindow *pExtStatusWindow = nullptr,
                      bool pAutoExit = false, bool pStacked = false);

      /**
       * Copy constructor
       * @param pThatForm Another cxForm object to be copied
       */
      cxForm(const cxForm& pThatForm);

      /**
       * Destructor
       */
      virtual ~cxForm();

      /**
       * \brief Adds an input to the form.  Applies the A_STANDOUT attribute
       * \brief to the value portion of the input.  Returns a pointer to the new input.
       *
       * @param pRow Y location of the upper-left corner of the input, relative to the form
       * @param pCol X location of the upper-left corner of the input, relative to the form
       * @param pHeight The height of the input
       * @param pWidth The width of the input
       * @param pLabel The label to use with the input
       * @param pValidator String to use to validate input; see README.txt for details;
       *  blank by default
       * @param pHelpString A string to be displayed in the statusbar of the form when the
       *  input has focus; defaults to empty string
       * @param pInputOption Defines the input option (i.e., normal, read-only, etc.);
       *  defaults to eINPUT_EDITABLE
       * @param pName A name for the input (can be used as an alternative way to
       *  identify the input)
       * @param pExtValue A pointer to a string to be linked to this input and used for its
       *  value.  Defaults to nullptr for none.
       * @param pRightlabelOffset The distance between the right edge of the
       *  input and the right label.  Defaults to 0.
       * @param pRightLabelHeight The height of the label to the right of the
       *  input (defaults to 1)
       * @param pRightLabelWidth The width of the label to the right of the
       *  input (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel Whether or not to show the right label.
       *  Defaults to false.
       * @param pInputType The type of input to append (either cxMultiLineInput
       *  or cxComboBox).  This can be either eIT_CXMULTILINEINPUT (for a
       *  cxMultiLineInput) or eIT_CXCOMBOBOX (for a cxComboBox).  Defaults to
       *  eIT_CXMULTILINEINPUT.  If eIT_CXCOMBOBOX is specified, then the
       *  return value should be casted to a cxComboBox* to make use of
       *  cxComboBox's extended functionality.
       *
       * @return A pointer to the new cxMultiLineInput object that is created.
       */
      virtual std::shared_ptr<cxMultiLineInput> append(int pRow, int pCol, int pHeight,
                                                       int pWidth, const std::string& pLabel,
                                                       const std::string& pValidator = "",
                                                       const std::string& pHelpString = "",
                                                       eInputOptions pInputOption = eINPUT_EDITABLE,
                                                       const std::string& pName = "",
                                                       std::string *pExtValue = nullptr,
                                                       int pRightLabelOffset = 0,
                                                       int pRightLabelHeight = 1,
                                                       int pRightLabelWidth = 7,
                                                       bool pShowRightLabel = false,
                                                       eInputType pInputType = eIT_CXMULTILINEINPUT);

      /**
       * \brief Adds a combo box to the form.  Applies the A_STANDOUT attribute
       * \brief to the value portion of the input.  Returns a pointer to the new input.
       *
       * @param pRow Y location of the upper-left corner of the input, relative to the form
       * @param pCol X location of the upper-left corner of the input, relative to the form
       * @param pHeight The height of the input
       * @param pWidth The width of the input
       * @param pLabel The label to use with the input
       * @param pValidator String to use to validate input; see README.txt for details;
       *  blank by default
       * @param pHelpString A string to be displayed in the statusbar of the form when the
       *  input has focus; defaults to empty string
       * @param pInputOption Defines the input option (i.e., normal, read-only, etc.);
       *  defaults to eINPUT_EDITABLE
       * @param pName A name for the input (can be used as an alternative way to
       *  identify the input)
       * @param pExtValue A pointer to a string to be linked to this input and used for its
       *  value.  Defaults to nullptr for none.
       * @param pRightlabelOffset The distance between the right edge of the
       *  input and the right label.  Defaults to 0.
       * @param pRightLabelHeight The height of the label to the right of the
       *  input (defaults to 1)
       * @param pRightLabelWidth The width of the label to the right of the
       *  input (defaults to 7 so there is room for "Invalid")
       * @param pShowRightLabel Whether or not to show the right label.
       *  Defaults to false.
       *
       * @return A pointer to the new cxMultiLineInput object that is created.
       */
      virtual std::shared_ptr<cxComboBox> appendComboBox(int pRow, int pCol, int pHeight,
                                                         int pWidth, const std::string& pLabel,
                                                         const std::string& pValidator = "",
                                                         const std::string& pHelpString = "",
                                                         eInputOptions pInputOption = eINPUT_EDITABLE,
                                                         const std::string& pName = "",
                                                         std::string *pExtValue = nullptr,
                                                         int pRightLabelOffset = 0,
                                                         int pRightLabelHeight = 1,
                                                         int pRightLabelWidth = 7,
                                                         bool pShowRightLabel = false);

      /**
       * \brief Appends a pair of inputs to a form, one to the right of the
       * \brief other.  They will share similar names, and the words "_start"
       * \brief and "_end" will be appended to the name for the first and
       * \brief second inputs, respectively.  Also, if the form doesn't have
       * \brief enough room for the second input, it will not be appended.
       *
       * @param pRow Y location of the upper-left corner of the inputs,
       *  relative to the form
       * @param pCol X location of the upper-left corner of the inputs,
       *  relative to the form
       * @param pHeight The height of each input
       * @param pWidth The width of each input
       * @param pLabel The label to use - This will only be used with the
       *  first input.
       * @param pValidator String to use to validate inputs; see README.txt for
       *  details; blank by default
       * @param pHelpString A string to be displayed in the statusbar of the
       *  form when the input has focus; defaults to empty string
       * @param pInputOption Defines the input option (editable or read-only).
       *  Defaults to eINPUT_EDITABLE.
       * @param pName A name for the inputs.  Note that "_low" and "_high" will
       *  be appended to the input names, referring to the first and second
       *  inputs, respectively.
       * @param pExtValue1 A pointer to a string to be linked to the first
       *  input and used for its value.  Defaults to nullptr for none.
       * @param pExtValue2 A pointer to a string to be linked to the second
       *  input and used for its value.  Defaults to nullptr for none.
       * @param pRightlabel1Offset The distance between the right edge of the
       *  first input and its right label.  Defaults to 0.
       * @param pRightLabel1Height The height of the first input's right label
       *  (defaults to 1)
       * @param pRightLabel1Width The width of the first input's right label
       *  (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel1 Whether or not to show the right label for the
       *  first input.  Defaults to false.
       * @param pRightLabel2Height The height of the second input's right label
       *  (defaults to 1)
       * @param pRightLabel2Width The width of the second input's right label
       *  (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel2 Whether or not to show the right label for the
       *  second input.  Defaults to false.
       * @param pRightlabel2Offset The distance between the right edge of the
       *  second input and its right label.  Defaults to 0.
       * @param pRightLabel2Height The height of the second input's right label
       *  (defaults to 1)
       * @param pRightLabel2Width The width of the second input's right label
       *  (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel2 Whether or not to show the right label for the
       *  second input.  Defaults to false.
       * @param pInputType1 The type of input to append for the first input
       *  (either cxMultiLineInput or cxComboBox).  This can be either
       *  eIT_CXMULTILINEINPUT (for a cxMultiLineInput) or eIT_CXCOMBOBOX (for
       *  a cxComboBox).  Defaults to eIT_CXMULTILINEINPUT.  If eIT_CXCOMBOBOX
       *  is specified, then the return value should be casted to a cxComboBox*
       *  to make use of cxComboBox's extended functionality.
       * @param pInputType2 The type of input to append for the second input.
       *
       * @return A pair of pointers to the inputs appended to the form
       */
      virtual cxMLInputPtrPair appendPair(int pRow, int pCol, int pHeight, int pWidth,
                              const std::string& pLabel,
                              const std::string& pValidator = "",
                              const std::string& pHelpString = "",
                              eInputOptions pInputOption = eINPUT_EDITABLE,
                              const std::string& pName = "",
                              std::string *pExtValue1 = nullptr,
                              std::string *pExtValue2 = nullptr,
                              int pRightLabel1Offset = 0,
                              int pRightLabel1Height = 1,
                              int pRightLabel1Width = 7,
                              bool pShowRightLabel1 = false,
                              int pRightLabel2Offset = 0,
                              int pRightLabel2Height = 1,
                              int pRightLabel2Width = 7,
                              bool pShowRightLabel2 = false,
                              eInputType pInput1Type = eIT_CXMULTILINEINPUT,
                              eInputType pInput2Type = eIT_CXMULTILINEINPUT);

      /**
       * \brief Appends a pair of cxComboBoxes to a form, one to the right of
       * \brief the other.  They will share similar names, and the words "_start"
       * \brief and "_end" will be appended to the name for the first and
       * \brief second inputs, respectively.  Also, if the form doesn't have
       * \brief enough room for the second input, it will not be appended.
       *
       * @param pRow Y location of the upper-left corner of the inputs,
       *  relative to the form
       * @param pCol X location of the upper-left corner of the inputs,
       *  relative to the form
       * @param pHeight The height of each input
       * @param pWidth The width of each input
       * @param pLabel The label to use - This will only be used with the
       *  first input.
       * @param pValidator String to use to validate inputs; see README.txt for
       *  details; blank by default
       * @param pHelpString A string to be displayed in the statusbar of the
       *  form when the input has focus; defaults to empty string
       * @param pInputOption Definse the input option (editable or read-only).
       *  Defaults to eINPUT_EDITABLE.
       * @param pName A name for the inputs.  Note that "low" and "high" will
       *  be appended to the input names, referring to the first and second
       *  inputs, respectively.
       * @param pExtValue1 A pointer to a string to be linked to the first
       *  input and used for its value.  Defaults to nullptr for none.
       * @param pExtValue2 A pointer to a string to be linked to the second
       *  input and used for its value.  Defaults to nullptr for none.
       * @param pRightlabel1Offset The distance between the right edge of the
       *  first input and its right label.  Defaults to 0.
       * @param pRightLabel1Height The height of the first input's right label
       *  (defaults to 1)
       * @param pRightLabel1Width The width of the first input's right label
       *  (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel1 Whether or not to show the right label for the
       *  first input.  Defaults to false.
       * @param pRightlabel2Offset The distance between the right edge of the
       *  second input and its right label.  Defaults to 0.
       * @param pRightLabel2Height The height of the second input's right label
       *  (defaults to 1)
       * @param pRightLabel2Width The width of the second input's right label
       *  (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel2 Whether or not to show the right label for the
       *  second input.  Defaults to false.
       * @param pRightlabel2Offset The distance between the right edge of the
       *  second input and its right label.  Defaults to 0.
       * @param pRightLabel2Height The height of the second input's right label
       *  (defaults to 1)
       * @param pRightLabel2Width The width of the second input's right label
       *  (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel2 Whether or not to show the right label for the
       *  second input.  Defaults to false.
       *
       * @return A pair of pointers to the cxComboBoxes appended to the form
       */
      virtual cxComboBoxPtrPair appendComboBoxPair(int pRow, int pCol, int pHeight,
                                     int pWidth,
                                     const std::string& pLabel,
                                     const std::string& pValidator = "",
                                     const std::string& pHelpString = "",
                                     eInputOptions pInputOption = eINPUT_EDITABLE,
                                     const std::string& pName = "",
                                     std::string *pExtValue1 = nullptr,
                                     std::string *pExtValue2 = nullptr,
                                     int pRightLabel1Offset = 0,
                                     int pRightLabel1Height = 1,
                                     int pRightLabel1Width = 7,
                                     bool pShowRightLabel1 = false,
                                     int pRightLabel2Offset = 0,
                                     int pRightLabel2Height = 1,
                                     int pRightLabel2Width = 7,
                                     bool pShowRightLabel2 = false);

      /**
       * \brief Appends 2 inputs to the form, one to the right of the other.
       * \brief Their right labels will be moved one to the right of the other
       * \brief with a "/" between them.  The name of the first input will be
       * \brief used for both, with "_start" and "_end" appended to the first
       * \brief and second input name, respectively.  The inputs need to be
       * \brief created dynamically with the 'new' operator.
       *
       * @param pInput1 A pointer to the first input
       * @param pInput2 A pointer to the second input
       * @param pRow The row on the form to append the inputs
       * @param pCol The column on the form to append the inputs
       * @param pName If this is non-blank, then this will be used for the
       *  input names.
       */
      virtual void appendPair(std::shared_ptr<cxMultiLineInput>& pInput1,
                              std::shared_ptr<cxMultiLineInput>& pInput2,
                              int pRow, int pCol, const std::string& pName = "");

      /**
       * \brief Appends an input to the form below the last item currently in
       * \brief the form.  Applies the A_STANDOUT attribute to the value
       * \brief portion of the input.
       *
       * @param pHeight The height (# of lines) of the item
       * @param pWidth The width (# of characters) of the item
       * @param pLabel The label to use with the input
       * @param pValidator String to use to validate input; see README.txt for details;
       *  blank by default
       * @param pHelpString A string to be displayed in the statusbar of the form when
       *  the input has focus
       * @param pInputOption Defines the input option (i.e., normal, read-only, etc.);
       *  from eInputOptions enumeration in cxInput.h
       * @param pName A name for the input (can be used as an alternative way to identify the input)
       * @param pExtValue A pointer to a string to be linked to this input and used for
       *  its value.  Defaults to nullptr for none.
       * @param pRightlabelOffset The distance between the right edge of the
       *  input and the right label.  Defaults to 0.
       * @param pRightLabelHeight The height of the label to the right of the
       *  input (defaults to 1)
       * @param pRightLabelHeight The width of the label to the right of the
       *  input (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel Whether or not to show the right label.
       *  Defaults to false.
       * @param pInputType The type of input to append (either cxMultiLineInput
       *  or cxComboBox).  This can be either eIT_CXMULTILINEINPUT (for a
       *  cxMultiLineInput) or eIT_CXCOMBOBOX (for a cxComboBox).  Defaults to
       *  eIT_CXMULTILINEINPUT.  If eIT_CXCOMBOBOX is specified, then the
       *  return value should be casted to a cxComboBox* to make use of
       *  cxComboBox's extended functionality.
       *
       * @return A pointer to the new cxMultiLineInput object that is created.
       */
      virtual std::shared_ptr<cxMultiLineInput> append(int pHeight, int pWidth, const std::string& pLabel,
                                                       const std::string& pValidator = "",
                                                       const std::string& pHelpString = "",
                                                       eInputOptions pInputOption = eINPUT_EDITABLE,
                                                       const std::string& pName = "",
                                                       std::string *pExtValue = nullptr,
                                                       int pRightLabelOffset = 0,
                                                       int pRightLabelHeight = 1,
                                                       int pRightLabelWidth = 7,
                                                       bool pShowRightLabel = false,
                                                       eInputType pInputType = eIT_CXMULTILINEINPUT);

      /**
       * \brief Appends a combo box to the form below the last
       * \brief item currently in the form.  Applies the A_STANDOUT
       * \brief attribute to the value portion of the input.  The new
       * \brief input will be as wide as the inner width of the form.
       * @param pHeight The height (# of lines) of the item
       * @param pWidth The width (# of characters) of the item
       * @param pLabel The label to use with the input
       * @param pValidator String to use to validate input; see README.txt for details;
       *  blank by default
       * @param pHelpString A string to be displayed in the statusbar of the form when
       *  the input has focus
       * @param pInputOption Defines the input option (i.e., normal, read-only, etc.);
       *  from eInputOptions enumeration in cxInput.h
       * @param pName A name for the input (can be used as an alternative way to identify the input)
       * @param pExtValue A pointer to a string to be linked to this input and used for
       *  its value.  Defaults to nullptr for none.
       * @param pRightlabelOffset The distance between the right edge of the
       *  input and the right label.  Defaults to 0.
       * @param pRightLabelHeight The height of the label to the right of the
       *  input (defaults to 1)
       * @param pRightLabelHeight The width of the label to the right of the
       *  input (defaults to 7 so that there is room for "Invalid" if validation
       *  is on and the user enters an invalid value)
       * @param pShowRightLabel Whether or not to show the right label.
       *  Defaults to false.
       *
       * @return A pointer to the new cxMultiLineInput object that is created.
       */
      virtual std::shared_ptr<cxComboBox> appendComboBox(int pHeight, int pWidth, const std::string& pLabel,
                                                         const std::string& pValidator = "",
                                                         const std::string& pHelpString = "",
                                                         eInputOptions pInputOption = eINPUT_EDITABLE,
                                                         const std::string& pName = "",
                                                         std::string *pExtValue = nullptr,
                                                         int pRightLabelOffset = 0,
                                                         int pRightLabelHeight = 1,
                                                         int pRightLabelWidth = 7,
                                                         bool pShowRightLabel = false);

      /**
       * \brief Appends a copy of an input to a form.
       *
       * @param pInput A cxMultiLineInput object to be added to the form.
       */
      virtual std::shared_ptr<cxMultiLineInput> append(const cxMultiLineInput& pInput);

      /**
       * \brief Appends a cxMultiLineInput to the form via a pointer.
       * \brief The pointer that is passed in <b>must point to a
       * \brief dynamically-created cxMultiLineInput object created with
       * \brief the 'new' operator</b>, because cxForm cleans up its
       * \brief input pointers in the destructor with the 'delete'
       * \brief operator.
       *
       * @param pInput A pointer to a cxMultiLineInput (or a deriving
       *  object)
       * @param pRow The row on the form where you want the input to
       *  be located
       * @param pCol The column on the form where you want the input to
       *  be located
       * @param pMoved If non-null, the boolean pointed to by this
       *  variable will store whether or not the input was able
       *  to be moved.
       *
       */
      virtual void append(std::shared_ptr<cxMultiLineInput>& pInput, int pRow, int pCol, bool* pMoved = nullptr);

      /**
       * Removes an input from the form (by index)
       * @param pIndex The index of the input to be deleted
       */
      virtual void remove(unsigned int pIndex);

     /**
       * Removes an input from the form (by label or name)
       * @param pLabel Label of the input to be deleted
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void remove(const std::string& pLabel, bool pIsLabel = true);

     /**
       * Removes an input from the form (by pointer)
       * @param pInput Pointer to the cxMultiLineInput object to remove
       */
      virtual void remove(const std::shared_ptr<cxMultiLineInput>& pInput);
      virtual void remove(cxMultiLineInput *pInput);

      /**
       * \brief Removes all inputs from the form
       */
      virtual void removeAll();

       /**
       * Sets a function to be executed via a keypress for one of the fields (by label).
       * This version adds a pointer to a function with this signature: string func(void*, void*, void*, void*).
       * Note that functions added with this method take precedence over functions with 2 void parameters.
       * @param pLabel The label of the input to add the function to
       * @param pFunctionKey The key for the input to use to fire the function.
       * @param pFieldFunction The function to add.
       * @param pExitAfterRun Whether or not this field should exit from the input loop once the function is done.
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setFieldKeyFunction(const std::string& pLabel, int pFunctionKey,
                            const std::shared_ptr<cxFunction>& pFieldFunction,
                            bool pIsLabel = true);

      /**
       * Sets a function to be executed via a keypress for one of the fields (by label).
       * This version adds a pointer to a function with this signature: string func(void*, void*, void*, void*).
       * Note that functions added with this method take precedence over functions with 2 void parameters.
       * @param pLabel The label of the input to add the function to
       * @param pFunctionKey The key for the input to use to fire the function.
       * @param pFieldFunction The name of the function to add. It must match this signature:
       *   string func(void*, void*, void*, void*)
       * @param p1 A pointer to the first parameter to use in the function
       * @param p2 A pointer to the second parameter to use in the function
       * @param p3 A pointer to the 3rd parameter to use in the function
       * @param p4 A pointer to the 4th parameter to use in the function
       * @param pUseVal Whether or not the function's return value should be set as the value in the field.
       * @param pExitAfterRun Whether or not this field should exit from the input loop once the function is done.
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setFieldKeyFunction(const std::string& pLabel, int pFunctionKey,
                            funcPtr4 pFieldFunction, void *p1, void *p2,
                            void *p3, void *p4, bool pUseVal,
                            bool pExitAfterRun = false,
                            bool pIsLabel = true);

       /**
       * Sets a function to be executed via a keypress for one of the fields (by index).
       * This version adds a pointer to a function with this signature: string func(void*, void*, void*, void*).
       * Note that functions added with this method take precedence over functions with 2 void parameters.
       * @param pIndex The index of the input to add the function to
       * @param pFunctionKey The key for the input to use to fire the function.
       * @param pFieldFunction The name of the function to add. It must match this signature:
       *   string func(void*, void*)
       * @param p1 A pointer to the first parameter to use in the function
       * @param p2 A pointer to the second parameter to use in the function
       * @param p3 A pointer to the 3rd parameter to use in the function
       * @param p4 A pointer to the 4th parameter to use in the function
       * @param pUseVal Whether or not the function's return value should be set as the value in the field.
       * @param pExitAfterRun Whether or not this field should exit from the input loop once the function is done.
       */
      virtual void setFieldKeyFunction(unsigned pIndex, int pFunctionKey,
                            funcPtr4 pFieldFunction, void *p1, void *p2,
                            void *p3, void *p4, bool pUseVal,
                            bool pExitAfterRun = false);

       /**
       * Sets a function to be executed via a keypress for one of the fields (by label).
       * @param pLabel The label of the input to add the function to
       * @param pFunctionKey The key for the input to use to fire the function.
       * @param pFieldFunction The name of the function to add. It must match this signature:
       *   string func(void*, void*)
       * @param p1 A pointer to the first parameter to use in the function
       * @param p2 A pointer to the second parameter to use in the function
       * @param pUseVal Whether or not the function's return value should be set as the value in the field.
       * @param pExitAfterRun Whether or not this field should exit from the input loop once the function is done.
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setFieldKeyFunction(const std::string& pLabel, int pFunctionKey,
                            funcPtr2 pFieldFunction, void *p1, void *p2,
                            bool pUseVal, bool pExitAfterRun = false,
                            bool pIsLabel = true);


       /**
       * Sets a function to be executed via a keypress for one of the fields (by index).
       * @param pIndex The index of the input to add the function to
       * @param pFunctionKey The key for the input to use to fire the function.
       * @param pFieldFunction The function to add.
       */
      virtual void setFieldKeyFunction(unsigned int pIndex, int pFunctionKey,
                               const std::shared_ptr<cxFunction>& pFieldFunction);


       /**
       * Sets a function to be executed via a keypress for one of the fields (by index).
       * @param pIndex The index of the input to add the function to
       * @param pFunctionKey The key for the input to use to fire the function.
       * @param pFieldFunction The name of the function to add. It must match this signature:
       *   string func(void*, void*)
       * @param p1 A pointer to the first parameter to use in the function
       * @param p2 A pointer to the second parameter to use in the function
       * @param pUseVal Whether or not the function's return value should be set as the value in the field.
       * @param pExitAfterRun Whether or not this field should exit from the input loop once the function is done.
       */
      virtual void setFieldKeyFunction(unsigned int pIndex, int pFunctionKey,
                               funcPtr2 pFieldFunction, void *p1, void *p2,
                               bool pUseVal, bool pExitAfterRun = false);

      /**
       * \brief Sets the form window's "on focus" function
       *
       * @param pFunction The function to be run
       */
      virtual void setOnFocusFunction(const std::shared_ptr<cxFunction>& pFunction) override;

      /**
       * Sets the "on focus" function pointer for a field (by label).
       * The function must have this signature: string func(void*, void*, void*, void*).
       * @param pLabel The label of the field
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setOnFocusFunction(const std::string& pLabel,
                           const std::shared_ptr<cxFunction>& pFunction,
                           bool pIsLabel = true);

      /**
       * Sets the "on focus" function pointer for a field (by index).
       * The function must have this signature: string func(void*, void*, void*, void*).
       * @param pIndex The index of the field
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       */
      virtual void setOnFocusFunction(unsigned pIndex, const std::shared_ptr<cxFunction>& pFunction);

      /**
       * Sets the "on focus" function pointer for a field (by label).
       * The function must have this signature: string func(void*, void*, void*, void*).
       * @param pLabel The label of the field
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseVal Whether or not the function's return value should be set as
       *  the value in the field
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setOnFocusFunction(const std::string& pLabel, funcPtr4 pFunction,
                           void *p1, void *p2, void *p3, void *p4, bool pUseVal,
                           bool pIsLabel = true);

      /**
       * Sets the "on focus" function pointer for a field (by index).
       * The function must have this signature: string func(void*, void*, void*, void*).
       * @param pIndex The index of the field
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseVal Whether or not the function's return value should be set as the value in the field
       */
      virtual void setOnFocusFunction(unsigned pIndex, funcPtr4 pFunction,
                         void *p1, void *p2, void *p3, void *p4, bool pUseVal);

      /**
       * \brief Sets the form window's "on leave" function
       *
       * @param pFunction The function to be run
       */
      virtual void setOnLeaveFunction(const std::shared_ptr<cxFunction>& pFunction) override;

      /**
       * Sets the "on leave" function pointer for a field (by label/name).
       * The function must have this signature: string func(void*, void*, void*, void*).
       * @param pLabel The label/name of the field
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setOnLeaveFunction(const std::string& pLabel, const std::shared_ptr<cxFunction>& pFunction,
                              bool pIsLabel = true);

      /**
       * Sets the "on leave" function pointer for a field (by index).
       * The function must have this signature: string func(void*, void*, void*, void*).
       * @param pIndex The index of the field
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setOnLeaveFunction(unsigned pIndex, const std::shared_ptr<cxFunction>& pFunction);

      /**
       * Sets the "on leave" function pointer for a field (by label/name).
       * The function must have this signature: string func(void*, void*, void*, void*).
       * @param pLabel The label/name of the field
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setOnLeaveFunction(const std::string& pLabel, funcPtr4 pFunction,
                              void *p1, void *p2, void *p3, void *p4,
                              bool pIsLabel = true);

      /**
       * Sets the "on leave" function pointer for a field (by index).
       * The function must have this signature: string func(void*, void*, void*, void*).
       * @param pIndex The index of the field
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setOnLeaveFunction(unsigned pIndex, funcPtr4 pFunction,
                 void *p1, void *p2, void *p3, void *p4);

      /**
       * Sets a field label (by index)
       * @param pIndex The index of the field
       * @param pLabel The label to be set in the field
       */
      virtual void setFieldLabel(unsigned pIndex, const std::string& pLabel);

      /**
       * Sets a field label (by name)
       * @param pName The name of the field
       * @param pLabel The label to be set in the field
       */
      virtual void setFieldLabel(const std::string& pName, const std::string& pLabel);

      /**
       * Sets a field name (by index)
       * @param pIndex The index of the field
       * @param pName The name to be set in the field
       */
      virtual void setFieldName(unsigned pIndex, const std::string& pName);

      /**
       * \brief Sets a field name (by label)
       * @param pLabel The label of the field
       * @param pName The name to be set in the field
       */
      virtual void setFieldName(const std::string& pLabel, const std::string& pName);

      /**
       * \brief Enables or disables the input loop for one of the
       * \brief inputs (by label/name).  When the input loop for
       * \brief an input is disabled, its onFocus and onLeave
       * \brief functions will still be run, but the input won't
       * \brief wait for input.
       *
       * @param pLabel The label/name of the input
       * @param pDoInputLoop Whether to enable or disable
       *  the input loop for the input (true/false)
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void enableInputLoop(const std::string& pLabel, bool pDoInputLoop, bool pIsLabel = true);

      /**
       * \brief Enables or disables the input loop for one of the
       * \brief inputs (by index).  When the input loop for an
       * \brief input is disabled, its onFocus and onLeave
       * \brief functions will still be run, but the input won't
       * \brief wait for input.
       *
       * @param pIndex The index of the input
       * @param pDoInputLoop Whether to enable or disable
       *  the input loop for the input (true/false)
       */
      virtual void enableInputLoop(unsigned pIndex, bool pDoInputLoop);

      /**
       * \brief Sets whether the form should automatically exit when the user
       * \brief presses enter/tab/etc. on the last field.
       * @param pAutoExit true/false; whether the form should automatically exit
       */
      virtual void setAutoExit(bool pAutoExit);

      /**
       * \brief Returns the set value of whether the form should exit upon
       * \brief exit of the last field.
       * @return whether the form should auto exit upon exit of the last field
       */
      bool getAutoExit() const;

      /**
       * \brief Sets whether or not to always start at the first editable
       * \brief input when showModal() is called.
       *
       * @param pStartAtFirstInput Whether or not to always start at the first
       *  editable input when showModal() is called.
       */
      virtual void setStartAtFirstInput(bool pStartAtFirstInput);

      /**
       * \brief Returns whether or not the form will always start at the first
       * \brief editable input when showModal() is called.
       *
       * @return Whether or not the form will always start at the first input
       *  when showModal() is called.
       */
      bool getStartAtFirstInput() const;

      /**
       * \brief Sets whether to apply the default attributes to the inputs when
       * \brief appending inputs.
       *
       * @param pApplyAttrDefaults A boolean value (true/false)
       */
      void setApplyAttrDefaults(bool pApplyAttrDefaults);

      /**
       * \brief Returns whether default attributes are applied to new inputs.
       *
       * @return The value of the default attributes setting
       */
      bool getApplyAttrDefaults() const;

      /**
       * \brief Shows the form
       * @param pBringToTop Whether or not to bring this window to the top.  Defaults to false.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults to true.
       * @return The return value of cxWindow::show()
       */
      virtual long show(bool pBringToTop = false, bool pShowSubwindows = true) override;

       /**
       * \brief Shows the form and waits for input.
       *
       * @param pShowSelf Whether or not to show the window before running the
       *  input loop.  Defaults to true.
       * @param pBringToTop Whether or not to bring this window to the top.  Defaults to false.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults to true.
       * @return A return code based on user input.  Returns cxID_QUIT if the user
       *  presses ESC; otherwise, returns cxID_EXIT.
       */
      virtual long showModal(bool pShowSelf = true, bool pBringToTop = false,
                             bool pShowSubwindows = true) override;

      /**
       * \brief Returns whether or not a call to showModal() will wait for a
       * \brief keypress from the user.  A cxForm will wait for a keypress if
       * \brief it is enabled and it has at least 1 editable input.  If it is
       * \brief enabled but does not have any editable inputs, it may or may
       * \brief not wait for a keypress, depending on whether
       * \brief setWaitForInputIfEmpty() has been called.
       *
       * @return Whether or not a call to showModal() will wait for a keypress
       *  from the user.
       */
      virtual bool modalGetsKeypress() const override;

      /**
       * Hides the form (along with all the inputs)
       * @param pHideSubwindows Whether or not to also hide the subwindows (defaults
       *    to true)
       */
      virtual void hide(bool pHideSubwindows = true) override;

      /**
       * Un-hides the form (along with all the inputs).
       * @param pUnhideSubwindows Whether or not to also unhide the subwindows
       *    (defaults to true)
       */
      virtual void unhide(bool pUnhideSubwindows = true) override;

       /**
       * Changes the window's position (based on a new upper-left corner).  Also moves the input windows.
       * @param pNewRow The new topmost row of the form
       * @param pNewCol The new leftmost column of the form
       * @param pRefresh Whether or not to refrseh the window (defaults to true)
       *
       * @return Whether or not the move succeeded or not (true/false)
       */
      virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

      /**
       * \brief Returns the number of inputs in the form.
       * @return The number of inputs in the form
       */
      size_t numInputs() const;

      /**
       * Returns the value associated with a particular input (by index).
       * @param pIndex The index of the input
       * @return The string at that index
       */
      virtual std::string getValue(int pIndex) const;

      /**
       * Returns the value associated with a particular input (by label or name).
       * @param pStr The label/name of the input
       * @param pIsLabel Whether pStr is a label or a name (true = label, false = name)
       * @return The input (string) associated with that label
       */
      virtual std::string getValue(const std::string& pStr, bool pIsLabel = true) const;

      /**
       * \brief Returns the label of one of the inputs (by index).
       *
       * @param pIndex The index of the input
       *
       * @return The label of the input, or a blank string if pIndex is out of bounds.
       */
      virtual std::string getLabel(int pIndex) const;

      /**
       * \brief Returns the name of one of the inputs (by index).
       *
       * @param pIndex The index of the input
       *
       * @return The name of the input, or a blank string if pIndex is out of bounds.
       */
      virtual std::string getName(int pIndex) const;

      /**
       * \brief Returns the name of one of the inputs (by label).
       *
       * @param pLabel The label of the input
       *
       * @return The name of the input, or a blank string if there is no input with the given label.
       */
      virtual std::string getName(const std::string& pLabel) const;

      /**
       * \brief Returns the status text of one of the inputs (by index).
       *
       * @param pIndex The index of the input
       *
       * @return The status text of the input, or a blank
       *  string if the given index is out of range.
       */
      virtual std::string getStatus(int pIndex) const;

      /**
       * \brief Returns the status text of one of the inputs (by label/name).
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel If true (default), pLabel specifies the label of the input.
       *   If false, pLabel specifies the name of the input.
       *
       * @return The status text of the input, or a blank
       *  string if there is no input with the given label/name.
       */
      virtual std::string getStatus(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * Sets the value of an input (by index).
       * @param pIndex The index of the input
       * @param pValue The value to be placed at that index.
       * @param pRefresh Whether or not to refresh the input (defaults to false)
       * @return True if successful or false if not
       */
      virtual bool setValue(int pIndex, const std::string& pValue, bool pRefresh = false);

      /**
       * Sets the value of an input (by label/name).
       * @param pLabel The label/name of the input
       * @param pValue The new string value for the input
       * @param pIsLabel If true (default), pLabel specifies the label of the input.
       *   If false, pLabel specifies the name of the input.
       * @param pRefresh Whether or not to refresh the input (defaults to false)
       * @return True if successful or false if not
       */
      virtual bool setValue(const std::string& pLabel, const std::string& pValue,
                            bool pIsLabel = true, bool pRefresh = false);

      /**
       * Refreshes an input (by calling its show() method)
       * @param pIndex The index of the input to refresh
       * @return The return value of the input's show() method (0)
       */
      virtual int refreshInput(unsigned pIndex);

      /**
       * Refreshes an input (by calling its show() method)
       * @param pStr The label of the input to refresh
       * @param pIsLabel If true, pStr specifies the input label.  If false, pStr specifies the input name.
       * @return The return value of the input's show() method (0)
       */
      virtual int refreshInput(const std::string& pStr, bool pIsLabel = true);

      /**
       * Sets the masking character for an input (by index).
       * @param pIndex The index of the input
       * @param pMaskChar The character to use for masking
       * @return True if it successful or false if not
       */
      bool setMaskChar(int pIndex, char pMaskChar);

      /**
       * \brief Sets the masking character for an input (by label).
       * @param pLabel The label of the input
       * @param pMaskChar The character to use for masking
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       *
       * @return True if successful or false if not
       */
      bool setMaskChar(const std::string& pLabel, char pMaskChar, bool pIsLabel = true);

      /**
       * Returns whether or not an input is masked (by index).
       * @param pIndex The index of the input
       * @return Whether or not the input is masked
       */
      bool getMasked(int pIndex);

      /**
       * Returns whether or not an input is masked (by label).
       * @param pLabel The label of the input
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       *
       * @return Whether or not the input is masked
       */
      bool getMasked(const std::string& pLabel, bool pIsLabel = true);

      /**
       * Enables/disables masking for an input (by index).
       * @param pIndex The index of the input
       * @param pMasking Whether or not the input should be masked (true/false)
       * @return True if successul or false if not
       */
      virtual bool toggleMasking(int pIndex, bool pMasking);

      /**
       * \brief Enables/disables masking for an input (by label).
       *
       * @param pLabel The label of the input
       * @param pMasking Whether or not the input should be masked (true/false)
       * @param pIsLabel True if pStr is the label (default), or false if pStr is the name
       *
       * @return True if successul or false if not
       */
      virtual bool toggleMasking(const std::string& pLabel, bool pMasking, bool pIsLabel = true);

      /**
       * \brief Changes which input on the form should have focus (by index).
       *
       * @param pIndex The index of the input that should have focus.  If this
       *  is out of bounds, or if the input is not editable, then nothing will
       *  be changed.
       *
       * @return True if successful or false if not.
       */
      virtual bool setCurrentInput(int pIndex);

      /**
       * \brief Changes which input on the form should have focus (by label/name).
       *
       * @param pLabel The label/name of which input on the form should get focus
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       *
       * @return True if successful or false if not.
       */
      virtual bool setCurrentInput(const std::string& pLabel, bool pIsLabel = true);

      /**
       * \brief Changes which input on the form should have focus (by pointer).
       *
       * @param pInput A pointer to the input on the form that should have
       *  focus.
       *
       * @return True if succeeded or false if not (i.e., will return false if
       *  the input is not on the form).
       */
      virtual bool setCurrentInputByPtr(const std::shared_ptr<cxMultiLineInput>& pInput);
      virtual bool setCurrentInputByPtr(const cxMultiLineInput* const pInput);

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
       * \brief Removes a function pointer for a keypress for the form so that it will no
       * \brief longer fire a function.
       *
       * @param pKey The hotkey for the function to remove
       */
      virtual void clearKeyFunction(int pKey) override;

      /**
       * \brief Adds a function to call when the user presses some key.
       *
       * @param pKey The keypress to fire the function
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running
       *  the function (defaults to false)
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
       * \brief Adds a function to call when the user presses some key.
       *
       * @param pKey The keypress to fire the function
       * @param pFunction The function to be run - Must have signature
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running the
       *  function (defaults to false)
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
       * \brief Adds a function to call when the user presses some key.
       *
       * @param pKey The keypress to fire the function
       * @param pFunction The function to be run - Must have signature
       *  string func()
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running the
       *  function (defaults to false)
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
       * \brief Sets a function to be called when a key is pressed.
       *
       * @param pIndex The index of the input on the form
       * @param pKey The key to use for the function
       * @param pFunction The function to call. This can be an instance of
       * one of the derived cxFunction classes as well.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pIndex, int pKey, const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets a function to be called when a key is pressed.
       *
       * @param pIndex The index of the input on the form
       * @param pLabel The label/name of the input
       * @param pFunction The function to call. This can be an instance of
       * one of the derived cxFunction classes as well.
       * @param pIsLabel If true (default), pLabel is the input's label.  If
       *  false, pLabel is the input's name.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(const std::string& pLabel, int pKey, const std::shared_ptr<cxFunction>& pFunction, bool pIsLabel = true);

      /**
       * \brief Adds a function to call when the user presses some key for
       * \brief one of the inputs (by index).
       *
       * @param pIndex The index of the input on the form
       * @param pKey The keypress to fire the function
       * @param pFunction The function to be run - Must have this
       *  signature: string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running
       *  the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       * @param pIsLabel If true (default), pLabel is the input's label.  If
       *  false, pLabel is the input's name.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pIndex, int pKey,
                                  funcPtr4 pFunction, void *p1, void *p2,
                                  void *p3, void *p4, bool pUseVal,
                                  bool pExitAfterRun, bool pRunOnLeaveFunction,
                                  bool pRunValidator);

      /**
       * \brief Adds a function to call when the user presses some key for
       * \brief one of the inputs (by label/name).
       *
       * @param pLabel The label/name of the input
       * @param pKey The keypress to fire the function
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running
       *  the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       * @param pIsLabel If true (default), pLabel is the input's label.  If
       *  false, pLabel is the input's name.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(const std::string& pLabel, int pKey,
                                  funcPtr4 pFunction, void *p1, void *p2,
                                  void *p3, void *p4, bool pUseVal,
                                  bool pExitAfterRun, bool pRunOnLeaveFunction,
                                  bool pRunValidator, bool pIsLabel = true);

      /**
       * \brief Adds a function to call when the user presses some key for
       * \brief one of the inputs (by index).
       *
       * @param pIndex The index of the input on the form
       * @param pKey The keypress to fire the function
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running
       *  the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       * @param pIsLabel If true (default), pLabel is the input's label.  If
       *  false, pLabel is the input's name.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pIndex, int pKey, funcPtr2 pFunction,
                                  void *p1, void *p2, bool pUseVal,
                                  bool pExitAfterRun, bool pRunOnLeaveFunction,
                                  bool pRunValidator);

      /**
       * \brief Adds a function to call when the user presses some key for
       * \brief one of the inputs (by label/name).
       *
       * @param pLabel The label/name of the input
       * @param pKey The keypress to fire the function
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running
       *  the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       * @param pIsLabel If true (default), pLabel is the input's label.  If
       *  false, pLabel is the input's name.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(const std::string& pLabel, int pKey,
                                  funcPtr2 pFunction, void *p1, void *p2,
                                  bool pUseVal, bool pExitAfterRun,
                                  bool pRunOnLeaveFunction, bool pRunValidator,
                                  bool pIsLabel = true);

      /**
       * \brief Adds a function to call when the user presses some key for
       * \brief one of the inputs (by index).
       *
       * @param pIndex The index of the input on the form
       * @param pKey The keypress to fire the function
       * @param pFunction The function to be run - Must have this signature:
       *  string func()
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running
       *  the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       * @param pIsLabel If true (default), pLabel is the input's label.  If
       *  false, pLabel is the input's name.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(int pIndex, int pKey, funcPtr0 pFunction,
                                  bool pUseVal, bool pExitAfterRun,
                                  bool pRunOnLeaveFunction, bool pRunValidator);

      /**
       * \brief Adds a function to call when the user presses some key for
       * \brief one of the inputs (by label/name).
       *
       * @param pLabel The label/name of the input
       * @param pKey The keypress to fire the function
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param pUseReturnVal Whether or not to use the function's return
       *  value (not used by cxForm, but could be useful for deriving
       *  classes - this is here so that setKeyFunction() in cxWindow will
       *  be overridden).  Defaults to false.
       * @param pExitAfterRun Whether or not to exit the form after running
       *  the function (defaults to false)
       * @param pRunOnLeaveFunction Whether or not to run the onLeave
       *  function when the window exits (if pExitAfterRun is true).
       *  Defaults to true.
       * @param pIsLabel If true (default), pLabel is the input's label.  If
       *  false, pLabel is the input's name.
       *
       * @return True if the key & function was added or false if not
       */
      virtual bool setKeyFunction(const std::string& pLabel, int pKey,
                                  funcPtr0 pFunction, bool pUseVal,
                                  bool pExitAfterRun, bool pRunOnLeaveFunction,
                                  bool pRunValidator, bool pIsLabel = true);

      /**
       * Returns whether all field values are blank.
       * @return whether or not all field values are blank
       */
      bool allFieldsBlank() const;

      /**
       * \brief Sets the color of the label of one of the inputs (by index)
       *
       * @param pIndex The index of the input whose label color to set
       * @param pColor The color to set
       */
      void setLabelColor(const int& pIndex, e_cxColors pColor);

      /**
       * \brief Sets the color of the label of one of the inputs (by label/name)
       *
       * @param pLabel The label/name of the input whose label color to set
       * @param pColor The color to set
       * @param pIsLabel If true (default), pLabel is the input's label.  If false, pLabel is the input's name.
       */
      void setLabelColor(const std::string& pLabel, e_cxColors pColor, bool pIsLabel = true);

      /**
       * \brief Sets the color of the labels on all inputs.
       *
       * @param pColor The color to set
       */
      void setAllLabelColor(e_cxColors pColor);

      /**
       * \brief Returns the label color of one of the inputs (by index).
       *
       * @param pIndex The index of the input
       *
       * @return The label color of the input at pIndex, or eDEFAULT if not found.
       */
      int getLabelColor(const int& pIndex) const;

      /**
       * \brief Returns the label color of one of the inputs (by label/name).
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel If true (default), pLabel is the input's label.  If false, pLabel is the input's name.
       *
       * @return The label color of the input at pIndex, or eDEFAULT if not found.
       */
      int getLabelColor(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Sets the color of the value section of one of the inputs (by index)
       *
       * @param pIndex The index of the input whose value color to set
       * @param pColor The color to set
       */
      void setValueColor(const int& pIndex, e_cxColors pColor);

      /**
       * \brief Sets the color of the value section of one of the inputs (by label/name)
       *
       * @param pLabel The label/name of the input whose value color to set
       * @param pColor The color to set
       * @param pIsLabel If true (default), pLabel is the input's label.  If false, pLabel is the input's name.
       */
      void setValueColor(const std::string& pLabel, e_cxColors pColor, bool pIsLabel = true);

      /**
       * \brief Sets the color of the value section of all inputs on the form.
       *
       * @param pColor The color to set
       */
      void setAllValueColor(e_cxColors pColor);

      /**
       * \brief Returns the color of the value section of one of the inputs (by index).
       *
       * @param pIndex The index of the input
       *
       * @return The color of the value section of the input at pIndex, or A_NORMAL if not found.
       */
      int getValueColor(const int& pIndex) const;

      /**
       * \brief Returns the color of the value section of one of the inputs (by label/name).
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel If true (default), pLabel is the input's label.  If false, pLabel is the input's name.
       *
       * @return The color of the value section of the input at pIndex, or A_NORMAL if not found.
       */
      int getValueColor(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Sets the label & value colors of all inputs on the form.
       *
       * @param pLabelColor The color to set for the labels
       * @param pValueColor The color to set for the values
       */
      void setAllColors(e_cxColors pLabelColor, e_cxColors pValueColor);

      /**
       * \brief Sets the color of one of the window items.  For label & value
       * \brief colors, sets those colors on all the inputs on the form; for
       * \brief any other colors, sets those on the form.
       *
       * @param pItem The item to set the color of (see the e_WidgetItems
       *  enumeration in cxWidgetItems.h).
       * @param pColor The color to set the item to (see the e_cxColors
       *  enumeration in cxColors.h)
       */
      virtual void setColor(e_WidgetItems pItem, e_cxColors pColor) override;

      /**
       * \brief Returns the top row of an input (by index), or -1 if the given
       * \brief index is out of bounds.
       *
       * @param pIndex The index of the input
       *
       * @return The top row of the input, or -1 if the given index is out of bounds.
       */
      int inputTopRow(int pIndex) const;

      /**
       * \brief Returns the top row of an input (by label/name), or -1 if there is no
       * \brief input with the given label/name.
       *
       * @param pLabel The label of the input
       * @param pIsLabel If true (default), pLabel specifies the input's label.  If false,
       *                 pLabel specifies the input's name.
       *
       * @return The top row of the input, or -1 if there is no input with the given label.
       */
      int inputTopRow(const std::string& pLabel, bool pIslabel = true) const;

      /**
       * \brief Returns the left column of an input (by index), or -1 if the given
       * \brief index is out of bounds.
       *
       * @param pIndex The index of the input
       *
       * @return The left column of the input, or -1 if the given index is out of bounds.
       */
      int inputLeftCol(int pIndex) const;

      /**
       * \brief Returns the left column of an input (by label/name), or -1 if there are
       * \brief no inputs wit the given label/name.
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel If true (default), pLabel specifies the input's label.  If false,
       *                 pLabel specifies the input's name.
       *
       * @return The left column of the input, or -1 if there is no input with the given label.
       */
      int inputLeftCol(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Returns the height of an input (by index), or -1 if the given index
       * \brief is invalid.
       *
       * @param pIndex The index of the input
       *
       * @return The height of the input, or -1 if the index is out of bounds.
       */
      int inputHeight(int pIndex) const;

      /**
       * \brief Returns the height of an input (by label/name), or -1 if there is no input
       * \brief with the given label/name.
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel If true (default), pLabel specifies the input's label.  If false,
       *                 pLabel specifies the input's name.
       *
       * @return The height of the input, or -1 if there is no input with the given label.
       */
      int inputHeight(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Returns the width of an input (by index), or -1 if the given index
       * \brief is invalid.
       *
       * @param pIndex The index of the input
       *
       * @return The width of the input, or -1 if the given index is out of bounds.
       */
      int inputWidth(int pIndex) const;

      /**
       * \brief Returns the width of an input (by label/name), or -1 if there is
       * \brief no input with the given label/name.
       *
       * @param pLabel The label of the input
       * @param pIsLabel If true (default), pLabel specifies the input's label.  If false,
       *                 pLabel specifies the input's name.
       *
       * @return The width of the input, or -1 if there is no input with the given label.
       */
      int inputWidth(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Returns the maximum length of input accepted by an input (by index),
       * \brief or 0 if the given index is invalid.
       *
       * @param pIndex The index of the input
       *
       * @return The maximum length of input accepted by the input, or 0 if the given index
       *  is out of bounds.
       */
      int maxInputLen(int pIndex) const;

      /**
       * \brief Returns the maximum length of input accepted by an input (by index),
       * \brief or 0 if the given index is invalid.
       *
       * @param pLabel The label of the input
       * @param pIsLabel If true (default), pLabel specifies the input's label.  If false,
       *                 pLabel specifies the input's name.
       *
       * @return The maximum length of input accepted by the input, or 0 if there is
       *  no input with the given label/name.
       */
      int maxInputLen(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Returns the label of an input at a given index.
       *
       * @param index The index of an input
       *
       * @return The label of the input at the given index, or a blank string if the
       *         index is out of bounds.
       */
      std::string inputLabel(int pIndex) const;

      /**
       * \brief Returns the label of an input.
       *
       * @param pName The name of the input
       *
       * @return The input's label, or a blank string if there is no input with the given
       *         name.
       */
      std::string inputLabel(const std::string& pName) const;

      /**
       * \brief Returns the name of an input at a given index.
       *
       * @param index The index of an input
       *
       * @return The name of the input at the given index, or a blank string if the
       *         index is out of bounds.
       */
      std::string inputName(int pIndex) const;

      /**
       * \brief Returns the name of an input.
       *
       * @param pLabel The label of the input
       *
       * @return The input's name, or a blank string if there is no input with the given
       *         name.
       */
      std::string inputName(const std::string& pLabel) const;

      /**
       * \brief Scrolls (moves) the inputs in the window
       *
       * @param pVScrollAmt The amount by which to scroll vertically
       * @param pHScrollAmt The amount by which to scroll horizontally
       * @param pRefresh Whether or not to refresh the form (defaults to false).
       * @param pBringToTop Whether or not to bring the window to the top when refreshing.  Defaults to false.
       */
      virtual void scrollInputs(int pVScrollAmt, int pHScrollAmt, bool pRefresh = false, bool pBringToTop = false);

      /**
       * \brief Toggles the display of the cursor for an input (by index).
       *
       * @param pIndex The index of the input for which to toggle the cursor
       * display.
       * @param pShowCursor Whether or not the input should show the cursor (true/false).
       */
      virtual void toggleCursor(int pIndex, bool pShowCursor);

      /**
       * \brief Toggles the display of the cursor for an input (by label/name).
       *
       * @param pLabel The label/name of the input for which to toggle the
       * cursor display.
       * @param pShowCursor Whether or not the input should show the cursor
       * (true/false).
       * @param pIsLabel If true, pLabel specifies the label; if false, pLabel
       * specifies the name of the input.
       */
      virtual void toggleCursor(const std::string& pLabel, bool pShowCursor, bool pIsLabel = true);

      /**
       * \brief Toggles the display of the cursor on all inputs.
       *
       * @param pShowCursor Whether or not the cursor should be shown on all inputs (true/false).
       */
      virtual void toggleCursor(bool pShowCursor);

      /**
       * \brief Returns a pointer to an input at a given index, or nullptr if the index
       * \brief is not valid.  Note that inputs are dynamically allocated when
       * \brief they're appended to a cxForm, and the memory is freed in the cxForm
       * \brief destructor.  Therefore, it is vital that if the return value of this
       * \brief method is used, the 'delete' operator could be used on it, but
       * \brief this <b>should not</b> be done; othewise, the application could crash
       * \brief when the cxForm object is destroyed.
       *
       * @param pIndex The index of the input
       *
       * @return A pointer to the input at pIndex, or nullptr if the input is not valid
       */
      std::shared_ptr<cxMultiLineInput> getInput(int pIndex) const;

      /**
       * \brief Returns a pointer to an input with a given label/name, or
       * \brief nullptr if no input is found with the given label/name.  Note
       * \brief that inputs are dynamically allocated when they're appended
       * \brief to a cxForm, and the memory is freed in the cxForm
       * \brief destructor.  Therefore, it is vital that if the return value
       * \brief of this method is used, the 'delete' operator could be used
       * \brief on it, but this <b>should not</b> be done; othewise, the
       * \brief application could crash when the cxForm object is destroyed.
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel True if pLabel specifies the label (default), or false if it specifies the name
       *
       * @return A pointer to the input with the given label/name, or nullptr if none is found
       */
      std::shared_ptr<cxMultiLineInput> getInput(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Returns a pointer to the current input.  If there are no inputs
       * \brief on the form or there is some other error, this method will
       * \brief return nullptr.
       *
       * @return A pointer to the current input on the form, or nullptr if there
       *  are no inputs or if there is an error.
       */
      std::shared_ptr<cxMultiLineInput> getCurrentInput() const;

      /**
       * \brief Returns the editability option for an input (by index).
       *
       * @param pIndex The index of the input
       *
       * @return The kind of the input - See the eInputOptions enumeration in
       *    cxInput.h.  If pIndex is out of bounds, eINPUT_READONLY will
       *    be returned.
       */
      virtual int getInputOption(int pIndex) const;

      /**
       * \brief Returns the editability option for an input (by label/name).
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel If true (default), pLabel specifies the label.
       *   If false, pLabel specifies the name of the input.
       *
       * @return The kind of the input - See the eInputOptions enumeration in
       *    cxInput.h.  Returns eINPUT_READONLY if no matching input
       *    is found.
       */
      virtual int getInputOption(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Sets the input kind for one of the inputs (by index).
       *
       * @param pIndex The index of the input
       * @param pInputOption The new input kind
       * @param pRefresh Whether or not to refresh the input on the screen,
       *  since its visual attributes will be changed.  Defaults to true.
       */
      virtual void setInputOption(int pIndex, eInputOptions pInputOption, bool pRefresh = true);

      /**
       * \brief Sets the input kind for one of the inputs (by label/name).
       *
       * @param pLabel The label/name of the input
       * @param pInputOption The new input kind
       * @param pIsLabel If true (default), pLabel specifies the label.
       *   If false, pLabel specifies the name of the input.
       * @param pRefresh Whether or not to refresh the input on the screen,
       *  since its visual attributes will be changed.  Defaults to true.
       */
      virtual void setInputOption(const std::string& pLabel, eInputOptions pInputOption,
                           bool pIsLabel = true, bool pRefresh = true);

      /**
       * \brief Sets the input kind for all inputs on the form.
       *
       * @param pInputOption The new input kind
       * @param pRefresh Whether or not to refresh the input on the screen,
       *  since its visual attributes will be changed.  Defaults to true.
       */
      virtual void setAllInputOption(eInputOptions pInputOption, bool pRefresh = true);

      /**
       * \brief Mutator for the status line text.  After this method
       * \brief is called, the status provided with this method will
       * \brief be used instead of the help messages from the inputs.
       * @param pStatus The new status message for window
       * @param pRefreshStatus Whether or not to refresh the status area (defaults to true)
       */
      virtual void setStatus(const std::string& pStatus, bool pRefreshStatus = true) override;

      /**
       * \brief Turns off the custom status text set with setStatus.
       * \brief After a call to this function, the current status
       * \brief text will be cleared, and the form will go back to
       * \brief using the help messages from the inputs as status.
       *
       * @param pRefreshStatus Whether or not to refresh the status area (defaults to true)
       *
       */
      virtual void disableCustomStatus(bool pRefreshStatus = true);

      /**
       * \brief Sets a validator function to be run before focus is lost for one
       * \brief of the inputs (by index).
       * @param pIndex The index of the input
       * @param pFunction The function to be run
       */
      virtual void setValidatorFunction(int pIndex, const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets a validator function to be run before focus is lost for one
       * \brief of the inputs (by index).
       * @param pIndex The index of the input
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setValidatorFunction(int pIndex, funcPtr4 pFunction, void *p1, void *p2,
                                        void *p3, void *p4);

      /**
       * \brief Sets a validator function to be run before focus is lost for one
       * \brief of the inputs (by label/name).
       * @param pLabel The label/name of the input
       * @param pFunction The function to be run
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setValidatorFunction(const std::string& pLabel, const std::shared_ptr<cxFunction>& pFunction,
                                        bool pIsLabel = true);

      /**
       * \brief Sets a validator function to be run before focus is lost for one
       * \brief of the inputs (by label/name).
       * @param pLabel The label/name of the input
       * @param pFunction The function to be run - Must have signature string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setValidatorFunction(const std::string& pLabel, funcPtr4 pFunction,
                                        void *p1, void *p2, void *p3, void *p4,
                                        bool pIsLabel = true);

      /**
       * \brief Toggles whether to show inputs if they're on
       * \brief a border if the form has a border.
       *
       * @param pShowInputsOnBorder Whether or not to show inputs
       *     if they're on a border
       */
      virtual void showInputsOnBorder(bool pShowInputsOnBorder);

      /**
       * \brief Sets a function to be run whenever a key is pressed in an input (by index).
       * @param pIndex The index of the input
       * @param pFunction The function to be run
       */
      virtual void setOnKeyFunction(int pIndex, const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets a function to be run whenever a key is pressed in an input (by index).
       * @param pIndex The index of the input
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setOnKeyFunction(int pIndex, funcPtr4 pFunction, void *p1,
                                   void *p2, void *p3, void *p4);

      /**
       * \brief Sets a function to be run whenever a key is pressed in an
       * \brief input (by label/name).  Note: If there are multiple inputs with
       * \brief the same label/name, all of them will be affected.
       *
       * @param pLabel The label/name of the input
       * @param pFunction The function to be run
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setOnKeyFunction(const std::string& pLabel, const std::shared_ptr<cxFunction>& pFunction,
                                    bool pIsLabel = true);

      /**
       * \brief Sets a function to be run whenever a key is pressed in an
       * \brief input (by label/name).  Note: If there are multiple inputs with
       * \brief the same label/name, all of them will be affected.
       *
       * @param pLabel The label/name of the input
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setOnKeyFunction(const std::string& pLabel, funcPtr4 pFunction, void *p1,
                                    void *p2, void *p3, void *p4, bool pIsLabel = true);

      /**
       * \brief Sets a function to be run whenever a key is pressed in an input (by index).
       * @param pIndex The index of the input
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       */
      virtual void setOnKeyFunction(int pIndex, funcPtr2 pFunction, void *p1, void *p2);

      /**
       * \brief Sets a function to be run whenever a key is pressed in an
       * \brief input (by label/name).  Note: If there are multiple inputs with
       * \brief the same label/name, all of them will be affected.
       *
       * @param pLabel The label/name of the input
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setOnKeyFunction(const std::string& pLabel, funcPtr2 pFunction, void *p1,
                                    void *p2, bool pIsLabel = true);

      /**
       * \brief Sets a function to be run whenever a key is pressed in an input (by index).
       * @param pIndex The index of the input
       * @param pFunction The function to be run - Must have this signature:
       *  string func()
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       */
      virtual void setOnKeyFunction(int pIndex, funcPtr0 pFunction);

      /**
       * \brief Sets a function to be run whenever a key is pressed in an
       * \brief input (by label/name).  Note: If there are multiple inputs with
       * \brief the same label/name, all of them will be affected.
       *
       * @param pLabel The label/name of the input
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setOnKeyFunction(const std::string& pLabel, funcPtr0 pFunction, bool pIsLabel = true);

      /**
       * \brief Sets a function to be run whenever a key is pressed, in all
       * \brief inputs on the form.
       *
       * @param pFunction The function to be run
       */
      virtual void setAllOnKeyFunction(const std::shared_ptr<cxFunction>& pFunction);

      /**
       * \brief Sets a function to be run whenever a key is pressed, in all
       * \brief inputs on the form.
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*, void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       * @param p3 Pointer to the 3rd parameter to be used for the function
       * @param p4 Pointer to the 4th parameter to be used for the function
       */
      virtual void setAllOnKeyFunction(funcPtr4 pFunction, void *p1, void *p2, void *p3, void *p4);

      /**
       * \brief Sets a function to be run whenever a key is pressed, in all
       * \brief inputs on the form.
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func(void*, void*)
       * @param p1 Pointer to the first parameter to be used for the function
       * @param p2 Pointer to the second parameter to be used for the function
       */
      virtual void setAllOnKeyFunction(funcPtr2 pFunction, void *p1, void *p2);

      /**
       * \brief Sets a function to be run whenever a key is pressed, in all
       * \brief inputs on the form.
       *
       * @param pFunction The function to be run - Must have this signature:
       *  string func()
       */
      virtual void setAllOnKeyFunction(funcPtr0 pFunction);

      /**
       * \brief Sets whether or not an onKey function should run for one of the
       * \brief inputs on the form (by index).
       *
       * @param pIndex The index of the input
       * @param pRunOnKeyFunction Whether or not the input's onKey function
       *  should run
       */
      virtual void toggleOnKeyFunction(int pIndex, bool pRunOnKeyFunction);

      /**
       * \brief Sets whether or not an onKey function should run for one of the
       * \brief inputs on the form (by label/name).  Note: If there are
       * \brief multiple inputs with the same label/name, they all will be
       * \brief affected.
       *
       * @param pLabel The label/name of the input
       * @param pRunOnKeyFunction Whether or not the input's onKey function
       *  should run
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *  If false, pLabel specifies the name of the input.
       */
      virtual void toggleOnKeyFunction(const std::string& pLabel, bool pRunOnKeyFunction, bool pIsLabel = true);

      /**
       * \brief Sets whether or not the onKey function for all inputs should
       * \brief run.
       *
       * @param pRunOnKeyFunction Whether or not the onKey function should run
       *  (for all inputs)
       */
      virtual void toggleAllOnKeyFunction(bool pRunOnKeyFunction);

      /**
       * \brief Returns whether the onKey function is enabled for one of the
       * \brief inputs (by index).
       *
       * @param pIndex The index of the input
       *
       * @return Whether or not the onKey function is enabled for that input.
       *  If pIndex is out of bounds (< 0 or more than the number of inputs),
       *  this function will return false.
       */
      virtual bool onKeyFunctionEnabled(int pIndex) const;

      /**
       * \brief Returns whether the onKey function is enabled for one of the
       * \brief inputs (by label/name).  If there are multiple inputs with the
       * \brief same label/name, the onKey status of only the first one will
       * \brief be returned.
       *
       * @param plabel The label/name of the input
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *  If false, pLabel specifies the name of the input.
       *
       * @return Whether or not the onKey function is enabled for that input.
       *  If there is no input with the given label/name, this function will
       *  return false.
       */
      virtual bool onKeyFunctionEnabled(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Assignment operator
       *
       * @param pThatForm Another cxForm to be copied
       *
       * @return
       */
      cxForm& operator =(const cxForm& pThatForm);

      /**
       * \brief Adds a hotkey for the form that will cause the
       * \brief focus to jump to another input during showModal().
       * \verbatim For example:
         cxForm iForm(nullptr, 1, 0, 12, 65, "Test form");
         iForm.append(1, 1, 1, 25, "Input 1:");
         iForm.append(1, 27, 1, 25, "Input 2:");
         iForm.append(2, 1, 1, 25, "Input 3:");
         iForm.append(2, 27, 1, 25, "Input 4:");
         iForm.append(3, 1, 1, 25, "Input 5:");
         iForm.append(3, 27, 1, 25, "Input 6:");
         iForm.setAutoExit(true);
         iForm.addInputJumpKey('n', 5);
         iForm.showModal();
This will cause the form to jump to the 5th input ("Input 6")
when the user presses the 'n' key.
       \endverbatim
       *
       * @param pKey The key to use as the hotkey
       * @param pIndex The index of the input to which to jump
       *
       * @return True if successful, or false if not (i.e., if
       *    that hotkey is already set up for a form function, etc.)
       */
      virtual bool addInputJumpKey(int pKey, int pIndex);

      /**
       * \brief Adds a hotkey for the form that will cause the
       * \brief focus to jump to another input during showModal().
       * \verbatim For example:
         cxForm iForm(nullptr, 1, 0, 12, 65, "Test form");
         iForm.append(1, 1, 1, 25, "Input 1:");
         iForm.append(1, 27, 1, 25, "Input 2:");
         iForm.append(2, 1, 1, 25, "Input 3:");
         iForm.append(2, 27, 1, 25, "Input 4:");
         iForm.append(3, 1, 1, 25, "Input 5:");
         iForm.append(3, 27, 1, 25, "Input 6:");
         iForm.setAutoExit(true);
         iForm.addInputJumpKey('n', "Input 6:");
         iForm.showModal();
         This will cause the form to jump to the input with the label "Input 6:"
         when the user presses the 'n' key.
       \endverbatim
       *
       * @param pKey The key to use as the hotkey
       * @param pLabel The label/name of the input to which to jump
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       *
       * @return True if successful, or false if not (i.e., if
       *    that hotkey is already set up for a form function, etc.)
       */
      virtual bool addInputJumpKey(int pKey, const std::string& pLabel, bool pIsLabel = true);

      /**
       * \brief Returns the index of the input that currently has focus.
       *
       * @return The index of the input that currently has focus
       */
      virtual int getCurrentInputIndex() const;

      /**
       * \brief Returns the label of the input that currently has focus.
       *
       * @return The label of the input taht currently has focus
       */
      virtual std::string getCurrentInputLabel() const;

      /**
       * \brief Returns the name of the input that currently has focus.
       *
       * @return The name of the input taht currently has focus
       */
      virtual std::string getCurrentInputName() const;

      /**
       * \brief Returns the index of an input, based on its label or name.
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       *
       * @return The index of the input, or -1 if there was no input with the
       *         given label/name.
       */
      virtual int getInputIndex(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Returns the index of an input based on a pointer.  May
       * \brief return -1 if the input is not in the form.
       *
       * @param pInput A pointer to an input
       *
       * @return The index of the input on the form, or -1 if the input is
       *  not in the form.
       */
      virtual int getInputIndex(const std::shared_ptr<cxMultiLineInput>& pInput) const;
      virtual int getInputIndex(const cxMultiLineInput *pInput) const;

      /**
       * \brief Sets the hotkey to be used to bring up the menu to choose
       * \brief which input to jump to.
       *
       * @param pInputJumpKey The hotkey to use
       */
      virtual void setInputJumpKey(int pInputJumpKey);

      /**
       * \brief Returns which key is being used as the jump hotkey
       *
       * @return The key currently being used as the jump hotkey
       */
      virtual int getInputJumpKey() const;

      /**
       * \brief Sets whether an input can be set editable (by index).  If
       * \brief this is called with false, then the input will be set
       * \brief read-only, and any calls to setInputOption() with eINPUT_EDITABLE, which
       * \brief would normally set it editable, will not set it editable.
       *
       * @param pIndex The index of the input
       * @param pCanBeEditable If true, the can be set editable.  If false, the
       *  input will always be read-only.
       */
      virtual void setCanBeEditable(int pIndex, bool pCanBeEditable);

      /**
       * \brief Sets whether an input can be set editable (by label/name).  For
       * \brief example, if this is called with false, then the input will be
       * \brief set read-only, and any calls to setInputOption() with eINPUT_EDITABLE,
       * \brief which would normally set it editable, will not set it editable.
       *
       * @param pLabel The label/name of the input
       * @param pCanBeEditable If true, the can be set editable.  If false, the
       *  input will always be read-only.
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       */
      virtual void setCanBeEditable(const std::string& pLabel, bool pCanBeEditable, bool pIsLabel = true);

      /**
       * \brief Sets the ability of all inputs to be able to be editable.
       *
       * @param pCanBeEditable If true, all inputs can be set editable. If
       *  false, all inputs will always be read-only.
       */
      virtual void setAllCanBeEditable(bool pCanBeEditable);

      /**
       * \brief Returns whether or not an input can be set editable.
       *
       * @param pIndex The index of the input
       *
       * @return Whether or not the input can be set editable
       */
      virtual bool canBeEditable(int pIndex) const;

      /**
       * \brief Returns whether or not an input can be set editable.
       *
       * @param pLabel The label/name of the input
       * @param pCanBeEditable If true, the can be set editable.  If false, the
       *  input will always be read-only.
       *
       * @return Whether or not the input can be set editable
       */
      virtual bool canBeEditable(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Returns whether any input on the form (at least 1) can be set editable.
       *
       * @return Whether or not at least 1 input on the form can be set editable.
       */
      virtual bool anyInputsCanBeEditable() const;

      /**
       * \brief Sets whether the form should allow the user to quit.
       *
       * @param pAllowQuit If true, the user will be able to quit out of the form; if
       *  false, the user won't be able to quit.
       */
      virtual void setAllowQuit(bool pAllowQuit);

      /**
       * \brief Returns whether the user is able to quit out of the form.
       *
       * @return Whether or not the use is able to quit out of the form
       */
      virtual bool getAllowQuit() const;

      /**
       * \brief Sets whether the form should allow the user to exit.
       *
       * @param pAllowExit If true, the user will be able to exit out of the form; if
       *  false, the user won't be able to exit.
       */
      virtual void setAllowExit(bool pAllowExit);

      /**
       * \brief Returns whether the user is able to exit out of the form.
       *
       * @return Whether or not the use is able to exit out of the form
       */
      virtual bool getAllowExit() const;

      /**
       * \brief Adds a key that will cause the form to quit and return
       * \brief cxID_QUIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave functions
       *  when the form exits.  Defaults to true.  Note that this affects both
       *  the onLeave function for the form, as well as the current input on
       *  the form when it is modal.
       * @param pOverride If true, then the key will be added as an exit key
       *  regardless if the key exists as a function key.  This defaults to
       *  false.
       *
       * @return Whether or not the key was added
       */
      virtual bool addQuitKey(int pKey, bool pRunOnLeaveFunction = true,
                              bool pOverride = false);

      /**
       * \brief Removes a quit key (but doesn't work for ESC)
       *
       * @param pKey The key to remove from the list of quit keys
       */
      virtual void removeQuitKey(int pKey);

      /**
       * \brief Adds a key that will cause the form to quit and return
       * \brief cxID_EXIT.  Normally, if the key already exists
       * \brief as a function key, it will not be added as an exit key.
       *
       * @param pKey The key to add
       * @param pRunOnLeaveFunction Whether or not to run the onLeave functions
       *  when the form exits.  Defaults to true.  Note that this affects both
       *  the onLeave function for the form, as well as the current input on
       *  the form when it is modal.
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
       * \brief Sets a function to be run when focus is gained (at
       * \brief the start of setFocus() or showModal().  The return
       * \brief value of the function is not used.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc(void*, void*, void*, void).
       * @param p1 A pointer to the first argument
       * @param p2 A pointer to the 2nd argument
       * @param p3 A pointer to the 3rd argument
       * @param p4 A pointer to the 4th argument
       * @param pUseVal Whether or not the function's return value should be set as
       *  the value in the field (defaults to false)
       * @param pExitAfterRun Whether or not to exit after
       *  the function runs (defaults to false; if true, the input
       *  loop won't be started).
       */
      virtual void setOnFocusFunction(funcPtr4 pFuncPtr, void *p1, void *p2,
                           void *p3, void *p4, bool pUseVal = false,
                           bool pExitAfterRun = false);

      /**
       * \brief Sets a function to be run when focus is gained (at
       * \brief the start of setFocus() or showModal().  The return
       * \brief value of the function is not used.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc(void*, void).
       * @param p1 A pointer to the first argument
       * @param p2 A pointer to the 2nd argument
       * @param pUseVal Whether or not the function's return value should be set as
       *  the value in the field (defaults to false)
       * @param pExitAfterRun Whether or not to exit after
       *  the function runs (defaults to false; if true, the input
       *  loop won't be started).
       */
      virtual void setOnFocusFunction(funcPtr2 pFuncPtr, void *p1, void *p2,
                           bool pUseVal = false, bool pExitAfterRun = false);

      /**
       * \brief Sets a function to be run when focus is gained (at
       * \brief the start of setFocus() or showModal().  The return
       * \brief value of the function is not used.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc().
       * @param pUseVal Whether or not the function's return value should be set as
       *  the value in the field (defaults to false)
       * @param pExitAfterRun Whether or not to exit after
       *  the function runs (defaults to false; if true, the input
       *  loop won't be started).
       */
      virtual void setOnFocusFunction(funcPtr0 pFuncPtr, bool pUseVal = false,
                                      bool pExitAfterRun = false);

      /**
       * \brief Sets a function to be run at the start of each
       * \brief input loop.  The return value of the function is
       * \brief not used.
       *
       * @param pFuncPtr A pointer to the function
       */
      virtual void setLoopStartFunction(const std::shared_ptr<cxFunction>& pFuncPtr);

      /**
       * \brief Sets a function to be run at the end of each
       * \brief cycle through the input loop.  The return value
       * \brief of the function is not used.
       *
       * @param pFuncPtr A pointer to the function
       */
      virtual void setLoopEndFunction(const std::shared_ptr<cxFunction>& pFuncPtr);

      /**
       * \brief Sets a function to be run when focus is lost.
       * \brief The return value of the function is not used.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc(void*, void*, void*, void).
       * @param p1 A pointer to the first argument
       * @param p2 A pointer to the 2nd argument
       * @param p3 A pointer to the 3rd argument
       * @param p4 A pointer to the 4th argument
       */
      virtual void setOnLeaveFunction(funcPtr4 pFuncPtr, void *p1, void *p2,
                           void *p3, void *p4);

      /**
       * \brief Sets a function to be run when focus is lost.
       * \brief The return value of the function is not used.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc(void*, void).
       * @param p1 A pointer to the first argument
       * @param p2 A pointer to the 2nd argument
       */
      virtual void setOnLeaveFunction(funcPtr2 pFuncPtr, void *p1, void *p2);

      /**
       * \brief Sets a function to be run when focus is lost.
       * \brief The return value of the function is not used.
       *
       * @param pFuncPtr A pointer to the function - Must have the
       *  signature string someFunc().
       * @param p1 A pointer to the first argument
       * @param p2 A pointer to the 2nd argument
       */
      virtual void setOnLeaveFunction(funcPtr0 pFuncPtr);

      /**
       * \brief Shows (refreshes) all inputs
       *
       * @param pBringToTop Whether or not to bring the inputs to the top (defaults
       *  to true)
       * @param pSkipCurrentInput Whether or not to skip the current input (defaults
       *  to false)
       */
      virtual void showAllInputs(bool pBringToTop = true, bool pSkipCurrentInput = false) const;

      /**
       * \brief Returns whether a key is set as a form function.
       *
       * @param pKey The key to look for
       *
       * @return true/false, whether or not pKey is associated
       *  with a function on the form (not on the inputs).
       */
      virtual bool formKeyIsSet(int pKey) const;

      /**
       * \brief Returns whether or not the form is in 'stacked'
       * \brief mode (which assumes each input is on top of the next).
       *
       * @return Whether or not the form is in 'stacked' mode
       */
      virtual bool isStacked() const;

      /**
       * \brief Returns whether the form is set to exit its showModal() when
       * \brief the user leaves the first input going backwards.
       *
       * @return Whether or not the form is set to exit its showModal() when
       *  the user leaves the first input going backwards
       */
      virtual bool getExitOnLeaveFirst() const;

      /**
       * \brief Sets whether the form should exit when the user leaves
       * \brief the first input going backwards.
       *
       * @param pExitOnLeaveFirst If true, the form will exit its showModal()
       *  when the user leaves the first input going backwards (normally, the
       *  form doesn't exit).
       */
      virtual void setExitOnLeaveFirst(bool pExitOnLeaveFirst);

      /**
       * \brief Returns whether the form has any inputs that are
       * \brief currently editable.
       *
       * @return Whether or not the form has any inputs that are currently
       *  editable
       */
      virtual bool hasEditableInputs() const;

      /**
       * \brief Returns the index of the first editable input in
       * \brief the form's collection of inputs, or -1 if there
       * \brief are no editable inputs.
       *
       * @return The index of the first editable input on the form
       */
      virtual int firstEditableInput() const;

      /**
       * \brief Returns the index of the last editable input in
       * \brief the form's collection of inputs, or -1 if there
       * \brief are no editable inputs.
       *
       * @return The index of the last editable input on the form
       */
      virtual int lastEditableInput() const;

      /**
       * \brief Brings the window to the top
       * @param pRefresh Whether to refresh the screen (defaults to true)
       */
      virtual void bringToTop(bool pRefresh = true);

      /**
       * \brief Returns whether or not any data on the form has changed.
       *
       * @return true if any data on the form has been changed, otherwise
       *  returns false.
       */
      virtual bool hasChanged() const;

      /**
       * \brief Setter for whether or not data should be considered changed.
       *
       * @param pDataChanged Whether or not data should be considered changed
       *  (true/false)
       */
      virtual void setChanged(bool pDataChanged);

      /**
       * \brief Sets whether non-assigned function keys should
       * \brief be trapped within the input loop and not cause
       * \brief the input to stop its input loop.
       *
       * @param pTrapNonAssignedFKeys If true, pressing function
       *  keys that are not assigned (when modal) will do nothing.
       *  If false, pressing non-assigned function keys while
       *  focus is set will cause the input to exit the input loop.
       */
      virtual void trapNonAssignedFKeys(bool pTrapNonAssignedFKeys);

      /**
       * \brief Returns the parent cxMultiForm pointer
       *
       * @return The parent cxMultiForm pointer
       */
      cxMultiForm* getParentMultiForm() const;

      /**
       * \brief Sets the parent cxMultiForm that this cxForm is associated with
       *
       * @param pParentMultiForm A pointer to the form's parent cxMultiForm
       *  (or nullptr if there is no parent cxMultiForm)
       */
      virtual void setParentMultiForm(cxMultiForm *pParentMultiForm);

      /**
       * \brief Runs the loop start function, if it's set.
       *
       * @return The value of the loop start function's mExitAfterRun, or
       *  false if the loop start function isn't set.
       */
      virtual bool runLoopStartFunction();

      /**
       * \brief Runs the loop end function, if it's set.
       *
       * @return The value of the loop end function's mExitAfterRun, or
       *  false if the loop start end isn't set.
       */
      virtual bool runLoopEndFunction();

      /**
       * \brief Returns the maximum length of the value that may be set for an
       * \brief input (by index).
       *
       * @param pIndex The index of the input
       *
       * @return The maximum length of the value that may be set.
       */
      virtual int maxValueLen(int pIndex) const;

      /**
       * \brief Returns the maximum length of the value that may be set for an
       * \brief input (by label/name).
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       *
       * @return The maximum length of the value that may be set.
       */
      virtual int maxValueLen(const std::string& pLabel, bool pIsLabel = true);

      /**
       * \brief Returns whether one of the inputs on
       * \brief the form is editable (by index)
       *
       * @param pIndex The index of the input
       *
       * @return Whether or not the input is editable
       */
      virtual bool isEditable(int pIndex) const;

      /**
       * \brief Returns whether one of the inputs on
       * \brief the form is editable (by label/name)
       *
       * @param pLabel The label/name of the input
       * @param pIsLabel If true (default), pLabel specifies the input label.
       *     If false, pLabel specifies the name of the input.
       *
       * @return Whether or not the input is editable
       */
      virtual bool isEditable(const std::string& pLabel, bool pIsLabel = true) const;

      /**
       * \brief Sets whether the window should disable the cursor
       * \brief when it's shown (this is the default behavior).
       *
       * @param pDisableCursorOnShow Whether or not to disable
       *  the cursor when shown
       */
      virtual void setDisableCursorOnShow(bool pDisableCursorOnShow);

      /**
       * \brief Toggles whether or not the form should wait for
       * \brief user input if there are no editable inputs.
       *
       * @param pWaitForInputIfEmpty Whether or not the form should wait for
       *  user input if there are no editable inputs.
       */
      virtual void setWaitForInputIfEmpty(bool pWaitForInputIfEmpty);

      /**
       * \brief Returns whether or not the form will wait for
       * \brief user input if there are no editable inputs.
       *
       * @return Whether or not the form will wait for user input
       *  if there are no editable inputs.
       */
      virtual bool getWaitForInputIfEmpty() const;

      /**
       * \brief Clears the values of all the inputs on the form.
       *
       * @param pRefresh Whether or not to refresh the inputs
       *  after clearing them (defaults to false)
       * @param pOnlyEditable If true, then only the editable inputs on the
       *  form will be cleared.  This defaults to false.
       */
      virtual void clearInputs(bool pRefresh = false,
                               bool pOnlyEditable = false);

      /**
       * \brief Gets the values from the form in the form of a map of input
       * \brief labels/names and their values.
       *
       * @param pValues This map will be populated with the labels/names of
       *  the inputs mapped to their values.
       * @param pUseLabels If true (default), the values will be mapped by
       *  the input labels.  If false, the values will be mapped by the input
       *  names.
       * @param pSkipBlankIDs If true, then inputs with a blank label/name
       *  will not be added to the map.  This defaults to false.
       */
      virtual void getValues(std::map<std::string, std::string>& pValues,
                             bool pUseLabels = true,
                             bool pSkipBlankIDs = false) const;

      /**
       * \brief Sets the values in the form with a map of input labels/names
       * \brief and values.
       *
       * @param pValues A map of input labels/names and values to be set in
       *  the form.
       * @param pUseLabels If true (default), the map keys will be used as
       *  input labels; if false, the map keys will be used as input names.
       * @param pRefresh Whether or not to refresh the inputs after setting
       *  their values.  Defaults to false.
       * @param pDisableValidatorFunctions Whether or not to temporarily
       *  disable each input's validator function before setting its value.
       *  This can be useful if you want to set the values to some defaults,
       *  but the default values would normally be considered invalid.
       */
      virtual void setValues(const std::map<std::string, std::string>& pValues,
                             bool pUseLabels = true, bool pRefresh = false,
                             bool pDisableValidatorFunctions = false);

      /**
       * \brief Enables or disables the form.  Enabling/disabling a
       * \brief cxForm consequently also enables/disables all the inputs
       * \brief on the form.
       *
       * @param pEnabled Whether the form should be enabled or not (true/false)
       */
      virtual void setEnabled(bool pEnabled) override;

      /**
       * \brief Toggles whether or not to use the status text of the inputs
       * \brief as the status text for the form (in the bottom border) when
       * \brief the inputs gain focus.  This is useful for inputs that have
       * \brief help text.
       *
       * @param pUseInputStatus Whether or not to use the status text of
       *  the inputs as the status text of the form.
       */
      virtual void useInputStatus(bool pUseInputStatus);

      /**
       * \brief Returns whether or not the form has an input with a given
       * \brief label/name.
       *
       * @param pStr The label/name of the input
       * @param pIsLabel If true, then pStr specifies the label; otherwise,
       *  pStr specifies the name of the input.  Defaults to true.
       *
       * @return true if the form has an input with the given label/name;
       *  false if not.
       */
      virtual bool hasInput(const std::string& pStr, bool pIsLabel = true) const;

      /**
       * \brief Returns a pointer to the parent window.  If the cxForm
       * \brief has a parent cxMultiForm, a pointer to that will be
       * \brief returned; otherwise, a pointer to the parent cxWindow
       * \brief will be returned.
       *
       * @return A pointer to the parent window
       */
      virtual cxWindow* getParent() const override;

      /**
       * \brief Returns the index of the next editable input after a given
       * \brief index.  This can loop around - i.e., if a form has 10 inputs
       * \brief and inputs 2 and 5 are editable, and 5 is passed to this
       * \brief function, this function will return 2.  Could also return -1
       * \brief if there are no editable inputs or if the given input is out
       * \brief of bounds.
       *
       * @param pIndex An index of an input on the form
       *
       * @return The index of the next editable input after pIndex.  Could
       *  return -1 if there are no editable inputs, and could return the same
       *  index if the given input is the only editable input on the form.
       */
      // Note: This is commented out because, for some reason, with this in
      //  the code, it seems to screw up all the forms in cprime so that they
      //  only have 1 input, even if this method is not used anywhere..
      //virtual int nextEditableInput(int pIndex) const;

      /**
       * \brief Adds an ncurses attribute to use for one of the items in the
       * \brief window (see the e_WidgetItems enumeration).  For eLABEL,
       * \brief eDATA_READONLY, and eDATA_EDITABLE, the attribute is added
       * \brief to all inputs on the form.  Other item types are added to
       * \brief the form window.
       *
       * @param pItem The item to add the attribute for (see the e_WidgetItems
       *  enumeration).
       * @param pAttr The ncurses atribute to add.
       */
      virtual void addAttr(e_WidgetItems pItem, attr_t pAttr) override;

      /**
       * \brief Sets the ncurses attribute to use for one of the items in the
       * \brief window (see the e_WidgetItems enumeration).  Clears the current
       * \brief set of attributes for the item and inserts the given attribute
       * \brief into the set.  For eLABEL, eDATA_READONLY, and eDATA_EDITABLE,
       * \brief the attribute is set in all inputs on the form.  Other item
       * \brief types are set in the form window.
       *
       * @param pItem The item to apply the attribute for (see the e_WidgetItems
       *  enumeration).
       * @param pAttr The ncurses atribute to apply.
       */
      virtual void setAttr(e_WidgetItems pItem, attr_t pAttr) override;

      /**
       * \brief Removes an ncurses attribute from one of the item lists.  For
       * \brief eLABEL, eDATA_READONLY, and eDATA_EDITABLE, the attribute is
       * \brief removed from all inputs on the form.  Other item types are
       * \brief removed from the form window.
       *
       * @param pItem The item to remove the attribute for (see the
       *  e_WidgetItems enumeration).
       * @param pAttr The ncurses attribute to remove
       */
      virtual void removeAttr(e_WidgetItems pItem, attr_t pAttr) override;

      /**
       * \brief Removes all attributes for a given window item.  For eLABEL,
       * \brief eDATA_READONLY, and eDATA_EDITABLE, the attribute is removed
       * \brief from all inputs on the form.  Other item types are removed
       * \brief from the form window.
       *
       * @param pItem The item to remove attributes for (see the e_WidgetItems
       *  enumeration).
       */
      virtual void removeAttrs(e_WidgetItems pItem) override;

      /**
       * \brief Returns the set of ncurses attributes for a given item.
       *
       * @param pItem The item to retrieve attributes for (see the e_WidgetItems
       *  enumeration).
       * @param pAttrs This will contain the attributes for the item.
       */
      virtual void getAttrs(e_WidgetItems pItem, std::set<attr_t>& pAttrs) const override;

      /**
       * \brief Gathers a set of keys considered to be "navigational" keys from
       * \brief all the inputs on the form.  Note that each input could have
       * \brief a different list of navigational keys; this method gets all
       * \brief of them.
       *
       * @param pNavKeys This will contain the navigational keys gathered from
       *  all inputs on the form.
       */
      virtual void getNavKeys(std::set<int>& pNavKeys) const;

      /**
       * \brief Creates a set of string representations of keys considered to
       * \brief be "navigational" keys, gathered from all inputs on the form.
       *
       * @param pNavKeyStrings This will contain string representations of the
       *  navigational keys, gathered from all inputs on the form.
       */
      virtual void getNavKeyStrings(std::set<std::string>& pNavKeyStrings) const;

      /**
       * \brief Returns whether at least one of the inputs on the form has
       * \brief a key in its list of navigational keys.
       *
       * @param pKey The key to look for
       */
      virtual bool hasNavKey(int pKey) const;

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

      /**
       * \brief Sets the last keypress
       *
       * @param pLastKey The last keypress
       */
      virtual void setLastKey(int pLastKey) override;

      /**
       * \brief This causes the form to exit its input loop with a code of
       * \brief cxID_QUIT.
       */
      virtual void quitNow() override;

      /**
       * \brief This causes the form to exit its input loop with a code of
       * \brief cxID_EXIT.
       */
      virtual void exitNow() override;

      /**
       * \brief Sets a single key to use to display extended help for all
       * \brief inputs.
       *
       * @param pKey A key to use to display extended help for the inputs
       */
      virtual void setExtendedHelpKey(int pKey);

      /**
       * \brief Adds an additional key to be used to display the extended
       * \brief help key for all inputs.
       *
       * @param pKey A key to be used to display extended help
       */
      virtual void addExtendedHelpKey(int pKey);

      /**
       * \brief Returns the keys used to display extended help for the inputs.
       * \brief This returns the set of keys set in the form.  This list is the
       * \brief same for all inputs except when extended help keys are changed
       * \brief on individual inputs.
       *
       * @return The keys used to display extended help for the inputs
       */
      virtual std::set<int> getExtendedHelpKeys() const;

      /**
       * \brief Returns a comma-separated list of strings representing the
       * \brief extended help keys for the form.  This list is the
       * \brief same for all inputs except when extended help keys are changed
       * \brief on individual inputs.
       *
       * @return A comma-separated list of strings representing the extended
       *  help keys for the input.
       */
      std::string getExtendedHelpKeyStrings() const;

      /**
       * \brief Removes all extended help keys.
       */
      virtual void clearExtendedHelpKeys();

      /**
       * \brief Sets whether or not to use the extended help keys.
       *
       * @param pUseExtendedHelpKey Whether or not to use the extended help
       *  keys (true/false)
       */
      virtual void setUseExtendedHelpKeys(bool pUseExtendedHelpKeys);

      /**
       * \brief Returns whether or not the extended help keys are enabled.
       *
       * @return Whether or not the extended help keys are enabled (true/false)
       */
      virtual bool getUseExtendedHelpKeys() const;

      /**
       * \brief Gets the extended help string from one of the inputs (by
       * \brief index).  If the given index is out of bounds, this will return
       * \brief a blank string.
       *
       * @param pIndex The index of the input
       *
       * @return The extended help for the input, or a blank string if pIndex
       *  is out of bounds.
       */
      virtual std::string getExtendedHelp(int pIndex) const;

      /**
       * \brief Returns the extended help string from one of the inputs (by
       * \brief label/name).  If the input is not found, this will return a
       * \brief blank string.
       *
       * @param pIDStr The label/name of the input
       * @param pIsLabel If true (default), then pIDStr will specify the label
       *  of the input.  Otherwise, pIDStr will specify the name of the input.
       *
       * @return The extended help for the input, or a blank string if the
       *  input is not found.
       */
      virtual std::string getExtendedHelp(const std::string& pIDStr, bool pIsLabel = true) const;

      /**
       * \brief Sets the extended help for one of the inputs (by index).
       *
       * @param pIndex The index of the input
       * @param pExtendedHelp The extended help text to set in the input
       */
      virtual void setExtendedHelp(int pIndex, const std::string& pExtendedHelp);

      /**
       * \brief Sets the extended help for one of the inputs (by label/name).
       *
       * @param pIDStr The label/name of the input
       * @param pExtendedHelp The extended help text to set in the input
       * @param pIsLabel If true (default), then pIDStr will specify the label
       *  of the input.  Otherwise, pIDStr will specify the name of the input.
       */
      virtual void setExtendedHelp(const std::string& pIDStr, const std::string& pExtendedHelp,
                                     bool pIsLabel = true);

      /**
       * \brief Sets whether or not the validator function should run for an
       * \brief input when the user presses the input jump key.  If not, the
       * \brief user will always be able to jump from that input, whether or
       * \brief not the input is valid.
       *
       * @param pIndex The index of the input
       * @param pValidate If true, then the input will do validation when the
       *  user presses the jump key (possibly not allowing the user to jump if
       *  the input is not valid).  If false, the user will be able to jump
       *  from the input whether or not the input is valid.
       */
      virtual void validateOnJumpKey(int pIndex, bool pValidate);

      /**
       * \brief Sets whether or not the validator function should run for an
       * \brief input when the user presses the input jump key.  If not, the
       * \brief user will always be able to jump from that input, whether or
       * \brief not the input is valid.
       *
       * @param pID The label/name of the input
       * @param pValidate If true, then the input will do validation when the
       *  user presses the jump key (possibly not allowing the user to jump if
       *  the input is not valid).  If false, the user will be able to jump
       *  from the input whether or not the input is valid.
       * @param pIsLabel If true (default), then pID specifies the input label.
       *  If false, then pID specifies the input name.
       */
      virtual void validateOnJumpKey(const std::string& pID, bool pValidate, bool pIsLabel = true);

      /**
       * \brief Sets whether or not the validator function should run for all
       * \brief inputs when the user presses the input jump key.  If not, the
       * \brief user will always be able to jump from the inputs, whether or
       * \brief not the input is valid.
       *
       * @param pValidate If true, then the inputs will do validation when the
       *  user presses the jump key (possibly not allowing the user to jump if
       *  the input is not valid).  If false, the user will be able to jump
       *  from the inputs whether or not the input is valid.
       */
      virtual void allValidateOnJumpKey(bool pValidate);

      /**
       * \brief Swaps the order of 2 inputs (by index).  If both indexes
       * \brief are the same, or one or both is out of bounds, nothing
       * \brief will be changed.  This does not refresh the screen.
       *
       * @param pInput1Index The index of the first input
       * @param pInput2Index The index of the second input
       *
       * @return Whether or not the inputs got swapped
       */
      virtual bool swap(int pInput1Index, int pInput2Index);

      /**
       * \brief Swaps the order of 2 inputs (by pointer).  If both pointers
       * \brief are the same, or one or both don't exist in the panel, nothing
       * \brief will be changed.  This does not refresh the screen.
       *
       * @param pInput1 A pointer to the first input
       * @param pInput2 A pointer to the second input
       *
       * @return Whether or not the inputs got swapped
       */
      virtual bool swap(std::shared_ptr<cxWindow>& pInput1, std::shared_ptr<cxWindow>& pInput2);

      // We still want to inherit versions of some functions from cxWindow
      using cxWindow::getName;
      using cxWindow::getStatus;

   protected:
      typedef std::vector<std::shared_ptr<cxMultiLineInput> > inputPtrContainer;
      // mInputs stores the pointers to the form's inputs.
      inputPtrContainer mInputs;
      int mCurrentInput = 0; // Keeps track of the current input w/ focus

      // mInputPositions stores the upper-left y and x
      //  coordinates of the inputs - this way, we can keep
      //  track of where the inputs are, even if they use
      //  "virtual" (negative) coordinates.  With ncurses,
      //  if you try to move a window outside the screen
      //  boundaries, it seems to only move the window to
      //  the far edges of the window (i.e., if a window
      //  were at (y, x) = (3, 2) on the screen and you
      //  move a window up 5 lines, ncurses says it's at
      //  (0, 2) rather than (-2, 2).  mInputPositions
      //  would store (-2, 2), and when the form does
      //  scrolling, it will check mInputPositions so
      //  that the inputs maintain their relative position
      //  to each other.
      std::vector<std::pair<int, int> > mInputPositions;

      /**
       * \brief If mTrapNonAssignedFKeys is true, function keys that aren't
       * \brief assigned to anything won't cause the input to exit its
       * \brief input loop.
       */
      bool mTrapNonAssignedFKeys = true;

      /**
       * \brief mStartAtFirstInput keeps track of whether or not the form
       * \brief should always start at the first editable input when
       * \brief showModal() is called.
       */
      bool mStartAtFirstInput = false;

      /**
       * \brief Makes a copy of a cxForm's member variables
       *
       * @param pThatForm A pointer to another cxForm whose members to copy
       */
      void copyCxFormStuff(const cxForm* pThatForm);

      /**
       * \brief Scans all inputs to see if any are editable and updates
       * \brief mEditableInputsExist.
       */
      void lookForEditableInputs();

      /**
       * \brief Handles the input loop.  Returns cxID_QUIT or cxID_EXIT,
       * \brief depending on the user's input.  This function is meant
       * \brief not to be virtual, since it is specifically for
       * \brief cxForm.
       *
       * @param pRunOnLeaveFunction This parameter will be set depending on
       *  whether the onLeave function should be run (this will be true
       *  unless the user presses a function key AND the form is on a parent
       *  multiForm).
       *
       * @return A return code (cxID_QUIT or cxID_EXIT, depending on the
       *  user's input).
       */
      long doInputLoop(bool& pRunOnLeaveFunction);

      /**
       * \brief Gets input from the current input (for use in the input loop).
       *
       * @param pReturnCode This variable will hold the return code as a
       *  result of the user's interactions.
       * @param pRunOnLeaveFunction This parameter will be set depending on
       *  whether the onLeave function should be run (this will be true
       *  unless the user presses a function key AND the form is on a parent
       *  multiForm).
       * @param pFunctionExists This parameter will be set depending on
       *  whether a function existed for the key that was caught in the form.
       *  If a function did exist, it would be useful not to cycle to the next
       *  input the next time through the input loop.
       * @param pExitOverride If true, this function will always return false
       *  when the user exits the last input (default).  This is here to aid
       *  input loops in deriving classes.
       *
       * @return Whether or not to continue the input loop
       */
      virtual bool doCurrentInput(long& pReturnCode,
                                  bool& pRunOnLeaveFunction,
                                  bool& pFunctionExists,
                                  bool pExitOverride = false);

      /**
       * \brief Sets whether the form is modal or not.
       *
       * @param pIsModal true if the form is modal, or false if not.
       */
      virtual void setIsModal(bool pIsModal);

   private:
      // If mStacked is true, then the form will behave
      //  assuming all the inputs are on their own line (no
      //  side-by-side inputs).
      bool mStacked;

      // mTotalInputHeight keeps track of the total height
      //  of all inputs, if the form were in "stacked" mode.  It
      //  represents the total height of all inputs as if they
      //  were all stretched out with each above the next.
      unsigned int mTotalInputHeight = 0;

      bool mAutoExit; // Whether or not to auto exit the form
                      //  if the user presses enter/tab/etc. on
                      //  the last field.
      bool mApplyAttrDefaults = true; // Whether or not to apply attribute defaults
      // mCustomStatus will be true if the user explicitly called setStatus()
      //  on the form, in which case the status text from the inputs will
      //  not be used as the status text for the form.
      bool mCustomStatus = false;
      // mEditableInputsExist will be true if at least 1
      //  editable input exists on the form.
      bool mEditableInputsExist = false;
      // If the user has put an input along one of the
      //  borders, mInputsOnBorder will be true.  Then,
      //  during scrolling, etc., the form will behave
      //  accordingly.
      bool mInputsOnBorder;
      cxMenu mJumpMenu;  // Menu to choose which input to jump to
      int mInputJumpKey = DEFAULT_CXFORM_JUMPMENU_KEY; // Hotkey to use for jumping to another input
      bool mAllowQuit = true;           // Whether or not to allow the user to quit
      bool mAllowExit = true;           // Whether or not to allow the user to exit
      bool mExitOnLeaveFirst = false;   // Whether or not to exit when the user
                                        //  leaves the first input when going backwards.
      bool mWaitForInputIfEmpty = true; // Whether or not to get input in showModal()
                                        //  if there are no editable inputs
      bool mDataChanged = false;        // Whether or not any data has changed

      // Functions to be run at various points in the input loop
      std::shared_ptr<cxFunction> mLoopStartFunction; // At the start of each cycle
      std::shared_ptr<cxFunction> mLoopEndFunction;   // At the end of each cycle

      // mParentMultiForm is a pointer to a cxMultiForm, in
      // case the form appears on a cxMultiForm (it can be
      // nullptr if it isn't on a cxMultiForm).
      cxMultiForm *mParentMultiForm = nullptr;

      // Whether or not to cycle to the next/previous input in the input loop
      // (this is set false in setCurrentInput()).
      bool mCycleInput = true;

      // mUseInputStatus specifies whether or not to use the status text of
      // the inputs as the status text for the form (in the bottom border
      // This is useful for inputs with help messages.
      bool mUseInputStatus = true;

      // mHasBeenShown specifies whether or not the cxForm has been shown.
      // It is set true when show() is called.  mHasBeenShown is used in
      // the unhide() method; if the form has been shown before, then the
      // inputs on the form will be unhidden in addition to the form.  The
      // reason for this is that if the form hasn't been shown, then the
      // text in the inputs wouldn't have been drawn, so unhiding the inputs
      // may cause holes on the screen.
      bool mHasBeenShown = false;

      // mExtendedHelpKeys specifies keys that can be pressed to display
      // extended help for the inputs.  mUseExtendedHelpKeys specifies whether
      // or not to use them.
      std::set<int> mExtendedHelpKeys;
      bool mUseExtendedHelpKeys = true;

      // mAllValidateOnJumpKey specifies whether or not the validator function
      // for each input should run when the user presses the form's input jump
      // key.  If this is set to false, then when an input is appended,
      // mInputJumpKey will be added to the input's list of keys that skip the
      // validator function, allowing the user to jump to another input
      // whether or not the text in the input is valid.  Otherwise, they could
      // only jump if the input is valid.
      bool mAllValidateOnJumpKey = true;

      // mInputShowBools contains a boolean for each input
      // that specifies whether it should be shown.  This
      // was added to aid in scrolling the inputs in
      // stacked mode.  It's possible to have an input
      // on a top/bottom border and still have it showing,
      // but when we're scrolling the inputs, we will want
      // to make sure that showAllInputs() doesn't show
      // certain inputs.
      std::vector<bool> mInputShowBools;

      // mInputJumpIndexes stores indexes for inputs to
      //  which to set focus.  Indexes are appended to
      //  this container when the user calls addInputJumpKey(),
      //  and the indexes in this container are used by
      //  changeFocus().
      std::vector<int> mInputJumpIndexes;

      // Makes sure this window is exactly the size needed to surround the inputs.
      void fitToInputs();

      // Copies the current menu item's help string to the
      // status string, and optionally updates the status.
      inline void useInputStatusAsFormStatus(bool refreshStatus = false);

      // Selects the next editable input.  Returns true if the current input
      // was the last input, or false otherwise.
      bool selectNextInput();

      // Selects the previous editable input.  Returns true if the current
      // input was the first input, or false otherwise.
      bool selectPrevInput();

      // The following functions are helpers for selectNextInput().

      // Returns the greatest left side of all the inputs.
      inline int greatestLeftSide() const;

      // Returns the lowest left side of all the inputs.
      inline int lowestLeftSide() const;

      // Returns the greatest top side of all the inputs.
      inline int greatestTopSide() const;

      // Returns the lowest top side of all the inputs.
      inline int lowestTopSide() const;

      // Returns the index in mInputs that has the next lowest
      // top side of an input, or -1 if one isn't found.
      inline int inputNextLowestTop(int topSide);

      // Returns the index in mInputs that has the next lowest
      // left side of an input, or -1 if one isn't found.
      inline int inputNextLowestLeft(int leftSide);

      // Removes the last character in the current input if it
      // is a space.  (In testing, it was found that when
      // the user hits the TAB key to go to the next input, it
      // was getting an extra space for the current input.  This
      // is a kludge, but it works..)
      inline void clearInputIfSpace();

      // Frees the memory used by the inputs
      inline void freeInputs();

      // Sets the current input for focus for a form.
      static std::string changeFocus(void *theForm, void *theIndex);

      // Scans a string for hotkeys and uses them for keys to press
      // to jump to a particular input.
      // Parameters:
      //  pItemText: The text to scan for hotkeys
      //  pIndex: The index of the input to which to jump for the hotkeys
      void addJumpKeys(const std::string& pItemText, int pIndex);

      // Shows the input jump menu for the form and handles jumping
      // to another input upon the user's selection.
      void showJumpMenu();

      // Returns the highest editable input index (or -1 if there are no inputs).
      int highestEditableInput() const;

      // Applies attribute defaults to an input.  This is meant for new inputs
      // being appended to the form (used in the append() methods).
      //
      // Parameters:
      //  pInput: A pointer to an input to which to apply the attributes.
      void applyAttrDefaults(std::shared_ptr<cxMultiLineInput>& pInput);

      // Applies all the form's attributes to an input.  This meant for new
      // inputs being appended to the form (used in the append() methods).
      //
      // Parameters:
      //  pInput: A pointer to an input to which to apply the attributes.
      void applyWinAttributes(std::shared_ptr<cxMultiLineInput>& pInput);

      friend class cxMultiLineInput;
      friend class cxMultiForm;
      friend class cxPanel;
};

#endif
