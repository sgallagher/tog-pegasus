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
#ifndef Pegasus_DynamicIndicationProvider_h
#define Pegasus_DynamicIndicationProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

class DynamicIndicationProvider :
    public CIMMethodProvider,
    public CIMInstanceProvider,
    public CIMIndicationProvider
{

public:
    DynamicIndicationProvider(void);
    virtual ~DynamicIndicationProvider(void);

    // CIMProvider interface
    virtual void initialize(CIMOMHandle & cimom);
    virtual void terminate(void);

    // CIMMethodProvider Interface
    void invokeMethod(
        const OperationContext & context,
        const CIMObjectPath & objectReference,
        const CIMName & methodName,
        const Array<CIMParamValue> & inParameters,
        MethodResultResponseHandler & handler);
    
    // CIMIndicationProvider interface
    virtual void enableIndications(IndicationResponseHandler & handler);

    virtual void disableIndications(void);

    virtual void createSubscription(
        const OperationContext & context,
        const CIMObjectPath & subscriptionName,
        const Array<CIMObjectPath> & classNames,
        const CIMPropertyList & propertyList,
        const Uint16 repeatNotificationPolicy);

    virtual void modifySubscription(
        const OperationContext & context,
        const CIMObjectPath & subscriptionName,
        const Array<CIMObjectPath> & classNames,
        const CIMPropertyList & propertyList,
        const Uint16 repeatNotificationPolicy);

    virtual void deleteSubscription(
        const OperationContext & context,
        const CIMObjectPath & subscriptionName,
        const Array<CIMObjectPath> & classNames);

    // CIMInstanceProvider interface
    virtual void getInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler);

    virtual void enumerateInstances(
        const OperationContext & context,
        const CIMObjectPath & classReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler);

    virtual void enumerateInstanceNames(
        const OperationContext & context,
        const CIMObjectPath & classReference,
        ObjectPathResponseHandler & handler);

    virtual void createInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        ObjectPathResponseHandler & handler);

    virtual void modifyInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList & propertyList,
        ResponseHandler & handler);

    virtual void deleteInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        ResponseHandler & handler);

public:
    virtual void sendIndication(int severity, String classname, String description);

protected:
    // CIMOMHandle _cimom;
    IndicationResponseHandler *_pHandler;

private:
    static AtomicInt _refCnt;

};

PEGASUS_NAMESPACE_END

#endif














