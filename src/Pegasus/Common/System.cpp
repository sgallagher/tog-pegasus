//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By: Rudy Schuet (rudy.schuet@compaq.com) 11/12/01
//					added nsk platform support
//				Ramnath Ravindran (Ramnath.Ravindran@compaq.com) 03/21/2002
//					replaced instances of "| ios::binary" with 
//					PEGASUS_OR_IOS_BINARY
//
//%/////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <Pegasus/Common/Config.h>           
#endif                                       


#include <fstream>
#include <cctype>  // for tolower()
#include <cstring>
#include "System.h"

#include <Pegasus/Common/PegasusVersion.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "SystemWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "SystemUnix.cpp"
#elif defined(PEGASUS_OS_TYPE_NSK)
# include "SystemNsk.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

Boolean System::copyFile(const char* fromPath, const char* toPath)
{
    ifstream is(fromPath PEGASUS_IOS_BINARY);
    ofstream os(toPath PEGASUS_IOS_BINARY);

    char c;

    while (is.get(c))
    {
	if (!os.put(c))
	    return false;
    }

    return true;
}

// ATTN: Move to platform-specific System implementation files and call
// strcasecmp where it is available.
Sint32 System::strcasecmp(const char* s1, const char* s2)
{
    while (*s1 && *s2)
    {
        int r = tolower(*s1++) - tolower(*s2++);

        if (r)
            return r;
    }

    if (*s2)
        return -1;
    else if (*s1)
        return 1;

    return 0;
}

PEGASUS_NAMESPACE_END
