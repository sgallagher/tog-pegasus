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
// Author: Paulo F. Borges (pfborges@wowmail.com)
//
// Modified By:  Jenny Yu, Hewlett-Packard Company (jenny.yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


// This file has the NISProvider-specific routines that will be called to get
// a validation of the CIM information vs. the current test system

#include "NISTestClient.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

//------------------------------------------------------------------------------
// FUNCTION: getUtilGetHostName
//
// REMARKS:
//
// PARAMETERS:  [OUT] systemName -> string that will contain the host name
//
// RETURN: TRUE if successful, FALSE otherwise
//------------------------------------------------------------------------------
Boolean getUtilGetHostName(String& systemName)
{
     return true;
}

/**
    Retrieves the SystemName property value for the NIS Provider Test Client.
    Checks the specified value against the expected value and returns
    TRUE if the same, else FALSE.
*/
Boolean NISTestClient::goodSystemName(String & param, Boolean verbose)
{
    if (verbose)
        cout<<"Checking " << param << " against SystemName "<<endl;
    cout<<"- No check written for SystemName " <<endl;
    return true;
}

/**
    Retrieves the SystemCreationClassName property value for the NIS Provider
    Test Client.  Checks the specified value against the expected value and
    returns TRUE if the same, else FALSE.
*/
Boolean NISTestClient::goodSystemCreationClassName(String & param, Boolean verbose)
{
   if(verbose)
      cout<< "Checking " << param << " against ";
      cout<< SYSTEM_CREATION_CLASS_NAME.getString() << endl;

   return (String::equalNoCase(param, SYSTEM_CREATION_CLASS_NAME.getString()));
}

/**
    Retrieves CreationClassName property value for the NIS Provider Test Client.
    Checks the specified value against the expected value and returns TRUE
    if the same, else FALSE.
*/
Boolean NISTestClient::goodCreationClassName(String & param, Boolean verbose)
{
   if(verbose)
      cout << "Checking " << param << " against " << CLASS_NAME.getString();
      cout << endl;
   return (String::equalNoCase(param, CLASS_NAME.getString()));
}

/**
    Retrieves Name property value for the NIS Provider Test Client.
    Checks the specified value against the expected value and returns
    TRUE if the same, else FALSE
*/
Boolean NISTestClient::goodName(String & param, Boolean verbose)
{
    if (verbose)
        cout << "Checking " << param << " against Name " << endl;
    cout << "- No check written for Name " << endl;
    return true;
}

/**
    Retrieves the Caption property value for the NIS Provider Test Client.
    Checks the specified value against the expected value and returns
    TRUE if the same, else FALSE.
*/
Boolean NISTestClient::goodCaption(String & param, Boolean verbose)
{
   if (verbose)
      cout << "Checking " << param << " against " << CAPTION <<endl;

   return (String::equalNoCase(param, CAPTION));
}

/**
    Retrieves the Description property value for the NIS Provider Test Client.
    Checks the specified value against the expected value and returns TRUE
    if the same, else FALSE.
*/
Boolean NISTestClient::goodDescription(String & param, Boolean verbose)
{
   if (verbose)
      cout << "Checking " << param << " against " << DESCRIPTION << endl;

   return (String::equalNoCase(param, DESCRIPTION));
}

/**
    Retrieves the ServerWaitFlag property value for the NIS Provider Test
    Client.  Checks the specified value against the expected value and returns
    TRUE if the same, else FALSE.
*/
Boolean NISTestClient::goodServerWaitFlag(Uint16 & param, Boolean verbose)
{
    if (verbose)
        cout << "Checking " << param << " against ServerWaitFlag " << endl;
    cout << "- No check written for ServerWaitFlag " << endl;
    return true;
}

/**
    Retrieves the ServerType property value for the NIS Provider Test Client.
    Checks the specified value against the expected value and returns
    TRUE if the same, else FALSE.
*/
Boolean NISTestClient::goodServerType(Uint16 & param, Boolean verbose)
{
    if (verbose)
        cout << "Checking " << param << " against ServerType " << endl;
    cout << "- No check written for ServerType " << endl;
    return true;
}
