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

#include "CIMClassRep.h"
#include "CIMInstanceRep.h"
#include "CIMObjectRep.h"
#include "CIMObject.h"
#include "CIMClass.h"
#include "CIMInstance.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMObject
//
////////////////////////////////////////////////////////////////////////////////

CIMObject::CIMObject()
    : _rep(0)
{
}

CIMObject::CIMObject(const CIMObject& x)
{
    Inc(_rep = x._rep);
}

CIMObject::CIMObject(const CIMClass& x)
{
    Inc(_rep = x._rep);
}

CIMObject::CIMObject(const CIMInstance& x)
{
    Inc(_rep = x._rep);
}

CIMObject::CIMObject(CIMObjectRep* rep)
    : _rep(rep)
{
}

CIMObject& CIMObject::operator=(const CIMObject& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
	Inc(_rep = x._rep);
    }
    return *this;
}

CIMObject& CIMObject::operator=(const CIMClass& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
	Inc(_rep = x._rep);
    }
    return *this;
}

CIMObject& CIMObject::operator=(const CIMInstance& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
	Inc(_rep = x._rep);
    }
    return *this;
}

CIMObject::~CIMObject()
{
    Dec(_rep);
}

const String& CIMObject::getClassName() const
{
    _checkRep();
    return _rep->getClassName();
}

const CIMObjectPath& CIMObject::getPath() const
{
    _checkRep();
    return _rep->getPath();
}

CIMObject& CIMObject::addQualifier(const CIMQualifier& qualifier)
{
    _checkRep();
    _rep->addQualifier(qualifier);
    return *this;
}

Uint32 CIMObject::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

Boolean CIMObject::existsQualifier(const String& name) const
{
    _checkRep();
    return _rep->existsQualifier(name);
}

CIMQualifier CIMObject::getQualifier(Uint32 pos)
{
    _checkRep();
    return _rep->getQualifier(pos);
}

CIMConstQualifier CIMObject::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

void CIMObject::removeQualifier(Uint32 pos)
{
    _checkRep();
    _rep->removeQualifier(pos);
}

Uint32 CIMObject::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

CIMObject& CIMObject::addProperty(const CIMProperty& x)
{
    _checkRep();
    _rep->addProperty(x);
    return *this;
}

Uint32 CIMObject::findProperty(const String& name) const
{
    _checkRep();
    return _rep->findProperty(name);
}

Boolean CIMObject::existsProperty(const String& name) const
{
    _checkRep();
    return _rep->existsProperty(name);
}

CIMProperty CIMObject::getProperty(Uint32 pos)
{
    _checkRep();
    return _rep->getProperty(pos);
}

CIMConstProperty CIMObject::getProperty(Uint32 pos) const
{
    _checkRep();
    return _rep->getProperty(pos);
}

void CIMObject::removeProperty(Uint32 pos)
{
    _checkRep();
    _rep->removeProperty(pos);
}

Uint32 CIMObject::getPropertyCount() const
{
    _checkRep();
    return _rep->getPropertyCount();
}

Boolean CIMObject::isNull() const
{
    return (_rep == 0)? true : false;
}

Boolean CIMObject::identical(const CIMConstObject& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMObject CIMObject::clone() const
{
    _checkRep();
    return CIMObject(_rep->clone());
}

void CIMObject::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstObject
//
////////////////////////////////////////////////////////////////////////////////

CIMConstObject::CIMConstObject()
    : _rep(0)
{
}

CIMConstObject::CIMConstObject(const CIMConstObject& x)
{
    Inc(_rep = x._rep);
}

CIMConstObject::CIMConstObject(const CIMObject& x)
{
    Inc(_rep = x._rep);
}

CIMConstObject::CIMConstObject(const CIMClass& x)
{
    Inc(_rep = x._rep);
}

CIMConstObject::CIMConstObject(const CIMConstClass& x)
{
    Inc(_rep = x._rep);
}

CIMConstObject::CIMConstObject(const CIMInstance& x)
{
    Inc(_rep = x._rep);
}

CIMConstObject::CIMConstObject(const CIMConstInstance& x)
{
    Inc(_rep = x._rep);
}

CIMConstObject& CIMConstObject::operator=(const CIMConstObject& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstObject& CIMConstObject::operator=(const CIMObject& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
        Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstObject& CIMConstObject::operator=(const CIMClass& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
	Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstObject& CIMConstObject::operator=(const CIMInstance& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
	Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstObject& CIMConstObject::operator=(const CIMConstClass& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
	Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstObject& CIMConstObject::operator=(const CIMConstInstance& x)
{
    if (x._rep != _rep)
    {
        Dec(_rep);
	Inc(_rep = x._rep);
    }
    return *this;
}

CIMConstObject::~CIMConstObject()
{
    Dec(_rep);
}

const String& CIMConstObject::getClassName() const
{
    _checkRep();
    return _rep->getClassName();
}

const CIMObjectPath& CIMConstObject::getPath() const
{
    _checkRep();
    return _rep->getPath();
}

Uint32 CIMConstObject::findQualifier(const String& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

CIMConstQualifier CIMConstObject::getQualifier(Uint32 pos) const
{
    _checkRep();
    return _rep->getQualifier(pos);
}

Uint32 CIMConstObject::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

Uint32 CIMConstObject::findProperty(const String& name) const
{
    _checkRep();
    return _rep->findProperty(name);
}

CIMConstProperty CIMConstObject::getProperty(Uint32 pos) const
{
    _checkRep();
    return _rep->getProperty(pos);
}

Uint32 CIMConstObject::getPropertyCount() const
{
    _checkRep();
    return _rep->getPropertyCount();
}

Boolean CIMConstObject::isNull() const
{
    return (_rep == 0)? true : false;
}

Boolean CIMConstObject::identical(const CIMConstObject& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

CIMObject CIMConstObject::clone() const
{
    return CIMObject(_rep->clone());
}

void CIMConstObject::_checkRep() const
{
    if (!_rep)
        ThrowUninitializedHandle();
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMObjectWithPath
//
////////////////////////////////////////////////////////////////////////////////

CIMObjectWithPath::CIMObjectWithPath()
{
}

CIMObjectWithPath::CIMObjectWithPath(
    const CIMObjectPath& reference,
    const CIMObject& object)
    : _reference(reference), _object(object)
{
}

CIMObjectWithPath::CIMObjectWithPath(const CIMObjectWithPath& x)
    : _reference(x._reference), _object(x._object)
{
}

CIMObjectWithPath::~CIMObjectWithPath()
{
}

CIMObjectWithPath& CIMObjectWithPath::operator=(const CIMObjectWithPath& x)
{
    if (this != &x)
    {
	_reference = x._reference;
	_object = x._object;
    }
    return *this;
}

void CIMObjectWithPath::set(
    const CIMObjectPath& reference,
    const CIMObject& object)
{
    _reference = reference;
    _object = object;
}

const CIMObjectPath& CIMObjectWithPath::getReference() const
{
    return _reference;
}

const CIMObject& CIMObjectWithPath::getObject() const
{
    return _object;
}

CIMObjectPath& CIMObjectWithPath::getReference()
{
    return _reference;
}

CIMObject& CIMObjectWithPath::getObject()
{
    return _object;
}

PEGASUS_NAMESPACE_END
