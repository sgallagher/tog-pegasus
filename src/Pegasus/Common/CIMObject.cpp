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
#include "XmlWriter.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMObject
//
////////////////////////////////////////////////////////////////////////////////

CIMClass CIMObject::getClass()
{
    CIMClassRep* rep = dynamic_cast<CIMClassRep*>(_rep);

    if (!rep)
	throw TypeMismatch();

    return CIMClass(rep);
}

CIMConstClass CIMObject::getClass() const
{
    CIMClassRep* rep = dynamic_cast<CIMClassRep*>(_rep);

    if (!rep)
	throw TypeMismatch();

    return CIMConstClass(rep);
}

CIMInstance CIMObject::getInstance()
{
    CIMInstanceRep* rep = dynamic_cast<CIMInstanceRep*>(_rep);

    if (!rep)
	throw TypeMismatch();

    return CIMInstance(rep);
}

CIMConstInstance CIMObject::getInstance() const
{
    CIMInstanceRep* rep = dynamic_cast<CIMInstanceRep*>(_rep);

    if (!rep)
	throw TypeMismatch();

    return CIMConstInstance(rep);
}

void CIMObject::toXml(Array<Sint8>& out) const
{
    if (!_rep)
	throw NullPointer();

    if (isClass())
    {
	CIMConstClass cimClass = getClass();
	cimClass.toXml(out);
    }
    else
    {
	CIMConstInstance cimInstance = getInstance();
	cimInstance.toXml(out);
    }
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
    CIMReference& reference,
    CIMObject& object) 
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

void CIMObjectWithPath::set(CIMReference& reference, CIMObject& object)
{
    _reference = reference;
    _object = object;
}

//------------------------------------------------------------------------------
//
// <!ELEMENT VALUE.OBJECTWITHPATH ((CLASSPATH,CLASS)|(INSTANCEPATH,INSTANCE))>
//
//------------------------------------------------------------------------------

void CIMObjectWithPath::toXml(Array<Sint8>& out) const
{
    out << "<VALUE.OBJECTWITHPATH>\n";

    _reference.toXml(out, false);
    _object.toXml(out);

    out << "</VALUE.OBJECTWITHPATH>\n";
}

PEGASUS_NAMESPACE_END
