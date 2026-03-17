#ifndef __CXTEXTVALIDATOR_H__
#define __CXTEXTVALIDATOR_H__

// Copyright (c) 2005-2007 Michael H. Kinney
//
// This class provides some text validation
//  functionality.
// See README.TXT for cxWidgets validation stuff.

#include "cxObject.h"

#include <string>

/** \class cxTextValidator
 * \brief Provides some text validation functionality.  See README.txt for
 * \brief  details of validation strings.
 *
 * \author $Author: erico $
 * \version $Revision: 1.10 $
 * \date $Date: 2007/12/04 00:23:46 $
*/
class cxTextValidator
{
   public:
      /**
       * Default constructor; takes a text string and a validator string
       * @param pTextStr String to validate; blank by default
       * @param pValidatorStr String used to validate text; blank by default
       */
      explicit cxTextValidator(const std::string& pTextStr = "",
                               const std::string& pValidatorStr = "");

      /**
       * Destructor
       */
      virtual ~cxTextValidator();

      /**
       * Sets the text string to be validated
       * @param pTextStr Text string to be validated
       */
      virtual void setTextStr(const std::string& pTextStr);

      /**
       * Returns the text string
       * @return The text string
       */
      virtual const std::string& getTextStr() const;

      /**
       * Sets the validator string
       * @param pValidatorStr Validator string--see README.txt for details of validator strings
       */
      virtual void setValidatorStr(const std::string& pValidatorStr);

      /**
       * Returns the validator string
       * @return Validator string
       */
      virtual const std::string& getValidatorStr() const;

      /**
       * Returns whether the text string passes validation
       * @return True or false, depending on whether string passes validation
       */
      virtual bool textIsValid() const;

      /**
       * Returns whether parameter string passes validation
       * @param pTextStr String to validate
       * @return True or false, depending on whether string passes validation
       */
      virtual bool textIsValid(const std::string& pTextStr) const;

      /**
       * Adds any implied characters from the validator string to the text string,
       *  if it is valid.  For example, with a string of "5031234567" and a
       *  validator string of "(DDD) DDD-DDDD", the string would become
       *  "(503) 123-4567".
       * @return The text string with any implied characters added.
       */
      virtual std::string addImpliedChars() const;

      /**
       * Adds any implied characters from the validator string to a text string,
       *  if it is valid.  For example, with a string of "5031234567" and a
       *  validator string of "(DDD) DDD-DDDD", the string would become
       *  "(503) 123-4567".
       * @param pTextStr A string to add implied characters to (it will get the implied characters added to it)
       */
      virtual void addImpliedChars(std::string& pTextStr) const;

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const;

   private:
      std::string mTextStr;      // The string to be validated
      std::string mValidatorStr; // String to use for input validation

      // Returns whether a character is a digit.
      static inline bool isDigit(char pChar);

      // Returns whether a character is a letter.
      static inline bool isAlpha(char pChar);

      // Returns whether a character is a digit or decimal point.
      static inline bool isDigitOrDecimal(char pChar);

      // Returns whether a character is alphanumeric.
      static inline bool isAlphaNum(char pChar);

      // Returns whether a character is alphanumeric or punctuation.
      static inline bool isAlphaNumOrPunc(char pChar);

      // Returns whether a character is an uppercase letter.
      static inline bool isUpper(char pChar);

      // Returns whether a character is a validator character.
      static inline bool isValidatorChar(char pChar);
};

#endif
