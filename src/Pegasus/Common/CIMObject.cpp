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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Dave Sudlik, IBM (dsudlik@us.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
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

#define PEGASUS_ARRAY_T CIMObject
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

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

CIMObject::~CIMObject()
{
    Dec(_rep);
}

const CIMName& CIMObject::getClassName() const
{
    _checkRep();
    return _rep->getClassName();
}

const CIMObjectPath& CIMObject::getPath() const
{
    _checkRep();
    return _rep->getPath();
}

void CIMObject::setPath (const CIMObjectPath & path)
{
    _checkRep ();
    _rep->setPath (path);
}

CIMObject& CIMObject::addQualifier(const CIMQualifier& qualifier)
{
    _checkRep();
    _rep->addQualifier(qualifier);
    return *this;
}

Uint32 CIMObject::findQualifier(const CIMName& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

CIMQualifier CIMObject::getQualifier(Uint32 index)
{
    _checkRep();
    return _rep->getQualifier(index);
}

CIMConstQualifier CIMObject::getQualifier(Uint32 index) const
{
    _checkRep();
    return _rep->getQualifier(index);
}

void CIMObject::removeQualifier(Uint32 index)
{
    _checkRep();
    _rep->removeQualifier(index);
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

Uint32 CIMObject::findProperty(const CIMName& name) const
{
    _checkRep();
    return _rep->findProperty(name);
}

CIMProperty CIMObject::getProperty(Uint32 index)
{
    _checkRep();
    return _rep->getProperty(index);
}

CIMConstProperty CIMObject::getProperty(Uint32 index) const
{
    _checkRep();
    return _rep->getProperty(index);
}

void CIMObject::removeProperty(Uint32 index)
{
    _checkRep();
    _rep->removeProperty(index);
}

Uint32 CIMObject::getPropertyCount() const
{
    _checkRep();
    return _rep->getPropertyCount();
}

Boolean CIMObject::isUninitialized() const
{
    return (_rep == 0)? true : false;
}

String CIMObject::toString () const
{
    Array<char> out;
    
    _checkRep();
    _rep->toXml(out);
    out.append('\0');

    return out.getData();
}

Boolean CIMObject::isClass () const
{
    try
    {
        const CIMClass c (*this);
        return true;
    }
    catch (DynamicCastFailedException&)
    {
        return false;
    }
}

Boolean CIMObject::isInstance () const
{
    try
    {
        const CIMInstance i (*this);
        return true;
    }
    catch (DynamicCastFailedException&)
    {
        return false;
    }
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
        throw UninitializedObjectException();
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

CIMConstObject::~CIMConstObject()
{
    Dec(_rep);
}

const CIMName& CIMConstObject::getClassName() const
{
    _checkRep();
    return _rep->getClassName();
}

const CIMObjectPath& CIMConstObject::getPath() const
{
    _checkRep();
    return _rep->getPath();
}

Uint32 CIMConstObject::findQualifier(const CIMName& name) const
{
    _checkRep();
    return _rep->findQualifier(name);
}

CIMConstQualifier CIMConstObject::getQualifier(Uint32 index) const
{
    _checkRep();
    return _rep->getQualifier(index);
}

Uint32 CIMConstObject::getQualifierCount() const
{
    _checkRep();
    return _rep->getQualifierCount();
}

Uint32 CIMConstObject::findProperty(const CIMName& name) const
{
    _checkRep();
    return _rep->findProperty(name);
}

CIMConstProperty CIMConstObject::getProperty(Uint32 index) const
{
    _checkRep();
    return _rep->getProperty(index);
}

Uint32 CIMConstObject::getPropertyCount() const
{
    _checkRep();
    return _rep->getPropertyCount();
}

Boolean CIMConstObject::isUninitialized() const
{
    return (_rep == 0)? true : false;
}

String CIMConstObject::toString () const
{
    Array<char> out;
    
    _checkRep();
    _rep->toXml(out);
    out.append('\0');

    return out.getData();
}

Boolean CIMConstObject::isClass() const
{
    try
    {
        const CIMConstClass c(*this);
        return true;
    }
    catch (DynamicCastFailedException&)
    {
        return false;
    }
}

Boolean CIMConstObject::isInstance() const
{
    try
    {
        const CIMConstInstance i(*this);
        return true;
    }
    catch (DynamicCastFailedException&)
    {
        return false;
    }
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
        throw UninitializedObjectException();
}

PEGASUS_NAMESPACE_END
