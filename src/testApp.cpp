// Copyright (c) 2005-2007 Michael H. Kinney
//
// This is a "testbed" for the different functionality of the cxWidgets.
// It's mainly used for testing/debugging. (use examples at own risk!!!) ;-)

#include "cxBase.h"
#include "cxDialog.h"
#include "cxMessageDialog.h"
#include "cxForm.h"
#include "cxWindow.h"
#include "cxButton.h"
#include "cxFileViewer.h"
#include "cxMenu.h"
#include "cxInput.h"
#include "cxNotebook.h"
#include "cxMultiLineInput.h"
#include "cxStringUtils.h"
#include "cxScrolledWindow.h"
#include "cxMultiForm.h"
#include "cxComboBox.h"
#include "cxDatePicker.h"
#include "cxOpenFileDialog.h"
#include "cxDate.h"
#include "cxPanel.h"
#include "cxSearchPanel.h"
#include "cxValidators.h"
#include "floatingPtInputWithRightLabel.h"
#include <unistd.h> //  for sleep()
#include <stdlib.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <memory>
#include <cassert>
using namespace cxStringUtils;
using namespace cxBase;
using namespace std;
using std::shared_ptr;
using std::make_shared;

// These IDs are used for the main menu.
// Note: When adding IDs to this list, also add it to getMenuItemIDStr() to
// return a string representation of the code - this is useful for
// debugging.
enum menuItemIDs
{
   menuCode = cxFIRST_AVAIL_RETURN_CODE,
   menuItemWithMultipleHotkeyCode,
   menuScrollingCode,
   inputWithFKeysCode,
   formWithFKeysCode,
   inputCode,
   inputMaskingCode,
   multiInputMaskingCode,
   multiLineInputLabelChangeCode,
   inputLabelChangeCode,
   msgOKCode,
   msgOKCancelCode,
   msgCancelCode,
   msgYesCode,
   msgYesNoCode,
   msgNoYesCode,
   msgNoCode,
   windowCode,
   extTitleWinCode,
   extStatusWinCode,
   msgBox1Code,
   msgBox2Code,
   msgBox3Code,
   msgBox4Code,
   splash1Code,
   splash2Code,
   fileViewerCode,
   attributesSetterCode,
   singleLineInputCode,
   multiLineInputCode,
   exitCode,
   someItem,
   formCode,
   stackedFormScrollingCode,
   stackedFormScrollingCode2,
   titleAlignmentCode,
   statusAlignmentCode,
   cxWindowCopyConstructorCode,
   panelDemoCode,
   inputsWithDifferentColorsCode,
   formInputColorsCode,
   borderlessFormCode,
   testFormReadOnlyCode,
   formWithInputValueValidatorCode,
   testFormPosCode,
   formWithMenuCode,
   cxWindowAssignmentCode,
   cxMenuAssignmentCode,
   cxFormAssignmentCode,
   multiLineInputResizeCode,
   setValFuncCode,
   changeMenuItemTextCode,
   windowWriteTextCode,
   formChangeInputFocusWhileModalCode,
   allFormInputsGoReadOnlyCode,
   formNoQuitCode,
   menuNoQuitCode,
   scrolledWindowCode,
   cxInputBorderChangeCode,
   cxMultiLineInputBorderChangeCode,
   comboBoxCode,
   comboBoxOnFormCode,
   inputWithoutBorderCode,
   formDataChangedCode,
   integerValidateCode,
   floatingPtValidateCode,
   enableDisableFormCode,
   multiFormWithDisabledSubformCode,
   comboBoxMenuDisableCode,
   multiForm2Code,
   inputQuitKeysOnFormCode,
   cxMenuFocusFunctionsCode,
   loneInputWithLoopDisabledOnFormCode,
   cxMenuWithMultipleItemHotkeysCode,
   cxMenuItemHotkeysOffMenuCode,
   cxPanelCode,
   cxPanelCode2,
   cxPanelDelAllWindowsCode,
   cxWindowWithFunctionKeysCode,
   cxWindowWithMultipleTitleStringsCode,
   cxWindowWithMultipleStatusStringsCode,
   cxMenuNoMoreSelectableItemsWhileModalCode,
   cxPanelWithNonEditableSubwindowsCode,
   cxMenuWithDuplicateHotkeysCode,
   cxPanelWindowSwapCode,
   cxPanel_cxMenuUpArrowLeaveCode,
   windowFunctionKeyNoOnLeaveCode,
   formFunctionKeyNoOnLeaveCode,
   cxMultiLineInputFunctionKeyNoValidatorCode,
   cxFormMultiLineInputFunctionKeyNoValidatorCode,
   keyFunctionAutoExitOnLastFormInputCode,
   cxFormNavKeysCode,
   addMessageLineCode,
   cxFormStartAtFirstInputCode,
   cxPanelGetLastWindowCode,
   cxFormClearOnlyEditableCode,
   cxButtonOnClickCode,
   cxMenuMiscTestsCode,
   cxSearchPanelTestCode,
   cxSearchPanelResizeCode,
   cxSearchPanelCopyConstructorCode,
   cxMenuResizeCode,
   cxWindowAlignCode,
   cxNotebookTest1Code,
   cxNotebookTest2Code,
   cxNotebookRemoveWindowTestCode,
   cxNotebookWithEmptyPanelTestCode,
   cxNotebookSetTabSpacingCode,
   cxNotebookSwapTestCode,
   cxWindowBorderCode,
   cxWindowSizeCode,
   cxWindowAttributesCode,
   cxWindowMouseFunctionTestCode,
   cxFormMouseFunctionTestCode,
   cxMenuMouseFunctionTestCode,
   cxPanelWithButtonTestCode,
   cxMultiLineInputNumericFloatingPtCode,
   cxMultiLineInputNumericFloatingPtWithRangeCode,
   cxMultiLineInputNumericWholeCode,
   cxMultiLineInputNumericWholeWithRangeCode,
   cxMultiLineInputTextValidationCode,
   cxMultiLineInputForceUpperCode,
   cxMultiLineInputNonBlankCode,
   cxMultiLineInputTextValidationNonBlankForceUpperCode,
   cxMultiLineInputTextValidationAutoCompletionCode,
   cxMultiLineInputExtendedHelpKeyCode,
   cxFormExtendedHelpKeyCode,
   cxMultiLineInputWithRightLabelCode,
   cxMultiLineInputWithRightLabelOnFormCode,
   cxMultiLineInputAutoGenerateHelpStringCode,
   itemSalesCode,
   cxFormAppendPairCode,
   cxFormAppendComboBoxPairCode,
   cxMultiLineInputSetValueCode,
   cxMultiLineInputMaxInputLengthCode,
   cxMultiLineInputExtendedHelpColorsAndAttrsCode,
   cxMenuAltItemTextCode,
   cxComboBoxNotEditableMenuEnabledCode,
   cxMultiLineInputOverrideOnKeypressCode,
   cxWindowAddMessageLinesAboveCode,
   cxWindowBorderStyles,
   cxDatePickerTestCode,
   cxOpenFileDialogTestCode
   // Note: When adding IDs to this list, also add it to getMenuItemIDStr() to
   //  return a string representation of the code - this is useful for
   //  debugging.
};

// Shows the main menu
void doMenu();

// These are tester functions for the various widgets.

void msgOK();

void msgOKCancel();

void msgCancel();

void msgYes();

void msgYesNo();

void msgNoYes();

void msgNo();

void testInput1();

void testMenu1();

void testInput2();

void window1();

void msgBox1();

void msgBox2();

void msgBox3();

void msgBox4();

void splash1();

void splash2();

void fileViewer();

void attributesSetter();

void singleLineInput();

void multiLineInput();

void form();

void cxMenuScrolling();

void inputsWithFKeys();

void formWithFKeys();

void cxInputMasking();

void cxMultiLineInputMasking();

void cxWindowMoveRelative();

void stackedFormScrolling();

void externalTitleWindow();

void externalStatusWindow();

void setInputLabel();

void setMultiLineInputLabel();

void titleAlignment();

void statusAlignment();

void cxWindowCopyConstructor();

void demoPanels();

void menuItemWithMultipleHotkeys();

void inputsWithDifferentColors();

void formInputColors();

void borderlessForm();

void unselectableMenuItem();

void testFormReadOnly();

void stackedFormScrolling2();

void formWithInputValueValidator();

void testFormPos();

void formWithMenu();

void cxWindowAssignment();

void cxMenuAssignment();

void cxFormAssignment();

void multiLineInputResize();

void setValFunc();

void formUpArrowFunction();

void changeMenuItemText();

void windowWriteText();

void formChangeInputFocusWhileModal();

void allFormInputsGoReadOnly();

void cxFormNoQuit();

void cxMenuNoQuit();

void scrolledWindow();

void cxInputBorderChange();

void cxMultiLineInputBorderChange();

void getFormKeys();

void multiForm();

void multiForm2();

void simpleForm1();

void comboBox();

void comboBoxOnForm();

void inputWithoutBorder();

void formDataChanged();

void integerValidate();

void floatingPtValidate();

void integerValidateOnForm();

void floatingPtValidateOnForm();

void formFunctionKeys();

void enableDisableForm();

void multiFormWithDisabledSubform();

void comboBoxMenuDisable();

void inputQuitKeysOnForm();

void cxMenuFocusFunctions();

// Tests a cxMultiLineInput, with its input
//  loop disabled, on a cxForm
void cxInputOnFormLoopDisabled();

// Tests a cxForm with 1 input, with
//  the input's loop disabled (the input's
//  onFocus and onLeave functions
//  should still run, just once, and
//  there should be no wacky problems).
void loneInputWithLoopDisabledOnForm();

// Tests the behavior of a cxMenu that has
//  multiple items with the same hotkey.
void cxMenuWithMultipleItemHotkeys();

// Tests the behavior of a cxMenu with item hotkeys
//  where some of the items are outside the
//  viewable list of items
void cxMenuItemHotkeysOffMenu();

// Tester for cxPanel
void cxPanelTest();

// Another tester for cxPanel - Has a cxMenu and a cxForm, similar to cprime's
//  entry screens
void cxPanelTest2();

// Tester for cxPanel's delAllWindows()
void cxPanelDelAllWindowsTest();

void cxWindowWithFunctionKeys();

void cxWindowWithMultipleTitleStrings();

void cxWindowWithMultipleStatusStrings();

void cxMenuNoMoreSelectableItemsWhileModal();

// Tests a cxPanel that has subwindows that are enabled but are not editable
//  (i.e., a non-editable input, etc.)
void cxPanelWithNonEditableSubwindows();

// Tests a cxMenu with the same hotkey for multiple items, and
//  where it needs to scroll to show each item.
void cxMenuWithDuplicateHotkeys();

// Tests swapping 2 windows in a cxPanel
void cxPanelWindowSwap();

void cxPanel_cxMenuUpArrowLeave();

// Tests the feature of cxWindow not to run its onLeave function when a
//  key function is run with runOnLeaveFunction set false.
void windowFunctionKeyNoOnLeave();

// Same as above, but with a cxForm
void formFunctionKeyNoOnLeave();

// Tests the feature of cxMultiLineInput not to run its validator function
//  when a key function is run with the runValidatorFunction set false.
void cxMultiLineInputFunctionKeyNoValidator();

// Similar to above, but on a form
void cxFormMultiLineInputFunctionKeyNoValidator();

// Tester for a key function on the last input of a cxForm, where the
//  input should exit after the function runs (the form should exit too).
void keyFunctionAutoExitOnLastFormInput();

// Tests cxForm::getNavKeys() and cxForm::hasNavKey()
void cxFormNavKeys();

// Tests addMessageLine() and removeMessageLine() in cxWindow.
void addMessageLine();

// Tests cxForm always starting at the first input when showModal() is called
//  (via use of setStartAtFirstInput()).
void cxFormStartAtFirstInput();

// Tester for cxPanel::getLastPage()
void cxPanelGetLastWindow();

// Tests cxForm's clearInputs() method only clearing inputs that are editable.
void cxFormClearOnlyEditable();

// Tester for a cxButton used with an onClick event function
void cxButtonOnClick();

// Tester for misc. cxMenu functions (user-interface not necessarily required)
void cxMenuMiscTests();

// General cxSearchPanel test
void cxSearchPanelTest();

// Test for cxSearchPanel's resize(), setFormHeight(), and setMenuHeight().
void cxSearchPanelResize();

// Tests the cxSearchPanel copy constructor
void cxSearchPanelCopyConstructor();

// Tester for cxMenu's resize()
void cxMenuResize();

// Tester for the window alignment code
void cxWindowAlignTest();

// Tester for cxNotebook
void cxNotebookTest1();

// Another tester for cxNotebook
void cxNotebookTest2();

// Tester for removing a window from a cxNotebook
void cxNotebookRemoveWindowTest();

// Tester for cxNotebook with an empty panel
void cxNotebookWithEmptyPanelTest();

// Tester for cxNotebook's setTabSpacing()
void cxNotebookSetTabSpacing();

// Tester for cxNotebook's swap()
void cxNotebookSwapTest();

// Tester for the window border code
void cxWindowBorderTest();

// Tester for the window sizes
void cxWindowSizeTest();

// Tester for window attributes
void cxWindowAttributesTest();

// Tester for setting events for mouse states in a cxWindow.
void cxWindowMouseFunctionTest();

// Tester for setting events for mouse states in a cxMenu.
void cxMenuMouseFunctionTest();

// Tester for setting events for mouse states in a cxForm.
void cxFormMouseFunctionTest();

// Displays a cxPanel with a cxButton on it - Helps make sure that a button's
//  onClick function runs when it is clicked on.
void cxPanelWithButton();

// Tests a cxMultiLineInput with its type set to eINPUT_TYPE_NUMERIC_FLOATING_PT
void cxMultiLineInputNumericFloatingPt();

// Tests a cxMultiLineInput with its type set to eINPUT_TYPE_NUMERIC_FLOATING_PT
//  with a range
void cxMultiLineInputNumericFloatingPtWithRange();

// Tests a cxMultiLineInput with its type set to eINPUT_TYPE_NUMERIC_WHOLE
void cxMultiLineInputNumericWhole();

// Tests a cxMultiLineInput with its type set to eINPUT_TYPE_NUMERIC_WHOLE
//  with a range
void cxMultiLineInputNumericWholeWithRange();

// Tests a cxMultiLineInput with its type set to eINPUT_TYPE_TEXT (which
//  is the default) with valid text set up.
void cxMultiLineInputTextValidation();

// Tests a cxMultiLineInput with its forceUpper option turned on
void cxMultiLineInputForceUpper();

// Tests a cxMultiLineInput with its allowBlank turned off
void cxMultiLineInputNonBlank();

// Tests changing and enabling/disabling the extended help key for a
//  cxMultiLineInput.
void cxMultiLineInputExtendedHelpKey();

// Tests changing and enabling/disabling the extended help key for a
//  cxForm.
void cxFormExtendedHelpKey();

// Tests a cxMultiLineInput with text validation, non blank, and force upper
//  options set.
void cxMultiLineInputTextValidationNonBlankForceUpper();

// Tests cxMultiLineInput set up as a text input with valid values - demonstrates
//  the auto-completion & right label updates as you type
void cxMultiLineInputTextValidationAutoCompletion();

// Tests a cxMultiLineInput with a right label
void cxMultiLineInputWithRightLabel();

// Tests cxMultiLineInput with a right label on a cxForm
void cxMultiLineInputWithRightLabelOnForm();

// Tests cxMultiLineInput's autoGenerateHelpString() feature.
void cxMultiLineInputAutoGenerateHelpString();

// playing with a semi-real form
void itemSales();

// Tester for cxForm::appendPair()
void cxFormAppendPair();

// Tester for cxForm::appendComboBoxPair()
void cxFormAppendComboBoxPair();

// Tests cxMultiLineInput's setValue() with various input types to make sure
//  the value is set when it should be and not set when it shouldn't be.
void cxMultiLineInputSetValue();

// Tester for setting the maximum input length of a cxMultiLineInput
void cxMultiLineInputMaxInputLength();

// Tester for setting the extended help color & attribute for a
//  cxMultiLineInput
void cxMultiLineInputExtendedHelpColorsAndAttrs();

// Tests setting a message with newlines in a cxWindow (currently, this is not working properly)
void cxWindowMessageNewlines();

// Tester for using alternate item text in a cxMenu
void cxMenuAltItemText();

// Tests a cxComboBox that is not editable but with a drop-down menu that is
//  enabled - It should allow the user to choose an option from the menu but
//  not type in the input.
void cxComboBoxNotEditableMenuEnabled();

// Tests overriding the onKeypress function for a cxMultiLineInput
void cxMultiLineInputOverrideOnKeypress();

// Tests adding message lines above current text in a cxWindow
void cxWindowAddMessageLinesAbove();

// Tests making windows with different border styles
void cxWindowVariousBorderStyles();

// Demonstrates the cxDatePicker dialog
void cxDatePickerTest();

// Demonstrates the cxOpenFileDialog
void cxOpenFileDialogTest();

// Functions for use with forms & fields
string someFunction(int& int1, int& int2);
string returnBBye();
string someFunction2(cxMultiLineInput& input);
string someFunction3(cxMultiLineInput& input);
string hello();
string genericMessageFunction();
string genericMessageFunction_1str(string& pMessage);
string testBoxSuccess(void* Foo1, void* Foo2, void* Foo3, void* Foo4);

// Tests whether an input has the values "Y" or "N"
string inputYesNo(void *theInput, void *, void *, void *);

// These functions can be used for simple key, onLeave, & validator function tests.
string testKeyFunction(void *unused, void *unused2)
{
   cxBase::messageBox("testKeyFunction()");

   return("");
} // testKeyFunction

string testOnFocusFunction(void *unused, void *unused2, void *unused3, void *unused4)
{
   cxBase::messageBox("testOnFocusFunction()");

   return("");
} // testOnFocusFunction

string testOnLeaveFunction(void *unused, void *unused2, void *unused3, void *unused4)
{
   cxBase::messageBox("testOnLeaveFunction()");

   return("");
} // testOnLeaveFunction

string testInputValidator(void *theMultiLineInput, void *unused2, void *unused3,
                          void *unused4)
                          {
   string retval;

   cxMultiLineInput *pInput = static_cast<cxMultiLineInput*>(theMultiLineInput);
   if (pInput->getValue() != "test")
   {
      cxBase::messageBox("Warning: Input is not 'test'");
      retval = "Input is not 'test'";
   }

   return(retval);
} // testInputValidator

// Title & status windows (must be created after
// calling cxBase::init()
shared_ptr<cxWindow> gTitleLine;
shared_ptr<cxWindow> gStatusLine;

// Returns a string representation of one of the menu item ID codes.  This is
//  useful for debugging.
string getMenuItemIDStr(long pMenuItemID);

string someWeirdFunc(void *theForm, void *unused)
{
   cxBase::messageBox("Hello.");
   return("");
}

//// MAIN ////
int main(int argc, char* argv[])
{
   cxBase::init(true);
   cxBase::setAttr(eDATA_EDITABLE, cxBase::getAttr("UNDERLINE"));

   cxObject ourcxObject;
   ourcxObject.UseColors(true);

   // Create & display the title & status windows
   gTitleLine = make_shared<cxWindow>(nullptr, 0, 0, 1, cxBase::width(), "",
                                      "cxWidgets test application", "", eBS_NOBORDER);
   gStatusLine = make_shared<cxWindow>(nullptr, cxBase::height()-1, 0, 1, cxBase::width(),
                                       "", "Status", "", eBS_NOBORDER);
   gStatusLine->setHorizMessageAlignment(eHP_CENTER);
   gTitleLine->show();
   gStatusLine->show();

   // Display the main menu
   doMenu(); // MAIN MENU
   // TODO: Setting a key function to a cxForm after inputs have been appended
   // no longer fires the function, as in this example:
   /*
   cxForm iForm(nullptr, 1, 0, 5, 70, "Form");
   cxMultiLineInput *iInput = iForm.append(1, 1, 1, 40, "Input:");
   iInput->setName("someInput");
   //iInput->setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
   iForm.setKeyFunction(KEY_F(5), someWeirdFunc, &iForm, nullptr, false, false, true);
   cxBase::messageBox("testApp: First input's first input's exit keys:" + iForm.getInput(0)->firstInputExitKeys() + ":");
   iForm.showModal();
   */

   /*
   cxPanel mainPanel(nullptr, 1, 0, 20, 80, "Main panel", "", "Main status",
                     eBS_SINGLE_LINE);
   //shared_ptr<cxPanel> subPanel = make_shared<cxPanel>(nullptr, 0, 0, 20, 80, "Subpanel", "",
   //                                                    "Subpanel status", eBS_SINGLE_LINE);
   //mainPanel.append(subPanel, 0, 0, false);
   shared_ptr<cxMultiForm> multiForm = make_shared<cxMultiForm>(nullptr, 0, 0, 20, 80, "MultiForm");
   mainPanel.append(multiForm, 0, 0, false);
   shared_ptr<cxForm> searchForm = make_shared<cxForm>(nullptr, 0, 0, 5, 18, "Search form", eBS_NOBORDER);
   searchForm->append(0, 0, 1, 18, "Input 1:");
   searchForm->append(1, 0, 1, 18, "Input 2:");
   searchForm->append(2, 0, 1, 18, "Input 3:");
   searchForm->append(3, 0, 1, 18, "Input 4:");
   searchForm->append(4, 0, 1, 18, "Input 5:");
   multiForm->appendForm(searchForm, 1, 1);
   //subPanel->append(searchForm, 1, 1, false);
   shared_ptr<cxForm> dataForm = make_shared<cxForm>(nullptr, 0, 0, 5, 18, "Data form", eBS_NOBORDER);
   dataForm->append(0, 0, 1, 18, "Input 1:");
   dataForm->append(1, 0, 1, 18, "Input 2:");
   dataForm->append(2, 0, 1, 18, "Input 3:");
   dataForm->append(3, 0, 1, 18, "Input 4:");
   dataForm->append(4, 0, 1, 18, "Input 5:");
   multiForm->appendForm(dataForm, 7, 1);
   //subPanel->append(dataForm, 7, 1, false);
   mainPanel.showModal();
   */

   /*
   // Quick test to demonstrate the ability of a cxMultiLineInput
   //  to allow movement with the up & down arrow, etc.
   cxMultiLineInput iInput(nullptr, 1, 0, 5, 10, "", eBS_SINGLE_LINE);
   iInput.setTitle("Input");
   iInput.setValue("This is a huge string.");
   iInput.addValueAttr(A_STANDOUT);
   iInput.showModal();
   messageBox(iInput.getValue());
   */


   //simpleForm1();
   //cxComboBox iComboBox(nullptr, 1, 0, 2, 25, "Input:", eBS_NOBORDER, eINPUT_EDITABLE, nullptr, true);
   //iComboBox.showModal();

/*
   // Test removing a menu item by index w/ no submenu
   cxMenu iMenu(nullptr, 0, 0, 20, 40, "Test");
   iMenu.append("Item 1", 0);
   iMenu.append("Item 2", 1);
   iMenu.removeMenuItem(2);
   iMenu.showModal();
*/
/*
   // Test removing a menu item by title
   cxMenu iMenu(nullptr, 0, 0, 20, 40, "Test");
   iMenu.append("Item 1", 0);
   iMenu.append("Item 2", 1);
   iMenu.removeMenuItem("Item 1");
   iMenu.showModal();
*/
/*
   // Test cxInput with more complicated input masking.
   cxInput iInput(nullptr, 0, 0, 23, "Phone:");
   iInput.setValidator("(ddd) ddd-dddd");
   //iInput.setValidator("ddddd");
   //iInput.toggleMasking(true);
   iInput.showModal();
   messageBox(iInput.getValue());
*/
/*
   // Test cxMultiLineInput with more complicated input masking.
   cxMultiLineInput iInput(nullptr, 0, 0, 1, 22);
   iInput.setValidator("(ddd) ddd-dddd");
   //iInput.toggleMasking(true);
   iInput.showModal();
   messageBox(iInput.getValue());
*/
/*
   cxForm iForm(nullptr, 1, 1, 4, 25, "Info");
   //iForm.append(1, "Name:");
   iForm.append(1, "Phone:", "(ddd) ddd-dddd");
   iForm.setValue("Phone:", "(503) 123-4567");
   iForm.showModal();
   messageBox(iForm.getValue("Phone:"));
*/
/*
   // Test an input with complex formatting
   //  and setting its value before setting
   //  focus.
   cxInput iInput(nullptr, 0, 0, 23, "Phone:");
   iInput.setValidator("(ddd) ddd-dddd");
   iInput.setValue("(503) 123-4567");
   iInput.showModal();
*/
/*
   ostringstream os;
   os << getch();
   messageBox(os.str());
*/

   cxBase::cleanup();

   return (0);
} // main

