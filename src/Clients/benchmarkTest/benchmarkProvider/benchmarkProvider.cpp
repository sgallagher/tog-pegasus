//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Denise Eckstein, Hewlett-Packard Company
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "benchmarkProvider.h"
#include "../benchmarkDefinition/benchmarkDefinition.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define NAMESPACE CIMNamespaceName ("root/benchmark")

benchmarkDefinition test;

benchmarkProvider::benchmarkProvider(void)
{
}

benchmarkProvider::~benchmarkProvider(void)
{
}

CIMObjectPath benchmarkProvider::_buildObjectPath(
                         const CIMName& className,
                         CIMValue Identifier)
{
    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding("Idenitifier", Identifier.toString(),
               CIMKeyBinding::NUMERIC));

    return CIMObjectPath(String(), CIMNamespaceName(NAMESPACE), className, keys);
}

CIMInstance benchmarkProvider::_buildInstance(
                         const    CIMName& className,
                         Uint32   numberOfProperties,
                         Uint32   propertySize,
                         CIMValue Identifier)

{
    char propertyName[20];
    char propertyValue[10000];

    try
    {

       for (Uint32 i = 0; i < propertySize; i++)
       {
          propertyValue[i] = 'a';
       }
       propertyValue[propertySize] = 0;

       CIMInstance instance(className);
       instance.addProperty(CIMProperty("Identifier", Identifier));

       for(Uint32 i = 0; i < numberOfProperties;  i++)
       {
          sprintf(propertyName, "Property%4.4d", i);
          instance.addProperty(CIMProperty(propertyName, propertyValue));
       }

       CIMObjectPath reference = _buildObjectPath(className, Identifier);
       instance.setPath(reference);
       return(instance);
    }

    catch (CIMException& e)
    {
#ifdef DEBUG
        cout << "benchmarkProvider::initialize(): Got CIMException:";
        cout << e.getMessage() << endl;
#endif
        throw;
    }
    catch (Exception& e)
    {
#ifdef DEBUG
        cout << "benchmarkProvider::initialize(): Got Exception: ";
        cout << e.getMessage() << endl;
#endif
        throw;
    }
    catch (...)
    {
#ifdef DEBUG
        cout << "benchmarkProvider::initialize(): Got Unknown Exception: ";
#endif
        throw;
    }
}

void benchmarkProvider::initialize(CIMOMHandle & cimom)
{
}

void benchmarkProvider::terminate(void)
{
}

void benchmarkProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    CIMInstance _instance;
    Uint32 numberOfProperties;
    Uint32 sizeOfPropertyValue;
    Uint32 numberOfInstances;

    CIMName className = instanceReference.getClassName();
    test.getConfiguration(className, numberOfProperties,
                 sizeOfPropertyValue, numberOfInstances);

    Array<CIMKeyBinding> keyBindings = instanceReference.getKeyBindings();
    if (keyBindings.size() != 1)
    {
       throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }

    String Identifier = keyBindings[0].getValue();

    // begin processing the request
    handler.processing();

    _instance = _buildInstance(className, numberOfProperties,
                        sizeOfPropertyValue , CIMValue(Identifier));   
    handler.deliver(_instance);

    // complete processing the request
    handler.complete();
}

void benchmarkProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    CIMInstance _instance;
    Uint32 numberOfProperties;
    Uint32 sizeOfPropertyValue;
    Uint32 numberOfInstances;

    CIMName className = classReference.getClassName();
    test.getConfiguration(className, numberOfProperties,
                          sizeOfPropertyValue, numberOfInstances);

    // begin processing the request
    handler.processing();

    for (Uint32 i = 1; i <= numberOfInstances; i++)
    {
       _instance = _buildInstance(className, numberOfProperties,
                        sizeOfPropertyValue , CIMValue(i));   
       handler.deliver(_instance);
    }

    // complete processing the request
    handler.complete();
}

void benchmarkProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
    CIMObjectPath _instanceName;
    Uint32 numberOfProperties;
    Uint32 sizeOfPropertyValue;
    Uint32 numberOfInstances;

    CIMName className = classReference.getClassName();
    test.getConfiguration(className, numberOfProperties,
                          sizeOfPropertyValue, numberOfInstances);

    // begin processing the request
    handler.processing();

    for (Uint32 i = 1; i <= numberOfInstances; i++)
    {
       _instanceName = _buildObjectPath(className, CIMValue(i)); 
       handler.deliver(_instanceName);
    }
 
    // complete processing the request
    handler.complete();
}

void benchmarkProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	const Boolean includeQualifiers,
	const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void benchmarkProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void benchmarkProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

PEGASUS_NAMESPACE_END
