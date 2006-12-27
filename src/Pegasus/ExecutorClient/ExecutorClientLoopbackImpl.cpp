#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/FileSystem.h>
#include "private/ExecutorClientLoopbackImpl.h"

PEGASUS_NAMESPACE_BEGIN

ExecutorClientLoopbackImpl::ExecutorClientLoopbackImpl()
{
}

ExecutorClientLoopbackImpl::~ExecutorClientLoopbackImpl()
{
}

int ExecutorClientLoopbackImpl::ping()
{
    // Nothing to do.
    return 0;
}

FILE* ExecutorClientLoopbackImpl::openFileForRead(
    const char* path)
{
    return fopen(path, "rb");
}

int ExecutorClientLoopbackImpl::removeFile(
    const char* path)
{
    return FileSystem::removeFile(path) ? 0 : -1;
}

int ExecutorClientLoopbackImpl::startProviderAgent(
    const char* module, 
    int gid, 
    int uid,
    int& pid,
    int& readFd,
    int& writeFd)
{
    return 0;
}

int ExecutorClientLoopbackImpl::daemonizeExecutor()
{
    // Nothing to do.
    return 0;
}

int ExecutorClientLoopbackImpl::shutdownExecutor()
{
    // Nothing to do.
    return 0;
}

int ExecutorClientLoopbackImpl::changeOwner(
    const char* path,
    const char* owner)
{
    return FileSystem::changeFileOwner(path, owner);
}

PEGASUS_NAMESPACE_END
