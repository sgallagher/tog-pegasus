//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//                  (sushma_fernandes@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Willis White (whiwill@ibm.com) PEP #192
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <math.h>
#include <errno.h>
#include "CIMDateTime.h"
#include "InternalException.h"
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/PegasusAssert.h>



#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include <Pegasus/Common/CIMDateTimeWindows.cpp>
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include <Pegasus/Common/CIMDateTimePOSIX.cpp>
#elif defined(PEGASUS_OS_TYPE_NSK)
# include <Pegasus/Common/CIMDateTimeNsk.cpp>
#elif defined(PEGASUS_OS_VMS)
# include <Pegasus/Common/CIMDateTimePOSIX.cpp>
#else
# error "Unsupported platform"
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMDateTime
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

// ATTN: P3 KS 04/17/02 Need methods for determining inequalities.

// ATTN: P3 KS 04/17/02 Need methods for extracting components (e.g., minutes, hours)?

// ATTN: P3 KS 04/17/02 Needs constructor that creates from individual elements(year,...)

static const char _NULL_INTERVAL_TYPE_STRING[] = "00000000000000.000000:000";
static const char _NULL_DATE_TYPE_STRING[] = "00000000000000.000000-000";
static const Uint64 _TEN_THOUSAND_YEARS = PEGASUS_UINT64_LITERAL(315569520000000000);
static const Uint64 _HUNDRED_MILLION_DAYS = PEGASUS_UINT64_LITERAL(8640000000000000000);
static const Uint64 _ONE_DAY = PEGASUS_UINT64_LITERAL(86400000000);
static const Uint64 _ONE_HOUR = PEGASUS_UINT64_LITERAL(3600000000);
static const Uint64 _ONE_MINUTE = 60000000;
static const Uint64 _ONE_SECOND = 1000000;

static const Uint32 _DAYS_IN_YEAR_0 = 366;

// The number of days preceding the (zero-based) specified month (for
// non-leap years)
static const Uint32 _MONTH_DAYS[12] =
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};


 /* the CIMDateTimeRep class holds the data for the CIMDateTime class as it's protected
data members.It also allows for "setting" and "getting" of individual components of
date time.
*/
class CIMDateTimeRep
{
public:
    String microSec;
    String seconds;
    String minutes;
    String hours;
    String days;
    String month;
    String year;
    String utcOffSet;

    enum { FORMAT_LENGTH = 25 };

    //
    // Length of the string required to store only the date and time without
    // the UTC sign and UTC offset.
    // Format is yyyymmddhhmmss.
    // Note : The size does not include the null byte.
    //
    enum { DATE_TIME_LENGTH = 14 };

    //
    // Length of the string required to store the  formatted date and time
    // Format is yyyy:mm:dd:hh:mm:ss.
    //
    enum { FORMATTED_DATE_TIME = 20 };

    //char buffer_day [5];
    //    sprintf(yearbuf, "%4d", (days_rem+1));   // day of the month is never 0 (1-31)
    //    ye_mo_da = ye_mo.append(buffer_day);


    char data[FORMAT_LENGTH + 1];


    /* Checks for correctness and sets the MicroSeconds value of CIMDateTimeRep
    */
    Uint16 set_microSec(const String & mS);

    /* Checks for correctness and sets the UTC offset of CIMDateTimeRep
    */
    Boolean set_utcOffSet(const String & uOff);

    /*Changes the CIMDateTimeRep data member data[] .
    @param value - value to be inserted into data[]
    @param index - position in data[] to start inserting the value
    @param size - size of the value paramiter (number of characters
    */
    void set_data(const String & value, Uint32 index, Uint32 size);


    /* these set functions are not used yet.
    when they become used the enum CIMDateTime::Field should be moved to CIMDateTimeRep
    */
    void set_seconds(const String & sec);
    void set_minutes(const String & min);
    void set_hours(const String & ho);
    void set_days(const String & da);
    void set_month(const String & mon);
    void set_year(const String & ye);

    void copy(const CIMDateTimeRep * cTR);

 };


/* this method  copies the all the information from one CIMDateTimeRep to another
*/
void CIMDateTimeRep::copy(const CIMDateTimeRep * cTR)
{
    //cout << "Start of Rep::copy" << endl;
    microSec = cTR->microSec;
    seconds = cTR->seconds;
    minutes = cTR->minutes;
    hours = cTR->hours;
    days = cTR->days;
    month = cTR->month;
    year = cTR->year;
    utcOffSet = cTR->utcOffSet;
    memcpy(data, cTR->data, sizeof(data));
    //cout << "end of Rep::copy" << endl <<endl;
}

/* set_microSec checks the format of the string that will be
used as the micro seconds value. If the format is correct it sets the
micro seconds value
*/
Uint16 CIMDateTimeRep::set_microSec(const String & mS)
{
    //  String fin_data = sum_data.append(mS.subString(21,4));

    //cout << "begining of set_microSec" << endl;
    Uint32 ast_post;

    ast_post = mS.find("*");
    if (ast_post == PEG_NOT_FOUND) {
        set_data(mS, 15, 6);
        microSec = mS;
        return 2;
    }

   // cout << "after find  block" << endl;
    Uint32 sub_len = 6 - ast_post;
    String ast = "******";
    String in_comp = mS.subString(ast_post, sub_len);
    if (!String::compare(in_comp, ast, sub_len)){
        set_data(mS, 15, 6);
        microSec = mS;
        return 0;
    }
    else{
       // cout << "error in set_microSec - this is the value " << mS << endl;
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
          "Error in CIMDateTimeRep::set_microSec - '*' was found in micor second string");
        return 1;
    }
}


