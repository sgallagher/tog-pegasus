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

#include "ExecutorClient.h"
#include <Pegasus/Common/Mutex.h>
#include "private/ExecutorClientLoopbackImpl.h"

#ifdef PEGASUS_ENABLE_PRIVILEGE_SEPARATION
# include "private/ExecutorClientSocketImpl.h"
#endif

#define EXTRACE  \
    fprintf(stderr, "ExecutorClient:TRACE: %s(%d)\n", __FILE__, __LINE__)

PEGASUS_NAMESPACE_BEGIN

static ExecutorClientImpl* _impl()
{
    static ExecutorClientImpl* impl = 0;
    static Mutex mutex;

    if (impl == 0)
    {
        mutex.lock();

        if (impl == 0)
        {
#ifdef PEGASUS_ENABLE_PRIVILEGE_SEPARATION

            char* env = getenv("PEGASUS_EXECUTOR_SOCKET");

            if (env)
            {
                char* end = 0;
                int sock = (int)strtol(env, &end, 10);
                PEGASUS_ASSERT(*end == '\0');
                impl = new ExecutorClientSocketImpl(sock);
            }
            else
                impl = new ExecutorClientLoopbackImpl;

#else
            impl = new ExecutorClientLoopbackImpl;
#endif
        }

        mutex.unlock();
    }

    return impl;
}

int ExecutorClient::ping()
{
    return _impl()->ping();
}

FILE* ExecutorClient::openFile(const char* path, int mode)
{
    return _impl()->openFile(path, mode);
}

int ExecutorClient::renameFile(
    const char* oldPath,
    const char* newPath)
{
    return _impl()->renameFile(oldPath, newPath);
}

int ExecutorClient::removeFile(const char* path)
{
    return _impl()->removeFile(path);
}

int ExecutorClient::startProviderAgent(
    const char* module, 
    int uid,
    int gid, 
    int& pid,
    AnonymousPipe*& readPipe,
    AnonymousPipe*& writePipe)
{
    return _impl()->startProviderAgent(
        module, uid, gid, pid, readPipe, writePipe);
}

int ExecutorClient::daemonizeExecutor()
{
    return _impl()->daemonizeExecutor();
}

int ExecutorClient::changeOwner(
    const char* path,
    const char* owner)
{
    return _impl()->changeOwner(path, owner);
}

PEGASUS_NAMESPACE_END
