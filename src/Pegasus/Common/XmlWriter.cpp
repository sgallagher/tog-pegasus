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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cstdio>
#include "CIMClass.h"
#include "CIMInstance.h"
#include "CIMQualifierDecl.h"
#include "XmlWriter.h"
#include "XmlParser.h"

PEGASUS_NAMESPACE_BEGIN

inline void AppendChar(Array<Sint8>& out, Char16 c)
{
    out.append(Sint8(c));
}

inline void AppendSpecialChar(Array<Sint8>& out, Char16 c)
{
    // ATTN-B: Only UTF-8 handled for now.

    switch (c)
    {
	case '&':
	    out.append("&amp;", 5);
	    break;

	case '<':
	    out.append("&lt;", 4);
	    break;

	case '>':
	    out.append("&gt;", 4);
	    break;

	case '"':
	    out.append("&quot;", 6);
	    break;

	case '\'':
	    out.append("&apos;", 6);
	    break;

	default:
	    out.append(Sint8(c));
    }
}

void XmlWriter::append(Array<Sint8>& out, Char16 x)
{
    AppendChar(out, x);
}

void XmlWriter::append(Array<Sint8>& out, Uint32 x)
{
    char buffer[32];
    sprintf(buffer, "%d", x);
    append(out, buffer);
}

void XmlWriter::append(Array<Sint8>& out, const char* str)
{
    while (*str)
	AppendChar(out, *str++);
}

void XmlWriter::appendSpecial(Array<Sint8>& out, Char16 x)
{
    AppendSpecialChar(out, x);
}

void XmlWriter::appendSpecial(Array<Sint8>& out, char x)
{
    AppendSpecialChar(out, Char16(x));
}

void XmlWriter::appendSpecial(Array<Sint8>& out, const char* str)
{
    while (*str)
	AppendSpecialChar(out, *str++);
}

void XmlWriter::appendSpecial(Array<Sint8>& out, const String& str)
{
    const Char16* tmp = str.getData();

    while (*tmp)
	AppendSpecialChar(out, *tmp++);
}

void XmlWriter::append(Array<Sint8>& out, const String& str)
{
    const Char16* tmp = str.getData();

    while (*tmp)
	AppendChar(out, *tmp++);
}

void XmlWriter::append(Array<Sint8>& out, const Indentor& x)
{
    for (Uint32 i = 0; i < 4 * x.getLevel(); i++)
	out.append(' ');
}

void XmlWriter::appendLocalNameSpaceElement(
    Array<Sint8>& out, 
    const String& nameSpace)
{
    out << "<LOCALNAMESPACEPATH>\n";

    char* tmp = nameSpace.allocateCString();

    for (char* p = strtok(tmp, "/"); p; p = strtok(NULL, "/"))
    {
	out << "<NAMESPACE NAME=\"" << p << "\"/>\n";
    }

    delete [] tmp;

    out << "</LOCALNAMESPACEPATH>\n";
}

static inline void AppendSpecialChar(PEGASUS_STD(ostream)& os, char c)
{
    switch (c)
    {
	case '&':
	    os << "&amp;";
	    break;

	case '<':
	    os << "&lt;";
	    break;

	case '>':
	    os << "&gt;";
	    break;

	case '"':
	    os << "&quot;";
	    break;

	case '\'':
	    os << "&apos;";
	    break;

	default:
	    os << c;
    }
}

static inline void AppendSpecial(PEGASUS_STD(ostream)& os, const char* str)
{
    while (*str)
	AppendSpecialChar(os, *str++);
}

//------------------------------------------------------------------------------
//
// formatGetHeader()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatGetHeader(
    const char* documentPath)
{
    Array<Sint8> out;
    return out << "GET " << documentPath << "HTTP/1.0\r\n\r\n";
}

//------------------------------------------------------------------------------
//
// formatMPostHeader()
//
//     Build HTTP request header.
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatMPostHeader(
    const char* host,
    const char* cimOperation,
    const char* cimMethod,
    const String& cimObject,
    const Array<Sint8>& content)
{
    Array<Sint8> out;
    out.reserve(1024);
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "M-POST /cimom HTTP/1.1\r\n";
    out << "HOST: " << host << "\r\n";
    out << "Content-CIMType: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << content.size() << "\r\n";
    out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMOperation: " << cimOperation << "\r\n";
    out << nn << "-CIMMethod: " << cimMethod << "\r\n";
    out << nn << "-CIMObject: " << cimObject << "\r\n\r\n";
    out << content;
    return out;
}

