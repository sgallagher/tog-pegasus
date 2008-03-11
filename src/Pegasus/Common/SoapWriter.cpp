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

#include <cstdlib>
#include <cstdio>

#include <Pegasus/Common/Config.h>
#include "Constants.h"
#include "CIMClass.h"
#include "CIMClassRep.h"
#include "CIMInstance.h"
#include "CIMInstanceRep.h"
#include "CIMProperty.h"
#include "CIMPropertyRep.h"
#include "CIMMethod.h"
#include "CIMMethodRep.h"
#include "CIMParameter.h"
#include "CIMParameterRep.h"
#include "CIMParamValue.h"
#include "CIMParamValueRep.h"
#include "CIMQualifier.h"
#include "CIMQualifierRep.h"
#include "CIMValue.h"
#include "SoapUtils.h"
#include "SoapWriter.h"
#include "Buffer.h"
#include "StrLit.h"
#include "StringConversion.h"

PEGASUS_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
// SoapWriter::formatHttpErrorRspMessage()
//
//-----------------------------------------------------------------------------

Buffer SoapWriter::formatHttpErrorRspMessage(
    const String& status,
    const String& cimError,
    const String& errorDetail)
{
    Buffer out;

    // ATTN WSMAN: Format an HTTP error response

    return out;
}

Buffer SoapWriter::formatWSManRspMessage(
        const String& rspName,
        const String& messageId,
        HttpMethod httpMethod,
        const ContentLanguageList& httpContentLanguages,
        const Buffer& body,
        Uint64 serverResponseTime)
{
    Buffer out;

    _appendHTTPResponseHeader(
        out, rspName, httpMethod, httpContentLanguages, 0);
    _appendSoapEnvelopeStart(out);

    _appendSoapHeader(out, rspName, messageId);

    _appendSoapBodyStart(out);
    if (body.size() != 0)
    {
        out << body;
    }
    _appendSoapBodyEnd(out);

    _appendSoapEnvelopeEnd(out);

    return out;
}

Buffer SoapWriter::formatWSManErrorRspMessage(
        const String& rspName,
        const String& messageId,
        HttpMethod httpMethod,
        const CIMException& cimException)
{
    Buffer out;

    // ATTN WSMAN: Format a WS-Man error response

    return out;
}

void SoapWriter::appendInstanceElement(
    Buffer& out,
    const CIMConstInstance& instance)
{
    CheckRep(instance._rep);
    const CIMInstanceRep* rep = instance._rep;

    // Class opening element:
    out << STRLIT("<class:") << rep->getClassName();
    out << STRLIT(" xmlns:class=\"");
    out << SoapNamespaces::supportedNamespaces[SoapNamespaces::WS_CIM_SCHEMA].
        extendedName;
    out << STRLIT("/") << rep->getClassName();
    out << STRLIT("\">\n");

    // Qualifiers:
    for (Uint32 i = 0, n = rep->getQualifierCount(); i < n; i++)
        SoapWriter::appendQualifierElement(out, rep->getQualifier(i));

    // Properties:
    for (Uint32 i = 0, n = rep->getPropertyCount(); i < n; i++)
        SoapWriter::appendPropertyElement(out, rep->getProperty(i));

    // Class closing element:
    out << STRLIT("</class:") << rep->getClassName() << STRLIT(">\n");
}

void SoapWriter::appendPropertyElement(
    Buffer& out,
    const CIMConstProperty& property)
{
    CheckRep(property._rep);
    const CIMPropertyRep* rep = property._rep;

    if (rep->getValue().isArray())
    {
        // ATTN WSMAN: Need to complete
    }
    else if (rep->getValue().getType() == CIMTYPE_REFERENCE)
    {
        // ATTN WSMAN: Need to complete
    }
    else
    {
        SoapWriter::appendValueElement(out, rep->getValue(), rep->getName());
    }
}

void SoapWriter::appendQualifierElement(
    Buffer& out,
    const CIMConstQualifier& qualifier)
{
    CheckRep(qualifier._rep);
    // ATTN WSMAN: Anything to do here?
}

