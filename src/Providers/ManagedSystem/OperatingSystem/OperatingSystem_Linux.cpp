///////////////////////////////////////////////////////////////////////////////
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
// Author: Al Stone <ahs3@fc.hp.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              Susan Campbell      <scampbell@hp.com>
//
///////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Logger.h>
#include "OperatingSystemProvider.h"

#include <iostream>
#include <set>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <utmpx.h>
#include <regex.h>
#include <dirent.h>

PEGASUS_USING_STD;

#define DEBUG(X) // Logger::put(Logger::DEBUG_LOG, "Linux OSProvider",Logger::INFORMATION, "$0", X)

OperatingSystem::OperatingSystem(void)
{
}

OperatingSystem::~OperatingSystem(void)
{
}

/**
   getName method of the Linux implementation for the OS Provider

   Calls uname() to get the operating system name.

  */
Boolean OperatingSystem::getName(String& osName)
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
   getUtilGetHostName method for the Linux implementation of the OS Provider

   Gets the name of the host system from gethostname and gethostbyname.

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

     he=gethostbyname(hostName);
     if (he)
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

/**
   getCaption method for Linux implementation of OS Provider

   Uses a string constant for the Caption.
  */
Boolean OperatingSystem::getCaption(String& caption)
{

   caption.assign("The current Operating System");

   return true;
}

Boolean OperatingSystem::getDescription(String& description)
{

   description.assign("This instance reflects the Operating System"
        " on which the CIMOM is executing (as distinguished from instances"
        " of other installed operating systems that could be run).");

   return true;
}

/**
   getInstallDate method for Linux implementation of OS provider

   Need to determine reliable method of knowing install date
   one possibility is date of OS image (but not always
   truly the date the OS was installed. For now, don't return
   any date (function returns FALSE).
  */
Boolean OperatingSystem::getInstallDate(CIMDateTime& installDate)
{
// ATTN-SLC-P2-17-Apr-02:  Implement getInstallDate for Linux 

   return false;
}

/**
   getStatus method for Linux implementation of OS provider

   Would like to be able to return and actual status vs. just
   always Unknown, but didn't know how to differentiate between
   OK and Degraded (assuming they are the only values that make
   sense, since the CIMOM is up and running), but one could see
   an argument for including Stopping if the Shutdown or Reboot
   methods have been invoked. For now, always return "Unknown".
    */
Boolean OperatingSystem::getStatus(String& status)
{

// ATTN-SLC-P3-17-Apr-02: Get true Linux status (vs. Unknown) BZ#44

   status.assign("Unknown");

   return true;
}

/**
   getVersion method for Linux implementation of OS provider

   Uses uname system call and extracts the release and version
   information (separated by a space).

   Returns FALSE if uname call results in errors.
   */
Boolean OperatingSystem::getVersion(String& osVersion)
{

    struct utsname  unameInfo;
    char version[sizeof(unameInfo.release) + sizeof(unameInfo.version)];

    // Call uname and check for any errors.

    if (uname(&unameInfo) < 0)
    {
       return false;
    }

    sprintf(version, "%s %s", unameInfo.release, unameInfo.version);
    osVersion.assign(version);

    return true;
}

Boolean OperatingSystem::getOSType(Uint16& osType)
{
    osType = LINUX;
    return true;
}

//-- this table indicates how to determine the distro in use;
//   search for the determining_filename in /etc, and if the
//   optional_string is NULL, read that file for distro info.
//   otherwise, use the optional string.
//
static const struct
{
   const char *vendor_name;
   const char *determining_filename;
   const char *optional_string;
} LINUX_VENDOR_INFO[] = {
   { "Caldera",          "coas",               "Caldera Linux" },
   { "Corel",            "environment.corel",  "Corel Linux"   },
   { "Debian GNU/Linux", "debian_version",     NULL            },
   { "Mandrake",         "mandrake-release",   NULL            },
   { "Red Hat",          "redhat-release",     NULL            },
   { "SuSE",             "SuSE-release",       NULL            },
   { "Turbolinux",       "turbolinux-release", NULL            },
   { NULL, NULL, NULL }
};
Boolean OperatingSystem::getOtherTypeDescription(String& otherTypeDescription)
{
   String s;
   char info_file[MAXPATHLEN];
   char buffer[MAXPATHLEN];
   struct stat statBuf;
   FILE *vf;

   s.clear();
   for (int ii = 0; LINUX_VENDOR_INFO[ii].vendor_name != NULL ; ii++)
   {
      memset(info_file, 0, MAXPATHLEN);
      strcat(info_file, "/etc/");
      strcat(info_file, LINUX_VENDOR_INFO[ii].determining_filename);

      if (!stat(info_file, &statBuf))
      {
         s.assign(LINUX_VENDOR_INFO[ii].vendor_name);
         s += " Distribution, ";
         if (LINUX_VENDOR_INFO[ii].optional_string == NULL)
         {
            vf = fopen(info_file, "r");
            if (vf)
            {
              if (fgets(buffer, MAXPATHLEN, vf) != NULL)
                  s += buffer;
               fclose(vf);
            }
         }
         else
         {
            s += LINUX_VENDOR_INFO[ii].optional_string;
         }
         s += " Release";
      }
   }
   otherTypeDescription.assign(s);
   return true;
}


