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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "CIMParamValue.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMScope.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

CIMParamValueRep::CIMParamValueRep(
    CIMParameter parameter,
    CIMValue value,
    Boolean isArray,
    Uint32 arraySize,
    const String& referenceClassName) 
    : _parameter(parameter), _value(value),
    _isArray(isArray), _arraySize(arraySize), 
    _referenceClassName(referenceClassName)
{
    if (_arraySize && !_isArray)
	throw IncompatibleTypes();

    if (referenceClassName.size())
    {
	if (!CIMName::legal(referenceClassName))
	    throw IllegalName();
    }
}

CIMParamValueRep::~CIMParamValueRep()
{

}

void CIMParamValueRep::resolve(
    DeclContext* declContext,
    const String& nameSpace)
{
    // Validate the qualifiers of the method (according to
    // superClass's method with the same name). This method
    // will throw an exception if the validation fails.

    CIMQualifierList dummy;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	CIMScope::PARAMETER,
	false,
	dummy);
}

void CIMParamValueRep::toXml(Array<Sint8>& out) const
{
    if (_isArray)
    {
	out << " ARGUMENT.ARRAY";

	out << " PARAMETER_NAME=\"" << _parameter.getName() << "\"";

	out << " PARAMETER_TYPE=\"" << TypeToString(_parameter.getType()) << "\"";

	out << " VALUE=\"" << _value.toString() << "\"";

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
	out << " ARGUMENT";
	out << " PARAMETER.NAME=\"" << _parameter.getName() << "\"";
	out << " PARAMETER.TYPE=\"" << TypeToString(_parameter.getType()) << "\"";
	out << " PARAMETER.VALUE=\"" << _value.toString() << "\"";
	out << ">\n";
	_qualifiers.toXml(out);
	out << "</ARGUMENT>\n";
    }
}

/** toMof - puts the Mof representation of the ParamValue object to
    the output parameter array
    The BNF for this conversion is:
    parameterList    = 	parameter *( "," parameter )

	parameter    = 	[ qualifierList ] (dataType|objectRef) parameterName
				[ array ]

	parameterName= 	IDENTIFIER
	
	array 	     = 	"[" [positiveDecimalValue] "]"
	
    Format on a single line.
    */
void CIMParamValueRep::toMof(Array<Sint8>& out) const
{
    // Output the qualifiers for the parameter
    _qualifiers.toMof(out);

    if (_qualifiers.getCount())
	out << " ";

    // Output the data parameter and value
    out << _parameter.getName() 
	<< " " << TypeToString(_parameter.getType())
	<< " " << _value.toString();

    if (_isArray)
    {
	//Output the array indicator "[ [arraysize] ]"
	if (_arraySize)
	{
	    char buffer[32];
	    sprintf(buffer, "[%d]", _arraySize);
	    out << buffer;
	}
	else
	    out << "[]";
    }
}

void CIMParamValueRep::print(PEGASUS_STD(ostream) &os) const 
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

Boolean CIMParamValueRep::identical(const CIMParamValueRep* x) const
{
    if (_parameter != x->_parameter)
	return false;

    if (_value != x->_value)
	return false;

    if (_referenceClassName != x->_referenceClassName)
	return false;

    if (!_qualifiers.identical(x->_qualifiers))
	return false;

    return true;
}

CIMParamValueRep::CIMParamValueRep()
{

}

CIMParamValueRep::CIMParamValueRep(const CIMParamValueRep& x) :
    Sharable(),
    _parameter(x._parameter),
    _value(x._value),
    _isArray(x._isArray),
    _arraySize(x._arraySize),
    _referenceClassName(x._referenceClassName)
{
    x._qualifiers.cloneTo(_qualifiers);
}

CIMParamValueRep& CIMParamValueRep::operator=(const CIMParamValueRep& x) 
{ 
    return *this; 
}

void CIMParamValueRep::setParameter(CIMParameter parameter)
{ 
    _parameter = parameter;

    if (_referenceClassName.size() == 0)
    {
	throw MissingReferenceClassName();
    }
}

void CIMParamValueRep::setValue(CIMValue value)
{ 
    _value = value;

    if (_referenceClassName.size() == 0)
    {
	throw MissingReferenceClassName();
    }
}

PEGASUS_NAMESPACE_END
