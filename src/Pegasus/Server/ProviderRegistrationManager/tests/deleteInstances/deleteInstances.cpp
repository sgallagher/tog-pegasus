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
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/test";
const String CLASSNAME = "PG_ProviderModule";
const String CLASSNAME2 = "PG_Provider";
const String CLASSNAME3 = "PG_ProviderCapabilities";

void TestDeleteInstances(ProviderRegistrationManager prmanager)
{
   KeyBinding kb1("ProviderModuleName", "providersModule1", KeyBinding::STRING);
   KeyBinding kb2("ProviderName", "PG_ProviderInstance1", KeyBinding::STRING);
   KeyBinding kb3("CapabilityID", "capability1", KeyBinding::STRING);
   KeyBinding kbp1("ProviderModuleName", "providersModule1", KeyBinding::STRING);
   KeyBinding kbp2("Name", "PG_ProviderInstance1", KeyBinding::STRING);
   KeyBinding kbm1("Name", "providersModule1", KeyBinding::STRING);

   CIMObjectPath instanceName;

  try
  {
    for (Uint32 i=1; i<=3; i++)
    {
      CIMObjectPath instanceName2;
      Array<KeyBinding> keys2;
      Array<KeyBinding> keysm;
      Array<KeyBinding> keys;

      //
      // create Provider module instances
      //
      if (i <= 1)
      {
        CIMObjectPath returnRef;
        CIMClass cimClass(CLASSNAME);
        CIMInstance cimInstance(CLASSNAME);

    	cimInstance.addProperty(CIMProperty("Name", "providersModule1"));
    	cimInstance.addProperty(CIMProperty("Vendor", "HP"));
    	cimInstance.addProperty(CIMProperty("Version", "2.0"));
    	cimInstance.addProperty(CIMProperty("InterfaceType", "PG_DefaultC++"));
    	cimInstance.addProperty(CIMProperty("InterfaceVersion", "2.0"));
    	cimInstance.addProperty(CIMProperty("Location", "/tmp/module1"));

    	instanceName = cimInstance.getInstanceName(cimClass);

    	instanceName.setNameSpace(NAMESPACE);
    	instanceName.setClassName(CLASSNAME);

    	returnRef = prmanager.createInstance(instanceName, cimInstance);
    }

    // create PG_Provider instances

    if (i <= 2 )
    {
    	CIMObjectPath returnRef2;

    	CIMClass cimClass2(CLASSNAME2);

    	CIMInstance cimInstance2(CLASSNAME2);

    	cimInstance2.addProperty(CIMProperty("ProviderModuleName", "providersModule1"));
    	cimInstance2.addProperty(CIMProperty("Name", "PG_ProviderInstance1"));

    	instanceName2 = cimInstance2.getInstanceName(cimClass2);

    	instanceName2.setNameSpace(NAMESPACE);
    	instanceName2.setClassName(CLASSNAME2);

    	returnRef2 = prmanager.createInstance(instanceName2, cimInstance2);
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

    returnRef3 = prmanager.createInstance(instanceName3, cimInstance3);

    switch (i)
    {
	case 1:
            //
            // delete cimInstance2
            //
            keys2.append(kbp1);
            keys2.append(kbp2);

            instanceName2.setKeyBindings(keys2);

    	    prmanager.deleteInstance(instanceName2);
	    break;

	case 2:
            //
            // delete cimInstance3
            //
            keys.append(kb1);
            keys.append(kb2);
            keys.append(kb3);

            instanceName3.setKeyBindings(keys);

    	    prmanager.deleteInstance(instanceName3);
	    break;

	case 3:
    	    //
    	    // delete cimInstance
            //
            keysm.append(kbm1);

            instanceName.setKeyBindings(keysm);

	    prmanager.deleteInstance(instanceName);
	    break;
        }
    }
  }
  catch(CIMException& e)
  {
      throw (e);
  }
}

int main(int argc, char** argv)
{


    CIMRepository r("../repository") ;

    ProviderRegistrationManager prmanager(&r);

    try
    {
	TestDeleteInstances(prmanager);
    }

    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	PEGASUS_STD (cout) << "+++++ delete instances failed"
                           << PEGASUS_STD (endl);
	exit(-1);
    }

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);
    
    exit (0);
}
