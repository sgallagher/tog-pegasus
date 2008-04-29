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

#include <cctype>
#include <cstdio>
#include <cstdlib>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/WsmServer/WsmConstants.h>
#include <Pegasus/WsmServer/WsmFault.h>
#include "WsmReader.h"

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//
// WsmReader
//
///////////////////////////////////////////////////////////////////////////////

WsmReader::WsmReader(char* text)
    : _parser(text, WsmNamespaces::supportedNamespaces)
{
}

WsmReader::~WsmReader()
{
}

//-----------------------------------------------------------------------------
//
// getXmlDeclaration()
//
//     <?xml version="1.0" encoding="utf-8"?>
//
//-----------------------------------------------------------------------------
Boolean WsmReader::getXmlDeclaration(
    const char*& xmlVersion,
    const char*& xmlEncoding)
{
    XmlEntry entry;

    if (_parser.next(entry))
    {
        if ((entry.type != XmlEntry::XML_DECLARATION) ||
            (strcmp(entry.text, "xml") != 0))
        {
            _parser.putBack(entry);
            return false;
        }

        entry.getAttributeValue("version", xmlVersion);
        entry.getAttributeValue("encoding", xmlEncoding);
        return true;
    }

    return false;
}

Boolean WsmReader::testStartTag(
    XmlEntry& entry,
    int nsType,
    const char* tagName)
{
    if (!_parser.next(entry))
    {
        return false;
    }

    if ((entry.type != XmlEntry::START_TAG) ||
        (entry.nsType != nsType) ||
        (tagName && strcmp(entry.localName, tagName) != 0))
    {
        _parser.putBack(entry);
        return false;
    }

    return true;
}

Boolean WsmReader::testStartOrEmptyTag(
    XmlEntry& entry,
    int nsType,
    const char* tagName)
{
    if (!_parser.next(entry))
    {
        return false;
    }

    if (((entry.type != XmlEntry::START_TAG) &&
         (entry.type != XmlEntry::EMPTY_TAG)) ||
        (entry.nsType != nsType) ||
        (tagName && strcmp(entry.localName, tagName) != 0))
    {
        _parser.putBack(entry);
        return false;
    }

    return true;
}

Boolean WsmReader::testEndTag(
    int nsType,
    const char* tagName)
{
    XmlEntry entry;

    if (!_parser.next(entry))
    {
        return false;
    }

    if ((entry.type != XmlEntry::END_TAG) ||
        (entry.nsType != nsType) ||
        (tagName && strcmp(entry.localName, tagName) != 0))
    {
        _parser.putBack(entry);
        return false;
    }

    return true;
}

void WsmReader::expectStartTag(
    XmlEntry& entry,
    int nsType,
    const char* tagName)
{
    if (!_parser.next(entry) ||
        entry.type != XmlEntry::START_TAG ||
        entry.nsType != nsType ||
        strcmp(entry.localName, tagName) != 0)
    {
        const char* nsUri;

        // The nsType must have already been declared in the XML or it must be
        // a supported namespace.
        XmlNamespace* ns = _parser.getNamespace(nsType);
        if (ns)
        {
            nsUri = ns->extendedName;
        }
        else
        {
            PEGASUS_ASSERT((nsType >= 0) && (nsType < WsmNamespaces::LAST));
            nsUri = WsmNamespaces::supportedNamespaces[nsType].extendedName;
        }

        MessageLoaderParms mlParms(
            "WsmServer.WsmReader.EXPECTED_OPEN",
            "Expecting a start tag for \"$0\" element in namespace \"$1\".",
            tagName, nsUri);
        throw XmlValidationError(_parser.getLine(), mlParms);
    }
}

