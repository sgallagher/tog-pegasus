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
    : ProviderFacade(0), _module(path, name)
{
}

Provider::Provider(const Provider & p)
    : ProviderFacade(p._module.getProvider()), _module(p._module)
{
}

Provider::~Provider(void)
{
}

String Provider::getName(void) const
{
    return(_module.getProviderName());
}

void Provider::initialize(CIMOMHandle & cimom)
{
    // NOTE: yield before a potentially lengthy operation.
    pegasus_yield();

    _module.load();

    ProviderFacade::_provider = _module.getProvider();

    // NOTE: yield before a potentially lengthy operation.
    pegasus_yield();

    ProviderFacade::initialize(cimom);
}

void Provider::terminate(void)
{
    // NOTE: yield before a potentially lengthy operation.
    pegasus_yield();

    ProviderFacade::terminate();

    // NOTE: yield before a potentially lengthy operation.
    pegasus_yield();

    _module.unload();
}

void Provider::terminateProvider(void)
{
    // NOTE: yield before a potentially lengthy operation.
    pegasus_yield();

    ProviderFacade::terminate();

    // NOTE: yield before a potentially lengthy operation.
    pegasus_yield();

    if(_provider != 0)
    {
        delete _provider;

        _provider = 0;
    }
}

PEGASUS_NAMESPACE_END
