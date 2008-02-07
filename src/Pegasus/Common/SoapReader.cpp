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

SoapReader::SoapNamespace SoapReader::_supportedNamespaces[] = 
{
    {"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope",
     NST_SOAP_ENVELOPE,       0},
    {"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding",
     NST_SOAP_ENCODING,       0}, 
    {"xsi",      "http://www.w3.org/2001/XMLSchema-instance",
     NST_XML_SCHEMA_INSTANCE, 0},
    {"xsd",      "http://www.w3.org/2001/XMLSchema",
     NST_XML_SCHEMA,          0}, 
    {"wsman",    "http://schemas.dmtf.org/wbem/wsman/1/wsman.xsd",
     NST_WS_MAN,              0},
    {"wsmb",     "http://schemas.dmtf.org/wbem/wsman/1/cimbinding.xsd", 
     NST_WS_CIM_BINDING,      0},
    {"wsa",      "http://schemas.xmlsoap.org/ws/2004/08/addressing",
     NST_WS_ADDRESSING,       0},
    {"wxf",      "http://schemas.xmlsoap.org/ws/2004/09/transfer",
     NST_WS_TRANSFER,         0},
    {"wsen",     "http://schemas.xmlsoap.org/ws/2004/09/enumeration",
     NST_WS_ENUMERATION,      0}, 
    {"wse",      "http://schemas.xmlsoap.org/ws/2004/08/eventing",
     NST_WS_EVENTING,         0},
    {"wsp",      "http://schemas.xmlsoap.org/ws/2004/09/policy",
     NST_WS_POLICY,           0},
    {"wsdl",     "http://schemas.xmlsoap.org/wsdl",
     NST_WSDL,                0},
    {"",         "",
     NST_LAST, 0}
};


Boolean SoapReader::_isSupportedNamespace(SoapNamespace* ns)
{
    for (int i = 0; i < NST_COUNT; i++)
    {
        if (!strcmp(_supportedNamespaces[i].extendedName, ns->extendedName))
        {
            ns->namespaceType = _supportedNamespaces[i].namespaceType;
            return true;
        }
    }
    return false;
}


SoapReader::NSType SoapReader::_getNamespaceType(const char* name)
{
    const char* pos = strchr(name, ':');

    // If ":" is not found, the name is not namespace qualified
    if (pos == NULL)
    {
        return NST_UNKNOWN;
    }

    // Search the namespace stack from the top
    for (int i = _nameSpaces.size() - 1; i >=0; i--)
    {
        if (!strncmp(_nameSpaces[i].localName, name, pos - name))
        {
            return _nameSpaces[i].namespaceType;
        }
    }
    return NST_UNKNOWN;
}


SoapReader::SoapNamespace* SoapReader::_getNamespace(SoapReader::NSType nsType)
{
    for (int i = _nameSpaces.size() - 1; i >=0; i--)
    {
        if (_nameSpaces[i].namespaceType == nsType)
        {
            return &_nameSpaces[i];
        }
    }
    return 0;
}


Boolean SoapReader::next(
    XmlEntry& entry, 
    NSType& nsType, 
    Boolean includeComment)
{
    Boolean firstTime;
    nsType = NST_UNKNOWN;

    // Get the next entry from the parser
    if (!_parser.next(entry, includeComment, &firstTime))
    {
        return false;
    }

    // We don't want to process this entry's namespaces if
    // it was popped off the parser's putback stack.
    if (!firstTime)
    {
        // Still need to look up the namespace
        if (entry.type == XmlEntry::START_TAG ||
            entry.type == XmlEntry::END_TAG ||
            entry.type == XmlEntry::EMPTY_TAG)
        {
            // The tag must be namespace qualified with a known namespace
            if ((nsType = _getNamespaceType(entry.text)) == NST_UNKNOWN)
            {
                // TODO: trow an exception
            }
        }
        return true;
    }

    // Process attributes and enter namespaces into the table
    if (entry.type == XmlEntry::START_TAG ||
        entry.type == XmlEntry::EMPTY_TAG)
    {
        _scopeLevel++;
        for (unsigned int i = 0; i < entry.attributes.size(); i++)
        {
            XmlAttribute attr = entry.attributes[i];
            if (strncmp(attr.name, "xmlns:", 6) == 0)
            {
                SoapNamespace ns;
                ns.localName = attr.name + 6;
                ns.extendedName = attr.value;
                ns.scopeLevel = _scopeLevel;
                
                // Make sure we know how to deal with entries from
                // this namespace
                if (_isSupportedNamespace(&ns))
                {
                    _nameSpaces.push(ns);
                }
                else
                {
                    // TODO: throw an exception
                }
            }
        }
        
        // The tag must be namespace qualified with a known namespace
        if ((nsType = _getNamespaceType(entry.text)) == NST_UNKNOWN)
        {
            // TODO: trow an exception
        }
    }
    else if (entry.type == XmlEntry::END_TAG)
    {
        // The tag must be namespace qualified with a known namespace
        if ((nsType = _getNamespaceType(entry.text)) == NST_UNKNOWN)
        {
            // TODO: trow an exception
        }

        // Remove any namespaces of the current scope level from
        // the scope stack.
        while (!_nameSpaces.isEmpty() &&
               _scopeLevel <= _nameSpaces.top().scopeLevel)
        {
            _nameSpaces.pop();
        }

        PEGASUS_ASSERT(_scopeLevel > 0);
        _scopeLevel--;
    }
    else
    {
        nsType = NST_UNKNOWN;
    }

    return true;
}


