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

#include "CIMObjectRep.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMObjectRep::CIMObjectRep(const CIMReference& reference)
    : _reference(reference), _resolved(false)
{
}

CIMObjectRep::~CIMObjectRep()
{

}

void CIMObjectRep::addProperty(const CIMProperty& x)
{
    if (!x)
	throw UnitializedHandle();

    // Reject duplicate property names:

    if (findProperty(x.getName()) != PEG_NOT_FOUND)
	throw AlreadyExists();

    // Append property:

    _properties.append(x);
}

Uint32 CIMObjectRep::findProperty(const String& name)
{
    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
    {
	if (CIMName::equal(_properties[i].getName(), name))
	    return i;
    }

    return PEG_NOT_FOUND;
}

Boolean CIMObjectRep::existsProperty(const String& name)
{
    return (findProperty(name) == PEG_NOT_FOUND) ? false : true;
}

CIMProperty CIMObjectRep::getProperty(Uint32 pos)
{
    if (pos >= _properties.size())
	throw OutOfBounds();

    return _properties[pos];
}

void CIMObjectRep::removeProperty(Uint32 pos)
    {
	if (pos >= _properties.size())
	    throw OutOfBounds();

	_properties.remove(pos);
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

    _properties.reserve(x._properties.size());

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

PEGASUS_NAMESPACE_END
