/**
 * @file cxDate.cpp
 * \brief C++17-improved implementation of the cxDate class.
 *
 * Modernizations applied vs. the original date.cpp:
 *  - std::chrono used in today() / getToday() for current date (thread-safe)
 *  - std::stoi() replaces atoi() / istringstream-based int parsing
 *  - std::to_string() replaces the custom anythingToString<T>() template
 *  - noexcept and [[nodiscard]] propagated from the header
 *  - std::clamp used in validateDateElements() for range clamping
 *  - Range-based for loops used where applicable
 *  - std::string_view parameters avoid unnecessary string copies
 *  - std::optional<cxDate> returned from tryParse()
 *
 * \author Eric Oulashin (original date class)
 */

#include "cxDate.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cctype>
#include <cmath>
#include <algorithm>   // std::clamp, std::all_of
#include <stdexcept>   // std::invalid_argument from std::stoi

using std::string;
using std::ostringstream;
using std::istringstream;
using std::ostream;
using std::istream;
using std::setw;
using std::endl;

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

cxDate::cxDate(eDateFormats pDateFormat, char pSepChar)
   : mDateFormat(pDateFormat != UNKNOWN ? pDateFormat : YYYY_MM_DD),
     mSepChar(pSepChar)
{
   // Use std::chrono for thread-safe access to the current date
   using namespace std::chrono;
   auto now   = system_clock::now();
   auto now_t = system_clock::to_time_t(now);
   std::tm tm{};
#ifdef _WIN32
   localtime_s(&tm, &now_t);
#else
   localtime_r(&now_t, &tm);
#endif
   mYear  = tm.tm_year + 1900;
   mMonth = tm.tm_mon  + 1;     // tm_mon is 0-based
   mDay   = tm.tm_mday;
}

cxDate::cxDate(int pYear, int pMonth, int pDay,
               eDateFormats pDateFormat, char pSepChar) noexcept
   : mYear(pYear), mMonth(pMonth), mDay(pDay),
     mDateFormat(pDateFormat != UNKNOWN ? pDateFormat : YYYY_MM_DD),
     mSepChar(pSepChar)
{
   validateDateElements(mYear, mMonth, mDay);
}

cxDate::cxDate(std::string_view pDateStr,
               eDateFormats pDateFormat, char pSepChar)
   : mDateFormat(pDateFormat != UNKNOWN ? pDateFormat : YYYY_MM_DD),
     mSepChar(pSepChar)
{
   fromString(pDateStr);
}

cxDate::cxDate(std::string_view pDateStr, char pSepChar)
   : mSepChar(pSepChar)
{
   eDateFormats fmt = getDateFormat(pDateStr);
   if (fmt != UNKNOWN)
   {
      mDateFormat = fmt;
      fromString(pDateStr);
   }
}

cxDate::cxDate(long pJulianDate,
               eDateFormats pDateFormat, char pSepChar) noexcept
   : mDateFormat(pDateFormat != UNKNOWN ? pDateFormat : YYYY_MM_DD),
     mSepChar(pSepChar)
{
   fromJulian(pJulianDate, mYear, mMonth, mDay);
}

// ---------------------------------------------------------------------------
// Mutators
// ---------------------------------------------------------------------------

void cxDate::setYear(int pYear) noexcept
{
   mYear = pYear;
   validateDateElements(mYear, mMonth, mDay);
}

void cxDate::setMonth(int pMonth) noexcept
{
   mMonth = pMonth;
   validateDateElements(mYear, mMonth, mDay);
}

void cxDate::setDay(int pDay) noexcept
{
   mDay = pDay;
   validateDateElements(mYear, mMonth, mDay);
}

void cxDate::setDateFormat(eDateFormats pDateFormat) noexcept
{
   if (pDateFormat != UNKNOWN)
   {
      mDateFormat = pDateFormat;
   }
}

