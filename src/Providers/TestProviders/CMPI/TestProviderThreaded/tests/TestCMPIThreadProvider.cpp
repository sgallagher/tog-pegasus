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


Boolean verbose;

void
_usage ()
{
  cerr << "Usage: TestCMPIThreadProvider " << "{test}" << endl;
}

void
test01( CIMClient & client)
{  
  try {
   Array<CIMInstance> instances = client.enumerateInstances(
		CIMNamespaceName("test/TestProvider"),
		CIMName("TestCMPI_Thread"));
  } catch (const CIMException &e)
  {
	// The provided is ONLY suppose to return not supported.
	if (e.getCode() != CIM_ERR_NOT_SUPPORTED)
		throw e;
  }
}
void
executeMethod (CIMClient & client, String operation)
{

  CIMObjectPath instanceName;
  Array<CIMKeyBinding> keyBindings;
  Array<CIMParamValue> inParm;
  Array < CIMParamValue > outParm;

  keyBindings.append (CIMKeyBinding ("Name", 
	"TestCMPIThreadProviderModule", CIMKeyBinding::STRING));

  instanceName.setNameSpace ("root/PG_InterOp");
  instanceName.setClassName ("PG_ProviderModule");
  instanceName.setKeyBindings(keyBindings);

  CIMValue retVal  = client.invokeMethod ("root/PG_InterOp",
                                              instanceName,
						operation,
						inParm,
						outParm);
}


void
_test (CIMClient & client)
{
  try
  {
    test01(client);
    executeMethod (client, "stop");
    executeMethod (client, "start"); 
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

  if (argc != 2)
    {
      _usage ();
      return 1;
    }

  else
    {
      const char *opt = argv[1];

      if (String::equalNoCase (opt, "test"))
	{
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
