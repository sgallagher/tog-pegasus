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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
// 		Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "OperationContext.h"
#include "ArrayInternal.h"
#include <Pegasus/Common/MessageLoader.h> //l10n

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

	//l10n
	MessageLoaderParms parms("Common.OperationContext.OBJECT_NOT_FOUND",
							 "object not found");
    throw Exception(parms);
    //throw Exception("object not found");
}

#ifdef PEGASUS_USE_DEPRECATED_INTERFACES
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

	//l10n
	MessageLoaderParms parms("Common.OperationContext.OBJECT_NOT_FOUND",
							 "object not found");
    throw Exception(parms);
    //throw Exception("object not found");
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

	//l10n
	MessageLoaderParms parms("Common.OperationContext.OBJECT_NOT_FOUND",
							 "object not found");
    throw Exception(parms);
    //throw Exception("object not found");
}

void OperationContext::insert(const OperationContext::Container & container)
{
    for(Uint32 i = 0, n = _rep->containers.size(); i < n; i++)
    {
        if(container.getName() == _rep->containers[i]->getName())
        {
        	//l10n
			MessageLoaderParms parms("Common.OperationContext.OBJECT_ALREADY_EXISTS",
							 "object already exists.");
    		throw Exception(parms);
            //throw Exception("object already exists.");
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

	//l10n
	MessageLoaderParms parms("Common.OperationContext.OBJECT_NOT_FOUND",
							 "object not found");
    throw Exception(parms);
    //throw Exception("object not found");
}
 
#ifdef PEGASUS_USE_DEPRECATED_INTERFACES
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

	//l10n
	MessageLoaderParms parms("Common.OperationContext.OBJECT_NOT_FOUND",
							 "object not found");
    throw Exception(parms);
    //throw Exception("object not found");
}
#endif

//
// OperationContext::Container
//

#ifdef PEGASUS_USE_DEPRECATED_INTERFACES
OperationContext::Container::Container(const Uint32 key) : _key(key)
{
}
#endif

OperationContext::Container::~Container(void)
{
}

#ifdef PEGASUS_USE_DEPRECATED_INTERFACES
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
#ifdef PEGASUS_USE_DEPRECATED_INTERFACES
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
#ifdef PEGASUS_USE_DEPRECATED_INTERFACES
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


const String TimeoutContainer::NAME = "TimeoutContainer";

TimeoutContainer::TimeoutContainer(const OperationContext::Container & container)
{
   const TimeoutContainer * p = dynamic_cast<const TimeoutContainer *>(&container);
   if(p == 0)
   {
      throw DynamicCastFailedException();
   }
   _value = p->_value;
}

TimeoutContainer::TimeoutContainer(Uint32 timeout)
{
   _value = timeout;
}

String TimeoutContainer::getName(void) const 
{
   return (NAME);
}

OperationContext::Container * TimeoutContainer::clone(void) const
{
   return (new TimeoutContainer(_value));
}

void TimeoutContainer::destroy(void)
{
   delete this;
}

Uint32 TimeoutContainer::getTimeOut(void) const
{
   return _value;
}


// l10n start

//
// AcceptLanguageListContainer
//

class AcceptLanguageListContainerRep
{
public:
    AcceptLanguages languages;
};

const String AcceptLanguageListContainer::NAME =
    "AcceptLanguageListContainer";

AcceptLanguageListContainer::AcceptLanguageListContainer
    (const OperationContext::Container & container)
{
    const AcceptLanguageListContainer * p = 
    	dynamic_cast<const AcceptLanguageListContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    _rep = new AcceptLanguageListContainerRep();
    _rep->languages = p->_rep->languages;
}

AcceptLanguageListContainer::AcceptLanguageListContainer
    (const AcceptLanguageListContainer & container)
{
    _rep = new AcceptLanguageListContainerRep();
    _rep->languages = container._rep->languages;
}

AcceptLanguageListContainer::AcceptLanguageListContainer
    (const AcceptLanguages & languages)
{
    _rep = new AcceptLanguageListContainerRep();
    _rep->languages = languages;
}

AcceptLanguageListContainer::~AcceptLanguageListContainer(void)
{
    delete _rep;
}

AcceptLanguageListContainer & AcceptLanguageListContainer::operator=(
    const AcceptLanguageListContainer & container)
{
    if (this == &container)
    {
        return (*this);
    }

    _rep->languages = container._rep->languages;

    return (*this);
}

String AcceptLanguageListContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * AcceptLanguageListContainer::clone(void) const
{
    return(new AcceptLanguageListContainer(_rep->languages));
}

void AcceptLanguageListContainer::destroy(void)
{
    delete this;
}

AcceptLanguages AcceptLanguageListContainer::getLanguages(void) const
{
    return(_rep->languages);
}

//
// SubscriptionLanguageListContainer
//

class SubscriptionLanguageListContainerRep
{
public:
    AcceptLanguages languages;
};

const String SubscriptionLanguageListContainer::NAME =
    "SubscriptionLanguageListContainer";

SubscriptionLanguageListContainer::SubscriptionLanguageListContainer
    (const OperationContext::Container & container)
{
    const SubscriptionLanguageListContainer * p = 
    	dynamic_cast<const SubscriptionLanguageListContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    _rep = new SubscriptionLanguageListContainerRep();
    _rep->languages = p->_rep->languages;
}

SubscriptionLanguageListContainer::SubscriptionLanguageListContainer
    (const SubscriptionLanguageListContainer & container)
{
    _rep = new SubscriptionLanguageListContainerRep();
    _rep->languages = container._rep->languages;
}

SubscriptionLanguageListContainer::SubscriptionLanguageListContainer
    (const AcceptLanguages & languages)
{
    _rep = new SubscriptionLanguageListContainerRep();
    _rep->languages = languages;
}

SubscriptionLanguageListContainer::~SubscriptionLanguageListContainer(void)
{
    delete _rep;
}

SubscriptionLanguageListContainer & SubscriptionLanguageListContainer::operator=(
    const SubscriptionLanguageListContainer & container)
{
    if (this == &container)
    {
        return (*this);
    }

    _rep->languages = container._rep->languages;

    return (*this);
}

String SubscriptionLanguageListContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * SubscriptionLanguageListContainer::clone(void) const
{
    return(new SubscriptionLanguageListContainer(_rep->languages));
}

void SubscriptionLanguageListContainer::destroy(void)
{
    delete this;
}

AcceptLanguages SubscriptionLanguageListContainer::getLanguages(void) const
{
    return(_rep->languages);
}

//
// ContentLanguageListContainer
//

class ContentLanguageListContainerRep
{
public:
    ContentLanguages languages;
};

const String ContentLanguageListContainer::NAME =
    "ContentLanguageListContainer";

ContentLanguageListContainer::ContentLanguageListContainer
    (const OperationContext::Container & container)
{
    const ContentLanguageListContainer * p = 
    	dynamic_cast<const ContentLanguageListContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    _rep = new ContentLanguageListContainerRep();
    _rep->languages = p->_rep->languages;
}

ContentLanguageListContainer::ContentLanguageListContainer
    (const ContentLanguageListContainer & container)
{
    _rep = new ContentLanguageListContainerRep();
    _rep->languages = container._rep->languages;
}

ContentLanguageListContainer::ContentLanguageListContainer
    (const ContentLanguages & languages)
{
    _rep = new ContentLanguageListContainerRep();
    _rep->languages = languages;
}

ContentLanguageListContainer::~ContentLanguageListContainer(void)
{
    delete _rep;
}

ContentLanguageListContainer & ContentLanguageListContainer::operator=(
    const ContentLanguageListContainer & container)
{
    if (this == &container)
    {
        return (*this);
    }

    _rep->languages = container._rep->languages;

    return (*this);
}

String ContentLanguageListContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * ContentLanguageListContainer::clone(void) const
{
    return(new ContentLanguageListContainer(_rep->languages));
}

void ContentLanguageListContainer::destroy(void)
{
    delete this;
}

ContentLanguages ContentLanguageListContainer::getLanguages(void) const
{
    return(_rep->languages);
}


// l10n end

//
// SnmpTrapOidContainer
//

class SnmpTrapOidContainerRep
{
public:
    String snmpTrapOid;
};

const String SnmpTrapOidContainer::NAME =
    "SnmpTrapOidContainer";

SnmpTrapOidContainer::SnmpTrapOidContainer
    (const OperationContext::Container & container)
{
    const SnmpTrapOidContainer * p =
        dynamic_cast<const SnmpTrapOidContainer *>(&container);

    if(p == 0)
    {
        throw DynamicCastFailedException();
    }

    _rep = new SnmpTrapOidContainerRep();
    _rep->snmpTrapOid = p->_rep->snmpTrapOid;
}

SnmpTrapOidContainer::SnmpTrapOidContainer
    (const SnmpTrapOidContainer & container)
{
    _rep = new SnmpTrapOidContainerRep();
    _rep->snmpTrapOid = container._rep->snmpTrapOid;
}

SnmpTrapOidContainer::SnmpTrapOidContainer
    (const String & snmpTrapOid)
{
    _rep = new SnmpTrapOidContainerRep();
    _rep->snmpTrapOid = snmpTrapOid;
}

SnmpTrapOidContainer::~SnmpTrapOidContainer(void)
{
    delete _rep;
}

SnmpTrapOidContainer & SnmpTrapOidContainer::operator=(
    const SnmpTrapOidContainer & container)
{
    if (this == &container)
    {
        return (*this);
    }

    _rep->snmpTrapOid = container._rep->snmpTrapOid;

    return (*this);
}

String SnmpTrapOidContainer::getName(void) const
{
    return(NAME);
}

OperationContext::Container * SnmpTrapOidContainer::clone(void) const
{
    return(new SnmpTrapOidContainer(_rep->snmpTrapOid));
}

void SnmpTrapOidContainer::destroy(void)
{
    delete this;
}

String SnmpTrapOidContainer::getSnmpTrapOid(void) const
{
    return(_rep->snmpTrapOid);
}
PEGASUS_NAMESPACE_END
