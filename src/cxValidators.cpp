// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxValidators.h"
#include "cxMultiLineInput.h"
#include <cctype> // For isprint()
using std::string;

string cxValidators::intOnKeyValidator(void *theInput, void *clearOnSpace)
{
   if (theInput == nullptr)
   {
      return("");
   }

   string retval;

   cxMultiLineInput *pInput = static_cast<cxMultiLineInput*>(theInput);
   // Only do this stuff if the user pressed a printable key.
   if (isprint(pInput->getLastKey()))
   {
      bool updateIt = false;

      string value = pInput->getValue();
      size_t valueLen = value.length();
      // Remove leading zeros
      if (valueLen > 0)
      {
         bool allZeros = true;
         string::iterator iter = value.begin();
         for (; iter != value.end(); ++iter)
         {
            if (*iter != '0')
            {
               allZeros = false;
               break;
            }
         }
         if (allZeros)
         {
            value = "0";
            updateIt = true;
         }
         else
         {
            if (value[0] == '0')
            {
               updateIt = true;
               size_t leadingZerosBegin = 0;
               size_t leadingZerosEnd = value.find_first_not_of('0', 0);
               value.erase(leadingZerosBegin, leadingZerosEnd);
            }
         }

         valueLen = value.length();
      }
      if (valueLen > 0)
      {
         switch(value[valueLen-1])
         {
            // Digits are OK.
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               break;
            case '-':
               {
                  // If the value already has a negative sign
                  //  and it is not the last character, remove
                  //  the last character.
                  // Does it have a negative sign?
                  size_t negLocation = value.find_last_of('-');
                  bool hasNeg = (negLocation != string::npos);
                  if (hasNeg && (negLocation > 0))
                  {
                     value.erase(valueLen-1);
                     updateIt = true;
                  }
               }
               break;
            case ' ':
               if (clearOnSpace != nullptr)
               {
                  bool *pClearOnSpace = (bool*)clearOnSpace;
                  if (*pClearOnSpace)
                  {
                     value = "";
                     updateIt = true;
                  }
                  else
                  {
                     // Remove the space from the value
                     value.erase(valueLen-1);
                     updateIt = true;
                  }
               }
               else
               {
                  // Spaces are invalid
                  value.erase(valueLen-1);
                  updateIt = true;
               }
               break;
            default:
               // Any other character is invalid.
               value.erase(valueLen-1);
               updateIt = true;
               retval = "Invalid key pressed.";
               break;
         }
      }

      if (updateIt)
      {
         // Refresh just the value portion of the input (no need to
         //  redraw the label)
         pInput->setValue(value, false);
         pInput->refreshValue(true);
      }
   }

   // Make sure the cursor is still enabled (for some reason,
   //  the cursor would get disabled without this).
   curs_set(1);

   return(retval);
} // intOnKeyValidator

