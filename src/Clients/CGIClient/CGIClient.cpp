//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: CGIClient.cpp,v $
// Revision 1.2  2001/01/26 23:26:53  mike
// reworked CGI inteface
//
// Revision 1.1  2001/01/20 21:45:14  karl
// relocated this directory
//
// Revision 1.1.1.1  2001/01/14 19:50:25  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <cstdlib>
#include <Pegasus/Common/CGIQueryString.h>
#include <Pegasus/Client/Client.h>

using namespace Pegasus;
using namespace std;

void PrintRule()
{
    cout << "<hr></hr>";
}

static void PrintHead(const String& title)
{
    cout << "<head>\n";
    cout << "  <title>" << title << "</title>\n";
    cout << "</head>\n";
}

static void PrintHeader(const String& title)
{
    String img = "http://localhost/pegasus/icons/OpenGroupLogo.gif";

    cout << "<table width=\"100%\">\n";
    cout << "<tr>\n";
    cout << "<td width=\"50%\" align=\"bottom\"><h1>" << title << "</h1><td>\n";
    cout << "<td width=\"50%\" align=\"right\">\n";
    cout << "<img border=1 src=\"" << img << "\">\n";
    cout << "</td>\n";
    cout << "</tr>\n";
    cout << "</table>\n";
}

void ErrorExit(const String& message)
{
    cout << "<html>\n";
    PrintHead("Message");
    PrintHeader("Error");
    PrintRule();
    cout << "  <body bgcolor=\"#CCCCCC\">\n";
    cout << "    <h1>" << message << "</h1>\n";
    cout << "  </body>\n";
    cout << "</html>\n";
    exit(1);
}

String GetNameSpaceQueryField(const CGIQueryString& qs)
{
    const char* tmp;

    if (!(tmp = qs.findValue("NameSpace")))
	ErrorExit("Missing NameSpace field");

    String nameSpace = tmp;

    if (!nameSpace.getLength())
	ErrorExit("NameSpace parameter is null");

    return nameSpace;
}

static String EncodeQueryStringValue(const String& x)
{
    String result;

    for (Uint32 j = 0, m = x.getLength(); j < m; j++)
    {
	char c = x[j];

	if (c == '/')
	    result.append("%2F");
	else
	    result.append(c);
    }

    return result;
}

static void PrintA(const String& href, const String& content)
{
    cout << "<a href=\"" << href << "\">\n";
    cout << content << '\n';
    cout << "</a>\n";
}

static void PrintTableHeader(const String& tableName)
{
    cout << "<h2>" << tableName << "</h2>\n";
    cout << "<table border=1 width=\"50%\">\n";
    cout << "<tr>\n";
    cout << "<th>Name</th>\n";
    cout << "<th>Type</th>\n";
    cout << "<th>Value</th>\n";
    cout << "</tr>\n";
}

static void PrintPropertiesTableHeader(const String& tableName)
{
    cout << "<h2>" << tableName << "</h2>\n";
    cout << "<table border=1 width=\"50%\">\n";
    cout << "<tr>\n";
    cout << "<th>Name</th>\n";
    cout << "<th>Type</th>\n";
    cout << "<th>Value</th>\n";
    cout << "<th>ClassOrigin</th>\n";
    cout << "<th>Propagated</th>\n";
    cout << "</tr>\n";
}

static void PrintTableTrailer()
{
    cout << "</table>\n";
}

static void PrintRow(
    const String& name,
    const String& type,
    const String& value)
{
    cout << "<tr>\n";
    cout << "<td>" << name << "</td>\n";
    cout << "<td>" << type << "</td>\n";

    if (value.getLength())
	cout << "<td>" << value << "</td>\n";
    else
	cout << "<td>null</td>\n";

    cout << "<tr>\n";
}

static void PrintLogo()
{
    cout << "<table border=2>\n";
    cout << "<tr>\n";
    cout << "<td><img src=\"http://localhost/pegasus/icons/OpenGroupLogo.gif\"></td>\n";
    cout << "</tr>\n";
    cout << "</table>\n";
}

void PrintSingleProperty(Property& property)
{
    PrintTableHeader("Property:");

    const Value& value = property.getValue();

    PrintRow(
	property.getName(),
	TypeToString(value.getType()),
	value.toString());

    PrintTableTrailer();
}

