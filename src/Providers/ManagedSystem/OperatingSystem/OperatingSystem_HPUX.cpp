//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Jim Metcalfe <jim_metcalfe@hp.com>
//         Susan Campbell <susan_campbell@hp.com>
//
// Modified By: 
//
//%////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Exception.h>

#include <iostream>
#include <set>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/pstat.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <utmp.h>
#include <regex.h>
#include <dirent.h>

/* ==========================================================================
   Type Definitions
   ========================================================================== */
typedef unsigned char boolean_t;
typedef unsigned long ErrorStatus_t;
typedef struct Timestamp {
char year[4];
char month[2];
char day[2];
char hour[2];
char minutes[2];
char seconds[2];
char dot;
char microSeconds[6];
char plusOrMinus;
char utcOffset[3];
char padding[3];
} Timestamp_t;

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/*
================================================================================
NAME              : _getOSName
DESCRIPTION       : Call uname() and get the operating system name.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
static Boolean _getOSName(String& osName)
{
    struct utsname  unameInfo;

    /* Call uname and check for any errors. */
    if (uname(&unameInfo) < 0)
    {
       return false;
    }

    osName.assign(unameInfo.sysname);

    return true;
}

/*
================================================================================
NAME              : getUtilGetHostName
DESCRIPTION       : Get the name of the host system.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
static Boolean getUtilGetHostName(String& csName)
{
     char    hostName[MAXHOSTNAMELEN];
     struct  hostent *he;

     if (gethostname(hostName, MAXHOSTNAMELEN) != 0)
     {
         return false;
     }

     /* Now get the official hostname.  If this call fails then return */
     /* the value from gethostname().                                  */
     if (he=gethostbyname(hostName))
     {
        strcpy(hostName, he->h_name);
     }

     csName.assign(hostName);

     return true;
}


OperatingSystem::OperatingSystem(void)
{
}

OperatingSystem::~OperatingSystem(void)
{
}

Boolean OperatingSystem::getCSName(String& csName)
{
    return getUtilGetHostName(csName);
}

Boolean OperatingSystem::getName(String& osName)
{
   return _getOSName(osName);
}

Boolean OperatingSystem::getCaption(String& caption)
{
   return false;
}

Boolean OperatingSystem::getDescription(String& description)
{
   return false;
}

Boolean OperatingSystem::getInstallDate(CIMDateTime& installDate)
{
   return false;
}

Boolean OperatingSystem::getStatus(String& status)
{
   return false;
}

Boolean OperatingSystem::getVersion(String& osVersion)
{
    struct utsname  unameInfo;
    char version[sizeof(unameInfo.release) + sizeof(unameInfo.version)];

    /* Call uname and check for any errors. */
    if (uname(&unameInfo) < 0)
    {
       return false;
    }

    osVersion = "";
    sprintf(version, "%s.%s", unameInfo.release, unameInfo.version);
    osVersion.assign(version);

    return true;
}

Boolean OperatingSystem::getOSType(Uint16& osType)
{
   const Uint16 CIM_HPUX_OS_TYPE = 8;
   osType = CIM_HPUX_OS_TYPE;
   return true;
}

Boolean OperatingSystem::getOtherTypeDescription(String& otherTypeDescription)
{
   otherTypeDescription = "";
   return true;
}

Boolean OperatingSystem::getLastBootUpTime(CIMDateTime& lastBootUpTime)
{
    char               tzString[40];
    long               year;
    struct timeval     tv;
    struct timezone    tz;
    struct tm          stmval;
    struct tm         *tmval = (&stmval);
#ifdef NOTDEFINED
    struct tm          tmval;
#endif
    struct pst_static  pst;
    ErrorStatus_t      rc;
    Timestamp_t        bootTime;
    char mTmpString[80];


    /* Get the static information from the pstat call to the system. */ 
    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1)
    {
	return false;
    }
    else
    {
        /* Get the boot time and convert to GMT time. */ 
	putenv(tzString);
        tmval = localtime((time_t *)&pst.boot_time);
        gettimeofday(&tv,&tz);

        year = 1900;
        memset((void *)&bootTime, 0, sizeof(Timestamp_t));

        /* Format the date. */ 
        sprintf((char *) &bootTime,"%04d%02d%02d%02d%02d%02d.%06d%04d",
                year + tmval->tm_year,
                tmval->tm_mon + 1,
                tmval->tm_mday,
                tmval->tm_hour,
                tmval->tm_min,
                tmval->tm_sec,
                0,
                tz.tz_minuteswest);
	if (tz.tz_minuteswest > 0) {
	    bootTime.plusOrMinus = '-';
	}
	else {
	   bootTime.plusOrMinus = '+';
	}
    }
    lastBootUpTime.clear();
    strcpy(mTmpString, (char *)&bootTime);
    lastBootUpTime.set(mTmpString);
    return true;
}

