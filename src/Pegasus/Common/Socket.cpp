//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
// The Open Group, Tivoli Systems
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Socket.h"

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <windows.h>
#else
# include <cctype>
# include <unistd.h>
# include <cstdlib>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
#endif

PEGASUS_NAMESPACE_BEGIN

static Uint32 _socketInterfaceRefCount = 0;

Sint32 Socket::read(Sint32 socket, void* ptr, Uint32 size)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return ::recv(socket, (char*)ptr, size, 0);
#elif defined(PEGASUS_OS_ZOS)
    int i=::read(socket, (char*)ptr, size);
    __atoe_l((char *)ptr,size);
    return i;
#else
    return ::read(socket, (char*)ptr, size);
#endif
}

Sint32 Socket::write(Sint32 socket, const void* ptr, Uint32 size)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return ::send(socket, (const char*)ptr, size, 0);
#elif defined(PEGASUS_OS_ZOS)
    char * ptr2 = (char *)malloc(size);
    int i;
    memcpy(ptr2,ptr,size);
    __etoa_l(ptr2,size);
    i = ::write(socket, ptr2, size);
    free(ptr2);
    return i;
#else
    return ::write(socket, (char*)ptr, size);
#endif
}

void Socket::close(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    closesocket(socket);
#else
    ::close(socket);
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
