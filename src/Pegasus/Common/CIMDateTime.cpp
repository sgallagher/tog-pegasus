//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//                  (sushma_fernandes@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Willis White (whiwill@ibm.com) PEP #192
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <time.h>
#include "CIMDateTime.h" 
#include "InternalException.h" 

#include <errno.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include <Pegasus/Common/CIMDateTimeWindows.cpp>
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include <Pegasus/Common/CIMDateTimeUnix.cpp>
#elif defined(PEGASUS_OS_TYPE_NSK)
# include <Pegasus/Common/CIMDateTimeNsk.cpp>
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





void CIMDateTimeRep::copy(CIMDateTimeRep * cTR)
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

Uint16 CIMDateTimeRep::set_microSec(String & mS)
{
    // update _rep->data to reflect change 
   /* String repData = String(data, FORMAT_LENGTH);
    String begin_data = repData.subString(0, 15);
    char mic_buf [7];
    sprintf(mic_buf,"%06",atol(mS.getCString()));  // this is the only way I can gareentee the length of string
    String mic_str = String(mic_buf);
    String sum_data = begin_data.append(mic_str);
    String fin_data = sum_data.append(mS.subString(21,4));    */
    
    //cout << "begining of set_microSec" << endl;
    Uint32 ast_post;
 
    ast_post = mS.find("*");
    //cout <<"this is the try block inside set micro this mS " << mS << endl;
    if (ast_post == PEG_NOT_FOUND) {
    //cout << "check to see if it equals PEG_NOT_FOUND in microSecheck" << endl;
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
        cout << "error in set_microSec - this is the value " << mS << endl;
        return 1;
    }
}

void CIMDateTimeRep::set_data(String value, Uint32 index, Uint32 size)
{
    //cout << "value is " << value << " this is index " << index << " this is size " << size << endl;
    //cout << "this is data " << (String)data << endl;
    for (Uint32 i=0; i < size; i++) {
       // cout << " this i " << i << " element of value " << value[i] << " element of data " << data[index+i] << endl;
        data[index+i] = value[i];
    } 
}

void CIMDateTimeRep::set_seconds(String sec)
{
    seconds = sec;
}
    
void CIMDateTimeRep::set_minutes(String min)
{
    minutes = min;
}
    
void CIMDateTimeRep::set_hours(String ho)
{
    hours = ho;
}
    
void CIMDateTimeRep::set_days(String da)
{
    days = da;
}

void CIMDateTimeRep::set_month(String mon)
{
    month = mon;
}
    
void CIMDateTimeRep::set_year(String ye)
{
    year = ye;
}

Boolean CIMDateTimeRep::set_utcOffSet(String uOffSet)
{
    if (uOffSet.size() != 4) {
        cout << "utc offset has the wrong format 1" << endl;
        return false;
    }

    char ch_one = (char) uOffSet[0];
    if (ch_one != ':' && ch_one != '+' && ch_one != '-') {
        cout << "utc offset has the wrong format 2" << endl;
        return false;
    }

    Uint32 spot = uOffSet.find("*");
    if(spot != PEG_NOT_FOUND){  // the UTC must not have astricks in it
        cout << "utc offset has the wrong format 3" << endl;
        return false;
    }
 

    String uOff_num = uOffSet.subString(1,3);
    for (int i=0; i < 3; i++) {
        if (!isdigit(uOff_num[i])) {
            cout << "utc off set contain non digit charichter" << endl;
            cout << "this is off set " << uOff_num << endl;
            return false;
        }
    }

    if (ch_one == (char)':' && !String::compare(uOff_num, "OOO")){    // intervals (:) must have 000 utc offset
        cout << "trying to incorrectly set a intervals time zone" << endl;
        return false;
    }

  // cout << "In set_uctOffSet the off set is " << uOffSet << endl; 
    utcOffSet =  uOffSet;
   // cout << "this is _rep->utcOffSet " << utcOffSet << endl;
   // cout << "this was data " << (String)data << endl;
    set_data(uOffSet, 21, 4);   // change _rep->data to reflect 
    //cout << "this is data now " << (String)data << endl;

    return true;

        
}



CIMDateTime::CIMDateTime()
{
    _rep = new CIMDateTimeRep();
    clear();
}

CIMDateTime::CIMDateTime(const String & str)
{
    _rep = new CIMDateTimeRep();
    if (!_set(str))
    {
        //cout << "in throw part of CIMDateTime(const String & str)" << endl; 
        delete _rep;
        throw InvalidDateTimeFormatException();
    }
    //else _rep->copy;
}

CIMDateTime::CIMDateTime(const CIMDateTime& x)
{
    _rep = new CIMDateTimeRep();
   // memcpy(_rep->data, x._rep->data, sizeof(_rep->data));
    _rep->copy(x._rep);
}  