/*set_data set the data member CIMDateTimeRep::set_data
*/
void CIMDateTimeRep::set_data(const String & value, Uint32 index, Uint32 size)
{
    for (Uint32 i=0; i < size; i++) {
        data[index+i] = (char) value[i];
    }
}


/* the following 6 methods are not used
*/
void CIMDateTimeRep::set_seconds(const String & sec)
{
    seconds = sec;
}

void CIMDateTimeRep::set_minutes(const String & min)
{
    minutes = min;
}

void CIMDateTimeRep::set_hours(const String & ho)
{
    hours = ho;
}

void CIMDateTimeRep::set_days(const String & da)
{
    days = da;
}

void CIMDateTimeRep::set_month(const String & mon)
{
    month = mon;
}

void CIMDateTimeRep::set_year(const String & ye)
{
    year = ye;
}

/* set_utcOffSet checks the format of the string representing the UTC
offset if it is correct it sets the data member CIMDateTimeRep::utcOffSet
*/
Boolean CIMDateTimeRep::set_utcOffSet(const String & uOffSet)
{
    if (uOffSet.size() != 4) {
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
              "The size of the UTC offset is %d but is but it should be 4", uOffSet.size());
        return false;
    }

    Char16 ch_one = uOffSet[0];
    if (ch_one != ':' && ch_one != '+' && ch_one != '-') {
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
        "The UTC off set must begin with a ':' or '+' or '-'. The value of the first character of UTC offset is %u", static_cast<Uint16>(ch_one));
        return false;
    }

    // the UTC must not have asterisks in it
    Uint32 spot = uOffSet.find("*");
    if (spot != PEG_NOT_FOUND)
    {
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                      "'*' was found in the UTC offset this is not allowed");
        return false;
    }


    String uOff_num = uOffSet.subString(1,3);
    for (int i=0; i < 3; i++)
    {
        if ((uOff_num[i] < '0') || (uOff_num[i] > '9'))
        {
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                 "Format is wrong - UTC offset contains non digit character.");
            return false;
        }
    }

    // intervals (:) must have 000 utc offset
    if ((ch_one == (char)':') && !String::equal(uOff_num, "000"))
    {
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                      "Trying to incorrectly set a intervals time zone");
        return false;
    }

    utcOffSet =  uOffSet;
    set_data(uOffSet, 21, 4);   // change _rep->data to reflect changes made

    return true;
}


/*Constructor
*/
CIMDateTime::CIMDateTime()
{
    _rep = new CIMDateTimeRep();
    AutoPtr<CIMDateTimeRep> rep(_rep);

    clear();

    rep.release();
}

/*Takes properly formated string and creates a CIMDateTime out of it
*/
CIMDateTime::CIMDateTime(const String & str)
{
    _rep = new CIMDateTimeRep();
    AutoPtr<CIMDateTimeRep> rep(_rep);

    if (!_set(str))
    {
        throw InvalidDateTimeFormatException();
    }

    rep.release();
}

/*Copy constructor
*/
CIMDateTime::CIMDateTime(const CIMDateTime& x)
{
    _rep = new CIMDateTimeRep();
    AutoPtr<CIMDateTimeRep> rep(_rep);

    _rep->copy(x._rep);

    rep.release();
}

CIMDateTime::CIMDateTime(Uint64 microSec, Boolean interval)
{
    if (microSec >= _TEN_THOUSAND_YEARS && !interval)
    {
        MessageLoaderParms parms(
            "Common.Exception.DATETIME_OUT_OF_RANGE_EXCEPTION",
            "Cannot create a CIMDateTime time stamp beyond the year 10,000");
        throw DateTimeOutOfRangeException(parms);
    }

    if (microSec >= _HUNDRED_MILLION_DAYS && interval)
    {
        MessageLoaderParms parms(
            "Common.Exception.DATETIME_OUT_OF_RANGE_EXCEPTION",
            "Cannot create a CIMDateTime interval greater than 100 million "
                "days");
        throw DateTimeOutOfRangeException(parms);
    }

    Uint32 year = 0;
    Uint32 month = 0;
    Uint32 day = 0;
    Uint32 hour = 0;
    Uint32 minute = 0;
    Uint32 second = 0;
    Uint32 microsecond = 0;
    char buffer[26];

    microsecond = microSec % 1000000;
    microSec /= 1000000;

    second = microSec % 60;
    microSec /= 60;

    minute = microSec % 60;
    microSec /= 60;

    hour = microSec % 24;
    microSec /= 24;

    if (interval)
    {
        day = microSec;

        sprintf(
            buffer,
            "%08u%02u%02u%02u.%06u:000",
            day,
            hour,
            minute,
            second,
            microsecond);
    }
    else
    {
        Uint32 daysRemaining = microSec;
        if (daysRemaining >= _DAYS_IN_YEAR_0)
        {
            const Uint32 _DAYS_IN_400_YEARS = 146097;
            const Uint32 _DAYS_IN_100_YEARS = 36524;
            const Uint32 _DAYS_IN_4_YEARS = 1461;

            // Account for year 0
            year = 1;
            daysRemaining -= _DAYS_IN_YEAR_0;

            year += (daysRemaining / _DAYS_IN_400_YEARS) * 400;
            daysRemaining -=
                daysRemaining / _DAYS_IN_400_YEARS * _DAYS_IN_400_YEARS;

            year += (daysRemaining / _DAYS_IN_100_YEARS) * 100;
            daysRemaining -=
                daysRemaining / _DAYS_IN_100_YEARS * _DAYS_IN_100_YEARS;

            year += (daysRemaining / _DAYS_IN_4_YEARS) * 4;
            daysRemaining -=
                daysRemaining / _DAYS_IN_4_YEARS * _DAYS_IN_4_YEARS;

            year += daysRemaining / 365;
            daysRemaining -= daysRemaining / 365 * 365;
        }

        // Determine whether this is a leap year
        Boolean leapYear =
            (((year%4 == 0) && (year%100 != 0)) || (year%400 == 0));

        // Calculate the month
        for (Uint32 m = 12; m > 0; m--)
        {
            Uint32 monthDays = _MONTH_DAYS[m-1];
            if ((m > 2) && leapYear)
            {
                monthDays += 1;
            }

            if (daysRemaining >= monthDays)
            {
                month = m;
                daysRemaining -= monthDays;
                break;
            }
        }

        // Calculate the day (days of the month start with 1)
        day = daysRemaining + 1;

        sprintf(
            buffer,
            "%04u%02u%02u%02u%02u%02u.%06u+000",
            year,
            month,
            day,
            hour,
            minute,
            second,
            microsecond);
    }

    _rep = new CIMDateTimeRep();
    AutoPtr<CIMDateTimeRep> rep(_rep);

    if (!_set(String(buffer)))
    {
        PEGASUS_ASSERT(false);
    }

    rep.release();
}

