//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Handler.h,v $
// Revision 1.1  2001/01/14 19:53:52  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Handler.h
//
//	This handler processes HTTP messages which have the following format:
//
//	    lines (one or more CRLF-terminated lines)
//	    terminator (CRLF on line by itself)
//	    message-body (optional depending on message type)
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Handler_h
#define Pegasus_Handler_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <ace/Service_Config.h>
#include <ace/Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/OS.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PROTOCOL_LINKAGE Handler 
    : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
public:

    Handler(ACE_Reactor* reactor = 0);

    virtual ~Handler();

    virtual int open(void* = 0);

    virtual int handle_input(ACE_HANDLE);

    virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask mask);

    void clear();

    void skipWhitespace(const char*& p);

    const char* getFieldValue(const char* fieldName) const;

    const char* getFieldValueSubString(const char* str) const;

    const char* getContent() const;

    Uint32 getContentLength() const;

    void print() const;

    virtual int handleMessage();

    void sendMessage(const Array<Sint8>& message);

    static void printMessage(std::ostream& os, const Array<Sint8>& message);

private:

    enum State { WAITING, LINES, CONTENT, DONE };
    State _state;
    typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> BaseClass;

protected:

    Array<char> _message;
    Array<Uint32> _lines;
    Uint32 _contentOffset;
    Uint32 _contentLength;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Handler_h */