string cxValidators::floatingPtOnKeyValidator(void *theInput, void *clearOnSpace)
{
   if (nullptr == theInput)
   {
      return("");
   }

   string retval;

   cxMultiLineInput *pInput = static_cast<cxMultiLineInput*>(theInput);
   // Only do this stuff if the user pressed a printable key.
   int lastKey = pInput->getLastKey();
   if (isprint(lastKey))
   {
      bool updateIt = false;

      string value = pInput->getValue();
      size_t valueLen = value.length();
      // Remove leading zeros.  But if the string is
      //  all zeros, then leave one zero.
      if (valueLen > 0)
      {
         bool allZeros = true;
         string::iterator iter = value.begin();
         for (; iter != value.end(); ++iter)
         {
            if (*iter != '0')
            {
               allZeros = false;
               break;
            }
         }
         if (allZeros)
         {
            value = "0";
            updateIt = true;
         }
         else
         {
            if (value[0] == '0')
            {
               updateIt = true;
               size_t leadingZerosBegin = 0;
               size_t leadingZerosEnd = value.find_first_not_of('0', 0);
               value.erase(leadingZerosBegin, leadingZerosEnd);
            }
         }

         valueLen = value.length();
      }
      if (valueLen > 0)
      {
         switch(value[valueLen-1])
         {
            // Digits are OK.
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               break;
            case '.':
               {
                  // Does it have a decimal point?
                  size_t decPtLocation = value.find('.', 0);
                  bool hasDecPt = (decPtLocation != string::npos);
                  // If the value already has a decimal point and it is not
                  //  the last character, remove the last character.
                  if (hasDecPt)
                  {
                     if (decPtLocation != valueLen-1)
                     {
                        value.erase(valueLen-1);
                        updateIt = true;
                     }
                  }
               }
               break;
            case ' ':
               if (clearOnSpace != nullptr)
               {
                  bool *pClearOnSpace = (bool*)clearOnSpace;
                  if (*pClearOnSpace)
                  {
                     value = "";
                     updateIt = true;
                  }
                  else
                  {
                     // Remove the space from the value
                     value.erase(valueLen-1);
                     updateIt = true;
                  }
               }
               else
               {
                  // Spaces are invalid
                  value.erase(valueLen-1);
                  updateIt = true;
               }
               break;
            case '-':
               {
                  // If the value already has a negative sign
                  //  and it is not the last character, remove
                  //  the last character.
                  // Does it have a negative sign?
                  size_t negLocation = value.find_last_of('-');
                  bool hasNeg = (negLocation != string::npos);
                  if (hasNeg && (negLocation > 0))
                  {
                     value.erase(valueLen-1);
                     updateIt = true;
                  }
               }
               break;
            // Remove anything that isn't a digit or a decimal point.
            default:
               value.erase(valueLen-1);
               updateIt = true;
               retval = "Invalid key pressed.";
               break;
         }
      }

      if (updateIt)
      {
         // Refresh just the value portion of the input (no need to
         //  redraw the label)
         pInput->setValue(value, false);
         pInput->refreshValue(true);
      }
   }

   // Make sure the cursor is still enabled (for some reason,
   //  the cursor would get disabled without this).
   curs_set(1);

   return(retval);
} // floatingPtOnKeyValidator

bool cxValidators::isValidFloatingPtNumStr(const string& pNumStr)
{
   bool valid = true;

   if (pNumStr != "")
   {
      // Make sure the first character is a digit, minus sign, or a decimal
      //  point.
      bool foundDecimalPt = (pNumStr[0] == '.');
      if ((isdigit(pNumStr[0]) != 0) || (pNumStr[0] == '-') || foundDecimalPt)
      {
         // Test to make sure that each character after the first is a digit or
         //  a decimal point (but there should only be one decimal point).
         for (unsigned i = 1; i < pNumStr.length(); ++i)
         {
            // Digits are valid.
            if (isdigit(pNumStr[i]) != 0)
            {
            }
            // If the character is a decimal point and we've already found a
            //  decimal point, then the value is invalid.
            else if (pNumStr[i] == '.')
            {
               if (foundDecimalPt)
               {
                  valid = false;
                  break;
               }
               else
               {
                  foundDecimalPt = true;
               }
            }
            // All other characters are invalid.
            else
            {
               valid = false;
               break;
            }
         }
      }
      else
      {
         valid = false;
      }
   }

   return(valid);
} // isValidFloatingPtNumStr

bool cxValidators::isValidWholeNumStr(const string& pNumStr)
{
   bool valid = true;

   if (pNumStr != "")
   {
      // Make sure the first character is a digit or a minus sign
      if ((isdigit(pNumStr[0]) != 0) || (pNumStr[0] == '-'))
      {
         // Make sure each number after the first is a digit.
         for (unsigned i = 1; i < pNumStr.length(); ++i)
         {
            if (isdigit(pNumStr[i]) == 0)
            {
               valid = false;
               break;
            }
         }
      }
      else
      {
         valid = false;
      }
   }

   return(valid);
} // isValidWholeNumStr
