#ifndef __CXDATE_H__
#define __CXDATE_H__

// Copyright (c) 2007 Eric Oulashin
//
// cxDate.h - C++17-improved date class for the cxWidgets library.
// This is a modernized version of the original 'date' class (date.h/date.cpp),
// renamed to cxDate to follow cxWidgets naming conventions, and updated to use
// C++17 standard features throughout.
//
// Changes from the original 'date' class:
//  - Class renamed from 'date' to 'cxDate'
//  - [[nodiscard]] attributes added to accessor/query methods
//  - noexcept specifications added to non-throwing methods
//  - std::string_view used for read-only string parameters
//  - std::optional<cxDate> returned by tryParse() for safe parsing
//  - std::chrono used in today() for current date retrieval
//  - std::stoi() replaces atoi() for safer string-to-int conversion
//  - std::to_string() replaces the custom anythingToString<T>() template
//  - inline constexpr replaces #define for numeric constants
//  - static cxDate today() added as a clear, expressive convenience method
//  - Default member initializers used in the class definition
//  - = default used for copy constructor, copy assignment, and destructor
//  - std::clamp used for range-clamping in validateDateElements()

#include <string>
#include <string_view>
#include <iostream>
#include <optional>

// Date format enumeration (kept compatible with original date.h)
enum eDateFormats
{
   YYYY_MM_DD,
   MM_DD_YYYY,
   DD_MM_YYYY,
   YY_MM_DD,
   MM_DD_YY,
   DD_MM_YY,
   YYYYMMDD,
   MMDDYYYY,
   DDMMYYYY,
   YYMMDD,
   MMDDYY,
   DDMMYY,
   UNKNOWN
};

enum eWeekDays
{
   eSUNDAY,
   eMONDAY,
   eTUESDAY,
   eWEDNESDAY,
   eTHURSDAY,
   eFRIDAY,
   eSATURDAY
};

// C++17: inline constexpr replaces #define for compile-time constants
inline constexpr int CXDATE_DEFAULT_YEAR           = 2000;
inline constexpr int CXDATE_YEAR_2DIGIT_LAST_CENT  = 49;
inline constexpr int CXDATE_MONTHS_IN_YEAR         = 12;
inline constexpr int CXDATE_START_MONTH            = 1;
inline constexpr int CXDATE_END_MONTH              = 12;
inline constexpr int CXDATE_START_DAY              = 1;
inline constexpr int CXDATE_YEAR_MIN               = 1800;
inline constexpr int CXDATE_YEAR_MAX               = 2500;

/**
 * \class cxDate
 * \brief Enables easy management of dates. C++17-enhanced version of the
 * original 'date' class.
 *
 * Note: Months and days are 1-based (months 1-12, days 1-31).
 * Julian date calculations assume noon for time of day, so Julian dates
 * are whole numbers.
 *
 * \author Eric Oulashin (original date class)
 * \author Mike Kinney (original date class)
 */
class cxDate
{
public:
   // -----------------------------------------------------------------------
   // Constructors / Destructor
   // -----------------------------------------------------------------------

   /** Default constructor - initializes to today's date. */
   explicit cxDate(eDateFormats pDateFormat = YYYY_MM_DD, char pSepChar = '-');

   /** Constructor with explicit year, month, day. */
   cxDate(int pYear, int pMonth, int pDay,
          eDateFormats pDateFormat = YYYY_MM_DD,
          char pSepChar = '-') noexcept;

   /** Constructor from a date string (e.g. "2026-03-16"). */
   cxDate(std::string_view pDateStr,
          eDateFormats pDateFormat = YYYY_MM_DD,
          char pSepChar = '-');

   /** Constructor from a date string with auto-detected format. */
   cxDate(std::string_view pDateStr, char pSepChar);

   /** Constructor from a Julian date number. */
   cxDate(long pJulianDate,
          eDateFormats pDateFormat = YYYY_MM_DD,
          char pSepChar = '-') noexcept;

   virtual ~cxDate() = default;
   cxDate(const cxDate&) = default;
   cxDate& operator=(const cxDate&) = default;

   // -----------------------------------------------------------------------
   // Accessors (all [[nodiscard]] and noexcept)
   // -----------------------------------------------------------------------

   /** @return The 4-digit year component of the date. */
   [[nodiscard]] int getYear() const noexcept
   {
      return mYear;
   }
   /** @return The month component of the date (1–12). */
   [[nodiscard]] int getMonth() const noexcept
   {
      return mMonth;
   }
   /** @return The day-of-month component of the date (1–31). */
   [[nodiscard]] int getDay() const noexcept
   {
      return mDay;
   }
   /** @return The date format used by toString() and stream output. */
   [[nodiscard]] eDateFormats getDateFormat() const noexcept
   {
      return mDateFormat;
   }
   /** @return The separator character used in formatted date strings (e.g. '-'). */
   [[nodiscard]] char getSepChar() const noexcept
   {
      return mSepChar;
   }
   /** @return True if the date is formatted in long human-readable form. */
   [[nodiscard]] bool getDisplayLong() const noexcept
   {
      return mDisplayLong;
   }

