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
// Author: Sean Keenan <sean.keenan@hp.com>
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// Includes.
// ==========================================================================
//


#include "ProcessPlatform.h"

#define MAXITMLST 16
#define MAXHOSTNAMELEN 256

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

// Initialize static class data

struct proc_info *Process::pInfo = NULL;
struct proc_info *Process::pData = NULL;

struct proc_info *pInfo = NULL;

typedef struct
{
  char fill1[568];
  long pcb$l_kernel_counter;
  long pcb$l_exec_counter;
  long pcb$l_super_counter;
  long pcb$l_user_counter;
  char fill2[72];
  long pcb$l_kt_high;
} PCB;

typedef struct
{
  unsigned short wlength;
  unsigned short wcode;
  void *pbuffer;
  void *pretlen;
} item_list;

item_list itmlst3[MAXITMLST];
item_list *itml3 = itmlst3;

char procimgnambuf[256],
     proctermbuf[8],
     usernamebuf[13];

int procimgnamlen,
    proctermlen,
    usernamlen;

unsigned __int64 proclgntim;

long procpid,
  proccputim,
  procgrp,
  procppid,
  procuic,
  procpgflquo,
  procpri,
  procbasepri,
  procstate;
long procwssize,
  proclgnflgs;

unsigned long int jpictx1;
unsigned long int jpictx2;

static int ii;
static int procCount;

proc_info_t proc_table = (proc_info_t) 0;

Process::Process ()
{
}

Process::~Process ()
{
}

//
// ================================================================================
// NAME              : getCaption
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getCaption (String & s)
  const
  {
    s = String (pInfo->command);
    return true;
  }