void WsmReader::expectStartOrEmptyTag(
    XmlEntry& entry,
    int nsType,
    const char* tagName)
{
    if (!_parser.next(entry) ||
        (entry.type != XmlEntry::START_TAG &&
         entry.type != XmlEntry::EMPTY_TAG) ||
        entry.nsType != nsType ||
        strcmp(entry.localName, tagName) != 0)
    {
        const char* nsUri;

        // The nsType must have already been declared in the XML or it must be
        // a supported namespace.
        XmlNamespace* ns = _parser.getNamespace(nsType);
        if (ns)
        {
            nsUri = ns->extendedName;
        }
        else
        {
            PEGASUS_ASSERT((nsType >= 0) && (nsType < WsmNamespaces::LAST));
            nsUri = WsmNamespaces::supportedNamespaces[nsType].extendedName;
        }

        MessageLoaderParms mlParms(
            "WsmServer.WsmReader.EXPECTED_OPENCLOSE",
            "Expecting a start tag or an empty tag for \"$0\" element in "
                "namespace \"$1\".",
            tagName, nsUri);
        throw XmlValidationError(_parser.getLine(), mlParms);
    }
}

void WsmReader::expectEndTag(
    int nsType,
    const char* tagName)
{
    XmlEntry entry;

    if (!_parser.next(entry) ||
        entry.type != XmlEntry::END_TAG ||
        entry.nsType != nsType ||
        strcmp(entry.localName, tagName) != 0)
    {
        // The nsType must have already been declared in the XML.  (Note that
        // ns->localName is null for a default namespace declaration.)
        XmlNamespace* ns = _parser.getNamespace(nsType);
        PEGASUS_ASSERT(ns);
        MessageLoaderParms mlParms(
            "WsmServer.WsmReader.EXPECTED_CLOSE",
            "Expecting an end tag for \"$0\" element in namespace \"$1\".",
            tagName, ns->extendedName);
        throw XmlValidationError(_parser.getLine(), mlParms);
    }
}

void WsmReader::expectContentOrCData(XmlEntry& entry)
{
    XmlReader::expectContentOrCData(_parser, entry);
}

Boolean WsmReader::next(XmlEntry& entry)
{
    return _parser.next(entry);
}

Boolean WsmReader::getAttributeValue(
    Uint32 lineNumber,
    XmlEntry& entry,
    const char* attributeName,
    String& attributeValue,
    Boolean required)
{
    if (!entry.getAttributeValue(attributeName, attributeValue))
    {
        if (required)
        {
            MessageLoaderParms parms(
                "WsmServer.WsmReader.MISSING_ATTRIBUTE",
                "The attribute $0.$1 is missing.",
                entry.text,
                attributeName);
            throw XmlValidationError(lineNumber, parms);
        }

        return false;
    }

    return true;
}

Boolean WsmReader::mustUnderstand(XmlEntry& entry)
{
    const XmlAttribute* attr = entry.findAttribute(
        WsmNamespaces::SOAP_ENVELOPE, "mustUnderstand");
    return (attr && (strcmp(attr->value, "true") == 0));
}

Boolean WsmReader::getElementStringValue(
    int nsType,
    const char* tagName,
    String& stringValue,
    Boolean required)
{
    XmlEntry entry;

    if (required)
    {
        expectStartTag(entry, nsType, tagName);
    }
    else if (!testStartTag(entry, nsType, tagName))
    {
        return false;
    }

    expectContentOrCData(entry);
    stringValue = entry.text;
    expectEndTag(nsType, tagName);
    return true;
}

const char* WsmReader::getElementContent(XmlEntry& entry)
{
    if (entry.type == XmlEntry::EMPTY_TAG)
    {
        return "";
    }

    expectContentOrCData(entry);
    return entry.text;
}

Boolean WsmReader::getSelectorElement(WsmSelector& selector)
{
    XmlEntry entry;
    if (!testStartOrEmptyTag(entry, WsmNamespaces::WS_MAN, "Selector"))
    {
        return false;
    }

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;
    String name;

    getAttributeValue(_parser.getLine(), entry, "Name", name);

    if (empty)
    {
        selector = WsmSelector(name, String::EMPTY);
    }
    else
    {
        WsmEndpointReference endpointReference;

        if (getSelectorEPRElement(endpointReference))
        {
            selector = WsmSelector(name, endpointReference);
        }
        else
        {
            expectContentOrCData(entry);
            selector = WsmSelector(name, entry.text);
        }

        expectEndTag(WsmNamespaces::WS_MAN, "Selector");
    }

    return true;
}

