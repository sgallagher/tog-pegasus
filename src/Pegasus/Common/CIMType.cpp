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

#include "CIMType.h"

PEGASUS_NAMESPACE_BEGIN

static const char* _typeStrings[] =
{
    "none", "boolean", "uint8", "sint8", "uint16", "sint16", "uint32", "sint32",
    "uint64", "sint64", "real32", "real64", "char16", "string", "datetime",
    "reference"
};

static const Uint32 _NUM_TYPES = sizeof(_typeStrings) / sizeof(_typeStrings[0]);

CIMType::CIMType() : _tag(NONE)
{
}

CIMType::CIMType(Tag tag) : _tag(tag)
{
}

#if 0
CIMType::CIMType(Uint32 tag) : _tag(Tag(tag))
{
}
#endif

CIMType::CIMType(const CIMType& x) : _tag(x._tag)
{
}

CIMType& CIMType::operator=(Tag tag)
{
    _tag = tag;
    return *this;
}

CIMType::operator Tag() const
{
    return _tag;
}

Boolean CIMType::equal(const CIMType& x) const
{
    return x._tag == _tag;
}

const char* CIMType::toString() const
{
    return _typeStrings[Uint32(_tag)];
}

PEGASUS_NAMESPACE_END