//------------------------------------------------------------------------------
//
// formatMethodResponseHeader()
//
//     Build HTTP response header.
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatMethodResponseHeader(
    const Array<Sint8>& content)
{
    Array<Sint8> out;
    out.reserve(1024);
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "HTTP/1.1 200 OK\r\n";
    out << "Content-CIMType: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << content.size() << "\r\n";
    out << "Ext:\r\n";
    out << "Cache-Control: no-cache\r\n";
    out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMOperation: MethodResponse\r\n\r\n";
    out << content;
    return out;
}

//------------------------------------------------------------------------------
//
// formatMessageElement()
//
//     <!ELEMENT MESSAGE (SIMPLEREQ|MULTIREQ|SIMPLERSP|MULTIRSP)>
//     <!ATTLIST MESSAGE
//         ID CDATA #REQUIRED
//         PROTOCOLVERSION CDATA #REQUIRED>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatMessageElement(
    const String& messageId,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    out.reserve(1024);

    out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
    out << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">\n";
    out << "<MESSAGE ID=\"" << messageId << "\" PROTOCOLVERSION=\"1.0\">\n";
    out << body;
    out << "</MESSAGE>\n";
    out << "</CIM>\n";
    return out;
}

//------------------------------------------------------------------------------
//
// formatSimpleReqElement()
//
//     <!ELEMENT SIMPLEREQ (IMETHODCALL|METHODCALL)>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleReqElement(
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    return out << "<SIMPLEREQ>\n" << body << "</SIMPLEREQ>\n";
}

//------------------------------------------------------------------------------
//
// formatSimpleRspElement()
//
//     <!ELEMENT SIMPLERSP (METHODRESPONSE|IMETHODRESPONSE)>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleRspElement(
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    return out << "<SIMPLERSP>\n" << body << "</SIMPLERSP>\n";
}

//------------------------------------------------------------------------------
//
// formatIMethodCallElement()
//
//     <!ELEMENT IMETHODCALL (LOCALNAMESPACEPATH,IPARAMVALUE*)>
//     <!ATTLIST IMETHODCALL %CIMName;>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatIMethodCallElement(
    const char* name,
    const String& nameSpace,
    const Array<Sint8>& iParamValues)
{
    Array<Sint8> out;
    out << "<IMETHODCALL NAME=\"" << name << "\">\n";
    XmlWriter::appendLocalNameSpaceElement(out, nameSpace);
    out << iParamValues;
    out << "</IMETHODCALL>\n";
    return out;
}

//------------------------------------------------------------------------------
//
// formatIMethodResponseElement()
//
//     <!ELEMENT IMETHODRESPONSE (ERROR|IRETURNVALUE?)>
//     <!ATTLIST IMETHODRESPONSE %CIMName;>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatIMethodResponseElement(
    const char* name,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    out << "<IMETHODRESPONSE NAME=\"" << name << "\">\n";
    out << body;
    out << "</IMETHODRESPONSE>\n";
    return out;
}

//------------------------------------------------------------------------------
//
// formatIReturnValueElement()
//
//      <!ELEMENT IRETURNVALUE (CLASSNAME*|INSTANCENAME*|VALUE*|
//          VALUE.OBJECTWITHPATH*|VALUE.OBJECTWITHLOCALPATH*|VALUE.OBJECT*|
//          OBJECTPATH*|QUALIFIER.DECLARATION*|VALUE.ARRAY?|VALUE.REFERENCE?|
//          CLASS*|INSTANCE*|VALUE.NAMEDINSTANCE*)>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatIReturnValueElement(
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    return out << "<IRETURNVALUE>\n" << body << "</IRETURNVALUE>\n";
}

//------------------------------------------------------------------------------
//
// formatIParamValueElement()
//
//     <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
//         |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
//         |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
//     <!ATTLIST IPARAMVALUE %CIMName;>
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::formatIParamValueElement(
    Array<Sint8>& out,
    const char* name,
    const Array<Sint8>& body)
{
    out << "<IPARAMVALUE NAME=\"" << name << "\">\n";
    out << body;
    out << "</IPARAMVALUE>\n";
    return out;
}

