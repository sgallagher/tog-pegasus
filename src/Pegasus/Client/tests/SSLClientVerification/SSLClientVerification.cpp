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
// Author: Heather Sterling (hsterl@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/cimv2");

/*
 * This is a test for basic SSL Client Verification that was introduced in PEP#165.
 *
 * The cimserver must be configured as following to test this:
 *
 *	enableHttpsConnection=true
 *  enableAuthentication=true
 *  sslClientVerificationMode=optional
 *  sslTrustStore=<directory>
 *  sslTrustStoreUserName=<valid sys user>
 * 
 * To test optional truststore automatic update:
 *
 *  PEGASUS_USE_AUTOMATIC_TRUSTSTORE_UPDATE is defined
 *  enableSSLTrustStoreAutoUpdate=true 
 *
 * Additionally, the client must have its own certificate and private key to present to the server during the handshake.
 * Finally, the client certificate must be located in the server's truststore in the correct format: "<subject-hash>.0"
 *
 */

int main(int argc, char** argv)
{

#ifdef PEGASUS_HAS_SSL
	try
	{
		String host = System::getHostName();
		Uint32 port = System::lookupPort(WBEM_HTTPS_SERVICE_NAME, WBEM_DEFAULT_HTTPS_PORT);

		//
		// Note that these files are separate from the client.pem which represents the client's truststore.
		//
		const char* pegasusHome = getenv("PEGASUS_HOME");
		String certPath = FileSystem::getAbsolutePath(pegasusHome, "clientkeystore/client_cert.pem"); 
		String keyPath = FileSystem::getAbsolutePath(pegasusHome, "clientkeystore/client_key.pem");

		PEGASUS_STD(cerr) << "certPath is " << certPath << "\n";
		PEGASUS_STD(cerr) << "keyPath is " << keyPath << "\n";

		String randPath = String::EMPTY;
#ifdef PEGASUS_SSL_RANDOMFILE
		randPath = FileSystem::getAbsolutePath(pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);
#endif

		CIMClient client;

		client.connect(
					  host, 
					  port,
					  SSLContext("", certPath, keyPath, NULL, randPath),
					  String::EMPTY, 
					  String::EMPTY);

		//
		// Do a generic call.  We have to do this call to test whether or not we get 401'ed.
		//
		CIMClass cimClass = client.getClass(
										   CIMNamespaceName("root/cimv2"), 
										   CIMName ("CIM_ManagedElement"),
										   true,
										   false,
										   false,
										   CIMPropertyList());

		client.disconnect();

	} catch (Exception& e)
	{
		PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
		PEGASUS_STD(cerr) << "Root cause could be PEGASUS_HAS_SSL is defined but enableHttpsConnection=false" << PEGASUS_STD(endl);
		exit(1);  
	}

	PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);

	return 0;

#endif

	//
	// This returns a false positive result.
	// But we should never get here since this test is only run if PEGASUS_HAS_SSL is defined.
	//

	return 0;
}
