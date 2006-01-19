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
// Author: Subodh Soni IBM Corporation, (ssubodh@in.ibm.com)
//
// Modified By:
//             
//             
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef ServerProfileSampleProvider_H
#define ServerProfileSampleProvider_H

#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>

PEGASUS_NAMESPACE_BEGIN

class ServerProfileSampleProvider :
    public CIMInstanceProvider, CIMAssociationProvider
{
public:
    ServerProfileSampleProvider(Uint16 providerType) :
        type(providerType) {}

    ~ServerProfileSampleProvider(void) {}

    // CIMProvider interface
    void initialize(CIMOMHandle & cimom);
    void terminate(void) {}

    // CIMInstanceProvider interface
    void getInstance(
        const OperationContext & context,
        const CIMObjectPath & ref,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler);

    void enumerateInstances(
        const OperationContext & context,
        const CIMObjectPath & ref,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler);

    void enumerateInstanceNames(
        const OperationContext & context,
        const CIMObjectPath & ref,
        ObjectPathResponseHandler & handler);

    void modifyInstance(
        const OperationContext & context,
        const CIMObjectPath & ref,
        const CIMInstance & obj,
        const Boolean includeQualifiers,
        const CIMPropertyList & propertyList,
        ResponseHandler & handler);

    void createInstance(
        const OperationContext & context,
        const CIMObjectPath & ref,
        const CIMInstance & obj,
        ObjectPathResponseHandler & handler);

    void deleteInstance(
        const OperationContext & context,
        const CIMObjectPath & ref,
        ResponseHandler & handler);

    // CIMAssociationProvider methods
    void associators(
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

    void associatorNames(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const CIMName & associationClass,
        const CIMName & resultClass,
        const String & role,
        const String & resultRole,
        ObjectPathResponseHandler & handler);

    void references(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const CIMName & resultClass,
        const String & role,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        ObjectResponseHandler & handler);

    void referenceNames(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const CIMName & resultClass,
        const String & role,
        ObjectPathResponseHandler & handler);

private:
    Uint16 type;
    Array<CIMInstance> instances;
};

PEGASUS_NAMESPACE_END

#endif
