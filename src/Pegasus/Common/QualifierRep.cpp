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
// $Log: QualifierRep.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:53:10  mike
// Pegasus import
//
//
//END_HISTORY

#include "Qualifier.h"
#include "Indentor.h"
#include "DeclContext.h"
#include "Name.h"
#include "Exception.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// QualifierRep
//
////////////////////////////////////////////////////////////////////////////////

QualifierRep::QualifierRep(
    const String& name, 
    const Value& value, 
    Uint32 flavor,
    Boolean propagated)
    : 
    _name(name), 
    _value(value), 
    _flavor(flavor),
    _propagated(propagated)
{
    if (!Name::legal(name))
	throw IllegalName();

    if (_value.getType() == Type::NONE)
	throw NullType();
}

QualifierRep::~QualifierRep()
{

}

void QualifierRep::setName(const String& name) 
{
    if (!Name::legal(name))
	throw IllegalName();

    _name = name; 
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void QualifierRep::toXml(Array<Sint8>& out) const
{
    out << "<QUALIFIER";
    out << " NAME=\"" << _name << "\"";
    out << " TYPE=\"" << TypeToString(_value.getType()) << "\"";

    if (_propagated != false)
	out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

    FlavorToXml(out, _flavor);

    out << ">\n";

    _value.toXml(out);

    out << "</QUALIFIER>\n";
}

void QualifierRep::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

QualifierRep::QualifierRep()
{

}

QualifierRep::QualifierRep(const QualifierRep& x) : 
    Sharable(),
    _name(x._name),
    _value(x._value),
    _flavor(x._flavor),
    _propagated(x._propagated)
{

}

QualifierRep& QualifierRep::operator=(const QualifierRep& x) 
{ 
    return *this; 
}

Boolean QualifierRep::identical(const QualifierRep* x) const
{
    return
	this == x ||
	_name == x->_name && 
	_value == x->_value && 
	_flavor == x->_flavor &&
	_propagated == x->_propagated;
}

PEGASUS_NAMESPACE_END
