#ifndef __CXMESSAGEDIALOGSTYLES_H__
#define __CXMESSAGEDIALOGSTYLES_H__

// Copyright (c) 2006-2007 Michael H. Kinney

// This file defines an enumeration for message dialog styles, which control
//  what buttons are shown on a message dialog.

enum eMessageDialogStyles
{
   cxYES = 2,          // Show Yes button
   cxOK = 4,           // Show OK button
   cxNO = 8,           // Show No button
   cxCANCEL = 16,       // Show Cancel button
   cxYES_NO = (cxYES | cxNO),   // Show Yes and No buttons
   cxOK_CANCEL = (cxOK | cxCANCEL), // Show OK and Cancel buttons
   cxYES_DEFAULT = 0,  // (default)
   cxNO_DEFAULT = 32    // Set No button to default
};

#endif
