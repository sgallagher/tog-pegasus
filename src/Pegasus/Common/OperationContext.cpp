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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "OperationContext.h"
#include "ArrayInternal.h"

PEGASUS_NAMESPACE_BEGIN

//
// OperationContext
//

class OperationContextRep
{
public:
    Array<OperationContext::Container *> containers;
};

OperationContext::OperationContext(void)
{
    _rep = new OperationContextRep;
}

OperationContext::OperationContext(const OperationContext & context)
{
    _rep = new OperationContextRep;
    *this = context;
}

OperationContext::~OperationContext(void)
{
    clear();
    delete _rep;
}

OperationContext & OperationContext::operator=(const OperationContext & context)
{
    if(this == &context)
    {
        return(*this);
    }

    clear();

    for(Uint32 i = 0, n = context._rep->containers.size(); i < n; i++)
    {
        _rep->containers.append(context._rep->containers[i]->clone());
    }

    return(*this);
}

void OperationContext::clear(void)
{
    for(Uint32 i = 0, n = _rep->containers.size(); i < n; i++)
    {
        _rep->containers[i]->destroy();
    }

    _rep->containers.clear();
}

const OperationContext::Container & OperationContext::get(
    const String& containerName) const
{
    for(Uint32 i = 0, n = _rep->containers.size(); i < n; i++)
    {
        if(containerName == _rep->containers[i]->getName())
        {
            Container * p = _rep->containers[i];

            return(*p);
        }
    }

    throw Exception("object not found");
}

#ifndef PEGASUS_REMOVE_DEPRECATED
const OperationContext::Container & OperationContext::get(const Uint32 key) const
{
    for(Uint32 i = 0, n = _rep->containers.size(); i < n; i++)
    {
        if(key == _rep->containers[i]->getKey())
        {
            Container * p = _rep->containers[i];

            return(*p);
        }
    }

    throw Exception("object not found");
}
#endif

void OperationContext::set(const OperationContext::Container & container)
{
    for(Uint32 i = 0, n = _rep->containers.size(); i < n; i++)
    {
        if(container.getName() == _rep->containers[i]->getName())
        {
            // delete previous container
            _rep->containers[i]->destroy();
            _rep->containers.remove(i);

            // append current container
            _rep->containers.append(container.clone());

            return;
        }
    }

    throw Exception("object not found");
}

void OperationContext::insert(const OperationContext::Container & container)
{
    for(Uint32 i = 0, n = _rep->containers.size(); i < n; i++)
    {
        if(container.getName() == _rep->containers[i]->getName())
        {
            throw Exception("object already exists.");
        }
    }

    _rep->containers.append(container.clone());
}

void OperationContext::remove(const String& containerName)
{
    for(Uint32 i = 0, n = _rep->containers.size(); i < n; i++)
    {
        if(containerName == _rep->containers[i]->getName())
        {
            _rep->containers[i]->destroy();
            _rep->containers.remove(i);

            return;
        }
    }

    throw Exception("object not found");
}

#ifndef PEGASUS_REMOVE_DEPRECATED
void OperationContext::remove(const Uint32 key)
{
    for(Uint32 i = 0, n = _rep->containers.size(); i < n; i++)
    {
        if(key == _rep->containers[i]->getKey())
        {
            delete _rep->containers[i];
            _rep->containers.remove(i);

            return;
        }
    }

    throw Exception("object not found");
}
#endif

//
// OperationContext::Container
//

#ifndef PEGASUS_REMOVE_DEPRECATED
OperationContext::Container::Container(const Uint32 key) : _key(key)
{
}
#endif

OperationContext::Container::~Container(void)
{
}

#ifndef PEGASUS_REMOVE_DEPRECATED
const Uint32 & OperationContext::Container::getKey(void) const
{
    return(_key);
}
#endif

//
// IdentityContainer
//

class IdentityContainerRep
{
public:
    String userName;
};

const String IdentityContainer::NAME = "IdentityContainer";

