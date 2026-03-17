#ifndef __CXCOLORS__H__
#define __CXCOLORS__H__

/**
 * \brief cxColors.h - contains an enumeration of the
 *  colors available for the cxWidgets objects.
 *
 *  Note: These are arranged so that the odd ones
 *   are bright (w/ the A_BOLD attribute set) -
 *   odd ones will have the rightmost bit (the
 *   one bit) set to 1.
 *
 *  Note to programmers: If you add a color, make
 *   sure to edit the init() function in cxBase.cpp
 *   and use init_pair() to initialize the ncurses
 *   color pair.  Also, make sure bright colors are
 *   odd and non-bright colors are even - cxWindow
 *   and other classes that display text use the ones
 *   bit (LSB) to check for brightness - if it's
 *   1, then the A_BOLD attribute will be used,
 *   effectively making the text a bright version
 *   of the color that is set.
 *
 *  Note that the total possible number of colors
 *   that ncurses can handle is fairly limited.
 *   Currently there are more colors listed here
 *   than ncurses can handle, so the ones near
 *   the end of the list won't actually work
 *   correctly.  In Fedora Core 4, with a
 *   version of ncurses current as of 10/12/05,
 *   the number of possible colors is 63.
 *
 * Copyright (C) 2005-2007 Michael H. Kinney
 *
 * \author $Author: erico $
 * \version $Revision: 1.6 $
 * \date $Date: 2007/12/04 00:23:46 $
 *
 */

#define cxCOLOR_BRIGHT 1
/**
 * \enum e_cxColors These are the color definitions used in cxWidgets.
 */
enum e_cxColors
{
   eDEFAULT = 0,
   // From the following line down,
   //  it's recommended that colors are
   //  added in pairs (a regular version
   //  and a 'bright' version) so that
   //  the numbers associated with the
   //  colors have the correct even &
   //  oddness.
   eRED_BLACK = 2,
   eBRTRED_BLACK,
   eGREEN_BLACK,
   eBRTGREEN_BLACK,
   eBROWN_BLACK,
   eYELLOW_BLACK,
   eBLUE_BLACK,
   eBRTBLUE_BLACK,
   eCYAN_BLACK,
   eBRTCYAN_BLACK,
   eMAGENTA_BLACK,
   eBRTMAGENTA_BLACK,
   eGRAY_BLACK,
   eWHITE_BLACK,
   eBLACK_BLACK,

   eRED_WHITE = eBLACK_BLACK + 2,
   eBRTRED_WHITE,
   eGREEN_WHITE,
   eBRTGREEN_WHITE,
   eBROWN_WHITE,
   eYELLOW_WHITE,
   eBLUE_WHITE,
   eBRTBLUE_WHITE,
   eCYAN_WHITE,
   eBRTCYAN_WHITE,
   eMAGENTA_WHITE,
   eBRTMAGENTA_WHITE,
   eGRAY_WHITE,
   eWHITE_WHITE,
   eBLACK_WHITE,

   eRED_BLUE = eBLACK_WHITE + 2,
   eBRTRED_BLUE,
   eGREEN_BLUE,
   eBRTGREEN_BLUE,
   eBROWN_BLUE,
   eYELLOW_BLUE,
   eBLUE_BLUE,
   eBRTBLUE_BLUE,
   eCYAN_BLUE,
   eBRTCYAN_BLUE,
   eMAGENTA_BLUE,
   eBRTMAGENTA_BLUE,
   eGRAY_BLUE,
   eWHITE_BLUE,
   eBLACK_BLUE,

   eRED_GREEN = eBLACK_BLUE + 2,
   eBRTRED_GREEN,
   eBROWN_GREEN,
   eYELLOW_GREEN,
   eBLUE_GREEN,
   eBRTBLUE_GREEN,
   eCYAN_GREEN,
   eBRTCYAN_GREEN,
   eMAGENTA_GREEN,
   eBRTMAGENTA_GREEN,
   eGRAY_GREEN,
   eWHITE_GREEN,
   eGREEN_GREEN,
   eBRTGREEN_GREEN,
   eBLACK_GREEN,

   eRED_RED = eBLACK_GREEN + 2,
   eBRTRED_RED,
   eGREEN_RED,
   eBRTGREEN_RED,
   eBROWN_RED,
   eYELLOW_RED,
   eBLUE_RED,
   eBRTBLUE_RED,
   eCYAN_RED,
   eBRTCYAN_RED,
   eMAGENTA_RED,
   eBRTMAGENTA_RED,
   eGRAY_RED,
   eWHITE_RED,
   eBLACK_RED,

   eRED_CYAN = eBLACK_RED + 2,
   eBRTRED_CYAN,
   eGREEN_CYAN,
   eBRTGREEN_CYAN,
   eBROWN_CYAN,
   eYELLOW_CYAN,
   eBLUE_CYAN,
   eBRTBLUE_CYAN,
   eCYAN_CYAN,
   eBRTCYAN_CYAN,
   eMAGENTA_CYAN,
   eBRTMAGENTA_CYAN,
   eGRAY_CYAN,
   eWHITE_CYAN,
   eBLACK_CYAN
   // Note: Next color added should be
   //  eBLACK_CYAN + 2 so that it's even
};

#endif