void PrintClassProperties(
    const String& nameSpace,
    ClassDecl& classDecl)
{
    PrintPropertiesTableHeader("Properties:");

    for (Uint32 i = 0, n = classDecl.getPropertyCount(); i < n; i++)
    {
	Property property = classDecl.getProperty(i);
	const Value& value = property.getValue();

	String href = "/pegasus/cgi-bin/CGIClient?";
	href.append("Operation=GetProperty&");
	href.append("NameSpace=");
	href.append(EncodeQueryStringValue(nameSpace));
	href.append("&");
	href.append("ClassName=");
	href.append(classDecl.getClassName());
	href.append("&");
	href.append("PropertyName=");
	href.append(property.getName());

	cout << "<tr>\n";
	cout << "<td>";
	PrintA(href, property.getName());
	cout << "</td>";
	cout << "<td>" << TypeToString(value.getType()) << "</td>\n";

	String valueString = value.toString();

	if (valueString.getLength())
	    cout << "<td>" << valueString << "</td>\n";
	else
	    cout << "<td>null</td>\n";

	cout << "<td>" << property.getClassOrigin() << "</td>\n";

	cout << "<td>" << (property.getPropagated() ? "true" : "false");
	cout << "</td>\n";

	cout << "<tr>\n";
    }

    PrintTableTrailer();
}

template<class OBJECT>
void PrintQualifiers(OBJECT& object)
{
    PrintTableHeader("Qualifiers:");

    for (Uint32 i = 0, n = object.getQualifierCount(); i < n; i++)
    {
	ConstQualifier qualifier = object.getQualifier(i);
	const Value& value = qualifier.getValue();

	PrintRow(
	    qualifier.getName(),
	    TypeToString(value.getType()),
	    value.toString());
    }

    PrintTableTrailer();
}

void PrintClassMethods(ClassDecl& classDecl)
{
    cout << "<h2>Methods:</h2>\n";
    cout << "<table border=1 width=\"50%\">\n";
    cout << "<tr>\n";
    cout << "<th>Name</th>\n";
    cout << "<th>Type</th>\n";
    cout << "</tr>\n";

    for (Uint32 i = 0, n = classDecl.getMethodCount(); i < n; i++)
    {
	Method method = classDecl.getMethod(i);
	Type type = method.getType();

	cout << "<tr>\n";
	cout << "<td>" << method.getName() << "</td>\n";
	cout << "<td>" << TypeToString(type) << "</td>\n";
	cout << "<tr>\n";
    }

    cout << "</table>\n";
}

void PrintClass(
    const String& nameSpace,
    ClassDecl& classDecl)
{
    cout << "<html>\n";
    PrintHead("GetClass");
    cout << "<body bgcolor=\"#CCCCCC\">\n";

    PrintHeader(classDecl.getClassName());
    PrintRule();

    PrintQualifiers(classDecl);
    PrintClassProperties(nameSpace, classDecl);
    PrintClassMethods(classDecl);

    cout << "</body>\n";
    cout << "</html>\n";
}

void PrintProperty(Property& property)
{
    cout << "<html>\n";
    PrintHead("GetProperty");
    cout << "<body bgcolor=\"#CCCCCC\">\n";

    PrintHeader(property.getName());
    PrintRule();

    PrintQualifiers(property);
    PrintSingleProperty(property);

    cout << "</body>\n";
    cout << "</html>\n";
}

