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
    _parser(text), 
    _scopeLevel(0),
    _currentSoapHeader(0),
    _currentSoapBody(0)
{
}

SoapReader::~SoapReader()
{
}


CIMName SoapReader::getNameAttribute(
    SoapEntry* soapEntry, 
    const char* elementName,
    Boolean acceptNull)
{
    String name;

    if (!soapEntry->entry.getAttributeValue("Name", name))
    {
        // TODO: throw an exception
    }

    if (acceptNull && name.size() == 0)
        return CIMName ();

    if (!CIMName::legal(name))
    {
        // TODO: throw an exception
    }

    return CIMNameUnchecked(name);
}


void SoapReader::decodeClassName(SoapEntry* soapEntry, String& className)
{
    PEGASUS_ASSERT(_currentSoapHeader < _soapHeader.size() &&
        testSoapStartTag(soapEntry, SoapNamespaces::WS_MAN, "ResourceURI"));

    soapEntry = nextSoapHeaderEntry();
    if (!soapEntry || soapEntry->entry.type != XmlEntry::CONTENT)
    {
        // TODO: throw an exception
    }

    const char* slash = strrchr(soapEntry->entry.text, '/');
    className = slash ? slash + 1 : soapEntry->entry.text;

    if (!testSoapEndTag(nextSoapHeaderEntry(), SoapNamespaces::WS_MAN, 
                        "ResourceURI"))
    {
        // TODO: throw an exception
    }
}


void SoapReader::decodeKeyBindingElement(SoapEntry* soapEntry, CIMName& name,
    String& value, CIMKeyBinding::Type& type)
{
    PEGASUS_ASSERT(_currentSoapHeader < _soapHeader.size() &&
        testSoapStartTag(soapEntry, SoapNamespaces::WS_MAN, "Selector"));

    name = getNameAttribute(soapEntry, "Selector");

    soapEntry = nextSoapHeaderEntry();
    if (!soapEntry || soapEntry->entry.type != XmlEntry::CONTENT)
    {
        // TODO: throw an exception
    }
    value = soapEntry->entry.text;

    // Set the key type to STRING; it will be fixed up by the dipatcher.
    type = CIMKeyBinding::STRING;

    if (!testSoapEndTag(
            nextSoapHeaderEntry(), SoapNamespaces::WS_MAN, "Selector"))
    {
        // TODO: throw an exception
    }
}


void SoapReader::decodeKeyBindings(
    SoapEntry* soapEntry,
    Array<CIMKeyBinding>& keyBindings, 
    String& nameSpace)
{
    PEGASUS_ASSERT(_currentSoapHeader < _soapHeader.size() &&
        testSoapStartTag(soapEntry, SoapNamespaces::WS_MAN, "SelectorSet"));

    keyBindings.clear();
    nameSpace.clear();
    while ((soapEntry = nextSoapHeaderEntry()) != 0 &&
           !testSoapEndTag(soapEntry, SoapNamespaces::WS_MAN, "SelectorSet"))
    {
        CIMName name;
        String value;
        CIMKeyBinding::Type type;
        if (testSoapStartTag(soapEntry, SoapNamespaces::WS_MAN, "Selector"))
        {
            decodeKeyBindingElement(soapEntry, name, value, type);

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
            // TODO: throw an exception
        }
    }

    // If neither keyBindings nor nameSpace has been set, the selector set
    // is empty and we need to report an error
    if (keyBindings.size() == 0 && nameSpace.size() == 0)
    {
        // TODO: throw an exception
    }
}


void SoapReader::decodeMessageId(SoapEntry* soapEntry, String& messageId)
{
    PEGASUS_ASSERT(_currentSoapHeader < _soapHeader.size() &&
        testSoapStartTag(soapEntry, SoapNamespaces::WS_ADDRESSING, 
            "MessageID"));

    soapEntry = nextSoapHeaderEntry();
    if (!soapEntry || soapEntry->entry.type != XmlEntry::CONTENT)
    {
        // TODO: throw an exception
    }

    messageId = soapEntry->entry.text;

    if (!testSoapEndTag(nextSoapHeaderEntry(), SoapNamespaces::WS_ADDRESSING,
            "MessageID"))
    {
        // TODO: throw an exception
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

        for (int i = 0; 
             SoapNamespaces::supportedNamespaces[i].type != 
                 SoapNamespaces::LAST; 
             i++)
        {
            String extName(
                SoapNamespaces::supportedNamespaces[i].extendedName);
            if (nameSpace == extName)
            {
                nsType = SoapNamespaces::supportedNamespaces[i].type;
                return;
            }
        }
    }

    action = String::EMPTY;
    nsType = SoapNamespaces::UNKNOWN;
}


SoapNamespaces::Type SoapReader::_getNamespaceType(const char* name)
{
    const char* pos = strchr(name, ':');

    // If ":" is not found, the name is not namespace qualified
    if (pos == NULL)
    {
        return SoapNamespaces::UNKNOWN;
    }

    // Search the namespace stack from the top
    for (int i = _nameSpaces.size() - 1; i >=0; i--)
    {
        if (!strncmp(_nameSpaces[i].localName, name, pos - name))
        {
            return _nameSpaces[i].type;
        }
    }
    return SoapNamespaces::UNKNOWN;
}


