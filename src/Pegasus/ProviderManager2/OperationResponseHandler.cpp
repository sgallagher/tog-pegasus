//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//%/////////////////////////////////////////////////////////////////////////////

#include "OperationResponseHandler.h"
#include "ProviderManagerService.h"

PEGASUS_NAMESPACE_BEGIN

OperationResponseHandler::OperationResponseHandler
(CIMRequestMessage *request,
 CIMResponseMessage *response) : 
	_request(request), _response(response)
{

#ifndef PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD
#define PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD 100
#elif PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD  == 0
#undef PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD 
#define PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD  ~0
#endif

	_responseObjectTotal = 0;
	_responseMessageTotal = 0;
    if(request->requestIsOOP == true)
    {
        _responseObjectThreshold = ~0;
    }
	else
    {
        _responseObjectThreshold = PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD;

#ifdef PEGASUS_DEBUG
	    static const char *responseObjectThreshold = 
		    getenv("PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD");
	    if (responseObjectThreshold)
	    {
		    Uint32 i = (Uint32) atoi(responseObjectThreshold);
		    if (i > 0)
			    _responseObjectThreshold = i;
	    }
#endif
    }
}

OperationResponseHandler::~OperationResponseHandler()
{
	_request = 0;
	_response = 0;
}

// This is only called from SimpleResponseHandler.deliver() but lives in this
// class because all asyncronous response must have a "response" pointer
// to go through. Only operation classes have a response pointer

void OperationResponseHandler::send(Boolean isComplete)
{
	// some handlers do not send async because their callers cannot handle
	// partial responses. If this is the case, stop here.

	if (isAsync() == false)
	{
		// preserve tradional behavior
		if (isComplete == true)
			transfer();
		return;
	}

	SimpleResponseHandler *simpleP = dynamic_cast<SimpleResponseHandler*>(this);

	// It is possible to instantiate this class directly (not derived)
	// The caller would do this only if the operation does not have any data to 
	// be returned

	if (! simpleP)
	{
		// if there is no data to be returned, then the message should NEVER be
		// incomplete (even on an error)
		if (isComplete == false)
			PEGASUS_ASSERT(false);
		return;
	}

	SimpleResponseHandler &simple = *simpleP;
	PEGASUS_ASSERT(_response);
	Uint32 objectCount = simple.size();

	// have not reached threshold yet
	if (isComplete == false && objectCount < _responseObjectThreshold)
		return;

	CIMResponseMessage *response = _response;

	// for complete responses, just use the one handed down from caller
	// otherwise, create our own that the caller never sees but is
	// utilized for async responses underneath

	if (isComplete == false)
		_response = _request->buildResponse();

	_response->setComplete(isComplete);
	_responseObjectTotal += objectCount;

	// since we are reusing response for every chunk,keep track of original count
	_response->setIndex(_responseMessageTotal++);

	// set the originally allocated response to one more than the current.
	// The reason for doing this is proactive in case of an exception. This
	// allows the last response to be set as it may not re-enter this code.

	if (isComplete == false)
		response->setIndex(_responseMessageTotal);

	validate();

	if (_response->cimException.getCode() != CIM_ERR_SUCCESS)
		simple.clear();
	
	String function = getClass() + "::" + "transfer";
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
							function);
	
	transfer();
	simple.clear();

	// l10n
	_response->operationContext.
		set(ContentLanguageListContainer(simple.getLanguages()));

	// call thru ProviderManager to get externally declared entry point

	if (isComplete == false)
	{
		ProviderManagerService::handleCimResponse(*_request, *_response);
	}

	// put caller's allocated response back in place. Note that _response
	// is INVALID after sending because it has been deleted externally

	_response = response;
}

PEGASUS_NAMESPACE_END
