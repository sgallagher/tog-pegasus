//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPDelegator_h
#define Pegasus_HTTPDelegator_h

#include <cstdio>
#include <cassert>
#include <iostream>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

class PEGASUS_COMMON_LINKAGE HTTPDelegator : public MessageQueue
{
public:

    /** Constructor. The constructor defines the output queues
        used by this the HTTP Delegator.   The Delegator outputs
        to:
        <UL>
        <LI>OperationsMessageQueue
        <LI>ExportMessageQueue
        <LI>HTTPOptionQueue
        <LI>HTTPAuthenticatorQueue
        </UL>
     */
    HTTPDelegator(
	MessageQueue* operationMessageQueue,
	MessageQueue* exportMessageQueue);

    /** Destructor. */
    ~HTTPDelegator();

    virtual void handleEnqueue();

    virtual const char* getQueueName() const;

    void handleHTTPMessage(HTTPMessage* httpMessage);

private:

    MessageQueue* _operationMessageQueue;
    MessageQueue* _exportMessageQueue;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPDelegator_h */