// Shows the main menu
void doMenu()
{
   // Set up the submenus

   // cxMenu tester stuff
   cxMenu menuSubMenu(nullptr, 1, 0, 0, 50, "Test cxMenu Stuff");
   menuSubMenu.setWrapping(true);
   menuSubMenu.append("Play with menus", menuCode, "", cxITEM_NORMAL, true);
   menuSubMenu.append("Menu item multiple hotkeys", menuItemWithMultipleHotkeyCode,
                      "", cxITEM_NORMAL, true);
   menuSubMenu.append("Item hotkeys w/ scrolling", cxMenuItemHotkeysOffMenuCode,
                      "", cxITEM_NORMAL, true);
   menuSubMenu.append("cxMenu focus/loop functions", cxMenuFocusFunctionsCode, "", cxITEM_NORMAL, true);
   menuSubMenu.append("Changing menu item text", changeMenuItemTextCode, "",
                       cxITEM_NORMAL, true);
   menuSubMenu.append("cxMenu assignment", cxMenuAssignmentCode,
                       "", cxITEM_NORMAL, true);
   menuSubMenu.append("Menu with scrolling", menuScrollingCode, "", cxITEM_NORMAL,
                       true);
   menuSubMenu.append("Menu with quit disabled", menuNoQuitCode, "", cxITEM_NORMAL, true);
   menuSubMenu.append("Menu with multi-item hotkeys", cxMenuWithMultipleItemHotkeysCode, "", cxITEM_NORMAL, true);
   menuSubMenu.append("All items => unselectable while modal", cxMenuNoMoreSelectableItemsWhileModalCode, "", cxITEM_NORMAL, true);
   menuSubMenu.append("Same hotkey for more than 1 item", cxMenuWithDuplicateHotkeysCode, "", cxITEM_NORMAL, true);
   menuSubMenu.append("Menu resize", cxMenuResizeCode, "", cxITEM_NORMAL, true);
   menuSubMenu.append("Mouse event function test", cxMenuMouseFunctionTestCode,
                      "", cxITEM_NORMAL, true);
   menuSubMenu.append("Display vs. alternate item text", cxMenuAltItemTextCode, "",
                      cxITEM_NORMAL, true);
   menuSubMenu.append("Misc. cxMenu tests", cxMenuMiscTestsCode, "",
                      cxITEM_NORMAL, true);

   // cxInput and cxMultiLineInput tester stuff
   cxMenu inputSubMenu(nullptr, 1, 0, 0, 46, "cxInput/cxMultiLineInput");
   inputSubMenu.setWrapping(true);
   inputSubMenu.append("Single-line input with border", inputCode, "", cxITEM_NORMAL,
                       true);
   inputSubMenu.append("Single-line input (borderless)", singleLineInputCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Single-line input with masking", inputMaskingCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Single-line input label change", inputLabelChangeCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Multi-line input", multiLineInputCode, "", cxITEM_NORMAL,
                       true);
   inputSubMenu.append("Multi-line input w/ masking", multiInputMaskingCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Multi-line input label change", multiLineInputLabelChangeCode,
                       "", cxITEM_NORMAL, true);
   inputSubMenu.append("Multi-color inputs", inputsWithDifferentColorsCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Input with Fn keys", inputWithFKeysCode, "", cxITEM_NORMAL,
                       true);
   inputSubMenu.append("Multi-line input resizing", multiLineInputResizeCode,
                       "", cxITEM_NORMAL, true);
   inputSubMenu.append("cxInput border change", cxInputBorderChangeCode, "", cxITEM_NORMAL,
                       true);
   inputSubMenu.append("cxMultiLineInput border change", cxMultiLineInputBorderChangeCode,
                       "", cxITEM_NORMAL, true);
   inputSubMenu.append("Integer validation", integerValidateCode, "", cxITEM_NORMAL, true);
   inputSubMenu.append("Floating-pt. validation", floatingPtValidateCode, "", cxITEM_NORMAL, true);
   inputSubMenu.append("Function hotkey, no run validator",
                       cxMultiLineInputFunctionKeyNoValidatorCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Floating-point type",
                       cxMultiLineInputNumericFloatingPtCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Floating-point type, range 2-5",
                       cxMultiLineInputNumericFloatingPtWithRangeCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Whole # type",
                       cxMultiLineInputNumericWholeCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Whole # type, range 2-5",
                       cxMultiLineInputNumericWholeWithRangeCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Text type w/ valid strings",
                       cxMultiLineInputTextValidationCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Force uppercase",
                       cxMultiLineInputForceUpperCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Allow blank = false",
                       cxMultiLineInputNonBlankCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Text validation, no blank, force upper",
                       cxMultiLineInputTextValidationNonBlankForceUpperCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Text validation auto-completion",
                       cxMultiLineInputTextValidationAutoCompletionCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Extended help key",
                       cxMultiLineInputExtendedHelpKeyCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Label to the right",
                       cxMultiLineInputWithRightLabelCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Auto-generate extended help",
                       cxMultiLineInputAutoGenerateHelpStringCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("setValue() with various input types",
                       cxMultiLineInputSetValueCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("cxMultiLineInput max input length",
                       cxMultiLineInputMaxInputLengthCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("cxMultiLineInput ext'd help colors and attrs",
                       cxMultiLineInputExtendedHelpColorsAndAttrsCode, "",
                       cxITEM_NORMAL, true);
   inputSubMenu.append("Overriding onKeypress() (floating-pt.)",
                       cxMultiLineInputOverrideOnKeypressCode, "",
                       cxITEM_NORMAL, true);

   // cxComboBox tester stuff
   cxMenu comboBoxSubMenu(nullptr, 1, 0, 0, 35, "Test cxComboBox Stuff");
   comboBoxSubMenu.setWrapping(true);
   comboBoxSubMenu.append("Combo box", comboBoxCode, "", cxITEM_NORMAL, true);
   comboBoxSubMenu.append("Disable menu", comboBoxMenuDisableCode, "", cxITEM_NORMAL, true);
   comboBoxSubMenu.append("Not editable but menu enabled",
                          cxComboBoxNotEditableMenuEnabledCode, "", cxITEM_NORMAL, true);

   // cxForm tester stuff
   cxMenu formSubMenu(nullptr, 1, 0, 0, 50, "Test cxForm Stuff");
   formSubMenu.setWrapping(true);
   formSubMenu.append("&Form", formCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Form with Fn keys", formWithFKeysCode, "", cxITEM_NORMAL,
                       true);
   formSubMenu.append("Form scrolling", stackedFormScrollingCode, "", cxITEM_NORMAL,
                       true);
   formSubMenu.append("Form scrolling 2", stackedFormScrollingCode2, "", cxITEM_NORMAL,
                       true);
   formSubMenu.append("Form input colors", formInputColorsCode, "", cxITEM_NORMAL,
                       true);
   formSubMenu.append("Borderless form", borderlessFormCode, "", cxITEM_NORMAL,
                       true);
   formSubMenu.append("testFormReadOnly", testFormReadOnlyCode, "", cxITEM_NORMAL,
                       true);
   formSubMenu.append("Form w/ input val. validator", formWithInputValueValidatorCode,
                       "", cxITEM_NORMAL, true);
   formSubMenu.append("Test form &positions", testFormPosCode,
                       "", cxITEM_NORMAL, true);
   formSubMenu.append("Form & menu input loop", formWithMenuCode,
                       "", cxITEM_NORMAL, true);
   formSubMenu.append("cxForm assignment", cxFormAssignmentCode,
                       "", cxITEM_NORMAL, true);
   formSubMenu.append("Validator function on form", setValFuncCode,"", cxITEM_NORMAL, true);
   formSubMenu.append("Form input jumping", formChangeInputFocusWhileModalCode, "",
                       cxITEM_NORMAL, true);
   formSubMenu.append("Hotkey to set all form inputs read-only",
                       allFormInputsGoReadOnlyCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Form with quit disabled", formNoQuitCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Combo box on form", comboBoxOnFormCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Input without border", inputWithoutBorderCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("cxForm hasChanged()", formDataChangedCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("cxForm disable/enable", enableDisableFormCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("cxMultiForm w/ disabled subform", multiFormWithDisabledSubformCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("cxMultiForm events", multiForm2Code, "", cxITEM_NORMAL, true);
   formSubMenu.append("cxForm with quit keys", inputQuitKeysOnFormCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Lone input w/ loop disabled on form",
                      loneInputWithLoopDisabledOnFormCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Function hotkey, no run onLeave", formFunctionKeyNoOnLeaveCode,
                      "", cxITEM_NORMAL, true);
   formSubMenu.append("Input w/ function hotkey, no run onLeave",
                      cxFormMultiLineInputFunctionKeyNoValidatorCode,
                      "", cxITEM_NORMAL, true);
   formSubMenu.append("Auto-exit hotkey on last input",
                      keyFunctionAutoExitOnLastFormInputCode, "",
                      cxITEM_NORMAL, true);
   formSubMenu.append("cxForm getNavKeys(), getNavKeyStrings(), & hasNavKey()",
                      cxFormNavKeysCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("cxForm always start @ first input",
                      cxFormStartAtFirstInputCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Clear all inputs except read-only",
                      cxFormClearOnlyEditableCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Mouse event function test",
                      cxFormMouseFunctionTestCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Extended help key",
                      cxFormExtendedHelpKeyCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("Inputs with right labels",
                      cxMultiLineInputWithRightLabelOnFormCode, "", cxITEM_NORMAL,
                      true);
   formSubMenu.append("&Item Sales", itemSalesCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("appendPair()", cxFormAppendPairCode, "", cxITEM_NORMAL, true);
   formSubMenu.append("appendComboBoxPair()", cxFormAppendComboBoxPairCode, "", cxITEM_NORMAL, true);

   // Message box tester stuff
   cxMenu msgSubMenu(nullptr, 1, 0, 0, 25, "Test Message Boxes");
   msgSubMenu.setWrapping(true);
   msgSubMenu.append("OK box",        msgOKCode, "", cxITEM_NORMAL, true);
   msgSubMenu.append("OK/Cancel box", msgOKCancelCode, "", cxITEM_NORMAL, true);
   msgSubMenu.append("Cancel box",    msgCancelCode, "", cxITEM_NORMAL, true);
   msgSubMenu.append("Yes box",       msgYesCode, "", cxITEM_NORMAL, true);
   msgSubMenu.append("Yes/No box",    msgYesNoCode, "", cxITEM_NORMAL, true);
   msgSubMenu.append("No/Yes box",    msgNoYesCode, "", cxITEM_NORMAL, true);
   msgSubMenu.append("No box",        msgNoCode, "", cxITEM_NORMAL, true);
   msgSubMenu.append("One",           msgBox1Code, "", cxITEM_NORMAL, true);
   msgSubMenu.append("Two",           msgBox2Code, "", cxITEM_NORMAL, true);
   msgSubMenu.append("Three",         msgBox3Code, "", cxITEM_NORMAL, true);
   msgSubMenu.append("Four",          msgBox4Code, "", cxITEM_NORMAL, true);
   msgSubMenu.append("Splash1",       splash1Code, "", cxITEM_NORMAL, true);
   msgSubMenu.append("Splash2",       splash2Code, "", cxITEM_NORMAL, true);

   // cxWindow tester stuff
   cxMenu windowSubMenu(nullptr, 1, 0, 3, 45, "Window tests");
   windowSubMenu.setWrapping(true);
   windowSubMenu.append("Window test", windowCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("Window with function keys", cxWindowWithFunctionKeysCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("Overlapped window refreshing", panelDemoCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("External title window",  extTitleWinCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("External status window", extStatusWinCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("Title text alignment",   titleAlignmentCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("Status text alignment",  statusAlignmentCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("cxWindow copy constructor", cxWindowCopyConstructorCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("cxWindow assignment operator", cxWindowAssignmentCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("Scrolled window", scrolledWindowCode, "", cxITEM_NORMAL, true);
   windowSubMenu.append("W/ multiple title strings", cxWindowWithMultipleTitleStringsCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("W/ multiple status strings", cxWindowWithMultipleStatusStringsCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("Function hotkey, no run onLeave",  windowFunctionKeyNoOnLeaveCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("Adding message lines manually",  addMessageLineCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("&Alignment tests",  cxWindowAlignCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("&Border tests",  cxWindowBorderCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("&Size tests",  cxWindowSizeCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("Attributes tests",  cxWindowAttributesCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("Mouse event function test", cxWindowMouseFunctionTestCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("Adding message lines above", cxWindowAddMessageLinesAboveCode,
                        "", cxITEM_NORMAL, true);
   windowSubMenu.append("Different border styles", cxWindowBorderStyles,
                        "", cxITEM_NORMAL, true);

   // cxPanel tester stuff
   cxMenu panelSubMenu(nullptr, 1, 0, 3, 49, "Panel tests");
   panelSubMenu.setWrapping(true);
   panelSubMenu.append("cxPanel test", cxPanelCode, "", cxITEM_NORMAL, true);
   panelSubMenu.append("cxPanel test 2", cxPanelCode2, "", cxITEM_NORMAL, true);
   panelSubMenu.append("cxPanel w/ no editable subwindows",
                       cxPanelWithNonEditableSubwindowsCode, "", cxITEM_NORMAL,
                       true);
   panelSubMenu.append("cxPanel swap() method", cxPanelWindowSwapCode, "",
                       cxITEM_NORMAL, true);
   panelSubMenu.append("cxPanel w/ cxMenu leaving on up arrow on item 1",
                       cxPanel_cxMenuUpArrowLeaveCode, "", cxITEM_NORMAL, true);
   panelSubMenu.append("cxPanel::getLastWindow()", cxPanelGetLastWindowCode,
                       "", cxITEM_NORMAL, true);
   panelSubMenu.append("cxPanel::delAllWindows()", cxPanelDelAllWindowsCode,
                       "", cxITEM_NORMAL, true);
   panelSubMenu.append("Panel with a form & a button", cxPanelWithButtonTestCode,
                       "", cxITEM_NORMAL, true);

   // cxSearchPanel tester stuff
   cxMenu searchPanelSubMenu(nullptr, 1, 0, 3, 49, "Panel tests");
   searchPanelSubMenu.setWrapping(true);
   searchPanelSubMenu.append("cxSearchPanel", cxSearchPanelTestCode, "",
                             cxITEM_NORMAL, true);
   searchPanelSubMenu.append("cxSearchPanel resize", cxSearchPanelResizeCode,
                             "", cxITEM_NORMAL, true);
   searchPanelSubMenu.append("Copy constructor", cxSearchPanelCopyConstructorCode,
                             "", cxITEM_NORMAL, true);

   // cxButton test stuff
   cxMenu buttonSubMenu(nullptr, 1, 0, 0, 29, "Test cxButton Stuff");
   buttonSubMenu.setWrapping(true);
   buttonSubMenu.append("onClick function", cxButtonOnClickCode, "", cxITEM_NORMAL, true);

   // cxNotebook test stuff
   cxMenu notebookSubMenu(nullptr, 1, 0, 0, 35, "Test cxNotebook Stuff");
   notebookSubMenu.append("Test 1", cxNotebookTest1Code, "", cxITEM_NORMAL, true);
   notebookSubMenu.append("Test 2", cxNotebookTest2Code, "", cxITEM_NORMAL, true);
   notebookSubMenu.append("Removing a panel", cxNotebookRemoveWindowTestCode,
                          "", cxITEM_NORMAL, true);
   notebookSubMenu.append("With empty panel", cxNotebookWithEmptyPanelTestCode,
                          "", cxITEM_NORMAL, true);
   notebookSubMenu.append("Changing the tab spacing", cxNotebookSetTabSpacingCode,
                          "", cxITEM_NORMAL, true);
   notebookSubMenu.append("Swapping the order of 2 panels", cxNotebookSwapTestCode,
                          "", cxITEM_NORMAL, true);

   // Create the main menu, add the submenus & menu items to it.
   cxMenu mainMenu(nullptr, 1, 0, 5, 25, "Main menu");
   mainMenu.setWrapping(true);
   mainMenu.appendWithPullRight("&Input", &inputSubMenu, "Test input stuff.", true);
   mainMenu.appendWithPullRight("&Combo box", &comboBoxSubMenu, "Test cxComboBox stuff.", true);
   mainMenu.appendWithPullRight("&Menus", &menuSubMenu, "Test cxMenu stuff", true);
   mainMenu.appendWithPullRight("&Forms", &formSubMenu, "Test cxForm stuff", true);
   mainMenu.appendWithPullRight("Message &Boxes", &msgSubMenu, "Test simple message boxes.", true);
   mainMenu.appendWithPullRight("&Window tests", &windowSubMenu, "Test cxWindow stuff", true);
   mainMenu.appendWithPullRight("&Panel tests", &panelSubMenu, "Test cxPanel stuff", true);
   mainMenu.appendWithPullRight("&Search panel tests", &searchPanelSubMenu,
                                "Test cxSearchPanel stuff", true);
   mainMenu.appendWithPullRight("B&utton tests", &buttonSubMenu, "Test cxButton stuff", true);
   mainMenu.appendWithPullRight("&Notebook tests", &notebookSubMenu, "Test cxNotebook stuff", true);
   mainMenu.append("&File viewer", fileViewerCode, "", cxITEM_NORMAL, true);
   mainMenu.append("&Date picker", cxDatePickerTestCode, "Show the cxDatePicker dialog", cxITEM_NORMAL, true);
   mainMenu.append("&Open file dialog", cxOpenFileDialogTestCode, "Show the cxOpenFileDialog", cxITEM_NORMAL, true);
   mainMenu.append("&Attributes", attributesSetterCode, "", cxITEM_NORMAL, true);
   mainMenu.append("E&xit", exitCode, "Exit this application", cxITEM_NORMAL, true);

   // Refresh the title & status lines (some of the menus created here overlap
   //  the title & status lines, and for for some reason, they seem to be
   //  erasing parts of those lines, and ncurses isn't redrawing those parts
   //  of the title & status after the menus are created (even though cxWindow
   //  objects are set hidden right after they're created).
   gTitleLine->show();
   gStatusLine->show();

   // Main menu loop
   bool continueOn = true;
   while (continueOn)
   {
      // Show the menu and take action depending on
      //  the user's choice.
      long userChoice = mainMenu.showModal();
      mainMenu.hide(); // So other windows appearing on top look OK
      switch (userChoice)
      {
         case cxID_QUIT:
            continueOn=false;
            break;
         case -1:
            cxBase::messageBox("-1");
            break;
         case menuCode:
            testMenu1();
            break;
         case menuItemWithMultipleHotkeyCode:
            menuItemWithMultipleHotkeys();
            break;
         case cxMenuItemHotkeysOffMenuCode:
            cxMenuItemHotkeysOffMenu();
            break;
         case inputCode:
            testInput1();
            break;
         case inputMaskingCode:
            cxInputMasking();
            break;
         case multiInputMaskingCode:
            cxMultiLineInputMasking();
            break;
         case inputLabelChangeCode:
            setInputLabel();
            break;
         case multiLineInputLabelChangeCode:
            setMultiLineInputLabel();
            break;
         case msgOKCode:
            msgOK();
            break;
         case msgOKCancelCode:
            msgOKCancel();
            break;
         case msgCancelCode:
            msgCancel();
            break;
         case msgYesCode:
            msgYes();
            break;
         case msgNoCode:
            msgNo();
            break;
         case msgYesNoCode:
            msgYesNo();
            break;
         case msgNoYesCode:
            msgNoYes();
            break;
         case msgBox1Code:
            msgBox1();
            break;
         case msgBox2Code:
            msgBox2();
            break;
         case msgBox3Code:
            msgBox3();
            break;
         case msgBox4Code:
            msgBox4();
            break;
         case splash1Code:
            splash1();
            break;
         case splash2Code:
            splash2();
            break;
         case windowCode:
            window1();
            break;
         case cxWindowWithFunctionKeysCode:
            cxWindowWithFunctionKeys();
            break;
         case panelDemoCode:
            demoPanels();
            break;
         case extTitleWinCode:
            externalTitleWindow();
            break;
         case extStatusWinCode:
            externalStatusWindow();
            break;
         case titleAlignmentCode:
            titleAlignment();
            break;
         case statusAlignmentCode:
            statusAlignment();
            break;
         case cxWindowCopyConstructorCode:
            cxWindowCopyConstructor();
            break;
         case cxWindowAssignmentCode:
            cxWindowAssignment();
            break;
         case scrolledWindowCode:
            scrolledWindow();
            break;
         case cxWindowWithMultipleTitleStringsCode:
            cxWindowWithMultipleTitleStrings();
            break;
         case cxWindowWithMultipleStatusStringsCode:
            cxWindowWithMultipleStatusStrings();
            break;
         case cxPanelCode:
            cxPanelTest();
            break;
         case cxPanelCode2:
            cxPanelTest2();
            break;
         case cxPanelWithNonEditableSubwindowsCode:
            cxPanelWithNonEditableSubwindows();
            break;
         case cxPanelDelAllWindowsCode:
            cxPanelDelAllWindowsTest();
            break;
         case cxPanelWithButtonTestCode:
            cxPanelWithButton();
            break;
         case fileViewerCode:
            fileViewer();
            break;
         case attributesSetterCode:
            attributesSetter();
            break;
         case singleLineInputCode:
            singleLineInput();
            break;
         case multiLineInputCode:
            multiLineInput();
            break;
         case formCode:
            form();
            break;
         case menuScrollingCode:
            cxMenuScrolling();
            break;
         case formWithInputValueValidatorCode:
            formWithInputValueValidator();
            break;
         case testFormPosCode:
            testFormPos();
            break;
         case formWithMenuCode:
            formWithMenu();
            break;
         case cxMenuAssignmentCode:
            cxMenuAssignment();
            break;
         case cxFormAssignmentCode:
            cxFormAssignment();
            break;
         case multiLineInputResizeCode:
            multiLineInputResize();
            break;
         case stackedFormScrollingCode:
            stackedFormScrolling();
            break;
         case stackedFormScrollingCode2:
            stackedFormScrolling2();
            break;
         case formInputColorsCode:
            formInputColors();
            break;
         case borderlessFormCode:
            borderlessForm();
            break;
         case inputWithFKeysCode:
            inputsWithFKeys();
            break;
         case inputsWithDifferentColorsCode:
            inputsWithDifferentColors();
            break;
         case formWithFKeysCode:
            formWithFKeys();
            break;
         case testFormReadOnlyCode:
            testFormReadOnly();
            break;
         case exitCode:
            continueOn = false;
            break;
         case setValFuncCode:
            setValFunc();
            break;
         case changeMenuItemTextCode:
            changeMenuItemText();
            break;
         case formChangeInputFocusWhileModalCode:
            formChangeInputFocusWhileModal();
            break;
         case allFormInputsGoReadOnlyCode:
            allFormInputsGoReadOnly();
            break;
         case formNoQuitCode:
            cxFormNoQuit();
            break;
         case menuNoQuitCode:
            cxMenuNoQuit();
            break;
         case cxMenuWithMultipleItemHotkeysCode:
            cxMenuWithMultipleItemHotkeys();
            break;
         case cxMenuNoMoreSelectableItemsWhileModalCode:
            cxMenuNoMoreSelectableItemsWhileModal();
            break;
         case cxMenuWithDuplicateHotkeysCode:
            cxMenuWithDuplicateHotkeys();
            break;
         case cxMenuResizeCode:
            cxMenuResize();
            break;
         case cxWindowAlignCode:
            cxWindowAlignTest();
            break;
         case cxNotebookTest1Code:
            cxNotebookTest1();
            break;
         case cxNotebookTest2Code:
            cxNotebookTest2();
            break;
         case cxNotebookRemoveWindowTestCode:
            cxNotebookRemoveWindowTest();
            break;
         case cxNotebookWithEmptyPanelTestCode:
            cxNotebookWithEmptyPanelTest();
            break;
         case cxNotebookSetTabSpacingCode:
            cxNotebookSetTabSpacing();
            break;
         case cxNotebookSwapTestCode:
            cxNotebookSwapTest();
            break;
         case cxWindowBorderCode:
            cxWindowBorderTest();
            break;
         case cxWindowSizeCode:
            cxWindowSizeTest();
            break;
         case cxWindowAttributesCode:
            cxWindowAttributesTest();
            break;
         case cxWindowMouseFunctionTestCode:
            cxWindowMouseFunctionTest();
            break;
         case cxMenuMiscTestsCode:
            cxMenuMiscTests();
            break;
         case cxPanelWindowSwapCode:
            cxPanelWindowSwap();
            break;
         case cxPanel_cxMenuUpArrowLeaveCode:
            cxPanel_cxMenuUpArrowLeave();
            break;
         case cxPanelGetLastWindowCode:
            cxPanelGetLastWindow();
            break;
         case cxSearchPanelTestCode:
            cxSearchPanelTest();
            break;
         case cxSearchPanelResizeCode:
            cxSearchPanelResize();
            break;
         case cxSearchPanelCopyConstructorCode:
            cxSearchPanelCopyConstructor();
            break;
         case cxFormClearOnlyEditableCode:
            cxFormClearOnlyEditable();
            break;
         case cxInputBorderChangeCode:
            cxInputBorderChange();
            break;
         case cxMultiLineInputBorderChangeCode:
            cxMultiLineInputBorderChange();
            break;
         case comboBoxCode:
            comboBox();
            break;
         case comboBoxOnFormCode:
            comboBoxOnForm();
            break;
         case comboBoxMenuDisableCode:
            comboBoxMenuDisable();
            break;
         case inputWithoutBorderCode:
            inputWithoutBorder();
            break;
         case formDataChangedCode:
            formDataChanged();
            break;
         case integerValidateCode:
            integerValidate();
            break;
         case floatingPtValidateCode:
            floatingPtValidate();
            break;
         case enableDisableFormCode:
            enableDisableForm();
            break;
         case multiFormWithDisabledSubformCode:
            multiFormWithDisabledSubform();
            break;
         case multiForm2Code:
            multiForm2();
            break;
         case inputQuitKeysOnFormCode:
            inputQuitKeysOnForm();
            break;
         case cxMenuFocusFunctionsCode:
            cxMenuFocusFunctions();
            break;
         case loneInputWithLoopDisabledOnFormCode:
            loneInputWithLoopDisabledOnForm();
            break;
         case windowFunctionKeyNoOnLeaveCode:
            windowFunctionKeyNoOnLeave();
            break;
         case addMessageLineCode:
            addMessageLine();
            break;
         case formFunctionKeyNoOnLeaveCode:
            formFunctionKeyNoOnLeave();
            break;
         case cxMultiLineInputFunctionKeyNoValidatorCode:
            cxMultiLineInputFunctionKeyNoValidator();
            break;
         case cxMultiLineInputNumericFloatingPtCode:
            cxMultiLineInputNumericFloatingPt();
            break;
         case cxMultiLineInputNumericFloatingPtWithRangeCode:
            cxMultiLineInputNumericFloatingPtWithRange();
            break;
         case cxMultiLineInputNumericWholeCode:
            cxMultiLineInputNumericWhole();
            break;
         case cxMultiLineInputNumericWholeWithRangeCode:
            cxMultiLineInputNumericWholeWithRange();
            break;
         case cxMultiLineInputTextValidationCode:
            cxMultiLineInputTextValidation();
            break;
         case cxMultiLineInputForceUpperCode:
            cxMultiLineInputForceUpper();
            break;
         case cxMultiLineInputNonBlankCode:
            cxMultiLineInputNonBlank();
            break;
         case cxMultiLineInputTextValidationNonBlankForceUpperCode:
            cxMultiLineInputTextValidationNonBlankForceUpper();
            break;
         case cxMultiLineInputTextValidationAutoCompletionCode:
            cxMultiLineInputTextValidationAutoCompletion();
            break;
         case cxMultiLineInputExtendedHelpKeyCode:
            cxMultiLineInputExtendedHelpKey();
            break;
         case cxFormExtendedHelpKeyCode:
            cxFormExtendedHelpKey();
            break;
         case cxFormMultiLineInputFunctionKeyNoValidatorCode:
            cxFormMultiLineInputFunctionKeyNoValidator();
            break;
         case keyFunctionAutoExitOnLastFormInputCode:
            keyFunctionAutoExitOnLastFormInput();
            break;
         case cxFormNavKeysCode:
            cxFormNavKeys();
            break;
         case cxFormStartAtFirstInputCode:
            cxFormStartAtFirstInput();
            break;
         case cxButtonOnClickCode:
            cxButtonOnClick();
            break;
         case cxFormMouseFunctionTestCode:
            cxFormMouseFunctionTest();
            break;
         case cxMenuMouseFunctionTestCode:
            cxMenuMouseFunctionTest();
            break;
         case cxMultiLineInputWithRightLabelCode:
            cxMultiLineInputWithRightLabel();
            break;
         case cxMultiLineInputAutoGenerateHelpStringCode:
            cxMultiLineInputAutoGenerateHelpString();
            break;
         case cxMultiLineInputWithRightLabelOnFormCode:
            cxMultiLineInputWithRightLabelOnForm();
            break;
         case itemSalesCode:
            itemSales();
            break;
         case cxFormAppendPairCode:
            cxFormAppendPair();
            break;
         case cxFormAppendComboBoxPairCode:
            cxFormAppendComboBoxPair();
            break;
         case cxMultiLineInputSetValueCode:
            cxMultiLineInputSetValue();
            break;
         case cxMultiLineInputMaxInputLengthCode:
            cxMultiLineInputMaxInputLength();
            break;
         case cxMultiLineInputExtendedHelpColorsAndAttrsCode:
            cxMultiLineInputExtendedHelpColorsAndAttrs();
            break;
         case cxMenuAltItemTextCode:
            cxMenuAltItemText();
            break;
         case cxComboBoxNotEditableMenuEnabledCode:
            cxComboBoxNotEditableMenuEnabled();
            break;
         case cxMultiLineInputOverrideOnKeypressCode:
            cxMultiLineInputOverrideOnKeypress();
            break;
         case cxWindowAddMessageLinesAboveCode:
            cxWindowAddMessageLinesAbove();
            break;
         case cxWindowBorderStyles:
            cxWindowVariousBorderStyles();
            break;
         case cxDatePickerTestCode:
            cxDatePickerTest();
            break;
         case cxOpenFileDialogTestCode:
            cxOpenFileDialogTest();
            break;
         case someItem: // do nuthin...
         default: // do nuthin...
            break;
      }
   }
}

void msgOK()
{
   cxMessageDialog a(nullptr, 12, 12, 10, 20, "I am ok", "This is a test.", cxOK, "Status");
   if (a.showModal() == cxID_OK)
   {
      cxBase::messageBox("You chose OK");
   }
   else
   {
      cxBase::messageBox("You chose Cancel");
   }
}

void msgOKCancel()
{
   cxMessageDialog a(nullptr, 12, 12, 10, 20, "Somethin...", "This is a test.", cxOK|cxCANCEL, "Status");
   if (a.showModal() == cxID_OK)
   {
      cxBase::messageBox("You chose OK");
   }
   else
   {
      cxBase::messageBox("You chose Cancel");
   }
}

void msgCancel()
{
   cxMessageDialog a(nullptr, 12, 12, 10, 20, "Somethin...", "Going to cancel...", cxCANCEL, "Status");
   if (a.showModal() == cxID_OK)
   {
      cxBase::messageBox("You chose OK");
   }
   else
   {
      cxBase::messageBox("You chose Cancel");
   }
}

void msgYes()
{
   cxMessageDialog a(nullptr, 12, 12, 10, 20, "Answer!", "This is a test.", cxYES, "Status");
   if (a.showModal() == cxID_OK)
   {
      cxBase::messageBox("You chose OK");
   }
   else
   {
      cxBase::messageBox("You chose Cancel");
   }
}

void msgYesNo()
{
   cxMessageDialog a(nullptr, 12, 12, 10, 20, "Answer!", "Is this a good test? (defaults to YES)", cxYES|cxNO, "Status");
   //cxBase::dump("msgYesNo.scn");
   if (a.showModal() == cxID_OK)
   {
      cxBase::messageBox("You chose OK");
   }
   else
   {
      cxBase::messageBox("You chose Cancel");
   }
}

void msgNoYes()
{
   cxMessageDialog a(nullptr, 12, 12, 10, 20, "Answer!", "Is this a good test?(defaults to NO)", cxYES|cxNO, "Status");
   a.setFocus(a.getCancelButton());
   //noButton->setFocus(false, false, false);
   if (a.showModal() == cxID_OK)
   {
      cxBase::messageBox("You chose OK");
   }
   else
   {
      cxBase::messageBox("You chose Cancel");
   }
}

void msgNo()
{
   cxMessageDialog a(nullptr, 2, 2, 10, 20, "Answer!", "Is this a bad test?", cxNO, "Status");
   if (a.showModal() == cxID_OK)
   {
      cxBase::messageBox("You chose OK");
   }
   else
   {
      cxBase::messageBox("You chose Cancel");
   }
}

void testInput1()
{
   // Test cxInput stuff
   cxInput anInput(nullptr, 1, 20, 23, "Name:", eBS_SINGLE_LINE);
   anInput.setExitOnFull(true);
   anInput.addAttr(eLABEL, A_UNDERLINE);
   anInput.addAttr(eDATA_EDITABLE, A_UNDERLINE);
   anInput.showModal();
   anInput.removeAttrs(eLABEL);
   anInput.removeAttrs(eDATA_EDITABLE);
   anInput.move(5, 5);
   anInput.showModal();

   ostringstream os;
   os << " You entered:" << anInput.getValue() << ", Input length:"
      << anInput.getValue().length();
   messageBox(os.str());
}

void testMenu1()
{
   // Test cxMenu stuff
   const int item1Code = cxFIRST_AVAIL_RETURN_CODE;
   const int item3Code = cxFIRST_AVAIL_RETURN_CODE+1;
   const int item4Code = cxFIRST_AVAIL_RETURN_CODE+2;
   const int item5Code = cxFIRST_AVAIL_RETURN_CODE+3;
   const int item6Code = cxFIRST_AVAIL_RETURN_CODE+4;
   const int item8Code = cxFIRST_AVAIL_RETURN_CODE+5;
   const int item9Code = cxFIRST_AVAIL_RETURN_CODE+6;
   const int item10Code = cxFIRST_AVAIL_RETURN_CODE+7;
   cxMenu subMenu(nullptr, 0, 0, 15, 15, "Submenu");
   subMenu.append("Item 4", item4Code);
   subMenu.append("Item 5", item5Code);
   subMenu.append("Item 6", item6Code);
   cxMenu subMenu2(nullptr, 0, 0, 15, 15, "Submenu2");
   subMenu2.append("Item 8", item8Code);
   subMenu2.append("Item 9", item9Code);
   subMenu2.append("Item 10", item10Code);
   subMenu.appendWithPullRight("Item 7", &subMenu2);
   cxMenu aMenu(nullptr, 1, 0, 12, 25, "Main menu");
   aMenu.append("Item 1", item1Code, "Help");
   aMenu.appendWithPullRight("Item 2", &subMenu);
   //aMenu.append("12345678901234567890123456789012345678901234567890123456789012345678901234567890zyxfqa", item3Code, "Long help text");
   aMenu.append("Item 3", item3Code, "Long help text");
   aMenu.append("Very long item text", 17, "Help!");
   aMenu.append("", 18, "Help!!!");
   mvwprintw(stdscr, 0, 15, "There are %d menu items.", aMenu.numSubWindows());
   wrefresh(stdscr);
   //aMenu.move(2, 75);
   int returnCode = aMenu.showModal();
   cxBase::messageBox("itemWasSelected() returns: " +
                      string(aMenu.itemWasSelected() ? "true" : "false"));
   // Display which menu item the user chose
   std::ostringstream os;
   switch (returnCode)
   {
      case cxID_QUIT:
         os << "cxID_QUIT";
         mvwprintw(stdscr, 3, 15, "Return code: cxID_QUIT");
         break;
      case cxID_EXIT:
         os << "CXID_EXIT";
         mvwprintw(stdscr, 3, 15, "Return code: cxID_EXIT");
         break;
      case item1Code:
         os << "item 1";
         break;
      case item3Code:
         os << "item 3";
         break;
      case item4Code:
         os << "item 4";
         break;
      case item5Code:
         os << "item 5";
         break;
      case item6Code:
         os << "item 6";
         break;
      case item8Code:
         os << "item 8";
         break;
      case item9Code:
         os << "item 9";
         break;
      case item10Code:
         os << "item 10";
         break;
      default:
         os << "return code 18";
         break;
   }
   messageBox(aMenu.bottom()+2, 5, "Your choice", "You chose " + os.str(), "");
}

void testInput2()
{
   // Note: Moving a window doesn't move its subwindows, so
   //  if you move a cxInput's parent window, you'll have to move
   //  the cxInput too.
   cxWindow aWindow(nullptr, 10, 10, 5, 40, "Info form", "", "Form test");
   cxInput nameInput(&aWindow, 1, 1, 38, "Name:");
   cxInput ageInput(&aWindow, 2, 1, 38, "Age:");
   cxInput locationInput(&aWindow, 3, 1, 38, "Location:");
   //aWindow.move(2, 5);
   //nameInput.move(3, 6);
   //ageInput.move(4, 6);
   //locationInput.move(5, 6);
   aWindow.show();
   nameInput.showModal();
   nameInput.show();
   ageInput.showModal();
   ageInput.show();
   locationInput.showModal();
   locationInput.show();
   mvwprintw(stdscr, 17, 0, "    Your name: %s", nameInput.getValue().c_str());
   mvwprintw(stdscr, 18, 0, "     Your age: %s", ageInput.getValue().c_str());
   mvwprintw(stdscr, 19, 0, "Your location: %s", locationInput.getValue().c_str());
   wrefresh(stdscr);
   getch();
   wclear(stdscr);
   wrefresh(stdscr);
}

void window1()
{
   cxWindow aWindow(nullptr, 0, 0, 5, 20, "Test", "This is a test", "");
   aWindow.show();
   getch();
}

void msgBox1()
{
   // Note: This message box is only 1 line, so the only
   //  thing that shows up is the "DEF".
   messageBox(10, 10, 1, 2, "ABC", "DEF", "GHI", eBS_NOBORDER);
}

void msgBox2()
{
   messageBox(10, 10, 3, 8, "ABC", "DEF", "GHI");
}
void msgBox3()
{
   messageBox("Test", "This is a pretty good test.", "status");
}

void msgBox4()
{
   messageBox(0, 0, 22, 60, "Test", "Test");
}

void splash1()
{
   splash("Test1", "Just testing...", 3);
}

void splash2()
{
   splash("More testing...", 3);
}

void fileViewer()
{
   //cxFileViewer aFileViewer(nullptr, "/etc/hosts", 2, 2, 20, 75);
   cxFileViewer aFileViewer(nullptr, "cxMenu.h", 2, 2, 20, 75);
   aFileViewer.showModal();
}

// MIKE
void attributesSetter()
{
   int topRow=2;
   int leftCol=2;
   int rightCol=38;

   // sample menu for the user to see their changes "real-time"
   cxWindow iWindow(nullptr, leftCol, rightCol, 4, 40, "Sample window title");
   iWindow.setMessage("Sample window message.");
   iWindow.setStatus("Sample window status");
   iWindow.show();

   // sample for for the user to see their changes "real-time"
   cxForm iForm(nullptr, 7, rightCol, 5, 40, "Sample form title");
   iForm.append(1, 1, 1, 30, "La&bel1:");
   iForm.append(2, 1, 1, 30, "Label2:", "", "", eINPUT_READONLY);
   iForm.setValue("La&bel1:", "Some data");
   iForm.setValue("Label2:", "Read only");
   iForm.setStatus("Sample form status");
   iForm.show();

   attr_t tmpAttribute;

   // which element?
   cxMenu elementMenu(nullptr, topRow, leftCol, 11, 35, "Menu");
   elementMenu.append("&Message",        1, "Change the message style.");
   elementMenu.append("&Title",          2, "Change the title style.");
   elementMenu.append("&Status",         3, "Change the status style.");
   elementMenu.append("&Border",         4, "Change the border style.");
   elementMenu.append("&Hotkey",         5, "Change the hotkey style.");
   elementMenu.append("&Label",          6, "Change the data label style.");
   elementMenu.append("&Editable",       7, "Change the editable data style.");
   elementMenu.append("&Readonly",       8, "Change the readonly data style.");
   elementMenu.append("&Menu Selection", 9, "Change the menu selection style.");
   long elementChoice = elementMenu.showModal();

   int leftRow=elementMenu.numMenuItems()+2; // don't forget the border

   while(elementChoice != cxID_QUIT)
   {
      // which attribute?
      cxMenu attributeMenu(nullptr, leftRow + topRow + 1, leftCol, 8, 15, "Attribute Menu");
      attributeMenu.append("&Normal",   1, "Normal");
      attributeMenu.append("&Dim",      2, "Dim");
      attributeMenu.append("&Bold",     3, "Bold");
      attributeMenu.append("&Standout", 4, "Standout");
      attributeMenu.append("&Reverse",  5, "Reverse");
      attributeMenu.append("&Underline",6, "Underline");
      long attributeChoice = attributeMenu.showModal();

      if (attributeChoice != cxID_QUIT)
      {
         // figure out which attribute to use
         switch(attributeChoice)
         {
            case 1:
               tmpAttribute=A_NORMAL;
               break;
            case 2:
               tmpAttribute=A_DIM;
               break;
            case 3:
               tmpAttribute=A_BOLD;
               break;
            case 4:
               tmpAttribute=A_STANDOUT;
               break;
            case 5:
               tmpAttribute=A_REVERSE;
               break;
            case 6:
               tmpAttribute=A_UNDERLINE;
               break;
            default:
               tmpAttribute=A_NORMAL;
               break;
         }

         // figure out which element to change
         e_WidgetItems item = eMESSAGE;
         switch(elementChoice)
         {
            case 1:
               item = eMESSAGE;
               break;
            case 2:
               item = eTITLE;
               break;
            case 3:
               item = eSTATUS;
               break;
            case 4:
               item = eBORDER;
               break;
            case 5:
               item = eHOTKEY;
               break;
            case 6:
               item = eLABEL;
               break;
            case 7:
               item = eDATA_EDITABLE;
               break;
            case 8:
               item = eDATA_READONLY;
               break;
            case 9:
               item = eMENU_SELECTION;
               break;
         }
         iWindow.setAttr(item, tmpAttribute);
         iForm.setAttr(item, tmpAttribute);
         cxBase::setAttr(item, tmpAttribute);
         iWindow.show();
         iForm.show();
      }
      attributeMenu.hide();
      elementChoice = elementMenu.showModal(); // allow them to choose again
   }
} // attributeSetter

void singleLineInput()
{
   cxInput input(nullptr, 0, 45, 23, "Number:", eBS_NOBORDER, eINPUT_EDITABLE, true);
   input.setValidator("ddd");
   input.showModal();
   mvwprintw(stdscr, 8, 0, "Number entered:%s:", input.getValue().c_str());
   mvwprintw(stdscr, 9, 0, "validator:%s:", input.getValidatorStr().c_str());
   wrefresh(stdscr);
   getch();
   mvwprintw(stdscr, 8, 0, "             ");
}

void multiLineInput()
{
   cxMultiLineInput input(nullptr, 0, 45, 5, 23, "Text:");
   //cxMultiLineInput input(nullptr, 0, 45, 5, 23, "Text:", eBS_SINGLE_LINE);
   input.showModal();
   mvwprintw(stdscr, 8, 0, "You typed:%s:", input.getValue().c_str());
   wrefresh(stdscr);
   getch();
   mvwprintw(stdscr, 8, 0, "             ");
   wrefresh(stdscr);
}

void form()
{
   cxForm iForm(nullptr, 2, 2, 7, 40, "Test form");
   iForm.append(1, 1, 1, 9, "&Input 1:");
   iForm.append(2, 1, 1, 18, "I&nput 2:", "", "", eINPUT_READONLY);
   iForm.setCanBeEditable(1, false);
   iForm.append(3, 1, 1, 18, "In&put 3:", "", "Testing the field highlighting");
   iForm.append(4, 12, 1, 7, "Test4:", "", "Testing the Y/N validation.");
   iForm.setOnKeyFunction(3, inputYesNo, iForm.getInput(3).get(), nullptr, nullptr, nullptr);
   iForm.append(5, 12, 1, 7, "Test5:", "", "Testing single character field.");
   iForm.append(0, 12, 1, 10, "Test:", "", "Field in the border");
   iForm.getInput(2)->setAttr(eDATA_EDITABLE, A_STANDOUT);
   iForm.setValue("Input 1:", "Blah");
   iForm.setValue("Input 2:", "read_only...");
   iForm.setAutoExit(true);
   messageBox("First editable input index: " + cxStringUtils::toString(iForm.firstEditableInput()));
   messageBox("Last editable input index: " + cxStringUtils::toString(iForm.lastEditableInput()));
   long retval = iForm.showModal(true, false, false);
   switch(retval)
   {
      case cxID_EXIT:
         messageBox("showModal() returned cxID_EXIT");
         break;
      case cxID_QUIT:
         messageBox("showModal() returned cxID_QUIT");
         break;
      case ENTER:
      case KEY_ENTER:
         messageBox("showModal() returned ENTER");
         break;
      case ESC:
         messageBox("showModal() returned ESC");
         break;
      default:
         messageBox("showModal() returned " + cxStringUtils::toString(retval));
         break;
   }
   messageBox("Input 1:" + iForm.getValue("Input 1:"));
   messageBox("Input 2:" + iForm.getValue("Input 2:"));
   messageBox("Input 3:" + iForm.getValue("Input 3:"));
   messageBox("Test4:" + iForm.getValue("Test4:"));
}

void cxMenuScrolling()
{
   // Test cxMenu scrolling (also test hotkeys)
   cxMenu subMenu(nullptr, 2, 2, 4, 8, "Submenu");
   subMenu.append("Sub item 1", -1);
   subMenu.append("Sub item 2", -1);
   cxMenu iMenu(nullptr, 1, 10, 7, 12, "Test menu");
   iMenu.append("&Item 1", 1, "", cxITEM_NORMAL, false);
   iMenu.append("I&tem 2", 2, "", cxITEM_NORMAL, false);
   iMenu.appendWithPullRight("Item 3", &subMenu, "");
   iMenu.append("It&em 4", 4, "", cxITEM_NORMAL, false);
   for (long i = 5; i <= 30; ++i)
   {
      iMenu.append("Item " + cxStringUtils::toString(i), i, "", cxITEM_NORMAL, false);
   }
   //for (int i = 1; i < 500; i++) {
   //   iMenu.append("Item foo", -1, "", cxITEM_NORMAL);
   //}
   //iMenu.append("bottom", -1, "", cxITEM_NORMAL);
   //iMenu.move(2, 2);
   iMenu.setTopItem("Item 4");
   long retCode = iMenu.showModal();
   cxBase::messageBox("You chose item " + cxStringUtils::toString(retCode));
}

void inputsWithFKeys()
{
   // Test function key stuff for inputs

   messageBox("F2, F3, and '/' are hotkeys for this input.");

   int x = 2;
   int y = 73;

   // We can use an anonymous (lambda) function as a cxFunction, as long as the lambda
   // function doesn't capture anything in its capture clause.
   /*
   auto func = [](int& int1, int& int2) -> string {
      ostringstream os;
      os << "1: " << int1 << ", 2: " << int2;
      messageBox(2, 31, 3, 18, "Test", os.str());
      return("hello");
   };
   */
   // If the function is a named function (not an anonymous/lambda function), we could do this:
   //auto keyFunc1 = cxFunction2RefTemplated<int, int>::create(someFunction, x, y);
   // With a lambda function, we can simply use the name of the function, as we would with
   // any function:
   //auto keyFunc1 = cxFunction2RefTemplated<int, int>::create(func, x, y);
   // We can also specify a lambda function directly as the first argument to create():
   auto keyFunc1 = cxFunction2RefTemplated<int, int>::create([](int& int1, int& int2) -> string {
      ostringstream os;
      os << "1: " << int1 << ", 2: " << int2;
      messageBox(2, 31, 3, 18, "Test", os.str());
      return("hello");
   }, x, y);

   cxMultiLineInput iInput(nullptr, 1, 0, 1, 20, "Prompt:");
   iInput.setKeyFunction(KEY_F(2), keyFunc1);
   // Use 2 keys to run the same function
   iInput.setKeyFunction(KEY_F(3), make_shared<cxFunction0>(returnBBye, true));
   auto inputFunc = cxFunction1RefTemplated<cxMultiLineInput>::create(someFunction2, iInput, false, false, true);
   iInput.setKeyFunction('/', inputFunc);
   iInput.showModal();
   messageBox(iInput.getValue());
}

void formWithFKeys()
{
   // Test a form w/ field functions.
   messageBox("F2 and '/' are hotkeys for the 'Name' field, and F3 is a hotkey for the 'City' field.  F3 is a hotkey for the form (the form won't quit afterwards), and F4 is another hotkey for the form (the form will quit afterwards).");

   int x = 2;
   int y = 73;

   cxForm iForm(nullptr, 0, 0, 7, 30, "Test form");
   iForm.append(1, 1, 2, 15, "Name:");
   iForm.append(3, 1, 1, 25, "City:");

   auto keyFunc1 = cxFunction2RefTemplated<int, int>::create(someFunction, x, y);
   auto keyFunc1UseReturnVal = cxFunction2RefTemplated<int, int>::create(someFunction, x, y, true);

   iForm.setFieldKeyFunction("Name:", KEY_F(2), keyFunc1);
   iForm.setFieldKeyFunction("Name:", '/', keyFunc1);
   iForm.setFieldKeyFunction("City:", KEY_F(3), make_shared<cxFunction0>(returnBBye, true));
   iForm.setKeyFunction(KEY_F(4), keyFunc1);
   iForm.setKeyFunction(KEY_F(5), keyFunc1UseReturnVal);
   iForm.setOnLeaveFunction(make_shared<cxFunction0>(genericMessageFunction));
   iForm.showModal();

   messageBox("Name:" + iForm.getValue("Name:"));
   messageBox("City:" + iForm.getValue("City:"));
} // formWithFKeys

void cxInputMasking()
{
   // Test input masking for cxInput
   cxInput iInput(nullptr, 5, 30, 30, "Password:");
   iInput.toggleMasking(true);
   //iInput.setValue("12345");
   //cxBase::messageBox("Value before showModal:" + iInput.getValue() + ":");
   iInput.showModal();

   messageBox("You entered:" + iInput.getValue() + ":");
}

void cxMultiLineInputMasking()
{
   // Test input masking for cxMultiLineInput
   cxMultiLineInput iInput(nullptr, 5, 30, 2, 30, "Password:");
   iInput.toggleMasking(true);
   iInput.showModal();

   messageBox("You entered:" + iInput.getValue() + ":");
} // cxMultiLineInputMasking

string someFunction(int& int1, int& int2)
{
   ostringstream os;
   os << "1: " << int1 << ", 2: " << int2;
   messageBox(2, 31, 3, 18, "Test", os.str());
   return("hello");
} // someFunction

string returnBBye()
{
   return("b'bye");
} // returnBBye

string someFunction2(cxMultiLineInput& input)
{
   input.setValue("Yep", true);
   return "";
} // someFunction2

string someFunction3(cxMultiLineInput& input)
{
   string retval;

   if (input.getValue() != "good")
   {
      retval = "Value is not 'good'";
      messageBox(retval);
   }

   return(retval);
} // someFunction3

string hello()
{
   cxBase::messageBox("hello");
   return("");
} // hello

string genericMessageFunction()
{
   messageBox("This is a generic message.");
   return("");
}

string genericMessageFunction_1str(string& pMessage)
{
   messageBox(pMessage);
   return("");
} // genericMessageFunction

void cxWindowMoveRelative()
{
   cxWindow iWindow(nullptr, 0, 0, 10, 30, "Test");
   iWindow.show();
   getch();
   iWindow.moveRelative(1, 0, true);
   //iWindow.show(false, true, true);
   getch();
   iWindow.moveRelative(0, 1, true);
   getch();
   iWindow.moveRelative(-1, 0, true);
   getch();
   iWindow.moveRelative(0, -1, true);
   getch();
}

void stackedFormScrolling()
{
   cxForm iForm(nullptr, 5, 1, 20, 30, "Test form", eBS_SINGLE_LINE, nullptr, nullptr, false, true);
   for (unsigned i = 0; i < 24; ++i)
   {
      ostringstream os;
      os << "Item " << i + 1 << ":";
      iForm.append(1, 0, os.str());
   }
   iForm.showModal();
}

void externalTitleWindow()
{
   // Tests the ability to change a window's
   //  external title window (the window
   //  should move its status to the external
   //  window)
   cxMenu iMenu(nullptr, 1, 1, 15, 10, "Menu");
   iMenu.setStatus("Test menu");
   iMenu.append("Item 1", 0, "Help", cxITEM_NORMAL, false);
   iMenu.showModal();
   //cxWindow iWindow(nullptr, 0, 0, 1, 20, "", "", "", eBS_NOBORDER);
   //iMenu.setExtTitleWindow(&iWindow);
   iMenu.setExtTitleWindow(gTitleLine.get());
   iMenu.showModal();
}

void externalStatusWindow()
{
   // Tests the ability to change a window's
   //  external status window (the window
   //  should move its status to the external
   //  window)
   cxMenu iMenu(nullptr, 1, 1, 15, 10, "Menu");
   iMenu.setStatus("Test menu");
   iMenu.append("Item 1", 0, "Help", cxITEM_NORMAL, false);
   iMenu.showModal();
   cxWindow iWindow(nullptr, 20, 0, 1, 20, "", "", "", eBS_NOBORDER);
   iMenu.setExtStatusWindow(&iWindow);
   //iMenu.setExtStatusWindow(gStatusLine);
   iMenu.showModal();
}

void setInputLabel()
{
   cxInput iInput(nullptr, 15, 0, 20, "Test:");
   iInput.showModal();
   cxBase::messageBox(iInput.getValue());
   //iInput.show();
   //getch();
   iInput.setLabel("New label:");
   iInput.showModal();
   cxBase::messageBox(iInput.getValue());
   //iInput.show();
   //getch();
}

void setMultiLineInputLabel()
{
   cxMultiLineInput iInput(nullptr, 15, 0, 3, 20, "Test:");
   iInput.show();
   getch();
   iInput.setLabel("New label:");
   iInput.show();
   getch();
}

void titleAlignment()
{
   // TODO: This code does not work
   cxWindow iWindow(nullptr, 1, 1, 5, 40, "Title", "Message", "Status");
   iWindow.show();
   sleep(1);
   iWindow.setHorizTitleAlignment(eHP_CENTER);
   iWindow.setTitle("Center");
   iWindow.show();
   sleep(1);
   iWindow.setHorizTitleAlignment(eHP_RIGHT);
   iWindow.setTitle("Right");
   iWindow.show();
   sleep(1);
   iWindow.setHorizTitleAlignment(eHP_CENTER);
   iWindow.setTitle("Center");
   iWindow.show();
   sleep(1);
   iWindow.setHorizTitleAlignment(eHP_LEFT);
   iWindow.setTitle("Left");
   iWindow.show();
   sleep(1);
} // titleAlignment

void statusAlignment()
{
   // TODO: This code does not work
   cxWindow iWindow(nullptr, 1, 1, 5, 40, "Title", "Message", "Status");
   iWindow.show();
   iWindow.setStatus("Center");
   sleep(1);
   iWindow.setHorizStatusAlignment(eHP_CENTER);
   iWindow.show();
   sleep(1);
   iWindow.setHorizStatusAlignment(eHP_RIGHT);
   iWindow.setStatus("Right");
   iWindow.show();
   sleep(1);
   iWindow.setHorizStatusAlignment(eHP_CENTER);
   iWindow.setStatus("Center");
   iWindow.show();
   sleep(1);
   iWindow.setHorizStatusAlignment(eHP_LEFT);
   iWindow.setStatus("Left");
   iWindow.show();
   sleep(1);
} // statusAlignment

// For use as a cxWindow key function (used in cxWindowWithFunctionKeys())
string sayHello(void *p1, void *p2)
{
   string message = "Hello";
   if (p1 != nullptr)
   {
      cxWindow *pWindow = static_cast<cxWindow*>(p1);
      message += " (" + pWindow->getName() + ")";
   }
   messageBox(message);
   return("");
} // sayHello
string sayHello2(void *p1, void *p2)
{
   string message = "Hello 2";
   if (p1 != nullptr)
   {
      cxWindow *pWindow = static_cast<cxWindow*>(p1);
      message += " (" + pWindow->getName() + ")";
   }
   messageBox(message);
   return("");
} // sayHello

void cxWindowCopyConstructor()
{
   string message = "This tests the cxWindow copy constructor.  " \
                    "If the app doesn't crash and the " \
                    "function keys do what they're supposed to do, this test passes!";
   cxBase::messageDialog(message);

   // Create the original window dynamically, and delete it after it's copied.
   //  This way, we can truly see if the copy constructor works okay.  If it
   //  does, the program shouldn't crash.
   shared_ptr<cxWindow> window1 = make_shared<cxWindow>(nullptr, 1, 1, 5, 40, "Title",
                                    "F1 and F2 fire functions.", "Status");
   window1->setName("The window");
   window1->setKeyFunction(KEY_F(1), sayHello, window1.get(), nullptr, false, false, false);
   window1->setKeyFunction(KEY_F(2), sayHello2, window1.get(), nullptr, false, false, false);
   cxWindow window2(*window1);
   window2.showModal();
}

void demoPanels()
{
   // This function tests displaying & moving multiple
   // windows, showing the benefits that the panels library
   // gives us (i.e., only redrawing what is needed when
   // something changes, rather than redrawing the entire
   // screen).
   cxWindow iWindow(nullptr, 1, 1, 10, 25, "Win1", "Window 1");
   cxWindow iWindow2(nullptr, 3, 3, 10, 25, "Win2", "Window 2");
   cxWindow iWindow3(nullptr, 0, 5, 10, 25, "Win3", "Window 3");
   cxWindow iWindow4(nullptr, 1, 6, 10, 25, "Win4", "Window 4");
   cxWindow iWindow5(nullptr, 2, 7, 10, 25, "Win5", "Window 5");
   cxWindow iWindow6(nullptr, 3, 10, 10, 25, "Win6", "Window 6");
   iWindow.show(false);
   iWindow2.show(false);
   iWindow3.show(false);
   iWindow4.show(false);
   iWindow5.show(false);
   iWindow6.show(false);
   cxBase::messageDialog("Each time you press a key, a window will move, and you can watch what happens as the screen refreshes.");
   getch();
   iWindow2.move(0, 22);
   getch();
   iWindow3.hide();
   getch();
   iWindow3.unhide();
   getch();
   iWindow3.move(1,25);
   getch();
   cxMenu iMenu(nullptr, 1, 1, 8, 15, "Menu");
   iMenu.append("Item 1", 1, "help1");
   iMenu.append("Item 2", 2, "Help2");
   iMenu.show();
   iMenu.move(2, 15);
   iMenu.showModal();
   iWindow.bringToTop(true);
   if (iWindow.isOnTop())
   {
      messageBox("Window 1 is on top!");
   }
   else
   {
      messageBox("Error with cxWindow::isOnTop() - Returned false when should be true");
   }
   if (iWindow.isAbove(iWindow3))
   {
      messageBox("Window 1 is above window 3!");
   }
   else
   {
      messageBox("Error with cxWindow::isAbove() - Returned false when it should return true");
   }
   if (!(iWindow.isBelow(iWindow3)))
   {
      messageBox("Window 1 is not below window 3!");
   }
   iWindow.bringToBottom(true);
   if (iWindow.isOnBottom())
   {
      messageBox("Window 1 is on the bottom!");
   }
   else
   {
      messageBox("Error with cxWindow::isOnBottom() - Returned false when should be true");
   }
   if (iWindow.isBelow(iWindow3))
   {
      messageBox("Window 1 is below window 3!");
   }
   else
   {
      messageBox("Error with cxWindow::isBelow() - Returned false when it should return true");
   }
   getch();
} // demoPanels

void menuItemWithMultipleHotkeys()
{
   cxMenu iMenu(nullptr, 0, 0, 6, 40, "Test");
   iMenu.append("Item 1", 1);
   iMenu.append("Item 2", 2);
   iMenu.append("&Item &3", 3);
   switch(iMenu.showModal())
   {
      case 1:
         messageBox("You chose item 1");
         break;
      case 2:
         messageBox("You chose item 2");
         break;
      case 3:
         messageBox("You chose item 3");
         break;
   }
} // menuItemWithMultipleHotkeys

void inputsWithDifferentColors()
{
   cxInput iInput(nullptr, 2, 25, 40, "Text:");
   iInput.setValueColor(eBLUE_WHITE);
   iInput.setLabelColor(eYELLOW_BLUE);
   iInput.setValue("Text");
   iInput.show();
   getch();

   cxMultiLineInput iMLInput(nullptr, 2, 25, 3, 40, "Text:");
   iMLInput.setValueColor(eYELLOW_WHITE);
   iMLInput.setLabelColor(eBRTCYAN_BLUE);
   iMLInput.setValue("Text");
   iMLInput.show();
   getch();
} // inputWIthDifferentLabelcolor

void formInputColors()
{
   // Test setValue(), setLabelColor(), and setValueColor()
   cxForm iForm(nullptr, 2, 2, 7, 40, "Test form");
   iForm.append(1, 1, 1, 18, "Input 1:");
   iForm.append(2, 1, 1, 18, "Input 2:", "", "");
   iForm.setValue("Input 1:", "Blah");
   iForm.setValue("Input 2:", "Hello");
   iForm.setLabelColor("Input 1:", eBRTRED_BLACK);
   iForm.setValueColor("Input 1:", eBRTGREEN_BLACK);
   iForm.setLabelColor("Input 2:", eYELLOW_WHITE);
   iForm.setValueColor("Input 2:", eBRTBLUE_WHITE);
   iForm.showModal();
   iForm.setLabelColor(0, eYELLOW_WHITE);
   iForm.setValueColor(0, eBRTBLUE_WHITE);
   iForm.setLabelColor(1, eBRTRED_BLACK);
   iForm.setValueColor(1, eBRTGREEN_BLACK);
   iForm.showModal();
   // Out-of-bounds indexes (should be handled okay)
   iForm.setLabelColor(5, eYELLOW_WHITE);
   iForm.setValueColor(6, eBRTBLUE_WHITE);
   iForm.setLabelColor(7, eBRTRED_BLACK);
   iForm.setValueColor(8, eBRTGREEN_BLACK);

   // Test setAllColors()
   cxForm iForm2(nullptr, 0, 2, 7, 40, "Test form 2");
   iForm2.append(1, 1, 1, 18, "Input 1:", "", "", eINPUT_READONLY);
   iForm2.append(2, 1, 1, 18, "Input 2:", "", "", eINPUT_READONLY);
   iForm2.setValue("Input 1:", "Text");
   iForm2.setValue("Input 2:", "More text");
   iForm2.setAllColors(eYELLOW_GREEN, eYELLOW_BLACK);
   iForm2.toggleMasking("Input 2:", true);
   iForm2.append(3, 1, 1, 18, "Input 3:", "", "", eINPUT_READONLY);
   iForm2.showModal();
   iForm2.clearInputs();
   iForm2.setValue("Input 1:", "Blah");
   iForm2.setValue("Input 2:", "Test");
   iForm2.showModal();

   // Test addAttr(), removeAttr(), removeAttrs()
   iForm2.addAttr(eLABEL, A_UNDERLINE);
   iForm2.showModal();
   iForm2.removeAttr(eLABEL, A_UNDERLINE);
   iForm2.showModal();
   iForm2.addAttr(eDATA_EDITABLE, A_UNDERLINE);
   iForm2.showModal();
   iForm2.removeAttr(eDATA_EDITABLE, A_UNDERLINE);
   iForm2.showModal();
   iForm2.addAttr(eLABEL, A_UNDERLINE);
   iForm2.addAttr(eDATA_EDITABLE, A_UNDERLINE);
   iForm2.removeAttrs(eLABEL);
   iForm2.removeAttrs(eDATA_EDITABLE);
   iForm2.showModal();
} // formInputColors

void borderlessForm()
{
   cxForm iForm(nullptr, 2, 2, 7, 40, "Test form", eBS_NOBORDER);
   iForm.append(0, 0, 1, 18, "Input 1:");
   iForm.append(1, 0, 1, 18, "Input 2:", "", "");
   iForm.setAllValueColor(eRED_WHITE);
   iForm.setAutoExit(true);
   iForm.showModal();
   cxBase::dump(true, "borderlessForm.scn");
} // borderlessForm

void unselectableMenuItem()
{
   cxMenu iMenu(nullptr, 1, 1, 10, 20, "Test menu");
   for (int index = 1; index < 11; ++index)
   {
      iMenu.append("Item " + cxStringUtils::toString(index), index);
   }
   iMenu.toggleSelectability(2, false);
   long choice = iMenu.showModal();
   messageBox("You chose item " + cxStringUtils::toString(choice));
} // unselectableMenuItem

void testFormReadOnly()
{
   //messageBox("HELLO");
   cxForm iForm(nullptr, 0, 0, 0, 0, "testFormReadOnly");
   //cxForm iForm(nullptr, 1, 1, 10, 40, "testFormReadOnly");
   iForm.append(1, 1, 1, 10, "A:");
   iForm.append(2, 1, 1, 10, "B:");
   iForm.append(3, 1, 1, 10, "C:");
   iForm.setStatus("foo");
   iForm.setInputOption("C:", eINPUT_READONLY);
   iForm.setAutoExit(true);
   int i = 0;

   iForm.show();
   cxBase::dump(true, "formReadOnly.scn");

   long choice = iForm.showModal();
   while(choice != cxID_QUIT)
   {
      ++i;
      if(i % 2 )
      {
         iForm.setInputOption("B:", eINPUT_EDITABLE);
      }
      else
      {
         iForm.setInputOption("B:", eINPUT_READONLY);
      }
      choice = iForm.showModal();
   }
} // testFormReadOnly

void stackedFormScrolling2()
{
   cxForm iForm(nullptr, 5, 1, 5, 30, "Catalog   Qty", eBS_SINGLE_LINE, nullptr, nullptr, false, true);
   for (unsigned i = 0; i < 24; ++i)
   {
      iForm.append(1, 0, "   " + cxStringUtils::toString(i) + ":");
   }
   iForm.showModal();
   /*
   cxForm iForm(nullptr, 2, 1, 5, 12, "Test form", eBS_SINGLE_LINE,
                nullptr, nullptr, false, true);
   for (unsigned i = 0; i < 8; ++i) {
      iForm.append(1, "Item " + cxStringUtils::toString(i), "", "", eINPUT_READONLY);
   }
   iForm.showModal();
   */
} // stackedFormScrolling2

void formWithInputValueValidator()
{
   messageBox("Note: The value typed into input 1 must be 'good' for it to be valid.");
   cxForm iForm(nullptr, 1, 1, 15, 45, "Test form");
   shared_ptr<cxMultiLineInput> input = iForm.append(1, 1, 1, 20, "Input 1:");
   iForm.append(2, 1, 1, 20, "Input 2:");
   auto validatorFunc = cxFunction1RefTemplated<cxMultiLineInput>::create(someFunction3, *(input.get()));
   iForm.setValidatorFunction("Input 1:", validatorFunc);
   iForm.showModal();
} // formWithInputValueValidator

void testFormPos()
{
   {
      cxForm iForm(nullptr, 0, 0, 0, 0, "Testing form positions (form 1)");
      iForm.append(1, iForm.right()-20, 1, 20, "Test1:");
      iForm.append(iForm.bottom()-1, iForm.right()-1,  1,  1, "", "", "", eINPUT_EDITABLE, "X");
      iForm.showModal();
   }
   // try again with "X" in corner
   {
      cxForm iForm(nullptr, 0, 0, 0, 0, "Testing form positions (form 2)");
      iForm.append(1, iForm.right()-20, 1, 20, "Test1:");
      iForm.append(iForm.bottom(), iForm.right(),  1,  1, "", "", "", eINPUT_EDITABLE, "X");
      iForm.showModal();
   }
} // testFormPos

/* void testFormFields()
{
   cxForm iForm(nullptr, 0, 0, 0, 0, "Testing form fields");
   iForm.append( 1, 1, 1, 20, "Test1:");
   iForm.append(iForm.bottom()-1, iForm.right()-1,  1,  1, "", "", "", eINPUT_EDITABLE, "X");
   iForm.showModal();
}
*/

string inputYesNo(void *theInput, void *unused, void *unused2, void *unused3)
{
   if (theInput == nullptr)
   {
      return("");
   }

   string retval;

   cxMultiLineInput *pInput = (cxMultiLineInput*)theInput;
   string input = pInput->getValue();
   if (input != "")
   {
      // Get rid of any characters except for just y or n
      //  (this is how our JavaScript validators work in the
      //  PHP pages for doing HTML form input validation) -
      //  While the current character in the string is
      //  not Y, N, y, or n, then erase it.
      while (input.find_last_of("YNyn") != input.size()-1)
      {
         input.erase(input.length()-1);
      }

      pInput->setValue(input, true);
   }

   return(retval);
} // inputYesNo

void formWithMenu()
{
   cxForm iForm(nullptr, 5, 0, -5, 0, "Test form");
   cxMenu iMenu(&iForm, 0, 0, 5, 0, "Test menu");

   iForm.append(1, 1, 1, 25, "Input 1:");
   iForm.append(1, 27, 1, 25, "Input 2:");
   iForm.append(2, 1, 1, 25, "Input 3:");
   iForm.append(2, 27, 1, 25, "Input 4:");
   iForm.setAutoExit(true);

   for (int i = 1; i < 10; ++i)
   {
      iMenu.append("Item " + cxStringUtils::toString(i), i);
   }

   long retval = 0;
   while (retval != cxID_QUIT)
   {
      retval = iForm.showModal();
      if (retval != cxID_QUIT)
      {
         retval = iMenu.showModal();
      }
   }
} // formWithMenu

void cxWindowAssignment()
{
   string message = "This tests the cxWindow assignment operator.  "
                  + string("If the app doesn't crash, this test passes!");
   cxBase::messageDialog(message);

   // Note: mParentWindow is not copied from one cxWindow to another.
   //  The reason is that segfaults could happen if it was copied.

   cxWindow windowCopy(nullptr, 0, 0, 5, 10, "This will change");
   {
      cxWindow window1(nullptr, 5, 20, 5, 40, "Title", "Message", "Status");
      windowCopy = window1;
   }

   windowCopy.show();
   getch();
} // cxWindowAssignment

void cxMenuAssignment()
{
   string message = "This tests the cxMenu assignment operator.  "
                  + string("If the app doesn't crash, this test passes!");
   cxBase::messageDialog(message);

   cxMenu menuCopy;
   menuCopy.append("These will", 1);
   menuCopy.append("be deleted", 2);

   {
      cxMenu someMenu(nullptr, 1, 5, 18, 15, "Test menu");
      someMenu.setHorizTitleAlignment(eHP_CENTER);
      for (int i = 1; i <= 25; ++i)
      {
         someMenu.append("Item " + cxStringUtils::toString(i), i);
      }
      menuCopy = someMenu;
   }

   menuCopy.showModal();
} // cxMenuAssignment

void cxFormAssignment()
{
   //string message = "This tests the cxForm assignment operator.  "
   //               + string("If the app doesn't crash, this test passes!");
   //cxBase::messageDialog(message);
   cxForm formCopy(nullptr, 5, 2, 8, 50, "This will go away");
   formCopy.append(1, 1, 1, 25, "This will be gone too:");
   formCopy.append(2, 1, 1, 25, "And so will this:");

   {
      cxForm iForm(nullptr, 2, 0, 7, 40, "Test form");
      iForm.append(1, 1, 1, 9, "Input 1:");
      iForm.append(2, 1, 1, 18, "Input 2:", "", "", eINPUT_READONLY);
      iForm.append(3, 1, 1, 18, "Input 3:", "", "Testing the field highlighting");
      iForm.append(4, 12, 1, 7, "Test4:", "", "Testing the Y/N validation.");
      iForm.setOnKeyFunction(3, inputYesNo, iForm.getInput(3).get(), nullptr, nullptr, nullptr);
      iForm.append(5, 12, 1, 7, "Test5:", "", "Testing single character field.");
      iForm.append(0, 12, 1, 10, "Test:", "", "Field in the border");
      iForm.getInput(2)->setAttr(eDATA_EDITABLE, A_STANDOUT);
      iForm.setValue("Input 1:", "Blah");
      iForm.setValue("Input 2:", "read_only...");
      iForm.setAutoExit(true);

      formCopy = iForm;
   }

   formCopy.showModal();
} // cxFormAssignment

void multiLineInputResize()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 5, 20, "Text:", eBS_SINGLE_LINE);
   iInput.showModal();
   messageBox(":" + iInput.getValue() + ":");
   iInput.resize(4, 22, false);
   iInput.showModal();
   messageBox(":" + iInput.getValue() + ":");
} // multiLineInputResize

void setValFunc()
{
   cxForm iForm(nullptr, 0, 0, cxBase::height(), cxBase::width(), "Test form");
   // this should pop up messageBox with "Success" as a message.
   iForm.append(1, 1, 1, 15, "test:", "", "Just Hit Enter.");
   iForm.setValue("test:", "Hit Enter");
   //iForm.setValidatorFunction("test:",testBoxSuccess, nullptr, nullptr, nullptr, nullptr);
   iForm.setOnLeaveFunction("test:", testBoxSuccess, nullptr, nullptr, nullptr, nullptr);

   int ret = 0;
   while(ret != cxID_QUIT)
   {
      ret = iForm.showModal();
   }
} // setValFunc

string testBoxSuccess(void* Foo1, void* Foo2, void* Foo3, void* Foo4)
{
   // If Foo1 is non-nullptr, assume it's a string pointer and use that
   //  for the message box.
   if (Foo1 != nullptr)
   {
      string *pStr = static_cast<string*>(Foo1);
      messageBox(*pStr);
   }
   else
   {
      messageBox("Success");
   }

   return("");
} // testBoxSuccess


void formUpArrowFunction()
{
   cxForm iForm(nullptr, 2, 0, 7, 40, "Test form");
   iForm.append(1, 1, 1, 9, "Input 1:");
   iForm.append(2, 1, 1, 18, "Input 2:");
   auto func = make_shared<cxFunction0>(genericMessageFunction);
   iForm.setOnLeaveFunction(0, func);
   iForm.setKeyFunction(KEY_UP, cxBase::noOp, nullptr, nullptr, true);
   iForm.showModal();
   if (iForm.getLastKey() == KEY_UP)
   {
      messageBox("Last key was an up arrow.");
   }
   else
   {
      messageBox("Last key not an up arrow - Last key: " + cxStringUtils::toString(iForm.getLastKey()));
   }
} // formUpArrowFunction

void changeMenuItemText()
{
   cxMenu iMenu(nullptr, 1, 0, 12, 20, "Test menu");
   for (int i = 1; i <= 10; ++i)
   {
      iMenu.append("Item " + cxStringUtils::toString(i), i);
   }
   iMenu.setItemTextByIndex(0, "Changed!"); // 1st menu item
   iMenu.setItemTextByReturnCode(2, "Also changed!"); // 2nd menu item
   iMenu.showModal();
} // changeMenuItemText

void windowWriteText()
{
   cxWindow iWindow(nullptr, 1, 0, 20, 50, "Test window", "Message", "Status");
   iWindow.show();
   iWindow.writeText(2, 1, "Text");
   getch();
} // windowWriteText

void formChangeInputFocusWhileModal()
{
   messageBox("Press 'n' to change the focus to the last input.");

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
} // formChangeInputFocusWhileModal

string setAllInputsReadOnly(void *theForm, void *unused, void *unused2, void *unused3)
{
   cxForm *pForm = (cxForm*)theForm;

   pForm->setAllInputOption(eINPUT_READONLY);

   return("");
} // setAllInputsReadOnly

void allFormInputsGoReadOnly()
{
   messageBox("Press F2 in Input 2 to set all inputs read-only.  The form should still be showing, and it should wait for a keypress.");

   cxForm iForm(nullptr, 1, 0, 12, 50, "Test form");
   iForm.append(1, 1, 1, 25, "Input 1:", "", "", eINPUT_READONLY);
   iForm.getInput(0)->setValue("Read-only");
   iForm.append(2, 1, 1, 25, "Input 2:");
   iForm.append(3, 1, 1, 25, "Input 3:", "", "", eINPUT_READONLY);
   iForm.getInput(2)->setValue("Read-only");
   iForm.append(4, 1, 1, 25, "Input 4:", "", "", eINPUT_READONLY);
   iForm.getInput(3)->setValue("Read-only");
   iForm.append(5, 1, 1, 25, "Input 5:", "", "", eINPUT_EDITABLE);
   iForm.append(6, 1, 1, 25, "Input 6:", "", "", eINPUT_EDITABLE);
   iForm.append(7, 1, 1, 25, "Input 7:", "", "", eINPUT_EDITABLE);
   iForm.append(8, 1, 1, 25, "Input 8:", "", "", eINPUT_EDITABLE);
   iForm.append(9, 1, 1, 25, "Input 9:", "", "", eINPUT_EDITABLE);
   iForm.append(10, 1, 1, 25, "Input 10:", "", "", eINPUT_EDITABLE);
   iForm.setFieldKeyFunction("Input 2:", KEY_F(2), setAllInputsReadOnly, &iForm, nullptr,
                          nullptr, nullptr, false);
   long retval = iForm.showModal();
   if (retval == cxID_QUIT)
   {
      messageBox("cxID_QUIT returned");
   }
   else if (retval == cxID_EXIT)
   {
      messageBox("cxID_EXIT returned");
   }
   else
   {
      // Shouldn't get here
      messageBox("Something besides cxID_QUIT and cxID_EXIT returned");
   }
} // allFormInputsGoReadOnly

void cxFormNoQuit()
{
   messageBox("The form shown here can't be quit out of using ESC.");
   cxForm iForm(nullptr, 1, 0, 12, 60, "Test form");
   iForm.append(1, 1, 1, 25, "Input 1:");
   iForm.append(2, 1, 1, 25, "Input 2:");
   iForm.append(1, 27, 1, 25, "Input 3:");
   iForm.append(2, 27, 1, 25, "Input 4:");
   iForm.setAutoExit(true);
   iForm.setAllowQuit(false);
   iForm.showModal();
} // cxFormNoQuit

void cxMenuNoQuit()
{
   messageBox("The menu shown here can't be quit out of using ESC.");
   cxMenu iMenu(nullptr, 1, 0, 15, 12, "Test menu");
   for (int i = 1; i <= 20; ++i)
   {
      iMenu.append("Item " + cxStringUtils::toString(i), i);
   }
   iMenu.setAllowQuit(false);
   iMenu.showModal();
} // cxMenuNoQuit

void scrolledWindow()
{
   // This demonstrates a scrolled window.  It also
   //  tests that its assignment operator works correctly, etc.
   shared_ptr<cxScrolledWindow> iWin = make_shared<cxScrolledWindow>(nullptr, 1, 0, 4, 10, "Test", "This is some very, very long text.  When this window is initialized, the window should scroll.  Yep, it sure should, yep yep yep.");
   cxScrolledWindow iWin2(nullptr, 2, 5, 10, 75, "Something", "Message", "Status");
   iWin2 = *iWin;
   iWin2.showModal();
} // scrolledWindow

void cxInputBorderChange()
{
   cxInput iInput(nullptr, 1, 0, 8, "Text:", eBS_NOBORDER);
   iInput.showModal();
   messageBox("Value:" + iInput.getValue() + ":");
   iInput.setBorderStyle(eBS_SINGLE_LINE);
   iInput.showModal();
   messageBox("Value:" + iInput.getValue() + ":");
   iInput.setBorderStyle(eBS_NOBORDER);
   iInput.showModal();
   messageBox("Value:" + iInput.getValue() + ":");
} // cxInputBorderChange

void cxMultiLineInputBorderChange()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 2, 8, "Text:", eBS_NOBORDER);
   iInput.setExitOnFull(false);
   iInput.showModal();
   messageBox("Value:" + iInput.getValue() + ":");
   iInput.setBorderStyle(eBS_SINGLE_LINE);
   iInput.showModal();
   messageBox("Value:" + iInput.getValue() + ":");
   iInput.setBorderStyle(eBS_NOBORDER);
   iInput.showModal();
   messageBox("Value:" + iInput.getValue() + ":");
} // cxInputBorderChange

bool fKeySort(const string& pStr1, const string& pStr2)
{
   // Find the indexes of the first digits in pStr1 and pStr2
   unsigned str1Index = 0;
   unsigned str2Index = 0;
   for (unsigned i = 0; i < pStr1.length(); ++i)
   {
      if ((pStr1[i] >= '0') && (pStr1[i] <= '9'))
      {
         str1Index = i;
         break;
      }
   }
   for (unsigned i = 0; i < pStr1.length(); ++i)
   {
      if ((pStr2[i] >= '0') && (pStr2[i] <= '9'))
      {
         str2Index = i;
         break;
      }
   }

   unsigned num1 = cxStringUtils::stringToUnsigned(pStr1.substr(str1Index));
   unsigned num2 = cxStringUtils::stringToUnsigned(pStr2.substr(str2Index));

   return(num1 < num2);
} // fKeySort

void getFormKeys()
{
   cxForm iForm(nullptr, 0, 0, 7, 30, "Test form");
   iForm.append(1, 1, 2, 15, "Name:");
   iForm.append(3, 1, 1, 25, "City:");

   int x = 0, y = 0;
   auto keyFunc1 = cxFunction2RefTemplated<int, int>::create(someFunction, x, y);
   auto keyFunc1UseReturnVal = cxFunction2RefTemplated<int, int>::create(someFunction, x, y, true);
   iForm.setKeyFunction(KEY_F(4), keyFunc1);
   iForm.setKeyFunction(KEY_F(5), keyFunc1UseReturnVal);
   iForm.setKeyFunction(KEY_NPAGE, keyFunc1UseReturnVal);
   iForm.setKeyFunction(KEY_PPAGE, keyFunc1UseReturnVal);
   iForm.setKeyFunction('/', keyFunc1UseReturnVal);

   vector<string> formKeys;
   iForm.getFunctionKeyStrings(formKeys);
   std::sort(formKeys.begin(), formKeys.end(), fKeySort);
   string allFormKeys;
   for (vector<string>::iterator iter = formKeys.begin(); iter != formKeys.end(); ++iter)
   {
      allFormKeys += *iter + ", ";
   }
   // Remove the trailing ", " from allFormKeys
   if (allFormKeys != "")
   {
      allFormKeys.erase(allFormKeys.size()-2);
   }
   messageBox("The form created in this function has these function keys: " + allFormKeys);
} // getFormKeys

void multiForm()
{
   // This function tests cxMultiForm..  It also tests
   //  its assignment operator to make sure it can
   //  assign properly (it copies another cxMultiform
   //  that goes out of scope - it should copy all
   //  the dynamic data so that there are no segfaults).
   cxMultiForm copyForm(nullptr, 2, 2, 10, 60, "Copy form", eBS_NOBORDER);
   {
      cxMultiForm iMultiForm(nullptr, 1, 0, 20, 70, "Multi-form", eBS_SINGLE_LINE);
      iMultiForm.append(1, 1, 1, 25, "Input 1:");
      iMultiForm.append(2, 1, 1, 25, "Input 2:");
      // Append an input on the same line as one of the subforms
      iMultiForm.append(3, 1, 1, 25, "Input 3:");
      shared_ptr<cxForm> iForm = iMultiForm.appendForm(3, 1, 3, 60, "Subform", eBS_SINGLE_LINE);
      iForm->append(1, 1, 1, 25, "Subform input 1:");
      iForm->append(1, 27, 1, 25, "Subform input 2:");
      iForm = iMultiForm.appendForm(6, 1, 1, 60, "Subform 2"); // Borderless subform
      iForm->append(0, 0, 1, 25, "Subform input 1:");
      iForm->append(0, 26, 1, 25, "Subform input 2:");
      iMultiForm.setAutoExit(false);
      copyForm = iMultiForm;
   }
   copyForm.move(2, 2, false);
   copyForm.showModal();
} // multiForm

string multiForm2_goToForm1_Input1(void *theMultiForm, void *unused, void *unused2, void *unused3)
{
   if (theMultiForm== nullptr)
   {
      return("");
   }
   //cxBase::messageBox("in multiForm2_goToForm1_Input1");
   cxMultiForm *pMultiForm = (cxMultiForm*)theMultiForm;
   pMultiForm->setCurrentSubform(0);
   pMultiForm->getForm(0)->setCurrentInput(0);

   return("");
}

string multiForm2_goToForm3_Input1(void *theMultiForm, void *unused, void *unused2, void *unused3)
{
   if (theMultiForm== nullptr)
   {
      return("");
   }
   //cxBase::messageBox("in multiForm2_goToForm3_Input1");
   cxMultiForm *pMultiForm = (cxMultiForm*)theMultiForm;
   pMultiForm->setCurrentSubform(2);
   pMultiForm->getForm(2)->setCurrentInput(0);

   return("");
}

string multiForm2_quit(void *theMultiForm, void *unused, void *unused2, void *unused3)
{
   if (theMultiForm== nullptr)
   {
      return("");
   }
   cxMultiForm *pMultiForm = (cxMultiForm*)theMultiForm;
   pMultiForm->quitNow();
   return("");
} // multiForm2_quit

void multiForm2()
{
   // This function tests cxMultiForm events.
   cxMultiForm iForm(nullptr, 1, 0, 20, 70, "Multi-form", eBS_SINGLE_LINE);

   shared_ptr<cxForm> pForm1 = make_shared<cxForm>(nullptr, 3, 1, 4, 60, "Form1");
   pForm1->append(1, 1, 1, 25, "Form 1 input 1:");
   pForm1->append(2, 1, 1, 25, "Form 1 input 2:");
   pForm1->setStatus("test status", true);

   shared_ptr<cxForm> pForm2 = make_shared<cxForm>(nullptr, 1, 1, 4, 60, "Form2");
   pForm2->append(1, 1, 1, 25, "Form 2 input 1:");
   pForm2->append(2, 1, 1, 25, "Form 2 input 2:");
   pForm2->addQuitKey(KEY_F(9));
   pForm2->setStatus("F1 = Go to form1/input1, F9=Quit F10=Quit", true);

   shared_ptr<cxForm> pForm3 = make_shared<cxForm>(nullptr, 1, 1, 4, 60, "Form3");
   pForm3->append(1, 1, 1, 25, "Form 3 input 1:");
   pForm3->append(2, 1, 1, 25, "Form 3 input 2:");
   pForm3->setStatus("F1 = Go to form1/input1, F2 = Go to form3/input1", true);
   //pForm3->setEnabled(false);

   iForm.appendForm(pForm1, 1, 1);
   iForm.appendForm(pForm2, 5, 1);
   iForm.appendForm(pForm3, 9, 1);
   iForm.setKeyFunction(KEY_F(1), multiForm2_goToForm1_Input1, &iForm, nullptr, nullptr, nullptr, false);
   iForm.setKeyFunction(KEY_F(2), multiForm2_goToForm3_Input1, &iForm, nullptr, nullptr, nullptr, false);
   iForm.setKeyFunction(KEY_F(10), multiForm2_quit, &iForm, nullptr, nullptr, nullptr, false);

   iForm.setAutoExit(true);
   iForm.showModal();
} // multiForm2

void simpleForm1()
{
   cxForm iForm(nullptr, 0, 0, 20, 42, "Keyboard Help");
   iForm.append(1, 1, 1, 40, "F1  = Field Help", "", "", eINPUT_READONLY);
   iForm.append(2, 1, 1, 40, "sF1 = Keyboard Help", "", "", eINPUT_READONLY);
   iForm.append(3, 1, 1, 40, "F2  = Search", "", "", eINPUT_READONLY);
   iForm.append(4, 1, 1, 40, "F5  = Maintenance/Inquiry", "", "", eINPUT_READONLY);
   iForm.center(false);
   iForm.showModal();
} // simpleForm1

string updateMenu(void *theComboBox, void *unused, void *unused2, void *unused3)
{
   if (theComboBox == nullptr)
   {
      return("");
   }

   cxComboBox *pComboBox = (cxComboBox*)theComboBox;
   //pComboBox->appendToMenu("Item " + cxStringUtils::toString(pComboBox->numMenuItems()+1), true, false);
   pComboBox->appendToMenu(pComboBox->getValue(), "", true, false);

   return("");
} // updateMenu

string onFocus(void *unused, void *unused2, void *unused3, void *unused4)
{
   messageBox("onFocus function");

   return ("");
} // onFocus

string onLeave(void *unused, void *unused2, void *unused3, void *unused4)
{
   messageBox("onLeave function");

   return ("");
} // onLeave

void comboBox()
{
   //messageBox("Each time you press a key in the input, a new item will be added to its drop-down menu, demonstrating its updating ability and ability to choose one of the items from the menu.");
   cxComboBox iComboBox(nullptr, 1, 0, 10, 40, "Input:");
   iComboBox.setOnKeyFunction(updateMenu, &iComboBox, nullptr, nullptr, nullptr);
   //iComboBox.setOnFocusFunction(onFocus, nullptr, nullptr, nullptr, nullptr, false);
   //iComboBox.setOnLeaveFunction(onLeave, nullptr, nullptr, nullptr, nullptr);
   // Change the menu height
   iComboBox.setMenuHeight(11);
   messageBox("Combo box height before resize: " + cxStringUtils::toString(iComboBox.height()));
   messageBox("Combo box width before resize: " + cxStringUtils::toString(iComboBox.width()));
   messageBox("Input height: " + cxStringUtils::toString(iComboBox.inputHeight()));
   messageBox("Menu height: " + cxStringUtils::toString(iComboBox.menuHeight()));
   iComboBox.showModal();
   iComboBox.resize(15, 50);
   messageBox("Combo box height after resize: " + cxStringUtils::toString(iComboBox.height()));
   messageBox("Combo box width after resize: " + cxStringUtils::toString(iComboBox.width()));
   messageBox("Input height after resize: " + cxStringUtils::toString(iComboBox.inputHeight()));
   messageBox("Menu height after resize: " + cxStringUtils::toString(iComboBox.menuHeight()));
   iComboBox.showModal();
} // comboBox

void comboBoxOnForm()
{
   //messageBox("Input A on the form is a cxComboBox.  All other inputs are cxMultiLineInputs.");
   cxForm iForm(nullptr, 1, 0, 12, 60, "Test form");
   iForm.appendComboBox(1, 1, 10, 12, "A:");
   iForm.append(1, 14, 1, 12, "B:");
   iForm.append(2, 1, 1, 12, "C:");
   iForm.setOnKeyFunction("A:", updateMenu, (cxComboBox*)iForm.getInput("A:").get(), nullptr,
                          nullptr, nullptr);
   iForm.showModal();
} // comboBoxOnForm

void inputWithoutBorder()
{
   cxForm iForm(nullptr, 0, 0, -1, 0, "Do you see my title?");
   iForm.showModal();
   cxForm iForm2(nullptr, 1, 0, 1, 0, "", eBS_NOBORDER);
   iForm2.append(0, 1, 1, 20, "A:", "", "Enter something janky...");
   iForm2.showModal();
} // inputWithoutBorder

void formDataChanged()
{
   cxForm iForm(nullptr, 1, 0, 12, 60, "Test form");
   iForm.append(1, 1, 1, 12, "A:");
   iForm.append(1, 14, 1, 12, "B:");
   iForm.append(2, 1, 1, 12, "C:");
   iForm.setAutoExit(true);
   iForm.showModal();
   if (iForm.hasChanged())
   {
      messageBox("Data has changed.");
   }
   else
   {
      messageBox("No data changed.");
   }
   iForm.showModal();
   if (iForm.hasChanged())
   {
      messageBox("Data has changed.");
   }
   else
   {
      messageBox("No data changed.");
   }
} // formDataChanged

void integerValidate()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 15, "Integer:");
   iInput.setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
   iInput.showModal();
   cxBase::messageBox("value:" + iInput.getValue() + ":");
} // integerValidate

void floatingPtValidate()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 25, "Floating-pt #:");
   iInput.setInputType(eINPUT_TYPE_NUMERIC_FLOATING_PT);
   iInput.showModal();
   cxBase::messageBox("value:" + iInput.getValue() + ":");
} // floatingPtValidate

void integerValidateOnForm()
{
   bool clearOnSpace = true;
   cxForm iForm(nullptr, 1, 0, 10, 60, "Test form");
   shared_ptr<cxMultiLineInput> input = iForm.append(1, 1, 1, 40, "Integer:");
   iForm.setOnKeyFunction("Integer:", cxValidators::intOnKeyValidator, input.get(),
                          &clearOnSpace, true);
   input = iForm.append(2, 1, 1, 40, "Integer 2:");
   iForm.setOnKeyFunction("Integer 2:", cxValidators::intOnKeyValidator, input.get(),
                          &clearOnSpace, true);
   iForm.showModal();
} // integerValidate

void floatingPtValidateOnForm()
{
   bool clearOnSpace = true;
   cxForm iForm(nullptr, 1, 0, 10, 60, "Test form");
   shared_ptr<cxMultiLineInput> input = iForm.append(1, 1, 1, 40, "Floating pt. # 1:");
   iForm.setOnKeyFunction("Floating pt. #1", cxValidators::floatingPtOnKeyValidator,
                          input.get(), &clearOnSpace, true);
   input = iForm.append(2, 1, 1, 40, "Floating pt. # 2:");
   iForm.setOnKeyFunction("Floating pt. #2", cxValidators::floatingPtOnKeyValidator,
                          input.get(), &clearOnSpace, true);
   iForm.showModal();
} // integerValidate

// This function (showAMessageBox() is for use with formFunctionKeys().
string showAMessageBox(void *classTypeStr, void *unused)
{
   if (classTypeStr == nullptr)
   {
      messageBox("showAMessageBox()");
   }
   else
   {
      string *pClassTypeStr = static_cast<string*>(classTypeStr);
      messageBox("showMessageBox() - Called from a " + *pClassTypeStr);
   }

   return("");
} // showAMessageBox

void keyFunctionAutoExitOnLastFormInput()
{
   cxBase::messageBox("F3 is set up on the first and last inputs of the form, and it's set up to exit the inputs after the function runs.  After pressing F3 on input 1, it should move to input 2.  After pressing F3 on input 3 (the last input), the form should exit.");

   cxForm iForm(nullptr, 1, 0, 5, 27, "Test form", eBS_SINGLE_LINE);
   shared_ptr<cxMultiLineInput> input1 = iForm.append(1, 1, 1, 25, "Input 1:");
   iForm.append(2, 1, 1, 25, "Input 2:");
   shared_ptr<cxMultiLineInput> input3 = iForm.append(3, 1, 1, 25, "Input 3:");
   input1->setKeyFunction(KEY_F(3), showAMessageBox, nullptr, nullptr, false, true);
   input3->setKeyFunction(KEY_F(3), showAMessageBox, nullptr, nullptr, false, true);
   iForm.setAutoExit(true);
   iForm.showModal();
}

void formFunctionKeys()
{
   // This function tests that cxMultiLineInput doesn't run its onLeave
   //  function when you press a function key that should be caught
   //  higher up (i.e., in cxForm).
   messageBox("This function tests that cxMultiLineInput doesn't run its onLeave function when you press a function key that should be caught higher up (i.e., in cxForm).  The input on the form has an onLeave function.  F2 fires a form function, and F3 fires a field function.  The onLeave message box should not appear when you press F2 or F3, because cxMultiLineInput doesn't run its onLeave when a parent form's function key is pressed.");

   string cxMLIStr = "cxMultiLineInput"; // For showAMessageBox()
   string cxFormStr = "cxForm";          // For showAMessageBox()

   cxForm iForm(nullptr, 1, 0, 5, 27, "Test form", eBS_SINGLE_LINE);
   iForm.setKeyFunction(KEY_F(2), showAMessageBox, &cxFormStr, nullptr, false);
   // Note: It shouldn't matter if we append inputs to the form after setting
   //  a form function key on the form.
   shared_ptr<cxMultiLineInput> input = iForm.append(1, 1, 1, 25, "Input 1:");
   input->setOnLeaveFunction(make_shared<cxFunction0>(genericMessageFunction));
   input->setKeyFunction(KEY_F(3), showAMessageBox, &cxMLIStr, nullptr, false, false);

   iForm.showModal();
} // formFunctionKeys

void enableDisableForm()
{
   messageBox("This function tests setEnabled() for a cxForm.  A cxForm is enabled by default.  A cxForm will be created and shown modally, then disabled, shown modally again, and enabled again & shown modally again.  When disabled, it should hide itself, and showModal() should not process user input.");

   cxForm iForm(nullptr, 1, 0, 4, 60, "Test form");
   iForm.append(1, 1, 1, 25, "Input 1:");
   iForm.append(2, 1, 1, 25, "Input 2:");
   iForm.append(1, 27, 1, 25, "Input 3:");
   iForm.append(2, 27, 1, 25, "Input 4:");
   iForm.setAutoExit(true);

   iForm.showModal();
   messageBox("Disabling the form..");
   iForm.setEnabled(false);
   if (!(iForm.isEnabled()))
   {
      messageBox("It's disabled.");
   }
   else
   {
      messageBox("Oops, iForm.isEnabled() returned true when it should've returned false!");
   }
   messageBox("Calling showModal() again after disabling the form..");
   long retval = iForm.showModal();
   messageBox("showModal() returned " + getReturnCodeStr(retval) +
              ".  Enabling the form again, and calling showModal().");
   iForm.setEnabled(true);
   iForm.showModal();
} // enableDisableForm

void multiFormWithDisabledSubform()
{
   cxMultiForm iMultiForm(nullptr, 1, 0, 23, 75, "Test form");
   // Append an input to the multiForm for good measure (even though
   //  this is just testing the enabled/disabled stuff on subforms).
   iMultiForm.append(1, 1, 1, 32, "MultiForm input:");
   // Append the subforms to the multiForm
   shared_ptr<cxForm> form = iMultiForm.appendForm(2, 1, 4, 73, "Subform 1", eBS_SINGLE_LINE);
   form->append(1, 1, 1, 32, "Sub1 input 1:");
   form->append(2, 1, 1, 32, "Sub1 input 2:");
   form->append(1, 34, 1, 32, "Sub1 input 3:");
   form->append(2, 34, 1, 32, "Sub1 input 4:");
   form = make_shared<cxForm>(nullptr, 1, 0, 4, 73, "Subform 2", eBS_SINGLE_LINE);
   form->append(1, 1, 1, 32, "Sub2 input 1:");
   form->append(2, 1, 1, 32, "Sub2 input 2:");
   form->append(1, 34, 1, 32, "Sub2 input 3:");
   form->append(2, 34, 1, 32, "Sub2 input 4:");
   iMultiForm.appendForm(form, 6, 1);
   form = make_shared<cxForm>(nullptr, 1, 0, 4, 73, "Subform 3", eBS_SINGLE_LINE);
   form->append(1, 1, 1, 32, "Sub3 input 1:");
   form->append(2, 1, 1, 32, "Sub3 input 2:");
   form->append(1, 34, 1, 32, "Sub3 input 3:");
   form->append(2, 34, 1, 32, "Sub3 input 4:");
   iMultiForm.appendForm(form, 10, 1);
   form = make_shared<cxForm>(nullptr, 1, 0, 4, 73, "Subform 4", eBS_SINGLE_LINE);
   form->append(1, 1, 1, 32, "Sub4 input 1:");
   form->append(2, 1, 1, 32, "Sub4 input 2:");
   form->append(1, 34, 1, 32, "Sub4 input 3:");
   form->append(2, 34, 1, 32, "Sub4 input 4:");
   iMultiForm.appendForm(form, 14, 1);
   form = make_shared<cxForm>(nullptr, 1, 0, 4, 73, "Subform 5", eBS_SINGLE_LINE);
   form->append(1, 1, 1, 32, "Sub5 input 1:");
   form->append(2, 1, 1, 32, "Sub5 input 2:");
   form->append(1, 34, 1, 32, "Sub5 input 3:");
   form->append(2, 34, 1, 32, "Sub5 input 4:");
   iMultiForm.appendForm(form, 18, 1);

   messageBox("Showing the multiForm with everything enabled.");
   iMultiForm.showModal();
   messageBox("Disabling the 4th subform..");
   iMultiForm.setSubformEnabled(3, false);
   messageBox("Showing the multiForm again (with the 4th subform disabled)");
   iMultiForm.showModal();
   messageBox("Re-enabling the 4th subform..");
   iMultiForm.setSubformEnabled(3, true);
   messageBox("Showing the multiForm again (with the 4th subform re-enabled)");
   iMultiForm.showModal();
   messageBox("Trying to disable subforms with an invalid index & invalid title");
   iMultiForm.setSubformEnabled(97, false);
   iMultiForm.setSubformEnabled("There is no subform with this title", false);
   iMultiForm.showModal();
} // multiFormWithDisabledSubform

void comboBoxMenuDisable()
{
   cxComboBox iComboBox(nullptr, 1, 0, 10, 40, "Input:");
   messageBox("Height: " + cxStringUtils::toString(iComboBox.height()) + ", width: " +
              cxStringUtils::toString(iComboBox.width()));
   iComboBox.setOnKeyFunction(updateMenu, &iComboBox, nullptr, nullptr, nullptr);
   iComboBox.showModal();
   messageBox("Disabling the menu and showing the combo box again..");
   iComboBox.toggleMenu(false, false);
   messageBox("Height: " + cxStringUtils::toString(iComboBox.height()) + ", width: " +
              cxStringUtils::toString(iComboBox.width()));
   iComboBox.showModal();
   messageBox("Resizing it and showing it again..");
   iComboBox.resize(3, 40);
   messageBox("Height: " + cxStringUtils::toString(iComboBox.height()) + ", width: " +
              cxStringUtils::toString(iComboBox.width()));
   iComboBox.showModal();
   messageBox("Re-enabling the menu and showing the combo box again..");
   iComboBox.toggleMenu(true, false);
   messageBox("Height: " + cxStringUtils::toString(iComboBox.height()) + ", width: " +
              cxStringUtils::toString(iComboBox.width()));
   iComboBox.showModal();
   messageBox("Resizing it and showing it again..");
   iComboBox.resize(15, 50);
   messageBox("Height: " + cxStringUtils::toString(iComboBox.height()) + ", width: " +
              cxStringUtils::toString(iComboBox.width()));
   iComboBox.showModal();
} // comboBoxMenuDisable

void inputQuitKeysOnForm()
{
   // Test addQuitKey() in cxForm and cxMultiForm with
   //  SHIFT_F2
   string fieldOnLeaveMsg = "Field onLeave function called";
   string validatorMsg = "Validator function called";
   cxForm iForm(nullptr, 1, 0, 5, 70, "SHIFT-F2 quits this form");
   iForm.append(1, 1, 1, 25, "Input 1:");
   iForm.append(2, 1, 1, 25, "Input 2:");
   iForm.append(3, 1, 1, 25, "Input 3:");
   iForm.append(1, 26, 1, 25, "Input 4:");
   iForm.append(2, 26, 1, 25, "Input 5:");
   iForm.append(3, 26, 1, 25, "Input 6:");
   iForm.setValidatorFunction(0, testBoxSuccess, &validatorMsg, nullptr, nullptr, nullptr);
   iForm.setValidatorFunction(1, testBoxSuccess, &validatorMsg, nullptr, nullptr, nullptr);
   iForm.setValidatorFunction(2, testBoxSuccess, &validatorMsg, nullptr, nullptr, nullptr);
   iForm.setValidatorFunction(3, testBoxSuccess, &validatorMsg, nullptr, nullptr, nullptr);
   iForm.setValidatorFunction(4, testBoxSuccess, &validatorMsg, nullptr, nullptr, nullptr);
   auto func = cxFunction1RefTemplated<string>::create(genericMessageFunction_1str, fieldOnLeaveMsg);
   iForm.setOnLeaveFunction(0, func);
   iForm.setOnLeaveFunction(1, func);
   iForm.setOnLeaveFunction(2, func);
   iForm.setOnLeaveFunction(3, func);
   iForm.setOnLeaveFunction(4, func);
   iForm.setAutoExit(true);
   iForm.addQuitKey(SHIFT_F2);
   iForm.addQuitKey(SHIFT_F3);
   iForm.removeQuitKey(SHIFT_F3);
   messageBox(getReturnCodeStr(iForm.showModal()));
} // inputQuitKeysOnForm

void cxMenuFocusFunctions()
{
   string onFocusMsg = "onFocus function called";
   string onLeaveMsg = "onLeave function called";
   string loopStartMsg = "loop start function called";
   string loopEndMsg = "loop end function called";

   cxMenu iMenu(nullptr, 1, 10, 15, 12, "Test menu");
   for (int index = 1; index < 25; ++index)
   {
      iMenu.append("Item " + cxStringUtils::toString(index), index);
   }
   auto func = cxFunction1RefTemplated<string>::create(genericMessageFunction_1str, onFocusMsg);
   iMenu.setOnFocusFunction(func);
   iMenu.setOnLeaveFunction(func);
   auto loopStartFunc = cxFunction1RefTemplated<string>::create(genericMessageFunction_1str, loopStartMsg);
   iMenu.setLoopStartFunction(loopStartFunc);
   auto loopEndFunc = cxFunction1RefTemplated<string>::create(genericMessageFunction_1str, loopEndMsg);
   iMenu.setLoopEndFunction(loopEndFunc);
   iMenu.showModal();
} // cxMenuFocusFunctions

// Tests a cxMultiLineInput, with its input
//  loop disabled, on a cxForm
void cxInputOnFormLoopDisabled()
{
   cxForm iForm(nullptr, 1, 0, 5, 55, "Test form");
   iForm.append(1, 1, 1, 25, "Input 1:");
   iForm.append(2, 1, 1, 25, "Input 2:");
   iForm.append(3, 1, 1, 25, "Input 3:");
   iForm.append(1, 27, 1, 25, "Input 4:");
   iForm.append(2, 27, 1, 25, "Input 5:");
   iForm.append(3, 27, 1, 25, "Input 6:");

   string onFocusMsg = "Input 2 onFocus function called";
   string onLeaveMsg = "Input 2 onLeave function called";
   auto onFocusFunc = cxFunction1RefTemplated<string>::create(genericMessageFunction_1str, onFocusMsg);
   iForm.setOnFocusFunction("Input 2:", onFocusFunc);
   auto onLeaveFunc = cxFunction1RefTemplated<string>::create(genericMessageFunction_1str, onLeaveMsg);
   iForm.setOnLeaveFunction("Input 2:", onLeaveFunc);
   iForm.enableInputLoop("Input 2:", false);

   iForm.showModal();
} // cxInputOnFormLoopDisabled

void loneInputWithLoopDisabledOnForm()
{
   // This function tests what happens when there is
   //  1 input on a cxForm, and the input's loop is
   //  disabled..  The input's onFocus and onLeave
   //  functions should still run, just once, and
   //  there shouldn't be any wacky problems with
   //  the form or anything (no infinite loop land,
   //  etc.).
   string onFocusMsg = "onFocus function";
   string onLeaveMsg = "onLeave function";
   cxForm iForm(nullptr, 1, 0, 5, 28, "Test form");
   shared_ptr<cxMultiLineInput> input = iForm.append(1, 1, 1, 25, "Input 1:");
   input->enableInputLoop(false);
   auto onFocusFunc = cxFunction1RefTemplated<string>::create(genericMessageFunction_1str, onFocusMsg, false, false, false);
   input->setOnFocusFunction(onFocusFunc);
   auto onLeaveFunc = cxFunction1RefTemplated<string>::create(genericMessageFunction_1str, onLeaveMsg);
   input->setOnLeaveFunction(onLeaveFunc);
   iForm.setAutoExit(true);
   iForm.showModal();
} // loneInputWithLoopDisabledOnForm

void cxMenuWithMultipleItemHotkeys()
{
   // This function tests the behavior of a cxMenu that has
   //  multiple items with the same hotkey.  If there is only
   //  1 of a hotkey, then the item should automatically be
   //  selected.  If there is more than 1 item with a hotkey,
   //  then the cxMenu should cycle through highlighting the
   //  next item with the hotkey but not actually select it.
   cxMenu iMenu(nullptr, 1, 0, 8, 12, "Test");
   iMenu.append("&Item 1", 1);
   iMenu.append("It&em 2", 2);
   iMenu.append("It&em 3", 3);
   iMenu.append("It&em 4", 4);
   iMenu.append("I&tem 5", 5);
   iMenu.append("I&tem 6", 6);
   long retval = iMenu.showModal();
   if (retval != cxID_QUIT)
   {
      string message = "You chose item " + cxStringUtils::toString(retval);
      cxBase::messageBox(message);
   }
} // cxMenuWithMultipleItemHotkeys

void cxMenuItemHotkeysOffMenu()
{
   cxMenu subMenu(nullptr, 1, 0, 5, 15, "Submenu");
   subMenu.append("Item 8", 8, "", cxITEM_NORMAL, false);
   subMenu.append("Item 9", 9, "", cxITEM_NORMAL, false);
   cxMenu iMenu(nullptr, 1, 0, 5, 15, "Test");
   iMenu.append("&Item 1", 1, "", cxITEM_NORMAL, false);
   iMenu.append("Item 2", 2, "", cxITEM_NORMAL, false);
   iMenu.append("Item 3", 3, "", cxITEM_NORMAL, false);
   iMenu.append("Item 4", 4, "", cxITEM_NORMAL, false);
   iMenu.append("Item 5", 5, "", cxITEM_NORMAL, false);
   iMenu.append("&Item 6", 6, "", cxITEM_NORMAL, false);
   iMenu.append("It&em 7", 7, "", cxITEM_NORMAL, false);
   iMenu.appendWithPullRight("&More items", &subMenu, "", false);
   iMenu.showModal();
} // cxMenuItemHotkeysOffMenu

string changeWindowFocus(void *thePanel, void *unused, void *unused2, void *unused3)
{
   if (thePanel == nullptr)
   {
      return("");
   }

   cxPanel *pPanel = static_cast<cxPanel*>(thePanel);
   pPanel->setCurrentWindow(2);

   return("");
} // changeWindowFocus

void cxPanelTest()
{
   cxPanel iPanel(nullptr, 2, 1, 20, 75);
   iPanel.setTitle("The panel"); // Could help with debugging

   // Add various other windows to the panel
   // We'll use iPanel.append() to add the following window to
   // the panel
   shared_ptr<cxWindow> iWindow = make_shared<cxWindow>(nullptr, 0, 0, 5, 10,
                                                        "cxWindow", "This is a cxWindow.");
   iPanel.append(iWindow, 1, 1);

   // We can pass a pointer to the panel to other windows as their
   //  parent, but we still need to call addWindow on the panel to
   //  add the window to its list of windows to cycle through.
   //  It would be nice to be able to detect whether an object is
   //  created dynamically (i.e., with the 'new' operator), and in
   //  that case, have cxWindow automatically call addWindow on the
   //  parent if it's a cxPanel, but I don't know if there's a way
   //  in C++ to tell if an object was created dynamically.

   shared_ptr<cxForm> iForm = make_shared<cxForm>(nullptr, 0, 0, 5, 20, "cxForm 1");
   iForm->append(1, 1, 1, 18, "Input 1:");
   iForm->append(2, 1, 1, 18, "Input 2:");
   iForm->append(3, 1, 1, 18, "Input 3:");
   // Set the F7 key on form 1 to change the window focus to the next
   //  window on the panel.
   iForm->setKeyFunction(KEY_F(7), changeWindowFocus, &iPanel, nullptr, nullptr, nullptr, true);
   // Set up simple onFocus and onLeave functions for the form
   iForm->setOnFocusFunction(testOnFocusFunction, nullptr, nullptr, nullptr, nullptr,
                             false, false);
   iForm->setOnLeaveFunction(testOnLeaveFunction, nullptr, nullptr, nullptr, nullptr);
   iPanel.append(iForm, 0, 12);

   shared_ptr<cxMultiLineInput> iInput = make_shared<cxMultiLineInput>(nullptr, 0, 0, 3, 10, "Input:");
   iInput->setTitle("The input");
   iPanel.append(iInput, 5, 24);

   shared_ptr<cxForm> iForm2 = make_shared<cxForm>(nullptr, 0, 0, 5, 20, "cxForm 2");
   iForm2->append(1, 1, 1, 18, "Input 1:");
   iForm2->append(2, 1, 1, 18, "Input 2:");
   iForm2->append(3, 1, 1, 18, "Input 3:");
   iPanel.append(iForm2, 7, 3);

   shared_ptr<cxMenu> iMenu = make_shared<cxMenu>(nullptr, 0, 0, 8, 14, "cxMenu");
   for (long returnCode = 1; returnCode <= 30; ++returnCode)
   {
      iMenu->append("Item " + cxStringUtils::toString(returnCode), returnCode, "Item " +
                    cxStringUtils::toString(returnCode));
   }
   iPanel.append(iMenu, 8, 25);

   // Test windowIsEnabled() with an index
   if (iPanel.windowIsEnabled(2))
   {
      messageBox("Window 2 in the panel is enabled.");
   }
   else
   {
      messageBox("Window 2 in the panel is not enabled!");
   }

   // Even if you pass the cxPanel pointer as the parent window to
   //  another cxWindow, the window isn't part of the panel unless
   //  you call the panel's append() to add it (also making sure
   //  that the window was created dynamically with 'new').

   cxWindow someWindow(&iPanel, 10, 45, 7, 15, "Test", "This window is not actually in the panel");

   // Test getWindow() with an index
   messageBox("Here is just the multi-line input by itself.");
   std::shared_ptr<cxWindow> win = iPanel.getWindow(2);
   win->showModal();

   //iPanel.setExitOnLeaveLast(true);
   iPanel.setExitOnLeaveFirst(true);
   iPanel.setCurrentWindow(1); // Let the form 1 have the focus
   // Swap windows 1 and 3 (form 2 will then get focus first)
   iPanel.swap(1, 3);
   iPanel.showModal();

   // Test moving the panel
   iPanel.move(3, 4, false);
   iPanel.showModal();
   // Hide the panel
   messageBox("The panel will now be hidden; after it's hidden, press a key to continue.");
   iPanel.hide();
   getch();
   iPanel.showModal();
   // Delete one of the windows
   messageBox("The menu will now be deleted from the panel.");
   iPanel.delWindow(4);
   long retval = iPanel.showModal();
   messageBox("Return code: " + getReturnCodeStr(retval));
   cxForm *form = dynamic_cast<cxForm*>(iPanel.getWindow(1).get());
   messageBox("Value of form 1 input 1:" + form->getValue(0) + ":");
   form = dynamic_cast<cxForm*>(iPanel.getWindow(3).get());
   messageBox("Value of form 2 input 1:" + form->getValue(0) + ":");
   cxMultiLineInput *input = static_cast<cxMultiLineInput*>(iPanel.getWindow(2).get());
   messageBox("Value of the lone input:" + input->getValue() + ":");

   // Note: We don't need to free the memory used by the windows
   //  added to the panel, because the cxPanel destructor will do
   //  that for us.
} // cxPanelTest

void cxPanelTest2()
{
   // This tests a cxPanel with a layout as in cprime's entry screens, with
   //  a read-only form at the top, a menu in the middle, and a form at the
   //  bottom.  Note: The order of appending the windows to the panel is
   //  important.
   cxPanel iPanel(nullptr, 1, 0, cxBase::height()-2, cxBase::width(), "Panel", "",
                  "", eBS_SINGLE_LINE);
   // Add a form with all read-only inputs at the top of the panel
   shared_ptr<cxForm> infoForm = make_shared<cxForm>(nullptr, 0, 0, 1, iPanel.width()-2, "Info form", eBS_NOBORDER);
   infoForm->append(0, 0, 1, 20, "Name:", "", "", eINPUT_READONLY);
   infoForm->append(0, 22, 1, 20, "Addr:", "", "", eINPUT_READONLY);
   infoForm->setValue(0, "Bob", false);
   infoForm->setValue(1, "123 A St.", false);
   infoForm->setCanBeEditable(0, false);
   infoForm->setCanBeEditable(1, false);
   // Have this form not wait for user input if there are no editable inputs.
   infoForm->setWaitForInputIfEmpty(false);
   iPanel.append(infoForm, 1, 1, false);
   // Add a form for user input
   shared_ptr<cxForm> inputForm = make_shared<cxForm>(nullptr, 0, 0, 4, iPanel.width(), "Input form");
   inputForm->append(1, 1, 1, 20, "Item#:");
   inputForm->append(2, 1, 1, 20, "Price:");
   inputForm->append(1, 22, 1, 20, "Cost:");
   inputForm->append(2, 22, 1, 20, "Weight:");
   iPanel.append(inputForm, iPanel.height() - inputForm->height(), 0, false);
   // Add a menu below the informational form
   int inputFormHeight = inputForm->height();
   int menuHeight = iPanel.height() - infoForm->height() - inputFormHeight - 1;
   shared_ptr<cxMenu> iMenu = make_shared<cxMenu>(nullptr, 0, 0, menuHeight, iPanel.width(), "Menu");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu->append("Item " + toString(i), i, "", cxITEM_NORMAL, false);
   }
   iPanel.append(iMenu, 2, 0, false);

   // Show the panel and wait for input
   iPanel.showModal();
}

void cxPanelDelAllWindowsTest()
{
   // This tests cxPanel's delAllWindows().  The app shouldn't crash/segfault,
   //  and there shouldn't be any memory leaks.
   cxPanel iPanel(nullptr, 2, 1, 20, 75);
   iPanel.setTitle("The panel"); // Could help with debugging

   // Add various other windows to the panel
   // We'll use iPanel.append() to add the following window to
   // the panel
   shared_ptr<cxWindow> iWindow = make_shared<cxWindow>(nullptr, 0, 0, 5, 10,
                                                        "cxWindow", "This is a cxWindow.");
   iPanel.append(iWindow, 1, 1);

   // We can pass a pointer to the panel to other windows as their
   //  parent, but we still need to call addWindow on the panel to
   //  add the window to its list of windows to cycle through.
   //  It would be nice to be able to detect whether an object is
   //  created dynamically (i.e., with the 'new' operator), and in
   //  that case, have cxWindow automatically call addWindow on the
   //  parent if it's a cxPanel, but I don't know if there's a way
   //  in C++ to tell if an object was created dynamically.

   shared_ptr<cxForm> iForm = make_shared<cxForm>(nullptr, 0, 0, 5, 20, "cxForm 1");
   iForm->append(1, 1, 1, 18, "Input 1:");
   iForm->append(2, 1, 1, 18, "Input 2:");
   iForm->append(3, 1, 1, 18, "Input 3:");
   // Set the F7 key on form 1 to change the window focus to the next
   //  window on the panel.
   iForm->setKeyFunction(KEY_F(7), changeWindowFocus, &iPanel, nullptr, nullptr, nullptr, true);
   // Set up simple onFocus and onLeave functions for the form
   iForm->setOnFocusFunction(testOnFocusFunction, nullptr, nullptr, nullptr, nullptr,
                             false, false);
   iForm->setOnLeaveFunction(testOnLeaveFunction, nullptr, nullptr, nullptr, nullptr);
   iPanel.append(iForm, 0, 12);

   shared_ptr<cxMultiLineInput> iInput = make_shared<cxMultiLineInput>(nullptr, 0, 0, 3, 10, "Input:");
   iInput->setTitle("The input");
   iPanel.append(iInput, 5, 24);

   shared_ptr<cxForm> iForm2 = make_shared<cxForm>(nullptr, 0, 0, 5, 20, "cxForm 2");
   iForm2->append(1, 1, 1, 18, "Input 1:");
   iForm2->append(2, 1, 1, 18, "Input 2:");
   iForm2->append(3, 1, 1, 18, "Input 3:");
   iPanel.append(iForm2, 7, 3);

   shared_ptr<cxMenu> iMenu = make_shared<cxMenu>(nullptr, 0, 0, 8, 14, "cxMenu");
   for (long returnCode = 1; returnCode <= 30; ++returnCode)
   {
      iMenu->append("Item " + cxStringUtils::toString(returnCode), returnCode, "Item " +
                    cxStringUtils::toString(returnCode));
   }
   iPanel.append(iMenu, 8, 25);

   cxWindow someWindow(&iPanel, 10, 45, 7, 15, "Test", "This window is not actually in the panel");

   iPanel.delAllWindows();

   iPanel.showModal();
} // cxPanelDelAlLWindowsTest

void cxWindowWithFunctionKeys()
{
   string message = "Try pressing F1 and F2.  All other keys will exit this window.";
   cxWindow iWindow(nullptr, 1, 0, 14, 70, "Test window", message);
   iWindow.setKeyFunction(KEY_F(1), sayHello, nullptr, nullptr, false, false);
   iWindow.setKeyFunction(KEY_F(2), sayHello2, nullptr, nullptr, false, false);
   long returnCode = iWindow.showModal();
   messageBox("Return code: " + getReturnCodeStr(returnCode));
} // cxWindowWithFunctionKeys

void cxWindowWithMultipleTitleStrings()
{
   cxWindow iWindow(nullptr, 1, 0, 7, 75, "",
                    "There should be multiple title strings.",
                    "", eBS_SINGLE_LINE);
   iWindow.addTitleString(1, "At 1", false);
   iWindow.addTitleString(15, "At 15", false);
   iWindow.addTitleString(32, "At 32", false);
   iWindow.showModal();
} // cxWindowWithMultipleTitleStrings

void cxWindowWithMultipleStatusStrings()
{
   cxWindow iWindow(nullptr, 1, 0, 10, 50, "Test",
                    "There should be multiple status strings.", "");
   iWindow.addStatusString(1, "At 1", false);
   iWindow.addStatusString(8, "At 8", false);
   iWindow.addStatusString(25, "At 25", false);
   iWindow.showModal();
} // cxWindowWithMultipleStatusStrings

string allMenuItemsUnselectable(void *menu, void *unused)
{
   if (menu == nullptr)
   {
      return("");
   }

   cxMenu *pMenu = static_cast<cxMenu*>(menu);
   unsigned numItems = pMenu->numMenuItems();
   for (unsigned i = 0; i < numItems; ++i)
   {
      pMenu->toggleSelectability(i, false);
   }

   return("");
} // allMenuItemsUnselectable

string clearAllMenuItems(void *menu, void *unused)
{
   if (menu == nullptr)
   {
      return("");
   }

   cxMenu *pMenu = static_cast<cxMenu*>(menu);
   pMenu->removeAllItems(false, true);

   return("");
} // clearAllMenuItems

void cxMenuNoMoreSelectableItemsWhileModal()
{
   messageBox("Press F11 to make all menu items un-editable or shift-F11 to clear all the items from the menu.  It should return immediately.");
   //cxMenu iMenu(nullptr, 1, 0, 15, 20, "Test");
   cxMenu iMenu(nullptr, 1, 0, 8, 20, "Test");
   for (int i = 0; i < 12; ++i)
   {
      iMenu.append("Item " + cxStringUtils::toString(i), i);
   }
   iMenu.setKeyFunction(KEY_F(11), allMenuItemsUnselectable, &iMenu, nullptr, false, false);
   iMenu.setKeyFunction(SHIFT_F11, clearAllMenuItems, &iMenu, nullptr, false, false);
   iMenu.showModal();
} // cxMenuNoMoreSelectableItemsWhileModal

void cxPanelWithNonEditableSubwindows()
{
   // Set up a cxPanel with some subwindows that don't have anything
   //  editable on them - Before, this would result in an infinite loop in
   //  cxPanel's showModal(); this was fixed on 04/21/06.
   cxPanel iPanel(nullptr, 1, 0, 20, 80, "Test", "", "", eBS_SINGLE_LINE);
   iPanel.setExitOnLeaveLast(true);
   shared_ptr<cxMultiLineInput> iInput = make_shared<cxMultiLineInput>(nullptr, 2, 10, 1, 15, "Input:",
                                                                       eBS_NOBORDER, eINPUT_READONLY);
   iPanel.append(iInput);
   shared_ptr<cxPanel> panel2 = make_shared<cxPanel>(nullptr, 3, 5, 3, 50, "Panel 2", "", "", eBS_SINGLE_LINE);
   iPanel.append(panel2);
   shared_ptr<cxScrolledWindow> iSWindow = make_shared<cxScrolledWindow>(nullptr, 7, 5, 3, 50, "Scrolled window");
   iPanel.append(iSWindow);
   shared_ptr<cxWindow> iWindow = make_shared<cxWindow>(nullptr, 11, 5, 3, 50, "Window");
   iPanel.append(iWindow);
   iPanel.showModal();
} // cxPanelWithNonEditableSubwindows

void cxMenuWithDuplicateHotkeys()
{
   // Items 4 and 10 have the same hotkey.  The menu will have to scroll to
   //  show each of them when the user presses the 'i' key.  It should go
   //  to each of them, and there should be no scrolling issues.
   cxMenu iMenu(nullptr, 1, 0, 10, 10, "Test");
   iMenu.append("Item 1", 1);
   iMenu.append("Item 2", 2);
   iMenu.append("Item 3", 3);
   iMenu.append("&Item 4", 4);
   iMenu.append("Item 5", 5);
   iMenu.append("Item 6", 6);
   iMenu.append("Item 7", 7);
   iMenu.append("Item 8", 8);
   iMenu.append("Item 9", 9);
   iMenu.append("&Item 10", 10);
   iMenu.showModal();
} // cxMenuWithDuplicateHotkeys

void cxPanelWindowSwap()
{
   cxPanel iPanel(nullptr, 1, 0, 20, 80, "Panel");
   shared_ptr<cxMenu> menu1 = make_shared<cxMenu>(nullptr, 0, 0, 5, 10, "Menu 1");
   shared_ptr<cxMenu> menu2 = make_shared<cxMenu>(nullptr, 0, 0, 5, 10, "Menu 2");
   shared_ptr<cxMenu> menu3 = make_shared<cxMenu>(nullptr, 0, 0, 5, 10, "Menu 3");
   for (int i = 1; i <= 5; ++i)
   {
      menu1->append("Item " + cxStringUtils::toString(i), i);
      menu2->append("Item " + cxStringUtils::toString(i), i);
      menu3->append("Item " + cxStringUtils::toString(i), i);
   }
   iPanel.append(menu1, 1, 1, false);
   iPanel.append(menu2, 1, 11, false);
   iPanel.append(menu3, 1, 21, false);
   iPanel.swap(0, 1);
   // Now the tab order should be 2, 1, 3
   iPanel.swap(menu2, menu3);
   // Now the tab order should be 3, 1, 2
   iPanel.showModal();
} // cxPanelWindowSwap

void cxPanel_cxMenuUpArrowLeave()
{
   cxBase::messageBox("In menu 2, pressing up arrow on the first item will "
                       "go to menu 1.");

   cxPanel iPanel(nullptr, 1, 0, height()-2, width(), "Test panel", "", "",
                  eBS_SINGLE_LINE);
   shared_ptr<cxMenu> iMenu1 = make_shared<cxMenu>(nullptr, 0, 0, 7, width(), "Menu 1");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu1->append("Item " + cxStringUtils::toString(i), i);
   }
   iPanel.append(iMenu1, 1, 0, false);
   shared_ptr<cxMenu> iMenu2 = make_shared<cxMenu>(nullptr, 0, 0, 7, width(), "Menu 2");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu2->append("Item " + cxStringUtils::toString(i), i);
   }
   iPanel.append(iMenu2, 8, 0, false);
   iMenu2->setExitWhenLeaveFirst(true);
   shared_ptr<cxMenu> iMenu3 = make_shared<cxMenu>(nullptr, 0, 0, 7, width(), "Menu 3");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu3->append("Item " + cxStringUtils::toString(i), i);
   }
   iPanel.append(iMenu3, 15, 0, false);
   iPanel.showModal();
} // cxPanel_cxMenuUpArrowLeave

string getMenuItemIDStr(long pMenuItemID)
{
   // Default the string version to whatever cxBase::getReturnCodeStr()
   //  says, in case the ID code isn't handeled in the switch.
   string menuItemIDStr = cxBase::getReturnCodeStr(pMenuItemID);

   switch (pMenuItemID)
   {
      case menuCode:
         menuItemIDStr = "menuCode";
         break;
      case menuItemWithMultipleHotkeyCode:
         menuItemIDStr = "menuItemWithMultipleHotkeyCode";
         break;
      case menuScrollingCode:
         menuItemIDStr = "menuScrollingCode";
         break;
      case inputWithFKeysCode:
         menuItemIDStr = "inputWithFKeysCode";
         break;
      case formWithFKeysCode:
         menuItemIDStr = "formWithFKeysCode";
         break;
      case inputCode:
         menuItemIDStr = "inputCode";
         break;
      case inputMaskingCode:
         menuItemIDStr = "inputMaskingCode";
         break;
      case multiInputMaskingCode:
         menuItemIDStr = "multiInputMaskingCode";
         break;
      case multiLineInputLabelChangeCode:
         menuItemIDStr = "multiLineInputLabelChangeCode";
         break;
      case inputLabelChangeCode:
         menuItemIDStr = "inputLabelChangeCode";
         break;
      case msgOKCode:
         menuItemIDStr = "msgOKCode";
         break;
      case msgOKCancelCode:
         menuItemIDStr = "msgOKCancelCode";
         break;
      case msgCancelCode:
         menuItemIDStr = "msgCancelCode";
         break;
      case msgYesCode:
         menuItemIDStr = "msgYesCode";
         break;
      case msgYesNoCode:
         menuItemIDStr = "msgYesNoCode";
         break;
      case msgNoYesCode:
         menuItemIDStr = "msgNoYesCode";
         break;
      case msgNoCode:
         menuItemIDStr = "msgNoCode";
         break;
      case windowCode:
         menuItemIDStr = "windowCode";
         break;
      case extTitleWinCode:
         menuItemIDStr = "extTitleWinCode";
         break;
      case extStatusWinCode:
         menuItemIDStr = "extStatusWinCode";
         break;
      case msgBox1Code:
         menuItemIDStr = "msgBox1Code";
         break;
      case msgBox2Code:
         menuItemIDStr = "msgBox2Code";
         break;
      case msgBox3Code:
         menuItemIDStr = "msgBox3Code";
         break;
      case msgBox4Code:
         menuItemIDStr = "msgBox4Code";
         break;
      case splash1Code:
         menuItemIDStr = "splash1Code";
         break;
      case splash2Code:
         menuItemIDStr = "splash2Code";
         break;
      case fileViewerCode:
         menuItemIDStr = "fileViewerCode";
         break;
      case attributesSetterCode:
         menuItemIDStr = "attributesSetterCode";
         break;
      case singleLineInputCode:
         menuItemIDStr = "singleLineInputCode";
         break;
      case multiLineInputCode:
         menuItemIDStr = "multiLineInputCode";
         break;
      case exitCode:
         menuItemIDStr = "exitCode";
         break;
      case someItem:
         menuItemIDStr = "someItem";
         break;
      case formCode:
         menuItemIDStr = "formCode";
         break;
      case stackedFormScrollingCode:
         menuItemIDStr = "stackedFormScrollingCode";
         break;
      case stackedFormScrollingCode2:
         menuItemIDStr = "stackedFormScrollingCode2";
         break;
      case titleAlignmentCode:
         menuItemIDStr = "titleAlignmentCode";
         break;
      case statusAlignmentCode:
         menuItemIDStr = "statusAlignmentCode";
         break;
      case cxWindowCopyConstructorCode:
         menuItemIDStr = "cxWindowCopyConstructorCode";
         break;
      case panelDemoCode:
         menuItemIDStr = "panelDemoCode";
         break;
      case inputsWithDifferentColorsCode:
         menuItemIDStr = "inputsWithDifferentColorsCode";
         break;
      case formInputColorsCode:
         menuItemIDStr = "formInputColorsCode";
         break;
      case borderlessFormCode:
         menuItemIDStr = "borderlessFormCode";
         break;
      case testFormReadOnlyCode:
         menuItemIDStr = "testFormReadOnlyCode";
         break;
      case formWithInputValueValidatorCode:
         menuItemIDStr = "formWithInputValueValidatorCode";
         break;
      case testFormPosCode:
         menuItemIDStr = "testFormPosCode";
         break;
      case formWithMenuCode:
         menuItemIDStr = "formWithMenuCode";
         break;
      case cxWindowAssignmentCode:
         menuItemIDStr = "cxWindowAssignmentCode";
         break;
      case cxMenuAssignmentCode:
         menuItemIDStr = "cxMenuAssignmentCode";
         break;
      case cxFormAssignmentCode:
         menuItemIDStr = "cxFormAssignmentCode";
         break;
      case multiLineInputResizeCode:
         menuItemIDStr = "multiLineInputResizeCode";
         break;
      case setValFuncCode:
         menuItemIDStr = "setValFuncCode";
         break;
      case changeMenuItemTextCode:
         menuItemIDStr = "changeMenuItemTextCode";
         break;
      case windowWriteTextCode:
         menuItemIDStr = "windowWriteTextCode";
         break;
      case formChangeInputFocusWhileModalCode:
         menuItemIDStr = "formChangeInputFocusWhileModalCode";
         break;
      case allFormInputsGoReadOnlyCode:
         menuItemIDStr = "allFormInputsGoReadOnlyCode";
         break;
      case formNoQuitCode:
         menuItemIDStr = "formNoQuitCode";
         break;
      case menuNoQuitCode:
         menuItemIDStr = "menuNoQuitCode";
         break;
      case scrolledWindowCode:
         menuItemIDStr = "scrolledWindowCode";
         break;
      case cxInputBorderChangeCode:
         menuItemIDStr = "cxInputBorderChangeCode";
         break;
      case cxMultiLineInputBorderChangeCode:
         menuItemIDStr = "cxMultiLineInputBorderChangeCode";
         break;
      case comboBoxCode:
         menuItemIDStr = "comboBoxCode";
         break;
      case comboBoxOnFormCode:
         menuItemIDStr = "comboBoxOnFormCode";
         break;
      case inputWithoutBorderCode:
         menuItemIDStr = "inputWithoutBorderCode";
         break;
      case formDataChangedCode:
         menuItemIDStr = "formDataChangedCode";
         break;
      case integerValidateCode:
         menuItemIDStr = "integerValidateCode";
         break;
      case floatingPtValidateCode:
         menuItemIDStr = "floatingPtValidateCode";
         break;
      case enableDisableFormCode:
         menuItemIDStr = "enableDisableFormCode";
         break;
      case multiFormWithDisabledSubformCode:
         menuItemIDStr = "multiFormWithDisabledSubformCode";
         break;
      case comboBoxMenuDisableCode:
         menuItemIDStr = "comboBoxMenuDisableCode";
         break;
      case multiForm2Code:
         menuItemIDStr = "multiForm2Code";
         break;
      case inputQuitKeysOnFormCode:
         menuItemIDStr = "inputQuitKeysOnFormCode";
         break;
      case cxMenuFocusFunctionsCode:
         menuItemIDStr = "cxMenuFocusFunctionsCode";
         break;
      case loneInputWithLoopDisabledOnFormCode:
         menuItemIDStr = "loneInputWithLoopDisabledOnFormCode";
         break;
      case cxMenuWithMultipleItemHotkeysCode:
         menuItemIDStr = "cxMenuWithMultipleItemHotkeysCode";
         break;
      case cxMenuItemHotkeysOffMenuCode:
         menuItemIDStr = "cxMenuItemHotkeysOffMenuCode";
         break;
      case cxPanelCode:
         menuItemIDStr = "cxPanelCode";
         break;
      case cxPanelCode2:
         menuItemIDStr = "cxPanelCode2";
         break;
       case cxPanelDelAllWindowsCode:
         menuItemIDStr = "cxPanelDelAllWindowsCode";
         break;
      case cxWindowWithFunctionKeysCode:
         menuItemIDStr = "cxWindowWithFunctionKeysCode";
         break;
      case cxWindowWithMultipleTitleStringsCode:
         menuItemIDStr = "cxWindowWithMultipleTitleStringsCode";
         break;
      case cxWindowWithMultipleStatusStringsCode:
         menuItemIDStr = "cxWindowWithMultipleStatusStringsCode";
         break;
      case cxMenuNoMoreSelectableItemsWhileModalCode:
         menuItemIDStr = "cxMenuNoMoreSelectableItemsWhileModalCode";
         break;
      case cxPanelWithNonEditableSubwindowsCode:
         menuItemIDStr = "cxPanelWithNonEditableSubwindowsCode";
         break;
      case cxMenuWithDuplicateHotkeysCode:
         menuItemIDStr = "cxMenuWithDuplicateHotkeysCode";
         break;
      case cxPanelWindowSwapCode:
         menuItemIDStr = "cxPanelWindowSwapCode";
         break;
      case cxPanel_cxMenuUpArrowLeaveCode:
         menuItemIDStr = "cxPanel_cxMenuUpArrowLeaveCode";
         break;
      case windowFunctionKeyNoOnLeaveCode:
         menuItemIDStr = "windowFunctionKeyNoOnLeaveCode";
         break;
      case formFunctionKeyNoOnLeaveCode:
         menuItemIDStr = "formFunctionKeyNoOnLeaveCode";
         break;
      case cxMultiLineInputFunctionKeyNoValidatorCode:
         menuItemIDStr = "cxMultiLineInputFunctionKeyNoValidatorCode";
         break;
      case cxMultiLineInputNumericFloatingPtCode:
         menuItemIDStr = "cxMultiLineInputNumericFloatingPtCode";
         break;
      case cxMultiLineInputNumericFloatingPtWithRangeCode:
         menuItemIDStr = "cxMultiLineInputNumericFloatingPtWithRangeCode";
         break;
      case cxMultiLineInputNumericWholeCode:
         menuItemIDStr = "cxMultiLineInputNumericWholeCode";
         break;
      case cxMultiLineInputNumericWholeWithRangeCode:
         menuItemIDStr = "cxMultiLineInputNumericWholeWithRangeCode";
         break;
      case cxMultiLineInputTextValidationCode:
         menuItemIDStr = "cxMultiLineInputTextValidation";
         break;
      case cxMultiLineInputForceUpperCode:
         menuItemIDStr = "cxMultiLineInputForceUpperCode";
         break;
      case cxMultiLineInputNonBlankCode:
         menuItemIDStr = "cxMultiLineInputNonBlankCode";
         break;
      case cxMultiLineInputTextValidationNonBlankForceUpperCode:
         menuItemIDStr = "cxMultiLineInputTextValidationNonBlankForceUpperCode";
         break;
      case cxMultiLineInputTextValidationAutoCompletionCode:
         menuItemIDStr = "cxMultiLineInputTextValidationAutoCompletionCode";
         break;
      case cxMultiLineInputExtendedHelpKeyCode:
         menuItemIDStr = "cxMultiLineInputExtendedHelpKeyCode";
      case cxFormExtendedHelpKeyCode:
         menuItemIDStr = "cxFormExtendedHelpKeyCode";
         break;
      case cxFormMultiLineInputFunctionKeyNoValidatorCode:
         menuItemIDStr = "cxFormMultiLineInputFunctionKeyNoValidatorCode";
         break;
      case keyFunctionAutoExitOnLastFormInputCode:
         menuItemIDStr = "keyFunctionAutoExitOnLastFormInputCode";
         break;
      case cxFormNavKeysCode:
         menuItemIDStr = "cxFormNavKeysCode";
         break;
      case cxFormStartAtFirstInputCode:
         menuItemIDStr = "cxFormStartAtFirstInputCode";
         break;
      case cxFormClearOnlyEditableCode:
         menuItemIDStr = "cxFormClearOnlyEditable";
         break;
      case cxPanelGetLastWindowCode:
         menuItemIDStr = "cxPanelGetLastWindowCode";
         break;
      case cxMenuMiscTestsCode:
         menuItemIDStr = "cxMenuMiscTestsCode";
         break;
      case cxSearchPanelTestCode:
         menuItemIDStr = "cxSearchPanelTestCode";
         break;
      case cxSearchPanelResizeCode:
         menuItemIDStr = "cxSearchPanelResizeCode";
         break;
      case cxSearchPanelCopyConstructorCode:
         menuItemIDStr = "cxSearchPanelCopyConstructorCode";
         break;
      case cxMenuResizeCode:
         menuItemIDStr = "cxMenuResizeCode";
         break;
      case cxWindowAlignCode:
         menuItemIDStr = "cxWindowAlignCode";
         break;
      case cxNotebookTest1Code:
         menuItemIDStr = "cxNotebookTest1Code";
         break;
      case cxNotebookTest2Code:
         menuItemIDStr = "cxNotebookTest2Code";
         break;
      case cxNotebookRemoveWindowTestCode:
         menuItemIDStr = "cxNotebookRemoveWindowTestCode";
         break;
      case cxNotebookWithEmptyPanelTestCode:
         menuItemIDStr = "cxNotebookWithEmptyPanelTestCode";
         break;
      case cxNotebookSetTabSpacingCode:
         menuItemIDStr = "cxNotebookSetTabSpacingCode";
         break;
      case cxNotebookSwapTestCode:
         menuItemIDStr = "cxNotebookSwapTestCode";
         break;
      case cxWindowBorderCode:
         menuItemIDStr = "cxWindowBorderCode";
         break;
      case cxWindowSizeCode:
         menuItemIDStr = "cxWindowSizeCode";
         break;
      case cxWindowAttributesCode:
         menuItemIDStr = "cxWindowAttributesCode";
         break;
      case cxWindowMouseFunctionTestCode:
         menuItemIDStr = "cxWindowMouseFunctionTestCode";
         break;
      case cxFormMouseFunctionTestCode:
         menuItemIDStr = "cxFormMouseFunctionTestCode";
         break;
      case cxMenuMouseFunctionTestCode:
         menuItemIDStr = "cxMenuMouseFunctionTestCode";
         break;
      case cxPanelWithButtonTestCode:
         menuItemIDStr = "cxPanelWithButtonTestCode";
         break;
      case cxMultiLineInputWithRightLabelCode:
         menuItemIDStr = "cxMultiLineInputWithRightLabelCode";
         break;
      case cxMultiLineInputAutoGenerateHelpStringCode:
         menuItemIDStr = "cxMultiLineInputAutoGenerateHelpString";
         break;
      case itemSalesCode:
         menuItemIDStr = "itemSales";
         break;
      case cxFormAppendPairCode:
         menuItemIDStr = "cxFormAppendPairCode";
         break;
      case cxFormAppendComboBoxPairCode:
         menuItemIDStr = "cxFormAppendComboBoxPairCode";
         break;
      case cxMultiLineInputWithRightLabelOnFormCode:
         menuItemIDStr = "cxMultiLineInputWithRightLabelOnFormCode";
         break;
      case cxMultiLineInputSetValueCode:
         menuItemIDStr = "cxMultiLineInputSetValueCode";
         break;
      case cxMultiLineInputMaxInputLengthCode:
         menuItemIDStr = "cxMultiLineInputMaxInputLengthCode";
         break;
      case cxMultiLineInputExtendedHelpColorsAndAttrsCode:
         menuItemIDStr = "cxMultiLineInputExtendedHelpColorsAndAttrsCode";
         break;
      case cxMenuAltItemTextCode:
         menuItemIDStr = "cxMenuAltItemTextCode";
         break;
      case cxComboBoxNotEditableMenuEnabledCode:
         menuItemIDStr = "cxComboBoxNotEditableMenuEnabledCode";
         break;
      case cxMultiLineInputOverrideOnKeypressCode:
         menuItemIDStr = "cxMultiLineInputOverrideOnKeypressCode";
         break;
      case cxWindowAddMessageLinesAboveCode:
         menuItemIDStr = "cxWindowAddMessageLinesAboveCode";
         break;
      case cxWindowBorderStyles:
         menuItemIDStr = "cxWindowBorderStyles";
         break;
         break;
      case cxDatePickerTestCode:
         menuItemIDStr = "cxDatePickerTestCode";
         break;
      case cxOpenFileDialogTestCode:
         menuItemIDStr = "cxOpenFileDialogTestCode";
         break;
   }

   return(menuItemIDStr);
} // getMenuItemIDStr

void windowFunctionKeyNoOnLeave()
{
   cxBase::messageBox("The next window has an onLeave function.  It will be "
                      "fired after any keypress except F1, which fires a key "
                      "function and is set not to run the onLeave function.");
   cxWindow iWindow(nullptr, 1, 0, 12, 50, "Test", "This is a test window.");
   iWindow.setOnLeaveFunction(testOnLeaveFunction, nullptr, nullptr, nullptr, nullptr);
   iWindow.setKeyFunction(KEY_F(1), testKeyFunction, nullptr, nullptr, false, true, false);
   iWindow.showModal();
} // windowFunctionKeyNoOnLeave

void formFunctionKeyNoOnLeave()
{
   cxBase::messageBox("The form here has an onLeave function.  It will be "
                      "fired when you tab through the inputs on the form.  F1 "
                      "fires a key function and is set to leave and not run "
                      "the onLeave function.");
   cxForm iForm(nullptr, 1, 0, 12, 70, "Test");
   iForm.setAutoExit(true);
   iForm.append(1, 1, 1, 25, "Input 1:");
   iForm.append(2, 1, 1, 25, "Input 2:");
   iForm.append(1, 27, 1, 25, "Input 3:");
   iForm.append(2, 27, 1, 25, "Input 4:");
   iForm.setOnLeaveFunction(testOnLeaveFunction, nullptr, nullptr, nullptr, nullptr);
   iForm.setKeyFunction(KEY_F(1), testKeyFunction, nullptr, nullptr, false, true, false);
   iForm.showModal();
} // windowFunctionKeyNoOnLeave

void cxMultiLineInputFunctionKeyNoValidator()
{
   cxBase::messageBox("The input has a validator that validates that you've "
                      "typed \"test\".  F1 is set up to run a function and "
                      "have the input leave focus without running the validator.");
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 30, "Input:");
   iInput.setValidatorFunction(testInputValidator, &iInput, nullptr, nullptr, nullptr);
   iInput.setKeyFunction(KEY_F(1), testKeyFunction, nullptr, nullptr, false, true, false, false);
   iInput.showModal();
} // cxMultiLineInputFunctionKeyNoValidator

void cxFormMultiLineInputFunctionKeyNoValidator()
{
   cxBase::messageBox("Input 1 on the form has an onLeave function and a "
                      "validator.  F1 on input 1 fires a key function and is "
                      "set to leave input 1 and not run its onLeave function "
                      "or its validator.");
   cxForm iForm(nullptr, 1, 0, 12, 70, "Test");
   iForm.setAutoExit(true);
   shared_ptr<cxMultiLineInput> iInput = iForm.append(1, 1, 1, 25, "Input 1:");
   iForm.append(2, 1, 1, 25, "Input 2:");
   iForm.append(1, 27, 1, 25, "Input 3:");
   iForm.append(2, 27, 1, 25, "Input 4:");
   iForm.setOnLeaveFunction("Input 1:", testOnLeaveFunction, nullptr, nullptr, nullptr,
                            nullptr, true);
   iInput->setValidatorFunction(testInputValidator, iInput.get(), nullptr, nullptr, nullptr);
   iForm.setKeyFunction("Input 1:", KEY_F(1), testKeyFunction, nullptr, nullptr,
                        false, true, false, false);
   iForm.showModal();
} // cxFormMultiLineInputFunctionKeyNoValidator

void cxFormNavKeys()
{
   cxForm iForm(nullptr, 1, 0, 10, 60, "Test form");
   iForm.append(1, 1, 1, 19, "Input 1:");
   iForm.append(2, 1, 1, 19, "Input 2:");
   iForm.append(3, 1, 1, 19, "Input 3:");
   iForm.append(1, 21, 1, 19, "Input 4:");
   iForm.append(2, 21, 1, 19, "Input 5:");
   iForm.append(3, 21, 1, 19, "Input 6:");
   // Get the navigational keys from the form
   set<int> navKeys;
   iForm.getNavKeys(navKeys);
   string navKeysStr;
   set<int>::const_iterator keyIter = navKeys.begin();
   for (; keyIter != navKeys.end(); ++keyIter)
   {
      navKeysStr += " " + cxBase::getKeyStr(*keyIter);
   }
   cxBase::messageBox("cxForm::getNavKeys() returns:" + navKeysStr);
   // Test cxForm::getNavKeyStrings()
   navKeysStr = "";
   set<string> navKeyStrings;
   iForm.getNavKeyStrings(navKeyStrings);
   set<string>::const_iterator strIter = navKeyStrings.begin();
   for (; strIter != navKeyStrings.end(); ++strIter)
   {
      navKeysStr += " " + *strIter;
   }
   cxBase::messageBox("cxForm::getNavKeyStrings() returns:" + navKeysStr);
   cxBase::messageBox("Is pageDown a nav key on the form?: " +
                string(iForm.hasNavKey(PAGE_DOWN) ? "Yes" : "No"));
} // cxFormNavKeys

void addMessageLine()
{
   cxWindow iWindow(nullptr, 1, 0, 10, 70, "Test");
   iWindow.addMessageLineBelow("Line 1");
   iWindow.addMessageLineBelow("Line 2");
   iWindow.addMessageLineBelow("Line 3");
   iWindow.showModal();
   iWindow.removeMessageLine(1);
   iWindow.showModal();
} // addMessageLine

void cxFormStartAtFirstInput()
{
   cxForm iForm(nullptr, 1, 0, 10, 60, "Test form");
   iForm.append(1, 1, 1, 19, "Input 1:");
   iForm.append(2, 1, 1, 19, "Input 2:");
   iForm.append(3, 1, 1, 19, "Input 3:");
   iForm.append(1, 21, 1, 19, "Input 4:");
   iForm.append(2, 21, 1, 19, "Input 5:");
   iForm.append(3, 21, 1, 19, "Input 6:");
   iForm.setAutoExit(true);
   iForm.setStartAtFirstInput(true);
   iForm.showModal();
   cxBase::messageBox("showing again");
   iForm.showModal();
} // cxFormStartAtFirstInput

// Event method to help with cxPanelGetLastWindow()
string onFocusWinForCxPanelGetLastPage(void *thePanel, void *theCurrentWindow,
                             void *unused2, void *unused3)
                             {
   if ((nullptr == thePanel) || (nullptr == theCurrentWindow))
   {
      return("");
   }

   cxPanel *pPanel = static_cast<cxPanel*>(thePanel);
   cxWindow *pCurrentWindow = static_cast<cxWindow*>(theCurrentWindow);

   shared_ptr<cxWindow> lastWindow = pPanel->getLastWindow();
   if (nullptr != lastWindow)
   {
      cxBase::messageBox("On focus of this window:" + pCurrentWindow->getTitle()
             + ":, the last window title is:" + lastWindow->getTitle() + ":");
   }
   else
   {
      cxBase::messageBox("On focus of this window:" + pCurrentWindow->getTitle()
                         + ":, the last window is nullptr.");
   }

   return("");
} // onFocusWinForCxPanelGetLastPage

void cxFormClearOnlyEditable()
{
   cxForm iForm(nullptr, 1, 0, 20, 70, "Form");
   iForm.append(1, 1, 1, 20, "Input 1:");
   iForm.append(2, 1, 1, 20, "Input 2:");
   iForm.append(3, 1, 1, 20, "Input 3:");
   iForm.append(1, 22, 1, 20, "Input 4:");
   iForm.append(2, 22, 1, 20, "Input 5:");
   iForm.append(3, 22, 1, 20, "Input 6:");
   iForm.setAutoExit(true);
   iForm.setInputOption(3, eINPUT_READONLY);
   iForm.setInputOption(4, eINPUT_READONLY);
   iForm.setInputOption(5, eINPUT_READONLY);
   for (int i = 0; i < 6; ++i)
   {
      iForm.setValue(i, "Value", false);
   }
   iForm.clearInputs(false, true);
   iForm.showModal();
}

// Tester for cxPanel::getLastPage()
void cxPanelGetLastWindow()
{
   cxPanel iPanel(nullptr, 1, 0, 21, 80, "Main panel", "", "", eBS_SINGLE_LINE);

   shared_ptr<cxMenu> iMenu = make_shared<cxMenu>(nullptr, 0, 0, 8, 80, "Menu");
   for (int i = 1; i <= 10; ++i)
   {
      iMenu->append("Item " + cxStringUtils::toString(i), i);
   }
   iMenu->setOnFocusFunction(onFocusWinForCxPanelGetLastPage, &iPanel, iMenu.get(),
                             nullptr, nullptr);
   iPanel.append(iMenu, 1, 0, false);

   shared_ptr<cxForm> iForm = make_shared<cxForm>(nullptr, 0, 0, 5, 80, "Form");
   iForm->append(1, 1, 1, 20, "Input 1:");
   iForm->append(2, 1, 1, 20, "Input 2:");
   iForm->append(3, 1, 1, 20, "Input 3:");
   iForm->append(1, 22, 1, 20, "Input 4:");
   iForm->append(2, 22, 1, 20, "Input 5:");
   iForm->append(3, 22, 1, 20, "Input 6:");
   iForm->setOnFocusFunction(onFocusWinForCxPanelGetLastPage, &iPanel, iForm.get(),
                             nullptr, nullptr);
   iPanel.append(iForm, 9, 0, false);

   // Show the window..  The first time the panel is shown, its last window
   //  should be nullptr.
   iPanel.showModal();

   // Show the panel again..  The last window should be the window that had
   //  focus the last time it was shown.
   cxBase::messageBox("Showing the panel again..");
   iPanel.showModal();
} // cxPanelGetLastWindow

// This function will be used for the cxButton's onClick function in the following
//  test.
string buttonClickFunction(void *theButton, void *unused, void *unused2, void *unused3)
{
   cxBase::messageBox("Button click function called");
   return("");
} // buttonClickFunction

// Tester for a cxButton used with an onClick event function
void cxButtonOnClick()
{
   cxBase::messageBox("If you press enter or click on the next button, its "
                      "onClick function will fire.");

   cxButton iButton(nullptr, 1, 0, 3, 10, "Button");
   iButton.setOnClickFunction(buttonClickFunction, &iButton, nullptr, nullptr, nullptr);
   iButton.showModal();
} // cxButtonOnClick

void cxMenuMiscTests()
{
   {
      // Test getItemTextByReturnCode()
      cxMenu iMenu(nullptr, 0, 0, 10, 16, "Test menu");
      for (unsigned i = 0; i < 5; ++i)
      {
         iMenu.append("Item " + toString(i), i);
      }
      // Good tests
      assert(iMenu.getItemTextByReturnCode(0) == "Item 0");
      assert(iMenu.getItemTextByReturnCode(1) == "Item 1");
      assert(iMenu.getItemTextByReturnCode(2) == "Item 2");
      assert(iMenu.getItemTextByReturnCode(3) == "Item 3");
      assert(iMenu.getItemTextByReturnCode(4) == "Item 4");
      // Out of bounds tests
      assert(iMenu.getItemTextByReturnCode(5) == "");
      assert(iMenu.getItemTextByReturnCode(-1) == "");
   }

   cxBase::messageBox("If you see this, the tests passed!");
} // cxMenuMiscTests

// Event functions for search panels
string onFocusForm(void *theSearchPanel, void *unused, void *unused2,
                   void *unused3)
                   {
   //cxBase::messageBox("onFocusForm");
   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   pSearchPanel->getMenu()->setStatus("ESC=Quit  F4=Goto  ENTER=Go  TAB=Next  sTAB=Previous", true);
   //pSearchPanel->getMenu()->setStatus("", true);
   //pSearchPanel->clearMenu(true);

   return("");
} // onFocusForm

string onLeaveForm(void *theSearchPanel, void *unused, void *unused2,
                   void *unused3)
                   {
   //cxBase::messageBox("onLeaveForm");
   //cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   //pSearchPanel->getForm()->setStatus("");
   /*
   pSearchPanel->appendToMenu("Item 1");
   pSearchPanel->appendToMenu("Item 2");
   pSearchPanel->appendToMenu("Item 3");
   pSearchPanel->appendToMenu("Item 4");
   */

   return("");
} // onLeaveForm

string onFocusMenu(void *theSearchPanel, void *unused, void *unused2,
                   void *unused3)
                   {
   //cxBase::messageBox("onFocusMenu");
   if (theSearchPanel == nullptr)
   {
      return("");
   }

   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   shared_ptr<cxMenu> iMenu = pSearchPanel->getMenu();
   shared_ptr<cxForm> iForm = pSearchPanel->getForm();
   iMenu->setStatus("TAB=Form  /=Search  HOME=First  END=Last  UP=Previous  DOWN=Next", true);
   //iForm->setStatus("", true);

   // if they changed something on the form...
   if (iForm->hasChanged())
   {
      pSearchPanel->clearMenu(true);

      cxWindow iWindow(nullptr, "Running...");
      iWindow.show();
      pSearchPanel->appendToMenu("Item 1");
      pSearchPanel->appendToMenu("Item 2");
      pSearchPanel->appendToMenu("Item 3");
      pSearchPanel->appendToMenu("Item 4");
      sleep(1);
   }
   iForm->setChanged(false);

   return("");
} // onFocusMenu

string onLeaveMenu(void *theSearchPanel, void *unused, void *unused2,
                   void *unused3)
                   {
   //cxBase::messageBox("onLeaveMenu");
   if (theSearchPanel == nullptr)
   {
      return("");
   }
   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   pSearchPanel->getMenu()->setStatus("");
   return("");
} // onLeaveMenu

string formOnEnter(void *theSearchPanel, void *unused, void *unused2,
                   void *unused3)
                   {
   //cxBase::messageBox("formOnEnter");
   if (theSearchPanel == nullptr)
   {
      return("");
   }
   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   shared_ptr<cxMenu> iMenu = pSearchPanel->getMenu();
   iMenu->append("formOnEnter was fired", 999);

   return("");
} // formOnEnter

string menuOnEnter(void *theSearchPanel, void *unused, void *unused2,
                   void *unused3)
                   {
   if (theSearchPanel == nullptr)
   {
      return("");
   }

   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   shared_ptr<cxMenu> iMenu = pSearchPanel->getMenu();
   cxBase::messageBox("The '" + iMenu->getCurrentItemText() + "' item was selected.", "ha ha");

   return("");
} // menuOnEnter

string searchPanelOnFocus(void *theSearchPanel, void *unused, void *unused2,
                   void *unused3)
                   {
   //cxBase::messageBox("searchPanelOnFocus");
   if (theSearchPanel == nullptr)
   {
      return("");
   }

   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   shared_ptr<cxForm> iForm = pSearchPanel->getForm();
   // Change the data in input 2
   iForm->setValue(2, "abc", false);
   // Set the menu as the current window.  This should simulate running the
   //  "search" and give the menu focus when the search panel is shown.
   pSearchPanel->setCurrentWindow(1);

   return("");
} // searchPanelOnFocus

void cxSearchPanelTest()
{
   cxSearchPanel iSearchPanel(nullptr, 1, 0, 20, 70, "Form", "Menu");
   iSearchPanel.appendToForm(1, 1, 1, 20, "Input 1:", "", "", eINPUT_EDITABLE,
                             "input1");
   iSearchPanel.appendToForm(2, 1, 1, 20, "Input 2:", "", "", eINPUT_EDITABLE,
                             "input2");
   iSearchPanel.appendToForm(1, 22, 1, 20, "Input 3:", "", "", eINPUT_EDITABLE,
                             "input3");
   iSearchPanel.appendToForm(2, 22, 1, 20, "Input 4:", "", "", eINPUT_EDITABLE,
                             "input4");
   iSearchPanel.setFormOnFocusFunction(onFocusForm, &iSearchPanel, nullptr,
                           nullptr, nullptr, false, false);
   iSearchPanel.setFormOnLeaveFunction(onLeaveForm, &iSearchPanel, nullptr,
                           nullptr, nullptr);
   iSearchPanel.setMenuOnFocusFunction(onFocusMenu, &iSearchPanel, nullptr,
                           nullptr, nullptr, false, false);
   iSearchPanel.setMenuOnLeaveFunction(onLeaveMenu, &iSearchPanel, nullptr,
                           nullptr, nullptr);
   iSearchPanel.setFormOnEnterFunction(formOnEnter, &iSearchPanel, nullptr,
                                       nullptr, nullptr);
   // A cxSearchPanel sets an onSelectItem function for its menu, but we can
   //  also set one if we want:
   //iSearchPanel.setMenuOnSelectItemFunction(menuOnEnter, &iSearchPanel, nullptr,
   //                             nullptr, nullptr, true, true);
   iSearchPanel.setOnFocusFunction(searchPanelOnFocus, &iSearchPanel, nullptr,
                                   nullptr, nullptr, false, false);
   iSearchPanel.showModal();
   if (iSearchPanel.itemWasSelected())
   {
      cxBase::messageBox("You chose this item:" + iSearchPanel.getSelectedItem() + ":");
   }
   else
   {
      cxBase::messageBox("An item was not selected.");
   }
} // cxSearchPanelTest

void cxSearchPanelResize()
{
   cxSearchPanel iSearchPanel(nullptr, 1, 0, 10, 70, "Form", "Menu");
   iSearchPanel.appendToForm(1, 1, 1, 20, "Input 1:", "", "", eINPUT_EDITABLE,
                             "input1");
   iSearchPanel.appendToForm(2, 1, 1, 20, "Input 2:", "", "", eINPUT_EDITABLE,
                             "input2");
   iSearchPanel.appendToForm(1, 22, 1, 20, "Input 3:", "", "", eINPUT_EDITABLE,
                             "input3");
   iSearchPanel.appendToForm(2, 22, 1, 20, "Input 4:", "", "", eINPUT_EDITABLE,
                             "input4");
   iSearchPanel.setFormOnFocusFunction(onFocusForm, &iSearchPanel, nullptr,
                           nullptr, nullptr, false, false);
   iSearchPanel.setFormOnLeaveFunction(onLeaveForm, &iSearchPanel, nullptr,
                           nullptr, nullptr);
   iSearchPanel.setMenuOnFocusFunction(onFocusMenu, &iSearchPanel, nullptr,
                           nullptr, nullptr, false, false);
   iSearchPanel.setMenuOnLeaveFunction(onLeaveMenu, &iSearchPanel, nullptr,
                           nullptr, nullptr);
   iSearchPanel.resize(23, 60, false);
   iSearchPanel.showModal();

   cxBase::messageBox("Now testing setFormHeight()");
   shared_ptr<cxForm> iForm = iSearchPanel.getForm(); // To get the form's current height
   iSearchPanel.setFormHeight(iForm->height()-2, false);
   iSearchPanel.showModal();

   cxBase::messageBox("Now testing setMenuHeight()");
   shared_ptr<cxMenu> iMenu = iSearchPanel.getMenu(); // To get the menu's current height
   iSearchPanel.setMenuHeight(iMenu->height()-2, false);
   iSearchPanel.showModal();
} // cxSearchPanelResize

void cxSearchPanelCopyConstructor()
{
   // Create one with 'new' so that it can be destroyed after it is copied.
   //  The copy should be like the original, and there should be no
   //  segfaults, etc.
   shared_ptr<cxSearchPanel> iSearchPanel = make_shared<cxSearchPanel>(nullptr, 1, 0, 20, 70, "Form", "Menu");
   iSearchPanel->appendToForm(1, 1, 1, 20, "Input 1:", "", "", eINPUT_EDITABLE,
                             "input1");
   iSearchPanel->appendToForm(2, 1, 1, 20, "Input 2:", "", "", eINPUT_EDITABLE,
                             "input2");
   iSearchPanel->appendToForm(1, 22, 1, 20, "Input 3:", "", "", eINPUT_EDITABLE,
                             "input3");
   iSearchPanel->appendToForm(2, 22, 1, 20, "Input 4:", "", "", eINPUT_EDITABLE,
                             "input4");
   iSearchPanel->setFormOnFocusFunction(onFocusForm, iSearchPanel.get(), nullptr,
                           nullptr, nullptr, false, false);
   iSearchPanel->setFormOnLeaveFunction(onLeaveForm, iSearchPanel.get(), nullptr,
                           nullptr, nullptr);
   iSearchPanel->setMenuOnFocusFunction(onFocusMenu, iSearchPanel.get(), nullptr,
                           nullptr, nullptr, false, false);
   iSearchPanel->setMenuOnLeaveFunction(onLeaveMenu, iSearchPanel.get(), nullptr,
                           nullptr, nullptr);
   iSearchPanel->setFormOnEnterFunction(formOnEnter, iSearchPanel.get(), nullptr,
                                        nullptr, nullptr);
   iSearchPanel->setOnFocusFunction(searchPanelOnFocus, iSearchPanel.get(), nullptr,
                                   nullptr, nullptr, false, false);
   iSearchPanel->setName("searchPanel");
   cxSearchPanel copyPanel(*iSearchPanel);
   iSearchPanel = nullptr;
   copyPanel.showModal();
} // cxSearchPanelCopyConstructor

void cxMenuResize()
{
   cxMenu iMenu(nullptr, 1, 0, 10, 15, "Test menu");
   for (int i = 1; i <= 15; ++i)
   {
      iMenu.append("Item " + toString(i), i);
   }
   cxBase::messageBox("Showing menu before resize");
   iMenu.showModal();
   iMenu.resize(8, 16);
   cxBase::messageBox("After resize");
   iMenu.showModal();
} // cxMenuResize

void cxWindowAlignTest()
{
   //string text;
   int windowWidth=20;

   cxWindow iWindow(nullptr, 0, 0, 3, windowWidth, "Test", "", "");
   iWindow.setMessage("1 dead center");
   iWindow.center();
   iWindow.show();
   //text=cxBase::getString(cxBase::bottom() / 2, (cxBase::width() / 2) + (windowWidth / 2), 2);
   //cxBase::messageBox(text);
   //assert(text == "1 ");
   int foundRow=-2;
   int foundCol=-2;
   cxBase::messageBox("foundRow:" + toString(foundRow) + " foundCol:" + toString(foundCol));
   iWindow.info();
   iWindow.show();
   sleep(1);

   iWindow.setMessage("2 Top Left");
   iWindow.alignHoriz(eHP_LEFT);
   iWindow.alignVert(eVP_TOP);
   iWindow.show();
   sleep(1);

   iWindow.setMessage("3 Top Middle");
   iWindow.alignHoriz(eHP_CENTER);
   iWindow.alignVert(eVP_TOP);
   iWindow.show();
   sleep(1);

   iWindow.setMessage("4 Top Right");
   iWindow.alignHoriz(eHP_RIGHT);
   iWindow.alignVert(eVP_TOP);
   iWindow.show();
   sleep(1);

   iWindow.setMessage("5 Center Left");
   iWindow.alignHoriz(eHP_LEFT);
   iWindow.alignVert(eVP_CENTER);
   iWindow.show();
   sleep(1);

   iWindow.setMessage("6 Center Middle");
   iWindow.alignHoriz(eHP_CENTER);
   iWindow.alignVert(eVP_CENTER);
   iWindow.show();
   sleep(1);

   iWindow.setMessage("7 Center Right");
   iWindow.alignHoriz(eHP_RIGHT);
   iWindow.alignVert(eVP_CENTER);
   iWindow.show();
   sleep(1);

   iWindow.setMessage("8 Bottom Left");
   iWindow.alignHoriz(eHP_LEFT);
   iWindow.alignVert(eVP_BOTTOM);
   iWindow.show();
   sleep(1);

   iWindow.setMessage("9 Bottom Middle");
   iWindow.alignHoriz(eHP_CENTER);
   iWindow.alignVert(eVP_BOTTOM);
   iWindow.show();
   sleep(1);

   iWindow.setMessage("10 Bottom Right");
   iWindow.alignHoriz(eHP_RIGHT);
   iWindow.alignVert(eVP_BOTTOM);
   iWindow.show();
   sleep(1);

   iWindow.setMessage("11 dead center");
   iWindow.center();
   iWindow.show();
   sleep(1);

} // cxWindowAlignTest

void cxNotebookTest1()
{
   // This tests various cxNotebook stuff: Adding panels of stuff, setting the
   //  label text, etc.
   cxNotebook iNotebook(nullptr, 2, 3, 15, 70, true, true, true, 0);
   // Append a panel containing a cxMenu and a cxForm
   shared_ptr<cxPanel> iPanel1 = iNotebook.append("Win1");
   shared_ptr<cxMenu> iMenu = make_shared<cxMenu>(nullptr, 0, 0, 5, 10, "Menu");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu->append("Item " + toString(i), i);
   }
   iPanel1->append(iMenu, 1, 1, false);
   shared_ptr<cxForm> iForm = make_shared<cxForm>(nullptr, 0, 0, 5, 31, "Form");
   iForm->append(1, 1, 1, 14, "Input 1:");
   iForm->append(2, 1, 1, 14, "Input 2:");
   iForm->append(3, 1, 1, 14, "Input 3:");
   iForm->append(1, 16, 1, 14, "Input 4:");
   iForm->append(2, 16, 1, 14, "Input 5:");
   iForm->append(3, 16, 1, 14, "Input 6:");
   iPanel1->append(iForm, 1, 12, false);
   // Append another panel containing some more stuff
   shared_ptr<cxPanel> iPanel2 = iNotebook.append("Win2");
   iMenu = make_shared<cxMenu>(nullptr, 0, 0, 6, iNotebook.width(), "Item       Qty       Price");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu->append("Item " + toString(i) + "       3      $12.00", i);
   }
   iPanel2->append(iMenu, 1, 0);
   iForm = make_shared<cxForm>(nullptr, 0, 0, 5, iNotebook.width(), "Item form");
   iForm->append(1, 1, 1, 14, "Desc:");
   iForm->append(2, 1, 1, 14, "Qty:");
   iForm->append(3, 1, 1, 14, "Price:");
   iPanel2->append(iForm, 7, 0, false);
   // Add a 3rd panel
   shared_ptr<cxPanel> iPanel3 = iNotebook.append("Win3");
   shared_ptr<cxFileViewer> iFileViewer = make_shared<cxFileViewer>(nullptr, "cxMenu.h", 0, 0, 7, iNotebook.width());
   iPanel3->append(iFileViewer, 1, 0);
   shared_ptr<cxWindow> iWindow = make_shared<cxWindow>(nullptr, 0, 0, 4, iNotebook.width(), "cxWindow", "This is a cxWindow");
   iPanel3->append(iWindow, 8, 0);
   // Add a 4th panel with a cxMultiForm on it
   shared_ptr<cxPanel> iPanel4 = iNotebook.append("Win4");
   shared_ptr<cxMultiForm> iMultiForm = make_shared<cxMultiForm>(nullptr, 0, 0, iPanel4->height()-2,
                                                                 iPanel4->width(), "MultiForm", eBS_SINGLE_LINE);
   iPanel4->append(iMultiForm, 1, 0, false);
   iForm = iMultiForm->appendForm(1, 1, 4, iMultiForm->width()-2, "Form 1",
                                  eBS_NOBORDER);
   iForm->append(0, 0, 1, 30, "Input 1:");
   iForm->append(1, 0, 1, 30, "Input 2:");
   iForm->append(2, 0, 1, 30, "Input 3:");
   iForm->append(3, 0, 1, 30, "Input 4:");
   iForm->append(0, 32, 1, 30, "Input 5:");
   iForm->append(1, 32, 1, 30, "Input 6:");
   iForm->append(2, 32, 1, 30, "Input 7:");
   iForm->append(3, 32, 1, 30, "Input 8:");
   iForm = iMultiForm->appendForm(6, 1, 4, iMultiForm->width()-2, "Form 2",
                                  eBS_NOBORDER);
   iForm->append(0, 0, 1, 30, "Input 1:");
   iForm->append(1, 0, 1, 30, "Input 2:");
   iForm->append(2, 0, 1, 30, "Input 3:");
   iForm->append(3, 0, 1, 30, "Input 4:");
   iForm->append(0, 32, 1, 30, "Input 5:");
   iForm->append(1, 32, 1, 30, "Input 6:");
   iForm->append(2, 32, 1, 30, "Input 7:");
   iForm->append(3, 32, 1, 30, "Input 8:");

   // Test setting some label text to make sure it works
   assert(iNotebook.setLabel(0, "Window 1", false));
   // Try to set one of the label texts too long - it shouldn't change
   assert(!iNotebook.setLabel(1, "This is a really long label text string.  The notebook should not update the label windows because this is too long.", false));

   iNotebook.showModal();
} // cxNotebookTest1

void cxNotebookTest2()
{
   // This tests various cxNotebook stuff: Adding panels of stuff, setting the
   //  label text, etc.

   bool labelsOnTop=true;
   // TODO: Why does the whole message not show?
   // TODO: Why is yes/no not centered in the window?
   //cxMessageDialog labelPos(nullptr, 0, 0, 3, 60, "Question", "Do you want the labels on the top?", cxYES | cxNO);

   cxMessageDialog labelPos(nullptr, 0, 0, 8, 60, "Question", "Do you want the labels on the top?", cxYES | cxNO);
   labelPos.center(false);
   if (labelPos.showModal() == cxID_OK)
   {
      labelsOnTop=true;
   }
   else
   {
      labelsOnTop=false;
   }

   cxNotebook iNotebook(nullptr, 3, 3, 15, 70, true, true, true, 2);
   if (labelsOnTop)
   {
      iNotebook.setStatus("Status"); // TODO: does not work
   }
   else
   {
      iNotebook.setTitle("Title"); // TODO: does not work
   }
   iNotebook.setLabelsOnTop(labelsOnTop);
   // Append a panel containing a cxMenu and a cxForm
   shared_ptr<cxPanel> iPanel1 = iNotebook.append("Win1");
   shared_ptr<cxForm> iForm = make_shared<cxForm>(nullptr, 0, 0, 5, 31, "Form");
   iForm->append(1, 1, 1, 14, "Input 1:");
   iForm->append(2, 1, 1, 14, "Input 2:");
   iForm->append(3, 1, 1, 14, "Input 3:");
   iForm->append(1, 16, 1, 14, "Input 4:");
   iForm->append(2, 16, 1, 14, "Input 5:");
   iForm->append(3, 16, 1, 14, "Input 6:");
   iPanel1->append(iForm, 1, 12, false);

   // Append another panel containing some more stuff
   shared_ptr<cxPanel> iPanel2 = iNotebook.append("Win2");
   iForm = make_shared<cxForm>(nullptr, 0, 0, 5, iNotebook.width(), "Item form");
   iForm->append(1, 1, 1, 14, "Desc:");
   iForm->append(2, 1, 1, 14, "Qty:");
   iForm->append(3, 1, 1, 14, "Price:");
   iPanel2->append(iForm, 7, 0, false);

   // Add a 3rd panel
   shared_ptr<cxPanel> iPanel3 = iNotebook.append("Win3");
   shared_ptr<cxWindow> iWindow = make_shared<cxWindow>(nullptr, 0, 0, 5, iNotebook.width(),
                                    "cxWindow", "This is a cxWindow");
   iPanel3->append(iWindow, 7, 0);

   // Test setting some label text to make sure it works
   assert(iNotebook.setLabel(0, "Window 1", false));

   iNotebook.showModal();
} // cxNotebookTest2

void cxNotebookRemoveWindowTest()
{
   // This test creates a cxNotebook with a few pages, and then removes one.
   //  The application shouldn't crash/segfault and shouldn't leak memory, and
   //  the cxNotebook should still look okay.
   cxNotebook iNotebook(nullptr, 1, 3, 15, 70);
   // Append a panel containing a cxMenu and a cxForm
   shared_ptr<cxPanel> iPanel1 = iNotebook.append("Win1");
   shared_ptr<cxMenu> iMenu = make_shared<cxMenu>(nullptr, 0, 0, 5, 10, "Menu");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu->append("Item " + toString(i), i);
   }
   iPanel1->append(iMenu, 1, 1, false);
   shared_ptr<cxForm> iForm = make_shared<cxForm>(nullptr, 0, 0, 5, 31, "Form");
   iForm->append(1, 1, 1, 14, "Input 1:");
   iForm->append(2, 1, 1, 14, "Input 2:");
   iForm->append(3, 1, 1, 14, "Input 3:");
   iForm->append(1, 16, 1, 14, "Input 4:");
   iForm->append(2, 16, 1, 14, "Input 5:");
   iForm->append(3, 16, 1, 14, "Input 6:");
   iPanel1->append(iForm, 1, 12, false);
   // Append another panel containing some more stuff
   shared_ptr<cxPanel> iPanel2 = iNotebook.append("Win2");
   iMenu = make_shared<cxMenu>(nullptr, 0, 0, 6, iNotebook.width(), "Item       Qty       Price");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu->append("Item " + toString(i) + "       3      $12.00", i);
   }
   iPanel2->append(iMenu, 1, 0);
   iForm = make_shared<cxForm>(nullptr, 0, 0, 5, iNotebook.width(), "Item form");
   iForm->append(1, 1, 1, 14, "Desc:");
   iForm->append(2, 1, 1, 14, "Qty:");
   iForm->append(3, 1, 1, 14, "Price:");
   iPanel2->append(iForm, 7, 0, false);
   // Add a 3rd panel
   shared_ptr<cxPanel> iPanel3 = iNotebook.append("Win3");
   shared_ptr<cxFileViewer> iFileViewer = make_shared<cxFileViewer>(nullptr, "cxMenu.h", 0, 0, 7, iNotebook.width());
   iPanel3->append(iFileViewer, 1, 0);
   shared_ptr<cxWindow> iWindow = make_shared<cxWindow>(nullptr, 0, 0, 4, iNotebook.width(),
                                                        "cxWindow", "This is a cxWindow");
   iPanel3->append(iWindow, 8, 0);
   // Remove the 2nd panel
   iNotebook.delWindow(1);

   iNotebook.showModal();
} // cxNotebookRemoveWindowTest

void cxNotebookWithEmptyPanelTest()
{
   // This tests a cxNotebook that has a panel with nothing in it.
   // The cxNotebook will have 3 panels.  The middle panel will be the one that
   //  has nothing in it.
   cxNotebook iNotebook(nullptr, 1, 3, 15, 70);
   // Append a panel containing a cxMenu and a cxForm
   shared_ptr<cxPanel> iPanel1 = iNotebook.append("Win1");
   shared_ptr<cxMenu> iMenu = make_shared<cxMenu>(nullptr, 0, 0, 5, 10, "Menu");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu->append("Item " + toString(i), i);
   }
   iPanel1->append(iMenu, 1, 1, false);
   shared_ptr<cxForm> iForm = make_shared<cxForm>(nullptr, 0, 0, 5, 31, "Form");
   iForm->append(1, 1, 1, 14, "Input 1:");
   iForm->append(2, 1, 1, 14, "Input 2:");
   iForm->append(3, 1, 1, 14, "Input 3:");
   iForm->append(1, 16, 1, 14, "Input 4:");
   iForm->append(2, 16, 1, 14, "Input 5:");
   iForm->append(3, 16, 1, 14, "Input 6:");
   iPanel1->append(iForm, 1, 12, false);
   // Append another panel containing some more stuff
   iNotebook.append("Win2");
   // Add a 3rd panel
   shared_ptr<cxPanel> iPanel3 = iNotebook.append("Win3");
   shared_ptr<cxFileViewer> iFileViewer = make_shared<cxFileViewer>(nullptr, "cxMenu.h", 0, 0, 7, iNotebook.width());
   iPanel3->append(iFileViewer, 1, 0);
   shared_ptr<cxWindow> iWindow = make_shared<cxWindow>(nullptr, 0, 0, 4, iNotebook.width(),
                                                        "cxWindow", "This is a cxWindow");
   iPanel3->append(iWindow, 8, 0);

   iNotebook.showModal();
} // cxNotebookWithEmptyPanelTest

void cxNotebookSetTabSpacing()
{
   // This tests the setTabSpacing() method of a cxNotebook.
   {
      // Labels on top
      cxNotebook iNotebook(nullptr, 2, 3, 15, 70, true, true, true, 0);
      // Append 3 panels
      iNotebook.append("Win1");
      iNotebook.append("Win2");
      iNotebook.append("Win3");
      iNotebook.show();
      sleep(1);
      // Set the tab spacing
      // These tab spacings should work
      assert(iNotebook.setTabSpacing(2, true));
      sleep(1);
      assert(iNotebook.setTabSpacing(5, true));
      sleep(1);
      // 24 is the biggest tab spacing we can use for this notebook.  A spacing of
      //  24 means there will be 22 spaces between each tab.  Each tab is 8
      //  characters wide.  So, the 3 tabs and all the spaces total to 70
      //  characters, which is the width of the notebook.
      assert(iNotebook.setTabSpacing(24, true));
      sleep(1);
      // This tab spacing shouldn't work because it's too much.
      assert(!iNotebook.setTabSpacing(25, true));
      sleep(1);
   }

   {
      // Labels on the bottom
      cxNotebook iNotebook(nullptr, 2, 3, 15, 70, false, true, true, 0);
      // Append 3 panels
      iNotebook.append("Win1");
      iNotebook.append("Win2");
      iNotebook.append("Win3");
      iNotebook.show();
      sleep(1);
      // Set the tab spacing
      // These tab spacings should work
      assert(iNotebook.setTabSpacing(2, true));
      sleep(1);
      assert(iNotebook.setTabSpacing(5, true));
      sleep(1);
      // 24 is the biggest tab spacing we can use for this notebook.  A spacing of
      //  24 means there will be 22 spaces between each tab.  Each tab is 8
      //  characters wide.  So, the 3 tabs and all the spaces total to 70
      //  characters, which is the width of the notebook.
      assert(iNotebook.setTabSpacing(24, true));
      sleep(1);
      // This tab spacing shouldn't work because it's too much.
      assert(!iNotebook.setTabSpacing(25, true));
      sleep(1);

      iNotebook.setLabelsOnTop(true);
      // Set the tab spacing
      // These tab spacings should work
      assert(iNotebook.setTabSpacing(0, true));
      sleep(1);
      assert(iNotebook.setTabSpacing(2, true));
      sleep(1);
      assert(iNotebook.setTabSpacing(5, true));
      sleep(1);
      // 24 is the biggest tab spacing we can use for this notebook.  A spacing of
      //  24 means there will be 22 spaces between each tab.  Each tab is 8
      //  characters wide.  So, the 3 tabs and all the spaces total to 70
      //  characters, which is the width of the notebook.
      assert(iNotebook.setTabSpacing(24, true));
      sleep(1);
      // This tab spacing shouldn't work because it's too much.
      assert(!iNotebook.setTabSpacing(25, true));
      sleep(1);
   }

   // Note: make two passes thru this code (that's why there is a for loop)
   // so we can see the labels on top, then re-run the code again with labels
   // on the bottom
   for (int i=0; i < 2; ++i)
   {
      // This tests the setTabSpacing() method of a cxNotebook.
      cxNotebook iNotebook(nullptr, 2, 3, 15, 70, i == 0 ? true : false, true, true, 0);
      // Append 3 panels
      iNotebook.append("Win1");
      iNotebook.append("Win2");
      iNotebook.append("Win3");
      iNotebook.show();
      sleep(1);
      // Set the tab spacing
      // These tab spacings should work
      assert(iNotebook.setTabSpacing(2, true));
      sleep(1);
      assert(iNotebook.setTabSpacing(5, true));
      sleep(1);
      // 24 is the biggest tab spacing we can use for this notebook.  A spacing of
      //  24 means there will be 22 spaces between each tab.  Each tab is 8
      //  characters wide.  So, the 3 tabs and all the spaces total to 70
      //  characters, which is the width of the notebook.
      assert(iNotebook.setTabSpacing(24, true));
      sleep(1);
      // This tab spacing shouldn't work because it's too much.
      assert(!iNotebook.setTabSpacing(25, true));
      sleep(1);
   }
} // cxNotebookSetTabSpacing

void cxNotebookSwapTest()
{
   // This tests cxNotebook's swap() method.
   cxNotebook iNotebook(nullptr, 2, 3, 15, 70, true, true, true, 0);
   // Append a panel containing a cxMenu and a cxForm
   shared_ptr<cxPanel> iPanel1 = iNotebook.append("Win1");
   shared_ptr<cxMenu> iMenu = make_shared<cxMenu>(nullptr, 0, 0, 5, 10, "Menu");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu->append("Item " + toString(i), i);
   }
   iPanel1->append(iMenu, 1, 1, false);
   shared_ptr<cxForm> iForm = make_shared<cxForm>(nullptr, 0, 0, 5, 31, "Form");
   iForm->append(1, 1, 1, 14, "Input 1:");
   iForm->append(2, 1, 1, 14, "Input 2:");
   iForm->append(3, 1, 1, 14, "Input 3:");
   iForm->append(1, 16, 1, 14, "Input 4:");
   iForm->append(2, 16, 1, 14, "Input 5:");
   iForm->append(3, 16, 1, 14, "Input 6:");
   iPanel1->append(iForm, 1, 12, false);
   // Append another panel containing some more stuff
   shared_ptr<cxPanel> iPanel2 = iNotebook.append("Win2");
   iMenu = make_shared<cxMenu>(nullptr, 0, 0, 6, iNotebook.width(), "Item       Qty       Price");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu->append("Item " + toString(i) + "       3      $12.00", i);
   }
   iPanel2->append(iMenu, 1, 0);
   iForm = make_shared<cxForm>(nullptr, 0, 0, 5, iNotebook.width(), "Item form");
   iForm->append(1, 1, 1, 14, "Desc:");
   iForm->append(2, 1, 1, 14, "Qty:");
   iForm->append(3, 1, 1, 14, "Price:");
   iPanel2->append(iForm, 7, 0, false);
   // Add a 3rd panel
   shared_ptr<cxPanel> iPanel3 = iNotebook.append("Win3");
   shared_ptr<cxFileViewer> iFileViewer = make_shared<cxFileViewer>(nullptr, "cxMenu.h", 0, 0, 7, iNotebook.width());
   iPanel3->append(iFileViewer, 1, 0);
   shared_ptr<cxWindow> iWindow = make_shared<cxWindow>(nullptr, 0, 0, 4, iNotebook.width(),
                                                        "cxWindow", "This is a cxWindow");
   iPanel3->append(iWindow, 8, 0);

   // Swap the first & second panels
   assert(iNotebook.swap(0, 1));
   // Also test the other swap() method that takes pointers: Get the current
   // 2nd & 3rd windows and swap them.  The order should then be Win2, Win3,
   // Win1.
   std::shared_ptr<cxWindow> win1 = iNotebook.getWindow(1);
   std::shared_ptr<cxWindow> win2 = iNotebook.getWindow(2);
   assert(iNotebook.swap(win1, win2));

   iNotebook.showModal();
} // cxNotebookSwapTest

void cxWindowBorderTest()
{
   {
      int height=20;
      cxWindow iWindow(nullptr, 0, 0, height, 20, "Title", "Message", "Status", eBS_SINGLE_LINE);
      iWindow.center();
      iWindow.show();
      sleep(1);

      iWindow.setMessage("topless!");
      iWindow.toggleBorderTop(false);
      iWindow.show();
      sleep(1);

      iWindow.setMessage("bottomless!");
      iWindow.toggleBorderBottom(false);
      iWindow.show();
      sleep(1);

      iWindow.setMessage("no left");
      iWindow.toggleBorderLeft(false);
      iWindow.show();
      sleep(1);

      iWindow.setMessage("no right");
      iWindow.toggleBorderRight(false);
      iWindow.show();
      sleep(1);
   }

   {
      cxWindow iWindow(nullptr, 4, 4, 3, 20, "", "Sample Tab", "", eBS_SINGLE_LINE);
      iWindow.toggleBorderBottom(false);
      iWindow.show();
      sleep(1);
   }

   {
      cxWindow iWindow(nullptr, 4, 4, 3, 20, "", "No border", "", eBS_NOBORDER);
      iWindow.show();
      sleep(1);
   }

   {
      cxWindow iWindow(nullptr, 4, 4, 3, 20, "", "Space Border", "", eBS_SPACE);
      iWindow.show();
      sleep(1);
   }
} // cxWindowBorderTest

void cxWindowSizeTest()
{
   try
   {
      cxBase::messageBox("about to clean up");
      cxBase::cleanup();
      cxBase::init();
      cxBase::messageBox("cleaned up and re-inited");
      cxBase::cleanup();
      cxBase::init();
      cxBase::messageBox("cleaned up and re-inited again");
      cxBase::cleanup();
   }
   catch(const cxWidgetsException &e)
   {
      cerr << "prob:" << e.getErrorMsg() << endl;
   }

   {
      if (cxBase::cxInitialized())
      {
         cxWindow iWindow(nullptr, "1", "1", "1");
         iWindow.show();
         sleep(1);
         iWindow.hide();
      }
   }

   {
      if (cxBase::cxInitialized())
      {
         cxWindow iWindow(nullptr, "12345", "xxxxxxxxxxxxxxxxxx", "");
         iWindow.show();
         sleep(1);
         iWindow.hide();
      }
   }
} // cxWindowSizeTest

void cxWindowAttributesTest()
{
   cxWindow iWindow(nullptr, "Title XXXXXXXXXX", "Message XXXXXXXXXXXXXXXXXXX", "Status");
   iWindow.show();
   sleep(1);

   iWindow.setMessage("Message BOLD");
   iWindow.setAttr(eMESSAGE, A_BOLD);
   iWindow.show();
   sleep(1);

   iWindow.setTitle("Title BOLD");
   iWindow.setAttr(eTITLE, A_BOLD);
   iWindow.setMessage("Message BOLD");
   iWindow.setAttr(eMESSAGE, A_BOLD);
   iWindow.setStatus("Status BOLD");
   iWindow.setAttr(eSTATUS, A_BOLD);
   iWindow.show();
   sleep(1);

   iWindow.setTitle("Title NORMAL");
   iWindow.setAttr(eTITLE, A_NORMAL);
   iWindow.setMessage("Message NORMAL");
   iWindow.setAttr(eMESSAGE, A_NORMAL);
   iWindow.setStatus("Status NORMAL");
   iWindow.setAttr(eSTATUS, A_NORMAL);
   iWindow.show();
   sleep(1);

   iWindow.setTitle("Title UNDERLINE");
   iWindow.setAttr(eTITLE, A_UNDERLINE);
   iWindow.setMessage("Message UNDERLINE");
   iWindow.setAttr(eMESSAGE, A_UNDERLINE);
   iWindow.setStatus("Status UNDERLINE");
   iWindow.setAttr(eSTATUS, A_UNDERLINE);
   iWindow.show();
   sleep(1);

   iWindow.setTitle("Title STANDOUT");
   iWindow.setAttr(eTITLE, A_STANDOUT);
   iWindow.setMessage("Message STANDOUT");
   iWindow.setAttr(eMESSAGE, A_STANDOUT);
   iWindow.setStatus("Status STANDOUT");
   iWindow.setAttr(eSTATUS, A_STANDOUT);
   iWindow.show();
   sleep(1);

   iWindow.setTitle("Title DIM");
   iWindow.setAttr(eTITLE, A_DIM);
   iWindow.setMessage("Message DIM");
   iWindow.setAttr(eMESSAGE, A_DIM);
   iWindow.setStatus("Status DIM");
   iWindow.setAttr(eSTATUS, A_DIM);
   iWindow.show();
   sleep(1);
} // cxWindowAttributesTest

string mouseFunction(void *theWindow, void *unused)
{
   messageBox("mouseFunction() fired");

   if (theWindow != nullptr)
   {
      cxWindow *pWindow = static_cast<cxWindow*>(theWindow);
      messageBox("Window title: " + pWindow->getTitle() + ", mouse state: " +
                 pWindow->getMouseStateStr());
   }

   return("");
} // mouseFunction

void cxWindowMouseFunctionTest()
{
   cxWindow iWindow(nullptr, 1, 0, 10, 30, "cxWindow", "Mouse function test window");
   iWindow.setMouseFunction(BUTTON1_CLICKED, mouseFunction, &iWindow, nullptr,
                            false, false, false);
   iWindow.setMouseFunction(BUTTON2_CLICKED, mouseFunction, &iWindow, nullptr,
                            false, false, false);
   iWindow.setMouseFunction(BUTTON3_CLICKED, mouseFunction, &iWindow, nullptr,
                            false, false, false);
   iWindow.showModal();
} // cxWindowMouseFunctionTest

void cxMenuMouseFunctionTest()
{
   cxMenu iMenu(nullptr, 1, 0, 10, 10, "cxMenu");
   for (int i = 1; i <= 30; ++i)
   {
      iMenu.append("Item " + toString(i), i);
   }
   iMenu.setMouseFunction(BUTTON1_CLICKED, mouseFunction, &iMenu, nullptr,
                          false, false, false);
   iMenu.setMouseFunction(BUTTON2_CLICKED, mouseFunction, &iMenu, nullptr,
                          false, false, false);
   iMenu.setMouseFunction(BUTTON3_CLICKED, mouseFunction, &iMenu, nullptr,
                          false, false, false);
   iMenu.showModal();
} // cxMenuMouseFunctionTest

void cxFormMouseFunctionTest()
{
   cxForm iForm(nullptr, 1, 0, 5, 70, "cxForm");
   iForm.append(1, 1, 1, 33, "Input 1:");
   iForm.append(2, 1, 1, 33, "Input 2:");
   iForm.append(3, 1, 1, 33, "Input 3:");
   iForm.append(1, 35, 1, 33, "Input 4:");
   iForm.append(2, 35, 1, 33, "Input 5:");
   iForm.append(3, 35, 1, 33, "Input 6:");
   iForm.setMouseFunction(BUTTON1_CLICKED, mouseFunction, &iForm, nullptr,
                          false, false, false);
   iForm.setMouseFunction(BUTTON2_CLICKED, mouseFunction, &iForm, nullptr,
                          false, false, false);
   iForm.setMouseFunction(BUTTON3_CLICKED, mouseFunction, &iForm, nullptr,
                          false, false, false);
   iForm.showModal();
} // cxFormMouseFunctionTest

void cxPanelWithButton()
{
   // This tests a cxPanel with a cxButton and something other than a button.
   //  The other window will be appended first.  When the user clicks on the
   //  button, its onClick function should fire.

   cxPanel iPanel(nullptr, 1, 0, 15, 70, "Panel", "", "", eBS_SINGLE_LINE);
   // Append a form to the panel, then append a button.
   shared_ptr<cxForm> iForm = make_shared<cxForm>(nullptr, 0, 0, 2, 68, "", eBS_NOBORDER);
   iForm->append(0, 0, 1, 32, "Input 1:");
   iForm->append(1, 0, 1, 32, "Input 2:");
   iForm->append(0, 34, 1, 32, "Input 3:");
   iForm->append(1, 34, 1, 32, "Input 4:");
   iPanel.append(iForm, 1, 1, false);

   shared_ptr<cxButton> iButton = make_shared<cxButton>(nullptr, 1, 0, 3, 10, "Button");
   iButton->setOnClickFunction(buttonClickFunction, &iButton, nullptr, nullptr, nullptr);
   iPanel.append(iButton, 4, 1, false);
   iButton->center(false); // Center the button horizontally in the panel

   iPanel.showModal();
} // cxPanelWithButton

void cxMultiLineInputNumericFloatingPt()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 30, "Floating-point:");
   iInput.setInputType(eINPUT_TYPE_NUMERIC_FLOATING_PT);
   iInput.showModal();
   cxBase::messageBox("Value:" + iInput.getValue() + ":");
} // cxMultiLineInputNumericFloatingPt

void cxMultiLineInputNumericFloatingPtWithRange()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 30, "Floating-point:");
   iInput.setInputType(eINPUT_TYPE_NUMERIC_FLOATING_PT);
   iInput.setRangeDouble(2.0, 5.0);
   iInput.validatorFuncMessageBox(true);
   iInput.showModal();
   cxBase::messageBox("Value:" + iInput.getValue() + ":");
} // cxMultiLineInputNumericFloatingPtWithRange

void cxMultiLineInputNumericWhole()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 30, "Whole #:");
   iInput.setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
   auto func = make_shared<cxFunction0>(hello,  false, false, true);
   iInput.setKeyFunction(KEY_F(8), func);
   iInput.showModal();
   cxBase::messageBox("Value:" + iInput.getValue() + ":");
} // cxMultiLineInputNumericWhole