/*copies CIMDateTimeRep from passed in paramiter to the the callers CIMDateTimeRep
effectivly copies value of one CIMDateTime to another. All data in held in
CIMDateTimeRep
*/
CIMDateTime& CIMDateTime::operator=(const CIMDateTime& x)
{
    //cout << "in copy constructor" << cout;
    if (&x != this){
        _rep->copy(x._rep);
    }
        //memcpy(_rep->data, x._rep->data, sizeof(_rep->data));

    return *this;
}

CIMDateTime::~CIMDateTime()
{
    delete _rep;
}

/* retruns a string holding the CIMDateTime value
*/
String CIMDateTime::toString () const
{
    return String (_rep->data);
}

/* sets a CIMDateTime to a zero valued interval
*/
void CIMDateTime::clear()
{
   //cout << "this is the start of the clear method" << endl;
   strcpy(_rep->data, _NULL_INTERVAL_TYPE_STRING);
    String blank = "";
    String str = String("000000");
    Uint16 dum = _rep->set_microSec(str);
    _rep->set_seconds("00");
    _rep->set_minutes("00");
    _rep->set_hours("00");
    _rep->set_days("00");
    _rep->set_utcOffSet(String(":000"));
   //cout << "end of the clear method" << endl << endl;

}


/* this is one of the only ways to create a CIMDateTime object most of the constructors
call this method. It checks the value of each filed and returns false if any of them do
not have the correct format
*/
Boolean CIMDateTime::_set(const String & dateTimeStr)
{
    clear();
    CString dtStr = dateTimeStr.getCString();
    const char* str = dtStr;


    // Be sure the incoming string is the proper length:
    if (dateTimeStr.size() != CIMDateTimeRep::FORMAT_LENGTH){
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
              "CIMDateTime object string is not of the proper length");
        return false;
    }


    // Determine the type (date or interval); examine the 21st character;
    // it must be one of ':' (interval), '+' (date), or '-' (date)
    const Uint32 SIGN_OFFSET = 21;
    const Uint32 DOT_OFFSET = 14;
    Boolean isInterval = (dateTimeStr[SIGN_OFFSET] == ':');

    if (!isInterval && dateTimeStr[SIGN_OFFSET] != '+' && dateTimeStr[SIGN_OFFSET] != '-'){
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
             "CIMDateTime object has an incorrect format.");
        return false;
    }


    // Check for the decimal place:

    if (dateTimeStr[DOT_OFFSET] != '.'){
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
             "Incorrect Format - CIMDateTime object dosen't have decimal point in position 14");
        return false;
    }



    // Check to see if other characters are digits or astrisks (*)

    for (Uint32 i = 0; i < CIMDateTimeRep::FORMAT_LENGTH; i++)
    {
        if (!((i == DOT_OFFSET) || (i == SIGN_OFFSET) ||
              ((dateTimeStr[i] >= '0') && (dateTimeStr[i] <= '9')) ||
              (dateTimeStr[i] == '*')))
        {
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                    "CIMdateTime object has an incorrect format.");
            return false;
        }
    }


    // Check to see if the month and day are in range (date only):

    String buffer;
    Field ans;

    if (!isInterval)
    {
      //get year
      /* need to check that the field is valid as far as astrisk (*) are concerned */
      buffer = dateTimeStr.subString(0,4);
      ans = fieldcheck(buffer, _rep->year);
      if (ans == SOME_WILD_CARDS){
          Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
               "CIMDateTime - Format of the string is incorrect. ");
          return false;
      }
      else if (ans == ONLY_WILD_CARDS) {
          if (!restOfFields(4,dateTimeStr)){
              Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the string is incorrect. All fields after the year \
                            field should be wild cards.");
              return false;
          }
      }

        // Get the month:
        buffer = dateTimeStr.subString(4,2);
        ans = fieldcheck(buffer, _rep->month);
        if (ans == SOME_WILD_CARDS) {
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the month field is incorrect.s");
           //cout << "one was returned form fieldcheck" << endl << endl;
            return false;           // month field has both wildcards and digits
        }

        else if (ans == ONLY_DIGITS) {          // month field has only digits
            long month = atoi(buffer.getCString());

            // Allow for zero month - default value processing
            if (month == 0 || month > 12) {
                Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the string is incorrect. Month fild is out of range");
                return false;
            }
        }
        else if (ans == ONLY_WILD_CARDS) {
          if (!restOfFields(6,dateTimeStr)){
              Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the string is incorrect. All fields after the month \
                        field should ve wild cards.");
              return false;
          }
        }


        // Get the day:

        buffer = dateTimeStr.subString(6,2);
        ans = fieldcheck(buffer, _rep->days);
        if (ans == SOME_WILD_CARDS) {
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the day field is incorrect.");
            return false;           // month field has both wildcards and digits
        }

        else if (ans == ONLY_DIGITS) {          // month field has only digits

            long day = atoi(buffer.getCString());

            // Allow for zero day - 0 "day" values are only allowed for default object
            /*ATTN: this does not check for the number of days in a
            particular month
            */
            if (day == 0 || day > 31){
                Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                        "CIMDateTime - Format of the string is incorrect. Day field is incorrect");
                return false;
            }
         }
        else if (ans == ONLY_WILD_CARDS) {
          if (!restOfFields(6,dateTimeStr)){
              Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the string is incorrect. All fields after day field \
                            should be only wild cards.");
              return false;
          }
        }

        //get UTC off set   for a Time Stamp

        buffer = dateTimeStr.subString(21,4);
        _rep->utcOffSet = buffer;
        Uint32 spot = buffer.find("*");
        //cout << "before if" << endl;
        if(spot != PEG_NOT_FOUND){  // the UTC must not have astricks in it
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the is incorrect. UTC offset should not have \
                          a wild card in it.");
            return false;
        }

    }  //end of if(!interval)

    else{     //Object is an Interval

        //get days if object is an interval
        buffer = dateTimeStr.subString(0,8);
        ans = fieldcheck(buffer, _rep->days);
        if (ans == SOME_WILD_CARDS) {
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the object is incorrect. Day field must be have all \
                          wild cards or no wild cards.");
            return false;
        }
        else if (ans == ONLY_WILD_CARDS) {
          if (!restOfFields(8,dateTimeStr)){
              Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                  "CIMDateTime - Format of the object is incorrect. All fields after day field \
                            should be wild cards.");
              return false;
          }
        }

                      //ATTN: this block MAY not be needed
        // check to make sure UTC for Intervals it '000'
        buffer = dateTimeStr.subString(21,4);
        _rep->utcOffSet = buffer;
        if (!String::equal(_rep->utcOffSet, ":000"))
        {
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                   "CIMDateTime - Format of the object is incorrect. Can not set the \
                          the UTC off set of an Interval");
           // cout << "interval UTC offset is worng" << endl;
            return false;
        }


    }

    //cout << "after is interval block" << endl;

    // Check the hours and minutes:
    buffer = dateTimeStr.subString(8,2);
    ans = fieldcheck(buffer, _rep->hours);
    if (ans == SOME_WILD_CARDS) {
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                   "CIMDateTime - Format of the object is incorrect.Hour field must have all \
                          wild cards or no wild cards.");
        return false;           // hour field has both wildcards and digits
     }

    else if (ans == ONLY_DIGITS) {          // hour field has only digits
        long hours = atoi(buffer.getCString());

        if (hours > 23) {
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                   "CIMDateTime - Format of the object is incorrect. Hour value is out of range");
            return false;
        }

    }

    else if (ans == ONLY_WILD_CARDS) {
          if (!restOfFields(10,dateTimeStr)){
              Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the object is incorrect. All fields after the hour field \
                            should be wild cards.");
              return false;
          }
     }


   //cout << "this is after getting hours" << endl;

    buffer = dateTimeStr.subString(10,2);
    ans = fieldcheck(buffer, _rep->minutes);
    if (ans == SOME_WILD_CARDS) {
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                   "CIMDateTime - Format of the object is incorrect.Minute field must have all \
                          wild cards or no wild cards.");
        return false;           // minutes field has both wildcards and digits
    }

    else if (ans == ONLY_DIGITS) {          // minutes field has only digits
        long minutes = atoi(buffer.getCString());

        if (minutes > 59) {
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                   "CIMDateTime - Format of the object is incorrect.Minute value is out of range");
            return false;

        }
    }

    else if (ans == ONLY_WILD_CARDS) {
          if (!restOfFields(12,dateTimeStr)){
              Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the object is incorrect. All fields after the minute \
                            field should be wild cards.");
              return false;
          }
      }



    // cout << "before getting seconds" << endl;

    buffer = dateTimeStr.subString(12,2);
    ans = fieldcheck(buffer, _rep->seconds);
    if (ans == SOME_WILD_CARDS) {
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                   "CIMDateTime - Format of the object is incorrect.Second field must have all \
                          wild cards or no wild cards.");
        return false;           // seconds field has both wildcards and digits
    }


    else if (ans == ONLY_DIGITS) {          // minutes field has only digits
        long seconds = atoi(buffer.getCString());
        if (seconds > 59){
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                   "CIMDateTime - Format of the object is incorrect.Minute value is out of range");
            return false;
        }

    }
    else if (ans == ONLY_WILD_CARDS) {
          if (!restOfFields(14,dateTimeStr)){
              Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of the object is incorrect. All fields after the seconds \
                            field should have wild cards.");
              return false;
          }
      }


    //get micro Seconds
    String buffer_micro = dateTimeStr.subString(15,6);
    Uint32 answ = _rep->set_microSec(buffer_micro);
    if (answ == 1) {
         Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "CIMDateTime - Format of micro seconds field is incorrect");
        return false;
    }


    memcpy(_rep->data, str, sizeof(_rep->data));

    return true;

}



