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
// Modified By: Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#2513
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Socket.h"

#ifdef PEGASUS_OS_TYPE_WINDOWS
#include <windows.h>
# ifndef _WINSOCKAPI_
#   include <winsock2.h>
# endif
#else
# include <cctype>
#ifndef PEGASUS_OS_OS400
#   include <unistd.h>
#else
#   include <unistd.cleinc>
#endif
#   include <string.h>  // added by rk for memcpy
# include <cstdlib>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <errno.h>
#endif

#include <Pegasus/Common/Sharable.h>
PEGASUS_NAMESPACE_BEGIN

static Uint32 _socketInterfaceRefCount = 0;

Sint32 Socket::read(Sint32 socket, void* ptr, Uint32 size)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return ::recv(socket, (char*)ptr, size, 0);
#else
#if defined (__GNUC__) && !defined(PEGASUS_OS_SOLARIS) && !defined(PEGASUS_OS_DARWIN) && !defined(PEGASUS_OS_LSB)
    int ccode = TEMP_FAILURE_RETRY(::read(socket, (char*)ptr, size));
    return ccode;
#else 
    return ::read(socket, (char*)ptr, size);
#endif
#endif
}

Sint32 Socket::write(Sint32 socket, const void* ptr, Uint32 size)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return ::send(socket, (const char*)ptr, size, 0);
#else
#if (__GNUC__) && !defined(PEGASUS_OS_SOLARIS) && !defined(PEGASUS_OS_DARWIN) && !defined(PEGASUS_OS_LSB)
    int ccode = TEMP_FAILURE_RETRY(::write(socket, (char*)ptr, size));
    return ccode;
#else
    return ::write(socket, (char*)ptr, size);
#endif
#endif
}

void Socket::close(Sint32 socket)
{
  if(-1 != socket)
   {
    #ifdef PEGASUS_OS_TYPE_WINDOWS
    if(!closesocket(socket)) socket=-1;
    #else
    #if (__GNUC__) && !defined(PEGASUS_OS_SOLARIS) && !defined(PEGASUS_OS_DARWIN) && !defined(PEGASUS_OS_LSB)
       if(!TEMP_FAILURE_RETRY(::close(socket))) socket = -1;
    #else
       if(!::close(socket)) socket = -1;
    #endif
    #endif
   }
}

int Socket::close2(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return closesocket(socket);
#else
#if (__GNUC__) && !defined(PEGASUS_OS_SOLARIS) && !defined(PEGASUS_OS_DARWIN) && !defined(PEGASUS_OS_LSB)
    int ccode = TEMP_FAILURE_RETRY(::close(socket));
    return ccode;
#else
    return ::close(socket);
#endif
#endif
}


void Socket::enableBlocking(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    unsigned long flag = 0;
    ioctlsocket(socket, FIONBIO, &flag);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(socket, F_SETFL, flags);
#endif
}

int Socket::enableBlocking2(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    unsigned long flag = 0;
    return ioctlsocket(socket, FIONBIO, &flag);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    return fcntl(socket, F_SETFL, flags);
#endif
}

void Socket::disableBlocking(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    unsigned long flag = 1;
    ioctlsocket(socket, FIONBIO, &flag);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(socket, F_SETFL, flags);
#endif
}

int Socket::disableBlocking2(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    unsigned long flag = 1;
    return ioctlsocket(socket, FIONBIO, &flag);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    return fcntl(socket, F_SETFL, flags);
#endif
}

void Socket::initializeInterface()
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    if (_socketInterfaceRefCount == 0)
    {
	WSADATA tmp;

	if (WSAStartup(0x202, &tmp) == SOCKET_ERROR)
	    WSACleanup();
    }

    _socketInterfaceRefCount++;
#endif
}

void Socket::uninitializeInterface()
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    _socketInterfaceRefCount--;

    if (_socketInterfaceRefCount == 0)
	WSACleanup();
#endif
}


PEGASUS_NAMESPACE_END

