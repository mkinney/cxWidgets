/* cxStringUtils.cpp - String utility functions
 *
 * Copyright (c) 2004-2007 Michael H. Kinney
 *
 */

#include "cxStringUtils.h"
#include "cxMiscDefines.h"
#include <assert.h>
#include <ncurses.h> // For ncurses key codes
#include <typeinfo>
#include <algorithm>
using std::string;
using std::vector;
using std::list;
using std::ostringstream;
using std::transform;

#ifndef NDEBUG
#define NDEBUG
#endif

// Splits a string based on a regular expression, similar to
//  Perl's split function.
void cxStringUtils::SplitStringRegex(const string& input, const string& regex,
       vector<string>& resultContainer)
       {
   regex_t pattern;          // Pattern to match
   int flags = REG_EXTENDED; // Allow extened regular expressions

   resultContainer.clear();

   if( regcomp(&pattern, regex.c_str(), flags) == 0 )
   {
      regmatch_t match;  // Start/end info for regex matches

      int wordStart = 0;  // Start index of words to extract
      const char* inputCStr = input.c_str();

      // Execute the regular expression and fill resultContainer
      while( (regexec(&pattern, inputCStr+(sizeof(char)*wordStart),
                     1, &match, 0) == 0) &&
             (wordStart < (int)input.size()) )
             {
         // Insert the substring into resultContainer, if a
         //  match is found.
         if( match.rm_so != -1 )
         {
            regoff_t wordLength; // Length of the next word to get
            // If rm_eo is 0, then we'll want to get
            //  only the next character.  Otherwise,
            //  we'll want to get the next whole word.
            if( match.rm_eo == 0 )
               wordLength = 1;
            else
               wordLength = match.rm_eo - 1;

            resultContainer.push_back(input.substr(wordStart, wordLength));

            // The end offset of the current match should
            //  be added to wordStart so that the next
            //  word in the string is extracted.  However,
            //  if the end offset is 0, the best we can do
            //  is to just increment wordStart by 1 (if the
            //  regex contains something like a 0-or-more
            //  match, it could match every character..which
            //  is true, since each character _is_ separated
            //  by 0 or more matches..)
            if( match.rm_eo != 0 )
               wordStart += match.rm_eo;
            else
               ++wordStart;
         }
         else // No more matches
            break;
      }

      // If wordStart is less than the last index
      //  of the input string, then there is one more
      //  word that needs to be inserted into resultContainer.
      //  However, if resultContainer is empty, that means no
      //  matches were found.  If that's true, then
      //  we wouldn't want to add a word here.
      if( resultContainer.size() > 0 && wordStart <= (int)input.size()-1 )
      {
         resultContainer.push_back(input.substr(wordStart));
      }
   }

   regfree(&pattern);

   // If resultContainer has no elements and input is non-blank,
   //  that means the regex wasn't found..  so just insert the
   //  whole input string into resultContainer.
   if (resultContainer.size() == 0 && input != "")
   {
      resultContainer.push_back(input);
   }
}

// Performs a regular expression search on a string.
//  Returns true or false, depending on whether or not
//  the regex was found.
// useBasicRegex specifies whether to use basic regular
//  expressions (when false, extended regular exprssions
//  are used).
bool cxStringUtils::Find(const string& input, const string& regex, bool useBasicRegex)
{
   bool found(false);
   regex_t regEx;
   int cflags;

   // Set the flags for the regular expression, depending on
   //  the value of useBasicRegex.
   if( useBasicRegex )
   {
      cflags = REG_NOSUB;
   }
   else
   {
      cflags = REG_EXTENDED|REG_NOSUB;
   }

   // Compile the regular expression and execute it..  If regexec
   //  returns 0, that means it was found.
   if( regcomp(&regEx, regex.c_str(), cflags) == 0 )
   {
      if( regexec(&regEx, input.c_str(), 0, nullptr, 0) == 0 )
         found = true;
      regfree(&regEx);
   }

   return found;
}

