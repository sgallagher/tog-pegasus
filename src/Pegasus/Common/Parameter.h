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
// $Log: Parameter.h,v $
// Revision 1.1.1.1  2001/01/14 19:53:02  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_Parameter_h
#define Pegasus_Parameter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ParameterRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Parameter
//
////////////////////////////////////////////////////////////////////////////////

class ConstParameter;

class PEGASUS_COMMON_LINKAGE Parameter
{
public:

    Parameter() : _rep(0)
    {

    }

    Parameter(const Parameter& x)
    {
	Inc(_rep = x._rep);
    }

    Parameter& operator=(const Parameter& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    Parameter(
	const String& name, 
	Type type,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY)
    {
	_rep = new ParameterRep(
	    name, type, isArray, arraySize, referenceClassName);
    }

    ~Parameter()
    {
	Dec(_rep);
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    void setName(const String& name)
    {
	_checkRep();
	_rep->setName(name);
    }

    Boolean isArray() const
    {
	_checkRep();
	return _rep->isArray();
    }

    Uint32 getAraySize() const
    {
	_checkRep();
	return _rep->getAraySize();
    }

    const String& getReferenceClassName() const 
    {
	_checkRep();
	return _rep->getReferenceClassName(); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType();
    }

    void setType(Type type);

    // Throws AlreadyExists.

    Parameter& addQualifier(const Qualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
    }

    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    void resolve(DeclContext* declContext, const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstParameter& x) const;

    Parameter clone() const
    {
	return Parameter(_rep->clone());
    }

private:

    Parameter(ParameterRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    ParameterRep* _rep;
    friend class ConstParameter;
};

////////////////////////////////////////////////////////////////////////////////
//
// ConstParameter
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE ConstParameter
{
public:

    ConstParameter() : _rep(0)
    {

    }

    ConstParameter(const ConstParameter& x)
    {
	Inc(_rep = x._rep);
    }

    ConstParameter(const Parameter& x)
    {
	Inc(_rep = x._rep);
    }

    ConstParameter& operator=(const ConstParameter& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstParameter& operator=(const Parameter& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    ConstParameter(
	const String& name, 
	Type type,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY)
    {
	_rep = new ParameterRep(
	    name, type, isArray, arraySize, referenceClassName);
    }

    ~ConstParameter()
    {
	Dec(_rep);
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    Boolean isArray() const
    {
	_checkRep();
	return _rep->isArray();
    }

    Uint32 getAraySize() const
    {
	_checkRep();
	return _rep->getAraySize();
    }

    const String& getReferenceClassName() const 
    {
	_checkRep();
	return _rep->getReferenceClassName(); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstParameter& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    ConstParameter clone() const
    {
	return ConstParameter(_rep->clone());
    }

private:

    ConstParameter(ParameterRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    ParameterRep* _rep;
    friend class Parameter;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Parameter_h */
