//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Jim Metcalfe <jim_metcalfe@@hp.com>
//         Susan Campbell <susan_campbell@@hp.com>
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
#include <utmpx.h>
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

OperatingSystem::OperatingSystem(void)
{
}

OperatingSystem::~OperatingSystem(void)
{
}

/**
   _getOSName method of the HP-UX implementation for the OS Provider

   Calls uname() to get the operating system name.

  */
static Boolean _getOSName(String& osName)
{
    struct utsname  unameInfo;

    // Call uname and check for any errors. 
    if (uname(&unameInfo) < 0)
    {
       return false;
    }

    osName.assign(unameInfo.sysname);

    return true;
}

/**
   getUtilGetHostName method for the HP-UX implementation of the OS Provider
   
   This supporting utility function gets the name of the host system 
   from gethostname and gethostbyname.

  */

static Boolean getUtilGetHostName(String& csName)
{
     char    hostName[MAXHOSTNAMELEN];
     struct  hostent *he;

     if (gethostname(hostName, MAXHOSTNAMELEN) != 0)
     {
         return false;
     }

     // Now get the official hostname.  If this call fails then return
     // the value from gethostname().                                 
     
     if (he=gethostbyname(hostName))
     {
        strcpy(hostName, he->h_name);
     }

     csName.assign(hostName);

     return true;
}


Boolean OperatingSystem::getCSName(String& csName)
{
    return getUtilGetHostName(csName);
}

Boolean OperatingSystem::getName(String& osName)
{
   return _getOSName(osName);
}

/**
   getCaption method for HP-UX implementation of OS Provider

   Uses a string constant for the Caption.
  */
Boolean OperatingSystem::getCaption(String& caption)
{

   caption.assign("The current Operating System");

   return true;
}

/**
   getDescription method for HP-UX implementation of OS Provider

   Uses a string constant for the Caption.
  */
Boolean OperatingSystem::getDescription(String& description)
{

   description.assign("This instance reflects the Operating System"
	" on which the CIMOM is executing (as distinguished from instances"
        " of other installed operating systems that could be run).");

   return true;
}

/**
   getInstallDate method for HP-UX implementation of OS provider

   Need to determine reliable method of knowing install date 
   one possibility is date of /stand/vmunix (but not always
   truly the date the OS was installed. For now, don't return
   any date (function returns FALSE).                    
*/
Boolean OperatingSystem::getInstallDate(CIMDateTime& installDate)
{
// ATTN-SLC-P2-17-Apr-02:  Implement getInstallDate for HP-UX

   return false;
}

/**
   getStatus method for HP-UX implementation of OS provider

   Would like to be able to return and actual status vs. just   
   always Unknown, but didn't know how to differentiate between
   OK and Degraded (assuming they are the only values that make
   sense, since the CIMOM is up and running), but one could see  
   an argument for including Stopping if the Shutdown or Reboot 
   methods have been invoked. For now, always return "Unknown".
*/                                 
Boolean OperatingSystem::getStatus(String& status)
{

// ATTN-SLC-P3-17-Apr-02: Get true HP-UX status (vs. Unknown) BZ#44


   status.assign("Unknown");

   return true;
}

/**
   getVersion method for HP-UX implementation of OS provider

   Uses uname system call and extracts the release information 
   (e.g., B.11.20).  Version field in uname actually contains
   user license info (thus isn't included).

   Returns FALSE if uname call results in errors.
  */

Boolean OperatingSystem::getVersion(String& osVersion)
{

    struct utsname  unameInfo;

    // Call uname and check for any errors. 

    if (uname(&unameInfo) < 0)
    {
       return false;
    }

    osVersion.assign(unameInfo.release);

    return true;
}

/**
    getOSType method for HP-UX implementation of OS Provider

    Always returns 8 = HPUX
  */

Boolean OperatingSystem::getOSType(Uint16& osType)
{
   osType = HPUX;  // from enum in .h file
   return true;
}

/**
    getOtherTypeDescription method for HP-UX implementation of OS provider

    Returns FALSE since not needed for HP-UX (don't return the empty string).
  */
Boolean OperatingSystem::getOtherTypeDescription(String& otherTypeDescription)
{
   return false;
}

/**
   getLastBootUpTime method for HP-UX implementation of OS Provider

   Gets information from pstat call.  Internally in UTC (Universal
   Time Code) which must be converted to localtime for CIM
  */
