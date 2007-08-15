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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Compiler/cmdline/cimmof/cmdlineExceptions.h>
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
#include <Pegasus/ControlProviders/Statistic/CIMOMStatDataProvider.h>

// NOCHKSRC
#include <Pegasus/ControlProviders/ConfigSettingProvider/ConfigSettingProvider.h>
// DOCHKSRC

#include <Pegasus/Client/CIMClientRep.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/XmlWriter.h>          // used only for test
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/ProviderManager2/Default/DefaultProviderManager.h>
#include <Pegasus/Common/Thread.h>
#include "CIMDirectAccess.h"
#include "CIMDirectAccessRep.h"
#include <Pegasus/Server/QuerySupportRouter.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include <Pegasus/WQL/WQLQueryExpressionRep.h>
#include <Pegasus/WQL/WQLInstancePropertySource.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/Common/OperationContext.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

#ifndef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL
# error "CIMDirectAccessRep.cpp should not be compiling without this flag"
#endif

#if !(PEGASUS_DIRECTACCESS_BUILDTYPE >= dacimINTEGRATED && \
      PEGASUS_DIRECTACCESS_BUILDTYPE <= dacimSTANDALONE)
# error  "PEGASUS_DIRECTACCESS_BUILDTYPE must have value in range."
#endif


PEGASUS_CLIENT_LINKAGE bool runtime_context_is_directaccess_cim = false;
template<class ObjectClass> void removePropagatedAndOriginAttributes(
    ObjectClass& ); // CIMord
static String _showPropertyList(const CIMPropertyList&);

static void *reposi__ = NULL,
    *interoppvdr__ = NULL,
    *nspvdr__ = NULL,
    *statdatapvdr__ = NULL;
static CIMOperationRequestDispatcher *opreqdispatch__;
static CIMDirectAccessRep *this__ = NULL;

static CIMDirectAccessRep *_dacim_ = NULL;
AtomicInt CIMDirectAccessRep::refcount_(0);
Mutex CIMDirectAccessRep::arequestlock_;
bool _directaccess_redirect;
void _cleanup(int i, void *)
{
    cout << "At Cleanup Ref = " << _dacim_->refcount_.get() << endl;
    if (_dacim_ && _dacim_->refcount_.get() > 0)
    {

        _dacim_->refcount_=0;
        cout << "At Cleanup Ref rest to = " << _dacim_->refcount_.get() << endl;
        //_dacim_->release();
        _dacim_->~CIMDirectAccessRep();
        _dacim_ = NULL;
    }

}

//-------------------------------------------------------
CIMDirectAccessRep *CIMDirectAccessRep::get()
{
    AutoMutex a(CIMDirectAccessRep::arequestlock_);
    refcount_++;

    if (!_dacim_)
    {
        on_exit(_cleanup,0);

        //return
        _dacim_ = new CIMDirectAccessRep;
        PEGASUS_ASSERT(_dacim_->opreqdispatch_ == opreqdispatch__);
        PEGASUS_ASSERT(_dacim_ == this__);
        return _dacim_;
    }
    PEGASUS_ASSERT(_dacim_->opreqdispatch_ == opreqdispatch__);
    PEGASUS_ASSERT(_dacim_ == this__);
    return _dacim_;
}

void CIMDirectAccessRep::release()
{
    AutoMutex a(CIMDirectAccessRep::arequestlock_);
    cout << "Reference = " << refcount_.get() << endl;
    if (refcount_.get() > 0)
    {
        refcount_--;
    }
    if (refcount_.get() == 0)
    {
        _dacim_ = NULL;
        cout << "Calling delete" << endl;
        delete this;
    }
    else
    {
        PEGASUS_ASSERT(opreqdispatch_ == opreqdispatch__);
        PEGASUS_ASSERT(this == this__);
    }
}

bool CIMDirectAccessRep::isCMPIInterface(CIMRequestMessage* request)
{
    String interfaceType;
    CIMInstance providerModule;
    ProviderIdContainer pidc = (ProviderIdContainer)
        request->operationContext.get(ProviderIdContainer::NAME);
    providerModule = pidc.getModule();
    //request->operationContext.get("requestor");
    bool remoteNameSpaceRequest=pidc.isRemoteNameSpace();
    CIMValue itValue = providerModule.getProperty(
        providerModule.findProperty("InterfaceType")).getValue();
    itValue.get(interfaceType);
    if (interfaceType == "CMPI")
    {
        CIMValue userContextValue = providerModule.getProperty(
            providerModule.findProperty("UserContext")).getValue();
        if (userContextValue == "Requestor")
        {
            cout<<"UserContext is A Requestor"<<endl;
            return true;
        }
        else
        {
           cout<<"UserContext is Not A Requestor"<<endl;
           // return false;
        }
        return true;
    }
    else
    {
        return false;
    }

}

#define _coutid_ " "<<__FILE__<<"@"<<__LINE__<<": "
//#if 0
//---------------------------------------------------------------------
static void chunkCallback(CIMRequestMessage*, CIMResponseMessage*)
{
    cout << _coutid_ << "fix chunkCallback()" << endl;
}
//------------------------------------------------------
static void indicationCallback(CIMProcessIndicationRequestMessage *indimsg)
{
    cout << _coutid_ << "fix indicationCallback()" << endl;
}
//---------------------------------------------------------------------
void CIMDirectAccessRep::chunkCallback_(
    CIMRequestMessage*,
    CIMResponseMessage* )
{
    cout << _coutid_ << "fix chunkCallback()" << endl;
}
//---------------------------------------------------------------------
void CIMDirectAccessRep::indicationCallback_(
    CIMProcessIndicationRequestMessage *indimsg)
{
    cout << _coutid_ << "fix indicationCallback()" << endl;
}



//-------------------------------------------------------------
static Message * controlProviderReceiveMessageCallback(
    Message * message,
    void * instance)
{
    CIMRequestMessage* request = dynamic_cast<CIMRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AcceptLanguageList* langs = new AcceptLanguageList(
        ((AcceptLanguageListContainer) request->operationContext.get(
        AcceptLanguageListContainer::NAME)).getLanguages());
    Thread::setLanguages(langs);

    ProviderMessageHandler* pmh =
        reinterpret_cast<ProviderMessageHandler*>(instance);
    return pmh->processMessage(request);
}
//#endif


#if 0
//-------------------------------------------------
const IdentityContainer& gettainer_identity()
{
    // put here, the platform-specific logic to determine id for process
    // we're running in.
    String s("root");
    const IdentityContainer& id_c( s );
    return id_c;
}

//-------------------------------------------------
const ProviderIdContainer& gettainer_pvdrId()
{
    //
    CIMInstance pvdr_module(CIMName("PG_ProviderModule"));
    pvdr_module.addProperty( CIMProperty(
        CIMName("Location"), String("location"))); // >>> fix
    CIMInstance pvdr(CIMName("PG_Provider"));
    pvdr.addProperty(CIMProperty(
        CIMName("Name"), String("PvdrName")));       // >>> fix
    ProviderIdContainer pvdrid_c(pvdr_module, pvdr);
    ProviderIdContainer& pvdrid_c2(pvdrid_c);
    return pvdrid_c2;                               // >>>fix
}

#endif


#define mkresponse(_t) \
    responsemsg_ = rm->buildResponse(); \
    responsemsg_->setType(_t);


//-------------------------------------------------------------------
Message *CIMDirectAccessRep::dorequest( AutoPtr<CIMRequestMessage>& req )
{

    AutoMutex a(CIMDirectAccessRep::arequestlock_);

    runtime_context_is_directaccess_cim = true;
    CIMRequestMessage *rm = req.get();
    //Message *response = NULL;


    CIMOperationRequestMessage *orm =
        dynamic_cast<CIMOperationRequestMessage*>(rm);
    PEGASUS_ASSERT(orm != NULL);


    //String s("root");
    const IdentityContainer& id_c(orm->userName);
    rm->operationContext.insert(id_c);
    orm->queueIds.push(MessageQueue::getNextQueueId());
    //MSA
    // above is needed to avoid stack
    // underflow in a QueueIdStack method

    CIMException cimexcep;
        // keep the following 'if else' in order of most common msg types
        // before less common.  And put the least common in the switch.

        if (rm->getType() == CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE)
        {
            responsemsg_ =
                do_ein_((CIMEnumerateInstanceNamesRequestMessage*)rm);
        }
        else if (rm->getType() == CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE)
        {
            responsemsg_ = do_ei_( (CIMEnumerateInstancesRequestMessage*)rm );
            PEGASUS_ASSERT(opreqdispatch_ == opreqdispatch__);
            PEGASUS_ASSERT(this == this__);
        }
        else if (rm->getType() == CIM_GET_INSTANCE_REQUEST_MESSAGE)
        {
            responsemsg_ = do_gi_(rm);
        }
        else if (rm->getType() == CIM_GET_CLASS_REQUEST_MESSAGE)
        {
            responsemsg_ = do_gc_(rm);
        }
        else if (rm->getType() == CIM_GET_PROPERTY_REQUEST_MESSAGE)
        {
            responsemsg_ = do_gp_(rm);
        }
        else if (rm->getType() == CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE)
        {
            responsemsg_ = do_ecn_(rm);
        }
        else if (rm->getType() == CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE)
        {
            responsemsg_ = do_ec_(rm);
        }
        else if (rm->getType() == CIM_HANDLE_INDICATION_REQUEST_MESSAGE)
        {
            mkresponse(DIRECTACCESSCIM_NOTSUPPORTED_TEMP)
        }
        else
        {
            switch (rm->getType())
            {
                case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
                    responsemsg_ = do_ci_(rm);
                    break;
                case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
                    responsemsg_ = do_di_(rm);
                    break;
                case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
                    responsemsg_ = do_mi_(rm);
                    break;
                case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
                    responsemsg_ = do_gq_(rm);
                    break;
                case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
                    responsemsg_ = do_eq_(rm);
                    break;
                case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
                    responsemsg_ = do_invoke_(rm);
                    break;
#if 0
                case CIM_DELETE_CLASS_REQUEST_MESSAGE:
                    responsemsg_ = do_dc_(rm);
                    break;
                case CIM_CREATE_CLASS_REQUEST_MESSAGE:
                    responsemsg_ = do_cc_(rm);
                    break;
                case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
                    responsemsg_ = do_mc_(rm);
                    break;
                case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
                    responsemsg_ = do_sq_(rm);
                    break;
               case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
                    responsemsg_ = do_dq_(rm);
                    break;
#endif

                case CIM_SET_PROPERTY_REQUEST_MESSAGE:
                    responsemsg_ = do_sp_(rm);
                    break;
                case CIM_ASSOCIATORS_REQUEST_MESSAGE:
                    responsemsg_ = do_ea_(rm);
                    break;
                case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
                    responsemsg_ = do_ean_(rm);
                    break;
                case CIM_REFERENCES_REQUEST_MESSAGE:
                    responsemsg_ = do_er_(rm);
                    break;
                case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
                    responsemsg_ = do_ern_(rm);
                    break;
                case CIM_EXEC_QUERY_REQUEST_MESSAGE:
                    responsemsg_ = do_query_(rm);
                    break;

                case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
                case CIM_PROCESS_INDICATION_REQUEST_MESSAGE:
                case CIM_HANDLE_INDICATION_REQUEST_MESSAGE:
                case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
                case CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE:
                case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
                case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
                case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
                case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
                case CIM_ENABLE_MODULE_REQUEST_MESSAGE:
                case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
                    mkresponse(DIRECTACCESSCIM_NOTSUPPORTED_TEMP)
                    break;

                default:
                    mkresponse(DIRECTACCESSCIM_NOTSUPPORTED_REQUEST) // fix
            }
        }
    PEGASUS_ASSERT(opreqdispatch_ == opreqdispatch__);
    PEGASUS_ASSERT(this == this__);

    if (cimexcep.getCode() != CIM_ERR_SUCCESS)
    {
        throw cimexcep;        // throw here, or use return via Message?
    }

    runtime_context_is_directaccess_cim = false;    // neded?

    return responsemsg_;
} // dorequest()


                  /*---------------------------
                   *                           *
                   *                           *
                   *          class            *
                   *                           *
                   *                           *
                   *---------------------------*/


//----------------------------------------------------------------
Message *CIMDirectAccessRep::do_gc_(CIMRequestMessage *req)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_gc_()");
    CIMGetClassRequestMessage *gcr = (CIMGetClassRequestMessage*)req;
    AutoPtr<CIMGetClassResponseMessage> response(
        dynamic_cast<CIMGetClassResponseMessage*>(gcr->buildResponse()));
    CIMException cimException;

    try
    {
        response->cimClass = reposi_->getClass(
            gcr->nameSpace,
            gcr->className,
            gcr->localOnly,
            gcr->includeQualifiers,
            gcr->includeClassOrigin,
            gcr->propertyList);
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::handleGetClassRequest - Namespace: "
            "$0  Class name: $1",
            gcr->nameSpace.getString(),
            gcr->className.getString()));
    }
    catch(const CIMException& exception)
    {
        cimException = exception;
    }
    catch(const Exception& exception)
    {
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
    }
    catch(...)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->cimException = cimException;
    PEG_METHOD_EXIT();
    return response.release();
} // do_gc_()




//-----------------------------------------------------------------------
Message *CIMDirectAccessRep::do_ecn_(CIMRequestMessage* req)
{

    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_ecn_()");

    CIMEnumerateClassNamesRequestMessage *ecnr =
        (CIMEnumerateClassNamesRequestMessage*)req;

    AutoPtr<CIMEnumerateClassNamesResponseMessage> response(
        dynamic_cast<CIMEnumerateClassNamesResponseMessage*>(
        ecnr->buildResponse()));

    Array<CIMName> classNames;
    CIMException cimException;

    try
    {
        classNames = reposi_->enumerateClassNames(
            ecnr->nameSpace,
            ecnr->className,
            ecnr->deepInheritance);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::do_ecn_() - "
            "Namespace: $0  Class name: $1",
            ecnr->nameSpace.getString(),
            ecnr->className.getString()));
    }
    catch(const CIMException& exception)
    {
        cimException = exception;
    }
    catch(const Exception& exception)
    {
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
    }
    catch(...)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->classNames = classNames;
    response->cimException = cimException;
    PEG_METHOD_EXIT();
    return response.release();
} // do_ecn_()


//--------------------------------------------------------------------
Message *CIMDirectAccessRep::do_ec_(CIMRequestMessage* req)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_ec_()");

    CIMEnumerateClassesRequestMessage *ecr =
            (CIMEnumerateClassesRequestMessage*)req;

    AutoPtr<CIMEnumerateClassesResponseMessage> response(
        dynamic_cast<CIMEnumerateClassesResponseMessage*>(
        ecr->buildResponse()));

    Array<CIMClass> cimClasses;
    CIMException cimException;

    try
    {
        cimClasses = reposi_->enumerateClasses(
            ecr->nameSpace,
            ecr->className,
            ecr->deepInheritance,
            ecr->localOnly,
            ecr->includeQualifiers,
            ecr->includeClassOrigin);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::handleEnumerateClassesRequest - "
            "Namespace: $0  Class name: $1",
            ecr->nameSpace.getString(),
            ecr->className.getString()));
    }
    catch(const CIMException& exception)
    {
        cimException = exception;
    }
    catch(const Exception& exception)
    {
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
    }
    catch(...)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->cimClasses = cimClasses;
    response->cimException = cimException;
    PEG_METHOD_EXIT();
    return response.release();
} // do_ec_()



#if 0
//-------------------------------------------------------------
Message *CIMDirectAccessRep::do_dc_( CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_dc_()");
    CIMDeleteClassRequestMessage *req =
        (CIMDeleteClassRequestMessage*) request;
    AutoPtr<CIMDeleteClassResponseMessage> response(
        dynamic_cast<CIMDeleteClassResponseMessage*>(
        req->buildResponse()));
    CIMException cimException;

    try
    {
        //StatProviderTimeMeasurement providerTime(response.get());

        reposi_->deleteClass( req->nameSpace, req->className);
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::do_dc_() - Namespace: $0  Class Name: $1",
            req->nameSpace.getString(),
            req->className.getString()));
    }
    catch(const CIMException& exception)
    {
        cimException = exception;
    }
    catch(const Exception& exception)
    {
        cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            exception.getMessage());
    }
    catch(...)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->cimException = cimException;
    //_enqueueResponse(request, response.release());
    PEG_METHOD_EXIT();
    return response.release();
} // do_dc_()


