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
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
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

inline void _appendChar(Array<Sint8>& out, Char16 c)
{
    out.append(Sint8(c));
}

inline void _appendSpecialChar(Array<Sint8>& out, Char16 c)
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

static inline void _appendSpecialChar(PEGASUS_STD(ostream)& os, char c)
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

static inline void _appendSpecial(PEGASUS_STD(ostream)& os, const char* str)
{
    while (*str)
	_appendSpecialChar(os, *str++);
}

void XmlWriter::append(Array<Sint8>& out, Char16 x)
{
    _appendChar(out, x);
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
	_appendChar(out, *str++);
}

void XmlWriter::append(Array<Sint8>& out, const String& str)
{
    const Char16* tmp = str.getData();

    while (*tmp)
	_appendChar(out, *tmp++);
}

void XmlWriter::append(Array<Sint8>& out, const Indentor& x)
{
    for (Uint32 i = 0; i < 4 * x.getLevel(); i++)
	out.append(' ');
}

void XmlWriter::appendSpecial(Array<Sint8>& out, Char16 x)
{
    _appendSpecialChar(out, x);
}

void XmlWriter::appendSpecial(Array<Sint8>& out, char x)
{
    _appendSpecialChar(out, Char16(x));
}

void XmlWriter::appendSpecial(Array<Sint8>& out, const char* str)
{
    while (*str)
	_appendSpecialChar(out, *str++);
}

void XmlWriter::appendSpecial(Array<Sint8>& out, const String& str)
{
    const Char16* tmp = str.getData();

    while (*tmp)
	_appendSpecialChar(out, *tmp++);
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

//------------------------------------------------------------------------------
//
// appendMethodCallHeader()
//
//     Build HTTP request header.
//
//------------------------------------------------------------------------------

void XmlWriter::appendMethodCallHeader(
    Array<Sint8>& out,
    const char* host,
    const char* cimMethod,
    const String& cimObject,
    const String& authenticationHeader,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "M-POST /cimom HTTP/1.1\r\n";
    out << "HOST: " << host << "\r\n";
    out << "Content-CIMType: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << contentLength << "\r\n";
    out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMOperation: MethodCall\r\n";
    out << nn << "-CIMMethod: " << cimMethod << "\r\n";
    out << nn << "-CIMObject: " << cimObject << "\r\n";
    if (authenticationHeader.size())
    {
        out << authenticationHeader << "\r\n";
    }
    out << "\r\n";
}

//------------------------------------------------------------------------------
//
// appendMethodResponseHeader()
//
//     Build HTTP response header.
//
//------------------------------------------------------------------------------

void XmlWriter::appendMethodResponseHeader(
    Array<Sint8>& out,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "HTTP/1.1 200 OK\r\n";
    out << "Content-CIMType: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << contentLength << "\r\n";
    out << "Ext:\r\n";
    out << "Cache-Control: no-cache\r\n";
    out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMOperation: MethodResponse\r\n\r\n";
}

//------------------------------------------------------------------------------
//
// appendUnauthorizedResponseHeader()
//
//     Build HTTP authentication response header for unauthorized requests.
//
//     Returns unauthorized message in the following format:
//
//        HTTP/1.1 401 Unauthorized
//        WWW-Authenticate: Basic "hostname:80"
//        <HTML><HEAD>
//        <TITLE>401 Unauthorized</TITLE>
//        </HEAD><BODY BGCOLOR="#99cc99">
//        <H2>TEST401 Unauthorized</H2>
//        <HR>
//        </BODY></HTML>
//
//------------------------------------------------------------------------------

void XmlWriter::appendUnauthorizedResponseHeader(
    Array<Sint8>& out,
    const String& content)
{
    out << "HTTP/1.1 401 Unauthorized\r\n";
    out << content << "\r\n";
    out << "\r\n";

//ATTN: We may need to include the following line, so that the browsers
//      can display the error message.
//    out << "<HTML><HEAD>\r\n";
//    out << "<TITLE>" << "401 Unauthorized" <<  "</TITLE>\r\n";
//    out << "</HEAD><BODY BGCOLOR=\"#99cc99\">\r\n";
//    out << "<H2>TEST" << "401 Unauthorized" << "</H2>\r\n";
//    out << "<HR>\r\n";
//    out << "</BODY></HTML>\r\n";
}

//------------------------------------------------------------------------------
//
// appendMessageElementBegin()
// appendMessageElementEnd()
//
//     <!ELEMENT MESSAGE (SIMPLEREQ|MULTIREQ|SIMPLERSP|MULTIRSP)>
//     <!ATTLIST MESSAGE
//         ID CDATA #REQUIRED
//         PROTOCOLVERSION CDATA #REQUIRED>
//
//------------------------------------------------------------------------------

void XmlWriter::appendMessageElementBegin(
    Array<Sint8>& out,
    const String& messageId)
{
    out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
    out << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">\n";
    out << "<MESSAGE ID=\"" << messageId << "\" PROTOCOLVERSION=\"1.0\">\n";
}

void XmlWriter::appendMessageElementEnd(
    Array<Sint8>& out)
{
    out << "</MESSAGE>\n";
    out << "</CIM>\n";
}

//------------------------------------------------------------------------------
//
// appendSimpleReqElementBegin()
// appendSimpleReqElementEnd()
//
//     <!ELEMENT SIMPLEREQ (IMETHODCALL|METHODCALL)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendSimpleReqElementBegin(
    Array<Sint8>& out)
{
    out << "<SIMPLEREQ>\n";
}

