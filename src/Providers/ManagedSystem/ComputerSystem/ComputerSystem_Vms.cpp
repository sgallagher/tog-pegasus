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
// Author: Ray Boucher, Hewlett-Packard Company (ray.boucher@hp.com)
//
// Modified By: 
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////

#include "ComputerSystemProvider.h"
#include "ComputerSystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <descrip.h>
#include <syidef.h>
#include <jpidef.h>
#include <pscandef.h>
#include <lib$routines.h>
#include <starlet.h>
#include <stsdef.h>
#include <ssdef.h>
#include <libdtdef.h>
#include <lnmdef.h>
#include <netdb.h>
#include <time.h>

#define hwrpb$b_sys_serialnum 64
#define MAXHOSTNAMELEN 256

extern "C" {
extern const long EXE$GPQ_HWRPB;
}

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

static String _hostName;
static String _otherInfo;
static String _serialNumber;

ComputerSystem::ComputerSystem()
{
}

ComputerSystem::~ComputerSystem()
{
}


Boolean ComputerSystem::getCaption(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_CAPTION,CAPTION);
    return true;
}

Boolean ComputerSystem::getDescription(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_DESCRIPTION,DESCRIPTION);
    return true;
}

/*
   convertToCIMDateString converts a tm struct to a CIMDateTime formatted
   char *. -1 is retuned on  error, 1 otherwise.
*/

int convertToCIMDateString(struct tm *t, char *time)
{
  // Format the date.
  sprintf(time,"%04d%02d%02d%02d%02d%02d.000000%c%03d",
          t->tm_year + 1900,
          t->tm_mon + 1,
          t->tm_mday,
          t->tm_hour,
          t->tm_min,
          t->tm_sec,
          (timezone>0)?'-':'+',
          timezone/60 - ( t->tm_isdst? 60:0 ));

  return 1;
}

Boolean ComputerSystem::getInstallDate(CIMProperty& p)
{
   int status, istr;
   char record1[512], *rptr1=0;
   FILE *fptr1=0;
   unsigned __int64 bintime=0;
   unsigned short int timbuf[7], val=0;
   char cimtime[80]="";
   struct tm timetm;
   struct tm *ptimetm=&timetm;
   time_t tme=0, tme1=0;
   char t_string[24]="", libdst;
   unsigned int retlen;
   unsigned long libop, libdayweek, libdayear;
   long dst_desc[2];
   char log_string[]="SYS$TIMEZONE_DAYLIGHT_SAVING";
   struct dsc$descriptor_s sysinfo;
   static $DESCRIPTOR(lnm_tbl,"LNM$SYSTEM");
   struct {
        unsigned short wLength;
        unsigned short wCode;
        void*    pBuffer;
        unsigned int* pRetLen;
        int term;
        } item_list;

   sysinfo.dsc$b_dtype=DSC$K_DTYPE_T;
   sysinfo.dsc$b_class=DSC$K_CLASS_S;
   sysinfo.dsc$w_length=sizeof(t_string);
   sysinfo.dsc$a_pointer=t_string;

   status = system("pipe product show history openvms | search/nolog/nowarn/out=history.out sys$input install");
   if (!$VMS_STATUS_SUCCESS(status)) return false;

   if (fptr1 = fopen("history.out", "r"))
   {
        while (fgets(record1, sizeof(record1), fptr1))
        {
           for (istr=0; istr<=(sizeof(record1)-4); istr++)
           {
                if ((rptr1 = strstr(record1+istr,"-")) && !strncmp(rptr1+4,"-",1)) break;
                rptr1 = 0;
           }
           if (rptr1)
           {
                time(&tme);
                tme1 = mktime(ptimetm);		/* get timezone */
		strcpy(t_string,rptr1-2);
                t_string[20]='.';
                t_string[21]='0';
                t_string[22]='0';
                t_string[23]='0';
		status = sys$bintim (&sysinfo, &bintime);
		if (!$VMS_STATUS_SUCCESS(status)) return false;

		libop=LIB$K_DAY_OF_WEEK;
		status=lib$cvt_from_internal_time (&libop,&libdayweek,&bintime);
		if (!$VMS_STATUS_SUCCESS(status)) return false;

		libop=LIB$K_DAY_OF_YEAR;
		status=lib$cvt_from_internal_time (&libop,&libdayear,&bintime);
		if (!$VMS_STATUS_SUCCESS(status)) return false;

		dst_desc[0]  = strlen(log_string);
		dst_desc[1]  = (long) log_string;
		item_list.wLength = 1;
		item_list.wCode = LNM$_STRING;
		item_list.pBuffer = &libdst;
		item_list.pRetLen = &retlen;
		item_list.term =0;

		status = sys$trnlnm (0,&lnm_tbl,&dst_desc,0,&item_list);
		if (!$VMS_STATUS_SUCCESS(status)) return false;

		status = sys$numtim(timbuf,&bintime);
		if (!$VMS_STATUS_SUCCESS(status)) return false;

		timetm.tm_sec = timbuf[5];
		timetm.tm_min = timbuf[4];
		timetm.tm_hour = timbuf[3];
		timetm.tm_mday = timbuf[2];
		timetm.tm_mon = timbuf[1]-1;
		timetm.tm_year = timbuf[0]-1900;
		timetm.tm_wday = libdayweek-1;
		timetm.tm_yday = libdayear-1;
		timetm.tm_isdst = 0;
		if (libdst != 48) timetm.tm_isdst = 1;

		status = convertToCIMDateString(ptimetm,cimtime);
		if (!$VMS_STATUS_SUCCESS(status)) return false;

		CIMDateTime _installDate(cimtime);
		p = CIMProperty(PROPERTY_INSTALL_DATE, _installDate);
                fclose (fptr1);
                status = system("if (f$search(\"history.out\") .nes. \"\") then delete history.out;*");
                return true;
           } // end if (rptr1 = strstr(record1,"Install"))
        }
        fclose (fptr1);
        status = system("if (f$search(\"history.out\") .nes. \"\") then delete history.out;*");
        return false;
   } // end if (fptr1 = fopen(history.out, "r"))
   else
   {
        fclose (fptr1);
        status = system("if (f$search(\"history.out\") .nes. \"\") then delete history.out;*");
        return false;
   }
}

