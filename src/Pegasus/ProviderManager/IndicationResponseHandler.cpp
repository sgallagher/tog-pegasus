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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "IndicationResponseHandler.h"

#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>

PEGASUS_NAMESPACE_BEGIN

IndicationResponseHandler::IndicationResponseHandler(MessageQueueService * service)
    : _service(service)
{
}

IndicationResponseHandler::~IndicationResponseHandler(void)
{
}

void IndicationResponseHandler::deliver(const CIMIndication & cimIndication)
{
    // find the indication service
    Array<Uint32> serviceIds;

    _service->find_services(String("Server::IndicationService"), 0, 0, &serviceIds);

    PEGASUS_ASSERT(serviceIds.size() != 0);

    // create message
    CIMGetInstanceResponseMessage * message =
	new CIMGetInstanceResponseMessage(
	"messageId",
	CIM_ERR_SUCCESS,
	"",
	QueueIdStack(serviceIds[0]),
	CIMInstance());

    AsyncOpNode * op = _service->get_op();

    AsyncLegacyOperationStart * asyncRequest =
	    new AsyncLegacyOperationStart(
		    _service->get_next_xid(),
		    op,
		    serviceIds[0],
		    message,
		    _service->getQueueId());

    _service->SendForget(asyncRequest);

    delete asyncRequest;
}

void IndicationResponseHandler::deliver(const OperationContext & context, const CIMIndication & object)
{
    deliver(object);
}

void IndicationResponseHandler::deliver(const Array<CIMIndication> & objects)
{
    for(Uint32 i = 0,n = objects.size(); i < n; i++)
    {
	deliver(objects[i]);
    }
}

void IndicationResponseHandler::deliver(const OperationContext & context, const Array<CIMIndication> & objects)
{
    deliver(objects);
}

void IndicationResponseHandler::reserve(const Uint32 size)
{
    // do nothing
}

void IndicationResponseHandler::processing(void)
{
    // do nothing
}

void IndicationResponseHandler::complete(void)
{
    // do nothing
}

void IndicationResponseHandler::complete(const OperationContext & context)
{
    complete();
}

PEGASUS_NAMESPACE_END
