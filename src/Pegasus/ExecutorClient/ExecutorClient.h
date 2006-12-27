#ifndef _ExecutorClient_ExecutorClient_h
#define _ExecutorClient_ExecutorClient_h

#include <Pegasus/Common/Config.h>
#include <cstdio>

#ifndef PEGASUS_EXECUTOR_CLIENT_LINKAGE
#   ifdef PEGASUS_CLIENT_INTERNAL
#       define PEGASUS_EXECUTOR_CLIENT_LINKAGE PEGASUS_EXPORT
#   else
#       define PEGASUS_EXECUTOR_CLIENT_LINKAGE PEGASUS_IMPORT
#   endif
#endif

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_EXECUTOR_CLIENT_LINKAGE ExecutorClient
{
public:

    static int ping();

    static FILE* openFileForRead(
        const char* path);

    static int removeFile(
        const char* path);

    static int startProviderAgent(
        const char* module, 
        int gid, 
        int uid,
        int& pid,
        int& readFd,
        int& writeFd);

    static int daemonizeExecutor();

    static int shutdownExecutor();

    static int changeOwner(
        const char* path,
        const char* owner);

private:
    // Private to prevent instantiation.
    ExecutorClient();
};

PEGASUS_NAMESPACE_END

#endif /* _ExecutorClient_ExecutorClient_h */
