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

#include "CIMInstanceRep.h"
#include "CIMInstance.h"
#include "DeclContext.h"
#include "Indentor.h"
#include "CIMName.h"
#include "XmlWriter.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMInstance
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

////////////////////////////////////////////////////////////////////////////////
//
// CIMInstance
//
////////////////////////////////////////////////////////////////////////////////

CIMInstance::CIMInstance()
    : _rep(0)
{
}

CIMInstance::CIMInstance(const CIMInstance& x)
{
    Inc(_rep = x._rep);
}

CIMInstance::CIMInstance(const CIMObject& x)
{
    if (!(_rep = dynamic_cast<CIMInstanceRep*>(x._rep)))
	throw DynamicCastFailed();
    Inc(_rep);
}

CIMInstance::CIMInstance(const CIMObject& x, NoThrow&)
{
    _rep = dynamic_cast<CIMInstanceRep*>(x._rep);
    Inc(_rep);
}

CIMInstance::CIMInstance(const String& className)
{
    _rep = new CIMInstanceRep(className);
}

CIMInstance::CIMInstance(CIMInstanceRep* rep)
    : _rep(rep)
{
}

CIMInstance& CIMInstance::operator=(const CIMInstance& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMInstance::~CIMInstance()
{
    Dec(_rep);
}

const String& CIMInstance::getClassName() const
{
    _checkRep();
    return _rep->getClassName();
}

const Boolean CIMInstance::equalClassName(const String& classname) const
{
    _checkRep();
    return _rep->equalClassName(classname);

}

const CIMReference& CIMInstance::getPath() const
{
    _checkRep();
    return _rep->getPath();
}

CIMInstance& CIMInstance::addQualifier(const CIMQualifier& qualifier)
{
    _checkRep();
    _rep->addQualifier(qualifier);
    return *this;
}

Uint32 CIMInstance::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

Boolean CIMInstance::existsQualifier(const String& name) const
{
    _checkRep();
    return _rep->existsQualifier(name);
}

CIMQualifier CIMInstance::getQualifier(Uint32 pos)
{
    _checkRep();
    return _rep->getQualifier(pos);
}

CIMConstQualifier CIMInstance::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

Uint32 CIMInstance::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

CIMInstance& CIMInstance::addProperty(const CIMProperty& x)
{
    _checkRep();
    _rep->addProperty(x);
    return *this;
}

Uint32 CIMInstance::findProperty(const String& name) const
{
    _checkRep();
    return _rep->findProperty(name);
}

Boolean CIMInstance::existsProperty(const String& name) const
{
   _checkRep();
   return _rep->existsProperty(name);
}

CIMProperty CIMInstance::getProperty(Uint32 pos)
{
    _checkRep();
    return _rep->getProperty(pos);
}

CIMConstProperty CIMInstance::getProperty(Uint32 pos) const
{
    _checkRep();
    return _rep->getProperty(pos);
}

void CIMInstance::removeProperty(Uint32 pos)
{
    _checkRep();
    _rep->removeProperty(pos);
}

Uint32 CIMInstance::getPropertyCount() const
{
    _checkRep();
    return _rep->getPropertyCount();
}

CIMInstance::operator int() const
{
    return (_rep != 0);
}

Boolean CIMInstance::identical(const CIMConstInstance& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

void CIMInstance::resolve(
    DeclContext* declContext, 
    const String& nameSpace,
    Boolean propagateQualifiers)
{
    _checkRep();
    CIMConstClass cimClass;
    _rep->resolve(declContext, nameSpace, cimClass, propagateQualifiers);
}

void CIMInstance::resolve(
    DeclContext* declContext,
    const String& nameSpace,
    CIMConstClass& cimClassOut,
    Boolean propagateQualifiers)
{
    _checkRep();
    _rep->resolve(declContext, nameSpace, cimClassOut, propagateQualifiers);
}

void CIMInstance::toXml(Array<Sint8>& out) const
{
    _checkRep();
    _rep->toXml(out);
}

void CIMInstance::print(PEGASUS_STD(ostream)& o) const
{
    _checkRep();
    _rep->print(o);
}

void CIMInstance::toMof(Array<Sint8>& out) const
{
    _checkRep();
    _rep->toMof(out);
}

CIMInstance CIMInstance::clone() const
{
    return CIMInstance((CIMInstanceRep*)(_rep->clone()));
}

CIMReference CIMInstance::getInstanceName(const CIMConstClass& cimClass) const
{
    _checkRep();
    return _rep->getInstanceName(cimClass);
}

String CIMInstance::toString() const
{
    _checkRep();
    return _rep->toString();
}

void CIMInstance::_checkRep() const
{
    if (!_rep)
        ThrowUnitializedHandle();
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstInstance
//
////////////////////////////////////////////////////////////////////////////////

CIMConstInstance::CIMConstInstance()
    : _rep(0)
{
}

CIMConstInstance::CIMConstInstance(const CIMConstInstance& x)
{
    Inc(_rep = x._rep);
}

CIMConstInstance::CIMConstInstance(const CIMInstance& x)
{
    Inc(_rep = x._rep);
}

CIMConstInstance::CIMConstInstance(const CIMObject& x)
{
    if (!(_rep = dynamic_cast<CIMInstanceRep*>(x._rep)))
	throw DynamicCastFailed();
    Inc(_rep);
}

CIMConstInstance::CIMConstInstance(const CIMConstObject& x)
{
    if (!(_rep = dynamic_cast<CIMInstanceRep*>(x._rep)))
	throw DynamicCastFailed();
    Inc(_rep);
}

CIMConstInstance::CIMConstInstance(const CIMObject& x, NoThrow&)
{
    _rep = dynamic_cast<CIMInstanceRep*>(x._rep);
    Inc(_rep);
}

CIMConstInstance::CIMConstInstance(const CIMConstObject& x, NoThrow&)
{
    _rep = dynamic_cast<CIMInstanceRep*>(x._rep);
    Inc(_rep);
}

CIMConstInstance::CIMConstInstance(const String& className)
{
    _rep = new CIMInstanceRep(className);
}

CIMConstInstance& CIMConstInstance::operator=(const CIMConstInstance& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstInstance& CIMConstInstance::operator=(const CIMInstance& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstInstance::~CIMConstInstance()
{
    Dec(_rep);
}

const String& CIMConstInstance::getClassName() const
{
    _checkRep();
    return _rep->getClassName();
}

const Boolean CIMConstInstance::equalClassName(const String& classname) const
{
    _checkRep();
    return _rep->equalClassName(classname);
}

const CIMReference& CIMConstInstance::getPath() const
{
    _checkRep();
    return _rep->getPath();
}

Uint32 CIMConstInstance::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

CIMConstQualifier CIMConstInstance::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

Uint32 CIMConstInstance::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

Uint32 CIMConstInstance::findProperty(const String& name) const
{
    _checkRep();
    return _rep->findProperty(name);
}

CIMConstProperty CIMConstInstance::getProperty(Uint32 pos) const
{
    _checkRep();
    return _rep->getProperty(pos);
}

Uint32 CIMConstInstance::getPropertyCount() const
{
    _checkRep();
    return _rep->getPropertyCount();
}

CIMConstInstance::operator int() const
{
    return (_rep != 0);
}

void CIMConstInstance::toXml(Array<Sint8>& out) const
{
    _checkRep();
    _rep->toXml(out);
}

void CIMConstInstance::print(PEGASUS_STD(ostream)& o) const
{
    _checkRep();
    _rep->print(o);
}

Boolean CIMConstInstance::identical(const CIMConstInstance& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMInstance CIMConstInstance::clone() const
{
    return CIMInstance((CIMInstanceRep*)(_rep->clone()));
}

CIMReference CIMConstInstance::getInstanceName(const CIMConstClass& cimClass) const
{
    _checkRep();
    return _rep->getInstanceName(cimClass);
}

String CIMConstInstance::toString() const
{
    _checkRep();
    return _rep->toString();
}

void CIMConstInstance::_checkRep() const
{
    if (!_rep)
        ThrowUnitializedHandle();
}

PEGASUS_NAMESPACE_END
