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

OperationContext::OperationContext(void)
{
}

OperationContext::OperationContext(const OperationContext & context)
    : _containers(context._containers)
{
}

OperationContext::~OperationContext(void)
{
}

void OperationContext::clear(void)
{
    _containers.clear();
}

const OperationContext::Container OperationContext::get(const Uint32 key) const
{
    for(Uint32 i = 0, n = _containers.size(); i < n; i++)
    {
        if(key == _containers[i].getKey())
        {
            return(_containers[i]);
        }
    }

    throw Exception("object not found");
}

void OperationContext::set(const OperationContext::Container & container)
{
    for(Uint32 i = 0, n = _containers.size(); i < n; i++)
    {
        if(container.getKey() == _containers[i].getKey())
        {
            // delete previous container
            _containers.remove(i);

            // append current container
            _containers.append(container);

            return;
        }
    }

    throw Exception("object not found");
}

void OperationContext::insert(const OperationContext::Container & container)
{
    for(Uint32 i = 0, n = _containers.size(); i < n; i++)
    {
        if(container.getKey() == _containers[i].getKey())
        {
            throw Exception("object already exists.");
        }
    }

    _containers.append(container);
}

void OperationContext::remove(const Uint32 key)
{
    for(Uint32 i = 0, n = _containers.size(); i < n; i++)
    {
        if(key == _containers[i].getKey())
        {
            _containers.remove(i);

            return;
        }
    }

    throw Exception("object not found");
}

OperationContext::Container::Container(const Uint32 key) : _key(key)
{
}

OperationContext::Container::~Container(void)
{
}

IdentityContainer::IdentityContainer(const String & userName)
    : OperationContext::Container(CONTEXT_IDENTITY), _userName(userName)
{
}

IdentityContainer::~IdentityContainer(void)
{
}

LocaleContainer::LocaleContainer(const String & languageId)
    : OperationContext::Container(CONTEXT_LOCALE), _languageId(languageId)
{
}

LocaleContainer::~LocaleContainer(void)
{
}

PEGASUS_NAMESPACE_END
