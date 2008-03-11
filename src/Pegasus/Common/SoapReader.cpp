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
#include <errno.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Common/AutoPtr.h>

#include "CIMName.h"
#include "CIMNameUnchecked.h"
#include "CIMQualifier.h"
#include "CIMQualifierDecl.h"
#include "CIMClass.h"
#include "CIMInstance.h"
#include "CIMObject.h"
#include "CIMParamValue.h"
#include "System.h"
#include "XmlReader.h"
#include "SoapReader.h"
#include "ExceptionRep.h"


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//
// SoapReader
//
///////////////////////////////////////////////////////////////////////////////

SoapReader::SoapReader(char* text) : 
    _parser(text, SoapNamespaces::supportedNamespaces), 
    _currentSoapHeader(0),
    _currentSoapBody(0)
{
}

SoapReader::~SoapReader()
{
}


CIMName SoapReader::getNameAttribute(
    XmlEntry* entry, 
    const char* elementName,
    Boolean acceptNull)
{
    String name;

    if (!entry->getAttributeValue("Name", name))
    {
        // ATTN WSMAN: throw an exception
    }

    if (acceptNull && name.size() == 0)
        return CIMName ();

    if (!CIMName::legal(name))
    {
        // ATTN WSMAN: throw an exception
    }

    return CIMNameUnchecked(name);
}


void SoapReader::decodeClassName(XmlEntry* entry, String& className)
{
    PEGASUS_ASSERT(_currentSoapHeader < _soapHeader.size() &&
        testSoapStartTag(entry, SoapNamespaces::WS_MAN, "ResourceURI"));

    entry = nextSoapHeaderEntry();
    if (!entry || entry->type != XmlEntry::CONTENT)
    {
        // ATTN WSMAN: throw an exception
    }

    const char* slash = strrchr(entry->text, '/');
    className = slash ? slash + 1 : entry->text;

    if (!testSoapEndTag(nextSoapHeaderEntry(), SoapNamespaces::WS_MAN, 
                        "ResourceURI"))
    {
        // ATTN WSMAN: throw an exception
    }
}


void SoapReader::decodeKeyBindingElement(XmlEntry* entry, CIMName& name,
    String& value, CIMKeyBinding::Type& type)
{
    PEGASUS_ASSERT(_currentSoapHeader < _soapHeader.size() &&
        testSoapStartTag(entry, SoapNamespaces::WS_MAN, "Selector"));

    name = getNameAttribute(entry, "Selector");

    entry = nextSoapHeaderEntry();
    if (!entry || entry->type != XmlEntry::CONTENT)
    {
        // ATTN WSMAN: throw an exception
    }
    value = entry->text;

    // Set the key type to STRING; it will be fixed up by the dipatcher.
    type = CIMKeyBinding::STRING;

    if (!testSoapEndTag(
            nextSoapHeaderEntry(), SoapNamespaces::WS_MAN, "Selector"))
    {
        // ATTN WSMAN: throw an exception
    }
}


void SoapReader::decodeKeyBindings(
    XmlEntry* entry,
    Array<CIMKeyBinding>& keyBindings, 
    String& nameSpace)
{
    PEGASUS_ASSERT(_currentSoapHeader < _soapHeader.size() &&
        testSoapStartTag(entry, SoapNamespaces::WS_MAN, "SelectorSet"));

    keyBindings.clear();
    nameSpace.clear();
    while ((entry = nextSoapHeaderEntry()) != 0 &&
           !testSoapEndTag(entry, SoapNamespaces::WS_MAN, "SelectorSet"))
    {
        CIMName name;
        String value;
        CIMKeyBinding::Type type;
        if (testSoapStartTag(entry, SoapNamespaces::WS_MAN, "Selector"))
        {
            decodeKeyBindingElement(entry, name, value, type);

            // If the name is __cimnamespace, it's a special selector we
            // use to set the CIM namespace.
            if (name.getString() == "__cimnamespace")
            {
                nameSpace = value;
            }
            else
            {
                keyBindings.append(CIMKeyBinding(name, value, type));
            }
        }
        else
        {
            // ATTN WSMAN: throw an exception
        }
    }

    // If neither keyBindings nor nameSpace has been set, the selector set
    // is empty and we need to report an error
    if (keyBindings.size() == 0 && nameSpace.size() == 0)
    {
        // ATTN WSMAN: throw an exception
    }
}