void XmlWriter::appendSimpleReqElementEnd(
    Array<Sint8>& out)
{
    out << "</SIMPLEREQ>\n";
}

//------------------------------------------------------------------------------
//
// appendMethodCallElementBegin()
// appendMethodCallElementEnd()
//
//     <!ELEMENT METHODCALL ((LOCALCLASSPATH|LOCALINSTANCEPATH),PARAMVALUE*)>
//     <!ATTLIST METHODCALL %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendMethodCallElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<METHODCALL NAME=\"" << name << "\">\n";
}

void XmlWriter::appendMethodCallElementEnd(
    Array<Sint8>& out)
{
    out << "</METHODCALL>\n";
}

//------------------------------------------------------------------------------
//
// appendIMethodCallElementBegin()
// appendIMethodCallElementEnd()
//
//     <!ELEMENT IMETHODCALL (LOCALNAMESPACEPATH,IPARAMVALUE*)>
//     <!ATTLIST IMETHODCALL %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendIMethodCallElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<IMETHODCALL NAME=\"" << name << "\">\n";
}

void XmlWriter::appendIMethodCallElementEnd(
    Array<Sint8>& out)
{
    out << "</IMETHODCALL>\n";
}

//------------------------------------------------------------------------------
//
// appendIParamValueElementBegin()
// appendIParamValueElementEnd()
//
//     <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
//         |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
//         |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
//     <!ATTLIST IPARAMVALUE %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendIParamValueElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<IPARAMVALUE NAME=\"" << name << "\">\n";
}

void XmlWriter::appendIParamValueElementEnd(
    Array<Sint8>& out)
{
    out << "</IPARAMVALUE>\n";
}

//------------------------------------------------------------------------------
//
// appendSimpleRspElementBegin()
// appendSimpleRspElementEnd()
//
//     <!ELEMENT SIMPLERSP (METHODRESPONSE|IMETHODRESPONSE)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendSimpleRspElementBegin(
    Array<Sint8>& out)
{
    out << "<SIMPLERSP>\n";
}

void XmlWriter::appendSimpleRspElementEnd(
    Array<Sint8>& out)
{
    out << "</SIMPLERSP>\n";
}

//------------------------------------------------------------------------------
//
// appendMethodResponseElementBegin()
// appendMethodResponseElementEnd()
//
//     <!ELEMENT METHODRESPONSE (ERROR|IRETURNVALUE?)>
//     <!ATTLIST METHODRESPONSE %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendMethodResponseElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<METHODRESPONSE NAME=\"" << name << "\">\n";
}

void XmlWriter::appendMethodResponseElementEnd(
    Array<Sint8>& out)
{
    out << "</METHODRESPONSE>\n";
}

//------------------------------------------------------------------------------
//
// appendIMethodResponseElementBegin()
// appendIMethodResponseElementEnd()
//
//     <!ELEMENT IMETHODRESPONSE (ERROR|IRETURNVALUE?)>
//     <!ATTLIST IMETHODRESPONSE %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendIMethodResponseElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<IMETHODRESPONSE NAME=\"" << name << "\">\n";
}

void XmlWriter::appendIMethodResponseElementEnd(
    Array<Sint8>& out)
{
    out << "</IMETHODRESPONSE>\n";
}

//------------------------------------------------------------------------------
//
// appendErrorElement()
//
//------------------------------------------------------------------------------

