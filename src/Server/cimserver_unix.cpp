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
#include <Pegasus/Common/Signal.h>

#define MAX_WAIT_TIME 25

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean handleSigUsr1 = false;

void cim_server_service(int argc, char **argv ) { return; }  

const char *fname = "/etc/opt/wbem/cimserver_start.conf";
pid_t server_pid;

void sigUsr1Handler(int s_n, PEGASUS_SIGINFO_T * s_info, void * sig)
{
    handleSigUsr1 = true;
}

// daemon_init , RW Stevens, "Advance UNIX Programming"

int cimserver_fork(void) 
{ 
getSigHandle()->registerHandler(PEGASUS_SIGUSR1, sigUsr1Handler);
getSigHandle()->activate(PEGASUS_SIGUSR1);
 
  pid_t pid;
  if( (pid = fork() ) < 0) 
  {
      getSigHandle()->deactivate(PEGASUS_SIGUSR1);
      return(-1);
  }
  else if (pid != 0)
  {
      //
      // parent wait for child
      // if there is a problem with signal, parent process terminates
      // when waitTime expires
      //
      Uint32 waitTime = MAX_WAIT_TIME;

      while(!handleSigUsr1 && waitTime > 0)
      {
	    sleep(1);
	    waitTime--;
      }
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
  fscanf(pid_file, "%d\n", &pid);

  fclose(pid_file);

  if (pid == 0)
  {
     return false;
  }

  //
  // check to see if cimserver process is alive
  //
#if defined(PEGASUS_OS_HPUX)
  struct pst_status pstru;

  int ret_code;
  ret_code = pstat_getproc(&pstru, sizeof(struct pst_status), (size_t)0, pid);

  if ( (ret_code != -1 ) && (strcmp(pstru.pst_ucomm, "cimserver")) == 0)
  {
      // cimserver is running
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
  fscanf(pid_file, "%d\n", &pid);

  fclose(pid_file);

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

  int ret_code;
  ret_code = pstat_getproc(&pstru, sizeof(struct pst_status), (size_t)0, pid);

  if ( (ret_code != -1 ) && (strcmp(pstru.pst_ucomm, "cimserver")) == 0)
  {
      // cimserver is running, kill the process
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
  pid_t ppid = getppid();

  kill(ppid, PEGASUS_SIGUSR1); 
}
