//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdio>
#include "Formatter.h"

PEGASUS_NAMESPACE_BEGIN

String Formatter::Arg::toString() const
{
    switch (_type)
    {
	case INTEGER:
	{
	    char buffer[32];
	    sprintf(buffer, "%d", _integer);
	    return buffer;
	}

	case UINTEGER:
	{
	    char buffer[32];
	    sprintf(buffer, "%u", _integer);
	    return buffer;
	}

	case BOOLEAN:
	{
	    //char buffer[32];
	    //buffer = (_boolean ? "true": "false");
	    //return buffer;
	    return  (_boolean ? "true": "false");
	}

	case REAL:
	{
	    char buffer[32];
	    sprintf(buffer, "%f", _real);
	    return buffer;
	}

	case LINTEGER:
	{
	    char buffer[32];
	    // ATTN-B: truncation here!
	    sprintf(buffer, "%ld", long(_lInteger));
	    return buffer;
	}
	
	case ULINTEGER:
	{
	    char buffer[32];
	    // ATTN-B: truncation here:
	    sprintf(buffer, "%lu", long(_lUInteger));
	    return buffer;
	}

	case STRING:
	    return _string;

	case VOID:
	default:
	    return String();
    }
}

String Formatter::format(
    const String& formatString,
    const Arg& arg0,
    const Arg& arg1,
    const Arg& arg2,
    const Arg& arg3,
    const Arg& arg4,
    const Arg& arg5,
    const Arg& arg6,
    const Arg& arg7,
    const Arg& arg8,
    const Arg& arg9)
{
    String result;
    const Char16* first = formatString.getData();
    const Char16* last = formatString.getData() + formatString.size();

    for (; first != last; first++)
    {
	if (*first == '$')
	{
	    Char16 c = *++first;

	    switch (c)
	    {
		case '0': result += arg0.toString(); break;
		case '1': result += arg1.toString(); break;
		case '2': result += arg2.toString(); break;
		case '3': result += arg3.toString(); break;
		case '4': result += arg4.toString(); break;
		case '5': result += arg5.toString(); break;
		case '6': result += arg6.toString(); break;
		case '7': result += arg7.toString(); break;
		case '8': result += arg8.toString(); break;
		case '9': result += arg9.toString(); break;
		default: break;
	    }
	}
	else if (*first == '\\')
	{
	    first++;
	    result += *first;
	}
	else
	    result += *first;
    }

    return result;
}

PEGASUS_NAMESPACE_END
