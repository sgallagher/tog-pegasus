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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMPropertyList.h"

PEGASUS_NAMESPACE_BEGIN

class CIMPropertyListRep
{
public:
    Array<String> propertyNames;
    Boolean isNull;
};


CIMPropertyList::CIMPropertyList()
{
    _rep = new CIMPropertyListRep();
    _rep->isNull = true;
}

CIMPropertyList::CIMPropertyList(const CIMPropertyList& x)
{
    _rep = new CIMPropertyListRep();
    _rep->propertyNames = x._rep->propertyNames;
    _rep->isNull = x._rep->isNull;
}

CIMPropertyList::CIMPropertyList(const Array<String>& propertyNames)
{
    _rep = new CIMPropertyListRep();
    _rep->propertyNames = propertyNames;
    _rep->isNull = false;
}

CIMPropertyList::~CIMPropertyList()
{
    delete _rep;
}

void CIMPropertyList::set(const Array<String>& propertyNames)
{
    _rep->propertyNames = propertyNames;
    _rep->isNull = false;
}

CIMPropertyList& CIMPropertyList::operator=(const CIMPropertyList& x)
{
    if (&x != this)
    {
	_rep->propertyNames = x._rep->propertyNames;
	_rep->isNull = x._rep->isNull;
    }

    return *this;
}

void CIMPropertyList::clear()
{
    _rep->propertyNames.clear();
    _rep->isNull = true;
}

Boolean CIMPropertyList::isNull() const
{
    return _rep->isNull;
}

Uint32 CIMPropertyList::getNumProperties() const
{
    return _rep->propertyNames.size();
}

const String& CIMPropertyList::getPropertyName(Uint32 pos) const
{
    return _rep->propertyNames[pos];
}

const Array<String>& CIMPropertyList::getPropertyNameArray() const
{
    return _rep->propertyNames;
}

CIMPropertyList CIMPropertyList::clone() const
{
    return CIMPropertyList(*this);
}

PEGASUS_NAMESPACE_END
