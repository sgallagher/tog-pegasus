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

#include <Pegasus/Common/Config.h>
#include <cstdio>
#include "CIMParameter.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMScope.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

CIMParameterRep::CIMParameterRep(
    const String& name, 
    CIMType type,
    Boolean isArray,
    Uint32 arraySize,
    const String& referenceClassName) 
    : _name(name), _type(type), 
    _isArray(isArray), _arraySize(arraySize), 
    _referenceClassName(referenceClassName)
{
    if (!CIMName::legal(name))
	throw IllegalName();

// ATTN-RK-P2-20020222: Had to remove this check to support InvokeMethod
// requests without the new PARAMTYPE attribute on the PARAMVALUEs.
// In this case, we can't determine the type during operating decoding,
// so we set it to NONE and then correct it during operation processing.
//    if (_type == CIMType::NONE)
//	throw NullType();

    if (_arraySize && !_isArray)
	throw IncompatibleTypes();

    if (referenceClassName.size())
    {
	if (!CIMName::legal(referenceClassName))
	    throw IllegalName();

	if (_type != CIMType::REFERENCE)
	{
	    throw ExpectedReferenceValue();
	}
    }
    else
    {

    // ATTN: revisit this later!
#if 0
	if (_type == CIMType::REFERENCE)
	    throw MissingReferenceClassName();
#endif
    }
}

CIMParameterRep::~CIMParameterRep()
{

}

void CIMParameterRep::setName(const String& name) 
{
    if (!CIMName::legal(name))
	throw IllegalName();

    _name = name; 
}

void CIMParameterRep::resolve(
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

void CIMParameterRep::toXml(Array<Sint8>& out) const
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
    else if (_type == CIMType::REFERENCE)
    {
	out << "<PARAMETER.REFERENCE";
	out << " NAME=\"" << _name << "\" ";
	out << " REFERENCECLASS=\"" << _referenceClassName << "\"";
	out << ">\n";

	_qualifiers.toXml(out);

	out << "</PARAMETER.REFERENCE>\n";
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

/** toMof - puts the Mof representation of teh Parameter object to
    the output parameter array
    The BNF for this conversion is:
    parameterList    = 	parameter *( "," parameter )

	parameter    = 	[ qualifierList ] (dataType|objectRef) parameterName
				[ array ]

	parameterName= 	IDENTIFIER
	
	array 	     = 	"[" [positiveDecimalValue] "]"
	
    Format on a single line.
    */
void CIMParameterRep::toMof(Array<Sint8>& out) const
{
    // Output the qualifiers for the parameter
    _qualifiers.toMof(out);

    if (_qualifiers.getCount())
	out << " ";

    // Output the data type and name
    out << TypeToString(_type) << " " <<  _name;

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


void CIMParameterRep::print(PEGASUS_STD(ostream) &os) const 
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

Boolean CIMParameterRep::identical(const CIMParameterRep* x) const
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

CIMParameterRep::CIMParameterRep()
{

}

CIMParameterRep::CIMParameterRep(const CIMParameterRep& x) :
    Sharable(),
    _name(x._name),
    _type(x._type),
    _isArray(x._isArray),
    _arraySize(x._arraySize),
    _referenceClassName(x._referenceClassName)
{
    x._qualifiers.cloneTo(_qualifiers);
}

CIMParameterRep& CIMParameterRep::operator=(const CIMParameterRep& x) 
{ 
    return *this; 
}

void CIMParameterRep::setType(CIMType type)
{ 
    _type = type;

    if (_referenceClassName.size() == 0 && _type == CIMType::REFERENCE)
    {
	throw MissingReferenceClassName();
    }
}

PEGASUS_NAMESPACE_END
