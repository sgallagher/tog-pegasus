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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
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
	String parameterName,
	CIMValue value,
	Boolean isTyped=true)
    {
	_rep = new CIMParamValueRep(parameterName, value, isTyped);
    }

    ~CIMParamValue()
    {
	Dec(_rep);
    }

    String getParameterName() const 
    { 
	_checkRep();
	return _rep->getParameterName();
    }

    CIMValue getValue() const 
    { 
	_checkRep();
	return _rep->getValue();
    }

    Boolean isTyped() const 
    { 
	_checkRep();
	return _rep->isTyped();
    }

    void setParameterName(String& parameterName)
    { 
	_checkRep();
	_rep->setParameterName(parameterName);
    }

    void setValue(CIMValue& value)
    { 
	_checkRep();
	_rep->setValue(value);
    }

    void setIsTyped(Boolean isTyped=true)
    { 
	_checkRep();
	_rep->setIsTyped(isTyped);
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
	String parameterName,
	CIMValue value,
	Boolean isTyped=true)
    {
	_rep = new CIMParamValueRep(parameterName, value, isTyped);
    }

    ~CIMConstParamValue()
    {
	Dec(_rep);
    }

    String getParameterName() const 
    { 
	_checkRep();
	return _rep->getParameterName();
    }

    CIMValue getValue() const 
    { 
	_checkRep();
	return _rep->getValue();
    }

    Boolean isTyped() const 
    { 
	_checkRep();
	return _rep->isTyped();
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