/* If a field has a wild card then all the fields with lower significance
    should have wild cards. This method makes sure of this.
*/
Boolean CIMDateTime::restOfFields(Uint32 start_position,const String & inStr)
{
    String splatCDT = "**************.******";
    Uint32 placeNum = splatCDT.size() - start_position;

    String comp = splatCDT.subString(start_position, placeNum);
    String in_comp = inStr.subString(start_position, placeNum);
    if (String::compare(comp, in_comp))
        return false;
    else
        return true;
}



/*this method is just a public rapper for the _set command
*/
void CIMDateTime::set(const String & str)
{
    if (!_set(str)){
        throw InvalidDateTimeFormatException();
    }
}



/*public rapper for _toMicroSeconds
converts object to UTC then it calculates the number of microseconds in the converted object
*/
Uint64 CIMDateTime::toMicroSeconds() const
{

    CIMDateTime un_norm;
    //un_norm._rep = new CIMDateTimeRep();
    un_norm._rep->copy(_rep);

    un_norm.convertToUTC();

    const Uint64 norm_micSec = un_norm._toMicroSeconds();

    return (norm_micSec);
}


// Returns the number of microseconds represented by a CIMDateTime object
Uint64 CIMDateTime::_toMicroSeconds()
{
    Uint64 microseconds = 0;

    // Retrieve the microseconds component from the CIMDateTime object
    Uint32 microsecondsSplatIndex = _rep->microSec.find('*');
    Uint32 microsecondsComponent = 0;
    if (microsecondsSplatIndex == PEG_NOT_FOUND)
    {
        microsecondsComponent = atol(_rep->microSec.getCString());
    }
    else if (microsecondsSplatIndex > 0)
    {
        String subMic = _rep->microSec.subString(0, microsecondsSplatIndex);
        microsecondsComponent = (Uint32)atol(subMic.getCString()) *
            (Uint32)pow((double)10,(double)(6-microsecondsSplatIndex));
    }
    else
    {
        microsecondsComponent = 0;
    }
    microseconds += microsecondsComponent;

    // Retrieve the seconds component from the CIMDateTime object
    if (_rep->seconds.find('*') == PEG_NOT_FOUND)
    {
        Uint64 secondsComponent = atol(_rep->seconds.getCString());
        microseconds += secondsComponent * 1000000;
    }

    // Retrieve the minutes component from the CIMDateTime object
    if (_rep->minutes.find('*') == PEG_NOT_FOUND)
    {
        Uint64 minutesComponent = atol(_rep->minutes.getCString());
        microseconds += minutesComponent * _ONE_MINUTE;
    }

    // Retrieve the hours component from the CIMDateTime object
    if (_rep->hours.find('*') == PEG_NOT_FOUND)
    {
        Uint64 hoursComponent = atol(_rep->hours.getCString());
        microseconds += hoursComponent * _ONE_HOUR;
    }

    if (isInterval())
    {
        // Retrieve the days component from the CIMDateTime object
        if (_rep->days.find('*') == PEG_NOT_FOUND)
        {
            Uint64 daysComponent = atol(_rep->days.getCString());
            microseconds += daysComponent * _ONE_DAY;
        }
    }
    else
    {
        // Retrieve the day component from the CIMDateTime object
        if (_rep->days.find('*') == PEG_NOT_FOUND)
        {
            Uint64 dayComponent = atol(_rep->days.getCString());
            microseconds += (dayComponent-1) * _ONE_DAY;
        }

        // Retrieve the month and year components from the CIMDateTime object
        if (_rep->year.find('*') == PEG_NOT_FOUND)
        {
            Uint64 yearComponent = atol(_rep->year.getCString());

            // Retrieve the month component from the CIMDateTime object.
            if (_rep->month.find('*') == PEG_NOT_FOUND)
            {
                Uint32 monthComponent = atol(_rep->month.getCString());
                PEGASUS_ASSERT((monthComponent > 0) && (monthComponent < 13));

                Uint32 monthDays = _MONTH_DAYS[monthComponent-1];
                if ((monthComponent > 2) && ((yearComponent%400 == 0) ||
                    ((yearComponent%4 == 0) && (yearComponent%100 != 0))))
                {
                    // Add the leap day
                    monthDays += 1;
                }

                // Convert months to days and then to microseconds
                microseconds += monthDays * _ONE_DAY;
            }

            if (yearComponent > 0)
            {
                // Convert years into microseconds, factoring in leap years
                yearComponent -= 1;
                microseconds +=
                    (_DAYS_IN_YEAR_0 +
                     yearComponent/400 * 146097 +
                     yearComponent%400/100 * 36524 +
                     yearComponent%100/4 * 1461 +
                     yearComponent%4 * 365) *
                    _ONE_DAY;
            }
        }
    }

    return microseconds;
}


