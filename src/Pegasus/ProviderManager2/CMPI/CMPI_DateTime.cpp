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
//
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_DateTime.h"
#include "CMPI_Ftabs.h"

#include <time.h>
#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC 
#define SNPRINTF _snprintf
#else
#define SNPRINTF snprintf
#include <sys/time.h>
#endif
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static CIMDateTime *makeCIMDateTime(time_t inTime, unsigned long usec, CMPIBoolean interval)
{
   CIMDateTime *dt=new CIMDateTime();
   char strTime[256];
   char utcOffset[20];
   char usTime[32];
   struct tm tmTime;

   if (interval) {
     // absolut time values needed
#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
     tmTime=*gmtime(&inTime);
#else 
     gmtime_r(&inTime,&tmTime);
#endif
     if (SNPRINTF(strTime,256,
		  "%04d%02d%02d%02d%02d%02d.%06ld:000",
		  tmTime.tm_year-70,
		  tmTime.tm_mon,
		  tmTime.tm_mday-1,
		  tmTime.tm_hour,
		  tmTime.tm_min,
		  tmTime.tm_sec,
		  usec) > 0)
       *dt=String(strTime);
   } else {
#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
     tmTime=*localtime(&inTime);
#else
     localtime_r(&inTime,&tmTime);
#endif
     if (strftime(strTime,256,"%Y%m%d%H%M%S.",&tmTime)) {
      SNPRINTF(usTime,32,"%6.6ld",usec);
      strcat(strTime,usTime);
#if defined (PEGASUS_OS_LINUX)
      SNPRINTF(utcOffset,20,"%+4.3ld",tmTime.tm_gmtoff/60);
#else
      SNPRINTF(utcOffset,20,"%+4.3ld",0);
#endif
      strncat(strTime,utcOffset,256);
      *dt=String(strTime);
      //cout<<"dt = " <<dt->toString()<<endl;
     }
   }
   return dt;
}

extern "C" {

   static CMPIStatus dtRelease(CMPIDateTime* eDt) {
   //   cout<<"--- dtRelease()"<<endl;
      CIMDateTime* dt=(CIMDateTime*)eDt->hdl;
      if (dt) {
         delete dt;
         ((CMPI_Object*)eDt)->unlinkAndDelete();
      }
      CMReturn(CMPI_RC_OK);
   }

   CMPIDateTime *newDateTime() {
      struct timeval tv;
#if defined (PEGASUS_OS_VMS)
      gettimeofday(&tv,NULL);
#else
      struct timezone tz;
      gettimeofday(&tv,&tz);
#endif
      return (CMPIDateTime*)new CMPI_Object(makeCIMDateTime(tv.tv_sec,tv.tv_usec,0));
   }

   CMPIDateTime *newDateTimeBin(CMPIUint64 tim, CMPIBoolean interval) {
      return (CMPIDateTime*)new CMPI_Object(makeCIMDateTime(tim/1000000,tim%1000000,interval));
   }

   CMPIDateTime *newDateTimeChar(const char *strTime) {
      CIMDateTime *dt=new CIMDateTime();
	  try {
      	*dt=String(strTime);
	  } catch ( ... )
		{
			delete dt;
			return NULL;
		}
      return (CMPIDateTime*)new CMPI_Object(dt);
   }

   static CMPIDateTime* dtClone(const CMPIDateTime* eDt, CMPIStatus* rc) {
      CIMDateTime* dt=(CIMDateTime*)eDt->hdl;
      if (!dt) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return NULL;
      }
      CIMDateTime* cDt=new CIMDateTime(dt->toString());
      CMPI_Object* obj=new CMPI_Object(cDt);
      obj->unlink();
      CMPIDateTime* neDt=(CMPIDateTime*)obj;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return neDt;
   }

   static CMPIBoolean dtIsInterval(const CMPIDateTime* eDt, CMPIStatus* rc) {
      CIMDateTime* dt=(CIMDateTime*)eDt->hdl;
      if (!dt) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return false;
      }
		
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return dt->isInterval();
   }

   static CMPIString *dtGetStringFormat(const CMPIDateTime* eDt, CMPIStatus* rc) {
      CIMDateTime* dt=(CIMDateTime*)eDt->hdl;
      if (!dt) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return NULL;
      }
      CMPIString *str=(CMPIString*)new CMPI_Object(dt->toString());
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return str;
   }

   static CMPIUint64 dtGetBinaryFormat(const CMPIDateTime* eDt, CMPIStatus* rc) {
      CIMDateTime* dt=(CIMDateTime*)eDt->hdl;
      if (!dt) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return 0;
      }
      CMPIUint64 days,hours,mins,secs,usecs,utc,lTime;
      struct tm tm,tmt;
      CString tStr=dt->toString().getCString();
      char cStr[26];
      memset( cStr, 0, 26);
      memcpy (cStr, (const char*)tStr, 25);
      if (dt->isInterval()) {
         cStr[21]=0;
         usecs=atoi(cStr+15);
         cStr[15]=0;
         secs=atoi(cStr+12);
         cStr[12]=0;
         mins=atoi(cStr+10);
         cStr[10]=0;
         hours=atoi(cStr+8);
         cStr[8]=0;
         days=atoi(cStr);
         lTime=(days*PEGASUS_UINT64_LITERAL(86400000000))+
               (hours*PEGASUS_UINT64_LITERAL(3600000000))+
               (mins*60000000)+(secs*1000000)+usecs;
      }

      else {
         time_t tt=time(NULL);
   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
         tmt=*localtime(&tt);
   #else
         localtime_r(&tt,&tmt);
   #endif
         memset(&tm,0,sizeof(tm));
         tm.tm_isdst=tmt.tm_isdst;
         utc=atoi(cStr+21);
         cStr[21]=0;
         usecs=atoi(cStr+15);
         cStr[15]=0;
         tm.tm_sec=atoi(cStr+12);
         cStr[12]=0;
         tm.tm_min=atoi(cStr+10);
         cStr[10]=0;
         tm.tm_hour=atoi(cStr+8);
         cStr[8]=0;
         tm.tm_mday=atoi(cStr+6);
         cStr[6]=0;
         tm.tm_mon=(atoi(cStr+4)-1);
         cStr[4]=0;
         tm.tm_year=(atoi(cStr)-1900);
         lTime=mktime(&tm);
         lTime*=1000000;
         lTime+=usecs;
      }

      return lTime;
   }

}

static CMPIDateTimeFT dateTime_FT={
     CMPICurrentVersion,
     dtRelease,
     dtClone,
     dtGetBinaryFormat,
     dtGetStringFormat,
     dtIsInterval,
};

CMPIDateTimeFT *CMPI_DateTime_Ftab=&dateTime_FT;

PEGASUS_NAMESPACE_END





