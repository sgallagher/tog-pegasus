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
// Author: Sean Keenan (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <Pegasus/Common/Signal.h>
#define MAX_WAIT_TIME 25

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean handleSigUsr1 = false;

String newPortNumber = "";
String pegasusTrace  = "";

void cim_server_service(int argc, char **argv )
{
  return;
}

pid_t server_pid;

void sigUsr1Handler(int s_n, PEGASUS_SIGINFO_T * s_info, void * sig)
{
    handleSigUsr1 = true;
}

int cimserver_fork(void) 
{ 
  return(0);
}

Boolean isCIMServerRunning(void)
{
  FILE *pid_file;
  pid_t pid = 0;

  // open the file containing the CIMServer process ID
  pid_file = fopen(CIMSERVER_START_FILE, "r");
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
  return false;
}

int cimserver_kill(void) 
{ 
  FILE *pid_file;
  pid_t pid = 0;
  
  // open the file containing the CIMServer process ID
  pid_file = fopen(CIMSERVER_START_FILE, "r");
  if (!pid_file) 
  {
      return (-1);
  }

  // get the pid from the file
  fscanf(pid_file, "%d\n", &pid);

  fclose(pid_file);

  if (pid == 0)
  {
     System::removeFile(CIMSERVER_START_FILE);
     return (-1);
  }

  //
  // kill the process if it is still alive
  //
  // remove the file
  System::removeFile(CIMSERVER_START_FILE);
  
  return(0);
}
// notify parent process to terminate so user knows that cimserver
// is ready to serve CIM requests.
void notify_parent(int id)
{
  pid_t ppid = getppid();
  if (id)
   kill(ppid, SIGTERM);
  else
   kill(ppid, PEGASUS_SIGUSR1); 
}

// Platform specific run
int platform_run( int argc, char** argv, Boolean shutdownOption )
{
//  newPortNumber = "";
//  pegasusTrace = "";
  return cimserver_run( argc, argv, shutdownOption );
}

void cimserver_set( CIMServer* s )
{
}

