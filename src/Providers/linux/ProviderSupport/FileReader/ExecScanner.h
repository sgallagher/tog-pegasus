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
//
//%////////////////////////////////////////////////////////////////////////////
//
//  A class used to start an executable running, and pass its output
//  through the StreamScanner routines of its parent class.
//  

#ifndef Pegasus_ExecScanner_h
#define Pegasus_ExecScanner_h

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include "StreamScanner.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN


/** A list of directories which will be searched for the requested
 *  executable if the supplied name does not begin with a '/'. */
static char const *default_path[] = { 
  "/bin", 
  "/usr/bin", 
  "/sbin", 
  "/usr/sbin", 
  NULL
};


/** A class whose objects can be told to locate and execute a named
 *  executable file, then parse the output of that file in the context of a
 *  StreamScanner object. */
class ExecScanner : public StreamScanner {

 public:

  /** Constructor.  May be called with an array of pointers to character
   *  strings, setting the default path.  All character strings must begin
   *  with a '/' character, those which do not are silently ignored.  The
   *  last element in this array must be NULL. */
  ExecScanner(char const * const *search_path = default_path);

  ~ExecScanner(void);

  /** Execute a named binary.  If an absolute pathname (begins with '/'),
   *  ignores the search path, otherwise takes it relative to that search
   *  path.  Returns 0 on success, indicating that scanning for text may
   *  proceed.  Returns -1 if it was unable to execute the specified file.
   *  The array "argslist" must hold the argv[] array, starting with
   *  argv[0] for the executable name.  It will be passed unmodified as the
   *  second argument to execv(), so must include a NULL pointer as the
   *  last element in the array.  This method simply generates absolute
   *  pathnames and passes the results to LaunchExecutable(), where the
   *  real work is done. */
  int ExecuteForScan(char const *exename, char const * const *argslist);

 private:
  /// The search path
  vector<string> path;

  /// The process ID of the executing binary
  pid_t exe_pid;

  /// The pipe connecting the executing binary to the invoking object
  int fildes[2];

 protected:
  virtual void HandleEndOfStream(void);

 private:

  /** Given an absolute pathname, tries to launch that binary, directing
   *  the output to 'stream' for the StreamScanner parent class.  It
   *  returns -1 if the named executable does not exist, or is not a file
   *  or symlink to a file with at least one of the execute bits set. */
  int LaunchExecutable(char const *exename, char const * const * argslist);
};


/** An exception thrown when system resources necessary to performing the
 *  task were unavailable */
class ResourceError : public Exception {
 public:
  static const char MSG[];
  
  ResourceError(void) : Exception(String(MSG)) { }
};


PEGASUS_NAMESPACE_END


#endif  /* !Pegasus_ExecScanner_h */