void cxMultiLineInputNumericWholeWithRange()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 30, "Whole #:");
   iInput.setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
   iInput.setRangeInt(2, 5);
   iInput.validatorFuncMessageBox(true);
   iInput.showModal();
   cxBase::messageBox("Value:" + iInput.getValue() + ":");
} // cxMultiLineInputNumericWholeWithRange

void cxMultiLineInputTextValidation()
{
   {
      // Test a text input with valid strings using setValidOptions()
      cxMultiLineInput iInput(nullptr, 1, 0, 1, 18, "Enter a, b, or c:");
      // eINPUT_TYPE_TEXT is the default, but set it anyway:
      iInput.setInputType(eINPUT_TYPE_TEXT);
      iInput.setValidOptions("abc");
      iInput.validatorFuncMessageBox(true);
      iInput.showModal();
      cxBase::messageBox("Value:" + iInput.getValue() + ":");
   }

   {
      // Test a text input with valid strings using addValidOption()
      cxMultiLineInput iInput(nullptr, 1, 0, 1, 18, "Enter d, e, or f:");
      // eINPUT_TYPE_TEXT is the default, but set it anyway:
      iInput.setInputType(eINPUT_TYPE_TEXT);
      iInput.addValidOption("d");
      iInput.addValidOption("e");
      iInput.addValidOption("f");
      iInput.validatorFuncMessageBox(true);
      iInput.showModal();
      cxBase::messageBox("Value:" + iInput.getValue() + ":");
   }
} // cxMultiLineInputTextValidation

