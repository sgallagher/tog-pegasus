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
// $Log: CIMPropertyRep.cpp,v $
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 03:56:01  mike
// Changed more class names (e.g., ConstClassDecl -> ConstCIMClass)
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.4  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.3  2001/01/23 01:25:35  mike
// Reworked resolve scheme.
//
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
#include "CIMProperty.h"
#include "XmlWriter.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMScope.h"

PEGASUS_NAMESPACE_BEGIN

CIMPropertyRep::CIMPropertyRep(
    const String& name, 
    const CIMValue& value,
    Uint32 arraySize,
    const String& referenceClassName,
    const String& classOrigin,
    Boolean propagated) :
    _name(name), _value(value), _arraySize(arraySize),
    _referenceClassName(referenceClassName), _classOrigin(classOrigin), 
    _propagated(propagated)
{
    if (!CIMName::legal(name))
	throw IllegalName();

    if (arraySize && (!value.isArray() || value.getArraySize() != arraySize))
	throw IncompatibleTypes();

    if (classOrigin.getLength() && !CIMName::legal(classOrigin))
	throw IllegalName();

    if (_value.getType() == CIMType::NONE)
	throw NullType();

    if (referenceClassName.getLength())
    {
	if (!CIMName::legal(referenceClassName))
	    throw IllegalName();

	if (_value.getType() != CIMType::REFERENCE)
	{
	    throw ExpectedReferenceValue();
	}
    }
    else
    {
	if (_value.getType() == CIMType::REFERENCE)
	{
	    throw MissingReferenceClassName();
	}
    }
}

CIMPropertyRep::~CIMPropertyRep()
{

}

void CIMPropertyRep::setName(const String& name) 
{
    if (!CIMName::legal(name))
	throw IllegalName();

    _name = name; 
}

void CIMPropertyRep::setClassOrigin(const String& classOrigin)
{
    if (!CIMName::legal(classOrigin))
	throw IllegalName();

    _classOrigin = classOrigin; 
}

void CIMPropertyRep::resolve(
    DeclContext* declContext, 
    const String& nameSpace,
    Boolean isInstancePart,
    const CIMConstProperty& inheritedProperty)
{
    assert (inheritedProperty);

    // Check the type:

    if (!inheritedProperty.getValue().typeCompatible(_value))
	throw TypeMismatch();

    // Validate the qualifiers of the property (according to
    // superClass's property with the same name). This method
    // will throw an exception if the validation fails.

    Uint32 scope = CIMScope::PROPERTY;

    if (_value.getType() == CIMType::REFERENCE)
	scope = CIMScope::REFERENCE;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	scope,
	isInstancePart,
	inheritedProperty._rep->_qualifiers);

    _classOrigin = inheritedProperty.getClassOrigin();
}

void CIMPropertyRep::resolve(
    DeclContext* declContext, 
    const String& nameSpace,
    Boolean isInstancePart)
{
    CIMQualifierList dummy;

    Uint32 scope = CIMScope::PROPERTY;

    if (_value.getType() == CIMType::REFERENCE)
	scope = CIMScope::REFERENCE;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	scope,
	isInstancePart,
	dummy);
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void CIMPropertyRep::toXml(Array<Sint8>& out) const
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
    else if (_value.getType() == CIMType::REFERENCE)
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

void CIMPropertyRep::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

Boolean CIMPropertyRep::identical(const CIMPropertyRep* x) const
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

CIMPropertyRep::CIMPropertyRep()
{

}

CIMPropertyRep::CIMPropertyRep(const CIMPropertyRep& x) : 
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

CIMPropertyRep& CIMPropertyRep::operator=(const CIMPropertyRep& x) 
{ 
    return *this; 
}

void CIMPropertyRep::setValue(const CIMValue& value)
{
    // CIMType of value is immutable:

    if (!value.typeCompatible(_value))
	throw IncompatibleTypes();

    if (_arraySize && _arraySize != value.getArraySize())
	throw IncompatibleTypes();

    _value = value;
}

PEGASUS_NAMESPACE_END
