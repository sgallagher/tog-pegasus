//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CMPIProviderManager_h
#define Pegasus_CMPIProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/ProviderManager2/ProviderName.h>
#include <Pegasus/ProviderManager2/ProviderManager.h>
#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Repository/CIMRepository.h>

#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/ProviderManager2/OperationResponseHandler.h>

#include <Pegasus/ProviderManager2/CMPI/CMPILocalProviderManager.h>
#include <Pegasus/ProviderManager2/CMPI/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

struct CMPI_SelectExp;

class PEGASUS_CMPIPM_LINKAGE CMPIProviderManager : public ProviderManager
{
public:
    enum Mode {
       CMPI_MODE,
       CMPI_R_MODE,
       CMPI_O_MODE
    };

    Mode getMode() { return mode; }
    CMPIProviderManager(Mode=CMPI_MODE);
    virtual ~CMPIProviderManager(void);

    virtual Boolean insertProvider(const ProviderName & providerName,
            const String &ns, const String &cn);

    virtual Message * processMessage(Message * request);

    virtual Boolean hasActiveProviders();
    virtual void unloadIdleProviders();

    virtual Boolean supportsRemoteNameSpaces() { return true; }

   struct indProvRecord {
      indProvRecord() : enabled(false), count(1), handler(NULL) {}
      Boolean enabled;
      int count;
      EnableIndicationsResponseHandler* handler;
   };

   struct indSelectRecord {
      indSelectRecord() : eSelx(NULL) {}
      CMPI_SelectExp *eSelx;
   };

   typedef HashTable<String,indProvRecord*,  EqualFunc<String>,HashFunc<String> > IndProvTab;
   typedef HashTable<String,indSelectRecord*,EqualFunc<String>,HashFunc<String> > IndSelectTab;
   typedef HashTable<String,ProviderName,EqualFunc<String>,HashFunc<String> > ProvRegistrar;

   static IndProvTab provTab;
   static IndSelectTab selxTab;
   static ProvRegistrar provReg;

protected:
    CMPILocalProviderManager providerManager;

    Mode mode;
    CIMRepository *_repository;

    Message * handleUnsupportedRequest(const Message * message);

    Message * handleGetInstanceRequest(const Message * message);
    Message * handleEnumerateInstancesRequest(const Message * message);
    Message * handleEnumerateInstanceNamesRequest(const Message * message);
    Message * handleCreateInstanceRequest(const Message * message);
    Message * handleModifyInstanceRequest(const Message * message);
    Message * handleDeleteInstanceRequest(const Message * message);

    Message * handleExecQueryRequest(const Message * message);

    Message * handleAssociatorsRequest(const Message * message);
    Message * handleAssociatorNamesRequest(const Message * message);
    Message * handleReferencesRequest(const Message * message);
    Message * handleReferenceNamesRequest(const Message * message);
/*
    Message * handleGetPropertyRequest(const Message * message);
    Message * handleSetPropertyRequest(const Message * message);
*/
    Message * handleInvokeMethodRequest(const Message * message);

    Message * handleCreateSubscriptionRequest(const Message * message);
//    Message * handleModifySubscriptionRequest(const Message * message);
    Message * handleDeleteSubscriptionRequest(const Message * message);
    Message * handleEnableIndicationsRequest(const Message * message);
    Message * handleDisableIndicationsRequest(const Message * message);

//  Not supported by CMPI
//    Message * handleExportIndicationRequest(const Message * message);

    Message * handleDisableModuleRequest(const Message * message);
    Message * handleEnableModuleRequest(const Message * message);
    Message * handleStopAllProvidersRequest(const Message * message);
    Message * handleInitializeProviderRequest(const Message * message);

    ProviderName _resolveProviderName(const ProviderIdContainer & providerId);
};

PEGASUS_NAMESPACE_END

#endif
