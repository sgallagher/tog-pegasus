//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

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
class CIMObject;
class CIMNamedInstance;
class CIMObjectWithPath;
class CIMParamValue;

class PEGASUS_COMMON_LINKAGE XmlReader
{
public:

    static void getXmlDeclaration(
	XmlParser& parser, 
	const char*& xmlVersion,
	const char*& xmlEncoding);

    static Boolean testXmlDeclaration (
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

    static void getCimStartTag(
	XmlParser& parser, 
	const char*& cimVersion,
	const char*& dtdVersion);

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
	const char* tagName,
	const char* attributeName = "TYPE",
	Boolean required = true);

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

    /* getValueElement - Gets the CIMType and CIMValue for a single
        XML Value element.  It expects <VALUE> as the start tag.
        @return Returns True if the value element exists 
    */
    static Boolean getValueElement(
	XmlParser& parser, 
	CIMType type, 
	CIMValue& value);

    static Boolean getStringValueElement(
	XmlParser& parser, 
	String& str, 
	Boolean required);

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

    static Boolean getInstanceNameElement(
	XmlParser& parser,
	CIMReference& instanceName);

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

    static Boolean getValueReferenceArrayElement(
	XmlParser& parser, 
	CIMValue& value);

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

    static Boolean getParameterReferenceArrayElement(
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

    static Boolean getNamedInstanceElement(
	XmlParser& parser, 
	CIMNamedInstance& namedInstance);

    static void getObject(XmlParser& parser, CIMClass& x);

    static void getObject(XmlParser& parser, CIMInstance& x);

    static void getObject(XmlParser& parser, CIMQualifierDecl& x);

    static Boolean getMessageStartTag(
	XmlParser& parser, 
	String& id,
	String& protocolVersion);

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
	CIMException& cimException,
	Boolean required = false);

    static Boolean getValueObjectElement(
	XmlParser& parser, 
	CIMObject& object);

    static Boolean getValueObjectWithPathElement(
	XmlParser& parser, 
	CIMObjectWithPath& objectWithPath);

    static Boolean getValueObjectWithLocalPathElement(
	XmlParser& parser, 
	CIMObjectWithPath& objectWithPath);

    static void getObjectArray(
	XmlParser& parser, 
	Array<CIMObjectWithPath>& objectArray);

    static Boolean getObjectNameElement(
	XmlParser& parser, 
	CIMReference& objectName);

    static Boolean getObjectPathElement(
	XmlParser& parser, 
	CIMReference& objectPath);

    static Boolean getEMethodCallStartTag(
	XmlParser& parser, 
	const char*& name);

    static Boolean getEMethodResponseStartTag(
	XmlParser& parser, 
	const char*& name);

    static Boolean getMethodCallStartTag(
	XmlParser& parser, 
	const char*& name);

    static Boolean getMethodResponseStartTag(
	XmlParser& parser, 
	const char*& name);

    static Boolean getParamValueElement(
	XmlParser& parser, 
	CIMParamValue& paramValue);

    static Boolean getReturnValueElement(
	XmlParser& parser, 
	CIMValue& returnValue);

private:

    XmlReader() 
    { 

    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_XmlReader_h */
