//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//==============================================================================
//
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By:
//         David Kennedy       <dkennedy@linuxcare.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//         Al Stone, Hewlett-Packard Company <ahs3@fc.hp.com>
//         Jim Metcalfe, Hewlett-Packard Company
//         Carlos Bonilla, Hewlett-Packard Company
//         Mike Glantz, Hewlett-Packard Company <michael_glantz@hp.com>
//         Chad Smith, Hewlett-Packard Company <chad_smith@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

/* ==========================================================================
   Includes.
   ========================================================================== */

#include "ProcessPlatform.h"
#include <time.h>			// for time(0)
#include <sys/param.h>			// for MAXPATHLEN
#include <netinet/in.h>
#include <netdb.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


Process::Process()
{
}

Process::~Process()
{
}

/*
================================================================================
NAME              : getCaption
DESCRIPTION       : currently returning the basename of the process 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getCaption(String& s) const
{
  s.assign(pInfo.pst_ucomm);
  return true;
}

/*
================================================================================
NAME              : getDescription
DESCRIPTION       : currently only returns the same thing as getCaption
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getDescription(String& s) const
{
  s.assign(pInfo.pst_cmd);
  return true;
}

/*
================================================================================
NAME              : getInstallDate
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getInstallDate(CIMDateTime& d) const
{
  // doesn't make much sense for a process
  return false;
}

/*
================================================================================
NAME              : getStatus
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getStatus(String& s) const
{
  // not supported check out executionState instead
  return false;
}

/*
================================================================================
NAME              : getName
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getName(String& s) const
{
  // We will return the basename of the executable image,
  // rather than the actual command line, since this is
  // reliably the name of an executable image, unlike the
  // first token on the command line, which could be an
  // alias, or could even be something completely irrelevant
  s = pInfo.pst_ucomm;
  return true;
}

/*
================================================================================
NAME              : getPriority
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getPriority(Uint32& i32) const
{
  i32 = pInfo.pst_pri;
  return true;
}

/*
================================================================================
NAME              : getExecutionState
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getExecutionState(Uint16& i16) const
{
  /* 
     From the MOF for this class:
      [Description (
        "Indicates the current operating condition of the Process. "
        "Values include ready (2), running (3), and blocked (4), "
        "among others."),
       Values {"Unknown", "Other", "Ready", "Running", 
               "Blocked", "Suspended Blocked", "Suspended Ready", 
               "Terminated", "Stopped", "Growing" },
   */

  enum
  { Unknown,
    Other,
    Ready,
    Running,
    Blocked,
    Suspended_Blocked,
    Suspended_Ready,
    Terminated,
    Stopped,
    Growing
  };

  switch (pInfo.pst_stat)
  {
  case 'S':
    i16 = Suspended_Ready;
    break;

  case 'R':
    i16 = Running;
    break;

  case 'T':
    i16 = Stopped;
    break;

  case 'Z':
    i16 = Other; // this is coordinated with OtherExecutionDescription
    break;

  case 'I':
    i16 = Ready;
    break;

  case 'O':
    i16 = Other; // This is coordinated with OtherExecutionDescription
    break;

  default:
    i16 = Unknown;
  }
  return true;

}

