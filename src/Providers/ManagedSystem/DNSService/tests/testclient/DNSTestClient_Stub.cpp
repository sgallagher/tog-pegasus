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


// This file has the DNS-specific routines that will be called to get
// a validation of the CIM information vs. the current test system

#include "DNSTestClient.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/*
   GoodCreationClassName method for the DNS Provider Test Client

   Checks the specified value against the expected value and
   returns TRUE if the same, else FALSE
*/
Boolean DNSTestClient::goodCreationClassName(const String &ccn, 
                                            Boolean verbose)
{
   if (verbose)
      cout<<"Checking " << ccn << " against " << CLASS_NAME <<endl;
   return (String::equalNoCase(ccn, CLASS_NAME));
}

/*
   GoodName method for the DNS Provider Test Client

   Checks the specified value against the expected value and
   returns TRUE if the same, else FALSE
*/
Boolean DNSTestClient::goodName(const String &name, Boolean verbose)
{
    if (verbose)
      cout<<"Checking " << name << " against Name" <<endl;
	
    cout<<"- No check written for DNS Name " << endl;
    return true;  
}

/* GoodCaption method for the DNS Provider Test Client 

   Checks the specified value against the expected value 
   and returns TRUE if the same, else FALSE 
*/
Boolean DNSTestClient::goodCaption(const String &cap, 
                                   	  Boolean verbose)
{
   if (verbose)
      cout<<"Checking Caption " << cap << endl;

   return (String::equalNoCase(cap, CAPTION));
}


/* GoodDescription method for the DNS Provider Test Client 

   Checks the specified value against the expected value 
   and returns TRUE if the same, else FALSE 
*/
Boolean DNSTestClient::goodDescription(const String &des, 
                                   	  Boolean verbose)
{
   if (verbose)
      cout<<"Checking Description " << des << endl;

   return (String::equalNoCase(des, DESCRIPTION));
}

/* GoodNameFormat method for the DNS Provider Test Client 

   Checks the specified value against the expected value 
   and returns TRUE if the same, else FALSE 
*/
Boolean DNSTestClient::goodNameFormat(const String &nf, 
                                   	  Boolean verbose)
{
   if (verbose)
      cout<<"Checking NameFormat " << nf << endl;
   return (String::equalNoCase(nf, NAMEFORMAT));
}

/* GoodSearchList method for the DNS Provider Test Client 

   Checks the specified values against the expected array 
   values and returns TRUE if the same, else FALSE 
 */
Boolean DNSTestClient::goodSearchList(const String &src, 
                                   	  Boolean verbose)
{
   if (verbose)
      cout<<"Checking SearchList " << src << endl;

   cout<<"- No check written for DNS SearchList " << endl;
   return true;
}

/* GoodAddresses method for the DNS Provider Test Client 

   Checks the specified values against the expected array 
   values and returns TRUE if the same, else FALSE 
*/
Boolean DNSTestClient::goodAddresses(const String &addr, 
                                     Boolean verbose)
{
   if (verbose)
      cout<<"Checking Addresses " << addr << endl;

   cout<<"- No check written for DNS Addresses " << endl;
   return true;
}