/*compare two CIMDateTime objects for equality
*/
Boolean operator==(const CIMDateTime& x, const CIMDateTime& y)
{
    return x.equal (y);
}



/* converts Time Stamps to their UTC (GMT) representation. For Intervals
    it does nothing.
*/
void CIMDateTime::convertToUTC()
{

    if (isInterval()) {
        return;    //no conversion should not be done on Intervals
    }

    Uint64 normNum = 0;

    Uint64 un_normNum = this->_toMicroSeconds();

   // Uint32 unnor = un_normNum/PEGASUS_UINT64_LITERAL(1000000000);
   // Uint32 runnor = un_normNum%PEGASUS_UINT64_LITERAL(1000000000);

    // get UTC offSet and change it in microseconds
    String utcOS = _rep->utcOffSet.subString(1,3);
    Uint32 offSet = atol((utcOS).getCString());
    Uint64 offSet_hor = (offSet/60) * _ONE_HOUR;
    Uint64 offSet_min = (offSet%60) * _ONE_MINUTE;
    String mesO = "overflow has occurred in normalization";
    MessageLoaderParms parmsOv("Common.CIMDateTime.UTC_OVERFLOW",
        "overflow has occurred during conversion to UTC");
    MessageLoaderParms parmsUn("Common.CIMDateTime.UTC_UNDERFLOW",
        "underflow has occurred during conversion to UTC");

    char sign;   // Get the sign and UTC offset.
    sign = _rep->data[21];

    //if there are no wild cards in the minute postion then the entire utc offSet
    //will effect the CIMDateTime value

    if (_rep->minutes.find('*') == PEG_NOT_FOUND) {
        if ( sign == '-' ) {
            if (_TEN_THOUSAND_YEARS < (un_normNum + (offSet_hor + offSet_min))){
               // cout << " this is value " << this->toString() << endl;
                throw DateTimeOutOfRangeException(parmsOv);
            }
            normNum = un_normNum + (offSet_hor + offSet_min);
        }
        else{
            if (un_normNum < (offSet_hor + offSet_min)) {
                throw DateTimeOutOfRangeException(parmsUn);
            }
             normNum = un_normNum - (offSet_hor + offSet_min);
        }
    }

    //if the hours section has no wild cards but the minutes section does then only on hour
    //position will be effected by the uct off Set
    else if (_rep->hours.find('*') == PEG_NOT_FOUND) {
        if ( sign == '-' ) {
            if (_TEN_THOUSAND_YEARS < (un_normNum + (offSet_hor))){
                throw DateTimeOutOfRangeException(parmsOv);
            }
             normNum = un_normNum + (offSet_hor);
        }
        else{
            if (un_normNum < (offSet_hor)) {
                throw DateTimeOutOfRangeException(parmsUn);
            }
             normNum = un_normNum - (offSet_hor);
        }
    }
    else{ //if this block is executed then the utc offSet has no effect on CIMDateTime value
        normNum = un_normNum;
    }


    CIMDateTime norm_CDT = CIMDateTime(normNum,false);

    this->_rep->copy(norm_CDT._rep);

    return;
}


