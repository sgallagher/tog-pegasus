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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

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

    if (classOrigin.size() && !CIMName::legal(classOrigin))
	throw IllegalName();

    if (_value.getType() == CIMType::NONE)
	throw NullType();

    if (referenceClassName.size())
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

	if (_classOrigin.size())
	    out << " CLASSORIGIN=\"" << _classOrigin << "\"";

	if (_propagated != false)
	    out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);

        // Output XML but output nothing if Null (the false)
	_value.toXml(out, false);

	out << "</PROPERTY.ARRAY>\n";
    }
    else if (_value.getType() == CIMType::REFERENCE)
    {
	out << "<PROPERTY.REFERENCE";

	out << " NAME=\"" << _name << "\" ";

	out << " REFERENCECLASS=\"" << _referenceClassName << "\"";

	if (_classOrigin.size())
	    out << " CLASSORIGIN=\"" << _classOrigin << "\"";

	if (_propagated != false)
	    out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);
        
        // Output XML but output nothing if Null (the false)
	_value.toXml(out, false);

	out << "</PROPERTY.REFERENCE>\n";
    }
    else
    {
	out << "<PROPERTY";
	out << " NAME=\"" << _name << "\" ";

	if (_classOrigin.size())
	    out << " CLASSORIGIN=\"" << _classOrigin << "\"";

	if (_propagated != false)
	    out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

	out << " TYPE=\"" << TypeToString(_value.getType()) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);
        
        // Output XML but output nothing if Null (the false)
	_value.toXml(out, false);

	out << "</PROPERTY>\n";
    }
}

void CIMPropertyRep::print(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

/** toMof - returns the MOF for the CIM Property Object in the parameter.
    The BNF for the property MOF is:
    <pre>
    propertyDeclaration     = 	[ qualifierList ] dataType propertyName
				[ array ] [ defaultValue ] ";"
   
    array 		    = 	"[" [positiveDecimalValue] "]"
    
    defaultValue 	    = 	"=" initializer
    </pre>
    Format with qualifiers on one line and declaration on another. Start
    with newline but none at the end.
*/
void CIMPropertyRep::toMof(Array<Sint8>& out) const  //ATTNKS:
{
    //Output the qualifier list
    if (_qualifiers.getCount())
	out << "\n"; 
    _qualifiers.toMof(out);

    // Output the Type and name on a new line
    out << "\n" << TypeToString(_value.getType()) << " " << _name;

    // If array put the Array indicator "[]" and possible size after name.
    if (_value.isArray())
    {
	if (_arraySize)
	{
	    char buffer[32];
	    sprintf(buffer, "[%d]", _arraySize);
	    out << buffer;
	}
	else
	    out << "[]";
    }

    // If the property value is not Null, add value after "="
    if (!_value.isNull())
    {
	out << " = "; 
	if (_value.isArray())
	{ 
	    // Insert any property values
	    _value.toMof(out);
	}
	else if (_value.getType() == CIMType::REFERENCE)
	{
	    _value.toMof(out);
	}
	else
	{ 
	    _value.toMof(out);
	}
    }
    // Close the property MOF
    out << ";";

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

Boolean CIMPropertyRep::isKey() const
{
    Uint32 pos = _qualifiers.findReverse("key");

    if (pos == PEG_NOT_FOUND)
	return false;

    Boolean flag;
    _qualifiers.getQualifier(pos).getValue().get(flag);

    return flag;
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
