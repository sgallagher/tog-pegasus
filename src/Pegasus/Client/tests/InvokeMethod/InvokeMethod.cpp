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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/cimv2";

int main(int argc, char** argv)
{
    try
    {
      CIMClient client;
      client.connect("localhost:5988");
      
      //Indication
      CIMClass cimClass = client.getClass(NAMESPACE, "TestSoftwarePkg", false);
      CIMInstance cimInstance("TestSoftwarePkg");
      cimInstance.addProperty(CIMProperty("PkgName", "WBEM"));
      cimInstance.addProperty(CIMProperty("PkgIndex", Uint32(101)));
      cimInstance.addProperty(CIMProperty("trapOid", "1.3.6.1.4.1.11.2.3.1.7.0.4"));
      cimInstance.addProperty(CIMProperty("computerName", "NU744781"));
      CIMObjectPath instanceName = cimInstance.getInstanceName(cimClass);
      instanceName.setNameSpace(NAMESPACE);
      client.createInstance(NAMESPACE, cimInstance);
      
	Array<CIMParamValue> inParams;
	Array<CIMParamValue> outParams;
	inParams.append(CIMParamValue("param1", CIMValue("Hewlett-Packard")));
	inParams.append(CIMParamValue("param2", CIMValue("California")));
	
	CIMValue retValue = client.invokeMethod(
	    NAMESPACE, 
	    instanceName, 
	    "ChangeName", 
	    inParams, 
	    outParams);

	//cout << "Output : " << retValue.toString() << endl;
	/*for (Uint8 i = 0; i < outParams.size(); i++)
	    cout << outParams[i].getParameter().getName() 
		<< " : " 
		<< outParams[i].getValue().toString()
		<< endl;
        */
	client.deleteInstance(NAMESPACE, instanceName);
    }
    catch(CIMClientException& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	exit(1);
    }

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);
    
    return 0;
}
