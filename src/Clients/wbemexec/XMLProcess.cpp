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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//         (carolann_graves@hp.com)
//
// Modified By:
//         Warren Otsuka (warren_otsuka@hp.com)
//         Sushma Fernandes, Hewlett-Packard Company
//             (sushma_fernandes@hp.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/XmlConstants.h>
#include "HttpConstants.h"
#include "XMLProcess.h"

PEGASUS_NAMESPACE_BEGIN

/**
  
    Constructs an Array <Sint8> containing the representation of the CIM
    object path corresponding to the &lt;LOCALNAMESPACE&gt; element, in
    accordance with Specification for CIM Operations over HTTP, 
    Version 1.0, Section 3.3.3.  This method should be called only the parser
    is currently at a &lt;LOCALNAMESPACE&gt; element.
  
    @param   parser              parser instance corresponding to the XML
                                 request
  
    @exception  XmlValidationError  if the XML input is invalid
    @exception  XmlSemanticError    if the XML input contains a semantic error
  
    @return  a String containing the representation of the CIM
             object path corresponding to the &lt;LOCALNAMESPACE&gt; element
  
 */

String XMLProcess::getObjPath (XmlParser& parser)
    throw (XmlValidationError, XmlSemanticError, XmlException, Exception)
{
    String                       namespaceName;

    if (!XmlReader::getLocalNameSpacePathElement(parser, namespaceName))
    {
        throw XmlValidationError (parser.getLine (), 
            MISSING_ELEMENT_LOCALNAMESPACEPATH);
    }

    return (namespaceName);
}

/**
  
    Encapsulates the XML request in an HTTP M-POST or POST request message.
    Generates the appropriate HTTP extension headers corresponding to the
    XML request, in accordance with Specifications for CIM Operations over
    HTTP, Version 1.0, Section 3.  This method should be called only when
    the current parser location is the xml declaration.  If the xml
    declaration is not found, but the first token in the input is 
    HTTP_METHOD_MPOST or HTTP_METHOD_POST, it is assumed that the request is 
    already encapsulated in an HTTP request, and the message is returned 
    unchanged.  No attempt is made to validate the complete HTTP request.
    If the useMPost parameter is TRUE, the headers are generated for an
    M-POST request.  Otherwise, the headers are generated for a POST
    request.  If the useHTTP11 parameter is TRUE, the headers are generated
    for an HTTP/1.1 request.  Otherwise, the headers are generated for an
    HTTP/1.0 request.  The combination of useMPost true and useHTTP11 false
    is invalid, but this function does not check for this case.  The XML 
    request is examined only as much as necessary to generate the required 
    headers.  This method does not attempt to validate the entire XML request.
  
    @param   parser              XmlParser instance corresponding to the
                                 XML request
    @param   hostName            host name to be used in HTTP Host header
    @param   useMPost            Boolean indicating that headers should be
                                 generated for an M-POST request
    @param   useHTTP11           Boolean indicating that headers should be
                                 generated for an HTTP/1.1 request
    @param   content             Array <Sint8> containing XML request
    @param   httpHeaders         Array <Sint8> returning the HTTP headers
  
    @return  Array <Sint8> containing the XML request encapsulated in an
             HTTP request message
  
    @exception  XmlValidationError  if the XML input is invalid
    @exception  XmlSemanticError    if the XML input contains a semantic error
    @exception  WbemExecException   if the input contains neither XML nor HTTP
                                    M-POST or POST method request
  
 */
Array <Sint8> XMLProcess::encapsulate( XmlParser parser,
                                       String hostName,
                                       Boolean useMPost,
                                       Boolean useHTTP11,
                                       Array <Sint8>& content,
                                       Array <Sint8>& httpHeaders
                                       )
