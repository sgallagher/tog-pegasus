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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Provider.h"

#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN

Provider::Provider(const String & name,
		   ProviderModule *module, 
		   CIMBaseProvider *pr)
   : Base(pr), _module(module), _cimom_handle(0), _name(name)
{
   
}


Provider::~Provider(void)
{
   
}


Provider::Status Provider::getStatus(void) const
{
    return(_status);
}

ProviderModule *Provider::getModule(void) const
{
    return(_module);
}

String Provider::getName(void) const
{
    return(_name);
}

void Provider::initialize(CIMOMHandle & cimom)
{
   
    _status = INITIALIZING;

    try
    {
	// yield before a potentially lengthy operation.
	pegasus_yield();

	ProviderFacade::initialize(cimom);
    }
    catch(...)
    {
	_status = UNKNOWN;
	_module->unloadModule();
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

	_module->unloadModule();
    }
    catch(...)
    {
	_status = UNKNOWN;

	throw;
    }

    _status = TERMINATED;

}

Boolean Provider::operator == (const void *key) const 
{
   if( (void *)this == key)
      return true;
   return false;
}

Boolean Provider::operator == (const Provider &prov) const
{
   if(String::equalNoCase(_name, prov._name))
      return true;
   return false;
}


PEGASUS_NAMESPACE_END
