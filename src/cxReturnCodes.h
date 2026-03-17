#ifndef __CXRETURNCODES_H__
#define __CXRETURNCODES_H__
// Copyright (c) 2005-2007 Michael H. Kinney
//
// This file defines return codes used by
//  various types of windows.

/**
* \enum eReturnCode Defines return codes used in various windows:
* cxID_OK: This is returned from modal dialog boxes when the user
* selects "OK" or "Yes".
* cxID_CANCEL: This is returned from modal dialog boxes when the
* user selects "Cancel" or "No".
* cxID_EXIT: This is returned from modal windows when the user
* completes the input successfully.
* cxID_QUIT: This is returned from modal windows when the user
* chooses to exit the window without doing anything.
* cxFIRST_AVAIL_RETURN_CODE: When using cxMenu or other windows
* that need return codes, this is the first return code you
* should use.
*/
enum eReturnCode
{
   cxID_OK = -400L,
   cxID_CANCEL = -399L,
   cxID_EXIT = -398L,
   cxID_QUIT = -397L,
   cxFIRST_AVAIL_RETURN_CODE = 0L
};

#endif
