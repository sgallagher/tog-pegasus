#ifndef _ExecutorClient_ExecutorClientImpl_h
#define _ExecutorClient_ExecutorClientImpl_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

class ExecutorClientImpl
{
public:

    ExecutorClientImpl();

    virtual ~ExecutorClientImpl();

    virtual int ping() = 0;

    virtual FILE* openFileForRead(
        const char* path) = 0;

    virtual int removeFile(
        const char* path) = 0;

    virtual int startProviderAgent(
        const char* module, 
        int gid, 
        int uid,
        int& pid,
        int& readFd,
        int& writeFd) = 0;

    virtual int daemonizeExecutor() = 0;

    virtual int shutdownExecutor() = 0;

    virtual int changeOwner(
        const char* path,
        const char* owner) = 0;

private:
};

PEGASUS_NAMESPACE_END

#endif /* _ExecutorClient_ExecutorClientImpl_h */