bool cxDate::setDate(int pYear, int pMonth, int pDay) noexcept
{
   bool valid = validateDateElements(pYear, pMonth, pDay);
   if (valid)
   {
      mYear  = pYear;
      mMonth = pMonth;
      mDay   = pDay;
   }
   return valid;
}

// ---------------------------------------------------------------------------
// Date arithmetic
// ---------------------------------------------------------------------------

void cxDate::addYears(int pNumYears) noexcept
{
   mYear += pNumYears;
   validateDateElements(mYear, mMonth, mDay);
}

void cxDate::subtractYears(int pNumYears) noexcept
{
   addYears(-pNumYears);
}

void cxDate::addMonths(int pNumMonths) noexcept
{
   mMonth += pNumMonths;
   while (mMonth > CXDATE_END_MONTH)
   {
      mMonth -= CXDATE_MONTHS_IN_YEAR;
      ++mYear;
   }
   while (mMonth < CXDATE_START_MONTH)
   {
      mMonth += CXDATE_MONTHS_IN_YEAR;
      --mYear;
   }
   validateDateElements(mYear, mMonth, mDay);
}

void cxDate::subtractMonths(int pNumMonths) noexcept
{
   addMonths(-pNumMonths);
}

void cxDate::addDays(int pNumDays) noexcept
{
   mDay += pNumDays;
   if (pNumDays > 0)
   {
      while (mDay > numMonthDays(mMonth, mYear))
      {
         mDay -= numMonthDays(mMonth, mYear);
         ++mMonth;
         if (mMonth > CXDATE_END_MONTH)
         {
            mMonth = CXDATE_START_MONTH;
            ++mYear;
         }
      }
   }
   else if (pNumDays < 0)
   {
      while (mDay < CXDATE_START_DAY)
      {
         --mMonth;
         if (mMonth < CXDATE_START_MONTH)
         {
            mMonth = CXDATE_END_MONTH;
            --mYear;
         }
         mDay += numMonthDays(mMonth, mYear);
      }
   }
}

void cxDate::subtractDays(int pNumDays) noexcept
{
   addDays(-pNumDays);
}

// ---------------------------------------------------------------------------
// Operators
// ---------------------------------------------------------------------------

void cxDate::operator+=(int pNumDays) noexcept
{
   addDays(pNumDays);
}
void cxDate::operator-=(int pNumDays) noexcept
{
   subtractDays(pNumDays);
}

bool cxDate::operator==(const cxDate& pThat) const noexcept
{
   return mYear == pThat.mYear && mMonth == pThat.mMonth && mDay == pThat.mDay;
}
bool cxDate::operator!=(const cxDate& pThat) const noexcept
{
   return !(*this == pThat);
}
bool cxDate::operator>(const cxDate& pThat) const noexcept
{
   if (mYear  != pThat.mYear)  return mYear  > pThat.mYear;
   if (mMonth != pThat.mMonth) return mMonth > pThat.mMonth;
   return mDay > pThat.mDay;
}
bool cxDate::operator>=(const cxDate& pThat) const noexcept
{
   return (*this > pThat) || (*this == pThat);
}
bool cxDate::operator<(const cxDate& pThat) const noexcept
{
   return !(*this >= pThat);
}
bool cxDate::operator<=(const cxDate& pThat) const noexcept
{
   return !(*this > pThat);
}

cxDate cxDate::operator++(int) noexcept
{
   cxDate old = *this;
   addDays(1);
   return old;
}
cxDate& cxDate::operator++() noexcept
{
   addDays(1);
   return *this;
}
cxDate cxDate::operator--(int) noexcept
{
   cxDate old = *this;
   subtractDays(1);
   return old;
}
cxDate& cxDate::operator--() noexcept
{
   subtractDays(1);
   return *this;
}

