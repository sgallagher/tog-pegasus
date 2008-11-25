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

#ifndef Pegasus_BinaryCodec_h
#define Pegasus_BinaryCodec_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Buffer.h>
#include <Pegasus/Common/CIMBuffer.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** This is the coder-decoder (codec) for the OpenPegasus proprietary binary 
    protocol.
*/
class PEGASUS_COMMON_LINKAGE BinaryCodec
{
public:

    // Peform hex dump of the given data.
    static void hexDump(const void* data, size_t size);

    static bool encodeRequest(
        Buffer& out,
        const char* host,
        const String& authenticationHeader,
        CIMOperationRequestMessage* msg,
        bool binaryResponse);

    static bool encodeResponseBody(
        Buffer& out,
        const CIMResponseMessage* msg,
        CIMName& name);

    static CIMOperationRequestMessage* decodeRequest(
        const Buffer& in,
        Uint32 queueId,
        Uint32 returnQueueId);

    static CIMResponseMessage* decodeResponse(
        const Buffer& in);

    static Buffer formatSimpleIMethodRspMessage(
        const CIMName& iMethodName,
        const String& messageId,
        HttpMethod httpMethod,
        const ContentLanguageList& httpContentLanguages,
        const Buffer& body,
        Uint64 serverResponseTime,
        Boolean isFirst,
        Boolean isLast);

private:

    BinaryCodec();
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_BinaryCodec_h */