throw (XmlValidationError, XmlSemanticError, WbemExecException,
               XmlException, Exception)
{
    XmlEntry                     entry;
    Array <Sint8>                message;
    String                       messageId;
    const char*                  cimVersion            = 0;
    const char*                  dtdVersion            = 0;
    String                       protocolVersion;
    String                       className;
    String                       methodName;
    Array<Sint8>                 encoded;
    Array <Sint8>                objPath;
    Array <KeyBinding>           keyBindings;
    KeyBinding::Type             type;
    Boolean                      multireq              = false;
    Uint32                       i                     = 0;
    static Uint32                BUFFERSIZE            = 1024;

    //
    //  xml declaration
    //
    if (!(XmlReader::testXmlDeclaration (parser, entry)))
    {
        //
        //  No xml declaration
        //  Request may already be encapsulated in HTTP
        //  Check for HTTP method
        //
        char tmp [8];
        char* tmpp = & (tmp [0]);
        strncpy (tmpp, entry.text, 8);
        char* p = strtok (tmpp, HTTP_SP);
        if (p != NULL)
        {
            if ((strcmp (p, HTTP_METHOD_MPOST) == 0) || 
                (strcmp (p, HTTP_METHOD_POST) == 0) ||
	        //  This is a special request used for testing.
	        //  It includes the HTTP header.
	        //  Return the message as is.
                (strcmp (p, HTTP_METHOD_BOGUS) == 0))
            {
	      return (content);
            }
            else
            {
                //
                //  Input contains neither XML declaration nor HTTP M-POST or
                //  POST method request
                //
                WbemExecException e (WbemExecException::INVALID_INPUT);
                throw e;
            }
        }  /* if p not NULL */
    }

    //
    //  CIM element
    //
    XmlReader::getCimStartTag (parser, cimVersion, dtdVersion);


    //
    //  MESSAGE element
    //
    if (!XmlReader::getMessageStartTag (parser, messageId, protocolVersion))
    {
        throw XmlValidationError (parser.getLine (), MISSING_ELEMENT_MESSAGE);
    }

    //
    //  MULTIREQ or SIMPLEREQ element
    //
    if (XmlReader::testStartTag (parser, entry, XML_ELEMENT_MULTIREQ))
    {
        multireq = true;
    }
    else if (!XmlReader::testStartTag (parser, entry, XML_ELEMENT_SIMPLEREQ))
    {
        throw XmlValidationError (parser.getLine (), MISSING_ELEMENT_REQ);
    }

    //
    //  SIMPLEREQ element
    //
    else
    {
        //
        //  IMETHODCALL element
        //
        if (XmlReader::testStartTag (parser, entry, XML_ELEMENT_IMETHODCALL))
        {
            //
            //  Get NAME attribute of IMETHODCALL element
            //
            methodName = XmlReader::getCimNameAttribute (parser.getLine (), 
                entry, XML_ELEMENT_IMETHODCALL);

            //
            //  Construct the object path from the LOCALNAMESPACEPATH 
            //  subelements (NAMESPACE elements)
            //
            objPath << getObjPath (parser);
        }

        //
        //  METHODCALL element
        //
        else if (XmlReader::testStartTag (parser, entry, 
            XML_ELEMENT_METHODCALL))
        {
            //
            //  Get NAME attribute of METHODCALL element
            //
            methodName = XmlReader::getCimNameAttribute (parser.getLine (), 
                entry, XML_ELEMENT_METHODCALL);

            //
            //  LOCALCLASSPATH or LOCALINSTANCEPATH element
            //
            if (XmlReader::testStartTag (parser, entry, 
                XML_ELEMENT_LOCALCLASSPATH))
            {
                //
                //  LOCALCLASSPATH element
                //
                //  Construct the name space portion of the object path 
                //  from the LOCALNAMESPACEPATH subelements (NAMESPACE 
                //  elements)
                //
                objPath << getObjPath (parser);
                objPath << LOCALNAMESPACEPATH_DELIMITER;

                //
                //  CLASSNAME element
                //
                if (!XmlReader::getClassNameElement (parser, className))
                {
                    throw XmlValidationError (parser.getLine (), 
                        MISSING_ELEMENT_CLASSNAME);
                }

                objPath << className;
            }
            else if (XmlReader::testStartTag (parser, entry, 
                XML_ELEMENT_LOCALINSTANCEPATH))
            {
                //
                //  LOCALINSTANCEPATH element
                //
                //  Construct the name space portion of the object path 
                //  from the LOCALNAMESPACEPATH subelements (NAMESPACE 
                //  elements)
                //
                objPath << getObjPath (parser);
                objPath << LOCALNAMESPACEPATH_DELIMITER;

                //
                //  INSTANCENAME element
                //
                if (!XmlReader::getInstanceNameElement (parser, className,
                                                        keyBindings))
                {
                    throw XmlValidationError (parser.getLine (), 
                        MISSING_ELEMENT_INSTANCENAME);
                }

                //
                //  Append CLASSNAME portion of object path
                //
                objPath << className;

                //
                //  Append KEYBINDING portion of object path
                //
                if (keyBindings.size () <= 0)
                {
                    objPath << SINGLETON_INSTANCE;
                }
                else
                {
                    for (i = 0; i < keyBindings.size (); i++)
                    {
                        if (i == 0)
                        {
                            objPath << KEYBINDING_DELIMITER;
                        }
                        else
                        {
                            objPath << KEYBINDING_SEPARATOR;
                        }
                        objPath << keyBindings [i].getName () << KEYVALUE_EQUAL;
                        type = keyBindings [i].getType ();
                        if (type == KeyBinding::STRING)
                        {
                            objPath << KEYVALUE_STRING_DELIMITER;
                        }
                        objPath << _escapeSpecialCharacters 
                                   (keyBindings [i].getValue ());
                        if (type == KeyBinding::STRING)
                        {
                            objPath << KEYVALUE_STRING_DELIMITER;
                        }
                    }
                }
            }
            else
            {
                throw XmlValidationError (parser.getLine (), 
                    MISSING_ELEMENT_LOCALPATH);
            }
        }
        else
        {
            throw XmlValidationError (parser.getLine (), 
                MISSING_ELEMENT_METHODCALL);
        }
    }

    //
    //  Set headers
    //
    message.reserve (BUFFERSIZE);

    //
    //  Generate header prefix
    //
    srand (TimeValue::getCurrentTime ().toMilliseconds ());
    char nn [] = { '0' + (rand () % 10), '0' + (rand () % 10), '\0' };

    if (useMPost) 
    {
        message << HTTP_METHOD_MPOST << HTTP_SP << HTTP_REQUEST_URI_CIMOM
                << HTTP_SP << HTTP_PROTOCOL << HTTP_VERSION_11 << HTTP_CRLF;
    }
    else
    {
        message << HTTP_METHOD_POST << HTTP_SP << HTTP_REQUEST_URI_CIMOM
                << HTTP_SP << HTTP_PROTOCOL;
        if (useHTTP11)
        {
            message << HTTP_VERSION_11;
        } 
        else
        {
            message << HTTP_VERSION_10;
        }
        message << HTTP_CRLF;
    }
    message << HEADER_NAME_HOST << HEADER_SEPARATOR << HTTP_SP << hostName 
            << HTTP_CRLF;
    message << HEADER_NAME_CONTENTTYPE << HEADER_SEPARATOR << HTTP_SP
            << HEADER_VALUE_CONTENTTYPE 
            << HTTP_CRLF;
    message << HEADER_NAME_CONTENTLENGTH << HEADER_SEPARATOR << HTTP_SP 
            << content.size () << HTTP_CRLF;

    if (useMPost)
    {
        message << HEADER_NAME_MAN << HEADER_SEPARATOR << HTTP_SP 
                << HEADER_VALUE_MAN << nn << HTTP_CRLF;
        message << nn << HEADER_PREFIX_DELIMITER 
                << HEADER_NAME_CIMPROTOCOLVERSION << HEADER_SEPARATOR 
                << HTTP_SP << protocolVersion << HTTP_CRLF;
        message << nn << HEADER_PREFIX_DELIMITER << HEADER_NAME_CIMOPERATION
                << HEADER_SEPARATOR << HTTP_SP << HEADER_VALUE_CIMOPERATION 
                << HTTP_CRLF;
        if (multireq)
        {
            message << nn << HEADER_PREFIX_DELIMITER << HEADER_NAME_CIMBATCH 
                << HEADER_SEPARATOR << HTTP_CRLF;
        }
        else
        {
            message << nn << HEADER_PREFIX_DELIMITER << HEADER_NAME_CIMMETHOD 
                << HEADER_SEPARATOR << HTTP_SP << methodName << HTTP_CRLF;
            message << nn << HEADER_PREFIX_DELIMITER << HEADER_NAME_CIMOBJECT
                << HEADER_SEPARATOR << HTTP_SP << objPath << HTTP_CRLF;
        }
    }
    else
    {
        message << HEADER_NAME_CIMPROTOCOLVERSION << HEADER_SEPARATOR 
                << HTTP_SP << protocolVersion << HTTP_CRLF;
        message << HEADER_NAME_CIMOPERATION << HEADER_SEPARATOR << HTTP_SP
                << HEADER_VALUE_CIMOPERATION << HTTP_CRLF;
        if (multireq)
        {
            message << HEADER_NAME_CIMBATCH << HEADER_SEPARATOR << HTTP_CRLF;
        }
        else
        {
            message << HEADER_NAME_CIMMETHOD << HEADER_SEPARATOR << HTTP_SP 
                    << methodName << HTTP_CRLF;
            message << HEADER_NAME_CIMOBJECT << HEADER_SEPARATOR << HTTP_SP 
                    << objPath << HTTP_CRLF;
        }
    }

    httpHeaders << message;
    message << HTTP_CRLF;
    message << content;

    return message;
}

/**
  
    Constructs a String from the input String, applying the standard
    escaping mechanism to escape any characters that are unsafe within
    an HTTP header.  Used for key values of type string in a CIM object
    path.
  
    @param   str                 input String to escape
  
    @return  a new String corresponding to the input string, with the
             escaping mechanism having been applied
  
 */
String XMLProcess::_escapeSpecialCharacters (const String& str)
{
    String result;

    for (Uint32 i = 0, n = str.size (); i < n; i++)
    {
        switch (str [i])
        {
            case '\n':
                result += "\\n";
                break;

            case '\r':
                result += "\\r";
                break;

            case '\t':
                result += "\\t";
                break;

            case '"':
                result += "\\\"";
                break;

            default:
                result += str [i];
        }
    }

    return result;
}

PEGASUS_NAMESPACE_END
