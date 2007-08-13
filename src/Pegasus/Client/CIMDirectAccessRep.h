//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//
//
//%/////////////////////////////////////////////////////////////////////////////
//

#ifndef _CIMDirectAccessRep_h
#define _CIMDirectAccessRep_h

#include <Pegasus/ProviderManager2/CMPI/CMPIProviderManager.h>
#include <Pegasus/ProviderManagerService/ProviderManagerService.h>

// NOCHKSRC
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
// DOCHKSRC

#include <Pegasus/Server/CIMOperationRequestDispatcher.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/HandlerService/IndicationHandlerService.h>
#include <Pegasus/IndicationService/IndicationService.h>
#include <Pegasus/ControlProviders/NamespaceProvider/NamespaceProvider.h>
#include <Pegasus/ControlProviders/InteropProvider/InteropProvider.h>

// NOCHKSRC
#include <Pegasus/ControlProviders/ProviderRegistrationProvider/ProviderRegistrationProvider.h>
// DOCHKSRC

#include <Pegasus/ControlProviders/Statistic/CIMOMStatDataProvider.h>

// NOCHKSRC
#include <Pegasus/ControlProviders/QueryCapabilitiesProvider/CIMQueryCapabilitiesProvider.h>
#include <Pegasus/ControlProviders/ConfigSettingProvider/ConfigSettingProvider.h>
// DOCHKSRC

#include <Pegasus/ProviderManager2/CMPI/CMPIProviderManager.h>
#include <Pegasus/Server/ShutdownProvider.h>
//#include <Pegasus/Client/CIMClientRep.h>
//#include <Pegasus/Client/CIMClient.h>
#include "CIMDirectAccess.h"


#define  dacimINTEGRATED  1
#define  dacimSEPREPOSI   2
#define  dacimSTANDALONE  3


PEGASUS_NAMESPACE_BEGIN


struct cimSubscription;
struct subscri_item;




//------------------------------------------------------------
class PEGASUS_CLIENT_LINKAGE  CIMDirectAccessRep
{

public:

    static CIMDirectAccessRep *get();
    void release();

    Message* dorequest(AutoPtr<CIMRequestMessage>& req);
#if 0
    void addSubscription(cimSubscription&);
    void removeSubscription(const cimSubscription&);
#endif
    //void handleEnqueue();
    //void handleEnqueue(Message*);
#ifdef DACIM_DEBUG
    void verify(const char*, int);
#endif
    CIMDirectAccessRep();           // see get()
    ~CIMDirectAccessRep();           // use release()
protected:
//    ~CIMDirectAccessRep();           // use release()

private:

//    CIMDirectAccessRep();           // see get()

    ProviderManagerService* pvdrmgrsvc_;
    CMPIProviderManager* cmpiProviderManager_;
    ProviderRegistrationManager* pvdrregimgr_;
    CIMRepository* reposi_;
    CIMOperationRequestDispatcher* opreqdispatch_;
    //IndicationService              *indisvc_;
    ProviderMessageHandler* nspvdr_,
        *interoppvdr_,
        *pvdrregipvdr_,
        *statdatapvdr_,
        *cfgpvdr_,
        *queryPvdr_;

    Array<ProviderMessageHandler*> controlpvdr_;
    ModuleController* controlsvc_;
    int numsubscri_;
    static Mutex arequestlock_;
    static AtomicInt refcount_;
    Message* responsemsg_;


#if PEGASUS_DIRECTACCESS_BUILDTYPE >= dacimSEPREPOSI
    IndicationHandlerService* indihandlersvc_;

    IndicationHandlerService* odiniter_(IndicationHandlerService*);
#endif


    CMPIProviderManager* odiniter_(ProviderManagerService*);
    //IndicationService        *odiniter_(IndicationService*);
    ProviderMessageHandler* odiniter_(NamespaceProvider*);
    ProviderMessageHandler* odiniter_(InteropProvider*);
    ProviderMessageHandler* odiniter_(ProviderRegistrationProvider*);
    ProviderMessageHandler* odiniter_(CIMOMStatDataProvider*);
    ProviderMessageHandler* odiniter_(ConfigSettingProvider*);
//#if 0
    ProviderMessageHandler* odiniter_(CIMQueryCapabilitiesProvider*);
//#endif
    void chunkCallback_(CIMRequestMessage*, CIMResponseMessage*);
    void indicationCallback_(CIMProcessIndicationRequestMessage*);

    //ProviderName mkpvdrname_();
    //Message *mkMessage_( Message*, const CIMName&, Uint32);
    //Message *mkMessage_( const cimSubscription& );

#if 0
    bool isvalidsubscription_(const cimSubscription&);
    bool isduplicatesubscription_(const cimSubscription&);
    subscri_item* findsubscription_(const cimSubscription&);
#endif

    Message* do_gc_(CIMRequestMessage*);

#if 0
    Message* do_mc_(CIMRequestMessage*);
    Message* do_dc_(CIMRequestMessage*);
    Message* do_cc_(CIMRequestMessage*);
#endif

    Message* do_gi_(CIMRequestMessage*);
    Message* do_mi_(CIMRequestMessage*);
    Message* do_di_(CIMRequestMessage*);
    Message* do_ci_(CIMRequestMessage*);
    Message* do_ec_(CIMRequestMessage*);
    Message* do_ecn_(CIMRequestMessage*);
    Message* do_ei_(CIMEnumerateInstancesRequestMessage*);
    Message* do_ein_(CIMRequestMessage*);
    Message* do_ea_(CIMRequestMessage*);
    Message* do_ean_(CIMRequestMessage*);
    Message* do_er_(CIMRequestMessage*);
    Message* do_ern_(CIMRequestMessage*);
    Message* do_gp_(CIMRequestMessage*);
    Message* do_sp_(CIMRequestMessage*);
    Message* do_gq_(CIMRequestMessage*);

#if 0
    Message* do_sq_ (CIMRequestMessage*);
    Message* do_dq_ (CIMRequestMessage*);
#endif

    Message* do_eq_(CIMRequestMessage*);
    Message* do_invoke_(CIMRequestMessage*);
    Message* do_query_(CIMRequestMessage*);

    Message* forwardRequestForAggregation_(
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request,
        OperationAggregate* poA,
        CIMResponseMessage* response = NULL );

    Message* forwardRequestToProvider_(
        const CIMName& className,
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request,
        CIMRequestMessage* requestCopy = NULL);

    Message* forwardRequestToService_(
        const String& serviceName,
        CIMRequestMessage* request,
        CIMRequestMessage* requestCopy = NULL);

    void handleEnumerateInstancesResponseAggregation(     // need?
        OperationAggregate*);
    void handleEnumerateInstanceNamesResponseAggregation( // need?
        OperationAggregate*);
    bool isCMPIInterface(CIMRequestMessage*);
    friend void _cleanup(int i = 0 , void* t = 0);

}; //CIMDirectAccessRep

PEGASUS_NAMESPACE_END
#endif /* _CIMDirectAccessRep_h */


