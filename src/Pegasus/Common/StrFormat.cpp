//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Michael E. Brasher
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <cctype>
#include <cstdarg>
#include <cassert>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>
#include "StrFormat.h"

#define MAX_ARGS 100

PEGASUS_NAMESPACE_BEGIN

struct Spec
{
    char used;
    char tag;

    union 
    {
	Boolean arg_o;
	Sint8 arg_b;
	Sint8 arg_B;
	Sint16 arg_s;
	Sint16 arg_S;
	Sint32 arg_i;
	Sint32 arg_I;
	Sint64 arg_l;
	Sint64 arg_L;
	Real32 arg_f;
	Real64 arg_d;
	char arg_c;
	Char16* arg_C;
	char* arg_z;
	String* arg_Z;
	CIMDateTime* arg_t;
    };
};

static void _PrintSpec(
    char*& str, 
    size_t& str_size, 
    size_t& str_cap, 
    const Spec& spec)
{
    /* Make room for 64 bytes up front */

    if (str_size + 64 >= str_cap)
    {
	str_cap += 64;
	str = (char*)realloc(str, str_cap);
    }

    /* Insert new argument */

    switch(spec.tag)
    {
	case 'o':
	{
	    if (spec.arg_o)
	    {
		strcat(str, "true");
		str_size += 4;
	    }
	    else
	    {
		strcat(str, "false");
		str_size += 5;
	    }
	    break;
	}
	case 'b':
	{
	    size_t n = sprintf(str + str_size, "%d", spec.arg_b);
	    str_size += n;
	    break;
	}
	case 'B':
	{
	    size_t n = sprintf(str + str_size, "%u", spec.arg_B);
	    str_size += n;
	    break;
	}
	case 's':
	{
	    size_t n = sprintf(str + str_size, "%d", spec.arg_s);
	    str_size += n;
	    break;
	}
	case 'S':
	{
	    size_t n = sprintf(str + str_size, "%u", spec.arg_S);
	    str_size += n;
	    break;
	}
	case 'i':
	{
	    size_t n = sprintf(str + str_size, "%d", spec.arg_i);
	    str_size += n;
	    break;
	}
	case 'I':
	{
	    size_t n = sprintf(str + str_size, "%u", spec.arg_I);
	    str_size += n;
	    break;
	}
	case 'l':
	{
	    size_t n = sprintf(str + str_size, 
		"%" PEGASUS_64BIT_CONVERSION_WIDTH "u", spec.arg_l);
	    str_size += n;
	    break;
	}
	case 'L':
	{
	    size_t n = sprintf(str + str_size, 
		"%" PEGASUS_64BIT_CONVERSION_WIDTH "u", spec.arg_L);
	    str_size += n;
	    break;
	}
	case 'f':
	{
	    size_t n = sprintf(str + str_size, "%f", spec.arg_f);
	    str_size += n;
	    break;
	}
	case 'd':
	{
	    size_t n = sprintf(str + str_size, "%lf", spec.arg_d);
	    str_size += n;
	    break;
	}
	case 'c':
	{
	    size_t n = sprintf(str + str_size, "%c", spec.arg_c);
	    str_size += n;
	    break;
	}
	case 'C':
	{
	    size_t n = sprintf(str + str_size, "%04X", (Uint16)(*spec.arg_C));
	    str_size += n;
	    break;
	}
	case 'z':
	{
	    size_t len = strlen(spec.arg_z);

	    if (str_size + len >= str_cap)
	    {
		str_cap += len;
		str = (char*)realloc(str, str_cap);
	    }
	    strcat(str + str_size, spec.arg_z);
	    str_size += len;
	    break;
	}
	case 'Z':
	{
	    CString cstring = spec.arg_Z->getCString();
	    const char* c_str = cstring;
	    size_t len = strlen(c_str);

	    if (str_size + len >= str_cap)
	    {
		str_cap += len;
		str = (char*)realloc(str, str_cap);
	    }
	    strcat(str + str_size, c_str);
	    str_size += len;
	    break;
	}
	case 't':
	{
	    // ATTN: implement this!
	    assert("NOT IMPLEMENTED!" == 0);
	    break;
	}
	default:
	    /* Not reachable */
	    break;
    }
}

static char _valid_tags[] = "obBsSiIlLfFcCzZd";

