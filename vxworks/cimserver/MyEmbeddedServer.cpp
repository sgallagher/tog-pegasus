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

// Uncomment this to register the singleton CIMPLE provider module.
// #define INCLUDE_CIMPLE_PROVIDERS

PEGASUS_NAMESPACE_BEGIN

#if defined(INCLUDE_CIMPLE_PROVIDERS)
extern "C" class CIMProvider* PegasusCreateProvider(const String&);
#endif

extern "C" class CIMProvider* PegasusCreateProviderMain(
    const String& providerName)
{
    CString cstr(providerName.getCString());

#if defined(INCLUDE_CIMPLE_PROVIDERS)
    return PegasusCreateProvider(providerName);
#else
    return 0;
#endif

}

MyEmbeddedServer::MyEmbeddedServer()
{
}

MyEmbeddedServer::~MyEmbeddedServer()
{
}

void MyEmbeddedServer::loadRepository(
    Array<Uint8>& data)
{
    // This function is expected to acquire data from  a store (whether
    // memory-resident or persistent). This example maintains a repository
    // in memory.
    data = _repository;
}
    
void MyEmbeddedServer::saveRepository(
    const Array<Uint8>& data)
{
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

    printf("LOG[%d:%s:%d:%s]\n", type, system, level, message);
}

void MyEmbeddedServer::initialize()
{
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

    if (!registerPegasusCreateProviderEntryPoint(PegasusCreateProviderMain))
    {
        fprintf(stderr, "***** addSymbol() failed: Employee\n");
    }

#if defined(INCLUDE_CIMPLE_PROVIDERS)

    //
    // Register "Employee" provider:
    //

    if (!registerProvider(
        nameSpaces,
        "Employee", /* classname */
        MyEmbeddedServer::INSTANCE_PROVIDER_TYPE))
    {
        fprintf(stderr, "***** registerProvider() failed: Employee\n");
    }

    if (!registerProvider(
        nameSpaces,
        "EmployeeLink", /* classname */
        MyEmbeddedServer::INSTANCE_PROVIDER_TYPE))
    {
        fprintf(stderr, "***** registerProvider() failed: Employee\n");
    }

#endif /* defined(INCLUDE_CIMPLE_PROVIDERS) */
}

bool MyEmbeddedServer::authenticate(const char* user, const char* pass)
{
    if (strcmp(user, "guest") == 0 && strcmp(pass, "changeme") == 0)
        return true;
    else
        return false;
}

PEGASUS_NAMESPACE_END
