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

#ifndef Pegasus_ParamValue_h
#define Pegasus_ParamValue_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMParamValueRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMParamValue
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstParamValue;

class PEGASUS_COMMON_LINKAGE CIMParamValue
{
public:

    CIMParamValue() : _rep(0)
    {

    }

    CIMParamValue(const CIMParamValue& x)
    {
	Inc(_rep = x._rep);
    }

    CIMParamValue& operator=(const CIMParamValue& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    CIMParamValue(
	CIMParameter parameter,
	CIMValue value,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY)
    {
	_rep = new CIMParamValueRep(
	    parameter, value, isArray, arraySize, referenceClassName);
    }

    ~CIMParamValue()
    {
	Dec(_rep);
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

    CIMParameter getParameter() const 
    { 
	_checkRep();
	return _rep->getParameter();
    }

    CIMValue getValue() const 
    { 
	_checkRep();
	return _rep->getValue();
    }

    // Throws AlreadyExists.

    CIMParamValue& addQualifier(const CIMQualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
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

    Boolean identical(const CIMConstParamValue& x) const;

    CIMParamValue clone() const
    {
	return CIMParamValue(_rep->clone());
    }

private:

    CIMParamValue(CIMParamValueRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMParamValueRep* _rep;
    friend class CIMConstParamValue;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstParamValue
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstParamValue
{
public:

    CIMConstParamValue() : _rep(0)
    {

    }

    CIMConstParamValue(const CIMConstParamValue& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstParamValue(const CIMParamValue& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstParamValue& operator=(const CIMConstParamValue& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    CIMConstParamValue& operator=(const CIMParamValue& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    CIMConstParamValue(
	CIMParameter parameter,
	CIMValue value,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY)
    {
	_rep = new CIMParamValueRep(
	    parameter, value, isArray, arraySize, referenceClassName);
    }

    ~CIMConstParamValue()
    {
	Dec(_rep);
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

    CIMParameter getParameter() const 
    { 
	_checkRep();
	return _rep->getParameter();
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

    Boolean identical(const CIMConstParamValue& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    CIMParamValue clone() const
    {
	return CIMParamValue(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMParamValueRep* _rep;
    friend class CIMParamValue;
};

#define PEGASUS_ARRAY_T CIMParamValue
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ParamValue_h */
