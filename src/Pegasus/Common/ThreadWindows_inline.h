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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: Steve Hills (steve.hills@ncr.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef ThreadWindows_inline_h
#define ThreadWindows_inline_h

inline ThreadStatus Thread::run(void)
{
    // Note: A Win32 thread ID is not the same thing as a pthread ID.
    // Win32 threads have both a thread ID and a handle.  The handle
    // is used in the wait functions, etc.
    // So _handle.thid is actually the thread handle.

    unsigned threadid = 0;
    _handle.thid = (PEGASUS_THREAD_TYPE)
        _beginthreadex(NULL, 0, _start, this, 0, &threadid);
    if (_handle.thid == 0)
    {
        if (errno == EAGAIN)
        {
            return PEGASUS_THREAD_INSUFFICIENT_RESOURCES;
        }
        else
        {
            return PEGASUS_THREAD_SETUP_FAILURE;
        }
    }
    return PEGASUS_THREAD_OK;
}

inline void Thread::cancel(void)
{
	_cancelled = true;
}

inline void Thread::test_cancel(void)
{
	if( _cancel_enabled && _cancelled )
	{
		exit_self( 0 );
	}
}

inline Boolean Thread::is_cancelled(void)
{
	return _cancelled;
}

inline void Thread::thread_switch(void)
{
	Sleep( 0 );
}

inline void Thread::sleep( Uint32 milliseconds )
{
	Sleep( milliseconds );
}

inline void Thread::join(void)
{
	if( _handle.thid != 0 )
	{
		if( !_is_detached )
		{
			if( !_cancelled )
			{
				// Emulate the unix join api. Caller sleeps until thread is done.
				WaitForSingleObject( _handle.thid, INFINITE );
			}
			else
			{
				// Currently this is the only way to ensure this code does not 
				// hang forever.
				if( WaitForSingleObject( _handle.thid, 10000 ) == WAIT_TIMEOUT )
				{
					TerminateThread( _handle.thid, 0 );
				}
			}

			DWORD exit_code = 0;
			GetExitCodeThread( _handle.thid, &exit_code );
			_exit_code = (PEGASUS_THREAD_RETURN)exit_code;
		}

		CloseHandle( _handle.thid );
		_handle.thid = 0;
	}
}

inline void Thread::thread_init(void)
{
	_cancel_enabled = true;
}

inline void Thread::detach(void)
{
	_is_detached = true;
}

#endif // ThreadWindows_inline_h
