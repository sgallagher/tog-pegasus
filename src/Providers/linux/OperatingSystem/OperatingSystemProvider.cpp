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
// Author: Al Stone <ahs3@fc.hp.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              David Eger          <dteger@us.ibm.com>
//
//%////////////////////////////////////////////////////////////////////////////


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
#include <unistd.h>
#include <time.h>
#include <utmp.h>
#include <regex.h>
#include <dirent.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define DEBUG(X) // Logger::put(Logger::DEBUG_LOG, "Linux_OperatingSystemProvider", Logger::INFORMATION, "$0", X)

OperatingSystemProvider::OperatingSystemProvider(void)
{
}

OperatingSystemProvider::~OperatingSystemProvider(void)
{
}

void
OperatingSystemProvider::getInstance(const OperationContext& context,
				     const CIMObjectPath& ref,
				     const Boolean includeQualifiers,
				     const Boolean includeClassOrigin,
				     const CIMPropertyList& propertyList,
				     InstanceResponseHandler &handler)
{
   Array<CIMKeyBinding> keys;
   CIMInstance instance;

   DEBUG("losp-> starting up getInstance");

   //-- make sure we're working on the right class
   if (ref.getClassName().equal(OPERATINGSYSTEMCLASSNAME))
      throw CIMException(CIM_ERR_INVALID_PARAMETER);

   DEBUG("losp-> getInstance right class name");

   //-- make sure we're the right instance
   int keyCount;
   CIMName keyName;

   keyCount = 4;
   keys = ref.getKeyBindings();

   if ((unsigned int)keys.size() < (unsigned int)keyCount)
      throw CIMException(CIM_ERR_INVALID_PARAMETER);

   DEBUG("losp-> getInstance right number of keys");
   
   for (unsigned int ii = 0; ii < keys.size(); ii++)
   {
      keyName = keys[ii].getName();

      if (keyName.equal("CSCreationClassName") &&
	  String::equal(keys[ii].getValue(), "CIM_UnitaryComputerSystem"))
      {
         keyCount--;
      }
      else if (keyName.equal("CSName") &&
	       String::equal(keys[ii].getValue(), _hostName()))
      {
         keyCount--;
      }
      else if (keyName.equal("CreationClassName") &&
	       String::equal(keys[ii].getValue(), OPERATINGSYSTEMCLASSNAME))
      {
         keyCount--;
      }
      else if (keyName.equal("Name") &&
	       String::equal(keys[ii].getValue(), _osName()))
      {
         keyCount--;
      }
      else
      {
         throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }
   
   if (keyCount)
      throw CIMException(CIM_ERR_INVALID_PARAMETER);

   DEBUG("losp-> getInstance got the right keys");

   //-- fill 'er up...
   instance = _build_instance(OPERATINGSYSTEMCLASSNAME);

   DEBUG("losp-> getInstance built an instance");

   handler.processing();
   handler.deliver(instance);
   handler.complete();

   DEBUG("losp-> getInstance done");
   return;
}

void 
OperatingSystemProvider::enumerateInstances(
      				const OperationContext& context, 
			        const CIMObjectPath& ref, 
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
			        const CIMPropertyList& propertyList,
			        InstanceResponseHandler& handler)
{
   CIMInstance instance;

   handler.processing();
   instance = _build_instance(OPERATINGSYSTEMCLASSNAME);
   handler.deliver(instance);
   handler.complete();

   return;
}

void 
OperatingSystemProvider::enumerateInstanceNames(
      				const OperationContext& context,
			  	const CIMObjectPath &ref,
			  	ObjectPathResponseHandler& handler )
{
   CIMObjectPath newref;

   newref = _fill_reference(ref.getNameSpace(), OPERATINGSYSTEMCLASSNAME);
   handler.processing();
   handler.deliver(newref);
   handler.complete();

   return;
}

void 
OperatingSystemProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
   throw CIMNotSupportedException(OPERATINGSYSTEMCLASSNAME"::modifyInstance");
}

void 
OperatingSystemProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
   throw CIMNotSupportedException(OPERATINGSYSTEMCLASSNAME"::createInstance");
}

void 
OperatingSystemProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler& handler )
{
   throw CIMNotSupportedException(OPERATINGSYSTEMCLASSNAME"::deleteInstance");
}

void OperatingSystemProvider::initialize(CIMOMHandle& handle)
{
}


void OperatingSystemProvider::terminate(void)
{
   delete this;
}


