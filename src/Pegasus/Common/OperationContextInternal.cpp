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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "OperationContextInternal.h"

PEGASUS_NAMESPACE_BEGIN

//
// LocaleContainer
//

const String LocaleContainer::NAME = "LocaleContainer";

LocaleContainer::LocaleContainer(const OperationContext::Container & container)
{
    const LocaleContainer * p = dynamic_cast<const LocaleContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    *this = *p;
}

LocaleContainer::LocaleContainer(const String & languageId)
{
    _languageId = languageId;
}

LocaleContainer::~LocaleContainer(void)
{
}

LocaleContainer & LocaleContainer::operator=(const LocaleContainer &container)
{
    if(this == &container)
    {
        return(*this);
    }

    _languageId = container._languageId;

    return(*this);
}

String LocaleContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * LocaleContainer::clone(void) const
{
    return(new LocaleContainer(*this));
}

void LocaleContainer::destroy(void)
{
    delete this;
}

String LocaleContainer::getLanguageId(void) const
{
    return(_languageId);
}

//
// ProviderIdContainer
//

const String ProviderIdContainer::NAME = "ProviderIdContainer";

ProviderIdContainer::ProviderIdContainer(const OperationContext::Container & container)
{
    const ProviderIdContainer * p = dynamic_cast<const ProviderIdContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    *this = *p;
}

ProviderIdContainer::ProviderIdContainer(
    const CIMInstance & module,
    const CIMInstance & provider,
    Boolean isRemoteNameSpace,
    const String & remoteInfo)
    : _module(module),
    _provider(provider),
    _isRemoteNameSpace(isRemoteNameSpace),
    _remoteInfo(remoteInfo)
{
}

ProviderIdContainer::~ProviderIdContainer(void)
{
}

ProviderIdContainer & ProviderIdContainer::operator=(const ProviderIdContainer & container)
{
    if(this == &container)
    {
        return(*this);
    }

    _module = container._module;
    _provider = container._provider;
    _isRemoteNameSpace = container._isRemoteNameSpace;
    _remoteInfo = container._remoteInfo;

    return(*this);
}

String ProviderIdContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * ProviderIdContainer::clone(void) const
{
    return(new ProviderIdContainer(*this));
}

void ProviderIdContainer::destroy(void)
{
    delete this;
}

CIMInstance ProviderIdContainer::getModule(void) const
{
    return(_module);
}

CIMInstance ProviderIdContainer::getProvider(void) const
{
    return(_provider);
}

Boolean ProviderIdContainer::isRemoteNameSpace(void) const
{
    return(_isRemoteNameSpace);
}

String ProviderIdContainer::getRemoteInfo(void) const
{
    return(_remoteInfo);
}

//
// CachedClassDefinitionContainer
//

const String CachedClassDefinitionContainer::NAME = "CachedClassDefinitionContainer";

CachedClassDefinitionContainer::CachedClassDefinitionContainer(const OperationContext::Container & container)
{
    const CachedClassDefinitionContainer * p = dynamic_cast<const CachedClassDefinitionContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    *this = *p;
}


CachedClassDefinitionContainer::CachedClassDefinitionContainer(const CIMClass & cimClass)
    : _cimClass(cimClass)
{
}

CachedClassDefinitionContainer::~CachedClassDefinitionContainer(void)
{
}

CachedClassDefinitionContainer & CachedClassDefinitionContainer::operator=(const CachedClassDefinitionContainer & container)
{
    if(this == &container)
    {
        return(*this);
    }

    _cimClass = container._cimClass;

    return(*this);
}

String CachedClassDefinitionContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * CachedClassDefinitionContainer::clone(void) const
{
    return(new CachedClassDefinitionContainer(*this));
}

void CachedClassDefinitionContainer::destroy(void)
{
    delete this;
}

CIMClass CachedClassDefinitionContainer::getClass(void) const
{
    return(_cimClass);
}

PEGASUS_NAMESPACE_END
