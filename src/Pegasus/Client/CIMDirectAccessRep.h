#ifndef _CIMDirectAccessRep_h
#define _CIMDirectAccessRep_h
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

#include <Pegasus/ProviderManager2/CMPI/CMPIProviderManager.h>
#include <Pegasus/ProviderManagerService/ProviderManagerService.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/Server/CIMOperationRequestDispatcher.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/HandlerService/IndicationHandlerService.h>
#include <Pegasus/IndicationService/IndicationService.h>
#include <Pegasus/ControlProviders/NamespaceProvider/NamespaceProvider.h>
#include <Pegasus/ControlProviders/InteropProvider/InteropProvider.h>
#include <Pegasus/ControlProviders/ProviderRegistrationProvider/ProviderRegistrationProvider.h>
#include <Pegasus/ControlProviders/Statistic/CIMOMStatDataProvider.h>
#include <Pegasus/ControlProviders/ConfigSettingProvider/ConfigSettingProvider.h>
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
class PEGASUS_COMMON_LINKAGE  CIMDirectAccessRep  {

    public:

        static CIMDirectAccessRep *get();
        void release();

        Message *dorequest( AutoPtr<CIMRequestMessage>& req );
        void addSubscription( cimSubscription& );
        void removeSubscription( const cimSubscription& );

        //void handleEnqueue();
        //void handleEnqueue(Message*);
       
        void verify(const char*,int);
        
        CIMDirectAccessRep();           // see get() 
       ~CIMDirectAccessRep();           // use release()

    protected:        
//       ~CIMDirectAccessRep();           // use release()

    private:

//        CIMDirectAccessRep();           // see get() 

        ProviderManagerService         *pvdrmgrsvc_;
        ProviderRegistrationManager    *pvdrregimgr_;
        CIMRepository                  *reposi_;
        CIMOperationRequestDispatcher  *opreqdispatch_;
        //IndicationService              *indisvc_;
        ProviderMessageHandler         *nspvdr_,
                                       *interoppvdr_,
                                       *pvdrregipvdr_,
                                       *statdatapvdr_,
                                       *cfgpvdr_;
        Array<ProviderMessageHandler*>  controlpvdr_;
        ModuleController               *controlsvc_;
        int                             numsubscri_;
        static Mutex                    arequestlock_;
        static AtomicInt                refcount_;            
        Message                        *responsemsg_;
        
        
#if PEGASUS_DIRECTACCESS_BUILDTYPE >= dacimSEPREPOSI        
        IndicationHandlerService       *indihandlersvc_;
        
        IndicationHandlerService *odiniter_(IndicationHandlerService*);
#endif        

       
        ProviderManagerService   *odiniter_(ProviderManagerService*);
        //IndicationService        *odiniter_(IndicationService*);
        ProviderMessageHandler *odiniter_(NamespaceProvider*);
        ProviderMessageHandler *odiniter_(InteropProvider*);
        ProviderMessageHandler *odiniter_(ProviderRegistrationProvider*);
        ProviderMessageHandler *odiniter_(CIMOMStatDataProvider*);
        ProviderMessageHandler *odiniter_(ConfigSettingProvider*);

                
        
        void chunkCallback_( CIMRequestMessage*, CIMResponseMessage* ),
             indicationCallback_( CIMProcessIndicationRequestMessage* );
  
        //ProviderName mkpvdrname_();
        //Message *mkMessage_( Message*, const CIMName&, Uint32);
        //Message *mkMessage_( const cimSubscription& );

        bool isvalidsubscription_(const cimSubscription& ),
             isduplicatesubscription_(const cimSubscription& );
        subscri_item *findsubscription_(const cimSubscription& );

        Message *do_gc_ ( CIMRequestMessage* ),
                *do_mc_ ( CIMRequestMessage* ),
                *do_dc_ ( CIMRequestMessage* ),
                *do_cc_ ( CIMRequestMessage* ),
                *do_gi_ ( CIMRequestMessage* ),        
                *do_mi_ ( CIMRequestMessage* ),
                *do_di_ ( CIMRequestMessage* ),
                *do_ci_ ( CIMRequestMessage* ),
                *do_ec_ ( CIMRequestMessage* ),
                *do_ecn_( CIMRequestMessage* ),
                *do_ei_ ( CIMEnumerateInstancesRequestMessage* ),
                *do_ein_( CIMRequestMessage* ),
                *do_ea_ ( CIMRequestMessage* ),
                *do_ean_( CIMRequestMessage* ),
                *do_er_ ( CIMRequestMessage* ),
                *do_ern_( CIMRequestMessage* ),
                *do_gp_ ( CIMRequestMessage* ),
                *do_sp_ ( CIMRequestMessage* ),
                *do_gq_ ( CIMRequestMessage* ),
                *do_sq_ ( CIMRequestMessage* ),
                *do_dq_ ( CIMRequestMessage* ),
                *do_eq_ ( CIMRequestMessage* ),
                *do_invoke_( CIMRequestMessage* ),
                *do_query_ ( CIMRequestMessage* );
 
        Message *forwardRequestForAggregation_( 
                      const String&       serviceName,
                      const String&       controlProviderName,
                      CIMRequestMessage  *request,
                      OperationAggregate *poA,
                      CIMResponseMessage *response = NULL ),
			 
                *forwardRequestToProvider_(
                      const CIMName&      className,    
                      const String&       serviceName,
                      const String&       controlProviderName,
                      CIMRequestMessage  *request,
                      CIMRequestMessage  *requestCopy = NULL ),
				
                *forwardRequestToService_(
                      const String&       serviceName,
                      CIMRequestMessage  *request,
                      CIMRequestMessage  *requestCopy = NULL);
		
        void handleEnumerateInstancesResponseAggregation(     // need?
                      OperationAggregate * ), 
             handleEnumerateInstanceNamesResponseAggregation( // need?
                      OperationAggregate * ); 


    }; //CIMDirectAccessRep




PEGASUS_NAMESPACE_END
#endif /* _CIMDirectAccessRep_h */
