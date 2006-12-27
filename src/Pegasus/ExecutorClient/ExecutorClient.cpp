#include "ExecutorClient.h"
#include <Pegasus/Common/Mutex.h>
#include "private/ExecutorClientLoopbackImpl.h"

#ifdef PEGASUS_ENABLE_PRIVILEGE_SEPARATION
# include "private/ExecutorClientSocketImpl.h"
#endif

PEGASUS_NAMESPACE_BEGIN

static ExecutorClientImpl* _impl()
{
    static ExecutorClientImpl* impl = 0;
    static Mutex implMutex;

    if (impl == 0)
    {
        AutoMutex autoMutex(implMutex);

        if (impl == 0)
        {
#ifdef PEGASUS_ENABLE_PRIVILEGE_SEPARATION
            if (getenv("__PEGASAUS_EXECUTOR__"))
                impl = new ExecutorClientSocketImpl;
            else
                impl = new ExecutorClientLoopbackImpl;
#else
            impl = new ExecutorClientLoopbackImpl;
#endif
        }
    }

    return impl;
}

int ExecutorClient::ping()
{
    return _impl()->ping();
}

FILE* ExecutorClient::openFileForRead(const char* path)
{
    return _impl()->openFileForRead(path);
}

int ExecutorClient::removeFile(const char* path)
{
    return _impl()->removeFile(path);
}

int ExecutorClient::startProviderAgent(
    const char* module, 
    int gid, 
    int uid,
    int& pid,
    int& readFd,
    int& writeFd)
{
    return _impl()->startProviderAgent(module, gid, uid, pid, readFd, writeFd);
}

int ExecutorClient::daemonizeExecutor()
{
    return _impl()->daemonizeExecutor();
}

int ExecutorClient::shutdownExecutor()
{
    return _impl()->shutdownExecutor();
}

int ExecutorClient::changeOwner(
    const char* path,
    const char* owner)
{
    return _impl()->changeOwner(path, owner);
}

PEGASUS_NAMESPACE_END
