//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
#include <cassert>



#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include <Pegasus/Common/CIMDateTimeWindows.cpp>
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include <Pegasus/Common/CIMDateTimeUnix.cpp>
#elif defined(PEGASUS_OS_TYPE_NSK)
# include <Pegasus/Common/CIMDateTimeNsk.cpp>
#elif defined(PEGASUS_OS_VMS)
# include <Pegasus/Common/CIMDateTimeVms.cpp>
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

/*constructs a CIMDateTime object from micro second value.
pusdo code:
check to make sure
    if not interval{
        check to make sure number of micro seconds is not greater then or equal to
        10,000 years (uper bound of time stamp)
     }
    if interval {
        check to make sure heck to make surenumber of micro seconds is not greater
        then or equal to 100 millon years (uperbound of interval)
    }

    number of days = day_sub1

    if interval{
        get number of 400 year blocks  (number of days in 400 years is always the same)
        While (numer of days after taking out 400 year blocks > 365){
            if (not a leap year){
                subtract 365 days from tottal
            }
            else{
                subtract 366 days from toatal
            }
        }

        tottal number of years = year

        if (year we are calculating months for is a leap year){
            add one to end date of Feb and add on to start end end days of all month after Feb
        }

        if (number of days left after taking of years is between 334 and 365){
            we are in the 12th month
            subtract months from number of days left
         }
         else if (number of days left after taking of years is between 334 and 304){
            we are in the 11th month
            subtract months from number of days left


            ...


         esle if (number of days left after taking of years is between 31 and 0){
            we are in the 1st month
            subtract months from number of days left
         }
         else{
            we should never get to this code
         }

         get number of days left
    }

    get number of hours, minutes, seconds, and microseconds

    build string representaion of object

    send string to set()
 */