void SoapReader::decodeMessageId(XmlEntry* entry, String& messageId)
{
    PEGASUS_ASSERT(_currentSoapHeader < _soapHeader.size() &&
        testSoapStartTag(entry, SoapNamespaces::WS_ADDRESSING, 
            "MessageID"));

    entry = nextSoapHeaderEntry();
    if (!entry || entry->type != XmlEntry::CONTENT)
    {
        // ATTN WSMAN: throw an exception
    }

    messageId = entry->text;

    if (!testSoapEndTag(nextSoapHeaderEntry(), SoapNamespaces::WS_ADDRESSING,
            "MessageID"))
    {
        // ATTN WSMAN: throw an exception
    }
}


void SoapReader::decodeSoapAction(
    String& soapAction, String& action, SoapNamespaces::Type& nsType)
{
    Uint32 pos = soapAction.reverseFind('/');

    if (pos != PEG_NOT_FOUND)
    {
        String nameSpace = soapAction.subString(0, pos);
        action = soapAction.subString(pos + 1);
        nsType = (SoapNamespaces::Type) 
            _parser.getSupportedNamespaceType(nameSpace.getCString());
        return;
    }

    action = String::EMPTY;
    nsType = SoapNamespaces::UNKNOWN;
}


void SoapReader::_expectStartTag(
    XmlEntry& entry, 
    SoapNamespaces::Type nsType, 
    const char* tagName)
{
    const char* pos;
    if (!_parser.next(entry) ||
        entry.type != XmlEntry::START_TAG ||
        entry.nsType != nsType ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {

        XmlNamespace* ns = _parser.getNamespace(nsType);
        PEGASUS_ASSERT(ns);
        MessageLoaderParms mlParms(
            "Common.SoapReader.EXPECTED_OPEN",
            "Expecting a start tag for $0:$1 element",
            ns->localName, tagName);
        throw XmlValidationError(_parser.getLine(), mlParms);
    }
}


void SoapReader::_expectStartOrEmptyTag(
    XmlEntry& entry, 
    SoapNamespaces::Type nsType, 
    const char* tagName)
{
    const char* pos;
    if (!_parser.next(entry) ||
        (entry.type != XmlEntry::START_TAG &&
         entry.type != XmlEntry::EMPTY_TAG) ||
        entry.nsType != nsType ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {
        XmlNamespace* ns = _parser.getNamespace(nsType);
        PEGASUS_ASSERT(ns);
        MessageLoaderParms mlParms(
            "Common.SoapReader.EXPECTED_OPENCLOSE",
            "Expecting a start tag or an empty tag for $0:$1 element",
            ns->localName, tagName);
        throw XmlValidationError(_parser.getLine(), mlParms);
    }
}


void SoapReader::_expectEndTag(
    XmlEntry& entry, 
    SoapNamespaces::Type nsType, 
    const char* tagName)
{
    const char* pos;
    if (!_parser.next(entry) ||
        entry.type != XmlEntry::END_TAG ||
        entry.nsType != nsType ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {
        XmlNamespace* ns = _parser.getNamespace(nsType);
        PEGASUS_ASSERT(ns);
        MessageLoaderParms mlParms(
            "Common.SoapReader.EXPECTED_CLOSE",
            "Expecting an end tag for $0:$1 element",
            ns->localName, tagName);
        throw XmlValidationError(_parser.getLine(), mlParms);
    }
}


Boolean SoapReader::_testEndTag(
    SoapNamespaces::Type nsType, 
    const char* tagName)
{
    XmlEntry entry;
    const char* pos;

    if (!_parser.next(entry) ||
        entry.type != XmlEntry::END_TAG ||
        entry.nsType != nsType ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {
        _parser.putBack(entry);
        return false;
    }

    _parser.putBack(entry);
    return true;
}


Boolean SoapReader::testSoapStartTag(
    XmlEntry* entry, 
    SoapNamespaces::Type nsType, 
    const char* tagName)
{
    const char* pos;
    if (entry->nsType == nsType &&
        entry->type == XmlEntry::START_TAG &&
        (pos = strchr(entry->text, ':')) != NULL &&
        strcmp(pos + 1, tagName) == 0)
    {
        return true;
    }
    return false;
}


Boolean SoapReader::testSoapEndTag(
    XmlEntry* entry, 
    SoapNamespaces::Type nsType, 
    const char* tagName)
{
    const char* pos;
    if (entry->nsType == nsType &&
        entry->type == XmlEntry::END_TAG &&
        (pos = strchr(entry->text, ':')) != NULL &&
        strcmp(pos + 1, tagName) == 0)
    {
        return true;
    }
    return false;
}


void SoapReader::processSoapEnvelope(String& soapAction)
{
    XmlEntry entry;
    String wsaAction;

    // Read SOAP-ENV:Envelope start tag.
    _processEnvelopeStartTag();

    // Read SOAP-ENV:Header start tag.
    _processHeaderStartTag();

    // Read SOAP-ENV:Header content
    while (!_testEndTag(SoapNamespaces::SOAP_ENVELOPE, "Header"))
    {
        _parser.next(entry);

        // We are looking for <wsa:Action> tag
        if (testSoapStartTag(&entry, 
               SoapNamespaces::WS_ADDRESSING, "Action"))
        {
            _expectContentOrCData(entry);
            wsaAction = entry.text;

            // If SOAPAction HTTP header has not been defined, set the
            // action to be whatever <wsa:Action> says.
            if (soapAction.size() == 0)
            {
                soapAction = wsaAction;
            }
            // If SOAPAction has been set, it must match the value of
            // the <wsa:Action> tag.
            else if (wsaAction != soapAction)
            {
                // ATTN WSMAN: throw an exception
            }

            // Next should be the end tag for <wsa:Action>
            _expectEndTag(entry,  SoapNamespaces::WS_ADDRESSING, 
                "Action");

            // Read the entry following </wsa:Action> so it can be
            // added tot the array
            _parser.next(entry);
        }

        _soapHeader.append(entry);
    }

    // Read SOAP-ENV:Header end tag.
    _processHeaderEndTag();

    // Read SOAP-ENV:Body start tag.
    if (_processBodyStartTag())
    {
        // Read SOAP-ENV:Body content
        while (!_testEndTag(SoapNamespaces::SOAP_ENVELOPE, "Body"))
        {
            _parser.next(entry);
            _soapBody.append(entry);
        }

        // Read SOAP-ENV:Body end tag.
        _processBodyEndTag();
    }

    // Read SOAP-ENV:Envelope end tag.
    _processEnvelopeEndTag();

    
}

void SoapReader::_expectContentOrCData(XmlEntry& entry)
{
    XmlReader::expectContentOrCData(_parser, entry);
}

//-----------------------------------------------------------------------------
//
// getXmlDeclaration()
//
//     <?xml version="1.0" encoding="utf-8"?>
//
//-----------------------------------------------------------------------------
void SoapReader::getXmlDeclaration(
    const char*& xmlVersion,
    const char*& xmlEncoding)
{
    XmlReader::getXmlDeclaration(_parser, xmlVersion, xmlEncoding);
}

//-----------------------------------------------------------------------------
//
// processEnvelopeStartTag()
//
//     <SOAP-ENV:Envelope
//          xmlns:SOAP-ENV="http://www.w3.org/2003/05/soap-envelope"
//          xmlns:SOAP-ENC="http://www.w3.org/2003/05/soap-encoding"
//          . . . >
//
//-----------------------------------------------------------------------------
void SoapReader::_processEnvelopeStartTag()
{
    XmlEntry entry;
    _expectStartTag(entry, SoapNamespaces::SOAP_ENVELOPE, "Envelope");
}

//-----------------------------------------------------------------------------
//
// processEnvelopeEndTag()
//
//     </SOAP-ENV:Envelope>
//
//-----------------------------------------------------------------------------
void SoapReader::_processEnvelopeEndTag()
{
    XmlEntry entry;
    _expectEndTag(entry, SoapNamespaces::SOAP_ENVELOPE, "Envelope");
}

//-----------------------------------------------------------------------------
//
// processHeaderStartTag()
//
//     <SOAP-ENV:Header>
//
//-----------------------------------------------------------------------------
void SoapReader::_processHeaderStartTag()
{
    XmlEntry entry;
    _expectStartTag(entry, SoapNamespaces::SOAP_ENVELOPE, "Header");
}

//-----------------------------------------------------------------------------
//
// processHeaderEndTag()
//
//     </SOAP-ENV:Header>
//
//-----------------------------------------------------------------------------
void SoapReader::_processHeaderEndTag()
{
    XmlEntry entry;
    _expectEndTag(entry, SoapNamespaces::SOAP_ENVELOPE, "Header");
}

//-----------------------------------------------------------------------------
//
// processBodyStartTag()
//
//     <SOAP-ENV:Body>
//
//-----------------------------------------------------------------------------
Boolean SoapReader::_processBodyStartTag()
{
    XmlEntry entry;
    _expectStartOrEmptyTag(entry, SoapNamespaces::SOAP_ENVELOPE, "Body");
    if (entry.type == XmlEntry::EMPTY_TAG)
    {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// processBodyEndTag()
//
//     </SOAP-ENV:Body>
//
//-----------------------------------------------------------------------------
void SoapReader::_processBodyEndTag()
{
    XmlEntry entry;
    _expectEndTag(entry, SoapNamespaces::SOAP_ENVELOPE, "Body");
}

PEGASUS_NAMESPACE_END