static CIMDateTime time_t_to_CIMDateTime(time_t *time_to_represent)
{
   const int CIM_DATE_TIME_ASCII_LEN = 256;
   const CIMDateTime NULLTIME;

   CIMDateTime dt;
   char date_ascii_rep[CIM_DATE_TIME_ASCII_LEN];
   char utc_offset[20];
   struct tm broken_time;

   dt = NULLTIME;
   localtime_r(time_to_represent, &broken_time);
   if (strftime(date_ascii_rep, CIM_DATE_TIME_ASCII_LEN,
                "%Y%m%d%H%M%S.000000", &broken_time))
   {
#if defined (PEGASUS_PLATFORM_LINUX_IX86_GNU) ||  \
    defined (PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
      //-- the following makes use of a GNU extension
      snprintf(utc_offset, 20, "%+03ld", broken_time.tm_gmtoff / 60);
#else
      snprintf(utc_offset, 20, "%+03ld", 0);
#endif
      strncat(date_ascii_rep, utc_offset, CIM_DATE_TIME_ASCII_LEN);
      dt = date_ascii_rep;
   }

   return dt;
}

/**
   getLastBootUpTime method for Linux implementation of OS Provider

   gets information from /proc/uptime file
  */

Boolean OperatingSystem::getLastBootUpTime(CIMDateTime& lastBootUpTime)
{
   const CIMDateTime NULLTIME;
   const char *UPTIME_FILE = "/proc/uptime";

   CIMDateTime dt;
   FILE *procfile;
   unsigned long seconds_since_boot;
   char read_buffer[MAXPATHLEN];
   time_t t_now, t_then;
   struct tm tm_now;

   dt = NULLTIME;
   procfile = fopen(UPTIME_FILE, "r");
   if (procfile)
   {
      if (fgets(read_buffer, MAXPATHLEN, procfile))
         if (sscanf(read_buffer, " %lu.", &seconds_since_boot))
         {
            //-- convert displacement in seconds to a date and time
            t_now = time(NULL);
            localtime_r(&t_now, &tm_now);
            tm_now.tm_sec -= seconds_since_boot;
            t_then = mktime(&tm_now);
            dt = time_t_to_CIMDateTime(&t_then);
         }
      fclose(procfile);
   }
   lastBootUpTime = dt;
   return true;
}

/**
   getLocalDateTime method for Linux implementation of OS Provider

   Currently calls time to get local time, should be changed to use
   the CIMOM date time and be consistent across all time properties
   (e.g., LastBootUpTime + SystemUpTime = LocalDateTime)
  */

Boolean OperatingSystem::getLocalDateTime(CIMDateTime& localDateTime)
{
   time_t now;

// ATTN-SLC-P2-17-Apr-02: should convert from time to use of CIMOM datetime
   now = time(NULL);
   localDateTime = time_t_to_CIMDateTime(&now);
   return true;
}

/**
   getCurrentTimeZone method for Linux implementation of OS Provider

   gets timezone from gmt offset (only if #define
   PEGASUS_PLATFORM_LINUX_IX86_GNU) - why?
  */

Boolean OperatingSystem::getCurrentTimeZone(Sint16& currentTimeZone)
{
   struct tm buf;
   time_t now;

// check vs. HP-UX implementation - can't use the same?

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
   now = time(NULL);
   localtime_r(&now, &buf);
   currentTimeZone = (buf.tm_gmtoff / 60);
   return true;
#else
   return false;
#endif
}

/**
   getNumberOfLicensedUsers method for Linux implementation of OS Provider

   Always returns 0 for unlimited
  */
Boolean OperatingSystem::getNumberOfLicensedUsers(Uint32& numberOfLicensedUsers)
{
   // According to the MOF, if it's unlimited, use zero
   numberOfLicensedUsers = 0;
   return true;
}

/**
   getNumberOfUsers method for Linux implementation of OS Provider

   Goes through the utents, counting the number of type USER_PROCESS
  */

Boolean OperatingSystem::getNumberOfUsers(Uint32& numberOfUsers)
{
    struct utmpx * utmpp;

    numberOfUsers = 0;
    
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
   getNumberOfProcesses method for Linux implementation of OS Provider

   Counts the number of sub-directories of /proc that are of the format
   to represent processes.
  */
Boolean OperatingSystem::getNumberOfProcesses(Uint32& numberOfProcesses)
{
   Uint32 count;
   DIR *procdir;
   struct dirent entry, *result;
   regex_t process_pattern_compiled;
   const char process_pattern[] = "^[1-9][0-9]*$";

   count = 0;
   if ((procdir = opendir("/proc")))
   {
      if (regcomp(&process_pattern_compiled, process_pattern, 0) == 0)
      {
         while (readdir_r(procdir, &entry, &result) == 0 && result != NULL)
         {
#if defined (PEGASUS_PLATFORM_LINUX_IX86_GNU)
            if (entry.d_type != DT_DIR)
               continue;
#endif
            if (regexec(&process_pattern_compiled, entry.d_name,
                        0, NULL, 0) == 0)
               count++;
         }
         regfree(&process_pattern_compiled);
      }
      closedir(procdir);
   }

   numberOfProcesses = count;
   return true;
}

/**
   getMaxNumberOfProcesses method for Linux implementation of OS Provider

   gets information from /proc/sys/kernel/threads-max
  */

Boolean OperatingSystem::getMaxNumberOfProcesses(Uint32& mMaxProcesses)
{
   //-- prior to 2.4.* kernels, this will not work.  also, this is
   //   technically the maximum number of threads allowed; since
   //   linux has no notion of kernel-level threads, this is the
   //   same as the total number of processes allowed.  should
   //   this change, the algorithm will need to change.
   Uint32 count;
   const char proc_file[] = "/proc/sys/kernel/threads-max";
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
            sscanf(buffer, "%u", &count);
         fclose(vf);
      }
    mMaxProcesses = count;
    return true; 
   }
   return false;
}

