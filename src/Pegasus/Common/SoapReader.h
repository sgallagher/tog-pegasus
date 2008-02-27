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

#ifndef Pegasus_SoapReader_h
#define Pegasus_SoapReader_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/SoapUtils.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMScope.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class CIMQualifier;
class CIMQualifierDecl;
class CIMClass;
class CIMInstance;
class CIMProperty;
class CIMParameter;
class CIMMethod;
class CIMObject;
class CIMParamValue;
class CIMValue;
class XmlEntry;


class PEGASUS_COMMON_LINKAGE SoapReader
{
public:

    struct SoapEntry
    {
        SoapNamespaces::Type nsType;
        XmlEntry entry;
    };

    SoapReader(char* text);
    ~SoapReader();

    void getXmlDeclaration(
        const char*& xmlVersion, const char*& xmlEncoding);

    Boolean testSoapStartTag(
        SoapEntry* soapEntry, 
        SoapNamespaces::Type nsType, 
        const char* tagName);
    Boolean testSoapEndTag(
        SoapEntry* soapEntry, 
        SoapNamespaces::Type nsType, 
        const char* tagName);

    void processSoapEnvelope(String& soapAction);
    void decodeSoapAction(
        String& soapAction, String& action, SoapNamespaces::Type& nsType);

    void decodeClassName(SoapEntry* soapEntry, String& className);
    void decodeKeyBindings(SoapEntry* soapEntry, Array<CIMKeyBinding>&, 
        String& nameSpace);
    void decodeMessageId(SoapEntry* soapEntry, String& messageId);
    void decodeKeyBindingElement(SoapEntry* soapEntry, CIMName& name,
        String& value, CIMKeyBinding::Type& type);

    CIMName getNameAttribute(SoapEntry* soapEntry, const char* elementName,
        Boolean acceptNull = false);

    void initSoapHeaderItr() 
    { 
        _currentSoapHeader = 0; 
    }
    SoapEntry* nextSoapHeaderEntry()
    {
        if (_currentSoapHeader < _soapHeader.size())
            return &_soapHeader[_currentSoapHeader++];
        else
            return 0;
    }

    void initSoapBodyItr() 
    {
        _currentSoapBody = 0; 
    }
    SoapEntry* nextSoapBodyEntry() 
    { 
        if (_currentSoapBody < _soapBody.size())
            return &_soapBody[_currentSoapBody++];
        else
            return 0;
    }

private:

    Boolean _next(
        XmlEntry& entry, 
        SoapNamespaces::Type& nsType, 
        Boolean includeComment = false);

    void _expectContentOrCData(XmlEntry& entry);
    void _expectStartOrEmptyTag(
        XmlEntry& entry, SoapNamespaces::Type nsType, const char* tagName);
    void _expectStartTag(
        XmlEntry& entry, SoapNamespaces::Type nsType, const char* tagName);
    void _expectEndTag(
        XmlEntry& entry, SoapNamespaces::Type nsType, const char* tagName);
    Boolean _testEndTag(SoapNamespaces::Type nsType, const char* tagName);

    SoapNamespaces::Type _getNamespaceType(const char* name);
    SoapNamespaces::Namespace* _getNamespace(SoapNamespaces::Type nsType);

    void _processEnvelopeStartTag();
    void _processEnvelopeEndTag();

    void _processHeaderStartTag();
    void _processHeaderEndTag();

    Boolean _processBodyStartTag();
    void _processBodyEndTag();

    XmlParser _parser;
    Uint32 _scopeLevel;

    Array<SoapEntry> _soapHeader;
    Uint32 _currentSoapHeader;

    Array<SoapEntry> _soapBody;
    Uint32 _currentSoapBody;

    Stack<SoapNamespaces::Namespace> _nameSpaces;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SoapReader_h */
