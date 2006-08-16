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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#2513
//              David Dillard, Symantec Corp.,  (david_dillard@symantec.com)
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

//------------------------------------------------------------------------------
//
// PEGASUS_RETRY_SYSTEM_CALL()
//
//     This macro repeats the given system call until it returns something
//     other than EINTR.
//
//------------------------------------------------------------------------------

#ifdef PEGASUS_OS_TYPE_WINDOWS
#   define PEGASUS_RETRY_SYSTEM_CALL(EXPR, RESULT) RESULT = EXPR
#else
#   define PEGASUS_RETRY_SYSTEM_CALL(EXPR, RESULT) \
        while (((RESULT = (EXPR)) == -1) && (errno == EINTR))
#endif

//------------------------------------------------------------------------------
//
// PEGASUS_SOCKET_ERROR
//
//    This macro defines the error return code of network functions.
//    Should be used instead of comparing with -1.
//
//------------------------------------------------------------------------------

#ifdef PEGASUS_OS_TYPE_WINDOWS
#   define PEGASUS_SOCKET_ERROR SOCKET_ERROR
#else
#   define PEGASUS_SOCKET_ERROR (-1)
#endif


PEGASUS_NAMESPACE_BEGIN

static Uint32 _socketInterfaceRefCount = 0;

Sint32 Socket::read(PEGASUS_SOCKET socket, void* ptr, Uint32 size)
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

Sint32 Socket::write(PEGASUS_SOCKET socket, const void* ptr, Uint32 size)
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

Sint32 Socket::timedWrite(PEGASUS_SOCKET socket, 
                          const void* ptr, 
                          Uint32 size, 
                          Uint32 socketWriteTimeout)
{
    Sint32 bytesWritten = 0;
    Sint32 totalBytesWritten = 0;
    Boolean socketTimedOut = false;
    int selreturn = 0;
    while (1)
    {
#ifdef PEGASUS_OS_TYPE_WINDOWS
        PEGASUS_RETRY_SYSTEM_CALL(
            ::send(socket, (const char*)ptr, size, 0), bytesWritten);
#else
        PEGASUS_RETRY_SYSTEM_CALL(
            ::write(socket, (char*)ptr, size), bytesWritten);
#endif
        // Some data written this cycle ? 
        // Add it to the total amount of written data.
        if (bytesWritten > 0)
        {
            totalBytesWritten += bytesWritten;
            socketTimedOut = false;
        }

        // All data written ? return amount of data written
        if ((Uint32)bytesWritten == size)
        {
            return totalBytesWritten;
        }
        // If data has been written partially, we resume writing data
        // this also accounts for the case of a signal interrupt
        // (i.e. errno = EINTR)
        if (bytesWritten > 0)
        {
            size -= bytesWritten;
            ptr = (void *)((char *)ptr + bytesWritten);
            continue;
        }
        // Something went wrong
        if (bytesWritten == PEGASUS_SOCKET_ERROR)
        {
            // if we already waited for the socket to get ready, bail out
            if( socketTimedOut ) return bytesWritten;
#ifdef PEGASUS_OS_TYPE_WINDOWS
            if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK)
#endif
            {
                fd_set fdwrite;
                 // max. timeout seconds waiting for the socket to get ready
                struct timeval tv = { socketWriteTimeout, 0 };
                FD_ZERO(&fdwrite);
                FD_SET(socket, &fdwrite);
                selreturn = select(FD_SETSIZE, NULL, &fdwrite, NULL, &tv);
                if (selreturn == 0) socketTimedOut = true; // ran out of time
                continue;            
            }
            return bytesWritten;
        }
    }
}

void Socket::close(PEGASUS_SOCKET socket)
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

void Socket::enableBlocking(PEGASUS_SOCKET socket)
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

void Socket::disableBlocking(PEGASUS_SOCKET socket)
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