IdentityContainer::IdentityContainer(const OperationContext::Container & container)
#ifndef PEGASUS_REMOVE_DEPRECATED
    : OperationContext::Container(container.getKey())
#endif
{
    const IdentityContainer * p = dynamic_cast<const IdentityContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    _rep = new IdentityContainerRep();
    _rep->userName = p->_rep->userName;
}

IdentityContainer::IdentityContainer(const IdentityContainer & container)
{
    _rep = new IdentityContainerRep();
    _rep->userName = container._rep->userName;
}

IdentityContainer::IdentityContainer(const String & userName)
#ifndef PEGASUS_REMOVE_DEPRECATED
    : OperationContext::Container(CONTEXT_IDENTITY)
#endif
{
    _rep = new IdentityContainerRep();
    _rep->userName = userName;
}

IdentityContainer::~IdentityContainer(void)
{
    delete _rep;
}

IdentityContainer & IdentityContainer::operator=(
    const IdentityContainer & container)
{
    if (this == &container)
    {
        return (*this);
    }

    _rep->userName = container._rep->userName;

    return (*this);
}

String IdentityContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * IdentityContainer::clone(void) const
{
    return(new IdentityContainer(_rep->userName));
}

void IdentityContainer::destroy(void)
{
    delete this;
}

String IdentityContainer::getUserName(void) const
{
    return(_rep->userName);
}

//
// SubscriptionInstanceContainer
//

class SubscriptionInstanceContainerRep
{
public:
    CIMInstance subscriptionInstance;
};

const String SubscriptionInstanceContainer::NAME =
    "SubscriptionInstanceContainer";

SubscriptionInstanceContainer::SubscriptionInstanceContainer
    (const OperationContext::Container & container)
{
    const SubscriptionInstanceContainer * p =
        dynamic_cast<const SubscriptionInstanceContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    _rep = new SubscriptionInstanceContainerRep();
    _rep->subscriptionInstance = p->_rep->subscriptionInstance;
}

SubscriptionInstanceContainer::SubscriptionInstanceContainer
    (const SubscriptionInstanceContainer & container)
{
    _rep = new SubscriptionInstanceContainerRep();
    _rep->subscriptionInstance = container._rep->subscriptionInstance;
}

SubscriptionInstanceContainer::SubscriptionInstanceContainer
    (const CIMInstance & subscriptionInstance)
{
    _rep = new SubscriptionInstanceContainerRep();
    _rep->subscriptionInstance = subscriptionInstance;
}

SubscriptionInstanceContainer::~SubscriptionInstanceContainer(void)
{
    delete _rep;
}

SubscriptionInstanceContainer & SubscriptionInstanceContainer::operator=(
    const SubscriptionInstanceContainer & container)
{
    if (this == &container)
    {
        return (*this);
    }

    _rep->subscriptionInstance = container._rep->subscriptionInstance;

    return (*this);
}

String SubscriptionInstanceContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * SubscriptionInstanceContainer::clone(void) const
{
    return(new SubscriptionInstanceContainer(_rep->subscriptionInstance));
}

void SubscriptionInstanceContainer::destroy(void)
{
    delete this;
}

CIMInstance SubscriptionInstanceContainer::getInstance(void) const
{
    return(_rep->subscriptionInstance);
}


//
// SubscriptionInstanceNamesContainer
//

class SubscriptionInstanceNamesContainerRep
{
public:
    Array<CIMObjectPath> subscriptionInstanceNames;
};

const String SubscriptionInstanceNamesContainer::NAME = 
    "SubscriptionInstanceNamesContainer";

SubscriptionInstanceNamesContainer::SubscriptionInstanceNamesContainer
    (const OperationContext::Container & container)
{
    const SubscriptionInstanceNamesContainer * p = 
        dynamic_cast<const SubscriptionInstanceNamesContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    _rep = new SubscriptionInstanceNamesContainerRep();
    _rep->subscriptionInstanceNames = p->_rep->subscriptionInstanceNames;
}

