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
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Provider.h"

#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN

Provider::Provider(const String & name, const String & path)
    : ProviderFacade(0), _status(UNKNOWN), _module(path, name)
{
}

Provider::Provider(const String & name, const String & path,
                   const String & interfaceName)
    : ProviderFacade(0), _status(UNKNOWN), _module(path, name ,interfaceName)
{
    //PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "Provider::Provider");
    //PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "name = " + name + "; path = " + path);
    //PEG_METHOD_EXIT();
}


Provider::Provider(const Provider & p)
    : ProviderFacade(p._module.getProvider()), _status(UNKNOWN),
      _module(p._module)
{
}

Provider::~Provider(void)
{
}

Provider::Status Provider::getStatus(void) const
{
    return(_status);
}

String Provider::getName(void) const
{
    return(_module.getProviderName());
}

void Provider::initialize(CIMOMHandle & cimom)
{
    _status = INITIALIZING;

    try
    {
	// yield before a potentially lengthy operation.
	pegasus_yield();

	_module.load();

	ProviderFacade::_provider = _module.getProvider();

	// yield before a potentially lengthy operation.
	pegasus_yield();

	ProviderFacade::initialize(cimom);
    }
    catch(...)
    {
	_status = UNKNOWN;

	throw;
    }

    _status = INITIALIZED;
}

void Provider::terminate(void)
{
    _status = TERMINATING;

    try
    {
	// yield before a potentially lengthy operation.
	pegasus_yield();

	ProviderFacade::terminate();

	// yield before a potentially lengthy operation.
	pegasus_yield();

	_module.unload();
    }
    catch(...)
    {
	_status = UNKNOWN;

	throw;
    }

    _status = TERMINATED;
}

PEGASUS_NAMESPACE_END