CIMDateTime::CIMDateTime(const Uint64 microSec, Boolean interval)
{                                         
    if (microSec >= PEGASUS_UINT64_LITERAL(315601056000000000) && !interval) { //time stamps must be less then number of micro Seconds in 10,000 years
        String mes = "trying to create a CIMDateTime object (time stamp) greater then the year 10,000";
        throw DateTimeOutOfRangeException(mes);
    } 
    if (microSec >= PEGASUS_UINT64_LITERAL(8640000000000000000) && interval) { //intervals must be less then the number of microseconds in 100 million days
        String mes1 = "trying to create a CIMDateTime object (interval) greater then the year 100 million days";
         throw DateTimeOutOfRangeException(mes1);
    }


    //Set of Strings that hold part parts of datetime    100,000,000
    String year, ye_mo, ye_mo_da, ye_mo_da_ho, ye_mo_da_ho_mn, ye_mo_da_ho_mn_se, final;

    Uint64 day_sub1 = microSec/PEGASUS_UINT64_LITERAL(86400000000);
    Uint64 days_in400 = 146097;

    if (!interval) {
        Uint32 blocks_400 = day_sub1/days_in400;  //calculates the number of 400 year blocks in number
        Uint32 blocks_rem = day_sub1%days_in400;  //clauclates number of day after 400 year blocks are removed
        Uint32 days_rem = blocks_rem;

       //cout << "this is the number of 400 year blocks " << blocks_400 << endl;
       //cout << " this is the number of days after 400 blocks taken out " << days_rem << endl;
       // cout << "this is of days total " << day_sub1 << endl;

        /*While loop computes number of years form number of day
        taking into accoutn leap years
        */
        Uint32 i = 1;
        Uint32 count = 0;
        Uint32 leap_next = 0;

        while (days_rem >= 365) {
            if ((i%4 != 0) || (count%100 == 0)) {
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
               // cout << "leap year" << endl;
            }                                
        }                                                                       

        Uint32 tot_year = (blocks_400 * 400) + count;


        //converting number of years from Uint32 -> String
        char buffer_year [10];
        sprintf(buffer_year, "%04d", tot_year);
        year = String(buffer_year);

     //  cout << " This is after the while loop in consrtutor total years is " << year << endl;


        /*Switch block is used to calculate the the number of months from the number of days 
        left after years are subtracted. 
        */ 

        Uint16 lp = 0;

        /*lp is 0 for non leap years and 1 for leap years
        */
        if ((leap_next == 3) && ((count != 99)||(count != 199)||(count != 299))) { 
            lp = 1;
          //  cout << "months are being calculated for a leap year" << endl;
        }

        char bu_day [5];
        sprintf(bu_day, "%02d", days_rem);
       // cout << "this is the days left " << String(bu_day) << endl; 
       
            
        if (days_rem < (365+lp) && (days_rem >= (334+lp))) {
            ye_mo = year.append(String("12"));
            days_rem = days_rem - (334+lp);
        }
        else if((days_rem < (334+lp)) && (days_rem >= (304+lp))){
            ye_mo = year.append(String("11"));
            days_rem = days_rem - (304+lp);
        }
        else if((days_rem < (304+lp)) && (days_rem >= (273+lp))){
            ye_mo = year.append(String("10"));
            days_rem = days_rem - (273+lp);
        }
        else if((days_rem < (273+lp)) && (days_rem >= (243+lp))){
            ye_mo = year.append(String("09"));
            days_rem = days_rem - (243+lp);
        }
        else if((days_rem < (243+lp)) && (days_rem >= (212+lp))){
            ye_mo = year.append(String("08"));
            days_rem = days_rem - (212+lp);
        }
        else if((days_rem < (212+lp)) && (days_rem >= (181+lp))){
            ye_mo = year.append(String("07"));
            days_rem = days_rem - (181+lp);
        }
        else if((days_rem < (181+lp)) && (days_rem >= (151+lp))){
            ye_mo = year.append(String("06"));
            days_rem = days_rem - (151+lp);
        }
        else if((days_rem < (151+lp)) && (days_rem >= (120+lp))){
            ye_mo = year.append(String("05"));
            days_rem = days_rem - (120+lp);
        }
        else if((days_rem < (120+lp)) && (days_rem >= (90+lp))){
            ye_mo = year.append(String("04"));
            days_rem = days_rem - (90+lp);
        }
        else if((days_rem < (90+lp)) && (days_rem >= (59+lp))){
            ye_mo = year.append(String("03"));
            days_rem = days_rem - (59+lp);
        }
        else if((days_rem < (59+lp)) && (days_rem >= 31)){
            ye_mo = year.append(String("02"));
            days_rem = days_rem - 31;
        }
        else if((days_rem < 31) && (days_rem >= 00)){
            ye_mo = year.append(String("01"));
      //      cout << "this is one month" << endl;
        }
        else{
            cout << "error in 'if else' bolck for months" << endl;
            cout << "days_rem = " << days_rem << endl;
            throw DateTimeOutOfRangeException("problem calculationg months");
        }
       // cout << "when months are add the string is  " << ye_mo << endl;

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
 
  // cout << "when days are added the string is  " << ye_mo_da << endl;

    //get hours, minutes, seconds and microseconds 

    Uint64 after_ymd = microSec%PEGASUS_UINT64_LITERAL(86400000000);

    Uint32 hour_num = after_ymd/PEGASUS_UINT64_LITERAL(3600000000);
    Uint32 after_ymdh = after_ymd%PEGASUS_UINT64_LITERAL(3600000000);

    Uint32 min_num = after_ymdh/60000000;
    Uint32 after_ymdhm = after_ymdh%60000000;

    Uint32 sec_num = after_ymdhm/1000000;

    Uint32 mic_num = after_ymdhm%1000000;



    //converting hours, minutes, seconds and microseconds from Uint32 -> String

    char buffer_hour [10];
    sprintf(buffer_hour, "%02d", hour_num);
    ye_mo_da_ho = ye_mo_da.append(buffer_hour);
        //cout << "when hours are added the string is  " << ye_mo_da_ho << endl;

    char buffer_min [10];
    sprintf(buffer_min, "%02d", min_num);
    ye_mo_da_ho_mn = ye_mo_da_ho.append(buffer_min);
      //  cout << "when minutes are added the string is  " << ye_mo_da_ho_mn << endl;


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
    if (!_set(final)) {
        delete _rep;
        throw InvalidDateTimeFormatException();
    }
 
}

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

String CIMDateTime::toString () const
{
    return String (_rep->data);
}

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

Boolean CIMDateTime::_set(const String & dateTimeStr)
{
    clear();
   // cout << "at the begining of _set" <<endl;

    CString dtStr = dateTimeStr.getCString();
    const char* str = dtStr;

    //cout << "befor first if this is size " << dateTimeStr.size() << endl;

    // Be sure the incoming string is the proper length:

    if (dateTimeStr.size() != CIMDateTimeRep::FORMAT_LENGTH)
	return false;

   //cout << "after first if" << endl;

    // Determine the type (date or interval); examine the 21st character;
    // it must be one of ':' (interval), '+' (date), or '-' (date).

    const Uint32 SIGN_OFFSET = 21;
    const Uint32 DOT_OFFSET = 14;

    const String offS =  dateTimeStr.subString(SIGN_OFFSET,4);

    Boolean isInterval = String::compare(offS, ":000") == 0;

    if (!isInterval && dateTimeStr[SIGN_OFFSET] != '+' && dateTimeStr[SIGN_OFFSET] != '-')
	return false;

   // cout << "after not interval sign check" << endl;

    // Check for the decimal place:

    if (dateTimeStr[DOT_OFFSET] != '.')
	return false;

    //cout << "after decimal point check" << endl;

     // Check to see if other characters are digits or astrisks (*)

    for (Uint32 i = 0; i < CIMDateTimeRep::FORMAT_LENGTH; i++){
	    if (i != DOT_OFFSET && i != SIGN_OFFSET && !isdigit(dateTimeStr[i]) && (String::compare(dateTimeStr.subString(i,1),"*") != 0)){
            cout << " this is the chariter it failed on " <<  dateTimeStr[i] << endl;
            cout << " this is the string " << dateTimeStr << endl;
            cout << " i = " << i << endl;
            return false;
        }
    }
	       

    // Check to see if the month and day are in range (date only):

    String buffer;
    Uint64 ans;
    

    //cout << "right before isInterval check" << endl;
    if (!isInterval)
    {
      //get year
      /* need to check that the field is valid as far as astrisk (*) are concerned */
      buffer = dateTimeStr.subString(0,4);
      ans = fieldcheck(buffer, _rep->year);
      //cout << "year " << _rep->year << endl;
      if (ans == 1){ 
       //   cout << "error (1) was returned from getting year" << endl;
          return false;
      }
      else if (ans == 0) {
          if (!restOfFields(4,dateTimeStr)){
        //      cout << "restOfFeilds said that that object has the wrong formatt" << endl;
              return false;
          }
      }
     
	    // Get the month:
    // cout << "after getting year" << endl;

	//sprintf(buffer, "%2.2s", str + 4);
        buffer = dateTimeStr.subString(4,2);
        ans = fieldcheck(buffer, _rep->month);
        if (ans == 1) {
           // cout << "one was returned form fieldcheck" << endl << endl;
            return false;           // month field has both wildcards and digits
        }

        else if (ans == 2) {          // month field has only digits                         
            long month = atoi(buffer.getCString());

            // Allow for zero month - default value processing
	        if (month == 0 || month > 12)
	            return false;
        }
        else if (ans == 0) {
          if (!restOfFields(6,dateTimeStr)){
            //  cout << "restOfFeilds said that that object has the wrong formatt" << endl;
              return false;
          }
        }


        //cout << "after getting month in _set" << endl;
	    // Get the day:                    

	    //sprintf(buffer, "%2.2s", str + 6);
        buffer = dateTimeStr.subString(6,2);
        ans = fieldcheck(buffer, _rep->days);
        if (ans == 1) {
           // cout << "one was returned form fieldcheck" << endl << endl;
            return false;           // month field has both wildcards and digits
        }

        else if (ans == 2) {          // month field has only digits                         
       
	        long day = atoi(buffer.getCString());

            // Allow for zero day - default value processing
            if (day == 0 || day > 31)
	            return false;
         }
        else if (ans == 0) {
          if (!restOfFields(6,dateTimeStr)){
          //    cout << "restOfFeilds said that that object has the wrong formatt" << endl;
              return false;
          }
        }

      // cout << "after getting days" << endl;


        //get UTC off set   for a Time Stamp
        buffer = dateTimeStr.subString(21,4);
        _rep->utcOffSet = buffer;
        Uint32 spot = buffer.find("*");
        //cout << "before if" << endl;
        if(spot == PEG_NOT_FOUND){  // the UTC must not have astricks in it
        // cout << "no spat found in UTC offset - this is  _rep->utcOffSet" << _rep->utcOffSet << endl;
        }
        else{
           //cout << "wrong UTC Off set - this value " << _rep->utcOffSet << endl;
            return false;
        }

    }
    else{     //Object is an Interval

        //get days if object is an interval
        buffer = dateTimeStr.subString(0,8);
        ans = fieldcheck(buffer, _rep->days);
        if (ans == 1) {
           // cout << "Interval days was set wrong" << endl;
            return false;
        }
        else if (ans == 0) {
          if (!restOfFields(8,dateTimeStr)){
             // cout << "restOfFeilds said that that object has the wrong formatt" << endl;
              return false;
          }
        }


        // check to make sure UTC for Intervals it '000'
        buffer = dateTimeStr.subString(21,4);
        _rep->utcOffSet = buffer;
        if ( !String::compare(_rep->utcOffSet, ":OOO")) {
           // cout << "interval UTC offset is worng" << endl;
            return false;
        }


    }

    //cout << "after is interval block" << endl;

    // Check the hours and minutes:
    buffer = dateTimeStr.subString(8,2);
    ans = fieldcheck(buffer, _rep->hours);
    if (ans == 1) {
        //cout << "one was returned form fieldcheck" << endl << endl;
        return false;           // hour field has both wildcards and digits
     }

    else if (ans == 2) {          // hour field has only digits   
        //sprintf(buffer, "%2.2s", str + 8);
        long hours = atoi(buffer.getCString());

        if (hours > 23)
	        return false;
    }
    else if (ans == 0) {
          if (!restOfFields(10,dateTimeStr)){
              cout << "restOfFeilds said that that object has the wrong formatt" << endl;
              return false;
          }
     }

                               
   //cout << "this is after getting hours" << endl;

    buffer = dateTimeStr.subString(10,2);
    ans = fieldcheck(buffer, _rep->minutes);
    //cout << " this is in _set, minutes = " << _rep->minutes << endl;
    if (ans == 1) {
    //cout << "one was returned form fieldcheck" << endl << endl;
        return false;           // minutes field has both wildcards and digits
    }

    else if (ans == 2) {          // minutes field has only digits  
    //sprintf(buffer, "%2.2s", str + 10);
        long minutes = atoi(buffer.getCString());

        if (minutes > 59)
	        return false;
    }else if (ans == 0) {
          if (!restOfFields(12,dateTimeStr)){
              cout << "restOfFeilds said that that object has the wrong formatt" << endl;
              return false;
          }
      }


                                          

    buffer = dateTimeStr.subString(12,2);
    ans = fieldcheck(buffer, _rep->seconds);
    if (ans == 1) {
        //cout << "one was returned form fieldcheck" << endl << endl;
        return false;           // seconds field has both wildcards and digits
    }

    else if (ans == 2) {          // minutes field has only digits 
    //sprintf(buffer, "%2.2s", str + 12);
        long seconds = atoi(buffer.getCString());

        if (seconds > 59)
	        return false;
    }
    else if (ans == 0) {
          if (!restOfFields(14,dateTimeStr)){
              cout << "restOfFeilds said that that object has the wrong formatt" << endl;
              return false;
          }
      }


    //get micro Seconds
    String buffer_micro = dateTimeStr.subString(15,6);
    //ans = microSecheck(buffer);
    ans = _rep->set_microSec(buffer_micro);
    if (ans == 1) {
        return false;
    }

         
        
    memcpy(_rep->data, str, sizeof(_rep->data));
        //   cout << "this is the end of _set " << endl;
    return true;

}

Uint64 CIMDateTime::microSecheck(String & in_p)
{
    Uint32 ast_post;
    _rep->microSec = in_p;
 
    ast_post = in_p.find("*");
    //cout <<"this is the try block inside fieldcheck" << endl;
    if (ast_post == PEG_NOT_FOUND) {
    //cout << "check to see if it equals PEG_NOT_FOUND in microSecheck" << endl;
        return 2;
    }
    Uint32 sub_len = 6 - ast_post;
    String ast = "******";
    String in_comp = in_p.subString(ast_post, sub_len);
    if (!String::compare(in_comp, ast, sub_len)){
        return 0;
    }
    else{
        cout << "error in microSe - this is the value " << in_p << endl;
        return 1;
    }

}


Boolean CIMDateTime::restOfFields(Uint32 start_position,const String inStr)
{
   // cout << "begining of rest of fields" << endl;
    String splatCDT = "**************.******";
    Uint32 placeNum = splatCDT.size() - start_position;

    String comp = splatCDT.subString(start_position, placeNum);
    String in_comp = inStr.subString(start_position, placeNum);
    if (String::compare(comp, in_comp))
        return false;
    else
        return true;
}

void CIMDateTime::set(const String & str)
{
    if (!_set(str))
	    throw InvalidDateTimeFormatException();
}

Uint64 CIMDateTime::toMicroSeconds()
{
    
    
    CIMDateTime un_norm;
    un_norm._rep = new CIMDateTimeRep();
    //CIMDateTimeRep& ref_rep = _rep;
    un_norm._rep->copy(_rep);
    CIMDateTime norm = un_norm.normalizer();
    //cout << "after normalizer call this is norm " << norm.toString() << endl;

    const Uint64 norm_micSec = norm._toMicroSeconds();
    // cout << "this is what the object holds year - " << norm._rep->year << " months - " << norm._rep->month << " - days  " << norm._rep->days << endl << "hour - " << norm._rep->hours << endl;

    
    //cout << "after _toMicroSeconds call in toMicroSecond" << endl;
    return (norm_micSec);
}

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

    Uint32 mic_sp= _rep->microSec.find('*');
    if (mic_sp == PEG_NOT_FOUND) {
        mic = atol(_rep->microSec.getCString());
    }
    else if (mic_sp > 0) {
        String subMic = _rep->microSec.subString(0, mic_sp);
        mic = atol(subMic.getCString()) * pow((double)10,(double)(6-mic_sp));
        //cout << " the multiplier is " << pow(10,(6-mic_sp)) << endl;
        //cout << "mic_sp = " << mic_sp << " and mic = " << mic << endl;
    }
    else{
        mic = 0;
    }
    
    //cout << "millis sec = " << (Uint32)mic << endl;

    if (_rep->seconds.find('*') == PEG_NOT_FOUND) {
        sec = atol(_rep->seconds.getCString()) * 1000000;
        //cout << "sec = " << (Uint32)(sec/1000000) << endl; 
       //cout << " this is what getCString gives " << atol(_rep->seconds.getCString()) << endl;

    }

    if (_rep->minutes.find('*') == PEG_NOT_FOUND) {  
       // cout << "this is min before is gets set - " << (Uint32)(min/1000000) << endl;
        min = atol(_rep->minutes.getCString()) * PEGASUS_UINT64_LITERAL(60000000);
       // cout << "min = " << (Uint32)(min/1000000) << endl; 
      // cout << " this is what getCString gives " << atol(_rep->minutes.getCString()) << endl;
    }
    
    if (_rep->hours.find('*') == PEG_NOT_FOUND) {
        hor = (atol(_rep->hours.getCString())) * PEGASUS_UINT64_LITERAL(3600000000);
        //cout << "hour = " << (Uint32)(hor/1000000) << endl; 
      // cout << " this is what getCString gives " << atol(_rep->hours.getCString()) << endl;

    }
         

 // cout << "after setting values in toMicroSeconds() " << endl;
 //cout << "this is what the object holds year - " << _rep->year << " months - " << _rep->month << " - days  " << _rep->days << endl << "hour - " << _rep->hours << endl;
 //cout << "minute - " << _rep->minutes << endl;
   

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

        Uint64 count_le = 0;
        Uint64 count_r = 0;
        Uint64 yea_rem = yea_num%400;
        Uint32 leap_next = 0;
        Uint32 count = 0;
        Uint16 lp = 0;

        //cout << "right before for loop for the leap year stuff" << endl;

        for (int i=1; i<=yea_rem; i++) {
            if ((i%4 != 0) || (count%100 == 0)) {
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
        if ((leap_next == 3) && ((count != 99)||(count != 199)||(count != 299))) { 
            lp = 1;
          //  cout << "months are being calculated for a leap year" << endl;
        }
        if (_rep->month.find('*') == PEG_NOT_FOUND) {
 
            //get number of days eqivalent to number of months in the object and multipy by number of
            //micro seconds in a day
            switch (atol(_rep->month.getCString())) { //months can't be equal to zero
            case 1:
                mon = 00;
                break;
            case 2:
                mon = 31 * PEGASUS_UINT64_LITERAL(86400000000);
                break;
            case 3:
                mon = ((59+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            case 4:
                mon = ((90+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            case 5:
                mon = ((120+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            case 6:
                mon = ((151+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            case 7:
                mon = ((181+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            case 8:
                mon = ((212+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            case 9: 
                mon = ((243+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            case 10:
                mon = ((273+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            case 11:
                mon = ((304+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            case 12:
                mon = ((334+lp) * PEGASUS_UINT64_LITERAL(86400000000));
                break;
            default:
                cout << "error calculating months" << endl;
                cout << "this is data " << (String)_rep->data << endl;
                throw InvalidDateTimeFormatException();
            }
        }

       // cout << "after switch statment in toMicroSeconds" << endl;

        
   /* char buf5 [10];
     sprintf(buf5, "%04d", count);

     char buf7 [10];
     sprintf(buf7, "%04d", count_le);

     char buf8 [10];
     sprintf(buf8, "%04d", count_r);  

    cout << "this is the counts - count-" << buf5 << " count_le-" << buf7  << " count_r-" << buf8 << endl;
      */                                                      
         
     
        Uint64 day_rem = (count_le * 366) + (count_r * 365);

        yea = (day_rem + day_400) * PEGASUS_UINT64_LITERAL(86400000000);
        
        if (_rep->days.find('*') == PEG_NOT_FOUND) {
            day = ((atol(_rep->days.getCString()))-1) * PEGASUS_UINT64_LITERAL(86400000000);   //time stamp "days" go from 1-31 ther is no zero
                //  cout << "this is day " << ((atol(_rep->days.getCString()))-1) << endl;
        }
    }
    else{
        if (_rep->days.find('*') == PEG_NOT_FOUND) {
            day = (atol(_rep->days.getCString())) * PEGASUS_UINT64_LITERAL(86400000000);
        }
    }
    

   /* Uint32 y = yea/PEGASUS_UINT64_LITERAL(1000000000);
    Uint32 y_r = yea%PEGASUS_UINT64_LITERAL(1000000000);

    Uint32 m = mon/PEGASUS_UINT64_LITERAL(1000000000);
    Uint32 m_r = mon%PEGASUS_UINT64_LITERAL(1000000000);

    Uint32 d = day/PEGASUS_UINT64_LITERAL(1000000000);
    Uint32 d_r = day%PEGASUS_UINT64_LITERAL(1000000000);

    Uint32 h = hor/PEGASUS_UINT64_LITERAL(1000000000);
    Uint32 h_r = hor%PEGASUS_UINT64_LITERAL(1000000000);

    Uint32 mi = min/PEGASUS_UINT64_LITERAL(1000000000);
    Uint32 mi_r = min%PEGASUS_UINT64_LITERAL(1000000000);

    Uint32 s = sec/PEGASUS_UINT64_LITERAL(1000000000);
    Uint32 s_r = sec%PEGASUS_UINT64_LITERAL(1000000000);    */



    date = mic+sec+min+hor+day+mon+yea; 

    /*if (date > PEGASUS_UINT64_LITERAL(315601056000000000)) {
        cout << "year " << y << " remainder " << y_r << endl;
        cout << "mon " << m << " remain " << m_r << endl;
        cout << "day " << d << " remai  " << d_r << endl;
        cout << "hour " << h << " remain " << h_r << endl;
        cout << "min " << mi << " reain " << mi_r << endl;
        cout << "sec " << s << " remain " << s_r << endl;
        cout << "date  " << (Uint32)(date/PEGASUS_UINT64_LITERAL(1000000000)) << endl;                                  

        cout << this->toString() << endl;
    }     */

    return date;
}

Boolean operator==(const CIMDateTime& x, const CIMDateTime& y)
{
    return x.equal (y);
}

CIMDateTime CIMDateTime::normalizer()
{
    if (isInterval()) {
        String copy_Str = String((this->_rep)->data);
        CIMDateTime current = CIMDateTime(copy_Str);
        //cout << "this is what comes from the normalizer if it's an interval " << current.toString() << endl;
        return(current);    //no nomalization should not be done on Intervals
    }
    Uint64 normNum = 0;

    //cout << "before toMicroSecond call in normakizer " << endl;
    Uint64 un_normNum = this->_toMicroSeconds();
    Uint32 unnor = un_normNum/PEGASUS_UINT64_LITERAL(1000000000);
    Uint32 runnor = un_normNum%PEGASUS_UINT64_LITERAL(1000000000);
   // cout << "this is what comes back from _toMicroSeconds in normalizer div by a billion "<< unnor << endl;
    //cout << "this is reainder " << runnor << endl;

    // get UTC offSet and change it in microseconds
    String utcOS = _rep->utcOffSet.subString(1,3);
    Uint32 offSet = atol((utcOS).getCString());
    Uint64 offSet_hor = (offSet/60) * PEGASUS_UINT64_LITERAL(3600000000);
    Uint64 offSet_min = (offSet%60) * 60000000;
    String mesO = "overflow has occured in nomalization";

    
    //cout << "UTC offset equals " << offSet << endl;

    char 		sign;   // Get the sign and UTC offset.
    sign = _rep->data[21];

    //if there are no wild cards in the minute postion then the entire utc offSet
    //will effect the CIMDateTime value

    if (_rep->minutes.find('*') == PEG_NOT_FOUND) {
        if ( sign == '-' ) {  
            if (PEGASUS_UINT64_LITERAL(315601056000000000) < (un_normNum + (offSet_hor + offSet_min))) 
                throw DateTimeOutOfRangeException(mesO);
            normNum = un_normNum + (offSet_hor + offSet_min);
        }
        else{
            if (un_normNum < (offSet_hor + offSet_min)) {
                String mes3 = "underflow has occured in nomalization";
                throw DateTimeOutOfRangeException(mes3);
            }
             normNum = un_normNum - (offSet_hor + offSet_min);
        }
    }
    //if the hours section has no wild cards but the minutes section does then only on hour
    //position will be effected by the uct off Set
    else if (_rep->hours.find('*') == PEG_NOT_FOUND) {
       // cout << "in only update hour block and sign is " << sign << endl;
        if ( sign == '-' ) {     
            if (PEGASUS_UINT64_LITERAL(315601056000000000) < (un_normNum + (offSet_hor))) 
                throw DateTimeOutOfRangeException(mesO);

             normNum = un_normNum + (offSet_hor);
        }
        else{
            if (un_normNum < (offSet_hor)) {
                String mes3 = "underflow has occured in nomalization";
                throw DateTimeOutOfRangeException(mes3);
            }
             normNum = un_normNum - (offSet_hor);
            // cout << "in only hour offset" << endl;
        }
    }
    else{ //if this block is executed then the utc offSet has no effect on CIMDateTime value
        normNum = un_normNum;
    }
   Uint32 prnor = normNum/1000000;
   //cout << "this is the sum of the number and the offset " << prnor << endl;

  /* if (PEGASUS_UINT64_LITERAL(315601056000000000) < normNum) {
       cout << "here is the problem " << endl;
       cout << "CDT " << this->toString() << endl;
   }  */

    //cout << "the sign is " << sign << endl;

    CIMDateTime norm_CDT = CIMDateTime(normNum,false);
    //cout << "after constructor call in normalizer" << endl;
   
    return(norm_CDT);
}

void formatDateTime(char* dateTimeStr, tm* tm)
{
    Uint32 index = 0, index1 = 0;
    long   year = 0;
    char   buffer[16];

    // Initialize the components of tm structure
    tm->tm_year = 0;
    tm->tm_mon  = 0;
    tm->tm_mday = 0;
    tm->tm_hour = 0;
    tm->tm_min  = 0;
    tm->tm_sec  = 0;
    tm->tm_isdst = 0;
    tm->tm_wday  = 0;
    tm->tm_yday  = 0;

    // Extract the year.
    sprintf(buffer, "%4.4s", dateTimeStr);
    year   = atoi(buffer);
    year = year - 1900;
    tm->tm_year   = year;

    // Extract the Month.
    sprintf(buffer, "%2.2s", dateTimeStr + 4);
    tm->tm_mon   = atoi(buffer);

    // Extract the Day.
    sprintf(buffer, "%2.2s", dateTimeStr + 6);
    tm->tm_mday   = atoi(buffer);

    // Extract the Hour.
    sprintf(buffer, "%2.2s", dateTimeStr + 8);
    tm->tm_hour   = atoi(buffer);

    // Extract the Minutes.
    sprintf(buffer, "%2.2s", dateTimeStr + 10);
    tm->tm_min   = atoi(buffer);

    // Extract the Seconds.
    sprintf(buffer, "%2.2s", dateTimeStr + 12);
    tm->tm_sec   = atoi(buffer);
}

Boolean CIMDateTime::isInterval() const
{
    const Uint32 	SIGN_OFFSET = 21;

    Boolean isInterval = strcmp(&_rep->data[SIGN_OFFSET], ":000") == 0 ;
    
    return isInterval;
}

Boolean CIMDateTime::equal (const CIMDateTime & x) const
{
    if ((x.isInterval() && !this->isInterval()) || (!x.isInterval() && this->isInterval())) {
        throw TypeMismatchException(); 
    }

    CIMDateTime current = CIMDateTime((String)_rep->data);
    CIMDateTime compare = CIMDateTime((String)x._rep->data);  // not sure why all this is needed but const has somthing to do with it
   
    Uint32 spl_pos = current.getHighestWildCardPosition(compare);

    CIMDateTime cur = current.insert_WildCard(spl_pos);
    CIMDateTime comp = compare.insert_WildCard(spl_pos);


   // Uint32 spot_cur = current.toString().find('*');
    //Uint32 spot_comp = compare.toString().find('*');
    /*char bcur [3];
    char bcom [3];
    sprintf(bcur,"%02d",spot_cur);
    cout << "spot_cur is " << bcur << endl;
    sprintf(bcom,"%02d",spot_comp);
    cout << "spot_comp is " << bcom << endl;*/

    /*    take this section out when sub rutiens are intergrated

    if (spot_cur == PEG_NOT_FOUND && spot_comp == PEG_NOT_FOUND) {   //start time have more wild cards then finish time 
        cur = current;
        comp = compare; 
      //  cout << "not splats" << endl;
    }
    else if (spot_cur == PEG_NOT_FOUND) {
        cur = current.insert_WildCard(spot_comp);
        comp = compare;
       // cout << "splats only in comp" << endl;
      //  cout << "cur " << cur.toString() << endl;
      //  cout << "comp " << comp.toString() << endl;
    }
    else if (spot_comp == PEG_NOT_FOUND) {
        comp = compare.insert_WildCard(spot_cur);
        cur = current;
      //   cout << "splats only in cur" << endl;
    }        
    else{
        if (spot_comp < spot_cur) {
            cur = current.insert_WildCard(spot_comp);
            comp = compare;
           /*  cout << "comp has more splats" << endl;
             cout << "cur " << cur.toString() << endl;
        cout << "comp " << comp.toString() << endl;       

        }
        else{
            comp = compare.insert_WildCard(spot_cur);
            cur = current;
      /*     cout << "cut has more splats" << endl;
            cout << "cur " << cur.toString() << endl;
        cout << "comp " << comp.toString() << endl;   
        }
    } */

    
    if (cur.toMicroSeconds() == comp.toMicroSeconds()) {
        return true;
    }
    else   
        return false;  
    
}

Sint64 CIMDateTime::getDifference(const CIMDateTime & startTime, const CIMDateTime & finishTime)
{
    CIMDateTime sta = startTime;
    CIMDateTime fin = finishTime;
    CIMDateTime startT;
    CIMDateTime finishT;
    CIMDateTime sta_norm;
    CIMDateTime fin_norm;
    Uint64 startT_num;
    Uint64 finishT_num;
    Sint64 diff_num;


    Uint32 splat_pos = sta.getHighestWildCardPosition(fin);
    startT = sta.insert_WildCard(splat_pos);
    finishT = fin.insert_WildCard(splat_pos);


   /*
    
    Uint32 spot_s = startTime.toString().find('*');
    Uint32 spot_f = finishTime.toString().find('*');

    
    if (spot_s == PEG_NOT_FOUND && spot_f == PEG_NOT_FOUND) {   //start time have more wild cards then finish time 
        startT = sta;
        finishT = fin; 
       // cout << "not splats" << endl;
    }
    else if (spot_f == PEG_NOT_FOUND) {
        finishT = fin.insert_WildCard(spot_s);
        startT = sta;
       // cout << "splats only in finish" << endl;
    }
    else if (spot_s == PEG_NOT_FOUND) {
        startT = sta.insert_WildCard(spot_f);
        finishT = fin;
         //cout << "splats only in cur" << endl;
    }        
    else{
        if (spot_f < spot_s) {
            startT = sta.insert_WildCard(spot_f);
            finishT = fin;
            // cout << "comp has more splats" << endl;
        }
        else{
            finishT = fin.insert_WildCard(spot_s);
            startT = sta;
            //cout << "cut has more splats" << endl;
        }
    }     */

  /* this block doesn't seem to be needed
    // normalize objects if they are timestamps
    if (!startT.isInterval()) {
        sta_norm = startT.normalizer();
    }
    if (!finishT.isInterval()) {
        fin_norm = finishT.normalizer();
    }
       */
      

   // cout << "this is startT._rep->month " << startT._rep->month << " this is startTime._rep->month " << startTime._rep->month << endl;


    //cout << "this is statrt norm " << startT_norm.toString() << endl;
    //cout << "this is finish norm " << finishT_norm.toString() << endl;


    startT_num = startT.toMicroSeconds();
    //cout << "after first toMicroSec call" << endl;
    finishT_num = finishT.toMicroSeconds();

    //cout << "this is start in MicroSec divided by a million " << (Uint32)startT_num/1000000 << endl;
    //cout << "this is finnish in MicroSec divided by a million " << (Uint32)finishT_num/1000000 << endl;



    /* the throwing of this expceptoin is only needed becasue this is the way getDifferance worked
    before. The operator- does not behave this way.
    */
    if ((startT.isInterval() && ! finishT.isInterval()) || (! startT.isInterval() &&  finishT.isInterval())) {
        throw InvalidDateTimeFormatException(); 
    }

    diff_num = finishT_num - startT_num;

    return diff_num;



   /* const char*         startDateTimeCString;
    const char*         finishDateTimeCString;
    char                dateTimeOnly[CIMDateTimeRep::FORMATTED_DATE_TIME];
    struct tm           tmvalStart;
    struct tm           tmvalFinish;
    Sint64              differenceInSeconds = 0;
    time_t		timeStartInSeconds;
    time_t 		timeFinishInSeconds;
    char 		sign;
    Uint32 		offset;
    char 		buffer[4];

    //
    // Get the dates in CString form
    //
    startDateTimeCString = startTime._rep->data;
    finishDateTimeCString = finishTime._rep->data;

    //
    // Check if the startTime or finishTime are intervals
    //
    if (startTime.isInterval() && finishTime.isInterval())
    {
        char 		intervalBuffer[9];
        //
        //  NOTE: although a Uint64 is not required to hold the maximum
        //  value for these variables, if they are not defined as Uint64s,
        //  overflow/truncation can occur during the calculation of the
        //  number of microseconds, and the final result may be incorrect
        //
	Uint64		startIntervalDays;
	Uint64		startIntervalHours;
	Uint64 		startIntervalMinutes;
	Uint64		startIntervalSeconds; 
	Uint64		startIntervalMicroseconds; 
	Uint64		finishIntervalDays;
	Uint64		finishIntervalHours;
	Uint64 		finishIntervalMinutes;
	Uint64		finishIntervalSeconds; 
	Uint64		finishIntervalMicroseconds; 
	Uint64		startIntervalInMicroseconds; 
	Uint64		finishIntervalInMicroseconds; 
	Sint64		intervalDifferenceInMicroseconds; 

        // Parse the start time interval and get the days, minutes, hours
        // and seconds

        // Extract the days.
        sprintf(intervalBuffer, "%8.8s", startDateTimeCString);
        startIntervalDays   = atoi(intervalBuffer);

        // Extract the Hour.
        sprintf(intervalBuffer, "%2.2s", startDateTimeCString + 8);
        startIntervalHours   = atoi(intervalBuffer);

        // Extract the Minutes.
        sprintf(intervalBuffer, "%2.2s", startDateTimeCString + 10);
        startIntervalMinutes = atoi(intervalBuffer);

        // Extract the Seconds.
        sprintf(intervalBuffer, "%2.2s", startDateTimeCString + 12);
        startIntervalSeconds = atoi(intervalBuffer);

        // Extract the Microseconds
        sprintf(intervalBuffer, "%6.6s", startDateTimeCString + 15);
        startIntervalMicroseconds = atoi(intervalBuffer);

        // Parse the finish time interval and get the days, minutes, hours
        // and seconds

        // Extract the days.
        sprintf(intervalBuffer, "%8.8s", finishDateTimeCString);
        finishIntervalDays   = atoi(intervalBuffer);

        // Extract the Hour.
        sprintf(intervalBuffer, "%2.2s", finishDateTimeCString + 8);
        finishIntervalHours   = atoi(intervalBuffer);

        // Extract the Minutes.
        sprintf(intervalBuffer, "%2.2s", finishDateTimeCString + 10);
        finishIntervalMinutes = atoi(intervalBuffer);

        // Extract the Seconds.
        sprintf(intervalBuffer, "%2.2s", finishDateTimeCString + 12);
        finishIntervalSeconds = atoi(intervalBuffer);

        // Extract the Microseconds
        sprintf(intervalBuffer, "%6.6s", finishDateTimeCString + 15);
        finishIntervalMicroseconds = atoi(intervalBuffer);

        // Convert all values to seconds and compute the start and finish
        // intervals in seconds.
        startIntervalInMicroseconds = 
            (startIntervalDays*PEGASUS_UINT64_LITERAL(86400000000)) +
            (startIntervalHours*PEGASUS_UINT64_LITERAL(3600000000)) +
            (startIntervalMinutes*60000000) +
            (startIntervalSeconds*1000000) +
             startIntervalMicroseconds;

        finishIntervalInMicroseconds = 
            (finishIntervalDays*PEGASUS_UINT64_LITERAL(86400000000)) +
            (finishIntervalHours*PEGASUS_UINT64_LITERAL(3600000000)) +
            (finishIntervalMinutes*60000000) + 
            (finishIntervalSeconds*1000000) +
             finishIntervalMicroseconds;

        // Get the difference.
        intervalDifferenceInMicroseconds =
            (Sint64)(finishIntervalInMicroseconds -
                     startIntervalInMicroseconds);

        return intervalDifferenceInMicroseconds;
    }
    else if ( startTime.isInterval() || finishTime.isInterval() )
    {
        // ATTN-RK-20020815: Wrong exception to throw.
        throw InvalidDateTimeFormatException();
    }

    //
    // Copy only the Start date and time in to the dateTimeOnly string
    //
    strncpy( dateTimeOnly, startDateTimeCString, CIMDateTimeRep::DATE_TIME_LENGTH );
    dateTimeOnly[CIMDateTimeRep::DATE_TIME_LENGTH] = 0;
    formatDateTime(dateTimeOnly ,&tmvalStart);

    //
    // Copy only the Finish date and time in to the dateTimeOnly string
    //
    strncpy( dateTimeOnly, finishDateTimeCString, CIMDateTimeRep::DATE_TIME_LENGTH );
    dateTimeOnly[CIMDateTimeRep::DATE_TIME_LENGTH] = 0;
    formatDateTime( dateTimeOnly, &tmvalFinish );

    // Convert local time to seconds since the epoch
    timeStartInSeconds  = mktime(&tmvalStart);
  
    // Check if the date is within the supported range of mktime.
    // If not return an error. Unix mktime sets errno to ERANGE. 
    // Check for both return code from mktime as well as errno. 
    // In case of Windows errno is not set, only check for return code 
    // from mktime for Windows platform.
#ifdef PEGASUS_OS_HPUX
    if ( timeStartInSeconds == (time_t)-1 && errno == ERANGE)
#else
    if ( timeStartInSeconds == (time_t)-1 )
#endif
    {
        throw DateTimeOutOfRangeException(startTime.toString());
    }
    
    timeFinishInSeconds = mktime(&tmvalFinish);
  
    // Check if the date is within the supported range of mktime.
    // If not return an error. Unix mktime sets errno to ERANGE. 
    // Check for both return code from mktime as well as errno. 
    // In case of Windows errno is not set, only check for return code 
    // from mktime for Windows platform.
#ifdef PEGASUS_OS_HPUX
    if ( timeFinishInSeconds == (time_t)-1 && errno == ERANGE)
#else
    if ( timeFinishInSeconds == (time_t)-1 )
#endif
    {
        throw DateTimeOutOfRangeException(finishTime.toString());
    }
    
    // Convert start time to UTC
    // Get the sign and UTC offset.
    sign = startDateTimeCString[21];
    sprintf(buffer, "%3.3s",  startDateTimeCString + 22);
    offset = atoi(buffer);

    if ( sign == '+' )
    {
        // Convert the offset from minutes to seconds and subtract it from
        // Start time
        timeStartInSeconds = timeStartInSeconds - ( offset * 60 );
    }
    else
    {
        // Convert the offset from minutes to seconds and add it to
        // Start time
        timeStartInSeconds = timeStartInSeconds + (offset * 60);
    }

    // Convert finish time to UTC
    // Get the sign and UTC offset.
    sign = finishDateTimeCString[21];
    sprintf(buffer, "%3.3s",  finishDateTimeCString + 22);
    offset = atoi(buffer);

    if ( sign == '+' )
    {
        // Convert the offset from minutes to seconds and subtract it from
        // finish time
        timeFinishInSeconds = timeFinishInSeconds - ( offset * 60 );
    }
    else
    {
        // Convert the offset from minutes to seconds and add it to
        // finish time
        timeFinishInSeconds = timeFinishInSeconds + (offset * 60);
    }

    //
    // Get the difference between the two times
    //
    differenceInSeconds = (Sint64) difftime( timeFinishInSeconds, timeStartInSeconds );

    //
    //  ATTN-CAKG-P1-20020816: struct tm and difftime don't handle microseconds
    //
    Sint64 differenceInMicroseconds = differenceInSeconds * 1000000;
    startDateTimeCString = startTime._rep->data;
    finishDateTimeCString = finishTime._rep->data;
    char dateBuffer [9];
    sprintf (dateBuffer, "%6.6s", startDateTimeCString + 15);
    Uint32 startDateMicroseconds = atoi (dateBuffer);
    sprintf (dateBuffer, "%6.6s", finishDateTimeCString + 15);
    Uint32 finishDateMicroseconds = atoi (dateBuffer);
    (finishDateMicroseconds > startDateMicroseconds) ?
        differenceInMicroseconds +=
            (finishDateMicroseconds - startDateMicroseconds) :
        differenceInMicroseconds -=
            (startDateMicroseconds - finishDateMicroseconds);

    return differenceInMicroseconds;  */
    Uint64 dif;
    return dif;
}
    
Uint64 CIMDateTime::fieldcheck(String & in_p,String & rep_field)
{
    Uint32 post;
    rep_field = in_p;
 
    post = in_p.find("*");
    //cout <<"this is the try block inside fieldcheck" << endl;
    if (post == PEG_NOT_FOUND) {
    //cout << "check to see if it equals PEG_NOT_FOUND" << endl;
        return 2;
    }

    const String ast = "**********";
    String comp = String(ast, in_p.size());   //creates a string of asteriks with the same length as in_p
    if (!String::compare(in_p, comp)) {
       // cout << "this should be good" << endl;
        return 0;                            //fields is all astriks
    }
    else{
        //cout << "this will cause an error" << endl;
       // cout << "this is in_p " << in_p << endl;
        return 1;                            //error - mix of asterisk and numbers  in field
    }

}
/*
PEGASUS_UINT64_LITERAL(86400000000);
  PEGASUS_UINT64_LITERAL(3600 000 000);
        60 000 000; */


CIMDateTime CIMDateTime::setUctOffSet(Sint32 utc)
{ 
    // convert CIMDateTime to microseconds.   
    Uint64 cdt_MicroSec = this->toMicroSeconds();
    Uint32 offSet = abs(utc);                                
    Uint64 offSet_hor = (offSet/60) * PEGASUS_UINT64_LITERAL(3600000000);
    Uint64 offSet_min = (offSet%60) * 60000000;
    Uint64 cdt_MicroSecSum = 0;          
    String sgn_offset;

    //Uint32 diva = cdt_MicroSec/1000000;
    //cout << "this is cdt_MicroSec divided million " << diva << endl;
    
    //Add (if utc is - ) or subtract (if utc is +) utc to/from DateTime.         
    if (utc >= 0) {
        if (cdt_MicroSec < (offSet_hor + offSet_min)) {
            String mes3 = "underflow has occured in nomalization";
            cout << " underflow error" << endl;
            throw DateTimeOutOfRangeException(mes3);
        }
        cdt_MicroSecSum = cdt_MicroSec + (offSet_hor + offSet_min);
        sgn_offset = "+";
      //  cout << "utc was posotive, it is  " << (offSet/60) << " and " << (offSet%60) <<  endl;
    }
    else{
        cdt_MicroSecSum = cdt_MicroSec - (offSet_hor + offSet_min);
        sgn_offset = "-";
       //  cout << "utc was negative it is  " << (offSet/60) << " and " << (offSet%60) << endl;
    }
    //cout << "after if in setUctOffSet" << endl;
    
     //Uint32 div = cdt_MicroSecSum/1000000;

     //cout << "this is cdt_MicroSecSum div by million " << div << endl;
    
    //Create new DateTime from sum of old DateTime and UTC and set UCT of new Date time. 
    CIMDateTime ans = CIMDateTime(cdt_MicroSecSum, false);

  // cout << " after constructor call in setUctOffSet this is it " << ans.toString() << endl;
    char utcBuff [5];
    sprintf(utcBuff, "%03d", offSet);
    String utc_str = sgn_offset.append(String(utcBuff));
   //cout << "in setUctOffSet this is utc offset " << utc_str << endl;
    Boolean res = ans._rep->set_utcOffSet(utc_str);

   //cout << "right before retrun in setUctOffSet - this is ans.UTC " << ans._rep->utcOffSet << endl;
    if (res) {
        return ans;   //  set_utcOffSet worked 
    }
    else
        throw InvalidDateTimeFormatException();
    

}

Uint32 CIMDateTime::getHighestWildCardPosition(CIMDateTime & cDT_s)
{
    //char bu [3];
    //sprintf(bu,"%02d",index);
    //cout << "index is " << bu << endl;

       
    Uint32 spot_s = cDT_s.toString().find('*'); //since this return a Uint32 and PEG_NOT_FOUND=-1 can't do a
    Uint32 spot_f = this->toString().find('*');  // straight compare

    
    if (spot_s == PEG_NOT_FOUND && spot_f == PEG_NOT_FOUND) {   //start time have more wild cards then finish time 
       // startT = sta;
       // finishT = fin; 
       //cout << "no splats" << endl;
       return PEG_NOT_FOUND;
    }
    else if (spot_f == PEG_NOT_FOUND) {
       // finishT = fin.insert_WildCard(spot_s);
       // startT = sta;
       //  cout << "splats only in second CDT" << endl;
         return spot_s;
    }
    else if (spot_s == PEG_NOT_FOUND) {
        //startT = sta.insert_WildCard(spot_f);
        //finishT = fin;
        // cout << "splats only in first CDT" << endl;
         return spot_f;
    }        
    else{
        if (spot_f < spot_s) {
            //startT = sta.insert_WildCard(spot_f);
            //finishT = fin;
            //cout << "comp has more splats" << endl;
            return spot_f;
        }
        else{
            //finishT = fin.insert_WildCard(spot_s);
            //startT = sta;
            //cout << "cut has more splats" << endl;
            return spot_s;
        }
    }

 }


CIMDateTime CIMDateTime::insert_WildCard(Uint32 ind)
{
    Uint32 index = ind;

    if (ind > 20) {
       index = 21;
    }

    Uint32 spot = this->toString().find('*');
    if (spot == index) {
        CIMDateTime cur = CIMDateTime(this->toString());
        return cur;
    }
    
    String splat = String("**************.******");
    String cdtStr = this->toString();
    String final;
    if (index > 0) {                                    
        String str_cdtStr = cdtStr.subString(0, index);
        String sub_splat = splat.subString(index, (21-index));
  
    
        //build result
        String cdt_Splat = str_cdtStr.append(sub_splat);
        //cout << "part of final string " << cdt_Splat << endl;
        final = cdt_Splat.append(this->_rep->utcOffSet);
    
       //cout << "this is the string sent to the built in insert_Wild " << final << endl;
   }
   else{
        final = splat.append(this->_rep->utcOffSet);
        //cout << "this is the string sent to the constructor,  built in insert_Wild " << final << endl;

   }

   CIMDateTime ans = CIMDateTime(final);
    //cout << "this is the CIMDateTime returned " << res.toString() << endl;

    return ans;
 
}

const CIMDateTime CIMDateTime::operator+(const CIMDateTime & cDT)
{
    CIMDateTime opt_spl;
    CIMDateTime cur_spl;
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;
    CIMDateTime mid_cdt;
    CIMDateTime fin_cdt;
    Sint32 utc;
    Boolean isInt = this->isInterval(); 

    // only interval+interval and timeStamp+interval are allowed. Therefor second operand must be interval
    if (!opt_cDT.isInterval()) {
        throw TypeMismatchException(); 
    }

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);
    //opt_spl = opt_cDT.insert_WildCard(splat_pos);
    //cur_spl = cur_cDT.insert_WildCard(splat_pos);


    //cout << " before microSecond calls" << endl;
    Uint64 opt_num = opt_cDT.toMicroSeconds();
    //cout << "after first micro sec call in operator+" << endl;
    Uint64 cur_num = cur_cDT.toMicroSeconds();
   // cout << "after seocnd micro sec call in operator+" << endl;


    Uint64 ans = opt_num + cur_num;
    CIMDateTime ans_cdt = CIMDateTime(ans, isInt);

    //cout << "after constructor call in operator+ " << endl;
   
    if (!isInt) {
        //cout << "this is the utc offSet of the answer << _rep->utcoffSet << endl;
        utc = atol((_rep->utcOffSet).getCString());
        mid_cdt = ans_cdt.setUctOffSet(utc);
    }
    else
        mid_cdt = ans_cdt;

    fin_cdt = mid_cdt.insert_WildCard(splat_pos);



    return fin_cdt;
}


void CIMDateTime::operator+=(const CIMDateTime & cDT)
{
    CIMDateTime sum_interval;
    CIMDateTime sum2_cdt;
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;
    CIMDateTime sum_cdt = cur_cDT + opt_cDT;
    
    
    /*//cout << " this is opt1 time 2 " << sum_cdt.toString() << endl;
    if (!sum_cdt.isInterval()) {
        sum_interval = CIMDateTime(sum_cdt.toMicroSeconds(), true);
        sum2_cdt = cur_cDT + sum_interval;
    }
    else
    sum2_cdt = cur_cDT + sum_cdt;

    //cout << "this is sum2_cdt " << sum2_cdt.toString() << endl;  */

    _rep->copy(sum_cdt._rep);
     
    //cout << " this is final value " << this->toString() << endl;
     return;
}


const CIMDateTime CIMDateTime::operator-(const CIMDateTime & cDT)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;
    CIMDateTime ans_cdt;
    CIMDateTime fin_cdt;
    Sint32 utc;
    Boolean cur_isIn = this->isInterval();
    Boolean opt_isIn = opt_cDT.isInterval();

    // only I-I, T-I and T-T are allowed
    if (cur_isIn && !opt_isIn) {
        throw TypeMismatchException(); 
    }

    Uint64 opt_num = opt_cDT.toMicroSeconds();
    //cout << "after first micro sec call in operator+" << endl;
    Uint64 cur_num = cur_cDT.toMicroSeconds();

    if (cur_num < opt_num) {
        DateTimeOutOfRangeException("result of date time subtraction is negative");
    }

    Uint64 diff = cur_num - opt_num;

    if ((cur_isIn && opt_isIn) || (!cur_isIn && !opt_isIn)) { //don't konw how to do logical xor
        ans_cdt = CIMDateTime(diff, true);
    }
    else{
        ans_cdt = CIMDateTime(diff, false);
        utc = atol((_rep->utcOffSet).getCString());
        ans_cdt = ans_cdt.setUctOffSet(utc);
    }

    //cout << "after constructor call in operator+ " << endl;

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);
    fin_cdt = ans_cdt.insert_WildCard(splat_pos);
    
     return fin_cdt;
}


void CIMDateTime::operator-=(const CIMDateTime & cDT)
{

    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    CIMDateTime dif_cdt = cur_cDT - opt_cDT;
      _rep->copy(dif_cdt._rep);

     return;
}

const CIMDateTime CIMDateTime::operator*(Uint64 num)
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
 
    CIMDateTime fin_cdt = prod_cdt.insert_WildCard(splat_pos);


     return fin_cdt;
}


void CIMDateTime::operator*=(Uint64 num)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));  

    CIMDateTime prod_cdt = cur_cDT * num;
                                      
    _rep->copy(prod_cdt._rep);

     return;
}

const CIMDateTime CIMDateTime::operator/(Uint64 num)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    

    if (!this->isInterval()){
        throw TypeMismatchException();
    }

    Uint64 cur_num = cur_cDT.toMicroSeconds();
    Uint64 prod = cur_num/num;
    CIMDateTime prod_cdt = CIMDateTime(prod, true);
    CIMDateTime dummy;

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(dummy);
    prod_cdt = prod_cdt.insert_WildCard(splat_pos);

     return prod_cdt;
}


void CIMDateTime::operator/=(Uint64 num)
{    
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));

    CIMDateTime ans = cur_cDT/num;
    _rep->copy(ans._rep);
   
     return;
}


Uint64 CIMDateTime::operator/(const CIMDateTime & cDT)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    if (!cur_cDT.isInterval() || !opt_cDT.isInterval()) {
        throw TypeMismatchException(); 
    }

    Uint64 opt_num = opt_cDT.toMicroSeconds();
    Uint64 cur_num = cur_cDT.toMicroSeconds();

    Uint64 ans = cur_num/opt_num;

    return ans;

}


const Boolean CIMDateTime::operator<(const CIMDateTime & cDT)
{
    
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    if ((!cur_cDT.isInterval() && opt_cDT.isInterval()) || (cur_cDT.isInterval() && !opt_cDT.isInterval())) {
        throw TypeMismatchException(); 
    }

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);
    CIMDateTime opt_spl = opt_cDT.insert_WildCard(splat_pos);
    CIMDateTime cur_spl = cur_cDT.insert_WildCard(splat_pos);

    Uint64 opt_num = opt_spl.toMicroSeconds();
    Uint64 cur_num = cur_spl.toMicroSeconds();

    if (cur_num < opt_num) {
        return true;
    }
    else{
        return false;
    }
}


const Boolean CIMDateTime::operator<=(const CIMDateTime & cDT)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    if ((!cur_cDT.isInterval() && opt_cDT.isInterval()) || (cur_cDT.isInterval() && !opt_cDT.isInterval())) {
            throw TypeMismatchException(); 
    }

   Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);
   CIMDateTime opt_spl = opt_cDT.insert_WildCard(splat_pos);
   CIMDateTime cur_spl = cur_cDT.insert_WildCard(splat_pos);

   Uint64 opt_num = opt_spl.toMicroSeconds();
   Uint64 cur_num = cur_spl.toMicroSeconds();

   if (cur_num <= opt_num) {
      return true;
   }
   else{
       return false;
        }
 }
   

const Boolean CIMDateTime::operator>(const CIMDateTime & cDT)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    if ((!cur_cDT.isInterval() && opt_cDT.isInterval()) || (cur_cDT.isInterval() && !opt_cDT.isInterval())) {
            throw TypeMismatchException(); 
    }

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);
    CIMDateTime opt_spl = opt_cDT.insert_WildCard(splat_pos);
    CIMDateTime cur_spl = cur_cDT.insert_WildCard(splat_pos);

    Uint64 opt_num = opt_spl.toMicroSeconds();
    Uint64 cur_num = cur_spl.toMicroSeconds();

    if (cur_num > opt_num) {
       return true;
    }
    else{
       return false;
    }
}


const Boolean CIMDateTime::operator>=(const CIMDateTime & cDT)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    if ((!cur_cDT.isInterval() && opt_cDT.isInterval()) || (cur_cDT.isInterval() && !opt_cDT.isInterval())) {
            throw TypeMismatchException(); 
    }

   Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);
   CIMDateTime opt_spl = opt_cDT.insert_WildCard(splat_pos);
   CIMDateTime cur_spl = cur_cDT.insert_WildCard(splat_pos);

   Uint64 opt_num = opt_spl.toMicroSeconds();
   Uint64 cur_num = cur_spl.toMicroSeconds();

   if (cur_num >= opt_num) {
      return true;
   }
   else{
      return false;
   }
}


    
const Boolean CIMDateTime::operator!=(const CIMDateTime & cDT)
{
    CIMDateTime cur_cDT = CIMDateTime((String)(this->_rep->data));
    CIMDateTime opt_cDT = cDT;

    Uint32 splat_pos = cur_cDT.getHighestWildCardPosition(opt_cDT);
    CIMDateTime opt_spl = opt_cDT.insert_WildCard(splat_pos);
    CIMDateTime cur_spl = cur_cDT.insert_WildCard(splat_pos);

    Uint64 opt_num = opt_spl.toMicroSeconds();
    Uint64 cur_num = cur_spl.toMicroSeconds();

    if (cur_num != opt_num) {
        return true;
    }
    else{
        return false;
    }
}

 

PEGASUS_NAMESPACE_END
