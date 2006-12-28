#ifndef _ExecutorClient_ExecutorClientLoopbackImpl_h
#define _ExecutorClient_ExecutorClientLoopbackImpl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Mutex.h>
#include "ExecutorClientImpl.h"

PEGASUS_NAMESPACE_BEGIN

class ExecutorClientLoopbackImpl : public ExecutorClientImpl
{
public:

    ExecutorClientLoopbackImpl();

    virtual ~ExecutorClientLoopbackImpl();

    virtual int ping();

    virtual FILE* openFile(
        const char* path,
        int mode);

    virtual int removeFile(
        const char* path);

    virtual int renameFile(
        const char* oldPath,
        const char* newPath);

    virtual int changeMode(
        const char* path,
        int mode);

    virtual int startProviderAgent(
        const char* module, 
        int gid, 
        int uid,
        int& pid,
        AnonymousPipe*& readPipe,
        AnonymousPipe*& writePipe);

    virtual int daemonizeExecutor();

    virtual int shutdownExecutor();

    virtual int changeOwner(
        const char* path,
        const char* owner);

private:
    Mutex _mutex;
};

PEGASUS_NAMESPACE_END

#endif /* _ExecutorClient_ExecutorClientLoopbackImpl_h */
