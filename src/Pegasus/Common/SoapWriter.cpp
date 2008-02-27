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
#include <Pegasus/Common/StatisticalData.h>

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
#include "CIMQualifierDecl.h"
#include "CIMQualifierDeclRep.h"
#include "CIMValue.h"
#include "SoapUtils.h"
#include "SoapWriter.h"
#include "XmlParser.h"
#include "Tracer.h"
#include "CommonUTF.h"
#include "Buffer.h"
#include "StrLit.h"
#include "LanguageParser.h"
#include "IDFactory.h"
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


    return out;
}

void SoapWriter::appendInstanceElement(
    Buffer& out,
    const CIMConstInstance& instance)
{
    CheckRep(instance._rep);
    instance._rep->toSoap(out);
}

void SoapWriter::appendPropertyElement(
    Buffer& out,
    const CIMConstProperty& property)
{
    CheckRep(property._rep);
    property._rep->toSoap(out);
}

void SoapWriter::appendQualifierElement(
    Buffer& out,
    const CIMConstQualifier& qualifier)
{
    CheckRep(qualifier._rep);
    qualifier._rep->toSoap(out);
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
    OUTPUT_CONTENTLENGTH;

    if (contentLanguages.size() > 0)
    {
        out << STRLIT("Content-Language: ") << contentLanguages <<
            STRLIT("\r\n");
    }
    if (httpMethod == HTTP_METHOD_M_POST)
    {
        // TODO: not sure about this!!!
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

    // TODO: this assumes we reply on the requestor's connection
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

PEGASUS_NAMESPACE_END
