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
// Revision 1.14  2001/02/18 23:28:14  karl
// add namespace stuff. Not finished
//
// Revision 1.13  2001/02/18 19:02:16  mike
// Fixed CIM debacle
//
// Revision 1.12  2001/02/18 03:56:00  mike
// Changed more class names (e.g., ConstClassDecl -> CIMConstClass)
//
// Revision 1.11  2001/02/16 02:06:06  mike
// Renamed many classes and headers.
//
// Revision 1.10  2001/02/11 05:45:33  mike
// Added case insensitive logic for files in CIMRepository
//
// Revision 1.9  2001/02/06 15:47:56  karl
// add propety functions
//
// Revision 1.8  2001/02/05 14:14:58  karl
// instance test
//
// Revision 1.7  2001/02/02 21:59:45  karl
// fix enuminstances and some DOC++
//
// Revision 1.6  2001/01/31 15:57:06  karl
// comments
//
// Revision 1.5  2001/01/31 09:26:32  karl
// Update CGIClient for instances
//
// Revision 1.4  2001/01/31 08:41:45  mike
// updated makefile
//
// Revision 1.3  2001/01/30 07:38:33  karl
// add instance operations, getinstance, enumerateinstance
//
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

/* CGIClient - This is a CGI driven test program that 
1. Makes calls to Pegasus CIMOperations client functions
using paramaters derived from an environment variable
defined using CGI specifications.
2. Analyzes the results and prints an HTML page 
with the results for the funtion defined.

NOTE: The functions in this program are largely
made up of single CIM Operation functions (ex. getclass)
so the parameters for each function match the parameters
defined for the CIM Operation iteslf.

This Program was intended to be a test and demonstration tool for
Pegasus.
*/

#include <cassert>
#include <cstdlib>
#include <Pegasus/Common/CGIQueryString.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Stopwatch.h>

using namespace Pegasus;
using namespace std;

/** Class to hold, get, put, etc. the host info.
This info must be maintained between calls to CGI client
so is maintained in a configuration file for the client.
These are set initially to localhost and 8888.
HostInfo is defined as those parameters associated with a particular
CIMOM CIMServer and user of that server
ATTN: Under Construction
*/
class HostInfo
{
public:
    void setHostName(const char* str);
    const char* getHostName();
    void setHostPort(const char* str);
    int getHostPort();
    const char* getHostPortString();
};
	
void HostInfo::setHostName( const char* str)
{
  ///ATTN: to be done.
}

const char* HostInfo::getHostName()
{
    return("localhost");
}
void HostInfo::setHostPort(const char* str)
{
  /// ATTN: To be done
}
int HostInfo::getHostPort()
{
   return 8888;
}
const char* HostInfo::getHostPortString()
{
   return "8888";
}

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

/** PrintHeader - Print the HTML banner. THis is a table with
    the text definition for the page and the TOG logo.
    @param - Text for the title
*/
static void PrintHeader(const String& title)
{
    String img = "/pegasus/icons/OpenGroupLogo.gif";

    cout << "<table width=\"100%\">\n";
    cout << "<tr>\n";
    cout << "<td width=\"50%\" align=\"bottom\"><h1>" << title << "</h1><td>\n";
    cout << "<td width=\"50%\" align=\"right\">\n";
    cout << "<img border=1 src=\"" << img << "\">\n";
    cout << "</td>\n";
    cout << "</tr>\n";
    cout << "</table>\n";
}

/** PrintHTMLHead - Prints the HTML opening, document title
    and the page banner informatio
    @param string title - The text for the title field
    @param string header - The text for the banner line. This string 
    identifies the function of the page.
*/
static void PrintHTMLHead(const String& title, const String& header)
{
     cout << "<html>\n";
     PrintHead(title);
     cout << "<body bgcolor=\"#CCCCCC\">\n";
     PrintHeader(header);
     PrintRule();
}