/*returns true if object is an interval.  Note: This method exists only for
  compatibility reasons.  It is superceded by the "const" form of the method.
*/
Boolean CIMDateTime::isInterval()
{
    return ((const CIMDateTime*)this)->isInterval();
}

/*returns true if object is an interval
*/
Boolean CIMDateTime::isInterval() const
{
    const Uint32 SIGN_OFFSET = 21;

    Boolean isInterval = strcmp(&_rep->data[SIGN_OFFSET], ":000") == 0 ;
    return isInterval;
}


/*compares caller to passed in paramiter for eqaulity
*/
Boolean CIMDateTime::equal (const CIMDateTime & x) const
{
    if ((x.isInterval() && !this->isInterval()) || (!x.isInterval() && this->isInterval())) {
        throw TypeMismatchException();
    }

    CIMDateTime current = CIMDateTime((String)_rep->data);
    CIMDateTime compare = CIMDateTime((String)x._rep->data);  // not sure why all this is needed but const has somthing to do with it

    Uint32 spl_pos = current.getHighestWildCardPosition(compare);

    current.insert_WildCard(spl_pos);
    compare.insert_WildCard(spl_pos);

    if (current.toMicroSeconds() == compare.toMicroSeconds()) {
        return true;
    }
    else
        return false;

}



/*subtacts two CIMDateTime objects of like types
*/
Sint64 CIMDateTime::getDifference(CIMDateTime startTime, CIMDateTime finishTime)
{

    CIMDateTime sta = startTime;
    CIMDateTime fin = finishTime;
    CIMDateTime sta_norm;
    CIMDateTime fin_norm;
    Uint64 startT_num;
    Uint64 finishT_num;
    Sint64 diff_num;

    /* the throwing of this expceptoin is only needed becasue of back wards compatability issues
    (i.e. this is the way getDifferance worked before). The operator- does not behave this way.
    */
    if ((sta.isInterval() && (!( fin.isInterval()))) || ((!( sta.isInterval())) &&  fin.isInterval())) {
        throw InvalidDateTimeFormatException();
    }

    Uint32 splat_pos;
    splat_pos = sta.getHighestWildCardPosition(fin);

    sta.insert_WildCard(splat_pos);
    fin.insert_WildCard(splat_pos);

    startT_num = sta.toMicroSeconds();
    finishT_num = fin.toMicroSeconds();

    diff_num = finishT_num - startT_num;

    return diff_num;
 }




/*  checks to make sure the format of a particular field in the DateTime string is correct.
    Returns
        ONLY_WILD_CARDS - field contains all wild cards
        SOME_WILD_CARDS - field contains some wild cards (error)
        ONLY_DIGITS - field contains only digits
*/
CIMDateTime::Field CIMDateTime::fieldcheck(const String & in_p, String & rep_field)
{
    Uint32 post;
    rep_field = in_p;

    post = in_p.find("*");
    if (post == PEG_NOT_FOUND) {
        return ONLY_DIGITS;
    }

    const String ast = "**********";
    String comp = String(ast, in_p.size());   //creates a string of asteriks with the same length as in_p
    if (!String::compare(in_p, comp)) {
        return ONLY_WILD_CARDS;                            //fields is all astriks
    }
    else{
        return SOME_WILD_CARDS;                //error - mix of asterisk and numbers  in field
    }

}




