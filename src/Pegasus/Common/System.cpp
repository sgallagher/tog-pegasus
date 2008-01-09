//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <Pegasus/Common/Config.h>
#endif

#include <fstream>
#include <cctype>  // for tolower()
#include <cstring>
#include "System.h"
#include "Socket.h"
#include "Network.h"
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HostAddress.h>
#include <Pegasus/Common/Array.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "SystemWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX) || defined(PEGASUS_OS_VMS)
# include "SystemPOSIX.cpp"
#else
# error "Unsupported platform"
#endif

#if defined(PEGASUS_OS_OS400)
# include "EBCDIC_OS400.h"
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

Boolean System::bindVerbose = false;

Boolean System::copyFile(const char* fromPath, const char* toPath)
{
    ifstream is(fromPath PEGASUS_IOS_BINARY);
    fstream os(toPath, ios::out  PEGASUS_OR_IOS_BINARY);

    char c;

    while (is.get(c))
    {
        if (!os.put(c))
            return false;
    }

    FileSystem::syncWithDirectoryUpdates(os);
    return is.eof();
}

static const Uint8 _toLowerTable[256] =
{
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
    0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
    0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
    0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
};

Sint32 System::strcasecmp(const char* s1, const char* s2)
{
    // Note: this is faster than glibc strcasecmp().

    Uint8* p = (Uint8*)s1;
    Uint8* q = (Uint8*)s2;
    int r;

    for (;;)
    {
        if ((r = _toLowerTable[p[0]] - _toLowerTable[q[0]]) || !p[0] ||
            (r = _toLowerTable[p[1]] - _toLowerTable[q[1]]) || !p[1] ||
            (r = _toLowerTable[p[2]] - _toLowerTable[q[2]]) || !p[2] ||
            (r = _toLowerTable[p[3]] - _toLowerTable[q[3]]) || !p[3])
            break;

        p += 4;
        q += 4;
    }

    return r;
}

