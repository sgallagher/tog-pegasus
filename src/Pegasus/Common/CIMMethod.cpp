//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMMethod.h"
#include "CIMMethodRep.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMMethod
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

///////////////////////////////////////////////////////////////////////////////
//
// CIMMethod
//
///////////////////////////////////////////////////////////////////////////////

CIMMethod::CIMMethod()
    : _rep(0)
{
}

CIMMethod::CIMMethod(const CIMMethod& x)
{
    Inc(_rep = x._rep);
}

CIMMethod::CIMMethod(
    const String& name,
    CIMType type,
    const String& classOrigin,
    Boolean propagated)
{
    _rep = new CIMMethodRep(name, type, classOrigin, propagated);
}

CIMMethod::CIMMethod(CIMMethodRep* rep)
    : _rep(rep)
{
}

CIMMethod::CIMMethod(const CIMConstMethod& x)
{
    Inc(_rep = x._rep);
}

CIMMethod::~CIMMethod()
{
    Dec(_rep);
}

CIMMethod& CIMMethod::operator=(const CIMMethod& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

const String& CIMMethod::getName() const
{
    _checkRep();
    return _rep->getName();
}

void CIMMethod::setName(const String& name)
{
    _checkRep();
    _rep->setName(name);
}

CIMType CIMMethod::getType() const
{
    _checkRep();
    return _rep->getType();
}

void CIMMethod::setType(CIMType type)
{
    _checkRep();
    _rep->setType(type);
}

const String& CIMMethod::getClassOrigin() const
{
    _checkRep();
    return _rep->getClassOrigin();
}

void CIMMethod::setClassOrigin(const String& classOrigin)
{
    _checkRep();
    _rep->setClassOrigin(classOrigin);
}

Boolean CIMMethod::getPropagated() const
{
    _checkRep();
    return _rep->getPropagated();
}

void CIMMethod::setPropagated(Boolean propagated)
{
    _checkRep();
    _rep->setPropagated(propagated);
}

CIMMethod& CIMMethod::addQualifier(const CIMQualifier& x)
{
    _checkRep();
    _rep->addQualifier(x);
    return *this;
}

Uint32 CIMMethod::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

Boolean CIMMethod::existsQualifier(const String& name) const
{
    _checkRep();
    return _rep->existsQualifier(name);
}

CIMQualifier CIMMethod::getQualifier(Uint32 pos)
{
    _checkRep();
    return _rep->getQualifier(pos);
}

CIMConstQualifier CIMMethod::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

void CIMMethod::removeQualifier(Uint32 pos)
{
    _checkRep();
    _rep->removeQualifier(pos);
}

Uint32 CIMMethod::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

CIMMethod& CIMMethod::addParameter(const CIMParameter& x)
{
    _checkRep();
    _rep->addParameter(x);
    return *this;
}

Uint32 CIMMethod::findParameter(const String& name) const
{
    _checkRep();
    return _rep->findParameter(name);
}

CIMParameter CIMMethod::getParameter(Uint32 pos)
{
    _checkRep();
    return _rep->getParameter(pos);
}

CIMConstParameter CIMMethod::getParameter(Uint32 pos) const
{
    _checkRep();
    return _rep->getParameter(pos);
}

Uint32 CIMMethod::getParameterCount() const
{
    _checkRep();
    return _rep->getParameterCount();
}

void CIMMethod::resolve(
    DeclContext* declContext,
    const String& nameSpace,
    const CIMConstMethod& method)
{
    _checkRep();
    _rep->resolve(declContext, nameSpace, method);
}

void CIMMethod::resolve(
    DeclContext* declContext,
    const String& nameSpace)
{
    _checkRep();
    _rep->resolve(declContext, nameSpace);
}

Boolean CIMMethod::isNull() const
{
    return (_rep == 0)? true : false;
}

Boolean CIMMethod::identical(const CIMConstMethod& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMMethod CIMMethod::clone() const
{
    return CIMMethod(_rep->clone());
}

void CIMMethod::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}


///////////////////////////////////////////////////////////////////////////////
//
// CIMConstMethod
//
///////////////////////////////////////////////////////////////////////////////

CIMConstMethod::CIMConstMethod()
    : _rep(0)
{
}

CIMConstMethod::CIMConstMethod(const CIMConstMethod& x)
{
    Inc(_rep = x._rep);
}

CIMConstMethod::CIMConstMethod(const CIMMethod& x)
{
    Inc(_rep = x._rep);
}

CIMConstMethod::CIMConstMethod(
    const String& name,
    CIMType type,
    const String& classOrigin,
    Boolean propagated)
{
    _rep = new CIMMethodRep(name, type, classOrigin, propagated);
}

CIMConstMethod::~CIMConstMethod()
{
    Dec(_rep);
}

CIMConstMethod& CIMConstMethod::operator=(const CIMConstMethod& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstMethod& CIMConstMethod::operator=(const CIMMethod& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

const String& CIMConstMethod::getName() const
{
    _checkRep();
    return _rep->getName();
}

CIMType CIMConstMethod::getType() const
{
    _checkRep();
    return _rep->getType();
}

const String& CIMConstMethod::getClassOrigin() const
{
    _checkRep();
    return _rep->getClassOrigin();
}

Boolean CIMConstMethod::getPropagated() const
{
    _checkRep();
    return _rep->getPropagated();
}

Uint32 CIMConstMethod::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

CIMConstQualifier CIMConstMethod::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

Uint32 CIMConstMethod::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

Uint32 CIMConstMethod::findParameter(const String& name) const
{
    _checkRep();
    return _rep->findParameter(name);
}

CIMConstParameter CIMConstMethod::getParameter(Uint32 pos) const
{
    _checkRep();
    return _rep->getParameter(pos);
}

Uint32 CIMConstMethod::getParameterCount() const
{
    _checkRep();
    return _rep->getParameterCount();
}

Boolean CIMConstMethod::isNull() const
{
    return (_rep == 0)? true : false;
}

Boolean CIMConstMethod::identical(const CIMConstMethod& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMMethod CIMConstMethod::clone() const
{
    return CIMMethod(_rep->clone());
}

void CIMConstMethod::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}

PEGASUS_NAMESPACE_END