Boolean OperatingSystem::getLocalDateTime(CIMDateTime& localDateTime)
{
    long         year;
    Timestamp_t  dateTime;
    char   mTmpString[80];
    time_t mSysTime;
    struct timeval   tv;
    struct timezone  tz;
    struct tm      * tmval;

    /* Get the date and time from the system. */
    mSysTime = time(NULL);
    tmval = gmtime(&mSysTime);
    gettimeofday(&tv,&tz);

    year = 1900;
    /* Format the date. */
    sprintf((char *)&dateTime,"%04d%02d%02d%02d%02d%02d.%06d+%03d",
                    year + tmval->tm_year,
                    tmval->tm_mon + 1,
                    tmval->tm_mday,
                    tmval->tm_hour,
                    tmval->tm_min,
                    tmval->tm_sec,
                    0,
                    tz.tz_minuteswest);

    if (tz.tz_minuteswest > 0) 
    {
        dateTime.plusOrMinus = '-';
    }
    else 
    {
        dateTime.plusOrMinus = '+';
    }

    localDateTime.clear();
    strcpy(mTmpString, (char *)&dateTime);
    localDateTime.set(mTmpString);
    return true;
}

Boolean OperatingSystem::getCurrentTimeZone(Sint16& currentTimeZone)
{
    struct timeval   tv;
    struct timezone  tz;
    struct tm      * tmval;

    /* Get the time from the system. */
    gettimeofday(&tv,&tz);
    currentTimeZone = -tz.tz_minuteswest;
    return true;
}

Boolean OperatingSystem::getNumberOfLicensedUsers(Uint32& numberOfLicensedUsers)
{
   //-- according to the mof, if it's unlimited, use zero
   numberOfLicensedUsers = 0;
   return true;
}

Boolean OperatingSystem::getNumberOfUsers(Uint32& numberOfUsers)
{
    struct utmp * utmpp;

    numberOfUsers = 0;

    while ((utmpp = getutent()) != NULL)
    {
        if (utmpp->ut_type == USER_PROCESS)
        {
            numberOfUsers++;
        }

        free(utmpp);
    }

    endutent();

    return true;
}

Boolean OperatingSystem::getNumberOfProcesses(Uint32& numberOfProcesses)
{
    struct pst_dynamic psd;

    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1)
    {
        return false;
    }
    else
    {
        numberOfProcesses = psd.psd_activeprocs;
    }

    return true;
}

Boolean OperatingSystem::getMaxNumberOfProcesses(Uint32& mMaxProcesses)
{
    struct pst_static pst;

    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1)
    {
       return false; 
    }
    else
    {
        mMaxProcesses = pst.max_proc;
    }

    return true;
}

Boolean OperatingSystem::getTotalSwapSpaceSize(Uint64& mTotalSwapSpaceSize)
{
    char               mline[80];
    FILE             * mswapInfo;

    /* Initialize the return parameter in case swapinfo is not available. */
    mTotalSwapSpaceSize = -1;

    /* Use a pipe to invoke swapinfo. */
    if ((mswapInfo = popen("swapinfo -q 2>/dev/null", "r")) != NULL)
    {
        /* Now extract the total swap space size from the swapinfo output. */
        while (fgets(mline, 80, mswapInfo))
        {
            mTotalSwapSpaceSize = atol (mline);
        }  /* while */

        (void)pclose (mswapInfo);
        return true;
    }
    else
    {
        return false;
    }
}

Boolean OperatingSystem::getTotalVirtualMemorySize(Uint64& total)
{
   return false;
}

Boolean OperatingSystem::getFreeVirtualMemory(Uint64& freeVirtualMemory)
{
   return false;
}

Boolean OperatingSystem::getFreePhysicalMemory(Uint64& total)
{
   return false;
}

