//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String CLASSNAME = "PG_ProviderModule";
const String CLASSNAME2 = "PG_Provider";
const String CLASSNAME3 = "PG_ProviderCapabilities";

void TestCreateInstances(CIMClient& client)
{
    //
    // Test create Provider module instances
    //
    CIMReference returnRef;

    CIMClass cimClass(CLASSNAME);

    CIMInstance cimInstance(CLASSNAME);

    cimInstance.addProperty(CIMProperty("Name", "providersModule1"));
    cimInstance.addProperty(CIMProperty("Vendor", "HP"));
    cimInstance.addProperty(CIMProperty("Version", "2.0"));
    cimInstance.addProperty(CIMProperty("InterfaceType", "PG_DefaultC++"));
    cimInstance.addProperty(CIMProperty("InterfaceVersion", "2.0"));
    cimInstance.addProperty(CIMProperty("Location", "/tmp/module1"));

    CIMReference instanceName = cimInstance.getInstanceName(cimClass);

    instanceName.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    instanceName.setClassName(CLASSNAME);

    try
    {
        returnRef = client.createInstance(PEGASUS_NAMESPACENAME_INTEROP, cimInstance);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    // Test create PG_Provider instances

    CIMReference returnRef2;

    CIMClass cimClass2(CLASSNAME2);

    CIMInstance cimInstance2(CLASSNAME2);

    cimInstance2.addProperty(CIMProperty("ProviderModuleName", "providersModule1"));
    cimInstance2.addProperty(CIMProperty("Name", "PG_ProviderInstance1"));

    CIMReference instanceName2 = cimInstance2.getInstanceName(cimClass2);

    instanceName2.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    instanceName2.setClassName(CLASSNAME2);

    try
    {
	returnRef2 = client.createInstance(PEGASUS_NAMESPACENAME_INTEROP, cimInstance2);
    }
    catch(CIMException& e)
    {
        throw (e);
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

    CIMReference returnRef3;

    CIMClass cimClass3(CLASSNAME3);

    CIMInstance cimInstance3(CLASSNAME3);

    cimInstance3.addProperty(CIMProperty("ProviderModuleName", "providersModule1"));
    cimInstance3.addProperty(CIMProperty("ProviderName", "PG_ProviderInstance1"));
    cimInstance3.addProperty(CIMProperty("CapabilityID", "capability1"));
    cimInstance3.addProperty(CIMProperty("ClassName", "TestSoftwarePkg"));
    cimInstance3.addProperty(CIMProperty("Namespaces", namespaces));
    cimInstance3.addProperty(CIMProperty("ProviderType", providerType));
    cimInstance3.addProperty(CIMProperty("SupportedMethods", supportedMethods));
    cimInstance3.addProperty(CIMProperty("SupportedProperties", supportedProperties));

    CIMReference instanceName3 = cimInstance3.getInstanceName(cimClass3);

    instanceName3.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    instanceName3.setClassName(CLASSNAME3);

    try
    {
        returnRef3 = client.createInstance(PEGASUS_NAMESPACENAME_INTEROP, cimInstance3);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    KeyBinding kb1("Name", "providersModule1", KeyBinding::STRING);
    Array<KeyBinding> keys;
    keys.append(kb1);

    instanceName.setKeyBindings(keys);
    client.deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, instanceName);
}

int main(int argc, char** argv)
{


//    CIMRepository r("../repository") ;

//   ProviderRegistrationManager prmanager(&r);

    try
    {
//	Monitor* monitor = new Monitor;
//	HTTPConnector* httpConnector = new HTTPConnector(monitor);
//	CIMClient client(monitor, httpConnector);
	CIMClient client;
	
	client.connect("localhost:5988");
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
