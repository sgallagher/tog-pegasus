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

#include "OperationContext.h"

PEGASUS_NAMESPACE_BEGIN

//
// OperationContext
//
OperationContext::OperationContext(void)
{
}

OperationContext::OperationContext(const OperationContext & context)
    : _containers(context._containers)
{
}

OperationContext::~OperationContext(void)
{
    clear();
}

OperationContext & OperationContext::operator=(const OperationContext & context)
{
    if(this == &context)
    {
        return(*this);
    }

    clear();

    for(Uint32 i = 0, n = context._containers.size(); i < n; i++)
    {
        _containers.append(context._containers[i]->clone());
    }

    return(*this);
}

void OperationContext::clear(void)
{
    for(Uint32 i = 0, n = _containers.size(); i < n; i++)
    {
        delete _containers[i];
    }

    _containers.clear();
}

const OperationContext::Container & OperationContext::get(const Uint32 key) const
{
    for(Uint32 i = 0, n = _containers.size(); i < n; i++)
    {
        if(key == _containers[i]->getKey())
        {
            Container * p = _containers[i];

            return(*p);
        }
    }

    throw Exception("object not found");
}

void OperationContext::set(const OperationContext::Container & container)
{
    for(Uint32 i = 0, n = _containers.size(); i < n; i++)
    {
        if(container.getKey() == _containers[i]->getKey())
        {
            // delete previous container
            _containers.remove(i);

            // append current container
            _containers.append(container.clone());

            return;
        }
    }

    throw Exception("object not found");
}

void OperationContext::insert(const OperationContext::Container & container)
{
    for(Uint32 i = 0, n = _containers.size(); i < n; i++)
    {
        if(container.getKey() == _containers[i]->getKey())
        {
            throw Exception("object already exists.");
        }
    }

    _containers.append(container.clone());
}

void OperationContext::remove(const Uint32 key)
{
    for(Uint32 i = 0, n = _containers.size(); i < n; i++)
    {
        if(key == _containers[i]->getKey())
        {
            _containers.remove(i);

            return;
        }
    }

    throw Exception("object not found");
}

//
// OperationContext::Container
//
OperationContext::Container::Container(const Uint32 key) : _key(key)
{
}

OperationContext::Container::~Container(void)
{
}

OperationContext::Container * OperationContext::Container::clone(void) const
{
    return(new Container(*this));
}

//
// IdentitiyContainer
//
IdentityContainer::IdentityContainer(const OperationContext::Container & container)
{
    const IdentityContainer * p = dynamic_cast<const IdentityContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailed();
    }

    *this = *p;
}

IdentityContainer::IdentityContainer(const String & userName)
    : OperationContext::Container(CONTEXT_IDENTITY), _userName(userName)
{
}

OperationContext::Container * IdentityContainer::clone(void) const
{
    return(new IdentityContainer(*this));
}

String IdentityContainer::getUserName(void) const
{
    return(_userName);
}

//
// LocaleContainer
//
LocaleContainer::LocaleContainer(const OperationContext::Container & container)
{
    const LocaleContainer * p = dynamic_cast<const LocaleContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailed();
    }

    *this = *p;
}

LocaleContainer::LocaleContainer(const String & languageId)
    : OperationContext::Container(CONTEXT_LOCALE), _languageId(languageId)
{
}

OperationContext::Container * LocaleContainer::clone(void) const
{
    return(new LocaleContainer(*this));
}

String LocaleContainer::getLanguageId(void) const
{
    return(_languageId);
}

//
// ProviderIdContainer
//
ProviderIdContainer::ProviderIdContainer(const OperationContext::Container & container)
{
    const ProviderIdContainer * p = dynamic_cast<const ProviderIdContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailed();
    }

    *this = *p;
}

ProviderIdContainer::ProviderIdContainer(const CIMInstance & module, const CIMInstance & provider)
    : OperationContext::Container(CONTEXT_PROVIDERID), _module(module), _provider(provider)
{
}

OperationContext::Container * ProviderIdContainer::clone(void) const
{
    return(new ProviderIdContainer(*this));
}

CIMInstance ProviderIdContainer::getModule(void) const
{
    return(_module);
}

CIMInstance ProviderIdContainer::getProvider(void) const
{
    return(_provider);
}

PEGASUS_NAMESPACE_END
