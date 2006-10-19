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
// Author: Konrad Rzeszutek <konradr@us.ibm.com>
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;


CIMNamespaceName ProviderNamespace;
const CIMName CLASSNAME = CIMName ("TestCMPI_Method");

Boolean verbose;


void _checkStringValue
  (CIMValue & theValue, const String & value, Boolean null = false)
{
  PEGASUS_TEST_ASSERT (theValue.getType () == CIMTYPE_STRING);
  PEGASUS_TEST_ASSERT (!theValue.isArray ());
  if (null)
    {
      PEGASUS_TEST_ASSERT (theValue.isNull ());
    }
  else
    {
      PEGASUS_TEST_ASSERT (!theValue.isNull ());
      String result;
      theValue.get (result);

      if (verbose)
	{
	  if (result != value)
	    {
	      cerr << "Property value comparison failed.  ";
	      cerr << "Expected " << value << "; ";
	      cerr << "Actual property value was " << result << "." << endl;
	    }
	}

      PEGASUS_TEST_ASSERT (result == value);
    }
}


void
_checkUint32Value (CIMValue & theValue, Uint32 value)
{

  PEGASUS_TEST_ASSERT (theValue.getType () == CIMTYPE_UINT32);
  PEGASUS_TEST_ASSERT (!theValue.isArray ());
  PEGASUS_TEST_ASSERT (!theValue.isNull ());

  Uint32 result;
  theValue.get (result);

  if (verbose)
    {
      if (result != value)
	{
	  cerr << "Property value comparison failed.  ";
	  cerr << "Expected " << value << "; ";
	  cerr << "Actual property value was " << result << "." << endl;
	}
    }

  PEGASUS_TEST_ASSERT (result == value);
}




void
_usage ()
{
  cerr << "Usage: TestCMPIMethod " << "{test}" << endl;
}

void
test01 (CIMClient & client)
{
  CIMObjectPath instanceName;

  instanceName.setNameSpace (ProviderNamespace);
  instanceName.setClassName (CLASSNAME);

  Array < CIMParamValue > inParams;
  Array < CIMParamValue > outParams;

  /*     String returnUint32(); */

  CIMValue retValue = client.invokeMethod (ProviderNamespace,
					   instanceName,
					   "returnUint32",
					   inParams,
					   outParams);
  _checkUint32Value (retValue, 42);
}

void
test02 (CIMClient & client)
{
  CIMObjectPath instanceName;

  instanceName.setNameSpace (ProviderNamespace);
  instanceName.setClassName (CLASSNAME);

  Array < CIMParamValue > inParams;
  Array < CIMParamValue > outParams;

  /*     String returnString(); */

  CIMValue retValue = client.invokeMethod (ProviderNamespace,
					   instanceName,
					   "returnString",
					   inParams,
					   outParams);

  _checkStringValue (retValue, "Returning string", false);

}