/** ErrorExit - Print out the error message as an
    HTML page and get out.
    @param - Text for error message
    @return - None, Terminates the program
    @execption - This function terminates the program
*/ 
void ErrorExit(const String& message)
{
    PrintHTMLHead("Message", "Error in CGIClient");
    // kscout << "<html>\n";
    // ksPrintHead("Message");
    // ksPrintHeader("Error");
    // ksPrintRule();
    // kscout << "  <body bgcolor=\"#CCCCCC\">\n";
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
/** PrintA - Prints a single href
    @param href - the reference for building the href
    @parm content - The content of the reference
*/
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
    cout << "<th>CIMName</th>\n";
    cout << "<th>CIMType</th>\n";
    cout << "<th>CIMValue</th>\n";
    cout << "</tr>\n";
}
/** Header for the Properties HTML table
Columns in the table are CIMProperty CIMName, type, Vlaue, ClassOrigin,
Propogated indicator.
*/
static void PrintPropertiesTableHeader(const String& tableName)
{
    cout << "<h2>" << tableName << "</h2>\n";
    cout << "<table border=1 width=\"50%\">\n";
    cout << "<tr>\n";
    cout << "<th>CIMName</th>\n";
    cout << "<th>CIMType</th>\n";
    cout << "<th>CIMValue</th>\n";
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
    cout <<
    "<td><img src=\"/pegasus/icons/OpenGroupLogo.gif\"></td>\n";
    cout << "</tr>\n";
    cout << "</table>\n";
}

void PrintSingleProperty(CIMProperty& property)
{
    PrintTableHeader("CIMProperty:");

    const CIMValue& value = property.getValue();

    PrintRow(
	property.getName(),
	TypeToString(value.getType()),
	value.toString());

    PrintTableTrailer();
}
/** PrintObjectProperties - Template for a function
that prints the Properties information for either Classes
or instances.	 This prints an HTML table of the properties
fields including name, type, value, ClassOrigin, and propagated.
The name is wrapped in an href so a click will get detailed
property information.

This is a template function so that it can be used with both instance
and class definitions.
@param nameSpace. Used to query for namespace value.  WHY???
@parm object - Either the class or instance object address
@param includeClassOrigin to be used to define tests on getting this field
*/
template<class OBJECT>
void PrintObjectProperties(
    const String& nameSpace,
    OBJECT& object,
    Boolean includeClassOrigin)
{
    PrintPropertiesTableHeader("Properties:");
    // Loop for each property
    for (Uint32 i = 0, n = object.getPropertyCount(); i < n; i++)
    {
	CIMProperty property = object.getProperty(i);
	const CIMValue& value = property.getValue();
	// Define href with the property name
	String href = "/pegasus/cgi-bin/CGIClient?";
	href.append("Operation=GetPropertyDeclaration&");
	href.append("NameSpace=");
	href.append(EncodeQueryStringValue(nameSpace));
	href.append("&");
	href.append("ClassName=");
	href.append(object.getClassName());
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
	// Output the ClassOrigin
	// ATTN: Make this optional
	cout << "<td>" << property.getClassOrigin() << "</td>\n";
	// Output the Propagated field
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
	CIMConstQualifier qualifier = object.getQualifier(i);
	const CIMValue& value = qualifier.getValue();

	PrintRow(
	    qualifier.getName(),
	    TypeToString(value.getType()),
	    value.toString());
    }

    PrintTableTrailer();
}
/** Prepare an HTML table with a header and an entry
    for each method defined in the class with the CIMName
    and type of the CIMMethod in each entry
    @param Classdecl - Class for which methods to be output
*/
void PrintClassMethods(CIMClass& classDecl)
{
    cout << "<h2>Methods:</h2>\n";
    // Create the table
    cout << "<table border=1 width=\"50%\">\n";
    cout << "<tr>\n";
    cout << "<th>CIMName</th>\n";
    cout << "<th>CIMType</th>\n";
    cout << "</tr>\n";

    for (Uint32 i = 0, n = classDecl.getMethodCount(); i < n; i++)
    {
	CIMMethod method = classDecl.getMethod(i);
	CIMType type = method.getType();

	cout << "<tr>\n";
	cout << "<td>" << method.getName() << "</td>\n";
	cout << "<td>" << TypeToString(type) << "</td>\n";
	cout << "<tr>\n";
    }

    cout << "</table>\n";
}

/** PrintClass - Print an HTML page with the characteristics of
    the Class defined in the call including:
    <UL>
	<LI>ClassName
	<LI>Qualifiers
	<LI>Properties
	<LI>Methods
    <UL>
    @param String with nameSpace
    @param pointer to class
    @param localOnly
    @param includeQualifiers
    @param includClassOrigin
*/
void PrintClass(
    const String& nameSpace,
    CIMClass& classDecl,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PrintHTMLHead("GetClass", classDecl.getClassName());
    
    if (includeQualifiers)
	PrintQualifiers(classDecl);
    PrintObjectProperties(nameSpace, classDecl,includeClassOrigin);
    PrintClassMethods(classDecl);

    cout << "</body>\n" << "</html>\n";
}

