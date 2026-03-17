#ifndef __CXVALIDATORS_H__
#define __CXVALIDATORS_H__
// Copyright (c) 2005-2007 Michael H. Kinney
//
// This file defines some input validators.

#include <string>

namespace cxValidators
{
   /**
    * \brief Validator for inputs that should only accept integers.  This is
    * \brief meant to be used as an 'onKey' validator (to validate on each
    * \brief keypress).
    *
    * @param theInput A pointer to a cxMultiLineInput whose input to validate
    * @param clearOnSpace A pointer to a boolean (if non-null and true, will clear the input
    *  when the spacebar is pressed).
    *
    * @return Blank string for valid data or a warning for invalid data.
    */
   std::string intOnKeyValidator(void *theInput, void *clearOnSpace);

   /**
   * \brief Validator for inputs that should only accept floating-point
   * \brief numbers.  This is meant to be used as an 'onKey' validator (to
   * \brief validate on each keypress).
   *
   * @param theInput A pointer to the cxMultiLineInput to validate
   * @param clearOnSpace A pointer to a boolean (if non-null and true, will clear the input
   *  when the spacebar is pressed).
   *
   * @return Blank string for valid data or a warning for invalid data.
   */
   std::string floatingPtOnKeyValidator(void *theInput, void *clearOnSpace);

   /**
    * \brief Returns whether or not a string contains a valid floating-point
    * \brief number.  A blank string is considered valid.
    *
    * @param pNumStr A string to test
    *
    * @return true if the string contains a valid floating-point number, or
    *  false if not.
    */
   bool isValidFloatingPtNumStr(const std::string& pNumStr);

   /**
    * \brief Returns whether or not a string contains a valid whole number.  A
    * \brief blank string is considered valid.
    *
    * @param pNumStr A string to test
    *
    * @return true if the string contains a valid whole number, or false if not.
    */
   bool isValidWholeNumStr(const std::string& pNumStr);

} // end of namespace cxValidators

#endif
