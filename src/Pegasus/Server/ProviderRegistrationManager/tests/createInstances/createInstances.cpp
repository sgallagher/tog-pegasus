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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/cimv2";
const String CLASSNAME = "PG_ProviderModule";
const String CLASSNAME2 = "PG_Provider";
const String CLASSNAME3 = "PG_ProviderCapabilities";

void TestCreateInstances(ProviderRegistrationManager prmanager)
{
    //
    // Test create Provider module instances
    //
    CIMObjectPath returnRef;

    CIMClass cimClass(CLASSNAME);

    CIMInstance cimInstance(CLASSNAME);

    cimInstance.addProperty(CIMProperty("Name", String("providersModule1")));
    cimInstance.addProperty(CIMProperty("Vendor", String("HP")));
    cimInstance.addProperty(CIMProperty("Version", String("2.0")));
    cimInstance.addProperty(CIMProperty("InterfaceType", String("PG_DefaultC++")));
    cimInstance.addProperty(CIMProperty("InterfaceVersion", String("2.0")));
    cimInstance.addProperty(CIMProperty("Location", String("/tmp/module1")));

    CIMObjectPath instanceName = cimInstance.buildPath(cimClass);

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

    // Test create PG_Provider instances

    CIMObjectPath returnRef2;

    CIMClass cimClass2(CLASSNAME2);

    CIMInstance cimInstance2(CLASSNAME2);

    cimInstance2.addProperty(CIMProperty("ProviderModuleName", String("providersModule1")));
    cimInstance2.addProperty(CIMProperty("Name", String("PG_ProviderInstance1")));

    CIMObjectPath instanceName2 = cimInstance2.buildPath(cimClass2);

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
    // test create provider capability instances
    //

    Array <String> namespaces;
    Array <Uint16> providerType;
    Array <String> supportedMethods;
    Array <String> supportedProperties;

    namespaces.append("root/cimv2");
    namespaces.append("root/cimv3");
    
    providerType.append(2);
    providerType.append(5);

    supportedMethods.append("test_method1");
    supportedMethods.append("test_method2");

    supportedProperties.append("PkgStatus");
    supportedProperties.append("PkgIndex");

    CIMObjectPath returnRef3;

    CIMClass cimClass3(CLASSNAME3);

    CIMInstance cimInstance3(CLASSNAME3);

    cimInstance3.addProperty(CIMProperty("ProviderModuleName", String("providersModule1")));
    cimInstance3.addProperty(CIMProperty("ProviderName", String("PG_ProviderInstance1")));
    cimInstance3.addProperty(CIMProperty("CapabilityID", String("capability1")));
    cimInstance3.addProperty(CIMProperty("ClassName", String("TestSoftwarePkg")));
    cimInstance3.addProperty(CIMProperty("Namespaces", namespaces));
    cimInstance3.addProperty(CIMProperty("ProviderType", providerType));
    cimInstance3.addProperty(CIMProperty("SupportedMethods", supportedMethods));
    cimInstance3.addProperty(CIMProperty("SupportedProperties", supportedProperties));

    CIMObjectPath instanceName3 = cimInstance3.buildPath(cimClass3);

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
}

int main(int argc, char** argv)
{
    const char* tmpDir = getenv ("PEGASUS_TMP");
    String repositoryRoot;
    if (tmpDir == NULL)
    {
        repositoryRoot = ".";
    }
    else
    {
        repositoryRoot = tmpDir;
    }
    repositoryRoot += "/repository";


    CIMRepository r (repositoryRoot) ;

    ProviderRegistrationManager prmanager(&r);

    try
    {
	TestCreateInstances(prmanager);
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
