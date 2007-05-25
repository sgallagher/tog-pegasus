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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////
*/

#include <Executor/Strlcat.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main()
{
    {
        char buf[8];
        size_t n;
        memset(buf, 'X', sizeof(buf));

        *buf = '\0';
        n = Strlcat(buf, "", sizeof(buf));
        assert(n == 0);
        assert(strcmp(buf, "") == 0);

        n = Strlcat(buf, "abc", sizeof(buf));
        assert(n == 3);
        assert(strcmp(buf, "abc") == 0);

        n = Strlcat(buf, "xyz", sizeof(buf));
        assert(n == 6);
        assert(strcmp(buf, "abcxyz") == 0);

        n = Strlcat(buf, "lmnop", sizeof(buf));
        assert(n == 11);
        assert(strcmp(buf, "abcxyzl") == 0);

        n = Strlcat(buf, "xxx", sizeof(buf));
        assert(n == 10);
        assert(strcmp(buf, "abcxyzl") == 0);
    }

    {
        char buf[8];
        size_t n;
        memset(buf, 'X', sizeof(buf));

        n = Strlcat(buf, "abc", sizeof(buf));
        assert(n == sizeof(buf));
        assert(memcmp(buf, "XXXXXXXX", 8) == 0);
    }

    {
        char buf[8];
        size_t n;
        memset(buf, 'X', sizeof(buf));

        *buf = '\0';
        n = Strlcat(buf, "1234", sizeof(buf));
        assert(n == 4);
        assert(strcmp(buf, "1234") == 0);

        n = Strlcat(buf, "5678", sizeof(buf));
        assert(n == 8);
        assert(strcmp(buf, "1234567") == 0);
    }

    printf("+++++ passed all tests\n");

    return 0;
}