//
// ================================================================================
// NAME              : getDescription
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getDescription (String & s)
  const
  {
    //  s = String(pInfo->args);
    //  return true;
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getInstallDate
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getInstallDate (CIMDateTime & d)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getStatus
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getStatus (String & s)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getName
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getName (String & s)
  const
  {
    s = String (pInfo->command);
    return true;
  }

//
// ================================================================================
// NAME              : getPriority
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getPriority (Uint32 & i32)
  const
  {
    i32 = pInfo->pri;
    return true;
  }

//
// ================================================================================
// NAME              : getExecutionState
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getExecutionState (Uint16 & i16)
  const
  {
    //
    // From the MOF for this class:
    // [Description (
    // "Indicates the current operating condition of the Process. "
    // "Values include ready (2), running (3), and blocked (4), "
    // "among others."),
    // Values {"Unknown", "Other", "Ready", "Running",
    // "Blocked", "Suspended Blocked", "Suspended Ready",
    // "Terminated", "Stopped", "Growing" },
    //

    enum
    {
      Unknown,
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

    switch (pInfo->state)
    {
      case 1:
	i16 = Other;
	break;
      case 2:
      case 3:
      case 4:
      case 5:
	i16 = Blocked;
        break;
      case 6:
	i16 = Suspended_Blocked;
	break;
      case 7:
	i16 = Blocked;
	break;
      case 8:
	i16 = Suspended_Blocked;
	break;
      case 9:
	i16 = Suspended_Ready;
	break;
      case 10:
	i16 = Suspended_Blocked;
	break;
      case 11:
	i16 = Blocked;
	break;
      case 12:
	i16 = Running;
	break;
      case 13:
	i16 = Suspended_Ready;
	break;
      case 14:
	i16 = Ready;
	break;
      default:
	i16 = Unknown;
    }
    return true;
  }

//
// ================================================================================
// NAME              : getOtherExecutionDescription
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getOtherExecutionDescription (String & s)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : convertToCIMDateString
// DESCRIPTION       : Convert VMS date string to CIM format
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

int convertToCIMDateString (struct tm *t, char *time)
{
  // Format the date.

  sprintf (time, "%04d%02d%02d%02d%02d%02d.000000%c%03d",
	   t->tm_year + 1900,
	   t->tm_mon + 1,
	   t->tm_mday,
	   t->tm_hour,
	   t->tm_min,
	   t->tm_sec,
	   (timezone > 0) ? '-' : '+',
	   timezone / 60 - (t->tm_isdst ? 60 : 0));

  return 1;
}

//
// ================================================================================
// NAME              : getCreationDate
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getCreationDate (CIMDateTime & d)
  const
  {
    long status,
      dst_desc[2];
    char cimtime[80] = "";
    char log_string[] = "SYS$TIMEZONE_DAYLIGHT_SAVING";
    char libdst;
    unsigned __int64 bintime = 0;
    unsigned short int timbuf[7];
    unsigned long libop,
      libdayweek,
      libdayear;
    unsigned int retlen;
    struct tm timetm;
    struct tm *ptimetm = &timetm;
    static $DESCRIPTOR (lnm_tbl, "LNM$SYSTEM");
    struct
    {
      unsigned short wLength;
      unsigned short wCode;
      void *pBuffer;
      unsigned int *pRetLen;
      int term;
    }
    item_list;

      bintime = pInfo->p_stime;

      libop = LIB$K_DAY_OF_WEEK;
      status = lib$cvt_from_internal_time (&libop, &libdayweek, &bintime);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return false;
    }

    libop = LIB$K_DAY_OF_YEAR;
    status = lib$cvt_from_internal_time (&libop, &libdayear, &bintime);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return false;
    }

    dst_desc[0] = strlen (log_string);
    dst_desc[1] = (long) log_string;
    item_list.wLength = 1;
    item_list.wCode = LNM$_STRING;
    item_list.pBuffer = &libdst;
    item_list.pRetLen = &retlen;
    item_list.term = 0;

    status = sys$trnlnm (0, &lnm_tbl, &dst_desc, 0, &item_list);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return false;
    }

    status = sys$numtim (timbuf, &bintime);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return false;
    }

    timetm.tm_sec = timbuf[5];
    timetm.tm_min = timbuf[4];
    timetm.tm_hour = timbuf[3];
    timetm.tm_mday = timbuf[2];
    timetm.tm_mon = timbuf[1] - 1;
    timetm.tm_year = timbuf[0] - 1900;
    timetm.tm_wday = libdayweek - 1;
    timetm.tm_yday = libdayear - 1;
    timetm.tm_isdst = 0;
    if (libdst != 48)
    {
      timetm.tm_isdst = 1;
    }
    timetm.tm_gmtoff = -18000;
    timetm.tm_zone = "EST";

    if (convertToCIMDateString (ptimetm, cimtime) != -1)
    {
      d = CIMDateTime (cimtime);
      return true;
    }
    return false;
  }

//
// ================================================================================
// NAME              : getTerminationDate
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getTerminationDate (CIMDateTime & d)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : GetCPUTicks
// DESCRIPTION       : Get the percentage of CPU time this process is taking.
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

// Kernel mode routine

int GetCPUTicks (long *pKernelTicks, long *pExecTicks, long *pSuperTicks, long *pUserTicks)
{
  PCB *pcb_ptr = 0;		// PCB pointer


  pcb_ptr = (PCB *) CTL$GL_PCB;

  *pKernelTicks = pcb_ptr->pcb$l_kernel_counter;
  *pExecTicks = pcb_ptr->pcb$l_exec_counter;
  *pSuperTicks = pcb_ptr->pcb$l_super_counter;
  *pUserTicks = pcb_ptr->pcb$l_user_counter;

  if (pcb_ptr->pcb$l_kt_high <= 1)
  {
    return (SS$_NORMAL);	// single thread only
  }
  else
  {
    return false;		// multithread not supported
  }
}

