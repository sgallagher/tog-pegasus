//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _Pegasus_Common_Executor_h
#define _Pegasus_Common_Executor_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/AnonymousPipe.h>
#include <Pegasus/Common/Linkage.h>
#include <Executor/Defines.h>
#include <cstdio>

PEGASUS_NAMESPACE_BEGIN

struct SessionKey
{
    char data[33];
};

class PEGASUS_COMMON_LINKAGE Executor
{
public:

    static void setSock(int sock);

    /** Return zero if the executor is present.
    */
    static int detectExecutor();

    static int ping();

    static FILE* openFile(
        const char* path,
        int mode);

    static int renameFile(
        const char* oldPath,
        const char* newPath);

    static int removeFile(
        const char* path);

    static int startProviderAgent(
        const char* module, 
        int uid,
        int gid, 
        int& pid,
        AnonymousPipe*& readPipe,
        AnonymousPipe*& writePipe);

    static int daemonizeExecutor();

    static int waitPid(
        int pid);

    static int pamAuthenticate(
        const char* username,
        const char* password);

    static int pamValidateUser(
        const char* username);

    static int startLocalAuth(
        const char* user,
        char path[EXECUTOR_BUFFER_SIZE],
        SessionKey* key);

    static int finishLocalAuth(
        const SessionKey* key,
        const char* token,
        SessionKey* newKey);

private:
    // Private to prevent instantiation.
    Executor();
};

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Executor_h */
