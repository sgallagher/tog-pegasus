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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include "CGIQueryString.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CGIQueryString:
//   This file provides the methods for the CIGQuery String class that
//   parse a CGI query string of the following form:
//
//     NameSpace=root%2Fcimv2&ClassName=&LocalOnly=true
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

#ifdef PEGASUS_HAS_EBCDIC
            *p = char(tmp);
            __atoe_l(p,1);
            p++;
#else
	    *p++ = char(tmp);
#endif
	    memcpy(p, p + 2, strlen(p) - 2 + 1);
	    // CORRECTION, KS. Add decrement. corrects problem with succesive
	    //             % sequences
	    p--;

	}
	else if (*p == '+')
	    *p = ' ';
    }
}

void CGIQueryString::_ParseCGIQueryString(
    char* queryString, 
    Array<CGIQueryStringEntry>& entries)
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

	CGIQueryStringEntry entry = { name, value };
	entries.append(entry);
    }
}

CGIQueryString::CGIQueryString(char* queryString)
{
    _ParseCGIQueryString(queryString, _entries);
}

const char* CGIQueryString::findValue(const char* name) const
{
    for (Uint32 i = 0, n = _entries.size(); i < n; i++)
    {
	if (strcmp(_entries[i].name, name) == 0)
	    return _entries[i].value;
    }

    return 0;
}

PEGASUS_NAMESPACE_END