Boolean WsmReader::getSelectorSetElement(WsmSelectorSet& selectorSet)
{
    XmlEntry entry;
    if (!testStartTag(entry, WsmNamespaces::WS_MAN, "SelectorSet"))
    {
        return false;
    }

    selectorSet.selectors.clear();

    WsmSelector selector;

    while (getSelectorElement(selector))
    {
        selectorSet.selectors.append(selector);
    }

    // If the selector set is empty, report an error
    if (selectorSet.selectors.size() == 0)
    {
        expectStartTag(entry, WsmNamespaces::WS_MAN, "Selector");
    }

    // Note: This "should" requirement is not implemented.
    // DSP0226 R5.1.2.2-4: The Selector Name attribute shall not be duplicated
    // at the same level of nesting.  If this occurs, the service should return
    // a wsman:InvalidSelectors fault with the following detail code:
    //     http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/
    //         DuplicateSelectors

    expectEndTag(WsmNamespaces::WS_MAN, "SelectorSet");

    return true;
}

void WsmReader::getEPRElement(WsmEndpointReference& endpointReference)
{
    XmlEntry entry;

    // DSP0227 Section 5. WS-Management default addressing model is based
    // on WS-Addressing. It makes use of wsa:ReferenceParameter field
    // (containing wsa:ResourceURI and wsa:SelectorSet) to identify objects.
    // The use of other fields defined by WS-Addressing in not specified.

    // wsa:Address is required by WS-Addressing
    getElementStringValue(
        WsmNamespaces::WS_ADDRESSING,
        "Address",
        endpointReference.address,
        true);

    expectStartTag(entry, WsmNamespaces::WS_ADDRESSING, "ReferenceParameters");

    // Though DSP0227 does not explicitly require wsa:ResourceURI to be present,
    // valid targets of operations must either have a class specific
    // ResourceURI to target classes/instances or all classes URI to target
    // the service itself.
    getElementStringValue(
        WsmNamespaces::WS_MAN,
        "ResourceURI",
        endpointReference.resourceUri,
        true);

    // wsa:SelectorSet is optional
    if (testStartTag(entry, WsmNamespaces::WS_MAN, "SelectorSet"))
    {
        _parser.putBack(entry);
        // Return value ignored; assumed to succeed because of precheck
        getSelectorSetElement(*endpointReference.selectorSet);
    }

    expectEndTag(WsmNamespaces::WS_ADDRESSING, "ReferenceParameters");
}

Boolean WsmReader::getSelectorEPRElement(
    WsmEndpointReference& endpointReference)
{
    // EPRs in selectors have enclosing wsa:EndpointReference tags
    XmlEntry entry;
    if (!testStartTag(
            entry, WsmNamespaces::WS_ADDRESSING, "EndpointReference"))
    {
        return false;
    }

    getEPRElement(endpointReference);
    expectEndTag(WsmNamespaces::WS_ADDRESSING, "EndpointReference");
    return true;
}


Boolean WsmReader::getInstanceEPRElement(
    WsmEndpointReference& endpointReference)
{
    XmlEntry entry;

    if (!testStartTag(entry, WsmNamespaces::WS_ADDRESSING, "Address"))
    {
        return false;
    }
    else
    {
        _parser.putBack(entry);
    }

    getEPRElement(endpointReference);
    return true;
}

void WsmReader::skipElement(XmlEntry& entry)
{
    const char* elementName = entry.text;

    if (entry.type == XmlEntry::EMPTY_TAG)
    {
        return;
    }

    while (XmlReader::testStartTagOrEmptyTag(_parser, entry))
    {
        skipElement(entry);
    }

    // Skip content data, if present
    XmlReader::testContentOrCData(_parser, entry);

    XmlReader::expectEndTag(_parser, elementName);
}

inline void checkDuplicateHeader(
    const char* elementName,
    Boolean isDuplicate)
{
    // DSP0226 R13.1-9: If a request contains multiple SOAP headers with the
    // same QName from WS-Management, WS-Addressing, or WS-Eventing, the
    // service should not process them and should issue a
    // wsa:InvalidMessageInformationHeaders fault if they are detected.
    // (No SOAP headers are defined by the WS-Transfer and WS-Enumeration
    // specifications.)

    if (isDuplicate)
    {
        throw WsmFault(
            WsmFault::wsa_InvalidMessageInformationHeader,
            MessageLoaderParms(
                "WsmServer.WsmReader.DUPLICATE_SOAP_HEADER",
                "Request contains multiple $0 SOAP Header elements.",
                elementName));
    }
}

