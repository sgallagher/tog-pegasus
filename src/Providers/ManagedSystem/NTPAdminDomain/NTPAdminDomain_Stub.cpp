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
//
// Modified By: Jair Francisco T. dos Santos (t.dos.santos.francisco@non.hp.com)
//==============================================================================
// Based on DNSAdminDomain_Stub.cpp file
//%////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
#include "NTPAdminDomain.h"

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
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
NTPAdminDomain::~NTPAdminDomain(void)
{
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
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getCreationClassName
//
// REMARKS: returns the CreationClassName property
//
// PARAMETERS: [OUT] strValue -> string that will receive the CreationClassName 
//                               property value
//
// RETURN: TRUE
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getCreationClassName(String & strValue) 
{
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getName
//
// REMARKS: returns the Name property
//
// PARAMETERS: [OUT] strValue -> string that will receive the Name property value
//
// RETURN: TRUE
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getName(String & strValue) 
{
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getCaption
//
// REMARKS: returns the Caption property
//
// PARAMETERS: [OUT] strValue -> string that will receive the Caption property 
//                                 value
//
// RETURN: TRUE
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getCaption(String & strValue) 
{
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getDescription
//
// REMARKS: returns the Description property
//
// PARAMETERS: [OUT] strValue -> string that will receive the Description 
//                                 property value
//
// RETURN: TRUE
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getDescription(String & strValue) 
{
    return true;
}

//------------------------------------------------------------------------------
// FUNCTION: getServerAddress
//
// REMARKS: returns the ServerAddress property
//
// PARAMETERS: [OUT] strValue -> string that will receive the ServerAddress 
//                                 property value
//
// RETURN: TRUE
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getServerAddress(Array<String> & strValue) 
{
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
// RETURN: TRUE
//------------------------------------------------------------------------------
Boolean
NTPAdminDomain::getNameFormat(String & strValue) 
{
    return true;
}
