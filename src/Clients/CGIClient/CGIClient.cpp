//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

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
#include <cstdio>
#include <Pegasus/Common/CGIQueryString.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/Selector.h>
#include <Pegasus/Common/Logger.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;


#define DDD(X) X
//#define DDD(X) /* X */

/*
class operationhRef
{
public:
    hRef(const String& operation)
    appendPair (const String& key, const String& value
    appendKey  (const String& key)
    appendValue(const String& value)

private:
}

void operationhRef:appendKey(const String& key)
{

}

*/

/** Class to hold, get, put, etc. the host info.
This info must be maintained between calls to CGI client
so is maintained in a configuration file for the client.
These are set initially to localhost and 5988.
HostInfo is defined as those parameters associated with a particular
CIMOM CIMServer and user of that server
ATTN: Under Construction
*/
class HostInfo
{
public:
    void setHostName(const char* str);
    const char* getAddress();
private:
    char _address[128];
};
//    sprintf(address, "%s:%d",
//	    hostinfo.getHostName(), hostinfo.getHostPort());


void HostInfo::setHostName( const char* str)
{
  ///ATTN: to be done.
}

const char* HostInfo::getAddress()
{
    const char* tmp = getenv("QUERY_STRING");
    char* queryString = strcpy(new char[strlen(tmp) + 1], tmp);
    CGIQueryString qs(queryString);

    if (tmp = qs.findValue("hostaddress"))
	return tmp; 
    else 
	return("localhost:5988");
}
String PrintSuperClassName(String superClassName)
{
    if (superClassName == "")
	return "No Superclass";
    else
	return superClassName;
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

    if (!nameSpace.size())
	ErrorExit("NameSpace parameter is null");

    return nameSpace;
}

static String EncodeQueryStringValue(const String& x)
{
    String result;

    for (Uint32 j = 0, m = x.size(); j < m; j++)
    {
	char c = x[j];

	if (c == '/')
	    result.append("%2F");
	else if	 (c == ':')
	    result.append("%3A");
	else    
	    result.append(c);
    }

    return result;
}
/** PrintA - Prints a single href
    @param href - the reference for building the href
    @param content - The content of the reference
*/
static void PrintA(const String& href, const String& content)
{
    cout << "<a href=\"" << href << "\">\n";
    cout << content << "</a>\n";
}

static void PrintTableHeader(const String& tableName)
{
    cout << "<h2>" << tableName << "</h2>\n";
    cout << "<table border=1 width=\"90%\">\n";
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
    cout << "<table border=1 width=\"90%\">\n";
    cout << "<tr>\n";
    cout << "<th>CIMName</th>\n";
    cout << "<th>CIMType</th>\n";
    cout << "<th>CIMValue</th>\n";
    cout << "<th>ClassOrigin</th>\n";
    cout << "<th>Propagated</th>\n";
    cout << "</tr>\n";
}

/** Create an href to call CGIClient with the namespace and
    operation field set up.
    @return - The result string
*/
//ATTN: Why do we not do the EncodeQueryStringValue
//ATTN: Why is host address part of this?
String BuildOperationHref(String operation, String nameSpace)
{
    
    String result = "/pegasus/cgi-bin/CGIClient?";
    result.append("Operation=");
    result.append(operation);
    result.append("&");
    result.append("NameSpace=");
    result.append(EncodeQueryStringValue(nameSpace));
    result.append("&");
    HostInfo hostinfo;
    result.append("hostaddress=");
    result.append(EncodeQueryStringValue(hostinfo.getAddress()));
    result.append("&");
    return(result);
}

/** createClassHref - Builds a HTML href from the class name to

*/
String createClassHref(String nameSpace,String operation, String className)
{
    // ATTN: Remove this if everything works
    // String href = "/pegasus/cgi-bin/CGIClient?";
    // href.append("Operation=");
    // href.append(operation);
    // href.append("&NameSpace=");
    // href.append(EncodeQueryStringValue(nameSpace));
    // href.append("&");

    String href = BuildOperationHref(operation,nameSpace);

    href.append("ClassName=");
    href.append(className);
    href.append("&");
    HostInfo hostinfo;
    href.append("hostaddress=");
    href.append(EncodeQueryStringValue(hostinfo.getAddress()));
    href.append("&");

    return(href);
}


