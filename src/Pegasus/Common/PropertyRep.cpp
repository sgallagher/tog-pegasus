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
// $Log: PropertyRep.cpp,v $
// Revision 1.2  2001/01/22 00:45:47  mike
// more work on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:53:05  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <cstdio>
#include "Property.h"
#include "XmlWriter.h"
#include "Indentor.h"
#include "Name.h"
#include "Scope.h"

PEGASUS_NAMESPACE_BEGIN

PropertyRep::PropertyRep(
    const String& name, 
    const Value& value,
    Uint32 arraySize,
    const String& referenceClassName,
    const String& classOrigin,
    Boolean propagated) :
    _name(name), _value(value), _arraySize(arraySize),
    _referenceClassName(referenceClassName), _classOrigin(classOrigin), 
    _propagated(propagated)
{
    if (!Name::legal(name))
	throw IllegalName();

    if (arraySize && (!value.isArray() || value.getArraySize() != arraySize))
	throw IncompatibleTypes();

    if (classOrigin.getLength() && !Name::legal(classOrigin))
	throw IllegalName();

    if (_value.getType() == Type::NONE)
	throw NullType();

    if (referenceClassName.getLength())
    {
	if (!Name::legal(referenceClassName))
	    throw IllegalName();

	if (_value.getType() != Type::REFERENCE)
	{
	    throw ExpectedReferenceValue();
	}
    }
    else
    {
	if (_value.getType() == Type::REFERENCE)
	{
	    throw MissingReferenceClassName();
	}
    }
}

PropertyRep::~PropertyRep()
{

}

void PropertyRep::setName(const String& name) 
{
    if (!Name::legal(name))
	throw IllegalName();

    _name = name; 
}

void PropertyRep::setClassOrigin(const String& classOrigin)
{
    if (!Name::legal(classOrigin))
	throw IllegalName();

    _classOrigin = classOrigin; 
}

void PropertyRep::resolve(
    DeclContext* declContext, 
    const String& nameSpace,
    Boolean isInstancePart,
    const ConstProperty& inheritedProperty)
{
    if (inheritedProperty)
    {
	assert(Name::equal(getName(), inheritedProperty.getName()));

	if (getValue().getType() != inheritedProperty.getValue().getType())
	{
	    String tmp = inheritedProperty.getName();
	    tmp.append("; attempt to change type");
	    throw InvalidPropertyOverride(tmp);
	}

	_classOrigin = inheritedProperty.getClassOrigin();
    }

    // Validate the qualifiers of the property (according to
    // superClass's property with the same name). This method
    // will throw an exception if the validation fails.

    if (inheritedProperty)
    {
	_qualifiers.resolve(
	    declContext,
	    nameSpace,
	    Scope::PROPERTY,
	    isInstancePart,
	    inheritedProperty._rep->_qualifiers);
    }
    else
    {
	QualifierList dummyQualifiers;
	_qualifiers.resolve(
	    declContext,
	    nameSpace,
	    Scope::PROPERTY,
	    isInstancePart,
	    dummyQualifiers);
    }
}

void PropertyRep::resolve(
    DeclContext* declContext, 
    const String& nameSpace,
    Boolean isInstancePart)
{
    QualifierList dummy;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	Scope::PROPERTY,
	isInstancePart,
	dummy);
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void PropertyRep::toXml(Array<Sint8>& out) const
{
    if (_value.isArray())
    {
	out << "<PROPERTY.ARRAY";

	out << " NAME=\"" << _name << "\" ";

	out << " TYPE=\"" << TypeToString(_value.getType()) << "\"";

	if (_arraySize)
	{
	    char buffer[32];
	    sprintf(buffer, "%d", _arraySize);
	    out << " ARRAYSIZE=\"" << buffer << "\"";
	}

	if (_classOrigin.getLength())
	    out << " CLASSORIGIN=\"" << _classOrigin << "\"";

	if (_propagated != false)
	    out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);

	_value.toXml(out);

	out << "</PROPERTY.ARRAY>\n";
    }
    else if (_value.getType() == Type::REFERENCE)
    {
	out << "<PROPERTY.REFERENCE";

	out << " NAME=\"" << _name << "\" ";

	out << " REFERENCECLASS=\"" << _referenceClassName << "\"";

	if (_classOrigin.getLength())
	    out << " CLASSORIGIN=\"" << _classOrigin << "\"";

	if (_propagated != false)
	    out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);

	_value.toXml(out);

	out << "</PROPERTY.REFERENCE>\n";
    }
    else
    {
	out << "<PROPERTY";
	out << " NAME=\"" << _name << "\" ";

	if (_classOrigin.getLength())
	    out << " CLASSORIGIN=\"" << _classOrigin << "\"";

	if (_propagated != false)
	    out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

	out << " TYPE=\"" << TypeToString(_value.getType()) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);

	_value.toXml(out);

	out << "</PROPERTY>\n";
    }
}

void PropertyRep::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

Boolean PropertyRep::identical(const PropertyRep* x) const
{
    if (_name != x->_name)
	return false;

    if (_value != x->_value)
	return false;

    if (_referenceClassName != x->_referenceClassName)
	return false;

    if (!_qualifiers.identical(x->_qualifiers))
	return false;

    if (_classOrigin != x->_classOrigin)
	return false;

    if (_propagated != x->_propagated)
	return false;

    return true;
}

PropertyRep::PropertyRep()
{

}

PropertyRep::PropertyRep(const PropertyRep& x) : 
    Sharable(),
    _name(x._name),
    _value(x._value),
    _arraySize(x._arraySize),
    _referenceClassName(x._referenceClassName),
    _classOrigin(x._classOrigin),
    _propagated(x._propagated)
{
    x._qualifiers.cloneTo(_qualifiers);
}

PropertyRep& PropertyRep::operator=(const PropertyRep& x) 
{ 
    return *this; 
}

void PropertyRep::setValue(const Value& value)
{
    // Type of value is immutable:

    if (!value.typeCompatible(_value))
	throw IncompatibleTypes();

    if (_arraySize && _arraySize != value.getArraySize())
	throw IncompatibleTypes();

    _value = value;
}

PEGASUS_NAMESPACE_END