   // -----------------------------------------------------------------------
   // Mutators
   // -----------------------------------------------------------------------

   /** Sets the year component, clamped to [CXDATE_YEAR_MIN, CXDATE_YEAR_MAX]. */
   void setYear(int pYear)   noexcept;
   /** Sets the month component, clamped to [1, 12]. */
   void setMonth(int pMonth) noexcept;
   /** Sets the day-of-month component, clamped to the valid range for the current month. */
   void setDay(int pDay)     noexcept;
   /** Sets the date format used by toString() and stream output. */
   void setDateFormat(eDateFormats pDateFormat) noexcept;
   /** Sets the separator character used in formatted date strings. */
   void setSepChar(char pSepChar) noexcept
   {
      mSepChar = pSepChar;
   }
   /** Enables or disables long human-readable date output. */
   void setDisplayLong(bool pDisplay) noexcept
   {
      mDisplayLong = pDisplay;
   }

   /**
    * Sets year, month, and day simultaneously.
    * @return true if the date is valid; false if values were out of range.
    */
   [[nodiscard]] bool setDate(int pYear, int pMonth, int pDay) noexcept;

   // -----------------------------------------------------------------------
   // Date arithmetic
   // -----------------------------------------------------------------------

   /** Adds @p pNumYears years to the date. */
   void addYears(int pNumYears)        noexcept;
   /** Subtracts @p pNumYears years from the date. */
   void subtractYears(int pNumYears)   noexcept;
   /** Adds @p pNumMonths months to the date, adjusting the year as needed. */
   void addMonths(int pNumMonths)      noexcept;
   /** Subtracts @p pNumMonths months from the date, adjusting the year as needed. */
   void subtractMonths(int pNumMonths) noexcept;
   /** Adds @p pNumDays days to the date. */
   void addDays(int pNumDays)          noexcept;
   /** Subtracts @p pNumDays days from the date. */
   void subtractDays(int pNumDays)     noexcept;

   // -----------------------------------------------------------------------
   // Operators
   // -----------------------------------------------------------------------

   /** Adds @p pNumDays days in-place. */
   void operator+=(int pNumDays) noexcept;
   /** Subtracts @p pNumDays days in-place. */
   void operator-=(int pNumDays) noexcept;

   /** @return True if this date equals @p pThat. */
   [[nodiscard]] bool operator==(const cxDate& pThat) const noexcept;
   /** @return True if this date differs from @p pThat. */
   [[nodiscard]] bool operator!=(const cxDate& pThat) const noexcept;
   /** @return True if this date is later than @p pThat. */
   [[nodiscard]] bool operator> (const cxDate& pThat) const noexcept;
   /** @return True if this date is later than or equal to @p pThat. */
   [[nodiscard]] bool operator>=(const cxDate& pThat) const noexcept;
   /** @return True if this date is earlier than @p pThat. */
   [[nodiscard]] bool operator< (const cxDate& pThat) const noexcept;
   /** @return True if this date is earlier than or equal to @p pThat. */
   [[nodiscard]] bool operator<=(const cxDate& pThat) const noexcept;

   cxDate  operator++(int) noexcept;  // post-increment (advance one day)
   cxDate& operator++()    noexcept;  // pre-increment  (advance one day)
   cxDate  operator--(int) noexcept;  // post-decrement (go back one day)
   cxDate& operator--()    noexcept;  // pre-decrement  (go back one day)

   /** @return A new date that is @p pNumDays days after this date. */
   [[nodiscard]] cxDate operator+(int pNumDays)       const noexcept;
   /** @return A new date that is @p pNumDays days before this date. */
   [[nodiscard]] cxDate operator-(int pNumDays)       const noexcept;
   /** @return The signed number of days between this date and @p pThat. */
   [[nodiscard]] int    operator-(const cxDate& pThat) const noexcept;

   /** Writes the formatted date to @p pOS. */
   friend std::ostream& operator<<(std::ostream& pOS, const cxDate& pDate);
   /** Reads a formatted date from @p pIS. */
   friend std::istream& operator>>(std::istream& pIS, cxDate& pDate);

   // -----------------------------------------------------------------------
   // String conversion
   // -----------------------------------------------------------------------

   /** @return The date as a formatted string (format determined by mDateFormat). */
   [[nodiscard]] std::string toString() const;
   /**
    * Parses @p pDateStr and updates this date.
    * @return True on success; false if the string could not be parsed.
    */
   bool fromString(std::string_view pDateStr);

   // -----------------------------------------------------------------------
   // Utility
   // -----------------------------------------------------------------------

