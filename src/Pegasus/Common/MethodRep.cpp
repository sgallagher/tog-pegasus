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
// $Log: MethodRep.cpp,v $
// Revision 1.1  2001/01/14 19:52:58  mike
// Initial revision
//
//
//END_HISTORY

#include <cassert>
#include "Method.h"
#include "Indentor.h"
#include "Name.h"
#include "Scope.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

MethodRep::MethodRep(
    const String& name, 
    Type type,
    const String& classOrigin,
    Boolean propagated) 
    : _name(name), _type(type), 
    _classOrigin(classOrigin), _propagated(propagated)
{
    if (!Name::legal(name))
	throw IllegalName();

    if (classOrigin.getLength() && !Name::legal(classOrigin))
	throw IllegalName();

    if (type == Type::NONE)
	throw NullType();
}

MethodRep::~MethodRep()
{

}

void MethodRep::setName(const String& name) 
{
    if (!Name::legal(name))
	throw IllegalName();

    _name = name; 
}

void MethodRep::setClassOrigin(const String& classOrigin)
{
    if (!Name::legal(classOrigin))
	throw IllegalName();

    _classOrigin = classOrigin; 
}

void MethodRep::addParameter(const Parameter& x)
{
    if (!x)
	throw UnitializedHandle();

    if (findParameter(x.getName()) != Uint32(-1))
	throw AlreadyExists();

    _parameters.append(x);
}

Uint32 MethodRep::findParameter(const String& name)
{
    for (Uint32 i = 0, n = _parameters.getSize(); i < n; i++)
    {
	if (Name::equal(_parameters[i].getName(), name))
	    return i;
    }

    return Uint32(-1);
}

Parameter MethodRep::getParameter(Uint32 pos)
{
    if (pos >= _parameters.getSize())
	throw OutOfBounds();

    return _parameters[pos];
}

Uint32 MethodRep::getParameterCount() const
{
    return _parameters.getSize();
}

void MethodRep::resolve(
    DeclContext* declContext, 
    const String& nameSpace,
    const ConstMethod& inheritedMethod)
{
    assert(Name::equal(getName(), inheritedMethod.getName()));

    if (getType() != inheritedMethod.getType())
    {
	String tmp = inheritedMethod.getName();
	tmp.append("; attempt to change type");
	throw InvalidMethodOverride(tmp);
    }

    // Validate the qualifiers of the method (according to
    // superClass's method with the same name). This method
    // will throw an exception if the validation fails.

    _qualifiers.resolve(
	declContext,
	nameSpace,
	Scope::METHOD,
	false,
	inheritedMethod._rep->_qualifiers);

    // Validate each of the parameters:

    for (size_t i = 0; i < _parameters.getSize(); i++)
	_parameters[i].resolve(declContext, nameSpace);
}

void MethodRep::resolve(
    DeclContext* declContext,
    const String& nameSpace)
{
    // Validate the qualifiers:

    QualifierList dummy;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	Scope::METHOD,
	false,
	dummy);

    // Validate each of the parameters:

    for (size_t i = 0; i < _parameters.getSize(); i++)
	_parameters[i].resolve(declContext, nameSpace);
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void MethodRep::toXml(Array<Sint8>& out) const
{
    out << "<METHOD";

    out << " NAME=\"" << _name << "\"";

    out << " TYPE=\"" << TypeToString(_type) << "\"";

    if (_classOrigin.getLength())
	out << " CLASSORIGIN=\"" << _classOrigin << "\"";

    if (_propagated != false)
	out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

    out << ">\n";

    _qualifiers.toXml(out);

    for (Uint32 i = 0, n = _parameters.getSize(); i < n; i++)
	_parameters[i].toXml(out);

    out << "</METHOD>\n";
}

void MethodRep::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

MethodRep::MethodRep()
{

}

MethodRep::MethodRep(const MethodRep& x) : 
    Sharable(),
    _name(x._name),
    _type(x._type),
    _classOrigin(x._classOrigin),
    _propagated(x._propagated)
{
    x._qualifiers.cloneTo(_qualifiers);

    _parameters.reserve(x._parameters.getSize());

    for (Uint32 i = 0, n = x._parameters.getSize(); i < n; i++)
	_parameters.append(x._parameters[i].clone());
}

MethodRep& MethodRep::operator=(const MethodRep& x) 
{ 
    return *this; 
}

Boolean MethodRep::identical(const MethodRep* x) const
{
    if (_name != x->_name)
	return false;

    if (_type != x->_type)
	return false;

    if (!_qualifiers.identical(x->_qualifiers))
	return false;

    if (_parameters.getSize() != x->_parameters.getSize())
	return false;

    for (Uint32 i = 0, n = _parameters.getSize(); i < n; i++)
    {
	if (!_parameters[i].identical(x->_parameters[i]))
	    return false;
    }

    return true;
}

PEGASUS_NAMESPACE_END