//------------------------------------------------------------------------------
//
// formatErrorElement()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatErrorElement(
    CIMStatusCode code,
    const char* description)
{
    Array<Sint8> out;
    out << "<ERROR";
    out << " CODE=\"" << Uint32(code) << "\"";
    out << " DESCRIPTION=\"";
    appendSpecial(out, description);
    out << "\"/>";
    return out;
}

//------------------------------------------------------------------------------
//
// appendBooleanParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendBooleanParameter(
    Array<Sint8>& out,
    const char* name,
    Boolean flag)
{
    Array<Sint8> tmp;
    tmp << "<VALUE>" << (flag ? "TRUE" : "FALSE") << "</VALUE>\n";
    return formatIParamValueElement(out, name, tmp);
}

//------------------------------------------------------------------------------
//
// appendStringIParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendStringIParameter(
    Array<Sint8>& out,
    const char* name,
    const String& str)
{
    Array<Sint8> tmp;
    tmp << "<VALUE>";
    appendSpecial(tmp, str);
    tmp << "</VALUE>\n";
    return formatIParamValueElement(out, name, tmp);
}

//------------------------------------------------------------------------------
//
// appendClassNameParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendClassNameParameter(
    Array<Sint8>& out,
    const char* name,
    const String& className)
{
    Array<Sint8> tmp;
    appendClassNameElement(tmp, className);
    return formatIParamValueElement(out, name, tmp);
}

//------------------------------------------------------------------------------
//
// appendQualifierNameParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendQualifierNameParameter(
    Array<Sint8>& out,
    const char* name,
    const String& qualifierName)
{
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    //
    // ATTN: notice that there is really no way to pass a qualifier name
    // as an IPARAMVALUE element according to the spec (look above). So we
    // just pass it as a class name. An answer must be obtained later.
    
    Array<Sint8> tmp;
    appendClassNameElement(tmp, qualifierName);
    return formatIParamValueElement(out, name, tmp);
}

//------------------------------------------------------------------------------
//
// appendClassParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendClassParameter(
    Array<Sint8>& out,
    const char* parameterName,
    const CIMConstClass& cimClass)
{
    Array<Sint8> tmp;
    cimClass.toXml(tmp);
    return formatIParamValueElement(out, parameterName, tmp);
}

//------------------------------------------------------------------------------
//
// appendInstanceNameParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendInstanceNameParameter(
    Array<Sint8>& out,
    const char* parameterName,
    const CIMReference& instanceName)
{
    Array<Sint8> tmp;
    instanceName.instanceNameToXml(tmp);
    return formatIParamValueElement(out, parameterName, tmp);
}

//------------------------------------------------------------------------------
//
// appendInstanceParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendInstanceParameter(
    Array<Sint8>& out,
    const char* parameterName,
    const CIMConstInstance& instance)
{
    Array<Sint8> tmp;
    instance.toXml(tmp);
    return formatIParamValueElement(out, parameterName, tmp);
}

//----------------------------------------------------------
//
//  appendPropertyNameParameter()
//   	
//     </IPARAMVALUE>
//     <IPARAMVALUE NAME="PropertyName"><VALUE>FreeSpace</VALUE></IPARAMVALUE>
//
//     USE: Create parameter for getProperty operation
//==========================================================
Array<Sint8>& XmlWriter::appendPropertyNameParameter(
    Array<Sint8>& out,
    const String& propertyName)
{
    Array<Sint8> tmp;
    tmp << "<VALUE>" << propertyName << "</VALUE>\n"; 
    return formatIParamValueElement(out,"PropertyName", tmp);}

//------------------------------------------------------------------------------
//
// appendPropertyValueParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendPropertyValueParameter(
    Array<Sint8>& out,
    const char* parameterName,
    const CIMValue& value)
{
    Array<Sint8> tmp;
    value.toXml(tmp);
    return formatIParamValueElement(out, parameterName, tmp);
}

//------------------------------------------------------------------------------
//
// appendPropertyListParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendPropertyListParameter(
    Array<Sint8>& out,
    const Array<String>& propertyList)
{
    Array<Sint8> tmp;

    tmp << "<VALUE.ARRAY>\n";

    for (Uint32 i = 0; i < propertyList.size(); i++)
    {
        tmp << "<VALUE>" << propertyList[i] << "</VALUE>\n"; 
    }

    tmp << "</VALUE.ARRAY>\n";
    return formatIParamValueElement(out, "PropertyList", tmp);
}

