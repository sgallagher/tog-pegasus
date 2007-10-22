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

#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Server/EmbeddedServer.h>
#include "root_cimv2_namespace.h"
#include "root_PG_Internal_namespace.h"
#include "root_PG_InterOp_namespace.h"

PEGASUS_USING_PEGASUS;

extern "C" int PegasusServerMain(int argc, char** argv);
extern "C" CIMProvider* PegasusCreateProvider_Hello(const String&);
extern "C" CIMProvider* PegasusCreateProvider_Goodbye(const String&);

static Pegasus::ProviderTableEntry _providerTable[] =
{
    {
        "HelloModule", 
        "HelloProvider", 
        "root/cimv2", 
        "Hello",
        PegasusCreateProvider_Hello,
    },
    {
        "GoodbyeModule", 
        "GoodbyeProvider", 
        "root/cimv2", 
        "Goodbye",
        PegasusCreateProvider_Goodbye,
    },
    { 0, 0, 0, 0, 0 },
};

static const char INSTANCE_REPOISTORY_PATH[] = "redbird:/tmp/instances.dat";

static const MetaNameSpace* _nameSpaces[] =
{
    &root_PG_InterOp_namespace,
    &root_cimv2_namespace,
    &root_PG_Internal_namespace,
    0,
};

static void _loadCallback(Buffer& buffer, void* data)
{
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
}

static void _saveCallback(const Buffer& buffer, void* data)
{
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
}

extern "C" int cimserver(int argc, char** argv)
{
    printf("===== CIMSERVER =====\n");

    // Setup the provider table:

    EmbeddedServer::installProviderTable(_providerTable);

    // Set the namespace array:

    EmbeddedServer::installNameSpaces(_nameSpaces);

    // Install load/save callbacks:

    EmbeddedServer::installLoadRepositoryCallback(_loadCallback, 0);
    EmbeddedServer::installSaveRepositoryCallback(_saveCallback, 0);

    // Run the pegasus server:

    return PegasusServerMain(argc, argv);
}
