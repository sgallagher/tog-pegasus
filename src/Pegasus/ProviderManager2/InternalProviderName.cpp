//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2003 BMC Software, Hewlett-Packard Company, IBM,
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "InternalProviderName.h"

#include <Pegasus/Common/CIMValue.h>

PEGASUS_NAMESPACE_BEGIN

InternalProviderName::InternalProviderName(const String & providerName)
    : _physicalName(String::EMPTY), _logicalName(String::EMPTY), _objectName(String::EMPTY), _capabilities(0)
{
    // NOTE: format ::= <physical_name>::<logical_name>::<object_name>::<capabilities>

    String s = providerName;

    Uint32 beg = 0;
    Uint32 end = 0;

    // get physical name
    beg = 0;
    end = s.find("::");

    String temp = s.subString(beg, end - beg);
    _physicalName = temp;

    s.remove(beg, end + 2); // skip past the "::"
    beg = end;

    // get logical name
    beg = 0;
    end = s.find("::");

    temp = s.subString(beg, end - beg);
    _logicalName = temp;

    s.remove(beg, end + 2); // skip past the "::"
    beg = end;

    // get object name
    beg = 0;
    end = s.find("::");

    temp = s.subString(beg, end - beg);
    _objectName = temp;

    //s.remove(beg, end + 2); // skip past the "::"
    //beg = end;

    _objectName = temp;

    /*
    // get capabilities mask
    beg = 0;
    end = s.find("::");

    _physicalName = String(s.subString(beg, end - beg));
    */
}

InternalProviderName::InternalProviderName(
    const String & physicalName,
    const String & logicalName,
    const String & objectName)
    : _physicalName(String::EMPTY), _logicalName(String::EMPTY), _objectName(String::EMPTY), _capabilities(0)
{
    // ATTN: validate arguments ???
    _physicalName = physicalName;

    _logicalName = logicalName;

    _objectName = objectName;
}

InternalProviderName::~InternalProviderName(void)
{
}

String InternalProviderName::toString(void) const
{
    String s;

    s.append(_physicalName);
    s.append("::");
    s.append(_logicalName);
    s.append("::");
    s.append(_objectName);
    //s.append("::");
    //s.append(CIMValue(_capabilities).toString());

    return(s);
}

String InternalProviderName::getPhysicalName(void) const
{
    return(_physicalName);
}

void InternalProviderName::setPhysicalName(const String & physicalName)
{
    _physicalName = physicalName;
}

String InternalProviderName::getLogicalName(void) const
{
    return(_logicalName);
}

void InternalProviderName::setLogicalName(const String & logicalName)
{
    _logicalName = logicalName;
}

String InternalProviderName::getObjectName(void) const
{
    return(_objectName);
}

void InternalProviderName::setObjectName(const String & logicalName)
{
    _logicalName = logicalName;
}

Uint32 InternalProviderName::getCapabilitiesMask(void) const
{
    return(_capabilities);
}

void InternalProviderName::setCapabilitiesMask(const Uint32 capabilities)
{
    _capabilities = capabilities;
}

PEGASUS_NAMESPACE_END
