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
// Author: Mike Brasher (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPOptions_h
#define Pegasus_HTTPOptions_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>


/*
  The HTTP Options module processes incoming HTTP Messages OPTIONS Method
  and generates the appropriate responses.
  The OPTIONS Header is used by a CIM Client to demand capabilities
  from the target CIM Server
  Example of a capabilities response
  HTTP/1.1 200 OK
  Opt: http://www.dmtf.org/cim/mapping/http/v1.0 ; ns=77
  77-CIMProtocolVersion: 1.0
  77-CIMSupportedFunctionalGroups: basic-read
  77-CIMBatch
  77-CIMSupportedQueryLanguages: wql
  Reference the CIM Operations standard section 4.5 for the exact definition
  of the OPTIONS response
  Note that we ignore any entity body for the Option method
*/
/*
  QUESTION: Do we support HTTP 1.1
*/ 
#include "HTTPOptions.h"


#include <cstdio>
#include <iostream>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>

PEGASUS_NAMESPACE_BEGIN

// REVIEW: What is this class used for? What feeds its queue? What queue
// REVIEW: does it feed?

class PEGASUS_COMMON_LINKAGE HTTPOptions : public MessageQueueService
{
   public:
      typedef MessageQueueService Base;
  
      /** Constructor. There are no further 
       */
      HTTPOptions() 
      {
#error  "this class does not appear to be used" // <<< Sat Feb  9 17:33:07 2002 mdd >>>
      }
  
      /** Destructor. */
      ~HTTPOptions();
      virtual void handleEnqueue(Message *);
      virtual void handleEnqueue();

      void handleHTTPMessage(HTTPMessage* httpMessage);

   private:


};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPOptions_h */
