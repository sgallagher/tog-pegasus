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

#include <Executor/Config.h>
#include <Executor/Globals.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main()
{
    /* Test GetConfigParamFromCommandLine() */
    {
        static const char* argv[] = { "program", "option1=one", (char*)0 };
        static const int argc = sizeof(argv) / sizeof(argv[0]);
        char buffer[EXECUTOR_BUFFER_SIZE];

        globals.argv = (char**)argv;
        globals.argc = argc;

        assert(GetConfigParamFromCommandLine("option1", buffer) == 0);
        assert(strcmp(buffer, "one") == 0);
    }

    /* Test GetConfigParamFromFile() */
    {
        char buffer[EXECUTOR_BUFFER_SIZE];

        assert(GetConfigParamFromFile("my.conf", "option1", buffer) == 0);
        assert(strcmp(buffer, "1") == 0);

        assert(GetConfigParamFromFile("my.conf", "option2", buffer) == 0);
        assert(strcmp(buffer, "2") == 0);

        assert(GetConfigParamFromFile("my.conf", "option3", buffer) == 0);
        assert(strcmp(buffer, "3") == 0);
    }

    printf("+++++ passed all tests\n");

    return 0;
}
