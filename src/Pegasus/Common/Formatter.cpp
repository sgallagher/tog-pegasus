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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdio>
#include "Formatter.h"

PEGASUS_NAMESPACE_BEGIN

const Formatter::Arg Formatter::DEFAULT_ARG;

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
            sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "d", _lInteger);
            return buffer;
        }

        case ULINTEGER:
        {
            char buffer[32];
            sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", _lUInteger);
            return buffer;
        }

	case STRING:
	    return _string;

	case VOIDT:
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

    for (Uint32 i = 0; i < formatString.size(); i++)
    {
	if (formatString[i] == '$')
	{
	    Char16 c = formatString[++i];

	    switch (c)
	    {
		case '0': result.append(arg0.toString()); break;
		case '1': result.append(arg1.toString()); break;
		case '2': result.append(arg2.toString()); break;
		case '3': result.append(arg3.toString()); break;
		case '4': result.append(arg4.toString()); break;
		case '5': result.append(arg5.toString()); break;
		case '6': result.append(arg6.toString()); break;
		case '7': result.append(arg7.toString()); break;
		case '8': result.append(arg8.toString()); break;
		case '9': result.append(arg9.toString()); break;
		default: break;
	    }
	}
	else if (formatString[i] == '\\')
	{
	    result.append(formatString[++i]);
	}
	else
	    result.append(formatString[i]);
    }

    return result;
}

PEGASUS_NAMESPACE_END
