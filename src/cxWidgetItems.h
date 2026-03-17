#ifndef __CXWIDGETITEMS_H__
#define __CXWIDGETITEMS_H__
// Copyright (c) 2006-2007 Michael H. Kinney
//

/**
 * @file cxWidgetItems.h
 * \brief Defines an enumeration for the different cxWidgets items (title,
 * \brief message, status, border, menu selection, label, read-only data item,
 * \brief editable data item, hotkey).<br>
 * \brief <br>
 * \brief Note to programmer: If you add to this enumeration, you may also want to add
 * \brief an appropriate collection to cxBase (and to the appropriate window
 * \brief classes) for that type of attribute and add a case to these functions in
 * \brief cxBase (and the window classes):
 * \brief <ul>
 * \brief <li> addAttr()
 * \brief <li> setAttr()
 * \brief <li> removeAttr()
 * \brief <li> removeAttrs()
 * \brief <li> getAttrs()
 * \brief </ul>
 * \brief Also, if you add to this enumeration, you will also have to update the
 * \brief following:
 * \brief <ul>
 * \brief <li>cxForm's applyWinAttributes() method, which applies the form's
              attributes to new inputs as they are appended to the form.
 * \brief <li>cxBase::getWidgetItemStr(), which returns a string version of a
              value from this enumeration.
 * \brief </ul>
*/

/**
 * \enum Specifies a cxWidgets item (title, message, status, input label,
 * input data, etc.).
 */
enum e_WidgetItems
{
   eMESSAGE = 0,    // Message
   eTITLE,          // Title
   eSTATUS,         // Status
   eBORDER,         // Borders
   eMENU_SELECTION, // Selected menu items
   eLABEL,          // Input labels
   eDATA_READONLY,  // Read-only data items
   eDATA_EDITABLE,  // Editable data items
   eDATA,           // Data items (general)
   eHOTKEY          // Hotkey
};

#endif
