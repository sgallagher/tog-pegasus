//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//             (carolann_graves@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Tracer.h>
#include "IndicationOperationAggregate.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

IndicationOperationAggregate::IndicationOperationAggregate (
    CIMRequestMessage * origRequest,
    const Array <CIMName> & indicationSubclasses)
:   _origRequest (origRequest),
    _indicationSubclasses (indicationSubclasses),
    _numberIssued (0),
    _magicNumber (_theMagicNumber)
{}

IndicationOperationAggregate::~IndicationOperationAggregate ()
{
    _magicNumber = 0;
    if (_origRequest)
    {
        delete _origRequest;
    }
    Uint32 numberRequests = getNumberRequests ();
    for (Uint32 i = 0; i < numberRequests; i++)
    {
        //
        //  Since deleteRequest also removes the element from the array, 
        //  delete first element of the array each time
        //
        deleteRequest (0);
    }
    Uint32 numberResponses = getNumberResponses ();
    for (Uint32 j = 0; j < numberResponses; j++)
    {
        //
        //  Since deleteResponse also removes the element from the array, 
        //  delete first element of the array each time
        //
        deleteResponse (0);
    }
}

Boolean IndicationOperationAggregate::valid ()
{
    return (_magicNumber == _theMagicNumber) ? true: false;
}

CIMRequestMessage * IndicationOperationAggregate::getOrigRequest ()
{
    return _origRequest;
}

Uint32 IndicationOperationAggregate::getOrigType ()
{
    if (_origRequest == 0)
    {
        return 0;
    }
    else
    {
        return _origRequest->getType ();
    }
}

String IndicationOperationAggregate::getOrigMessageId ()
{
    if (_origRequest == 0)
    {
        return String::EMPTY;
    }
    else
    {
        return _origRequest->messageId;
    }
}

Uint32 IndicationOperationAggregate::getOrigDest ()
{
    if (_origRequest == 0)
    {
        return 0;
    }
    else
    {
        return _origRequest->queueIds.top ();
    }
}

Boolean IndicationOperationAggregate::requiresResponse ()
{
    if ((getOrigType () == CIM_CREATE_INSTANCE_REQUEST_MESSAGE) ||
        (getOrigType () == CIM_MODIFY_INSTANCE_REQUEST_MESSAGE) ||
        (getOrigType () == CIM_DELETE_INSTANCE_REQUEST_MESSAGE))
    {
        return true;
    }
    else
    {
        return false;
    }
}

Array <CIMName> & IndicationOperationAggregate::getIndicationSubclasses ()
{
    return _indicationSubclasses;
}

Uint32 IndicationOperationAggregate::getNumberIssued ()
{
    return _numberIssued;
}

void IndicationOperationAggregate::setNumberIssued (Uint32 i)
{
    _numberIssued = i;
}

Boolean IndicationOperationAggregate::appendResponse (
    CIMResponseMessage * response)
{
    _appendResponseMutex.lock (pegasus_thread_self ());
    _responseList.append (response);
    Boolean returnValue = (getNumberResponses () == getNumberIssued ());
    _appendResponseMutex.unlock ();
    return returnValue;
}

Uint32 IndicationOperationAggregate::getNumberResponses ()
{
    return _responseList.size ();
}

CIMResponseMessage * IndicationOperationAggregate::getResponse (
    const Uint32 & pos)
{
    return _responseList [pos];
}

void IndicationOperationAggregate::deleteResponse (
    const Uint32 & pos)
{
    delete _responseList [pos];
    _responseList.remove (pos);
}

void IndicationOperationAggregate::appendRequest (
    CIMRequestMessage * request)
{
    _appendRequestMutex.lock (pegasus_thread_self ());
    _requestList.append (request);
    _appendRequestMutex.unlock ();
}

Uint32 IndicationOperationAggregate::getNumberRequests ()
{
    return _requestList.size ();
}

CIMRequestMessage * IndicationOperationAggregate::getRequest (
    const Uint32 & pos)
{
    return _requestList [pos];
}

void IndicationOperationAggregate::deleteRequest (
    const Uint32 & pos)
{
    delete _requestList [pos];
    _requestList.remove (pos);
}

ProviderClassList IndicationOperationAggregate::findProvider (
    const String & messageId)
{
    //
    //  Look in the list of requests for the request with the message ID 
    //  corresponding to the message ID in the response
    //
    ProviderClassList provider;
    Uint32 numberRequests = getNumberRequests ();
    for (Uint32 i = 0; i < numberRequests; i++)
    {
        if (getRequest (i)->messageId == messageId )
        {
            //
            //  Get the provider and provider module from the matching request
            //
            switch (getRequest (i)->getType ())
            {
                case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
                {
                    CIMCreateSubscriptionRequestMessage * request =
                        (CIMCreateSubscriptionRequestMessage *) getRequest (i);
                    provider.provider = request->provider;
                    provider.providerModule = request->providerModule;
                    break;
                }
        
                case CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE:
                {
                    CIMEnableIndicationsRequestMessage * request =
                        (CIMEnableIndicationsRequestMessage *) getRequest (i);
                    provider.provider = request->provider;
                    provider.providerModule = request->providerModule;
                    break;
                }
        
                case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
                {
                    CIMModifySubscriptionRequestMessage * request =
                        (CIMModifySubscriptionRequestMessage *) getRequest (i);
                    provider.provider = request->provider;
                    provider.providerModule = request->providerModule;
                    break;
                }
        
                case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
                {
                    CIMDeleteSubscriptionRequestMessage * request =
                        (CIMDeleteSubscriptionRequestMessage *) getRequest (i);
                    provider.provider = request->provider;
                    provider.providerModule = request->providerModule;
                    break;
                }
        
                default:
                {
                    PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2,
                        "Unexpected origRequest type " + String 
                        (MessageTypeToString (getRequest (i)->getType ())) +
                        " in findProvider");
                    PEGASUS_ASSERT (false);
                break;
                }
            }

            return provider;
        }
    }

    //
    //  No request found with message ID matching message ID from response
    //
    PEGASUS_ASSERT (false);
}

const Uint32 IndicationOperationAggregate::_theMagicNumber = 98765;

PEGASUS_NAMESPACE_END
