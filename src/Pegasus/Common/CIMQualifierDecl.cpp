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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMQualifierDecl.h"
#include "CIMQualifierDeclRep.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMQualifierDecl
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifierDecl
//
////////////////////////////////////////////////////////////////////////////////

CIMQualifierDecl::CIMQualifierDecl()
    : _rep(0)
{
}

CIMQualifierDecl::CIMQualifierDecl(const CIMQualifierDecl& x)
{
    Inc(_rep = x._rep);
}

CIMQualifierDecl::CIMQualifierDecl(
    const String& name,
    const CIMValue& value,
    Uint32 scope,
    Uint32 flavor,
    Uint32 arraySize)
{
    _rep = new CIMQualifierDeclRep(name, value, scope, flavor, arraySize);
}

CIMQualifierDecl::CIMQualifierDecl(CIMQualifierDeclRep* rep)
    : _rep(rep)
{
}

CIMQualifierDecl::~CIMQualifierDecl()
{
    Dec(_rep);
}

CIMQualifierDecl& CIMQualifierDecl::operator=(const CIMQualifierDecl& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }

    return *this;
}

const String& CIMQualifierDecl::getName() const
{
    _checkRep();
    return _rep->getName();
}

void CIMQualifierDecl::setName(const String& name)
{
    _checkRep();
    _rep->setName(name);
}

CIMType CIMQualifierDecl::getType() const
{
    _checkRep();
    return _rep->getType();
}

Boolean CIMQualifierDecl::isArray() const
{
    _checkRep();
    return _rep->isArray();
}

const CIMValue& CIMQualifierDecl::getValue() const
{
    _checkRep();
    return _rep->getValue();
}

void CIMQualifierDecl::setValue(const CIMValue& value)
{
    _checkRep();
    _rep->setValue(value);
}

Uint32 CIMQualifierDecl::getScope() const
{
    _checkRep();
    return _rep->getScope();
}

Uint32 CIMQualifierDecl::getFlavor() const
{
    _checkRep();
    return _rep->getFlavor();
}

Boolean CIMQualifierDecl::isFlavor(Uint32 flavor) const
{
    return ((getFlavor() & flavor) !=0);
}

Uint32 CIMQualifierDecl::getArraySize() const
{
    _checkRep();
    return _rep->getArraySize();
}

Boolean CIMQualifierDecl::isNull() const
{
    return (_rep == 0)? true : false;
}

void CIMQualifierDecl::toMof(Array<Sint8>& out) const
{
    _checkRep();
    _rep->toMof(out);
}

Boolean CIMQualifierDecl::identical(const CIMConstQualifierDecl& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMQualifierDecl CIMQualifierDecl::clone() const
{
    return CIMQualifierDecl(_rep->clone());
}

void CIMQualifierDecl::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMConstQualifierDecl
//
////////////////////////////////////////////////////////////////////////////////

CIMConstQualifierDecl::CIMConstQualifierDecl()
    : _rep(0)
{
}

CIMConstQualifierDecl::CIMConstQualifierDecl(const CIMConstQualifierDecl& x)
{
    Inc(_rep = x._rep);
}

CIMConstQualifierDecl::CIMConstQualifierDecl(const CIMQualifierDecl& x)
{
    Inc(_rep = x._rep);
}

CIMConstQualifierDecl::CIMConstQualifierDecl(
    const String& name,
    const CIMValue& value,
    Uint32 scope,
    Uint32 flavor,
    Uint32 arraySize)
{
    _rep = new CIMQualifierDeclRep(name, value, scope, flavor, arraySize);
}

CIMConstQualifierDecl::~CIMConstQualifierDecl()
{
    Dec(_rep);
}

CIMConstQualifierDecl& CIMConstQualifierDecl::operator=(
    const CIMConstQualifierDecl& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }

    return *this;
}

CIMConstQualifierDecl& CIMConstQualifierDecl::operator=(
    const CIMQualifierDecl& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }

    return *this;
}

const String& CIMConstQualifierDecl::getName() const
{
    _checkRep();
    return _rep->getName();
}

CIMType CIMConstQualifierDecl::getType() const
{
    _checkRep();
    return _rep->getType();
}

Boolean CIMConstQualifierDecl::isArray() const
{
    _checkRep();
    return _rep->isArray();
}

const CIMValue& CIMConstQualifierDecl::getValue() const
{
    _checkRep();
    return _rep->getValue();
}

Uint32 CIMConstQualifierDecl::getScope() const
{
    _checkRep();
    return _rep->getScope();
}

const Uint32 CIMConstQualifierDecl::getFlavor() const
{
    _checkRep();
    return _rep->getFlavor();
}

Boolean CIMConstQualifierDecl::isFlavor(Uint32 flavor) const
{
    return ((getFlavor() & flavor) !=0);
}

Uint32 CIMConstQualifierDecl::getArraySize() const
{
    _checkRep();
    return _rep->getArraySize();
}

Boolean CIMConstQualifierDecl::isNull() const
{
    return (_rep == 0)? true : false;
}

Boolean CIMConstQualifierDecl::identical(const CIMConstQualifierDecl& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMQualifierDecl CIMConstQualifierDecl::clone() const
{
    return CIMQualifierDecl(_rep->clone());
}

void CIMConstQualifierDecl::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}

PEGASUS_NAMESPACE_END
