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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_DateTime.h"
#include "CMPI_Ftabs.h"

#include <time.h>
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
#include <sys/time.h>
#endif
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

extern "C" {

   static CMPIStatus dtRelease(CMPIDateTime* eDt) {
   //   cout<<"--- dtRelease()"<<endl;
      CIMDateTime* dt=(CIMDateTime*)eDt->hdl;
      if (dt) {
         delete dt;
         (reinterpret_cast<CMPI_Object*>(eDt))->unlinkAndDelete();
      }
      CMReturn(CMPI_RC_OK);
   }

   CMPIDateTime *newDateTime() {
      CIMDateTime *dt=new CIMDateTime();
      *dt=CIMDateTime::getCurrentDateTime();
      return reinterpret_cast<CMPIDateTime*>(new CMPI_Object(dt));
   }

   CMPIDateTime *newDateTimeBin(CMPIUint64 tim, CMPIBoolean interval) {
      CIMDateTime *dt=new CIMDateTime(tim, interval);
      return reinterpret_cast<CMPIDateTime*>(new CMPI_Object(dt));
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
      return reinterpret_cast<CMPIDateTime*>(new CMPI_Object(dt));
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
      CMPIDateTime* neDt=reinterpret_cast<CMPIDateTime*>(obj);
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
      CMPIString *str=reinterpret_cast<CMPIString*>(new CMPI_Object(dt->toString()));
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
   #if defined(PEGASUS_OS_TYPE_WINDOWS)
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