   /** @return The number of days in the current month. */
   [[nodiscard]] int       numMonthDays()  const noexcept;
   /** @return True if the current year is a leap year. */
   [[nodiscard]] bool      isLeapYear()    const noexcept;
   /** @return The day of the week for this date. */
   [[nodiscard]] eWeekDays dayOfWeek()     const noexcept;
   /** @return The day-of-week name as a string (e.g. "Monday"). */
   [[nodiscard]] std::string dayOfWeekStr()  const;

   // -----------------------------------------------------------------------
   // Static methods
   // -----------------------------------------------------------------------

   /**
    * @return The number of days in @p pMonth of @p pYear,
    *         accounting for leap years.
    */
   [[nodiscard]] static int       numMonthDays(int pMonth, int pYear)       noexcept;
   /** @return True if @p pYear is a leap year. */
   [[nodiscard]] static bool      isLeapYear(int pYear)                     noexcept;
   /** @return The day of the week for the given year/month/day. */
   [[nodiscard]] static eWeekDays dayOfWeek(int pYear, int pMonth, int pDay) noexcept;
   /** @return The name of @p pWeekDay as a string (e.g. "Monday"). */
   [[nodiscard]] static std::string dayOfWeekStr(eWeekDays pWeekDay);
   /** @return The day-of-week name for the given year/month/day. */
   [[nodiscard]] static std::string dayOfWeekStr(int pYear, int pMonth, int pDay);
   /**
    * @return The full or abbreviated name of @p pMonth (1–12).
    * @param pAbbreviated If true, returns a 3-letter abbreviation.
    */
   [[nodiscard]] static std::string monthName(int pMonth, bool pAbbreviated = false);

   /** Returns today's date as "YYYY-MM-DD", or in long format if pLong=true. */
   [[nodiscard]] static std::string getToday(bool pLong = false);
   /** @return The current 4-digit year. */
   [[nodiscard]] static int     getCurrentYear() noexcept;

   /**
    * C++17 addition: returns today's date as a cxDate object.
    * More expressive than the string-based getToday().
    */
   [[nodiscard]] static cxDate today() noexcept;

   /**
    * C++17 addition: tries to parse a date string, returning an
    * std::optional<cxDate> so the caller can check success without
    * exceptions or sentinel values.
    */
   [[nodiscard]] static std::optional<cxDate> tryParse(
      std::string_view pDateStr,
      eDateFormats pDateFormat = YYYY_MM_DD,
      char pSepChar = '-');

   // -----------------------------------------------------------------------
   // Julian date conversions
   // -----------------------------------------------------------------------

   /** @return This date as a Julian day number. */
   [[nodiscard]] long toJulian() const noexcept;
   /** @return The Julian day number for the given year/month/day. */
   static long  toJulian(int pYear, int pMonth, int pDay) noexcept;
   /** Sets this date from a Julian day number. */
   void         fromJulian(long pJulianDate)               noexcept;
   /** Converts @p pJulianDate to year/month/day components. */
   static void  fromJulian(long pJulianDate, int& pYear, int& pMonth, int& pDay) noexcept;

   // -----------------------------------------------------------------------
   // Date format detection
   // -----------------------------------------------------------------------

   /** @return The date format detected from @p pDateStr, or UNKNOWN. */
   [[nodiscard]] static eDateFormats getDateFormat(std::string_view pDateStr);
   /** @return A human-readable string name for @p pDateFormat. */
   [[nodiscard]] static std::string getDateFormatStr(eDateFormats pDateFormat);

   // -----------------------------------------------------------------------
   // Calendar output
   // -----------------------------------------------------------------------

   /**
    * Prints a one-month calendar to @p pOutStream.
    * @param pTrailingEndlines Number of blank lines printed after the calendar.
    */
   static void monthCalendar(int pYear, int pMonth,
                              std::ostream& pOutStream = std::cout,
                              int pTrailingEndlines = 2);
   /**
    * Prints a full-year calendar to @p pOutStream.
    * @param pTrailingEndlines Number of blank lines printed after the calendar.
    */
   static void yearCalendar(int pYear,
                             std::ostream& pOutStream = std::cout,
                             int pTrailingEndlines = 2);

private:
   int          mYear        = CXDATE_DEFAULT_YEAR;
   int          mMonth       = 1;
   int          mDay         = 1;
   eDateFormats mDateFormat  = YYYY_MM_DD;
   char         mSepChar     = '-';
   bool         mDisplayLong = false;

   // Validates and clamps month/day to legal ranges.
   // Returns true if all values were already valid.
   static bool validateDateElements(int pYear, int& pMonth, int& pDay) noexcept;

   [[nodiscard]] static std::string getMonthName(int pMonth);
   [[nodiscard]] static bool   allDigits(std::string_view pStr)        noexcept;
   [[nodiscard]] static bool   startsWithNumber(std::string_view pStr) noexcept;
};

#endif // __CXDATE_H__
