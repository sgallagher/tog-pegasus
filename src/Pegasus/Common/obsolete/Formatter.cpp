//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

#include <cctype>
#include <cstdlib>
#include <cstdio>
#include "Formatter.h"
#include "Exception.h"

PEGASUS_NAMESPACE_BEGIN

void ArrIns::insert(Array<Sint8>& out) const
{
    out.append(_arr.getData(), _arr.getSize());
}

void IntIns::insert(Array<Sint8>& out) const
{
    char buffer[32];
    sprintf(buffer, "%d", _x);
    out.append(buffer, strlen(buffer));
}

void StringIns::insert(Array<Sint8>& out) const
{
    CString tmp = _x.getCString();
    out.append(tmp, strlen(tmp));
}

void Formatter::format(
    Array<Sint8>& out,
    const char* format,
    const Inserter& i0,
    const Inserter& i1,
    const Inserter& i2,
    const Inserter& i3,
    const Inserter& i4,
    const Inserter& i5,
    const Inserter& i6,
    const Inserter& i7,
    const Inserter& i8,
    const Inserter& i9)
{
    const Inserter* ins[] =
    {
	&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9
    };

    _format(out, format, ins, sizeof(ins) / sizeof(ins[0]));
}

void Formatter::_format(
    Array<Sint8>& out,
    const char* format,
    const Inserter* ins[],
    Uint32 insSize)
{
    for (const char* p = format; *p; p++)
    {
	if (*p == '$')
	{
	    // Expect '{':

	    if (*++p != '{')
		throw BadFormat();

	    p++;

	    // Expect digits termianted by '}':

	    const char *q = p;

	    for (q = p; *q && *q != '}'; q++)
	    {
		if (!isdigit(*q))
		    throw BadFormat();
	    }

	    if (*q != '}' || p == q)
		throw BadFormat();

	    char buffer[32];
	    *buffer = '\0';
	    strncat(buffer, p, q - p);

	    Uint32 index = atoi(buffer);

	    if (index >= insSize)
		throw BadFormat();

	    ins[index]->insert(out);

	    p = q;

	    continue;
	}

	out.append(*p);
    }
}

PEGASUS_NAMESPACE_END