/** PrintInstance - Print an HTML page with the characteristics
    of the instance including:
    <UL>
	<LI>ClassName
	<LI>Qualifiers
	<LI>Properties
    </UL>
    Note that methods are at the class level, not the instance
    level so they do not appear in the Instance page.
*/
void PrintInstance(
const String& nameSpace,
    CIMInstance& instanceDecl,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PrintHTMLHead("GetInstance", instanceDecl.getClassName());
    PrintQualifiers(instanceDecl);
    PrintObjectProperties(nameSpace, instanceDecl, localOnly);

    cout << "</body>\n" << "</html>\n";
}

void PrintPropertyDeclaration(CIMProperty& property)
{
    PrintHTMLHead("GetPropertyDeclaration", property.getName());
    
    PrintQualifiers(property);
    PrintSingleProperty(property);

    cout << "</body>\n";
    cout << "</html>\n";
}

/** Function GetClass Peforms the getClass
    request and prints the result as an HTML page  
*/
static void GetClass(const CGIQueryString& qs)
{
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
	ErrorExit("Missing ClassName field");

    String className = tmp;

    if (!className.getLength())
	ErrorExit("ClassName parameter is null");

    // Process Checkbox items that become call opptions
    // Set the Defaults
    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;
    // Process possible input fields
    // Wierd because the form entry only sends info if
    // 
    if (!(tmp = qs.findValue("LocalOnly")))
	localOnly = false;
    if (!(tmp = qs.findValue("IncludeQualifiers")))
	includeQualifiers = false;
    if ((tmp = qs.findValue("IncludeClassOrigin")))
	includeClassOrigin = true;

    try
    {
	CIMClient client;
	HostInfo hostinfo;
	client.connect(hostinfo.getHostName(), hostinfo.getHostPort());

	CIMClass classDecl = client.getClass(nameSpace, className,
	    localOnly, includeQualifiers, includeClassOrigin);

	PrintClass(nameSpace, classDecl,localOnly, includeQualifiers, 
	    includeClassOrigin);
    }
     catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

/** Function GetPropertyDeclaration
    This function is NOT a a WBEM Function. It is used by
    the Get Class function to get properties for the getClass
    presentation. This function uses the getClass with the
    PropertyName parameter to find each property
    get the property and Print each property.
*/
static void GetPropertyDeclaration(const CGIQueryString& qs)
{
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:

    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
	ErrorExit("Missing ClassName field");

    String className = tmp;

    if (!className.getLength())
	ErrorExit("ClassName parameter is null");

    // Get PropertyName:

    if (!(tmp = qs.findValue("PropertyName")))
	ErrorExit("Missing ClassName field");

    String propertyName = tmp;

    if (!propertyName.getLength())
	ErrorExit("PropertyName parameter is null");

    //

    try
    {
	CIMClient client;
	client.connect("localhost", 8888);
	// get the class
	CIMClass classDecl = client.getClass(
	    nameSpace, className, false, true, true);
	// 
	Uint32 pos = classDecl.findProperty(propertyName);

	if (pos == Uint32(-1))
	{
	    ErrorExit("No such property");
	    return;
	}
	// Now Get the property
	CIMProperty property = classDecl.getProperty(pos);

	PrintPropertyDeclaration(property);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

static void PrintClassNames(
    const String& nameSpace,
    const Array<String>& classNames,
    double elapsedTime)
{
    PrintHTMLHead("GetClassNames", "EnumerateClassNames Result");
    // cout << "<html>\n";
    // PrintHead("GetClassNames");
    // cout << "<body bgcolor=\"#CCCCCC\">\n";

    // PrintHeader("EnumerateClassNames Result");
    //PrintRule();

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
    // Close the Table
    cout << "</table>\n";

    // Close the Page
    cout << "<p>Returned " << classNames.getSize() << " ClassNames ";
    cout << " in " << elapsedTime << " Seconds</p>\n";
    cout << "</body>\n" << "</html>\n";
}
/** EnumerateClassNames gets the parameters for NameSpace
and ClassName and calls the  enumerate class name
CIMOperation.
The returned array in sent to printclassnames
*/
static void EnumerateClassNames(const CGIQueryString& qs)
{
    // Get NameSpace: 
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String className;

    const char* tmp;

    // Get the ClassName field:
    if ((tmp = qs.findValue("ClassName")))
	className = tmp;

    // Get DeepInheritance: 
    Boolean deepInheritance = false;

    if (qs.findValue("DeepInheritance"))
	deepInheritance = true;

    // Invoke the method: 
    try
    {
	// Time the connection
	Stopwatch elapsedTime;
	
	// Make the Connection
	CIMClient client;
	client.connect("localhost", 8888);
	
	Array<String> classNames = client.enumerateClassNames(
	    nameSpace, className, deepInheritance);

	// Print the results
	PrintClassNames(nameSpace, classNames, elapsedTime.getElapsed());

    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

static void DeleteClass(const CGIQueryString& qs)
{
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
	CIMClient client;
	client.connect("localhost", 8888);

	client.deleteClass(nameSpace, className);

	String message = "Class \"";
	message += className;
	message.append("\" was deleted");
	PrintHTMLHead("DeleteClass", "Delete Class");
        cout << "    <h1>" << message << "</h1>\n";
        cout << "  </body>\n";
        cout << "</html>\n";
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

void PrintQualifierRow(const String& nameSpace,
		       const CIMQualifierDecl& qd)
{
    cout << "<tr>\n";

    const CIMValue& value = qd.getValue();

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
    CIMQualifierDecl qualifierDecl)
{
    PrintHTMLHead("GetQualifier", "GetQualifier");
    // cout << "<html>\n";
    // PrintHead("GetQualifier");
    // cout << "<body bgcolor=\"#CCCCCC\">\n";

    // PrintHeader("GetQualifier");
    // PrintRule();

    cout << "<table border=1 width=\"50%\">\n";
    cout << "  <tr>\n";
    cout << "    <th>CIMName</th>\n";
    cout << "    <th>CIMType</th>\n";
    cout << "    <th>CIMValue</th>\n";
    cout << "    <th>CIMScope</th>\n";
    cout << "    <th>CIMFlavor</th>\n";
    cout << "    <th>ArraySize</th>\n";
    cout << "  </tr>\n";
    cout << "</tr>\n";

    cout << "<h1>CIMQualifier:</h1>\n";
    PrintQualifierRow(nameSpace, qualifierDecl);

    cout << "</table>\n";

    cout << "</body>\n";
    cout << "</html>\n";
}

void PrintEnumerateQualifiers(
    const String& nameSpace,
    const Array<CIMQualifierDecl>& qualifierDecls)
{
    // Check this, why no HTML header here.????

    PrintHead("EnumerateQualifiers");
    cout << "<body bgcolor=\"#CCCCCC\">\n";

    PrintHeader("EnumerateQualifiers");
    PrintRule();

    cout << "<table border=1 width=\"50%\">\n";
    cout << "  <tr>\n";
    cout << "    <th>CIMName</th>\n";
    cout << "    <th>CIMType</th>\n";
    cout << "    <th>CIMValue</th>\n";
    cout << "    <th>CIMScope</th>\n";
    cout << "    <th>CIMFlavor</th>\n";
    cout << "    <th>ArraySize</th>\n";
    cout << "  </tr>\n";
    cout << "</tr>\n";

    cout << "<h1>Qualifiers:</h1>\n";

    for (Uint32 i = 0; i < qualifierDecls.getSize(); i++)
    {
	PrintQualifierRow(nameSpace, qualifierDecls[i]);
    }

    cout << "</table>\n";

    cout << "</body>\n" << "</html>\n";
}
/* CIMMethod to execute the EnumerateQualifiers operation
*/
static void EnumerateQualifiers(const CGIQueryString& qs)
{
    String nameSpace = GetNameSpaceQueryField(qs);

    try
    {
	CIMClient client;
	client.connect("localhost", 8888);

	Array<CIMQualifierDecl> qualifierDecls =
	    client.enumerateQualifiers(nameSpace);

	PrintEnumerateQualifiers(nameSpace, qualifierDecls);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

/* CIMMethod to execute the getQualifier Operation
*/
static void GetQualifier(const CGIQueryString& qs)
{
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get CIMQualifier name:
    const char* tmp;

    if (!(tmp = qs.findValue("QualifierName")))
	ErrorExit("Missing QualifierName field");

    String qualifierName = tmp;

    if (!qualifierName.getLength())
	ErrorExit("QualifierName parameter is null");

    try
    {
	CIMClient client;
	client.connect("localhost", 8888);

	CIMQualifierDecl qd = client.getQualifier(nameSpace, qualifierName);

	PrintGetQualifier(nameSpace, qd);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

/** PrintInstanceNames
    Prints the HTML form for the names provided in the
    String array of instancenames.
    The table created includes an href for each name so that
    a click on the href entry will get the instance
    Note that we assume the defaults for the extra
    parameters on the getInstance
    ATTN: Change so the user can select extra params.
*/
static void PrintInstanceNames(
    const String& nameSpace,
    const Array<String>& InstanceNames,
    double elapsedTime)
{
    PrintHTMLHead("GetInstanceNames", "EnumerateInstanceNames Result");
    // cout << "<html>\n";
    // PrintHead("GetClassNames");
    // cout << "<body bgcolor=\"#CCCCCC\">\n";

    // PrintHeader("EnumerateClassNames Result");
    //PrintRule();
    cout << "trace operation Print Instance Names\r\n\r\n";  

    cout << "<table border=1>\n";
    cout << "<tr><th>Instance Names</th><tr>\n";
    // For each name prepare the table entry with an href for
    // click access to the getInstance for that name
    for (Uint32 i = 0, n = InstanceNames.getSize(); i < n; i++)
    {
	cout << "<tr><td>\n";

	String href = "/pegasus/cgi-bin/CGIClient?";
	href.append("Operation=GetInstance&");
	href.append("NameSpace=");
	href.append(EncodeQueryStringValue(nameSpace));
	href.append("&");

	href.append("InstanceName=");
	href.append(InstanceNames[i]);
	href.append("&");

	href.append("LocalOnly=true");
	href.append("includQualifiers=false");
	href.append("includeClassOrigin=false");
        
	PrintA(href, InstanceNames[i]);

	cout << "</tr></td>\n";
    }
    // Close the HTML Table
    cout << "</table>\n";

    // Close the Page
    cout << "<p>Returned " << InstanceNames.getSize() << " Instances ";
    cout << " in " << elapsedTime << " Seconds</p>\n";
    cout << "</body>\n" << "</html>\n";
  
}

/** EnumerateInstanceNames Function
    Called for evaluation of the EvaluateInstance Names operation.
    Gets the parameters from the CGIQuery String and calls
    the enumerateInstanceNames CIMClient function.
    The resulting string array of names is printed by
    the function printInstanceNames
*/
static void EnumerateInstanceNames(const CGIQueryString& qs)
{
    // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;

    // Invoke the method:

    try
    {
	// Time the connection
	Stopwatch elapsedTime;

	CIMClient client;
	client.connect("localhost", 8888);
	
	// Call enumerate Instances CIM Method
	Array<CIMReference> instanceNames = client.enumerateInstanceNames(
	    nameSpace, className);
	
	Array<String> tmpInstanceNames;

	for (Uint32 i = 0; i < instanceNames.getSize(); i++)
	{
	    String tmp;
	    CIMReference::referenceToInstanceName(instanceNames[i], tmp);
	    tmpInstanceNames.append(tmp);
	}

	// Print the name array
	PrintInstanceNames(nameSpace, tmpInstanceNames, elapsedTime.getElapsed());
        }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

/** GetInstance Function
This function is executed for the getInstance Operation
It takes the parameters from the CGIQueryString
to create parameters for the getInstance CIMClient
method call.
The results of the call are printed in an HTML page.
*/
static void GetInstance(const CGIQueryString& qs)
{
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get InstanceName:
    const char* tmp;

    if (!(tmp = qs.findValue("InstanceName")))
	ErrorExit("Missing InstanceName field");

    // This must be modified for the toString ATTN KS
    CIMReference referenceName;
    try
    {
    CIMReference::instanceNameToReference(tmp,referenceName);
    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }


    // if (!instanceName.getLength())
    // ErrorExit("InstanceName parameter is null");

    // ATTN: handle these later!

    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    try
    {
	CIMClient client;
	client.connect("localhost", 8888);
          
	CIMInstance instanceDecl = client.getInstance(nameSpace, 
	    referenceName, localOnly, includeClassOrigin, includeClassOrigin);

	PrintInstance(nameSpace, instanceDecl, localOnly, includeQualifiers, 
		includeClassOrigin); }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}


static void EnumerateInstances(const CGIQueryString& qs)
{
 // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;

    // Invoke the method:
    String message = "operation EnumerateInstances Under Construction: ";
    ErrorExit(message);
    //try
    //{
    //    CIMClient client;
    //    client.connect("localhost", 8888);
    //
    //    Array<String> classNames = client.enumerateInstancs(
    //        nameSpace, className);
    //    // Print the name array
    //    PrintInstance(nameSpace, classNames);
    //}
    //catch(Exception& e)
    //{
    //    ErrorExit(e.getMessage());
    //}
   
}

static void GetProperty(const CGIQueryString& qs)
{
  // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;
    String message = "operation getProperty Under Construction: ";
    ErrorExit(message);

}

static void SetProperty(const CGIQueryString& qs)
{
  // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;
    String message = "operation setProperty Under Construction: ";
    ErrorExit(message);

}


static void DeleteInstance(const CGIQueryString& qs)
{
  // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;
    String message = "operation DeleteInstance Under Construction: ";
    ErrorExit(message);

}

static void CreateNameSpace(const CGIQueryString& qs)
{
  // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String nameSpaceName;
    String className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;
    String message = "CreateNameSpace Under Construction: "; 
    ErrorExit(message);

}

static void DeleteNameSpace(const CGIQueryString& qs)
{
  // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String nameSpaceName;
    String className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;
    String message = "operation deleteNameSpace Under Construction: ";
    ErrorExit(message);

}


/** DefineHostParameters - Function to make the changes
to the basic host parameters if the input parameters are 
set.
*/
static void DefineHostParameters(const CGIQueryString& qs)
{
    const char* tmp;
    HostInfo hostInfo;

    if ((tmp = qs.findValue("HostURL")))
	hostInfo.setHostName(tmp);

    if ((tmp = qs.findValue("HostPort")))
	hostInfo.setHostPort("8888");

    /// Respond with the new parameters
    PrintHTMLHead("GetInstanceNames", "EnumerateInstanceNames Result");

    cout << "<B>Host CIMName</B>  ";
    cout << hostInfo.getHostName();
    cout << "\n";
    cout << "<P><B>Host Port</B>  ";
    cout << hostInfo.getHostPortString();
    cout << "\n";
    cout << "</body>\n" << "</html>\n";


}
/******************************************************************
   MAIN - Main function of CGIClient.
   Outputs the top line of the HTML, gets the function type
   from the Env variable QUERY_STRING (CGI env var)
   and calls the appropriate function
   No command line arguments are expected for CGICLIENT
*/
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
	else if (strcmp(operation, "GetPropertyDeclaration") == 0)
	    GetPropertyDeclaration(qs);
        else if (strcmp(operation, "EnumerateQualifiers") == 0)
	    EnumerateQualifiers(qs);
        else if (strcmp(operation, "GetQualifier") == 0)
	    GetQualifier(qs);
        else if (strcmp(operation, "GetInstance") == 0)
	    GetInstance(qs);
        else if (strcmp(operation, "DeleteInstance") == 0)
	    DeleteInstance(qs);
        else if (strcmp(operation, "EnumerateInstanceNames") == 0)
	    EnumerateInstanceNames(qs);
        else if (strcmp(operation, "EnumerateInstances") == 0)
	    EnumerateInstances(qs);
	else if (strcmp(operation, "DefineHostParameters") == 0)
	    DefineHostParameters(qs);
        else if (strcmp(operation, "GetProperty") == 0)
	    GetProperty(qs);
	else if (strcmp(operation, "SetProperty") == 0)
	    SetProperty(qs);
	else if (strcmp(operation, "CreateNameSpace") == 0)
	    CreateNameSpace(qs);
	else if (strcmp(operation, "DeleteNameSpace") == 0)
	    DeleteNameSpace(qs);
        else
	{
	    String message = "CGIClient - Unknown operation: ";
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
/* TO List
Make sure that The program name is in all error mesages.
Version on CGIClient.
Is class names and instance names the same basic thing that we can do 
with template.
*/
