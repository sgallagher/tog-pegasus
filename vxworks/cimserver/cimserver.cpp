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

// This module is a typical implementation of server main for an embedded
// environment. This work is based on the definitions in PEP 305 and the 
// EmbeddedSystemBuild readme. This example creates an embedded cim server and 
// adds the static provider definitions and memory-resident repository 
// definitions. It is expected that an embedded system creator could use this 
// example to create a cim server main specifically for their needs and define 
// their providers and repository in that main.  There is no need that the code
// exist in the Pegasus CVS source tree. This example is in the source tree
// to make it available as a working example. This example was created 
// specifically for VxWorks and the kernel mode.

#include <cstdio>
#include <routeLib.h>
#include <netShow.h>
#include "MyEmbeddedServer.h"
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/System.h>

PEGASUS_USING_PEGASUS;

// main function for the cimserver. Note that in this sample code the
// main function is named cimserver because this example is based on
// use of VxWorks and of the kernel mode rather than the real-time-process 
// mode so that the symbol cimserver becomse the explicit start point for the 
// cimserver.  If this were a VxWorks RTP, this would be main(...)

extern "C" int cimserver(int argc, char** argv)
{
    printf("\n===== CIMSERVER =====\n");

    MyEmbeddedServer server;

    server.registerSLPService(5988, false);
#if 0
    server.registerSLPService(5989, true);
#endif
    server.run(argc, argv);

    return 0;
}
