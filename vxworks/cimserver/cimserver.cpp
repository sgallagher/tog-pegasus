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

/* This module is a typical implementation of server main for an embedded
   environment.
   This work is based on the definitions in PEP 305 and the EmbeddedSystemBuild
   readme.
   This example creates an embedded cim server and adds the static provider
   definitions and memory-resident repository definitions.
   It is expected that an embedded system creator could use this example to
   create a cim server main specifically for their needs and define their
   providers and repository in that main.  There is no need that the code
   exist in the Pegasus CVS source tree. This example is in the source tree
   to make it available as a working example.

   This example was created specifically for VxWorks and the kernel mode.
*/

#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Server/EmbeddedServer.h>

// Header files for each of the namespaces that are to be created for this
// server.
// Each include statement below should define a c++ header file created by the 
// compilation of a set of cim classes and qualifiers into a single namespace.
// These are the header files that are created by the compilation.
// 
#include "root_cimv2_namespace.h"
#include "root_PG_Internal_namespace.h"
#include "root_PG_InterOp_namespace.h"

PEGASUS_USING_PEGASUS;

// Define the Pegasus Servermain that will be called by this main function

extern "C" int PegasusServerMain(int argc, char** argv);

//
//  Define the static providers that will be installed and registered as part
//  of the startup of the server.
//  NOTE that Pegasus standard control providers are defined elsewhere.
//

// Defines the entry point for each static provider. 
extern "C" CIMProvider* PegasusCreateProvider_Hello(const String&);
extern "C" CIMProvider* PegasusCreateProvider_Goodbye(const String&);

//
// Table of user static providers to be registered
//
//
// This table defines an entry for each provider to be registered when the
// server starts.  Each entry consists of the following information.
//   ModuleName
//   Provider Name
//   Namespace in which this provider will be registered Class for which
//     this provider is registered Entry point for the provider
//   The provider entry point
// 
//   NOTE: The table MUST end with an all zeros entry. That is
//   the terminator for the functions that perform the
//   installation.
//

static Pegasus::ProviderTableEntry _providerTable[] =
{
    // Define HelloProvider in its own module
    {
        "HelloModule", 
        "HelloProvider", 
        "root/cimv2", 
        "Hello",
        PegasusCreateProvider_Hello,
    },
    // Define GoodbyeProvider in its own module
    {
        "GoodbyeModule", 
        "GoodbyeProvider", 
        "root/cimv2", 
        "Goodbye",
        PegasusCreateProvider_Goodbye,
    },
    // empty entry defines end of table
    { 0, 0, 0, 0, 0 },
};

static const char INSTANCE_REPOISTORY_PATH[] = "redbird:/tmp/instances.dat";

//
// Definition of class repository that will be defined in the installation of
// the repository. These names correspond to the source files for the class
// and qualifier definitions for individual namespaces that were created with
// cimmofl.
// 
// NOTE: that this table must include a zero entry to terminate the table.
// Create a single entry in this table for each namespace which was compiled.
// 

static const MetaNameSpace* _nameSpaces[] =
{
    &root_PG_InterOp_namespace,            // root/PG_InterOp namespace
    &root_cimv2_namespace,
    &root_PG_Internal_namespace,
    0,
};

//
// Callbacks for the save and restore of the instance repository. The
// implementations here are examples where the persistent store is a
// file.
//

// Example of the instance repository load function.  The function is expected
// to acquire data from a persistent store and place it into the buffer
// parameter. The data parameter represents data defined by the install of
// this callback.
static void _loadCallback(Buffer& buffer, void* data)
{
#if 0
    printf("===== _loadCallback()\n");

    // If this file exists, pass its contents to the memory resident 
    // repository.

    FILE* is = fopen(INSTANCE_REPOISTORY_PATH, "rb");

    if (!is)
    {
        printf("DOES NOT EXIST[%s]\n", INSTANCE_REPOISTORY_PATH);
        return;
    }

    size_t n;
    char buf[4096];

    while ((n = fread(buf, 1, sizeof(buf), is)) > 0)
        buffer.append(buf, n);

    fclose(is);
#endif
}

// The save instance repository callback function.  This function is called by
// the memory-resident CIM repository function upon the completion of any
// change to the instance repository. the buffer parameter contains the data
// to be persisted.  The data parameter defines data which the user defined 
// upon the installation of the callback.  This function is an example of the
// implementation of a function to persist the instance repository. It simply
// writes the repository to a file and, of course, depends on the existence
// of a file system that would support the fopen, fwrite. The only requirement
// on the save and load functions is that they be able to persist the buffer
// in the saveCallback function and restore it in the _loadCallback function.

static void _saveCallback(const Buffer& buffer, void* data)
{
#if 0
    printf("===== _saveCallback()\n");

    FILE* os = fopen(INSTANCE_REPOISTORY_PATH, "wb");

    if (!os)
    {
        printf("FAILED TO OPEN[%s]\n", INSTANCE_REPOISTORY_PATH);
        return;
    }

    const char* ptr = buffer.getData();
    size_t size = buffer.size();

    if (fwrite(ptr, 1, size, os) != ssize_t(size))
    {
        printf("FAILED TO WRITE[%s]\n", INSTANCE_REPOISTORY_PATH);
    }

    fclose(os);
#endif
}

// main function for the cimserver. Note that in this sample code the
// main function is named cimserver because this example is based on
// use of VxWorks and of the kernel mode rather than the
// real-time-process mode so that the symbol cimserver becomse the 
// explicit start point for the cimserver.  If this were a VxWorks
// RTP, this would be main(...)

extern "C" int cimserver(int argc, char** argv)
{
    printf("\n===== CIMSERVER =====\n");

    // Setup the provider table:

    EmbeddedServer::installProviderTable(_providerTable);

    // Install the namespaces, classes and qualifiers defined
    // into the memory-resident repository.

    EmbeddedServer::installNameSpaces(_nameSpaces);

    // Install instance repository load/save callback functions:

    EmbeddedServer::installLoadRepositoryCallback(_loadCallback, 0);
    EmbeddedServer::installSaveRepositoryCallback(_saveCallback, 0);

    // Run the pegasus server
    // for embedded systems, the cimserver main has been redefined
    // to PegasusServerMain, a Pegasus function.

    try
    {
        return PegasusServerMain(argc, argv);
    }
    catch (...)
    {
        printf("cimserver(): exception\n");
        return -1;
    }
}