CIMInstance
OperatingSystemProvider::_build_instance(const String &className) 
{
   CIMInstance instance(className);

   //-- fill in all the blanks
   instance.addProperty(CIMProperty("CSCreationClassName",
	                            String("CIM_UnitaryComputerSystem")));

   instance.addProperty(CIMProperty("CSName", _hostName()));

   instance.addProperty(CIMProperty("CreationClassName",
	                            String(OPERATINGSYSTEMCLASSNAME)));

   instance.addProperty(CIMProperty("Name", _osName()));

   instance.addProperty(CIMProperty("OSType", _osType()));

   instance.addProperty(CIMProperty("OtherTypeDescription", _otherTypeDesc()));

   instance.addProperty(CIMProperty("Version", _version()));

   instance.addProperty(CIMProperty("LastBootUpTime", _lastBoot()));

   instance.addProperty(CIMProperty("LocalDateTime", _localDateTime()));

   instance.addProperty(CIMProperty("CurrentTimeZone", _timeZone()));

   instance.addProperty(CIMProperty("NumberOfLicensedUsers", _licensedUsers()));

   instance.addProperty(CIMProperty("NumberOfUsers", _users()));

   instance.addProperty(CIMProperty("NumberOfProcesses", _processes()));

   instance.addProperty(CIMProperty("MaxNumberOfProcesses", _maxProcesses()));

   instance.addProperty(CIMProperty("TotalSwapSpaceSize", _totalSwap()));

   instance.addProperty(CIMProperty("TotalVirtualMemorySize", _totalVM()));

   instance.addProperty(CIMProperty("FreeVirtualMemory", _freeVM()));

   instance.addProperty(CIMProperty("FreePhysicalMemory", _freePhysical()));

   instance.addProperty(CIMProperty("TotalVisibleMemorySize", _totalVisible()));

   instance.addProperty(CIMProperty("SizeStoredInPagingFiles", _totalPaging()));

   instance.addProperty(CIMProperty("FreeSpaceInPagingFiles", _freePaging()));

   instance.addProperty(CIMProperty("MaxProcessMemorySize", _maxProcessMem()));

   instance.addProperty(CIMProperty("Distributed", _distributed()));

   instance.addProperty(CIMProperty("MaxProcessesPerUser", _maxUserProcs()));

   return instance;
}

CIMObjectPath 
OperatingSystemProvider::_fill_reference(const CIMNamespaceName &nameSpace,
				         const String &className)
{
   Array<CIMKeyBinding> keys;

   keys.append(CIMKeyBinding("CSCreationClassName", 
	                  "CIM_UnitaryComputerSystem",
			  CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("CSName", _hostName(), CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("CreationClassName", className, CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("Name", _osName(), CIMKeyBinding::STRING));

   return CIMObjectPath(_hostName(), nameSpace, className, keys);
}

String OperatingSystemProvider::_hostName()
{
   String s;
   struct utsname uts;

   if (uname(&uts) == 0)
   {
      s.assign(uts.nodename);
   }
   else
   {
      s.assign("unknown");
   }
   return s;
}

String OperatingSystemProvider::_osName()
{
   String s;
   struct utsname uts;

   if (uname(&uts) == 0)
   {
      s.assign(uts.sysname);
   }
   else
   {
      s.assign("unknown");
   }
   return s;
}

String OperatingSystemProvider::_version()
{
   String s;
   struct utsname uts;

   if (uname(&uts) == 0)
   {
      s.assign(uts.release);
      s.append(" ");
      s.append(uts.version);
   }
   else
   {
      s.assign("unknown");
   }
   return s;
}

Uint16 OperatingSystemProvider::_osType()
{
   const Uint16 CIM26_OSType_LINUX = 36;  //-- "LINUX"
   return CIM26_OSType_LINUX;
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

String OperatingSystemProvider::_otherTypeDesc()
{
   String s;
   char info_file[MAXPATHLEN];
   char buffer[MAXPATHLEN];
   struct stat statBuf;
   FILE *vf;

   for (int ii = 0; LINUX_VENDOR_INFO[ii].vendor_name != NULL ; ii++)
   {
      memset(info_file, 0, MAXPATHLEN);
      strcat(info_file, "/etc/");
      strcat(info_file, LINUX_VENDOR_INFO[ii].determining_filename);

      if (!stat(info_file, &statBuf))
      {
	 s.assign(LINUX_VENDOR_INFO[ii].vendor_name);
	 s.append(" Distribution, ");
	 if (LINUX_VENDOR_INFO[ii].optional_string == NULL)
	 {
	    vf = fopen(info_file, "r");
	    if (vf)
	    {
	       if (fgets(buffer, MAXPATHLEN, vf) != NULL)
		  s.append(buffer);
	       fclose(vf);
	    }
	 }
	 else
	 {
	    s.append(LINUX_VENDOR_INFO[ii].optional_string);
	 }
	 s.append(" Release");
      }
   }

   return s;
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
#if defined (PEGASUS_PLATFORM_LINUX_IX86_GNU) || defined (PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
      //-- the following makes use of a GNU extension
      snprintf(utc_offset, 20, "%+03ld", broken_time.tm_gmtoff / 60);
#else
      snprintf(utc_offset, 20, "%+03ld", 0);
#endif
      strncat(date_ascii_rep, utc_offset, CIM_DATE_TIME_ASCII_LEN);
      dt = String(date_ascii_rep);
      cout << "dt = " << dt.toString() << endl;
   }

   return dt;
}

CIMDateTime OperatingSystemProvider::_lastBoot()
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

   return dt;
}

CIMDateTime OperatingSystemProvider::_localDateTime()
{
   time_t now;

   now = time(NULL);
   return time_t_to_CIMDateTime(&now);
}

Sint16 OperatingSystemProvider::_timeZone()
{
   struct tm buf;
   time_t now;

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
   now = time(NULL);
   localtime_r(&now, &buf);
   return (buf.tm_gmtoff / 60);
#else
   return 0;
#endif
}

Uint32 OperatingSystemProvider::_licensedUsers()
{
   //-- according to the mof, if it's unlimited, use zero
   return 0;
}

Uint32 OperatingSystemProvider::_users()
{
   Uint32 result;
   set<String> user_names;
   String s;
   struct utmp *p;

   result = 0;
   utmpname(UTMP_FILENAME);
   setutent();
   while ((p = getutent()) != NULL)
   {
      if (p->ut_type == USER_PROCESS)
      {
	 s = p->ut_user;
	 user_names.insert(s);
      }
   }
   endutent();
   
   result = user_names.size();
   user_names.clear();

   return result;
}

Uint32 OperatingSystemProvider::_processes()
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
#if defined (PEGASUS_PLATFORM_LINUX_IX86_GNU) || defined (PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
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

   return count;
}

Uint32 OperatingSystemProvider::_maxProcesses()
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
   }

   return count;
}

