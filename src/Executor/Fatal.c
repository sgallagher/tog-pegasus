/*
//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////
*/
#include <stdarg.h>
#include <syslog.h>
#include <stdio.h>
#include "Fatal.h"
#include "Log.h"
#include "Exit.h"
#include "Globals.h"
#include "Strlcpy.h"
#include "Strlcat.h"

/*
**==============================================================================
**
** Fatal()
**
**     Report fatal errors. The callar set fatal_file and fatal_line before
**     calling this function. Note that since this is a single threaded
**     application, there is no attempt to synchronize access to these
**     globals.
**
**==============================================================================
*/

void Fatal(const char* file, size_t line, const char* format, ...)
{
    char prefixedFormat[EXECUTOR_BUFFER_SIZE];
    char lineStr[32];

    /* Prepend "__FILE__(__LINE__): FATAL: " to format. */

    Strlcpy(prefixedFormat, file, sizeof(prefixedFormat));
    Strlcat(prefixedFormat, "(", sizeof(prefixedFormat));
    sprintf(lineStr, "%u", (unsigned int)line);
    Strlcat(prefixedFormat, lineStr, sizeof(prefixedFormat));
    Strlcat(prefixedFormat, "): FATAL: ", sizeof(prefixedFormat));
    Strlcat(prefixedFormat,  format, sizeof(prefixedFormat));

    /* Print to syslog. */
    {
        va_list ap;
        char buffer[EXECUTOR_BUFFER_SIZE];

        va_start(ap, format);
        /* Flawfinder: ignore */
        vsprintf(buffer, prefixedFormat, ap);
        va_end(ap);

        syslog(LOG_CRIT, "%s", buffer);
    }

    /* Print to stderr. */
    {
        va_list ap;

        fprintf(stderr, "%s: ", globals.argv[0]);
        va_start(ap, format);
        /* Flawfinder: ignore */
        vfprintf(stderr, prefixedFormat, ap);
        va_end(ap);
        fputc('\n', stderr);
    }

    Exit(1);
}
