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
// Author: Venkateswara Rao Puvvada, IBM, vpuvvada@in.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

/*!
   \file AppendNs.c
   \brief Appends Namespaces property in provider registration MOF fies.

    This program helps in modifying existing registration MOF files, it appends additional
    namespace names to the Namespaces property.

    Usage:
         AppendeNs <additional-namespace-name> [ ... ] <mof-file-name>

    Where
        additional-namespace-name is the namespace name to be added to existing Namespaces
        specification.
	Multiple namespace names can be specified, separated by blanks.
	mof-file-name is the name of the mof registration file to used.
	Output of the script is routed to std out.

     Example:
        The following command adds additional namespace names to the existing Namespaces
        specification.

	AppendNs root/local root/node* Linux_baseR.mof >Linux_baseCMPIR.mof

        On Linux root/node* is used to define the provider supports all namespaces
        starting with root/node.

        But on windows it does not work because command shell does not expand wildcards.
*/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../Common/tokenizer.h"


int main(int argc,char *argv[])
{
    FILE *fp;
    char buff[BUFFLEN];
    char token[TOKENLEN];
    char *tmp;
    char *prev;
    int  i;

    if (3 > argc)
    {
        printf(
               "%s\n","Usage: AppendeNs <additional-namespace-name> [ ... ]"
               " <mof-file-name>");
        exit(1);
    }

    fp = fopen(argv[argc-1],"r");
    if (NULL == fp)
    {
        perror(argv[argc-1]);
        exit(1);
    }

    while (NULL != fgets(buff,BUFFLEN,fp))
    {
        tmp = buff;
        gettoken(&tmp,token);
        if (0 != strcmp("Namespaces",token) || 0 != strcmp("namespaces",token) )
        {
            fputs(buff,stdout);
            continue;
        }

        assert(EQUAL == gettoken(&tmp,token)); // simple check

        while (RB != gettoken(&tmp,token))
        {
           prev = tmp;  // we completly rely on correctness of registration MOF file.
        }

        *prev = '\0';
        /* Append new namespaces */
        for (i = 1; i < argc -1 ;++i)
        {
            strcat(buff,", \"");
            strcat(buff,argv[i]);
            strcat(buff,"\"");
        }
        strcat(buff," };\n");
        fputs(buff,stdout);
    }

    return 0;
}

