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
// $Log: ParameterRep.cpp,v $
// Revision 1.1  2001/01/14 19:53:02  mike
// Initial revision
//
//
//END_HISTORY

#include <cstdio>
#include "Parameter.h"
#include "Indentor.h"
#include "Name.h"
#include "Scope.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

ParameterRep::ParameterRep(
    const String& name, 
    Type type,
    Boolean isArray,
    Uint32 arraySize,
    const String& referenceClassName) 
    : _name(name), _type(type), 
    _isArray(isArray), _arraySize(arraySize), 
    _referenceClassName(referenceClassName)
{
    if (!Name::legal(name))
	throw IllegalName();

    if (_type == Type::NONE)
	throw NullType();

    if (_arraySize && !_isArray)
	throw IncompatibleTypes();

    if (referenceClassName.getLength())
    {
	if (!Name::legal(referenceClassName))
	    throw IllegalName();

	if (_type != Type::REFERENCE)
	{
	    throw ExpectedReferenceValue();
	}
    }
    else
    {

    // ATTN: revisit this later!
#if 0
	if (_type == Type::REFERENCE)
	    throw MissingReferenceClassName();
#endif
    }
}

ParameterRep::~ParameterRep()
{

}

void ParameterRep::setName(const String& name) 
{
    if (!Name::legal(name))
	throw IllegalName();

    _name = name; 
}

void ParameterRep::resolve(
    DeclContext* declContext,
    const String& nameSpace)
{
    // Validate the qualifiers of the method (according to
    // superClass's method with the same name). This method
    // will throw an exception if the validation fails.

    QualifierList dummy;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	Scope::PROPERTY,
	false,
	dummy);
}

void ParameterRep::toXml(Array<Sint8>& out) const
{
    if (_isArray)
    {
	out << "<PARAMETER.ARRAY";

	out << " NAME=\"" << _name << "\" ";

	out << " TYPE=\"" << TypeToString(_type) << "\"";

	if (_arraySize)
	{
	    char buffer[32];
	    sprintf(buffer, "%d", _arraySize);
	    out << " ARRAYSIZE=\"" << buffer << "\"";
	}

	out << ">\n";

	_qualifiers.toXml(out);

	out << "</PARAMETER.ARRAY>\n";
    }
    else
    {
	out << "<PARAMETER";

	out << " NAME=\"" << _name << "\" ";

	out << " TYPE=\"" << TypeToString(_type) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);

	out << "</PARAMETER>\n";
    }
}

void ParameterRep::print() const 
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

Boolean ParameterRep::identical(const ParameterRep* x) const
{
    if (_name != x->_name)
	return false;

    if (_type != x->_type)
	return false;

    if (_referenceClassName != x->_referenceClassName)
	return false;

    if (!_qualifiers.identical(x->_qualifiers))
	return false;

    return true;
}

ParameterRep::ParameterRep()
{

}

ParameterRep::ParameterRep(const ParameterRep& x) :
    Sharable(),
    _name(x._name),
    _type(x._type),
    _isArray(x._isArray),
    _arraySize(x._arraySize),
    _referenceClassName(x._referenceClassName)
{
    x._qualifiers.cloneTo(_qualifiers);
}

ParameterRep& ParameterRep::operator=(const ParameterRep& x) 
{ 
    return *this; 
}

void ParameterRep::setType(Type type)
{ 
    _type = type;

    if (_referenceClassName.getLength() == 0 && _type == Type::REFERENCE)
    {
	throw MissingReferenceClassName();
    }
}

PEGASUS_NAMESPACE_END