/**
   getTotalSwapSpaceSize method for Linux implementation of OS Provider

   Linux doesn't have swap space, so return FALSE
  */
Boolean OperatingSystem::getTotalSwapSpaceSize(Uint64& mTotalSwapSpaceSize)
{
    //-- it is a technicality, but, linux does not have swap space;
    //   it has paging space.  Hence, set return false.
    return false;
}
 
/** _totalVM method for Linux implementation of OS Provider

    Gets the information from SwapTotal in /proc/meminfo
    (already in KB).   Is always called to get the TotalVirtualMemory, 
    may be called to get this amount for the MaxProcessMemory size 
    if nothing is found in /proc/sys/vm/overcommit_memoryt.

   Would be more efficient to store this and not call twice
  */

Uint64 OperatingSystem::_totalVM()
{
  Uint64 total;
   const char proc_file[] = "/proc/meminfo";
   char buffer[MAXPATHLEN];
   struct stat statBuf;
   regex_t pattern;
   FILE *vf;

   total = 0;
   if (!stat(proc_file, &statBuf))
   {
      vf = fopen(proc_file, "r");
      if (vf)
      {
         if (regcomp(&pattern, "^SwapTotal:", 0) == 0)
         {
            while (fgets(buffer, MAXPATHLEN, vf) != NULL)
            {
               if (regexec(&pattern, buffer, 0, NULL, 0) == 0)
               {
                  sscanf(buffer, "SwapTotal: %llu kB", &total);
               }
            }
            regfree(&pattern);
         }
      }
      fclose(vf);
   }

   return total;
}

/**
   getTotalVirtualMemorySize method for Linux implementation of OS Provider

   Gets information from SwapTotal in /proc/meminfo
  */
Boolean OperatingSystem::getTotalVirtualMemorySize(Uint64& total)
{
    total = _totalVM();
    if (total) return true;
    else return false;   // possible that we had trouble with file
}

/**
   getFreeVirtualMemorySize method for Linux implementation of OS Provider

   Gets information from SwapFree in /proc/meminfo
  */
Boolean OperatingSystem::getFreeVirtualMemory(Uint64& freeVirtualMemory)
{
   const char proc_file[] = "/proc/meminfo";
   char buffer[MAXPATHLEN];
   struct stat statBuf;
   regex_t pattern;
   FILE *vf;

   freeVirtualMemory = 0;
   if (!stat(proc_file, &statBuf))
   {
      vf = fopen(proc_file, "r");
      if (vf)
      {
         if (regcomp(&pattern, "^SwapFree:", 0) == 0)
         {
            while (fgets(buffer, MAXPATHLEN, vf) != NULL)
            {
               if (regexec(&pattern, buffer, 0, NULL, 0) == 0)
               {
                  sscanf(buffer, "SwapFree: %llu kB", &freeVirtualMemory);
               }
            }
            regfree(&pattern);
         }
      }
      fclose(vf);
      if (freeVirtualMemory) return true;  // did get info
      else return false;       // didn't get info
   }
   return false;
}