cxDate cxDate::operator+(int pNumDays) const noexcept
{
   cxDate result = *this;
   result.addDays(pNumDays);
   return result;
}
cxDate cxDate::operator-(int pNumDays) const noexcept
{
   cxDate result = *this;
   result.subtractDays(pNumDays);
   return result;
}
int cxDate::operator-(const cxDate& pThat) const noexcept
{
   return static_cast<int>(toJulian() - pThat.toJulian());
}

// ---------------------------------------------------------------------------
// Stream operators
// ---------------------------------------------------------------------------

ostream& operator<<(ostream& pOS, const cxDate& pDate)
{
   // C++17: if-with-initializer in nested scope
   auto pad2 = [&](int v){ pOS << std::setfill('0') << setw(2) << v; };

   if (pDate.mDisplayLong)
   {
      // Long format: e.g., "Monday, January 1 2026"
      pOS << pDate.dayOfWeekStr() << ", ";
      switch (pDate.mDateFormat)
      {
         case YYYY_MM_DD: case YY_MM_DD: case YYYYMMDD: case YYMMDD:
            pOS << pDate.mYear << ' '
                << cxDate::getMonthName(pDate.mMonth) << ' ' << pDate.mDay;
            break;
         case MM_DD_YYYY: case MM_DD_YY: case MMDDYYYY: case MMDDYY:
            pOS << cxDate::getMonthName(pDate.mMonth) << ' '
                << pDate.mDay << ' ' << pDate.mYear;
            break;
         case DD_MM_YYYY: case DD_MM_YY: case DDMMYYYY: case DDMMYY:
            pOS << pDate.mDay << ' '
                << cxDate::getMonthName(pDate.mMonth) << ", " << pDate.mYear;
            break;
         default: break;
      }
   }
   else
   {
      // Short format based on mDateFormat
      const string yearStr2 = std::to_string(pDate.mYear).substr(2, 2);
      const char   s        = pDate.mSepChar;
      switch (pDate.mDateFormat)
      {
         case YYYY_MM_DD: default:
            pOS << pDate.mYear << s; pad2(pDate.mMonth); pOS << s; pad2(pDate.mDay);
            break;
         case MM_DD_YYYY:
            pad2(pDate.mMonth); pOS << s; pad2(pDate.mDay); pOS << s << pDate.mYear;
            break;
         case DD_MM_YYYY:
            pad2(pDate.mDay); pOS << s; pad2(pDate.mMonth); pOS << s << pDate.mYear;
            break;
         case YY_MM_DD:
            pOS << yearStr2 << s; pad2(pDate.mMonth); pOS << s; pad2(pDate.mDay);
            break;
         case MM_DD_YY:
            pad2(pDate.mMonth); pOS << s; pad2(pDate.mDay); pOS << s << yearStr2;
            break;
         case DD_MM_YY:
            pad2(pDate.mDay); pOS << s; pad2(pDate.mMonth); pOS << s << yearStr2;
            break;
         case YYYYMMDD:
            pOS << pDate.mYear; pad2(pDate.mMonth); pad2(pDate.mDay);
            break;
         case MMDDYYYY:
            pad2(pDate.mMonth); pad2(pDate.mDay); pOS << pDate.mYear;
            break;
         case DDMMYYYY:
            pad2(pDate.mDay); pad2(pDate.mMonth); pOS << pDate.mYear;
            break;
         case YYMMDD:
            pOS << yearStr2; pad2(pDate.mMonth); pad2(pDate.mDay);
            break;
         case MMDDYY:
            pad2(pDate.mMonth); pad2(pDate.mDay); pOS << yearStr2;
            break;
         case DDMMYY:
            pad2(pDate.mDay); pad2(pDate.mMonth); pOS << yearStr2;
            break;
      }
   }
   return pOS;
}

istream& operator>>(istream& pIS, cxDate& pDate)
{
   string input;
   pIS >> input;
   pDate.fromString(input);
   return pIS;
}

// ---------------------------------------------------------------------------
// String conversion
// ---------------------------------------------------------------------------

