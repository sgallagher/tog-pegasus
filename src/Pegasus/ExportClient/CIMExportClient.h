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

#ifndef Pegasus_ExportClient_h
#define Pegasus_ExportClient_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/ExportClient/ExportClientException.h>

PEGASUS_NAMESPACE_BEGIN

class ExportClientHandler;
class Selector;
class Channel;

class PEGASUS_EXPORTCLIENT_LINKAGE CIMExportClient
{
public:

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };

    CIMExportClient(
	Selector* selector,
	Uint32 timeOutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

    ~CIMExportClient();

    Uint32 getTimeOut() const
    {
	return _timeOutMilliseconds;
    }

    void setTimeOut(Uint32 timeOutMilliseconds)
    {
	_timeOutMilliseconds = timeOutMilliseconds;
    }

    void deliverIndication(
	const char* address, 
	CIMInstance& indicationInstance, 
	String nameSpace);

private:

    ExportClientHandler* _getHandler();

    void _sendMessage(const Array<Sint8>& message);

    const char* _getHostName() const { return "localhost"; }

    Selector* _selector;
    Channel* _channel;
    Uint32 _timeOutMilliseconds;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ExportClient_h */
