//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

#include "ProviderManagerModule.h"

PEGASUS_NAMESPACE_BEGIN

ProviderManagerModule::ProviderManagerModule(void)
{
}

ProviderManagerModule::ProviderManagerModule(const ProviderManagerModule & module) : DynamicLibrary(module)
{
}

ProviderManagerModule::ProviderManagerModule(const String & fileName) : DynamicLibrary(fileName)
{
}

ProviderManagerModule::~ProviderManagerModule(void)
{
}

ProviderManagerModule & ProviderManagerModule::operator=(const ProviderManagerModule & module)
{
    if(this == &module)
    {
        return(*this);
    }

    DynamicLibrary::operator=(module);

    return(*this);
}

Boolean ProviderManagerModule::load(void)
{
    if(DynamicLibrary::load())
    {
        // export entry points
        _createProviderManager = (CREATE_PROVIDER_MANAGER_FUNCTION)getSymbol("PegasusCreateProviderManager");

        if(_createProviderManager != 0)
        {
            return(true);
        }

        DynamicLibrary::unload();
    }

    return(false);
}

Boolean ProviderManagerModule::unload(void)
{
    return(DynamicLibrary::unload());
}

ProviderManager * ProviderManagerModule::getProviderManager(const String & s) const
{
    return(_createProviderManager(s));
}

PEGASUS_NAMESPACE_END
