// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxTextValidator.h"
#include <cstdlib> // For toupper()
using std::string;

// Default constructor - Takes a text string and
//  a validator string.
cxTextValidator::cxTextValidator(const string& pTextStr,
                        const string& pValidatorStr)
   : mTextStr(pTextStr),
     mValidatorStr(pValidatorStr)
{
}

cxTextValidator::~cxTextValidator()
{
}

// Sets the text string to be validated.
void cxTextValidator::setTextStr(const string& pTextStr)
{
   mTextStr = pTextStr;
}

// Returns the text string.
const string& cxTextValidator::getTextStr() const
{
   return(mTextStr);
}

// Sets the validator string.
void cxTextValidator::setValidatorStr(const string& pValidatorStr)
{
   mValidatorStr = pValidatorStr;

   // Go through mValidatorStr from reverse, and if
   //  any of the letters are capitals, then make
   //  all the letters before it capitals (capitals
   //  signify that the characters in their position
   //  are required, and this would simplify the
   //  input validation).
   bool setCapital = false;
   if (mValidatorStr.length() > 0)
   {
      for (int i = (int)mValidatorStr.length()-1; i >= 0; --i)
      {
         if (isAlpha(mValidatorStr[i]))
         {
            if (setCapital)
            {
               // Don't convert 'm' or 'r' to capitals
               //  ('m' is for masking (for passwords, etc.),
               //  and 'r' is for using regular expressions).
               if ((mValidatorStr[i] != 'm') && (mValidatorStr[i] != 'r'))
               {
                  mValidatorStr[i] = toupper(mValidatorStr[i]);
               }
            }
            else
            {
               // If this alpha is uppercase, then
               //  we need to make sure the other letters
               //  are capitals.
               if ((mValidatorStr[i] >= 'A') && (mValidatorStr[i] <= 'Z'))
               {
                  setCapital = true;
               }
            }
         }
      }
   }
}

// Accessor for the validator string.
const string& cxTextValidator::getValidatorStr() const
{
   return(mValidatorStr);
}

// Returns whether the text string passes validation.
bool cxTextValidator::textIsValid() const
{
   return(textIsValid(mTextStr));
}

