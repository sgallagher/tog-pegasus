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
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////


// This file has the NTPProvider-specific routines that will be called to get
// a validation of the CIM information vs. the current test system

#include "NTPTestClient.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

// Standard verify method to property <do not remove>

   /*
GoodThis property retrieve class namemethod for the NTP Provider Test Client
    Checks the specified value against the expected value and
    returns TRUE if the same, else FALSE
*/
Boolean
NTPTestClient::goodCreationClassName(String & param, Boolean verbose)
{
   Boolean ok = true;
   String retValue;

   // Insert here source code to assign the variable retValue

   if(verbose)
      // Insert here source code to display debug messages
      // Sample: cout<< "Checking " << param << " against " << retValue <<endl;

   return ok;
}

   /*
GoodThis property retrieve primary server name method for the NTP Provider Test Client
    Checks the specified value against the expected value and
    returns TRUE if the same, else FALSE
*/
Boolean
NTPTestClient::goodName(String & param, Boolean verbose)
{
   Boolean ok = true;
   String retValue;

   // Insert here source code to assign the variable retValue

   if(verbose)
      // Insert here source code to display debug messages
      // Sample: cout<< "Checking " << param << " against " << retValue <<endl;

   return ok;
}

   /*
GoodThis property retrieve product captionmethod for the NTP Provider Test Client
    Checks the specified value against the expected value and
    returns TRUE if the same, else FALSE
*/
Boolean
NTPTestClient::goodCaption(String & param, Boolean verbose)
{
   Boolean ok = true;
   String retValue;

   // Insert here source code to assign the variable retValue

   if(verbose)
      // Insert here source code to display debug messages
      // Sample: cout<< "Checking " << param << " against " << retValue <<endl;

   return ok;
}

   /*
GoodThis property retrieve product descriptionmethod for the NTP Provider Test Client
    Checks the specified value against the expected value and
    returns TRUE if the same, else FALSE
*/
Boolean
NTPTestClient::goodDescription(String & param, Boolean verbose)
{
   Boolean ok = true;
   String retValue;

   // Insert here source code to assign the variable retValue

   if(verbose)
      // Insert here source code to display debug messages
      // Sample: cout<< "Checking " << param << " against " << retValue <<endl;

   return ok;
}

   /*
GoodThis property retrieve server addresses array, if is NTP clientmethod for the NTP Provider Test Client
    Checks the specified value against the expected value and
    returns TRUE if the same, else FALSE
*/
Boolean
NTPTestClient::goodServerAddress(Array<String> & param, Boolean verbose)
{
   Boolean ok = true;
   Array<String> retValue;

   // Insert here source code to assign the variable retValue

   if(verbose)
      // Insert here source code to display debug messages
      // Sample: cout<< "Checking " << param << " against " << retValue <<endl;

   return ok;
}

   /*
GoodThis property retrieve name format valuemethod for the NTP Provider Test Client
    Checks the specified value against the expected value and
    returns TRUE if the same, else FALSE
*/
Boolean
NTPTestClient::goodNameFormat(String & param, Boolean verbose)
{
   Boolean ok = true;
   String retValue;

   // Insert here source code to assign the variable retValue

   if(verbose)
      // Insert here source code to display debug messages
      // Sample: cout<< "Checking " << param << " against " << retValue <<endl;

   return ok;
}