//------------------------------------------------------------------------------
//
// appendQualifierDeclarationParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendQualifierDeclarationParameter(
    Array<Sint8>& out,
    const char* parameterName,
    const CIMConstQualifierDecl& qualifierDecl)
{
    Array<Sint8> tmp;
    qualifierDecl.toXml(tmp);
    return formatIParamValueElement(out, parameterName, tmp);
}

//------------------------------------------------------------------------------
//
// appendClassNameElement()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendClassNameElement(
    Array<Sint8>& out,
    const String& className)
{
    return out << "<CLASSNAME NAME=\"" << className << "\"/>\n";
}

//------------------------------------------------------------------------------
//
// appendInstanceNameElement()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendInstanceNameElement(
    Array<Sint8>& out,
    const CIMReference& instanceName)
{
    instanceName.instanceNameToXml(out);
    return out;
}

//------------------------------------------------------------------------------
//
// _printAttributes()
//
//------------------------------------------------------------------------------

static void _printAttributes(
    PEGASUS_STD(ostream)& os,
    const XmlAttribute* attributes,
    Uint32 attributeCount)
{
    for (Uint32 i = 0; i < attributeCount; i++)
    {
	os << attributes[i].name << "=";

	os << '"';
	AppendSpecial(os, attributes[i].value);
	os << '"';

	if (i + 1 != attributeCount)
	    os << ' ';
    }
}

//------------------------------------------------------------------------------
//
// _indent()
//
//------------------------------------------------------------------------------

static void _indent(PEGASUS_STD(ostream)& os, Uint32 level, Uint32 indentChars)
{
    Uint32 n = level * indentChars;

    for (Uint32 i = 0; i < n; i++)
	os << ' ';
}

//------------------------------------------------------------------------------
//
// indentedPrint()
//
//------------------------------------------------------------------------------