// Performs a regular expression search on a string and
//  copies the resulting string into output.
void cxStringUtils::Find(const string& input, const string& regex, string& output)
{
   regex_t pattern;  // Regex pattern to be matched

   if( regcomp(&pattern, regex.c_str(), REG_EXTENDED) == 0 )
   {
      regmatch_t match;  // Start/end info for regex matches
      if( regexec(&pattern, input.c_str(), 1, &match, 0) == 0 )
         output = input.substr(match.rm_so, match.rm_eo - match.rm_so);
      else
         output.erase();
      regfree(&pattern);
   }
}

// Performs a search & replace basd on a regular expression
//  search pattern & a replacement string (the replacemnt
//  string supports backquoting of named paranthetical
//  buffers, i.e. \\1, \\2, etc.).  Returns whether or not
//  the regular expression string was found.
bool cxStringUtils::Replace(string& srcStr, const string& regexStr,
        const string& replacementStr)
        {
   bool regexFound(false);
   regex_t regex;  // Regular expression to match

   // Compile the regex
   if( regcomp(&regex, regexStr.c_str(), REG_EXTENDED) == 0 )
   {
      regmatch_t* matches = new regmatch_t[regex.re_nsub+1];

      // Execute the regex and generate the resulting string.
      if( regexec(&regex, srcStr.c_str(), regex.re_nsub+1, matches, 0) == 0 )
      {
         regexFound = true; // The regex was found

         int i; // Loop counter
         unsigned replacementStrLen = replacementStr.size();

         // Start off with the front part of the source string
         //  not replaced by the regular expression
         string result = srcStr.substr(0, matches[0].rm_so);
         for( i = 0; i < (int)replacementStrLen; ++i )
         {
            if( replacementStr[i] != '\\' )
               result += replacementStr[i];
            else
            {
               if( i < (int)replacementStrLen - 1 )
               {
                  if( isdigit(replacementStr[i+1]) )
                  {
                     // We've found a reference to a parenthecised
                     //  subexpression..  Copy the characters from
                     //  srcStr to result.
                     //int referenceNum = atoi(&(replacementStr[i+1]));
                     int referenceNum = replacementStr[i+1]-48;

                     if( referenceNum > 0 && referenceNum < (int)regex.re_nsub+1 )
                     {
                        result += srcStr.substr(matches[referenceNum].rm_so,
                                    matches[referenceNum].rm_eo - matches[referenceNum].rm_so);
                     }

                     // Skip the next character, since we've already
                     //  seen it
                     ++i;
                  }
                  else  // The next character isn't a digit
                     result += replacementStr[i];
               }
               else  // This is just a backslash
                  result += replacementStr[i];
            }
         }

         // Now set the source string equal to result +
         //  the end part of the source string not
         //  replaced by the regular expression
         srcStr = result + srcStr.substr(matches[0].rm_eo,
                               srcStr.size()-matches[0].rm_eo);
      }

      delete [] matches;
   }

   return regexFound;
}

string cxStringUtils::replace(const string& in, const string& search, const string& replace)
{
   string out;
   out=in;
   if (search.length() > 0)
   {
      size_t start=out.find(search);
      while ((start != string::npos) && (out.length() > 0))
      {
         if (replace.length() == 0)
         {
            out.erase(start, search.length());
         }
         else
         {
            out.replace(start, search.length(), replace);
         }
         start=out.find(search, start+ replace.length());
      }
   }

   return(out);
} // replace

int cxStringUtils::stringTo(const std::string& s)
{
   std::istringstream iss(s);
   int x = 0;
   iss >> x;
   return(x);
} // stringTo

long cxStringUtils::stringToLong(const std::string& s)
{
   std::istringstream iss(s);
   long x = 0;
   iss >> x;
   return(x);
} // stringToLong

unsigned cxStringUtils::stringToUnsigned(const std::string& s)
{
   std::istringstream iss(s);
   unsigned x = 0;
   iss >> x;
   return(x);
} // stringToUnsigned

double cxStringUtils::stringToDouble(const std::string& s)
{
   std::istringstream iss(s);
   double x = 0.0;
   iss >> x;
   return(x);
} // stringToDouble;

long double cxStringUtils::stringToLongDouble(const std::string& s)
{
   std::istringstream iss(s);
   long double x = 0.0;
   iss >> x;
   return(x);
} // stringToLongDouble

