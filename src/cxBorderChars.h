#ifndef __CXBORDERCHARS_H__
#define __CXBORDERCHARS_H__

/**
 * \brief cxBorderChars.h - Defines border characters to be used
 * in drawing a box (i.e., in cxWindow and all its derivitives)
 *
 * Copyright (C) 2026 Eric N. Oulashin
 *
 * \author $Author: erico $
 * \version $Revision: 1.0 $
 * \date $Date: 2026/02/09 16:09:00 $
 *
 * Date     User    Description
 *
 */

#include "cxBorderStyles.h"
#include <ncurses.h>

/**
* \struct cxBorderChars Defines border characters to be used in drawing a box (i.e., in cxWindow and all its derivitives).
* The characters all defult to a single line.
* Member variables are left (left side), right (right side), top (top line), bottom (bottom line), topLeft (top left corner),
* topRight (top right corner), bottomLeft (bottom left corner), and bottomRight (bottom right corner).
*/
struct cxBorderChars
{
   cxBorderChars(eBorderStyle pBorderStyle = eBS_SINGLE_LINE);

   chtype left = ACS_VLINE;
   chtype right = ACS_VLINE;
   chtype top = ACS_HLINE;
   chtype bottom = ACS_HLINE;
   chtype topLeft = ACS_ULCORNER;
   chtype topRight = ACS_URCORNER;
   chtype bottomLeft = ACS_LLCORNER;
   chtype bottomRight = ACS_LRCORNER;
};

#endif