/* This method does not change the value of object, it only converts the
    representation to a specified time zone.
    @param utc utc-offset in minutes that represents the time zone to be
    converted to.
*/
void CIMDateTime::setUtcOffSet(Sint32 utc)
{
    PEGASUS_ASSERT(!isInterval());

    MessageLoaderParms parmsOv("Common.CIMDateTime.UTC_OVERFLOW",
        "overflow has occurred during conversion to UTC");
    MessageLoaderParms parmsUn("Common.CIMDateTime.UTC_UNDERFLOW",
        "underflow has occurred during conversion to UTC");

    // convert CIMDateTime to microseconds.
    Uint64 origMicroseconds = toMicroSeconds();
    Sint64 newUtcMicroseconds = utc * _ONE_MINUTE;
    Uint64 newMicroseconds = origMicroseconds - newUtcMicroseconds;

    // Check for underflow or overflow
    if (utc >= 0)
    {
        if (origMicroseconds < (Uint64)newUtcMicroseconds)
        {
            throw DateTimeOutOfRangeException(parmsUn);
        }
    }
    else
    {
        if (_TEN_THOUSAND_YEARS < (origMicroseconds - newUtcMicroseconds))
        {
            throw DateTimeOutOfRangeException(parmsOv);
        }
    }

    // Create a new CIMDateTime with the recalculated microseconds value
    // and use the specified UTC value.
    CIMDateTime newDateTime = CIMDateTime(newMicroseconds, false);

    char newUtcBuffer[5];
    sprintf(newUtcBuffer, "%+04d", utc);
    Boolean res = newDateTime._rep->set_utcOffSet(newUtcBuffer);

    if (!res)
    {
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                      "CIMDateTime::setUTCOffSet() failed");
        throw InvalidDateTimeFormatException();
    }

    this->_rep->copy(newDateTime._rep);
}



/* finds the Wild Card in the most significant position and returns
    that position
    @param cDT_s gets searched along with calling object to find the
    wild card in the most significant
    position
*/
Uint32 CIMDateTime::getHighestWildCardPosition(const CIMDateTime & cDT_s)
{

    Uint32 spot_s = cDT_s.toString().find('*'); //since this return a Uint32 and PEG_NOT_FOUND=-1 can't do a
    Uint32 spot_f = this->toString().find('*');  // straight compare


    if (spot_s == PEG_NOT_FOUND && spot_f == PEG_NOT_FOUND) {   //start time have more wild cards then finish time
       return PEG_NOT_FOUND;
    }
    else if (spot_f == PEG_NOT_FOUND) {
         return spot_s;
    }
    else if (spot_s == PEG_NOT_FOUND) {
         return spot_f;
    }
    else{
        if (spot_f < spot_s) {
            return spot_f;
        }
        else{
            return spot_s;
        }
    }
 }




/*inserts wild cards into CIMDateTime object.
    @param index - position to start placing wild card characters
    @param cdt - object to be modified
    Returns a copy of calling object with wild cards starting at
    position index
    @exception InvalidDateTimeFormatException because of invalid index
    (see rules for wild cards)
*/
void CIMDateTime::insert_WildCard(Uint32 ind)
{

    Uint32 index = ind;
    if (ind > 20) {
       index = 21;
    }

    Uint32 spot = this->toString().find('*');
    if (spot == index) {
        CIMDateTime cur = CIMDateTime(this->toString());
        return;
    }

    String splat = String("**************.******");
    String cdtStr = this->toString();
    String final;
    if (index != PEG_NOT_FOUND) {
        String str_cdtStr = cdtStr.subString(0, index);
        String sub_splat = splat.subString(index, (21-index));

        //build result
        String cdt_Splat = str_cdtStr.append(sub_splat);
        final = cdt_Splat.append(this->_rep->utcOffSet);
   }

   else{
        final = splat.append(this->_rep->utcOffSet);
   }

   CIMDateTime ans = CIMDateTime(final);
   this->_rep->copy(ans._rep);
   return;

}



CIMDateTime CIMDateTime::operator+(const CIMDateTime & cDT) const
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;
    Sint32 utc;
    Boolean isInt = this->isInterval();

    // only interval+interval and timeStamp+interval are allowed. Therefor second operand must be interval
    if (!opt_cDT.isInterval()) {
        throw TypeMismatchException();
    }

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);

    Uint64 opt_num = opt_cDT.toMicroSeconds();
    Uint64 cur_num = cur_cDT.toMicroSeconds();

    Uint64 ans = opt_num + cur_num;
    CIMDateTime ans_cdt = CIMDateTime(ans, isInt);

    if (!isInt) {
        utc = atol((_rep->utcOffSet).getCString());
        ans_cdt.setUtcOffSet(utc);
    }

    ans_cdt.insert_WildCard(splat_pos);

    return ans_cdt;
}




CIMDateTime & CIMDateTime::operator+=(const CIMDateTime & cDT)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    CIMDateTime sum_cdt = cur_cDT + opt_cDT;

    _rep->copy(sum_cdt._rep);

     return *this;
}