static void PrintTableTrailer()
{
    cout << "</table>\n";
}

/** PrintRow - Prints a single table row with 3 entires
    The entries are name, type, value
*/
static void PrintRow(
    const String& name,
    const String& type,
    const String& value)
{
    cout << "<tr>\n";
    cout << "<td>" << name << "</td>\n";
    cout << "<td>" << type << "</td>\n";

    if (value.size())
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
@param object Either the class or instance object address
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
	// ATTN: All of this can become PrintProperty()
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
	href.append("&"); 
	HostInfo hostinfo;
	href.append("hostaddress=");
	href.append(EncodeQueryStringValue(hostinfo.getAddress()));

	cout << "<tr>\n";
	cout << "<td>";
	PrintA(href, property.getName());
	cout << "</td>";
	cout << "<td>" << TypeToString(value.getType()) << "</td>\n";

	String valueString = value.toString();

	if (valueString.size())
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
    @param cimClass - Class for which methods to be output
*/
void PrintClassMethods(CIMClass& cimClass)
{
    // ATTN:If there are no methods.  State that rather than empty table
    cout << "<h2>Methods:</h2>\n";
    // Create the table
    cout << "<table border=1 width=\"50%\">\n";
    cout << "<tr>\n";
    cout << "<th>CIMName</th>\n";
    cout << "<th>CIMType</th>\n";
    cout << "</tr>\n";

    for (Uint32 i = 0, n = cimClass.getMethodCount(); i < n; i++)
    {
	CIMMethod method = cimClass.getMethod(i);
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
    CIMClass& cimClass,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    //ATTN: Should say GetClass and then classname
    //Aregument for combining to one parm
    PrintHTMLHead("GetClass", cimClass.getClassName());

    //Consider short table of request parameters

    cout << "<h2>SuperClass Name:</h2>\n";
    if (cimClass.getSuperClassName() == "")
	cout << "No Super Class" <<endl;
    else
	cout << cimClass.getSuperClassName() << endl;


    if (includeQualifiers)
	PrintQualifiers(cimClass);
    else
	cout << "\n\nQualifier Parameters Not Requested" << endl;
    PrintObjectProperties(nameSpace, cimClass,includeClassOrigin);
    PrintClassMethods(cimClass);

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
    CIMInstance& cimInstance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PrintHTMLHead("GetInstance", cimInstance.getClassName());
    PrintQualifiers(cimInstance);
    PrintObjectProperties(nameSpace, cimInstance, localOnly);

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

/***************************************************************************
   GetClass Function
***************************************************************************/

/** Function GetClass Peforms the getClass
    request and prints the result as an HTML page
*/
static void GetClass(const CGIQueryString& qs)
{
    String nameSpace = GetNameSpaceQueryField(qs);
    DDD(cout << "GetClass" << endl;)
    // Get ClassName:
    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
	ErrorExit("Missing ClassName field");

    String className = tmp;

    if (!className.size())
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
	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	CIMClass cimClass = client.getClass(nameSpace, className,
	    localOnly, includeQualifiers, includeClassOrigin);

	PrintClass(nameSpace, cimClass,localOnly, includeQualifiers,
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

    if (!className.size())
	ErrorExit("ClassName parameter is null");

    // Get PropertyName:

    if (!(tmp = qs.findValue("PropertyName")))
	ErrorExit("Missing ClassName field");

    String propertyName = tmp;

    if (!propertyName.size())
	ErrorExit("PropertyName parameter is null");

    //

    try
    {
	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	// get the class
	CIMClass cimClass = client.getClass(
	    nameSpace, className, false, true, true);
	//
	Uint32 pos = cimClass.findProperty(propertyName);

	if (pos == PEG_NOT_FOUND)
	{
	    ErrorExit("No such property");
	    return;
	}
	// Now Get the property
	CIMProperty property = cimClass.getProperty(pos);

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

    cout << "<table border=1>\n";
    cout << "<tr><th>Class Names</th><tr>\n";

    for (Uint32 i = 0, n = classNames.size(); i < n; i++)
    {
	cout << "<tr><td>\n";

	String href = createClassHref(nameSpace,
	    "GetClass",
	    classNames[i]);


	PrintA(href, classNames[i]);

	cout << "</tr></td>\n";
           }
    // Close the Table
    cout << "</table>\n";

    // Close the Page
    cout << "<p>Returned " << classNames.size() << " ClassNames ";
    cout << " in " << elapsedTime << " Seconds</p>\n";
    cout << "</body>\n" << "</html>\n";
}


/***************************************************************************
   EnumerateClassNames Function
***************************************************************************/
 /**
    EnumerateClassNames gets the parameters for NameSpace
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

    DDD(cout << "EnumerateClassNames" << endl;)

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
	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

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

 /***************************************************************************
   DeleteClass Function
***************************************************************************/

/** DeleteClass - Deletes the class defined on input
*/
static void DeleteClass(const CGIQueryString& qs)
{
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
	ErrorExit("Missing ClassName field");

    String className = tmp;

    if (!className.size())
	ErrorExit("ClassName parameter is null");

    try
    {
	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	client.deleteClass(nameSpace, className);

	String message = "Class \"";
	message += className;
	message.append("\" was deleted");
	PrintHTMLHead("DeleteClass", "Delete Class Result");
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

    href.append("&"); 
    HostInfo hostinfo;
    href.append("&hostaddress=");
    href.append(EncodeQueryStringValue(hostinfo.getAddress()));


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
    PrintHTMLHead("GetQualifier", "GetQualifier Result");
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

    for (Uint32 i = 0; i < qualifierDecls.size(); i++)
    {
	PrintQualifierRow(nameSpace, qualifierDecls[i]);
    }

    cout << "</table>\n" << "</body>\n" << "</html>\n";
}

/***************************************************************************
   EnumerateQualifiers Function
***************************************************************************/
/* CIMMethod to execute the EnumerateQualifiers operation
*/
static void EnumerateQualifiers(const CGIQueryString& qs)
{
    String nameSpace = GetNameSpaceQueryField(qs);

    try
    {
	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	Array<CIMQualifierDecl> qualifierDecls =
	    client.enumerateQualifiers(nameSpace);

	PrintEnumerateQualifiers(nameSpace, qualifierDecls);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

/***************************************************************************
   GetQualifier Function
***************************************************************************/
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

    if (!qualifierName.size())
	ErrorExit("QualifierName parameter is null");

    try
    {
	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	CIMQualifierDecl qd = client.getQualifier(nameSpace, qualifierName);

	PrintGetQualifier(nameSpace, qd);
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

/***************************************************************************
    PrintObjectNames Function
***************************************************************************/
/** PrintInstanceNames
    Prints the HTML form for the names provided in the
    String array of instancenames. Note that the instance names
    are provided as an array of CIMReference.
    The table created includes an href for each name so that
    a click on the href entry will get the instance
    Note that we assume the defaults for the extra
    parameters on the getInstance
    @param nameSpace
    @param InstanceNames - The array of references to print
    ATTN: Change so the user can select extra params.
*/
static void PrintObjectNames(
    const String& header,
    const String& nameSpace,
    const Array<CIMReference>& instanceNames,
    double elapsedTime)
{
    PrintHTMLHead("GetInstanceNames", header);

    cout << "<table border=1>\n";
    cout << "<tr><th>Object Names</th><tr>\n";
    // For each name prepare the table entry with an href for
    // click access to the getInstance for that name
    for (Uint32 i = 0, n = instanceNames.size(); i < n; i++)
    {
	cout << "<tr><td>\n";

	String href = "/pegasus/cgi-bin/CGIClient?";
	href.append("Operation=GetInstance&");
	href.append("NameSpace=");
	href.append(EncodeQueryStringValue(nameSpace));

	href.append("&");
	HostInfo hostinfo;
	href.append("hostaddress=");
	href.append(EncodeQueryStringValue(hostinfo.getAddress()));
	href.append("&");
	href.append("InstanceName=");

        //
        // Need to convert the '"' (double quote) character to '%22' to
        // make it a valid URL string in the HTML tag.  Also, need to
        // convert the '_' (underscore) character in the instanceName
        // string to a '.' (dot).
        //
        const String instanceName = instanceNames[i].toString();
        String nameString;

        for (Uint32 j = 0, n = instanceName.size(); j < n; j++)
        {
            switch (instanceName[j])
            {
                case '-':
                    nameString += ".";
                    break;

                case '"':
                    nameString += "%22";
                    break;

                default:
                    nameString += instanceName[j];
            }
        }

        href.append(nameString);
        href.append("&");

	href.append("LocalOnly=true");
	href.append("includQualifiers=false");
	href.append("includeClassOrigin=false");

	PrintA(href, instanceNames[i].toString());

	cout << "</tr></td>\n";
    }
    // Close the HTML Table
    cout << "</table>\n";

    // Close the Page
    cout << "<p>Returned " << instanceNames.size() << " Instances ";
    cout << " in " << elapsedTime << " Seconds</p>\n";
    cout << "</body>\n" << "</html>\n";

}

/***************************************************************************
    EnumerateInstanceNames Function
***************************************************************************/
/** EnumerateInstanceNames Function
    Called for evaluation of the EvaluateInstance Names operation.
    Gets the parameters from the CGIQuery String and calls
    the enumerateInstanceNames CIMClient function.
    The resulting string array of names is printed by
    the function PrintObjectNames
*/
static void EnumerateInstanceNames(const CGIQueryString& qs)
{
    // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String className;
    DDD(cout << "EnumerateInstanceNames for " << className <<endl;)

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;

    // Invoke the method:

    try
    {
	// Time the connection
	Stopwatch elapsedTime;

	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	// Call enumerate Instances CIM Method
	Array<CIMReference> instanceNames = client.enumerateInstanceNames(
	    nameSpace, className);

	// Print the CIMReference array
	PrintObjectNames( "EnumerateInstanceNames Result",
	    nameSpace, instanceNames, elapsedTime.getElapsed());
        }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}


/***************************************************************************
    GetInstance Function
***************************************************************************/
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
	referenceName = tmp;
    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }


    // if (!instanceName.size())
    // ErrorExit("InstanceName parameter is null");

    // ATTN: handle these later!

    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    try
    {
	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	CIMInstance cimInstance = client.getInstance(nameSpace,
	    referenceName, localOnly, includeClassOrigin, includeClassOrigin);

	PrintInstance(nameSpace, cimInstance, localOnly, includeQualifiers,
		includeClassOrigin); }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

/***************************************************************************
    EnumerateInstances Function
***************************************************************************/
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
    //    client.connect("localhost:5988");
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

/***************************************************************************
   GetProperty Function
***************************************************************************/
 static void GetProperty(const CGIQueryString& qs)
{
  // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    // String instanceName;
    String propertyName;

    const char* inputInstanceName;
    const char* tmp;

    if (!(inputInstanceName = qs.findValue("InstanceName")))
	ErrorExit("Missing InstanceName field");
    cout << "DEBUG GetProperty " << __LINE__
	<< " Name " << inputInstanceName << endl;
    // This must be modified for the toString ATTN KS
    CIMReference referenceName;

    // Convert instanceName to referenceName
    try
    {
	referenceName = inputInstanceName;
    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }
    if (!(tmp = qs.findValue("PropertyName")))
	ErrorExit("Missing propertyName field");
    else
	propertyName = tmp;
    cout << "GetProperty " << __LINE__ << endl;

    try
	{
	    Selector selector;
	    CIMClient client(&selector);
	    HostInfo hostinfo;
	    client.connect(hostinfo.getAddress());

	    CIMValue value = client.getProperty(nameSpace,
		referenceName, propertyName);


	    PrintHTMLHead("GetProperty", "GetProperty Result");


	    cout << "<B>Instance = </B> " <<
		inputInstanceName << "\n";

	    cout << "<B>Property = </B> " << propertyName << "\n\n";
	    cout << "<B>Value Type = </B>";

	    cout <<  TypeToString(value.getType()) << "\n\n";

	    String valueString = value.toString();
	    cout << "<B>Value = </B> ";

	    if (valueString.size())
	       cout << " " << valueString << " \n\n";
	    else
	       cout << " NULL \n\n";

	    cout << "</body>\n" << "</html>\n";
	}
	catch(Exception& e)
	{
	    ErrorExit(e.getMessage());
	}


}

/***************************************************************************
   SetProperty Function
***************************************************************************/
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

/***************************************************************************
   DeleteInstance Function
***************************************************************************/
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

/***************************************************************************
   CreateNameSpace Function
***************************************************************************/
static void CreateNameSpace(const CGIQueryString& qs)
{
  // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get nameSpace Name:
    String nameSpaceName;
    String className;

    const char* tmp;

    if ((tmp = qs.findValue("NewNameSpace")))
	nameSpaceName = tmp;

    // Create the instance
    CIMInstance newInstance("__Namespace");
    newInstance.addProperty(CIMProperty("name", nameSpaceName));
    try
    {
	// Time the connection
	Stopwatch elapsedTime;

	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	// Call create Instances CIM Method for class __Namespace
	cout << "Creating " << nameSpaceName;
	client.createInstance(nameSpace, newInstance);
	PrintHTMLHead("CreateNameSpace", "Create a NameSpace Result");
	cout << "<h1>Namespace " << nameSpaceName << " Created</H1>";
	cout << " in " << elapsedTime.getElapsed() << " Seconds</p>\n";
	cout << "</body>\n" << "</html>\n";
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
}

/***************************************************************************
   DeleteNameSpace Function
***************************************************************************/
/** DeleteNameSpace - Deletes the Namespace defined.
    Namespace deletion is done by deleting the instance of
    __Namespace defined by the input parameter
*/
static void DeleteNameSpace(const CGIQueryString& qs)
{
  // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get NameSpaceName to delete:
    String nameSpaceToDelete;
    const char* tmp;
    if ((tmp = qs.findValue("DeletionNameSpace")))
	nameSpaceToDelete= tmp;

    // Create Instance Name
    String instanceName = "__Namespace.name=\"";
    instanceName.append(nameSpaceToDelete);
    instanceName.append("\"");

    // Create Instance Reference. Name must be in form Reference
    CIMReference referenceName;
    try
    {
	referenceName = instanceName;
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
    // Now make connection and Delete the instance
    // Deleting the Instance of __Namespace deletes
    // the Namespace.
    try
    {
	// Time the connection
	Stopwatch elapsedTime;

	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	// Call delete Instances CIM Method for class __Namespace
	client.deleteInstance(nameSpace, referenceName);
	PrintHTMLHead("DeleteNameSpace", "Delete a NameSpace Result");
	cout << "<h1>Namespace " << nameSpaceToDelete << " Deleted</H1>";

	cout << " in " << elapsedTime.getElapsed() << " Seconds</p>\n";
	cout << "</body>\n" << "</html>\n";
    }
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }
 }

/***************************************************************************
   EnumerateNameSpaces Function
***************************************************************************/
static void EnumerateNameSpaces(const CGIQueryString& qs)
{
  // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String nameSpaceName;
    String className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
	className = tmp;

    // Invoke the method:
    try
    {
	// Time the connection
	Stopwatch elapsedTime;

	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	// Call enumerate Instances CIM Method
	Array<CIMReference> instanceNames = client.enumerateInstanceNames(
	    nameSpace, className);

	// Convert from CIMReference to String form
	Array<String> tmpInstanceNames;

        for (Uint32 i = 0; i < instanceNames.size(); i++)
	    tmpInstanceNames.append(instanceNames[i].toString());

	// Print the name array
	PrintHTMLHead("EnumerateNameSpaces", "Enumerate NameSpaces Result");

	cout << "<table border=1>\n";
	cout << "<tr><th>Namespaces</th><tr>\n";
	for (Uint32 i = 0, n = instanceNames.size(); i < n; i++)
	{
	    cout << "<tr><td>\n";
	    // Instnance name in form
	    // __Namespace.name="namespace"
	    // Strip off all but the namespace itself
	    String work = tmpInstanceNames[i];
	    Uint32 pos = work.find('\"');
	    if (pos != PEG_NOT_FOUND)
		work = tmpInstanceNames[i].subString((pos+1), Uint32(-1));

	    // remove trailing quote
	    work.remove((work.size() - 1),Uint32(-1));

	    // Create href for click to get classnames


	    String href = BuildOperationHref("EnumerateClassNames",work);

	    href.append("InstanceName=&");
	    href.append("DeepInheritance=true");

	    PrintA(href, work);

	    cout << "</tr></td>\n";
	}
	// Close the HTML Table
	cout << "</table>\n";

	// Close the Page
	cout << "<p>Click on a namespace to enumerate class Names</p>";
	cout << "<p>Returned " << instanceNames.size() << " Names";
	cout << " in " << elapsedTime.getElapsed() << " Seconds</p>\n";
	cout << "</body>\n" << "</html>\n";

	}
    catch(Exception& e)
    {
	ErrorExit(e.getMessage());
    }

}

/***************************************************************************
   DefineHostParameters Function
***************************************************************************/
/** DefineHostParameters - Function to make the changes
to the basic host parameters if the input parameters are
set.
*/
static void DefineHostParameters(const CGIQueryString& qs)
{
    // const char* tmp;
    HostInfo hostInfo;

    // if ((tmp = qs.findValue("HostURL")))
	// hostInfo.setHostName(tmp);

    // if ((tmp = qs.findValue("HostPort")))
	// hostInfo.setHostPort("8888");

    /// Respond with the new parameters
    PrintHTMLHead("DefineHostParameters", "HostName/Port");

    cout << "<B>Host CIMName</B>  ";
    cout << hostInfo.getAddress();
    cout << "\n";
    cout << "</body>\n" << "</html>\n";

}

/** PrintClassTreeEntry - Prints a single table entry for the class
    with the proper indenation, etc.
*/
void PrintClassTreeEntry(const String& nameSpace,
			 const String& className,
			 Uint32 level)
{
    cout << "<LI>";
    cout << level;
    cout << " ";

    //ATTN figure out why clasName in href and printa.
    String href = createClassHref(nameSpace,
		"GetClass",
		className);

    href.append("LocalOnly=true");


    PrintA(href, className);

}
/***************************************************************************
   TraverseClassTree Function
***************************************************************************/

/** TraverseClassTree - Traverse the Tree of super-to-subclasses
    printing each new subclass.	This function uses recursieve calls
    to traverse the complete Class Tree.
    Note that the initial call is expected to be -1 which starts
    us at the root.  We don't want to print the root.
    @param - nameSpace - target namespace. Used to build href
    @param - className at top of hiearchy (should be "" for complete
    @param - superClassNames - Array of superclasses to className
    array
    @param - classNames - Array of all classNames
    @param - size - size of className array
    @level - Current level of inheritance tree.
*/
void TraverseClassTree(
    const String& nameSpace,
    const String& className,
    const Array<String>& superClassNames,
    const Array<String>& classNames,
    Uint32 size,
    Uint32 level)
 {
    Boolean putUL = false;
    level++;

    for (Uint32 i = 0; i < size; i++)
    {
       if (className == superClassNames[i])
       {
	   if (!putUL)
	   {
	       putUL = true;
	       cout << "<ul>";
	   }

	   PrintClassTreeEntry(nameSpace,classNames[i], level);

	   TraverseClassTree(nameSpace,
			  classNames[i],
			  superClassNames,
			  classNames, size, level);
       }
    }

    if (putUL)
	cout << "</UL><!-- " << level << " -->\n";
}

/***************************************************************************
   ClassInheritance Function
***************************************************************************/

/**
    ClassInheritance
    This function operates on the same parameters as the enumerate classes
    This version simply prints out one level of the class tree starting at
    a defined superclass. Note that it does not print the starting class,
    Simply the subclasses.
    ATTN: In reality this is the enumerate class itself and should be
    merged with that.

*/
static void ClassInheritance(const CGIQueryString& qs)
{
    // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    String className;

    const char* tmp;

    // Get the timeout that is an option on this command
    Uint32 timeOut = 5 * 1000;
    if ((tmp = qs.findValue("timeout")))
	timeOut = 60 * 1000;
    // ATTN: Need the ascitointeger function.


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
	Boolean localOnly = false;
	Boolean includeQualifiers = false;
	Boolean includeClassOrigin = true;


	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());
	client.setTimeOut(timeOut);


	Array<CIMClass> classArray = client.enumerateClasses(
					nameSpace,
					className,
					deepInheritance,
					localOnly,
					includeQualifiers,
					includeClassOrigin);
	// Organize and Print the Class Tree results

	PrintHTMLHead("EnumerateInheritance", "Class Inheritance Result");

	cout << "<table border=1>\n";
	cout << "<tr><th>Super Class Names<th>Class Names<tr></th>\n";

	String classNameHrefBuilder;
	cout << "Count " <<   classArray.size() << endl;

	for (Uint32 i = 0, n = classArray.size(); i < n; i++)
	{
	    cout << "<tr><td>\n";

	    if (classArray[i].getSuperClassName() == "")
		cout << "No Superclass";
	    else
		cout << classArray[i].getSuperClassName();

	    cout << "<td>\n";

	    String href = createClassHref(nameSpace,
			"GetClass",
			classArray[i].getClassName());

	    href.append("LocalOnly=true");



	    PrintA(href, classArray[i].getClassName());

	    cout << "</td></tr>\n";
	}
	// Close the Table
	cout << "</table>\n";

	// Close the Page
	cout << "<p>Returned " << classArray.size() << " Classes ";
	cout << " in " << elapsedTime.getElapsed() << " Seconds</p>\n";
	cout << "</body>\n" << "</html>\n";



    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }

 }

/***************************************************************************
   ClassTree Function
***************************************************************************/

/**
    ClassTree
    Create a complete class tree of all of the classes in a namespace.

*/
static void ClassTree(const CGIQueryString& qs)
{
    const String rootClass = "";
    // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    // Invoke the method:
    try
    {
	// Time the connection
	Stopwatch elapsedTime;

	// Make the Connection
	Boolean deepInheritance = true;
	String className = "";

	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	Array<String> classNames = client.enumerateClassNames(
					nameSpace,
					className,
					deepInheritance);

	// Organize and Print the Class Tree results

	PrintHTMLHead("EnumerateClassTree", "Enumerate Classes Tree Result");

  	cout << "Count of Class Enumerate " <<   classNames.size() << endl;
	cout << " in " << elapsedTime.getElapsed() << " Seconds</p>\n";

        elapsedTime.reset();

	Boolean localOnly = true;
	Boolean includeQualifiers = false;
	Boolean includeClassOrigin = true;
	CIMClass myClass;
	Array<String> superClassNames;

	// Now we make loop getting classes to get the superclassname
	for (Uint32 i = 0, n = classNames.size(); i < n; i++)
	{
            // Now get the class corresponding to the class name
	    myClass = client.getClass(nameSpace,
				      classNames[i],
				      localOnly,
				      includeQualifiers,
				      includeClassOrigin);


	   superClassNames.append(myClass.getSuperClassName());
	}
 	TraverseClassTree(nameSpace, rootClass, superClassNames,
			    classNames,classNames.size(),PEG_NOT_FOUND);

	// Close the Page
	cout << "<p>Returned " << classNames.size() << " getClasses ";
	cout << " in " << elapsedTime.getElapsed() << " Seconds</p>\n";
	cout << "</body>\n" << "</html>\n";

    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }
}
/***************************************************************************
   ReferenceNames Function
***************************************************************************/

static void ReferenceNames(const CGIQueryString& qs)
{
    // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    const char* tmp;

    String objectName;
    if ((tmp = qs.findValue("ObjectName")))
	objectName = tmp;
    
    String resultClass = String::EMPTY;
    if ((tmp = qs.findValue("ResultClass")))
	resultClass = tmp;

    String role = String::EMPTY;
    if ((tmp = qs.findValue("Role")))
	role = tmp;

    // Invoke the method:
    try
    {
	// Time the connection
	Stopwatch elapsedTime;

	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	Array<CIMReference> objectReferences; 
	objectReferences = client.referenceNames(
		    nameSpace,
		    objectName,
		    resultClass,
		    role);
	// Generate table of returned CIMReferences
	// Similar to Instance names response.
	// Print the CIMReference array
	PrintObjectNames( "EnumerateReferenceNames Result",
	    nameSpace, objectReferences, elapsedTime.getElapsed());


    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }
  }

/***************************************************************************
   Associator Function
***************************************************************************/
static void AssociatorNames(const CGIQueryString& qs)
{
    // Get NameSpace:
    String nameSpace = GetNameSpaceQueryField(qs);

    const char* tmp;

    //Finish this one.
    CIMReference objectNameRef;
    String objectName;
    if ((tmp = qs.findValue("ObjectName")))
	objectName = tmp;
    
    String assocClass = String::EMPTY;
    if ((tmp = qs.findValue("AssocClass")))
	assocClass = tmp;
    
    
    String resultClass = String::EMPTY;
    if ((tmp = qs.findValue("ResultClass")))
	resultClass = tmp;

    String role = String::EMPTY;
    if ((tmp = qs.findValue("ResultRole")))
	role = tmp;

    String resultRole = String::EMPTY;
    if ((tmp = qs.findValue("Role")))
	resultRole = tmp;

    // Invoke the method:
    try
    {
	// Time the connection
	Stopwatch elapsedTime;

	Selector selector;
	CIMClient client(&selector);
	HostInfo hostinfo;
	client.connect(hostinfo.getAddress());

	Array<CIMReference> objectReferences; 
	objectReferences = client.associatorNames(
	    nameSpace,
	    objectName,
	    assocClass,
	    resultClass,
	    role,
	    resultRole);

	// Generate table of returned CIMReferences
	// Similar to Instance names response.

	PrintObjectNames( "EnumerateAssociationNames Result",
	    nameSpace, objectReferences, elapsedTime.getElapsed());

    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }
  }


/******************************************************************
   Main Function
*******************************************************************/

/**MAIN - Main function of CGIClient.
   Outputs the top line of the HTML, gets the function type
   from the Env variable QUERY_STRING (CGI env var)
   and calls the appropriate function
   No command line arguments are expected for CGICLIENT
*/
int main(int argc, char** argv)
{
    Logger::setHomeDirectory("");
    
    /*Logger::put( Logger::TRACE_LOG,  "CGIClient", Logger::WARNING,
	"X=$0, Y=$1, Z=$2", 
	88, 
	"Hello World", 
	7.5);
    */
    cout << "Content-type: text/html\r\n\r\n";

    if (argc != 1)
	ErrorExit("unexpected command line arguments");
    const char* tmp = getenv("QUERY_STRING");

    if (!tmp)
	ErrorExit("QUERY_STRING environment variable missing");

    try
    {
        char* queryString = strcpy(new char[strlen(tmp) + 1], tmp);

	


	Logger::put( Logger::TRACE_LOG,  "CGIClient", Logger::INFORMATION,
	    "Query String $0", queryString);

	CGIQueryString qs(queryString);

	// Test for debug display. Note needs keyword test.
	String debug = String::EMPTY;
	//if ((tmp = qs.findValue("debug")))
	{
	    cout << "Query String " << tmp << endl;
	}

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
        //else if (strcmp(operation, "SetQualifier") == 0)
	//    SetQualifier(qs);
	//else if (strcmp(operation, "DeleteQualifier") == 0)
	//    DeleteQualifier(qs);
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
	else if (strcmp(operation, "EnumerateNameSpaces") == 0)
	    EnumerateNameSpaces(qs);
	else if (strcmp(operation, "ReferenceNames") == 0)
	    ReferenceNames(qs);
	else if (strcmp(operation, "AssociatorNames") == 0)
	    AssociatorNames(qs);


	else if (strcmp(operation, "ClassInheritance") == 0)
	    ClassInheritance(qs);
        else if (strcmp(operation, "ClassTree") == 0)
	    ClassTree(qs);

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
