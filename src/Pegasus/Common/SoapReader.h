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


class PEGASUS_COMMON_LINKAGE SoapHeader
{
};

class PEGASUS_COMMON_LINKAGE SoapReader
{
public:

    enum NSType
    {
        NST_UNKNOWN = -1,

        NST_SOAP_ENVELOPE = 0,
        NST_SOAP_ENCODING,
        NST_XML_SCHEMA_INSTANCE,
        NST_XML_SCHEMA,
        NST_WS_MAN,
        NST_WS_CIM_BINDING,
        NST_WS_ADDRESSING,
        NST_WS_TRANSFER,
        NST_WS_ENUMERATION,
        NST_WS_EVENTING,
        NST_WS_POLICY,
        NST_WSDL,
        
        NST_COUNT,
        NST_LAST = NST_COUNT
    };   

    struct SoapNamespace
    {
        const char* localName;
        const char* extendedName;
        NSType namespaceType;
        Uint32 scopeLevel;
    };

    SoapReader(char* text) : _parser(text), _scopeLevel(0) {}

    void getXmlDeclaration(
        const char*& xmlVersion,
        const char*& xmlEncoding);

    Boolean next(
        XmlEntry& entry, 
        NSType& nsType, 
        Boolean includeComment = false);

    void expectStartOrEmptyTag(XmlEntry& entry, NSType nsType, 
        const char* tagName);
    void expectStartTag(XmlEntry& entry, NSType nsType, const char* tagName);
    void expectEndTag(XmlEntry& entry, NSType nsType, const char* tagName);
    Boolean testEndTag(NSType nsType, const char* tagName);

    void processEnvelopeStartTag();
    void processEnvelopeEndTag();

    void processHeaderStartTag();
    void processHeaderEndTag();
    Boolean isHeaderEndTag();

    Boolean processBodyStartTag();
    void processBodyEndTag();
    Boolean isBodyEndTag();

private:

    Boolean _isSupportedNamespace(SoapNamespace* ns);
    NSType _getNamespaceType(const char* name);
    SoapNamespace* _getNamespace(NSType nsType);

    XmlParser _parser;
    Uint32 _scopeLevel;

    SoapHeader _soapHeader;

    Stack<SoapNamespace> _nameSpaces;
    static SoapNamespace _supportedNamespaces[];
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SoapReader_h */