void cxMultiLineInputForceUpper()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 18, "Input:");
   iInput.setForceUpper(true);
   iInput.validatorFuncMessageBox(true);
   iInput.showModal();
   cxBase::messageBox("Value:" + iInput.getValue() + ":");
} // cxMultiLineInputForceUpper

void cxMultiLineInputNonBlank()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 18, "Input:");
   iInput.setAllowBlank(false);
   iInput.validatorFuncMessageBox(true);
   iInput.showModal();
   cxBase::messageBox("Value:" + iInput.getValue() + ":");
} // cxMultiLineInputNonBlank

void cxMultiLineInputTextValidationNonBlankForceUpper()
{
   {
      // Test a text input with valid strings using setValidOptions()
      cxMultiLineInput iInput(nullptr, 1, 0, 1, 18, "Enter A, B, or C:");
      // eINPUT_TYPE_TEXT is the default, but set it anyway:
      iInput.setInputType(eINPUT_TYPE_TEXT);
      iInput.setValidOptions("ABC");
      iInput.validatorFuncMessageBox(true);
      iInput.setAllowBlank(false);
      iInput.setForceUpper(true);
      iInput.showModal();
      cxBase::messageBox("Value:" + iInput.getValue() + ":");
   }

   {
      // Test a text input with valid strings using addValidOption()
      cxMultiLineInput iInput(nullptr, 2, 0, 1, 18, "Enter D, E, or F:");
      // eINPUT_TYPE_TEXT is the default, but set it anyway:
      iInput.setInputType(eINPUT_TYPE_TEXT);
      iInput.addValidOption("D");
      iInput.addValidOption("E");
      iInput.addValidOption("F");
      iInput.validatorFuncMessageBox(true);
      iInput.setAllowBlank(false);
      iInput.setForceUpper(true);
      iInput.showModal();
      cxBase::messageBox("Value:" + iInput.getValue() + ":");
   }
} // cxMultiLineInputTextValidationNonBlankForceUpper

