//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:  Jenny Yu (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/pstat.h>
#include <fcntl.h>
#include <unistd.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;


void cim_server_service(int argc, char **argv ) { return; }  
unsigned int cimserver_remove_nt_service(void) { return(0) ; }
unsigned int cimserver_install_nt_service(String &pegasusHome ) { return(0) ; }

const char *fname = "/etc/wbem/cimserver_start.conf";
pid_t server_pid;

// daemon_init , RW Stevens, "Advance UNIX Programming"

int cimserver_fork(void) 
{ 
  pid_t pid;
  if( (pid = fork() ) < 0) 
    return(-1);
  else if (pid != 0)
    exit(0);
  
  setsid();
  umask(0);

  // get the pid of the cimserver process
  server_pid = getpid();

  return(0);
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
  struct pst_status pstru;

  if (pstat_getproc(&pstru, sizeof(struct pst_status), (size_t)0, pid) != -1)
  {
      kill(pid, SIGKILL);
  }

  // remove the file
  System::removeFile(fname);
  
  return(0);
}