static void GetClass(const CGIQueryString& qs)
{
    // Get NameSpace:

    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:

    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
	ErrorExit("Missing ClassName field");

    String className = tmp;

    if (!className.getLength())
	ErrorExit("ClassName parameter is null");

    // ATTN: handle these later!

    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    try
    {
	Client client;
	client.connect("localhost", 8888);

	ClassDecl classDecl = client.getClass(nameSpace, className, 
	    localOnly, includeClassOrigin, includeClassOrigin);

	PrintClass(nameSpace, classDecl);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

static void GetProperty(const CGIQueryString& qs)
{
    //--------------------------------------------------------------------------
    // Get NameSpace:
    //--------------------------------------------------------------------------

    String nameSpace = GetNameSpaceQueryField(qs);

    //--------------------------------------------------------------------------
    // Get ClassName:
    //--------------------------------------------------------------------------

    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
	ErrorExit("Missing ClassName field");

    String className = tmp;

    if (!className.getLength())
	ErrorExit("ClassName parameter is null");

    //--------------------------------------------------------------------------
    // Get PropertyName:
    //--------------------------------------------------------------------------

    if (!(tmp = qs.findValue("PropertyName")))
	ErrorExit("Missing ClassName field");

    String propertyName = tmp;

    if (!propertyName.getLength())
	ErrorExit("PropertyName parameter is null");

    //--------------------------------------------------------------------------

    try
    {
	Client client;
	client.connect("localhost", 8888);

	ClassDecl classDecl = client.getClass(
	    nameSpace, className, false, true, true);

	Uint32 pos = classDecl.findProperty(propertyName);

	if (pos == Uint32(-1))
	{
	    ErrorExit("No such property");
	    return;
	}

	Property property = classDecl.getProperty(pos);

	PrintProperty(property);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

static void PrintClassNames(
    const String& nameSpace,
    const Array<String>& classNames)
{
    cout << "<html>\n";
    PrintHead("GetClassNames");
    cout << "<body bgcolor=\"#CCCCCC\">\n";

    PrintHeader("EnumerateClassNames Result");
    PrintRule();

    cout << "<table border=1>\n";
    cout << "<tr><th>Class Names</th><tr>\n";

    for (Uint32 i = 0, n = classNames.getSize(); i < n; i++)
    {
	cout << "<tr><td>\n";

	String href = "/pegasus/cgi-bin/CGIClient?";
	href.append("Operation=GetClass&");
	href.append("NameSpace=");
	href.append(EncodeQueryStringValue(nameSpace));
	href.append("&");

	href.append("ClassName=");
	href.append(classNames[i]);
	href.append("&");

	href.append("LocalOnly=true");

	PrintA(href, classNames[i]);

	cout << "</tr></td>\n";
    }

    cout << "</table>\n";
    cout << "</body>\n";
    cout << "</html>\n";
}

static void EnumerateClassNames(const CGIQueryString& qs)
{
    //--------------------------------------------------------------------------
    // Get NameSpace:
    //--------------------------------------------------------------------------

    String nameSpace = GetNameSpaceQueryField(qs);

    //--------------------------------------------------------------------------
    // Get ClassName:
    //--------------------------------------------------------------------------

    String className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;

    //--------------------------------------------------------------------------
    // Get DeepInheritance:
    //--------------------------------------------------------------------------

    Boolean deepInheritance = false;

    if (qs.findValue("DeepInheritance"))
	deepInheritance = true;

    //--------------------------------------------------------------------------
    // Invoke the method:
    //--------------------------------------------------------------------------

    try
    {
	Client client;
	client.connect("localhost", 8888);

	Array<String> classNames = client.enumerateClassNames(
	    nameSpace, className, deepInheritance); 

	PrintClassNames(nameSpace, classNames);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

static void DeleteClass(const CGIQueryString& qs)
{
    // Get NameSpace:

    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:

    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
	ErrorExit("Missing ClassName field");

    String className = tmp;

    if (!className.getLength())
	ErrorExit("ClassName parameter is null");

    try
    {
	Client client;
	client.connect("localhost", 8888);

	client.deleteClass(nameSpace, className);

	String message = "Class \"";
	message += className;
	message.append("\" was deleted");
	ErrorExit(message);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

void PrintQualifierRow(const String& nameSpace, const QualifierDecl& qd)
{
    cout << "<tr>\n";

    const Value& value = qd.getValue();

    String href = "/pegasus/cgi-bin/CGIClient?";
    href.append("Operation=GetQualifier&");
    href += "NameSpace=";
    href += EncodeQueryStringValue(nameSpace);
    href += '&';
    href += "QualifierName=";
    href += qd.getName();

    cout << "<td>";
    PrintA(href, qd.getName());
    cout << "</td>";
    cout << "<td>" << TypeToString(value.getType()) << "</td>\n";
    cout << "<td>" << value.toString() << "</td>\n";
    cout << "<td>" << ScopeToString(qd.getScope()) << "</td>\n";
    cout << "<td>" << FlavorToString(qd.getFlavor()) << "</td>\n";
    cout << "<td>" << qd.getArraySize() << "</td>\n";

    cout << "</tr>\n";
}

void PrintGetQualifier(
    const String& nameSpace,
    QualifierDecl qualifierDecl)
{
    cout << "<html>\n";
    PrintHead("GetQualifier");
    cout << "<body bgcolor=\"#CCCCCC\">\n";

    PrintHeader("GetQualifier");
    PrintRule();

    cout << "<table border=1 width=\"50%\">\n";
    cout << "  <tr>\n";
    cout << "    <th>Name</th>\n";
    cout << "    <th>Type</th>\n";
    cout << "    <th>Value</th>\n";
    cout << "    <th>Scope</th>\n";
    cout << "    <th>Flavor</th>\n";
    cout << "    <th>ArraySize</th>\n";
    cout << "  </tr>\n";
    cout << "</tr>\n";

    cout << "<h1>Qualifier:</h1>\n";
    PrintQualifierRow(nameSpace, qualifierDecl);

    cout << "</table>\n";

    cout << "</body>\n";
    cout << "</html>\n";
}

void PrintEnumerateQualifiers(
    const String& nameSpace,
    const Array<QualifierDecl>& qualifierDecls)
{
    cout << "<html>\n";
    PrintHead("EnumerateQualifiers");
    cout << "<body bgcolor=\"#CCCCCC\">\n";

    PrintHeader("EnumerateQualifiers");
    PrintRule();

    cout << "<table border=1 width=\"50%\">\n";
    cout << "  <tr>\n";
    cout << "    <th>Name</th>\n";
    cout << "    <th>Type</th>\n";
    cout << "    <th>Value</th>\n";
    cout << "    <th>Scope</th>\n";
    cout << "    <th>Flavor</th>\n";
    cout << "    <th>ArraySize</th>\n";
    cout << "  </tr>\n";
    cout << "</tr>\n";

    cout << "<h1>Qualifiers:</h1>\n";

    for (Uint32 i = 0; i < qualifierDecls.getSize(); i++)
    {
	PrintQualifierRow(nameSpace, qualifierDecls[i]);
    }

    cout << "</table>\n";

    cout << "</body>\n";
    cout << "</html>\n";
}

static void EnumerateQualifiers(const CGIQueryString& qs)
{
    // Get NameSpace:

    String nameSpace = GetNameSpaceQueryField(qs);

    try
    {
	Client client;
	client.connect("localhost", 8888);

	Array<QualifierDecl> qualifierDecls =
	    client.enumerateQualifiers(nameSpace);

	PrintEnumerateQualifiers(nameSpace, qualifierDecls);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

static void GetQualifier(const CGIQueryString& qs)
{
    // Get NameSpace:

    String nameSpace = GetNameSpaceQueryField(qs);

    // Get Qualifier name:

    const char* tmp;

    if (!(tmp = qs.findValue("QualifierName")))
	ErrorExit("Missing QualifierName field");

    String qualifierName = tmp;

    if (!qualifierName.getLength())
	ErrorExit("QualifierName parameter is null");

    try
    {
	Client client;
	client.connect("localhost", 8888);

	QualifierDecl qd = client.getQualifier(nameSpace, qualifierName);

	PrintGetQualifier(nameSpace, qd);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

int main(int argc, char** argv)
{
    cout << "Content-type: text/html\r\n\r\n";

    if (argc != 1)
	ErrorExit("unexpected command line arguments");

    const char* tmp = getenv("QUERY_STRING");

    if (!tmp)
	ErrorExit("QUERY_STRING environment variable missing");

    try
    {
        char* queryString = strcpy(new char[strlen(tmp) + 1], tmp);

	CGIQueryString qs(queryString);

        const char* operation = qs.findValue("Operation");

	if (!operation)
	    ErrorExit("Missing Operation field");

	if (strcmp(operation, "GetClass") == 0)
	    GetClass(qs);
	else if (strcmp(operation, "EnumerateClassNames") == 0)
	    EnumerateClassNames(qs);
	else if (strcmp(operation, "DeleteClass") == 0)
	    DeleteClass(qs);
	else if (strcmp(operation, "GetProperty") == 0)
	    GetProperty(qs);
	else if (strcmp(operation, "EnumerateQualifiers") == 0)
	    EnumerateQualifiers(qs);
	else if (strcmp(operation, "GetQualifier") == 0)
	    GetQualifier(qs);
	else
	{
	    String message = "Unknown operation: ";
	    message.append(operation);
	    ErrorExit(message);
	}
    }
    catch (Exception& e)
    {
	char* msg = e.getMessage().allocateCString();
	ErrorExit(msg);
    }

    return 0;
}
