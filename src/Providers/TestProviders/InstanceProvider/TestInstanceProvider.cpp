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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>

#include <Pegasus/Common/CIMDateTime.h>

#include "TestInstanceProvider.h"

#include <iostream>

PEGASUS_NAMESPACE_BEGIN

TestInstanceProvider::TestInstanceProvider(void)
{
}

TestInstanceProvider::~TestInstanceProvider(void)
{
}

void TestInstanceProvider::initialize(CIMOMHandle & cimom)
{
	// save cimom handle
	//_cimom = cimom;

	// create default instances
    PEGASUS_STD(cout) << "creating default instances" << PEGASUS_STD(endl);

    {
        CIMInstance instance("TST_Instance1");

        instance.addProperty(CIMProperty("name", String("001")));

        instance.setPath(CIMObjectPath("TST_Instance1.Name=\"001\""));

        _instances.append(instance);
    }

    {
        CIMInstance instance("TST_Instance1");

        instance.addProperty(CIMProperty("name", String("002")));

        instance.setPath(CIMObjectPath("TST_Instance1.Name=\"002\""));

        _instances.append(instance);
    }

    {
        CIMInstance instance("TST_Instance1");

        instance.addProperty(CIMProperty("name", String("003")));

        instance.setPath(CIMObjectPath("TST_Instance1.Name=\"003\""));

        _instances.append(instance);
    }

    // create instances without object paths
    PEGASUS_STD(cout) << "creating instances without paths" << PEGASUS_STD(endl);

    {
        CIMInstance instance("TST_Instance1");

        instance.addProperty(CIMProperty("name", String("004")));

        //instance.setPath(CIMObjectPath("TST_Instance1.Name=\"003\""));

        _instances.append(instance);
    }

    // create instances with incorrect class name
    PEGASUS_STD(cout) << "creating instances without incorrect instance name" << PEGASUS_STD(endl);

    {
        CIMInstance instance("TST_InstanceBAD");

        instance.addProperty(CIMProperty("name", String("005")));

        instance.setPath(CIMObjectPath("TST_Instance1.Name=\"005\""));

        _instances.append(instance);
    }
}

void TestInstanceProvider::terminate(void)
{
}

void TestInstanceProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    handler.processing();

    for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    {
        if(instanceReference == _instances[i].getPath())
        {
            PEGASUS_STD(cout) << "delivering " << _instances[i].getPath().toString() << PEGASUS_STD(endl);

            handler.deliver(_instances[0]);

            break;
        }
    }

    handler.complete();
}

void TestInstanceProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & ref,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    handler.processing();

    for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    {
        PEGASUS_STD(cout) << "delivering " << _instances[i].getPath().toString() << PEGASUS_STD(endl);

        handler.deliver(_instances[i]);
    }

    handler.complete();
}

void TestInstanceProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
    handler.processing();

    for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    {
        PEGASUS_STD(cout) << "delivering " << _instances[i].getPath().toString() << PEGASUS_STD(endl);

        handler.deliver(_instances[i].getPath());
    }

    handler.complete();
}

void TestInstanceProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	const Boolean includeQualifiers,
	const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void TestInstanceProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void TestInstanceProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

PEGASUS_NAMESPACE_END
