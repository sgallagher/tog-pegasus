//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//         (carolann_graves@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HttpConstants_h
#define Pegasus_HttpConstants_h

PEGASUS_NAMESPACE_BEGIN

// REVIEW: Using static declarations will cause multiple declarations of these
// REVIEW: strings. Either use #define (best) or use PEGASUS_COMMON_LINKAGE and
// REVIEW: eliminate use of statics.

// REVIEW: Name of file may not be the best. Many constants in this file are
// REVIEW: "HTTP Constants".

//
//  This include file contains constants related to the HTTP encapsulation 
//  of an XML-encoded CIM operation, including HTTP request elements, HTTP
//  header names and values, and special characters used in headers and 
//  CIMObject values.
//

//
//  HTTP Request Line elements
//

// ATTN: note: this use of static will cause the string to be created in
// every compiliation unit which includes it.

/**
 *  A string representing the HTTP M-POST method.
 */
static const char HTTP_METHOD_MPOST [] = "M-POST";

/**
 *  A string representing the HTTP POST method.
 */
static const char HTTP_METHOD_POST [] = "POST";

/**
 *  A string representing a bogus HTTP method, to allow testing of CIM server
 *  handling of unknown HTTP methods.
 */
static const char HTTP_METHOD_BOGUS [] = "BOGUS";

/**
 *  A string representing the request URI cimom.
 */
static const char HTTP_REQUEST_URI_CIMOM [] = "/cimom";

/**
 *  A string representing the protocol HTTP
 */
static const char HTTP_PROTOCOL [] = "HTTP/";

/**
 *  A string representing the HTTP/1.0 version.
 */
static const char HTTP_VERSION_10 [] = "1.0";

/**
 *  A string representing the HTTP/1.1 version.
 */
static const char HTTP_VERSION_11 [] = "1.1";

/**
 *  A string representing basic authentication
 */
static const char HTTP_AUTH_BASIC [] = "Basic";
/**
 *  A string representing the required CRLF sequence.
 */
static const char HTTP_CRLF [] = "\r\n";

/**
 *  A character representing the required SP separator.
 */
static const char HTTP_SP [] = " ";


//
//  Header names
//

/**
 *  The name of the HTTP header specifying the authentication type and value
 */
static const char HEADER_NAME_AUTHORIZATION [] =
                                    "Authorization";
/**
 *  The name of the HTTP header specifying the Internet host of the resource
 *  being requested.
 */
static const char HEADER_NAME_HOST [] = "Host";

/**
 *  The name of the HTTP header specifying the media type of the underlying
 *  data.
 */
static const char HEADER_NAME_CONTENTTYPE [] = "Content-Type";

/**
 *  The name of the HTTP header specifying the size of the entity-body.
 */
static const char HEADER_NAME_CONTENTLENGTH [] = "Content-Length";

/**
 *  The name of the HTTP header specifying a mandatory extension 
 *  declaration.
 */
static const char HEADER_NAME_MAN [] = "Man";

/**
 *  The name of the HTTP extension header identifying the version of the
 *  CIM mapping onto HTTP being used by the sending entity.
 */
static const char HEADER_NAME_CIMPROTOCOLVERSION [] = "CIMProtocolVersion";

/**
 *  The name of the HTTP extension header identifying the message as a CIM
 *  operation request or response.
 */
static const char HEADER_NAME_CIMOPERATION [] = "CIMOperation";

/**
 *  The name of the HTTP extension header identifying the name of the CIM
 *  method to be invoked.
 */
static const char HEADER_NAME_CIMMETHOD [] = "CIMMethod";

/**
 *  The name of the HTTP extension header identifying the name of the CIM
 *  object on which the method is to be invoked.
 */
static const char HEADER_NAME_CIMOBJECT [] = "CIMObject";

/**
 *  The name of the HTTP extension header identifying the encapsulated
 *  Operation Request Message as containing multiple method invocations.
 */
static const char HEADER_NAME_CIMBATCH [] = "CIMBatch";

//
//  Header values
//

/**
 *  The value of the HTTP Content-type header specifying that the media 
 *  type of the underlying data is application data in XML format.
 */
static const char HEADER_VALUE_CONTENTTYPE [] = 
                           "application/xml; charset=\"utf-8\"";

/**
 *  The value of the HTTP Man header specifying the CIM mapping onto HTTP
 *  extension header name space URI.
 */
static const char HEADER_VALUE_MAN [] = 
                           "http://www.dmtf.org/cim/mapping/http/v1.0;ns=";

/**
 *  The value of the HTTP CIMOperation header identifying the message as
 *  carrying a CIM operation request.
 */
static const char HEADER_VALUE_CIMOPERATION [] = "MethodCall";


//
//  Characters used in headers and CIMObject values
//

/**
 *  The character used to separate the header name and header value.
 */
static const char HEADER_SEPARATOR = ':';

/**
 *  The character used to delimit the header prefix and extension header.
 */
static const char HEADER_PREFIX_DELIMITER = '-';

/**
 *  The character used to delimit &lt;NAMESPACE&gt; subelements in a CIM
 *  object path.
 */
static const char NAMESPACE_SUBELEMENT_DELIMITER = '/';

/**
 *  The character used to delimit the &lt;LOCALNAMESPACEPATH&gt; subelement
 *  and either the &lt;CLASSNAME&gt; or &lt;INSTANCENAME&gt; subelement in
 *  a CIM object path.
 */
static const char LOCALNAMESPACEPATH_DELIMITER = ':';

/**
 *  The character used to equate a keybinding name and value in a CIM object 
 *  path.
 */
static const char KEYVALUE_EQUAL = '=';

/**
 *  The character used to delimit a key value of type string in a CIM object 
 *  path.
 */
static const char KEYVALUE_STRING_DELIMITER = '"';

/**
 *  The character used to indicate a singleton instance in a CIM object path.
 */
static const char SINGLETON_INSTANCE = '@';

/**
 *  The character used to delimit the &lt;CLASSNAME&gt; and first
 *  &lt;KEYBINDING&gt; subelements in a CIM object path.
 */
static const char KEYBINDING_DELIMITER = '.';

/**
 *  The character used to separate &lt;KEYBINDING&gt; subelements in a CIM
 *  object path.
 */
static const char KEYBINDING_SEPARATOR = ',';


PEGASUS_NAMESPACE_END

#endif /* Pegasus_HttpConstants_h */
