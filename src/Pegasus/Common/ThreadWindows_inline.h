//%///////////-*-c++-*-//////////////////////////////////////////////////////
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


#ifndef ThreadWindows_inline_h
#define ThreadWindows_inline_h

inline void Thread::run(void)
{
   _handle.thid = (PEGASUS_THREAD_TYPE)_beginthread( ((void (__cdecl *)(void *))(_start)), 
						     0, 
						     (void *)this) ;
}

inline void Thread::cancel(void)
{
  _cancelled = true;
}

inline void Thread::test_cancel(void)
{
   if(_cancel_enabled == true)
   {
      if(_cancelled == true)
      {
	 exit_self(0);
	 _endthread();
	 _handle.thid = 0 ;
	 
      }
   }
}

inline Boolean Thread::is_cancelled(void)
{
   return _cancelled;
}

inline void Thread::thread_switch(void)
{
   Sleep(0);
}

inline void Thread::sleep(Uint32 milliseconds)
{
  Sleep(milliseconds);
}

inline void Thread::join(void)
{
   if( (! _is_detached) && (_handle.thid != 0))
   {
      // emulate the unix join api. caller sleeps until 
      // thread is done.
      DWORD exit_code = 0;
      do 
      {
	 GetExitCodeThread(_handle.thid, &exit_code);
	 Sleep(0);
      }
      while( exit_code == STILL_ACTIVE);
   }
   _handle.thid = 0;
}

inline void Thread::thread_init(void)
{
   ;
   
}

inline void Thread::detach(void)
{
   _is_detached = true;
}


#endif // ThreadWindows_inline_h