void
test03 (CIMClient & client)
{
  CIMObjectPath instanceName;

  instanceName.setNameSpace (ProviderNamespace);
  instanceName.setClassName (CLASSNAME);

  Array < CIMParamValue > inParams;
  Array < CIMParamValue > outParams;

  /*     uint32 TestCMPIBroker (
   *          [IN, Description (
   *          "The requested are to test different CMPI data structure operations."),
   *          ValueMap { "1", "2", "3", "4", "5", "6"},
   *          Values {"CDGetType", "CDToString", "CDIsOfType", "CMGetMessage",  "CMLogMessage","CDTraceMessage"}]
   *          uint32 Operation,
   *          [OUT, Description (
   *          " The result of what the operation carried out.")]
   *          string Result);
   *
   */
  {
    /* CDGetType */
    inParams.append (CIMParamValue ("Operation", CIMValue (Uint32 (1))));
    CIMValue retValue = client.invokeMethod (ProviderNamespace,
					     instanceName,
					     "TestCMPIBroker",
					     inParams,
					     outParams);
    // Check the return value. Make sure it is 0.
    _checkUint32Value (retValue, 0);

    PEGASUS_TEST_ASSERT (outParams.size () == 1);
    CIMValue paramValue = outParams[0].getValue ();
    _checkStringValue (paramValue, "CMPIArgs");
  }
  inParams.clear ();
  outParams.clear ();
  {
    /* CDToString */
    inParams.append (CIMParamValue ("Operation", CIMValue (Uint32 (2))));
    CIMValue retValue = client.invokeMethod (ProviderNamespace,
					     instanceName,
					     "TestCMPIBroker",
					     inParams,
					     outParams);
    // Check the return value. Make sure it is 0.
    _checkUint32Value (retValue, 0);

    PEGASUS_TEST_ASSERT (outParams.size () == 1);
    CIMValue paramValue = outParams[0].getValue ();
    _checkStringValue (paramValue, " Operation:2\n");
  }
  inParams.clear ();
  outParams.clear ();
  {
    /* CDIsOfType */
    inParams.append (CIMParamValue ("Operation", CIMValue (Uint32 (3))));
    CIMValue retValue = client.invokeMethod (ProviderNamespace,
					     instanceName,
					     "TestCMPIBroker",
					     inParams,
					     outParams);
    // Check the return value. Make sure it is 0.
    _checkUint32Value (retValue, 0);

    PEGASUS_TEST_ASSERT (outParams.size () == 1);
    CIMValue paramValue = outParams[0].getValue ();
    _checkStringValue (paramValue, "++++ CMPIArgs = Yes");
  }
  inParams.clear ();
  outParams.clear ();
  {
    /* CDGetMessage */
    inParams.append (CIMParamValue ("Operation", CIMValue (Uint32 (4))));
    CIMValue retValue = client.invokeMethod (ProviderNamespace,
					     instanceName,
					     "TestCMPIBroker",
					     inParams,
					     outParams);
    // Check the return value. Make sure it is 0.
    _checkUint32Value (retValue, 0);

    PEGASUS_TEST_ASSERT (outParams.size () == 1);
    CIMValue paramValue = outParams[0].getValue ();
    _checkStringValue (paramValue, "CIM_ERR_SUCCESS: Successful.");
  }
  inParams.clear ();
  outParams.clear ();
  {
    /* CDLogMessage */
    inParams.append (CIMParamValue ("Operation", CIMValue (Uint32 (5))));
    CIMValue retValue = client.invokeMethod (ProviderNamespace,
					     instanceName,
					     "TestCMPIBroker",
					     inParams,
					     outParams);
    // Check the return value. Make sure it is 0.
    _checkUint32Value (retValue, 0);
    // Nothing is returned
    PEGASUS_TEST_ASSERT (outParams.size () == 1);

    CIMValue paramValue = outParams[0].getValue ();
    PEGASUS_TEST_ASSERT (paramValue.isNull ());

  }
  inParams.clear ();
  outParams.clear ();
  {
    /* CDTraceMessage */
    inParams.append (CIMParamValue ("Operation", CIMValue (Uint32 (6))));
    CIMValue retValue = client.invokeMethod (ProviderNamespace,
					     instanceName,
					     "TestCMPIBroker",
					     inParams,
					     outParams);
    // Check the return value. Make sure it is 0.
    _checkUint32Value (retValue, 0);
    // Nothing is returned
    PEGASUS_TEST_ASSERT (outParams.size () == 1);

    CIMValue paramValue = outParams[0].getValue ();
    PEGASUS_TEST_ASSERT (paramValue.isNull ());

  }
}
void
test04 (CIMClient & client)
{
  CIMObjectPath instanceName;

  instanceName.setNameSpace (ProviderNamespace);
  instanceName.setClassName (CLASSNAME);

  Array < CIMParamValue > inParams;
  Array < CIMParamValue > outParams;

  /*     [EmbeddedObject] String returnInstance(); */

  CIMValue retValue = client.invokeMethod (ProviderNamespace,
					   instanceName,
					   "returnInstance",
					   inParams,
					   outParams);

  PEGASUS_TEST_ASSERT (retValue.getType () == CIMTYPE_OBJECT);
  PEGASUS_TEST_ASSERT (!retValue.isArray ());
  PEGASUS_TEST_ASSERT (!retValue.isNull ());

  CIMObject result;
  retValue.get (result);

  CIMObjectPath objPath  = result.getPath();
  PEGASUS_TEST_ASSERT (objPath.toString() == "TestCMPI_Instance");

}
void
test05 (CIMClient & client)
{
  CIMObjectPath instanceName;

  instanceName.setNameSpace (ProviderNamespace);
  instanceName.setClassName (CLASSNAME);

  Array < CIMParamValue > inParams;
  Array < CIMParamValue > outParams;

  /*     String returnDateTime(); */

  CIMValue retValue = client.invokeMethod (ProviderNamespace,
					   instanceName,
					   "returnDateTime",
					   inParams,
					   outParams);
  PEGASUS_TEST_ASSERT (retValue.getType () == CIMTYPE_DATETIME);
  PEGASUS_TEST_ASSERT (!retValue.isArray ());
  PEGASUS_TEST_ASSERT (!retValue.isNull ());

}
void
test06 (CIMClient & client)
{
  CIMObjectPath instanceName;
  Uint32 exception  =0;
  instanceName.setNameSpace (ProviderNamespace);
  instanceName.setClassName (CLASSNAME);

  Array < CIMParamValue > inParams;
  Array < CIMParamValue > outParams;

  /*     String returnDateTime(); */
  try {
  CIMValue retValue = client.invokeMethod (ProviderNamespace,
					   instanceName,
					   "noSuchFunction",
					   inParams,
					   outParams);
  } catch (const CIMException &e)
  {
	  exception ++;
  	PEGASUS_TEST_ASSERT (e.getCode() == CIM_ERR_NOT_FOUND);
  }
  PEGASUS_TEST_ASSERT (exception == 1);
}
void
test07 (CIMClient & client)
{
  CIMObjectPath instanceName;
  Uint32 exception  =0;
  instanceName.setNameSpace (ProviderNamespace);
  instanceName.setClassName (CLASSNAME);

  Array < CIMParamValue > inParams;
  Array < CIMParamValue > outParams;

    inParams.append (CIMParamValue ("Operation", CIMValue (String ("Boom"))));
  CIMValue retValue = client.invokeMethod (ProviderNamespace,
					   instanceName,
					   "TestCMPIBroker",
					   inParams,
					   outParams);
    _checkUint32Value (retValue, 1);
}
void
test08 (CIMClient & client)
{
  CIMObjectPath instanceName;
  Uint32 exception  =0;
  instanceName.setNameSpace (ProviderNamespace);
  instanceName.setClassName (CLASSNAME);

  Array < CIMParamValue > inParams;
  Array < CIMParamValue > outParams;

  inParams.append (CIMParamValue ("Operation", CIMValue (Uint64 (1))));
  CIMValue retValue = client.invokeMethod (ProviderNamespace,
					   instanceName,
					   "TestCMPIBroker",
					   inParams,
					   outParams);
    _checkUint32Value (retValue, 1);
}

