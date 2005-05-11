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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//             (carolann_graves@hp.com)
//
// Modified By:
//         Warren Otsuka (warren_otsuka@hp.com)
//         Sushma Fernandes, Hewlett-Packard Company
//             (sushma_fernandes@hp.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Aruran, IBM (ashanmug@in.ibm.com) for BUG# 2574
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_XMLProcess_h
#define Pegasus_XMLProcess_h

#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/XmlParser.h>
#include "WbemExecException.h"

PEGASUS_NAMESPACE_BEGIN

/**
  
    XMLProcess provides static methods for processing an XML request tree or 
    subtree structure, in accordance with Specifications for CIM Operations 
    over HTTP, Version 1.0.  These methods are used by the WbemExecCommand 
    class to encapsulate the XML request in an HTTP M-POST or POST request
    message.
  
    @author  Hewlett-Packard Company
  
 */
class XMLProcess 
{
public:

    /**
      
        Encapsulates the XML request in an HTTP M-POST or POST request message.
        Generates the appropriate HTTP extension headers corresponding to the 
        XML request, in accordance with Specifications for CIM Operations over 
        HTTP, Version 1.0, Section 3.  This method should be called only when
        the current parser location is the xml declaration.  If the xml
        declaration is not found, it is assumed that the request is already
        encapsulated in an HTTP request, and the message is returned unchanged.
        If the useMPost parameter is TRUE, the headers are generated for an 
        M-POST request.  Otherwise, the headers are generated for a POST
        request.  If the useHTTP11 parameter is TRUE, the headers are generated
        for an HTTP/1.1 request.  Otherwise, the headers are generated for an
        HTTP/1.0 request.  The combination of useMPost true and useHTTP11 false
        is invalid, but this function does not check for this case.  The XML
        request is examined only as much as necessary to generate the required
        headers.  This method does not attempt to validate the entire XML 
        request.
      
        @param   parser              XmlParser instance corresponding to the 
                                     XML request
        @param   hostName            host name to be used in HTTP Host header
        @param   useMPost            Boolean indicating that headers should be 
                                     generated for an M-POST request 
        @param   useHTTP11           Boolean indicating that headers should be 
                                     generated for an HTTP/1.1 request 
        @param   clientAuthenticator Authenticator object used to generate
                                     authentication headers
        @param   useAuthentication   Boolean indicating that an authentication
                                     header should be added to the request
        @param   content             Array<char> containing XML request
        @param   httpHeaders         Array<char> returning the HTTP headers
      
        @return  Array<char> containing the XML request encapsulated in an
                 HTTP request message
      
        @exception  XmlValidationError  if the XML input is invalid
        @exception  XmlSemanticError    if the XML input contains a semantic 
                                        error
        @exception  WbemExecException   if the input contains neither XML
                                        declaration nor HTTP M-POST or POST 
                                        method request
        @exception  XmlException        if the XML input is invalid
        @exception  Exception           internal error 
      
     */
    static Array<char> encapsulate (XmlParser& parser, 
                                      const String& hostName,
                                      Boolean useMPost,
                                      Boolean useHTTP11,
                                      Array<char>& content,
                                      Array<char>& httpHeaders) 
        throw (XmlValidationError, XmlSemanticError, WbemExecException,
               XmlException, Exception);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_XMLProcess_h */