int VaStrFormat(char*& str, const char* format, va_list ap)
{
    Spec specs[MAX_ARGS];
    size_t specs_size = 0;

    /* 
     * Clear out specs array 
     */

    memset(specs, 0, sizeof specs);

    /* 
     * Make string null for now in case an error occurs. 
     */

    str = NULL;

    /* 
     * Scan the format and find the highest valued argument number. 
     */

    for (const char* p = format; *p; )
    {
	if (*p == '%')
	{
	    p++;

	    if (isdigit(*p))
	    {
		/* Convert the argument number to integer */

		const char* start = p;
		char* end;
		unsigned long arg_num = strtoul(start, &end, 10);
		p = end;

		if (arg_num >= MAX_ARGS)
		    return -1;

		/* Get the tag character */

		char tag = *p++;

		if (strchr(_valid_tags, tag) == NULL)
		{
		    /* Invalid tag character */
		    return -1;
		}

		/* Create a spec entry */

		specs[arg_num].tag = tag;
		specs[arg_num].used = 1;

		if (specs_size <= arg_num)
		    specs_size = arg_num + 1;
	    }
	    else if (*p == '%')
	    {
		/* double percent */
		p++;
		continue;
	    }
	    else
		return -1;
	}
	else
	    p++;
    }

    /* 
     * Check for gaps in the specs (an error) 
     */

    for (size_t i = 0; i < specs_size; i++)
    {
	if (!specs[i].used)
	    return -1;
    }

    /* 
     * Iterate over the specs, getting all the arguments. 
     */

    for (size_t i = 0; i < specs_size; i++)
    {
	switch(specs[i].tag)
	{
	    case 'o':
	    {
		specs[i].arg_o = va_arg(ap, int);
		break;
	    }
	    case 'b':
	    {
		specs[i].arg_b = va_arg(ap, int);
		break;
	    }
	    case 'B':
	    {
		specs[i].arg_B = va_arg(ap, int);
		break;
	    }
	    case 's':
	    {
		specs[i].arg_s = va_arg(ap, int);
		break;
	    }
	    case 'S':
	    {
		specs[i].arg_S = va_arg(ap, int);
		break;
	    }
	    case 'i':
	    {
		specs[i].arg_i = va_arg(ap, Sint32);
		break;
	    }
	    case 'I':
	    {
		specs[i].arg_I = va_arg(ap, Uint32);
		break;
	    }
	    case 'l':
	    {
		specs[i].arg_l = va_arg(ap, Sint64);
		break;
	    }
	    case 'L':
	    {
		specs[i].arg_L = va_arg(ap, Uint64);
		break;
	    }
	    case 'f':
	    {
		specs[i].arg_f = va_arg(ap, double);
		break;
	    }
	    case 'd':
	    {
		specs[i].arg_d = va_arg(ap, double);
		break;
	    }
	    case 'c':
	    {
		specs[i].arg_c = va_arg(ap, int);
		break;
	    }
	    case 'C':
	    {
		specs[i].arg_C = va_arg(ap, Char16*);
		break;
	    }
	    case 'z':
	    {
		specs[i].arg_z = va_arg(ap, char*);
		break;
	    }
	    case 'Z':
	    {
		specs[i].arg_Z = va_arg(ap, String*);
		break;
	    }
	    case 't':
	    {
		specs[i].arg_t = va_arg(ap, CIMDateTime*);
		break;
	    }
	    default:
		return -1;
	}
    }

    va_end(ap);

    /*
     * At last! Build the string.
     */

    /* 
     * Guess at the required size (extened allocation later if necessary). 
     */

    const size_t GROW_BY = 64; 
    size_t str_cap = 64;
    size_t str_size = 0;

    if ((str = (char*)malloc(str_cap)) == NULL)
	return -1;

    *str = '\0';

    /* 
     * Scan the format and find the highest valued argument number. 
     */

    for (const char* p = format; *p; )
    {
	if (*p == '%')
	{
	    p++;

	    if (isdigit(*p))
	    {
		/* Get argument number */

		const char* start = p;
		char* end;
		unsigned long arg_num = strtoul(start, &end, 10);
		p = end;

		/* Skip over tag */

		p++;

		_PrintSpec(str, str_size, str_cap, specs[arg_num]);
	    }
	    else if (*p == '%')
	    {
		if (str_size + 1 >= str_cap)
		{
		    str_cap += GROW_BY;
		    str = (char*)realloc(str, str_cap);
		}
		str[str_size++] = '%';
		str[str_size] = '\0';
		p++;
	    }
	    else
	    {
		/* Not reachable (checked earlier). */
		return -1;
	    }
	}
	else
	{
	    if (str_size + 1 >= str_cap)
	    {
		str_cap += GROW_BY;
		str = (char*)realloc(str, str_cap);
	    }
	    str[str_size++] = *p++;
	    str[str_size] = '\0';
	}
    }

    return 0;
}

int StrFormat(char*& str, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    return VaStrFormat(str, format, ap);
}

PEGASUS_NAMESPACE_END