string cxDate::toString() const
{
   ostringstream os;
   os << *this;
   return os.str();
}

bool cxDate::fromString(std::string_view pDateStr)
{
   const size_t len = pDateStr.size();
   if (len != 6 && len != 8 && len != 10)
   {
      return false;
   }

   // Helper: parse integer from a substring using stoi (C++17-style)
   auto parseInt = [&](size_t pos, size_t count) -> int
   {
      try
      {
         return std::stoi(string(pDateStr.substr(pos, count)));
      }
      catch (...)
      {
         return -1;
      }
   };

   size_t yearStart = 0, monthStart = 0, dayStart = 0;
   bool   twoDigitYear = false;
   bool   allGood      = true;

   // Position indices follow the same logic as the original date::fromString()
   if (len == 6)
   {
      twoDigitYear = true;
      switch (mDateFormat)
      {
         case YY_MM_DD: case YYYY_MM_DD: case YYYYMMDD: case YYMMDD:
            yearStart = 0; monthStart = 2; dayStart = 4; break;
         case MM_DD_YY: case MM_DD_YYYY: case MMDDYYYY: case MMDDYY:
            yearStart = 4; monthStart = 0; dayStart = 2; break;
         case DD_MM_YY: case DD_MM_YYYY: case DDMMYYYY: case DDMMYY:
            yearStart = 4; monthStart = 2; dayStart = 0; break;
         default: allGood = false; break;
      }
   }
   else if (len == 8)
   {
      // Detect presence of separators at positions 2 and 5
      bool sep2 = !std::isdigit(static_cast<unsigned char>(pDateStr[2]));
      bool sep5 = !std::isdigit(static_cast<unsigned char>(pDateStr[5]));
      if (sep2 && sep5)
      {
         // ##-##-## format (2-digit year)
         twoDigitYear = true;
         int part1 = parseInt(0, 2);
         switch (mDateFormat)
         {
            case MM_DD_YY: case MM_DD_YYYY: case MMDDYYYY: case MMDDYY:
               monthStart = 0; dayStart = 3; yearStart = 6; break;
            case DD_MM_YY: case DD_MM_YYYY: case DDMMYYYY: case DDMMYY:
               dayStart = 0; monthStart = 3; yearStart = 6; break;
            default: // auto-detect
               if (part1 > 12)
               {
                  dayStart = 0; monthStart = 3; yearStart = 6;
               }
               else
               {
                  monthStart = 0; dayStart = 3; yearStart = 6;
               }
               break;
         }
      }
      else if (!sep2 && !sep5)
      {
         // YYYYMMDD or MMDDYYYY or DDMMYYYY (no separators, 8 digits)
         switch (mDateFormat)
         {
            case MM_DD_YYYY: case MMDDYYYY: case MMDDYY:
               monthStart = 0; dayStart = 2; yearStart = 4; break;
            case DD_MM_YYYY: case DDMMYYYY: case DDMMYY:
               dayStart = 0; monthStart = 2; yearStart = 4; break;
            default: // YYYYMMDD
               yearStart = 0; monthStart = 4; dayStart = 6; break;
         }
      }
      else
      {
         allGood = false;
      }
   }
   else // len == 10: YYYY-MM-DD, MM-DD-YYYY, DD-MM-YYYY
   {
      switch (mDateFormat)
      {
         case MM_DD_YYYY: case MMDDYYYY:
            monthStart = 0; dayStart = 3; yearStart = 6; break;
         case DD_MM_YYYY: case DDMMYYYY:
            dayStart = 0; monthStart = 3; yearStart = 6; break;
         default: // YYYY-MM-DD
            yearStart = 0; monthStart = 5; dayStart = 8; break;
      }
   }

   if (!allGood) return false;

   int year  = parseInt(yearStart,  (len == 6 || (len == 8 && twoDigitYear)) ? 2 : 4);
   int month = parseInt(monthStart, 2);
   int day   = parseInt(dayStart,   2);

   if (year < 0 || month < 0 || day < 0) return false;

   // Expand 2-digit years
   if (twoDigitYear)
   {
      year += (year > CXDATE_YEAR_2DIGIT_LAST_CENT)
              ? (CXDATE_DEFAULT_YEAR - 100)
              : CXDATE_DEFAULT_YEAR;
   }

   validateDateElements(year, month, day);
   mYear  = year;
   mMonth = month;
   mDay   = day;
   return true;
}

// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------

int cxDate::numMonthDays() const noexcept
{
   return numMonthDays(mMonth, mYear);
}

bool cxDate::isLeapYear() const noexcept
{
   return isLeapYear(mYear);
}

eWeekDays cxDate::dayOfWeek() const noexcept
{
   return dayOfWeek(mYear, mMonth, mDay);
}

string cxDate::dayOfWeekStr() const
{
   return dayOfWeekStr(dayOfWeek());
}

// ---------------------------------------------------------------------------
// Static utility methods
// ---------------------------------------------------------------------------

int cxDate::numMonthDays(int pMonth, int pYear) noexcept
{
   // Expand 2-digit year
   if (pYear < 100)
   {
      pYear += (pYear > CXDATE_YEAR_2DIGIT_LAST_CENT)
               ? (CXDATE_DEFAULT_YEAR - 100)
               : CXDATE_DEFAULT_YEAR;
   }
   switch (pMonth)
   {
      case 4: case 6: case 9: case 11: return 30;
      case 2: return isLeapYear(pYear) ? 29 : 28;
      default:                          return 31;
   }
}

bool cxDate::isLeapYear(int pYear) noexcept
{
   return ((pYear % 4 == 0) && (pYear % 100 != 0)) || (pYear % 400 == 0);
}

eWeekDays cxDate::dayOfWeek(int pYear, int pMonth, int pDay) noexcept
{
   // Tomohiko Sakamoto's algorithm
   static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
   if (pMonth < 1) pMonth = 1;
   else if (pMonth > 12) pMonth = 12;
   if (pMonth < 3) --pYear;
   int dow = (pYear + pYear/4 - pYear/100 + pYear/400 + t[pMonth-1] + pDay) % 7;
   return static_cast<eWeekDays>(dow);
}

string cxDate::dayOfWeekStr(eWeekDays pWeekDay)
{
   switch (pWeekDay)
   {
      case eSUNDAY:    return "Sunday";
      case eMONDAY:    return "Monday";
      case eTUESDAY:   return "Tuesday";
      case eWEDNESDAY: return "Wednesday";
      case eTHURSDAY:  return "Thursday";
      case eFRIDAY:    return "Friday";
      case eSATURDAY:  return "Saturday";
      default:         return "Unknown";
   }
}

string cxDate::dayOfWeekStr(int pYear, int pMonth, int pDay)
{
   return dayOfWeekStr(dayOfWeek(pYear, pMonth, pDay));
}

string cxDate::monthName(int pMonth, bool pAbbreviated)
{
   static const char* const names[] = {
      "January","February","March","April","May","June",
      "July","August","September","October","November","December"
   };
   string name = (pMonth >= 1 && pMonth <= 12) ? names[pMonth-1] : "Unknown";
   if (pAbbreviated && name.size() > 3)
   {
      name = name.substr(0, 3);
   }
   return name;
}

string cxDate::getToday(bool pLong)
{
   if (pLong)
   {
      cxDate d;
      d.setDisplayLong(true);
      return d.toString();
   }
   // Use std::chrono for a thread-safe current time
   using namespace std::chrono;
   auto now   = system_clock::now();
   auto now_t = system_clock::to_time_t(now);
   std::tm tm{};
#ifdef _WIN32
   localtime_s(&tm, &now_t);
#else
   localtime_r(&now_t, &tm);
#endif
   ostringstream os;
   os.fill('0');
   os << setw(4) << (tm.tm_year + 1900) << '-'
      << setw(2) << (tm.tm_mon  + 1)    << '-'
      << setw(2) << tm.tm_mday;
   return os.str();
}

