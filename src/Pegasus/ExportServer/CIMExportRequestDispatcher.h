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

#ifndef Pegasus_CIMExportRequestDispatcher_h
#define Pegasus_CIMExportRequestDispatcher_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Provider2/CIMIndicationConsumer.h>
#include <Pegasus/ExportServer/Linkage.h>

#include "ConsumerTable.h"
#include "HandlerTable.h"

PEGASUS_NAMESPACE_BEGIN

class CIMRepository;
class HandlerTable;

/** This class dispatches CIM export requests. For now it simply
*/
class PEGASUS_EXPORT_SERVER_LINKAGE CIMExportRequestDispatcher 
    : public MessageQueue
{
public:

    
    CIMExportRequestDispatcher(CIMRepository* repository);

    virtual ~CIMExportRequestDispatcher();

    void handleIndication(
        CIMInstance& indicationHandlerInstance,
        CIMInstance& indicationInstance,
	String nameSpace);

protected:
    CIMHandler* _lookupHandlerForClass(
	const String& nameSpace,
	const String& className);

    CIMIndicationConsumer* _lookupConsumer(const String& url);

    virtual void handleEnqueue();

    virtual const char* getQueueName() const;

    CIMRepository* _repository;
    HandlerTable _handlerTable;
    ConsumerTable _consumerTable;

private:

    void _handleExportIndicationRequest(
	CIMExportIndicationRequestMessage* request);

    void _enqueueResponse(
	CIMRequestMessage* request,
	CIMResponseMessage* response);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMExportRequestDispatcher_h */
