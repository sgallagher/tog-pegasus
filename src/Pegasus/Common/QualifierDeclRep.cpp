//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: QualifierDeclRep.cpp,v $
// Revision 1.1  2001/01/14 19:53:08  mike
// Initial revision
//
//
//END_HISTORY

#include <cstdio>
#include "QualifierDecl.h"
#include "Indentor.h"
#include "DeclContext.h"
#include "Name.h"
#include "Exception.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// QualifierDeclRep
//
////////////////////////////////////////////////////////////////////////////////

QualifierDeclRep::QualifierDeclRep(
    const String& name, 
    const Value& value, 
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
    if (!Name::legal(name))
	throw IllegalName();

    if (_value.getType() == Type::NONE)
	throw NullType();
}

QualifierDeclRep::~QualifierDeclRep()
{

}

void QualifierDeclRep::setName(const String& name) 
{
    if (!Name::legal(name))
	throw IllegalName();

    _name = name; 
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void QualifierDeclRep::toXml(Array<Sint8>& out) const
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

void QualifierDeclRep::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

QualifierDeclRep::QualifierDeclRep()
{

}

QualifierDeclRep::QualifierDeclRep(const QualifierDeclRep& x) : 
    Sharable(),
    _name(x._name),
    _value(x._value),
    _scope(x._scope),
    _flavor(x._flavor),
    _arraySize(x._arraySize)
{

}

QualifierDeclRep& QualifierDeclRep::operator=(const QualifierDeclRep& x) 
{ 
    return *this; 
}

Boolean QualifierDeclRep::identical(const QualifierDeclRep* x) const
{
    return
	this == x ||
	_name == x->_name && 
	_value == x->_value && 
	_scope == x->_scope &&
	_flavor == x->_flavor &&
	_arraySize == x->_arraySize;
}

PEGASUS_NAMESPACE_END