// Returns whether a text string passes validation.
bool cxTextValidator::textIsValid(const string& pTextStr) const
{
   bool isValid(true);

   if (mValidatorStr != "")
   {
      // Add any implied characters from mValidatorStr to
      //  pTextStr.
      string textStr = pTextStr;
      addImpliedChars(textStr);

      // If the last character in mValidatorStr is a capital
      //  letter and pTextStr is shorter than mValidatorStr,
      //  then we know right away that textStr is not
      //  valid (because a capital says we need that many
      //  characters).
      if (isUpper(mValidatorStr[mValidatorStr.length()-1]) &&
          textStr.length() < mValidatorStr.length())
          {
         return(false);
      }

      // If textStr is shorter or equal in length to
      //  mValidatorStr, go through textStr and check each
      //  character in textStr based on the type of
      //  validator character in mValidatorStr.
      if (textStr.length() <= mValidatorStr.length())
      {
         unsigned valueLen = textStr.length();
         // The variable i in this loop is used as an index
         //  into mValidatorStr, but we're only going the length
         //  of textStr (mValidatorStr could still be longer).
         for (unsigned i = 0; i < valueLen; ++i)
         {
            switch(mValidatorStr[i])
            {
               case 'd': // Any digit ([0-9])
                  if (!isDigit(textStr[i]))
                  {
                     isValid = false;
                     break;  // Break from the for loop
                  }
                  break;
               case 'D': // Like above, but all are required
                  if (!isDigit(textStr[i]))
                  {
                     isValid = false;
                     break;
                  }
                  else
                  {
                     // Since this is a 'required' character,
                     //  that means textStr needs to have the
                     //  same number of characters as
                     //  mValidatorStr at this index.
                     if (valueLen < i+1)
                     {
                        isValid = false;
                        break;
                     }
                  }
                  break;
               case 'n': // Any number ([0-9.])
                  if (!isDigitOrDecimal(textStr[i]))
                  {
                     isValid = false;
                     break;
                  }
                  break;
               case 'N': // Like above, but all are required
                  if (!isDigitOrDecimal(textStr[i]))
                  {
                     isValid = false;
                     break;
                  }
                  else
                  {
                     // Since this is a 'required' character,
                     //  that means textStr needs to have the
                     //  same number of characters as
                     //  mValidatorStr at this index.
                     if (valueLen < i+1)
                     {
                        isValid = false;
                        break;
                     }
                  }
                  break;
               case '@': // Fixed decimal
                  {
                     // Find a decimal point in textStr, and if
                     //  there is one, then count how many
                     //  characters there are in textStr after the
                     //  decimal point.  If this matches the number
                     //  of characters in the validator string after
                     //  the '@', then the string is OK.
                     size_t dotPos = textStr.find('.');
                     if (dotPos != string::npos)
                     {
                        //unsigned textStrLen = textStr.substr(dotPos+1).length();
                        //unsigned validatorStrLen = mValidatorStr.substr(i+1).length();
                        if (textStr.substr(dotPos+1).length() !=
                            mValidatorStr.substr(i+1).length())
                            {
                           // The length after the '.' does not
                           //  equal the length in the validator
                           //  string after the '@'.
                           isValid = false;
                           break;
                        }
                     }
                     else
                     {
                        // Since the string doesn't have a decimal
                        //  point, it's not valid.
                        isValid = false;
                        break;
                     }
                  }
                  break;
               case 'a': // Alphanumeric ([0-9a-zA-Z])
                  if (!isAlphaNum(textStr[i]))
                  {
                     isValid = false;
                     break;
                  }
                  break;
               case 'A': // Like above, but all are required
                  if (!isAlphaNum(textStr[i]))
                  {
                     isValid = false;
                     break;
                  }
                  else
                  {
                     // Since this is a 'required' character,
                     //  that means textStr needs to have the
                     //  same number of characters as
                     //  mValidatorStr at this index.
                     if (valueLen < i+1)
                     {
                        isValid = false;
                        break;
                     }
                  }
                  break;
               case 'p': // Alphanumeric w/ punctuation ([0-9a-zA-Z ,-.])
                  if (!isAlphaNumOrPunc(textStr[i]))
                  {
                     isValid = false;
                     break;
                  }
                  break;
               case 'P': // Like above, but all are required
                  if (!isAlphaNumOrPunc(textStr[i]))
                  {
                     isValid = false;
                     break;
                  }
                  else
                  {
                     // Since this is a 'required' character,
                     //  that means textStr needs to have the
                     //  same number of characters as
                     //  mValidatorStr at this index.
                     if (valueLen < i+1)
                     {
                        isValid = false;
                        break;
                     }
                  }
                  break;
               case 'x': // Any character
                  break;
               case 'X': // Like above, but all are required
                  // Since this is a 'required' character,
                  //  that means textStr needs to have the
                  //  same number of characters as
                  //  mValidatorStr at this index.
                  if (valueLen < i+1)
                  {
                     isValid = false;
                     break;
                  }
                  break;
               case 'r': // Compares with a regular expression
                  break;
               default:
                  // If the character in textStr at this position
                  //  doesn't match the one in mValidatorStr, then
                  //  textStr isn't valid.
                  if (textStr[i] != mValidatorStr[i])
                  {
                     isValid = false;
                     break;
                  }
                  break;
            }
         }

         // Check the next character in the validator string - if it's
         //  a capital letter, then we know the string is not valid
         //  (since the string is not long enough).
         // First see if mValidatorStr has any more characters..
         if (mValidatorStr.length() > valueLen)
         {
            if (isUpper(mValidatorStr[valueLen]))
            {
               isValid = false;
            }
         }
      }
      else
      {
         // textStr is longer than mValidatorStr..  so
         //  the input is invalid.
         isValid = false;
      }
   }

   return(isValid);
} // textIsValid

string cxTextValidator::addImpliedChars() const
{
   string newString = mTextStr;
   addImpliedChars(newString);

   return(newString);
}