Uint64 OperatingSystemProvider::_totalSwap()
{
   //-- it is a technicality, but, linux does not have swap space;
   //   it has paging space.  hence, set this to 0.
   return 0;
}

Uint64 OperatingSystemProvider::_totalVM()
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

Uint64 OperatingSystemProvider::_freeVM()
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
	 if (regcomp(&pattern, "^SwapFree:", 0) == 0)
	 {
	    while (fgets(buffer, MAXPATHLEN, vf) != NULL)
	    {
	       if (regexec(&pattern, buffer, 0, NULL, 0) == 0)
	       {
		  sscanf(buffer, "SwapFree: %llu kB", &total);
	       }
	    }
	    regfree(&pattern);
	 }
      }
      fclose(vf);
   }

   total += _freePhysical();

   return total;
}

Uint64 OperatingSystemProvider::_freePhysical()
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
   }

   return total;
}

Uint64 OperatingSystemProvider::_totalVisible()
{
   return _freePhysical();
}

Uint64 OperatingSystemProvider::_totalPaging()
{
   return _totalSwap();
}

Uint64 OperatingSystemProvider::_freePaging()
{
   return _freeVM();
}

Uint64 OperatingSystemProvider::_maxProcessMem()
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

   return ((count > 0) ? count : _totalVM());
}

Boolean OperatingSystemProvider::_distributed()
{
   //-- since we have no real way of knowing whether or not
   //   we're in a cluster, report that we are *not* distributed
   return false;
}

Uint32 OperatingSystemProvider::_maxUserProcs()
{
   return sysconf(_SC_CHILD_MAX);
}

Uint32 OperatingSystemProvider::Reboot()
{
   const char *reboot[] = { "reboot", NULL };
   const char *paths[] = { "/sbin", "/usr/sbin", "/usr/local/sbin", NULL };
   struct stat sbuf;
   String fname;
   CString p;
   Uint32 result;

   result = 1;
   for (int ii = 0; paths[ii] != NULL; ii++)
   {
      for (int jj = 0; reboot[jj]; jj++)
      {
	 fname = paths[ii];
	 fname.append("/");
	 fname.append(reboot[jj]);
	 p = fname.getCString();
	 if (stat(p, &sbuf) == 0 && (sbuf.st_mode & S_IXUSR))
	 {
	    result = 2;
	    if (system(p) == 0)
	       result = 0;

	    return result;
	 }
      }
   }
   return result;
}

Uint32 OperatingSystemProvider::Shutdown()
{
   const char *poweroff[] = { "poweroff", NULL };
   const char *paths[] = { "/sbin", "/usr/sbin", "/usr/local/sbin", NULL };
   struct stat sbuf;
   String fname;
   CString p;
   Uint32 result;

   result = 1;
   for (int ii = 0; paths[ii] != NULL; ii++)
   {
      for (int jj = 0; poweroff[jj]; jj++)
      {
	 fname = paths[ii];
	 fname.append("/");
	 fname.append(poweroff[jj]);
	 p = fname.getCString();
	 if (stat(p, &sbuf) == 0 && (sbuf.st_mode & S_IXUSR))
	 {
	    result = 2;
	    if (system(p) == 0)
	       result = 0;

	    return result;
	 }
      }
   }
   return result;
}

void OperatingSystemProvider::invokeMethod(
      				const OperationContext& context,
    				const CIMObjectPath& objectReference,
				const CIMName& methodName,
				const Array<CIMParamValue>& inParameters,
				MethodResultResponseHandler& handler)
{
   throw CIMNotSupportedException(OPERATINGSYSTEMCLASSNAME"::invokeMethod");
}

PEGASUS_NAMESPACE_END

