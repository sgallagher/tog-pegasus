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
/* Check one file
*/
void chksrc(const char* path,
            int checktab,
            int checklen,
            int checkbadcr,
            int summarize)
{
    char buf[4096];
    FILE* is = fopen(path, "rb");

    int reject = 0;
    int line = 1;
    int notest = 0;
    int tabcount = 0;
    int longlinecount = 0;
    int badcrcount = 0;

    if (!is)
    {
        fprintf(stderr, "%s: failed to open: %s\n", arg0, path);
        exit(1);
    }

    for (; fgets(buf, sizeof(buf), is) != NULL; line++)
    {
        /* Look for NOCHKSRC tag */

        if (strstr(buf, "NOCHKSRC"))
        {
            notest = 1;
        }

        if (strstr(buf, "DOCHKSRC"))
        {
            notest = 0;
        }

        if (notest == 1)
        {
            continue;
        }

        /* Check for tabs */

        if ( checktab && (strchr(buf, '\t')))
        {
            if (!summarize)
            {
                fprintf(stderr, "%s:%d: illegal tab character\n", path, line);
            }
            reject = 1;
            tabcount++;
        }

        /* Check for Ctrl-M characters in non-windows platforms */

#if !defined(PEGASUS_PLATFORM_WIN64_IA64_MSVC) && \
    !defined(PEGASUS_PLATFORM_WIN64_X86_64_MSVC) && \
    !defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)

        if (checkbadcr && (strchr(buf, '\r')))
        {
            if (!summarize)
            {
                fprintf(stderr, "%s:%d: illegal carriage return character\n", 
                    path, line);
            }
            reject = 1;
            badcrcount++;
        }

#endif /* PEGASUS_OS_TYPE_WINDOWS */

        /* Check for lines longer than 80 characters
           Note: This is actually testing for 81 characters because
           it includes the EOL in the test. 
           ISSUE: We must confirm that this works for windows.
        */

        if ( checklen && (strlen(buf) > 81))
        {
            if (!summarize)
            {
                fprintf(stderr, "%s:%d: line longer than 80 characters\n", 
                    path, line);
            }
            reject = 1;
            longlinecount++;
        }
    }

    fclose(is);

    if (reject)
    {
        if (summarize)
        {
            fprintf(stderr,
                    "Rejected source file %s tabs = %u, long lines = %u",
                    path, tabcount, longlinecount);
#if !defined(PEGASUS_PLATFORM_WIN64_IA64_MSVC) && \
    !defined(PEGASUS_PLATFORM_WIN64_X86_64_MSVC) && \
    !defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
            if (badcrcount != 0)
            {
                fprintf(stderr, " Bad CRs = %u",badcrcount);
            }
#endif /* PEGASUS_OS_TYPE_WINDOWS */
            fprintf(stderr,"\n");
        }
        else
        {
            fprintf(stderr, "Rejected source file %s\n", path);
            exit(1);
        }
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


void usage()
{
    printf("Usage: %s [options] source-files...\n", arg0);
    printf("    Checks file list for tabs characters and lines");
    printf(" longer than 80 characters");
    /* show the following if not windows */
#if !defined(PEGASUS_PLATFORM_WIN64_IA64_MSVC) && \
    !defined(PEGASUS_PLATFORM_WIN64_X86_64_MSVC) && \
    !defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
    printf("\n    and windows Ctrl-M returns in non-windows platforms");
#endif
    printf(".\n");
    printf("    Disable with keyword NOCHKSRC in source code.\n");
    printf("    Reenable with keyword DOCHKSRC in source code.\n");
    printf("Options:\n");

    printf("    -t : Test only for tabs\n");
    printf("    -l : Test only for length\n");
    printf("    -m : Test only Ctrl-M (useful only on linux platforms)\n");
    printf("    -s : Generate only a summary file path list\n");
    printf("    -h : help\n");
}

int main(int argc, char** argv)
{
    int i;

    int checktab = 0;
    int checklen = 0;
    int summarize = 0;
    int checkbadcr = 0;

    char c;

    arg0 = argv[0];

    if (argc < 2)
    {
        usage();
        exit(1);
    }
    /* Get options from first parameter */
    if( argc > 1 && argv[1][0] == '-' )
    {
        for( i=1; (c=argv[1][i]) != '\0'; i++ )
        {
            if( c =='t' )
                checktab++;
            else if( c =='l' )
                checklen++;
            else if( c =='m' )
                checkbadcr++;
            else if( c =='s' )
                summarize++;
            else if( c =='h' )
                {usage(); exit(1);}
            else
               printf("Error Option %c?\n", c);
        }
           --argc;
           ++argv;
    }
    /* default is to test all  if none optioned*/
    if (checktab == 0 && checklen == 0)
    {
        checktab = 1;
        checklen = 1;
        checkbadcr = 1;
    }

    /* retest after argument removal */
    if (argc < 2)
    {
        usage();
        exit(1);
    }
    for (i = 1; i < argc; i++)
    {
        if (isSourceFile(argv[i]))
            chksrc(argv[i], checktab, checklen, checkbadcr,summarize);
    }

    return 0;
}