//
// ================================================================================
// NAME              : getKernelModeTime
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getKernelModeTime (Uint64 & i64)
  const
  {
    int status = SS$_NORMAL;
    long lKernelTicks = 0,
      lExecTicks = 0,
      lSuperTicks = 0,
      lUserTicks = 0;

    struct k1_arglist
    {				// kernel call arguments


      long lCount;		// number of arguments


      long *pKernelTicks;
      long *pExecTicks;
      long *pSuperTicks;
      long *pUserTicks;
    }
    getcputickskargs = {4};	// init to 4 arguments


    getcputickskargs.pKernelTicks = &lKernelTicks;
    getcputickskargs.pExecTicks = &lExecTicks;
    getcputickskargs.pSuperTicks = &lSuperTicks;
    getcputickskargs.pUserTicks = &lUserTicks;

    status = sys$cmkrnl (GetCPUTicks, &getcputickskargs);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return false;
    }

    i64 = lKernelTicks / 10;	// milliseconds
    return true;
  }

//
// ================================================================================
// NAME              : getUserModeTime
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// KERNELNOTES             : 
// ================================================================================
//

Boolean Process::getUserModeTime (Uint64 & i64)
  const
  {
    int status = SS$_NORMAL;
    long lKernelTicks = 0,
      lExecTicks = 0,
      lSuperTicks = 0,
      lUserTicks = 0;

    struct k1_arglist
    {				// kernel call arguments
      long lCount;		// number of arguments
      long *pKernelTicks;
      long *pExecTicks;
      long *pSuperTicks;
      long *pUserTicks;
    }
    getcputickskargs = {4};	// init to 4 arguments


    getcputickskargs.pKernelTicks = &lKernelTicks;
    getcputickskargs.pExecTicks = &lExecTicks;
    getcputickskargs.pSuperTicks = &lSuperTicks;
    getcputickskargs.pUserTicks = &lUserTicks;

    status = sys$cmkrnl (GetCPUTicks, &getcputickskargs);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return false;
    }

    i64 = lUserTicks / 10;	// milliseconds
    return true;
  }

//
// ================================================================================
// NAME              : getWorkingSetSize
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getWorkingSetSize (Uint64 & i64)
  const
  {
    i64 = pInfo->pset;
    return true;
  }

//
// ================================================================================
// NAME              : getRealUserID
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getRealUserID (Uint64 & i64)
  const
  {
    i64 = pInfo->uid;
    return true;
  }

//
// ================================================================================
// NAME              : getProcessGroupID
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getProcessGroupID (Uint64 & i64)
  const
  {
    i64 = pInfo->pgrp;
    return true;
  }

//
// ================================================================================
// NAME              : getProcessSessionID
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getProcessSessionID (Uint64 & i64)
  const
  {
    i64 = pInfo->session;
    return true;
  }

//
// ================================================================================
// NAME              : getProcessTTY
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getProcessTTY (String & s)
  const
  {
    s = String (pInfo->tty);
    return true;
  }

//
// ================================================================================
// NAME              : getModulePath
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getModulePath (String & s)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getParameters
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getParameters (Array < String > &as)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getProcessNiceValue
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getProcessNiceValue (Uint32 & i32)
  const
  {
    int nice_value;

    nice_value = pInfo->base_pri - NZERO;

    // Sanitize nice value to set below zero values equal to zero

    if (nice_value < 0)
    {
      i32 = 0;
    }
    else
    {
      i32 = nice_value;
    }
    return true;
  }

//
// ================================================================================
// NAME              : getProcessWaitingForEvent
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getProcessWaitingForEvent (String & s)
  const
  {
    char buf[100];

    switch (pInfo->state)
    {
      case 1:
	sprintf (buf, "Collided Page WAIT");
	s = buf;
	break;
      case 2:
	sprintf (buf, "Miscellaneous WAIT");
	s = buf;
	break;
      case 3:
	sprintf (buf, "Common Event Flag WAIT");
	s = buf;
	break;
      case 4:
	sprintf (buf, "Page Fault WAIT");
	s = buf;
	break;
      case 5:
	sprintf (buf, "Local Event Flag WAIT (resident)");
	s = buf;
	break;
      case 6:
	sprintf (buf, "Local Event Flag WAIT (outswapped)");
	s = buf;
	break;
      case 7:
	sprintf (buf, "Hibernate WAIT (resident)");
	s = buf;
	break;
      case 8:
	sprintf (buf, "Hibernate WAIT (outswapped)");
	s = buf;
	break;
      case 9:
	sprintf (buf, "Suspend WAIT (resident)");
	s = buf;
	break;
      case 10:
	sprintf (buf, "Suspend WAIT (outswapped)");
	s = buf;
	break;
      case 11:
	sprintf (buf, "Free Page WAIT");
	s = buf;
	break;
      default:
	sprintf (buf, "Not Waiting!");
	s = buf;
    }
    return true;
  }