void XmlWriter::indentedPrint(
    PEGASUS_STD(ostream)& os, 
    const char* text, 
    Uint32 indentChars)
{
    char* tmp = strcpy(new char[strlen(text) + 1], text);

    XmlParser parser(tmp);
    XmlEntry entry;
    Stack<const char*> stack;

    while (parser.next(entry))
    {
	switch (entry.type)
	{
	    case XmlEntry::XML_DECLARATION:
	    {
		_indent(os, stack.size(), indentChars);

		os << "<?" << entry.text << " ";
		_printAttributes(os, entry.attributes, entry.attributeCount);
		os << "?>";
		break;
	    }

	    case XmlEntry::START_TAG:
	    {
		_indent(os, stack.size(), indentChars);

		os << "<" << entry.text;

		if (entry.attributeCount)
		    os << ' ';

		_printAttributes(os, entry.attributes, entry.attributeCount);
		os << ">";
		stack.push(entry.text);
		break;
	    }

	    case XmlEntry::EMPTY_TAG:
	    {
		_indent(os, stack.size(), indentChars);

		os << "<" << entry.text << " ";
		_printAttributes(os, entry.attributes, entry.attributeCount);
		os << "/>";
		break;
	    }

	    case XmlEntry::END_TAG:
	    {
		if (!stack.isEmpty() && strcmp(stack.top(), entry.text) == 0)
		    stack.pop();

		_indent(os, stack.size(), indentChars);

		os << "</" << entry.text << ">";
		break;
	    }

	    case XmlEntry::COMMENT:
	    {

		_indent(os, stack.size(), indentChars);
		os << "<!--";
		AppendSpecial(os, entry.text);
		os << "-->";
		break;
	    }

	    case XmlEntry::CONTENT:
	    {
		_indent(os, stack.size(), indentChars);
		AppendSpecial(os, entry.text);
		break;
	    }

	    case XmlEntry::CDATA:
	    {
		_indent(os, stack.size(), indentChars);
		os << "<![CDATA[...]]>";
		break;
	    }

	    case XmlEntry::DOCTYPE:
	    {
		_indent(os, stack.size(), indentChars);
		os << "<!DOCTYPE...>";
		break;
	    }
	}

	os << PEGASUS_STD(endl);
    }

    delete [] tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::getNextMessageId()
//
//------------------------------------------------------------------------------

String XmlWriter::getNextMessageId()
{
    // ATTN: make thread-safe:
    static Uint32 messageId = 1000;

    messageId++;

    if (messageId < 1000)
	messageId = 1001;

    char buffer[16];
    sprintf(buffer, "%d", messageId);
    return buffer;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleIMethodReqMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleIMethodReqMessage(
    const char* host,
    const String& nameSpace,
    const char* iMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    return XmlWriter::formatMPostHeader(
	host,
	"MethodCall",
	iMethodName,
	nameSpace,
	XmlWriter::formatMessageElement(
	    messageId,
	    XmlWriter::formatSimpleReqElement(
		XmlWriter::formatIMethodCallElement(
		    iMethodName,
		    nameSpace, 
		    body))));
}

Array<Sint8> XmlWriter::formatSimpleRspMessage(
    const char* iMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    return XmlWriter::formatMethodResponseHeader(
        XmlWriter::formatMessageElement(
	    messageId,
            XmlWriter::formatSimpleRspElement(
                XmlWriter::formatIMethodResponseElement(
                    iMethodName,
                    XmlWriter::formatIReturnValueElement(body)))));
}

Array<Sint8>& operator<<(Array<Sint8>& out, const char* x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, char x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, Char16 x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, const String& x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, const Indentor& x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, const Array<Sint8>& x)
{
    out.appendArray(x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, Uint32 x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& XmlWriter::appendObjectNameParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMReference& objectName)
{
    if (objectName.isClassName())
    {
	XmlWriter::appendClassNameParameter(
	    out, name, objectName.getClassName());
    }
    else
    {
	XmlWriter::appendInstanceNameParameter(
	    out, name, objectName);
    }

    return out;
}

Array<Sint8> XmlWriter::formatEMethodCallElement(
    const char* name,
    const Array<Sint8>& iParamValues)
{
    Array<Sint8> out;
    out << "<EXPMETHODCALL NAME=\"" << name << "\">\n";
    out << iParamValues;
    out << "</EXPMETHODCALL>\n";
    return out;
}

Array<Sint8> XmlWriter::formatSimpleIndicationReqMessage(
    const char* host,
    const char* iMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    return XmlWriter::formatMPostIndicationHeader(
        host,
        "MethodRequest",
        iMethodName,
        XmlWriter::formatMessageElement(
            messageId,
            XmlWriter::formatSimpleExportReqElement(
                XmlWriter::formatEMethodCallElement(
                    iMethodName,
                    body))));
}

Array<Sint8> XmlWriter::formatMPostIndicationHeader(
    const char* host,
    const char* cimOperation,
    const char* cimMethod,
    const Array<Sint8>& content)
{
    Array<Sint8> out;
    out.reserve(1024);
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "M-POST /cimom HTTP/1.1\r\n";
    out << "HOST: " << host << "\r\n";
    out << "Content-CIMType: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << content.size() << "\r\n";
    out << "Man: http://www.hp.com; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMExport: " << cimOperation << "\r\n";
    out << nn << "-CIMExportMethod: " << cimMethod << "\r\n\r\n";
    out << content;
    return out;
}

Array<Sint8> XmlWriter::formatSimpleExportReqElement(
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    return out << "<SIMPLEEXPREQ>\n" << body << "</SIMPLEEXPREQ>\n";
}

Array<Sint8> XmlWriter::formatSimpleIndicationRspMessage(
    const char* iMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    return XmlWriter::formatEMethodResponseHeader(
        XmlWriter::formatMessageElement(
	    messageId,
            XmlWriter::formatSimpleExportRspElement(
                XmlWriter::formatEMethodResponseElement(
                    iMethodName,
                    XmlWriter::formatIReturnValueElement(body)))));
}

//------------------------------------------------------------------------------
//
// formatSimpleExportRspElement()
//
//     <!ELEMENT SIMPLEEXPRSP (METHODRESPONSE|EXPMETHODRESPONSE)>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleExportRspElement(
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    return out << "<SIMPLEEXPRSP>\n" << body << "</SIMPLEEXPRSP>\n";
}

//------------------------------------------------------------------------------
//
// formatIMethodResponseElement()
//
//     <!ELEMENT EXPMETHODRESPONSE (ERROR|IRETURNVALUE?)>
//     <!ATTLIST EXPMETHODRESPONSE %CIMName;>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatEMethodResponseElement(
    const char* name,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    out << "<EXPMETHODRESPONSE NAME=\"" << name << "\">\n";
    out << body;
    out << "</EXPMETHODRESPONSE>\n";
    return out;
}

//------------------------------------------------------------------------------
//
// formatMethodResponseHeader()
//
//     Build HTTP response header.
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatEMethodResponseHeader(
    const Array<Sint8>& content)
{
    Array<Sint8> out;
    out.reserve(1024);
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "HTTP/1.1 200 OK\r\n";
    out << "Content-CIMType: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << content.size() << "\r\n";
    out << "Ext:\r\n";
    out << "Cache-Control: no-cache\r\n";
    out << "Man:  http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMExport: MethodResponse\r\n\r\n";
    out << content;
    return out;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleMethodReqMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleMethodReqMessage(
    const char* host,
    const String& nameSpace,
    const char* iMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    return XmlWriter::formatMPostHeader(
	host,
	"MethodCall",
	iMethodName,
	nameSpace,
	XmlWriter::formatMessageElement(
	    messageId,
	    XmlWriter::formatSimpleReqElement(
		XmlWriter::formatMethodCallElement(
		    iMethodName,
		    nameSpace, 
		    body))));
}

//------------------------------------------------------------------------------
//
// formatMethodCallElement()
//
//     <!ELEMENT METHODCALL (LOCALNAMESPACEPATH,IPARAMVALUE*)>
//     <!ATTLIST METHODCALL %CIMName;>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatMethodCallElement(
    const char* name,
    const String& nameSpace,
    const Array<Sint8>& iParamValues)
{
    Array<Sint8> out;
    out << "<METHODCALL NAME=\"" << name << "\">\n";
    out << iParamValues;
    out << "</METHODCALL>\n";
    return out;
}

Array<Sint8> XmlWriter::formatSimpleMethodRspMessage(
    const char* iMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    /*return XmlWriter::formatMethodResponseHeader(
        XmlWriter::formatMessageElement(
	    messageId,
            XmlWriter::formatSimpleRspElement(
                XmlWriter::formatMethodResponseElement(
                    iMethodName,
                    XmlWriter::formatReturnValueElement(body)))));*/
    return XmlWriter::formatMethodResponseHeader(
        XmlWriter::formatMessageElement(
	    messageId,
            XmlWriter::formatSimpleRspElement(
                XmlWriter::formatMethodResponseElement(
                    iMethodName,
                    body))));
}

//------------------------------------------------------------------------------
//
// formatMethodResponseElement()
//
//     <!ELEMENT METHODRESPONSE (ERROR|IRETURNVALUE?)>
//     <!ATTLIST METHODRESPONSE %CIMName;>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatMethodResponseElement(
    const char* name,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    out << "<METHODRESPONSE NAME=\"" << name << "\">\n";
    out << body;
    out << "</METHODRESPONSE>\n";
    return out;
}

