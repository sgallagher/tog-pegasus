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
// Modified By: Adrian Schuur (schuur@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderName.h"

#include <Pegasus/Common/CIMValue.h>

PEGASUS_NAMESPACE_BEGIN

ProviderName::ProviderName(void) : _capabilities(0)
{
}

ProviderName::ProviderName(
    const CIMNamespaceName & nameSpace,
    const CIMName & className,
    const Uint32 capabilities,
    const CIMName & method)
    : _capabilities(capabilities)
{
    _nameSpace = nameSpace;
    _className = className;
    _method = method;
}

ProviderName::ProviderName(
    const CIMObjectPath & path,
    const Uint32 capabilities,
    const CIMName & method)
    : _capabilities(capabilities)
{
    _nameSpace = path.getNameSpace();
    _className = path.getClassName();
    _method = method;
}

ProviderName::ProviderName(
    const String & logicalName,
    const String & physicalName,
    const String & interfaceName,
    const Uint32 capabilities,
    const CIMName & method)
    : _capabilities(capabilities)
{
    _logicalName = logicalName;
    _physicalName = physicalName;
    _interfaceName = interfaceName;    
    _method = method;
}

ProviderName::~ProviderName(void)
{
}

String ProviderName::getPhysicalName(void) const
{
    return(_physicalName);
}

void ProviderName::setPhysicalName(const String & physicalName)
{
    _physicalName = physicalName;
}

String ProviderName::getLogicalName(void) const
{
    return(_logicalName);
}

String ProviderName::getInterfaceName(void) const
{
    return(_interfaceName);
}

String ProviderName::getLocation(void) const
{
    return(_location);
}

void ProviderName::setLocation(const String &location)
{
    _location=location;
}

Uint32 ProviderName::getCapabilitiesMask(void) const
{
    return(_capabilities);
}

CIMName ProviderName::getMethodName(void) const
{
    return(_method);
}

CIMName ProviderName::getClassName() const {
   return _className;
}

CIMNamespaceName ProviderName::getNameSpace() const {
   return _nameSpace;
}   

PEGASUS_NAMESPACE_END
