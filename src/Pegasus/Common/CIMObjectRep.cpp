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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMObjectRep.h"
#include "CIMName.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMObjectRep::CIMObjectRep(const CIMObjectPath& reference)
    : _reference(reference), _resolved(false)
{
}

CIMObjectRep::~CIMObjectRep()
{

}

void CIMObjectRep::addProperty(const CIMProperty& x)
{
    if (x.isUninitialized())
	throw UninitializedObjectException();

    // Reject duplicate property names:

    if (findProperty(x.getName()) != PEG_NOT_FOUND)
        throw AlreadyExistsException("property \"" + x.getName() + "\"");

    // Append property:

    _properties.append(x);
}

Uint32 CIMObjectRep::findProperty(const CIMName& name) const
{
    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
    {
	if (name.equal(_properties[i].getName()))
	    return i;
    }

    return PEG_NOT_FOUND;
}

CIMProperty CIMObjectRep::getProperty(Uint32 index)
{
    if (index >= _properties.size())
	throw IndexOutOfBoundsException();

    return _properties[index];
}

void CIMObjectRep::removeProperty(Uint32 index)
    {
	if (index >= _properties.size())
	    throw IndexOutOfBoundsException();

	_properties.remove(index);
    }


Uint32 CIMObjectRep::getPropertyCount() const
{
    return _properties.size();
}


CIMObjectRep::CIMObjectRep()
{

}

CIMObjectRep::CIMObjectRep(const CIMObjectRep& x) : 
    Sharable(),
    _reference(x._reference),
    _resolved(x._resolved)
{
    x._qualifiers.cloneTo(_qualifiers);

    _properties.reserveCapacity(x._properties.size());

    for (Uint32 i = 0, n = x._properties.size(); i < n; i++)
	_properties.append(x._properties[i].clone());
}

Boolean CIMObjectRep::identical(const CIMObjectRep* x) const
{
    if (!_reference.identical(x->_reference))
	return false;

    if (!_qualifiers.identical(x->_qualifiers))
	return false;

    // Compare properties:

    {
	const Array<CIMProperty>& tmp1 = _properties;
	const Array<CIMProperty>& tmp2 = x->_properties;

	if (tmp1.size() != tmp2.size())
	    return false;

	for (Uint32 i = 0, n = tmp1.size(); i < n; i++)
	{
	    if (!tmp1[i].identical(tmp2[i]))
		return false;
	}
    }

    if (_resolved != x->_resolved)
	return false;

    return true;
}

void CIMObjectRep::setPath (const CIMObjectPath& path)
{
    _reference = path;
}

PEGASUS_NAMESPACE_END
