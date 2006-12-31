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

    virtual int startProviderAgent(
        const char* module, 
        int uid,
        int gid, 
        int& pid,
        AnonymousPipe*& readPipe,
        AnonymousPipe*& writePipe) = 0;

    virtual int daemonizeExecutor() = 0;

    virtual int changeOwner(
        const char* path,
        const char* owner) = 0;

    virtual int waitPid(
        int pid) = 0;

    virtual int pamAuthenticate(
        const char* username,
        const char* password) = 0;

    virtual int pamValidateUser(
        const char* username) = 0;

private:
};

PEGASUS_NAMESPACE_END

#endif /* _ExecutorClient_ExecutorClientImpl_h */