//
// ================================================================================
// NAME              : getCPUTime
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getCPUTime (Uint32 & i32)
  const
  {
    int status = SS$_NORMAL;
    long lKernelTicks = 0,
      lExecTicks = 0,
      lSuperTicks = 0,
      lUserTicks = 0,
      lTotalTicks = 0;
    long avcpucnt;
    __int64 pstartime,
      qcurtime;
    float fTotalTicks,
      fpercntime;

    struct k1_arglist
    {				// kernel call arguments
      long lCount;		// number of arguments
      long *pKernelTicks;
      long *pExecTicks;
      long *pSuperTicks;
      long *pUserTicks;
    }
    getcputickskargs = {4};	// init to 4 arguments

    typedef struct
    {
      unsigned short wlength;
      unsigned short wcode;
      void *pbuffer;
      void *pretlen;
    } item_list;

    item_list itmlst3[2];

    getcputickskargs.pKernelTicks = &lKernelTicks;
    getcputickskargs.pExecTicks = &lExecTicks;
    getcputickskargs.pSuperTicks = &lSuperTicks;
    getcputickskargs.pUserTicks = &lUserTicks;

    status = sys$cmkrnl (GetCPUTicks, &getcputickskargs);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return false;
    }

    lTotalTicks = lKernelTicks + lExecTicks + lSuperTicks + lUserTicks;

    fTotalTicks = lTotalTicks;	// 10 millisec ticks
    fTotalTicks = fTotalTicks * 10000000;	// 100 nanosec ticks

    pstartime = pInfo->p_stime;	// 100 nanosec ticks

    itmlst3[0].wlength = 4;
    itmlst3[0].wcode = SYI$_AVAILCPU_CNT;
    itmlst3[0].pbuffer = &avcpucnt;
    itmlst3[0].pretlen = NULL;
    itmlst3[1].wlength = 0;
    itmlst3[1].wcode = 0;
    itmlst3[1].pbuffer = NULL;
    itmlst3[1].pretlen = NULL;

    status = sys$getsyiw (0, 0, 0, itmlst3, 0, 0, 0);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return false;
    }

    status = sys$gettim (&qcurtime);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return false;
    }

    fpercntime = avcpucnt;
    fpercntime = fpercntime * (qcurtime - pstartime);
    fpercntime = (fTotalTicks / fpercntime) * 100;
    i32 = fpercntime;
    return true;
  }