CIMDateTime::CIMDateTime(Uint64 microSec, Boolean interval)
{
    if (microSec >= _TEN_THOUSAND_YEARS && !interval) { //time stamps must be less then number of micro Seconds in 10,000 years
        MessageLoaderParms parmsTS("Common.Exception.DATETIME_OUT_OF_RANGE_EXCEPTION",
               "trying to create a CIMDateTime object (time stamp) greater then the year 10,000");
        throw DateTimeOutOfRangeException(parmsTS);
    }
    if (microSec >= _HUNDRED_MILLION_DAYS && interval) { //intervals must be less then the number of microseconds in 100 million days
        MessageLoaderParms parmsIN("Common.Exception.DATETIME_OUT_OF_RANGE_EXCEPTION",
               "can't create a CIMDateTime object (interval) greater then the year 100 million days");
         throw DateTimeOutOfRangeException(parmsIN);
    }


    //Set of Strings that hold part parts of datetime    100,000,000
    String year, ye_mo, ye_mo_da, ye_mo_da_ho, ye_mo_da_ho_mn, ye_mo_da_ho_mn_se, final;

    Uint32 day_sub1 = (Uint32)(microSec/_ONE_DAY);
    Uint32 days_in400 = 146097;

    if (!interval) {
        Uint32 blocks_400 = day_sub1/days_in400;  //calculates the number of 400 year blocks in number
        Uint32 blocks_rem = day_sub1%days_in400;  //clauclates number of day after 400 year blocks are removed
        Uint32 days_rem = blocks_rem;


        /*While loop computes number of years form number of day
        taking into accoutn leap years
        */
        //cout << " days_rem in constuctor that takes Micro = " << days_rem << endl;
        Uint32 i = 1;
        Uint32 count = 0;
        Uint32 leap_next = 1;

        /*  ((i%4 == 0) && (i%100 != 0)) || (i%400 == 0)) is only true for leap years. So this while
        loop says "do if days_rem >= for non-leap years or days_rem >= 366 for leap years
        */
        while ((days_rem >= 365 && !(((i%4 == 0) && (i%100 != 0)) || (i%400 == 0))) || (days_rem >= 366 && (((i%4 == 0) && (i%100 != 0)) || (i%400 == 0)))) {
            if (!(((i%4 == 0) && (i%100 != 0)) || (i%400 == 0))) {
                days_rem = days_rem - 365;
                count = count + 1;
                i = i+1;
                leap_next = leap_next + 1;
            }
            else{
                days_rem = days_rem - 366;
                count = count + 1;
                i = i + 1;
                leap_next = 0;
                 //cout << "leap year" << endl;
            }
        }

      // cout << "leap next = " << leap_next << " and count = " << count << " days_rem = " << days_rem << endl;
        Uint32 tot_year = (blocks_400 * 400) + count;

        //converting number of years from Uint32 -> String
        char buffer_year [10];
        sprintf(buffer_year, "%04d", tot_year);
        year = String(buffer_year);
        if (tot_year > 9999) {
            assert(false);
            // the calculated year should never be greater then 9999
        }


        /*Switch block is used to calculate the the number of months from the number of days
        left after years are subtracted.
        */

        Uint16 lp = 0;

        /*lp is 0 for non leap years and 1 for leap years
        */
        if ((((i%4 == 0) && (i%100 != 0)) || (i%400 == 0))) {
            lp = 1;
           // cout << "months are being calculated for a leap year" << endl;
        }

        char bu_day [5];
        sprintf(bu_day, "%02d", days_rem);



        /* this block of if else statments figures out the number of months. When it subtracts
        days it is subtracting whole days. i.e. if 0 days for the year means Jan. 1
        */
        if (days_rem < Uint32(365+lp) && (days_rem >= Uint32(334+lp))) {
            ye_mo = year.append(String("12"));
            days_rem = days_rem - (334+lp);
        }
        else if((days_rem < Uint32(334+lp)) && (days_rem >= Uint32(304+lp))){
            ye_mo = year.append(String("11"));
            days_rem = days_rem - (304+lp);
        }
        else if((days_rem < Uint32(304+lp)) && (days_rem >= Uint32(273+lp))){
            ye_mo = year.append(String("10"));
            days_rem = days_rem - (273+lp);
        }
        else if((days_rem < Uint32(273+lp)) && (days_rem >= Uint32(243+lp))){
            ye_mo = year.append(String("09"));
            days_rem = days_rem - (243+lp);
        }
        else if((days_rem < Uint32(243+lp)) && (days_rem >= Uint32(212+lp))){
            ye_mo = year.append(String("08"));
            days_rem = days_rem - (212+lp);
        }
        else if((days_rem < Uint32(212+lp)) && (days_rem >= Uint32(181+lp))){
            ye_mo = year.append(String("07"));
            days_rem = days_rem - (181+lp);
        }
        else if((days_rem < Uint32(181+lp)) && (days_rem >= Uint32(151+lp))){
            ye_mo = year.append(String("06"));
            days_rem = days_rem - (151+lp);
        }
        else if((days_rem < Uint32(151+lp)) && (days_rem >= Uint32(120+lp))){
            ye_mo = year.append(String("05"));
            days_rem = days_rem - (120+lp);
        }
        else if((days_rem < Uint32(120+lp)) && (days_rem >= Uint32(90+lp))){
            ye_mo = year.append(String("04"));
            days_rem = days_rem - (90+lp);
        }
        else if((days_rem < Uint32(90+lp)) && (days_rem >= Uint32(59+lp))){
            ye_mo = year.append(String("03"));
            days_rem = days_rem - (59+lp);
        }
        else if((days_rem < Uint32(59+lp)) && (days_rem >= 31)){
            ye_mo = year.append(String("02"));
            days_rem = days_rem - 31;
        }
        else if(days_rem < 31){
            ye_mo = year.append(String("01"));
        }
        else{
            // this code should never be exicuted
            Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                "Error when caculating months in CIMDateTime::CIMDateTime(Uint \
                          microSec, Boolean interval)");
            assert(false);
        }

       char bu_mon [5];
       sprintf(bu_mon, "%02d", days_rem);
      // cout << "this is the days left after months is taken " << String(bu_mon) << endl;


               //converting number of days from from Uint32 -> String
        char buffer_day [5];
        sprintf(buffer_day, "%02d", (days_rem+1));   // day of the month is never 0 (1-31)
        ye_mo_da = ye_mo.append(buffer_day);

    }  //end of if(!interval)
    else {

      // cout << "number is an interval" << endl;
        char buffer_day [20];
        sprintf(buffer_day, "%08d", day_sub1);
        ye_mo_da = String(buffer_day);
    }

   //cout << "when days are added the string is  " << ye_mo_da << endl;

    //get hours, minutes, seconds and microseconds
    Uint64 after_ymd = microSec%_ONE_DAY;

    Uint32 hour_num = (Uint32)(after_ymd/_ONE_HOUR);
    Uint32 after_ymdh = (Uint32)(after_ymd%_ONE_HOUR);

    Uint32 min_num = after_ymdh/_ONE_MINUTE;
    Uint32 after_ymdhm = after_ymdh%_ONE_MINUTE;

    Uint32 sec_num = after_ymdhm/_ONE_SECOND;

    Uint32 mic_num = after_ymdhm%_ONE_SECOND;



    //converting hours, minutes, seconds and microseconds from Uint32 -> String

    char buffer_hour [10];
    sprintf(buffer_hour, "%02d", hour_num);
    ye_mo_da_ho = ye_mo_da.append(buffer_hour);
        //cout << "when hours are added the string is  " << ye_mo_da_ho << endl;

    char buffer_min [10];
    sprintf(buffer_min, "%02d", min_num);
    ye_mo_da_ho_mn = ye_mo_da_ho.append(buffer_min);
       //cout << "when minutes are added the string is  " << ye_mo_da_ho_mn << endl;


    char buffer_sec [10];
    sprintf(buffer_sec, "%02d", sec_num);
    ye_mo_da_ho_mn_se = ye_mo_da_ho_mn.append(buffer_sec);
   //cout << "when seconds are adder string is  " << ye_mo_da_ho_mn_se << endl;

    char buffer_mic [15];
    if (interval) {     //adding correct UCT off set depending on wether object should be an interval or not
        sprintf(buffer_mic, ".%06d:000", mic_num);
    }
    else{
        sprintf(buffer_mic, ".%06d+000", mic_num);
    }
    final = ye_mo_da_ho_mn_se.append(buffer_mic);
    //cout << "when at the end of constructor that takes Uint64 created string is  " << final  << endl << endl;

    _rep = new CIMDateTimeRep();
    AutoPtr<CIMDateTimeRep> rep(_rep);

    if (!_set(final)) {
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                    "CIMDateTime::CIMDateTime(Uint64 microSec, Boolean interval) failed");
        throw InvalidDateTimeFormatException(); //can't pass message to this exceptions
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



