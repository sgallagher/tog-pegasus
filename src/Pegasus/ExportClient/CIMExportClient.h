//%/////-*-c++-*-///////////////////////////////////////////////////////////////
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Client_h
#define Pegasus_Client_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Client/ClientAuthenticator.h>
#include <Pegasus/ExportClient/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class Monitor;
class CIMExportResponseDecoder;
class CIMExportRequestEncoder;

/** This class provides the interface that a client uses to communicate
    with a CIMOM.
*/
class PEGASUS_EXPORT_CLIENT_LINKAGE CIMExportClient : public MessageQueueService
{
public:
  typedef MessageQueueService Base;
  

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };

    ///
    CIMExportClient(
	Monitor* monitor,
	HTTPConnector* httpConnector,
	Uint32 timeOutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

    ///
    ~CIMExportClient();

    ///
    virtual void handleEnqueue();

    /** Returns the queue name. */
    virtual const char* getQueueName() const;

    ///
    Uint32 getTimeOut() const
    {
	return _timeOutMilliseconds;
    }

    ///
    void setTimeOut(Uint32 timeOutMilliseconds)
    {
	_timeOutMilliseconds = timeOutMilliseconds;
    }

    ///
    void connect(const String& address);

    //
    // Connection used by local clients
    //
    void connectLocal(
        const String& address,
        const String& userName = String::EMPTY);

    ///
    virtual void exportIndication(
	const String& nameSpace,
	const CIMInstance& instanceName);

private:

    Message* _waitForResponse(
	const Uint32 messageType,
	const String& messageId,
	const Uint32 timeOutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

    void _checkError(const CIMResponseMessage* responseMessage);

    Monitor* _monitor;
    HTTPConnector* _httpConnector;
    Uint32 _timeOutMilliseconds;
    Boolean _connected;
    CIMExportResponseDecoder* _responseDecoder;
    CIMExportRequestEncoder* _requestEncoder;
    ClientAuthenticator* _authenticator;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Client_h */