//#if 0
//-----------------------------------------------------------------
Message *CIMDirectAccessRep::do_cc_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"CIMDirectAccessRep::do_cc_()");
    CIMCreateClassRequestMessage *req =
        (CIMCreateClassRequestMessage*)request;
    AutoPtr<CIMCreateClassResponseMessage> response(
        dynamic_cast<CIMCreateClassResponseMessage*>(
        req->buildResponse()));

    CIMException cimException;

    try
    {
        removePropagatedAndOriginAttributes(req->newClass);

        //StatProviderTimeMeasurement providerTime(response.get());

        reposi_->createClass(
            req->nameSpace,
            req->newClass,
            ((ContentLanguageListContainer)req->operationContext.get(
            ContentLanguageListContainer::NAME)).getLanguages());

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::do_cc_() - Namespace: $0  Class Name: $1",
            req->nameSpace.getString(),
            req->newClass.getClassName().getString()));
    }
    catch(const CIMException& exception)
    {
        cimException = exception;
    }
    catch(const Exception& exception)
    {
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
    }
    catch(...)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->cimException = cimException;

    //_enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
    return response.release();
} // do_cc_()




//--------------------------------------------------------------
Message *CIMDirectAccessRep::do_mc_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleModifyClassRequest");
    CIMModifyClassRequestMessage *req =
        (CIMModifyClassRequestMessage*)request;
    AutoPtr<CIMModifyClassResponseMessage> response(
        dynamic_cast<CIMModifyClassResponseMessage*>(
        req->buildResponse()));

    CIMException cimException;

    try
    {
        removePropagatedAndOriginAttributes(req->modifiedClass);

        //StatProviderTimeMeasurement providerTime(response.get());

        reposi_->modifyClass(
            req->nameSpace,
            req->modifiedClass,
            ((ContentLanguageListContainer)req->operationContext.get(
            ContentLanguageListContainer::NAME)).getLanguages());
    }
    catch(const CIMException& exception)
    {
        cimException = exception;
    }
    catch(const Exception& exception)
    {
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
    }
    catch(...)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->cimException = cimException;

    //_enqueueResponse(request, response.release());
    PEG_METHOD_EXIT();
    return response.release();
} // do_mc_()

#endif

                  /*---------------------------
                   *                           *
                   *                           *
                   *       instance            *
                   *                           *
                   *                           *
                   *---------------------------*/




//---------------------------------------------------------------------------
Message* CIMDirectAccessRep::do_ei_(CIMEnumerateInstancesRequestMessage *req)
{

    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::_do_ei_()");

    // get the class name
    CIMName className = req->className;
    CIMException checkClassException;

    PEGASUS_ASSERT(opreqdispatch_ == opreqdispatch__);
    PEGASUS_ASSERT(this == this__);

    CIMClass cimClass = opreqdispatch_->_getClass(
        req->nameSpace,
        className,
        checkClassException);

    PEGASUS_ASSERT(opreqdispatch_ == opreqdispatch__);
    PEGASUS_ASSERT(this == this__);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = checkClassException;

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

//#ifdef DACIM_DEBUG
//    CDEBUG("CIMOP ei client propertyList = " <<
 //       _showPropertyList(request->propertyList));
//#endif

    // If DeepInheritance==false and no PropertyList was specified by the
    // client, the provider PropertyList should contain all the properties
    // in the specified class.
    if (!req->deepInheritance && req->propertyList.isNull())
    {
        Array<CIMName> propertyNameArray;
        Uint32 numProperties = cimClass.getPropertyCount();
        for (Uint32 i = 0; i < numProperties; i++)
        {
            propertyNameArray.append(cimClass.getProperty(i).getName());
        }

        req->propertyList.set(propertyNameArray);
    }

    //CDEBUG("CIMOP ei provider propertyList = " <<
    //    _showPropertyList(req->propertyList));

    //
    // Get names of descendent classes:
    //
    CIMException cimException;
    Array<ProviderInfo> providerInfos;

    Uint32 providerCount;

    // Get list of providers.
    try
    {
        //CDEBUG("Looking up Instance Providers");
        providerInfos = opreqdispatch_->_lookupAllInstanceProviders(
            req->nameSpace,
            className,
            providerCount);
    }
    catch(const CIMException& exception)
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = exception;

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    Uint32 toIssueCount = providerInfos.size();

    // Test for "enumerate too Broad" and if so, execute exception.
    // This limits the number of provider invocations, not the number
    // of instances returned.
    if (providerCount > opreqdispatch_->_maximumEnumerateBreadth)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "Request-too-broad exception.  Namespace: $0  Class Name: $1  "
            "Limit: $2  ProviderCount: $3",
            req->nameSpace.getString(),
            req->className.getString(),
            opreqdispatch_->_maximumEnumerateBreadth,
            providerCount));

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            Formatter::format(
            "ERROR Enumerate too broad for class $0. Limit = $1, "
            "Request = $2",
            req->className.getString(),
            opreqdispatch_->_maximumEnumerateBreadth,
            providerCount));

        CIMResponseMessage* response = req->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_NOT_SUPPORTED, MessageLoaderParms(
            "Server.CIMDirectAccessRep.ENUM_REQ_TOO_BROAD",
            "Enumerate request too Broad"));

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED

    if ((providerCount == 0) && !(reposi_->isDefaultInstanceProvider()))
    {
        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            "CIM_ERROR_NOT_SUPPORTED for " + req->className.getString());

        CIMResponseMessage* response = req->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    //
    // Get names of descendent classes:
    //
    //CIMException cimException;
    //Array<ProviderInfo> providerInfos;


    CIMEnumerateInstancesRequestMessage *eirm =
        new CIMEnumerateInstancesRequestMessage(*req);

    // We have instances for Providers and possibly repository.
    // Set up an aggregate object and save a copy of the original request.
    OperationAggregate* poA = new OperationAggregate(
        new CIMEnumerateInstancesRequestMessage(*req),
        req->getType(),
        req->messageId,
        req->queueIds.top(),
        req->className);

    poA->_aggregationSN = opreqdispatch_->cimOperationAggregationSN++;
    Uint32 numClasses = providerInfos.size();


    CIMResponseMessage *outerresponse = req->buildResponse();
    // gather up the repository responses and send it to out as one response
    // with many instances
    if (reposi_->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++)        // ei loop 1
        {
            ProviderInfo& providerInfo = providerInfos[i];

            // this class is registered to a provider - skip
            if (providerInfo.hasProvider)
            {
                continue;
            }

            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                Formatter::format(
                "EnumerateInstances Req. class $0 to repository, "
                "No $1 of $2, SN $3",
                providerInfo.className.getString(),
                i,
                numClasses,
                poA->_aggregationSN));

            AutoPtr<CIMEnumerateInstancesResponseMessage> response(
                dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
                req->buildResponse()));

            CIMException cimException;
            Array<CIMInstance> cimNamedInstances;

            try
            {
                //StatProviderTimeMeasurement providerTime(response.get());
                // Enumerate instances only for this class
                cimNamedInstances = reposi_->enumerateInstancesForClass(
                    req->nameSpace,
                    providerInfo.className,
                    req->localOnly,
                    req->includeQualifiers,
                    req->includeClassOrigin,
                    req->propertyList);
            }
            catch(const CIMException& exception)
            {
                cimException = exception;
            }
            catch(const Exception& exception)
            {
                cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch(...)
            {
                cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    String::EMPTY);
            }

            response->cimNamedInstances = cimNamedInstances;
            response->cimException = cimException;

            poA->appendResponse(response.release());
        } // for all classes and derived classes  -- loop 1  ei
                                                 //  loop 1
                                                 //  loop 1
                                                 //
                                                 //

        Uint32 numberResponses = poA->numberResponses();
        Uint32 totalIssued = providerCount + (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            //handleEnumerateInstancesResponseAggregation(poA);   ????

            CIMResponseMessage* response = poA->removeResponse(0);
            forwardRequestForAggregation_(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMEnumerateInstancesRequestMessage(*req),
                poA,
                response);
        }
    } // if isDefaultInstanceProvider
    else
    {
        // Set the number of expected responses in the OperationAggregate
        poA->setTotalIssued(providerCount);
    }

    //CDEBUG("Before Loop to send requests. numClasses = " << numClasses);



    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)         // loop 2 ei
                                                    // loop 2 ei
    {

        ProviderInfo& providerInfo = providerInfos[i];

        // this class is NOT registered to a provider - skip
        if ( !providerInfo.hasProvider )
        {
            continue;
        }

        if (providerInfo.className == "CIM_Namespace" &&
            (providerInfo.controlProviderName ==
            "ControlService::InteropProvider"))
        {
            continue;
        }

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            Formatter::format(
            "EnumerateInstances Req. class $0 to svc \"$1\" for control "
            "provider \"$2\", No $3 of $4, SN $5",
            providerInfo.className.getString(),
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            i,
            numClasses,
            poA->_aggregationSN));

        CIMEnumerateInstancesRequestMessage *requestCopy =
            new CIMEnumerateInstancesRequestMessage(*req);   //why??

        requestCopy->className = providerInfo.className;

        CIMException checkClassException;

        CIMClass cimClass = opreqdispatch_->_getClass(
            req->nameSpace,
            providerInfo.className,
            checkClassException);

        // The following is not correct. Need better way to terminate.
        if (checkClassException.getCode() != CIM_ERR_SUCCESS)
        {
            CIMResponseMessage* response = req->buildResponse();

            forwardRequestForAggregation_(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMEnumerateInstancesRequestMessage(*req),   // why ya?
                poA,
                response);
        }

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy /*req*/ ->operationContext.insert(
                *(providerInfo.providerIdContainer.get()));
            if (! isCMPIInterface(requestCopy))
            {
                cout<<"Not a CMPI Method"<<endl;
                throw CIMException(
                    CIM_ERR_DACIM_NOT_SUPPORTED,
                    "DIRECTACCESS NOT SUPPORTED");
            }
        }

        //if ( !req->operationContext.contains() )
        //{
        //    req->operationContext.insert();
        //}

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
        if (opreqdispatch_->_enableNormalization &&
            providerInfo.hasProviderNormalization)
        {
            requestCopy /*req*/ ->operationContext.insert(
                CachedClassDefinitionContainer(cimClass));
        }
#endif
                                                          // is ei
        const ProviderIdContainer *pid =
            providerInfo.providerIdContainer.get();
        if (pid)
        {
            const CIMName mod = pid->getModule().getClassName();
            const CIMName pvdr = pid->getProvider().getClassName();
        }

        if (checkClassException.getCode() == CIM_ERR_SUCCESS)
        {
#ifdef DACIM_DEBUG
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                Formatter::format(
                "EnumerateInstances Req. Fwd class $0 to svc \"$1\" for "
                "control provider \"$2\", PropertyList= $3",
                providerInfo.className.getString(),
                providerInfo.serviceName,
                providerInfo.controlProviderName,
                _showPropertyList(req->propertyList)));
#endif
            forwardRequestForAggregation_(
                providerInfo.serviceName,
                providerInfo.controlProviderName,
                requestCopy,    // or req??  why need copy?
                poA,
                outerresponse);
        }
    } // for all classes and dervied classes -- loop 2 ei

    opreqdispatch_->handleEnumerateInstancesResponseAggregation(poA);
    outerresponse = poA->removeResponse(Uint32(0));
    outerresponse->setComplete(true);

    PEG_METHOD_EXIT();
    return outerresponse;
} // do_ei_()



//------------------------------------------------------------------------
Message* CIMDirectAccessRep::do_ein_(CIMRequestMessage* req)
{
//verify(__FILE__,__LINE__);

    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_ein_()");
    CIMEnumerateInstanceNamesRequestMessage *request =
        (CIMEnumerateInstanceNamesRequestMessage*)req;
    CIMName className = request->className;

    CIMException checkClassException;

    CIMClass cimClass = opreqdispatch_->_getClass(
        request->nameSpace,
        className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        //_enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return response;
    }

    //
    // Get names of descendent classes:
    //
    CIMException cimException;
    Array<ProviderInfo> providerInfos;

    // This gets set by _lookupAllInstanceProviders()
    Uint32 providerCount;

    try
    {
        providerInfos = opreqdispatch_->_lookupAllInstanceProviders(
            request->nameSpace,
            request->className,
            providerCount);
    }
    catch(const CIMException& exception)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = exception;

        //_enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return response;
    }


    // Test for "enumerate too Broad" and if so, execute exception.
    // This limits the number of provider invocations, not the number
    // of instances returned.
    if (providerCount > opreqdispatch_->_maximumEnumerateBreadth)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "Request-too-broad exception.  Namespace: $0  "
            "Class Name: $1 Limit: $2  ProviderCount: $3",
            request->nameSpace.getString(),
            request->className.getString(),
            opreqdispatch_->_maximumEnumerateBreadth,
            providerCount));

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            Formatter::format(
            "ERROR Enumerate too broad for class $0. "
            "Limit = $1, Request = $2",
            request->className.getString(),
            opreqdispatch_->_maximumEnumerateBreadth,
            providerCount));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_NOT_SUPPORTED,
            MessageLoaderParms(
            "Server.CIMDirectAccessRep.ENUM_REQ_TOO_BROAD",
            "Enumerate request too Broad"));


        //_enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return response;
    }

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED
    if ((providerCount == 0) && !(reposi_->isDefaultInstanceProvider()))
    {
        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            "CIM_ERROR_NOT_SUPPORTED for " + request->className.getString());

        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);


        //_enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return response;
    }


    // We have instances for Providers and possibly repository.
    // Set up an aggregate object and save a copy of the original request.
    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstanceNamesRequestMessage(*request),
        request->getType(),
        request->messageId,
        request->queueIds.top(),
        request->className);

    poA->_aggregationSN = opreqdispatch_->cimOperationAggregationSN++;
    Uint32 numClasses = providerInfos.size();


    CIMResponseMessage *globresponse = request->buildResponse();
    CIMEnumerateInstanceNamesResponseMessage* providerResponse =
        dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>
        (request->buildResponse());

    if (reposi_->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++) // loop 1  ein
                                                // loop 1  ein
                                                // loop 1  ein
        {
            ProviderInfo& providerInfo = providerInfos[i];


            // this class is registered to a provider - skip
            if (providerInfo.hasProvider)
            {
                continue;
            }

            // If this class does not have a provider

            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                Formatter::format(
                "EnumerateInstanceNames Req. class $0 to repository, "
                "No $1 of $2, SN $3",
                providerInfo.className.getString(),
                i,
                numClasses,
                poA->_aggregationSN));

            AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response(
                dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
                request->buildResponse()));

            try
            {
                // Enumerate instances only for this class
                response->instanceNames =
                    reposi_->enumerateInstanceNamesForClass(
                    request->nameSpace,
                    providerInfo.className);
                providerResponse->instanceNames.appendArray(
                    response->instanceNames);
            }
            catch(const CIMException& exception)
            {
                response->cimException = exception;
            }
            catch(const Exception& exception)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch(...)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    String::EMPTY);
            }

            poA->appendResponse(response.release());

        } // for all classes and derived classes -- loop 1 ein
                                                 // loop 1 ein -- end

        Uint32 numberResponses = poA->numberResponses();
        Uint32 totalIssued = providerCount + (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            //opreqdispatch_->handleEnumerateInstanceNamesResponseAggregation(
            // poA);
            // ????????????  why???
            CIMResponseMessage *response = poA->removeResponse(0);
        }
    } // if isDefaultInstanceProvider
    else
    {
        // Set the number of expected responses in the OperationAggregate
        poA->setTotalIssued(providerCount);
    }

    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)  // loop 2 ein
                                             // loop 2 ein
                                             // loop 2 ein
    {
        ProviderInfo& providerInfo = providerInfos[i];

        // this class is NOT registered to a provider - skip
        if (!providerInfo.hasProvider)
        {
            continue;
        }

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            Formatter::format(
            "EnumerateInstanceNames Req. class $0 to svc \"$1\" for "
            "control provider \"$2\", No $3 of $4, SN $5",
            providerInfo.className.getString(),
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            i,
            numClasses,
            poA->_aggregationSN));

        CIMEnumerateInstanceNamesRequestMessage* requestCopy =
            new CIMEnumerateInstanceNamesRequestMessage(*request);

        requestCopy->className = providerInfo.className;

        CIMException checkClassException;

        CIMClass cimClass = opreqdispatch_->_getClass(
            request->nameSpace,
            providerInfo.className,
            checkClassException );

        // The following is not correct. Need better way to terminate.
        if (checkClassException.getCode() != CIM_ERR_SUCCESS)
        {
            CIMResponseMessage* response = request->buildResponse();
            CIMEnumerateInstanceNamesResponseMessage* aggResp =
                dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
                forwardRequestForAggregation_(   // loop 2 ein cim err
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMEnumerateInstanceNamesRequestMessage(*request),
                poA,
                response));
            if (aggResp)
            {
                while (!globresponse->isComplete())
                {
                }

                providerResponse->instanceNames.appendArray(
                    providerResponse->instanceNames);
            }
        }

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(         // <-- note 'Copy'
                *(providerInfo.providerIdContainer.get()));
        }
        const ProviderIdContainer* pid = providerInfo.providerIdContainer.get();

        if (pid)
        {
            const CIMName mod = pid->getModule().getClassName();
            const CIMName pvdr = pid->getProvider().getClassName();
        }

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
        if (opreqdispatch_->_enableNormalization &&
            providerInfo.hasProviderNormalization)
        {
            requestCopy->operationContext.insert(
                CachedClassDefinitionContainer(cimClass));
            if (! isCMPIInterface(requestCopy))
            {
                cout<<"Not a CMPI Method"<<endl;
                throw CIMException(
                    CIM_ERR_DACIM_NOT_SUPPORTED,
                    "DIRECTACCESS NOT SUPPORTED");
            }

        }
