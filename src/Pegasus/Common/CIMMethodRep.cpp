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
#include "CIMMethod.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMScope.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

CIMMethodRep::CIMMethodRep(
    const String& name,
    CIMType type,
    const String& classOrigin,
    Boolean propagated)
    : _name(name), _type(type),
    _classOrigin(classOrigin), _propagated(propagated)
{
    if (!CIMName::legal(name))
	throw IllegalName();

    if (classOrigin.size() && !CIMName::legal(classOrigin))
	throw IllegalName();

    if (type == CIMType::NONE)
	throw NullType();
}

CIMMethodRep::~CIMMethodRep()
{

}

void CIMMethodRep::setName(const String& name)
{
    if (!CIMName::legal(name))
	throw IllegalName();

    _name = name;
}

void CIMMethodRep::setClassOrigin(const String& classOrigin)
{
    if (!CIMName::legal(classOrigin))
	throw IllegalName();

    _classOrigin = classOrigin;
}

void CIMMethodRep::addParameter(const CIMParameter& x)
{
    if (!x)
	throw UnitializedHandle();

    if (findParameter(x.getName()) != PEG_NOT_FOUND)
	throw AlreadyExists();

    _parameters.append(x);
}

Uint32 CIMMethodRep::findParameter(const String& name)
{
    for (Uint32 i = 0, n = _parameters.size(); i < n; i++)
    {
	if (CIMName::equal(_parameters[i].getName(), name))
	    return i;
    }

    return PEG_NOT_FOUND;
}

CIMParameter CIMMethodRep::getParameter(Uint32 pos)
{
    if (pos >= _parameters.size())
	throw OutOfBounds();

    return _parameters[pos];
}

Uint32 CIMMethodRep::getParameterCount() const
{
    return _parameters.size();
}

void CIMMethodRep::resolve(
    DeclContext* declContext,
    const String& nameSpace,
    const CIMConstMethod& inheritedMethod)
{
    // ATTN: Check to see if this method has same signature as
    // inherited one.

    // Check for type mismatch between return types.

    assert (inheritedMethod);

    // Validate the qualifiers of the method (according to
    // superClass's method with the same name). This method
    // will throw an exception if the validation fails.

    _qualifiers.resolve(
	declContext,
	nameSpace,
	CIMScope::METHOD,
	false,
	inheritedMethod._rep->_qualifiers);

    // Validate each of the parameters:

    for (size_t i = 0; i < _parameters.size(); i++)
	_parameters[i].resolve(declContext, nameSpace);

    _classOrigin = inheritedMethod.getClassOrigin();
}

void CIMMethodRep::resolve(
    DeclContext* declContext,
    const String& nameSpace)
{
    // Validate the qualifiers:

    CIMQualifierList dummy;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	CIMScope::METHOD,
	false,
	dummy);

    // Validate each of the parameters:

    for (size_t i = 0; i < _parameters.size(); i++)
	_parameters[i].resolve(declContext, nameSpace);
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void CIMMethodRep::toXml(Array<Sint8>& out) const
{
    out << "<METHOD";

    out << " NAME=\"" << _name << "\"";

    out << " TYPE=\"" << TypeToString(_type) << "\"";

    if (_classOrigin.size())
	out << " CLASSORIGIN=\"" << _classOrigin << "\"";

    if (_propagated != false)
	out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

    out << ">\n";

    _qualifiers.toXml(out);

    for (Uint32 i = 0, n = _parameters.size(); i < n; i++)
	_parameters[i].toXml(out);

    out << "</METHOD>\n";
}

void CIMMethodRep::print(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

/**
    The BNF for this is;
    methodDeclaration 	=  [ qualifierList ] dataType methodName
			   "(" [ parameterList ] ")" ";"

    parameterList 	=  parameter *( "," parameter )
    Format with qualifiers on one line and declaration on another. Start
    with newline but none at the end.
*/
void CIMMethodRep::toMof(Array<Sint8>& out) const   //ATTNKS:
{
    // Output the qualifier list starting on new line
    if (_qualifiers.getCount())
	out << "\n";

    _qualifiers.toMof(out);

    // output the type,	MethodName and ParmeterList left enclosure
    out << "\n" << TypeToString(_type) << " " << _name << "(";

    // output the param list separated by commas.
    
    for (Uint32 i = 0, n = _parameters.size(); i < n; i++)
    {
	// If not first, output comma separator
	if (i)
	    out << ", ";

	_parameters[i].toMof(out);
    }

    // output the parameterlist and method terminator
    out << ");";
}


CIMMethodRep::CIMMethodRep()
{

}

CIMMethodRep::CIMMethodRep(const CIMMethodRep& x) :
    Sharable(),
    _name(x._name),
    _type(x._type),
    _classOrigin(x._classOrigin),
    _propagated(x._propagated)
{
    x._qualifiers.cloneTo(_qualifiers);

    _parameters.reserve(x._parameters.size());

    for (Uint32 i = 0, n = x._parameters.size(); i < n; i++)
	_parameters.append(x._parameters[i].clone());
}

CIMMethodRep& CIMMethodRep::operator=(const CIMMethodRep& x)
{
    return *this;
}

Boolean CIMMethodRep::identical(const CIMMethodRep* x) const
{
    if (_name != x->_name)
	return false;

    if (_type != x->_type)
	return false;

    if (!_qualifiers.identical(x->_qualifiers))
	return false;

    if (_parameters.size() != x->_parameters.size())
	return false;

    for (Uint32 i = 0, n = _parameters.size(); i < n; i++)
    {
	if (!_parameters[i].identical(x->_parameters[i]))
	    return false;
    }

    return true;
}

void CIMMethodRep::setType(CIMType type)
{
    _type = type;

    if (type == CIMType::NONE)
	throw NullType();
}

PEGASUS_NAMESPACE_END
