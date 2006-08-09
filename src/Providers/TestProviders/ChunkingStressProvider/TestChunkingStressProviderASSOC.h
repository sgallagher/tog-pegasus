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
// Author: Dave Sudlik (dsudlik@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_TestChunkingStressProviderASSOC_h
#define Pegasus_TestChunkingStressProviderASSOC_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>

PEGASUS_NAMESPACE_BEGIN

class TestChunkingStressProviderASSOC :
	public CIMAssociationProvider
{
public:
	TestChunkingStressProviderASSOC(void);
	virtual ~TestChunkingStressProviderASSOC(void);

	// CIMProvider interface
	virtual void initialize(CIMOMHandle & cimom);
	virtual void terminate(void);

	// CIMAssociationProvider interface
        // CIMAssociationProvider interface
        virtual void associators(
                const OperationContext & context,
                const CIMObjectPath & objectName,
                const CIMName & associationClass,
                const CIMName & resultClass,
                const String & role,
                const String & resultRole,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                ObjectResponseHandler & handler);

        virtual void associatorNames(
                const OperationContext & context,
                const CIMObjectPath & objectName,
                const CIMName & associationClass,
                const CIMName & resultClass,
                const String & role,
                const String & resultRole,
                ObjectPathResponseHandler & handler);

        virtual void references(
                const OperationContext & context,
                const CIMObjectPath & objectName,
                const CIMName & resultClass,
                const String & role,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                ObjectResponseHandler & handler);

        virtual void referenceNames(
                const OperationContext & context,
                const CIMObjectPath & objectName,
                const CIMName & resultClass,
                const String & role,
                ObjectPathResponseHandler & handler);

protected:
    CIMOMHandle _cimom;
    Mutex _CIMOMHandle_mut;

};

PEGASUS_NAMESPACE_END

#endif