#endif

        if (checkClassException.getCode() == CIM_ERR_SUCCESS)
        {
            CIMEnumerateInstanceNamesResponseMessage* aggResp =
                dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
                forwardRequestForAggregation_(          // loop 2 ein
                providerInfo.serviceName,
                providerInfo.controlProviderName,
                requestCopy,
                poA,
                globresponse));
            if (aggResp)
            {
                if (aggResp->cimException.getCode() != CIM_ERR_SUCCESS)
                {
                    providerResponse->cimException = aggResp->cimException;
                    return providerResponse;
                }

                // CIMClient need not know neither the namespace nor the host
                // name in the response as the client would have specified them
                // in the CIM Request. So reset the host and namespace to NULL
                /*for (
                    Uint32 index =0;
                    index < aggResp->instanceNames.size();
                    index++)
                {
                    aggResp->instanceNames[index].setHost(String::EMPTY);
                    CIMNamespaceName name;
                    aggResp->instanceNames[index].setNameSpace(name);
                }
                */
                while (!globresponse->isComplete())
                {
                }
                providerResponse->instanceNames.appendArray(
                    aggResp->instanceNames);

            }
        }
    } // for all classes and derived classes -- loop 2 ein

    // now, aggregate whatever responses are in poA.
    // opreqdispatch_->handleEnumerateInstanceNamesResponseAggregation(poA);
    // globresponse = poA->removeResponse(0);
    // globresponse->setComplete(true);
    for (
        Uint32 index =0;
        index < providerResponse->instanceNames.size();
        index++)
    {
        providerResponse->instanceNames[index].setHost(String::EMPTY);
        CIMNamespaceName name;
        providerResponse->instanceNames[index].setNameSpace(name);
    }

    PEG_METHOD_EXIT();
    //return globresponse;
    return providerResponse;

} // do_ein_()



//-----------------------------------------------------------------
Message* CIMDirectAccessRep::do_gi_(CIMRequestMessage* req)
{
    CIMGetInstanceRequestMessage *gir = (CIMGetInstanceRequestMessage*)req;

    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_gi_()");

    // ATTN: Need code here to expand partial instance!
    // get the class name
    CIMName className = gir->instanceName.getClassName();
    CIMException checkClassException;

    CIMClass cimClass = opreqdispatch_->_getClass(
        gir->nameSpace,
        className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::do_gi_() - "
            "CIM exist exception has occurred.  Namespace: $0  "
            "Class Name: $1",
            gir->nameSpace.getString(),
            className.getString()));

        CIMResponseMessage *response = gir->buildResponse();
        response->cimException = checkClassException;
        return response;
        PEG_METHOD_EXIT();
    }
    //String serviceName;
    //String controlProviderName;
    //ProviderIdContainer* providerIdContainer = 0;

    ProviderInfo providerInfo = opreqdispatch_->_lookupNewInstanceProvider(
        gir->nameSpace,
        className );

    if (providerInfo.hasProvider)
    {
        CIMGetInstanceRequestMessage* requestCopy =
            new CIMGetInstanceRequestMessage(*gir);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
            if (! isCMPIInterface(requestCopy))
            {
                cout<<"Not a CMPI Method"<<endl;
                throw CIMException(
                    CIM_ERR_DACIM_NOT_SUPPORTED,
                    "DIRECTACCESS NOT SUPPORTED");
            }

            //delete providerIdContainer;
            //providerIdContainer = 0;
        }

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
        if (opreqdispatch_->_enableNormalization &&
            providerInfo.hasProviderNormalization)
        {
            requestCopy->operationContext.insert(
                CachedClassDefinitionContainer(cimClass));
        }
#endif

        CIMGetInstanceRequestMessage* requestCallbackCopy =
            new CIMGetInstanceRequestMessage(*requestCopy);  // fix

        Message *rsp = forwardRequestToProvider_(
            providerInfo.className,
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy);

         PEG_METHOD_EXIT();
         return rsp;
    }


    // not internal or found provider, try default provider
    AutoPtr<CIMGetInstanceResponseMessage> response(
        dynamic_cast<CIMGetInstanceResponseMessage*>(
        gir->buildResponse()));

    if (reposi_->isDefaultInstanceProvider())
    {
        CIMException cimException;
        CIMInstance cimInstance;

        try
        {
            cimInstance = reposi_->getInstance(
                gir->nameSpace,
                gir->instanceName,
                gir->localOnly,
                gir->includeQualifiers,
                gir->includeClassOrigin,
                gir->propertyList);
        }
        catch(const CIMException& exception)
        {
            cimException = exception;
        }
        catch(const Exception& exception)
        {
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        response->cimInstance = cimInstance;
        response->cimException = cimException;
        return response.release();
    }
    else
    {
        // No provider is registered and the repository isn't the default
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
        return response.release();
    }

    PEG_METHOD_EXIT();
    return NULL;          // ebbfix
} // do_gi_()



//-----------------------------------------------------------------------
Message* CIMDirectAccessRep::do_ci_(CIMRequestMessage* request)
{

    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_ci_()");

    CIMCreateInstanceRequestMessage *req =
        (CIMCreateInstanceRequestMessage*)request;

    // get the class name
    CIMName className = req->newInstance.getClassName();

    CIMException checkClassException;

    opreqdispatch_->_checkExistenceOfClass(
        req->nameSpace,
        className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::do_ci_() - "
            "CIM exist exception has occurred.  Namespace: $0  "
            "Class Name: $1",
            req->nameSpace.getString(),
            className.getString()));

        CIMResponseMessage* response = req->buildResponse();
        response->cimException = checkClassException;

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    //String serviceName;
    //String controlProviderName;
    //ProviderIdContainer* providerIdContainer = 0;

    ProviderInfo providerInfo =
        opreqdispatch_->_lookupNewInstanceProvider(req->nameSpace, className);

    if (providerInfo.hasProvider)
    {
        CIMCreateInstanceRequestMessage* requestCopy =    // fix
            new CIMCreateInstanceRequestMessage(*req);

        removePropagatedAndOriginAttributes(requestCopy->newInstance);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
            if (! isCMPIInterface(requestCopy))
            {
                cout<<"Not a CMPI Method"<<endl;
                throw CIMException(
                    CIM_ERR_DACIM_NOT_SUPPORTED,
                    "DIRECTACCESS NOT SUPPORTED");
            }

            //delete providerIdContainer;
            //providerIdContainer = 0;
        }

        CIMCreateInstanceRequestMessage* requestCallbackCopy =
            new CIMCreateInstanceRequestMessage(*requestCopy);


        return forwardRequestToProvider_(
            providerInfo.className,
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();

    }
    else if (reposi_->isDefaultInstanceProvider())
    {
        AutoPtr<CIMCreateInstanceResponseMessage> response(
            dynamic_cast<CIMCreateInstanceResponseMessage*>(
            req->buildResponse()));

        CIMException cimException;
        CIMObjectPath instanceName;

        try
        {
            removePropagatedAndOriginAttributes(req->newInstance);

            //StatProviderTimeMeasurement providerTime(response.get());

            instanceName = reposi_->createInstance(
                req->nameSpace,
#if 1 // temp code; despite being a const parm, deep under createInstance()
      // it can change!   E.g. in CIMPropertyRep::resolve() (at least).
      // (this is temp for the classOrigin problem found in Client.cpp)
                req->newInstance.clone(),
#else
                req->newInstance,
#endif
                ((ContentLanguageListContainer)req->operationContext.get(
                ContentLanguageListContainer::NAME)).getLanguages());

            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "CIMDirectAccessRep::handleCreateInstanceRequest - "
                "Namespace: $0  Instance name: $1",
                req->nameSpace.getString(),
                req->newInstance.getClassName().getString()));
        }
        catch(const CIMException& exception)
        {
            cimException = exception;
        }
        catch(const Exception& exception)
        {
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        response->instanceName = instanceName;
        response->cimException = cimException;

        //_enqueueResponse(request, response.release());
        return response.release();
    }
    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        //_enqueueResponse(request, response);
        return response;
    }

    CIMResponseMessage *response = request->buildResponse();
    response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            "No provider, no default provider, no nothing");
    PEG_METHOD_EXIT();
    return response;
} // do_ci_()





//---------------------------------------------------------------------
Message* CIMDirectAccessRep::do_di_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_di_()");

    CIMDeleteInstanceRequestMessage *req =
        (CIMDeleteInstanceRequestMessage*)request;

    // get the class name
    CIMName className = req->instanceName.getClassName();

    CIMException checkClassException;

    opreqdispatch_->_checkExistenceOfClass(
        req->nameSpace,
        className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::do_ci_() - "
            "CIM exist exception has occurred.  Namespace: $0  "
            "Class Name: $1",
            req->nameSpace.getString(),
            className.getString()));

        CIMResponseMessage* response = req->buildResponse();
        response->cimException = checkClassException;

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    //String serviceName;
    //String controlProviderName;
    //ProviderIdContainer* providerIdContainer = 0;

    ProviderInfo providerInfo =
        opreqdispatch_->_lookupNewInstanceProvider(req->nameSpace, className);

    if (providerInfo.hasProvider)
    {
        CIMDeleteInstanceRequestMessage* requestCopy =
            new CIMDeleteInstanceRequestMessage(*req);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
            if (! isCMPIInterface(requestCopy))
            {
                cout<<"Not a CMPI Method"<<endl;
                throw CIMException(
                    CIM_ERR_DACIM_NOT_SUPPORTED,
                    "DIRECTACCESS NOT SUPPORTED");
            }

            //delete providerIdContainer;
            //providerIdContainer = 0;
        }

        CIMDeleteInstanceRequestMessage* requestCallbackCopy =
            new CIMDeleteInstanceRequestMessage(*requestCopy);    // fix

        return forwardRequestToProvider_(
            providerInfo.className,
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        //return;
    }
    else if (reposi_->isDefaultInstanceProvider())
    {
        AutoPtr<CIMDeleteInstanceResponseMessage> response(
            dynamic_cast<CIMDeleteInstanceResponseMessage*>(
            req->buildResponse()));

        CIMException cimException;

        try
        {
            //StatProviderTimeMeasurement providerTime(response.get());

            reposi_->deleteInstance(req->nameSpace, req->instanceName);

            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "CIMDirectAccessRep::do_di_() - "
                "Namespace: $0  Instance Name: $1",
                req->nameSpace.getString(),
                req->instanceName.toString()));
        }
        catch(const CIMException& exception)
        {
            cimException = exception;
        }
        catch(const Exception& exception)
        {
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        response->cimException = cimException;

        //_enqueueResponse(request, response.release());
        return response.release();
    }
    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            "No provider found");

        //_enqueueResponse(request, response);
        return response;
    }

    CIMResponseMessage* response = req->buildResponse();
    response->cimException = PEGASUS_CIM_EXCEPTION(
        CIM_ERR_NOT_SUPPORTED,
        "This msg should never happen.");
    PEG_METHOD_EXIT();
    return response;
} // do_di_()




//---------------------------------------------------------------------
Message* CIMDirectAccessRep::do_mi_(CIMRequestMessage* request)
{

    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_mi_()");
    CIMModifyInstanceRequestMessage *req =
        (CIMModifyInstanceRequestMessage*)request;

    // ATTN: Who makes sure the instance name and the instance match?
    // ATTN: KS May 28. Change following to reflect new instancelookup
    // get the class name
    CIMName className = req->modifiedInstance.getClassName();

    CIMException checkClassException;

    opreqdispatch_->_checkExistenceOfClass(
        req->nameSpace,
        className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::do_mi_() - "
            "CIM exist exception has occurred.  Namespace: $0  "
            "Class Name: $1",
            req->nameSpace.getString(),
            className.getString()));

        CIMResponseMessage* response = req->buildResponse();
        response->cimException = checkClassException;

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    ProviderInfo providerInfo = opreqdispatch_->_lookupNewInstanceProvider(
            req->nameSpace,
            className );

    if (providerInfo.hasProvider)
    {
        CIMModifyInstanceRequestMessage* requestCopy =    // fix?
            new CIMModifyInstanceRequestMessage(*req);
        removePropagatedAndOriginAttributes(requestCopy->modifiedInstance);
        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
            if (! isCMPIInterface(requestCopy))
            {
                cout<<"Not a CMPI Method"<<endl;
                throw CIMException(
                    CIM_ERR_DACIM_NOT_SUPPORTED,
                    "DIRECTACCESS NOT SUPPORTED");
            }

        }

        CIMModifyInstanceRequestMessage* requestCallbackCopy =
            new CIMModifyInstanceRequestMessage(*requestCopy);

        PEG_METHOD_EXIT();
        return forwardRequestToProvider_(
            providerInfo.className,
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy);
    }

    else if (reposi_->isDefaultInstanceProvider())
    {
        // translate and forward request to repository
        CIMException cimException;

        AutoPtr<CIMModifyInstanceResponseMessage> response(
            dynamic_cast<CIMModifyInstanceResponseMessage*>(
            req->buildResponse()));

        try
        {
            removePropagatedAndOriginAttributes(req->modifiedInstance);

            StatProviderTimeMeasurement providerTime(response.get());

            reposi_->modifyInstance(
                req->nameSpace,
                req->modifiedInstance,
                req->includeQualifiers, req->propertyList,
                ((ContentLanguageListContainer)req->operationContext.get(
                ContentLanguageListContainer::NAME)).getLanguages());

            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "CIMDirectAccessRep::do_mi_() - "
                "Namespace: $0  Instance name: $1",
                req->nameSpace.getString(),
                req->modifiedInstance.getClassName().getString()));
        }
        catch(const CIMException& exception)
        {
            cimException = exception;
        }
        catch(const Exception& exception)
        {
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        response->cimException = cimException;
        //_enqueueResponse(request, response.release());
        return response.release();
    }

    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            "No provider found");

        //_enqueueResponse(request, response);
        return response;
    }

    CIMResponseMessage* response = req->buildResponse();
    response->cimException = PEGASUS_CIM_EXCEPTION(
        CIM_ERR_NOT_SUPPORTED,
        "this msg should never happen");
    PEG_METHOD_EXIT();
    return response;

} // do_mi_()





                  /*---------------------------------------
                   *                                       *
                   *                                       *
                   *       association & reference         *
                   *                                       *
                   *                                       *
                   *---------------------------------------*/



