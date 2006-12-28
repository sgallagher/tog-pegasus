#ifndef _ExecutorClient_ExecutorClientImpl_h
#define _ExecutorClient_ExecutorClientImpl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/AnonymousPipe.h>

PEGASUS_NAMESPACE_BEGIN

class ExecutorClientImpl
{
public:

    ExecutorClientImpl();

    virtual ~ExecutorClientImpl();

    virtual int ping() = 0;

    virtual FILE* openFile(
        const char* path,
        int mode) = 0;

    virtual int removeFile(
        const char* path) = 0;

    virtual int renameFile(
        const char* oldPath,
        const char* newPath) = 0;

    virtual int changeMode(
        const char* path,
        int mode) = 0;

    virtual int startProviderAgent(
        const char* module, 
        int uid,
        int gid, 
        int& pid,
        AnonymousPipe*& readPipe,
        AnonymousPipe*& writePipe) = 0;

    virtual int daemonizeExecutor() = 0;

    virtual int shutdownExecutor() = 0;

    virtual int changeOwner(
        const char* path,
        const char* owner) = 0;

private:
};

PEGASUS_NAMESPACE_END

#endif /* _ExecutorClient_ExecutorClientImpl_h */
