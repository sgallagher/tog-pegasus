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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ReadWriteSem_h
#define Pegasus_ReadWriteSem_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Semaphore.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/AtomicInt.h>

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// Select the read-write-lock implementation for your platform:
//
//     PEGASUS_USE_POSIX_RWLOCK
//     PEGASUS_USE_SEMAPHORE_RWLOCK
//
//==============================================================================

#if defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX)
# define PEGASUS_USE_POSIX_RWLOCK
#elif defined(PEGASUS_PLATFORM_PASE_ISERIES_IBMCXX)
# define PEGASUS_USE_POSIX_RWLOCK
#elif defined(PEGASUS_PLATFORM_HPUX_ACC)
# define PEGASUS_USE_POSIX_RWLOCK
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
# define PEGASUS_USE_POSIX_RWLOCK
#elif defined(PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
# define PEGASUS_USE_POSIX_RWLOCK
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# define PEGASUS_USE_POSIX_RWLOCK
#elif defined(PEGASUS_PLATFORM_VMS_IA64_DECCXX)
# define PEGASUS_USE_POSIX_RWLOCK
#elif defined(PEGASUS_PLATFORM_VMS_ALPHA_DECCXX)
# define PEGASUS_USE_POSIX_RWLOCK
#elif defined(PEGASUS_PLATFORM_LINUX_X86_64_GNU)
# define PEGASUS_USE_POSIX_RWLOCK
#else
# define PEGASUS_USE_SEMAPHORE_RWLOCK
#endif

//==============================================================================
//
// ReadWriteSemRep
//
//==============================================================================

#ifdef PEGASUS_USE_POSIX_RWLOCK
struct ReadWriteSemRep
{
    pthread_rwlock_t rwlock;
    ThreadType owner;
};
#endif /* PEGASUS_USE_POSIX_RWLOCK */

#ifdef PEGASUS_USE_SEMAPHORE_RWLOCK
struct ReadWriteSemRep
{
    Semaphore _rlock;
    Mutex _wlock;
    Mutex _internal_lock;
    ThreadType _owner;
    ReadWriteSemRep() :
        _rlock(10), _wlock(), _internal_lock(), _owner(Threads::self())
    {
    }
};
#endif /* PEGASUS_USE_POSIX_RWLOCK */

//==============================================================================
//
// ReadWriteSem
//
//==============================================================================

class PEGASUS_COMMON_LINKAGE ReadWriteSem
{
public:

    ReadWriteSem();

    ~ReadWriteSem();

    // @exception Permission
    // @exception WaitFailed
    inline void wait_read(ThreadType caller)
    {
        _wait(false, caller);
    }

    // @exception Permission
    // @exception WaitFailed
    inline void wait_write(ThreadType caller)
    {
        _wait(true, caller);
    }

    // @exception Permission
    inline void unlock_read(ThreadType caller)
    {
        _unlock(false, caller);
    }

    // @exception Permission
    inline void unlock_write(ThreadType caller)
    {
        _unlock(true, caller);
    }

    int read_count() const;
    int write_count() const;

private:
    // @exception Permission
    // @exception WaitFailed
    void _wait(Boolean writeLock, ThreadType caller);

    // @exception Permission
    void _unlock(Boolean writeLock, ThreadType caller);

    AtomicInt _readers;
    AtomicInt _writers;
    ReadWriteSemRep _rwlock;
};

//==============================================================================
//
// ReadLock
//
//==============================================================================

class ReadLock
{
public:

    ReadLock(ReadWriteSem& rwsem) : _rwsem(rwsem)
    {
        _rwsem.wait_read(Threads::self());
    }

    ~ReadLock()
    {
        _rwsem.unlock_read(Threads::self());
    }

private:
    ReadWriteSem & _rwsem;
};

//==============================================================================
//
// WriteLock
//
//==============================================================================

class WriteLock
{
public:

    WriteLock(ReadWriteSem& rwsem) : _rwsem(rwsem)
    {
        _rwsem.wait_write(Threads::self());
    }

    ~WriteLock()
    {
        _rwsem.unlock_write(Threads::self());
    }

private:
    ReadWriteSem & _rwsem;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ReadWriteSem_h */
