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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              Jim Metcalfe
//              Carlos Bonilla
//              Mike Glantz         <michael_glantz@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

/* ==========================================================================
   Includes.
   ========================================================================== */

#include "Process.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


Process::Process()
{
}

Process::~Process()
{
}

/*
================================================================================
NAME              : getCaption
DESCRIPTION       : returns pst_ucomm, the "executable basename the
                  : process is running" (so no path info)
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getCaption(String& s) const
{
  s = pInfo.pst_ucomm;
  return true;
}

/*
================================================================================
NAME              : getDescription
DESCRIPTION       : returns pst_cmd (command line) as a string (contrast
                  : this with ModulePath and Parameters below)
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getDescription(String& s) const
{
  s = pInfo.pst_cmd;
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
  // not supported
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
  // not supported, but see getExecutionState below
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
  // rather than the actual command line, since this is a
  // reliable command, unlike the command line
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
  switch (pInfo.pst_stat)
  {
  case PS_SLEEP:
    i16 = 6; // Suspended Ready
    break;

  case PS_RUN:
    i16 = 3;
    break;

  case PS_STOP:
    i16 = 8;
    break;

  case PS_ZOMBIE:
    i16 = 1; // need to coordinate this with OtherExecutionDescription
    break;

  case PS_IDLE:
    i16 = 2; // we are saying this is Ready
    break;

  case PS_OTHER:
    i16 = 1;
    break;

  default:
    i16 = 0; // unknown
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
  case PS_ZOMBIE:
    s = "Zombie";
    break;
  
  case PS_OTHER:
    s = "Other";
    break;
  
  default:
    s = String::EMPTY; // ExecutionState is not Other
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
  // convert time to a usable format
  struct tm *t = localtime((time_t*)&pInfo.pst_start);
  // convert to CIMDateTime format
  char timstr[26];
  sprintf(timstr,"%04d%02d%02d%02d%02d%02d.000000%c%03d",t->tm_year+1900,
                       t->tm_mon+1,
                       t->tm_mday,
                       t->tm_hour,
                       t->tm_min,
                       t->tm_sec,
                       (timezone>0)?'-':'+',
                       timezone/60 - (t->tm_isdst? 60:0));
  d = timstr;
  return true;
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
  // ATTN: The following conversion from major/minor returned
  // by pstat_getproc() to deviceID used by devnm() may
  // not be supported
  
  if (pInfo.pst_major == -1 && pInfo.pst_minor == -1)
  {
    s = "?";
    return true;
  }

  dev_t devID = (pInfo.pst_major << 24) + pInfo.pst_minor;
  char result[132];
  int rc = devnm(S_IFCHR, devID, result, sizeof(result)-1, 1);
  s = result;
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
  // ATTN: Not sure how to get this, because the first token
  // of the command line is not guaranteed to be the path, and
  // there is really no way to get the path of the image being
  // executed by a process (but we will return the entire command
  // string as the Parameters property below)
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
  // ATTN: we will return the full command line (see ModulePath above)

  // start with p at beginning of string
  // q is position of first blank after p
  // loop doesn't execute if q==0
  // otherwise, assigns as[i] to string starting at p and
  // going for q-p characters
  // p then set to point past blank for next iteration

  const char *p=pInfo.pst_cmd;
  const char *q;
  for ( ; q=strchr(p, ' '); p=q+1) as.append(String(p, q-p));

  // when no more blanks found, stick what's left into last
  // element of array
  as.append(String(p));
  return true;
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
  i32 = pInfo.pst_pctcpu * 100;
  return true;
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
  i64 = pInfo.pst_ssize * getpagesize() / 1024;
  return true;
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
  i64 = pInfo.pst_vtsize * getpagesize() / 1024;
  return true;
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
// ATTN this field seems not to be available
#ifdef _RUSAGE_EXTENDED
  // value to be returned in clock ticks, not time
  i64 = ((Uint64)pInfo.pst_child_usercycles.psc_hi << 32) +
          (Uint64)pInfo.pst_child_usercycles.psc_lo +
        ((Uint64)pInfo.pst_child_systemcycles.psc_hi << 32) +
          (Uint64)pInfo.pst_child_systemcycles.psc_lo +
        ((Uint64)pInfo.pst_child_interruptcycles.psc_hi << 32) +
          (Uint64)pInfo.pst_child_interruptcycles.psc_lo;
  return true;
#else
  // not supported
  return false;
#endif
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
#ifdef _RUSAGE_EXTENDED
  // these values are to be returned in clock ticks, not
  // time
  i64 = ((Uint64)pInfo.pst_child_systemcycles.psc_hi << 32) +
          (Uint64)pInfo.pst_child_systemcycles.psc_lo;
  return true;
#else
  // not supported
  return false;
#endif
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
  char buf[100];
  sprintf(buf,"%d",pInfo.pst_ppid);
  s = buf;
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
  // total up all pages used, compute Kbyte (1024 = 1K)
  i64 = (pInfo.pst_dsize +  // real data
         pInfo.pst_tsize +  // real text
         pInfo.pst_ssize)   // real stack
        * getpagesize() / 1024;
  return true;
}

/*
================================================================================
NAME              : getHandle
DESCRIPTION       : 
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
  return buf;
}

/*
================================================================================
NAME              : getCSName
DESCRIPTION       : Platform-specific routine to get CSName
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
String Process::getCSName(void) const
{
  struct hostent *he;
  char hn[MAXHOSTNAMELEN];

  // fill hn with what this system thinks is name
  gethostname(hn,MAXHOSTNAMELEN);

  // find out what nameservices think is full name
  if (he=gethostbyname(hn)) return he->h_name;

  // but if that failed, return what gethostname said
  else return hn;
}

/*
================================================================================
NAME              : getOSName
DESCRIPTION       : Platform-specific routine to get OSName
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
String Process::getOSName(void) const
{
  struct utsname unameInfo;

  /* Call uname, handle errors */ 
  if (uname(&unameInfo) < 0)
  {
    throw CIMException(CIM_ERR_FAILED);
  }
  return unameInfo.sysname;
}

/*
================================================================================
NAME              : getProcessInfo
DESCRIPTION       : Use pstat_getproc() to fill in a struct pst_status
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean Process::getProcessInfo(int &pIndex)
{
  // This routine fills in the protected member pInfo by calling
  // pstat_getproc. If modifies pIndex so that a subsequent call
  // to this routine will fetch the next process. getProcessInfo
  // is therefore functionally equivalent to pstat_getproc(),
  // except that it hides the platform-specific structure pst_status.
  // It returns true is it succeeded in fetching a process, otherwise
  // false, incidating that there are no more processes to be fetched.

  // pstat_getproc takes an empty struct to fill in, each element's size,
  // the number of elements (0 if last arg is pid instead of index),
  // and an index to start at

  int stat = pstat_getproc(&pInfo, sizeof(pInfo), 1, pIndex);

  // pstat_getproc returns number of returned structures
  if (stat != 1) return false;

  pIndex = pInfo.pst_idx + 1;
  return true;
}

PEGASUS_NAMESPACE_END