void WsmReader::decodeRequestSoapHeaders(
    String& wsaMessageId,
    String& wsaTo,
    String& wsaAction,
    String& wsaFrom,
    String& wsaReplyTo,
    String& wsaFaultTo,
    String& wsmResourceUri,
    WsmSelectorSet& wsmSelectorSet,
    Uint32& wsmMaxEnvelopeSize,
    AcceptLanguageList& wsmLocale,
    Boolean& wsmRequestEpr)
{
    // Note: This method does not collect headers that should appear only in
    // responses: wsa:RelatesTo, wsman:RequestedEPR.

    XmlEntry entry;
    Boolean gotEntry;

    expectStartTag(entry, WsmNamespaces::SOAP_ENVELOPE, "Header");

    while ((gotEntry = _parser.next(entry)) &&
           ((entry.type == XmlEntry::START_TAG) ||
            (entry.type == XmlEntry::EMPTY_TAG)))
    {
        int nsType = entry.nsType;
        const char* elementName = entry.localName;
        Boolean needEndTag = (entry.type == XmlEntry::START_TAG);

        if ((nsType == WsmNamespaces::WS_ADDRESSING) &&
            (strcmp(elementName, "To") == 0))
        {
            checkDuplicateHeader(entry.text, wsaTo.size());
            wsaTo = getElementContent(entry);
        }
        else if ((nsType == WsmNamespaces::WS_ADDRESSING) &&
            (strcmp(elementName, "From") == 0))
        {
            checkDuplicateHeader(entry.text, wsaFrom.size());
            wsaFrom = getElementContent(entry);
        }
        else if ((nsType == WsmNamespaces::WS_ADDRESSING) &&
            (strcmp(elementName, "ReplyTo") == 0))
        {
            checkDuplicateHeader(entry.text, wsaReplyTo.size());
            // ATTN: Reference parameters not handled yet.
            // DSP0226 R5.4.2-5: Any reference parameters supplied in the
            // wsa:ReplyTo address shall be included in the actual response
            // message as top-level headers as specified in WS-Addressing
            // unless the response is a fault.  If the response is a fault,
            // the service should include the reference parameters but may
            // omit these values if the resulting message size would exceed
            // the encoding limits.
            getElementStringValue(
                WsmNamespaces::WS_ADDRESSING, "Address", wsaReplyTo, true);
        }
        else if ((nsType == WsmNamespaces::WS_ADDRESSING) &&
            (strcmp(elementName, "FaultTo") == 0))
        {
            checkDuplicateHeader(entry.text, wsaFaultTo.size());
            // ATTN: Reference parameters not handled yet.
            // DSP0226 R5.4.3-4: Any reference parameters supplied in the
            // wsa:FaultTo address should be included as top-level headers in
            // the actual fault, as specified in the WS-Addressing
            // specification.  In some cases, including this information would
            // cause the fault to exceed encoding size limits, and thus may be
            // omitted in those cases.
            getElementStringValue(
                WsmNamespaces::WS_ADDRESSING, "Address", wsaFaultTo, true);
        }
        else if ((nsType == WsmNamespaces::WS_ADDRESSING) &&
            (strcmp(elementName, "Action") == 0))
        {
            checkDuplicateHeader(entry.text, wsaAction.size());
            wsaAction = getElementContent(entry);
        }
        else if ((nsType == WsmNamespaces::WS_ADDRESSING) &&
            (strcmp(elementName, "MessageID") == 0))
        {
            checkDuplicateHeader(entry.text, wsaMessageId.size());
            wsaMessageId = getElementContent(entry);
        }
        else if ((nsType == WsmNamespaces::WS_MAN) &&
            (strcmp(elementName, "ResourceURI") == 0))
        {
            checkDuplicateHeader(entry.text, wsmResourceUri.size());
            wsmResourceUri = getElementContent(entry);
        }
        else if ((nsType == WsmNamespaces::WS_MAN) &&
            (strcmp(elementName, "SelectorSet") == 0))
        {
            checkDuplicateHeader(entry.text, wsmSelectorSet.selectors.size());
            _parser.putBack(entry);
            getSelectorSetElement(wsmSelectorSet);
            // The end tag has already been consumed.
            needEndTag = false;
        }
        else if ((nsType == WsmNamespaces::WS_MAN) &&
            (strcmp(elementName, "OperationTimeout") == 0))
        {
            if (mustUnderstand(entry))
            {
                // DSP0226 R6.1-3: If the service does not support user-defined
                // timeouts, a wsman:UnsupportedFeature fault should be
                // returned with the following detail code:
                //     http://schemas.dmtf.org/wbem/wsman/faultDetail/
                //         OperationTimeout

                throw WsmFault(
                    WsmFault::wsman_UnsupportedFeature,
                    String::EMPTY,
                    ContentLanguageList(),
                    WSMAN_FAULTDETAIL_OPERATIONTIMEOUT);
            }

            // Note: It is not an error for a compliant service to ignore the
            // timeout value or treat it as a hint if mustUnderstand is omitted.

            skipElement(entry);
            // The end tag, if any, has already been consumed.
            needEndTag = false;
        }
        else if ((nsType == WsmNamespaces::WS_MAN) &&
            (strcmp(elementName, "MaxEnvelopeSize") == 0))
        {
            checkDuplicateHeader(entry.text, wsmMaxEnvelopeSize > 0);

            // DSP0226 R6.2-3: If the mustUnderstand attribute is set to
            // "false", the service may ignore the header.
            const char* content = getElementContent(entry);
            if (*content == '+')
            {
                content++;
            }
            Uint64 value;
            if (!StringConversion::decimalStringToUint64(content, value) ||
                (value == 0) || (value > 0xFFFFFFFF))
            {
                throw WsmFault(
                    WsmFault::wsa_InvalidMessageInformationHeader,
                    MessageLoaderParms(
                        "WsmServer.WsmReader.INVALID_MAXENVELOPESIZE_VALUE",
                        "The MaxEnvelopeSize value \"$0\" is not a valid "
                            "positive integer.",
                        content));
            }

            // DSP0226 R6.2-4:  Services should reject any MaxEnvelopeSize
            // value less than 8192 octets.  This number is the safe minimum
            // in which faults can be reliably encoded for all character sets.
            // If the requested size is less than this, the service should
            // return a wsman:EncodingLimit fault with the following detail
            // code:
            //     http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/
            //         MinimumEnvelopeLimit

            if (value < WSM_MIN_MAXENVELOPESIZE_VALUE)
            {
                throw WsmFault(
                    WsmFault::wsman_EncodingLimit,
                    MessageLoaderParms(
                        "WsmServer.WsmReader.MAXENVELOPESIZE_TOO_SMALL",
                        "The MaxEnvelopeSize $0 is less than "
                            "minimum allowable value of $1.",
                        value, WSM_MIN_MAXENVELOPESIZE_VALUE),
                    WSMAN_FAULTDETAIL_MINIMUMENVELOPELIMIT);
            }
            
            wsmMaxEnvelopeSize = value & 0xFFFFFFFF;
        }
        else if ((nsType == WsmNamespaces::WS_MAN) &&
            (strcmp(elementName, "Locale") == 0))
        {
            checkDuplicateHeader(entry.text, wsmLocale.size());

            if (mustUnderstand(entry))
            {
                // DSP0226 R6.3-2: If the mustUnderstand attribute is set to
                // "true", the service shall ensure that the replies contain
                // localized information where appropriate, or else the service
                // shall issue a wsman:UnsupportedFeature fault with the
                // following detail code:
                //     http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/
                //         Locale
                // A service may always fault if wsman:Locale contains
                // s:mustUnderstand set to "true", because it may not be able
                // to ensure that the reply is localized.

                throw WsmFault(
                    WsmFault::wsman_UnsupportedFeature,
                    MessageLoaderParms(
                        "WsmServer.WsmReader.LOCALE_NOT_ENSURED",
                        "Translation of responses to a specified locale "
                            "cannot be ensured."),
                    WSMAN_FAULTDETAIL_LOCALE);
            }

            // DSP0226 R6.3-1 If the mustUnderstand attribute is omitted or set
            // to "false", the service should use this value when composing the
            // response message and adjust any localizable values accordingly.
            // This use is recommended for most cases. The locale is treated as
            // a hint in this case.

            // DSP0226 R6.3-3: The value of the xml:lang attribute in the
            // wsman:Locale header shall be a valid RFC 3066 language code.

            String languageTag;
            if (getAttributeValue(
                    _parser.getLine(), entry, "xml:lang", languageTag, false))
            {
                wsmLocale.insert(LanguageTag(languageTag), Real32(1.0));
            }
        }
        else if ((nsType == WsmNamespaces::WS_MAN) &&
            (strcmp(elementName, "OptionSet") == 0))
        {
            // DSP0226 R6.4-3: If the mustUnderstand attribute is omitted from
            // the OptionSet block, the service may ignore the entire
            // wsman:OptionSet block. If it is present and the service does not
            // support wsman:OptionSet, the service shall return a
            // s:NotUnderstood fault.

            if (mustUnderstand(entry))
            {
                throw SoapNotUnderstoodFault(
                    _parser.getNamespace(nsType)->extendedName, elementName);
            }

            skipElement(entry);
            // The end tag, if any, has already been consumed.
            needEndTag = false;
        }
        else if ((nsType == WsmNamespaces::WS_MAN) &&
            (strcmp(elementName, "RequestEPR") == 0))
        {
            checkDuplicateHeader(entry.text, wsmRequestEpr);
            wsmRequestEpr = true;
        }
        else if ((nsType == WsmNamespaces::WS_MAN) &&
            (strcmp(elementName, "FragmentTransfer") == 0))
        {
            // DSP0226 R7.7-1: A conformant service may support fragment-level
            // WS-Transfer.  If the service supports fragment-level access, the
            // service shall not behave as if normal WS-Transfer operations
            // were in place but shall operate exclusively on the fragments
            // specified.  If the service does not support fragment-level
            // access, it shall return a wsman:UnsupportedFeature fault with
            // the following detail code:
            //     http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/
            //         FragmentLevelAccess

            if (mustUnderstand(entry))
            {
                throw WsmFault(
                    WsmFault::wsman_UnsupportedFeature,
                    String::EMPTY,
                    ContentLanguageList(),
                    WSMAN_FAULTDETAIL_FRAGMENTLEVELACCESS);
            }

            skipElement(entry);
            // The end tag, if any, has already been consumed.
            needEndTag = false;
        }
        else if (mustUnderstand(entry))
        {
            // DSP0226 R5.2-2: If a service cannot comply with a header
            // marked with mustUnderstand="true", it shall issue an
            // s:NotUnderstood fault.
            throw SoapNotUnderstoodFault(
                _parser.getNamespace(nsType)->extendedName, elementName);
        }
        else
        {
            skipElement(entry);
            // The end tag, if any, has already been consumed.
            needEndTag = false;
        }

        if (needEndTag)
        {
            expectEndTag(nsType, elementName);
        }
    }

    if (gotEntry)
    {
        _parser.putBack(entry);
    }

    expectEndTag(WsmNamespaces::SOAP_ENVELOPE, "Header");
}

