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

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

DNSAdminDomain::DNSAdminDomain(void)
{
}

DNSAdminDomain::~DNSAdminDomain(void)
{
}

// Verify if exists file resolv.conf
Boolean 
DNSFileOk() 
{
    // Implement source code here.
    return true;
}


// Verify and return name property
Boolean 
DNSAdminDomain::getName(char name[]) {
    return true;
}

// Return SearchList property, if exists.
void
DNSAdminDomain::getSearchList(char srclst[]) {
    return;
}

// Verify and return Addresses property
Boolean
DNSAdminDomain::getAddresses(char addrlst[]) {
    return true;
}

// Read domain name, addresses e search list from /etc/resolv.conf
Boolean
DNSAdminDomain::getDNSInfo(String &domain, Array<String> &srcl, Array<String> &addr) 
{
    return true;
}    
