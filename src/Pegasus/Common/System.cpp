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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Rudy Schuet (rudy.schuet@compaq.com) 11/12/01
//					added nsk platform support
//				Ramnath Ravindran (Ramnath.Ravindran@compaq.com) 03/21/2002
//					replaced instances of "| ios::binary" with
//					PEGASUS_OR_IOS_BINARY
//              Robert Kieninger, IBM (kieningr@de.ibm.com) for Bug#667
//              Dave Sudlik, IBM (dsudlik@us.ibm.com) for Bug#1462
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <Pegasus/Common/Config.h>
#endif


#include <fstream>
#include <cctype>  // for tolower()
#include <cstring>
#include "System.h"
#include "Socket.h"

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
# include <windows.h>
#else
# include <arpa/inet.h>
#endif

#include <Pegasus/Common/PegasusVersion.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "SystemWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "SystemUnix.cpp"
#elif defined(PEGASUS_OS_TYPE_NSK)
# include "SystemNsk.cpp"
#elif defined(PEGASUS_OS_VMS)
# include "SystemVms.cpp"
#else
# error "Unsupported platform"
#endif

#if defined(PEGASUS_OS_OS400)
# include "OS400ConvertChar.h"
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

Boolean System::bindVerbose = false;

Boolean System::copyFile(const char* fromPath, const char* toPath)
{
    ifstream is(fromPath PEGASUS_IOS_BINARY);
    ofstream os(toPath PEGASUS_IOS_BINARY);

    char c;

    while (is.get(c))
    {
	if (!os.put(c))
	    return false;
    }

    return true;
}

// ATTN: Move to platform-specific System implementation files and call
// strcasecmp where it is available.
Sint32 System::strcasecmp(const char* s1, const char* s2)
{
    while (*s1 && *s2)
    {
        int r = tolower(*s1++) - tolower(*s2++);

        if (r)
            return r;
    }

    if (*s2)
        return -1;
    else if (*s1)
        return 1;

    return 0;
}

// Return the just the file name from the path into basename
char *System::extract_file_name(const char *fullpath, char *basename)
{
  char *p;
  char buff[2048];
  if (fullpath == NULL)
    {
      basename[0] = '\0';
      return basename;
    }
  strcpy(buff, fullpath);
  for(p = buff + strlen(buff); p >= buff; p--)
    {
      if (*p == '\\' || *p == '/')
        {
          strcpy(basename, p+1);
          return basename;
        }
    }
  strcpy(basename, fullpath);
  return basename;
}

// Return the just the path to the file name into dirname
char *System::extract_file_path(const char *fullpath, char *dirname)
{
  char *p;
  char buff[2048];
  if (fullpath == NULL)
    {
      dirname[0] = '\0';
      return dirname;
    }
  strcpy(buff, fullpath);
  for(p = buff + strlen(buff); p >= buff; p--)
    {
      if (*p == '\\' || *p == '/')
        {
          strncpy(dirname, buff, p+1 - buff);
          dirname[p+1 - buff] = '\0';
          return dirname;
        }
    }
  strcpy(dirname, fullpath);
  return dirname;
}

String System::getHostIP(const String &hostName)
{
    struct hostent * phostent;
    struct in_addr   inaddr;
    String ipAddress = String::EMPTY;
    CString csName = hostName.getCString();
    const char * ccName = csName;
#ifndef PEGASUS_OS_OS400
    if ((phostent = ::gethostbyname(ccName)) != NULL)
#else
    char ebcdicHost[PEGASUS_MAXHOSTNAMELEN];
    if (strlen(ccName) < PEGASUS_MAXHOSTNAMELEN)
        strcpy(ebcdicHost, ccName);
    else
        return ipAddress;
    AtoE(ebcdicHost);
    if ((phostent = ::gethostbyname(ebcdicHost)) != NULL)
#endif
    {
        ::memcpy( &inaddr, phostent->h_addr,4);
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
        char * gottenIPAdress = NULL;
        gottenIPAdress = ::inet_ntoa( inaddr );
        __etoa(gottenIPAdress);
        if (gottenIPAdress != NULL)
        {
            ipAddress.assign(gottenIPAdress);
        }
#else
        ipAddress = ::inet_ntoa( inaddr );
#endif
    }
    return ipAddress;
}