void WsmReader::getInstanceElement(WsmInstance& instance)
{
    XmlEntry entry;
    Boolean isInstanceElement = false;
    int nsClassType = 0;
    const char* classNameTag = 0;

    // The first tag should be the class name start tag.
    if (_parser.next(entry) && (entry.type == XmlEntry::START_TAG))
    {
        XmlNamespace* ns = _parser.getNamespace(entry.nsType);
        classNameTag = entry.localName;

        // Class namespace must be of the following form:
        // http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2/<class name>
        if (ns)
        {
            const char* pos = strrchr(ns->extendedName, '/');
            if ((pos == ns->extendedName +
                sizeof(WSM_RESOURCEURI_CIMSCHEMAV2) - 1) &&
                (strncmp(ns->extendedName, WSM_RESOURCEURI_CIMSCHEMAV2,
                    sizeof(WSM_RESOURCEURI_CIMSCHEMAV2) - 1) == 0) &&
                (strcmp(pos + 1, classNameTag) == 0))
            {
                // All properties must be qualified with the class namespace
                nsClassType = entry.nsType;
                instance.setClassName(String(pos + 1));
                isInstanceElement = true;
            }
        }
    }

    if (!isInstanceElement)
    {
        MessageLoaderParms mlParms(
            "WsmServer.WsmReader.EXPECTED_INSTANCE_ELEMENT",
            "Expecting an instance element.");
        throw XmlValidationError(_parser.getLine(), mlParms);
    }

    String propName;
    WsmValue propValue;
    while (getPropertyElement(nsClassType, propName, propValue))
    {
        instance.addProperty(WsmProperty(propName, propValue));
    }

    expectEndTag(nsClassType, classNameTag);
}