SubscriptionInstanceNamesContainer::SubscriptionInstanceNamesContainer
    (const SubscriptionInstanceNamesContainer & container)
{
    _rep = new SubscriptionInstanceNamesContainerRep();
    _rep->subscriptionInstanceNames = container._rep->subscriptionInstanceNames;
}

SubscriptionInstanceNamesContainer::SubscriptionInstanceNamesContainer
    (const Array<CIMObjectPath> & subscriptionInstanceNames)
{
    _rep = new SubscriptionInstanceNamesContainerRep();
    _rep->subscriptionInstanceNames = subscriptionInstanceNames;
}

SubscriptionInstanceNamesContainer::~SubscriptionInstanceNamesContainer(void)
{
    delete _rep;
}

SubscriptionInstanceNamesContainer & 
    SubscriptionInstanceNamesContainer::operator=(
    const SubscriptionInstanceNamesContainer & container)
{
    if (this == &container)
    {
        return (*this);
    }

    _rep->subscriptionInstanceNames = container._rep->subscriptionInstanceNames;

    return (*this);
}

String SubscriptionInstanceNamesContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * 
    SubscriptionInstanceNamesContainer::clone(void) const
{
    return(new SubscriptionInstanceNamesContainer
        (_rep->subscriptionInstanceNames));
}

void SubscriptionInstanceNamesContainer::destroy(void)
{
    delete this;
}

Array<CIMObjectPath> 
    SubscriptionInstanceNamesContainer::getInstanceNames(void) const
{
    return(_rep->subscriptionInstanceNames);
}

//
// SubscriptionFilterConditionContainer
//

class SubscriptionFilterConditionContainerRep
{
public:
    String filterCondition;
    String queryLanguage;
};

const String SubscriptionFilterConditionContainer::NAME = 
    "SubscriptionFilterConditionContainer";

SubscriptionFilterConditionContainer::SubscriptionFilterConditionContainer
    (const OperationContext::Container & container)
{
    const SubscriptionFilterConditionContainer * p = 
        dynamic_cast<const SubscriptionFilterConditionContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    _rep = new SubscriptionFilterConditionContainerRep();
    _rep->filterCondition = p->_rep->filterCondition;
    _rep->queryLanguage = p->_rep->queryLanguage;
}

SubscriptionFilterConditionContainer::SubscriptionFilterConditionContainer
    (const SubscriptionFilterConditionContainer & container)
{
    _rep = new SubscriptionFilterConditionContainerRep();
    _rep->filterCondition = container._rep->filterCondition;
    _rep->queryLanguage = container._rep->queryLanguage;
}

SubscriptionFilterConditionContainer::SubscriptionFilterConditionContainer(
    const String & filterCondition,
    const String & queryLanguage)
{
    _rep = new SubscriptionFilterConditionContainerRep();
    _rep->filterCondition = filterCondition;
    _rep->queryLanguage = queryLanguage;
}

SubscriptionFilterConditionContainer::~SubscriptionFilterConditionContainer
    (void)
{
    delete _rep;
}

SubscriptionFilterConditionContainer & 
    SubscriptionFilterConditionContainer::operator=(
    const SubscriptionFilterConditionContainer & container)
{
    if (this == &container)
    {
        return (*this);
    }

    _rep->filterCondition = container._rep->filterCondition;
    _rep->queryLanguage = container._rep->queryLanguage;

    return (*this);
}

String SubscriptionFilterConditionContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * SubscriptionFilterConditionContainer::clone(void) const
{
    return(new SubscriptionFilterConditionContainer(_rep->filterCondition, 
        _rep->queryLanguage));
}

void SubscriptionFilterConditionContainer::destroy(void)
{
    delete this;
}

String SubscriptionFilterConditionContainer::getFilterCondition(void) const
{
    return(_rep->filterCondition);
}

String SubscriptionFilterConditionContainer::getQueryLanguage(void) const
{
    return(_rep->queryLanguage);
}

PEGASUS_NAMESPACE_END