/*
================================================================================
NAME              : getOtherExecutionDescription
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getOtherExecutionDescription(String& s) const
{
  switch (pInfo.pst_stat)
  {
  case 'Z':
    s = "Zombie";
    break;
  
  case 'O':
    s = "Other";
    break;
  
  default:
    s = String::EMPTY; // ExecutionState is not Other
    // In this case, the caller must know to set the
    // property value to NULL (XML: no <VALUE> element)
  }

  return true;
}

/*
================================================================================
NAME              : getCreationDate
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getCreationDate(CIMDateTime& d) const
{
  return false;
}

/*
================================================================================
NAME              : getTerminationDate
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getTerminationDate(CIMDateTime& d) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getKernelModeTime
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getKernelModeTime(Uint64& i64) const
{
  // time obtained in seconds from pstat needs to be
  // returned in milliseconds
  i64 = 1000 * pInfo.pst_stime;
  return true;
}

/*
================================================================================
NAME              : getUserModeTime
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getUserModeTime(Uint64& i64) const
{
  // time obtained in seconds from pstat needs to be
  // returned in milliseconds
  i64 = 1000 * pInfo.pst_utime;
  return true;
}

/*
================================================================================
NAME              : getWorkingSetSize
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getWorkingSetSize(Uint64& i64) const
{
  /*
     From MOF:
      [Gauge, Description (
        "The amount of memory in bytes that a Process needs to "
        "execute efficiently, for an OperatingSystem that uses "
        "page-based memory management.  If an insufficient amount "
        "of memory is available (< working set size), thrashing "
        "will occur.  If this information is not known, NULL or 0 "
        "should be entered.  If this data is provided, it could be "
        "monitored to understand a Process' changing memory "
        "requirements as execution proceeds."),
       Units ("Bytes") ]
   uint64 WorkingSetSize;
   */

  i64 = 0;
  return true;
}

/*
================================================================================
NAME              : getRealUserID
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getRealUserID(Uint64& i64) const
{
  i64 = pInfo.pst_uid;
  return true;
}

/*
================================================================================
NAME              : getProcessGroupID
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getProcessGroupID(Uint64& i64) const
{
  i64 = pInfo.pst_gid;
  return true;
}

/*
================================================================================
NAME              : getProcessSessionID
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getProcessSessionID(Uint64& i64) const
{
  i64 = pInfo.pst_sid;
  return true;
}

/*
================================================================================
NAME              : getProcessTTY
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getProcessTTY(String& s) const
{
  if( pInfo.pst_tty == -1 )
    s = "?";
  else
    s.assign(" " + pInfo.pst_tty);
  return true;
}

/*
================================================================================
NAME              : getModulePath
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getModulePath(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getParameters
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getParameters(Array<String>& as) const
{
  int idx_new, idx_old=0;
  String p = pInfo.pst_cmd;


  while( idx_new != PEG_NOT_FOUND )
  {
    idx_new = pInfo.pst_cmd.find(idx_old, ' ');
    if( idx_new != PEG_NOT_FOUND )
    {
	   p = pInfo.pst_cmd.subString(idx_old,idx_new-idx_old);
	   as.append(String(p));
	   idx_old = idx_new+1;
    }
    else
    {
	p = pInfo.pst_cmd.subString(idx_old);
	as.append(String(p));
	return true;
    }
    
  }
}

/*
================================================================================
NAME              : getProcessNiceValue
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getProcessNiceValue(Uint32& i32) const
{
  i32 = pInfo.pst_nice;
  return true;
}

/*
================================================================================
NAME              : getProcessWaitingForEvent
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getProcessWaitingForEvent(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getCPUTime
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getCPUTime(Uint32& i32) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getRealText
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getRealText(Uint64& i64) const
{
  i64 = pInfo.pst_tsize * getpagesize() / 1024;
  return true;
}

/*
================================================================================
NAME              : getRealData
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getRealData(Uint64& i64) const
{
  i64 = pInfo.pst_dsize * getpagesize() / 1024;
  return true;
}

/*
================================================================================
NAME              : getRealStack
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getRealStack(Uint64& i64) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getVirtualText
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getVirtualText(Uint64& i64) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getVirtualData
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getVirtualData(Uint64& i64) const
{
  i64 = pInfo.pst_vdsize * getpagesize() / 1024;
  return true;
}

/*
================================================================================
NAME              : getVirtualStack
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getVirtualStack(Uint64& i64) const
{
  i64 = pInfo.pst_vssize * getpagesize() / 1024;
  return true;
}

/*
================================================================================
NAME              : getVirtualMemoryMappedFileSize
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getVirtualMemoryMappedFileSize(Uint64& i64) const
{
  i64 = pInfo.pst_vmmsize * getpagesize() / 1024;
  return true;
}

/*
================================================================================
NAME              : getVirtualSharedMemory
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getVirtualSharedMemory(Uint64& i64) const
{
  i64 = pInfo.pst_vshmsize * getpagesize() / 1024;
  return true;
}

/*
================================================================================
NAME              : getCpuTimeDeadChildren
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getCpuTimeDeadChildren(Uint64& i64) const
{
  i64 = pInfo.pst_cutime;
  return true;
}

/*
================================================================================
NAME              : getSystemTimeDeadChildren
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getSystemTimeDeadChildren(Uint64& i64) const
{
  i64 = pInfo.pst_cstime;
  return true;
}

/*
================================================================================
NAME              : getParentProcessID
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getParentProcessID(String& s) const
{
  s.assign(" " + pInfo.pst_ppid);
  return true;
}

/*
================================================================================
NAME              : getRealSpace
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : returns a modified pIndex that can be used in a
                    subsequent call to get next process structure
================================================================================
*/
Boolean Process::getRealSpace(Uint64& i64) const
{
  i64 = (pInfo.pst_dsize +  // real data
         pInfo.pst_tsize)   // real text  may need to figure out how to get
	  		    // realStackSize
        * getpagesize() / 1024;
  return true;
}