Boolean OperatingSystem::getTotalVisibleMemorySize(Uint64& memory)
{
    float         psize;
    float         total;
    struct        pst_static pst;

    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1)
    {
        return false;
    }
    else
    {
        psize = pst.page_size * 0.000977;
        total = ((float)pst.physical_memory * 0.000977 * psize);
        memory = total;
        return true;
    }
}

Boolean OperatingSystem::getSizeStoredInPagingFiles(Uint64& total)
{
   return getTotalSwapSpaceSize(total);
}

Boolean OperatingSystem::getFreeSpaceInPagingFiles(Uint64& freeSpaceInPagingFiles)
{
   return false;
}

Boolean OperatingSystem::getMaxProcessMemorySize(Uint64& maxProcessMemorySize)
{
   Uint32 count;
   const char proc_file[] = "/proc/sys/vm/overcommit_memoryt";
   char buffer[MAXPATHLEN];
   struct stat statBuf;
   FILE *vf;

   count = 0;
   if (!stat(proc_file, &statBuf))
   {
      vf = fopen(proc_file, "r");
      if (vf)
      {
         if (fgets(buffer, MAXPATHLEN, vf) != NULL)
	    sscanf(buffer, "%d", &count);
         fclose(vf);
      }
   }

   if (count > 0)
   {
       maxProcessMemorySize = count;
       return true;
   }
   else
   {
       return getTotalVirtualMemorySize(maxProcessMemorySize);
   }
}

Boolean OperatingSystem::getDistributed(Boolean& distributed)
{
   return false;
}

Boolean OperatingSystem::getSystemUpTime(Uint64& mUpTime)
{
    time_t timeval;
    struct pst_static pst;

    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1)
    {
       return false;
    }
    else
    {
        timeval= time((time_t *)NULL);
        timeval= (time_t)((long)timeval - (long)pst.boot_time);
        mUpTime = (long)timeval;
    }

    return true;
}

Boolean OperatingSystem::getOperatingSystemCapability(String& scapability)
{
    char               capability[80];
    long               ret;

/* Define the following for compiling on 10.20. This will provide values */
/* that are not valid.  This is ok since the calls will return -1 and "" */
/* will get returned.                                                    */
#ifndef _SC_KERNEL_BITS
#       define _SC_KERNEL_BITS 10013
#endif

    ret = sysconf (_SC_KERNEL_BITS);
    if (ret != -1)
    {
        sprintf (capability, "%d", ret);
    }
    else
    {
       return false;
    }

    scapability.assign(capability);
    return true;
}

Uint32 OperatingSystem::Reboot()
{
   const char *reboot[] = { "reboot", NULL };
   const char *paths[] = { "/sbin", "/usr/sbin", "/usr/local/sbin", NULL };
   struct stat sbuf;
   String fname;
   char *p;
   Uint32 result;

   result = 1;
   for (int ii = 0; paths[ii] != NULL; ii++)
   {
      for (int jj = 0; reboot[jj]; jj++)
      {
	 fname = paths[ii];
	 fname += "/";
	 fname += reboot[jj];
	 p = fname.allocateCString();
	 if (stat(p, &sbuf) == 0 && (sbuf.st_mode & S_IXUSR))
	 {
	    result = 2;
	    if (system(p) == 0)
	       result = 0;

	    delete [] p;
	    return result;
	 }
	 delete [] p;
      }
   }
   return result;
}

Uint32 OperatingSystem::Shutdown()
{
   const char *poweroff[] = { "poweroff", NULL };
   const char *paths[] = { "/sbin", "/usr/sbin", "/usr/local/sbin", NULL };
   struct stat sbuf;
   String fname;
   char *p;
   Uint32 result;

   result = 1;
   for (int ii = 0; paths[ii] != NULL; ii++)
   {
      for (int jj = 0; poweroff[jj]; jj++)
      {
	 fname = paths[ii];
	 fname += "/";
	 fname += poweroff[jj];
	 p = fname.allocateCString();
	 if (stat(p, &sbuf) == 0 && (sbuf.st_mode & S_IXUSR))
	 {
	    result = 2;
	    if (system(p) == 0)
	       result = 0;

	    delete [] p;
	    return result;
	 }
	 delete [] p;
      }
   }
   return result;
}

PEGASUS_NAMESPACE_END

