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

    virtual FILE* openFileForRead(
        const char* path);

    virtual int removeFile(
        const char* path);

    virtual int startProviderAgent(
        const char* module, 
        int gid, 
        int uid,
        int& pid,
        int& readFd,
        int& writeFd);

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