void SoapReader::expectStartTag(
    XmlEntry& entry, 
    NSType nsType, 
    const char* tagName)
{
    NSType nst;
    const char* pos;
    if (!next(entry, nst) ||
        entry.type != XmlEntry::START_TAG ||
        nsType != nst ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {

        SoapReader::SoapNamespace* ns = _getNamespace(nsType);
        PEGASUS_ASSERT(ns);
        MessageLoaderParms mlParms(
            "Common.SoapReader.EXPECTED_OPEN",
            "Expecting a start tag for $0:$1 element",
            ns->localName, tagName);
        throw XmlValidationError(_parser.getLine(), mlParms);
    }
}


void SoapReader::expectStartOrEmptyTag(
    XmlEntry& entry, 
    NSType nsType, 
    const char* tagName)
{
    NSType nst;
    const char* pos;
    if (!next(entry, nst) ||
        (entry.type != XmlEntry::START_TAG &&
         entry.type != XmlEntry::EMPTY_TAG) ||
        nsType != nst ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {
        SoapReader::SoapNamespace* ns = _getNamespace(nsType);
        PEGASUS_ASSERT(ns);
        MessageLoaderParms mlParms(
            "Common.SoapReader.EXPECTED_OPENCLOSE",
            "Expecting a start tag or an empty tag for $0:$1 element",
            ns->localName, tagName);
        throw XmlValidationError(_parser.getLine(), mlParms);
    }
}


void SoapReader::expectEndTag(
    XmlEntry& entry, 
    NSType nsType, 
    const char* tagName)
{
    NSType nst;
    const char* pos;
    if (!next(entry, nst) ||
        entry.type != XmlEntry::END_TAG ||
        nsType != nst ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {
        SoapReader::SoapNamespace* ns = _getNamespace(nsType);
        PEGASUS_ASSERT(ns);
        MessageLoaderParms mlParms(
            "Common.SoapReader.EXPECTED_CLOSE",
            "Expecting an end tag for $0:$1 element",
            ns->localName, tagName);
        throw XmlValidationError(_parser.getLine(), mlParms);
    }
}


Boolean SoapReader::testEndTag(NSType nsType, const char* tagName)
{
    XmlEntry entry;
    NSType nst;
    const char* pos;

    if (!next(entry, nst) ||
        entry.type != XmlEntry::END_TAG ||
        nsType != nst ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {
        _parser.putBack(entry);
        return false;
    }

    _parser.putBack(entry);
    return true;
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
//          . . .
//     >
//
//-----------------------------------------------------------------------------
void SoapReader::processEnvelopeStartTag()
{
    XmlEntry entry;
    expectStartTag(entry, NST_SOAP_ENVELOPE, "Envelope");
}

//-----------------------------------------------------------------------------
//
// processEnvelopeEndTag()
//
//     </SOAP-ENV:Envelope>
//
//-----------------------------------------------------------------------------
void SoapReader::processEnvelopeEndTag()
{
    XmlEntry entry;
    expectEndTag(entry, NST_SOAP_ENVELOPE, "Envelope");
}

//-----------------------------------------------------------------------------
//
// processHeaderStartTag()
//
//     <SOAP-ENV:Header>
//
//-----------------------------------------------------------------------------
void SoapReader::processHeaderStartTag()
{
    XmlEntry entry;
    expectStartTag(entry, NST_SOAP_ENVELOPE, "Header");
}

//-----------------------------------------------------------------------------
//
// processHeaderEndTag()
//
//     </SOAP-ENV:Header>
//
//-----------------------------------------------------------------------------
void SoapReader::processHeaderEndTag()
{
    XmlEntry entry;
    expectEndTag(entry, NST_SOAP_ENVELOPE, "Header");
}

//-----------------------------------------------------------------------------
//
// isHeaderEndTag()
//
//     Test of the next token is </SOAP-ENV:Header>
//
//-----------------------------------------------------------------------------
Boolean SoapReader::isHeaderEndTag()
{
    return testEndTag(NST_SOAP_ENVELOPE, "Header");
}

//-----------------------------------------------------------------------------
//
// processBodyStartTag()
//
//     <SOAP-ENV:Body>
//
//-----------------------------------------------------------------------------
Boolean SoapReader::processBodyStartTag()
{
    XmlEntry entry;
    expectStartOrEmptyTag(entry, NST_SOAP_ENVELOPE, "Body");
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
void SoapReader::processBodyEndTag()
{
    XmlEntry entry;
    expectEndTag(entry, NST_SOAP_ENVELOPE, "Body");
}

//-----------------------------------------------------------------------------
//
// isBodyEndTag()
//
//     Test of the next token is </SOAP-ENV:Body>
//
//-----------------------------------------------------------------------------
Boolean SoapReader::isBodyEndTag()
{
    return testEndTag(NST_SOAP_ENVELOPE, "Body");
}

PEGASUS_NAMESPACE_END
