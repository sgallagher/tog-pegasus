//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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

#ifndef Pegasus_DNSAdminDomain_h
#define Pegasus_DNSAdminDomain_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Pair.h>

//used by gethostname function
#include <unistd.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

class DNSAdminDomain
{
    
public:
    DNSAdminDomain(void);
    virtual ~DNSAdminDomain(void);

public:
    // This function retrieves the local host name
    Boolean getLocalHostName(String & hostName);

    // Verify and return Name property
    Boolean getName(String & name);
      
    // Return SearchList parameter, if exists.
    Boolean getSearchList(Array<String> & srclst);
      
    // Verify and return Addresses parameter
    Boolean getAddresses(Array<String> & addrlst);

private:
    String dnsName;
    Array<String> dnsSearchList;
    Array<String> dnsAddresses;

    // Retrieve DNS information from file /etc/resolv.conf
    Boolean getDNSInfo(void);
    
    // Compare two string (Upper or Lower case)
    Boolean IsEqual(char text[], const char txtcomp[]);

    // Verify if found string in array
    Boolean FindInArray(Array<String> src, String text);
};

#endif
