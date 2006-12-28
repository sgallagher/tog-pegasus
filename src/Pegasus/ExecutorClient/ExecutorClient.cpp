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

int ExecutorClient::changeMode(const char* path, int mode)
{
    return _impl()->changeMode(path, mode);
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
