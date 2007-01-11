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

#include "Options.h"
#include <string.h>

/*
**==============================================================================
**
** _TestFlagOption()
**
**     Check whether argv contains the given option. Return 0 if so. Else
**     return -1. Remove the argument from the list if the *remove* argument
**     is non-zero.
**
**         if (_TestFlagOption(&argc, &argv, "--help", 0) == 0)
**         {
**         }
**
**==============================================================================
*/

static int _TestFlagOption(
    int* argc_, char*** argv_, const char* option, int remove)
{
    int argc = *argc_;
    char** argv = *argv_;
    int i;

    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], option) == 0)
        {
            if (remove)
            {
                memmove(&argv[i], &argv[i + 1], (argc-i) * sizeof(char*));
                argc--;
            }

            *argc_ = argc;
            *argv_ = argv;
            return 0;
        }
    }

    /* Not found */
    return -1;
}

/*
**==============================================================================
**
** GetOptions()
**
**     Get all "minus" options from the command line. Place corresponding flags
**     into Options structure.
**
**==============================================================================
*/

void GetOptions(int* argc, char*** argv, struct Options* options)
{
    memset(options, 0, sizeof(struct Options));

    if (_TestFlagOption(argc, argv, "--policy", 1) == 0)
        options->policy = 1;

    if (_TestFlagOption(argc, argv, "--macros", 1) == 0)
        options->macros = 1;

    if (_TestFlagOption(argc, argv, "--version", 0) == 0)
        options->version = 1;

    if (_TestFlagOption(argc, argv, "-v", 0) == 0)
        options->version = 1;

    if (_TestFlagOption(argc, argv, "--help", 0) == 0)
        options->help = 1;

    if (_TestFlagOption(argc, argv, "-h", 0) == 0)
        options->help = 1;

    if (_TestFlagOption(argc, argv, "-s", 0) == 0)
        options->shutdown = 1;

    if (_TestFlagOption(argc, argv, "--perror", 1) == 0)
        options->perror = 1;
}
