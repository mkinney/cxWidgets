#ifndef __FLOATINGPTINPUTWITHRIGHTLABEL_H__
#define __FLOATINGPTINPUTWITHRIGHTLABEL_H__

// Copyright (c) 2007 Michael H. Kinney

// This is a class that derives from cxMultiLineInput so that the onKeypress()
//  function can be overridden.

#include "cxMultiLineInput.h"
#include <string>

class floatingPtInputWithRightLabel : public cxMultiLineInput
{
   public:
      explicit floatingPtInputWithRightLabel(cxWindow *pParentWindow = nullptr,
                                int pRow = 0, int pCol = 0, int pHeight = 1,
                                int pWidth = DEFAULT_WIDTH,
                                const std::string& pLabel = "",
                                eBorderStyle pBorderStyle = eBS_NOBORDER,
                                eInputOptions pInputOption = eINPUT_EDITABLE,
                                std::string *pExtValue = nullptr);

      ~floatingPtInputWithRightLabel();

      // In addition do doing the per-key validation, this will update the
      //  right label.
      std::string onKeypress() override;
};

#endif
