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

#ifndef Pegasus_TestStorageSystemProvider_h
#define Pegasus_TestStorageSystemProvider_h

#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>

PEGASUS_USING_PEGASUS;

class TestServerProfileProvider :
	public CIMInstanceProvider,
    public CIMAssociationProvider
{
public:
	TestServerProfileProvider(const String &);
	virtual ~TestServerProfileProvider();

	virtual void initialize(CIMOMHandle & cimom);
	virtual void terminate();

	// Instance Provider interface
	virtual void getInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const Boolean includeQualifiers,
		const Boolean includeClassOrigin,
		const CIMPropertyList & propertyList,
		InstanceResponseHandler & handler);

	virtual void enumerateInstances(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const Boolean includeQualifiers,
		const Boolean includeClassOrigin,
		const CIMPropertyList & propertyList,
		InstanceResponseHandler & handler);

	virtual void enumerateInstanceNames(
		const OperationContext & context,
		const CIMObjectPath & ref,
		ObjectPathResponseHandler & handler);

	virtual void modifyInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const CIMInstance & obj,
		const Boolean includeQualifiers,
		const CIMPropertyList & propertyList,
		ResponseHandler & handler);

	virtual void createInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const CIMInstance & obj,
		ObjectPathResponseHandler & handler);

	virtual void deleteInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		ResponseHandler & handler);

    //
    // Association Provider Interface
    //
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

private:
    Array<CIMInstance> localEnumerateInstances(
        const CIMName & className,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList & propList);

    CIMOMHandle cimomHandle;
    CIMName testClassName;
    CIMClass testClass;
    Array<String> names;
};

#endif