void XmlWriter::appendErrorElement(
    Array<Sint8>& out,
    CIMStatusCode code,
    const char* description)
{
    out << "<ERROR";
    out << " CODE=\"" << Uint32(code) << "\"";
    out << " DESCRIPTION=\"";
    appendSpecial(out, description);
    out << "\"/>";
}

//------------------------------------------------------------------------------
//
// appendReturnValueElement()
//
// <!ELEMENT RETURNVALUE (VALUE|VALUE.REFERENCE)>
// <!ATTLIST RETURNVALUE
//     %ParamType;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendReturnValueElement(
    Array<Sint8>& out,
    const CIMValue& value)
{
    out << "<RETURNVALUE";

    CIMType type = value.getType();
    if (type != CIMType::NONE)
    {
        out << " PARAMTYPE=\"" << TypeToString(type) << "\"";
    }

    out << ">\n";
    value.toXml(out);
    out << "</RETURNVALUE>\n";
}

//------------------------------------------------------------------------------
//
// appendIReturnValueElementBegin()
// appendIReturnValueElementEnd()
//
//      <!ELEMENT IRETURNVALUE (CLASSNAME*|INSTANCENAME*|VALUE*|
//          VALUE.OBJECTWITHPATH*|VALUE.OBJECTWITHLOCALPATH*|VALUE.OBJECT*|
//          OBJECTPATH*|QUALIFIER.DECLARATION*|VALUE.ARRAY?|VALUE.REFERENCE?|
//          CLASS*|INSTANCE*|VALUE.NAMEDINSTANCE*)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendIReturnValueElementBegin(
    Array<Sint8>& out)
{
    out << "<IRETURNVALUE>\n";
}

void XmlWriter::appendIReturnValueElementEnd(
    Array<Sint8>& out)
{
    out << "</IRETURNVALUE>\n";
}

