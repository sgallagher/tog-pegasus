//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
// The Open Group, Tivoli Systems
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
//=============================================================================
//
// Author: Sushma Fernandes, Hewlett Packard Company (sushma_fernandes@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/CIMDateTime.h>

#include <sstream>
#include <iomanip>
#include <windows.h>
#include <time.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//
// This function returns the currentTimeZone.
//
Boolean getCurrentTimeZone(Sint16& currentTimeZone)
{
   currentTimeZone = 0;

   TIME_ZONE_INFORMATION timezone;

   ::memset(&timezone, 0, sizeof(timezone));

   switch(::GetTimeZoneInformation(&timezone)) {
   case TIME_ZONE_ID_UNKNOWN:
      currentTimeZone = timezone.Bias;
   case TIME_ZONE_ID_STANDARD:
      currentTimeZone = timezone.Bias + timezone.StandardBias;
   case TIME_ZONE_ID_DAYLIGHT:
      currentTimeZone = timezone.Bias + timezone.DaylightBias;
   default:
      break;
   }

   // the bias used to calculate the time zone is a factor that is used to 
   // determine the UTC time from the local time. to get the UTC offset from 
   // the local time, use the inverse.
   if(currentTimeZone != 0) {
      currentTimeZone *= -1;
   }

   return(true);
}

CIMDateTime CIMDateTime::getCurrentDateTime()
{
   SYSTEMTIME time;
   Sint16 currentTimeZone;

   ::memset(&time, 0, sizeof(time));

   // Get the local time
   ::GetLocalTime(&time);

   std::stringstream ss;

   ss << std::setfill('0');
   ss << std::setw(4) << time.wYear;
   ss << std::setw(2) << time.wMonth;
   ss << std::setw(2) << time.wDay;
   ss << std::setw(2) << time.wHour;
   ss << std::setw(2) << time.wMinute;
   ss << std::setw(2) << time.wSecond;
   ss << ".";
   ss << std::setw(6) << time.wMilliseconds * 1000;
   if (getCurrentTimeZone(currentTimeZone))
   {
      ss << (currentTimeZone < 0 ? "-" : "+");
   }
   ss << std::setw(3) << ::abs(currentTimeZone);

   CIMDateTime currentDateTime = ss.str().c_str();

   return currentDateTime;
}

PEGASUS_NAMESPACE_END



