// Copyright (c) 2026 Eric N. Oulashin

#include "cxBorderChars.h"

cxBorderChars::cxBorderChars(eBorderStyle pBorderStyle)
{
   switch (pBorderStyle)
   {
      case eBS_SINGLE_LINE:
      default:
         left = right = ACS_VLINE;
         top = bottom = ACS_HLINE;
         topLeft = ACS_ULCORNER;
         topRight = ACS_URCORNER;
         bottomLeft = ACS_LLCORNER;
         bottomRight = ACS_LRCORNER;
         break;
      case eBS_DOUBLE_LINE:
         left = right = 0x000000ba;
         top = bottom = 0x000000cd;
         topLeft = 0x000000c9;
         topRight = 0x000000bb;
         bottomLeft = 0x000000c8;
         bottomRight = 0x000000bc;
         break;
      case eBS_DOUBLE_TOP_SINGLE_SIDES:
         left = right = ACS_VLINE;
         top = bottom = 0x000000cd;
         topLeft = 0x000000d5;
         topRight = 0x000000b8;
         bottomLeft = 0x000000d4;
         bottomRight = 0x000000be;
         break;
      case eBS_SINGLE_TOP_DOUBLE_SIDES:
         left = right = 0x000000ba;
         top = bottom = ACS_HLINE;
         topLeft = 0x000000d6;
         topRight = 0x000000b7;
         bottomLeft = 0x000000d3;
         bottomRight = 0x000000bd;
         break;
      case eBS_NOBORDER:
      case eBS_SPACE:
         left = right = top = bottom = topLeft = topRight = bottomLeft = bottomRight = ' ';
         break;
   }
}
