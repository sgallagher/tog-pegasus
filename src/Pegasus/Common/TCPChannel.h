//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_TCPChannel_h
#define Pegasus_TCPChannel_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Channel.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Selector.h>

PEGASUS_NAMESPACE_BEGIN
/**  TCP Channel Class derives from Channel and SelectorHandler
     and implements the interface to TCP

*/
class PEGASUS_COMMON_LINKAGE TCPChannel 
    : public Channel, public SelectorHandler
{
public:
    ///
    TCPChannel(Selector* selector, Uint32 desc, ChannelHandler* handler);
    ///
    virtual ~TCPChannel();

    /** read Read from the channel.
    @param ptr
    @param size - 
    @seealso channel:read
    */
    virtual Sint32 read(void* ptr, Uint32 size);
    ///
    virtual Sint32 write(const void* ptr, Uint32 size);
    ///
    virtual Sint32 readN(void* ptr, Uint32 size);
    ///
    virtual Sint32 writeN(const void* ptr, Uint32 size);
    ///
    virtual void enableBlocking();
    ///
    virtual void disableBlocking();
    ///
    virtual Boolean wouldBlock() const;
    ///
    virtual Boolean getBlocking() const { return _blocking; }
    ///
    virtual Boolean handle(Sint32 desc, Uint32 reasons);
    ///
    virtual ChannelHandler* getChannelHandler() { return _handler; }

private:
    Selector* _selector;
    Sint32 _desc;
    Boolean _blocking;
    ChannelHandler* _handler;
};

class PEGASUS_COMMON_LINKAGE TCPChannelConnector : public ChannelConnector
{
public:

    TCPChannelConnector(
	ChannelHandlerFactory* factory,
	Selector* selector);

    virtual ~TCPChannelConnector();

    virtual Channel* connect(const char* address);

    virtual void disconnect(Channel* channel);

private:

    Selector* _selector;
};

class PEGASUS_COMMON_LINKAGE TCPChannelAcceptor 
    : public ChannelAcceptor, public SelectorHandler
{
public:

    TCPChannelAcceptor(
	ChannelHandlerFactory* factory,
	Selector* selector);

    virtual ~TCPChannelAcceptor();

    virtual Boolean bind(const char* address);

    virtual Boolean handle(Sint32 desc, Uint32 reasons);

private:

    Selector* _selector;
    Sint32 _desc;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_TCPChannel_h */
