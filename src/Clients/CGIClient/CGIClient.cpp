//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company
//                   (carolann_graves@hp.com)
//               Amit K Arora (amita@in.ibm.com) for Bug# 1081 (mofFormat())
//               Alagaraja Ramasubramanian (alags_raj@in.ibm.com)
//               David Dillard, VERITAS Software Corp.
//                   (david.dillard@veritas.com)
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
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Common/CIMMethod.h>

#include "CGIQueryString.h"

#define PEGASUS_SINT64_MIN (PEGASUS_SINT64_LITERAL(0x8000000000000000))
#define PEGASUS_UINT64_MAX PEGASUS_UINT64_LITERAL(0xFFFFFFFFFFFFFFFF)

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
KSREVIEW: Under Construction
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
//      hostinfo.getHostName(), hostinfo.getHostPort());


void HostInfo::setHostName( const char* str)
{
  ///KSREVIEW: to be done.
}

const char* HostInfo::getAddress()
{
    const char* tmp = getenv("QUERY_STRING");
    char* queryString = strcpy(new char[strlen(tmp) + 1], tmp);
    CGIQueryString qs(queryString);

    if ((tmp = qs.findValue("hostaddress")))
    return tmp;
    else
    return("localhost:5988");
}

void getHostAndPort (String & host, Uint32 & portNumber)
{
    const char* tmp = getenv ("QUERY_STRING");
    char* queryString = strcpy (new char [strlen (tmp) + 1], tmp);
    CGIQueryString qs (queryString);

    String tmpStr= qs.findValue ("hostaddress");

    if (tmpStr.size () > 0)
    {
        Uint32 index = tmpStr.find (':');
        host = tmpStr.subString (0, index);
        portNumber = 0;
        if (index != PEG_NOT_FOUND)
        {
            String portStr = tmpStr.subString (index + 1, tmpStr.size ());
            sscanf (portStr.getCString (), "%u", &portNumber);
        }
    }
    else
    {
        host = "localhost";
        portNumber = 5988;
    }
}

CIMName PrintSuperClassName(CIMName superClassName)
{
    if (superClassName.isNull ())
    return CIMName ("No_Superclass");
    else
    return superClassName;
}

void PrintRule()
{
    cout << "<hr></hr>";
}


void PrintHead(const String& title)
{
    cout << "<head>\n";
    cout << "  <title>" << title << "</title>\n";
    cout << "</head>\n";
}

/** PrintHeader - Print the HTML banner. THis is a table with
    the text definition for the page and the TOG logo.
    @param - Text for the title
*/
void PrintHeader(const String& title)
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
void PrintHTMLHead(const String& title, const String& header)
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
    cout << "    <h1>" << "CIM Error" << "</h1>\n";
    cout << "    <p><B>" << " Error Message " << "</B></p>\n";
    cout << "    <p>" << message << "</p>\n";

    //KSREVIEWKS: Define standard ender instead of this special
    cout << "  </body>\n";
    cout << "</html>\n";
    exit(1);
}

CIMNamespaceName GetNameSpaceQueryField(const CGIQueryString& qs)
{
    const char* tmp;

    if (!(tmp = qs.findValue("NameSpace")))
    ErrorExit("Missing NameSpace field");

    String nameSpace = tmp;

    if (!nameSpace.size())
    ErrorExit("NameSpace parameter is null");

    return CIMNamespaceName (nameSpace);
}

String EncodeQueryStringValue(const String& x)
{
    String result;

    for (Uint32 j = 0, m = x.size(); j < m; j++)
    {
    //ALAGS : Typecast to avoid compiler warning
    char c = (char)x[j];

    if (c == '/')
        result.append("%2F");
    else if  (c == ':')
        result.append("%3A");
    else
        result.append(c);
    }

    return result;
}
/** PrintAHref - Prints a single href
    @param href - the reference for building the href
    @param content - The content of the reference
*/
void PrintAHref(const String& href, const String& content)
{
    cout << "<a href=\"" << href << "\">\n" << content << "</a>\n";
}
/* just prints HTML form spaces. Count is number of spaces
*/
void PrintSpaces(Uint32 count)
{
    for (Uint32 i = 0; i < count; i++)
    {
        cout << "&nbsp;";
    }
}
void PrintTableHeader(const String& tableName)
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
void PrintPropertiesTableHeader(const String& tableName)
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
//KSREVIEW: Why do we not do the EncodeQueryStringValue
//KSREVIEW: Why is host address part of this?
String BuildOperationHref(String operation, const CIMNamespaceName & nameSpace)
{

    String result = "/pegasus/cgi-bin/CGIClient?";
    result.append("Operation=");
    result.append(operation);
    result.append("&");
    result.append("NameSpace=");
    result.append(EncodeQueryStringValue(nameSpace.getString()));
    result.append("&");
    HostInfo hostinfo;
    result.append("hostaddress=");
    result.append(EncodeQueryStringValue(hostinfo.getAddress()));
    result.append("&");
    return(result);
}

/** AppendHrefField - Install a single key/value pair in an href.
KSREVIEW: this should have been an object href with the constructors, etc.
Constructs the string key=value& and appends to the href.
*/
String AppendHrefField(String& href, String key, String value)
{
    href.append(key);
    href.append("=");
    href.append(value);
    href.append("&");
    return href;
}
/** createClassHref - Builds a HTML href from the class name input to provide
    click to the defined class

*/
String createGetClassHref(const CIMNamespaceName & nameSpace,
    const CIMName & className)
{
    String href = BuildOperationHref("GetClass", nameSpace);

    href.append("ClassName=");
    href.append(className.getString());
    href.append("&");
    // KSREVIEWKS: The following is probably duplicate.  See BuildOperationHref above
    HostInfo hostinfo;
    href.append("hostaddress=");
    href.append(EncodeQueryStringValue(hostinfo.getAddress()));
    href.append("&");

    return(href);
}
/** PrintEnumInstanceNameHref - Put out a link to the enumerate
instance Names function for this class.
@return - void
*/
void PrintEnumInstanceNameHref(const CIMNamespaceName & nameSpace,
    const CIMName & className)
{
    String href = BuildOperationHref("EnumerateInstanceNames", nameSpace);
    AppendHrefField(href,"ClassName",className.getString());

    PrintAHref(href, "<BOLD Align=right>Instance Names</BOLD>");

}


void PrintTableTrailer()
{
    cout << "</table>\n";
}

/** PrintRow - Prints a single table row with 3 entires
    The entries are name, type, value
*/
void PrintRow(
    const CIMName& name,
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

void PrintLogo()
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
    cimTypeToString (value.getType ()),
    value.toString());

    PrintTableTrailer();
}
/** PrintObjectProperties - Template for a function
that prints the Properties information for either Classes
or instances.    This prints an HTML table of the properties
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
    const CIMNamespaceName& nameSpace,
    OBJECT& object,
    Boolean includeClassOrigin)
{
    PrintPropertiesTableHeader("Properties:");

    // Loop for each property
    for (Uint32 i = 0, n = object.getPropertyCount(); i < n; i++)
    {
    // KSREVIEW: All of this can become PrintProperty()
    CIMProperty property = object.getProperty(i);
    const CIMValue& value = property.getValue();

    // Define href with the property name
    //ALAGS : FIX done - call func BuildOperationHref() with correct arguments
    String href =
            BuildOperationHref("GetPropertyDeclaration", nameSpace);
    href.append("ClassName=");
    href.append(object.getClassName().getString());
    href.append("&");
    href.append("PropertyName=");
    href.append(property.getName().getString());
    href.append("&");
    cout << "<tr>\n";
    cout << "<td>";
    PrintAHref(href, property.getName().getString());
    cout << "</td>";
    cout << "<td>" << cimTypeToString (value.getType ()) << "</td>\n";

    String valueString = value.toString();

    if (valueString.size())
        cout << "<td>" << valueString << "</td>\n";
    else
        cout << "<td>null</td>\n";
    // Output the ClassOrigin
    // KSREVIEW: Make this optional
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
        cimTypeToString (value.getType ()),
        value.toString());
    }

    PrintTableTrailer();
}
/** Prepare an HTML table with a header and an entry
    for each method defined in the class with the CIMName
    and type of the CIMMethod in each entry
    @param cimClass - Class for which methods to be output
    KSREVIEWKS: this is here temporarily.  Should be moved to more
    common client library so generally available
*/