int cxDate::getCurrentYear() noexcept
{
   using namespace std::chrono;
   auto now_t = system_clock::to_time_t(system_clock::now());
   std::tm tm{};
#ifdef _WIN32
   localtime_s(&tm, &now_t);
#else
   localtime_r(&now_t, &tm);
#endif
   return tm.tm_year + 1900;
}

cxDate cxDate::today() noexcept
{
   return cxDate(); // default constructor already retrieves today
}

std::optional<cxDate> cxDate::tryParse(std::string_view pDateStr,
                                        eDateFormats pDateFormat,
                                        char pSepChar)
{
   cxDate d(pDateFormat, pSepChar);
   if (d.fromString(pDateStr))
   {
      return d;
   }
   return std::nullopt;
}

// ---------------------------------------------------------------------------
// Julian date conversions  (Fliegel-Van Flandern algorithm)
// ---------------------------------------------------------------------------

long cxDate::toJulian() const noexcept
{
   return toJulian(mYear, mMonth, mDay);
}

long cxDate::toJulian(int pYear, int pMonth, int pDay) noexcept
{
   long M1 = (static_cast<long>(pMonth) - 14) / 12;
   long Y1 = static_cast<long>(pYear) + 4800;
   return 1461*(Y1+M1)/4
        + 367*(static_cast<long>(pMonth)-2-12*M1)/12
        - (3*((Y1+M1+100)/100))/4
        + static_cast<long>(pDay)
        - 32075;
}

void cxDate::fromJulian(long pJulianDate) noexcept
{
   fromJulian(pJulianDate, mYear, mMonth, mDay);
}

void cxDate::fromJulian(long pJulianDate,
                         int& pYear, int& pMonth, int& pDay) noexcept
{
   long p = pJulianDate + 68569;
   long q = 4*p/146097;
   long r = p - (146097*q + 3)/4;
   long s = 4000*(r+1)/1461001;
   long t = r - 1461*s/4 + 31;
   long u = 80*t/2447;
   long v = u/11;
   pYear  = static_cast<int>(100*(q-49)+s+v);
   pMonth = static_cast<int>(u + 2 - 12*v);
   pDay   = static_cast<int>(t - 2447*u/80);
}

// ---------------------------------------------------------------------------
// Date format detection
// ---------------------------------------------------------------------------

eDateFormats cxDate::getDateFormat(std::string_view pDateStr)
{
   eDateFormats fmt = UNKNOWN;
   const size_t len = pDateStr.size();

   auto isDigit = [](char c){ return std::isdigit(static_cast<unsigned char>(c)); };
   auto part1   = [&]() -> int
   {
      try
      {
         return std::stoi(string(pDateStr.substr(0, 2)));
      }
      catch (...)
      {
         return 0;
      }
   };

   switch (len)
   {
      case 6:
         if (allDigits(pDateStr))
         {
            fmt = (part1() > 12) ? DDMMYY : MMDDYY;
         }
         break;
      case 8:
         if (allDigits(pDateStr))
         {
            fmt = YYYYMMDD;
         }
         else if (!isDigit(pDateStr[2]) && !isDigit(pDateStr[5]))
         {
            // ##-##-## (2-digit year)
            fmt = (part1() > 12) ? DD_MM_YY : MM_DD_YY;
         }
         break;
      case 10:
         if (!isDigit(pDateStr[2]) && !isDigit(pDateStr[5]))
         {
            // ##-##-#### (day/month first)
            fmt = (part1() > 12) ? DD_MM_YYYY : MM_DD_YY;
         }
         else if (!isDigit(pDateStr[4]) && !isDigit(pDateStr[7]))
         {
            // ####-##-## (year first)
            fmt = YYYY_MM_DD;
         }
         break;
      default:
         break;
   }
   return fmt;
}

