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
// Modified By:
//
//%////////////////////////////////////////////////////////////////////

PEGASUS_NAMESPACE_BEGIN

Thread::Thread( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *start )(void *),
		void *parameter, 
		Boolean detached ) : _is_detached(detached), 
				     _cancel_enabled(true), 
				     _cancelled(false),
				     _suspend_count(), 
				     _start(start), 
				     _cleanup(true),
				     _tsd(true),
				     _thread_parm(parameter), 
				     _exit_code(0)
{
   _handle.thatt = NULL;
   _handle.thid = (PEGASUS_THREAD_TYPE)0;

}


Thread::~Thread()
{
   try 
   {
      
   if( (! _is_detached) && (_handle.thid != 0))
   {
      // emulate the unix join api. caller sleeps until 
      // thread is done.
     if(false  == is_cancelled())
       {
	 DWORD exit_code = 0;
	 do 
	   {
	     GetExitCodeThread(_handle.thid, &exit_code);
	     Sleep(0);
	   }
	 while( exit_code == STILL_ACTIVE);
       }
   }
   CloseHandle(_handle.thid);
   }
   catch(..)
   {
   }
   
   
}

PEGASUS_NAMESPACE_END
