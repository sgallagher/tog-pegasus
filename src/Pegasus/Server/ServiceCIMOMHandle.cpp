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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ServiceCIMOMHandle.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// ServiceCIMOMHandle:
//
////////////////////////////////////////////////////////////////////////////////

ServiceCIMOMHandle::ServiceCIMOMHandle(void) : _server(0)
{
}

ServiceCIMOMHandle::ServiceCIMOMHandle(
    MessageQueue* outputQueue,
    CIMRepository * repository,
	ProviderManager * providerManager,
    CIMServer * server)
:
    //CIMOMHandle(outputQueue),
    _repository(repository),
	_providerManager(providerManager),
    _server(server)
{
}

ServiceCIMOMHandle::~ServiceCIMOMHandle(void)
{
}

ServiceCIMOMHandle& ServiceCIMOMHandle::operator=(const ServiceCIMOMHandle& handle)
{
    if(this == &handle)
    {
        return(*this);
    }

    CIMOMHandle::operator=(handle);

    _server = handle._server;

    _repository = handle._repository;

	_providerManager = handle._providerManager;

    return(*this);
}

PEGASUS_NAMESPACE_END