/*returns number of Micro seconds represented by a CIMDateTime object
*/
Uint64 CIMDateTime::_toMicroSeconds()
{

    Uint64 mic = 0;
    Uint64 sec = 0;
    Uint64 min = 0;
    Uint64 hor = 0;
    Uint64 day = 0;
    Uint64 mon = 0;
    Uint64 yea = 0;
    Uint64 date = 0;
    Uint64 yea_400 = 0;
    Uint64 day_400 =0;

    Uint32 mic_sp= _rep->microSec.find('*');
    if (mic_sp == PEG_NOT_FOUND) {
        mic = atol(_rep->microSec.getCString());
    }
    else if (mic_sp > 0) {
        String subMic = _rep->microSec.subString(0, mic_sp);
        mic = Uint64(atol(subMic.getCString()) * pow((double)10,(double)(6-mic_sp)));
    }
    else{
        mic = 0;
    }


    if (_rep->seconds.find('*') == PEG_NOT_FOUND) {
        sec = atol(_rep->seconds.getCString()) * 1000000;
    }

    if (_rep->minutes.find('*') == PEG_NOT_FOUND) {
        min = atol(_rep->minutes.getCString()) * _ONE_MINUTE;
    }

    if (_rep->hours.find('*') == PEG_NOT_FOUND) {
        hor = (atol(_rep->hours.getCString())) * _ONE_HOUR;
    }


 //cout << "this is what the object holds year - " << _rep->year << " months - " << /
    // _rep->month << " - days  " << _rep->days << endl << "hour - " << _rep->hours << endl;
// cout << "minute - " << _rep->minutes << endl;


    if (!isInterval()) {

        /* change years into micro Seconds. Taking leap years into accout there are 146,097
         days every 400 years. Remainder years outside of 400 year blocks will be handled by
         a while loop
         */
        Uint64 yea_num = 0;

        if (_rep->year.find('*') == PEG_NOT_FOUND) {
            yea_num = atol(_rep->year.getCString());
        }


        Uint64 yea_400 = yea_num/400;
        Uint64 day_400  = yea_400 * 146097;

        Uint32 count_le = 0;
        Uint32 count_r = 0;
        Uint64 yea_rem = yea_num%400;
        Uint32 leap_next = 0;
        Uint32 count = 1;
        Uint16 lp = 0;
        Uint64 day_rem;


           // (((count%4 == 0) && (count%100 != 0)) || (count%400 == 0)) is only true for leap years
        for (Uint32 i=1; i<=yea_rem; i++) {
            if (!(((count%4 == 0) && (count%100 != 0)) || (count%400 == 0))) {
                count_r = count_r + 1;
                count = count + 1;
                leap_next = leap_next + 1;
            }
            else {
                count_le = count_le + 1;
                count = count + 1;
                leap_next = 0;
            }
        }


        /*lp is 0 for non leap years and 1 for leap years
        */
        if ((((count%4 == 0) && (count%100 != 0)) || (count%400 == 0))) {
            lp = 1;
           //cout << "months are being calculated for a leap year" << endl;
        }


        if (_rep->month.find('*') == PEG_NOT_FOUND) {

            //get number of days eqivalent to number of months in the object and multipy by number of
            //micro seconds in a day
            switch (atol(_rep->month.getCString())) { //months can't be equal to zero
            case 1:
                mon = 00;
                break;
            case 2:
                mon = 31 * _ONE_DAY;
                break;
            case 3:
                mon = ((59+lp) * _ONE_DAY);
                break;
            case 4:
                mon = ((90+lp) * _ONE_DAY);
                break;
            case 5:
                mon = ((120+lp) * _ONE_DAY);
                break;
            case 6:
                mon = ((151+lp) * _ONE_DAY);
                break;
            case 7:
                mon = ((181+lp) * _ONE_DAY);
                break;
            case 8:
                mon = ((212+lp) * _ONE_DAY);
                break;
            case 9:
                mon = ((243+lp) * _ONE_DAY);
                break;
            case 10:
                mon = ((273+lp) * _ONE_DAY);
                break;
            case 11:
                mon = ((304+lp) * _ONE_DAY);
                break;
            case 12:
                mon = ((334+lp) * _ONE_DAY);
                break;
            default:
               // cout << "error calculating months" << endl;
               // cout << "this is data " << (String)_rep->data << endl;
                throw InvalidDateTimeFormatException();
                Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                              "Code should never reach this point in \
                              CIMDateTime::_toMicroSecdonds() ");
                assert(false);
            }
        } // end of if(!interval) block



        day_rem = (count_le * 366) + (count_r * 365);
        Uint64 yea = (day_rem+day_400) * _ONE_DAY;


        if (_rep->days.find('*') == PEG_NOT_FOUND) {
            day = ((atol(_rep->days.getCString()))-1) * _ONE_DAY;   //time stamp "days" go from 1-31 ther is no zero
        }
        day += yea; // not sure why this is needed but yea doesn't hold it's vlaue outside of
                    //the if!(interval) block

        //y_dd = yea/PEGASUS_UINT64_LITERAL(86400000000);  //this is just here to illistate need for "day+=yea"
        //  cout << " this is at the end of the not interval block and year is " << y_dd << endl;

    }// end of if(!interval)

    else{
        if (_rep->days.find('*') == PEG_NOT_FOUND) {
            day = (atol(_rep->days.getCString())) * _ONE_DAY;
        }
    }


    // Uint32 y_dd = yea/PEGASUS_UINT64_LITERAL(86400000000);   //this is just here to illistate need for "day+=yea"
   // cout << " this is right before the addition and year is " << y_dd << endl;

    date = mic+sec+min+hor+day+mon; //yea is not include becaue it is included in the day value of TimeStamps

    return date;
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
    representation form one time zone to the time zone specified.
    @param cdt Time Stamp object that will be converted
    @param utc uct-offset in minutes that represents the time zone  to be
    converted to
    Returns a copy of the calling object modified to represent the same
    time in a different time zone.