Boolean ComputerSystem::getCreationClassName(CIMProperty& p)
{
    // can vary, depending on class
    p = CIMProperty(PROPERTY_CREATION_CLASS_NAME,
                    String(CLASS_CIM_UNITARY_COMPUTER_SYSTEM));
    return true;
}

Boolean ComputerSystem::getName(CIMProperty& p)
{
    p = CIMProperty(PROPERTY_NAME,_hostName);
    return true;
}

Boolean ComputerSystem::getStatus(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_STATUS,String("Unknown"));
    return true;
}

Boolean ComputerSystem::getNameFormat(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_NAME_FORMAT,String(NAME_FORMAT));
    return true;
}

Boolean ComputerSystem::getPrimaryOwnerName(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_PRIMARY_OWNER_NAME,"");
    return true;
}

Boolean ComputerSystem::setPrimaryOwnerName(const String&)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getPrimaryOwnerContact(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_PRIMARY_OWNER_CONTACT,"");
    return true;
}

Boolean ComputerSystem::setPrimaryOwnerContact(const String&)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getRoles(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getOtherIdentifyingInfo(CIMProperty& p)
{
  // return model for this property

   int status;
   Array<String> s;
   char hwname[32];
   typedef struct {
        unsigned short wlength;
        unsigned short wcode;
        void *pbuffer;
        void *pretlen; } item_list;
   item_list itmlst3[2];

   itmlst3[0].wlength = sizeof(hwname);
   itmlst3[0].wcode = SYI$_HW_NAME;
   itmlst3[0].pbuffer = hwname;
   itmlst3[0].pretlen = NULL;
   itmlst3[1].wlength = 0;
   itmlst3[1].wcode = 0;
   itmlst3[1].pbuffer = NULL;
   itmlst3[1].pretlen = NULL;

   status = sys$getsyiw (0, 0, 0, itmlst3, 0, 0, 0);
   if ($VMS_STATUS_SUCCESS(status))
   {
	_otherInfo.assign(hwname);
	s.append(_otherInfo);
	p = CIMProperty(PROPERTY_OTHER_IDENTIFYING_INFO,s);
        return true;
   }
   else
        return false;
}

Boolean ComputerSystem::getIdentifyingDescriptions(CIMProperty& p)
{
    // hardcoded
    Array<String> s;
    s.append("Model");
    p = CIMProperty(PROPERTY_IDENTIFYING_DESCRIPTIONS,s);
    return true;
}

Boolean ComputerSystem::getDedicated(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getResetCapability(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getPowerManagementCapabilities(CIMProperty& p)
{
    // hardcoded
    Array<Uint16> s;
    s.append(1);
    p = CIMProperty(PROPERTY_POWER_MANAGEMENT_CAPABILITIES, s);
    return true;
}

Boolean ComputerSystem::getInitialLoadInfo(CIMProperty& p)
{
   long status, dst_desc[2];
   char log_string[]="SYS$SYSDEVICE";
   char res_string[256]="", *ptr1=0, *ptr2=0;
   unsigned int retlen;
   static $DESCRIPTOR(lnm_tbl,"LNM$SYSTEM");
   struct {
        unsigned short wLength;
        unsigned short wCode;
        void*    pBuffer;
        unsigned int* pRetLen;
        int term;
        } item_list;

   dst_desc[0]  = strlen(log_string);
   dst_desc[1]  = (long) log_string;
   item_list.wLength = sizeof(res_string);
   item_list.wCode = LNM$_STRING;
   item_list.pBuffer = res_string;
   item_list.pRetLen = &retlen;
   item_list.term =0;

   status = sys$trnlnm (0,&lnm_tbl,&dst_desc,0,&item_list);
   if ($VMS_STATUS_SUCCESS(status))
   {
	ptr1 = res_string;
	ptr2 = strchr(ptr1,':');
	if (ptr2) res_string[ptr2-ptr1] = '\0';
	p = CIMProperty(PROPERTY_INITIAL_LOAD_INFO, res_string);
	return true;
   }
   else
   {
	p = CIMProperty(PROPERTY_INITIAL_LOAD_INFO, "Unknown");
	return false;
   }
}

Boolean ComputerSystem::getLastLoadInfo(CIMProperty& p)
{
  // ATTN: not sure yet
  return false;
}

Boolean ComputerSystem::getPowerManagementSupported(CIMProperty& p)
{
  // hardcoded
  p = CIMProperty(PROPERTY_POWER_MANAGEMENT_SUPPORTED,false); // on PA-RISC!!
  return true;
}

Boolean ComputerSystem::getPowerState(CIMProperty& p)
{
  // hardcoded
/*
ValueMap {"1", "2", "3", "4", "5", "6", "7", "8"},
Values {"Full Power", "Power Save - Low Power Mode",
                   "Power Save - Standby", "Power Save - Other",
                   "Power Cycle", "Power Off", "Hibernate", "Soft Off"}
*/
  p = CIMProperty(PROPERTY_POWER_STATE,Uint16(1));  // Full Power on PA-RISC!!
  return true;
}

Boolean ComputerSystem::getWakeUpType(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getPrimaryOwnerPager(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_PRIMARY_OWNER_PAGER,"");
    return true;
}

Boolean ComputerSystem::setPrimaryOwnerPager(const String&)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getSecondaryOwnerName(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_SECONDARY_OWNER_NAME,"");
    return true;
}

Boolean ComputerSystem::setSecondaryOwnerName(const String&)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getSecondaryOwnerContact(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_SECONDARY_OWNER_CONTACT,"");
    return true;
}

Boolean ComputerSystem::setSecondaryOwnerContact(const String&)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getSecondaryOwnerPager(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_SECONDARY_OWNER_PAGER,"");
    return true;
}

