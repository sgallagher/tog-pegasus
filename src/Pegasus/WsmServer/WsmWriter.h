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
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WsmWriter_h
#define Pegasus_WsmWriter_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Buffer.h>
#include <Pegasus/Common/StrLit.h>
#include <Pegasus/Common/XmlGenerator.h>
#include <Pegasus/WsmServer/WsmUtils.h>
#include <Pegasus/WsmServer/WsmInstance.h>
#include <Pegasus/WsmServer/WsmProperty.h>
#include <Pegasus/WsmServer/WsmValue.h>
#include <Pegasus/WsmServer/WsmFault.h>
#include <Pegasus/WsmServer/WsmEndpointReference.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_WSMSERVER_LINKAGE WsmWriter : public XmlGenerator
{
public:

    static Buffer formatSoapFault(
        const SoapNotUnderstoodFault& fault,
        const String& messageId,
        const String& relatesTo,
        HttpMethod httpMethod,
        Uint32& httpHeaderSize);

    static Buffer formatWsmFault(
        const WsmFault& fault,
        const String& messageId,
        const String& relatesTo,
        HttpMethod httpMethod,
        Uint32& httpHeaderSize);

    static Buffer formatHttpErrorRspMessage(
        const String& status,
        const String& cimError = String::EMPTY,
        const String& errorDetail = String::EMPTY);

    static Buffer formatWsmRspMessage(
        const String& action,
        const String& messageId,
        const String& relatesTo,
        HttpMethod httpMethod,
        const ContentLanguageList& contentLanguages,
        const Buffer& body,
        const Buffer& headers,
        Uint32& httpHeaderSize);

    static void appendInstanceElement(
        Buffer& out, WsmInstance& instance, Boolean isEmbedded = false);
    static void appendPropertyElement(
        Buffer& out, WsmProperty& property);
    static void appendEPRElement(
        Buffer& out, const WsmEndpointReference& epr);
    static void appendStringElement(
        Buffer& out, const String& str);

    static void appendStartTag(
        Buffer& out,
        WsmNamespaces::Type nsType,
        const StrLit& tagName,
        const char* attrName = 0,
        const String& attrValue = String::EMPTY);
    static void appendEndTag(
        Buffer& out,
        WsmNamespaces::Type nsType,
        const StrLit& tagName);
    static void appendEmptyTag(
        Buffer& out,
        WsmNamespaces::Type nsType,
        const StrLit& tagName);
    static void appendTagValue(
        Buffer& out,
        WsmNamespaces::Type nsType,
        const StrLit& tagName,
        const String& value,
        const char* attrName = 0,
        const String& attrValue = String::EMPTY);

private:

    WsmWriter();

    static void _appendHTTPResponseHeader(
        Buffer& out,
        const String& action,
        HttpMethod httpMethod,
        const ContentLanguageList& contentLanguages,
        Boolean isFault,
        Uint32 contentLength = 0);

    static void _appendSoapEnvelopeStart(
        Buffer& out,
        const ContentLanguageList& contentLanguages = ContentLanguageList());
    static void _appendSoapEnvelopeEnd(Buffer& out);
    static void _appendSoapHeaderStart(Buffer& out);
    static void _appendSoapHeaderEnd(Buffer& out);
    static void _appendSoapBodyStart(Buffer& out);
    static void _appendSoapBodyEnd(Buffer& out);

    static void _appendSoapHeader(
        Buffer& out,
        const String& action,
        const String& messageId,
        const String& relatesTo);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmWriter_h */
