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
// Author: Tony Fiorentino (fiorentino_tony@emc.com)
//
// Modified By: Keith Petley (keithp@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Attribute.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

Attribute::Attribute(const String& attrEntry)
{
    _tag = attrEntry;
}

Attribute::~Attribute()
{
    if (_vals.size())
    {
        _vals.clear();	
    }
}

const Array<String>& Attribute::getValues() const
{
    return _vals;
}

const String& Attribute::getTag() const
{
    return _tag;
}

void Attribute::addValue(const String& value)
{
    _vals.append(value);
}

PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os,
    const Attribute& attr)
{
    os << attr.getTag();
    Array<String> vals = attr.getValues();
    for (int i = 0; i < vals.size(); i++)
    {
        if(i == 0)
        {
            os << " = ";
        }
        else
        {
            os << " | ";
        }
        os << vals[i];
    }
    return os;
}

#define PEGASUS_ARRAY_T Attribute
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END