//------------------------------------------------------------------------------
//
// appendStringParameter()
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::appendStringParameter(
    Array<Sint8>& out,
    const char* name,
    const String& str)
{
    Array<Sint8> tmp;
    tmp << "<VALUE>";
    appendSpecial(tmp, str);
    tmp << "</VALUE>\n";
    return formatParamValueElement(out, name, tmp);
}

//------------------------------------------------------------------------------
//
// formatParamValueElement()
//
//     <!ELEMENT PARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
//         |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
//         |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
//     <!ATTLIST PARAMVALUE %CIMName;>
//
//------------------------------------------------------------------------------

Array<Sint8>& XmlWriter::formatParamValueElement(
    Array<Sint8>& out,
    const char* name,
    const Array<Sint8>& body)
{
    out << "<PARAMVALUE NAME=\"" << name << "\">\n";
    out << body;
    out << "</PARAMVALUE>\n";
    return out;
}

//------------------------------------------------------------------------------
//
// formatReturnValueElement()
//
//      <!ELEMENT RETURNVALUE (CLASSNAME*|INSTANCENAME*|VALUE*|
//          VALUE.OBJECTWITHPATH*|VALUE.OBJECTWITHLOCALPATH*|VALUE.OBJECT*|
//          OBJECTPATH*|QUALIFIER.DECLARATION*|VALUE.ARRAY?|VALUE.REFERENCE?|
//          CLASS*|INSTANCE*|VALUE.NAMEDINSTANCE*)>
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatReturnValueElement(
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    return out << "<RETURNVALUE>\n" << body << "</RETURNVALUE>\n";
}

PEGASUS_NAMESPACE_END
