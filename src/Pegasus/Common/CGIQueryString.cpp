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
// $Log: CGIQueryString.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:50:35  mike
// Pegasus import
//
//
//END_HISTORY

#include <cstdio>
#include <cstdlib>
#include "CGIQueryString.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CGIQueryString:
//
//     NameSpace=root%2Fcimv20&ClassName=&LocalOnly=true
//
////////////////////////////////////////////////////////////////////////////////

static void _ExpandCGIQueryValue(char* value)
{
    // Look for '%' characters followed by hex codes:

    for (char* p = value; *p; p++)
    {
	if (*p == '%')
	{
	    char hexString[3];
	    sprintf(hexString, "%*.*s", 2, 2, p + 1);
	    char* end = 0;
	    long tmp = strtol(hexString, &end, 16);

	    if (*end)
		throw BadlyFormedCGIQueryString();

	    *p++ = char(tmp);
	    memcpy(p, p + 2, strlen(p) - 2 + 1);
	}
	else if (*p == '+')
	    *p = ' ';
    }
}

static void _ParseCGIQueryString(
    char* queryString, 
    Array <CGIQueryString::Entry>& entries)
{
    // First split about the '&' characters:

    for (char* p = strtok(queryString, "&"); p; p = strtok(NULL, "&"))
    {
	char* name = p;

	// Find equal sign:

	char* equalChar = strchr(p, '=');

	if (!equalChar)
	    throw BadlyFormedCGIQueryString();

	*equalChar++ = '\0';

	char* value = equalChar;

	_ExpandCGIQueryValue(value);

	// cout << "name=[" << name << "]" << endl;
	// cout << "value=[" << value << "]" << endl;

	CGIQueryString::Entry entry = { name, value };
	entries.append(entry);
    }
}

CGIQueryString::CGIQueryString(char* queryString)
{
    _ParseCGIQueryString(queryString, _entries);
}

const char* CGIQueryString::findValue(const char* name) const
{
    for (Uint32 i = 0, n = _entries.getSize(); i < n; i++)
    {
	if (strcmp(_entries[i].name, name) == 0)
	    return _entries[i].value;
    }

    return 0;
}

PEGASUS_NAMESPACE_END
