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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

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

CIMObject::CIMObject(const CIMClass& x)
{
    Inc(_rep = x._rep);
}

CIMObject::CIMObject(const CIMInstance& x)
{
    Inc(_rep = x._rep);
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

Boolean CIMObject::identical(const CIMConstObject& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstObject
//
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
//
// CIMObjectWithPath
//
////////////////////////////////////////////////////////////////////////////////

CIMObjectWithPath::CIMObjectWithPath()
{

}

CIMObjectWithPath::CIMObjectWithPath(
    const CIMReference& reference,
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
    const CIMReference& reference,
    const CIMObject& object)
{
    _reference = reference;
    _object = object;
}

void CIMObjectWithPath::toXml(Array<Sint8>& out) const
{
    out << "<VALUE.OBJECTWITHPATH>\n";

    _reference.toXml(out, false);
    _object.toXml(out);

    out << "</VALUE.OBJECTWITHPATH>\n";
}

PEGASUS_NAMESPACE_END
