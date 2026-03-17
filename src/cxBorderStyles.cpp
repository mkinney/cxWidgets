// Copyright (c) 2026 Eric N. Oulashin

#include "cxBorderStyles.h"
using std::string;

string eBorderStyleToStr(eBorderStyle borderStyle)
{
    switch (borderStyle)
    {
        case eBS_NOBORDER:
            return "No Border";
        case eBS_SINGLE_LINE:
            return "Single Line";
        case eBS_DOUBLE_LINE:
            return "Double Line";
        case eBS_DOUBLE_TOP_SINGLE_SIDES:
            return "Double Top, Single Sides";
        case eBS_SINGLE_TOP_DOUBLE_SIDES:
            return "Single Top, Double Sides";
        case eBS_SPACE:
            return "Spaces";
        default:
            return "Unknown";
    }
}