//---------------------------------------------------------------------------
Message* CIMDirectAccessRep::do_ea_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleAssociatorsRequest");
    CIMAssociatorsRequestMessage *req = (CIMAssociatorsRequestMessage*)request;
    if (!opreqdispatch_ -> _enableAssociationTraversal)
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            "Associators");

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    // Validate role parameter syntax
    if ((req->role != String::EMPTY) && (!CIMName::legal(req->role)))
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = CIMException(
            CIM_ERR_INVALID_PARAMETER,
            req->role);

        PEG_METHOD_EXIT();
        return response;
    }

    // Validate resultRole parameter syntax
    if ((req->resultRole != String::EMPTY) &&
        (!CIMName::legal(req->resultRole)))
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = CIMException(
            CIM_ERR_INVALID_PARAMETER,
            req->role);

        PEG_METHOD_EXIT();
        return response;
    }

    CIMException checkClassException;
    opreqdispatch_->_checkExistenceOfClass(
        req->nameSpace,
        req->objectName.getClassName(),
        checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER,
                req->objectName.toString());
        }

        CIMResponseMessage* response = req->buildResponse();
        response->cimException = checkClassException;

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    PEG_LOGGER_TRACE((
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "CIMDirectAccessRep::handleAssociators - "
        "Namespace: $0  Class name: $1",
        req->nameSpace.getString(),
        req->objectName.toString()));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (req->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "Associators executing Class request");
        */
        AutoPtr<CIMAssociatorsResponseMessage> response(
            dynamic_cast<CIMAssociatorsResponseMessage*>(
            req->buildResponse()));

        try
        {
            //StatProviderTimeMeasurement providerTime(response.get());

            response->cimObjects = reposi_->associators(
                req->nameSpace,
                req->objectName,
                req->assocClass,
                req->resultClass,
                req->role,
                req->resultRole,
                req->includeQualifiers,
                req->includeClassOrigin,
                req->propertyList);
        }
        catch(const CIMException& exception)
        {
            response->cimException = exception;
        }
        catch(const Exception& exception)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                exception.getMessage());
        }
        catch(...)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                String::EMPTY);
        }

        //_enqueueResponse(request, response.release());
        return response.release();
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfos;
        Uint32 providerCount;
        try
        {
            providerInfos = opreqdispatch_->_lookupAllAssociationProviders(
                req->nameSpace,
                req->objectName,
                req->assocClass,
                String::EMPTY,
                providerCount);
        }
        catch(const CIMException& cimException)
        {
            CIMResponseMessage* response = req->buildResponse();
            response->cimException = cimException;
            //_enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return response;
        }

        /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount);
        */
        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if ((providerCount == 0) && !reposi_->isDefaultInstanceProvider())
        {
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for " + req->className.getString());

            CIMResponseMessage* response = req->buildResponse();
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                String::EMPTY);

            //_enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return response;
        }

        //
        // Get the instances from the repository, as necessary
        //

        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        AutoPtr<CIMAssociatorsResponseMessage> response;

        if (reposi_->isDefaultInstanceProvider())
        {
            response.reset(dynamic_cast<CIMAssociatorsResponseMessage*>(
                req->buildResponse()));

            try
            {
                //StatProviderTimeMeasurement providerTime(response.get());

                response->cimObjects = reposi_->associators(
                    req->nameSpace,
                    req->objectName,
                    req->assocClass,
                    req->resultClass,
                    req->role,
                    req->resultRole,
                    req->includeQualifiers,
                    req->includeClassOrigin,
                    req->propertyList);
            }
            catch(const CIMException& exception)
            {
                response->cimException = exception;
            }
            catch(const Exception& exception)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch(...)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    String::EMPTY);
            }

            /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                "Associators repository access: class = %s, count = %u.",
                    (const char*)req->objectName.toString().getCString(),
                    response->cimObjects.size());
            */
        }

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
             //_enqueueResponse(request, response.release());
             PEG_METHOD_EXIT();
             return response.release();
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate* poA = new OperationAggregate(
            new CIMAssociatorsRequestMessage(*req),
            req->getType(),
            req->messageId,
            req->queueIds.top(),
            req->objectName.getClassName(),
            req->nameSpace);

        //poA->_aggregationSN = cimOperationAggregationSN++;

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerCount+1);
            // send the repository's results
        }
        else
        {
            poA->setTotalIssued(providerCount);
        }
        for (Uint32 i = 0; i < providerInfos.size(); i++)
        {
            if (providerInfos[i].hasProvider)
            {
                CIMAssociatorsRequestMessage* requestCopy =
                    new CIMAssociatorsRequestMessage(*req);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->assocClass = providerInfos[i].className;

                if (providerInfos[i].providerIdContainer.get() != 0)
                {
                    requestCopy->operationContext.insert(
                        *(providerInfos[i].providerIdContainer.get()));
                }
                if (! isCMPIInterface(requestCopy))
                {
                    cout<<"Not a CMPI Method"<<endl;
                    throw CIMException(
                        CIM_ERR_DACIM_NOT_SUPPORTED,
                        "DIRECTACCESS NOT SUPPORTED");
                }

                PEG_TRACE_STRING(
                    TRC_DISPATCHER,
                    Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfos[i].className.getString());
                CIMAssociatorsResponseMessage* aggResp =
                    dynamic_cast<CIMAssociatorsResponseMessage*>(
                    forwardRequestForAggregation_(
                    providerInfos[i].serviceName,
                    providerInfos[i].controlProviderName,
                    requestCopy, poA));
                if (aggResp)
                {
                    if (aggResp->cimException.getCode() != CIM_ERR_SUCCESS)
                    {
                        response->cimException = aggResp->cimException;
                        return response.release();
                    }

                    for (
                        Uint32 index = 0;
                        index < aggResp->cimObjects.size();
                        index++)
                    {
                        CIMNamespaceName n1;
                        CIMObjectPath& objPath1 = const_cast<CIMObjectPath&>(
                            aggResp->cimObjects[index].getPath());
                        objPath1.setHost(String::EMPTY);
                        objPath1.setNameSpace(n1);
                    }
                }
                response->cimObjects.appendArray((aggResp->cimObjects));
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
        return response.release();
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return NULL;               //fix
} // do_ea_()




//---------------------------------------------------------------------
Message *CIMDirectAccessRep::do_ean_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleAssociatorNamesRequest");
    CIMAssociatorNamesRequestMessage *req =
        (CIMAssociatorNamesRequestMessage*)request;

    if (!opreqdispatch_ -> _enableAssociationTraversal)
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            "AssociatorNames");

        PEG_METHOD_EXIT();
        return response;
    }

    // Validate role parameter syntax
    if ((req->role != String::EMPTY) && (!CIMName::legal(req->role)))
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = CIMException(
            CIM_ERR_INVALID_PARAMETER,
            req->role);

        PEG_METHOD_EXIT();
        return response;
    }

    // Validate resultRole parameter syntax
    if ((req->resultRole != String::EMPTY) &&
        (!CIMName::legal(req->resultRole)))
    {
        CIMResponseMessage* response = req->buildResponse();

        response->cimException = CIMException(
            CIM_ERR_INVALID_PARAMETER,
            req->resultRole);

        PEG_METHOD_EXIT();
        return response;
    }

    CIMException checkClassException;
    opreqdispatch_->_checkExistenceOfClass(
        req->nameSpace,
        req->objectName.getClassName(),
        checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = CIMException(
                CIM_ERR_INVALID_PARAMETER,
                req->objectName.toString());
        }

        CIMResponseMessage* response = req->buildResponse();
        response->cimException = checkClassException;

        PEG_METHOD_EXIT();
        return response;
    }

    PEG_LOGGER_TRACE((
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "CIMDirectAccessRep::handleAssociatorNames - "
        "Namespace: $0  Class name: $1",
        req->nameSpace.getString(),
        req->objectName.toString()));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (req->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "AssociatorNames executing Class request");
        */
        AutoPtr<CIMAssociatorNamesResponseMessage> response(
            dynamic_cast<CIMAssociatorNamesResponseMessage*>(
            req->buildResponse()));

        try
        {
            //StatProviderTimeMeasurement providerTime(response.get());

            response->objectNames = reposi_->associatorNames(
                req->nameSpace,
                req->objectName,
                req->assocClass,
                req->resultClass,
                req->role,
                req->resultRole);
        }
        catch(const CIMException& exception)
        {
            response->cimException = exception;
        }
        catch(const Exception& exception)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                exception.getMessage());
        }
        catch(...)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                String::EMPTY);
        }

        //_enqueueResponse(request, response.release());
        return response.release();
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfos;
        Uint32 providerCount;
        try
        {
            providerInfos = opreqdispatch_->_lookupAllAssociationProviders(
                req->nameSpace,
                req->objectName,
                req->assocClass,
                String::EMPTY,
                providerCount);
        }
        catch(const CIMException& cimException)
        {
            CIMResponseMessage* response = req->buildResponse();
            response->cimException = cimException;

            //_enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return response;
        }

        /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount);
        */
        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if ((providerCount == 0) && !reposi_->isDefaultInstanceProvider())
        {
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for " + req->className.getString());

            CIMResponseMessage* response = req->buildResponse();
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                String::EMPTY);

            //_enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return response;
        }

        //
        // Get the instances from the repository, as necessary
        //

        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        AutoPtr<CIMAssociatorNamesResponseMessage> response;

        if (reposi_->isDefaultInstanceProvider())
        {
            response.reset(dynamic_cast<CIMAssociatorNamesResponseMessage*>(
                req->buildResponse()));

            try
            {
                //StatProviderTimeMeasurement providerTime(response.get());

                response->objectNames = reposi_->associatorNames(
                    req->nameSpace,
                    req->objectName,
                    req->assocClass,
                    req->resultClass,
                    req->role,
                    req->resultRole);
            }
            catch(const CIMException& exception)
            {
                response->cimException = exception;
            }
            catch(const Exception& exception)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch(...)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    String::EMPTY);
            }

            /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                "AssociatorNames repository access: class = %s, count = %u.",
                    (const char*)req->objectName.toString().getCString(),
                    response->objectNames.size());
            */
        }

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
             //_enqueueResponse(request, response.release());
             PEG_METHOD_EXIT();
             return response.release();
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate* poA = new OperationAggregate(
            new CIMAssociatorNamesRequestMessage(*req),
            req->getType(),
            req->messageId,
            req->queueIds.top(),
            req->objectName.getClassName(),
            req->nameSpace);

         //poA->_aggregationSN = cimOperationAggregationSN++;

         // Include the repository response in the aggregation, if applicable
         if (response.get() != 0)
        {
            poA->setTotalIssued(providerCount+1);
            // send the repository's results
        }
        else
        {
            poA->setTotalIssued(providerCount);
        }


        for (Uint32 i = 0; i < providerInfos.size(); i++)
        {
            if (providerInfos[i].hasProvider)
            {
                CIMAssociatorNamesRequestMessage* requestCopy =
                    new CIMAssociatorNamesRequestMessage(*req);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->assocClass = providerInfos[i].className;

                if (providerInfos[i].providerIdContainer.get() != 0)
                {
                    requestCopy->operationContext.insert(
                        *(providerInfos[i].providerIdContainer.get()));
                }
                if (! isCMPIInterface(requestCopy))
                {
                    cout<<"Not a CMPI Method"<<endl;
                    throw CIMException(
                        CIM_ERR_DACIM_NOT_SUPPORTED,
                        "DIRECTACCESS NOT SUPPORTED");
                }

                PEG_TRACE_STRING(
                    TRC_DISPATCHER,
                    Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfos[i].className.getString());
                CIMAssociatorNamesResponseMessage* aggResp =
                    dynamic_cast<CIMAssociatorNamesResponseMessage*>(
                    forwardRequestForAggregation_(
                    providerInfos[i].serviceName,    // fix
                    providerInfos[i].controlProviderName,
                    requestCopy, poA));
                if (aggResp)
                {
                    if (aggResp->cimException.getCode() != CIM_ERR_SUCCESS)
                    {
                        response->cimException = aggResp->cimException;
                        return response.release();
                    }

                    response->objectNames.appendArray((aggResp->objectNames));
                }
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
        return response.release();
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return NULL;             // fix
} // do_ean_()




//-----------------------------------------------------------------
Message* CIMDirectAccessRep::do_er_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleReferencesRequest");
    CIMReferencesRequestMessage *req = (CIMReferencesRequestMessage*)request;
    if (!opreqdispatch_->_enableAssociationTraversal)
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            "References");

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    // Validate role parameter syntax
    if ((req->role != String::EMPTY) && (!CIMName::legal(req->role)))
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER,
            req->role);

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    CIMException checkClassException;
    opreqdispatch_->_checkExistenceOfClass(
        req->nameSpace,
        req->objectName.getClassName(),
        checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = //PEGASUS_CIM_EXCEPTION(
                CIMException(
                CIM_ERR_INVALID_PARAMETER,
                req->objectName.toString());
        }

        CIMResponseMessage* response = req->buildResponse();
        response->cimException = checkClassException;

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    PEG_LOGGER_TRACE((
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "CIMDirectAccessRep::handleReferences - "
        "Namespace: $0  Class name: $1",
        req->nameSpace.getString(),
        req->objectName.toString()));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (req->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "References executing Class request");
        */
        AutoPtr<CIMReferencesResponseMessage> response(
            dynamic_cast<CIMReferencesResponseMessage*>(
            req->buildResponse()));

        try
        {
            //StatProviderTimeMeasurement providerTime(response.get());

            response->cimObjects = reposi_->references(
                req->nameSpace,
                req->objectName,
                req->resultClass,
                req->role,
                req->includeQualifiers,
                req->includeClassOrigin,
                req->propertyList);
        }
        catch(const CIMException& exception)
        {
            response->cimException = exception;
        }
        catch(const Exception& exception)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                exception.getMessage());
        }
        catch(...)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                String::EMPTY);
        }

        //_enqueueResponse(request, response.release());
        return response.release();
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfos;
        Uint32 providerCount;
        try
        {
            providerInfos = opreqdispatch_->_lookupAllAssociationProviders(
                req->nameSpace,
                req->objectName,
                req->resultClass,
                String::EMPTY,
                providerCount);
        }
        catch(const CIMException& cimException)
        {
            CIMResponseMessage* response = request->buildResponse();
            response->cimException = cimException;

            //_enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return response;
        }

        /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount);
        */
        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if ((providerCount == 0) && !reposi_->isDefaultInstanceProvider())
        {
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for " + req->className.getString());

            CIMResponseMessage* response = req->buildResponse();
            response->cimException =
                  CIMException (CIM_ERR_NOT_SUPPORTED, String::EMPTY);

            //_enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return response;
        }

        //
        // Get the instances from the repository, as necessary
        //

        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        AutoPtr<CIMReferencesResponseMessage> response;

        if (reposi_->isDefaultInstanceProvider())
        {
            response.reset(dynamic_cast<CIMReferencesResponseMessage*>(
                req->buildResponse()));

            try
            {
                //StatProviderTimeMeasurement providerTime(response.get());

                response->cimObjects = reposi_->references(
                    req->nameSpace,
                    req->objectName,
                    req->resultClass,
                    req->role,
                    req->includeQualifiers,
                    req->includeClassOrigin,
                    req->propertyList);
            }
            catch(const CIMException& exception)
            {
                response->cimException = exception;
            }
            catch(const Exception& exception)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch(...)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    String::EMPTY);
            }

            /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                "References repository access: class = %s, count = %u.",
                    (const char*)req->objectName.toString().getCString(),
                    response->cimObjects.size());
            */
        }

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
             //_enqueueResponse(request, response.release());
             PEG_METHOD_EXIT();
             return response.release();
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate* poA = new OperationAggregate(
            new CIMReferencesRequestMessage(*req),
            req->getType(),
            req->messageId,
            req->queueIds.top(),
            req->objectName.getClassName(),
            req->nameSpace);

         //poA->_aggregationSN = cimOperationAggregationSN++;

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerCount+1);
            // send the repository's results
        }
        else
        {
            poA->setTotalIssued(providerCount);
        }


        for (Uint32 i = 0; i < providerInfos.size(); i++)
        {
            if (providerInfos[i].hasProvider)
            {
                CIMReferencesRequestMessage* requestCopy =
                    new CIMReferencesRequestMessage(*req);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->resultClass = providerInfos[i].className;

                if (providerInfos[i].providerIdContainer.get() != 0)
                {
                    requestCopy->operationContext.insert(
                        *(providerInfos[i].providerIdContainer.get()));
                }
                if (! isCMPIInterface(requestCopy))
                {
                    cout<<"Not a CMPI Method"<<endl;
                    throw CIMException(
                        CIM_ERR_DACIM_NOT_SUPPORTED,
                        "DIRECTACCESS NOT SUPPORTED");
                }

                PEG_TRACE_STRING(
                    TRC_DISPATCHER,
                    Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfos[i].className.getString());

                CIMReferencesResponseMessage*  aggResp =
                    dynamic_cast<CIMReferencesResponseMessage*>(
                    forwardRequestForAggregation_(
                    providerInfos[i].serviceName,     // fix
                    providerInfos[i].controlProviderName,
                    requestCopy,
                    poA));
                if (aggResp)
                {
                    if (aggResp->cimException.getCode() != CIM_ERR_SUCCESS)
                    {
                        response->cimException = aggResp->cimException;
                        return response.release();
                    }

                    response->cimObjects.appendArray((aggResp->cimObjects));
                    if (aggResp->cimObjects.size())
                    {
                        for (
                            Uint32 index = 0;
                            index < aggResp->cimObjects.size();
                            index++)
                        {
                            CIMNamespaceName n1;
                            CIMObjectPath& objPath1 = const_cast<CIMObjectPath&>
                                (aggResp->cimObjects[index].getPath());
                            objPath1.setHost(String::EMPTY);
                            objPath1.setNameSpace(n1);
                        }
                    }
                }
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
        return response.release();
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return NULL;
} // do_er_()




//--------------------------------------------------------------------
Message* CIMDirectAccessRep::do_ern_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleReferenceNamesRequest");
    CIMReferenceNamesRequestMessage* req =
        (CIMReferenceNamesRequestMessage*)request;
    if (!opreqdispatch_->_enableAssociationTraversal)
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            "ReferenceNames");

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    // Validate role parameter syntax
    if ((req->role != String::EMPTY) && (! CIMName::legal(req->role)))
    {
        CIMResponseMessage* response = req->buildResponse();
        response->cimException =
          CIMException(CIM_ERR_INVALID_PARAMETER, req->role);
        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    CIMException checkClassException;
    opreqdispatch_->_checkExistenceOfClass(
        req->nameSpace,
        req->objectName.getClassName(),
        checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = CIMException(
                CIM_ERR_INVALID_PARAMETER,
                req->objectName.toString());
        }

        CIMResponseMessage* response = req->buildResponse();
        response->cimException = checkClassException;

        //_enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return response;
    }

    PEG_LOGGER_TRACE((
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "CIMDirectAccessRep::handleReferenceNames - "
        "Namespace: $0  Class name: $1",
        req->nameSpace.getString(),
        req->objectName.toString()));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (req->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "ReferenceNames executing Class request");
        */
        AutoPtr<CIMReferenceNamesResponseMessage> response(
            dynamic_cast<CIMReferenceNamesResponseMessage*>(
            req->buildResponse()));

        try
        {
            //StatProviderTimeMeasurement providerTime(response.get());

            response->objectNames = reposi_->referenceNames(
                req->nameSpace,
                req->objectName,
                req->resultClass,
                req->role);
        }
        catch(const CIMException& exception)
        {
            response->cimException = exception;
        }
        catch(const Exception& exception)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                exception.getMessage());
        }
        catch(...)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                String::EMPTY);
        }

        //_enqueueResponse(request, response.release());
        return response.release();
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfos;
        Uint32 providerCount;
        try
        {
            providerInfos = opreqdispatch_->_lookupAllAssociationProviders(
                req->nameSpace,
                req->objectName,
                req->resultClass,
                String::EMPTY,
                providerCount);
        }
        catch(const CIMException& cimException)
        {
            CIMResponseMessage* response = req->buildResponse();
            response->cimException = cimException;

            //_enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return response;
        }

        /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount);
        */
        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if ((providerCount == 0) && !reposi_->isDefaultInstanceProvider())
        {
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for " + req->className.getString());

            CIMResponseMessage* response = req->buildResponse();
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                String::EMPTY);

            //_enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return response;
        }

        //
        // Get the instances from the repository, as necessary
        //

        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        AutoPtr<CIMReferenceNamesResponseMessage> response;

        if (reposi_->isDefaultInstanceProvider())
        {
            response.reset(dynamic_cast<CIMReferenceNamesResponseMessage*>(
                req->buildResponse()));

            try
            {
                //StatProviderTimeMeasurement providerTime(response.get());

                response->objectNames = reposi_->referenceNames(
                    req->nameSpace,
                    req->objectName,
                    req->resultClass,
                    req->role);
            }
            catch(const CIMException& exception)
            {
                response->cimException = exception;
            }
            catch(const Exception& exception)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch(...)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    String::EMPTY);
            }

            /* Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                "ReferenceNames repository access: class = %s, count = %u.",
                    (const char*)req->objectName.toString().getCString(),
                    response->objectNames.size());
            */
        }

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
            //_enqueueResponse(request, response.release());
            PEG_METHOD_EXIT();
            return response.release();
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate* poA = new OperationAggregate(
            new CIMReferenceNamesRequestMessage(*req),
            req->getType(),
            req->messageId,
            req->queueIds.top(),
            req->objectName.getClassName(),
            req->nameSpace);

        //poA->_aggregationSN = cimOperationAggregationSN++;

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerCount+1);
        }
        else
        {
            poA->setTotalIssued(providerCount);
        }


        for (Uint32 i = 0; i < providerInfos.size(); i++)
        {
            if (providerInfos[i].hasProvider)
            {
                CIMReferenceNamesRequestMessage* requestCopy =
                    new CIMReferenceNamesRequestMessage(*req);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->resultClass = providerInfos[i].className;

                if (providerInfos[i].providerIdContainer.get() != 0)
                {
                    requestCopy->operationContext.insert(
                        *(providerInfos[i].providerIdContainer.get()));
                }
                if (! isCMPIInterface(requestCopy))
                {
                    cout<<"Not a CMPI Method"<<endl;
                    throw CIMException(
                        CIM_ERR_DACIM_NOT_SUPPORTED,
                        "DIRECTACCESS NOT SUPPORTED");
                }

                PEG_TRACE_STRING(
                    TRC_DISPATCHER,
                    Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfos[i].className.getString());

                CIMReferenceNamesResponseMessage* aggResp =
                    dynamic_cast<CIMReferenceNamesResponseMessage*>(
                    forwardRequestForAggregation_(
                    providerInfos[i].serviceName,    // fix
                    providerInfos[i].controlProviderName,
                    requestCopy,
                    poA));
                if (aggResp)
                {
                    if (aggResp->cimException.getCode() != CIM_ERR_SUCCESS)
                    {
                        response->cimException = aggResp->cimException;
                        return response.release();
                    }

                    response->objectNames.appendArray((aggResp->objectNames));
                }


                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    return response.release();
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return NULL;
} // do_ern_()

                  /*---------------------------
                   *                           *
                   *                           *
                   *        qualifier          *
                   *                           *
                   *                           *
                   *---------------------------*/

//-----------------------------------------------------------------
Message* CIMDirectAccessRep::do_gq_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_gq_()");
    CIMGetQualifierRequestMessage *req =
        (CIMGetQualifierRequestMessage*) request;
    AutoPtr<CIMGetQualifierResponseMessage> response(
        dynamic_cast<CIMGetQualifierResponseMessage*>(
        req->buildResponse()));

    try
    {
        //StatProviderTimeMeasurement providerTime(response.get());
        response->cimQualifierDecl = reposi_->getQualifier(
            req->nameSpace,
            req->qualifierName);
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::do_gq_() - Namespace: $0 Qualifier Name: $1",
            req->nameSpace.getString(),
            req->qualifierName.getString()));
    }
    catch(const CIMException& exception)
    {
        response->cimException = exception;
    }
    catch(const Exception& exception)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            exception.getMessage());
    }
    catch(...)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            String::EMPTY);
    }

    //_enqueueResponse(request, response.release());
    PEG_METHOD_EXIT();
    return response.release();
} // do_gq_()



