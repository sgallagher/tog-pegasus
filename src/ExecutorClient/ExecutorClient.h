#ifndef _ExecutorClient_ExecutorClient_h
#define _ExecutorClient_ExecutorClient_h

#include <Pegasus/Common/Config.h>
#include "Linkage.h"

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_EXECUTOR_CLIENT_LINKAGE ExecutorClient
{
public:

    ExecutorClient(int sock);

    ~ExecutorClient();

    int ping();

    int openFileForRead(const char* path);

private:

    int _sock;
};

PEGASUS_NAMESPACE_END

#endif /* _ExecutorClient_ExecutorClient_h */
