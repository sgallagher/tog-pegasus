//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMServerState_h
#define Pegasus_CIMServerState_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//
// This class contains the current state information about the CIMServer, as
// well as methods to get and set the CIMServer state information. 
//
///////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMServerState
{
public:

    enum StateCode { STARTING = 1, RUNNING = 2, TERMINATING = 3 };

    /**  Construct the singleton instance of the CIMServerState and return a
         pointer to that instance.
    */
    static CIMServerState* getInstance();

    void setState(Uint32 code);

    Uint32 getState();

    const char* CIMServerStateToString(Uint32 code);

    Uint32 getRequestCount();

    void incrementRequestCount();

    void decrementRequestCount();

private:

    //
    // This is meant to be a singleton, so the constructor and the
    // destructor are made private.
    //

    CIMServerState();

    ~CIMServerState();

    /**  The singleton instance of the CIMServerState
    */
    static CIMServerState* _instance;

    Uint32 _currentState;

    Uint32 _requestCount;

    Mutex *_mutex;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMServerState_h */
