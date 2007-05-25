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

#include <Executor/Macro.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main()
{

    /* Test DefineMacro() */
    {
        assert(DefineMacro("x", "100") == 0);
        assert(DefineMacro("y", "hello") == 0);
        assert(DefineMacro("z", "true") == 0);
    }

    /* Test FindMacro() */
    {
        const char* x;
        const char* y;
        const char* z;

        x = FindMacro("x");
        assert(x != NULL && strcmp(x, "100") == 0);

        y = FindMacro("y");
        assert(y != NULL && strcmp(y, "hello") == 0);

        z = FindMacro("z");
        assert(z != NULL && strcmp(z, "true") == 0);
    }

    /* Test ExpandMacro() */
    {
        char buffer[EXECUTOR_BUFFER_SIZE];

        assert(ExpandMacros("${x} ${y} ${z}", buffer) == 0);
        assert(strcmp(buffer, "100 hello true") == 0);
    }

#if 0
    DumpMacros();
#endif

    printf("+++++ passed all tests\n");

    return 0;
}
