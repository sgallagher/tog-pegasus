//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM,
// Compaq Computer Corporation
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
//              added nsk platform support           
//
//%/////////////////////////////////////////////////////////////////////////////

#include "TCPChannel.h"

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "TCPChannelWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "TCPChannelUnix.cpp"
#elif defined(PEGASUS_OS_TYPE_NSK)
# include "TCPChannelNsk.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_NAMESPACE_BEGIN

Sint32 TCPChannel::readN(void* ptr, Uint32 size)
{
    // ATTN-A: need a timeout here!

    // Enable blocking temporarily:
    Boolean blocking = getBlocking();

    if (!blocking)
        enableBlocking();

    // Try to read size bytes:

    char* p = (char*)ptr;
    Uint32 r = size;
    Uint32 m = 0;

    while (r)
    {
        Sint32 n = read(p, r);

        if (n == -1)
            return m;

        m += n;
        p += n;
        r -= n;
    }

    // Restore non-blocking if applicable:

    if (!blocking)
        disableBlocking();

    // Return number of bytes actually read:

    return m;
}

Sint32 TCPChannel::writeN(const void* ptr, Uint32 size)
{
    // ATTN-A: need a timeout here!

    // Enable blocking temporarily:

    Boolean blocking = getBlocking();

    if (!blocking)
        enableBlocking();

    // Try to write size bytes:

    const char* p = (const char*)ptr;
    Uint32 r = size;
    Uint32 m = 0;

    while (r)
    {
        Sint32 n = write(p, r);

        if (n == -1)
            return m;

        m += n;
        p += n;
        r -= n;
    }

    // Restore non-blocking if applicable:

    if (!blocking)
        disableBlocking();

    // Return number of bytes actually written:

    return m;
}

Boolean TCPChannel::handle(Sint32 desc, Uint32 reasons)
{
    if (desc != _desc)
        return false;

    if (reasons & Selector::READ)
    {
        if (!_handler->handleInput(this))
        {
            _handler->handleClose(this);
            return false;
        }
    }
    else if (reasons & Selector::WRITE)
    {
        if (!_handler->handleOutput(this))
        {
            _handler->handleClose(this);
            return false;
        }
    }

    // ATTN-A: what about Selector::EXCEPTION and handleException()?

    return true;
}

PEGASUS_NAMESPACE_END
