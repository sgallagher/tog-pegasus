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
   \file tokenizer.h
   \brief Defines gettoken function.

*/

#ifndef _REMOTE_CMPI_TOKENIZER_H
#define _REMOTE_CMPI_TOKENIZER_H

#include <ctype.h>

#define EOL   0
#define EQUAL 100
#define LB    101
#define RB    102
#define COMMA 103
#define ALNUM 104

#define BUFFLEN  4096
#define TOKENLEN 1024


int gettoken(char **buff,char *token)
{
    int tokentype;

    while (**buff && isspace(**buff))
    {
        ++*buff;
    }

    if (!**buff)
    {
        return *token = EOL;
    }

    *token = *(*buff)++;
    switch (*token++)
    {
        case '=':
            tokentype = EQUAL;
            break;
        case '{':
            tokentype = LB;
            break;
        case '}':
            tokentype = RB;
            break;
        case ',':
            tokentype = COMMA;
            break;
        default:
            while (**buff && !isspace(**buff) && (isalnum(**buff)|| **buff=='_'))
            {
                *token++ = *(*buff)++;
            }
            tokentype = ALNUM;
    }
    *token ='\0';

    return tokentype;
}

#endif
