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
// Author: Paulo F. Borges (pfborges@wowmail.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _NTPTESTCLIENT_H
#define _NTPTESTCLIENT_H

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>	
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

// Used defines
static const String NAMESPACE("root/cimv2");	
static const String CLASS_NAME("PG_NTPAdminDomain");
static const String CAPTION("NTP Admin Domain");
static const String DESCRIPTION("This is the PG_NTPAdminDomain object");
static const String NAME_FORMAT("IP");
static const String FILE_CONFIG("/etc/ntp.conf");
static const String ROLE_CLIENT("server");

class NTPTestClient
{
  public:
     NTPTestClient(CIMClient &client);
     ~NTPTestClient(void);

     void testEnumerateInstanceNames(CIMClient &client, Boolean verbose);
     void testEnumerateInstances(CIMClient &client, Boolean verbose);
     void testGetInstance(CIMClient &client, Boolean verbose);

     // utility methods for common test functions
     void errorExit(const String &message, Boolean AbnormalTerminiation);
     void testLog(const String &message);

	// Return TRUE if CreationClassName property is ok
	Boolean goodCreationClassName(String & ccn, Boolean verbose);

	// Return TRUE if Name property is ok
	Boolean goodName(String & name, Boolean verbose);

	// Return TRUE if Caption property is ok
	Boolean goodCaption(String & cap, Boolean verbose);

	// Return TRUE if Description property is ok
	Boolean goodDescription(String & des, Boolean verbose);

	// Return TRUE if ServerAddress property is ok
	Boolean goodServerAddress(Array<String> & srvAddress, Boolean verbose);

	// Return TRUE if NameFormat property is ok
	Boolean goodNameFormat(String & nf, Boolean verbose);

  private:
	 // Function: getHostName
	 // This function resolves name server
	 Boolean getHostName(String serverAddress, String & hostName);
     
	 // Function: getHostAddress
	 // This function resolves address servers
	 Boolean getHostAddress(String serverName, String & serverAddress);
     
	 // Function: isHostAddress
	 // This function verify if host is address
	 Boolean isHostAddress(String host);

	 // Function: getLocalHostName
	 // This function retrieves the local host name
	 Boolean getLocalHostName(String & hostName);

	 // Return partial string from string variable
	 Boolean piece(String strText, String strSep, int numPos, String &strOut);

	 // Return string cleaned
	 void trim(String strText, String & strOut);

	 // Verify string into array
	 Boolean FindInArray(Array<String> src, String text);
    
     // validate keys of the class
     void _validateKeys(CIMObjectPath &cimRef, Boolean verboseTest);
     
     // validate properties of the class
     void _validateProperties(CIMInstance &cimInst, Boolean verboseTest);
};

#endif
