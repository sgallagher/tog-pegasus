//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "CIMPropertyRep.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMScope.h"
#include "XmlWriter.h"
#include "MofWriter.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMPropertyRep::CIMPropertyRep(
    const CIMName& name,
    const CIMValue& value,
    Uint32 arraySize,
    const CIMName& referenceClassName,
    const CIMName& classOrigin,
    Boolean propagated) 
    :
    _name(name), _value(value), _arraySize(arraySize),
    _referenceClassName(referenceClassName), _classOrigin(classOrigin),
    _propagated(propagated)
{
    if (arraySize && (!value.isArray() || value.getArraySize() != arraySize))
		throw IncompatibleTypesException();

    // If referenceClassName exists, must be legal namd and CIMType REFERENCE.
    if (!referenceClassName.isNull())
    {
        if (_value.getType() != CIMTYPE_REFERENCE)
            throw ExpectedReferenceValueException();
    }
    else
    {
        if (_value.getType() == CIMTYPE_REFERENCE)
            throw MissingReferenceClassNameException();
    }
}

CIMPropertyRep::~CIMPropertyRep()
{

}

void CIMPropertyRep::setName(const CIMName& name)
{
    _name = name;
}

void CIMPropertyRep::setClassOrigin(const CIMName& classOrigin)
{
    _classOrigin = classOrigin;
}

void CIMPropertyRep::resolve(
    DeclContext* declContext,
    const CIMNamespaceName& nameSpace,
    Boolean isInstancePart,
    const CIMConstProperty& inheritedProperty,
    Boolean propagateQualifiers)
{
    PEGASUS_ASSERT(!inheritedProperty.isUninitialized());

    // Check the type:

    if (!inheritedProperty.getValue().typeCompatible(_value))
	throw TypeMismatchException();

    // Validate the qualifiers of the property (according to
    // superClass's property with the same name). This method
    // will throw an exception if the validation fails.

    CIMScope scope = CIMScope::PROPERTY;

    if (_value.getType() == CIMTYPE_REFERENCE)
	scope = CIMScope::REFERENCE;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	scope,
	isInstancePart,
	inheritedProperty._rep->_qualifiers, 
	propagateQualifiers);

    _classOrigin = inheritedProperty.getClassOrigin();
}

void CIMPropertyRep::resolve(
    DeclContext* declContext,
    const CIMNamespaceName& nameSpace,
    Boolean isInstancePart,
    Boolean propagateQualifiers)
{
    CIMQualifierList dummy;

    CIMScope scope = CIMScope::PROPERTY;

    if (_value.getType() == CIMTYPE_REFERENCE)
	scope = CIMScope::REFERENCE;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	scope,
	isInstancePart,
	dummy,
	propagateQualifiers);
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

	out << " TYPE=\"" << cimTypeToString (_value.getType ()) << "\"";

	if (_arraySize)
	{
	    char buffer[32];
	    sprintf(buffer, "%d", _arraySize);
	    out << " ARRAYSIZE=\"" << buffer << "\"";
	}

	if (!_classOrigin.isNull())
	    out << " CLASSORIGIN=\"" << _classOrigin << "\"";

	if (_propagated != false)
	    out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);

	XmlWriter::appendValueElement(out, _value);

	out << "</PROPERTY.ARRAY>\n";
    }
    else if (_value.getType() == CIMTYPE_REFERENCE)
    {
	out << "<PROPERTY.REFERENCE";

	out << " NAME=\"" << _name << "\" ";

	out << " REFERENCECLASS=\"" << _referenceClassName << "\"";

	if (!_classOrigin.isNull())
	    out << " CLASSORIGIN=\"" << _classOrigin << "\"";

	if (_propagated != false)
	    out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);
        
	XmlWriter::appendValueElement(out, _value);

	out << "</PROPERTY.REFERENCE>\n";
    }
    else
    {
	out << "<PROPERTY";
	out << " NAME=\"" << _name << "\" ";

	if (!_classOrigin.isNull())
	    out << " CLASSORIGIN=\"" << _classOrigin << "\"";

	if (_propagated != false)
	    out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

	out << " TYPE=\"" << cimTypeToString (_value.getType ()) << "\"";

	out << ">\n";

	_qualifiers.toXml(out);
        
	XmlWriter::appendValueElement(out, _value);

	out << "</PROPERTY>\n";
    }
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
    out << "\n" << cimTypeToString (_value.getType ()) << " " << _name;

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
	    MofWriter::appendValueElement(out, _value);
	}
	else if (_value.getType() == CIMTYPE_REFERENCE)
	{
	    MofWriter::appendValueElement(out, _value);
	}
	else
	{ 
	    MofWriter::appendValueElement(out, _value);
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

CIMPropertyRep::CIMPropertyRep()
{

}

CIMPropertyRep::CIMPropertyRep(
    const CIMPropertyRep& x, 
    Boolean propagateQualifiers) 
    :
    Sharable(),
    _name(x._name),
    _value(x._value),
    _arraySize(x._arraySize),
    _referenceClassName(x._referenceClassName),
    _classOrigin(x._classOrigin),
    _propagated(x._propagated)
{
    if (propagateQualifiers)
	x._qualifiers.cloneTo(_qualifiers);
}

void CIMPropertyRep::setValue(const CIMValue& value)
{
    // CIMType of value is immutable:

    if (!value.typeCompatible(_value))
	throw IncompatibleTypesException();

    if (_arraySize && _arraySize != value.getArraySize())
	throw IncompatibleTypesException();

    _value = value;
}

PEGASUS_NAMESPACE_END
