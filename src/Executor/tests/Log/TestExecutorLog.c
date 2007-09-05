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

#include <Executor/Log.h>
#include <Executor/Globals.h>
#include <Executor/tests/TestAssert.h>
#include <stdio.h>
#include <string.h>

int main()
{
    /* Test default logLevel value */
    PEGASUS_TEST_ASSERT(GetLogLevel() == LL_INFORMATION);

    /* Test InitLogLevel() */

    /* Test with no logLevel specified.  Value is unchanged. */
    {
        static const char* argv[] = { "program" };
        static const int argc = sizeof(argv) / sizeof(argv[0]);
        globals.argv = (char**)argv;
        globals.argc = argc;

        InitLogLevel();
        PEGASUS_TEST_ASSERT(GetLogLevel() == LL_INFORMATION);
    }

    /* Test with invalid logLevel specified.  Value is unchanged. */
    {
        static const char* argv[] = { "program", "logLevel=123" };
        static const int argc = sizeof(argv) / sizeof(argv[0]);
        globals.argv = (char**)argv;
        globals.argc = argc;

        InitLogLevel();
        PEGASUS_TEST_ASSERT(GetLogLevel() == LL_INFORMATION);
    }

    /* Test with logLevel WARNING specified */
    {
        static const char* argv[] = { "program", "logLevel=WARNING" };
        static const int argc = sizeof(argv) / sizeof(argv[0]);
        globals.argv = (char**)argv;
        globals.argc = argc;

        InitLogLevel();
        PEGASUS_TEST_ASSERT(GetLogLevel() == LL_WARNING);
    }

    /* Test SetLogLevel() */

    PEGASUS_TEST_ASSERT(SetLogLevel("TRACE") == 0);
    PEGASUS_TEST_ASSERT(GetLogLevel() == LL_TRACE);
    PEGASUS_TEST_ASSERT(SetLogLevel("INFORMATION") == 0);
    PEGASUS_TEST_ASSERT(GetLogLevel() == LL_INFORMATION);
    PEGASUS_TEST_ASSERT(SetLogLevel("WARNING") == 0);
    PEGASUS_TEST_ASSERT(GetLogLevel() == LL_WARNING);
    PEGASUS_TEST_ASSERT(SetLogLevel("SEVERE") == 0);
    PEGASUS_TEST_ASSERT(GetLogLevel() == LL_SEVERE);
    PEGASUS_TEST_ASSERT(SetLogLevel("FATAL") == 0);
    PEGASUS_TEST_ASSERT(GetLogLevel() == LL_FATAL);
    PEGASUS_TEST_ASSERT(SetLogLevel("TRACEERROR") == -1);
    PEGASUS_TEST_ASSERT(GetLogLevel() == LL_FATAL);

    printf("+++++ passed all tests\n");

    return 0;
}
