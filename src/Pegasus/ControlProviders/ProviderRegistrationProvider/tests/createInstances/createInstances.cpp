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
// Author: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMName CLASSNAME = CIMName ("PG_ProviderModule");
const CIMName CLASSNAME2 = CIMName ("PG_Provider");
const CIMName CLASSNAME3 = CIMName ("PG_ProviderCapabilities");

void TestCreateInstances(CIMClient& client)
{
    //
    // Test create Provider module instances
    //
    CIMObjectPath returnRef;

    CIMClass cimClass(CLASSNAME);

    CIMInstance cimInstance(CLASSNAME);

    cimInstance.addProperty(CIMProperty(CIMName ("Name"), 
        String("providersModule1")));
    cimInstance.addProperty(CIMProperty(CIMName ("Vendor"), String("HP")));
    cimInstance.addProperty(CIMProperty(CIMName ("Version"), String("2.0")));
    cimInstance.addProperty(CIMProperty(CIMName ("InterfaceType"), 
        String("C++Default")));
    cimInstance.addProperty(CIMProperty(CIMName ("InterfaceVersion"), 
        String("2.1.0")));
    cimInstance.addProperty(CIMProperty(CIMName ("Location"), 
        String("/tmp/module1")));

    CIMObjectPath instanceName = cimInstance.buildPath(cimClass);

    instanceName.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    instanceName.setClassName(CLASSNAME);

    try
    {
        returnRef = client.createInstance(PEGASUS_NAMESPACENAME_INTEROP, cimInstance);
    }
    catch(const CIMException&)
    {
        throw;
    }

    // Test create PG_Provider instances

    CIMObjectPath returnRef2;

    CIMClass cimClass2(CLASSNAME2);

    CIMInstance cimInstance2(CLASSNAME2);

    cimInstance2.addProperty(CIMProperty(CIMName ("ProviderModuleName"), 
        String("providersModule1")));
    cimInstance2.addProperty(CIMProperty(CIMName ("Name"), 
        String("PG_ProviderInstance1")));

    CIMObjectPath instanceName2 = cimInstance2.buildPath(cimClass2);

    instanceName2.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    instanceName2.setClassName(CLASSNAME2);

    try
    {
        returnRef2 = client.createInstance(PEGASUS_NAMESPACENAME_INTEROP, cimInstance2);
    }
    catch(const CIMException&)
    {
        throw;
    }

    //
    // test create provider capability instances
    //

    Array <String> namespaces;
    Array <Uint16> providerType;
    Array <String> supportedMethods;
    Array <String> supportedProperties;

    namespaces.append("root/cimv2");
    namespaces.append("root/cimv3");
    
    providerType.append(4);
    providerType.append(5);

    supportedMethods.append("test_method1");
    supportedMethods.append("test_method2");

    supportedProperties.append("PkgStatus");
    supportedProperties.append("PkgIndex");

    CIMObjectPath returnRef3;

    CIMClass cimClass3(CLASSNAME3);

    CIMInstance cimInstance3(CLASSNAME3);

    cimInstance3.addProperty(CIMProperty(CIMName ("ProviderModuleName"), 
        String("providersModule1")));
    cimInstance3.addProperty(CIMProperty(CIMName ("ProviderName"), 
        String("PG_ProviderInstance1")));
    cimInstance3.addProperty(CIMProperty(CIMName ("CapabilityID"), 
        String("capability1")));
    cimInstance3.addProperty(CIMProperty(CIMName ("ClassName"), 
        String("TestSoftwarePkg")));
    cimInstance3.addProperty(CIMProperty(CIMName ("Namespaces"), namespaces));
    cimInstance3.addProperty(CIMProperty(CIMName ("ProviderType"), 
        providerType));
    cimInstance3.addProperty(CIMProperty(CIMName ("SupportedMethods"), 
        supportedMethods));
    cimInstance3.addProperty(CIMProperty(CIMName ("SupportedProperties"), 
        supportedProperties));

    CIMObjectPath instanceName3 = cimInstance3.buildPath(cimClass3);

    instanceName3.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    instanceName3.setClassName(CLASSNAME3);

    try
    {
        returnRef3 = client.createInstance(PEGASUS_NAMESPACENAME_INTEROP, cimInstance3);
    }
    catch(const CIMException&)
    {
        throw;
    }

    CIMKeyBinding kb1(CIMName ("Name"), "providersModule1", 
        CIMKeyBinding::STRING);
    Array<CIMKeyBinding> keys;
    keys.append(kb1);

    instanceName.setKeyBindings(keys);

    // test getInstance
    try
    {
    	client.getInstance(PEGASUS_NAMESPACENAME_INTEROP, instanceName);
    }
    catch(const CIMException&)
    {
        throw;
    }

    // test enumerateInstances
    try
    {
    	client.enumerateInstances(PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER);
    }
    catch(const CIMException&)
    {
        throw;
    }

    // test enumerateInstanceNames
    try
    {
    	client.enumerateInstanceNames(PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER);
    }
    catch(const CIMException&)
    {
        throw;
    }


    client.deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, instanceName);
}

int main(int argc, char** argv)
{


    try
    {
	CIMClient client;
	
	client.connectLocal();
	TestCreateInstances(client);
    }

    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	PEGASUS_STD (cout) << "+++++ create instances failed"
                           << PEGASUS_STD (endl);
	exit(-1);
    }

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);
    
    exit (0);
}