#if 0
//----------------------------------------------------------------
Message* CIMDirectAccessRep::do_sq_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleSetQualifierRequest");
    CIMSetQualifierRequestMessage *req =
        (CIMSetQualifierRequestMessage*) request;
    AutoPtr<CIMSetQualifierResponseMessage> response(
        dynamic_cast<CIMSetQualifierResponseMessage*>(
        req->buildResponse()));

    try
    {
        //StatProviderTimeMeasurement providerTime(response.get());

        reposi_->setQualifier(
            req->nameSpace,
            req->qualifierDeclaration,
            ((ContentLanguageListContainer)req->operationContext.get(
            ContentLanguageListContainer::NAME)).getLanguages());

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::handleSetQualifierRequest - "
            "Namespace: $0  Qualifier Name: $1",
        req->nameSpace.getString(),
        req->qualifierDeclaration.getName().getString()));
    }
    catch(const CIMException& exception)
    {
        response->cimException = exception;
    }
    catch(const Exception& exception)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            exception.getMessage());
    }
    catch(...)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            String::EMPTY);
    }

    //_enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
    return response.release();
} // do_sq_()
#endif

#if 0
//-------------------------------------------------------------------
Message* CIMDirectAccessRep::do_dq_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleDeleteQualifierRequest");
    CIMDeleteQualifierRequestMessage* req =
        (CIMDeleteQualifierRequestMessage*)request;
    AutoPtr<CIMDeleteQualifierResponseMessage> response(
        dynamic_cast<CIMDeleteQualifierResponseMessage*>(
        req->buildResponse()));

    try
    {
        //StatProviderTimeMeasurement providerTime(response.get());

        reposi_->deleteQualifier(req->nameSpace, req->qualifierName);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::handleDeleteQualifierRequest - "
            "Namespace: $0  Qualifier Name: $1",
            req->nameSpace.getString(),
            req->qualifierName.getString()));
    }
    catch(const CIMException& exception)
    {
        response->cimException = exception;
    }
    catch(const Exception& exception)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            exception.getMessage());
    }
    catch(...)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            String::EMPTY);
    }

    //_enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
    return response.release();
} // do_dq_()
#endif



//--------------------------------------------------------------------
Message* CIMDirectAccessRep::do_eq_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleEnumerateQualifiersRequest");
    CIMEnumerateQualifiersRequestMessage* req =
        (CIMEnumerateQualifiersRequestMessage*)request;
    AutoPtr<CIMEnumerateQualifiersResponseMessage> response(
        dynamic_cast<CIMEnumerateQualifiersResponseMessage*>(
        req->buildResponse()));

    try
    {
        //StatProviderTimeMeasurement providerTime(response.get());

        response->qualifierDeclarations =
            reposi_->enumerateQualifiers(req->nameSpace);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::handleEnumerateQualifiersRequest - "
            "Namespace: $0",
            req->nameSpace.getString()));
    }
    catch(const CIMException& exception)
    {
        response->cimException = exception;
    }
    catch(const Exception& exception)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            exception.getMessage());
    }
    catch(...)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            String::EMPTY);
    }

    //_enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
    return response.release();
} // do_eq_()

          /*---------------------------*
           *                           *
           *                           *
           *       property            *
           *                           *
           *                           *
           *---------------------------*/


//----------------------------------------------------------------
Message* CIMDirectAccessRep::do_gp_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleGetPropertyRequest");
    CIMGetPropertyRequestMessage *req = (CIMGetPropertyRequestMessage*)request;
    CIMName className = req->instanceName.getClassName();

    // check the class name for an "external provider"
    // Assumption here is that there are no "internal" property requests.
    // teATTN: KS 20030402 - This needs cleanup along with the setproperty.

    ProviderInfo providerInfo = opreqdispatch_->_lookupInstanceProvider(
        req->nameSpace,
        className);

    if (providerInfo.hasProvider)
    {
        CIMGetPropertyRequestMessage* requestCopy =
            new CIMGetPropertyRequestMessage(*req);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
            if (! isCMPIInterface(requestCopy))
            {
                cout<<"Not a CMPI Method"<<endl;
                throw CIMException(
                    CIM_ERR_DACIM_NOT_SUPPORTED,
                    "DIRECTACCESS NOT SUPPORTED");
            }

        }

        CIMGetPropertyRequestMessage* requestCallbackCopy =
            new CIMGetPropertyRequestMessage(*requestCopy);

        CIMGetPropertyResponseMessage* resp =
            dynamic_cast<CIMGetPropertyResponseMessage*>(
            forwardRequestToProvider_(
            providerInfo.className,
            PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy));
        CIMInstance Inst;
        resp->value.set(resp->value.toString());

        PEG_METHOD_EXIT();
        return resp;

    }
    else if (reposi_->isDefaultInstanceProvider())
    {
        AutoPtr<CIMGetPropertyResponseMessage> response(
            dynamic_cast<CIMGetPropertyResponseMessage*>(req->buildResponse()));

        try
        {
            //StatProviderTimeMeasurement providerTime(response.get());

            response->value = reposi_->getProperty(
                req->nameSpace,
                req->instanceName,
                req->propertyName);
        }
        catch(const CIMException& exception)
        {
            response->cimException = exception;
        }
        catch(const Exception& exception)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
              CIM_ERR_FAILED,
              exception.getMessage());
        }
        catch(...)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
              CIM_ERR_FAILED,
              String::EMPTY);
        }

        //_enqueueResponse(request, response.release());
        return response.release();
    }
    // (else) // No provider is registered and the repository isn't the default
    CIMResponseMessage* response = req->buildResponse();
    response->cimException =
        PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
    //_enqueueResponse(request, response);
    return response;
} // do_gp_()


//--------------------------------------------------------------
Message* CIMDirectAccessRep::do_sp_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleSetPropertyRequest");
    CIMSetPropertyRequestMessage* req = (CIMSetPropertyRequestMessage*)request;
    {
        CIMException cimException;
        try
        {
            opreqdispatch_->_fixSetPropertyValueType(req);
        }
        catch(CIMException& exception)
        {
            cimException = exception;
        }
        catch(const Exception& exception)
        {
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        if (cimException.getCode() != CIM_ERR_SUCCESS)
        {
            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "CIMDirectAccessRep::handleSetPropertyRequest - "
                "CIM exception has occurred."));

            CIMResponseMessage* response = req->buildResponse();
            response->cimException = cimException;

            //_enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return response;
        }
    }

    CIMName className = req->instanceName.getClassName();

    // check the class name for an "external provider"
    ProviderInfo providerInfo = opreqdispatch_->_lookupInstanceProvider(
        req->nameSpace,
        className);

    if (providerInfo.hasProvider)
    {
        CIMSetPropertyRequestMessage* requestCopy =
            new CIMSetPropertyRequestMessage(*req);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
            if (! isCMPIInterface(requestCopy))
            {
                cout<<"Not a CMPI Method"<<endl;
                throw CIMException(
                    CIM_ERR_DACIM_NOT_SUPPORTED,
                    "DIRECTACCESS NOT SUPPORTED");
            }
        }

        CIMSetPropertyRequestMessage* requestCallbackCopy =
            new CIMSetPropertyRequestMessage(*requestCopy);

        return forwardRequestToProvider_(   //Service(
            providerInfo.className,
            PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
    }
    else if (reposi_->isDefaultInstanceProvider())
    {
        AutoPtr<CIMSetPropertyResponseMessage> response(
            dynamic_cast<CIMSetPropertyResponseMessage*>(
            request->buildResponse()));

        try
        {
            //StatProviderTimeMeasurement providerTime(response.get());

            reposi_->setProperty(
                req->nameSpace,
                req->instanceName,
                req->propertyName,
                req->newValue,
                ((ContentLanguageListContainer)req->operationContext.get(
                ContentLanguageListContainer::NAME)).getLanguages());

            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "CIMDirectAccessRep::handleSetPropertyRequest - "
                "Namespace: $0  Instance Name: $1  Property Name: $2  New "
                "Value: $3",
                req->nameSpace.getString(),
                req->instanceName.getClassName().getString(),
                req->propertyName.getString(),
                req->newValue.toString()));
        }
        catch(const CIMException& exception)
        {
            response->cimException = exception;
        }
        catch(const Exception& exception)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                exception.getMessage());
        }
        catch(...)
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                String::EMPTY);
        }

        //_enqueueResponse(request, response.release());
        return response.release();
    }

    // (else) // No provider is registered and the repository isn't the default
    CIMResponseMessage* response = req->buildResponse();
    response->cimException =
        PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
    //_enqueueResponse(request, response);
    PEG_METHOD_EXIT();
    return response;
} // do_sp_()


           /*---------------------------
           *                           *
           *                           *
           *        misc op            *
           *                           *
           *                           *
           *---------------------------*/

