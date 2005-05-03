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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "MethodTestProvider.h"

PEGASUS_NAMESPACE_BEGIN

MethodTestProvider::MethodTestProvider()
{
}

MethodTestProvider::~MethodTestProvider()
{
}

void MethodTestProvider::initialize(CIMOMHandle& cimom)
{
    _cimom = cimom;
}

void MethodTestProvider::terminate()
{
    delete this;
}

void MethodTestProvider::invokeMethod(
    const OperationContext& context,
    const CIMObjectPath& objectReference,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    MethodResultResponseHandler& handler)
{
    if (!objectReference.getClassName().equal("Test_MethodProviderClass"))
    {
        throw CIMNotSupportedException(
            objectReference.getClassName().getString());
    }

    handler.processing();

    if (methodName.equal("test1"))
    {
        CIMValue returnValue;
        Array<CIMParamValue> inParams;
        Array<CIMParamValue> outParams;

        inParams.append(CIMParamValue("InParam1", CIMValue(Uint32(1))));
        inParams.append(CIMParamValue("InParam2", CIMValue(Uint32(2))));

        CIMObjectPath localReference = CIMObjectPath(
            String(),
            CIMNamespaceName(),
            objectReference.getClassName(),
            objectReference.getKeyBindings());

        returnValue = _cimom.invokeMethod(
            context,
            objectReference.getNameSpace(),
            localReference,
            CIMName("test2"),
            inParams,
            outParams);

        Uint32 rc;
        returnValue.get(rc);
        if (rc != 10)
        {
            throw CIMOperationFailedException("Incorrect return value");
        }

        if (outParams.size() != 2)
        {
            throw CIMOperationFailedException("Incorrect out parameters");
        }

        Uint32 outParam1;
        Uint32 outParam2;
 
        for (Uint32 i = 0; i < 2; i++)
        {
            if (outParams[i].getParameterName() == "OutParam1")
            {
                outParams[i].getValue().get(outParam1);
            }
            else if (outParams[i].getParameterName() == "OutParam2")
            {
                outParams[i].getValue().get(outParam2);
            }
            else
            {
                throw CIMOperationFailedException("Incorrect out parameters");
            }
        }

        if ((outParam1 != 21) || (outParam2 != 32))
        {
            throw CIMOperationFailedException("Incorrect out parameters");
        }

        handler.deliverParamValue(outParams[0]);
        handler.deliverParamValue(outParams[1]);
        handler.deliver(returnValue);
    }
    else if (methodName.equal("test2"))
    {
        if (inParameters.size() != 2)
        {
            throw CIMOperationFailedException("Incorrect in parameters");
        }

        Uint32 inParam1;
        Uint32 inParam2;

        for (Uint32 i = 0; i < 2; i++)
        {
            if (inParameters[i].getParameterName() == "InParam1")
            {
                inParameters[i].getValue().get(inParam1);
            }
            else if (inParameters[i].getParameterName() == "InParam2")
            {
                inParameters[i].getValue().get(inParam2);
            }
            else
            {
                throw CIMOperationFailedException("Incorrect in parameters");
            }
        }

        if ((inParam1 != 1) || (inParam2 != 2))
        {
            throw CIMOperationFailedException("Incorrect in parameters");
        }

        handler.deliverParamValue(CIMParamValue("OutParam1", Uint32(21)));
        handler.deliverParamValue(CIMParamValue("OutParam2", Uint32(32)));
        handler.deliver(Uint32(10));
    }
    else
    {
        throw CIMNotSupportedException(methodName.getString());
    }

    handler.complete();
}

PEGASUS_NAMESPACE_END
