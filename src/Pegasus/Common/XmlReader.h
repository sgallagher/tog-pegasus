//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// XmlReader.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_XmlReader_h
#define Pegasus_XmlReader_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMScope.h>
#include <Pegasus/Common/CIMReference.h>

PEGASUS_NAMESPACE_BEGIN

class CIMQualifier;
class CIMQualifierDecl;
class CIMClass;
class CIMInstance;
class CIMProperty;
class CIMParameter;
class CIMMethod;

class PEGASUS_COMMON_LINKAGE XmlReader
{
public:

    static void expectXmlDeclaration(
	XmlParser& parser, 
	XmlEntry& entry);

    static void expectStartTag(
	XmlParser& parser, 
	XmlEntry& entry,
	const char* tagName);

    static void expectEndTag(
	XmlParser& parser, 
	const char* tagName);

    static void expectStartTagOrEmptyTag(
	XmlParser& parser, 
	XmlEntry& entry,
	const char* tagName);

    static Boolean expectContentOrCData(
	XmlParser& parser, 
	XmlEntry& entry);

    static Boolean testStartTag(
	XmlParser& parser, 
	XmlEntry& entry,
	const char* tagName);

    static Boolean testEndTag(
	XmlParser& parser, 
	const char* tagName);

    static Boolean testStartTagOrEmptyTag(
	XmlParser& parser, 
	XmlEntry& entry,
	const char* tagName);

    static Boolean testContentOrCData(
	XmlParser& parser, 
	XmlEntry& entry);

    static void testCimStartTag(XmlParser& parser);

    static Boolean getIsArrayAttribute(
	Uint32 lineNumber,
	const XmlEntry& entry,
	const char* tagName,
	Boolean& value);

    static String getCimNameAttribute(
	Uint32 lineNumber, 
	const XmlEntry& entry,
	const char* elementName,
	Boolean acceptNull = false);

    static String getClassNameAttribute(
	Uint32 lineNumber, 
	const XmlEntry& entry,
	const char* elementName);

    static String getClassOriginAttribute(
	Uint32 lineNumber, 
	const XmlEntry& entry,
	const char* tagName);

    static String getReferenceClassAttribute(
	Uint32 lineNumber, 
	const XmlEntry& entry,
	const char* elementName);

    static String getSuperClassAttribute(
	Uint32 lineNumber, 
	const XmlEntry& entry,
	const char* tagName);

    static CIMType getCimTypeAttribute(
	Uint32 lineNumber, 
	const XmlEntry& entry, 
	const char* tagName);

    static Boolean getCimBooleanAttribute(
	Uint32 lineNumber,
	const XmlEntry& entry,
	const char* tagName,
	const char* attributeName,
	Boolean defaultValue,
	Boolean required);

    static Boolean stringToReal(
	const char* stringValue, 
	Real64& x);

    static Boolean stringToSignedInteger(
	const char* stringValue, 
	Sint64& x);

    static Boolean stringToUnsignedInteger(
	const char* stringValue, 
	Uint64& x);

    static CIMValue stringToValue(
	Uint32 lineNumber, 
	const char* valueString, 
	CIMType type);

    static Boolean getValueElement(
	XmlParser& parser, 
	CIMType type, 
	CIMValue& value);

    static CIMValue stringArrayToValue(
	Uint32 lineNumber, 
	const Array<const char*>& array, 
	CIMType type);

    static Boolean getValueArrayElement(
	XmlParser& parser, 
	CIMType type, 
	CIMValue& value);

    static Uint32 getFlavor(
	XmlEntry& entry, 
	Uint32 lineNumber, 
	const char* tagName);

    static Uint32 getOptionalScope(
	XmlParser& parser);

    static Boolean getQualifierElement(
	XmlParser& parser, 
	CIMQualifier& qualifier);

    static Boolean getPropertyElement(
	XmlParser& parser, 
	CIMProperty& property);

    static Boolean getArraySizeAttribute(
	Uint32 lineNumber,
	const XmlEntry& entry,
	const char* tagName,
	Uint32& value);

    static Boolean getPropertyArrayElement(
	XmlParser& parser, 
	CIMProperty& property);

    static Boolean getHostElement(
	XmlParser& parser,
	String& host);

    static Boolean getNameSpaceElement(
	XmlParser& parser,
	String& nameSpaceComponent);

    static Boolean getLocalNameSpacePathElement(
	XmlParser& parser,
	String& nameSpace);

    static Boolean getNameSpacePathElement(
	XmlParser& parser,
	String& host,
	String& nameSpace);

    static Boolean getClassNameElement(
	XmlParser& parser,
	String& className,
	Boolean required = false);

    static KeyBinding::Type getValueTypeAttribute(
	Uint32 lineNumber, 
	const XmlEntry& entry,
	const char* elementName);

    static Boolean getKeyValueElement(
	XmlParser& parser,
	KeyBinding::Type& type,
	String& value);

    static Boolean getKeyBindingElement(
	XmlParser& parser,
	String& name,
	String& value,
	KeyBinding::Type& type);

    static Boolean getInstanceNameElement(
	XmlParser& parser,
	String& className,
	Array<KeyBinding>& keyBindings);

    static Boolean getInstancePathElement(
	XmlParser& parser,
	CIMReference& reference);

    static Boolean getLocalInstancePathElement(
	XmlParser& parser,
	CIMReference& reference);

    static Boolean getClassPathElement(
	XmlParser& parser,
	CIMReference& reference);

    static Boolean getLocalClassPathElement(
	XmlParser& parser,
	CIMReference& reference);

    static Boolean getValueReferenceElement(
	XmlParser& parser,
	CIMReference& reference);

    static Boolean getPropertyReferenceElement(
	XmlParser& parser, 
	CIMProperty& property);

    static Boolean getParameterElement(
	XmlParser& parser, 
	CIMParameter& parameter);

    static Boolean getParameterArrayElement(
	XmlParser& parser, 
	CIMParameter& parameter);

    static Boolean getParameterReferenceElement(
	XmlParser& parser, 
	CIMParameter& parameter);

    static Boolean getQualifierDeclElement(
	XmlParser& parser, 
	CIMQualifierDecl& qualifierDecl);

    static Boolean getMethodElement(
	XmlParser& parser, 
	CIMMethod& method);

    static Boolean getClassElement(
	XmlParser& parser, 
	CIMClass& cimClass);

    static Boolean getInstanceElement(
	XmlParser& parser, 
	CIMInstance& cimInstance);

    static void getObject(XmlParser& parser, CIMClass& x);

    static void getObject(XmlParser& parser, CIMInstance& x);

    static void getObject(XmlParser& parser, CIMQualifierDecl& x);

    static Boolean getMessageStartTag(
	XmlParser& parser, 
	String& id,
	const char*& protocolVersion);

    static Boolean getIMethodCallStartTag(
	XmlParser& parser, 
	const char*& name);

    static Boolean getIMethodResponseStartTag(
	XmlParser& parser, 
	const char*& name);

    static Boolean getIParamValueTag(
	XmlParser& parser, 
	const char*& name);

    static Boolean getPropertyValue(
	XmlParser& parser, 
	CIMValue& cimValue);

    static Boolean getBooleanValueElement(
	XmlParser& parser, 
	Boolean& result,
	Boolean required = false);

    static Boolean getErrorElement(
	XmlParser& parser, 
	CIMException::Code& code,
	const char*& description,
	Boolean required = false);

private:

    XmlReader() 
    { 

    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_XmlReader_h */
