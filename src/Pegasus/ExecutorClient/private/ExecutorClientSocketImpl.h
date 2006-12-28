#ifndef _ExecutorClient_ExecutorClientSocketImpl_h
#define _ExecutorClient_ExecutorClientSocketImpl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Mutex.h>
#include "ExecutorClientImpl.h"

PEGASUS_NAMESPACE_BEGIN

class ExecutorClientSocketImpl : public ExecutorClientImpl
{
public:

    ExecutorClientSocketImpl();

    virtual ~ExecutorClientSocketImpl();

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
        int uid,
        int gid, 
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

#endif /* _ExecutorClient_ExecutorClientSocketImpl_h */