//
// ================================================================================
// NAME              : getRealText
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getRealText (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getRealData
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getRealData (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getRealStack
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getRealStack (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getVirtualText
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getVirtualText (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getVirtualData
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getVirtualData (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getVirtualStack
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getVirtualStack (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getVirtualMemoryMappedFileSize
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getVirtualMemoryMappedFileSize (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getVirtualSharedMemory
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getVirtualSharedMemory (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getCpuTimeDeadChildren
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getCpuTimeDeadChildren (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getSystemTimeDeadChildren
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getSystemTimeDeadChildren (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getParentProcessID
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getParentProcessID (String & s)
  const
  {
    char buf[100];

//    sprintf (buf, "%d", pInfo->ppid);
    sprintf (buf, "%x", pInfo->ppid);
    s = String (buf);
    return true;
  }

//
// ================================================================================
// NAME              : getRealSpace
// DESCRIPTION       : 
// ASSUMPTIONS       : 
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : returns a modified pIndex that can be used in a
// subsequent call to get next process structure
// ================================================================================
//

Boolean Process::getRealSpace (Uint64 & i64)
  const
  {
    // not supported

    return false;
  }

//
// ================================================================================
// NAME              : getHandle
// DESCRIPTION       : Uses the process id (PID) for the handle value.
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

String Process::getHandle (void)
  const
  {
    char buf[100];

//    sprintf (buf, "%d", pInfo->pid);
    sprintf (buf, "%x", pInfo->pid);
    return String (buf);
  }

//
// ================================================================================
// NAME              : getCSName
// DESCRIPTION       : Platform-specific method to get CSName
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

String Process::getCSName (void)
  const
  {
    struct hostent *he;
    char hostName[128];

      gethostname (hostName, MAXHOSTNAMELEN);

    // Now get the official hostname.  If this call fails then return
    // the value from gethostname().

    if (he = gethostbyname (hostName))
    {
      strcpy (hostName, he->h_name);
      return String (hostName);
    }
    else
    {
      return String ("unknown");
    }
  }

//
// ================================================================================
// NAME              : getOSName
// DESCRIPTION       : Platform-specific method to get OSname.
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

String Process::getOSName (void)
  const
  {
    struct utsname unameInfo;

    // Call uname and check for any errors.

    if (uname (&unameInfo) < 0)
    {
      return String ("unknown");
    }
    else
    {
      return String (unameInfo.sysname);
    }
  }

//
// ================================================================================
// NAME              : getCurrentTime
// DESCRIPTION       : Platform-specific routine to get a timestamp stat Name key
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

String Process::getCurrentTime (void)
  const
  {
    time_t t = time (0);

    return String (ctime (&t));
  }

//
// ================================================================================
// NAME              : getmaxprocount
// DESCRIPTION       : Get the maximum number of processes allowed on this system.
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

int getmaxprocount ()
{
  int status;
  unsigned long maxprocount;
  typedef struct
  {
    unsigned short wlength;
    unsigned short wcode;
    void *pbuffer;
    void *pretlen;
  } item_list;

  item_list itmlst3[2];
  item_list *itml3 = itmlst3;

  itmlst3[0].wlength = sizeof (maxprocount);
  itmlst3[0].wcode = SYI$_MAXPROCESSCNT;
  itmlst3[0].pbuffer = &maxprocount;
  itmlst3[0].pretlen = NULL;
  itmlst3[1].wlength = 0;
  itmlst3[1].wcode = 0;
  itmlst3[1].pbuffer = NULL;
  itmlst3[1].pretlen = NULL;

  status = sys$getsyiw (0, 0, 0, itmlst3, 0, 0, 0);
  if ($VMS_STATUS_SUCCESS (status))
  {
    return maxprocount;
  }
  else
  {
    return 0;
  }
}
//
// ================================================================================
// NAME              : loadProcessInfo
// DESCRIPTION       : get process info from system into internal data struct
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::loadProcessInfo (int &pIndex)
{
  int status;
  unsigned long maxprocount;
  __int64 qpid;
  char handle[100];

  Boolean stat;

  if ((maxprocount = getmaxprocount ()) == 0)
  {
    return false;
  }

  if (pIndex == 0)
  {
    if (proc_table != NULL)
    {
      free (proc_table);
    }
    proc_table = (proc_info_t) calloc (maxprocount + 1, sizeof (struct proc_info));
    pInfo = pData = proc_table;

    jpictx2 = 0;
    procCount = 0;
    ii = 1;

    itmlst3[0].wlength = 0;
    itmlst3[0].wcode = PSCAN$_MODE;
    itmlst3[0].pbuffer = (void *) JPI$K_OTHER;
    itmlst3[0].pretlen = NULL;
    itmlst3[1].wlength = 0;
    itmlst3[1].wcode = 0;
    itmlst3[1].pbuffer = NULL;
    itmlst3[1].pretlen = NULL;

    status = sys$process_scan (&jpictx2, itmlst3);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return status;
    }

    stat = buildProcessTable();

    jpictx2 = 0;
    itmlst3[0].wlength = 0;
    itmlst3[0].wcode = PSCAN$_MODE;
    itmlst3[0].pbuffer = (void *) JPI$K_BATCH;
    itmlst3[0].pretlen = NULL;
    itmlst3[1].wlength = 0;
    itmlst3[1].wcode = 0;
    itmlst3[1].pbuffer = NULL;
    itmlst3[1].pretlen = NULL;

    status = sys$process_scan (&jpictx2, itmlst3);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return status;
    }

    stat = buildProcessTable();

    jpictx2 = 0;
    itmlst3[0].wlength = 0;
    itmlst3[0].wcode = PSCAN$_MODE;
    itmlst3[0].pbuffer = (void *) JPI$K_NETWORK;
    itmlst3[0].pretlen = NULL;
    itmlst3[1].wlength = 0;
    itmlst3[1].wcode = 0;
    itmlst3[1].pbuffer = NULL;
    itmlst3[1].pretlen = NULL;

    status = sys$process_scan (&jpictx2, itmlst3);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return status;
    }

    stat = buildProcessTable();

    jpictx2 = 0;
    itmlst3[0].wlength = 0;
    itmlst3[0].wcode = PSCAN$_MODE;
    itmlst3[0].pbuffer = (void *) JPI$K_INTERACTIVE;
    itmlst3[0].pretlen = NULL;
    itmlst3[1].wlength = 0;
    itmlst3[1].wcode = 0;
    itmlst3[1].pbuffer = NULL;
    itmlst3[1].pretlen = NULL;

    status = sys$process_scan (&jpictx2, itmlst3);
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return status;
    }

    stat = buildProcessTable();

    pInfo = pData;
  }
  else
  {
    if (ii < procCount)
    {
      pInfo++;
      ii++;
    }
    else
    {
      return false;
    }
  }

  if (procCount > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

//
// ================================================================================
// NAME              : buildProcessTable
// DESCRIPTION       : process table build loop
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::buildProcessTable ()
{
  int status;

  itmlst3[0].wlength = sizeof (usernamebuf);
  itmlst3[0].wcode = JPI$_USERNAME;
  itmlst3[0].pbuffer = usernamebuf;
  itmlst3[0].pretlen = &usernamlen;
  itmlst3[1].wlength = 4;
  itmlst3[1].wcode = JPI$_PID;
  itmlst3[1].pbuffer = &procpid;
  itmlst3[1].pretlen = NULL;
  itmlst3[2].wlength = 4;
  itmlst3[2].wcode = JPI$_CPUTIM;
  itmlst3[2].pbuffer = &proccputim;
  itmlst3[2].pretlen = NULL;
  itmlst3[3].wlength = 4;
  itmlst3[3].wcode = JPI$_GRP;
  itmlst3[3].pbuffer = &procgrp;
  itmlst3[3].pretlen = NULL;
  itmlst3[4].wlength = 4;
  itmlst3[4].wcode = JPI$_MASTER_PID;
  itmlst3[4].pbuffer = &procppid;
  itmlst3[4].pretlen = NULL;
  itmlst3[5].wlength = 4;
  itmlst3[5].wcode = JPI$_UIC;
  itmlst3[5].pbuffer = &procuic;
  itmlst3[5].pretlen = NULL;
  itmlst3[6].wlength = sizeof (procimgnambuf);
  itmlst3[6].wcode = JPI$_IMAGNAME;
  itmlst3[6].pbuffer = procimgnambuf;
  itmlst3[6].pretlen = &procimgnamlen;
  itmlst3[7].wlength = 4;
  itmlst3[7].wcode = JPI$_PGFLQUOTA;
  itmlst3[7].pbuffer = &procpgflquo;
  itmlst3[7].pretlen = NULL;
  itmlst3[8].wlength = 4;
  itmlst3[8].wcode = JPI$_PRI;
  itmlst3[8].pbuffer = &procpri;
  itmlst3[8].pretlen = NULL;
  itmlst3[9].wlength = 4;
  itmlst3[9].wcode = JPI$_PRIB;
  itmlst3[9].pbuffer = &procbasepri;
  itmlst3[9].pretlen = NULL;
  itmlst3[10].wlength = 4;
  itmlst3[10].wcode = JPI$_STATE;
  itmlst3[10].pbuffer = &procstate;
  itmlst3[10].pretlen = NULL;
  itmlst3[11].wlength = 4;
  itmlst3[11].wcode = JPI$_WSSIZE;
  itmlst3[11].pbuffer = &procwssize;
  itmlst3[11].pretlen = NULL;
  itmlst3[12].wlength = 4;
  itmlst3[12].wcode = JPI$_LOGIN_FLAGS;
  itmlst3[12].pbuffer = &proclgnflgs;
  itmlst3[12].pretlen = NULL;
  itmlst3[13].wlength = 8;
  itmlst3[13].wcode = JPI$_LOGINTIM;
  itmlst3[13].pbuffer = &proclgntim;
  itmlst3[13].pretlen = NULL;
  itmlst3[14].wlength = sizeof (proctermbuf);
  itmlst3[14].wcode = JPI$_TERMINAL;
  itmlst3[14].pbuffer = proctermbuf;
  itmlst3[14].pretlen = &proctermlen;
  itmlst3[MAXITMLST - 1].wlength = 0;
  itmlst3[MAXITMLST - 1].wcode = 0;
  itmlst3[MAXITMLST - 1].pbuffer = NULL;
  itmlst3[MAXITMLST - 1].pretlen = NULL;

  while (1)
  {
    status = sys$getjpiw (0, &jpictx2, NULL, itmlst3, 0, NULL, 0);
    if (status == SS$_NOMOREPROC)
    {
      break;
    }
    if (!$VMS_STATUS_SUCCESS (status))
    {
      return status;
    }
    procCount++;
    usernamebuf[12] = '\0';

    pInfo->ppid = procppid;
    pInfo->pid = procpid;
    pInfo->uid = procuic & 0xFFFF;
    pInfo->pgrp = (procuic >> 16) & 0xFFFF;
    pInfo->rgid = procgrp;
    pInfo->cpu = proccputim;
    pInfo->virtual_size = procpgflquo;
    pInfo->pri = procpri;
    pInfo->base_pri = procbasepri;
    pInfo->state = procstate;
    pInfo->pset = procwssize;
    pInfo->p_stime = proclgntim;
    strncpy (pInfo->uname, usernamebuf, 12);
    strncpy (pInfo->command, procimgnambuf, 256);
    strncpy (pInfo->tty, proctermbuf, 8);
    pInfo++;
  }
  return true;
}
//
// ================================================================================
// NAME              : getProcessInfo
// DESCRIPTION       : reset process table pointer based on the index
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::getProcessInfo (int Index)
{
  int i;

  pInfo = proc_table;
  for (i=0; i < Index; i++)
  {
    pInfo++;
  }
  return true;
}

//
// ================================================================================
// NAME              : findProcess
// DESCRIPTION       : find the requested process and load its data
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   : 
// NOTES             : 
// ================================================================================
//

Boolean Process::findProcess (const String & handle)
{
  long i;
  unsigned __int64 pid;
  unsigned int maxprocount;

// Convert handle to an integer

//  pid = atoq (handle.getCString ());
  pid = strtol(handle.getCString (), (char **)NULL, 16);
  if ((maxprocount = getmaxprocount ()) == 0)
  {
    return false;
  }

  pInfo = proc_table;
  for (i = 0; i < maxprocount; i++)
  {
    if (pInfo->pid == pid)
    {
      return true;
    }
    if (pInfo->pid == 0)
    {
      break;
    }
    pInfo++;
  }
  return false;
}