void SoapWriter::_appendHTTPResponseHeader(
    Buffer& out,
    const String& rspName,
    HttpMethod httpMethod,
    const ContentLanguageList& contentLanguages,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };
    out << STRLIT("HTTP/1.1 " HTTP_STATUS_OK "\r\n");

    out << STRLIT("Content-Type: application/xml+soap; "
        "charset=\"utf-8\"\r\n");
    OUTPUT_CONTENTLENGTH(out, contentLength);

    if (contentLanguages.size() > 0)
    {
        out << STRLIT("Content-Language: ") << contentLanguages <<
            STRLIT("\r\n");
    }
    if (httpMethod == HTTP_METHOD_M_POST)
    {
        // ATTN WSMAN: not sure about this!!!
        out << STRLIT("Ext:\r\n");
        out << STRLIT("Cache-Control: no-cache\r\n");
        out << nn << STRLIT("\r\n");
        out << nn << STRLIT("-SOAPAction: ");
        out << nn << rspName;
        out << nn << ("\r\n\r\n");
    }
    else
    {
        out << STRLIT("SOAPAction: ");
        out << rspName;
        out << ("\r\n\r\n");
    }
}

void SoapWriter::_appendStartTag(
    Buffer& out, SoapNamespaces::Type nsType, StrLit tag)
{
    // ATTN WSMAN: Is this needed?
}

void SoapWriter::_appendEndTag(
    Buffer& out, SoapNamespaces::Type nsType, StrLit tag)
{
    out << ("</");
    out << SoapNamespaces::supportedNamespaces[nsType].localName;
    out << (":") << tag << (">\n");
}

void SoapWriter::_appendSoapEnvelopeStart(Buffer& out)
{
    out << STRLIT("<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::SOAP_ENVELOPE].localName;
    out << STRLIT(":Envelope");
    for (unsigned int i = 0; i < SoapNamespaces::LAST; i++)
    {
        out << STRLIT("\nxmlns:");
        out << SoapNamespaces::supportedNamespaces[i].localName;
        out << STRLIT("=\"");
        out << SoapNamespaces::supportedNamespaces[i].extendedName;
        out << STRLIT("\"");
    }
    out << STRLIT(">\n");
}

void SoapWriter::_appendSoapEnvelopeEnd(Buffer& out)
{
    out << STRLIT("</");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::SOAP_ENVELOPE].localName;
    out << STRLIT(":Envelope>\n");
}

void SoapWriter::_appendSoapBodyStart(Buffer& out)
{
    out << STRLIT("<");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::SOAP_ENVELOPE].localName;
    out << STRLIT(":Body>\n");
}

void SoapWriter::_appendSoapBodyEnd(Buffer& out)
{
    out << STRLIT("</");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::SOAP_ENVELOPE].localName;
    out << STRLIT(":Body>\n");
}

void SoapWriter::_appendSoapHeader(
    Buffer& out, 
    const String& rspName, 
    const String& messageId)
{
    // Header start tag
    out << STRLIT("<");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::SOAP_ENVELOPE].localName;
    out << STRLIT(":Header>\n");

    // ATTN WSMAN: this assumes we reply on the requestor's connection
    // Add <wsa:To> entry
    out << STRLIT("<");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::WS_ADDRESSING].localName;
    out << STRLIT(":To>\n");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::WS_ADDRESSING].extendedName;
    out << STRLIT("/role/anonymous");
    out << STRLIT("</");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::WS_ADDRESSING].localName;
    out << STRLIT(":To>\n");

    // Add <wsa:Action> entry
    out << STRLIT("<");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::WS_ADDRESSING].localName;
    out << STRLIT(":Action>\n");
    out << rspName;
    out << STRLIT("</");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::WS_ADDRESSING].localName;
    out << STRLIT(":Action>\n");

    // Add <wsa:MessageID> entry
    out << STRLIT("<");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::WS_ADDRESSING].localName;
    out << STRLIT(":MessageID>\n");
    out << SoapUtils::getMessageId();
    out << STRLIT("</");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::WS_ADDRESSING].localName;
    out << STRLIT(":MessageID>\n");

    // Add <wsa:RelatesTo> entry
    out << STRLIT("<");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::WS_ADDRESSING].localName;
    out << STRLIT(":RelatesTo>\n");
    out << messageId;
    out << STRLIT("</");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::WS_ADDRESSING].localName;
    out << STRLIT(":RelatesTo>\n");

    // Header end tag
    out << STRLIT("</");
    out << SoapNamespaces::supportedNamespaces[
        SoapNamespaces::SOAP_ENVELOPE].localName;
    out << STRLIT(":Header>\n");
}

