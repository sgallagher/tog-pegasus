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

#include "CIMPropertyRep.h"
#include "CIMProperty.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMProperty
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T


////////////////////////////////////////////////////////////////////////////////
//
// CIMProperty
//
////////////////////////////////////////////////////////////////////////////////

CIMProperty::CIMProperty()
    : _rep(0)
{
}

CIMProperty::CIMProperty(const CIMProperty& x)
{
    Inc(_rep = x._rep);
}

CIMProperty::CIMProperty(
    const String& name,
    const CIMValue& value,
    Uint32 arraySize,
    const String& referenceClassName,
    const String& classOrigin,
    Boolean propagated)
{
    _rep = new CIMPropertyRep(name, value,
        arraySize, referenceClassName, classOrigin, propagated);
}

// This constructor allows the CIMClassRep friend class to cast
// away constness.
CIMProperty::CIMProperty(const CIMConstProperty& x)
{
    Inc(_rep = x._rep);
}

CIMProperty::CIMProperty(CIMPropertyRep* rep)
    : _rep(rep)
{
}

CIMProperty::~CIMProperty()
{
    Dec(_rep);
}

CIMProperty& CIMProperty::operator=(const CIMProperty& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

const String& CIMProperty::getName() const
{
    _checkRep();
    return _rep->getName();
}

void CIMProperty::setName(const String& name)
{
    _checkRep();
    _rep->setName(name);
}

const CIMValue& CIMProperty::getValue() const
{
    _checkRep();
    return _rep->getValue();
}

CIMType CIMProperty::getType() const
{
    _checkRep();
    return _rep->getValue().getType();
}

Boolean CIMProperty::isArray() const
{
    _checkRep();
    return _rep->getValue().isArray();
}

void CIMProperty::setValue(const CIMValue& value)
{
    _checkRep();
    _rep->setValue(value);
}

Uint32 CIMProperty::getArraySize() const
{
    _checkRep();
    return _rep->getArraySize();
}

const String& CIMProperty::getReferenceClassName() const
{
    _checkRep();
    return _rep->getReferenceClassName();
}

const String& CIMProperty::getClassOrigin() const
{
    _checkRep();
    return _rep->getClassOrigin();
}

void CIMProperty::setClassOrigin(const String& classOrigin)
{
    _checkRep();
    _rep->setClassOrigin(classOrigin);
}

Boolean CIMProperty::getPropagated() const
{
    _checkRep();
    return _rep->getPropagated();
}

void CIMProperty::setPropagated(Boolean propagated)
{
    _checkRep();
    _rep->setPropagated(propagated);
}

CIMProperty& CIMProperty::addQualifier(const CIMQualifier& x)
{
    _checkRep();
    _rep->addQualifier(x);
    return *this;
}

Uint32 CIMProperty::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

Boolean CIMProperty::existsQualifier(const String& name) const
{
    _checkRep();
    return _rep->existsQualifier(name);
}

CIMQualifier CIMProperty::getQualifier(Uint32 pos)
{
    _checkRep();
    return _rep->getQualifier(pos);
}

CIMConstQualifier CIMProperty::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

void CIMProperty::removeQualifier(Uint32 pos)
{
    _checkRep();
    _rep->removeQualifier(pos);
}
    
Uint32 CIMProperty::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

void CIMProperty::resolve(
    DeclContext* declContext,
    const String& nameSpace,
    Boolean isInstancePart,
    const CIMConstProperty& property,
    Boolean propagateQualifiers)
{
    _checkRep();
    _rep->resolve(declContext, 
        nameSpace, isInstancePart, property, propagateQualifiers);
}

void CIMProperty::resolve(
    DeclContext* declContext,
    const String& nameSpace,
    Boolean isInstancePart,
    Boolean propagateQualifiers)
{
    _checkRep();
    _rep->resolve(
        declContext, nameSpace, isInstancePart, propagateQualifiers);
}

Boolean CIMProperty::isNull() const
{
    return (_rep == 0)? true : false;
}

void CIMProperty::toXml(Array<Sint8>& out) const
{
    _checkRep();
    _rep->toXml(out);
}

void CIMProperty::print(PEGASUS_STD(ostream)& o) const
{
    _checkRep();
    _rep->print(o);
}

void CIMProperty::toMof(Array<Sint8>& out) const
{
    _checkRep();
    _rep->toMof(out);
}

Boolean CIMProperty::identical(const CIMConstProperty& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

Boolean CIMProperty::isKey() const
{
    _checkRep();
    return _rep->isKey();
}

CIMProperty CIMProperty::clone(Boolean propagateQualifiers) const
{
    return CIMProperty(_rep->clone(propagateQualifiers));
}

void CIMProperty::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstProperty
//
////////////////////////////////////////////////////////////////////////////////

CIMConstProperty::CIMConstProperty()
    : _rep(0)
{
}

CIMConstProperty::CIMConstProperty(const CIMConstProperty& x)
{
    Inc(_rep = x._rep);
}

CIMConstProperty::CIMConstProperty(const CIMProperty& x)
{
    Inc(_rep = x._rep);
}

// Throws IllegalName if name argument not legal CIM identifier.

CIMConstProperty::CIMConstProperty(
    const String& name,
    const CIMValue& value,
    Uint32 arraySize,
    const String& referenceClassName,
    const String& classOrigin,
    Boolean propagated)
{
    _rep = new CIMPropertyRep(name, value,
        arraySize, referenceClassName, classOrigin, propagated);
}

CIMConstProperty::~CIMConstProperty()
{
    Dec(_rep);
}

CIMConstProperty& CIMConstProperty::operator=(const CIMConstProperty& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstProperty& CIMConstProperty::operator=(const CIMProperty& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

const String& CIMConstProperty::getName() const
{
    _checkRep();
    return _rep->getName();
}

const CIMValue& CIMConstProperty::getValue() const
{
    _checkRep();
    return _rep->getValue();
}

CIMType CIMConstProperty::getType() const
{
    _checkRep();
    return _rep->getValue().getType();
}

Boolean CIMConstProperty::isArray() const
{
    _checkRep();
    return _rep->getValue().isArray();
}

Uint32 CIMConstProperty::getArraySize() const
{
    _checkRep();
    return _rep->getArraySize();
}

const String& CIMConstProperty::getReferenceClassName() const
{
    _checkRep();
    return _rep->getReferenceClassName();
}

const String& CIMConstProperty::getClassOrigin() const
{
    _checkRep();
    return _rep->getClassOrigin();
}

Boolean CIMConstProperty::getPropagated() const
{
    _checkRep();
    return _rep->getPropagated();
}

Uint32 CIMConstProperty::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

CIMConstQualifier CIMConstProperty::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

Uint32 CIMConstProperty::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

Boolean CIMConstProperty::isNull() const
{
    return (_rep == 0)? true : false;
}

void CIMConstProperty::toXml(Array<Sint8>& out) const
{
    _checkRep();
    _rep->toXml(out);
}

void CIMConstProperty::print(PEGASUS_STD(ostream)& o) const
{
    _checkRep();
    _rep->print(o);
}

void CIMConstProperty::toMof(Array<Sint8>& out) const
{
    _checkRep();
    _rep->toMof(out);
}

Boolean CIMConstProperty::identical(const CIMConstProperty& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

Boolean CIMConstProperty::isKey() const
{
    _checkRep();
    return _rep->isKey();
}

CIMProperty CIMConstProperty::clone(Boolean propagateQualifiers) const
{
    return CIMProperty(_rep->clone(propagateQualifiers));
}

void CIMConstProperty::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}

PEGASUS_NAMESPACE_END
