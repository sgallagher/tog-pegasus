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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//         Warren Otsuka (warren_otsuka@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Handler.h
//
//      This handler processes HTTP messages which have the following format:
//
//          lines (one or more CRLF-terminated lines)
//          terminator (CRLF on line by itself)
//          message-body (optional depending on message type)
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Handler_h
#define Pegasus_Handler_h

#include <iostream>
#include <strstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Logger.h>
#include "Channel.h"

PEGASUS_NAMESPACE_BEGIN

class Handler : public ChannelHandler
{
public:

    Handler();

    virtual ~Handler();

    virtual Boolean handleOpen(Channel* channel);

    virtual Boolean handleInput(Channel* channel);

    virtual Boolean handleOutput(Channel* channel);

    virtual void handleClose(Channel* channel);

    void clear();

    void skipWhitespace(const char*& p);

    const char* getFieldValue(const char* fieldName) const;

    const char* getFieldValueSubString(const char* str) const;

    const char* getContent() const;

    Uint32 getContentLength() const;

    void print() const;

    void log() const;

    //void logMessage(LogFileType logFileType, String& subsystem, 
    //              Uint32 severity, String& formatMsg) const;

    virtual int handleMessage();

    static void printMessage(PEGASUS_STD(ostream)& os, const Array<Sint8>& message);

    static void setMessageTrace(const Boolean flag)
    {
        _messageTrace = flag;
    }
    static void setMessageLogTrace(const Boolean flag)
    {
        _messageLogTrace = flag;
    }

    ;

private:

    enum State { WAITING, LINES, CONTENT, DONE };
    State _state;


protected:

    Array<char> _message;
    Array<Uint32> _lines;
    Uint32 _contentOffset;
    Uint32 _contentLength;
    static Boolean _messageTrace;
    static Boolean _messageLogTrace;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Handler_h */
