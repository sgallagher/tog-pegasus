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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMExportResponseEncoder_h
#define Pegasus_CIMExportResponseEncoder_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/ExportServer/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** This class encodes CIM operation requests and passes them up-stream.
*/
class PEGASUS_EXPORT_SERVER_LINKAGE CIMExportResponseEncoder 
    : public MessageQueue
{
public:

    CIMExportResponseEncoder();

    ~CIMExportResponseEncoder();

    void sendResponse(Uint32 queueId, Array<Sint8>& message);

    void sendError(
	Uint32 queueId, 
	const String& messageId,
	const String& methodName,
	CIMStatusCode code,
	const String& description);

    void sendError(
	CIMResponseMessage* response,
	const String& cimMethodName);

    virtual void handleEnqueue();

    virtual const char* getQueueName() const;

    void encodeExportIndicationResponse(
	CIMExportIndicationResponseMessage* response);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMExportResponseEncoder_h */