string cxDate::getDateFormatStr(eDateFormats pDateFormat)
{
   switch (pDateFormat)
   {
      case YYYY_MM_DD: return "YYYY_MM_DD";
      case MM_DD_YYYY: return "MM_DD_YYYY";
      case DD_MM_YYYY: return "DD_MM_YYYY";
      case YY_MM_DD:   return "YY_MM_DD";
      case MM_DD_YY:   return "MM_DD_YY";
      case DD_MM_YY:   return "DD_MM_YY";
      case YYYYMMDD:   return "YYYYMMDD";
      case MMDDYYYY:   return "MMDDYYYY";
      case DDMMYYYY:   return "DDMMYYYY";
      case YYMMDD:     return "YYMMDD";
      case MMDDYY:     return "MMDDYY";
      case DDMMYY:     return "DDMMYY";
      default:         return "UNKNOWN";
   }
}

// ---------------------------------------------------------------------------
// Calendar output
// ---------------------------------------------------------------------------

void cxDate::monthCalendar(int pYear, int pMonth,
                             ostream& pOutStream, int pTrailingEndlines)
{
   pOutStream << monthName(pMonth) << ' ' << pYear << '\n';
   pOutStream << " Su Mo Tu We Th Fr Sa\n";

   const int numDays       = numMonthDays(pMonth, pYear);
   const int firstWeekDay  = static_cast<int>(dayOfWeek(pYear, pMonth, 1));

   // Print leading blanks
   for (int d = 0; d < firstWeekDay; ++d)
   {
      pOutStream << "   ";
   }

   int col = firstWeekDay;
   for (int day = 1; day <= numDays; ++day)
   {
      pOutStream << setw(2) << std::setfill(' ') << day;
      if (++col == 7)
      {
         pOutStream << '\n';
         col = 0;
      }
      else
      {
         pOutStream << ' ';
      }
   }
   if (col != 0) pOutStream << '\n';

   for (int i = 0; i < pTrailingEndlines; ++i)
   {
      pOutStream << '\n';
   }
}

void cxDate::yearCalendar(int pYear, ostream& pOutStream, int pTrailingEndlines)
{
   for (int month = 1; month <= 12; ++month)
   {
      monthCalendar(pYear, month, pOutStream, (month < 12) ? 2 : 0);
   }
   for (int i = 0; i < pTrailingEndlines; ++i)
   {
      pOutStream << '\n';
   }
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

bool cxDate::validateDateElements(int pYear, int& pMonth, int& pDay) noexcept
{
   bool valid = true;
   // C++17: std::clamp for concise range-clamping
   int clampedMonth = std::clamp(pMonth, CXDATE_START_MONTH, CXDATE_END_MONTH);
   if (clampedMonth != pMonth)
   {
      pMonth = clampedMonth; valid = false;
   }

   int maxDay = numMonthDays(pMonth, pYear);
   int clampedDay = std::clamp(pDay, CXDATE_START_DAY, maxDay);
   if (clampedDay != pDay)
   {
      pDay = clampedDay; valid = false;
   }

   return valid;
}

string cxDate::getMonthName(int pMonth)
{
   // Short 3-char month name (kept for internal use)
   static const char* const abbr[] = {
      "Jan","Feb","Mar","Apr","May","Jun",
      "Jul","Aug","Sep","Oct","Nov","Dec"
   };
   return (pMonth >= 1 && pMonth <= 12) ? abbr[pMonth-1] : "Unk";
}

bool cxDate::allDigits(std::string_view pStr) noexcept
{
   return std::all_of(pStr.begin(), pStr.end(),
      [](unsigned char c){ return std::isdigit(c); });
}

bool cxDate::startsWithNumber(std::string_view pStr) noexcept
{
   return !pStr.empty() && std::isdigit(static_cast<unsigned char>(pStr[0]));
}
