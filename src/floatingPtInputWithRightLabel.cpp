#include "floatingPtInputWithRightLabel.h"
using std::string;

floatingPtInputWithRightLabel::floatingPtInputWithRightLabel(cxWindow *pParentWindow,
                                int pRow, int pCol, int pHeight, int pWidth,
                                const string& pLabel, eBorderStyle pBorderStyle,
                                eInputOptions pInputOption, string *pExtValue)
   : cxMultiLineInput(pParentWindow, pRow, pCol, pHeight, pWidth, pLabel,
                      pBorderStyle, pInputOption, eINPUT_TYPE_NUMERIC_FLOATING_PT,
                      pExtValue, 1, 1, (pWidth - (int)(pLabel.length())), true)
{
} // constructor

floatingPtInputWithRightLabel::~floatingPtInputWithRightLabel()
{
} // destructor

string floatingPtInputWithRightLabel::onKeypress()
{
   string retval = cxMultiLineInput::onKeypress();
   // Update the right label with the input value
   setRightLabel(getValue(), true);

   return(retval);
} // onKeypress