void mofFormat(
    PEGASUS_STD(ostream)& os,
    const char* text,
    Uint32 indentChars)
{
    char* var = new char[strlen(text)+1];
    char* tmp = strcpy(var, text);

    //const char* tmp = x.getData();
    Uint32 count = 0;
    Uint32 indent = 0;
    Boolean quoteState = false;
    char c;
    while ((c = *tmp++))
    {
    count++;
    switch (c)
    {
        case '\n':
        os << Sint8(c);
        count = 0;
        indent = 0;
        break;

        case '\"':   // quote
        os <<Sint8(c);
        quoteState = !quoteState;
        break;

        case ' ':
        os <<Sint8(c);
        if (count > 70)
        {
            if (quoteState)
            os <<"\"\n    \"";
            else
            os <<"\n    ";
            count = 0 - indent;
        }
        break;

        default:
        os <<Sint8(c);
    }

    }
    delete [] var;
}

/** PrintClassMethods - Print a table of the methods for the class
*/
void PrintClassMethods(CIMClass& cimClass)
{
    // KSREVIEW:If there are no methods.  State that rather than empty table
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
    cout << "<td>" << cimTypeToString (type) << "</td>\n";
    cout << "<tr>\n";
    }

    cout << "</table>\n";
}

/** PrintClass - Print an HTML page with the characteristics of
    the Class defined in the call including:
    <UL>
    <LI>ClassName
        <LI> SuperClass Name
    <LI>Qualifiers
    <LI>Properties
    <LI>Methods
    <UL>
    @param CIMNamespaceName with nameSpace
    @param pointer to class
    @param localOnly
    @param includeQualifiers
    @param includClassOrigin
    @param showMof
*/
void PrintClass(
    const CIMNamespaceName& nameSpace,
    CIMClass& cimClass,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    Boolean showMof)
{
    //KSREVIEW: Should say GetClass and then classname
    //Argument for combining to one parm
    PrintHTMLHead("GetClass", cimClass.getClassName().getString());

    //Consider short table of request parameters

    cout << "\n<B>SuperClass Name:  </b>";
    if (cimClass.getSuperClassName().isNull())
    cout << "No Super Class" <<endl;
    else
    {
    //cout << cimClass.getSuperClassName() << endl;
    // output the superclass name as an href
        String href = createGetClassHref(nameSpace,
                                  cimClass.getSuperClassName());

        // KSREVIEWKS (localOnly)?"  ": " ";
        // KSREVIEWKS (includeQualifiers)? "%IncludeQualifiers=true" : "";
    PrintAHref(href, cimClass.getSuperClassName().getString());
        //ANNTTNKS 26 Jan 2002 - Add the options params to this call

        //ALAGS : commented statement below as it is of no use
        //String hrefenumInstance =
        //    BuildOperationHref("enumerateInstanceNames", nameSpace);

    }

    // Output the href so user can get to instance names in one click

    PrintSpaces(4);
    //ALAGS : commented Statement below
    //PrintEnumInstanceNameHref(nameSpace, cimClass.getClassName());

    if (includeQualifiers)
    PrintQualifiers(cimClass);
    // KSREVIEWKS: Delete this optionelse
    //    cout << "\n\nQualifier Parameters Not Requested" << endl;

    PrintObjectProperties(nameSpace, cimClass,includeClassOrigin);
    PrintClassMethods(cimClass);
    if (showMof)
    {
    cout << "<h2>Display MOF for Class " << cimClass.getClassName() << "</h2>";
    cout << "<pre>";
    Array<char> x;
    MofWriter::appendClassElement(x, cimClass);
    x.append('\0');

    mofFormat(cout, x.getData(), 4);

    //MofWriter::printClassElement(cimClass);
    cout << "</pre>";

        // Now show the XML for this entity
        /* note that XML formatting is real mess.  Hard to reformat as HTML
        without cleaning up the XML tags, etc.
        cout << "\n<h2>XML</h2>\n";

        cout << "<pre>";
        XmlWriter::printClassElement(cimClass);
        cout << "</pre>\n";  */
    }
    cout << "</body>\n" << "</html>\n";
}
/* PrintInstanceBody - Prints the body of an instance without
the headers and trailers.  Needed because we use both for getInstance
and EnumerateInstance.
*/
void PrintInstanceBody(
const CIMNamespaceName& nameSpace,
    CIMInstance& cimInstance,
    Boolean localOnly)
{
    PrintQualifiers(cimInstance);
    PrintObjectProperties(nameSpace, cimInstance, localOnly);
}
/** PrintInstance - Print an HTML page with the instance and its properties, etc.
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
const CIMNamespaceName& nameSpace,
    CIMInstance& cimInstance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PrintHTMLHead("GetInstance", cimInstance.getClassName().getString());
    PrintInstanceBody(nameSpace, cimInstance,localOnly);
    //KSREVIEWKS: Add showMof here
    cout << "<h2>Display MOF for Instance</h2>";
    cout << "<pre>";
    Array<char> x;
    MofWriter::appendInstanceElement(x, cimInstance);
    x.append('\0');

    mofFormat(cout, x.getData(), 4);


    cout << "</body>\n" << "</html>\n";
}

void PrintPropertyDeclaration(CIMProperty& property)
{
    PrintHTMLHead("GetPropertyDeclaration", property.getName().getString());

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
void GetClass(const CGIQueryString& qs)
{

    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);
    DDD(cout << "GetClass" << endl;)
    // Get ClassName:
    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
    ErrorExit("Missing ClassName field");

    CIMName className = CIMName (tmp);

    if (className.isNull())
    ErrorExit("ClassName parameter is null");

    // Process Checkbox items that become call opptions
    // Set the Defaults
    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;
    Boolean showMof = true;

    // Process possible input fields
    // Wierd because the form entry only sends info if
    //
    if (!(tmp = qs.findValue("LocalOnly")))
    localOnly = false;
    if (!(tmp = qs.findValue("IncludeQualifiers")))
    includeQualifiers = false;
    if ((tmp = qs.findValue("IncludeClassOrigin")))
    includeClassOrigin = true;
    // KSREVIEW: KS - Just delete thisif ((tmp = qs.findValue("ShowMof")))
    //    showMof = true;

    CIMClass cimClass;
    try
    {
    CIMClient client;

        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

        cimClass = client.getClass(nameSpace, className,
        localOnly, includeQualifiers, includeClassOrigin);

        PrintClass(nameSpace, cimClass,localOnly, includeQualifiers,
        includeClassOrigin, showMof);
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
void GetPropertyDeclaration(const CGIQueryString& qs)
{
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:

    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
    ErrorExit("Missing ClassName field");

    CIMName className = CIMName (tmp);

    if (className.isNull())
    ErrorExit("ClassName parameter is null");

    // Get PropertyName:

    if (!(tmp = qs.findValue("PropertyName")))
    ErrorExit("Missing ClassName field");

    CIMName propertyName = CIMName (tmp);

    if (propertyName.isNull())
    ErrorExit("PropertyName parameter is null");

    //

    try
    {
    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

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
/** PrintClassNames  - Generates a table with the class names
*/
void PrintClassNames(
    const CIMNamespaceName& nameSpace,
    const Array<CIMName>& classNames,
    double elapsedTime)
{
    PrintHTMLHead("GetClassNames", "EnumerateClassNames Result");

    cout << "<table border=1>\n";
    cout << "<tr><th>Class Names</th><tr>\n";

    for (Uint32 i = 0, n = classNames.size(); i < n; i++)
    {
        cout << "<tr><td>\n";

        String href = createGetClassHref(nameSpace,
                                    classNames[i]);

        PrintAHref(href, classNames[i].getString());
            cout << "&nbsp;&nbsp;&nbsp;&nbsp;";

            // Build href for enumerate instance names
            PrintEnumInstanceNameHref(nameSpace,classNames[i]);

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
void EnumerateClassNames(const CGIQueryString& qs)
{
    // Get NameSpace:
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    CIMName className;

    DDD(cout << "EnumerateClassNames" << endl;)

    const char* tmp;

    // Get the ClassName field:
    if ((tmp = qs.findValue("ClassName")) && *tmp)
    className = CIMName (tmp);

    // Get DeepInheritance:
    Boolean deepInheritance = false;

    if (qs.findValue("DeepInheritance"))
    deepInheritance = true;

    // Invoke the method:
    try
    {
    // Time the connection
    Stopwatch elapsedTime;

    elapsedTime.start();

    // Make the Connection
    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    Array<CIMName> classNames = client.enumerateClassNames(
        nameSpace, className, deepInheritance);

    elapsedTime.stop();

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
void DeleteClass(const CGIQueryString& qs)
{
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    const char* tmp;

    if (!(tmp = qs.findValue("ClassName")))
    ErrorExit("Missing ClassName field");

    CIMName className = CIMName (tmp);

    if (className.isNull())
    ErrorExit("ClassName parameter is null");

    try
    {
    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    client.deleteClass(nameSpace, className);

    String message = "Class \"";
    message.append(className.getString());
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

void PrintQualifierRow(const CIMNamespaceName& nameSpace,
               const CIMQualifierDecl& qd)
{
    cout << "<tr>\n";

    const CIMValue& value = qd.getValue();

    String href =
        BuildOperationHref("GetQualifier", nameSpace);
    href.append("QualifierName=");
    href.append(qd.getName().getString());
    href.append("&");

    cout << "<td>";
    PrintAHref(href, qd.getName().getString());
    cout << "</td>";
    cout << "<td>" << cimTypeToString (value.getType ()) << "</td>\n";
    cout << "<td>" << value.toString() << "</td>\n";
    cout << "<td>" << qd.getScope ().toString () << "</td>\n";
    cout << "<td>" << MofWriter::getQualifierFlavor(qd.getFlavor()) << "</td>\n";
    cout << "<td>" << qd.getArraySize() << "</td>\n";

    cout << "</tr>\n";
}

void PrintGetQualifier(
    const CIMNamespaceName& nameSpace,
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


    // Now show the XML and the MOF for this entity

    cout << "\n<h2>MOF</h2>\n";
    cout << "<pre>";
    Array<char> x;
    MofWriter::appendQualifierDeclElement(x, qualifierDecl);
    x.append('\0');
    mofFormat(cout, x.getData(), 4);
    cout << "</pre>\n";

    // Now show the XML for this entity

    cout << "\n<h2>XML</h2>\n";

    cout << "<pre>";
    XmlWriter::printQualifierDeclElement(qualifierDecl);
    cout << "</pre>\n";

    cout << "</body>\n";
    cout << "</html>\n";

}

void PrintEnumerateQualifiers(
    const CIMNamespaceName& nameSpace,
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
void EnumerateQualifiers(const CGIQueryString& qs)
{
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    try
    {
    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

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
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get CIMQualifier name:
    const char* tmp;

    if (!(tmp = qs.findValue("QualifierName")))
    ErrorExit("Missing QualifierName field");

    CIMName qualifierName = CIMName (tmp);

    if (qualifierName.isNull())
    ErrorExit("QualifierName parameter is null");

    try
    {
    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    CIMQualifierDecl qd = client.getQualifier(nameSpace, qualifierName);

    PrintGetQualifier(nameSpace, qd);
    }
    catch(Exception& e)
    {
    ErrorExit(e.getMessage());
    }
}

 /***************************************************************************
   DeleteQualifier Function
***************************************************************************/

/** DeleteQualifier - Deletes the Qualifier defined on input
*/
void DeleteQualifier(const CGIQueryString& qs)
{
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get CIMQualifier name:
    const char* tmp;

    if (!(tmp = qs.findValue("QualifierName")))
    ErrorExit("Missing QualifierName field");

    CIMName qualifierName = CIMName (tmp);

    if (qualifierName.isNull())
    ErrorExit("QualifierName parameter is null");

    try
    {
    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    client.deleteQualifier(nameSpace, qualifierName);

    String message = "Qualifier \"";
    message.append(qualifierName.getString());
    message.append("\" was deleted");
    PrintHTMLHead("DeleteQualifier", "Delete Qualifier Result");
        cout << "    <h1>" << message << "</h1>\n";
        cout << "  </body>\n";
        cout << "</html>\n";
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
    are provided as an array of CIMObjectPath.
    The table created includes an href for each name so that
    a click on the href entry will get the instance
    Note that we assume the defaults for the extra
    parameters on the getInstance
    @param nameSpace
    @param InstanceNames - The array of references to print
    KSREVIEW: Change so the user can select extra params.

    ALAGS : Added last parameter isClass to decide on href for
            Associatior/Reference Names and to avoid a new method
*/
static void PrintObjectNames(
    const String& header,
    const CIMNamespaceName& nameSpace,
    const Array<CIMObjectPath>& instanceNames,
    double elapsedTime,
    bool isClass )
{
    PrintHTMLHead("GetInstanceNames", header);

    cout << "<table border=1>\n";
    cout << "<tr><th>Object Names</th><tr>\n";
    // For each name prepare the table entry with an href for
    // click access to the getInstance for that name
    for (Uint32 i = 0, n = instanceNames.size(); i < n; i++)
    {
        cout << "<tr><td>\n";
        //ALAGS : Decide if href is to call GetClass or GetInstance
        //        This would be needed in Associator/Reference Name calls
        //        Set this parameter to "true" if the ObjectName textbox is
        //        passing a Class
        String href;
        if(isClass)
        {
            href = BuildOperationHref("GetClass", nameSpace);
            href.append("ClassName=");
            href.append(instanceNames[i].getClassName().getString());
        }
        else
        {
            href = BuildOperationHref("GetInstance", nameSpace);
            href.append("InstanceName=");

            //
            // KSREVIEWKS: - Need to convert the '"' (double quote) character to '%22' to
            // make it a valid URL string in the HTML tag.  Also, need to
            // convert the '_' (underscore) character in the instanceName
            // string to a '.' (dot).
            //

            //ALAGS : No need to convert '-' to '.' as a '-' is already a valid
            // character in a URL string. A underscore '_' must not be converted
            // as it would affect all CIM_xxx classes.

            const String instanceName = instanceNames[i].toString();
            String nameString;


            for (Uint32 j = 0, n = instanceName.size(); j < n; j++)
            {
                switch (instanceName[j])
                {
                    /*ALAGS: Commented to fix Bug #148
                    case '-':
                        nameString.append(".");
                        break;*/

                    case '"':
                        nameString.append("%22");
                        break;

                    default:
                        nameString.append(instanceName[j]);
                }
            }


            href.append(nameString);
            href.append("&");

            href.append("LocalOnly=true");
            href.append("includQualifiers=false");
            href.append("includeClassOrigin=false");

        }

        PrintAHref(href, instanceNames[i].toString());

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
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    CIMName className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
    className = CIMName (tmp);

    // Invoke the method:

    try
    {
    // Time the connection
    Stopwatch elapsedTime;

    elapsedTime.start();

    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    // Call enumerate Instances CIM Method
    Array<CIMObjectPath> instanceNames = client.enumerateInstanceNames(
        nameSpace, className);

    elapsedTime.stop();

    // Print the CIMObjectPath array
    PrintObjectNames( "EnumerateInstanceNames Result",
        nameSpace, instanceNames, elapsedTime.getElapsed(), false);
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
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get InstanceName:
    const char* tmp;

    if (!(tmp = qs.findValue("InstanceName")))
    ErrorExit("Missing InstanceName field");

    // KSREVIEWKS: This must be modified for the toString
    CIMObjectPath referenceName;
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

    // KSREVIEWKS: Test the following!

    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    if (!(tmp = qs.findValue("LocalOnly")))
    localOnly = false;
    if (!(tmp = qs.findValue("IncludeQualifiers")))
    includeQualifiers = false;
    if ((tmp = qs.findValue("IncludeClassOrigin")))
    includeClassOrigin = true;

    try
    {
    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    CIMInstance cimInstance = client.getInstance(nameSpace,
        referenceName, localOnly, includeClassOrigin, includeClassOrigin);

    PrintInstance(nameSpace, cimInstance, localOnly, includeQualifiers,
        includeClassOrigin); }
    catch(Exception& e)
    {
    ErrorExit(e.getMessage());
    }
}


/** PrintInstancetable - Print a table of enumerated instance information
*/
void PrintInstanceTableRow(const CIMNamespaceName & nameSpace,
    CIMInstance CIMInstance)
{
   // KSREVIEWKS: Need to add code here
    // Loop for each property

    String className = CIMInstance.getClassName().getString();

    for (Uint32 i = 0, n = CIMInstance.getPropertyCount(); i < n; i++)
    {
        // KSREVIEW: All of this can become PrintProperty()
        CIMProperty property = CIMInstance.getProperty(i);
        const CIMValue& value = property.getValue();


        String href =
            BuildOperationHref("GetPropertyDeclaration", nameSpace);
        href.append("ClassName=");
        href.append(className);
        href.append("&");
        href.append("PropertyName=");
        href.append(property.getName().getString());
        href.append("&");
        cout << "<tr>\n";
        cout << "<td>";
        PrintAHref(href, property.getName().getString());
        cout << "</td>";



        /*cout << "<tr>\n";
        cout << "<td>";
        cout << property.getName().getString();
        cout << "</td>";*/
        cout << "<td>" << cimTypeToString (value.getType ()) << "</td>\n";

        String valueString = value.toString();

        if (valueString.size())
            cout << "<td>" << valueString << "</td>\n";
        else
            cout << "<td>null</td>\n";
        // Output the ClassOrigin
        // KSREVIEW: Make this optional
        cout << "<td>" << property.getClassOrigin() << "</td>\n";
        // Output the Propagated field
        cout << "<td>" << (property.getPropagated() ? "true" : "false");
        cout << "</td>\n";

        cout << "</tr>\n";
    }

}
/***************************************************************************
    EnumerateInstances Function
***************************************************************************/
static void EnumerateInstances(const CGIQueryString& qs)
{
 // Get NameSpace:
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    CIMName className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
    className = CIMName (tmp);

    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    if (!(tmp = qs.findValue("LocalOnly")))
    localOnly = false;
    if (!(tmp = qs.findValue("IncludeQualifiers")))
    includeQualifiers = false;
    if ((tmp = qs.findValue("IncludeClassOrigin")))
    includeClassOrigin = true;

    // Get DeepInheritance:
    Boolean deepInheritance = false;

    if (qs.findValue("DeepInheritance"))
    deepInheritance = true;


    CIMPropertyList propertyList;
    //KSREVIEWKS: Fill out the property list

    // Invoke the method:
    try
    {
        // Time the connection
        Stopwatch elapsedTime;

        elapsedTime.start();

        CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);
        /*
        virtual Array<CIMInstance> enumerateInstances(
            const CIMNamespaceName& nameSpace,
            const CIMName& className,
            Boolean deepIn
            heritance = true,
            Boolean localOnly = true,
            Boolean includeQualifiers = false,
            Boolean includeClassOrigin = false,
            const CIMPropertyList& propertyList = CIMPropertyList());
        */
        Array<CIMInstance> instances = client.enumerateInstances(
                                            nameSpace,
                                            className,
                                            deepInheritance,
                                            localOnly,
                                            includeQualifiers,
                                            includeClassOrigin,
                                            propertyList);

        elapsedTime.stop();

        // Generate Table Head
        //Number of Colums is determined by number of properties
        // Name row and each property row. ?? from class or instance
        // step through properties and get name fields. Generate
        // header entries and a table so that individual entires can
        // be gotten.

        PrintHTMLHead("GetInstanceNames", "EnumerateInstances Result");

        //ALAGS : commented following block
        /*for (Uint32 i = 0, n = instances.size(); i < n; i++)
        {
        cout << "<th>xxx</th>";
        }
        cout << "</tr>\n";*/
        //Generate a Table Row for every instance found

        for (Uint32 i = 0, n = instances.size(); i < n; i++)
        {
            cout << "<table border=1>\n";
            cout << "<th colspan=5>Instance " << i+1 << "</th>\n";
            cout << "<tr>\n";
            cout << "<th>CIMName</th>\n";
            cout << "<th>CIMType</th>\n";
            cout << "<th>CIMValue</th>\n";
            cout << "<th>ClassOrigin</th>\n";
            cout << "<th>Propagated</th>\n";
            cout << "</tr>\n";
            PrintInstanceTableRow(nameSpace, instances[i]);
            cout << "</table>\n<br><br>";

            //ALAGS : MOF disp can be put into a function
            cout << "<h2>Display MOF for Instance " << i+1 << "</h2>";
            cout << "<pre>";
            Array<char> x;
            MofWriter::appendInstanceElement(x, instances[i]);
            x.append('\0');

            mofFormat(cout, x.getData(), 4);
            cout << "</pre><br><br>";
        }

        // Close the HTML Table
        //cout << "</table>\n";

        // Close the Page
        cout << "<p>Returned " << instances.size() << " Instance(s) ";
        cout << " in " << elapsedTime.getElapsed() << " Seconds</p>\n";
        cout << "</body>\n" << "</html>\n";
    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }

}

/***************************************************************************
   GetProperty Function
***************************************************************************/
 static void GetProperty(const CGIQueryString& qs)
{
  // Get NameSpace:
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    // String instanceName;
    CIMName propertyName;

    const char* inputInstanceName;
    const char* tmp;

    if (!(inputInstanceName = qs.findValue("InstanceName")))
    ErrorExit("Missing InstanceName field");

    // This must be modified for the toString KSREVIEW KS
    CIMObjectPath referenceName;

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
        propertyName = CIMName (tmp);


    try
    {
        CIMClient client;
            String host;
            Uint32 portNumber;
            getHostAndPort (host, portNumber);
            client.connect (host, portNumber, String::EMPTY, String::EMPTY);

        //ALAGS : this call does not return the correct "value type"
        // for a given property. So use the fix implemented below
        CIMValue value = client.getProperty(nameSpace,
        referenceName, propertyName);


        PrintHTMLHead("GetProperty", "GetProperty Result");


        cout << "<B>Instance = </B> " <<
        inputInstanceName << "\n<br>";



        //ALAGS : use this for getting correct property type
        // as value.getType() is not returning correct type when "value"
        // is obtained through a client.getProperty(...) call
        CIMInstance cimInstance = client.getInstance(nameSpace, referenceName);

        CIMValue value1;
        CIMProperty property;
        for (Uint32 i = 0, n = cimInstance.getPropertyCount(); i < n; i++)
        {
            property = cimInstance.getProperty(i);
            if(String::equalNoCase(property.getName().getString(),
                                   propertyName.getString()))
            {
                value1 = property.getValue();
                break;
            }
        }

        cout << "<B>Property = </B> " << property.getName().getString() << "\n\n<br>";
        cout << "<B>Value Type = </B>";

        cout <<  cimTypeToString (value1.getType ()) << "\n\n<br>";

        String valueString = value1.toString();
        cout << "<B>Value = </B> ";

        if (valueString.size())
           cout << " " << valueString << " \n\n<br>";
        else
           cout << " NULL \n\n<br>";

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
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get InstanceName:
    const char* tmp;

    if (!(tmp = qs.findValue("InstanceName")))
    ErrorExit("Missing InstanceName field");

    // KSREVIEWKS: This must be modified for the toString
    CIMObjectPath referenceName;
    CIMName propertyName;
    CIMValue newValue;
    try
    {
    referenceName = tmp;
    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }

    if ((tmp = qs.findValue("PropertyName")))
    propertyName = CIMName (tmp);

    String strValue;
    if ((tmp = qs.findValue("NewValue")))
    strValue = tmp;

    newValue = CIMValue (strValue);


    try
    {
    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    client.setProperty(nameSpace, referenceName, propertyName, newValue);

    String message = "Property \"";
    message.append(propertyName.getString());
    message.append("\" has been set to ");
    message.append(newValue.toString());
    PrintHTMLHead("SetProperty", "Set Property Result");
        cout << "    <h1>" << message << "</h1>\n";
        cout << "  </body>\n";
        cout << "</html>\n";
    }
    catch(Exception& e)
    {
    ErrorExit(e.getMessage());
    }

}

/***************************************************************************
   DeleteInstance Function
***************************************************************************/
static void DeleteInstance(const CGIQueryString& qs)
{
    // Get NameSpace:
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get InstanceName:
    const char* tmp;

    if (!(tmp = qs.findValue("InstanceName")))
    ErrorExit("Missing InstanceName field");

    CIMObjectPath referenceName;
    try
    {
        referenceName = tmp;
    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }

    try
    {
        // Time the connection
        Stopwatch elapsedTime;

        elapsedTime.start();

        CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

        client.deleteInstance(nameSpace, referenceName);

        elapsedTime.stop();

        PrintHTMLHead("DeleteInstance", "Delete an Instance Result");

        cout << "<h1>Instance " << referenceName.toString() << " Deleted</H1>";
        cout << " in " << elapsedTime.getElapsed() << " Seconds</p>\n";
        cout << "</body>\n" << "</html>\n";

        //PrintInstance(nameSpace, cimInstance, localOnly, includeQualifiers,
        //includeClassOrigin);
    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }

}

/***************************************************************************
   CreateNameSpace Function
***************************************************************************/
static void CreateNameSpace(const CGIQueryString& qs)
{
  // Get NameSpace:
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get nameSpace Name:
    CIMNamespaceName nameSpaceName;
    CIMName className;

    const char* tmp;

    if ((tmp = qs.findValue("NewNameSpace")))
    nameSpaceName = CIMNamespaceName (tmp);

    // Create the instance with full instancename since this is the key.
    String instanceName = "__Namespace";

    //ALAGS : FIX - No need to append property "name" while creating
    // Instance here. CIMInstance constructor failed when this was done earlier
    /*instanceName.append( ".name=\"");
    instanceName.append(nameSpaceName.getString());
    instanceName.append("\"");*/


    CIMInstance newInstance(instanceName);

    newInstance.addProperty(CIMProperty(CIMName ("name"),
        nameSpaceName.getString()));
    try
    {
    // Time the connection
    Stopwatch elapsedTime;

    elapsedTime.start();

    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    // Call create Instances CIM Method for class __Namespace
    cout << "Creating " << nameSpaceName;
    client.createInstance(nameSpace, newInstance);

    elapsedTime.stop();

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
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get NameSpaceName to delete:
    CIMNamespaceName nameSpaceToDelete;
    const char* tmp;
    if ((tmp = qs.findValue("DeletionNameSpace")))
    nameSpaceToDelete= CIMNamespaceName (tmp);

    // Create Instance Name
    String instanceName = "__Namespace.name=\"";
    instanceName.append(nameSpaceToDelete.getString());
    instanceName.append("\"");

    // Create Instance Reference. Name must be in form Reference
    CIMObjectPath referenceName;
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

    elapsedTime.start();

    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    // Call delete Instances CIM Method for class __Namespace
    client.deleteInstance(nameSpace, referenceName);

    elapsedTime.stop();

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
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    //CIMNamespaceName nameSpaceName;
    CIMName className;

    const char* tmp;

    if ((tmp = qs.findValue("ClassName")))
    className = CIMName (tmp);

    // Invoke the method:
    try
    {
    // Time the connection
    Stopwatch elapsedTime;

    elapsedTime.start();

    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    // Call enumerate Instances CIM Method
    Array<CIMObjectPath> instanceNames = client.enumerateInstanceNames(
        nameSpace, className);

    elapsedTime.stop();

    // Convert from CIMObjectPath to String form
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
        work = tmpInstanceNames[i].subString((pos+1), PEG_NOT_FOUND);

        // remove trailing quote
        work.remove((work.size() - 1), PEG_NOT_FOUND);

        // Create href for click to get classnames

        //ALAGS : FIX - Prefix "root/" to the Namespace Name for link to work
        CIMNamespaceName nameSpaceName("root/"+work);

        String href = BuildOperationHref("EnumerateClassNames",nameSpaceName);


        href.append("InstanceName=&");
        href.append("DeepInheritance=true");

        PrintAHref(href, work);

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
void PrintClassTreeEntry(const CIMNamespaceName& nameSpace,
             const CIMName& className,
             Uint32 level)
{
    cout << "<LI>";
    cout << level;
    cout << " ";

    //KSREVIEW figure out why clasName in href and printaHref.
    String href = createGetClassHref(nameSpace,
                          className);

    href.append("LocalOnly=true");


    PrintAHref(href, className.getString());

}
/***************************************************************************
   TraverseClassTree Function
***************************************************************************/

/** TraverseClassTree - Traverse the Tree of super-to-subclasses
    printing each new subclass. This function uses recursieve calls
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
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const Array<CIMName>& superClassNames,
    const Array<CIMName>& classNames,
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

           cout << "&nbsp;&nbsp;&nbsp;";

           // Build href for enumerate instance names
           PrintEnumInstanceNameHref(nameSpace,classNames[i]);

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
    KSREVIEW: In reality this is the enumerate class itself and should be
    merged with that.

*/
static void ClassInheritance(const CGIQueryString& qs)
{
    // Get NameSpace:
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Get ClassName:
    CIMName className;

    const char* tmp;

    // Get the timeout that is an option on this command
    Uint32 timeOut = 5 * 1000;
    if ((tmp = qs.findValue("timeout")))
    timeOut = 60 * 1000;
    // KSREVIEW: Need the ascitointeger function.


    // Get the ClassName field:
    if ((tmp = qs.findValue("ClassName")) && *tmp)
    className = CIMName (tmp);

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

    elapsedTime.start();

    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);
    client.setTimeout(timeOut);


    Array<CIMClass> classArray = client.enumerateClasses(
                    nameSpace,
                    className,
                    deepInheritance,
                    localOnly,
                    includeQualifiers,
                    includeClassOrigin);
    // Organize and Print the Class Tree results

    elapsedTime.stop();

    PrintHTMLHead("EnumerateInheritance", "Class Inheritance Result");

    cout << "<table border=1>\n";
    cout << "<tr><th>Super Class Names<th>Class Names<tr></th>\n";

    String classNameHrefBuilder;
    cout << "Count " <<   classArray.size() << endl;

    for (Uint32 i = 0, n = classArray.size(); i < n; i++)
    {
        cout << "<tr><td>\n";

        if (classArray[i].getSuperClassName().isNull ())
        cout << "No Superclass";
        else
        cout << classArray[i].getSuperClassName();

        cout << "<td>\n";

        String href = createGetClassHref(nameSpace,
                              classArray[i].getClassName());

        href.append("LocalOnly=true");



        PrintAHref(href, classArray[i].getClassName().getString());

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
    const CIMName rootClass;
    // Get NameSpace:
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Invoke the method:
    try
    {
    // Time the connection
    Stopwatch elapsedTime;

    // Make the Connection
    Boolean deepInheritance = true;
    CIMName className;

    elapsedTime.start();

    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    Array<CIMName> classNames = client.enumerateClassNames(
                    nameSpace,
                    className,
                        deepInheritance);

    // Organize and Print the Class Tree results

    PrintHTMLHead("EnumerateClassTree", "Enumerate Classes Tree Result");


    Boolean localOnly = true;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = true;
    CIMClass myClass;
    Array<CIMName> superClassNames;

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

    elapsedTime.stop();

    cout << "Count of Classes Enumerated = " <<   classNames.size() << endl;
    cout << " in " << elapsedTime.getElapsed() << " Seconds</p>\n";

    elapsedTime.reset();

    elapsedTime.start();

    TraverseClassTree(nameSpace, rootClass, superClassNames,
                classNames,classNames.size(),PEG_NOT_FOUND);

    elapsedTime.stop();

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

/************************************************************************
    AllInstances

************************************************************************/

/** AllInstances - Show the classes that have existing instances
and a count of the instances for the class. Includes a hyperlink to the
enumaterate Instance Names for the instances.
*/
static void AllInstances(const  CGIQueryString& qs)
{
    // Get NameSpace:
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    // Invoke the method:
    try
    {
    // Make the Connection
    Boolean deepInheritance = true;
    CIMName className;

    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    Array<CIMName> classNames = client.enumerateClassNames(
                    nameSpace,
                    className,
                    deepInheritance);

    PrintHTMLHead("EnumerateAllInstances", "Enumerate Classes with Instances");


        Array<CIMObjectPath> instanceNames;

        for (Uint32 i = 0; i < classNames.size(); i++)
        {
            //ALAGS : Querying CIM_NameSpace class is not allowed presently !
            // So avoid that class alone. Remove check after this is rectified
            if(!String::equalNoCase(classNames[i].getString(),"CIM_NameSpace"))
            {
                instanceNames = client.enumerateInstanceNames(nameSpace,classNames[i]);
                if (instanceNames.size() > 0)
                {
                    cout << "Class " << classNames[i] << " "
                        << instanceNames.size() << " Instances" << endl;
                    //KSREVIEWKS: Link to the instances  Expand for keyword in Href.
                    PrintEnumInstanceNameHref(nameSpace,classNames[i]);
                    cout << "<br>" << endl;
                }
            }
        }
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
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    const char* tmp;

    String objectName;
    bool isClass = false;
    if ((tmp = qs.findValue("ObjectName")))
    {
        String objName = tmp;
        if(objName.find('.') == PEG_NOT_FOUND)
            isClass = true;  //Value in ObjectName textfield is a Class
        else
            isClass = false; //Value in ObjectName textfield is InstanceName

        objectName = tmp;
    }

    CIMName resultClass;
    if ((tmp = qs.findValue("ResultClass")))
    {
        String tempRC = tmp;
        if(String::equalNoCase(tempRC, "NULL"))
            resultClass = CIMName ();
        else
            resultClass = CIMName (tempRC);
    }

    String role = String::EMPTY;
    if ((tmp = qs.findValue("Role")))
    role = tmp;

    // Invoke the method:
    try
    {
    // Time the connection
    Stopwatch elapsedTime;

    elapsedTime.start();

    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    Array<CIMObjectPath> objectReferences;
    objectReferences = client.referenceNames(
            nameSpace,
            objectName,
            resultClass,
            role);

    elapsedTime.stop();

    // Generate table of returned CIMObjectPath
    // Similar to Instance names response.
    // Print the CIMObjectPath array
    PrintObjectNames( "EnumerateReferenceNames Result",
        nameSpace, objectReferences, elapsedTime.getElapsed(), isClass);


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
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);

    const char* tmp;

    //Finish this one.
    CIMObjectPath objectNameRef;
    String objectName;
    bool isClass = false;
    if ((tmp = qs.findValue("ObjectName")))
    {
        String objName = tmp;
        if(objName.find('.') == PEG_NOT_FOUND)
            isClass = true;  //Value in ObjectName textfield is a Class
        else
            isClass = false; //Value in ObjectName textfield is InstanceName

        objectName = tmp;
    }

    CIMName assocClass = CIMName ();
    if ((tmp = qs.findValue("AssocClass")))
      assocClass = CIMName (tmp);


    CIMName resultClass;
    if ((tmp = qs.findValue("ResultClass")))
    {
        String tempRC = tmp;
        if(String::equalNoCase(tempRC, "NULL"))
            resultClass = CIMName ();
        else
            resultClass = CIMName (tempRC);
    }



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

    elapsedTime.start();

    CIMClient client;
        String host;
        Uint32 portNumber;
        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

    Array<CIMObjectPath> objectReferences;
    objectReferences = client.associatorNames(
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole);

    elapsedTime.stop();

    // Generate table of returned CIMObjectPath
    // Similar to Instance names response.

    PrintObjectNames( "EnumerateAssociationNames Result",
        nameSpace, objectReferences, elapsedTime.getElapsed(), isClass);

    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }
  }

inline Uint8 hexCharToNumeric(const char c)
{
    Uint8 n;

    if (isdigit(c))
        n = (c - '0');
    else if (isupper(c))
        n = (c - 'A' + 10);
    else // if (islower(c))
        n = (c - 'a' + 10);

    return n;
}

static Boolean stringToUnsignedInteger(
    const char* stringValue,
    Uint64& x)
{
    x = 0;
    const char* p = stringValue;

    if (!p || !*p)
    return false;

    if (*p == '0')
    {
        if ( (p[1] == 'x') || (p[1] == 'X') )
        {
            // Convert a hexadecimal string

            // Skip over the "0x"
            p+=2;

            // At least one hexadecimal digit is required
            if (!*p)
                return false;

            // Add on each digit, checking for overflow errors
            while (isxdigit(*p))
            {
                // Make sure we won't overflow when we multiply by 16
                if (x > PEGASUS_UINT64_MAX/16)
                {
                    return false;
                }
                x = x << 4;

                // We can't overflow when we add the next digit
                Uint64 newDigit = Uint64(hexCharToNumeric(*p++));
                if (PEGASUS_UINT64_MAX - x < newDigit)
                {
                    return false;
                }
                x = x + newDigit;
            }

            // If we found a non-hexadecimal digit, report an error
            if (*p)
                return false;

            return true;
        }
        else
        {
            // A decimal string that starts with '0' must be exactly "0".
        return p[1] == '\0';
        }
    }

    // Expect a positive decimal digit:

    // Add on each digit, checking for overflow errors
    while (isdigit(*p))
    {
        // Make sure we won't overflow when we multiply by 10
        if (x > PEGASUS_UINT64_MAX/10)
        {
            return false;
        }
        x = 10 * x;

        // Make sure we won't overflow when we add the next digit
        Uint64 newDigit = (*p++ - '0');
        if (PEGASUS_UINT64_MAX - x < newDigit)
        {
            return false;
        }
        x = x + newDigit;
    }

    // If we found a non-decimal digit, report an error
    if (*p)
    return false;

    return true;
}

static Boolean stringToSignedInteger(
    const char* stringValue,
    Sint64& x)
{
    x = 0;
    const char* p = stringValue;

    if (!p || !*p)
    return false;

    // Skip optional sign:

    Boolean negative = *p == '-';

    if (negative || *p == '+')
    p++;

    if (*p == '0')
    {
        if ( (p[1] == 'x') || (p[1] == 'X') )
        {
            // Convert a hexadecimal string

            // Skip over the "0x"
            p+=2;

            // At least one hexadecimal digit is required
            if (!isxdigit(*p))
                return false;

            // Build the Sint64 as a negative number, regardless of the
            // eventual sign (negative numbers can be bigger than positive ones)

            // Add on each digit, checking for overflow errors
            while (isxdigit(*p))
            {
                // Make sure we won't overflow when we multiply by 16
                if (x < PEGASUS_SINT64_MIN/16)
                {
                    return false;
                }
                x = x << 4;

                // Make sure we don't overflow when we add the next digit
                Sint64 newDigit = Sint64(hexCharToNumeric(*p++));
                if (PEGASUS_SINT64_MIN - x > -newDigit)
                {
                    return false;
                }
                x = x - newDigit;
            }

            // If we found a non-hexadecimal digit, report an error
            if (*p)
                return false;

            // Return the integer to positive, if necessary, checking for an
            // overflow error
            if (!negative)
            {
                if (x == PEGASUS_SINT64_MIN)
                {
                    return false;
                }
                x = -x;
            }
            return true;
        }
        else
        {
            // A decimal string that starts with '0' must be exactly "0".
        return p[1] == '\0';
        }
    }

    // Expect a positive decimal digit:

    // At least one decimal digit is required
    if (!isdigit(*p))
        return false;

    // Build the Sint64 as a negative number, regardless of the
    // eventual sign (negative numbers can be bigger than positive ones)

    // Add on each digit, checking for overflow errors
    while (isdigit(*p))
    {
        // Make sure we won't overflow when we multiply by 10
        if (x < PEGASUS_SINT64_MIN/10)
        {
            return false;
        }
        x = 10 * x;

        // Make sure we won't overflow when we add the next digit
        Sint64 newDigit = (*p++ - '0');
        if (PEGASUS_SINT64_MIN - x > -newDigit)
        {
            return false;
        }
        x = x - newDigit;
    }

    // If we found a non-decimal digit, report an error
    if (*p)
    return false;

    // Return the integer to positive, if necessary, checking for an
    // overflow error
    if (!negative)
    {
        if (x == PEGASUS_SINT64_MIN)
        {
            return false;
        }
        x = -x;
    }
    return true;
}

static Boolean stringToReal(const char* stringValue, Real64& x)
{
    //
    // Check the string against the DMTF-defined grammar
    //
    const char* p = stringValue;

    if (!*p)
    return false;

    // Skip optional sign:

    if (*p == '+' || *p  == '-')
    p++;

    // Skip optional first set of digits:

    while (isdigit(*p))
    p++;

    // Test required dot:

    if (*p++ != '.')
    return false;

    // One or more digits required:

    if (!isdigit(*p++))
    return false;

    while (isdigit(*p))
    p++;

    // If there is an exponent now:

    if (*p)
    {
    // Test exponent:

    if (*p != 'e' && *p != 'E')
        return false;

    p++;

    // Skip optional sign:

    if (*p == '+' || *p  == '-')
        p++;

    // One or more digits required:

    if (!isdigit(*p++))
        return false;

    while (isdigit(*p))
        p++;
    }

    if (*p)
    return false;

    //
    // Do the conversion
    //
    char* end;
    errno = 0;
    x = strtod(stringValue, &end);
    if (*end || (errno == ERANGE))
    {
        return false;
    }

    return true;
}

/***************************************************************************
   InvokeMethod Function
***************************************************************************/
static void InvokeMethod(const CGIQueryString& qs)
{
    CIMNamespaceName nameSpace = GetNameSpaceQueryField(qs);
    Array<CIMParamValue>  inParams;
    Array<CIMParamValue>  outParams;

    const char* tmp;

    String methodName = String::EMPTY;
    if ((tmp = qs.findValue("MethodName")))
    methodName = tmp;

    String argumentList = String::EMPTY;
    if ((tmp = qs.findValue("ArgumentList")))
    argumentList = tmp;

    if (!(tmp = qs.findValue("InstanceName")))
    ErrorExit("Missing InstanceName field");

    CIMObjectPath objectReference;
    try
    {
        objectReference = tmp;
    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }

    //ALAGS : Input argument parser for Semicolon separated values.
    // What is the best delimiter we could use ?
    // Commas and colons CANNOT be used as they are used
    // in REFs and DateTimes properties respectively.

    Array<String> inpArgs;
    Uint32 startIndex = 0;
    Uint32 endIndex = 0 ;
    Uint32 curIndex = 0; //Used while checking [IN] params

    if (argumentList.size () > 0)
    {
        endIndex = argumentList.find (';'); //ARGUMENT DELIMITER
        if(endIndex == PEG_NOT_FOUND)
        {
            argumentList = argumentList.subString (startIndex, endIndex);
            inpArgs.append(argumentList);
        }
        while(endIndex != PEG_NOT_FOUND)
        {
            String parmStr = argumentList.subString (startIndex, endIndex);
            inpArgs.append(parmStr);
            argumentList = argumentList.subString (endIndex+1,PEG_NOT_FOUND);
            endIndex = argumentList.find (';'); //ARGUMENT DELIMITER
            if(endIndex == PEG_NOT_FOUND)
                inpArgs.append(argumentList);
        }

    }


    // ALAGS : Method inspection code starts


    try
    {

        CIMClient client;
        String host;
        Uint32 portNumber;
        Uint32 inpargCount = 0;

        getHostAndPort (host, portNumber);
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);

        //Just to check if instance exists in given Namespace
        //Here, the returned instance is NOT used for data extraction
        CIMInstance cimInstance = client.getInstance(nameSpace,
                                                     objectReference);


        CIMClass c1=client.getClass(nameSpace,
                                    objectReference.getClassName(), false);

        for (Uint32 i = 0; i < c1.getMethodCount(); i++)
        {
            CIMMethod mth = c1.getMethod(i);

            //"if" check to decide if this method is the one requested in call
            if(String::equalNoCase(mth.getName().getString(), methodName))
            {

                for (Uint32 j = 0; j < mth.getParameterCount(); j++)
                {
                    Boolean inputQualifier = false;
                    CIMParameter mp = mth.getParameter(j);
                    String parName = mp.getName().getString();

                    for(Uint32 k = 0; k < mp.getQualifierCount();k++)
                    {
                        CIMQualifier qual = mp.getQualifier(k);
                        if(String::equalNoCase(qual.getName().getString(),"IN"))
                        {
                            inpargCount++; //Inc for every [IN] arg found
                            inputQualifier = true;
                        }
                    }


                    //Assign input Args from WebPage to Parameter only if
                    //Qualifier == [IN]
                    if((inputQualifier == true) &&
                        (inpArgs.size() >= inpargCount))
                    {
                        char valStr[64];
                        switch(mp.getType())
                        {
                            case CIMTYPE_BOOLEAN:
                                if(String::equalNoCase(inpArgs[curIndex++],
                                                       "true"))
                                {
                                    inParams.append( CIMParamValue( parName,
                                              CIMValue( Boolean(true) ) ) );
                                }
                                else
                                {
                                    inParams.append( CIMParamValue( parName,
                                              CIMValue( Boolean(false) ) ) );
                                }
                                break;
                            case CIMTYPE_UINT8:
                            case CIMTYPE_UINT16:
                            case CIMTYPE_UINT32:
                            case CIMTYPE_UINT64:
                            {
                                sprintf(valStr,"%s",
                                 (const char*)inpArgs[curIndex++].getCString());

                                Uint64 x=0;
                                if (!stringToUnsignedInteger(valStr, x))
                                {
                                    ErrorExit("Invalid Unsigned Integer value");
                                }
                                switch(mp.getType())
                                {
                                    case CIMTYPE_UINT8:
                                        {
                                        if (x >= (Uint64(1)<<8))
                                        {
                                            ErrorExit("Uint8 value out of range");
                                        }
                                        inParams.append( CIMParamValue( parName,
                                                         CIMValue( Uint8(x))));
                                        break;
                                        }
                                    case CIMTYPE_UINT16:
                                        {
                                        if (x >= (Uint64(1)<<16))
                                        {
                                            ErrorExit("Uint16 value out of range");
                                        }
                                        inParams.append( CIMParamValue( parName,
                                                         CIMValue( Uint16(x))));
                                        break;
                                        }
                                    case CIMTYPE_UINT32:
                                        {
                                        if (x >= (Uint64(1)<<32))
                                        {
                                            ErrorExit("Uint32 value out of range");
                                        }
                                        inParams.append( CIMParamValue( parName,
                                                         CIMValue( Uint32(x))));
                                        break;
                                        }
                                    case CIMTYPE_UINT64:
                                        {
                                        inParams.append( CIMParamValue( parName,
                                                         CIMValue( Uint64(x))));
                                        break;
                                        }
                                    default: break;

                                }
                                break;
                            }

                            case CIMTYPE_SINT8:
                            case CIMTYPE_SINT16:
                            case CIMTYPE_SINT32:
                            case CIMTYPE_SINT64:
                            {
                                sprintf(valStr,"%s",
                                 (const char*)inpArgs[curIndex++].getCString());
                                Sint64 x=0;
                                if (!stringToSignedInteger(valStr, x))
                                {
                                    ErrorExit("Invalid Signed Integer value.");
                                }
                                switch(mp.getType())
                                {
                                    case CIMTYPE_SINT8:
                                        {
                                        if(  (x >= (Sint64(1)<<7)) ||
                                             (x < (-(Sint64(1)<<7))) )
                                        {
                                         ErrorExit("Sint8 value out of range");
                                        }
                                        inParams.append( CIMParamValue( parName,
                                                         CIMValue( Sint8(x) )));
                                        break;
                                        }
                                    case CIMTYPE_SINT16:
                                        {
                                        if(  (x >= (Sint64(1)<<15)) ||
                                             (x < (-(Sint64(1)<<15))) )
                                        {
                                         ErrorExit("Sint16 value out of range");
                                        }
                                        inParams.append( CIMParamValue( parName,
                                                        CIMValue( Sint16(x) )));
                                        break;
                                        }
                                    case CIMTYPE_SINT32:
                                        {
                                        if(  (x >= (Sint64(1)<<31)) ||
                                             (x < (-(Sint64(1)<<31))) )
                                        {
                                         ErrorExit("Sint32 value out of range");
                                        }
                                        inParams.append( CIMParamValue( parName,
                                                        CIMValue( Sint32(x) )));
                                        break;
                                        }
                                    case CIMTYPE_SINT64:
                                        {
                                        inParams.append( CIMParamValue( parName,
                                                        CIMValue( Sint64(x) )));
                                        break;
                                        }
                                    default: break;

                                }
                                break;
                            }
                            case CIMTYPE_REAL32:
                                {
                                Real64 x=0;

                                sprintf(valStr,"%s",
                                 (const char*)inpArgs[curIndex++].getCString());
                                if (!stringToReal(valStr, x))
                                {
                                    ErrorExit("Invalid Real32 Number value");
                                }
                                inParams.append( CIMParamValue( parName,
                                                CIMValue( Real32(x) )));
                                break;
                                }
                            case CIMTYPE_REAL64:
                                {
                                Real64 x=0;
                                sprintf(valStr,"%s",
                                 (const char*)inpArgs[curIndex++].getCString());
                                if (!stringToReal(valStr, x))
                                {
                                    ErrorExit("Invalid Real64 Number value");
                                }
                                inParams.append( CIMParamValue( parName,
                                                 CIMValue(x) ));
                                break;
                                }
                            case CIMTYPE_STRING:
                                inParams.append( CIMParamValue( parName,
                                  CIMValue( inpArgs[curIndex++] )));
                                break;
                            case CIMTYPE_DATETIME:
                                {
                                CIMDateTime tmpdt;
                                String strDate = inpArgs[curIndex++];
                                if(strDate.size() !=0 )
                                    tmpdt.set(strDate);
                                else
                                    ErrorExit("Datetime string is null");

                                inParams.append( CIMParamValue( parName,
                                  CIMValue( CIMDateTime(tmpdt))));
                                break;
                                }
                            case CIMTYPE_REFERENCE:
                                inParams.append( CIMParamValue( parName,
                                 CIMValue(CIMObjectPath(inpArgs[curIndex++]))));
                                break;
                            case CIMTYPE_CHAR16:
                                {
                                    String tmp(inpArgs[curIndex++]);
                                    if (tmp.size() != 1)
                                    {
                                        ErrorExit("Invalid Char16 value");
                                    }
                                    inParams.append( CIMParamValue( parName,
                                    CIMValue( tmp[0] )));
                                    break;
                                }
                            case CIMTYPE_OBJECT:
                                // No support for embedded objects in this
                                // "test and demonstration tool". This case
                                // eliminate the compiler warning msg.
                                break;
                        }
                    }


                    // Check to see if client supplied atleast as many INPUT
                    // parameters that are required by the Method. Supplying
                    // more params than what is required by the method is OK.
                    if(inpArgs.size() < inpargCount)
                    {
                        ErrorExit("Insufficient input parameters");
                    }
                }
            }
        }


        // ALAGS : Method inspection code ends

        // Time the method call
        Stopwatch elapsedTime;

        elapsedTime.start();

        CIMValue retValue = client.invokeMethod(
            nameSpace,
            objectReference,
            CIMName (methodName),
            inParams,
            outParams);

        elapsedTime.stop();

        PrintHTMLHead("InvokeMethod", "Invoke an Extrinsic Method");

        cout << "<br><b>Method return value : </b>" << retValue.toString()
             << "<br>" << endl;

        cout << "<br><b>Method Output Parameters : </b><br>" << endl;
        cout << "<table border=1 width=\"50%\">\n";
        cout << "  <tr>\n";
        cout << "    <th>Parameter Name</th>\n";
        cout << "    <th>Value</th>\n";
        cout << "  </tr>\n";

        for (Uint8 i = 0; i < outParams.size(); i++)
        {
            cout << "<tr>";
            cout << "   <td>" << outParams[i].getParameterName() << "</td>";
            cout << "   <td>" << outParams[i].getValue().toString() << "</td>";
            cout << "</tr><br>" << endl;
        }

        cout << "</table>\n<br>";

        cout << "<br>InvokeMethod completed in " << elapsedTime.getElapsed()
             << " Seconds</p>\n";
        cout << "</body>\n" << "</html>\n";

    }
    catch(Exception& e)
    {
        ErrorExit(e.getMessage());
    }
}


/******************************************************************
*******************************************************************
   Main Function
*******************************************************************
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


    //ALAGS : for Bug 996 : Querystring is passed as second cmd line
    //agrument in Linux. The preprocess check below is to solve this issue
    //in Linux.

    // IS this "argc != 1" check required anyway ??
    #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
    if (argc != 1)
    ErrorExit("unexpected command line arguments");
    #endif
    const char* tmp = getenv("QUERY_STRING");

    if (!tmp)
    ErrorExit("QUERY_STRING environment variable missing");

    try
    {
        char* queryString = strcpy(new char[strlen(tmp) + 1], tmp);




    Logger::put( Logger::TRACE_LOG,  "CGIClient", Logger::INFORMATION,
        "Query String $0", queryString);

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
    //else if (strcmp(operation, "SetQualifier") == 0)
    //    SetQualifier(qs);
    else if (strcmp(operation, "DeleteQualifier") == 0)
        DeleteQualifier(qs);
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
    else if (strcmp(operation, "AllInstances") == 0)
        AllInstances(qs);
    else if (strcmp(operation, "InvokeMethod") == 0)
        InvokeMethod(qs);
    else
    {
        String message = "CGIClient - Unknown operation: ";
        message.append(operation);
        ErrorExit(message);
    }
    }
    catch (Exception& e)
    {
    ErrorExit(e.getMessage());
    }

    return 0;
}
/* TO List
Make sure that The program name is in all error messages.
Version on CGIClient.
Is class names and instance names the same basic thing that we can do
with template.
*/
