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
// Author: Warren Otsuka (warren_otsuka@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WbemExecQueue_h
#define Pegasus_WbemExecQueue_h

#include <fstream>
#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Client/ClientAuthenticator.h>
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** This class receives HTTP messages and prints out the messages.
*/
class WbemExecQueue : public MessageQueue
{
public:

    /** Constuctor.
	@param outputQueue queue to receive decoded HTTP messages.
    */
   WbemExecQueue(
        MessageQueue* encoderQueue,
	PEGASUS_STD(ostream&) os,
	Boolean debugOutput,
        ClientAuthenticator* authenticator);

    /** Destructor. */
    ~WbemExecQueue();

    /** Initializes the encoder queue */
    void setEncoderQueue(MessageQueue* encoderQueue);

    /** This method is called when a message is enqueued on this queue. */
    virtual void handleEnqueue();

    /** Returns the queue name. */
    virtual const char* getQueueName() const;

private:

    void _handleHTTPMessage(
	HTTPMessage* message);

    void _handleMethodResponse(
	char* content);

    void _printAll(
		  HTTPMessage* httpMessage );

    MessageQueue*        _outputQueue;

    MessageQueue*        _encoderQueue;

    PEGASUS_STD(ostream)& _os;

    Boolean              _debugOutput;

    ClientAuthenticator* _authenticator;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WbemExecQueue_h */