void cxMultiLineInputTextValidationAutoCompletion()
{
   cxBase::messageBox("For this input, \"EMAIL\" and \"EMULE\" are valid.");

   cxMultiLineInput iInput(nullptr, 2, 0, 1, 20, "Input:", eBS_NOBORDER,
                           eINPUT_EDITABLE, eINPUT_TYPE_TEXT);
   iInput.setForceUpper(true);
   iInput.setShowRightLabel(true);
   iInput.setRightLabelWidth(10, false);
   iInput.setRightLabelOffset(1, false);
   iInput.addValidOption("EMAIL", "Email");
   iInput.addValidOption("EMULE", "eMule");
   iInput.validatorFuncMessageBox(true);
   iInput.showModal();
   cxBase::messageBox("Value:" + iInput.getValue() + ":");
} // cxMultiLineInputTextValidationAutoCompletion

void cxMultiLineInputExtendedHelpKey()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 18, "Input:");
   iInput.setExtendedHelp("This is the extended help.");
   iInput.setExtendedHelpKey(KEY_F(1));
   iInput.setUseExtendedHelpKeys(true);
   cxBase::messageBox("The extended help keys are " +
                      iInput.getExtendedHelpKeyStrings());
   iInput.showModal();

   cxBase::messageBox("The extended help key will now be changed to F2.");
   iInput.setExtendedHelpKey(KEY_F(2));
   iInput.showModal();

   cxBase::messageBox("The extended help key will now be diabled.");
   iInput.setUseExtendedHelpKeys(false);
   iInput.showModal();
} // cxMultiLineInputExtendedHelpKey

