//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMNamedInstance.h"
#include "XmlWriter.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMNamedInstance
//
////////////////////////////////////////////////////////////////////////////////

CIMNamedInstance::CIMNamedInstance()
{

}

CIMNamedInstance::CIMNamedInstance(
    const CIMReference& instanceName,
    const CIMInstance& instance) 
    : _instanceName(instanceName), _instance(instance)
{

}

CIMNamedInstance::CIMNamedInstance(const CIMNamedInstance& namedInstance)
    : _instanceName(namedInstance._instanceName),
      _instance(namedInstance._instance)
{

}

CIMNamedInstance::~CIMNamedInstance()
{

}

CIMNamedInstance& CIMNamedInstance::operator=(
    const CIMNamedInstance& namedInstance
)
{
    if (this != &namedInstance)
    {
	_instanceName = namedInstance._instanceName;
	_instance = namedInstance._instance;
    }
    return *this;
}

void CIMNamedInstance::set(
    const CIMReference& instanceName, 
    const CIMInstance& instance)
{
    _instanceName = instanceName;
    _instance = instance;
}

//------------------------------------------------------------------------------
//
// <!ELEMENT VALUE.NAMEDINSTANCE (INSTANCENAME,INSTANCE)>
//
//------------------------------------------------------------------------------

void CIMNamedInstance::toXml(Array<Sint8>& out) const
{
    out << "<VALUE.NAMEDINSTANCE>\n";

    XmlWriter::appendInstanceNameElement(out, _instanceName);
    _instance.toXml(out);

    out << "</VALUE.NAMEDINSTANCE>\n";
}

PEGASUS_NAMESPACE_END
