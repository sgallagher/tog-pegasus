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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

/*!
    \file getopts.h
    \brief defines getopts function
*/

#ifndef _REMOTE_CMPI_GETOPTS_H
#define _REMOTE_CMPI_GETOPTS_H

#include <string.h>

/*
    Retrives the options like UNIX getopts command
    opts (in) - option list.
    n    (in) - Should be initialized to zero when calling this function for 
                the first time. This value should not be changed elsewhere in
                the program until getopts function finishes parsing the 
                arguements. Using this value getopts knows the next argument 
                to be parsed.
    optsarg (out) - A pointer to the argument of the option is stored in this.
    argc (in) - number of arguments.
    argv (in) - Actual arguments list.
*/

char* getopts(char *opts,int *n,char **optsarg,int argc, char *argv[])
{
    int i;
    char *arg,*tmp;

    if (*n + 1 >= argc)
    {
        return 0;
    }

    ++*n;
    arg = argv[*n];
    *optsarg = "Unknown option.";
    if (*arg++ == '-')
    {
        while ((tmp = strchr(opts,':')))
        {
            i = tmp-opts;
            if (!strncmp(arg,opts,i))
            {
                arg += i;
                if (*arg)
                {
                    *optsarg = arg;
                }
                else if (*n + 1 <argc)
                {
                    *optsarg = argv[++*n];
                }
                else
                {
                    *optsarg = "Option requires value.";
                    break;
                }
                return(char*)opts;
            }
            else
            {
                opts += i+1;
            }
        }
    }

    return "";   /* return an empty string, an error has occured */
}

#endif
