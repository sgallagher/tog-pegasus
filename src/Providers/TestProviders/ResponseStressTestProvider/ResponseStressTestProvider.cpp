//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>

#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/TimeValue.h>

#include "ResponseStressTestProvider.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

// String pattern used to build up size in response instances
String pattern = "abcdefghighjklmnopqrstuvwxyz01234567890";

// Name of class used by this provider
CIMName TestClass = "TST_ResponseTest";

//
//  Local Methods
//

// Build a String of size defined by size parameter * size of pattern
// parameter
String _buildString(Uint64 size, const String& pattern)
{
    String rtn;
    for (Uint32 i = 0; i <= pattern.size(); i++)
    {
        rtn.append(pattern);
    }
    return(rtn);
}

/*
    Builds a single path of the TST_ResponseTest class
*/
CIMObjectPath _buildPath(Uint64 sequenceNumber)
{

    Array<CIMKeyBinding> keyBindings;

    char namebuf[60];
    sprintf(namebuf, "%lu", (unsigned long)sequenceNumber);

    keyBindings.append(CIMKeyBinding("Id",
        namebuf, CIMKeyBinding::STRING));
    CIMObjectPath path("", CIMNamespaceName(),
        TestClass, keyBindings);
    return(path);
}

/*
    Builds a single instance with the defined sequence number, instanceSize,
    and timeDiff parameters.
*/
CIMInstance _buildInstance(Uint64 sequenceNumber,
                           Uint64 instanceSize, Uint64 timeDiff,
                           const CIMPropertyList& propertyList)
{

    CIMInstance instance(TestClass);

    char namebuf[60];
    sprintf(namebuf, "%lu", (unsigned long)sequenceNumber);

    instance.addProperty(CIMProperty("Id", String(namebuf)));
    instance.addProperty(CIMProperty("SequenceNumber", sequenceNumber));
    instance.addProperty(CIMProperty("Pattern", pattern));
    instance.addProperty(CIMProperty("interval", timeDiff));

    // Would be cheaper to build this once for the operation.
    String pad = _buildString(instanceSize, pattern);
    instance.addProperty(CIMProperty("S1", pad));

    instance.setPath(_buildPath(sequenceNumber));

    // filter out unwanted properties. Would be cheaper to not add
    // the unwanted properties
    instance.filter(true, true, propertyList);

    return(instance);
}

//
//      ResponseStressTestProvider Class Methods
//

ResponseStressTestProvider::ResponseStressTestProvider()
{
}

ResponseStressTestProvider::~ResponseStressTestProvider()
{
}

void ResponseStressTestProvider::initialize(CIMOMHandle& cimom)
{
    // save cimom handle
    _cimom = cimom;

    // set default  instance size and response count
    _instanceSize = 100;
    _responseCount = 5;
    _continue = true;
}

void ResponseStressTestProvider::terminate()
{
    delete this;
}

/*
    Minimal implementation since the goal of this provider is to return
    enumerates.
*/
void ResponseStressTestProvider::getInstance(
    const OperationContext& context,
    const CIMObjectPath& instanceReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler& handler)
{
    handler.processing();

    // NOTE: For getInstance ignore the timeDiff property
    // and set to zero.

    CIMInstance instance = _buildInstance(0,
        _instanceSize,
        0,
        propertyList);
    handler.deliver(instance);

    throw CIMException(CIM_ERR_NOT_FOUND);

    handler.complete();
}

/*
    Build and return a set of instances based on the provider parameters
    of _responseCount and _instanceSixe
*/
void ResponseStressTestProvider::enumerateInstances(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler& handler)
{
    handler.processing();

    Uint64 prevTime = TimeValue::getCurrentTime().toMicroseconds();

    for (Uint32 i = 0, n = _responseCount ; i < n; i++)
    {
        // Each instance reflects time difference from previous instance
        // creation.
        Uint64 newTime = TimeValue::getCurrentTime().toMicroseconds();

        try
        {
            CIMInstance instance = _buildInstance(i,
                      _instanceSize,
                      newTime - prevTime,
                      propertyList);
            prevTime = newTime;
            handler.deliver(instance);
        }
        catch (CIMException&)
        {
            // suppress error
        }
    }

    handler.complete();
}

void ResponseStressTestProvider::enumerateInstanceNames(
    const OperationContext& context,
    const CIMObjectPath& classReference,
    ObjectPathResponseHandler& handler)
{
    handler.processing();

    for (Uint32 i = 0, n = _responseCount ; i < n; i++)
    {

        try
        {
            CIMObjectPath path = _buildPath(i);
            handler.deliver(path);
        }
        catch (CIMException&)
        {
            // suppress error
        }
    }

    handler.complete();
}

void ResponseStressTestProvider::modifyInstance(
    const OperationContext& context,
    const CIMObjectPath& instanceReference,
    const CIMInstance& instanceObject,
    const Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    ResponseHandler& handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void ResponseStressTestProvider::createInstance(
    const OperationContext& context,
    const CIMObjectPath& instanceReference,
    const CIMInstance& instanceObject,
    ObjectPathResponseHandler& handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void ResponseStressTestProvider::deleteInstance(
    const OperationContext& context,
    const CIMObjectPath& instanceReference,
    ResponseHandler& handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void ResponseStressTestProvider::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & objectReference,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParameters,
    MethodResultResponseHandler & handler)
{
    // convert a fully qualified reference into a local reference
    // (class name and keys only).

    CIMObjectPath localReference = CIMObjectPath(
        String(),
        CIMNamespaceName(),
        objectReference.getClassName(),
        objectReference.getKeyBindings());

    handler.processing();
    Uint32 rtnCode = 0;
    if (objectReference.getClassName().equal("TST_ResponseStressTest"))
    {
        // set method sets the _responseCount and instanceSize based on
        // input parameters.
        if (methodName.equal("set"))
        {
            for(Uint32 i = 0; i < inParameters.size(); ++i)
            {
                if (_continue == false)
                {
                    _continue = true;
                }

                String paramName = inParameters[i].getParameterName();
                CIMValue v = inParameters[i].getValue();

                if(paramName =="ResponseCount")
                {
                    Uint64 responseCount;
                    v.get(responseCount);
                    _responseCount = responseCount;
                }
                else if(paramName =="Size")
                {
                    Uint64 instanceSize;
                    v.get(instanceSize);
                    _instanceSize = instanceSize;
                }
                else
                {
                    rtnCode = 1;
                }
            }
        }

        // get method returns current _responseCount and instanceSize
        // parameters.
        else if (methodName.equal("get"))
        {

            Array<CIMParamValue> OutParams;

            OutParams.append(CIMParamValue("ResponseCount", _responseCount));
            OutParams.append(CIMParamValue("Size", (Uint64)_instanceSize));

            handler.deliverParamValue(OutParams);
            handler.deliver(Uint32(0));
        }

        // stop method set stop flag for the provider.  Hopefully this
        // would stop any existing generation of responses.  Note that
        // this flag remains set until provider is reloaded.
        // FUTURE: We do not use the method today.
        else if (methodName.equal("stop"))
        {
            Uint32 rtn = (_continue)? 0 : 1;
            _continue = false;
            handler.deliver(rtn);
        }
        else
        {
            throw CIMException(CIM_ERR_METHOD_NOT_AVAILABLE);
        }
    }
    handler.deliver(rtnCode);
    handler.complete();
}
PEGASUS_NAMESPACE_END