SoapNamespaces::Namespace* SoapReader::_getNamespace(
    SoapNamespaces::Type nsType)
{
    for (int i = _nameSpaces.size() - 1; i >=0; i--)
    {
        if (_nameSpaces[i].type == nsType)
        {
            return &_nameSpaces[i];
        }
    }
    return 0;
}


Boolean SoapReader::_next(
    XmlEntry& entry, 
    SoapNamespaces::Type& nsType, 
    Boolean includeComment)
{
    Boolean firstTime;
    nsType = SoapNamespaces::UNKNOWN;

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
            if ((nsType = _getNamespaceType(entry.text)) == 
                    SoapNamespaces::UNKNOWN)
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
                SoapNamespaces::Namespace ns;
                ns.localName = attr.name + 6;
                ns.extendedName = attr.value;
                ns.scopeLevel = _scopeLevel;
                
                // Make sure we know how to deal with entries from
                // this namespace
                if (SoapNamespaces::isSupportedNamespace(&ns))
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
        if ((nsType = _getNamespaceType(entry.text)) == 
            SoapNamespaces::UNKNOWN)
        {
            // TODO: trow an exception
        }
    }
    else if (entry.type == XmlEntry::END_TAG)
    {
        // The tag must be namespace qualified with a known namespace
        if ((nsType = _getNamespaceType(entry.text)) == 
            SoapNamespaces::UNKNOWN)
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
        nsType = SoapNamespaces::UNKNOWN;
    }

    return true;
}


void SoapReader::_expectStartTag(
    XmlEntry& entry, 
    SoapNamespaces::Type nsType, 
    const char* tagName)
{
    SoapNamespaces::Type nst;
    const char* pos;
    if (!_next(entry, nst) ||
        entry.type != XmlEntry::START_TAG ||
        nsType != nst ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {

        SoapNamespaces::Namespace* ns = _getNamespace(nsType);
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
    SoapNamespaces::Type nst;
    const char* pos;
    if (!_next(entry, nst) ||
        (entry.type != XmlEntry::START_TAG &&
         entry.type != XmlEntry::EMPTY_TAG) ||
        nsType != nst ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {
        SoapNamespaces::Namespace* ns = _getNamespace(nsType);
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
    SoapNamespaces::Type nst;
    const char* pos;
    if (!_next(entry, nst) ||
        entry.type != XmlEntry::END_TAG ||
        nsType != nst ||
        (pos = strchr(entry.text, ':')) == NULL ||
        strcmp(pos + 1, tagName) != 0)
    {
        SoapNamespaces::Namespace* ns = _getNamespace(nsType);
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
    SoapNamespaces::Type nst;
    const char* pos;

    if (!_next(entry, nst) ||
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


Boolean SoapReader::testSoapStartTag(
    SoapEntry* soapEntry, 
    SoapNamespaces::Type nsType, 
    const char* tagName)
{
    const char* pos;
    if (soapEntry->nsType == nsType &&
        soapEntry->entry.type == XmlEntry::START_TAG &&
        (pos = strchr(soapEntry->entry.text, ':')) != NULL &&
        strcmp(pos + 1, tagName) == 0)
    {
        return true;
    }
    return false;
}


Boolean SoapReader::testSoapEndTag(
    SoapEntry* soapEntry, 
    SoapNamespaces::Type nsType, 
    const char* tagName)
{
    const char* pos;
    if (soapEntry->nsType == nsType &&
        soapEntry->entry.type == XmlEntry::END_TAG &&
        (pos = strchr(soapEntry->entry.text, ':')) != NULL &&
        strcmp(pos + 1, tagName) == 0)
    {
        return true;
    }
    return false;
}


void SoapReader::processSoapEnvelope(String& soapAction)
{
    SoapEntry soapEntry;
    String wsaAction;

    // Read SOAP-ENV:Envelope start tag.
    _processEnvelopeStartTag();

    // Read SOAP-ENV:Header start tag.
    _processHeaderStartTag();

    // Read SOAP-ENV:Header content
    while (!_testEndTag(SoapNamespaces::SOAP_ENVELOPE, "Header"))
    {
        _next(soapEntry.entry, soapEntry.nsType);

        // We are looking for <wsa:Action> tag
        if (testSoapStartTag(&soapEntry, 
               SoapNamespaces::WS_ADDRESSING, "Action"))
        {
            _expectContentOrCData(soapEntry.entry);
            wsaAction = soapEntry.entry.text;

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
                // TODO: throw an exception
            }

            // Next should be the end tag for <wsa:Action>
            _expectEndTag(soapEntry.entry,  SoapNamespaces::WS_ADDRESSING, 
                "Action");

            // Read the entry following </wsa:Action> so it can be
            // added tot the array
            _next(soapEntry.entry, soapEntry.nsType);
        }

        _soapHeader.append(soapEntry);
    }

    // Read SOAP-ENV:Header end tag.
    _processHeaderEndTag();

    // Read SOAP-ENV:Body start tag.
    if (_processBodyStartTag())
    {
        // Read SOAP-ENV:Body content
        while (!_testEndTag(SoapNamespaces::SOAP_ENVELOPE, "Body"))
        {
            _next(soapEntry.entry, soapEntry.nsType);
            _soapBody.append(soapEntry);
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