// Return the just the file name from the path into basename
char *System::extract_file_name(const char *fullpath, char *basename)
{
    if (fullpath == NULL)
    {
        basename[0] = '\0';
        return basename;
    }

    for (const char* p = fullpath + strlen(fullpath) - 1; p >= fullpath; p--)
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
    char buff[4096];
    if (fullpath == NULL)
    {
        dirname[0] = '\0';
        return dirname;
    }
    strncpy(buff, fullpath, sizeof(buff)-1);
    buff[sizeof(buff)-1] =  '\0';
    for (p = buff + strlen(buff); p >= buff; p--)
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

Boolean System::getHostIP(const String &hostName, int *af, String &hostIP)
{
#ifdef PEGASUS_ENABLE_IPV6
    struct addrinfo *info, hints;
    memset (&hints, 0, sizeof(struct addrinfo));

    // Check for valid IPV4 address, if found return ipv4 address
    *af = AF_INET;
    hints.ai_family = *af;
    hints.ai_protocol = IPPROTO_TCP;   
    hints.ai_socktype = SOCK_STREAM;
    if (!getaddrinfo(hostName.getCString(), 0, &hints, &info))
    {
        char ipAddress[PEGASUS_INET_ADDRSTR_LEN];
        HostAddress::convertBinaryToText(info->ai_family, 
            &((struct sockaddr_in*)(info->ai_addr))->sin_addr, ipAddress,
            PEGASUS_INET_ADDRSTR_LEN);
        hostIP = ipAddress;
        freeaddrinfo(info);
        return true;
    }

    // Check for valid IPV6 Address. 
    *af = AF_INET6;
    hints.ai_family = *af;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;
    if (!getaddrinfo(hostName.getCString(), 0, &hints, &info))
    {
        char ipAddress[PEGASUS_INET6_ADDRSTR_LEN];
        HostAddress::convertBinaryToText(info->ai_family,
            &((struct sockaddr_in6*)(info->ai_addr))->sin6_addr, ipAddress,
            PEGASUS_INET6_ADDRSTR_LEN);
        hostIP = ipAddress;
        freeaddrinfo(info);
        return true;
    }

    return false;
#else
    *af = AF_INET;
    struct hostent* hostEntry;
    struct in_addr inaddr;
    String ipAddress;
    CString hostNameCString = hostName.getCString();
    const char* hostNamePtr = hostNameCString;

#if defined(PEGASUS_OS_LINUX)
    char hostEntryBuffer[8192];
    struct hostent hostEntryStruct;
    int hostEntryErrno;

    gethostbyname_r(
        hostNamePtr,
        &hostEntryStruct,
        hostEntryBuffer,
        sizeof(hostEntryBuffer),
        &hostEntry,
        &hostEntryErrno);
#elif defined(PEGASUS_OS_SOLARIS)
    char hostEntryBuffer[8192];
    struct hostent hostEntryStruct;
    int hostEntryErrno;

    hostEntry = gethostbyname_r(
        (char *)hostNamePtr,
        &hostEntryStruct,
        hostEntryBuffer,
        sizeof(hostEntryBuffer),
        &hostEntryErrno);
#elif defined(PEGASUS_OS_OS400)
    char ebcdicHost[PEGASUS_MAXHOSTNAMELEN];
    if (strlen(hostNamePtr) < PEGASUS_MAXHOSTNAMELEN)
        strcpy(ebcdicHost, hostNamePtr);
    else
        return ipAddress;
    AtoE(ebcdicHost);
    hostEntry = gethostbyname(ebcdicHost);
#else
    hostEntry = gethostbyname(hostNamePtr);
#endif

    if (hostEntry)
    {
        ::memcpy( &inaddr, hostEntry->h_addr,4);
#if defined(PEGASUS_OS_OS400)
        char * gottenIPAdress = NULL;
        gottenIPAdress = ::inet_ntoa( inaddr );

        if (gottenIPAdress != NULL)
        {
            EtoA(gottenIPAdress);
            ipAddress.assign(gottenIPAdress);
        }
#else
        ipAddress = ::inet_ntoa( inaddr );
#endif
    }
    hostIP = ipAddress;
    return true;
#endif
}


#ifdef PEGASUS_ENABLE_IPV6
Boolean System::isIPv6StackActive()
{
    SocketHandle ip6Socket;
    if ((ip6Socket = Socket::createSocket(AF_INET6, SOCK_STREAM, IPPROTO_TCP))
        == PEGASUS_INVALID_SOCKET)
    {
        if (getSocketError() == PEGASUS_INVALID_ADDRESS_FAMILY)
        {
            return false;
        }
    }
    else
    {
        Socket::close(ip6Socket);
    }

    return true;
}
#endif

// ------------------------------------------------------------------------
// Convert a hostname into a a single host unique integer representation
// ------------------------------------------------------------------------
Boolean System::_acquireIP(const char* hostname, int *af, void *dst)
{
#ifdef PEGASUS_ENABLE_IPV6
    String ipAddress;
    if(getHostIP(hostname, af, ipAddress))
{
        HostAddress::convertTextToBinary(*af, ipAddress.getCString(), dst);
        return true;
    }
    return false;
#else
    *af = AF_INET;
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
// to indicate hostname instead of IP address. But RFC 1123, section 2.1,
// relaxed this requirement to alphabetic character *or* digit. So bug 1462
// changed the flow here to call inet_addr first to check for a valid IP
// address in dotted decimal notation. If it's not a valid IP address, then
// try to validate it as a hostname.
// RFC 1123 states: The host SHOULD check the string syntactically for a
// dotted-decimal number before looking it up in the Domain Name System.
// Hence the call to inet_addr() first.
////////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_OS_OS400
    Uint32 tmp_addr = inet_addr(ebcdicHost);
#else
    Uint32 tmp_addr = inet_addr((char *) hostname);
#endif

    struct hostent* hostEntry;

// Note: 0xFFFFFFFF is actually a valid IP address (255.255.255.255).
//       A better solution would be to use inet_aton() or equivalent, as
//       inet_addr() is now considered "obsolete".

    if (tmp_addr == 0xFFFFFFFF)  // if hostname is not an IP address
    {
#if defined(PEGASUS_OS_LINUX)
        char hostEntryBuffer[8192];
        struct hostent hostEntryStruct;
        int hostEntryErrno;

        gethostbyname_r(
            hostname,
            &hostEntryStruct,
            hostEntryBuffer,
            sizeof(hostEntryBuffer),
            &hostEntry,
            &hostEntryErrno);
#elif defined(PEGASUS_OS_SOLARIS)
        char hostEntryBuffer[8192];
        struct hostent hostEntryStruct;
        int hostEntryErrno;

        hostEntry = gethostbyname_r(
            (char *)hostname,
            &hostEntryStruct,
            hostEntryBuffer,
            sizeof(hostEntryBuffer),
            &hostEntryErrno);
#elif defined(PEGASUS_OS_OS400)
        hostEntry = gethostbyname(ebcdicHost);
#elif defined(PEGASUS_OS_ZOS)
        char hostName[PEGASUS_MAXHOSTNAMELEN + 1];
        if (String::equalNoCase("localhost",String(hostname)))
        {
            gethostname( hostName, PEGASUS_MAXHOSTNAMELEN );
            hostName[sizeof(hostName)-1] = 0;
            hostEntry = gethostbyname(hostName);
        }
        else
        {
            hostEntry = gethostbyname((char *)hostname);
        }
#else
        hostEntry = gethostbyname((char *)hostname);
#endif
        if (!hostEntry)
        {
            // error, couldn't resolve the ip
            memcpy(dst, &ip, sizeof (Uint32));
            return false;
        }
        unsigned char ip_part1,ip_part2,ip_part3,ip_part4;

        ip_part1 = hostEntry->h_addr[0];
        ip_part2 = hostEntry->h_addr[1];
        ip_part3 = hostEntry->h_addr[2];
        ip_part4 = hostEntry->h_addr[3];
        ip = ip_part1;
        ip = (ip << 8) + ip_part2;
        ip = (ip << 8) + ip_part3;
        ip = (ip << 8) + ip_part4;
    }
    else    // else hostname *is* a dotted-decimal IP address
    {
        // resolve hostaddr to a real host entry
        // casting to (const char *) as (char *) will work as (void *) too,
        // those it fits all platforms
#if defined(PEGASUS_OS_LINUX)
        char hostEntryBuffer[8192];
        struct hostent hostEntryStruct;
        int hostEntryErrno;

        gethostbyaddr_r(
            (const char*) &tmp_addr,
            sizeof(tmp_addr),
            AF_INET,
            &hostEntryStruct,
            hostEntryBuffer,
            sizeof(hostEntryBuffer),
            &hostEntry,
            &hostEntryErrno);
#elif defined(PEGASUS_OS_SOLARIS)
        char hostEntryBuffer[8192];
        struct hostent hostEntryStruct;
        int hostEntryErrno;

        hostEntry = gethostbyaddr_r(
            (const char *) &tmp_addr,
            sizeof(tmp_addr),
            AF_INET,
            &hostEntryStruct,
            hostEntryBuffer,
            sizeof(hostEntryBuffer),
            &hostEntryErrno);
#elif defined(PEGASUS_OS_OS400)
        hostEntry =
            gethostbyaddr((char *) &tmp_addr, sizeof(tmp_addr), AF_INET);
#else
        hostEntry =
            gethostbyaddr((const char *) &tmp_addr, sizeof(tmp_addr), AF_INET);
#endif
        if (hostEntry == 0)
        {
            // error, couldn't resolve the ip
            memcpy(dst, &ip, sizeof (Uint32));
            return false;
        }
        else
        {
            unsigned char ip_part1,ip_part2,ip_part3,ip_part4;

            ip_part1 = hostEntry->h_addr[0];
            ip_part2 = hostEntry->h_addr[1];
            ip_part3 = hostEntry->h_addr[2];
            ip_part4 = hostEntry->h_addr[3];
            ip = ip_part1;
            ip = (ip << 8) + ip_part2;
            ip = (ip << 8) + ip_part3;
            ip = (ip << 8) + ip_part4;
        }
    }
    memcpy(dst, &ip, sizeof (Uint32));

    return true;
#endif
}

Boolean System::resolveHostNameAtDNS(
    const char* hostname,
    Uint32* resolvedNameIP)
{
    // ask the DNS for hostname resolution to IP address
    // this can mean a time delay for as long as the DNS
    // takes to answer
    struct hostent* hostEntry;

#if defined(PEGASUS_OS_LINUX)
    char hostEntryBuffer[8192];
    struct hostent hostEntryStruct;
    int hostEntryErrno;

    gethostbyname_r(
        hostname,
        &hostEntryStruct,
        hostEntryBuffer,
        sizeof(hostEntryBuffer),
        &hostEntry,
        &hostEntryErrno);
#elif defined(PEGASUS_OS_SOLARIS)
    char hostEntryBuffer[8192];
    struct hostent hostEntryStruct;
    int hostEntryErrno;

    hostEntry = gethostbyname_r(
        (char *)hostname,
        &hostEntryStruct,
        hostEntryBuffer,
        sizeof(hostEntryBuffer),
        &hostEntryErrno);
#else
    hostEntry = gethostbyname((char *)hostname);
#endif
    if (hostEntry == 0)
    {
        // error, couldn't resolve the hostname to an ip address
        return false;
    }
    else
    {
        unsigned char ip_part1,ip_part2,ip_part3,ip_part4;
        ip_part1 = hostEntry->h_addr[0];
        ip_part2 = hostEntry->h_addr[1];
        ip_part3 = hostEntry->h_addr[2];
        ip_part4 = hostEntry->h_addr[3];
        *resolvedNameIP = ip_part1;
        *resolvedNameIP = (*resolvedNameIP << 8) + ip_part2;
        *resolvedNameIP = (*resolvedNameIP << 8) + ip_part3;
        *resolvedNameIP = (*resolvedNameIP << 8) + ip_part4;
    }
    return true;
}

Boolean System::resolveIPAtDNS(Uint32 ip_addr, Uint32 * resolvedIP)
{
    struct hostent *entry;

#ifndef PEGASUS_OS_OS400
    entry = gethostbyaddr((const char *) &ip_addr, sizeof(ip_addr), AF_INET);
#else
    entry = gethostbyaddr((char *) &ip_addr, sizeof(ip_addr), AF_INET);
#endif
    if (entry == 0)
    {
        // error, couldn't resolve the ip
        return false;
    }
    else
    {
        unsigned char ip_part1,ip_part2,ip_part3,ip_part4;
        ip_part1 = entry->h_addr[0];
        ip_part2 = entry->h_addr[1];
        ip_part3 = entry->h_addr[2];
        ip_part4 = entry->h_addr[3];
        *resolvedIP = ip_part1;
        *resolvedIP = (*resolvedIP << 8) + ip_part2;
        *resolvedIP = (*resolvedIP << 8) + ip_part3;
        *resolvedIP = (*resolvedIP << 8) + ip_part4;
    }
    return true;
}


Boolean System::isLoopBack(int af, void *binIPAddress)
{
#ifdef PEGASUS_ENABLE_IPV6
    struct in6_addr ip6 = PEGASUS_IPV6_LOOPBACK_INIT;
#endif
    Uint32 ip4 = PEGASUS_IPV4_LOOPBACK_INIT;
    switch (af)
    {
#ifdef PEGASUS_ENABLE_IPV6
        case AF_INET6:
            return !memcmp(&ip6, binIPAddress, sizeof (ip6));
#endif
        case AF_INET:
            Uint32 n = ntohl( *(Uint32*)binIPAddress);
            return !memcmp(&ip4, &n, sizeof (ip4));       
    }

    return false;
}

Boolean System::isLocalHost(const String &hostName)
{
// Get all ip addresses on the node and compare them with the given hostname.
#ifdef PEGASUS_ENABLE_IPV6
    CString csName = hostName.getCString();
    struct addrinfo hints, *res1, *res2, *res1root, *res2root;
    char localHostName[PEGASUS_MAXHOSTNAMELEN];
    gethostname(localHostName, PEGASUS_MAXHOSTNAMELEN);
    Boolean isLocal = false;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    res1root = res2root = 0;
    getaddrinfo(csName, 0, &hints, &res1root);
    getaddrinfo(localHostName, 0, &hints, &res2root);

    res1 = res1root;
    while (res1 && !isLocal)
    {
        if (isLoopBack(AF_INET,
            &((struct sockaddr_in*)res1->ai_addr)->sin_addr))
        {
            isLocal = true;
            break;
        }

        res2 = res2root;
        while (res2) 
        {
            if (!memcmp(&((struct sockaddr_in*)res1->ai_addr)->sin_addr,
                &((struct sockaddr_in*)res2->ai_addr)->sin_addr,
                sizeof (struct in_addr)))
            {
                isLocal = true;
                break;
            }
            res2 = res2->ai_next;
        }
        res1 = res1->ai_next;
    }   
    freeaddrinfo(res1root);
    freeaddrinfo(res2root);
    if (isLocal)
    {
        return true;
    } 

    hints.ai_family = AF_INET6;
    res1root = res2root = 0;
    getaddrinfo(csName, 0, &hints, &res1root);
    getaddrinfo(localHostName, 0, &hints, &res2root);

    res1 = res1root;
    while (res1 && !isLocal)
    {
        if (isLoopBack(AF_INET6,
            &((struct sockaddr_in6*)res1->ai_addr)->sin6_addr))
        {
            isLocal = true;
            break;
        }

        res2 = res2root;
        while (res2)
        {
            if (!memcmp(&((struct sockaddr_in6*)res1->ai_addr)->sin6_addr,
                &((struct sockaddr_in6*)res2->ai_addr)->sin6_addr,
                sizeof (struct in6_addr)))
            {
                isLocal = true;
                break;
            }
            res2 = res2->ai_next;
        }
        res1 = res1->ai_next;
    }
    freeaddrinfo(res1root);
    freeaddrinfo(res2root);

    return isLocal;
#else

    // differentiate between a dotted IP address given
    // and a real hostname given
    CString csName = hostName.getCString();
    char cc_hostname[PEGASUS_MAXHOSTNAMELEN];
    strcpy(cc_hostname, (const char*) csName);
    Uint32 tmp_addr = 0xFFFFFFFF;
    Boolean hostNameIsIPNotation;

#ifdef PEGASUS_OS_OS400
    AtoE(cc_hostname);
#endif

    // Note: Platforms already supporting the inet_aton()
    //       should define their platform here,
    //        as this is the superior way to work
#if defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_AIX)

    struct in_addr inaddr;
    // if inet_aton failed(return=0),
    // we do not have a valip IP address (x.x.x.x)
    int atonSuccess = inet_aton(cc_hostname, &inaddr);
    if (atonSuccess == 0) hostNameIsIPNotation = false;
    else
    {
        hostNameIsIPNotation = true;
        tmp_addr = inaddr.s_addr;
    }
#else
    // Note: 0xFFFFFFFF is actually a valid IP address (255.255.255.255).
    //       A better solution would be to use inet_aton() or equivalent, as
    //       inet_addr() is now considered "obsolete".
    // Note: inet_aton() not yet supported on all Pegasus platforms
    tmp_addr = inet_addr((char *) cc_hostname);
    if (tmp_addr == 0xFFFFFFFF) hostNameIsIPNotation = false;
    else hostNameIsIPNotation = true;
#endif

    if (!hostNameIsIPNotation)  // if hostname is not an IP address
    {
        // localhost ?
        if (String::equalNoCase(hostName,String("localhost"))) return true;
        char localHostName[PEGASUS_MAXHOSTNAMELEN];
        CString cstringLocalHostName = System::getHostName().getCString();
        strcpy(localHostName, (const char*) cstringLocalHostName); 
        // given hostname equals what system returns as local hostname ?
        if (String::equalNoCase(hostName,localHostName)) return true;
        Uint32 hostIP;
        // bail out if hostname unresolveable
        if (!System::resolveHostNameAtDNS(cc_hostname, &hostIP)) return false;
        // lets see if the IP is defined on one of the network interfaces
        // this can help us avoid another call to DNS
        if (System::isIpOnNetworkInterface(hostIP)) return true;
        // need to check if the local hosts name is possibly
        // registered at the DNS with the IP address equal resolvedNameIP
        Uint32 localHostIP;
        if (!System::resolveHostNameAtDNS(localHostName, &localHostIP))
            return false;
        if (localHostIP == hostIP) return true;
    }
    else
    {   // hostname is an IP address
        // 127.0.0.1 is always the loopback
        // inet_addr returns network byte order
        if (tmp_addr == htonl(0x7F000001)) return true;
        // IP defined on a local AF_INET network interface
        if (System::isIpOnNetworkInterface(tmp_addr)) return true;
        // out of luck so far, lets ask the DNS what our IP is
        // and check against what we got
        Uint32 localHostIP;
        if (!System::resolveHostNameAtDNS(
                (const char*) System::getHostName().getCString(), &localHostIP))
            return false;
        if (localHostIP == tmp_addr) return true;
        // not yet, sometimes resolving the IP address we got against the DNS
        // can solve the problem
        // casting to (const char *) as (char *) will work as (void *) too,
        // those it fits all platforms
        Uint32 hostIP;
        if (!System::resolveIPAtDNS(tmp_addr, &hostIP)) return false;
        if (hostIP == localHostIP) return true;
    }
    return false;
#endif
}

// System ID constants for Logger::put and Logger::trace
const String System::CIMLISTENER = "cimlistener"; // Listener systme ID

PEGASUS_NAMESPACE_END

