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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              David Eger          <dteger@us.ibm.com>
//
//%////////////////////////////////////////////////////////////////////////////
//
//
//  Implementation of the ExecScanner class.
//

#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#define _USE_BSD
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#undef _USE_BSD


#include "ExecScanner.h"


#ifndef COMPILE_STAND_ALONE

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

#endif  /* !COMPILE_STAND_ALONE */

#define DIRSEP '/'
#define DIRSEPSTR "/"

const char ResourceError::MSG[] = "Unexpected failure to allocate a system resource.";

ExecScanner::ExecScanner(char const * const *search_path)
{
  int i;
  string hold_path;
 
  fildes[0] = fildes[1] = -1;
  path.clear();
  i = 0;
  while (search_path[i] != NULL) {
    if (search_path[i][0] != DIRSEP)
      continue;   // skip anything which isn't an absolute path

    hold_path = search_path[i];
    path.push_back(hold_path);
    i++;
  }
}


ExecScanner::~ExecScanner(void)
{
  HandleEndOfStream();
}


int ExecScanner::ExecuteForScan(char const *exename, 
				char const * const *argslist)
{
  int i;
  string trial_name;

  if (exename[0] == DIRSEP) {  // absolute pathname

    return LaunchExecutable(exename, argslist);

  } else {

    for (i = 0; i < (int) path.size(); i++) {
      trial_name = path[i] + DIRSEPSTR + exename;
      if (LaunchExecutable(trial_name.c_str(), argslist) == 0)
	return 0;
    }

    return -1;
  }
}



int ExecScanner::LaunchExecutable(char const *exename,
				  char const * const *argslist)
{
  struct stat statbuff;

  // Make sure the file exists, is a regular file, and is executable by at
  // least somebody
  if (stat(exename, &statbuff) != 0 ||
      !S_ISREG(statbuff.st_mode) ||
      !(statbuff.st_mode & ( S_IXUSR | S_IXGRP | S_IXOTH )))
    return -1;

  if (pipe(fildes) == -1)
    throw ResourceError();

  exe_pid = fork();

  if (exe_pid == -1) {   // failed to fork
    close(fildes[0]); close(fildes[1]);
    throw ResourceError();
  }

  if (exe_pid != 0) {    // I'm the parent

    close(fildes[1]);
    fildes[1] = -1;
    if ((stream = fdopen(fildes[0], "r")) == NULL) {
      //  Hmmm.  Problem.  Infanticide and exit.
      kill(exe_pid, SIGKILL);
      throw ResourceError();
    }
    return 0;

  } else {               // I'm the child
    int i;
    char **args_copy;  /* I don't know why execv needs writable string in
			* the second argument, but here goes... */
    
    for (i = 0; argslist[i] != NULL; i++) ;
    args_copy = new char * [i+1];
    args_copy[i] = NULL;
    for (i--; i >= 0; i--)
      args_copy[i] = strdup(argslist[i]);

    fildes[0] = -1;
    close(fildes[0]);
    if (dup2(fildes[1], fileno(stdout)) == -1)
      exit(EXIT_FAILURE);

    execv(exename, args_copy);

    exit(EXIT_FAILURE);   // If I get here, something went wrong.
  }
}


void ExecScanner::HandleEndOfStream(void)
{
  int i;

  if (stream != NULL)
    fclose(stream);
  stream = NULL;

  for (i = 0; i < 2; i++)
    if (fildes[i] != -1) {
      close(fildes[i]);
      fildes[i] = -1;
    }
  
  if (exe_pid != 0) {
    kill(exe_pid, SIGKILL);
#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    wait4(exe_pid, NULL, WNOHANG | WUNTRACED, NULL);
#endif
  }
  exe_pid = 0;
}

#ifndef COMPILE_STAND_ALONE

PEGASUS_NAMESPACE_END

#endif  /* !COMPILE_STAND_ALONE */
