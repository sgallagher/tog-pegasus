//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/test";
const String CLASSNAME = "PG_ProviderModule";
const String CLASSNAME2 = "PG_Provider";
const String CLASSNAME3 = "PG_ProviderCapabilities";

Boolean TestLookupMethodProvider(ProviderRegistrationManager prmanager)
{

    //
    // create Provider module instances
    //
    CIMObjectPath returnRef;

    CIMClass cimClass(CLASSNAME);

    CIMInstance cimInstance(CLASSNAME);

    cimInstance.addProperty(CIMProperty("Name", "providersModule1"));
    cimInstance.addProperty(CIMProperty("Vendor", "HP"));
    cimInstance.addProperty(CIMProperty("Version", "2.0"));
    cimInstance.addProperty(CIMProperty("InterfaceType", "PG_DefaultC++"));
    cimInstance.addProperty(CIMProperty("InterfaceVersion", "2.0"));
    cimInstance.addProperty(CIMProperty("Location", "/tmp/module1"));

    CIMObjectPath instanceName = cimInstance.getInstanceName(cimClass);

    instanceName.setNameSpace(NAMESPACE);
    instanceName.setClassName(CLASSNAME);

    try
    {
    	returnRef = prmanager.createInstance(instanceName, cimInstance);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    // create PG_Provider instances

    CIMObjectPath returnRef2;

    CIMClass cimClass2(CLASSNAME2);

    CIMInstance cimInstance2(CLASSNAME2);

    cimInstance2.addProperty(CIMProperty("ProviderModuleName", "providersModule1"));
    cimInstance2.addProperty(CIMProperty("Name", "PG_ProviderInstance1"));

    CIMObjectPath instanceName2 = cimInstance2.getInstanceName(cimClass2);

    instanceName2.setNameSpace(NAMESPACE);
    instanceName2.setClassName(CLASSNAME2);

    try
    {
    	returnRef2 = prmanager.createInstance(instanceName2, cimInstance2);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    //
    // create provider capability instances
    //

    Array <String> namespaces;
    Array <Uint16> providerType;
    Array <String> supportedMethods;

    namespaces.append("test_namespace1");
    namespaces.append("test_namespace2");
    
    providerType.append(2);
    providerType.append(5);

    supportedMethods.append("test_method1");
    supportedMethods.append("test_method2");

    CIMObjectPath returnRef3;

    CIMClass cimClass3(CLASSNAME3);

    CIMInstance cimInstance3(CLASSNAME3);

    cimInstance3.addProperty(CIMProperty("ProviderModuleName", "providersModule1"));
    cimInstance3.addProperty(CIMProperty("ProviderName", "PG_ProviderInstance1"));
    cimInstance3.addProperty(CIMProperty("CapabilityID", "capability1"));
    cimInstance3.addProperty(CIMProperty("ClassName", "test_class1"));
    cimInstance3.addProperty(CIMProperty("Namespaces", namespaces));
    cimInstance3.addProperty(CIMProperty("ProviderType", providerType));
    cimInstance3.addProperty(CIMProperty("SupportedMethods", supportedMethods));

    CIMObjectPath instanceName3 = cimInstance3.getInstanceName(cimClass3);

    instanceName3.setNameSpace(NAMESPACE);
    instanceName3.setClassName(CLASSNAME3);

    try 
    {
    	returnRef3 = prmanager.createInstance(instanceName3, cimInstance3);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    //
    // test lookupMethodProvider Interface
    //
    String _providerModuleName;
    String _providerModuleName2;

    CIMInstance providerIns;
    CIMInstance providerModuleIns;

    if (prmanager.lookupMethodProvider("test_namespace1", "test_class1", 
	"test_method2", providerIns, providerModuleIns))
    {
	providerIns.getProperty(providerIns.findProperty
	    ("ProviderModuleName")).getValue().get(_providerModuleName);

	providerModuleIns.getProperty(providerModuleIns.findProperty("Name"))
	    .getValue().get(_providerModuleName2);

	if (String::equal(_providerModuleName, _providerModuleName2))
	{
	    return (true);
	}
	else
	{
	    return (false);
	}
    }
    else
    {
	return (false);
    }
}

int main(int argc, char** argv)
{
    CIMRepository r("../repository") ;
    ProviderRegistrationManager prmanager(&r);

    try
    {
	if (!TestLookupMethodProvider(prmanager))
	{
	    PEGASUS_STD(cerr) << "Error: lookupMethodProvider Failed" << PEGASUS_STD(endl);
	    exit (-1);
	}
    }

    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	PEGASUS_STD (cout) << "+++++ lookup method provider failed"
                           << PEGASUS_STD (endl);
	exit(-1);
    }

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);
    
    exit (0);
}
