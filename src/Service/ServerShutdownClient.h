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

#ifndef Pegasus_ServerShutdownClient_h
#define Pegasus_ServerShutdownClient_h

#include <Pegasus/Common/Config.h>
#include <Service/ServerRunStatus.h>
#include <Service/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The ServerShutdownClient shuts down a specified server using a CIM shutdown
    request.
*/
class PEGASUS_SERVICE_LINKAGE ServerShutdownClient
{
public:

    /**
       Constructs a ServerShutdownClient.  The specified ServerRunStatus
       object is used to determine whether the server is running and to kill
       it if necessary.
    */
    ServerShutdownClient(ServerRunStatus* serverRunStatus);

    /**
        Destructs the ServerShutdownClient object.
    */
    ~ServerShutdownClient();

    /**
        Shuts down the server.  If the shutdown is not successful within the
        specified timeoutValue, the server is killed.
    */
    void shutdown(Uint32 timeoutValue);

private:

    ServerShutdownClient();

    void _waitForTerminationOrTimeout(Uint32 maxWaitTime);

    ServerRunStatus* _serverRunStatus;
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_ServerShutdownClient_h
