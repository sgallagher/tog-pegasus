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

#ifndef Pegasus_Parameter_h
#define Pegasus_Parameter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMParameterRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMParameter
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstParameter;

class PEGASUS_COMMON_LINKAGE CIMParameter
{
public:

    CIMParameter() : _rep(0)
    {

    }

    CIMParameter(const CIMParameter& x)
    {
	Inc(_rep = x._rep);
    }

    CIMParameter& operator=(const CIMParameter& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    CIMParameter(
	const String& name, 
	CIMType type,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY)
    {
	_rep = new CIMParameterRep(
	    name, type, isArray, arraySize, referenceClassName);
    }

    ~CIMParameter()
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

    CIMType getType() const 
    { 
	_checkRep();
	return _rep->getType();
    }

    void setType(CIMType type);

    // Throws AlreadyExists.

    CIMParameter& addQualifier(const CIMQualifier& x)
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

    CIMQualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
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

    void toMof(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toMof(out);
    }


    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    Boolean identical(const CIMConstParameter& x) const;

    CIMParameter clone() const
    {
	return CIMParameter(_rep->clone());
    }

private:

    CIMParameter(CIMParameterRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMParameterRep* _rep;
    friend class CIMConstParameter;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstParameter
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstParameter
{
public:

    CIMConstParameter() : _rep(0)
    {

    }

    CIMConstParameter(const CIMConstParameter& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstParameter(const CIMParameter& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstParameter& operator=(const CIMConstParameter& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    CIMConstParameter& operator=(const CIMParameter& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    CIMConstParameter(
	const String& name, 
	CIMType type,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY)
    {
	_rep = new CIMParameterRep(
	    name, type, isArray, arraySize, referenceClassName);
    }

    ~CIMConstParameter()
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

    CIMType getType() const 
    { 
	_checkRep();
	return _rep->getType();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
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

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    Boolean identical(const CIMConstParameter& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    CIMParameter clone() const
    {
	return CIMParameter(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMParameterRep* _rep;
    friend class CIMParameter;
};

#define PEGASUS_ARRAY_T CIMParameter
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Parameter_h */
