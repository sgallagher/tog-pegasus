//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMParamValueRep.h"
#include "CIMParamValue.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMParamValue
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T


CIMParamValue::CIMParamValue()
    : _rep(0)
{
}

CIMParamValue::CIMParamValue(const CIMParamValue& x)
{
    Inc(_rep = x._rep);
}

CIMParamValue::CIMParamValue(CIMParamValueRep* rep)
    : _rep(rep)
{
}

CIMParamValue& CIMParamValue::operator=(const CIMParamValue& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMParamValue::CIMParamValue(
    String parameterName,
    CIMValue value,
    Boolean isTyped)
{
    _rep = new CIMParamValueRep(parameterName, value, isTyped);
}

CIMParamValue::~CIMParamValue()
{
    Dec(_rep);
}

String CIMParamValue::getParameterName() const 
{ 
    _checkRep();
    return _rep->getParameterName();
}

CIMValue CIMParamValue::getValue() const 
{ 
    _checkRep();
    return _rep->getValue();
}

Boolean CIMParamValue::isTyped() const 
{ 
    _checkRep();
    return _rep->isTyped();
}

void CIMParamValue::setParameterName(String& parameterName)
{ 
    _checkRep();
    _rep->setParameterName(parameterName);
}

void CIMParamValue::setValue(CIMValue& value)
{ 
    _checkRep();
    _rep->setValue(value);
}

void CIMParamValue::setIsTyped(Boolean isTyped)
{ 
    _checkRep();
    _rep->setIsTyped(isTyped);
}

Boolean CIMParamValue::isUninitialized() const
{
    return (_rep == 0)? true : false;
}

CIMParamValue CIMParamValue::clone() const
{
    return CIMParamValue(_rep->clone());
}

void CIMParamValue::_checkRep() const
{
    if (!_rep)
        throw UninitializedObjectException();
}

PEGASUS_NAMESPACE_END
