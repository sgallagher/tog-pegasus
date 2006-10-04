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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: Markus Mueller
//              Ramnath Ravindran (Ramnath.Ravindran@compaq.com)
//              David Eger (dteger@us.ibm.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for BUG# 3518
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Mutex_h
#define Pegasus_Mutex_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/IPCTypes.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE Mutex
{
public:

    Mutex();
    Mutex(int type);

    ~Mutex();

    // block until gaining the lock - throw a deadlock
    // exception if process already holds the lock
    // @exception Deadlock
    // @exception WaitFailed
    void lock(PEGASUS_THREAD_TYPE caller);

    // try to gain the lock - lock succeeds immediately if the
    // mutex is not already locked. throws an exception and returns
    // immediately if the mutex is currently locked.
    // @exception Deadlock
    // @exception AlreadyLocked
    // @exception WaitFailed
    void try_lock(PEGASUS_THREAD_TYPE caller);

    // wait for milliseconds and throw an exception then return if the wait
    // expires without gaining the lock. Otherwise return without throwing an
    // exception.
    // @exception Deadlock
    // @exception TimeOut
    // @exception WaitFailed
    void timed_lock( Uint32 milliseconds, PEGASUS_THREAD_TYPE caller);

    // unlock the semaphore
    // @exception Permission
    void unlock();

    inline PEGASUS_THREAD_TYPE get_owner() { return(_mutex.owner); }

#if defined(PEGASUS_OS_LINUX)
    /**
        This method must only be called after a fork() to reset the mutex
        lock status in the new process.  Any other use of this method is
        unsafe.
    */
    void reinitialize();
#endif

private:
    inline void _set_owner(PEGASUS_THREAD_TYPE owner) { _mutex.owner = owner; }
    PEGASUS_MUTEX_HANDLE _mutex;
    PEGASUS_MUTEX_HANDLE & _get_handle()
    {
        return _mutex;
    }

    // Hide the assignment operator to avoid implicit use of the default
    // assignment operator.  Do not use this method.
    Mutex& operator=(const Mutex& original) {return *this;}

    // Hide the copy constructor to avoid implicit use of the default
    // copy constructor.  Do not use this method.
    Mutex(const Mutex& _mutex);

    friend class Condition;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Mutex_h */