void cxFormExtendedHelpKey()
{
   cxForm iForm(nullptr, 1, 0, 4, 70, "Form");
   iForm.append(1, 1, 1, 25, "Input 1:");
   iForm.append(2, 1, 1, 25, "Input 2:");
   iForm.append(1, 27, 1, 25, "Input 3:");
   iForm.append(2, 27, 1, 25, "Input 4:");
   iForm.setExtendedHelpKey(KEY_F(1));
   iForm.setUseExtendedHelpKeys(true);
   cxBase::messageBox("The extended help keys are " +
                      iForm.getExtendedHelpKeyStrings());
   iForm.showModal();

   cxBase::messageBox("The extended help key will now be changed to F2.");
   iForm.setExtendedHelpKey(KEY_F(2));
   iForm.showModal();

   cxBase::messageBox("The extended help key will now be diabled.");
   iForm.setUseExtendedHelpKeys(false);
   iForm.showModal();
} // cxFormExtendedHelpKey

void cxMultiLineInputWithRightLabel()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 18, "Input:");
   iInput.setShowRightLabel(true);
   iInput.setRightLabel("Text", false);
   iInput.setRightLabelOffset(1, false);
   //iInput.showModal();
   iInput.show();
   sleep(1);
   iInput.setRightLabel(3, "Text2", false);
   iInput.show();
   sleep(1);
   iInput.resizeRightLabel(1, 10, false);
   iInput.setRightLabel("Help text", false);
   iInput.show();
   sleep(1);
   iInput.resizeRightLabel(3, 10, false);
   iInput.setRightLabel(1, "This is some more help text.");
   iInput.show();
   sleep(1);
   iInput.setRightLabel("More help text", false);
   iInput.resizeRightLabel(1, 10, false);
   iInput.show();
   sleep(1);
} // cxMultiLineInputWithRightLabel

