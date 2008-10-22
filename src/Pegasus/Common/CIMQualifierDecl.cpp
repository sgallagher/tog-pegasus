//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
    _rep = x._rep;
     if (_rep)
         _rep->Inc();
}

CIMQualifierDecl::CIMQualifierDecl(
    const CIMName& name,
    const CIMValue& value,
    const CIMScope & scope,
    const CIMFlavor & flavor,
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
    if (_rep)
        _rep->Dec();
}

CIMQualifierDecl& CIMQualifierDecl::operator=(const CIMQualifierDecl& x)
{
    if (x._rep != _rep)
    {
        if (_rep)
            _rep->Dec();
        _rep = x._rep;
         if (_rep)
             _rep->Inc();
    }

    return *this;
}

const CIMName& CIMQualifierDecl::getName() const
{
    CheckRep(_rep);
    return _rep->getName();
}

void CIMQualifierDecl::setName(const CIMName& name)
{
    CheckRep(_rep);
    _rep->setName(name);
}

CIMType CIMQualifierDecl::getType() const
{
    CheckRep(_rep);
    return _rep->getType();
}

Boolean CIMQualifierDecl::isArray() const
{
    CheckRep(_rep);
    return _rep->isArray();
}

const CIMValue& CIMQualifierDecl::getValue() const
{
    CheckRep(_rep);
    return _rep->getValue();
}

void CIMQualifierDecl::setValue(const CIMValue& value)
{
    CheckRep(_rep);
    _rep->setValue(value);
}

const CIMScope & CIMQualifierDecl::getScope() const
{
    CheckRep(_rep);
    return _rep->getScope();
}

const CIMFlavor & CIMQualifierDecl::getFlavor() const
{
    CheckRep(_rep);
    return _rep->getFlavor();
}

Uint32 CIMQualifierDecl::getArraySize() const
{
    CheckRep(_rep);
    return _rep->getArraySize();
}

Boolean CIMQualifierDecl::isUninitialized() const
{
    return _rep == 0;
}

Boolean CIMQualifierDecl::identical(const CIMConstQualifierDecl& x) const
{
    CheckRep(x._rep);
    CheckRep(_rep);
    return _rep->identical(x._rep);
}

CIMQualifierDecl CIMQualifierDecl::clone() const
{
    return CIMQualifierDecl(_rep->clone());
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
    _rep = x._rep;
     if (_rep)
         _rep->Inc();
}

CIMConstQualifierDecl::CIMConstQualifierDecl(const CIMQualifierDecl& x)
{
    _rep = x._rep;
     if (_rep)
         _rep->Inc();
}

CIMConstQualifierDecl::CIMConstQualifierDecl(
    const CIMName& name,
    const CIMValue& value,
    const CIMScope & scope,
    const CIMFlavor & flavor,
    Uint32 arraySize)
{
    _rep = new CIMQualifierDeclRep(name, value, scope, flavor, arraySize);
}

CIMConstQualifierDecl::~CIMConstQualifierDecl()
{
    if (_rep)
        _rep->Dec();
}

CIMConstQualifierDecl& CIMConstQualifierDecl::operator=(
    const CIMConstQualifierDecl& x)
{
    if (x._rep != _rep)
    {
        if (_rep)
            _rep->Dec();
        _rep = x._rep;
         if (_rep)
             _rep->Inc();
    }

    return *this;
}

CIMConstQualifierDecl& CIMConstQualifierDecl::operator=(
    const CIMQualifierDecl& x)
{
    if (x._rep != _rep)
    {
        if (_rep)
            _rep->Dec();
        _rep = x._rep;
         if (_rep)
             _rep->Inc();
    }

    return *this;
}

const CIMName& CIMConstQualifierDecl::getName() const
{
    CheckRep(_rep);
    return _rep->getName();
}

CIMType CIMConstQualifierDecl::getType() const
{
    CheckRep(_rep);
    return _rep->getType();
}

Boolean CIMConstQualifierDecl::isArray() const
{
    CheckRep(_rep);
    return _rep->isArray();
}

const CIMValue& CIMConstQualifierDecl::getValue() const
{
    CheckRep(_rep);
    return _rep->getValue();
}

const CIMScope & CIMConstQualifierDecl::getScope() const
{
    CheckRep(_rep);
    return _rep->getScope();
}

const CIMFlavor & CIMConstQualifierDecl::getFlavor() const
{
    CheckRep(_rep);
    return _rep->getFlavor();
}

Uint32 CIMConstQualifierDecl::getArraySize() const
{
    CheckRep(_rep);
    return _rep->getArraySize();
}

Boolean CIMConstQualifierDecl::isUninitialized() const
{
    return _rep == 0;
}

Boolean CIMConstQualifierDecl::identical(const CIMConstQualifierDecl& x) const
{
    CheckRep(x._rep);
    CheckRep(_rep);
    return _rep->identical(x._rep);
}

CIMQualifierDecl CIMConstQualifierDecl::clone() const
{
    return CIMQualifierDecl(_rep->clone());
}

PEGASUS_NAMESPACE_END
