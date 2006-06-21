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
// Author: Dave Sudlik
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "TestChunkingStressProviderASSOC.h"

PEGASUS_NAMESPACE_BEGIN

TestChunkingStressProviderASSOC::TestChunkingStressProviderASSOC(void)
{
}

TestChunkingStressProviderASSOC::~TestChunkingStressProviderASSOC(void)
{
}

void TestChunkingStressProviderASSOC::initialize(CIMOMHandle & cimom)
{
    _cimom = cimom;
}

void TestChunkingStressProviderASSOC::terminate(void)
{
    delete this;
}

void TestChunkingStressProviderASSOC::associators(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & associationClass,
    const CIMName & resultClass,
    const String & role,
    const String & resultRole,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    ObjectResponseHandler & handler)
{
    CIMNamespaceName NAMESPACE(String("test/TestProvider"));
    CIMName CLASSNAME(String("TST_ChunkingStressInstance"));

    handler.processing();

    Array<CIMInstance> cimObjects =
        _cimom.enumerateInstances(
            context, NAMESPACE, CLASSNAME,
            true, true, false, false, CIMPropertyList());
        
    for (Uint32 i = 0, n = cimObjects.size(); i < n; i++)
    {
        handler.deliver(cimObjects[i]);
    }
    
    // complete processing the request
    handler.complete();
}

void TestChunkingStressProviderASSOC::associatorNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & associationClass,
    const CIMName & resultClass,
    const String & role,
    const String & resultRole,
    ObjectPathResponseHandler & handler)
{
    CIMNamespaceName NAMESPACE(String("test/TestProvider"));
    CIMName CLASSNAME(String("TST_ChunkingStressInstance"));

    handler.processing();

    Array<CIMObjectPath> cimObjectNames =
        _cimom.enumerateInstanceNames(
            context, NAMESPACE, CLASSNAME);
        
    for (Uint32 i = 0, n = cimObjectNames.size(); i < n; i++)
    {
        handler.deliver(cimObjectNames[i]);
    }
    
    // complete processing the request
    handler.complete();
}

void TestChunkingStressProviderASSOC::references(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    ObjectResponseHandler & handler)
{
    throw CIMNotSupportedException("TestChunkingStressProviderASSOC::references");
}

void TestChunkingStressProviderASSOC::referenceNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role,
    ObjectPathResponseHandler & handler)
{
    throw CIMNotSupportedException("TestChunkingStressProviderASSOC::referenceNames");
}

PEGASUS_NAMESPACE_END
