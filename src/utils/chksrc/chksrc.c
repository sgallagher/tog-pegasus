/*
**%2006=========================================================================
**
** Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
** Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
** Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
** IBM Corp.; EMC Corporation, The Open Group.
** Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
** IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
** Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
** EMC Corporation; VERITAS Software Corporation; The Open Group.
** Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
** EMC Corporation; Symantec Corporation; The Open Group.
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to
** deal in the Software without restriction, including without limitation the
** rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
** sell copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
** 
** THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
** ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
** "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
** LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
** PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
** HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
** ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**==============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* arg0;

void chksrc(const char* path)
{
    char buf[4096];
    FILE* is = fopen(path, "rb");
    int reject = 0;
    int line = 1;

    if (!is)
    {
        fprintf(stderr, "%s: failed to open: %s\n", arg0, path);
        exit(1);
    }

    for (; fgets(buf, sizeof(buf), is) != NULL; line++)
    {
        /* Look for NOCHKSRC tag */

        if (strstr(buf, "NOCHKSRC"))
            break;

        /* Check for tabs */

        if (strchr(buf, '\t'))
        {
            fprintf(stderr, "%s:%d: illegal tab character\n", path, line);
            reject = 1;
        }

        /* Check for Ctrl-M characters */

#if !defined(PEGASUS_PLATFORM_WIN64_IA64_MSVC) && \
    !defined(PEGASUS_PLATFORM_WIN64_X86_64_MSVC) && \
    !defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)

        if (strchr(buf, '\r'))
        {
            fprintf(stderr, "%s:%d: illegal carriage return character\n", 
                path, line);
            reject = 1;
        }

#endif /* PEGASUS_OS_TYPE_WINDOWS */

        /* Check for lines longer than 80 characters */

        if (strlen(buf) > 81)
        {
            fprintf(stderr, "%s:%d: line longer than 80 characters\n", 
                path, line);
            reject = 1;
        }
    }

    fclose(is);

    if (reject)
    {
        fprintf(stderr, "Rejected source file\n");
        exit(1);
    }
}

int isSourceFile(const char* path)
{
    const char* suffixes[] =
    {
        ".h",
        ".c",
        ".cpp",
    };
    size_t num_suffixes = sizeof(suffixes) / sizeof(suffixes[0]);
    size_t n = strlen(path);
    size_t i;

    for (i = 0; i < num_suffixes; i++)
    {
        size_t m = strlen(suffixes[i]);

        if (n >= m && strcmp(path + n - m, suffixes[i]) == 0)
        {
            return 1;
        }
    }

    /* Not a source file. */
    return 0;
}

int main(int argc, char** argv)
{
    int i;

    arg0 = argv[0];

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s source-files...\n", argv[0]);
        exit(1);
    }

    for (i = 1; i < argc; i++)
    {
        if (isSourceFile(argv[i]))
            chksrc(argv[i]);
    }

    return 0;
}
