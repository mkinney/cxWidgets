#ifndef __CXMENUITEMTYPE_H__
#define __CXMENUITEMTYPE_H__
// Copyright (c) 2005-2007 Michael H. Kinney
//
// This header defines the enumeration
//  cxMenuItemType, which is used by cxMenu for
//  different tyeps of menu items.

enum cxMenuItemType
{
   cxITEM_NORMAL = 0,
   cxITEM_SUBMENU,
   cxITEM_POPUPMENU,
   cxITEM_SEPARATOR,
   cxITEM_UNSELECTABLE
};

#endif
