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

#include "MyEmbeddedServer.h"
#include <Pegasus/Common/String.h>
#include <Pegasus/Provider/CIMProvider.h>

// Header files for each of the namespaces that are to be created for this
// server. Each include statement below should define a c++ header file created
// by the compilation of a set of cim classes and qualifiers into a single
// namespace. These are the header files that are created by the compilation.
#include "root_cimv2_namespace.h"
#include "root_PG_Internal_namespace.h"
#include "root_PG_InterOp_namespace.h"

PEGASUS_NAMESPACE_BEGIN

// Define entry points for each static provider. 
extern "C" class CIMProvider* PegasusCreateProvider(const String&);

extern "C" class CIMProvider* PegasusCreateProviderWrapper(const String& arg)
{
    fprintf(stderr, "Called PegasusCreateProviderWrapper(%s)\n",
        (const char*)arg.getCString());

    class CIMProvider* prov = PegasusCreateProvider(arg);

    if (!prov)
    {
        fprintf(stderr, "PegasusCreateProviderWrapper(): null pointer\n");
    }
    else if (!dynamic_cast<CIMProvider*>(prov))
    {
        fprintf(stderr, "PegasusCreateProviderWrapper() dynamic cast failed\n");
    }
    else 
    {
        fprintf(stderr, "PegasusCreateProviderWrapper(): okay\n");
    }

    return prov;
}

MyEmbeddedServer::MyEmbeddedServer()
{
    printf("MyEmbeddedServer::MyEmbeddedServer()\n");
}

MyEmbeddedServer::~MyEmbeddedServer()
{
    printf("MyEmbeddedServer::~MyEmbeddedServer()\n");
}

void MyEmbeddedServer::loadRepository(
    Array<Uint8>& data)
{
    printf("MyEmbeddedServer::loadRepository()\n");

    // This function is expected to acquire data from  a store (whether
    // memory-resident or persistent). This example maintains a repository
    // in memory.
    data = _repository;
}
    
void MyEmbeddedServer::saveRepository(
    const Array<Uint8>& data)
{
    printf("MyEmbeddedServer::saveRepository()\n");

    // This function is expected to save the dynamic elements of the memory
    // resident repository. It can save them in memory or on a persitent
    // device. This example saves them in memory.
    _repository = data;
}

void MyEmbeddedServer::putLog(
    int type,
    const char* system,
    int level,
    const char* message)
{
    // This function is responsible for adding a record to the log. This
    // implementation simply prints the log record to standard output.

    printf("MyEmbeddedServer::putLog():\n");
    printf("    type=%d\n", type);
    printf("    system=%s\n", system);
    printf("    level=%d\n", level);
    printf("    message=%s\n", message);
}

void MyEmbeddedServer::initialize()
{
    printf("MyEmbeddedServer::initialize()\n");

    addNameSpace(&root_PG_InterOp_namespace);
    addNameSpace(&root_cimv2_namespace);
    addNameSpace(&root_PG_Internal_namespace);

    // Build list of registration namespaces.

    Array<CIMNamespaceName> nameSpaces;
    nameSpaces.append("root/cimv2");

    //
    // Register the PegasusCreateProvider entry point (there can be only
    // one of these).
    //

    if (!registerPegasusCreateProviderEntryPoint(PegasusCreateProviderWrapper))
    {
        printf("***** addSymbol() failed: Employee\n");
    }

    //
    // Register "Employee" provider:
    //

    if (!registerProvider(
        nameSpaces,
        "Employee", /* classname */
        MyEmbeddedServer::PEGASUS_PROVIDER_INTERFACE,
        MyEmbeddedServer::INSTANCE_PROVIDER_TYPE))
    {
        printf("***** registerProvider() failed: Employee\n");
    }

    if (!registerProvider(
        nameSpaces,
        "EmployeeLink", /* classname */
        MyEmbeddedServer::PEGASUS_PROVIDER_INTERFACE,
        MyEmbeddedServer::INSTANCE_PROVIDER_TYPE))
    {
        printf("***** registerProvider() failed: Employee\n");
    }
}

PEGASUS_NAMESPACE_END