void cxMultiLineInputWithRightLabelOnForm()
{
   // This sets up a form with some inputs with right labels.  This also
   //  sets up valid text for the inputs with help strings that should appear
   //  in the right labels when the text is validated (when the user leaves
   //  each input).
   cxForm iForm(nullptr, 1, 0, 5, 79, "Form");

   shared_ptr<cxMultiLineInput> iInput = iForm.append(1, 1, 1, 25, "Input:", "", "Original status field",
                                                      eINPUT_EDITABLE, "", nullptr, 1, 1, 9, true);
   iInput->setForceUpper(true);
   iInput->setValidateOnReverse(false);
   iInput->addValidOption("SCREEN", "Screen");
   iInput->addValidOption("FAX", "Fax");
   iInput->addValidOption("EMAIL", "Email");
   iInput->setExtendedHelp("This is my extended help message. (" + iInput->getValidOptionStrings(eMLIF_COMMA_SEP_WITH_DESC) + ")");
   iInput->setStatus("New status field (" + iInput->getValidOptionStrings() + ")");

   iInput = iForm.append(2, 1, 1, 25, "D, E, F:", "", "",
                            eINPUT_EDITABLE, "", nullptr, 1, 1, 9, true);
   iInput->setShowRightLabel(true);
   iInput->setRightLabel("Text2");
   iInput->setValidateOnReverse(false);
   iInput->addValidOption("D", "Item D");
   iInput->addValidOption("E", "Item E");
   iInput->addValidOption("F", "Item F");
   iInput->autoGenerateExtendedHelp(true);
   iInput->setForceUpper(true);
   iInput = iForm.append(3, 1, 1, 25, "G, H, I:", "", "",
                            eINPUT_EDITABLE, "", nullptr, 1, 1, 9, true);
   iInput->setShowRightLabel(true);
   iInput->setRightLabel("Text3");
   iInput->setForceUpper(true);
   iInput->addValidOption("G", "Item G");
   iInput->addValidOption("H", "Item H");
   iInput->addValidOption("I", "Item I");
   iInput = iForm.append(1, 37, 1, 25, "J, K, L:", "", "",
                            eINPUT_EDITABLE, "", nullptr, 1, 1, 9, true);
   iInput->setShowRightLabel(true);
   iInput->setRightLabel("Text4");
   iInput->setForceUpper(true);
   iInput->addValidOption("J", "Item J");
   iInput->addValidOption("K", "Item K");
   iInput->addValidOption("L", "Item L");
   iInput = iForm.append(2, 37, 1, 25, "M, N, O:", "", "",
                            eINPUT_EDITABLE, "", nullptr, 1, 1, 9, true);
   iInput->setShowRightLabel(true);
   iInput->setRightLabel("Text5");
   iInput->setForceUpper(true);
   iInput->addValidOption("M", "Item M");
   iInput->addValidOption("N", "Item N");
   iInput->addValidOption("O", "Item O");
   iInput = iForm.append(3, 37, 1, 25, "P, Q, R:", "", "",
                            eINPUT_EDITABLE, "", nullptr, 1, 1, 9, true);
   iInput->setShowRightLabel(true);
   iInput->setRightLabel("Text6");
   iInput->setForceUpper(true);
   iInput->addValidOption("P", "Item P");
   iInput->addValidOption("Q", "Item Q");
   iInput->addValidOption("R", "Item R");
   iForm.showModal();
} // cxMultiLineInputWithRightLabelOnForm