/*
================================================================================
NAME              : getHandle
DESCRIPTION       : Call uname() and get the operating system name.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
String Process::getHandle(void) const
{
  char buf[100];
  sprintf(buf,"%d",pInfo.pst_pid);
  return String(buf);
}


/*
================================================================================
NAME              : getCSName
DESCRIPTION       : Platform-specific method to get CSName
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
String Process::getCSName(void) const
{
     char    hostName[MAXHOSTNAMELEN];
     struct  hostent *he;
     String csName;

     if (gethostname(hostName, MAXHOSTNAMELEN) != 0)
     {
         return String::EMPTY;
     }

     // Now get the official hostname.  If this call fails then return
     // the value from gethostname().

     he=gethostbyname(hostName);
     if (he)
     {
        strcpy(hostName, he->h_name);
     }

     csName.assign(hostName);

     return csName;
}

// This table is used by getOSName to determine the exact Linux OS on which
// we are running.
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
/*
================================================================================
NAME              : getOSName
DESCRIPTION       : Platform-specific method to get OSname.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
String Process::getOSName(void) const
{
   String osName, s, buffer_s;
   Uint32 buffer_index;	// rexex match index
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


      // If the file exists in /etc, we know what distro we're in
      if (!stat(info_file, &statBuf))
      {
         s.assign(LINUX_VENDOR_INFO[ii].vendor_name);
         s.append(" Distribution");
         if (LINUX_VENDOR_INFO[ii].optional_string == NULL)
         {
	    // try to set s to a more descript value from the etc file
            vf = fopen(info_file, "r");
            if (vf)
            {
               if (fgets(buffer, MAXPATHLEN, vf) != NULL)
	       {
		  fclose(vf);
                  buffer_s.assign(buffer);
	    
		  // parse the text to extract Distribution Name
		  buffer_index = buffer_s.find(" release");
		  if ( buffer_index != PEG_NOT_FOUND )
		  {
		     // then we have found a valid index into the config file
		     s.assign(buffer_s.subString(0,buffer_index));
		  }
	       }
	    }
         }
      }
   }
   osName.assign(s);
   return osName;

}


/*
================================================================================
NAME              : getCurrentTime
DESCRIPTION       : Platform-specific routine to get a timestamp stat Name key
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
String Process::getCurrentTime(void) const
{
  time_t t = time(0);
  return String(ctime(&t));
}


/*
================================================================================
NAME              : loadProcessInfo
DESCRIPTION       : get process info from system into internal data struct
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::loadProcessInfo(int &pIndex)
{
  // This routine fills in the protected member pInfo by calling
  // get_proc. Because Linux process entries are not contiguous
  // this routine modifies pIndex so that the caller, after
  // incrementing pIndex, will be able to fetch the next process in
  // a subsequent call. 
  // It returns true if it succeeded in fetching a process, otherwise
  // false, incidating that there are no more processes to be fetched.

  // get_proc() an empty peg_proc_t to fill in, an integer ,
  // and flag: either GET_PROC_BY_INDEX or GET_PROC_BY_PID
  // if GET_PROC_BY_INDEX is set, get_proc takes the integer (pIndex)
  // and locates the (pIndex)th process in the proc filesystem.
  // After the call is completed pIndex is incremented.

  // How this works: get_proc walks through the /proc directory and
  // counts as it steps through process dirs within proc. When it
  // encounters the pIndex'th process dir within /proc it instantiates 
  // pInfo with that reference and update pIndex

  // If GET_PROC_BY_PID is set, get_proc will find the process by the pid
  // get_proc(&pInfo, pid, GET_PROC_BY_PID);
  return get_proc(&pInfo, pIndex, GET_PROC_BY_INDEX);

  // get_proc returns true if it successfully located a process dir at the
  // pIndex search if we return a false, we either can't open /proc, which 
  // will be logger, or we are out of processes
}

/*
================================================================================
NAME              : findProcess
DESCRIPTION       : find the requested process and load its data
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::findProcess(const String& handle)
{
  int pIndex;


  // Convert handle to an integer
  int pid = atoi(handle.getCString());

  return get_proc(&pInfo, pid, GET_PROC_BY_PID);
}


/* ==========================================================================
    Functions.
   ========================================================================== */