*/
void CIMDateTime::setUtcOffSet(Sint32 utc)
{
    if(this->isInterval()){
        return;
    }

    MessageLoaderParms parmsOv("Common.CIMDateTime.UTC_OVERFLOW",
        "overflow has occurred during conversion to UTC");
    MessageLoaderParms parmsUn("Common.CIMDateTime.UTC_UNDERFLOW",
        "underflow has occurred during conversion to UTC");

    // convert CIMDateTime to microseconds.
    Uint64 cdt_MicroSec = this->toMicroSeconds();
    Uint32 offSet = abs(utc);
    Uint64 offSet_hor = (offSet/60) * _ONE_HOUR;
    Uint64 offSet_min = (offSet%60) * _ONE_MINUTE;
    Uint64 cdt_MicroSecSum = 0;
    String sgn_offset;

    //Add (if utc is - ) or subtract (if utc is +) utc to/from DateTime.
    if (utc >= 0) {
        if (cdt_MicroSec < (offSet_hor + offSet_min)) {
            throw DateTimeOutOfRangeException(parmsOv);
        }
        cdt_MicroSecSum = cdt_MicroSec - (offSet_hor + offSet_min);
        sgn_offset = "+";
    }

    else{
        if (_TEN_THOUSAND_YEARS < (cdt_MicroSec + offSet_hor + offSet_min)) {
            throw DateTimeOutOfRangeException(parmsUn);
        }
        cdt_MicroSecSum = cdt_MicroSec + (offSet_hor + offSet_min);
        sgn_offset = "-";
    }


    //Create new DateTime from sum of old DateTime and UTC and set UCT of new Date time.
    CIMDateTime ans = CIMDateTime(cdt_MicroSecSum, false);

    char utcBuff [5];
    sprintf(utcBuff, "%03d", offSet);
    String utc_str = sgn_offset.append(String(utcBuff));
    Boolean res = ans._rep->set_utcOffSet(utc_str);

    if (res) {
        this->_rep->copy(ans._rep);   //  set_utcOffSet worked
        return;
    }
    else{
        Tracer::trace(__FILE__,__LINE__,TRC_CIM_DATA,Tracer::LEVEL2,
                      "CIMDateTime::setUTCOffSet() failed");
        throw InvalidDateTimeFormatException();
    }
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
