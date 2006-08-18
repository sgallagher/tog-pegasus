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
//
// Author: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean verbose = false;

const String NAMESPACE = "root/test";
const String CLASSNAME = "PG_ProviderModule";
const String CLASSNAME2 = "PG_Provider";
const String CLASSNAME3 = "PG_ProviderCapabilities";

Boolean TestLookupIndicationProvider(ProviderRegistrationManager & prmanager)
{

    //
    // create Provider module instances
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
    catch(const CIMException&)
    {
        throw;
    }

    // create PG_Provider instances

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
    catch(const CIMException&)
    {
        throw;
    }

    CIMObjectPath returnRef4;

    CIMClass cimClass4(CLASSNAME2);

    CIMInstance cimInstance4(CLASSNAME2);

    cimInstance4.addProperty(CIMProperty("ProviderModuleName", String("providersModule1")));
    cimInstance4.addProperty(CIMProperty("Name", String("PG_ProviderInstance2")));

    CIMObjectPath instanceName4 = cimInstance4.buildPath(cimClass4);

    instanceName4.setNameSpace(NAMESPACE);
    instanceName4.setClassName(CLASSNAME2);

    try
    {
    	returnRef4 = prmanager.createInstance(instanceName4, cimInstance4);
    }
    catch(const CIMException&)
    {
        throw;
    }

    //
    // create provider capability instances
    //

    Array <String> namespaces;
    Array <Uint16> providerType;
    Array <String> supportedMethods;
    Array <String> supportedProperties;

    namespaces.append("test_namespace1");
    namespaces.append("test_namespace2");

    providerType.append(4);

    supportedMethods.append("test_method1");
    supportedMethods.append("test_method2");

    supportedProperties.append("p1");
    supportedProperties.append("p2");
    supportedProperties.append("p3");
    supportedProperties.append("p4");
    supportedProperties.append("p5");

    CIMObjectPath returnRef3;

    CIMClass cimClass3(CLASSNAME3);

    CIMInstance cimInstance3(CLASSNAME3);

    cimInstance3.addProperty(CIMProperty("ProviderModuleName", String("providersModule1")));
    cimInstance3.addProperty(CIMProperty("ProviderName", String("PG_ProviderInstance1")));
    cimInstance3.addProperty(CIMProperty("CapabilityID", String("capability1")));
    cimInstance3.addProperty(CIMProperty("ClassName", String("test_class1")));
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
    catch(const CIMException&)
    {
        throw;
    }

    Array <String> supportedProperties2;

    supportedProperties2.append("p1");
    supportedProperties2.append("p2");
    supportedProperties2.append("p3");
    supportedProperties2.append("p4");
    supportedProperties2.append("p6");

    CIMObjectPath returnRef5;

    CIMClass cimClass5(CLASSNAME3);

    CIMInstance cimInstance5(CLASSNAME3);

    cimInstance5.addProperty(CIMProperty("ProviderModuleName", String("providersModule1")));
    cimInstance5.addProperty(CIMProperty("ProviderName", String("PG_ProviderInstance2")));
    cimInstance5.addProperty(CIMProperty("CapabilityID", String("capability2")));
    cimInstance5.addProperty(CIMProperty("ClassName", String("test_class1")));
    cimInstance5.addProperty(CIMProperty("Namespaces", namespaces));
    cimInstance5.addProperty(CIMProperty("ProviderType", providerType));
    cimInstance5.addProperty(CIMProperty("SupportedProperties", supportedProperties2));

    CIMObjectPath instanceName5 = cimInstance5.buildPath(cimClass5);

    instanceName5.setNameSpace(NAMESPACE);
    instanceName5.setClassName(CLASSNAME3);

    try
    {
    	returnRef5 = prmanager.createInstance(instanceName5, cimInstance5);
    }
    catch(const CIMException&)
    {
        throw;
    }

    //
    // test lookupIndicationProvider Interface
    //
    String _providerName;
    String _providerModuleName2;
    Array <String> requiredProperties;

    Array <CIMInstance> providerIns;
    Array <CIMInstance> providerModuleIns;

    requiredProperties.append("p1");
    requiredProperties.append("p3");
    requiredProperties.append("p4");

    CIMPropertyList requiredPropertyList(requiredProperties);

    if (prmanager.getIndicationProviders("test_namespace1",
	"test_class1", requiredPropertyList, providerIns, providerModuleIns))
    {
	return (true);
    }
    else
    {
	return (false);
    }
}

int main(int argc, char** argv)
{
    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;
    if (verbose) cout << argv[0] << ": started" << endl;

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
    repositoryRoot.append("/repository");
    CIMRepository r (repositoryRoot) ;
    ProviderRegistrationManager prmanager(&r);

    try
    {
	if (!TestLookupIndicationProvider(prmanager))
	{
	    PEGASUS_STD(cerr) << "Error: lookupIndicationProvider Failed" << PEGASUS_STD(
endl);
            exit (-1);
	}
    }

    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	PEGASUS_STD (cout) << argv[0] << " +++++ lookup indication provider failed"
                           << PEGASUS_STD (endl);
	exit(-1);
    }

    PEGASUS_STD(cout) << argv[0] << " +++++ passed all tests" << PEGASUS_STD(endl);

    return 0;
}