/**
 * This tests the embedded instance functionality through the CMPI Test
 * Method Provider. It first invokes the returnInstance() method to retrieve
 * an instance that can be used 
 */
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
void test09 (CIMClient & client)
{
  CIMObjectPath instanceName;

  instanceName.setNameSpace (ProviderNamespace);
  instanceName.setClassName (CLASSNAME);

  Array < CIMParamValue > inParams;
  Array < CIMParamValue > outParams;

  /*     [EmbeddedObject] String returnInstance(); */

  CIMValue retValue = client.invokeMethod (ProviderNamespace,
					   instanceName,
					   "returnInstance",
					   inParams,
					   outParams);

  PEGASUS_TEST_ASSERT (retValue.getType () == CIMTYPE_OBJECT);
  PEGASUS_TEST_ASSERT (!retValue.isArray ());
  PEGASUS_TEST_ASSERT (!retValue.isNull ());

  CIMObject result;
  retValue.get (result);

  CIMObjectPath objPath  = result.getPath();

  CIMInstance inputInstance(result);
  CIMInstance outputInstance;

  inParams.append(
      CIMParamValue(String("inputInstance"), CIMValue(inputInstance)));

  retValue = client.invokeMethod (ProviderNamespace,
      instanceName,
      "processEmbeddedInstance",
      inParams,
      outParams);

  // First test the return value
  PEGASUS_TEST_ASSERT(retValue.getType() == CIMTYPE_INSTANCE);
  PEGASUS_TEST_ASSERT(!retValue.isArray());
  PEGASUS_TEST_ASSERT(!retValue.isNull());
  retValue.get(outputInstance);
  PEGASUS_TEST_ASSERT(objPath.toString() ==
    outputInstance.getPath().toString());
  PEGASUS_TEST_ASSERT(outputInstance.getPropertyCount() ==
    inputInstance.getPropertyCount());
  for(unsigned int i = 0, n = outputInstance.getPropertyCount(); i < n; ++i)
  {
    CIMProperty outputProp(outputInstance.getProperty(i));
    CIMProperty inputProp(inputInstance.getProperty(i));

    PEGASUS_TEST_ASSERT(outputProp.getName() == inputProp.getName());
    PEGASUS_TEST_ASSERT(outputProp.getValue() == inputProp.getValue());
  }

  // Now test the output parameter
  PEGASUS_TEST_ASSERT(outParams.size() == 1);
  retValue = outParams[0].getValue();

  PEGASUS_TEST_ASSERT(retValue.getType() == CIMTYPE_INSTANCE);
  PEGASUS_TEST_ASSERT(!retValue.isArray());
  PEGASUS_TEST_ASSERT(!retValue.isNull());
  retValue.get(outputInstance);
  PEGASUS_TEST_ASSERT(objPath.toString() ==
    outputInstance.getPath().toString());
  PEGASUS_TEST_ASSERT(outputInstance.getPropertyCount() ==
    inputInstance.getPropertyCount());
  for(unsigned int i = 0, n = outputInstance.getPropertyCount(); i < n; ++i)
  {
    CIMProperty outputProp(outputInstance.getProperty(i));
    CIMProperty inputProp(inputInstance.getProperty(i));

    PEGASUS_TEST_ASSERT(outputProp.getName() == inputProp.getName());
    PEGASUS_TEST_ASSERT(outputProp.getValue() == inputProp.getValue());
  }
}
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT

void
_test (CIMClient & client)
{
  try
  {

    test01 (client);
    test02 (client);
    test03 (client);
    test04 (client);
    test05 (client);
    test06 (client);
    test07 (client);
    test08 (client);
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
    // Don't run against the remote-namespace, not yet suppoted. 
    if (ProviderNamespace == "test/TestProvider")
    {
        test09 (client); // Embedded Instance Test
    }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
    
  }
  catch (Exception & e)
  {
    cerr << "test failed: " << e.getMessage () << endl;
    exit (-1);
  }

  cout << "+++++ test completed successfully" << endl;
}


int
main (int argc, char **argv)
{
  verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;
  CIMClient client;
  try
  {
    client.connectLocal ();
  }
  catch (Exception & e)
  {
    cerr << e.getMessage () << endl;
    return -1;
  }

  if (argc != 3)
    {
      _usage ();
      return 1;
    }

  else
    {
      const char *opt = argv[1];

      if (String::equalNoCase (opt, "test"))
	{
          ProviderNamespace = CIMNamespaceName (argv[2]);
	  _test (client);
	}
      else
	{
	  cerr << "Invalid option: " << opt << endl;
	  _usage ();
	  return -1;
	}
    }

  return 0;
}