/*
================================================================================
NAME              : file2str
DESCRIPTION       : opens a file and turns it into a string ad sends the result
		  : back as char *ret
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/

int file2str(char *directory, char *myFile, char *ret, int cap) {
    static char filename[80];
    int fd, num_read;

    sprintf(filename, "%s/%s", directory, myFile);
    if ( (fd       = open(filename, O_RDONLY, 0)) == -1 ) return -1;
    if ( (num_read = read(fd, ret, cap - 1))      <= 0 ) num_read = -1;
    else ret[num_read] = 0;
    close(fd);
    return num_read;
}

/*
================================================================================
NAME              : parseProcStatm
DESCRIPTION       : parses data from within /proc/X/statm extracting for proc X
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void parseProcStatm(char* inputFileString, peg_proc_t* P) {
    int num;
    num = sscanf(inputFileString, "%ld %ld %ld %ld",
	   &P->size, &P->pst_dsize, &P->pst_vshmsize,
	   &P->pst_tsize);
}

/*
================================================================================
NAME              : parseProcStat
DESCRIPTION       : parses data from within /proc/X/stat extracting for proc X
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void parseProcStat(char* inputFileString, peg_proc_t* P) {
    int num;
    long not_used;
    char* tmp = strrchr(inputFileString, ')');
    *tmp = '\0';			
    memset(P->pst_ucomm, 0, sizeof P->pst_ucomm);	
    sscanf(inputFileString, "%d (%15c", &P->pst_pid, P->pst_ucomm); /* ucomm[16] in kernel */
    num = sscanf(tmp + 2,			/* skip space after ')' too */
       "%c "
       "%d %d %d %d %d "
       "%lu %lu %lu %lu %lu %lu %lu "
       "%ld %ld %ld %ld %ld %ld "
       "%lu %lu ",
       &P->pst_stat,
       &P->pst_ppid, &P->pst_pgrp, &P->pst_sid, &P->pst_tty, &not_used,
       &not_used, &not_used, &not_used, &not_used, &not_used, &P->pst_utime, &P->pst_stime,
       &P->pst_cutime, &P->pst_cstime, &P->pst_pri, &P->pst_nice, &not_used, &not_used,
       &P->pst_start, &P->pst_vmmsize);
    
    if (P->pst_tty == 0)
	P->pst_tty = -1;  /* the old notty val, update elsewhere bef. moving to 0 */
}


