//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

// These must be global to all threads

static int stackMul = 2;
static bool doneOnce = false;

PEGASUS_NAMESPACE_BEGIN

static sigset_t *block_signal_mask(sigset_t * sig)
{
  sigemptyset(sig);
  // should not be used for main()
  sigaddset(sig, SIGHUP);
  sigaddset(sig, SIGINT);
  // maybe useless, since KILL can't be blocked according to POSIX
  sigaddset(sig, SIGKILL);

  sigaddset(sig, SIGABRT);
  sigaddset(sig, SIGALRM);
  sigaddset(sig, SIGPIPE);

  sigprocmask(SIG_BLOCK, sig, NULL);
  return sig;
}

////////////////////////////////////////////////////////////////////////////

Thread::Thread(
	PEGASUS_THREAD_RETURN(PEGASUS_THREAD_CDECL * start) (void *),
	void *parameter,
	Boolean detached)
:_is_detached(detached),
_cancel_enabled(true),
_cancelled(false),
_suspend_count(),
_start(start),
_cleanup(true),
_tsd(true),
_thread_parm(parameter),
_exit_code(0)
{
  pthread_attr_init(&_handle.thatt);
  size_t stacksize;

  // 
  // This code uses a, 'hidden' (non-documented), VMS only, logical 
  //  name (environment variable), PEGASUS_VMS_THREAD_STACK_MULTIPLIER,
  //  to allow in the field adjustment of the thread stack size.
  //
  // We only check for the logical name once to not have an
  //  impact on performance.
  // 
  // Note:  This code may have problems in a multithreaded environment
  //  with the setting of doneOnce to true.
  // 
  // Current code in Cimserver and the clients do some serial thread
  //  creations first so this is not a problem now.
  // 
  if (!doneOnce)
  {
    // 
    // Test for the logical name.
    // 
    const char *env = getenv("PEGASUS_VMS_THREAD_STACK_MULTIPLIER");
    if (env)
    {
      // 
      // The logical is defined, convert it to a number and
      //  test it's validity.
      // 
      char *end = NULL;
      stackMul = strtol(env, &end, 10);
      if (*end)
      {
        // 
        // Not valid, set to the default multiplier
        // 
        stackMul = 2;
      }
    }
    // 
    // Don't come through here again.
    // 
    doneOnce = true;
  }

  // 
  // Get the system default thread stack size
  // 
  if (pthread_attr_getstacksize(&_handle.thatt, &stacksize) == 0)
  {
    // 
    // Replace it with the VMS default thread stack size.
    // 
    int rc = pthread_attr_setstacksize(&_handle.thatt, stacksize * stackMul);
    // 
    // Make sure it succeeded
    // 
    PEGASUS_ASSERT(rc == 0);
  }

  _handle.thid = 0;
}

Thread::~Thread()
{
  try
  {
    join();
    pthread_attr_destroy(&_handle.thatt);

    empty_tsd();
  }
  catch(...)
  {
    // Do not allow the destructor to throw an exception
  }
}

PEGASUS_NAMESPACE_END
