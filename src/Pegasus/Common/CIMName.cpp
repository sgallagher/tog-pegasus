//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: CIMName.cpp,v $
// Revision 1.2  2001/04/04 20:02:27  karl
// documentation update
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.1.1.1  2001/01/14 19:52:58  mike
// Pegasus import
//
//
//END_HISTORY

#include <cctype>
#include "CIMName.h"
#include "Destroyer.h"

PEGASUS_NAMESPACE_BEGIN

Boolean CIMName::legal(const Char16* name)
{
    const Char16* p = name;

    if (!p)
	throw NullPointer();

    if (!*p || !(isalpha(*p) || *p == '_'))
	return false;

    for (p++; *p; p++)
    {
	if (*p > 127)
	    return false;

	if (!(isalnum(*p) || *p == '_'))
	    return false;
    }

    return true;
}

Boolean CIMName::equal(const String& name1, const String& name2)
{
    if (name1.getLength() != name2.getLength())
	return false;

    const Char16* p = name1.getData();
    const Char16* q = name2.getData();

    Uint32 n = name1.getLength();

    while (n--)
    {
	if (*p <= 127 && *q <= 127)
	{
	    if (tolower(*p++) != tolower(*q++))
		return false;
	}
	else if (*p++ != *q++)
	    return false;
    }

    return true;
}

PEGASUS_NAMESPACE_END
