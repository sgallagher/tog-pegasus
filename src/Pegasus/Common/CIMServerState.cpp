///////////////////////////////////////////////////////////////////////////////
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
//=============================================================================
//
// Author: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By:
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/CIMServerState.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static const char* STATE_STRINGS[] =
{
    "starting",
    "running",
    "terminating",
};

/**
    Initialize the CIMServerState instance
*/
CIMServerState* CIMServerState::_instance = 0;

/**
    Constructor
*/
CIMServerState::CIMServerState()
{
    _requestCount = 0;
    _currentState = RUNNING;

    //
    // Create Mutex.  The request count can only be changed with lock 
    // Mutex held
    //
    _mutex = new Mutex();

};

/**
    Destructor
*/
CIMServerState::~CIMServerState()
{
    delete _mutex;
};

/**
    Return a pointer to the CIMServerState instance.
*/
CIMServerState* CIMServerState::getInstance()
{
    if (!_instance)
    {
        _instance = new CIMServerState();
    }
    return _instance;
}

/**
    Set the state of the CIM Server.
*/
void CIMServerState::setState(Uint32 code)
{
    _currentState = code;
}

/**
    Return the state of the CIM Server.
*/
Uint32 CIMServerState::getState()
{
    return _currentState;
}

/**
    Return a string representation of the state of the CIM Server.
*/
const char* CIMServerState::CIMServerStateToString(Uint32 code)
{
    if (code > 0 && code < TERMINATING)
    {
        return STATE_STRINGS[code-1];
    }
    else
    {
        return "Unknown CIMServer state";
    }
}

/**
    Return the current request count.
*/
Uint32 CIMServerState::getRequestCount()
{
    _mutex->lock(pegasus_thread_self());

    Uint32 count = _requestCount;

    _mutex->unlock();

    return count;
}

/**
    Increment the current request count.
*/
void CIMServerState::incrementRequestCount()
{
    _mutex->lock(pegasus_thread_self());

    _requestCount++;

    _mutex->unlock();
}

/**
    Decrement the current request count.
*/
void CIMServerState::decrementRequestCount()
{
    _mutex->lock(pegasus_thread_self());

    _requestCount--;

    _mutex->unlock();
}

PEGASUS_NAMESPACE_END