CIMDateTime CIMDateTime::operator-(const CIMDateTime & cDT) const
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    CIMDateTime ans_cdt;
    Sint32 utc;

    Boolean cur_isIn = this->isInterval();
    Boolean opt_isIn = opt_cDT.isInterval();

    // only I-I, T-I and T-T are allowed
    if (cur_isIn && !opt_isIn) {
        throw TypeMismatchException();
    }

    Uint64 opt_num = opt_cDT.toMicroSeconds();
    Uint64 cur_num = cur_cDT.toMicroSeconds();

    if (cur_num < opt_num) {
        MessageLoaderParms parmsSub("Common.Exception.DATETIME_OUT_OF_RANGE_EXCEPTION",
               "Result of subtracting two CIMDateTimes would be negative.");
        throw DateTimeOutOfRangeException(parmsSub);
    }

    Uint64 diff = cur_num - opt_num;

    if ((cur_isIn && opt_isIn) || (!cur_isIn && !opt_isIn)) { //don't konw how to do logical xor
        ans_cdt = CIMDateTime(diff, true);
    }
    else{
        ans_cdt = CIMDateTime(diff, false);
        utc = atol((_rep->utcOffSet).getCString());
        ans_cdt.setUtcOffSet(utc);
    }

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);
    ans_cdt.insert_WildCard(splat_pos);

     return ans_cdt;
}




CIMDateTime & CIMDateTime::operator-=(const CIMDateTime & cDT)
{

    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    CIMDateTime dif_cdt = cur_cDT - opt_cDT;
      _rep->copy(dif_cdt._rep);

     return *this;
}




CIMDateTime CIMDateTime::operator*(Uint64 num) const
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));

    if (!this->isInterval()){
        throw TypeMismatchException();
    }

    Uint64 cur_num = cur_cDT.toMicroSeconds();
    Uint64 prod = cur_num * num;

    CIMDateTime prod_cdt = CIMDateTime(prod, true);

    CIMDateTime dummy;
    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(dummy);

    prod_cdt.insert_WildCard(splat_pos);

    return prod_cdt;
}




CIMDateTime & CIMDateTime::operator*=(Uint64 num)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));

    CIMDateTime prod_cdt = cur_cDT * num;

    _rep->copy(prod_cdt._rep);

     return *this;
}




CIMDateTime CIMDateTime::operator/(Uint64 num) const
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));

    if (!(this->isInterval())){
        MessageLoaderParms parmsD("Common.CIMDateTime.INVALID_OPERATION_DIV_INT",
                                 "Can not divide a TimeStamp by an integer");
        throw TypeMismatchException(parmsD);
    }

    if (num == 0) {
        MessageLoaderParms parmsZ("Common.CIMDateTime.INVALID_OPERATION_DIV_ZERO",
                                 "Can not divide CIMDateTime by zero");
        throw Exception(parmsZ);
    }

    Uint64 cur_num = cur_cDT.toMicroSeconds();
    Uint64 prod = cur_num/num;

    CIMDateTime prod_cdt = CIMDateTime(prod, true);
    CIMDateTime dummy;

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(CIMDateTime(dummy));
    prod_cdt.insert_WildCard(splat_pos);

    return prod_cdt;
}



CIMDateTime & CIMDateTime::operator/=(Uint64 num)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));

    CIMDateTime ans = cur_cDT/num;
    _rep->copy(ans._rep);

     return *this;
}




Uint64 CIMDateTime::operator/(const CIMDateTime & cDT) const
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;


    if (!cur_cDT.isInterval() || !opt_cDT.isInterval()) {
        MessageLoaderParms parmsMM("Common.CIMDateTime.INVALID_OPERATION_DIV_TS",
                             "Can not divide two CIMDateTime objects if one of them is a TimeStamp");
        throw TypeMismatchException(parmsMM);
    }

    Uint64 opt_num = opt_cDT.toMicroSeconds();
    Uint64 cur_num = cur_cDT.toMicroSeconds();

    if (opt_num == 0) {
        MessageLoaderParms parmsDZ("Common.CIMDateTime.INVALID_OPERATION_DIV_ZERO_CDT",
                             "Trying to divide a CIMDateTime object by a zero value CIMDateTime object");
        throw Exception(parmsDZ);
    }

    Uint64 ans = cur_num/opt_num;

    return ans;
}




Boolean CIMDateTime::operator<(const CIMDateTime & cDT) const
{

    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    if ((!cur_cDT.isInterval() && opt_cDT.isInterval()) || (cur_cDT.isInterval() && !opt_cDT.isInterval())) {
        MessageLoaderParms parms("Common.CIMDateTime.INVALID_OPERATION_COMP_DIF",
                             "Trying to compare CIMDateTime objects of differing types");
        throw TypeMismatchException(parms);
    }

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);
    opt_cDT.insert_WildCard(splat_pos);
    cur_cDT.insert_WildCard(splat_pos);

    Uint64 opt_num = opt_cDT.toMicroSeconds();
    Uint64 cur_num = cur_cDT.toMicroSeconds();

    if (cur_num < opt_num) {
        return true;
    }
    else{
        return false;
    }
}




Boolean CIMDateTime::operator<=(const CIMDateTime & cDT) const
{
    CIMDateTime cur = CIMDateTime((String)(this->_rep->data));

   if ((cur < cDT) || (cur == cDT)) {
      return true;
   }
   else{
       return false;
   }
}




Boolean CIMDateTime::operator>(const CIMDateTime & cDT) const
{
    CIMDateTime cur = CIMDateTime((String)(this->_rep->data));

    if ((!(cur < cDT)) && (!(cur == cDT))) {
       return true;
    }
    else{
       return false;
    }
}




Boolean CIMDateTime::operator>=(const CIMDateTime & cDT) const
{
    CIMDateTime cur = CIMDateTime((String)(this->_rep->data));

   if (!(cur < cDT)) {
      return true;
   }
   else{
      return false;
   }
}



Boolean CIMDateTime::operator!=(const CIMDateTime & cDT) const
{
    CIMDateTime cur = CIMDateTime((String)(this->_rep->data));

    if (!(cur  == cDT)) {
        return true;
    }
    else{
        return false;
    }
}



PEGASUS_NAMESPACE_END