//--------------------------------------------------------------------
Message* CIMDirectAccessRep::do_invoke_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMDirectAccessRep::do_invoke_()");
    CIMInvokeMethodRequestMessage *req =
        (CIMInvokeMethodRequestMessage*)request;
    {
        CIMException cimException;
        try
        {
            opreqdispatch_->_fixInvokeMethodParameterTypes(req);
        }
        catch(CIMException& exception)
        {
            cimException = exception;
        }
        catch(const Exception& exception)
        {
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        if (cimException.getCode() != CIM_ERR_SUCCESS)
        {
            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "CIMDirectAccessRep::do_invoke_() - "
                "CIM exception has occurred."));

            CIMResponseMessage* response = req->buildResponse();
            response->cimException = cimException;

            //_enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return response;
        }
    }

    CIMName className = req->instanceName.getClassName();

    CIMException checkClassException;
    opreqdispatch_->_checkExistenceOfClass(
        req->nameSpace,
        className,
        checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        // map CIM_ERR_INVALID_CLASS to CIM_ERR_NOT_FOUND
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_FOUND,
                className.getString());
        }

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMDirectAccessRep::do_invoke_() - "
            "CIM exist exception has occurred.  Namespace: $0  "
            "Class Name: $1",
            req->nameSpace.getString(),
            className.getString()));

        CIMResponseMessage* response = req->buildResponse();
        response->cimException = checkClassException;

        //_enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return response;
    }

    String serviceName;
    String controlProviderName;

    // Check for class provided by an internal provider
    if (opreqdispatch_->_lookupInternalProvider(
        req->nameSpace,
        className,
        serviceName,
        controlProviderName))
    {
        CIMInvokeMethodRequestMessage* requestCopy =
            new CIMInvokeMethodRequestMessage(*req);

        CIMInvokeMethodRequestMessage* requestCallbackCopy =
            new CIMInvokeMethodRequestMessage(*requestCopy);  // fix
        odiniter_((ProviderManagerService*) NULL);
        /*OperationAggregate* poA = new OperationAggregate(
            new CIMInvokeMethodRequestMessage(*requestCopy),
            requestCopy->getType(),
            requestCopy->messageId,
            requestCopy->queueIds.top(),
            className,
            requestCopy->nameSpace);
        return forwardRequestForAggregation_(
            className.getString(),
            serviceName,
            requestCopy,poA);//,
            //requestCallbackCopy);
        */
        odiniter_((ProviderManagerService*) NULL);
        ProviderInfo providerInfo(className);
        providerInfo.serviceName = serviceName;
        providerInfo.controlProviderName = controlProviderName;
        /*requestCopy->operationContext.insert(
              *(providerInfo.providerIdContainer.get()));
        */
        return forwardRequestToProvider_(
            className,
            serviceName,
            controlProviderName,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        //return;
    }

    // check the class name for an "external provider"
    ProviderIdContainer* providerIdContainer = NULL;

    String providerName = opreqdispatch_->_lookupMethodProvider(
        req->nameSpace,
        className,
        req->methodName,
        &providerIdContainer);

    if (providerName.size() != 0)
    {
        CIMInvokeMethodRequestMessage* requestCopy =
            new CIMInvokeMethodRequestMessage(*req);

        if (providerIdContainer != NULL)
        {
            requestCopy->operationContext.insert(*providerIdContainer);
            if (! isCMPIInterface(requestCopy))
            {
                cout<<"Not a CMPI Method"<<endl;
                throw CIMException(
                    CIM_ERR_DACIM_NOT_SUPPORTED,
                    "DIRECTACCESS NOT SUPPORTED");
            }

            delete providerIdContainer;
            providerIdContainer = NULL;
        }

        CIMInvokeMethodRequestMessage* requestCallbackCopy =
            new CIMInvokeMethodRequestMessage(*requestCopy);

        CIMInvokeMethodResponseMessage* resp =
            dynamic_cast<CIMInvokeMethodResponseMessage*>(
            forwardRequestToProvider_(
            className,
            PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP,
            String::EMPTY,
            requestCopy,
            requestCallbackCopy));

        switch (resp->retValue.getType())
        {
            case CIMTYPE_OBJECT:
                {
                    CIMObjectPath obj;
                    CIMObject cObj;
                    resp->retValue.get(cObj);

                    obj.set(cObj.getClassName().getString());
                    cObj.setPath(obj);
                    resp->retValue.set(cObj);
                }
                break;
            case CIMTYPE_INSTANCE:
                {
                    CIMName className;
                    CIMInstance outputInstance;//(className);

                    resp->retValue.get(outputInstance);
                    outputInstance.setPath(
                        outputInstance.getClassName().getString());
                    resp->retValue.set(outputInstance);

                }
                //MSA break;
            default:
                break;
        }
        PEG_METHOD_EXIT();
        return resp;
    }

    CIMResponseMessage* response = req->buildResponse();
    response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_METHOD_NOT_AVAILABLE,
            req->methodName.getString());

    //_enqueueResponse(request, response);

    PEG_METHOD_EXIT();
    return response;
} // do_invoke_()




//-----------------------------------------------------------------
Message* CIMDirectAccessRep::do_query_(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleExecQueryRequest");
    CIMExecQueryRequestMessage* req = (CIMExecQueryRequestMessage*)request;
    AutoPtr<CIMExecQueryResponseMessage> response(
        dynamic_cast<CIMExecQueryResponseMessage*>(
        req->buildResponse()));

    Boolean exception = false;

#ifdef PEGASUS_DISABLE_EXECQUERY
    response->cimException = PEGASUS_CIM_EXCEPTION(
        CIM_ERR_NOT_SUPPORTED,
        String::EMPTY);
    exception = true;
#else
    /*
    if (QuerySupportRouter::routeHandleExecQueryRequest(
        this->opreqdispatch_,req)==false)  // fix
    {
        if (req->operationContext.contains(
                SubscriptionFilterConditionContainer::NAME))
        {
            SubscriptionFilterConditionContainer sub_cntr =
                req->operationContext.get(
                    SubscriptionFilterConditionContainer::NAME);
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED,
                sub_cntr.getQueryLanguage());
        }
        else
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED, req->queryLanguage);
        }

        exception = true;
    }
    */
    CIMException cimException;
    CIMExecQueryRequestMessage* queryRequest =
        dynamic_cast<CIMExecQueryRequestMessage*>(request);
        CIMName className;

    AutoPtr<WQLQueryExpressionRep> qx;
    CIMExecQueryResponseMessage* aggResp = NULL;
    if (queryRequest->queryLanguage == "WQL")
    {
        Boolean exception = false;
        AutoPtr<WQLSelectStatement> selectStatement(new WQLSelectStatement());
        //AutoPtr<WQLQueryExpressionRep> qx;
        CIMName className;
        //CIMExecQueryResponseMessage * aggResp = NULL;
        try
        {
            WQLParser::parse(queryRequest->query, *selectStatement.get());
            className = selectStatement->getClassName();
            qx.reset(new WQLQueryExpressionRep("WQL", selectStatement.get()));
            selectStatement.release();
        }
        catch(ParseError&)
        {
            cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_QUERY,
                queryRequest->query);
            exception = true;
        }
        catch(MissingNullTerminator&)
        {
            cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_QUERY,
                queryRequest->query);
            exception = true;
        }

        if (exception == false)
        {
            opreqdispatch_->_checkExistenceOfClass(
                queryRequest->nameSpace,
                className,
                cimException);
            if (cimException.getCode() != CIM_ERR_SUCCESS)
            {
                exception = true;
            }
        }

    }
    else
    {
        cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED,
            queryRequest->queryLanguage);
        exception = true;
    }
    if (exception)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = cimException;

        //_enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return response;
    }
    //
    // Get names of descendent classes:
    //
    Array<ProviderInfo> providerInfos;

    // This gets set by _lookupAllInstanceProviders()
    Uint32 providerCount;

    try
    {
        providerInfos = opreqdispatch_->_lookupAllInstanceProviders(
            queryRequest->nameSpace,
            className,
            providerCount);
    }
    catch(CIMException& exception)
    {
        // Return exception response if exception from getSubClasses
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = exception;

        //_enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return response;
    }
    if (providerCount > opreqdispatch_->_maximumEnumerateBreadth)
    {
        Logger::put(
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "Request-too-broad exception.  Namespace: $0  "
            "Class Name: $1 Limit: $2  ProviderCount: $3",
            queryRequest->nameSpace.getString(),
            queryRequest->className.getString(),
            opreqdispatch_->_maximumEnumerateBreadth,
            providerCount);

        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL2,
            "ERROR: Enumerate operation too broad for class %s.  "
            "Limit = %u, providerCount = %u",
            (const char*)queryRequest->className.getString().getCString(),
            opreqdispatch_->_maximumEnumerateBreadth,
            providerCount));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_NOT_SUPPORTED,
            MessageLoaderParms(
            "Server.CIMOperationRequestDispatcher.QUERY_REQ_TOO_BROAD",
            "Query request too Broad"));

        if (aggResp)
        {
            aggResp->cimObjects.appendArray(
                dynamic_cast<CIMExecQueryResponseMessage *>(
                response)->cimObjects);
        }
        //_enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return aggResp;
    }
    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED
    if ((providerCount == 0) && !(reposi_->isDefaultInstanceProvider()))
    {
        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            "CIM_ERROR_NOT_SUPPORTED for " +
            queryRequest->className.getString());

        CIMResponseMessage* response = queryRequest->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            String::EMPTY);
        if (aggResp)
        {
            aggResp->cimObjects.appendArray(
                dynamic_cast<CIMExecQueryResponseMessage *>(
                response)->cimObjects);
        }
        //_enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return aggResp;
    }

    // We have instances for Providers and possibly repository.
    // Set up an aggregate object and save a copy of the original request.
    CIMEnumerateInstancesResponseMessage* instanceResponseMsg = NULL;

    OperationAggregate* poA = new OperationAggregate(
        new CIMExecQueryRequestMessage(*queryRequest),
        queryRequest->getType(),
        queryRequest->messageId,
        queryRequest->queueIds.top(),
        className, CIMNamespaceName(),
        qx.release(),
        "WQL");

    // Set the number of expected responses in the OperationAggregate
    Uint32 numClasses = providerInfos.size();
    poA->_aggregationSN = opreqdispatch_->cimOperationAggregationSN++;
    poA->_nameSpace = queryRequest->nameSpace;

    if (reposi_->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++)
        {
            ProviderInfo& providerInfo = providerInfos[i];

            // this class is registered to a provider - skip
            if (providerInfo.hasProvider)
            {
                continue;
            }
            // If this class does not have a provider

            PEG_TRACE((
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "Routing ExecQuery request for class %s to the "
                "repository.  Class # %u of %u, aggregation SN %u.",
                (const char*)providerInfo.className.getString().getCString(),
                (unsigned int)(i + 1),
                (unsigned int)(numClasses),
                (unsigned int)(poA->_aggregationSN)));

            AutoPtr<CIMEnumerateInstancesResponseMessage> response(
                new CIMEnumerateInstancesResponseMessage(
                request->messageId,
                CIMException(),
                request->queueIds.copyAndPop(),
                Array<CIMInstance>()));
            response->syncAttributes(request);

            try
            {
                // Enumerate instances only for this class
                response->cimNamedInstances =
                    reposi_->enumerateInstancesForClass(
                    queryRequest->nameSpace,
                    providerInfo.className);
                instanceResponseMsg = response.get();
            }
            catch(CIMException& exception)
            {
                response->cimException = exception;
            }
            catch(Exception& exception)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch(...)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    String::EMPTY);
            }

            poA->appendResponse(response.release());
        } // for all classes and derived classes
        Uint32 numberResponses = poA->numberResponses();
        Uint32 totalIssued = providerCount + (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            //handleEnumerateInstancesResponseAggregation(poA);
            CIMResponseMessage* response = poA->removeResponse(0);
            CIMEnumerateInstancesResponseMessage* providerResponse =
                dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
                forwardRequestForAggregation_(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMExecQueryRequestMessage(*queryRequest),
                poA,
                response));
            if (providerResponse)
            {
                instanceResponseMsg->cimNamedInstances.appendArray(
                    providerResponse->cimNamedInstances);
            }
        }
    } // if isDefaultInstanceProvider
    else
    {
        // Set the number of expected responses in the OperationAggregate
        poA->setTotalIssued(providerCount);
    }
    //CIMEnumerateInstancesResponseMessage* instanceResponseMsg = NULL;
    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)
    {
        // If this class has a provider
        ProviderInfo& providerInfo = providerInfos[i];

        // this class is NOT registered to a provider - skip
        if (!providerInfo.hasProvider)
        {
            continue;
        }

        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            "Routing ExecQuery request for class %s to "
            "service \"%s\" for control provider \"%s\".  "
            "Class # %u of %u, aggregation SN %u.",
            (const char*)providerInfo.className.getString().getCString(),
            (const char*)providerInfo.serviceName.getCString(),
            (const char*)providerInfo.controlProviderName.getCString(),
            (unsigned int)(i + 1),
            (unsigned int)numClasses,
            (unsigned int)(poA->_aggregationSN)));

        ProviderIdContainer* providerIdContainer =
            providerInfo.providerIdContainer.get();

        if (providerInfo.hasNoQuery)
        {
            OperationContext* context = &request->operationContext;
            const OperationContext::Container* container = 0;
            container = &context->get(IdentityContainer::NAME);

            const IdentityContainer& identityContainer =
                dynamic_cast<const IdentityContainer&>(*container);

            AutoPtr<CIMEnumerateInstancesRequestMessage> enumReq(
                new CIMEnumerateInstancesRequestMessage(
                    queryRequest->messageId,
                    queryRequest->nameSpace,
                    providerInfo.className,
                    false,false,false,false,
                    CIMPropertyList(),
                    queryRequest->queueIds,
                    queryRequest->authType,
                    identityContainer.getUserName()));

            context = &enumReq->operationContext;
            if (providerIdContainer)
            {
                context->insert(*providerIdContainer);
            }

            context->insert(identityContainer);
            CIMEnumerateInstancesResponseMessage* providerResponse =
                dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
                forwardRequestForAggregation_(
                providerInfo.serviceName,
                providerInfo.controlProviderName,
                enumReq.release(), poA));
            if (providerResponse)
            {
                instanceResponseMsg->cimNamedInstances.appendArray(
                    providerResponse->cimNamedInstances);
            }

        }
        else
        {
            AutoPtr<CIMExecQueryRequestMessage> requestCopy(
                new CIMExecQueryRequestMessage(*queryRequest));

            OperationContext* context = &request->operationContext;
            if (providerIdContainer &&
                !context->contains(
                providerInfo.providerIdContainer.get()->getName()))
            {
                context->insert(*(providerInfo.providerIdContainer.get()));
            }
            requestCopy->operationContext = *context;
            requestCopy->className = providerInfo.className;
            CIMEnumerateInstancesResponseMessage* providerResponse =
                dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
                forwardRequestForAggregation_(
                providerInfo.serviceName,
                providerInfo.controlProviderName,
                requestCopy.release(),
                poA));
            if (providerResponse)
            {
               instanceResponseMsg->cimNamedInstances.appendArray(
                   providerResponse->cimNamedInstances);
            }

        }
    } // for all classes and derived classes


