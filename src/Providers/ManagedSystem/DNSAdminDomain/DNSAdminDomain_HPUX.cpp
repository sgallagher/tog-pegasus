//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/ProviderException.h>    
#include "DNSAdminDomainProvider.h"

//used by gethostname function
#include <unistd.h>
#include <cctype>  // For toupper()

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define SEARCHLIST 1
#define ADDRESSES  2

static const String RESOLVCONF("/etc/resolv.conf");
    
// Constructor of DNSAdminDomain class
DNSAdminDomain::DNSAdminDomain(void)
{
    if(!getDNSInfo())
        throw CIMObjectNotFoundException("DNSAdminDomain "
                  "can't create PG_DNSAdminDomain instance");
}

DNSAdminDomain::~DNSAdminDomain(void)
{
}


// Verify if exists file resolv.conf and 
// contents are ok.
Boolean 
DNSFileOk() 
{
    FILE *fp;
    char buffer[150];
    int count = 0;
    Boolean ok = false;
    
    if((fp = fopen(RESOLVCONF.getCString(), "r")) == NULL)
        return ok;
    
    while(!feof(fp)) {
        memset(buffer, 0, sizeof(buffer));
        fscanf(fp, "%s", buffer);

        // Verify if keys exists.
        if(strstr(buffer, "domain") != NULL || 
           strstr(buffer, "search") != NULL ||
           strstr(buffer, "nameserver") != NULL)
            count++;

        if(count > 1) {
            ok = true;
            break;
        }
    }
    fclose(fp);
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
DNSAdminDomain::getLocalHostName(String & hostName) 
{
    char host[30];
    if(gethostname(host, sizeof(host)))
        return false;

       hostName.assign(host);
    return true;
}

// Verify if string is equal (Upper or Lower case)
Boolean
DNSAdminDomain::IsEqual(char text[], const char txtcomp[])
{
    Boolean ok = true;
    int i;
    int max = strlen(text);
    char chin, chcmp;
    
    if(!max)
        return false;
    
    for(i=0; i<max; i++) {
        chin = toupper(text[i]);
        chcmp = toupper(txtcomp[i]);
        if(chin != chcmp) {
            ok = false;
            break;
        }
    }
    return ok;
}

// Verify if found string in array
Boolean
DNSAdminDomain::FindInArray(Array<String> src, String text)
{
    Boolean ok = false;
    int i;
    
    for(i=0; i<src.size(); i++) {
        if(src[i] == text) {
            ok = true;
            break;
        }
    }
    return ok;
}

// Verify and return name property
Boolean 
DNSAdminDomain::getName(String & name) 
{
    if(dnsName.size() > 0) {
        name.assign(dnsName);
        return true;
    }
    return false;
}

// Verify and return SearchList property, if exists.
Boolean
DNSAdminDomain::getSearchList(Array<String> & srclst) 
{
    srclst.clear();
    for(int i=0; i < dnsSearchList.size(); i++) 
        srclst.append(dnsSearchList[i]);
    return true;
}

// Verify and return Addresses property
Boolean
DNSAdminDomain::getAddresses(Array<String> & addrlst) 
{
    addrlst.clear();
    for(int i=0; i < dnsAddresses.size(); i++) 
        addrlst.append(dnsAddresses[i]);
    return true;
}

// Read domain name, addresses e search list from /etc/resolv.conf
Boolean
DNSAdminDomain::getDNSInfo()
{
    FILE *fp;
    int i, ind = 0;
    char *ptr;
    char buffer[512];
    Boolean ok = true;
    
    // Clear all parameters
    dnsName.clear();
    dnsSearchList.clear();
    dnsAddresses.clear();
    
    // Open file /etc/resolv.conf
    if((fp = fopen(RESOLVCONF.getCString(), "r")) == NULL)
        return false;
    
    // Retrieve DNS informations from file
    while(!feof(fp)) {
        memset(buffer, 0, sizeof(buffer));
        fscanf(fp, "%s", buffer);

        if(!strlen(buffer))
            continue;
        
        // Verify if key is domain name
        if(IsEqual(buffer, "domain")) {
            fscanf(fp, "%s", buffer);
            dnsName.assign(buffer);
        }
        else
        {
            // Verify if key is search list
            if(IsEqual(buffer, "search")) {
                ind = SEARCHLIST;
                continue;
            }
            // Verify if key is address (DNS server)
            else if(IsEqual(buffer, "nameserver")) {
                ind = ADDRESSES;
                continue;
            }
            else
            {
                switch(ind) {
                    case SEARCHLIST:
                        if(!FindInArray(dnsSearchList, String(buffer))) {
                            dnsSearchList.append(String(buffer));
                    		if(dnsName.size() == 0)
                                dnsName.assign(buffer);
                    	}
                        break;
                    case ADDRESSES:
                        if(!FindInArray(dnsAddresses, String(buffer)))
                            dnsAddresses.append(String(buffer));
                        break;
                    default:
                        ok = false;
                        break;
                }
            }
        }
        if(!ok)
            break;
    }
    fclose(fp);
    return ok;
}                        
