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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase1
//		   Seema Gupta (gseema@in.ibm.com) for Bug#1096
//         David Dillard, VERITAS Software Corp.  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPMessage_h
#define Pegasus_HTTPMessage_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/AcceptLanguages.h>
#include <Pegasus/Common/ContentLanguages.h>

PEGASUS_NAMESPACE_BEGIN

typedef Pair<String, String> HTTPHeader;

/** This message is sent from a connection to its output queue when
    a complete HTTP message is received.
*/
class PEGASUS_COMMON_LINKAGE HTTPMessage : public Message
{
   public:
  
      typedef Message Base;
      
      HTTPMessage(const Array<char>& message_, Uint32 queueId_ = 0,
									const CIMException *cimException_ = 0);

      HTTPMessage(const HTTPMessage & msg);

      Array<char> message;
      Uint32 queueId;
      AuthenticationInfo* authInfo;
      AcceptLanguages acceptLanguages;
      ContentLanguages contentLanguages;
      Boolean acceptLanguagesDecoded;
      Boolean contentLanguagesDecoded;
			CIMException cimException;

      void parse(
	 String& startLine,
	 Array<HTTPHeader>& headers,
	 Uint32& contentLength) const;

      #ifdef PEGASUS_DEBUG
       void printAll(PEGASUS_STD(ostream)& os) const;
      #endif

		  static void lookupHeaderPrefix(
    Array<HTTPHeader>& headers,
    const String& fieldName,
    String& prefix);

      static Boolean lookupHeader(
	 Array<HTTPHeader>& headers,
	 const String& fieldName,
	 String& fieldValue,
	 Boolean allowNamespacePrefix = false);

      static Boolean parseRequestLine(
	 const String& startLine,
	 String& methodName,
	 String& requestUri,
	 String& httpVersion);

      static Boolean parseStatusLine(
	 const String& statusLine,
	 String& httpVersion,
	 Uint32& statusCode,
	 String& reasonPhrase);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPMessage_h */