// Helper functions for appendValueElement()
void SoapWriter::_appendValueArray(
    Buffer& out, const CIMObjectPath* p, Uint32 size, const CIMName& name)
{
    // ATTN WSMAN: need to implement
    while (size--)
    {
        _appendValue(out, *p++);
    }
}

template<class T>
void SoapWriter::_appendValueArray(
    Buffer& out, const T* p, Uint32 size, const CIMName& name)
{
    while (size--)
    {
        out << STRLIT("<class:") << name << STRLIT(">");
        _appendValue(out, *p++);
        out << STRLIT("</class:") << name << STRLIT(">\n");
    }
}

void SoapWriter::_appendValueReferenceElement(
    Buffer& out,
    const CIMObjectPath& reference,
    Boolean putValueWrapper)
{
}

void SoapWriter::appendValueElement(
    Buffer& out,
    const CIMValue& value,
    const CIMName& name)
{
    if (value.isNull())
    {
        return;
    }
    if (value.isArray())
    {
        switch (value.getType())
        {
            case CIMTYPE_BOOLEAN:
            {
                Array<Boolean> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_UINT8:
            {
                Array<Uint8> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_SINT8:
            {
                Array<Sint8> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_UINT16:
            {
                Array<Uint16> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_SINT16:
            {
                Array<Sint16> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_UINT32:
            {
                Array<Uint32> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_SINT32:
            {
                Array<Sint32> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_UINT64:
            {
                Array<Uint64> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_SINT64:
            {
                Array<Sint64> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_REAL32:
            {
                Array<Real32> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_REAL64:
            {
                Array<Real64> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Array<Char16> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_STRING:
            {
                const String* data;
                Uint32 size;
                value._get(data, size);
                _appendValueArray(out, data, size, name);
                break;
            }

            case CIMTYPE_DATETIME:
            {
                Array<CIMDateTime> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_REFERENCE:
            {
                Array<CIMObjectPath> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }

            case CIMTYPE_OBJECT:
            {
                Array<CIMObject> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            case CIMTYPE_INSTANCE:
            {
                Array<CIMInstance> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size(), name);
                break;
            }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            default:
                PEGASUS_ASSERT(false);
        }
    }
    else if (value.getType() == CIMTYPE_REFERENCE)
    {
        // Has to be separate because it uses VALUE.REFERENCE tag
        CIMObjectPath v;
        value.get(v);
        _appendValue(out, v);
    }
    else
    {
        out << STRLIT("<class:") << name << STRLIT(">");

        switch (value.getType())
        {
            case CIMTYPE_BOOLEAN:
            {
                Boolean v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT8:
            {
                Uint8 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT8:
            {
                Sint8 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT16:
            {
                Uint16 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT16:
            {
                Sint16 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT32:
            {
                Uint32 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT32:
            {
                Sint32 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT64:
            {
                Uint64 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT64:
            {
                Sint64 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_REAL32:
            {
                Real32 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_REAL64:
            {
                Real64 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Char16 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_STRING:
            {
                String v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_DATETIME:
            {
                CIMDateTime v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_OBJECT:
            {
                CIMObject v;
                value.get(v);
                _appendValue(out, v);
                break;
            }
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            case CIMTYPE_INSTANCE:
            {
                CIMInstance v;
                value.get(v);
                _appendValue(out, v);
                break;
            }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            default:
                PEGASUS_ASSERT(false);
        }

        out << STRLIT("</class:") << name << STRLIT(">");
    }
}

PEGASUS_NAMESPACE_END
