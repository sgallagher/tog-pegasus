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
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Thread.h"

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "ThreadWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "ThreadUnix.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_NAMESPACE_BEGIN


// for non-native implementations
#ifndef PEGASUS_THREAD_CLEANUP_NATIVE 
void Thread::cleanup_push( void (*routine)(void *), void *parm) throw(IPCException)
{
  cleanup_handler *cu = new cleanup_handler(routine, parm);
  try { _cleanup.insert_first(cu); } 
  catch(IPCException& e) { delete cu; throw; }
  return;
}

void Thread::cleanup_pop(Boolean execute = true) throw(IPCException)
{
  cleanup_handler *cu ;
  try { cu = _cleanup.remove_first() ;}
  catch(IPCException& e) { assert(0); }
  if(execute == true)
    cu->execute();
  delete cu;
}

thread_data *Thread::put_tsd(Sint8 *key, void (*delete_func)(void *), Uint32 size, void *value) throw(IPCException)
{
  PEGASUS_ASSERT(key != NULL);
  PEGASUS_ASSERT(delete_func != NULL);
  thread_data *tsd ;
  tsd = _tsd.remove((void *)key);  // may throw an IPC exception 
  thread_data *ntsd = new thread_data(key);
  ntsd->put_data(delete_func, size, value);
  try { _tsd.insert_first(ntsd); }
  catch(IPCException& e) { delete ntsd; throw; }
  return(tsd);
}

#endif

#ifndef PEGASUS_THREAD_CLEANUP_NATIVE 
void Thread::exit_self(PEGASUS_THREAD_RETURN exit_code) 
{ 
  // execute the cleanup stack and then return 
  while( _cleanup.count(); )
  {
    try { cleanup_pop(true); }
    catch(IPCException& e) { PEGASUS_ASSERT(0) ; break; } 
  }
  _exit_code = exit_code;
}
#endif


PEGASUS_NAMESPACE_END