#endif

    if (exception)
    {
        //_enqueueResponse(request, response.release());
        PEG_METHOD_EXIT();
        return response.release();
    }

    PEG_METHOD_EXIT();
    return response.release();
} // do_query_()





//--------------------------------------------------------------------
Message* CIMDirectAccessRep::forwardRequestForAggregation_(
    const String& serviceName,
    const String& controlProviderName,
    CIMRequestMessage  *request,      // why not a CIMOperationRequestMessage?
    OperationAggregate *poA,
    CIMResponseMessage *response)
{

    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::forwardRequestForAggregation_");

    //Array<Uint32> serviceIds;
    //find_services(serviceName, 0, 0, &serviceIds);
    //find_services(serviceName, 0, 0, &serviceIds);
    //PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode* op = //this->get_op();
        MessageQueueService::get_op();
    // if a response is given, this means the caller wants to run only the
    // callback asynchronously
    if (response)
    {
        AsyncLegacyOperationResult* asyncResponse =
            new AsyncLegacyOperationResult(op, response);

        // By setting this to complete, this allows ONLY the callback to run
        // without going through the typical async request apparatus
        op->complete();
    }


    // If ControlProviderName empty, forward to service.
    if (controlProviderName == String::EMPTY)
    {

        //AsyncLegacyOperationStart* asyncRequest =
        //    new AsyncLegacyOperationStart(
        //        op,
        //        serviceIds[0],
        //        request,
        //        this->getQueueId());


        if (serviceName == "Server::ProviderManagerService")
        {
            Message* m = odiniter_(pvdrmgrsvc_)->processMessage(request);
            response = dynamic_cast<CIMResponseMessage*>(m);
            if (! response)
            {
                CIMResponseMessage* r = request->buildResponse();
                r->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_NOT_SUPPORTED,
                    "dyn cast failed");
                throw r->cimException;
            }
        }
        else
        {
            MessageQueue* q = MessageQueue::lookup(request->queueIds.top());
        }

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL3,
            "Forwarding " + String(MessageTypeToString(request->getType())) +
            " to service " + serviceName +
            ". Response should go to queue " +
            ((MessageQueue::lookup(request->queueIds.top())) ?
            String(((MessageQueue::lookup(
            request->queueIds.top()))->getQueueName())) :
            String("BAD queue name")));


        //SendAsync(
        //    op,
        //    serviceIds[0],
        //    CIMDirectAccessRep::_forwardForAggregationCallback,
        //    this,
        //    poA);
    }
    else
    {

        //AsyncModuleOperationStart* moduleControllerRequest =
        //    new AsyncModuleOperationStart(
        //        op,
        //        serviceIds[0],
        //        this->getQueueId(),
        //        true,
        //        controlProviderName,
        //        request);

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL3,
            "Forwarding " + String(MessageTypeToString(request->getType())) +
            " to service " + serviceName + ", control provider " +
            controlProviderName + ". Response should go to queue " +
            ((MessageQueue::lookup(request->queueIds.top())) ?
            String(((MessageQueue::lookup(
            request->queueIds.top()))->getQueueName())) :
            String("BAD queue name")));


        if (controlProviderName == "ControlService::NamespaceProvider")
        {
//verify(__FILE__,__LINE__);
            response =
                odiniter_((NamespaceProvider*)NULL)->processMessage(request);
        }
        else if (controlProviderName == "ControlService::InteropProvider")
        {
//verify(__FILE__,__LINE__);
            response =
                odiniter_((InteropProvider*)NULL)->processMessage(request);
        }
        else if (controlProviderName ==
            "ControlService::ProviderRegistrationProvider")
        {
//verify(__FILE__,__LINE__);
            response =
                odiniter_((
                ProviderRegistrationProvider*)NULL)->processMessage(request);
        }
        else if (controlProviderName ==
            "ControlService::CIMOMStatDataProvider")
        {
//verify(__FILE__,__LINE__);
            response =
                odiniter_((CIMOMStatDataProvider*)NULL)->
                processMessage(request);
        }
        else if (controlProviderName == "ControlService::ConfigProvider")
        {
            response =
                odiniter_((
                ConfigSettingProvider*)NULL)->processMessage(request);
        }
//#if 0
        else if (controlProviderName ==
            "ControlService::CIMQueryCapabilitiesProvider")
        {
            odiniter_((CIMQueryCapabilitiesProvider*)NULL);
            response =
                odiniter_((
                CIMQueryCapabilitiesProvider*)NULL)->processMessage(request);
        }
//#endif
        else if (controlProviderName == "ControlService::ShutdownProvider")
        {
            throw CIMException(CIM_ERR_DACIM_REDIRECT,"Redirect to server");

            //return pvdrmgrsvc_->_processMessage(request);
        }
        //MSA -- do we nee this empty else part
        else
        {
        }


        //SendAsync(
        //    op,
        //    serviceIds[0],
        //    CIMDirectAccessRep::_forwardForAggregationCallback,
        //    this,
        //    poA);
    }

    bool aggdone = poA->appendResponse(response);

    PEG_METHOD_EXIT();
    return response;       // fix.  probably not needed; resp. is in poA.
} // forwardRequestForAggregation()




//-------------------------------------------------------------------
Message* CIMDirectAccessRep::forwardRequestToProvider_(
    const CIMName& className,
    const String& serviceName,
    const String& controlProviderName,
    CIMRequestMessage* request,
    CIMRequestMessage* requestCopy)
{

    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::forwardRequestToProvider_()");

    //Array<Uint32> serviceIds;
    //opreqdispatch_->find_services(serviceName, 0, 0, &serviceIds);
    //PEGASUS_ASSERT(serviceIds.size() != 0);

    //AsyncOpNode* op = this->get_op();

    //AutoPtr<CIMResponseMessage> response(
    //      dynamic_cast<CIMResponseMessage*>(request->buildResponse()));
    CIMResponseMessage* response = NULL;

    if (controlProviderName == String::EMPTY)
    {
        if (serviceName == "Server::ProviderManagerService")
        {
            return odiniter_(pvdrmgrsvc_)->processMessage(request);
        }
        else if (serviceName == "Server::IndicationService")
        {
#if PEGASUS_DIRECTACCESS_BUILDTYPE == dacimINTEGRATED
            // dacim should never see it.  but just in case.
            CIMException cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                "Server::IndicationService");
            response = request->buildResponse();
            response->cimException = cimException;
#elif PEGASUS_DIRECTACCESS_BUILDTYPE >= dacimSEPREPOSI
            CIMException cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED, "Server::IndicationService is TBD");
                response = request->buildResponse();
                response->cimException = cimException;
#endif
        }
        else if (serviceName == "IndicationHandlerService")
        {
#if PEGASUS_DIRECTACCESS_BUILDTYPE == dacimINTEGRATED
            // dacim should never see it.  but just in case.
            CIMException cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                "Server::IndicationHandlerService");
            response = request->buildResponse();
            response->cimException = cimException;
#elif PEGASUS_DIRECTACCESS_BUILDTYPE >= dacimSEPREPOSI
            CIMException cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                "Server::IndicationHandlerService is TBD");
            response = request->buildResponse();
            response->cimException = cimException;
#endif
        }
        else
        {
            CIMException cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                ">> unk serviceName");  // fix
            response = request->buildResponse();
            response->cimException = cimException;
        }
    }
    else
    {
        if (controlProviderName == "ControlService::NamespaceProvider")
        {
            response =
                odiniter_((NamespaceProvider*)NULL)->processMessage(request);
        }
        else if (controlProviderName == "ControlService::InteropProvider")
        {
            response =
                odiniter_((InteropProvider*)NULL)->processMessage(request);
        }
        else if (controlProviderName ==
            "ControlService::ProviderRegistrationProvider")
        {
            response = odiniter_((
                ProviderRegistrationProvider*)NULL)->processMessage(request);
        }
        else if (controlProviderName == "ControlService::ConfigProvider")
        {
            response = odiniter_((
                ConfigSettingProvider*)NULL)->processMessage(request);
        }
        else if (controlProviderName ==
            "ControlService::CIMQueryCapabilitiesProvider")
        {
            response = odiniter_((
                CIMQueryCapabilitiesProvider*)NULL)->processMessage(request);
        }
        else if (controlProviderName ==
            "ControlService::CIMOMStatDataProvider")
        {
            response = odiniter_((
                CIMOMStatDataProvider*)NULL)->processMessage(request);
        }
        else if (controlProviderName ==
            "ControlService::ShutdownProvider")
        {
            throw CIMException(CIM_ERR_DACIM_REDIRECT,"Redirect to server");
        }
        else
        {
            CIMException cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                ">> unk serviceName");  // fix
                response = request->buildResponse();
                response->cimException = cimException;
        }

    }

    PEG_METHOD_EXIT();
    return response;  //.release();
} // forwardRequestToProvider_()



#if 0 // needed?
//----------------------------------------------------------------
Message* CIMDirectAccessRep::forwardRequestToService_(
    const String& serviceName,
    CIMRequestMessage* request,
    CIMRequestMessage* requestCopy)
{

    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::forwardRequestToService_()");

    //Array<Uint32> serviceIds;
    //find_services(serviceName, 0, 0, &serviceIds);
    //PEGASUS_ASSERT(serviceIds.size() != 0);

    //AsyncOpNode* op = this->get_op();

    //AsyncLegacyOperationStart* asyncRequest =
    //    new AsyncLegacyOperationStart(
    //        op,
    //        serviceIds[0],
    //        request,
    //        this->getQueueId());

    //asyncRequest->dest = serviceIds[0];



    PEG_TRACE_STRING(
        TRC_DISPATCHER,
        Tracer::LEVEL3,
        "Forwarding " + String(MessageTypeToString(request->getType())) +
        " to service " + serviceName + ". Response should go to queue " +
        ((MessageQueue::lookup(request->queueIds.top())) ?
        String(((MessageQueue::lookup(
        request->queueIds.top()))->getQueueName())) :
        String("BAD queue name")));

    //SendAsync(
    //    op,
    //    serviceIds[0],
    //    CIMDirectAccessRep::_forwardRequestCallback,
    //    this,
    //    requestCopy);


    if (serviceName == "ControlService")
    {
        response = nspvdr_->processMessage(request);
    }
    else if (serviceName == "IndicationService")
    {
        response = interoppvdr_->processMessage(request);
    }
    else if (serviceName == "IndicationHandlerServicer")
    {
        response = pvdrregipvdr_->processMessage(request);
    }
    else if (serviceName == "ProviderManagerService")
    {
        CIMException cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            ">> qry capab not ready yet");  // fix
        response = request->buildResponse();
        response->cimException = cimException;
    }
    else
    {
        CIMException cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            ">> unk serviceName");  // fix
        response = request->buildResponse();
        response->cimException = cimException;
    }

    PEG_METHOD_EXIT();
    return response;
} // forwardRequestToService()
#endif



#if 0
//-----------------------------------------------------------------------------
void CIMDirectAccessRep::handleEnumerateInstancesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::handleEnumerateInstancesResponse");

    CIMEnumerateInstancesResponseMessage* toResponse =
        (CIMEnumerateInstancesResponseMessage*)poA->getResponse(0);

    PEG_LOGGER_TRACE((
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "CIMDirectAccessRep::EnumerateInstancesResponseAggregation"
        "- Namespace: $0 Class name: $1 Response Count: $2",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses()));


    CIMEnumerateInstancesRequestMessage* request =
        (CIMEnumerateInstancesRequestMessage*)poA->getRequest();

    // Work backward and delete each response off the end of the array
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMEnumerateInstancesResponseMessage* fromResponse =
            (CIMEnumerateInstancesResponseMessage*)poA->getResponse(i);

        for (Uint32 j = 0; j < fromResponse->cimNamedInstances.size(); j++)
        {
            toResponse->cimNamedInstances.append(
                fromResponse->cimNamedInstances[j]);
        }

        poA->deleteResponse(i);
    }

    PEG_LOGGER_TRACE((
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "CIMDirectAccessRep::"
        "EnumerateInstancesResponseAggregation - "
        "Local Only: $0 Include Qualifiers: $1 Include Class Origin: $2",
        (request->localOnly == true ? "true" : "false"),
        (request->includeQualifiers == true ? "true" : "false"),
        (request->includeClassOrigin == true ? "true" : "false")));

    PEG_METHOD_EXIT();
} // ei resp agg


//--------------------------------------------------------------------------
/* aggregate the responses for enumerateinstancenames into a single response
*/
void CIMDirectAccessRep::handleEnumerateInstanceNamesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMDirectAccessRep::HandleEnumerateInstanceNamesResponseAggregation");

    CIMEnumerateInstanceNamesResponseMessage* toResponse =
        (CIMEnumerateInstanceNamesResponseMessage*) poA->getResponse(0);

    PEG_LOGGER_TRACE((
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "CIMDirectAccessRep::EnumerateInstanceNames Response - "
        "Namespace: $0  Class name: $1 Response Count: $2",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses()));

    // Work backward and delete each response off the end of the array
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMEnumerateInstanceNamesResponseMessage* fromResponse =
            (CIMEnumerateInstanceNamesResponseMessage*)poA->getResponse(i);

        for (Uint32 j = 0; j < fromResponse->instanceNames.size(); j++)
        {
            // Duplicate test goes here if we decide to eliminate dups in
            // the future.
            toResponse->instanceNames.append(fromResponse->instanceNames[j]);
        }
        poA->deleteResponse(i);
    }
    PEG_METHOD_EXIT();
} // ein resp agg
#endif




#if 0
//------------------------------------------------------
class subscri_item
{
    subscri_item(const cimSubscription&);
    ~subscri_item();
    const cimSubscription& sub_;
    subscri_item* nxt_;
    Thread* th_;
    void* (*thfun_)(void*);
    friend class CIMDirectAccessRep;
};
#endif