//------------------------------------------------------------------------------
//
// appendBooleanIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendBooleanIParameter(
    Array<Sint8>& out,
    const char* name,
    Boolean flag)
{
    appendIParamValueElementBegin(out, name);
    out << "<VALUE>" << (flag ? "TRUE" : "FALSE") << "</VALUE>\n";
    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendStringIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendStringIParameter(
    Array<Sint8>& out,
    const char* name,
    const String& str)
{
    appendIParamValueElementBegin(out, name);
    out << "<VALUE>";
    appendSpecial(out, str);
    out << "</VALUE>\n";
    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendQualifierNameIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendQualifierNameIParameter(
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
    
    appendIParamValueElementBegin(out, name);
    appendClassNameElement(out, qualifierName);
    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendClassNameIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassNameIParameter(
    Array<Sint8>& out,
    const char* name,
    const String& className)
{
    appendIParamValueElementBegin(out, name);
    appendClassNameElement(out, className);
    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendInstanceNameIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceNameIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMReference& instanceName)
{
    appendIParamValueElementBegin(out, name);
    instanceName.instanceNameToXml(out);
    appendIParamValueElementEnd(out);
}

void XmlWriter::appendObjectNameIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMReference& objectName)
{
    if (objectName.isClassName())
    {
	XmlWriter::appendClassNameIParameter(
	    out, name, objectName.getClassName());
    }
    else
    {
	XmlWriter::appendInstanceNameIParameter(
	    out, name, objectName);
    }
}

//------------------------------------------------------------------------------
//
// appendClassIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMConstClass& cimClass)
{
    appendIParamValueElementBegin(out, name);
    cimClass.toXml(out);
    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendInstanceIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMConstInstance& instance)
{
    appendIParamValueElementBegin(out, name);
    instance.toXml(out);
    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendNamedInstanceIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendNamedInstanceIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMNamedInstance& namedInstance)
{
    appendIParamValueElementBegin(out, name);
    namedInstance.toXml(out);
    appendIParamValueElementEnd(out);
}

//----------------------------------------------------------
//
//  appendPropertyNameIParameter()
//   	
//     </IPARAMVALUE>
//     <IPARAMVALUE NAME="PropertyName"><VALUE>FreeSpace</VALUE></IPARAMVALUE>
//
//     USE: Create parameter for getProperty operation
//==========================================================
void XmlWriter::appendPropertyNameIParameter(
    Array<Sint8>& out,
    const String& propertyName)
{
    appendIParamValueElementBegin(out, "PropertyName");
    out << "<VALUE>" << propertyName << "</VALUE>\n"; 
    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendPropertyValueIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendPropertyValueIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMValue& value)
{
    appendIParamValueElementBegin(out, name);
    value.toXml(out);
    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendPropertyListIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendPropertyListIParameter(
    Array<Sint8>& out,
    const CIMPropertyList& propertyList)
{
    appendIParamValueElementBegin(out, "PropertyList");

    out << "<VALUE.ARRAY>\n";
    for (Uint32 i = 0; i < propertyList.getNumProperties(); i++)
    {
        out << "<VALUE>" << propertyList.getPropertyName(i) << "</VALUE>\n"; 
    }
    out << "</VALUE.ARRAY>\n";

    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendQualifierDeclarationIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendQualifierDeclarationIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMConstQualifierDecl& qualifierDecl)
{
    appendIParamValueElementBegin(out, name);
    qualifierDecl.toXml(out);
    appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendClassNameElement()
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassNameElement(
    Array<Sint8>& out,
    const String& className)
{
    out << "<CLASSNAME NAME=\"" << className << "\"/>\n";
}

//------------------------------------------------------------------------------
//
// appendInstanceNameElement()
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceNameElement(
    Array<Sint8>& out,
    const CIMReference& instanceName)
{
    instanceName.instanceNameToXml(out);
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleMethodReqMessage()
//
//------------------------------------------------------------------------------

// ATTN-RK-P1-20020228: Need to complete copy elimination optimization
Array<Sint8> XmlWriter::formatSimpleMethodReqMessage(
    const char* host,
    const String& nameSpace,
    const char* methodName,
    const String& messageId,
    const String& authenticationHeader,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    appendMessageElementBegin(out, messageId);
    appendSimpleReqElementBegin(out);
    appendMethodCallElementBegin(out, methodName);
//    appendLocalPathElement(out, nameSpace); // ATTN-RK-P1-20020228:  Need this
    out << body;
    appendMethodCallElementEnd(out);
    appendSimpleReqElementEnd(out);
    appendMessageElementEnd(out);

    appendMethodCallHeader(
	tmp,
	host,
	methodName,
	nameSpace,  // ATTN-RK: Is this right?
        authenticationHeader,
	out.size());
    tmp << out;

    return tmp;
}

Array<Sint8> XmlWriter::formatSimpleMethodRspMessage(
    const char* methodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    appendMessageElementBegin(out, messageId);
    appendSimpleRspElementBegin(out);
    appendMethodResponseElementBegin(out, methodName);
    out << body;
    appendMethodResponseElementEnd(out);
    appendSimpleRspElementEnd(out);
    appendMessageElementEnd(out);

    appendMethodResponseHeader(tmp, out.size());
    tmp << out;

    return tmp;
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
    const String& authenticationHeader,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    appendMessageElementBegin(out, messageId);
    appendSimpleReqElementBegin(out);
    appendIMethodCallElementBegin(out, iMethodName);
    appendLocalNameSpaceElement(out, nameSpace);
    out << body;
    appendIMethodCallElementEnd(out);
    appendSimpleReqElementEnd(out);
    appendMessageElementEnd(out);

    appendMethodCallHeader(
	tmp,
	host,
	iMethodName,
	nameSpace,
        authenticationHeader,
	out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleIMethodRspMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleIMethodRspMessage(
    const char* iMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    appendMessageElementBegin(out, messageId);
    appendSimpleRspElementBegin(out);
    appendIMethodResponseElementBegin(out, iMethodName);
    appendIReturnValueElementBegin(out);
    out << body;
    appendIReturnValueElementEnd(out);
    appendIMethodResponseElementEnd(out);
    appendSimpleRspElementEnd(out);
    appendMessageElementEnd(out);

    appendMethodResponseHeader(tmp, out.size());
    tmp << out;

    return tmp;
}

//******************************************************************************
//
// Export Messages (used for indications)
//
//******************************************************************************

//------------------------------------------------------------------------------
//
// appendEMethodRequestHeader()
//
//     Build HTTP request header for export operation.
//
//------------------------------------------------------------------------------

void XmlWriter::appendEMethodRequestHeader(
    Array<Sint8>& out,
    const char* host,
    const char* cimMethod,
    const String& authenticationHeader,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "M-POST /cimom HTTP/1.1\r\n";
    out << "HOST: " << host << "\r\n";
    out << "Content-CIMType: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << contentLength << "\r\n";
    out << "Man: http://www.hp.com; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMExport: MethodRequest\r\n";  // ATTN-RK-P2-20020228: Should this be "MethodCall"?
    out << nn << "-CIMExportMethod: " << cimMethod << "\r\n";
    if (authenticationHeader.size())
    {
        out << authenticationHeader << "\r\n";
    }
    out << "\r\n";
}

//------------------------------------------------------------------------------
//
// appendEMethodResponseHeader()
//
//     Build HTTP response header for export operation.
//
//------------------------------------------------------------------------------

void XmlWriter::appendEMethodResponseHeader(
    Array<Sint8>& out,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "HTTP/1.1 200 OK\r\n";
    out << "Content-CIMType: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << contentLength << "\r\n";
    out << "Ext:\r\n";
    out << "Cache-Control: no-cache\r\n";
    out << "Man:  http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMExport: MethodResponse\r\n\r\n";
}

//------------------------------------------------------------------------------
//
// appendSimpleExportReqElementBegin()
// appendSimpleExportReqElementEnd()
//
//     <!ELEMENT SIMPLEEXPREQ (EXPMETHODCALL)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendSimpleExportReqElementBegin(
    Array<Sint8>& out)
{
    out << "<SIMPLEEXPREQ>\n";
}

void XmlWriter::appendSimpleExportReqElementEnd(
    Array<Sint8>& out)
{
    out << "</SIMPLEEXPREQ>\n";
}

//------------------------------------------------------------------------------
//
// appendEMethodCallElementBegin()
// appendEMethodCallElementEnd()
//
//     <!ELEMENT EXPMETHODCALL (IPARAMVALUE*)>
//     <!ATTLIST EXPMETHODCALL %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendEMethodCallElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<EXPMETHODCALL NAME=\"" << name << "\">\n";
}

void XmlWriter::appendEMethodCallElementEnd(
    Array<Sint8>& out)
{
    out << "</EXPMETHODCALL>\n";
}

//------------------------------------------------------------------------------
//
// appendSimpleExportRspElementBegin()
// appendSimpleExportRspElementEnd()
//
//     <!ELEMENT SIMPLEEXPRSP (EXPMETHODRESPONSE)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendSimpleExportRspElementBegin(
    Array<Sint8>& out)
{
    out << "<SIMPLEEXPRSP>\n";
}

void XmlWriter::appendSimpleExportRspElementEnd(
    Array<Sint8>& out)
{
    out << "</SIMPLEEXPRSP>\n";
}

//------------------------------------------------------------------------------
//
// formatEMethodResponseElement()
//
//     <!ELEMENT EXPMETHODRESPONSE (ERROR|IRETURNVALUE?)>
//     <!ATTLIST EXPMETHODRESPONSE %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendEMethodResponseElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<EXPMETHODRESPONSE NAME=\"" << name << "\">\n";
}

void XmlWriter::appendEMethodResponseElementEnd(
    Array<Sint8>& out)
{
    out << "</EXPMETHODRESPONSE>\n";
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleEMethodReqMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleEMethodReqMessage(
    const char* host,
    const char* eMethodName,
    const String& messageId,
    const String& authenticationHeader,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    appendMessageElementBegin(out, messageId);
    appendSimpleExportReqElementBegin(out);
    appendEMethodCallElementBegin(out, eMethodName);
    out << body;
    appendEMethodCallElementEnd(out);
    appendSimpleExportReqElementEnd(out);
    appendMessageElementEnd(out);

    appendEMethodRequestHeader(
        tmp,
        host,
        eMethodName,
        authenticationHeader,
	out.size());
    tmp << out;

    return out;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleEMethodRspMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleEMethodRspMessage(
    const char* eMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    appendMessageElementBegin(out, messageId);
    appendSimpleExportRspElementBegin(out);
    appendEMethodResponseElementBegin(out, eMethodName);
    out << body;
    appendEMethodResponseElementEnd(out);
    appendSimpleExportRspElementEnd(out);
    appendMessageElementEnd(out);

    appendEMethodResponseHeader(tmp, out.size());
    tmp << out;

    return tmp;
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
	_appendSpecial(os, attributes[i].value);
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
		_appendSpecial(os, entry.text);
		os << "-->";
		break;
	    }

	    case XmlEntry::CONTENT:
	    {
		_indent(os, stack.size(), indentChars);
		_appendSpecial(os, entry.text);
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

PEGASUS_NAMESPACE_END
