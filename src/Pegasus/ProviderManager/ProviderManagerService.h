//%//-*-c++-*-//////////////////////////////////////////////////////////////////
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//              Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//              Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManagerService_h
#define Pegasus_ProviderManagerService_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/ProviderManager/ProviderManager.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE ProviderManagerService : public MessageQueueService
{
   public:
      ProviderManagerService(void);
      virtual ~ProviderManagerService(void);

      virtual void handleEnqueue(Message *);
      virtual void handleEnqueue(void);

      // short term hack
      ProviderManager * getProviderManager(void)
      {
	 return(&providerManager);
      }

   protected:
      Pair<String, String> _lookupProviderForClass(const CIMObjectPath & objectPath);
	
   protected:	
      void handleGetInstanceRequest(const Message * message);
      void handleEnumerateInstancesRequest(const Message * message);
      void handleEnumerateInstanceNamesRequest(const Message * message);
      void handleCreateInstanceRequest(const Message * message);
      void handleModifyInstanceRequest(const Message * message);
      void handleDeleteInstanceRequest(const Message * message);
	
      void handleGetPropertyRequest(const Message * message);
      void handleSetPropertyRequest(const Message * message);
	
      void handleInvokeMethodRequest(const Message * message);

      void handleEnableIndicationRequest(const Message * message);
      void handleModifyIndicationRequest(const Message * message);
      void handleDisableIndicationRequest(const Message * message);

   protected:
      ProviderManager providerManager;
	
};

PEGASUS_NAMESPACE_END

#endif
