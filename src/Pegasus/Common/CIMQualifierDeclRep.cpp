//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "CIMQualifierDecl.h"
#include "Indentor.h"
#include "DeclContext.h"
#include "CIMName.h"
#include "Exception.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifierDeclRep
//
////////////////////////////////////////////////////////////////////////////////

CIMQualifierDeclRep::CIMQualifierDeclRep(
    const String& name, 
    const CIMValue& value, 
    Uint32 scope,
    Uint32 flavor,
    Uint32 arraySize)
    : 
    _name(name), 
    _value(value), 
    _scope(scope),
    _flavor(flavor),
    _arraySize(arraySize)
{
    if (!CIMName::legal(name))
	throw IllegalName();

    if (_value.getType() == CIMType::NONE)
	throw NullType();
}

CIMQualifierDeclRep::~CIMQualifierDeclRep()
{

}

void CIMQualifierDeclRep::setName(const String& name) 
{
    if (!CIMName::legal(name))
	throw IllegalName();

    _name = name; 
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void CIMQualifierDeclRep::toXml(Array<Sint8>& out) const
{
    out << "<QUALIFIER.DECLARATION";
    out << " NAME=\"" << _name << "\"";
    out << " TYPE=\"" << TypeToString(_value.getType()) << "\"";

    if (_value.isArray())
    {
	out << " ISARRAY=\"true\"";

	if (_arraySize)
	{
	    char buffer[64];
	    sprintf(buffer, " ARRAYSIZE=\"%d\"", _arraySize);
	    out << buffer;
	}
    }

    FlavorToXml(out, _flavor);

    out << ">\n";

    ScopeToXml(out, _scope);

    _value.toXml(out);

    out << "</QUALIFIER.DECLARATION>\n";
}

void CIMQualifierDeclRep::print(std::ostream &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << std::endl;
}

CIMQualifierDeclRep::CIMQualifierDeclRep()
{

}

CIMQualifierDeclRep::CIMQualifierDeclRep(const CIMQualifierDeclRep& x) : 
    Sharable(),
    _name(x._name),
    _value(x._value),
    _scope(x._scope),
    _flavor(x._flavor),
    _arraySize(x._arraySize)
{

}

CIMQualifierDeclRep& CIMQualifierDeclRep::operator=(const CIMQualifierDeclRep& x) 
{ 
    return *this; 
}

Boolean CIMQualifierDeclRep::identical(const CIMQualifierDeclRep* x) const
{
    return
	this == x ||
	CIMName::equal(_name, x->_name) && 
	_value == x->_value && 
	_scope == x->_scope &&
	_flavor == x->_flavor &&
	_arraySize == x->_arraySize;
}

void CIMQualifierDeclRep::setValue(const CIMValue& value) 
{
    _value = value; 

    if (_value.getType() == CIMType::NONE)
	throw NullType();
}

PEGASUS_NAMESPACE_END
