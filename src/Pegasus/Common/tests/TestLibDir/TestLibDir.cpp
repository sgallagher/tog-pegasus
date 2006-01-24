//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Dir.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const char USAGE[] = "\n\
Usage: %s path\n\
\n\
This tool attempts to load all of the libraries found in the directory given\n\
by the path argument. It uses whatever dynamic loading mechanism is provided\n\
by the operating system. For example, on Linux/Unix systems it uses dlopen().\n\
It loads the libraries one by one and terminates with an error and exit code\n\
of one if any library cannot be loaded.\n\
\n\
";

int main(int argc, char** argv)
{
    // Check arguments:

    if (argc != 2)
    {
	fprintf(stderr, USAGE, argv[0]);
	exit(1);
    }

    // Scan the directory for libraries. Open the ones found.

    try
    {
	Uint32 failures = 0;

	for (Dir dir(argv[1]); dir.more(); dir.next())
	{
	    String name = dir.getName();

	    // Skip files ending in ".a".
	    // Skip files with no ".".

	    if (name.find('.') == PEG_NOT_FOUND ||
		name.find(".a") == name.size() - 2)
		continue;

	    String path = String(argv[1]) + String("/") + name;
	    CString cstr = path.getCString();

	    // Skip directories:

	    if (System::isDirectory(cstr))
		continue;

	    DynamicLibraryHandle handle = System::loadDynamicLibrary(cstr);

	    if (handle == 0)
	    {
		cout << name << " (********** failed **********)" << endl;
		failures++;
	    }
	    else 
	    {
		cout << name << " (okay)" << endl;
		System::unloadDynamicLibrary(handle);
	    }
	}

	if (failures > 0)
	{
	    fprintf(stderr, 
		"\n%s: failed to load %u libraries\n", argv[0], failures);
	    exit(1);
	}
    }
    catch(...)
    {
	fprintf(stderr, "%s: failed to open directory: \"%s\"\n\n", 
	    argv[0], argv[1]);
	exit(1);
    }

    return 0;
}
