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
// Based on DNSAdminDomain.h file
//%/////////////////////////////////////////////////////////////////////////////
#ifndef _NTP_H
#define _NTP_H

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
//Pegasus includes
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>

//------------------------------------------------------------------------------
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

// File configurations
static const String NTP_FILE_CONFIG("/etc/ntp.conf");

// Role definitions
static const String NTP_ROLE_CLIENT("server");

// Defines
static const String CLASS_NAME("PG_NTPAdminDomain");
static const String NTP_CAPTION("NTP Admin Domain");
static const String NTP_DESCRIPTION("This is the PG_NTPAdminDomain object");
static const String NTP_NAME_FORMAT("IP");

// Insert MOF property definitions
static const int MAX_KEYS = 2;
static const String PROPERTY_CREATION_CLASS_NAME("CreationClassName");
static const String PROPERTY_NAME("Name");
static const String PROPERTY_CAPTION("Caption");
static const String PROPERTY_DESCRIPTION("Description");
static const String PROPERTY_SERVER_ADDRESS("ServerAddress");
static const String PROPERTY_NAME_FORMAT("NameFormat");
    
//------------------------------------------------------------------------------
// Class [NTPAdminDomain] Definition
//------------------------------------------------------------------------------
class NTPAdminDomain
{
    public:
        //Constructor/Destructor
        NTPAdminDomain(void);
          ~NTPAdminDomain(void);
        
    public:    
        // This function retrieves the local host name
        Boolean getLocalHostName(String & hostName);

        // This function returns TRUE if the CreationClassName is valid
        // returns the CreationClassName property on the 'strValue' argument
        Boolean getCreationClassName(String & strValue);

        // This function returns TRUE if the Name is valid
        // returns the Name property on the 'strValue' argument
        Boolean getName(String & strValue);

        // This function returns TRUE if the Caption is valid
        // returns the Caption property on the 'strValue' argument
        Boolean getCaption(String & strValue);

        // This function returns TRUE if the Description is valid
        // returns the Description property on the 'strValue' argument
        Boolean getDescription(String & strValue);

        // This function returns TRUE if the ServerAddress is valid
        // returns the ServerAddress property on the 'strValue' argument
        Boolean getServerAddress(Array<String> & strValue);

        // This function returns TRUE if the NameFormat is valid
        // returns the NameFormat property on the 'strValue' argument
        Boolean getNameFormat(String & strValue);
            
    private:

        //
        // Functions
        //

        // This function retrieves the NTP information from "/etc/ntp.conf" file
        Boolean getNTPInfo(void);

        // This function resolves host name servers
        Boolean getHostName(String serverAddress, String & nameServer);

        // This function verify if host is address
        Boolean isHostAddress(String host);

        // This function resolves address servers
        Boolean getHostAddress(String nameServer, String & serverAddress);

        // This function retrieves the key value from line buffer
        Boolean getKeyValue(String strLine, String & strValue);

        //
        // Variables
        //
        String ntpName;
        Array<String> ntpServerAddress;         
};
#endif
