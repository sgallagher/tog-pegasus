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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMClassRep.h"
#include "CIMClass.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMClass
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

////////////////////////////////////////////////////////////////////////////////
//
// CIMClass
//
////////////////////////////////////////////////////////////////////////////////

CIMClass::CIMClass()
    : _rep(0)
{
}

CIMClass::CIMClass(const CIMClass& x)
{
    Inc(_rep = x._rep);
}

CIMClass::CIMClass(const CIMObject& x) throw(DynamicCastFailed)
{
    if (!(_rep = dynamic_cast<CIMClassRep*>(x._rep)))
	throw DynamicCastFailed();
    Inc(_rep);
}

CIMClass::CIMClass(
    const CIMObjectPath& reference,
    const String& superClassName)
{
    _rep = new CIMClassRep(reference, superClassName);
}

CIMClass::CIMClass(CIMClassRep* rep)
    : _rep(rep)
{
}

CIMClass& CIMClass::operator=(const CIMClass& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMClass::~CIMClass()
{
    Dec(_rep);
}

Boolean CIMClass::isAssociation() const
{
    _checkRep();
    return _rep->isAssociation();
}

Boolean CIMClass::isAbstract() const
{
    _checkRep();
    return _rep->isAbstract();
}

const String& CIMClass::getClassName() const
{
    _checkRep();
    return _rep->getClassName();
}

const CIMObjectPath& CIMClass::getPath() const
{
    _checkRep();
    return _rep->getPath();
}

void CIMClass::setPath (const CIMObjectPath & path)
{
    _checkRep ();
    _rep->setPath (path);
}

const String& CIMClass::getSuperClassName() const
{
    _checkRep();
    return _rep->getSuperClassName();
}

void CIMClass::setSuperClassName(const String& superClassName)
{
    _checkRep();
    _rep->setSuperClassName(superClassName);
}

CIMClass& CIMClass::addQualifier(const CIMQualifier& qualifier)
{
    _checkRep();
    _rep->addQualifier(qualifier);
    return *this;
}

Uint32 CIMClass::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

Boolean CIMClass::existsQualifier(const String& name) const
{
    _checkRep();
    return _rep->existsQualifier(name);
}

Boolean CIMClass::isTrueQualifier(const String& name) const
{
    _checkRep();
    return _rep->isTrueQualifier(name);
}

CIMQualifier CIMClass::getQualifier(Uint32 pos)
{
    _checkRep();
    return _rep->getQualifier(pos);
}

CIMConstQualifier CIMClass::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

void CIMClass::removeQualifier(Uint32 pos)
{
    _checkRep();
    _rep->removeQualifier(pos);
}

Uint32 CIMClass::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

CIMClass& CIMClass::addProperty(const CIMProperty& x)
{
    _checkRep();
    _rep->addProperty(x);
    return *this;
}

Uint32 CIMClass::findProperty(const String& name) const
{
    _checkRep();
    return _rep->findProperty(name);
}

Boolean CIMClass::existsProperty(const String& name) const
{
    _checkRep();
    return _rep->existsProperty(name);
}

CIMProperty CIMClass::getProperty(Uint32 pos)
{
    _checkRep();
    return _rep->getProperty(pos);
}

CIMConstProperty CIMClass::getProperty(Uint32 pos) const
{
    _checkRep();
    return _rep->getProperty(pos);
}

void CIMClass::removeProperty(Uint32 pos)
{
    _checkRep();
    _rep->removeProperty(pos);
}

Uint32 CIMClass::getPropertyCount() const
{
    _checkRep();
    return _rep->getPropertyCount();
}

CIMClass& CIMClass::addMethod(const CIMMethod& x)
{
    _checkRep();
    _rep->addMethod(x);
    return *this;
}

Uint32 CIMClass::findMethod(const String& name) const
{
    _checkRep();
    return _rep->findMethod(name);
}

Boolean CIMClass::existsMethod(const String& name) const
{
    _checkRep();
    return _rep->existsMethod(name);
}

CIMMethod CIMClass::getMethod(Uint32 pos)
{
    _checkRep();
    return _rep->getMethod(pos);
}

CIMConstMethod CIMClass::getMethod(Uint32 pos) const
{
    _checkRep();
    return _rep->getMethod(pos);
}

void CIMClass::removeMethod(Uint32 pos)
{
    _checkRep();
    _rep->removeMethod(pos);
}

Uint32 CIMClass::getMethodCount() const
{
    _checkRep();
    return _rep->getMethodCount();
}

void CIMClass::resolve(
    DeclContext* declContext,
    const String& nameSpace)
{
    _checkRep();
    _rep->resolve(declContext, nameSpace);
}

Boolean CIMClass::isNull() const
{
    return (_rep == 0)? true : false;
}

Boolean CIMClass::identical(const CIMConstClass& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMClass CIMClass::clone() const
{
    return CIMClass((CIMClassRep*)(_rep->clone()));
}

void CIMClass::getKeyNames(Array<String>& keyNames) const
{
    _checkRep();
    _rep->getKeyNames(keyNames);
}

Boolean CIMClass::hasKeys() const
{
    _checkRep();
    return _rep->hasKeys();
}

void CIMClass::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstClass
//
////////////////////////////////////////////////////////////////////////////////

CIMConstClass::CIMConstClass()
    : _rep(0)
{
}

CIMConstClass::CIMConstClass(const CIMConstClass& x)
{
    Inc(_rep = x._rep);
}

CIMConstClass::CIMConstClass(const CIMClass& x)
{
    Inc(_rep = x._rep);
}

CIMConstClass::CIMConstClass(const CIMObject& x) throw(DynamicCastFailed)
{
    if (!(_rep = dynamic_cast<CIMClassRep*>(x._rep)))
	throw DynamicCastFailed();
    Inc(_rep);
}

CIMConstClass::CIMConstClass(const CIMConstObject& x) throw(DynamicCastFailed)
{
    if (!(_rep = dynamic_cast<CIMClassRep*>(x._rep)))
	throw DynamicCastFailed();
    Inc(_rep);
}

CIMConstClass::CIMConstClass(
    const CIMObjectPath& reference,
    const String& superClassName)
{
    _rep = new CIMClassRep(reference, superClassName);
}

CIMConstClass& CIMConstClass::operator=(const CIMConstClass& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstClass& CIMConstClass::operator=(const CIMClass& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstClass::~CIMConstClass()
{
    Dec(_rep);
}

Boolean CIMConstClass::isAssociation() const
{
    _checkRep();
    return _rep->isAssociation();
}

Boolean CIMConstClass::isAbstract() const
{
    _checkRep();
    return _rep->isAbstract();
}

const String& CIMConstClass::getClassName() const
{
    _checkRep();
    return _rep->getClassName();
}

const CIMObjectPath& CIMConstClass::getPath() const
{
    _checkRep();
    return _rep->getPath();
}

const String& CIMConstClass::getSuperClassName() const
{
    _checkRep();
    return _rep->getSuperClassName();
}

Uint32 CIMConstClass::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

CIMConstQualifier CIMConstClass::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

Boolean CIMConstClass::isTrueQualifier(const String& name) const
{
    _checkRep();
    return _rep->isTrueQualifier(name);
}

Uint32 CIMConstClass::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

Uint32 CIMConstClass::findProperty(const String& name) const
{
    _checkRep();
    return _rep->findProperty(name);
}

CIMConstProperty CIMConstClass::getProperty(Uint32 pos) const
{
    _checkRep();
    return _rep->getProperty(pos);
}

Uint32 CIMConstClass::getPropertyCount() const
{
    _checkRep();
    return _rep->getPropertyCount();
}

Uint32 CIMConstClass::findMethod(const String& name) const
{
    _checkRep();
    return _rep->findMethod(name);
}

CIMConstMethod CIMConstClass::getMethod(Uint32 pos) const
{
    _checkRep();
    return _rep->getMethod(pos);
}

Uint32 CIMConstClass::getMethodCount() const
{
    _checkRep();
    return _rep->getMethodCount();
}

Boolean CIMConstClass::isNull() const
{
    return (_rep == 0)? true : false;
}

Boolean CIMConstClass::identical(const CIMConstClass& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMClass CIMConstClass::clone() const
{
    return CIMClass((CIMClassRep*)(_rep->clone()));
}

void CIMConstClass::getKeyNames(Array<String>& keyNames) const
{
    _checkRep();
    _rep->getKeyNames(keyNames);
}

Boolean CIMConstClass::hasKeys() const
{
    _checkRep();
    return _rep->hasKeys();
}

void CIMConstClass::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}

PEGASUS_NAMESPACE_END
