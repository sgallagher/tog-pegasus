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

#include "CIMQualifier.h"
#include "CIMQualifierRep.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMQualifier
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifier
//
////////////////////////////////////////////////////////////////////////////////

CIMQualifier::CIMQualifier()
    : _rep(0)
{
}

CIMQualifier::CIMQualifier(const CIMQualifier& x)
{
    Inc(_rep = x._rep);
}

CIMQualifier::CIMQualifier(
    const String& name,
    const CIMValue& value,
    Uint32 flavor,
    Boolean propagated)
{
    _rep = new CIMQualifierRep(name, value, flavor, propagated);
}

CIMQualifier::CIMQualifier(CIMQualifierRep* rep)
    : _rep(rep)
{
}

CIMQualifier::~CIMQualifier()
{
    Dec(_rep);
}

CIMQualifier& CIMQualifier::operator=(const CIMQualifier& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }

    return *this;
}

const String& CIMQualifier::getName() const
{
    _checkRep();
    return _rep->getName();
}

void CIMQualifier::setName(const String& name)
{
    _checkRep();
    _rep->setName(name);
}

CIMType CIMQualifier::getType() const
{
    _checkRep();
    return _rep->getType();
}

Boolean CIMQualifier::isArray() const
{
    _checkRep();
    return _rep->isArray();
}

const CIMValue& CIMQualifier::getValue() const
{
    _checkRep();
    return _rep->getValue();
}

void CIMQualifier::setValue(const CIMValue& value)
{
    _checkRep();
    _rep->setValue(value);
}

void CIMQualifier::setFlavor(Uint32 flavor)
{
    _checkRep();
    _rep->setFlavor(flavor);
}

void CIMQualifier::unsetFlavor(Uint32 flavor)
{
    _checkRep();
    _rep->unsetFlavor(flavor);
}

Uint32 CIMQualifier::getFlavor() const
{
    _checkRep();
    return _rep->getFlavor();
}

Boolean CIMQualifier::isFlavor(Uint32 flavor) const
{
    return _rep->isFlavor(flavor);
}

void CIMQualifier::resolveFlavor(Uint32 inheritedFlavor, Boolean inherited)
{
    _checkRep();
    _rep->resolveFlavor(inheritedFlavor, inherited);
}

const Uint32 CIMQualifier::getPropagated() const
{
    _checkRep();
    return _rep->getPropagated();
}

void CIMQualifier::setPropagated(Boolean propagated)
{
    _checkRep();
    _rep->setPropagated(propagated);
}

Boolean CIMQualifier::isNull() const
{
    return (_rep == 0)? true : false;
}

Boolean CIMQualifier::identical(const CIMConstQualifier& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMQualifier CIMQualifier::clone() const
{
    return CIMQualifier(_rep->clone());
}

void CIMQualifier::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMConstQualifier
//
////////////////////////////////////////////////////////////////////////////////

CIMConstQualifier::CIMConstQualifier()
    : _rep(0)
{
}

CIMConstQualifier::CIMConstQualifier(const CIMConstQualifier& x)
{
    Inc(_rep = x._rep);
}

CIMConstQualifier::CIMConstQualifier(const CIMQualifier& x)
{
    Inc(_rep = x._rep);
}

CIMConstQualifier::CIMConstQualifier(
    const String& name,
    const CIMValue& value,
    Uint32 flavor,
    Boolean propagated)
{
    _rep = new CIMQualifierRep(name, value, flavor, propagated);
}

CIMConstQualifier::~CIMConstQualifier()
{
    Dec(_rep);
}

CIMConstQualifier& CIMConstQualifier::operator=(const CIMConstQualifier& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }

    return *this;
}

CIMConstQualifier& CIMConstQualifier::operator=(const CIMQualifier& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }

    return *this;
}

const String& CIMConstQualifier::getName() const
{
    _checkRep();
    return _rep->getName();
}

CIMType CIMConstQualifier::getType() const
{
    _checkRep();
    return _rep->getType();
}

Boolean CIMConstQualifier::isArray() const
{
    _checkRep();
    return _rep->isArray();
}

const CIMValue& CIMConstQualifier::getValue() const
{
    _checkRep();
    return _rep->getValue();
}

const Uint32 CIMConstQualifier::getFlavor() const
{
    _checkRep();
    return _rep->getFlavor();
}

Boolean CIMConstQualifier::isFlavor(Uint32 flavor) const
{
    return ((getFlavor() & flavor) !=0);
}

Boolean CIMConstQualifier::isFlavorToSubclass() const
{
    return ((getFlavor() & CIMFlavor::TOSUBCLASS) != 0);
}

Boolean CIMConstQualifier::isFlavorToInstance() const
{
    return ((getFlavor() & CIMFlavor::TOINSTANCE) != 0);
}

Boolean CIMConstQualifier::isFlavorOverridable() const
{
    return ((getFlavor() & CIMFlavor::OVERRIDABLE) != 0);
}

const Uint32 CIMConstQualifier::getPropagated() const
{
    _checkRep();
    return _rep->getPropagated();
}

Boolean CIMConstQualifier::isNull() const
{
    return (_rep == 0)? true : false;
}

Boolean CIMConstQualifier::identical(const CIMConstQualifier& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMQualifier CIMConstQualifier::clone() const
{
    return CIMQualifier(_rep->clone());
}

void CIMConstQualifier::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}

PEGASUS_NAMESPACE_END