void cxTextValidator::addImpliedChars(string& pTextStr) const
{
   if (mValidatorStr != "")
   {
      // If pTextStr is shorter or equal in length to
      //  mValidatorStr, go through mValidatorStr, and insert
      //  each non-validator character into pTextStr.
      unsigned validatorStrLen = mValidatorStr.length();
      if (pTextStr.length() <= validatorStrLen)
      {
         for (unsigned i = 0; i < validatorStrLen; ++i)
         {
            // If the current character in mValidatorStr is not a validator,
            //  then insert the character into pTextStr.
            if (!isValidatorChar(mValidatorStr[i]))
            {
               // Only insert the character if the index (i) isn't too large..
               //  If i is greater than the length of pTextStr, the insert
               //  will fail.
               if (i <= pTextStr.length())
               {
                  // '@' means a fixed decimal point..  If the current character
                  //  in mValidatorStr is an '@', insert a '.'.  Otherwise, just
                  //  insert whatever the character is.
                  if (mValidatorStr[i] == '@')
                  {
                     // Only insert a decimal point if there isn't one already
                     //  in pTextStr.
                     auto dotPos = pTextStr.find('.');
                     if (dotPos == string::npos)
                     {
                        pTextStr.insert(pTextStr.begin()+i, 1, '.');
                     }
                  }
                  else
                  {
                     // Only add the character if the current character in pTextStr
                     //  is not already this character.
                     if (pTextStr[i] != mValidatorStr[i])
                     {
                        pTextStr.insert(pTextStr.begin()+i, 1, mValidatorStr[i]);
                     }
                  }
               }
               else
               {
                  break;
               }
            }
         }
      }
   } // if (mValidatorStr != "")
}

string cxTextValidator::cxTypeStr() const
{
   return("cxValidator");
} // cxTypeStr


/// Private facilitator functions

// Returns whether a character is a digit.
inline bool cxTextValidator::isDigit(char pChar)
{
   //return(Find(&pChar, "[0-9]"));

   if ((pChar >= '0') && (pChar <= '9'))
   {
      return(true);
   }
   else
   {
      return(false);
   }
}

// Returns whether a character is a letter.
inline bool cxTextValidator::isAlpha(char pChar)
{
   if (((pChar >= 'a') && (pChar <= 'z')) ||
       ((pChar >= 'A') && (pChar <= 'Z')))
       {
      return(true);
   }
   else
   {
      return(false);
   }
}

// Returns whether a character is a digit or decimal point.
inline bool cxTextValidator::isDigitOrDecimal(char pChar)
{
   //return(Find(&pChar, "[0-9\\.]"));

   if (((pChar >= '0') && (pChar <= '9')) || (pChar == '.'))
   {
      return(true);
   }
   else
   {
      return(false);
   }
}

// Returns whether a character is alphanumeric.
inline bool cxTextValidator::isAlphaNum(char pChar)
{
   //return(Find(&pChar, "[0-9a-zA-Z]"));

   if (((pChar >= 'a') && (pChar <= 'z')) ||
       ((pChar >= 'A') && (pChar <= 'Z')) ||
       ((pChar >= '0') && (pChar <= '9')))
       {
      return(true);
   }
   else
   {
      return(false);
   }
}

// Returns whether a character is alphanumeric or punctuation.
inline bool cxTextValidator::isAlphaNumOrPunc(char pChar)
{
   //return(Find(&pChar, "[0-9a-zA-Z ,-\\.]"));

   if (((pChar >= 'a') && (pChar <= 'z')) ||
       ((pChar >= 'A') && (pChar <= 'Z')) ||
       ((pChar >= '0') && (pChar <= '9')) ||
       ((pChar >= 32) && (pChar <= 46)) ||
       ((pChar >= 58) && (pChar <= 64)))
       {
      return(true);
   }
   else
   {
      return(false);
   }
}

// Returns whether a character is an uppercase letter.
inline bool cxTextValidator::isUpper(char pChar)
{
   return((pChar >= 'A') && (pChar <= 'Z'));
}

// Returns whether a character is a validator character.
inline bool cxTextValidator::isValidatorChar(char pChar)
{
   return((pChar == 'd') || (pChar == 'D') || (pChar == 'n') ||
          (pChar == 'N') || (pChar == 'a') || (pChar == 'A') ||
          (pChar == 'p') || (pChar == 'P') || (pChar == 'x') ||
          (pChar == 'X') || (pChar == 'r'));
}
