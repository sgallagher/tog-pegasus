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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:  Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//		 Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#if defined(PEGASUS_OS_HPUX)
#include <sys/pstat.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

// Note: on the Unix platform, PEGASUS_RETURN_WHEN_READY flag is turned on 
//       by default. This means that the cimserver is ready to serve CIM
//	 requests when the command cimserver is returned. If you like to 
// 	 turn off this feature on your platform, just undefine this flag for
// 	 your platform.
#define PEGASUS_RETURN_WHEN_READY

#define MAX_WAIT_TIME 15

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#ifdef PEGASUS_RETURN_WHEN_READY
static sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;
Boolean signalFlag = true;
#endif

void cim_server_service(int argc, char **argv ) { return; }  
unsigned int cimserver_remove_nt_service(void) { return(0) ; }
unsigned int cimserver_install_nt_service(String &pegasusHome ) { return(0) ; }
unsigned int cimserver_start_nt_service(void) { return(0) ; }
unsigned int cimserver_stop_nt_service(void) { return(0) ; }

const char *fname = "/etc/opt/wbem/cimserver_start.conf";
pid_t server_pid;

#ifdef PEGASUS_RETURN_WHEN_READY
static void sig_usr(int signo)
{
    sigflag = 1;
    return;
}
#endif

// daemon_init , RW Stevens, "Advance UNIX Programming"

int cimserver_fork(void) 
{ 
#ifdef PEGASUS_RETURN_WHEN_READY
    // set up things
    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
    {
	signalFlag = false;
    }
    else
    {
    	sigemptyset(&zeromask);
    	sigemptyset(&newmask);
    	sigaddset(&newmask, SIGUSR1);

    	// block SIGUSR1 and save current signal mask
    	sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    }
#endif
 
  pid_t pid;
  if( (pid = fork() ) < 0) 
    return(-1);
  else if (pid != 0)
  {
#ifdef PEGASUS_RETURN_WHEN_READY
    if (signalFlag)
    {
    	// parent wait for child
    	while (sigflag == 0)
       	    sigsuspend(&zeromask);

        // reset signal mask to original value
        sigprocmask(SIG_SETMASK, &oldmask, NULL);
    }
    else
    {
	// if there is a problem with signal, parent process terminates
	// until file cimserver_start.conf exists or maxWaitTime expires

        Uint32 maxWaitTime = MAX_WAIT_TIME;

	while(!FileSystem::exists(fname) && maxWaitTime > 0)
	{
	    sleep(1);
	    maxWaitTime = maxWaitTime - 1;
	}
    }
#endif
    exit(0);
  }
  
  setsid();
  umask(0);

  // get the pid of the cimserver process
  server_pid = getpid();

  return(0);
}

Boolean isCIMServerRunning(void)
{
  FILE *pid_file;
  pid_t pid = 0;

  // open the file containing the CIMServer process ID
  pid_file = fopen(fname, "rw");
  if (!pid_file)
  {
      return false;
  }

  // get the pid from the file
  fscanf(pid_file, "%ld\n", &pid);

  if (pid == 0)
  {
     return false;
  }

  //
  // check to see if cimserver process is alive
  //
#if defined(PEGASUS_OS_HPUX)
  struct pst_status pstru;

  if (pstat_getproc(&pstru, sizeof(struct pst_status), (size_t)0, pid) != -1)
  {
      return true;
  }
#endif

  return false;
}

int cimserver_kill(void) 
{ 
  FILE *pid_file;
  pid_t pid = 0;
  
  // open the file containing the CIMServer process ID
  pid_file = fopen(fname, "r");
  if (!pid_file) 
  {
      return (-1);
  }

  // get the pid from the file
  fscanf(pid_file, "%ld\n", &pid);

  if (pid == 0)
  {
     System::removeFile(fname);
     return (-1);
  }

  //
  // kill the process if it is still alive
  //
#if defined(PEGASUS_OS_HPUX)
  struct pst_status pstru;

  if (pstat_getproc(&pstru, sizeof(struct pst_status), (size_t)0, pid) != -1)
  {
      kill(pid, SIGKILL);
  }
#endif

  // remove the file
  System::removeFile(fname);
  
  return(0);
}

// notify parent process to terminate so user knows that cimserver
// is ready to serve CIM requests.
void notify_parent(void)
{
#ifdef PEGASUS_RETURN_WHEN_READY
  pid_t ppid = getppid();

  // if kill() fails, no signal is sent
  if (kill(ppid, SIGUSR1) == -1)
  {
	signalFlag = false;
  }
#endif
}
