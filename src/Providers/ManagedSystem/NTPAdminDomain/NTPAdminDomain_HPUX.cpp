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
// Modified By: Jair Francisco T. dos Santos (t.dos.santos.francisco@non.hp.com)
//==============================================================================
// Based on DNSAdminDomain_HPUX.cpp file
//%////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
//Pegasus includes
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

// The following includes are necessary to gethostbyaddr and gethostname
// functions 
#include <ctype.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "NTPAdminDomainProvider.h"

//------------------------------------------------------------------------------
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

//==============================================================================
//
// Class [NTPAdminDomain] methods
//
//==============================================================================

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
NTPAdminDomain::NTPAdminDomain(void)
{
    // Retrieve NTP informations
    if(!getNTPInfo())
        throw CIMObjectNotFoundException("NTPAdminDomain "
            "can't create PG_NTPAdminDomain instance");
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
NTPAdminDomain::~NTPAdminDomain(void)
{
}

//------------------------------------------------------------------------------
// FUNCTION: getHostName
//
// REMARKS: Resolves name servers
//
// PARAMETERS:  [IN] serverAddress -> string containing the IP address
//                [OUT] hostName -> string that will contain the name server
//
// RETURN: TRUE if valid host IP, FALSE otherwise
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getHostName(String serverAddress, String & hostName) 
{
    Boolean ok = false;
    int ps, value = 0;
    String strValue;
    struct hostent *host;
    struct in_addr ia;
    
    hostName.clear();
    if((ia.s_addr = inet_addr(serverAddress.getCString())) != -1) 
    {
        host = gethostbyaddr((const char *)&ia, 
                              sizeof(struct in_addr),
                               AF_INET);
        if(host != NULL) 
        {
            hostName.assign(host->h_name);
            ok = true;
        }
   }
   return ok;
}    

//------------------------------------------------------------------------------
// FUNCTION: getHostAddress
//
// REMARKS: Resolves address servers
//
// PARAMETERS:  [IN] serverName     -> string containing the name server
//                [OUT] serverAddress -> string that will contain the IP server
//
// RETURN: TRUE if valid host name, FALSE otherwise
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getHostAddress(String serverName, String & serverAddress) 
{
    Boolean ok = false;
    int ps, value = 0;
    String strValue;
    struct hostent *host;
    struct in_addr ia;
    
    serverAddress.clear();
    host = gethostbyname(serverName.getCString());
    if(host != NULL) 
    {
        ia = *(struct in_addr *)(host->h_addr);
        serverAddress.assign(inet_ntoa(ia));
        ok = true;
    }
       return ok;
}    

//------------------------------------------------------------------------------
// FUNCTION: isHostAddress
//
// REMARKS: Verify if host is address
//
// PARAMETERS:  [IN] host -> string containing the host name or address
//
// RETURN: TRUE if host address, FALSE otherwise
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::isHostAddress(String host) {
    int ps;
    String strValue;
    Boolean ok = false;
    
    ps = host.find(".");
    if(ps > 0) {
        strValue.assign(host.subString(0, ps - 1));
        ok = (atoi(strValue.getCString()) > 0);
    }
    return ok;
}

//------------------------------------------------------------------------------
// FUNCTION: getLocalHostName
//
// REMARKS: Retrieves the local host name
//
// PARAMETERS:  [OUT] hostName -> string that will contain the local host name
//
// RETURN: TRUE if local hostname is valid, FALSE otherwise
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getLocalHostName(String & hostName) 
{
    char host[30];
    if(gethostname(host, sizeof(host)))
        return false;

       hostName.assign(host);
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getKeyValue
//
// REMARKS: Retrieves the key value from line buffer
//
// PARAMETERS:  [IN] strLine   -> string that will contain the line buffer
//                [OUT] strValue -> string key value 
//
// RETURN: TRUE if Ok, otherwise FALSE.
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getKeyValue(String strLine, String & strValue) {
    int ps;
    Boolean ok = false;
    String strTmp;
    
    strValue.clear();
    // Verify is value exists after space character
    ps = strLine.find(" ");
    if(ps < 0)
        ps = strLine.find("\t"); // If not a space, then verify a tab character
    if(ps > 0) {
        // Retrieve partial value
        strTmp.assign(strLine.subString(ps + 1));
        ps = strTmp.find(" ");
        if(ps < 0)
            ps = strTmp.find("\t");
        if(ps > 0) {
            // Retrieve real value
            strValue.assign(strTmp.subString(0, ps));
            ok = true;
        }
    }
    return ok;
}

//------------------------------------------------------------------------------
// FUNCTION: getNTPInfo
//
// REMARKS: Retrieves the NTP information from the "/etc/ntp.conf" file, 
//            and sets private variables to hold the data read.
//
// RETURN: 
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getNTPInfo() 
{
    FILE *fp;
    int i, ps = 0;
    char buffer[5000];
    Boolean ok = false,
            okRet = false;
    String strKey, 
           strHost, 
           strBuffer;

    // Open NTP configuration file
    if((fp = fopen(NTP_FILE_CONFIG.getCString(), "r")) == NULL)
        throw CIMOperationFailedException("NTPAdminDomain "
              "can't open ntp.conf file");

    // Clear attributes 
    ntpName.clear();
    ntpServerAddress.clear();

    memset(buffer, 0, sizeof(buffer));
    while(fgets(buffer, sizeof(buffer), fp) != NULL) 
    {
        buffer[strlen(buffer)-1] = 0;
        strBuffer.assign(buffer);

        ps = strBuffer.find(NTP_ROLE_CLIENT);
        
        if(ps == 0) 
        {
            okRet = true;
            
            //Insert address server into array
            if(!getKeyValue(strBuffer, strHost))
                continue;
            
            ok = false;    
            // If strHost value is name server then assign ntpName and 
            // retrieve IP address from this server
            if(!isHostAddress(strHost)) {
                // Set ntpName variable with first name server
                if(ntpName.size() == 0) {
                    ntpName.assign(strHost);  
                    okRet = (ntpName.size() > 0);
                }
                getHostAddress(strHost, strHost);
            }
            
            // Verify if name server exists in array
            for(i=0; i < ntpServerAddress.size(); i++) 
            {
                if(String::equalNoCase(ntpServerAddress[i], strHost)) 
                {
                    ok = true;
                    break;
                }
            }
            if(!ok) 
            {
                ntpServerAddress.append(strHost);
                if(ntpName.size() == 0) 
                    // Set ntpName variable with name server, if strHost variable
                    // is an IP address.
                    getHostName(strHost, ntpName);
            }
        }
    }
    fclose(fp);        
    return okRet;
}

//------------------------------------------------------------------------------
// FUNCTION: getCreationClassName
//
// REMARKS: returns the CreationClassName property
//
// PARAMETERS: [OUT] strValue -> string that will receive the CreationClassName 
//                                 property value
//
// RETURN: TRUE (hard-coded property value)
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getCreationClassName(String & strValue) 
{
    strValue.assign(CLASS_NAME);
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getName
//
// REMARKS: returns the Name property
//
// PARAMETERS: [OUT] strValue -> string that will receive the Name property value
//
// RETURN: TRUE if 'Name' was successfully obtained, FALSE otherwise.
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getName(String & strValue) 
{
    if(ntpName.size() > 0) {
    	strValue.assign(ntpName);
    	return true;
	}
    return false;
}

//------------------------------------------------------------------------------
// FUNCTION: getCaption
//
// REMARKS: returns the Caption property
//
// PARAMETERS: [OUT] strValue -> string that will receive the Caption property 
//                                 value
//
// RETURN: TRUE (hard-coded property value)
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getCaption(String & strValue) 
{
    strValue.assign(NTP_CAPTION);
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getDescription
//
// REMARKS: returns the Description property
//
// PARAMETERS: [OUT] strValue -> string that will receive the Description property 
//                                 value
//
// RETURN: TRUE (hard-coded property value)
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getDescription(String & strValue) 
{
    strValue.assign(NTP_DESCRIPTION);
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getServerAddress
//
// REMARKS: returns the ServerAddress array property, if is a NTP client
//
// PARAMETERS: [OUT] strValue -> string that will receive the ServerAddress 
//                                 property value
//
// RETURN: TRUE if 'ServerAddress' was successfully obtained, FALSE otherwise.
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getServerAddress(Array<String> & strValue) 
{
    strValue.clear();
    for(int i=0; i < ntpServerAddress.size(); i++) 
        strValue.append(ntpServerAddress[i]);
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getNameFormat
//
// REMARKS: returns the NameFormat property
//
// PARAMETERS: [OUT] strValue -> string that will receive the NameFormat 
//                                 property value
//
// RETURN: TRUE (hard-coded property value)
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getNameFormat(String & strValue) 
{
    strValue.assign(NTP_NAME_FORMAT);
    return true;
}