/*
================================================================================
NAME              : parseProcStatus
DESCRIPTION       : parses data from within /proc/X/status extracting for proc X
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void parseProcStatus (char* inputFileString, peg_proc_t* P) {
    /*
     * sets pst_ucomm , pst_stat, pst_pid, pst_ppid, pst_uid,
     * pst_gid, pst_vmmsize, pst_vdsize, pst_vssize
     *
     */

    char* tmp;

    memset(P->pst_ucomm, 0, sizeof P->pst_ucomm);
    sscanf (inputFileString, "Name:\t%15c", P->pst_ucomm);
    tmp = strchr(P->pst_ucomm,'\n');
    *tmp='\0';
    tmp = strstr (inputFileString,"State");
    sscanf (tmp, "State:\t%c", &P->pst_stat);

    tmp = strstr (inputFileString,"Pid:");
    if(tmp) sscanf (tmp,
        "Pid:\t%d\n"
        "PPid:\t%d\n",
        &P->pst_pid,
        &P->pst_ppid
    );
    else cerr << "sscanf failed. PG_UnixProcess" << endl;

    tmp = strstr (inputFileString,"Uid:");
    if(tmp) sscanf (tmp,
        "Uid:\t%d",
        &P->pst_uid
    );
    else cerr << "sscanf failed. PG_UnixProcess" << endl;

    tmp = strstr (inputFileString,"Gid:");
    if(tmp) sscanf (tmp,
        "Gid:\t%d",
        &P->pst_gid
    );
    else cerr << "sscanf failed. PG_UnixProcess" << endl;

    tmp = strstr (inputFileString,"VmSize:");
    if(tmp) sscanf (tmp,
        "VmSize: %lu kB\n",
        &P->pst_vmmsize
    	);
    else
        P->pst_vmmsize = 0;

    tmp = strstr (inputFileString,"VmData:");
    if(tmp) sscanf (tmp,
        "VmData: %lu kB\n"
        "VmStk: %lu kB\n",
	&P->pst_vdsize,
	&P->pst_vssize
	);
    else /* probably a kernel kernel thread */
    {
        P->pst_vdsize = 0;
	P->pst_vssize = 0;
    }

}


Boolean get_proc(peg_proc_t* P, int &pIndex , Boolean find_by_pid)
{
  static struct dirent *dir;    
  static struct stat stat_buff;	
  static char path[32];
  static char buffer[512];
  int  processId;
  int allocated = 0;
  DIR* procDir;
  int count;

  count = 0;
  if( ! (procDir = opendir("/proc")))
    return false;

   // get rid of any .ZZZ files
   while( ( dir = readdir(procDir)) 
		   && (*dir->d_name < '0' || *dir->d_name > '9'))
	   ;

   do 
   { 
    // make sure we get only dirs that start with [0-9]
    if (*dir->d_name < '0' || *dir->d_name > '9')
    {
	if (procDir) closedir(procDir);
	return false;
    }

    if ( find_by_pid )
    { 
       if ( atoi( dir->d_name ) == pIndex )
         break;
    }
    else
    {
      if ( count == pIndex )
	break;
    }
    count++;
  } while ( dir = readdir(procDir));
  
  if (!dir || !dir->d_name)	// then we've finished lookin at all the procs
  {
    if (procDir) closedir(procDir);
    return false;
  }

  // we now have the right processId and proc table entry
   
  sprintf(path, "/proc/%s", dir->d_name); 
  if (stat(path, &stat_buff) == -1) 		// our process stopped running
  {
    if (procDir) closedir(procDir);
    return false;
  }

  if ((file2str(path, "stat", buffer, sizeof buffer)) == -1)
  {
    if (procDir) closedir(procDir);
    return false;
  }
  parseProcStat(buffer, P);

  if ((file2str(path, "statm", buffer, sizeof buffer)) != -1 )
    parseProcStatm(buffer, P);
     
  if ((file2str(path, "status", buffer, sizeof buffer)) != -1 )
    parseProcStatus(buffer, P);

  if ((file2str(path, "cmdline", buffer, sizeof buffer)) != -1)
	  P->pst_cmd.assign(buffer);

  if (procDir) closedir(procDir);



  pIndex = count + 1;   		// set pIndex to the next "reference"
  return true;
}