// ------------------------------------------------------------------------
// Convert a hostname into a a single host unique integer representation
// ------------------------------------------------------------------------
Uint32 System::_acquireIP(const char* hostname)
{
	Uint32 ip = 0xFFFFFFFF;
	if (!hostname) return 0xFFFFFFFF;

#ifdef PEGASUS_OS_OS400
	char ebcdicHost[PEGASUS_MAXHOSTNAMELEN];
	if (strlen(hostname) < PEGASUS_MAXHOSTNAMELEN)
		strcpy(ebcdicHost, hostname);
	else
		return 0xFFFFFFFF;
	AtoE(ebcdicHost);
#endif

////////////////////////////////////////////////////////////////////////////////
// This code used to check if the first character of "hostname" was alphabetic
// to indicate hostname instead of IP address. But RFC 1123, section 2.1, relaxed
// this requirement to alphabetic character *or* digit. So bug 1462 changed the
// flow here to call inet_addr first to check for a valid IP address in dotted
// decimal notation. If it's not a valid IP address, then try to validate
// it as a hostname.
// RFC 1123 states: The host SHOULD check the string syntactically for a 
// dotted-decimal number before looking it up in the Domain Name System. 
// Hence the call to inet_addr() first.
////////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_OS_OS400
	Uint32 tmp_addr = inet_addr(ebcdicHost);
#elif defined(PEGASUS_OS_ZOS)
	Uint32 tmp_addr = inet_addr_ebcdic((char *)hostname);
#else
	Uint32 tmp_addr = inet_addr((char *) hostname);
#endif

	struct hostent *entry;

// Note: 0xFFFFFFFF is actually a valid IP address (255.255.255.255).
//       A better solution would be to use inet_aton() or equivalent, as
//       inet_addr() is now considered "obsolete".

    if (tmp_addr == 0xFFFFFFFF)  // if hostname is not an IP address
	{
#ifdef PEGASUS_PLATFORM_SOLARIS_SPARC_CC
#define HOSTENT_BUFF_SIZE        8192
		char      buf[HOSTENT_BUFF_SIZE];
		int       h_errorp;
		struct    hostent hp;

		entry = gethostbyname_r((char *)hostname, &hp, buf,
								HOSTENT_BUFF_SIZE, &h_errorp);
#elif defined(PEGASUS_OS_OS400)
		entry = gethostbyname(ebcdicHost);
#elif defined(PEGASUS_OS_ZOS)
		char hostName[ PEGASUS_MAXHOSTNAMELEN ];
		if (String::equalNoCase("localhost",String(hostname)))
		{
			gethostname( hostName, PEGASUS_MAXHOSTNAMELEN );
			entry = gethostbyname(hostName);
		} else
		{
			entry = gethostbyname((char *)hostname);
		}
#else
		entry = gethostbyname((char *)hostname);
#endif
		if (!entry)
		{
			return 0xFFFFFFFF;
		}
		unsigned char ip_part1,ip_part2,ip_part3,ip_part4;

		ip_part1 = entry->h_addr[0];
		ip_part2 = entry->h_addr[1];
		ip_part3 = entry->h_addr[2];
		ip_part4 = entry->h_addr[3];
		ip = ip_part1;
		ip = (ip << 8) + ip_part2;
		ip = (ip << 8) + ip_part3;
		ip = (ip << 8) + ip_part4;
	}
    else    // else hostname *is* a dotted-decimal IP address
	{
		// resolve hostaddr to a real host entry
		// casting to (const char *) as (char *) will work as (void *) too, those it fits all platforms
#ifndef PEGASUS_OS_OS400
        entry = gethostbyaddr((const char *) &tmp_addr, sizeof(tmp_addr), AF_INET);
#else
		entry = gethostbyaddr((char *) &tmp_addr, sizeof(tmp_addr), AF_INET);
#endif
		if (entry == 0)
		{
			// error, couldn't resolve the ip
			return 0xFFFFFFFF;
		} else
		{

			unsigned char ip_part1,ip_part2,ip_part3,ip_part4;

			ip_part1 = entry->h_addr[0];
			ip_part2 = entry->h_addr[1];
			ip_part3 = entry->h_addr[2];
			ip_part4 = entry->h_addr[3];
			ip = ip_part1;
			ip = (ip << 8) + ip_part2;
			ip = (ip << 8) + ip_part3;
			ip = (ip << 8) + ip_part4;
		}
	}

	return ip;
}

Boolean System::sameHost (const String & hostName)
{
    //
    //  If a port is included, return false
    //
    if (hostName.find (":") != PEG_NOT_FOUND)
    {
        return false;
    }

    //
    //  Retrieve IP addresses for both hostnames
    //
    Uint32 hostNameIP, systemHostIP = 0xFFFFFFFF;
    hostNameIP = System::_acquireIP ((const char *) hostName.getCString ());
    if (hostNameIP == 0x7F000001)
    {
        //
        //  localhost or IP address of 127.0.0.1
        //  real IP address needed for compare
        //
        hostNameIP = System::_acquireIP
            ((const char *) System::getHostName ().getCString ());
    }
    if (hostNameIP == 0xFFFFFFFF)
    {
        //
        //  Malformed IP address or not resolveable
        //
        return false;
    }

    systemHostIP = System::_acquireIP
        ((const char *) System::getFullyQualifiedHostName ().getCString ());

    if (systemHostIP == 0x7F000001)
    {
        //
        //  localhost or IP address of 127.0.0.1
        //  real IP address needed for compare
        //
        systemHostIP = System::_acquireIP
            ((const char *) System::getHostName ().getCString ());
    }
    if (systemHostIP == 0xFFFFFFFF)
    {
        //
        //  Malformed IP address or not resolveable
        //
        return false;
    }

    if (hostNameIP != systemHostIP)
    {
        return false;
    }

    return true;
}

// System ID constants for Logger::put and Logger::trace
const String System::CIMLISTENER = "cimlistener"; // Listener systme ID

PEGASUS_NAMESPACE_END