void cxMultiLineInputAutoGenerateHelpString()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 20, "A, B, or C:");
   iInput.setForceUpper(true);
   iInput.autoGenerateExtendedHelp(true);
   iInput.addValidOption("A", "Item A");
   iInput.addValidOption("B", "Item B");
   iInput.addValidOption("C", "Item C");
   iInput.showModal();
} // cxMultiLineInputAutoGenerateHelpString

string storeSearchFormOnEnter(void *theSearchPanel, void *unused, void *unused2, void *unused3)
{
   if (theSearchPanel == nullptr)
   {
      return("");
   }
   cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   shared_ptr<cxMenu> iMenu = pSearchPanel->getMenu();
   // Note: This is where you would populate the menu from something (like an SQL call)
   iMenu->clear(false);
   iMenu->append("Atlanta    1", 1);
   iMenu->append("Portland   2", 2);
   iMenu->append("Vancouver  3", 3);

   return("");
} // storeSearchFormOnEnter

string storeSearchMenuOnEnter(void *theSearchPanel, void *unused, void *unused2, void *unused3)
{
   if (theSearchPanel == nullptr)
   {
      return("");
   }

   //cxSearchPanel *pSearchPanel = static_cast<cxSearchPanel*>(theSearchPanel);
   //shared_ptr<cxMenu> iMenu = pSearchPanel->getMenu();
   //cxBase::messageBox("The '" + iMenu->getCurrentItemText() + "' item was selected.", "ha ha");

   return("");
} // storeSearchMenuOnEnter

string storeOnLeave(void *theInput, void *unused)
{
   if (theInput == nullptr)
   {
      return("");
   }

   string retval;
   cxMultiLineInput *pInput = static_cast<cxMultiLineInput*>(theInput);
   string tmpStore=pInput->getValue();
   string rightLabel="Unk";
   if (tmpStore == "1")
   {
      rightLabel="Atlanta";
   }
   else if (tmpStore == "2")
   {
      rightLabel="Portland";
   }
   else if (tmpStore == "3")
   {
      rightLabel="Vancouver";
   }

   // refresh the right label
   pInput->setRightLabel(rightLabel, true);

   return(retval);
} // storeOnLeave

string storeSearch(void *theInput, void *unused)
{
   if (theInput == nullptr)
   {
      return("");
   }

   string retval;
   cxMultiLineInput *pInput = static_cast<cxMultiLineInput*>(theInput);
   cxSearchPanel iSearchPanel(nullptr, 1, 0, 20, 70, "Search", "Menu");
   iSearchPanel.setFormHeight(3, false); // TODO: When setting the form, the status goes away. Why?

   shared_ptr<cxMultiLineInput> input = iSearchPanel.appendToForm(1, 1, 1, 30, "Keyword:", "", "Enter partial information for searching", eINPUT_EDITABLE, "keyword");
   // set keyword to the value that was in the input form
   input->setValue(pInput->getValue());
   input->setExtendedHelp("Enter information to limit the search. A case insensitve search is performed on each column displayed in the results.");

   shared_ptr<cxMenu> iMenu = iSearchPanel.getMenu();
   iMenu->setStatus("TAB=Form  /=Search  HOME=First  END=Last  UP=Previous  DOWN=Next", true);
   // TODO: Need to have ENTER actually leave storeSearch() and set the value in the "store" field on the cxForm

   //shared_ptr<cxForm> iForm = pSearchPanel->getForm();

   iSearchPanel.setFormOnEnterFunction(storeSearchFormOnEnter, &iSearchPanel, nullptr, nullptr, nullptr);
   long searchRetval=iSearchPanel.showModal();
   if (searchRetval != cxID_QUIT)
   {
      //messageBox("searchRetval:" + toString(searchRetval));
      //cxBase::messageBox("Selected item:" + iSearchPanel.getSelectedItem() + ":");
      // Get the store # from the selected item - Get the selected item and
      //  strip off the first part - Look for the last space and get the
      //  substring after that position.
      string storeNum = iSearchPanel.getSelectedItem();
      unsigned spacePos = storeNum.rfind(' ');
      if (spacePos != string::npos)
      {
         storeNum = storeNum.substr(spacePos+1);
      }
      // Set the value in the input
      pInput->setValue(storeNum, true);
   }
   return(retval);
} // storeSearch

void itemSales()
{
   cxForm iForm(nullptr, 1, 0, 5, 79, "Item Sales");
   // Disable input validation when the user presses the jump-to key (F4) so
   //  that we can jump from all inputs.
   iForm.allValidateOnJumpKey(false);

   int row=2;

   // store
   pair<shared_ptr<cxMultiLineInput>, shared_ptr<cxMultiLineInput> > inputs = iForm.appendPair(row,
                               2, 1, 8, "Store:", "", "Store number", eINPUT_EDITABLE,
                               "store");
   inputs.first->setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
   inputs.first->setRangeInt(1, 99);
   inputs.first->setExtendedHelp("Enter store number.");
   inputs.first->setAllowBlank(false);
   inputs.first->setValidateOnReverse(false);
   inputs.first->setExtendedHelp("Enter store. (" + toString(inputs.first->getRangeLowInt()) + "-" + toString(inputs.first->getRangeHighInt()) + ")");
   inputs.first->setStatus("Store Number (" + toString(inputs.first->getRangeLowInt()) + "-" + toString(inputs.first->getRangeHighInt()) + ")");
   // storeSearch() will set the input from the search.
   inputs.first->setKeyFunction('+', storeSearch, inputs.first.get(), nullptr, false);

   // right label stuffs
   inputs.first->setRightLabelOffset(5, false);
   inputs.first->setShowRightLabel(true);
   inputs.first->setOnLeaveFunction(storeOnLeave, inputs.first.get(), nullptr);

   inputs.second->setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
   inputs.second->setRangeInt(1, 99);
   inputs.second->resize(1, 2);
   inputs.second->setExtendedHelp("Enter store number.");
   inputs.second->setAllowBlank(false);
   inputs.second->setValidateOnReverse(false);
   inputs.second->setExtendedHelp("Enter store. (" + toString(inputs.second->getRangeLowInt()) + "-" + toString(inputs.second->getRangeHighInt()) + ")");
   inputs.second->setStatus("Store Number (" + toString(inputs.second->getRangeLowInt()) + "-" + toString(inputs.second->getRangeHighInt()) + ")");
   // storeSearch() will set the input from the search.
   inputs.second->setKeyFunction('+', storeSearch, inputs.second.get(), &iForm, false);

   // right label stuffs
   inputs.second->setRightLabelOffset(10, false);
   inputs.second->setShowRightLabel(true);
   inputs.second->setOnLeaveFunction(storeOnLeave, inputs.second.get(), nullptr);

   // '+' character
   shared_ptr<cxMultiLineInput> iInput = iForm.append(row, 13, 1, 1, "+", "", "", eINPUT_READONLY, "storePlus");
   iInput->setCanBeEditable(false); // so you cannot jump to this input
   // '/' character
   iInput = iForm.append(row, 22, 1, 1, "/", "", "", eINPUT_READONLY, "storeSlash");
   iInput->setCanBeEditable(false); // so you cannot jump to this input

   // item
   row++;
   iInput = iForm.append(row, 2, 1, 13, "Item:", "", "Item number", eINPUT_EDITABLE, "item");
   iInput->setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
   iInput->setExtendedHelp("Enter item number.");
   iInput->setRangeInt(0, 99999999);
   iInput->setAllowBlank(false);
   iInput->setValidateOnReverse(false);
   iInput->setExtendedHelp("Enter item number. (" + toString(iInput->getRangeLowInt()) + "-" + toString(iInput->getRangeHighInt()) + ")");
   iInput->setStatus("Item Number (" + toString(iInput->getRangeLowInt()) + "-" + toString(iInput->getRangeHighInt()) + ")");

   iForm.setAutoExit(true);

   long formRetval=iForm.showModal();
   while (formRetval != cxID_QUIT)
   {
      cxScrolledWindow iScrolledWindow(nullptr, 0, 0, -1, -1, "Item Sales Report", "", "");
      iScrolledWindow.addMessageLineBelow("one");
      iScrolledWindow.addMessageLineBelow("two");
      iScrolledWindow.addMessageLineBelow("three");
      iScrolledWindow.showModal();
      iScrolledWindow.hide();
      formRetval = iForm.showModal();
   }
} // itemSales

void cxFormAppendPair()
{
   try
   {
      cxMenu iMenu(nullptr, 0, 0, 4, 11, "Menu");
      iMenu.center(false);
      iMenu.append("Version 1", 1);
      iMenu.append("Version 2", 2);
      long returnCode = iMenu.showModal();
      iMenu.hide();
      cxForm iForm(nullptr, 1, 0, 5, 75, "Form");
      iForm.setAutoExit(true);
      if (returnCode == 1)
      {
         pair<shared_ptr<cxMultiLineInput>, shared_ptr<cxMultiLineInput> > inputs = iForm.appendPair(1, 1,
               1, 30, "Store #:", "", "", eINPUT_EDITABLE, "stno");
         if (inputs.first != nullptr)
         {
            cxBase::messageBox("First input name:" + inputs.first->getName() + ":");
            inputs.first->setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
         }
         else
         {
            cxBase::messageBox("Warning: The first input is nullptr");
         }
         if (inputs.second != nullptr)
         {
            cxBase::messageBox("Second input name:" + inputs.second->getName() + ":");
            inputs.second->setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
         }
         else
         {
            cxBase::messageBox("Warning: The second input is nullptr");
         }

         iForm.showModal();
      }
      else if (returnCode == 2)
      {
         shared_ptr<cxMultiLineInput> input1 = make_shared<cxMultiLineInput>(nullptr, 0, 0, 1, 20,
                                       "User:", eBS_NOBORDER, eINPUT_EDITABLE,
                                       eINPUT_TYPE_TEXT, nullptr, 1, 1, 7, true);
         input1->setName("user");
         shared_ptr<cxMultiLineInput> input2 = make_shared<cxMultiLineInput>(nullptr, 0, 0, 1, 20,
                                       "User:", eBS_NOBORDER, eINPUT_EDITABLE,
                                       eINPUT_TYPE_TEXT, nullptr, 1, 1, 7, true);
         input1->setName("user");
         iForm.appendPair(input1, input2, 1, 1);
         input1->setRightLabel("1234567", false);
         input2->setRightLabel("1234567", false);
         iForm.showModal();
         map<string, string> values;
         iForm.getValues(values, false, true);
         map<string, string>::iterator iter = values.begin();
         for (; iter != values.end(); ++iter)
         {
            cxBase::messageBox(iter->first + ":" + iter->second + ":");
         }
      }
   }
   catch (const cxWidgetsException& e)
   {
      cxBase::messageBox("cxWidgets exception caught: " + e.getErrorMsg());
   }
   catch (const std::exception& e)
   {
      cxBase::messageBox("std::exception caught: " + string(e.what()));
   }
   catch (...)
   {
      cxBase::messageBox("Unknown exception caught");
   }
} // cxFormAppendPair

void cxFormAppendComboBoxPair()
{
   cxForm iForm(nullptr, 1, 0, 4, 70, "Form");
   try
   {
      pair<shared_ptr<cxComboBox>, shared_ptr<cxComboBox> > comboBoxes = iForm.appendComboBoxPair(1, 1,
                           1, 30, "Store:", "", "", eINPUT_EDITABLE, "store");
      if (comboBoxes.first != nullptr)
      {
         cxBase::messageBox("First combo box name:" + comboBoxes.first->getName() + ":");
         comboBoxes.first->appendToMenu("Portland");
         comboBoxes.first->appendToMenu("Aloha");
         comboBoxes.first->appendToMenu("Vancouver");
         comboBoxes.first->appendToMenu("Seattle");
      }
      else
      {
         cxBase::messageBox("Warning: The first combo box is nullptr");
      }
      if (comboBoxes.second != nullptr)
      {
         cxBase::messageBox("Second combo box name:" + comboBoxes.second->getName() + ":");
         comboBoxes.second->appendToMenu("Portland");
         comboBoxes.second->appendToMenu("Aloha");
         comboBoxes.second->appendToMenu("Vancouver");
         comboBoxes.second->appendToMenu("Seattle");
      }
      else
      {
         cxBase::messageBox("Warning: The second combo box is nullptr");
      }
   }
   catch (const cxWidgetsException& e)
   {
      cxBase::messageBox("cxWidgets exception caught: " + e.getErrorMsg());
   }
   catch (const std::exception& e)
   {
      cxBase::messageBox("std::exception caught: " + string(e.what()));
   }
   catch (...)
   {
      cxBase::messageBox("Unknown exception caught");
   }
   iForm.showModal();
} // cxFormAppendComboBoxPair

void cxMultiLineInputSetValue()
{
   cxBase::messageBox("This test uses assert() to validate the tests.  If "
                      "the program doesn't bail, then all the tests passed.");

   cxMultiLineInput iInput(nullptr, 1, 0, 1, 70, "Input:");
   // Try it with text validation
   iInput.setInputType(eINPUT_TYPE_TEXT);
   iInput.useBuiltInValidator(); // This is set up by default, but do it anyway..
   iInput.setAllowBlank(true);
   // There are no valid strings set up, so any string should work:
   assert(iInput.setValue("New value", false));
   assert(iInput.getValue() == "New value");
   // Add some valid strings and try to set a valid value, and then an invalid
   //  value
   iInput.setValidOptions("YN");
   assert(iInput.getValue() == "");
   assert(iInput.setValue("Y", false));
   assert(iInput.getValue() == "Y");
   assert(iInput.setValue("N", false));
   assert(iInput.getValue() == "N");
   assert(!iInput.setValue("Z", false)); // Should fail and not change the value
   assert(iInput.getValue() == "N");
   // Make sure we can set a blank value (allow blank is true)
   assert(iInput.setValue("", false));
   assert(iInput.getValue() == "");

   // Test not allowing blank
   iInput.setValue("Y");
   iInput.setAllowBlank(false);
   assert(!iInput.setValue("", false));
   assert(iInput.getValue() == "Y");

   // Test with numeric floating-point validation
   iInput.setAllowBlank(true);
   assert(iInput.setValue("", false));
   iInput.setInputType(eINPUT_TYPE_NUMERIC_FLOATING_PT);
   assert(iInput.setValue("2.3", false));
   assert(iInput.getValue() == "2.3");
   assert(iInput.setValue("-2.3", false));
   assert(iInput.getValue() == "-2.3");
   assert(iInput.setValue("0.0", false));
   assert(iInput.getValue() == "0.0");
   assert(iInput.setValue("0", false));
   assert(iInput.getValue() == "0");
   // Bad ones
   assert(!iInput.setValue("abc", false));
   assert(iInput.getValue() == "0");
   assert(!iInput.setValue("--2.3", false));
   assert(iInput.getValue() == "0");
   assert(!iInput.setValue("2.3.4", false));
   assert(iInput.getValue() == "0");
   iInput.setAllowBlank(false);
   assert(!iInput.setValue("", false));
   assert(iInput.getValue() == "0");

   // Test with numeric whole validation
   iInput.setAllowBlank(true);
   iInput.setValue("", false);
   iInput.setInputType(eINPUT_TYPE_NUMERIC_WHOLE);
   assert(iInput.setValue("0", false));
   assert(iInput.getValue() == "0");
   assert(iInput.setValue("1", false));
   assert(iInput.getValue() == "1");
   assert(iInput.setValue("2", false));
   assert(iInput.getValue() == "2");
   assert(iInput.setValue("3", false));
   assert(iInput.getValue() == "3");
   assert(iInput.setValue("4", false));
   assert(iInput.getValue() == "4");
   assert(iInput.setValue("5", false));
   assert(iInput.getValue() == "5");
   assert(iInput.setValue("6", false));
   assert(iInput.getValue() == "6");
   assert(iInput.setValue("7", false));
   assert(iInput.getValue() == "7");
   assert(iInput.setValue("8", false));
   assert(iInput.getValue() == "8");
   assert(iInput.setValue("9", false));
   assert(iInput.getValue() == "9");
   assert(iInput.setValue("247", false));
   assert(iInput.getValue() == "247");
   assert(iInput.setValue("-123"));
   assert(iInput.getValue() == "-123");
   // Bad ones
   assert(!iInput.setValue("2.3"));
   assert(iInput.getValue() == "-123");
   assert(!iInput.setValue("--123"));
   assert(iInput.getValue() == "-123");
   assert(!iInput.setValue("abc"));
   assert(iInput.getValue() == "-123");
   iInput.setAllowBlank(false);
   assert(!iInput.setValue(""));
   assert(iInput.getValue() == "-123");
} // cxMultiLineInputSetValue

void cxMultiLineInputMaxInputLength()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 2, 10, "Input:", eBS_NOBORDER);
   iInput.setMaxInputLength(10);
   cxBase::messageBox("The max input length is " + toString(iInput.getMaxInputLength()));
   iInput.showModal();
   cxBase::messageBox("Value:" + iInput.getValue() + ":");
} // cxMultiLineInputMaxInputLength

void cxMultiLineInputExtendedHelpColorsAndAttrs()
{
   cxMultiLineInput iInput(nullptr, 1, 0, 1, 10, "Input:", eBS_NOBORDER);
   iInput.setExtendedHelp("This is the extended help.");
   cxBase::messageBox("Using default extended help color & attribute");
   iInput.showModal();

   iInput.setExtendedHelpColor(eGREEN_BLACK);
   iInput.setExtendedHelpAttribute(A_BOLD);
   cxBase::messageBox("Changed the extended help color & attribute");
   iInput.showModal();
} // cxMultiLineInputExtendedHelpColorsAndAttrs

void cxWindowMessageNewlines()
{
   cxWindow iWindow(nullptr, 1, 0, 8, 40, "cxWindow", "This message\nhad a newline.");
   iWindow.showModal();
} // cxWindowMessageNewlines

void cxMenuAltItemText()
{
   cxMenu iMenu(nullptr, 1, 0, 10, 20, "Menu");
   // Append some items with alternate item text different than the display
   //  text
   for (int i = 1; i <= 20; ++i)
   {
      iMenu.append("Item " + toString(i), i, "", cxITEM_NORMAL, false,
                   "Alternate text for item " + toString(i));
   }
   // Make sure one of the items exists using itemExists() with both options for
   //  pUseDisplayText
   assert(iMenu.itemExists("Item 2", true));
   assert(iMenu.itemExists("Alternate text for item 2", false));
   // Remove item 2 using its display text (2nd parameter true)
   iMenu.remove("Item 2", true, false, false);
   // Remove item 3 using its alternate text (2nd parameter false)
   iMenu.remove("Alternate text for item 3", false, false, false);
   // Check to make sure items 2 and 3 don't exist using itemExists()
   assert(!iMenu.itemExists("Item 2", true));
   assert(!iMenu.itemExists("Alternate text for item 2", false));
   assert(!iMenu.itemExists("Item 3", true));
   assert(!iMenu.itemExists("Alternate text for item 3", false));
   // Test some other functions using display text & alternate text
   assert(iMenu.getReturnCode("Item 4", true) == 4);
   assert(iMenu.getReturnCode("Alternate text for item 4", false) == 4);
   // Get an item's display text & alternate text via its index
   assert(iMenu.getItemText(0, true) == "Item 1");
   assert(iMenu.getItemText(0, false) == "Alternate text for item 1");
   // Get an item's display text & alternate text via its return code
   assert(iMenu.getItemTextByReturnCode(1, true) == "Item 1");
   assert(iMenu.getItemTextByReturnCode(1, false) == "Alternate text for item 1");

   // Show the menu & display the user's selection
   iMenu.showModal();
   cxBase::messageBox("Your choice (display text):" + iMenu.getCurrentItemText(true) + ":");
   cxBase::messageBox("Your choice (alt text):" + iMenu.getCurrentItemText(false) + ":");
   // Get the current item's display text & alternate display text
   cxBase::messageBox("Current item display text:" + iMenu.getCurrentItemText(true) + ":");
   cxBase::messageBox("Current item alternate text:" + iMenu.getCurrentItemText(false) + ":");
} // cxMenuAltItemText

void cxComboBoxNotEditableMenuEnabled()
{
   // This tests a cxComboBox that is not "editable", but its drop-dowm nenu
   //  is enabled.  This means that the user can't type in the input, but they
   //  can choose something from the drop-down menu.
   cxComboBox iComboBox(nullptr, 1, 0, 1, 20, "Input:");
   iComboBox.appendToMenu("Item 1");
   iComboBox.appendToMenu("Item 2");
   iComboBox.appendToMenu("Item 3");
   iComboBox.setEditable(false);
   iComboBox.showModal();
   cxBase::messageBox("Value:" + iComboBox.getValue() + ":");
} // cxComboBoxNotEditableMenuEnabled

void cxMultiLineInputOverrideOnKeypress()
{
   floatingPtInputWithRightLabel iInput(nullptr, 1, 0, 1, 25, "Floating-pt. #:");
   iInput.showModal();
} // cxMultiLineInputOverrideOnKeypress

void cxWindowAddMessageLinesAbove()
{
   cxWindow iWindow(nullptr, 1, 0, 12, 50, "cxWindow", "Original message");
   iWindow.addMessageLineAbove("First line added above");
   vector<string> moreLines;
   moreLines.push_back("Another line 1");
   moreLines.push_back("Another line 2");
   moreLines.push_back("Another line 3");
   moreLines.push_back("Another line 4");
   iWindow.addMessageLinesAbove(moreLines);
   iWindow.show();
   getch();
} // cxWindowAddMessageLinesAbove

void cxWindowVariousBorderStyles()
{
   const eBorderStyle borderStyles[] = { eBS_NOBORDER, eBS_SINGLE_LINE, eBS_DOUBLE_LINE, eBS_DOUBLE_TOP_SINGLE_SIDES,
                                         eBS_SINGLE_TOP_DOUBLE_SIDES, eBS_SPACE };
   const size_t numBorderStyles = sizeof(borderStyles) / sizeof(borderStyles[0]);
   for (size_t i = 0; i < numBorderStyles; ++i)
   {
      string borderStyleStr = eBorderStyleToStr(borderStyles[i]);
      string message = "Border style: " + borderStyleStr;
      // cxWindow *pParentWindow, int pRow, int pCol, int pHeight, int pWidth, string pTitle, string pMessage,
      // string pStatus, eBorderStyle pBorderStyle
      unique_ptr<cxWindow> iWindow = make_unique<cxWindow>(nullptr, 2, 0, 12, 50, borderStyleStr, message, "", borderStyles[i]);
      iWindow->show();
      getch();
   }
} // cxWindowBorderStyles

// Demonstrates cxDatePicker: opens the calendar dialog defaulting to today;
// reports the chosen date (or that the user cancelled).
void cxDatePickerTest()
{
   cxBase::messageBox("On the cxDatePicker, use tab to move "
                      "between controls, arrow keys to navigate the "
                      "calendar, and Enter/OK to confirm a date.");
   cxDatePicker picker(nullptr); // centered, defaults to today
   long result = picker.showModal();
   if (result == cxID_OK)
   {
      const cxDate& chosen = picker.getSelectedDate();
      cxBase::messageBox("You chose: " + chosen.toString());
   }
   else
   {
      cxBase::messageBox("Date selection was cancelled.");
   }
} // cxDatePickerTest

void cxOpenFileDialogTest()
{
   cxBase::messageBox("cxOpenFileDialog: Use Tab to move between controls. "
                      "Arrow keys navigate directory/file lists. "
                      "Enter on a directory enters it; Enter on a file confirms. "
                      "Left/Right on File type cycles filters.");

   cxOpenFileDialog dlg;
   dlg.addFilter("C/C++ Source (*.cpp;*.c;*.h)", "*.cpp;*.c;*.h");
   dlg.addFilter("Text Files (*.txt)", "*.txt");
   dlg.setAllowNewFile(true); // This is the default, but enable it anyway

   long result = dlg.showModal();
   if (result == cxID_OK)
   {
      cxBase::messageBox("Selected file: " + dlg.getSelectedFilePath());
   }
   else
   {
      cxBase::messageBox("File selection was cancelled.");
   }
} // cxOpenFileDialogTest