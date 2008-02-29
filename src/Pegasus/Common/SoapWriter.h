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

#ifndef Pegasus_SoapWriter_h
#define Pegasus_SoapWriter_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Indentor.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMMethod.h>
#include <Pegasus/Common/CIMParameter.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/Buffer.h>
#include <Pegasus/Common/StrLit.h>
#include <Pegasus/Common/SoapUtils.h>
#include <Pegasus/Common/XmlUtils.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE SoapWriter
{
public:

    static Buffer formatHttpErrorRspMessage(
        const String& status,
        const String& cimError = String::EMPTY,
        const String& errorDetail = String::EMPTY);
    
    static Buffer formatWSManRspMessage(
        const String& rspName,
        const String& messageId,
        HttpMethod httpMethod,
        const ContentLanguageList& httpContentLanguages,
        const Buffer& body,
        Uint64 serverResponseTime);

    static Buffer formatWSManErrorRspMessage(
        const String& rspName,
        const String& messageId,
        HttpMethod httpMethod,
        const CIMException& cimException);

    static void appendInstanceElement(
        Buffer& out,
        const CIMConstInstance& instance);

    static void appendPropertyElement(
        Buffer& out,
        const CIMConstProperty& property);

    static void appendQualifierElement(
        Buffer& out,
        const CIMConstQualifier& qualifier);

    static void appendValueElement(
        Buffer& out,
        const CIMValue& value,
        const CIMName& name);

private:

    SoapWriter() { }

    static void _appendHTTPResponseHeader(
        Buffer& out, 
        const String& rspName,
        HttpMethod httpMethod, 
        const ContentLanguageList& contentLanguages,
        Uint32 contentLength);

    static void _appendStartTag(
        Buffer& out, SoapNamespaces::Type nsType, StrLit tag);
    static void _appendEndTag(
        Buffer& out, SoapNamespaces::Type nsType, StrLit tag);

    static void _appendSoapEnvelopeStart(Buffer& out);
    static void _appendSoapEnvelopeEnd(Buffer& out);
    static void _appendSoapBodyStart(Buffer& out);
    static void _appendSoapBodyEnd(Buffer& out);

    static void _appendSoapHeader(
        Buffer& out, 
        const String& rspName, 
        const String& messageId);

    // Helper functions for appendValueElement()
    static void _appendValueArray(
        Buffer& out, const CIMObjectPath* p, Uint32 size, const CIMName& name);
    template<class T> static void _appendValueArray(
        Buffer& out, const T* p, Uint32 size, const CIMName& name);
    static void _appendValueReferenceElement(
        Buffer& out,
        const CIMObjectPath& reference,
        Boolean putValueWrapper);

    static void _appendValue(Buffer& out, Boolean x)
    { XmlUtils::append(out, x); }

    static void _appendValue(Buffer& out, Uint8 x)
    { XmlUtils::append(out, Uint32(x)); }

    static void _appendValue(Buffer& out, Sint8 x)
    { XmlUtils::append(out, Sint32(x)); }

    static void _appendValue(Buffer& out, Uint16 x)
    { XmlUtils::append(out, Uint32(x)); }

    static void _appendValue(Buffer& out, Sint16 x)
    { XmlUtils::append(out, Sint32(x)); }

    static void _appendValue(Buffer& out, Uint32 x)
    { XmlUtils::append(out, x); }

    static void _appendValue(Buffer& out, Sint32 x)
    { XmlUtils::append(out, x); }

    static void _appendValue(Buffer& out, Uint64 x)
    { XmlUtils::append(out, x); }

    static void _appendValue(Buffer& out, Sint64 x)
    { XmlUtils::append(out, x); }

    static void _appendValue(Buffer& out, Real32 x)
    { XmlUtils::append(out, x); }

    static void _appendValue(Buffer& out, Real64 x)
    { XmlUtils::append(out, x); }

    static void _appendValue(Buffer& out, const Char16& x)
    { XmlUtils::appendSpecial(out, x); }

    static void _appendValue(Buffer& out, const String& x)
    { XmlUtils::appendSpecial(out, x); }

    static void _appendValue(Buffer& out, const CIMDateTime& x)
    { out << x.toString(); }

    static void _appendValue(Buffer& out, const CIMObjectPath& x)
    { _appendValueReferenceElement(out, x, true); }

    static void _appendValue(Buffer& out, const CIMObject& x)
    {
        String myStr = x.toString();
        _appendValue(out, myStr);
    }
    
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SaopWriter_h */