//------------------------------------------------------------
CIMDirectAccessRep::CIMDirectAccessRep() :
    //MessageQueue("CIMDirectAccessRepQ"),
    pvdrmgrsvc_(NULL), pvdrregimgr_(NULL), reposi_(NULL),
    opreqdispatch_(NULL), nspvdr_(NULL), interoppvdr_(NULL),
    pvdrregipvdr_(NULL), statdatapvdr_(NULL), cfgpvdr_(NULL),
    queryPvdr_(NULL), controlsvc_(NULL), numsubscri_(0), responsemsg_(NULL)
{
    // fix; make as much of the following on-demand rather than creating them
    // all here in ctor.

    runtime_context_is_directaccess_cim = true;

#if PEGASUS_DIRECTACCESS_BUILDTYPE == dacimINTEGRATED
    char * pegHome = new char[100];
    char * envHome = getenv("PEGASUS_HOME");
    strcpy(pegHome,envHome);

    if (pegHome != NULL)
    {
        ConfigManager::setPegasusHome(String(envHome));
        ConfigManager::getInstance()->useConfigFiles = true;
        ConfigManager::getInstance()->loadConfigFiles();
        delete [] pegHome;
    }

    String reposirootpath = ConfigManager::getHomedPath(
        ConfigManager::getInstance()->
        getCurrentValue("repositoryDir"));
#else
    String reposirootpath = "in-memory repository";
#endif
    reposi_ = new CIMRepository(reposirootpath);
    pvdrregimgr_ = new ProviderRegistrationManager(reposi_);


#if 0
    pvdrmgrsvc_ = new ProviderManagerService(
        pvdrregimgr_,  reposi_,
        DefaultProviderManager::createDefaultProviderManagerCallback); //>>>fix
    indihandlersvc_ = new IndicationHandlerService(reposi_);  // fix;
                                                    //crt on 1st subscri
#endif

    controlsvc_ = new ModuleController(PEGASUS_QUEUENAME_CONTROLSERVICE);
    controlpvdr_.reserveCapacity(5);   // needed??
#if 0
    //1
    nspvdr_ = new ProviderMessageHandler(
        "NamespaceProvider",
        new NamespaceProvider(reposi_),
        &indicationCallback,
        &chunkCallback,
        false);
    controlpvdr_.append(nspvdr_);
    ModuleController::register_module(
        PEGASUS_QUEUENAME_CONTROLSERVICE,
        PEGASUS_MODULENAME_NAMESPACEPROVIDER,
        nspvdr_,
        controlProviderReceiveMessageCallback,
        0);
    //2
    interoppvdr_ = new ProviderMessageHandler(
        "InteropProvider",
        new InteropProvider(reposi_),
        &indicationCallback,
        &chunkCallback,
        false);

    controlpvdr_.append(interoppvdr_);
    ModuleController::register_module(
        PEGASUS_QUEUENAME_CONTROLSERVICE,
        PEGASUS_MODULENAME_INTEROPPROVIDER,
        interoppvdr_,
        controlProviderReceiveMessageCallback,
        0);
    //3
    pvdrregipvdr_ = new ProviderMessageHandler(
        "ProviderRegistrationProvider",
        new ProviderRegistrationProvider(pvdrregimgr_),
        &indicationCallback,
        &chunkCallback,
        false);
    controlpvdr_.append(pvdrregipvdr_);
    ModuleController::register_module(
        PEGASUS_QUEUENAME_CONTROLSERVICE,
        PEGASUS_MODULENAME_PROVREGPROVIDER,
        pvdrregipvdr_,
        controlProviderReceiveMessageCallback,
        0);
    //4
    statdatapvdr_ = new ProviderMessageHandler(
        "CIMOMStatDataProvider",
        new CIMOMStatDataProvider(),
        &indicationCallback,
        &chunkCallback,
        false);
    controlpvdr_.append(statdatapvdr_);
    ModuleController::register_module(
        PEGASUS_QUEUENAME_CONTROLSERVICE,
        PEGASUS_MODULENAME_CIMOMSTATDATAPROVIDER,
        statdatapvdr_,
        controlProviderReceiveMessageCallback,
        0);
    //5
    cfgpvdr_ = new ProviderMessageHandler(
        "ConfigSettingProvider",
        new ConfigSettingProvider(),
        &indicationCallback,
        &chunkCallback,
        false);
    controlpvdr_.append(cfgpvdr_);
    ModuleController::register_module(
        PEGASUS_QUEUENAME_CONTROLSERVICE,
        PEGASUS_MODULENAME_CONFIGPROVIDER,
        cfgpvdr_,
        controlProviderReceiveMessageCallback,
        0);
#endif
    //MSA --- Check the assignemnt it is duplicated
    opreqdispatch__ =
        opreqdispatch_ = new CIMOperationRequestDispatcher(
        reposi_,
        pvdrregimgr_);

    PEGASUS_ASSERT(opreqdispatch_ == opreqdispatch__);
    pvdrregimgr_->initializeProviders();  // fix; need this?
    runtime_context_is_directaccess_cim = false; // set again in do_request()
                                                 // needed??  fix

    this__ = this;
    PEGASUS_ASSERT(this == this__);


    //verify(__FILE__,__LINE__);

} // ctor



//---------------------------------------------------------------------------
CMPIProviderManager* CIMDirectAccessRep::odiniter_(ProviderManagerService*)
{
    if (!pvdrmgrsvc_)
    {
        // a service (not a ctl pvdr)
        pvdrmgrsvc_ = new ProviderManagerService(
            pvdrregimgr_,
            reposi_,
            /*DefaultProviderManager::
            createDefaultProviderManagerCallback
            */
            0);
        cmpiProviderManager_ = new CMPIProviderManager();
    }
    return cmpiProviderManager_;
}
#if PEGASUS_DIRECTACCESS_BUILDTYPE >= dacimSEPREPOSI
//---------------------------------------------------------------------------
IndicationHandlerService* CIMDirectAccessRep::odiniter_(
    IndicationHandlerService*)
{
    if (!indihandlersvc_)
    {
        // a service (not a ctl pvdr)
        indihandlersvc_ = new IndicationHandlerService(reposi_);  // fix;
    }
    return indihandlersvc_;
}
#endif
//---------------------------------------------------------------------------
ProviderMessageHandler* CIMDirectAccessRep::odiniter_(NamespaceProvider*)
{
    if (!nspvdr_)
    {
        //1
        nspvdr_ = new ProviderMessageHandler(
            "NamespaceProvider",
            new NamespaceProvider(reposi_),
            &indicationCallback,
            &chunkCallback,
            false);
        controlpvdr_.append(nspvdr_);
        ModuleController::register_module(
            PEGASUS_QUEUENAME_CONTROLSERVICE,
            PEGASUS_MODULENAME_NAMESPACEPROVIDER,
            nspvdr_,
            controlProviderReceiveMessageCallback,
            0);
    }
    return nspvdr_;
}
//---------------------------------------------------------------------------
ProviderMessageHandler* CIMDirectAccessRep::odiniter_(InteropProvider*)
{
    if (!interoppvdr_)
    {
        //2
        interoppvdr_ = new ProviderMessageHandler(
            "InteropProvider",
            new InteropProvider(reposi_),
            &indicationCallback,
            &chunkCallback,
            false);
        controlpvdr_.append(interoppvdr_);
        ModuleController::register_module(
            PEGASUS_QUEUENAME_CONTROLSERVICE,
            PEGASUS_MODULENAME_INTEROPPROVIDER,
            interoppvdr_,
            controlProviderReceiveMessageCallback,
            0);
    }
    return interoppvdr_;
}
//---------------------------------------------------------------------------
ProviderMessageHandler* CIMDirectAccessRep::odiniter_(
    ProviderRegistrationProvider*)
{
    if (!pvdrregipvdr_)
    {
        //3
        pvdrregipvdr_ = new ProviderMessageHandler(
            "ProviderRegistrationProvider",
            new ProviderRegistrationProvider( pvdrregimgr_ ),
            &indicationCallback,
            &chunkCallback,
            false);
        controlpvdr_.append(pvdrregipvdr_);
        ModuleController::register_module(
            PEGASUS_QUEUENAME_CONTROLSERVICE,
            PEGASUS_MODULENAME_PROVREGPROVIDER,
            pvdrregipvdr_,
            controlProviderReceiveMessageCallback,
            0);
    }
    return pvdrregipvdr_;
}
//---------------------------------------------------------------------------
ProviderMessageHandler* CIMDirectAccessRep::odiniter_(CIMOMStatDataProvider*)
{
    if (!statdatapvdr_)
    {
        //4
        statdatapvdr_ = new ProviderMessageHandler(
            "CIMOMStatDataProvider",
            new CIMOMStatDataProvider(),
            &indicationCallback,
            &chunkCallback,
            false);
        controlpvdr_.append(statdatapvdr_);
        ModuleController::register_module(
            PEGASUS_QUEUENAME_CONTROLSERVICE,
            PEGASUS_MODULENAME_CIMOMSTATDATAPROVIDER,
            statdatapvdr_,
            controlProviderReceiveMessageCallback,
            0);
    }
    return statdatapvdr_;
}
//---------------------------------------------------------------------------
ProviderMessageHandler* CIMDirectAccessRep::odiniter_(ConfigSettingProvider*)
{
    if (!cfgpvdr_)
    {
        //5
        cfgpvdr_ = new ProviderMessageHandler(
            "ConfigSettingProvider",
            new ConfigSettingProvider(),
            &indicationCallback,
            &chunkCallback,
            false);
        controlpvdr_.append(cfgpvdr_);
        ModuleController::register_module(
            PEGASUS_QUEUENAME_CONTROLSERVICE,
            PEGASUS_MODULENAME_CONFIGPROVIDER,
            cfgpvdr_,
            controlProviderReceiveMessageCallback,
            0);
    }
    return cfgpvdr_;
}
//#if 0
ProviderMessageHandler* CIMDirectAccessRep::odiniter_(
    CIMQueryCapabilitiesProvider*)
{

    if (!queryPvdr_)
    {
        //cout<<"In Init of QueryCap1"<<endl;
        queryPvdr_ = new ProviderMessageHandler(
            "CIMQueryCapabilitiesProvider",
            new CIMQueryCapabilitiesProvider(),
            0,
            0,
            false);
        controlpvdr_.append(queryPvdr_);
        ModuleController::register_module(
            PEGASUS_QUEUENAME_CONTROLSERVICE,
            PEGASUS_MODULENAME_CIMQUERYCAPPROVIDER,
            queryPvdr_,
            controlProviderReceiveMessageCallback,
            0);
    }
    return queryPvdr_;
}
//#endif


#if PEGASUS_DIRECTACCESS_BUILDTYPE >= dacimSEPREPOSI
//---------------------------------------------------------------------------
IndicationService *CIMDirectAccessRep::odiniter_(IndicationService*)
{
    if (!indisvc_)
    {
        // a service (not a ctl pvdr)
        // For this dacim buildtype, follow to cim server build parms
        // when appropriate. By default, this svc is enabled.
        ConfigManager *cm = ConfigManager::getInstance();   // save this?
        if (ConfigManager::parseBooleanValue(
            cm->getCurrentValue("enableIndicationService")))
        {
            indisvc_ = new IndicationService(reposi_, pvdrregimgr_);
        }
        else
        {
            CIMException cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                "Not configured for Indication Service");
            throw cimException;  // maybe a more graceful way is better?
        }
    }
    return indisvc_;
}
#endif



#if 0
static subscri_item *subscri_1 = NULL;
static int numsubscri_item = 0;
#endif


//-----------------------------------------------
CIMDirectAccessRep::~CIMDirectAccessRep()
{
    //{
    //    AutoMutex a(arequestlock_);
    //    if (!_dacim_) return;
    //    _dacim_ = NULL;
    //}
    if (opreqdispatch_)
    {
        delete opreqdispatch_;
        opreqdispatch_ = NULL;
        for (int i=0, n=controlpvdr_.size(); i<n; ++i) 
        {
            // these are deleted here; cfgpvdr_, statdatapvdr_,
            // pvdrregipvdr_, interoppvdr_, nspvdr_;
            ProviderMessageHandler *p = controlpvdr_[i];

            delete p->getProvider();
            delete p;
        }
    
        delete controlsvc_;
#if PEGASUS_DIRECTACCESS_BUILDTYPE >= dacimSEPREPOSI
        delete indisvc_;
        delete indihandlersvc_;
#endif
        delete pvdrmgrsvc_;
        delete pvdrregimgr_;
        delete reposi_;
    }   
#if 0
    while (subscri_1)
    {
        delete subscri_1;
    }
#endif
}



#if 0
//--------------------------------------------------------------
void CIMDirectAccessRep::addSubscription(cimSubscription& s)
{
    if (s.subscriptionName == "*")
    {
        return;
    }
    if (!isvalidsubscription_(s))
    {
        String msg = "Invalid data in cimSubscription. subscriptionName = ";
        msg.append(s.subscriptionName);
        throw Exception(msg);
    }
    if (isduplicatesubscription_(s))
    {
        String msg = "Duplicate cimSubscription. subscriptionName = ";
        msg.append(s.subscriptionName);
        throw Exception(msg);
    }
    //CIMRequestMessage *indimsg = (CIMRequestMessage*)mkMessage_( s );
    //Message *respmsg = odiniter_(pvdrmgrsvc_)->_processMessage( indimsg );
    // >>>what err ckg needed here?
    //
    // IndicationService::_handleCreateInstanceRequest() throws this if
    // no eligable pvdrs are found.
    //     throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_NOT_SUPPORTED,
    //              MessageLoaderParms(_MSG_NO_PROVIDERS_KEY,
    //              _MSG_NO_PROVIDERS));
    //
    new subscri_item(s);
}




//-------------------------------------------------------------------
void CIMDirectAccessRep::removeSubscription(const cimSubscription& s)
{
    if (s.subscriptionName == "*")
    {
        while (subscri_1)
        {
            delete subscri_1;
        }
    }
    else
    {
        delete findsubscription_(s);
    }
}


//--------------------------------------------------------
bool CIMDirectAccessRep::isvalidsubscription_(const cimSubscription& s)
{
    return s.isvalid();
}
//-----------------------------------------------------------
bool CIMDirectAccessRep::isduplicatesubscription_(const cimSubscription& s)
{
    return findsubscription_(s) != NULL;
}
//-----------------------------------------------------------
subscri_item *CIMDirectAccessRep::findsubscription_(const cimSubscription& s)
{
    subscri_item *si = subscri_1;
    while (si)
    {
        if (si->sub_ == s)
        {
            return si;
        }
        si = si->nxt_;
    }
    return NULL;
}

#endif

#ifdef DACIM_DEBUG
//------------------------------------------------------
void CIMDirectAccessRep::verify(const char *file, int line)
{
#if 0
    printf("CIMDirectAccessRep::verify() calledfrom %s, %d:"
        " this(%p), reposi_(%p),\n    interoppvdr_(%p),interoppvdr__(%p),"
        "nspvdr_(%p),nspvdr__(%p)\n",
        file, line,this,reposi_,interoppvdr_,interoppvdr__,nspvdr_,nspvdr__);
#endif

    if ((void*)this != this__)
    {
    }
    if ((void*)reposi_ != reposi__)
    {
    }
    if ((void*)interoppvdr_ != interoppvdr__)
    {
        interoppvdr_ = (ProviderMessageHandler*)interoppvdr__;
    }
    if ((void*)nspvdr_ != nspvdr__)
    {
        nspvdr_ = (ProviderMessageHandler*)nspvdr__;
    }
    if ((void*)statdatapvdr_ != statdatapvdr__)
    {
        statdatapvdr_ = (ProviderMessageHandler*)statdatapvdr__;
    }
}
#endif

#if 0
//---------------------------------------------------------
subscri_item::subscri_item(const cimSubscription& s):
    sub_(s),
    nxt_(NULL),
    th_(NULL),
    thfun_(NULL)
{
    if (subscri_1 == NULL)
    {
        subscri_1 = this;
    }
    else
    {
        subscri_item *si = subscri_1;
        while (si->nxt_)
        {
            si=si->nxt_;
        }
        si->nxt_ = this;
    }
    ++numsubscri_item;
    //thfun_ = &s.indicationCallback;    //>>>tbd
    Thread th_(thfun_, NULL, false);
}


//---------------------------------------
subscri_item::~subscri_item()
{
    // need to delete th_?                      //>>tbd
    if (subscri_1 == this)
    {
        subscri_1 = nxt_;
    }
    else
    {
        subscri_item *s1 = subscri_1,
            *s2 = subscri_1->nxt_;
        while(s1 && s2)
        {
            if (s2 == this)
            {
                s1->nxt_ = s2->nxt_;
                break;
            }
            s1 = s2;
            s2 = s2->nxt_;
            if (s2 == NULL)
            {
                break;
            }
        }
    }
    --numsubscri_item;
}

#endif



#ifdef DACIM_DEBUG
//------------------------------------------------------------
static String _showPropertyList(const CIMPropertyList& pl)
{
    String returnString;

    if (pl.isNull())
    {
        returnString = "NULL";
    }
    else if (pl.size() == 0)
    {
        returnString = "EMPTY";
    }
    else
    {
        for (Uint32 i = 0; i < pl.size(); i++)
        {
            if (i > 0)
            {
                returnString.append(", ");
            }
            returnString.append(pl[i].getString());
        }
    }

    return returnString;
}
#endif

template<class ObjectClass>
void removePropagatedAndOriginAttributes(ObjectClass& newObject)
{
    Uint32 numProperties = newObject.getPropertyCount();
    for (Uint32 i = 0; i < numProperties; i++)
    {
        CIMProperty currentProperty = newObject.getProperty(i);
        if (currentProperty.getPropagated() == true ||
            currentProperty.getClassOrigin().getString().size() > 0)
        {
            newObject.removeProperty(i);
            currentProperty.setPropagated(false);
            currentProperty.setClassOrigin(CIMName());
            newObject.addProperty(currentProperty);
            --i;
        }
    }
}

PEGASUS_NAMESPACE_END