string cxStringUtils::stringToString(const std::string& s)
{
   return(s);
} // stringToString

void cxStringUtils::test_stringTo()
{
   assert(stringTo("0")==0);
   assert(stringTo("")==0);
   assert(stringTo("123")==123);
   assert(stringTo("-123")== -123);

   assert(stringToUnsigned("123")== 123);

   string s = "999999999";
   assert(stringToLong(s) == 999999999);
   assert(stringToLong("123456789")== 123456789);

   assert(stringToDouble("0.00")== 0.00);
   assert(stringToDouble("123.12")== 123.12);
   assert(stringToDouble("-123.12")== -123.12);

   assert(stringToString("test") == string("test"));

   // throw some garbage to it...
   assert(stringTo("25.5") == 25);
   assert(stringToDouble("11A.34") == 11.0);
   assert(stringToDouble("") == 0.0);
   assert(stringTo("") == 0);
   assert(stringTo("A") == 0);
}

std::string cxStringUtils::toString(const int& x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

std::string cxStringUtils::toString(const long& x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

std::string cxStringUtils::toString(const double& x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

std::string cxStringUtils::toString(const long double& x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

std::string cxStringUtils::toString(const unsigned& x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

std::string cxStringUtils::toString(const unsigned long& x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

std::string cxStringUtils::toString(const eReturnCode& x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

std::string cxStringUtils::toString(const eInputOptions& x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

std::string cxStringUtils::toString(const eBorderStyle& x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

std::string cxStringUtils::toString(const void* x)
{
   ostringstream oss;
   oss << x;
   return oss.str();
} // toString

void cxStringUtils::test_toString()
{
   int i=1;
   assert(toString(i)=="1");
   i=0;
   assert(toString(i)=="0");

   i=-1;
   assert(toString(i)=="-1");

   double d=9.25;
   assert(toString(d)=="9.25");

   d=0.01;
   assert(toString(d)=="0.01");

   long l=1234567890;
   assert(toString(l)=="1234567890");

   i=1234;
   assert(toString(i)=="1234");

   unsigned u=1234;
   assert(toString(u) == "1234");

   unsigned long ul = 1234;
   assert(toString(ul) == "1234");

   eReturnCode erc=cxID_QUIT;
   assert(toString(erc) == "-397");

   eInputOptions eik = eINPUT_EDITABLE;
   assert(toString(eik) == "0");

   eBorderStyle ebs = eBS_NOBORDER;
   assert(toString(ebs) == "0");

   unsigned int x=1;
   assert(toString(x)=="1");
   assert(toString((void*)(&x)) != "");
} // test_toString

bool cxStringUtils::startsWithNumber(const string& pStr)
{
   bool startsWithNum = false;

   // True if the first character is 0-9
   if ((pStr[0] >= '0') && (pStr[0] <= '9'))
   {
      startsWithNum = true;
   }
   else
   {
      // True if the first character is - or + and the next
      //  character is 0-9
      if (pStr.length() > 1)
      {
         if (((pStr[0] == '-') || (pStr[0] == '+')) &&
              (pStr[1] >= '0') && (pStr[1] <= '9'))
              {
            startsWithNum = true;;
         }
      }
   }

   return(startsWithNum);
}

bool cxStringUtils::isPrintable(const int& pKey)
{
   // We want to use isprint(), but we want to return false for some keys that
   //  isprint() returns true for.
   return(isprint(pKey) && (pKey != PAGE_UP) && (pKey != PAGE_DOWN) &&
          (pKey != KEY_IC) && (pKey != KEY_MOUSE));
}

int cxStringUtils::indexOfLastCap(const string& pStr)
{
   int lastCapIndex = -1;

   for (int i = (int)pStr.size()-1; i >= 0; --i)
   {
      if (isupper(pStr[i]))
      {
         lastCapIndex = i;
         break;
      }
   }

   return(lastCapIndex);
}

void cxStringUtils::toUpper(string& theString)
{
   transform(theString.begin(), theString.end(), theString.begin(), toupper);
} // toUpper

string cxStringUtils::stripNewlines(const string& pStr)
{
   string newStr(pStr);
   unsigned strLen = newStr.length();
   for (unsigned i = 0; i < strLen; ++i)
   {
      while (newStr[i] == '\n')
      {
         newStr.erase(i, 1);
         strLen = newStr.length();
         if (i >= strLen)
         {
            break;
         }
      }
   }

   return(newStr);
} // stripNewlines

string cxStringUtils::replaceNewlines(const string& pStr, const char& pChar)
{
   string newStr(pStr);
   unsigned strLen = newStr.length();
   for (unsigned i = 0; i < strLen; ++i)
   {
      while (newStr[i] == '\n')
      {
         newStr[i] = pChar;
      }
   }

   return(newStr);
} // replaceNewlines

void cxStringUtils::insertUnderline(string& pStr, char pChar, bool pAll)
{
   unsigned strLen = pStr.length();
   for (unsigned i = 0; i < strLen; ++i)
   {
      // If the current character in the string matches
      //  pChar, then add an underline character.
      if (toupper(pStr[i]) == toupper(pChar))
      {
         // Only do it if the previous character in
         //  the string isn't already a hotkey character.
         if (i > 0)
         {
            if (pStr[i-1] != HOTKEY_CHAR)
            {
               pStr.insert(i, HOTKEY_CHAR_STR);
            }
         }
         else
         {
            pStr.insert(i, HOTKEY_CHAR_STR);
         }

         // If they don't want to underline all occurances of
         //  the character, then stop going through the string.
         if (!pAll)
         {
            break;
         }
      }
   }
} // insertUnderline

string cxStringUtils::removeChar(const string& pString, const int pPos)
{
   string retval;

   retval=pString; // just return the original string if they use
                   // invalid values for pPos

   // we have to have something before we can take some away
   if (pString.length() > 0)
   {
      // as long as we have a string longer than the position
      if (pPos < (int)pString.length())
      {
         retval="";
         if (pPos > 0)
         {
            retval=pString.substr(0, pPos); // get up to that pos
         }
         retval+=pString.substr(pPos+1); // rest of string
      }
   }
   return(retval);
} // removeChar

string cxStringUtils::strToUpper(const string& theString)
{
   string upperString = theString;
   transform(upperString.begin(), upperString.end(), upperString.begin(), toupper);
   return(upperString);
} // strToUpper

string cxStringUtils::strToLower(const string& theString)
{
   string lowerString = theString;
   // Help with this call to transform came from
   //  http://lists.debian.org/debian-gcc/2002/04/msg00092.html
   transform(lowerString.begin(), lowerString.end(), lowerString.begin(),
         (int(*)(int))std::tolower);
   return(lowerString);
} // strToLower

void cxStringUtils::TrimSpaces(string& str)
{
   if (str.size() > 0)
   {
      // Find the last index of a space in str, and
      //  if there are any leading spaces, then erase them.
      size_t pos = string::npos;
      unsigned i = 0;
      while (str[i] == ' ')
      {
         pos = i;
         ++i;
      }
      if (pos != string::npos)
      {
         str.erase(0, pos+1);
      }

      // Do the same from the reverse end of str.
      pos = string::npos;
      if (!str.empty())
      {
         i = str.size()-1;
         while (str[i] == ' ')
         {
            pos = i;
            if (i == 0) break;
            --i;
         }
      }
      if (pos != string::npos)
      {
         str.erase(pos);
      }
   }
} // TrimSpaces

void cxStringUtils::splitStringOnNewlines(const string& pStr, list<string>& pStrings)
{
   pStrings.clear();

   if (pStr != "")
   {
      unsigned startPos = 0; // Starting position for substrings
      unsigned len = 0;      // Length of substrings
      size_t pos = 0;        // Index of a newline
      while ((pos = pStr.find('\n', startPos)) != string::npos)
      {
         len = pos - startPos;
         pStrings.push_back(pStr.substr(startPos, len));
         startPos = pos + 1;
      }
      // Edge case: Insert the last string
      if (pStr.substr(startPos) != "")
      {
         pStrings.push_back(pStr.substr(startPos));
      }
   }
} // splitStringOnNewlines