Boolean WsmReader::getPropertyElement(
    int nsType,
    String& propName,
    WsmValue& propValue)
{
    XmlEntry entry;
    const char* propNameTag = 0;
    Boolean firstTime = true;

    // Initialize propValue to a null value string
    propValue = WsmValue();

    // The first time we enter this loop, propNameTag is NULL. It will
    // give us any tag name in the given namespace. Subsequent iterations
    // will get entries with the same name as retrieved in the first
    // iteration.
    while (testStartOrEmptyTag(entry, nsType, propNameTag))
    {
        // Look for xsi:nil="true" to determine of this entry represents
        // a null value.
        const XmlAttribute* attr =
            entry.findAttribute(WsmNamespaces::XML_SCHEMA_INSTANCE, "nil");
        Boolean isNilValue = (attr && strcmp(attr->value, "true") == 0);

        // The only time it's OK to see a null entry is on the first iteration.
        // The above indicates a null property value. If a null entry appears
        // in a sequence representing an array, it's an error.
        if ((isNilValue || propValue.isNull()) && !firstTime)
        {
            MessageLoaderParms mlParms(
                "WsmServer.WsmReader.NULL_ARRAY_ELEMENTS",
                "Setting array elements to null is not supported.");
            throw XmlValidationError(_parser.getLine(), mlParms);
        }

        // If we haven't found xsi:nil="true" attribute and this is
        // an empty tag, throw an exception: invalid property value.
        if (entry.type == XmlEntry::EMPTY_TAG && !isNilValue)
        {
            MessageLoaderParms mlParms(
                "WsmServer.WsmReader.MISSING_PROPERTY_VALUE",
                "No value specified for non-null property $0.",
                entry.text);
            throw XmlValidationError(_parser.getLine(), mlParms);
        }

        if (firstTime)
        {
            firstTime = false;

            // Set the property name
            propNameTag = entry.localName;
            propName = propNameTag;

            if (isNilValue)
            {
                // A null element should be either an empty tag or a start
                // tag followed by an end tag (no content).
                if (entry.type == XmlEntry::START_TAG)
                {
                    expectEndTag(nsType, propNameTag);
                }

                // No need to set propValue - it's initialized to a null
                // string value
            }
            else
            {
                PEGASUS_ASSERT(entry.type == XmlEntry::START_TAG);
                getValueElement(propValue, nsType, propNameTag);
            }
        }
        else
        {
            PEGASUS_ASSERT(entry.type == XmlEntry::START_TAG);
            WsmValue val;
            getValueElement(val, nsType, propNameTag);
            propValue.add(val);
        }
    }

    // If we never entered the main loop, it means that we haven't seen the
    // right initial tags to indicate a class property. In this case firstTime
    // is still set to true.
    return !firstTime;
}

void WsmReader::getValueElement(
    WsmValue& value, int nsType, const char* propNameTag)
{
    XmlEntry entry;

    if (testEndTag(nsType, propNameTag))
    {
        // It's an empty tag, enter and empty sting value
        value.set(String::EMPTY);
    }
    else
    {
        if (XmlReader::testContentOrCData(_parser, entry))
        {
            // Simple string content
            value.set(String(entry.text));
        }
        else
        {
            // The value is either an EPR or an embedded instance.
            WsmEndpointReference epr;
            if (getInstanceEPRElement(epr))
            {
                value.set(epr);
            }
            else
            {
                // This must be an embedded instance
                WsmInstance instance;
                getInstanceElement(instance);
                value.set(instance);
            }
        }
        expectEndTag(nsType, propNameTag);
    }
}

PEGASUS_NAMESPACE_END