Boolean ComputerSystem::setSecondaryOwnerPager(const String&)
{
  // not supported
  return false;
}

int GetSerNum (char *pSerNum)
{
    void *phwrpb;

    phwrpb = (void *) ((int)EXE$GPQ_HWRPB+hwrpb$b_sys_serialnum);
    strncpy(pSerNum,(char*)phwrpb,16);
    return(SS$_NORMAL);
}

Boolean ComputerSystem::getSerialNumber(CIMProperty& p)
{
    long status = SS$_NORMAL, i;
    char lrSerNum[16]="",lSerNum[16]="";

    struct k1_arglist {                 // kernel call arguments
            long    lCount;             // number of arguments
            char *pSerNum;
        } getsernumkargs = {1};                // init 1 argument

    getsernumkargs.pSerNum = lrSerNum;

    status = sys$cmkrnl(GetSerNum,&getsernumkargs);
    if ($VMS_STATUS_SUCCESS(status))
    {
        for (i=0;i<strlen(lrSerNum);i++)
        {
           lSerNum[strlen(lrSerNum)-i-1] = lrSerNum[i];
        }
	_serialNumber.assign(lSerNum);
	p = CIMProperty(PROPERTY_SERIAL_NUMBER, _serialNumber);
        return true;
    }

    return false;
}

Boolean ComputerSystem::getIdentificationNumber(CIMProperty& p)
{
  // Not supported
  return false;
}

void ComputerSystem::initialize(void)
{
     char    hostName[MAXHOSTNAMELEN];
     struct  hostent *he;

     if (gethostname(hostName, MAXHOSTNAMELEN) != 0)
     {
        _hostName.assign("Unknown");
         return;
     }

     // Now get the official hostname.  If this call fails then return
     // the value from gethostname().

     he=gethostbyname(hostName);
     if (he)
     {
        strcpy(hostName, he->h_name);
     }

     _hostName.assign(hostName);
     return;
}

String ComputerSystem::getHostName(void)
{
    return _hostName;
}