Boolean OperatingSystem::getLastBootUpTime(CIMDateTime& lastBootUpTime)
{
    long               year;
    struct timeval     tv;
    struct timezone    tz;
    struct tm          *tmval;
    struct pst_static  pst;
    ErrorStatus_t      rc;
    Timestamp_t        bootTime;
    char mTmpString[80];


    // Get the static information from the pstat call to the system.
 
    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1)
    {
	return false;
    }
    else
    {
        // Get the boot time and convert to local time. 

// ATTN-SLC-P2-17-Apr-02: use CIMOM cim date time function for consistency

        tmval = localtime((time_t *)&pst.boot_time);
        gettimeofday(&tv,&tz);

        year = 1900;
        memset((void *)&bootTime, 0, sizeof(Timestamp_t));

        // Format the date. 
        sprintf((char *) &bootTime,"%04d%02d%02d%02d%02d%02d.%06d%04d",
                year + tmval->tm_year,
                tmval->tm_mon + 1,   // HP-UX stores month 0-11
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

/**
   getLocalDateTime method for HP-UX implementation of OS Provider

   Gets information from localtime call, converted to CIM
   DateTime format. 
  */
Boolean OperatingSystem::getLocalDateTime(CIMDateTime& localDateTime)
{
    long         year;
    Timestamp_t  dateTime;
    char   mTmpString[80];
    time_t mSysTime;
    struct timeval   tv;
    struct timezone  tz;
    struct tm      * tmval;

    // Get the date and time from the system. 

// ATTN-SLC-P2-17-Apr-02: use CIMOM cim date time function for consistency
    
    mSysTime = time(NULL);
    tmval = localtime(&mSysTime);
    gettimeofday(&tv,&tz);

    year = 1900;
    // Format the date.
    sprintf((char *)&dateTime,"%04d%02d%02d%02d%02d%02d.%06d+%03d",
                    year + tmval->tm_year,
                    tmval->tm_mon + 1,   // HP-UX stored month as 0-11
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
/**
   getCurrentTimeZone method for HP-UX implementation of OS Provider

   Gets information from gettimeofday call and ensures sign follows
   CIM standard.
  */
Boolean OperatingSystem::getCurrentTimeZone(Sint16& currentTimeZone)
{
    struct timeval   tv;
    struct timezone  tz;
    struct tm      * tmval;

    // Get the time from the system. 
    gettimeofday(&tv,&tz);
    currentTimeZone = -tz.tz_minuteswest;
    return true;
}

/**
   getNumberOfLicensedUsers method for HP-UX implementation of OS provider

   Calls uname and checks the version string (to get user license
   information.  This version field doesn't currently distinguish
   between 128, 256, and unlimited user licensed (all = U).
   Need to determine how to differentiate and fix this, for now return 
   0 (which is unlimited).  Don't know if uname -l has same limitation.
  */
Boolean OperatingSystem::getNumberOfLicensedUsers(Uint32& numberOfLicensedUsers)
{
    struct utsname  unameInfo;

    // Call uname and check for any errors. 
    if (uname(&unameInfo) < 0)
    {
       return false;
    }
   // For HP-UX, the number of licensed users is returned in the version
   // field of uname result.
    switch (unameInfo.version[0]) {
        case 'A' : { numberOfLicensedUsers = 2; break; }
        case 'B' : { numberOfLicensedUsers = 16; break; }
        case 'C' : { numberOfLicensedUsers = 32; break; }
        case 'D' : { numberOfLicensedUsers = 64; break; }
        case 'E' : { numberOfLicensedUsers = 8; break; }
        case 'U' : {
            // U could be 128, 256, or unlimited 
            // need to find test system with 128 or 256 user license
            // to determine if uname -l has correct value
            // for now, return 0 = unlimited
//ATTN-SLC-P2-18-Apr-02: Distinguish HP-UX 128,256,unliminted licenses BZ#43
	    numberOfLicensedUsers = 0;
            break;
        }
        default : return false;
     }
    return true;
}

/**
   getNumberOfUsers method for HP-UX implementation of OS Provider

   Goes through the utents, counting the number of type USER_PROCESS
  */
Boolean OperatingSystem::getNumberOfUsers(Uint32& numberOfUsers)
{
    struct utmpx * utmpp;

    numberOfUsers = 0;

// ATTN-SLC-P3-17-Apr-02: optimization? parse uptime instead?

    while ((utmpp = getutxent()) != NULL)
    {
        if (utmpp->ut_type == USER_PROCESS)
        {
            numberOfUsers++;
        }
    }

    endutxent();

    return true;
}

/**
   getNumberOfProcesses method for HP-UX implementation of OS Provider

   Gets number of active processes from pstat.
  */
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

/**
   getMaxNumberOfProcesses method for HP-UX implementation of OS Provider

   Gets maximum number of processes from pstat.
  */
Boolean OperatingSystem::getMaxNumberOfProcesses(Uint32& mMaxProcesses)
{
    struct pst_static pst;

// ATTN-SLC-P2-17-Apr-02: getting value of 820? correct?

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

/** 
   getTotalSwapSpaceSize method for HP-UX implementation of HP-UX

   Gets information from swapinfo -q command (techically not swap
   space, it's paging).

  */
Boolean OperatingSystem::getTotalSwapSpaceSize(Uint64& mTotalSwapSpaceSize)
{
    char               mline[80];
    FILE             * mswapInfo;
    Uint32             swapSize;

    // Initialize the return parameter in case swapinfo is not available. 
    mTotalSwapSpaceSize = -1;

    // Use a pipe to invoke swapinfo. 
    if ((mswapInfo = popen("swapinfo -q 2>/dev/null", "r")) != NULL)
    {
        // Now extract the total swap space size from the swapinfo output. 
        while (fgets(mline, 80, mswapInfo))
        {
           sscanf(mline, "%d", &swapSize);
           mTotalSwapSpaceSize = swapSize;
//    mTotalSwapSpaceSize = atol (mline);  changed to sscanf for portability
        }  // end while 

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

/**
   getTotalVisibleMemorySize method for HP-UX implementation of OS Provider

   Gets information from pstat, use of numeric constant is historical
   (DMI folks got it from the SAM folks).
   */
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
        // use of this constant was in SAM and then DMI
        psize = pst.page_size * 0.000977;
        total = ((float)pst.physical_memory * 0.000977 * psize);
        memory = total;
        return true;
    }
}

Boolean OperatingSystem::getSizeStoredInPagingFiles(Uint64& total)
{

// 
//   return getTotalSwapSpaceSize(total);

    return false;
}

Boolean OperatingSystem::getFreeSpaceInPagingFiles(Uint64& freeSpaceInPagingFiles)
{
   return false;
}

Boolean OperatingSystem::getMaxProcessMemorySize(Uint64& maxProcessMemorySize)
{

// ATTN-SLC-P1-17-Apr-02: implement for HP-UX - pst_maxmem?
       return false;
}

/**
   getDistributed method for HP-UX implementation of OS Provider

   always sets the distributed boolean to FALSE
  */
Boolean OperatingSystem::getDistributed(Boolean& distributed)
{
   distributed = false;

   return true;
}

/**
   getSystemUpTime method for HP-UX implementation of OS Provider

   Calculates the information from the local time and boot time.
   Could also consider use of uptime information.  Ideally, would
   like to have consistency across the time-related properties
   (e.g., uptime = local time - Boot time). 
  */
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
// ATTN-SLC-P2-17-Apr-02: use CIMOM date time for consistency
        timeval= time((time_t *)NULL);
        timeval= (time_t)((long)timeval - (long)pst.boot_time);
        mUpTime = (long)timeval;
    }

    return true;
}

/**
   getOperatingSystemCapability method for HP-UX implementation of OS Provider

   Gets information from sysconf call (using _SC_KERNEL_BITS).  
   */
Boolean OperatingSystem::getOperatingSystemCapability(String& scapability)
{
    char               capability[80];
    long               ret;

    ret = sysconf (_SC_KERNEL_BITS);
    if (ret != -1)
    {
        sprintf (capability, "%d bit", ret);
    }
    else
    {
       return false;
    }

    scapability.assign(capability);
    return true;
}

/**
   Reboot method for HP-UX implementation of OS Provider

   Finds executable in /sbin, /usr/bin, or /usr/local/sbin and invokes.
   Currently disabled (as we don't want folks rebooting systems yet)

   Invokes as via system system call, so have full checking of user's 
   authorization (already authenticated by CIMOM)
   */
Uint32 OperatingSystem::Reboot()
{
   const char *reboot[] = { "reboot", NULL };
   const char *paths[] = { "/sbin", "/usr/sbin", "/usr/local/sbin", NULL };
   struct stat sbuf;
   String fname;
   char *p;
   Uint32 result;

// ATTN-SLC-P2-17-Apr-02: this method not invoked for HP-UX (run as root)
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

// ATTN-SLC-P2-17-Apr-02: this method not invoked for HP-UX (run as root)

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