/**
   getFreePhysicalMemory method for Linux implementation of OS Provider

   Gets information from MemFree in /proc/meminfo
  */
Boolean OperatingSystem::getFreePhysicalMemory(Uint64& total)
{
   const char proc_file[] = "/proc/meminfo";
   char buffer[MAXPATHLEN];
   struct stat statBuf;
   regex_t pattern;
   FILE *vf;

   total = 0;
   if (!stat(proc_file, &statBuf))
   {
      vf = fopen(proc_file, "r");
      if (vf)
      {
         if (regcomp(&pattern, "^MemFree:", 0) == 0)
         {
            while (fgets(buffer, MAXPATHLEN, vf) != NULL)
            {
               if (regexec(&pattern, buffer, 0, NULL, 0) == 0)
               {
                  sscanf(buffer, "MemFree: %llu kB", &total);
               }
            }
            regfree(&pattern);
         }
      }
      fclose(vf);
      if (total) return true;  // did get info
      else return false;       // didn't get info
   }
   return false;
}

/**
   getTotalVisibleMemorySize method for Linux implementation of OS Provider
 
   Was returning FreePhysical - correct? diabled it.
  */
Boolean OperatingSystem::getTotalVisibleMemorySize(Uint64& memory)
{
    return false;
}

/**
   getSizeStoredInPagingFiles method for Linux implementation of OS Provider
 
   Was returning TotalSwap - correct? diabled it.
  */
Boolean OperatingSystem::getSizeStoredInPagingFiles(Uint64& total)
{
    return false;
}
 
/**
   getFreeSpaceInPagingFiles method for Linux implementation of OS Provider
 
   Was returning TotalVirtualMemory - correct? diabled it.
  */
Boolean OperatingSystem::getFreeSpaceInPagingFiles(
                                              Uint64& freeSpaceInPagingFiles)
{
    return false;
}
/**
   getMaxProcessMemorySize method for Linux implementation of OS Provider

   Gets information from /proc/sys/vm/overcommit_memoryt or returns
   TotalVirtualMemory
   */
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
   if (count) {
      maxProcessMemorySize = count;
   }
   else {
//ATTN-SLC-P3-18-Apr-02: Optimization?  get this once & share
      maxProcessMemorySize = _totalVM();
   }
   return true;
}

 /**
   getDistributed method for Linux implementation of OS Provider

   Always sets the distributed boolean to FALSE
  */
Boolean OperatingSystem::getDistributed(Boolean& distributed)
{
    distributed = false;
    return true;
}

/**
   getMaxProcsPerUser method for Linux implementation of OS Provider

   Retrieves the _SC_CHILD_MAX value from sysconf.
  */
Boolean OperatingSystem::getMaxProcsPerUser (Uint32& maxProcsPerUser)
{
    return sysconf(_SC_CHILD_MAX);
}
     
/**
   getSystemUpTime method for Linux implementation of OS Provider

   Gets information from /proc/uptime (already in seconds).
  */
Boolean OperatingSystem::getSystemUpTime(Uint64& mUpTime)
{
   const char *UPTIME_FILE = "/proc/uptime";
   FILE *procfile;
   char read_buffer[MAXPATHLEN];
   long uptime;

//ATTN-SLC-P3-18-Apr-02: Optimization?  get this once & share
   procfile = fopen(UPTIME_FILE, "r");
   if (procfile)
   {
      if (fgets(read_buffer, MAXPATHLEN, procfile))
         if (sscanf(read_buffer, " %lu.", &uptime))
         {
         mUpTime = uptime;
         return true;
         }
        fclose(procfile);
   }
   return false;
}

Boolean OperatingSystem::getOperatingSystemCapability(String& scapability)
{
    return false;
}
/**
   _reboot method for Linux implementation of OS Provider

   Finds executable in /sbin, /usr/bin, or /usr/local/sbin and invokes.

   Invokes as via system system call, so have full checking of user's
   authorization (already authenticated by CIMOM)

   Don't we want to do some additional cleanup before actually 
   invoking the reboot command?  For example, we know the CIMOM is up
   and running.  Perhaps set the OS state to 'Stopping' and do a 
   graceful shutdown of the CIMOM (at least)?

   */
Uint32 OperatingSystem::_reboot()
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
/**
   _shutdown method for Linux implementation of OS Provider

   Finds executable in /sbin, /usr/bin, or /usr/local/sbin and invokes.

   Invokes as via system system call, so have full checking of user's
   authorization (already authenticated by CIMOM)


   Don't we want to add some more cleanup - especially since we know
   the CIMOM is running - this could cause things to be set into a
   'Stopping' state while the OS cleans up before actually invoking
   the poweroff command.
   */
Uint32 OperatingSystem::_shutdown()
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

