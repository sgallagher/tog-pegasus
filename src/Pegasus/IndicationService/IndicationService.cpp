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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company
//                   (carolann_graves@hp.com)
//               Ben Heilbronn, Hewlett-Packard Company (ben_heilbronn@hp.com)
//               Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//               Dave Rosckes (rosckes@us.ibm.com)
//               Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//               Seema Gupta (gseema@in.ibm.com for PEP135)
//               Dan Gorey, IBM (djgorey@us.ibm.com)
//               Amit K Arora, IBM (amita@in.ibm.com) for Bug#1730
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/MessageQueue.h>
#ifdef PEGASUS_INDICATION_PERFINST
#include <Pegasus/Common/Stopwatch.h>
#endif
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/AcceptLanguages.h> // l10n
#include <Pegasus/Common/ContentLanguages.h> // l10n
#include <Pegasus/Common/OperationContextInternal.h>
// l10n
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/WQL/WQLSelectStatement.h>

#include "IndicationConstants.h"
#include "IndicationMessageConstants.h"
#include "SubscriptionRepository.h"
#include "SubscriptionTable.h"
#include "IndicationService.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// ATTN-RK-20020730: Temporary hack to fix Windows build
Boolean ContainsCIMName(const Array<CIMName>& a, const CIMName& x)
{
    Uint32 n = a.size();

    for (Uint32 i = 0; i < n; i++)
    {
        if (a[i].equal(x))
            return true;
    }

    return false;
}

Mutex IndicationService::_mutex;

IndicationService::IndicationService (
    CIMRepository * repository,
    ProviderRegistrationManager * providerRegManager)
    : Base (PEGASUS_QUEUENAME_INDICATIONSERVICE,
            MessageQueue::getNextQueueId ()),
         _providerRegManager (providerRegManager)
{
    _enableSubscriptionsForNonprivilegedUsers = false;

    try
    {
        // Determine the value for the configuration parameter
        // enableSubscriptionsForNonprivilegedUsers
        ConfigManager* configManager = ConfigManager::getInstance();

        if (String::equalNoCase(
            configManager->getCurrentValue("enableAuthentication"), "true"))
        {
            _enableSubscriptionsForNonprivilegedUsers = String::equalNoCase(
                configManager->getCurrentValue(
                "enableSubscriptionsForNonprivilegedUsers"), "true");
        }
        else
        {
            // Authentication needs to be enabled to perform authorization
            // tests.
            _enableSubscriptionsForNonprivilegedUsers = true;
        }
     }
     catch (...)
     {
        // If there is an error reading the configuration file then
        // the value of _enableSubscriptionsForNonprivilegedUsers will
        // default to false (i.e., the more restrictive security
        // setting.
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
           "Failure attempting to read configuration parameters during initialization.");
     }

    Tracer::trace (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
        "Value of _enableSubscriptionsForNonprivilegedUsers is %d",
        _enableSubscriptionsForNonprivilegedUsers);

    try
    {
        //
        //  Create Subscription Repository
        //
        _subscriptionRepository = new SubscriptionRepository (repository);

        //
        //  Create Subscription Table
        //
        _subscriptionTable = new SubscriptionTable (_subscriptionRepository);

        // Initialize the Indication Service
        _initialize ();
    }
    catch (Exception & e)
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
           "Exception caught in attempting to initialize Indication Service: " +
            e.getMessage ());
    }
}

IndicationService::~IndicationService (void)
{
    delete _subscriptionTable;
    delete _subscriptionRepository;
}

void IndicationService::_handle_async_request(AsyncRequest *req)
{
    if ( req->getType() == async_messages::CIMSERVICE_STOP )
    {
        req->op->processing();

        //
        //  Call _terminate
        //
        _terminate ();

        handle_CimServiceStop(static_cast<CimServiceStop *>(req));
    }
    else if (req->getType () == async_messages::CIMSERVICE_START)
    {
        req->op->processing ();

        handle_CimServiceStart (static_cast <CimServiceStart *> (req));
    }
    else if ( req->getType() == async_messages::ASYNC_LEGACY_OP_START )
    {
       try
       {
          req->op->processing();
          Message *legacy = 
              (static_cast<AsyncLegacyOperationStart *>(req)->get_action());
          legacy->put_async(req);
  
          handleEnqueue(legacy);
       }
       catch(Exception & )
       {
           PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL3, 
               "Caught Exception in IndicationService while handling a wrapped legacy  message ");
               _make_response(req, async_results::CIM_NAK );
       }

        return;
    }
    else
        Base::_handle_async_request(req);
}

void IndicationService::handleEnqueue(Message* message)
{
#ifdef PEGASUS_INDICATION_PERFINST
    Stopwatch stopWatch;
#endif

// l10n
    // Set the client's requested language into this service thread.
    // This will allow functions in this service to return messages
    // in the correct language.
    CIMMessage * msg = dynamic_cast<CIMMessage *>(message);
    if (msg != NULL)
    {
        if (msg->thread_changed())
        {
            AcceptLanguages *langs =  new AcceptLanguages
                (((AcceptLanguageListContainer)msg->operationContext.get
                (AcceptLanguageListContainer::NAME)).getLanguages());
            Thread::setLanguages(langs);
        }
    }
    else
    {
        Thread::clearLanguages();
    }          

    switch(message->getType())
    {
        case CIM_GET_INSTANCE_REQUEST_MESSAGE:
            try 
            {
                _handleGetInstanceRequest(message);
            }
            catch( ... ) 
            {
            ;
            }
            break;

        case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
            try 
            {
                _handleEnumerateInstancesRequest(message);
            }
            catch( ... ) 
            {
            ;
            }
            break;

        case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
            try 
            {
                _handleEnumerateInstanceNamesRequest(message);
            }
            catch( ... ) 
            {
            ;
            }
            break;

        case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
            try
            {
                _handleCreateInstanceRequest(message);
            }
            catch( ... )
            {
            ;
            }
            break;

        case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
            try
            {
                _handleModifyInstanceRequest(message);
            }
            catch( ... )
            {
            ;
            }
            break;

        case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
            try
            {
                _handleDeleteInstanceRequest(message);
            }
            catch( ... )
            {
            ;
            }
            break;

        case CIM_PROCESS_INDICATION_REQUEST_MESSAGE:
            try
            {
                _handleProcessIndicationRequest(message);
            }
            catch( ... )
            {
            ;
            }
            break;

        case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
            try
            {
                _handleNotifyProviderRegistrationRequest(message);
            }
            catch( ... )
            {
            ;
            }
            break;

        case CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE:
            try
            {
                _handleNotifyProviderTerminationRequest(message);
            }
            catch( ... )
            {
            ;
            }
            break;

      case CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE:
         try
         {
             _handleNotifyProviderEnableRequest (message);
         }
         catch (...)
         {
             ;
         }
         break;

      default:
        //
        //  A message type not supported by the Indication Service
        //  Should not reach here
        //
        // << Mon Apr 29 16:29:10 2002 mdd >>
        PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL3,
            "IndicationService::handleEnqueue(msg *) rcv'd unsupported msg "
            + String(MessageTypeToString(message->getType())));

        // Note: not setting Content-Language in the response
        CIMRequestMessage* cimRequest =
            dynamic_cast<CIMRequestMessage*>(message);
        CIMResponseMessage* response = cimRequest->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_NOT_SUPPORTED,
            MessageLoaderParms( 
                "IndicationService.IndicationService.UNSUPPORTED_OPERATION",
                "The requested operation is not supported or not recognized "
                "by the indication service.")),

        Base::_enqueueResponse(cimRequest, response);
        break;
    }

#ifdef PEGASUS_INDICATION_PERFINST
    double elapsed = stopWatch.getElapsed ();

    Tracer::trace (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
        "%s: %.3f seconds", MessageTypeToString (message->getType ()), elapsed);
#endif

   delete message;
}

void IndicationService::handleEnqueue(void)
{
    Message * message = dequeue();

    PEGASUS_ASSERT(message != 0);
    handleEnqueue(message);
}

void IndicationService::_initialize (void)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_initialize");

#ifdef PEGASUS_INDICATION_PERFINST
    Stopwatch stopWatch;
#endif

    Array <CIMInstance> activeSubscriptions;
    Array <CIMInstance> noProviderSubscriptions;
    Array <ProviderClassList> enableProviders;
    Boolean invalidInstance = false;

    //
    //  Find required services
    //  NOTE: Indication Service expects to find exactly one Provider Manager
    //  Service and exactly one Handler Manager Service
    //
    Array <Uint32> pmservices;
    find_services (PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, 0, 0, &pmservices);
    pegasus_yield ();
    PEGASUS_ASSERT (pmservices.size () == 1);
    _providerManager = pmservices [0];

    Array <Uint32> hmservices;
    find_services (PEGASUS_QUEUENAME_INDHANDLERMANAGER, 0, 0, &hmservices);
    pegasus_yield ();
    PEGASUS_ASSERT (hmservices.size () == 1);
    _handlerService = hmservices [0];

    //
    //  Set arrays of valid and supported property values
    //
    //  Note: Valid values are defined by the CIM Event Schema MOF
    //  Supported values are a subset of the valid values
    //  Some valid values, as defined in the MOF, are not currently supported
    //  by the Pegasus IndicationService
    //
    _validStates.append (_STATE_UNKNOWN);
    _validStates.append (_STATE_OTHER);
    _validStates.append (_STATE_ENABLED);
    _validStates.append (_STATE_ENABLEDDEGRADED);
    _validStates.append (_STATE_DISABLED);
    _supportedStates.append (_STATE_ENABLED);
    _supportedStates.append (_STATE_DISABLED);
    _validRepeatPolicies.append (_POLICY_UNKNOWN);
    _validRepeatPolicies.append (_POLICY_OTHER);
    _validRepeatPolicies.append (_POLICY_NONE);
    _validRepeatPolicies.append (_POLICY_SUPPRESS);
    _validRepeatPolicies.append (_POLICY_DELAY);
    _supportedRepeatPolicies.append (_POLICY_UNKNOWN);
    _supportedRepeatPolicies.append (_POLICY_OTHER);
    _supportedRepeatPolicies.append (_POLICY_NONE);
    _supportedRepeatPolicies.append (_POLICY_SUPPRESS);
    _supportedRepeatPolicies.append (_POLICY_DELAY);
    _validErrorPolicies.append (_ERRORPOLICY_OTHER);
    _validErrorPolicies.append (_ERRORPOLICY_IGNORE);
    _validErrorPolicies.append (_ERRORPOLICY_DISABLE);
    _validErrorPolicies.append (_ERRORPOLICY_REMOVE);
    _supportedErrorPolicies.append (_ERRORPOLICY_IGNORE);
    _supportedErrorPolicies.append (_ERRORPOLICY_DISABLE);
    _supportedErrorPolicies.append (_ERRORPOLICY_REMOVE);
    _validPersistenceTypes.append (_PERSISTENCE_OTHER);
    _validPersistenceTypes.append (_PERSISTENCE_PERMANENT);
    _validPersistenceTypes.append (_PERSISTENCE_TRANSIENT);
    _supportedPersistenceTypes.append (_PERSISTENCE_PERMANENT);
    _supportedPersistenceTypes.append (_PERSISTENCE_TRANSIENT);

    //
    //  Get existing active subscriptions from each namespace in the repository
    //
    invalidInstance = _subscriptionRepository->getActiveSubscriptions
        (activeSubscriptions);
    noProviderSubscriptions.clear ();

    String condition;
    String query;
    String queryLanguage;
    CIMPropertyList propertyList;
    Array <ProviderClassList> indicationProviders;

    for (Uint32 i = 0; i < activeSubscriptions.size (); i++)
    {
        //
        //  Check for expired subscription
        //
        try{
            if (_isExpired (activeSubscriptions [i]))
            {
               CIMObjectPath path = activeSubscriptions [i].getPath ();
                _deleteExpiredSubscription (path);
               //If subscription is expired delete the subscription
               // and continue on to the next one.
               continue;
            }
        } catch (DateTimeOutOfRangeException& e)
        {
            //
            //  This instance from the repository is invalid
            //  Log a message and skip it
            //
            Logger::put (Logger::STANDARD_LOG , System::CIMSERVER, 
                Logger::WARNING,
                "An invalid Subscription instance was ignored: " + 
                e.getMessage ());
            continue;
        }

        CIMNamespaceName sourceNameSpace;
        Array <CIMName> indicationSubclasses;
        _getCreateParams
            (activeSubscriptions [i].getPath ().getNameSpace (),
            activeSubscriptions [i], indicationSubclasses, indicationProviders,
            propertyList, sourceNameSpace, condition, query, queryLanguage);

        if (indicationProviders.size () == 0)
        {
            //
            //  There are no providers that can support this subscription
            //  Implement the subscription's On Fatal Error Policy
            //  If subscription is not disabled or removed,
            //  Append this subscription to no provider list and
            //  Insert entries into the subscription hash tables
            //
            if (!_subscriptionRepository->reconcileFatalError
                (activeSubscriptions [i]))
            {
                noProviderSubscriptions.append (activeSubscriptions [i]);

                _subscriptionTable->insertSubscription (activeSubscriptions [i],
                    indicationProviders, indicationSubclasses, sourceNameSpace);
            }
            continue;
        }

        //
        //  Send Create request message to each provider
        //  NOTE: These Create requests are not associated with a user request,
        //  so there is no associated authType or userName
        //  The Creator from the subscription instance is used for userName,
        //  and authType is not set
        //
        CIMInstance instance = activeSubscriptions [i];
        String creator;
        if (!_getCreator (instance, creator))
        {
            //
            //  This instance from the repository is corrupted
            //  Skip it
            //
            invalidInstance = true;
            break;
        }

//l10n start
        // Get the language tags that were saved with the subscription instance
        String acceptLangs = String::EMPTY;
        Uint32 propIndex = instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);
        if (propIndex != PEG_NOT_FOUND)
        {  
             instance.getProperty(propIndex).getValue().get(acceptLangs);   
        }         
        String contentLangs = String::EMPTY;
        propIndex = instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);
        if (propIndex != PEG_NOT_FOUND)
        {  
             instance.getProperty(propIndex).getValue().get(contentLangs);   
        }
// l10n end

        _sendCreateRequests (indicationProviders, sourceNameSpace,
            propertyList, condition, query, queryLanguage,
            activeSubscriptions [i],
            AcceptLanguages(acceptLangs), // l10n
            ContentLanguages(contentLangs),  // 110n
            0,  // initialize -- no request
            indicationSubclasses,
            creator);

    }  // for each active subscription

    //
    //  Log a message if any invalid instances were found
    //
    if (invalidInstance)
    {
        Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
            _MSG_INVALID_INSTANCES_KEY, _MSG_INVALID_INSTANCES);
    }

    //
    //  Log a message for any subscription for which there is no longer any
    //  provider
    //
    if (noProviderSubscriptions.size () > 0)
    {
#if 0
        //
        //  Send NoProviderAlertIndication to handler instances
        //  ATTN: NoProviderAlertIndication must be defined
        //
        CIMInstance indicationInstance = _createAlertInstance
            (_CLASS_NO_PROVIDER_ALERT, noProviderSubscriptions);

        Tracer::trace (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
            "Sending NoProvider Alert for %d subscriptions",
            noProviderSubscriptions.size ());
        _sendAlerts (noProviderSubscriptions, indicationInstance);
#endif
        //
        //  Log a message for each subscription
        //
        for (Uint32 i = 0; i < noProviderSubscriptions.size (); i++)
        {
            //
            //  Get Subscription Filter Name and Handler Name
            //
            String logString = _getSubscriptionLogString
                (noProviderSubscriptions [i]);

            Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER,
                Logger::WARNING, _MSG_NO_PROVIDER_KEY, _MSG_NO_PROVIDER,
                logString);
        }
    }

#ifdef PEGASUS_INDICATION_PERFINST
    double elapsed = stopWatch.getElapsed ();

    Tracer::trace (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
        "%s: %.3f seconds", "Initialize", elapsed);
#endif

    PEG_METHOD_EXIT ();
}

void IndicationService::_terminate (void)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_terminate");

    Array <CIMInstance> activeSubscriptions;
    CIMInstance indicationInstance;

    //
    //  A message is already logged that CIM Server is shutting down --
    //  no need to log a message
    //
#if 0
    //
    //  Get existing active subscriptions from hash table
    //
    activeSubscriptions = _getActiveSubscriptions ();

    if (activeSubscriptions.size () > 0)
    {
        //
        //  Create CimomShutdownAlertIndication instance
        //  ATTN: CimomShutdownAlertIndication must be defined
        //
        indicationInstance = _createAlertInstance (_CLASS_CIMOM_SHUTDOWN_ALERT,
            activeSubscriptions);

        //
        //  Send CimomShutdownAlertIndication to each unique handler instance
        //
        Tracer::trace (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
            "Sending CIMServerShutdown Alert for %d subscriptions",
            activeSubscriptions.size ());
        _sendAlerts (activeSubscriptions, indicationInstance);
    }
#endif

    //
    //  Remove entries from the SubscriptionTable's Active Subscriptions and
    //  Subscription Classes tables
    //
    //  NOTE: The table entries are removed when the SubscriptionTable
    //  destructor is called by the IndicationService destructor.  However,
    //  currently the IndicationService destructor is never called, so the
    //  IndicationService must call the SubscriptionTable clear() function to
    //  remove the table entries.
    _subscriptionTable->clear ();

    PEG_METHOD_EXIT ();
}

void IndicationService::_checkNonprivilegedAuthorization(
    const String & userName)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_checkNonprivilegedAuthorization");

    Boolean accessDenied = false;
    try
    {
        if (_enableSubscriptionsForNonprivilegedUsers)
        {
           return;
        }
        else
        {
           PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
              "_checkNonprivilegedAuthorization - checking whether user is privileged"
              + userName);
           if (!System::isPrivilegedUser(userName))
           {
               accessDenied = true;
           }
        }
    }
    catch (...)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }
    if (accessDenied)
    {
       MessageLoaderParms parms(_MSG_NON_PRIVILEGED_ACCESS_DISABLED_KEY,
          _MSG_NON_PRIVILEGED_ACCESS_DISABLED, userName);
       PEG_METHOD_EXIT();
       throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, parms);
    }
}

void IndicationService::_handleCreateInstanceRequest (const Message * message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleCreateInstanceRequest");

    CIMCreateInstanceRequestMessage* request =
        (CIMCreateInstanceRequestMessage*) message;

    CIMException cimException;
    Boolean responseSent = false;

    CIMObjectPath instanceRef;

    CIMInstance instance = request->newInstance.clone ();

    try
    {
        String userName = ((IdentityContainer)request->operationContext.get
            (IdentityContainer :: NAME)).getUserName();
        _checkNonprivilegedAuthorization(userName);

        AcceptLanguages acceptLangs = 
            ((AcceptLanguageListContainer)request->operationContext.get
            (AcceptLanguageListContainer::NAME)).getLanguages();
        ContentLanguages contentLangs =
            ((ContentLanguageListContainer)request->operationContext.get
            (ContentLanguageListContainer::NAME)).getLanguages();

        if (_canCreate (instance, request->nameSpace))
        {
            //
            //  If the instance is of the PEGASUS_CLASSNAME_INDSUBSCRIPTION
            //  class and subscription state is enabled, determine if any
            //  providers can serve the subscription
            //
            Uint16 subscriptionState;
            String condition;
            String query;
            String queryLanguage;
            CIMPropertyList requiredProperties;
            CIMNamespaceName sourceNameSpace;
            Array <CIMName> indicationSubclasses;
            Array <ProviderClassList> indicationProviders;

            if (instance.getClassName ().equal
                (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
            {
                //
                //  Get subscription state
                //
                //  NOTE: _canCreate has already validated the
                //  SubscriptionState property in the instance; if missing, it
                //  was added with the default value; if null, it was set to
                //  the default value; if invalid, an exception was thrown
                //
                CIMValue subscriptionStateValue;
                subscriptionStateValue = instance.getProperty
                    (instance.findProperty (_PROPERTY_STATE)).getValue ();
                subscriptionStateValue.get (subscriptionState);

                if ((subscriptionState == _STATE_ENABLED) ||
                    (subscriptionState == _STATE_ENABLEDDEGRADED))
                {
                    _getCreateParams (request->nameSpace, instance,
                        indicationSubclasses, indicationProviders,
                        requiredProperties, sourceNameSpace, condition,
                        query, queryLanguage);

                    if (indicationProviders.size () == 0)
                    {
                        //
                        //  There are no providers that can support this
                        //  subscription
                        //
                        PEG_METHOD_EXIT ();

                        // l10n
                        // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                        //  _MSG_NO_PROVIDERS);

                        throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_NOT_SUPPORTED,
                            MessageLoaderParms(_MSG_NO_PROVIDERS_KEY,
                                _MSG_NO_PROVIDERS));
                    }

                    //
                    //  Send Create request message to each provider
                    //
                    // l10n
                    _sendCreateRequests (indicationProviders,
                        sourceNameSpace, requiredProperties, condition,
                        query, queryLanguage, instance,
                        acceptLangs,
                        contentLangs,
                        request,
                        indicationSubclasses,
                        userName, request->authType);

                    //
                    //  Response is sent from _handleCreateResponseAggregation
                    //  or _handleEnableResponseAggregation
                    //
                    responseSent = true;
                }
                else
                {
                    //
                    //  Create instance for disabled subscription
                    //
                    instanceRef = _subscriptionRepository->createInstance
                        (instance, request->nameSpace, userName,
                         acceptLangs, contentLangs,
                        false);
                }
            }
            else
            {
                //
                //  Create instance for filter or handler
                //
                instanceRef = _subscriptionRepository->createInstance
                    (instance, request->nameSpace, userName,
                     acceptLangs, contentLangs,
                    false);
            }
        }
    }
    catch (CIMException & exception)
    {
        cimException = exception;
    }
    catch (Exception & exception)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            exception.getMessage());
    }

    //
    //  Send response, if not sent from callback
    //  (for example, if there are no indication providers that can support a
    //  subscription)
    //
    if (!responseSent)
    {
// l10n - no Content-Language in response
        CIMCreateInstanceResponseMessage* response =
            new CIMCreateInstanceResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                instanceRef);

        //
        //  Preserve message key
        //
        response->setKey (request->getKey ());

        //
        //  Set response destination
        //
        response->dest = request->queueIds.top ();

        //
        //  Set HTTP method in response from request
        //
        response->setHttpMethod (request->getHttpMethod ());

        Base::_enqueueResponse (request, response);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleGetInstanceRequest (const Message* message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleGetInstanceRequest");

    CIMGetInstanceRequestMessage* request =
        (CIMGetInstanceRequestMessage*) message;

    CIMException cimException;
    CIMInstance instance;
    String contentLangs = String::EMPTY;  // l10n

    try
    {
        String userName = ((IdentityContainer)request->operationContext.get
            (IdentityContainer :: NAME)).getUserName();
        _checkNonprivilegedAuthorization(userName );

        //
        //  Add Creator to property list, if not null
        //  Also, if a Subscription and Time Remaining is requested,
        //  Ensure Subscription Duration and Start Time are in property list
        //
        Boolean setTimeRemaining;
        Boolean startTimeAdded;
        Boolean durationAdded;
        CIMPropertyList propertyList = request->propertyList;
        CIMName className = request->instanceName.getClassName ();
        _updatePropertyList (className, propertyList, setTimeRemaining,
            startTimeAdded, durationAdded);

        //
        //  Get instance from repository
        //
        instance = _subscriptionRepository->getInstance
            (request->nameSpace, request->instanceName, request->localOnly,
            request->includeQualifiers, request->includeClassOrigin,
            propertyList);

        //
        //  Remove Creator property from instance before returning
        //
        String creator;
        if (!_getCreator (instance, creator))
        {
            //
            //  This instance from the repository is corrupted
            //  L10N TODO DONE -- new throw of exception
            //
            PEG_METHOD_EXIT ();
            //throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED,
                //_MSG_INVALID_INSTANCES);
            MessageLoaderParms parms(_MSG_INVALID_INSTANCES_KEY, 
                _MSG_INVALID_INSTANCES);
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
        }
        instance.removeProperty (instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_CREATOR));

// l10n start
        //
        //  Remove the language properties from instance before returning
        //
        Uint32 propIndex = instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);      
        if (propIndex != PEG_NOT_FOUND)
        {
             instance.removeProperty (propIndex);
        }

        propIndex = instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);      
        if (propIndex != PEG_NOT_FOUND)
        {
             // Get the content languages to be sent in the Content-Language header
             instance.getProperty(propIndex).getValue().get(contentLangs);
             instance.removeProperty (propIndex);
        }
// l10n end

        //
        //  If a subscription with a duration, calculate subscription time
        //  remaining, and add property to the instance
        //
        if (setTimeRemaining)
        {
            try
            {
                _setTimeRemaining (instance);
            }
            catch (DateTimeOutOfRangeException &)
            {
                //
                //  This instance from the repository is invalid
                //
                throw;
            }
            if (startTimeAdded)
            {
                instance.removeProperty (instance.findProperty
                    (_PROPERTY_STARTTIME));
            }
            if (durationAdded)
            {
                instance.removeProperty (instance.findProperty
                    (_PROPERTY_DURATION));
            }
        }
    }
    catch (CIMException& exception)
    {
        cimException = exception;
    }
    catch (Exception& exception)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                             exception.getMessage());
    }

// l10n
    // Note: setting Content-Language in the response to the contentLanguage
    // in the repository.
    CIMGetInstanceResponseMessage* response = new CIMGetInstanceResponseMessage
        (request->messageId,
        cimException,
        request->queueIds.copyAndPop(),
        instance);

    response->operationContext.set(ContentLanguageListContainer
        (ContentLanguages(contentLangs)));

    //
    //  Preserve message key
    //
    response->setKey (request->getKey ());

    //
    //  Set response destination
    //
    response->dest = request->queueIds.top ();

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    Base::_enqueueResponse (request, response);

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleEnumerateInstancesRequest(const Message* message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleEnumerateInstancesRequest");

    CIMEnumerateInstancesRequestMessage* request =
        (CIMEnumerateInstancesRequestMessage*) message;

    Array <CIMInstance> enumInstances;
    Array <CIMInstance> returnedInstances;

    CIMException cimException;
    CIMInstance cimInstance;
    String aggregatedLangs = String::EMPTY;    // l10n


    try
    {
        String userName = ((IdentityContainer)request->operationContext.get
            (IdentityContainer :: NAME)).getUserName();
        _checkNonprivilegedAuthorization(userName);

        //
        //  Add Creator to property list, if not null
        //  Also, if a Subscription and Time Remaining is requested,
        //  Ensure Subscription Duration and Start Time are in property list
        //
        Boolean setTimeRemaining;
        Boolean startTimeAdded;
        Boolean durationAdded;
        CIMPropertyList propertyList = request->propertyList;
        _updatePropertyList (request->className,
            propertyList, setTimeRemaining, startTimeAdded, durationAdded);

        enumInstances = _subscriptionRepository->enumerateInstancesForClass
            (request->nameSpace, request->className,
             request->deepInheritance, request->localOnly,
             request->includeQualifiers, request->includeClassOrigin,
             false, propertyList);

// l10n
        // Vars used to aggregate the content languages of the subscription
        // instances.
        Boolean langMismatch = false;
        Uint32 propIndex;

        //
        //  Remove Creator and language properties from instances before 
        //  returning
        //
        for (Uint32 i = 0; i < enumInstances.size (); i++)
        {
            String creator;
            if (!_getCreator (enumInstances [i], creator))
            {
                //
                //  This instance from the repository is corrupted
                //  Skip it
                //
                break;
            }
            enumInstances [i].removeProperty
                (enumInstances [i].findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_CREATOR));

// l10n start
            propIndex = enumInstances [i].findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);      
            String contentLangs = String::EMPTY;
            if (propIndex != PEG_NOT_FOUND)
            {
                enumInstances [i].getProperty(propIndex).getValue().get
                    (contentLangs); 
                enumInstances [i].removeProperty(propIndex); 
            }

            propIndex = enumInstances [i].findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);
            if (propIndex != PEG_NOT_FOUND)
            {
                enumInstances [i].removeProperty(propIndex);
            }

            // Determine what to set into the Content-Language header back to the client
            if (!langMismatch)
            {
                if (contentLangs == String::EMPTY)
                {
                    langMismatch = true;
                    aggregatedLangs = String::EMPTY;
                }
                else
                {
                    if (aggregatedLangs == String::EMPTY)
                    {
                        aggregatedLangs = contentLangs;
                    }
                    else if (aggregatedLangs != contentLangs)
                    {
                        langMismatch = true;
                        aggregatedLangs = String::EMPTY;
                    }
                }
            }
// l10n end

            //
            //  If a subscription with a duration, calculate subscription time
            //  remaining, and add property to the instance
            //
            if (setTimeRemaining)
            {
                try
                {
                    _setTimeRemaining (enumInstances [i]);
                } catch (DateTimeOutOfRangeException &)
                {
                    //
                    //  This instance from the repository is invalid
                    //  Skip it
                    //
                    continue;
                }
                if (startTimeAdded)
                {
                    enumInstances [i].removeProperty
                        (enumInstances [i].findProperty (_PROPERTY_STARTTIME));
                }
                if (durationAdded)
                {
                    enumInstances [i].removeProperty
                        (enumInstances [i].findProperty (_PROPERTY_DURATION));
                }
            }

            returnedInstances.append (enumInstances [i]);
        }
    }
    catch (CIMException& exception)
    {
        cimException = exception;
    }
    catch (Exception& exception)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                             exception.getMessage());
    }

// l10n
    // Note: setting Content-Language in the response to the aggregated
    // contentLanguage from the instances in the repository.
    CIMEnumerateInstancesResponseMessage* response =
        new CIMEnumerateInstancesResponseMessage(
            request->messageId,
            cimException,
            request->queueIds.copyAndPop(),
            returnedInstances);

    response->operationContext.set(ContentLanguageListContainer
        (ContentLanguages(aggregatedLangs))); 

    //
    //  Preserve message key
    //
    response->setKey (request->getKey ());

    //
    //  Set response destination
    //
    response->dest = request->queueIds.top ();

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    Base::_enqueueResponse (request, response);

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleEnumerateInstanceNamesRequest
    (const Message* message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleEnumerateInstancesNamesRequest");

    CIMEnumerateInstanceNamesRequestMessage* request =
        (CIMEnumerateInstanceNamesRequestMessage*) message;

    Array<CIMObjectPath> enumInstanceNames;

    CIMException cimException;

    try
    {
        String userName = ((IdentityContainer)request->operationContext.get
            (IdentityContainer :: NAME)).getUserName();
        _checkNonprivilegedAuthorization(userName);
        enumInstanceNames =
            _subscriptionRepository->enumerateInstanceNamesForClass
            (request->nameSpace, request->className, false);
    }
    catch (CIMException& exception)
    {
        cimException = exception;
    }
    catch (Exception& exception)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                             exception.getMessage());
    }

// l10n
    // Note: not setting Content-Language in the response
    CIMEnumerateInstanceNamesResponseMessage* response =
        new CIMEnumerateInstanceNamesResponseMessage(
            request->messageId,
            cimException,
            request->queueIds.copyAndPop(),
            enumInstanceNames);

    // preserve message key
    response->setKey(request->getKey());


    //  Set the response destination !!!
    response->dest = request->queueIds.top();

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // Note: In this particular case you can call either SendForget
    // OR Base::_enqueueResponse
    //    SendForget(response);
    Base::_enqueueResponse(request, response);

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleModifyInstanceRequest (const Message* message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleModifyInstanceRequest");

    CIMModifyInstanceRequestMessage* request =
        (CIMModifyInstanceRequestMessage*) message;

    CIMException cimException;
    Boolean responseSent = false;

    try
    {
        String userName = ((IdentityContainer)request->operationContext.get
            (IdentityContainer :: NAME)).getUserName();
        _checkNonprivilegedAuthorization(userName);

        //
        //  Get the instance name
        //
        CIMObjectPath instanceReference =
            request->modifiedInstance.getPath ();

        //
        //  Get instance from repository
        //
        CIMInstance instance;

        instance = _subscriptionRepository->getInstance
            (request->nameSpace, instanceReference);

        CIMInstance modifiedInstance = request->modifiedInstance;
        if (_canModify (request, instanceReference, instance, modifiedInstance))
        {
            //
            //  Check for expired subscription
            //
            try
            {
                if (_isExpired (instance))
                {
                  //
                  //  Delete the subscription instance
                  //
                  instanceReference.setNameSpace (request->nameSpace);
                  _deleteExpiredSubscription (instanceReference);

                  PEG_METHOD_EXIT ();

                  // l10n

                  // String exceptionStr = _MSG_EXPIRED;
                  // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);

                  throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED,
                      MessageLoaderParms(_MSG_EXPIRED_KEY, _MSG_EXPIRED));
                }
            } catch (DateTimeOutOfRangeException &)
            {
                //
                //  This instance from the repository is invalid
                //
                throw;
            }

            //
            //  _canModify, above, already checked that propertyList is not
            //  null, and that numProperties is 0 or 1
            //
            CIMPropertyList propertyList = request->propertyList;
            if (request->propertyList.size () > 0)
            {
                //
                //  Get current state from instance
                //
                Uint16 currentState;
                Boolean valid = true;
                if (_subscriptionRepository->getState (instance, currentState))
                {
                    valid = _validateState (currentState);
                }

                if (!valid)
                {
                    //
                    //  This instance from the repository is corrupted
                    //  L10N TODO DONE -- new throw of exception
                    //
                    PEG_METHOD_EXIT ();
                    //throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED,
                        //_MSG_INVALID_INSTANCES);
                    MessageLoaderParms parms (_MSG_INVALID_INSTANCES_KEY,
                        _MSG_INVALID_INSTANCES);
                    throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED, parms);
                }

                //
                //  Get new state
                //
                //  NOTE: _canModify has already validated the
                //  SubscriptionState property in the instance; if missing, it
                //  was added with the default value; if null, it was set to
                //  the default value; if invalid, an exception was thrown
                //
                Uint16 newState;
                modifiedInstance.getProperty (modifiedInstance.findProperty
                    (_PROPERTY_STATE)).getValue ().get (newState);

                //
                //  If Subscription State has changed,
                //  Set Time of Last State Change to current date time
                //
                CIMDateTime currentDateTime =
                    CIMDateTime::getCurrentDateTime ();
                if (newState != currentState)
                {
                    if (modifiedInstance.findProperty (_PROPERTY_LASTCHANGE) !=
                        PEG_NOT_FOUND)
                    {
                        CIMProperty lastChange = modifiedInstance.getProperty
                            (modifiedInstance.findProperty
                            (_PROPERTY_LASTCHANGE));
                        lastChange.setValue (CIMValue (currentDateTime));
                    }
                    else
                    {
                        modifiedInstance.addProperty (CIMProperty
                            (_PROPERTY_LASTCHANGE, CIMValue (currentDateTime)));
                    }
                    Array <CIMName> properties =
                        propertyList.getPropertyNameArray ();
                    properties.append (_PROPERTY_LASTCHANGE);
                    propertyList.set (properties);
                }

                //
                //  If Subscription is to be enabled, and this is the first
                //  time, set Subscription Start Time
                //
                if ((newState == _STATE_ENABLED) ||
                    (newState == _STATE_ENABLEDDEGRADED))
                {
                    //
                    //  If Subscription Start Time is null, set value
                    //  to the current date time
                    //
                    CIMDateTime startTime;
                    CIMProperty startTimeProperty = instance.getProperty
                        (instance.findProperty (_PROPERTY_STARTTIME));
                    CIMValue startTimeValue = instance.getProperty
                        (instance.findProperty
                        (_PROPERTY_STARTTIME)).getValue ();
                    Boolean setStart = false;
                    if (startTimeValue.isNull ())
                    {
                        setStart = true;
                    }
                    else
                    {
                        startTimeValue.get (startTime);
                        if (startTime.equal
                            (CIMDateTime (_ZERO_INTERVAL_STRING)))
                        {
                            setStart = true;
                        }
                    }

                    if (setStart)
                    {
                        if (modifiedInstance.findProperty (_PROPERTY_STARTTIME)
                            != PEG_NOT_FOUND)
                        {
                            CIMProperty startTime = modifiedInstance.getProperty
                                (modifiedInstance.findProperty
                                (_PROPERTY_STARTTIME));
                            startTime.setValue (CIMValue (currentDateTime));
                        }
                        else
                        {
                            modifiedInstance.addProperty (CIMProperty
                                (_PROPERTY_STARTTIME,
                                CIMValue (currentDateTime)));
                        }

                        Array <CIMName> properties =
                            propertyList.getPropertyNameArray ();
                        properties.append (_PROPERTY_STARTTIME);
                        propertyList.set (properties);
                    }
                }

// l10n
                // Add the language properties to the modified instance.
                // Note:  These came from the Accept-Language and Content-Language
                // headers in the HTTP messages, and may be empty.
                AcceptLanguages acceptLangs = 
                    ((AcceptLanguageListContainer)request->operationContext.get
                    (AcceptLanguageListContainer::NAME)).getLanguages();
                modifiedInstance.addProperty (CIMProperty 
                    (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS, 
                    acceptLangs.toString()));

                ContentLanguages contentLangs =
                    ((ContentLanguageListContainer)request->operationContext.get
                    (ContentLanguageListContainer::NAME)).getLanguages();
                modifiedInstance.addProperty (CIMProperty 
                    (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS, 
                    contentLangs.toString()));

                Array <CIMName> properties = propertyList.getPropertyNameArray ();
                properties.append (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);
                properties.append (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);
                propertyList.set (properties);
// l10n -end

                //
                //  If subscription is to be enabled, determine if there are
                //  any indication providers that can serve the subscription
                //
                Array <ProviderClassList> indicationProviders;
                CIMPropertyList requiredProperties;
                CIMNamespaceName sourceNameSpace;
                String condition;
                String query;
                String queryLanguage;
                Array <CIMName> indicationSubclasses;

                if (((newState == _STATE_ENABLED) ||
                     (newState == _STATE_ENABLEDDEGRADED))
                    && ((currentState != _STATE_ENABLED) &&
                        (currentState != _STATE_ENABLEDDEGRADED)))
                {
                    //
                    //  Subscription was previously not enabled but is now to
                    //  be enabled
                    //
                    _getCreateParams (request->nameSpace, instance,
                        indicationSubclasses, indicationProviders,
                        requiredProperties, sourceNameSpace, condition,
                        query, queryLanguage);

                    if (indicationProviders.size () == 0)
                    {
                        //
                        //  There are no providers that can support this
                        //  subscription
                        //
                        instance.setPath (instanceReference);
                        _subscriptionRepository->reconcileFatalError (instance);
                        PEG_METHOD_EXIT ();

                        // l10n
                        // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                        //  _MSG_NO_PROVIDERS);

                        throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_NOT_SUPPORTED,
                            MessageLoaderParms (_MSG_NO_PROVIDERS_KEY,
                            _MSG_NO_PROVIDERS));
                    }
                }

                //
                //  Modify the instance in the repository
                //
                try
                {
                    modifiedInstance.setPath (instanceReference);
                    _subscriptionRepository->modifyInstance
                        (request->nameSpace, modifiedInstance,
                        request->includeQualifiers, propertyList);

                    Logger::put (Logger::STANDARD_LOG, System::CIMSERVER,
                        Logger::TRACE,
                        "IndicationService::_handleModifyInstanceRequest - Name Space: $0  Instance name: $1",
                        request->nameSpace.getString (),
                        modifiedInstance.getClassName ().getString ());
                }
                catch (CIMException & exception)
                {
                    cimException = exception;
                }
                catch (Exception & exception)
                {
                    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                       exception.getMessage());
                }

                if (cimException.getCode() != CIM_ERR_SUCCESS)
                {
                    CIMModifyInstanceResponseMessage* response =
                        new CIMModifyInstanceResponseMessage (
                            request->messageId,
                            cimException,
                            request->queueIds.copyAndPop ());

                    //
                    //  Preserve message key
                    //
                    response->setKey (request->getKey ());

                    //
                    //  Set response destination
                    //
                    response->dest = request->queueIds.top ();

                    //
                    //  Set HTTP method in response from request
                    //
                    response->setHttpMethod (request->getHttpMethod ());

                    Base::_enqueueResponse (request, response);

                    PEG_METHOD_EXIT ();
                    return;
                }

                //
                //  If subscription is newly enabled, send Create requests
                //  and enable providers
                //
                if (((newState == _STATE_ENABLED) ||
                     (newState == _STATE_ENABLEDDEGRADED))
                    && ((currentState != _STATE_ENABLED) &&
                        (currentState != _STATE_ENABLEDDEGRADED)))
                {
                    instanceReference.setNameSpace (request->nameSpace);
                    instance.setPath (instanceReference);
// l10n
                    _sendCreateRequests (indicationProviders,
                        sourceNameSpace, requiredProperties, condition,
                        query,
                        queryLanguage,
                        instance,
                        acceptLangs,
                        contentLangs,
                        request,
                        indicationSubclasses,
                        userName, request->authType);

                    //
                    //  Response is sent from _handleCreateResponseAggregation
                    //  or _handleEnableResponseAggregation
                    //
                    responseSent = true;
                }
                else if ((newState == _STATE_DISABLED) &&
                         ((currentState == _STATE_ENABLED) ||
                          (currentState == _STATE_ENABLEDDEGRADED)))
                {
                    //
                    //  Subscription was previously enabled but is now to be
                    //  disabled
                    //
                    Array <ProviderClassList> indicationProviders;
                    instanceReference.setNameSpace (request->nameSpace);
                    instance.setPath (instanceReference);
                    indicationProviders = _getDeleteParams (request->nameSpace,
                        instance, indicationSubclasses, sourceNameSpace);

                    //
                    //  Send Delete requests
                    //
//l10n
                    if (indicationProviders.size () > 0)
                    {
                        _sendDeleteRequests (indicationProviders,
                            sourceNameSpace,
                            instance,
                            acceptLangs,
                            contentLangs,
                            request,
                            indicationSubclasses,
                            userName, request->authType);

                        //
                        //  Response is sent from 
                        //  _handleDeleteResponseAggregation or
                        //  _handleDisableResponseAggregation
                        //
                        responseSent = true;
                    }
                }
            }
        }
    }
    catch (CIMException& exception)
    {
        cimException = exception;
    }
    catch (Exception& exception)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                             exception.getMessage());
    }

    //
    //  Send response, if not sent from callback
    //  (for example, if there are no indication providers that can support a
    //  subscription)
    //
    if (!responseSent)
    {
// l10n
        // Note: don't need to set content-language in the response.
        CIMModifyInstanceResponseMessage* response =
            new CIMModifyInstanceResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop());

        //
        //  Preserve message key
        //
        response->setKey (request->getKey ());

        //
        //  Set response destination
        //
        response->dest = request->queueIds.top ();

        //
        //  Set HTTP method in response from request
        //
        response->setHttpMethod (request->getHttpMethod ());

        Base::_enqueueResponse (request, response);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleDeleteInstanceRequest (const Message* message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleDeleteInstanceRequest");

    CIMDeleteInstanceRequestMessage* request =
        (CIMDeleteInstanceRequestMessage*) message;

    CIMException cimException;
    Boolean responseSent = false;

    try
    {
        String userName = ((IdentityContainer)request->operationContext.get
            (IdentityContainer :: NAME)).getUserName();
        _checkNonprivilegedAuthorization(userName);

        //
        //  Check if instance may be deleted -- a filter or handler instance
        //  referenced by a subscription instance may not be deleted
        //
        if (_canDelete (request->instanceName, request->nameSpace,
             userName))
        {
            //
            //  If a subscription, get the instance from the repository
            //
            CIMInstance subscriptionInstance;
            if (request->instanceName.getClassName ().equal
                (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
            {
                subscriptionInstance =
                    _subscriptionRepository->getInstance
                    (request->nameSpace, request->instanceName);
            }

            //
            //  Delete instance from repository
            //
            try
            {
                _subscriptionRepository->deleteInstance
                    (request->nameSpace, request->instanceName);

                Logger::put (Logger::STANDARD_LOG, System::CIMSERVER,
                    Logger::TRACE,
                    "IndicationService::_handleDeleteInstanceRequest - Name Space: $0  Instance name: $1",
                    request->nameSpace.getString (),
                    request->instanceName.getClassName ().getString ());
            }
            catch (CIMException & exception)
            {
                cimException = exception;
            }
            catch (Exception & exception)
            {
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                   exception.getMessage());
            }

            if (cimException.getCode() != CIM_ERR_SUCCESS)
            {
                CIMDeleteInstanceResponseMessage* response =
                    new CIMDeleteInstanceResponseMessage (
                        request->messageId,
                        cimException,
                        request->queueIds.copyAndPop ());

                //
                //  Preserve message key
                //
                response->setKey (request->getKey ());

                //
                //  Set response destination
                //
                response->dest = request->queueIds.top ();

                //
                //  Set HTTP method in response from request
                //
                response->setHttpMethod (request->getHttpMethod ());

                Base::_enqueueResponse (request, response);

                PEG_METHOD_EXIT ();
                return;
            }

            if (request->instanceName.getClassName ().equal
                (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
            {
                //
                //  If subscription is active, send delete requests to providers
                //  and update hash tables
                //
                Uint16 subscriptionState;
                CIMValue subscriptionStateValue;
                subscriptionStateValue = subscriptionInstance.getProperty
                    (subscriptionInstance.findProperty
                    (_PROPERTY_STATE)).getValue ();
                subscriptionStateValue.get (subscriptionState);

                if ((subscriptionState == _STATE_ENABLED) ||
                    (subscriptionState == _STATE_ENABLEDDEGRADED))
                {
                    Array <ProviderClassList> indicationProviders;
                    Array <CIMName> indicationSubclasses;
                    CIMNamespaceName sourceNamespaceName;
                    CIMObjectPath instanceReference = request->instanceName;
                    instanceReference.setNameSpace (request->nameSpace);
                    subscriptionInstance.setPath (instanceReference);

                    indicationProviders = _getDeleteParams
                        (request->nameSpace, subscriptionInstance,
                        indicationSubclasses, sourceNamespaceName);

                    if (indicationProviders.size () > 0)
                    {
                        //
                        //  Send Delete requests
                        //
// l10n
                        _sendDeleteRequests (indicationProviders,
                            sourceNamespaceName, subscriptionInstance,
                            ((AcceptLanguageListContainer)
                            request->operationContext.get
                            (AcceptLanguageListContainer::NAME)).getLanguages(),
                            ((ContentLanguageListContainer)
                            request->operationContext.get
                            (ContentLanguageListContainer::NAME)).getLanguages(),
                            request,
                            indicationSubclasses,
                            userName, request->authType);

                        //
                        //  Response is sent from 
                        //  _handleDeleteResponseAggregation or
                        //  _handleDisableResponseAggregation
                        //
                        responseSent = true;
                    }
                    else
                    {
                        //
                        //  Subscription was enabled, but had no providers
                        //  Remove entries from the subscription hash tables
                        //
                        _subscriptionTable->removeSubscription
                            (subscriptionInstance,
                            indicationSubclasses, sourceNamespaceName,
                            indicationProviders);
                    }
                }
            }
        }
    }
    catch (CIMException& exception)
    {
        cimException = exception;
    }
    catch (Exception& exception)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                             exception.getMessage());
    }

    //
    //  Send response, if not sent from callback
    //  (for example, if a subscription had no indication providers)
    //
    if (!responseSent)
    {
        CIMDeleteInstanceResponseMessage* response =
            new CIMDeleteInstanceResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop());

        //
        //  Preserve message key
        //
        response->setKey (request->getKey ());

        //
        //  Set response destination
        //
        response->dest = request->queueIds.top ();

        //
        //  Set HTTP method in response from request
        //
        response->setHttpMethod (request->getHttpMethod ());

        Base::_enqueueResponse (request, response);
    }

    PEG_METHOD_EXIT ();
}

// l10n TODO - might need to globalize another flow and another consumer interface
// (ie. mdd's) if we can't agree on one export flow and consumer interface
// (see PEP67)

void IndicationService::_handleProcessIndicationRequest (const Message* message)
{
#ifdef PEGASUS_INDICATION_PERFINST
    Stopwatch stopWatch;
    double elapsed;
#endif

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleProcessIndicationRequest");

    CIMProcessIndicationRequestMessage* request =
        (CIMProcessIndicationRequestMessage*) message;

    CIMException cimException;

    CIMProcessIndicationResponseMessage* response =
        new CIMProcessIndicationResponseMessage(
            request->messageId,
            cimException,
            request->queueIds.copyAndPop());

    String filterQuery;
    Boolean match;

    Array <CIMInstance> matchedSubscriptions;
    CIMInstance handlerNamedInstance;

    WQLSelectStatement selectStatement;

    CIMInstance handler;
    CIMInstance indication = request->indicationInstance;

    PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
        "Received Indication " + indication.getClassName().getString());

    try
    {
        WQLSimplePropertySource propertySource =
            _getPropertySourceFromInstance (indication);

        //
        //  Check if property list contains all properties of class
        //  If so, set to null
        //
        Array <CIMName> propertyNames;
        CIMPropertyList propertyList;
        for (Uint32 i = 0; i < indication.getPropertyCount(); i++)
            propertyNames.append(indication.getProperty(i).getName());
        propertyList = _checkPropertyList (propertyNames,
            request->nameSpace, indication.getClassName ());

        Array <CIMNamespaceName> nameSpaces;
        nameSpaces.append (request->nameSpace);

        if (request->subscriptionInstanceNames.size() > 0)
        {
#ifdef PEGASUS_INDICATION_PERFINST
            stopWatch.reset ();
#endif

            for (Uint32 i = 0; i < request->subscriptionInstanceNames.size();
                 i++)
            {
                //
                //  Look up the subscription in the active subscriptions table
                //
                ActiveSubscriptionsTableEntry tableValue;
                if (_subscriptionTable->getSubscriptionEntry
                    (request->subscriptionInstanceNames [i], tableValue))
                {
                    matchedSubscriptions.append (tableValue.subscription);
                }
            }

#ifdef PEGASUS_INDICATION_PERFINST
            elapsed = stopWatch.getElapsed ();

            Tracer::trace (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
                "%s: %.3f seconds",
                "Look up Subscriptions", elapsed);
#endif
        }
        else
        {
#ifdef PEGASUS_INDICATION_PERFINST
            stopWatch.reset ();
#endif

            matchedSubscriptions = _getMatchingSubscriptions(
                indication.getClassName (), nameSpaces, propertyList,
                true, request->provider);

#ifdef PEGASUS_INDICATION_PERFINST
            elapsed = stopWatch.getElapsed ();

            Tracer::trace (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
                "%s: %.3f seconds",
                "Get Matching Subscriptions", elapsed);
#endif
        }

        for (Uint32 i = 0; i < matchedSubscriptions.size(); i++)
        {
            match = true;

            //
            // copy the indication, format it based on the subscription,
            // and send the formatted indication to the consumer
            //
            CIMInstance formattedIndication = indication.clone();

            //
            //  Check for expired subscription
            //
            try
            {
                if (_isExpired (matchedSubscriptions [i]))
                {
                    CIMObjectPath path = matchedSubscriptions [i].getPath ();
                    _deleteExpiredSubscription (path);
                    // If the subscription is expired, delete and continue to the next one.
                    continue;
                }
            } catch (DateTimeOutOfRangeException &)
            {
                //
                //  This instance from the repository is invalid
                //  Skip it
                //
                continue;
            }

            _subscriptionRepository->getFilterProperties (
                matchedSubscriptions[i],
                matchedSubscriptions[i].getPath ().getNameSpace (),
                filterQuery);

            selectStatement = _getSelectStatement (filterQuery);

            if (selectStatement.hasWhereClause())
            {
#ifdef PEGASUS_INDICATION_PERFINST
                stopWatch.reset ();
#endif

                if (!selectStatement.evaluateWhereClause(&propertySource))
                {
                    match = false;
                }

#ifdef PEGASUS_INDICATION_PERFINST
                elapsed = stopWatch.getElapsed ();

                Tracer::trace (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
                    "%s: %.3f seconds",
                    "Evaluate WHERE clause", elapsed);
#endif
            }

            if (match)
            {
                //
                // Format indication
                // Remove properties not listed in SELECT clause from
                // indication as they are not required to be passed to consumer
                // If SELECT includes all properties ("*"), pass all properties
                // to the consumer
                //
                if (!selectStatement.getAllProperties ())
                {
                    CIMPropertyList selectPropertyList;
                    Array <CIMName> selectPropertyNames;

                    //
                    // Get properties listed in SELECT clause
                    //
                    selectPropertyList =
                        selectStatement.getSelectPropertyList ();
                    selectPropertyNames =
                        selectPropertyList.getPropertyNameArray ();

                    //
                    // Remove properties not listed in SELECT clause
                    // from indication
                    //
                    for (Uint32 j = 0; j < propertyNames.size(); j++)
                    {
                        if (!ContainsCIMName (selectPropertyNames,
                                              propertyNames[j]))
                        {
                            formattedIndication.removeProperty(
                                formattedIndication.findProperty
                                (propertyNames[j]));
                        }
                    }
                }

                handlerNamedInstance = _subscriptionRepository->getHandler
                    (matchedSubscriptions[i]);

// l10n
// Note: not expecting any language in the response
                CIMRequestMessage * handler_request =
                    new CIMHandleIndicationRequestMessage (
                        XmlWriter::getNextMessageId (),
                        request->nameSpace,
                        handlerNamedInstance,
                        formattedIndication,
                        QueueIdStack(_handlerService, getQueueId()),
                        String::EMPTY,
                        String::EMPTY);

                handler_request->operationContext = request->operationContext;

                AsyncOpNode* op = this->get_op();

                AsyncLegacyOperationStart *async_req =
                    new AsyncLegacyOperationStart(
                    get_next_xid(),
                    op,
                    _handlerService,
                    handler_request,
                    _queueId);

                PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL4,
                    "Sending (SendAsync) Indication to " +
                    ((MessageQueue::lookup(_handlerService)) ?
                    String(((MessageQueue::lookup
                    (_handlerService))->getQueueName())) :
                    String("BAD queue name")) +
                    " via CIMHandleIndicationRequestMessage");

                SendAsync (op,
                           _handlerService,
                           IndicationService::_handleIndicationCallBack,
                           this,
                           (void *) & (matchedSubscriptions [i]));
            }
        }
    }
    catch (CIMException& exception)
    {
        response->cimException = exception;

        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
            "CIMException caught in attempting to process indication: " +
            exception.getMessage ());
    }
    catch (Exception& exception)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED,
            exception.getMessage ());

        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
            "Exception caught in attempting to process indication: " +
            exception.getMessage ());
    }

    Base::_enqueueResponse (request, response);

    PEG_METHOD_EXIT ();
    return;
}

void IndicationService::_handleIndicationCallBack (
    AsyncOpNode * operation,
    MessageQueue * destination,
    void * userParameter)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleIndicationCallBack");

    IndicationService * service =
        static_cast <IndicationService *> (destination);
    CIMInstance * subscription =
        reinterpret_cast <CIMInstance *> (userParameter);
    AsyncReply * asyncReply =
        static_cast <AsyncReply *> (operation->get_response ());
    CIMHandleIndicationResponseMessage * handlerResponse =
        reinterpret_cast <CIMHandleIndicationResponseMessage *>
        ((static_cast <AsyncLegacyOperationResult *>
        (asyncReply))->get_result ());
    PEGASUS_ASSERT (handlerResponse != 0);

    if (handlerResponse->cimException.getCode () != CIM_ERR_SUCCESS)
    {
        PEG_TRACE_STRING (TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Sending Indication and HandlerService returns "
            "CIMException: " +
            handlerResponse->cimException.getMessage ());

        //
        //  ATTN-CAKG-P1-20020326: Implement subscription's OnFatalErrorPolicy
        //
        //service->_subscriptionRepository->reconcileFatalError (*subscription);
    }

    delete handlerResponse;
    delete asyncReply;
    operation->release ();
    service->return_op (operation);

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleNotifyProviderRegistrationRequest
    (const Message* message)
{
    PEG_METHOD_ENTER ( TRC_INDICATION_SERVICE,
        "IndicationService::_handleNotifyProviderRegistrationRequest");

    CIMNotifyProviderRegistrationRequestMessage* request =
        (CIMNotifyProviderRegistrationRequestMessage*) message;

    CIMException cimException;
    Boolean responseSent = false;

    ProviderIdContainer pidc = request->operationContext.get
        (ProviderIdContainer::NAME); 
    CIMInstance provider = pidc.getProvider(); 
    CIMInstance providerModule = pidc.getModule();
    CIMName className = request->className;
    Array <CIMNamespaceName> newNameSpaces = request->newNamespaces;
    Array <CIMNamespaceName> oldNameSpaces = request->oldNamespaces;
    CIMPropertyList newPropertyNames = request->newPropertyNames;
    CIMPropertyList oldPropertyNames = request->oldPropertyNames;

    //
    //  Set path in each instance, so instances may later be easily
    //  compared
    //  ATTN: this code may be removed once bugzilla 1196 has been fixed
    //
    CIMClass providerClass = _subscriptionRepository->getClass
        (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER,
         true, true, false, CIMPropertyList ());
    CIMClass providerModuleClass = _subscriptionRepository->getClass
        (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDERMODULE,
         true, true, false, CIMPropertyList ());
    provider.setPath (provider.buildPath (providerClass));
    providerModule.setPath (providerModule.buildPath (providerModuleClass));

    Array <CIMInstance> newSubscriptions;
    Array <CIMInstance> formerSubscriptions;
    Array <ProviderClassList> indicationProviders;
    ProviderClassList indicationProvider;

    newSubscriptions.clear ();
    formerSubscriptions.clear ();

    switch (request->operation)
    {
        case OP_CREATE:
        {
            //
            //  Get matching subscriptions
            //
            newSubscriptions = _getMatchingSubscriptions (className,
                newNameSpaces, newPropertyNames);

            break;
        }

        case OP_DELETE:
        {
            //
            //  Get matching subscriptions
            //
            formerSubscriptions = _getMatchingSubscriptions (className,
                oldNameSpaces, oldPropertyNames);

            break;
        }

        case OP_MODIFY:
        {
            //
            //  Get lists of affected subscriptions
            //
            _getModifiedSubscriptions (className, newNameSpaces, oldNameSpaces,
                newPropertyNames, oldPropertyNames,
                newSubscriptions, formerSubscriptions);

            break;
        }
        default:
            //
            //  Error condition: operation not supported
            //
            PEG_METHOD_EXIT ();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
            break;
    }  // switch

    //
    //  Construct provider class list from input provider and class name
    //
    indicationProvider.provider = provider;
    indicationProvider.providerModule = providerModule;
    indicationProvider.classList.append (className);
    indicationProviders.append (indicationProvider);

    if (newSubscriptions.size () > 0)
    {
        CIMPropertyList requiredProperties;
        String condition;
        String query;
        String queryLanguage;

        //
        //  Send Create or Modify request for each subscription that can newly
        //  be supported
        //
        for (Uint32 i = 0; i < newSubscriptions.size (); i++)
        {
            CIMNamespaceName sourceNameSpace;
            Array <CIMName> indicationSubclasses;
            _getCreateParams
                (newSubscriptions [i].getPath ().getNameSpace (),
                newSubscriptions [i], indicationSubclasses,
                requiredProperties, sourceNameSpace, condition,
                query, queryLanguage);

            //
            //  NOTE: These Create or Modify requests are not associated with a
            //  user request, so there is no associated authType or userName
            //  The Creator from the subscription instance is used for
            //  userName, and authType is not set
            //
            //  NOTE: the subscriptions in the newSubscriptions list came from
            //  the IndicationService's internal hash tables, and thus
            //  each instance is known to have a valid Creator property
            //
            CIMInstance instance = newSubscriptions [i];
            String creator = instance.getProperty (instance.findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_CREATOR)).getValue ().toString ();

// l10n start
            String acceptLangs = String::EMPTY;
            Uint32 propIndex = instance.findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);
            if (propIndex != PEG_NOT_FOUND)
            {
                 instance.getProperty(propIndex).getValue().get(acceptLangs);
            }
            String contentLangs = String::EMPTY;
            propIndex = instance.findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);
            if (propIndex != PEG_NOT_FOUND)
            {
                 instance.getProperty(propIndex).getValue().get(contentLangs);
            }
// l10n end

            //
            //  Look up the subscription in the active subscriptions table
            //
            ActiveSubscriptionsTableEntry tableValue;
            if (_subscriptionTable->getSubscriptionEntry
                (newSubscriptions [i].getPath (), tableValue))
            {
                //
                //  If the provider is already in the subscription's list,
                //  send a Modify request, otherwise send a Create request
                //
                Uint32 providerIndex = _subscriptionTable->providerInList
                    (provider, tableValue);
                if (providerIndex != PEG_NOT_FOUND)
                {
                    //
                    //  Send Modify requests
                    //
// l10n
                    _sendModifyRequests (indicationProviders,
                        sourceNameSpace,
                        requiredProperties, condition, query, queryLanguage,
                        newSubscriptions [i],
                        AcceptLanguages(acceptLangs),
                        ContentLanguages(contentLangs),
                        request,
                        creator);

                    //
                    //  Response is sent from _handleModifyResponseAggregation
                    //
                    responseSent = true;
                }
                else
                {
                    //
                    //  Send Create requests
                    //
// l10n
                    _sendCreateRequests (indicationProviders,
                        sourceNameSpace, requiredProperties, condition,
                        query, queryLanguage, newSubscriptions [i],
                        AcceptLanguages(acceptLangs),
                        ContentLanguages(contentLangs),
                        request,
                        indicationSubclasses,
                        creator);

                    //
                    //  Response is sent from _handleCreateResponseAggregation
                    //  or _handleEnableResponseAggregation
                    //
                    responseSent = true;
                }
            }
            else
            {
                //
                //  Subscription not found in Active Subscriptions table
                //
            }
        }

        //
        //  NOTE: When a provider that was previously not serving a subscription
        //  now serves the subscription due to a provider registration change,
        //  a log message is sent, even if there were previously other providers
        //  serving the subscription
        //

        //
        //  Log a message for each subscription
        //
        CIMClass providerClass = _subscriptionRepository->getClass
            (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER,
             true, true, false, CIMPropertyList ());
        CIMInstance providerCopy = provider.clone ();
        CIMObjectPath path = providerCopy.buildPath (providerClass);
        providerCopy.setPath (path);
        for (Uint32 j = 0; j < newSubscriptions.size (); j++)
        {
            //
            //  Get Provider Name, Subscription Filter Name and Handler Name
            //
            String logString1 = getProviderLogString (providerCopy);
            String logString2 = _getSubscriptionLogString
                (newSubscriptions [j]);

            Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER,
                Logger::WARNING, _MSG_PROVIDER_NOW_SERVING_KEY,
                _MSG_PROVIDER_NOW_SERVING, logString1, logString2);
        }
    }

    if (formerSubscriptions.size () > 0)
    {
        CIMPropertyList requiredProperties;
        String condition;
        String query;
        String queryLanguage;

        //
        //  Send Delete or Modify request for each subscription that can no
        //  longer be supported
        //
        for (Uint32 i = 0; i < formerSubscriptions.size (); i++)
        {
            //
            //  NOTE: These Delete or Modify requests are not associated with a
            //  user request, so there is no associated authType or userName
            //  The Creator from the subscription instance is used for userName,
            //  and authType is not set
            //
            //  NOTE: the subscriptions in the formerSubscriptions list came
            //  from the IndicationService's internal hash tables, and thus
            //  each instance is known to have a valid Creator property
            //
            CIMInstance instance = formerSubscriptions [i];
            String creator = instance.getProperty (instance.findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_CREATOR)).getValue ().toString ();
// l10n start
            String acceptLangs = String::EMPTY;
            Uint32 propIndex = instance.findProperty(PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);
            if (propIndex != PEG_NOT_FOUND)
            {
                 instance.getProperty(propIndex).getValue().get(acceptLangs);
            }
            String contentLangs = String::EMPTY;
            propIndex = instance.findProperty(PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);
            if (propIndex != PEG_NOT_FOUND)
            {
                 instance.getProperty(propIndex).getValue().get(contentLangs);
            }
// l10n end

            //
            //  Look up the subscription in the active subscriptions table
            //  If class list contains only the class name from the current
            //  operation, send a Delete request
            //  Otherwise, send a Modify request
            //
            ActiveSubscriptionsTableEntry tableValue;
            if (_subscriptionTable->getSubscriptionEntry
                (formerSubscriptions [i].getPath (), tableValue))
            {
                Uint32 providerIndex = _subscriptionTable->providerInList
                    (provider, tableValue);
                if (providerIndex != PEG_NOT_FOUND)
                {
                    CIMNamespaceName sourceNameSpace;
                    Array <CIMName> indicationSubclasses;
                    _getCreateParams
                        (formerSubscriptions [i].getPath ().getNameSpace (),
                        formerSubscriptions [i], indicationSubclasses,
                        requiredProperties, sourceNameSpace, condition,
                        query, queryLanguage);

                    //
                    //  If class list contains only the class name from the
                    //  current delete, send a Delete request
                    //
                    if ((tableValue.providers [providerIndex].classList.size ()
                            == 1) &&
                        (tableValue.providers [providerIndex].classList
                            [0].equal (className)))
                    {
// l10n
                        _sendDeleteRequests (indicationProviders,
                            sourceNameSpace,
                            formerSubscriptions [i],
                            AcceptLanguages(acceptLangs),
                            ContentLanguages(contentLangs),
                            request,
                            indicationSubclasses,
                            creator);

                        //
                        //  Response is sent from 
                        //  _handleDeleteResponseAggregation or
                        //  _handleDisableResponseAggregation
                        //
                        responseSent = true;
                    }

                    //
                    //  Otherwise, send a Modify request
                    //
                    else
                    {
                        Uint32 classIndex = _subscriptionTable->classInList
                            (className, tableValue.providers [providerIndex]);
                        if (classIndex != PEG_NOT_FOUND)
                        {
                        //
                        //  Send Modify requests
                        //
// l10n
                        _sendModifyRequests (indicationProviders,
                            sourceNameSpace,
                            requiredProperties, condition,
                            query, queryLanguage,
                            formerSubscriptions [i],
                            AcceptLanguages(acceptLangs),
                            ContentLanguages(contentLangs),
                            request,
                            creator);

                            //
                            //  Response is sent from 
                            //  _handleModifyResponseAggregation
                            //
                            responseSent = true;
                        }
                        else
                        {
                            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                                Tracer::LEVEL2,
                                "Class " + className.getString() +
                                " not found in tableValue.providers");
                        }
                    }
                }
                else
                {
                    //
                    //  The subscription was not served by the provider
                    //
                }
            }
            else
            {
                //
                //  Subscription not found in Active Subscriptions table
                //
            }
        }

#if 0
        //
        //  Create NoProviderAlertIndication instance
        //  ATTN: NoProviderAlertIndication must be defined
        //
        CIMInstance indicationInstance = _createAlertInstance
            (_CLASS_NO_PROVIDER_ALERT, formerSubscriptions);

        //
        //  Send NoProviderAlertIndication to each unique handler instance
        //
        Tracer::trace (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
            "Sending NoProvider Alert for %d subscriptions",
            formerSubscriptions.size ());
        _sendAlerts (formerSubscriptions, indicationInstance);
#endif
        //
        //  NOTE: When a provider that was previously serving a subscription
        //  no longer serves the subscription due to a provider registration
        //  change, a log message is sent, even if there are still other
        //  providers serving the subscription
        //

        //
        //  Log a message for each subscription
        //
        CIMClass providerClass = _subscriptionRepository->getClass
            (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER,
             true, true, false, CIMPropertyList ());
        CIMInstance providerCopy = provider.clone ();
        CIMObjectPath path = providerCopy.buildPath (providerClass);
        providerCopy.setPath (path);
        for (Uint32 j = 0; j < formerSubscriptions.size (); j++)
        {
            //
            //  Get Provider Name, Subscription Filter Name and Handler Name
            //
            String logString1 = getProviderLogString (providerCopy);
            String logString2 = _getSubscriptionLogString
                (formerSubscriptions [j]);

            Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER,
                Logger::WARNING, _MSG_PROVIDER_NO_LONGER_SERVING_KEY,
                _MSG_PROVIDER_NO_LONGER_SERVING, logString1, logString2);
        }
    }

    //
    //  Send response, if not sent from callback 
    //  (for example, if there were no matching subscriptions)
    //
    if (!responseSent)
    {
        CIMNotifyProviderRegistrationResponseMessage * response =
            new CIMNotifyProviderRegistrationResponseMessage
                (request->messageId,
                cimException,
                request->queueIds.copyAndPop ());

        Base::_enqueueResponse (request, response);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleNotifyProviderTerminationRequest
    (const Message * message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleNotifyProviderTermination");

    Array <CIMInstance> providerSubscriptions;
    CIMInstance indicationInstance;

    CIMNotifyProviderTerminationRequestMessage* request = 
        (CIMNotifyProviderTerminationRequestMessage*) message;

    Array <CIMInstance> providers = request->providers;

    CIMClass providerClass = _subscriptionRepository->getClass
        (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER,
         true, true, false, CIMPropertyList ());

    for (Uint32 i = 0; i < providers.size (); i++)
    {
        //
        //  Set path in each instance, so instances may later be easily
        //  compared
        //  ATTN: this code may be removed once bugzilla 1196 has been fixed
        //  (including getClass call above)
        //
        providers [i].setPath (providers [i].buildPath (providerClass));

        //
        //  Get list of affected subscriptions
        //
        //  _subscriptionTable->getProviderSubscriptions also updates the
        //  Active Subscriptions hash table, and implements each subscription's
        //  On Fatal Error policy, if necessary
        //
        providerSubscriptions.clear ();
        providerSubscriptions = _subscriptionTable->getProviderSubscriptions
            (providers [i]);

        if (providerSubscriptions.size () > 0)
        {
            //
            //  NOTE: When a provider that was previously serving a subscription
            //  no longer serves the subscription due to a provider termination,
            //  an alert is always sent, even if there are still other providers
            //  serving the subscription
            //

#if 0
            //
            //  Create ProviderTerminatedAlertIndication instance
            //  ATTN: ProviderTerminatedAlertIndication must be defined
            //
            indicationInstance = _createAlertInstance
                (_CLASS_PROVIDER_TERMINATED_ALERT, providerSubscriptions);

            //
            //  Send ProviderTerminatedAlertIndication to each unique handler
            //  instance
            //
            Tracer::trace (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
                "Sending ProviderDisabled Alert for %d subscriptions",
                providerSubscriptions.size ());
            _sendAlerts (providerSubscriptions, indicationInstance);
#endif
            //
            //  Log a message for each subscription
            //
            CIMClass providerClass = _subscriptionRepository->getClass
                (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER,
                 true, true, false, CIMPropertyList ());
            CIMInstance providerCopy = providers [i].clone ();
            CIMObjectPath path = providerCopy.buildPath (providerClass);
            providerCopy.setPath (path);
            for (Uint32 j = 0; j < providerSubscriptions.size (); j++)
            {
                //
                //  Get Provider Name, Subscription Filter Name and Handler Name
                //
                String logString1 = getProviderLogString (providerCopy);
                String logString2 = _getSubscriptionLogString
                    (providerSubscriptions [j]);

                Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER,
                    Logger::WARNING, _MSG_PROVIDER_NO_LONGER_SERVING_KEY,
                    _MSG_PROVIDER_NO_LONGER_SERVING, logString1, logString2);
            }
        }
    }

    CIMException cimException;

    CIMNotifyProviderTerminationResponseMessage * response =
        new CIMNotifyProviderTerminationResponseMessage 
            (request->messageId,
            cimException,
            request->queueIds.copyAndPop ());

    _enqueueResponse (request, response);

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleNotifyProviderEnableRequest
    (const Message * message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleNotifyProviderEnableRequest");

    CIMNotifyProviderEnableRequestMessage * request =
        (CIMNotifyProviderEnableRequestMessage *) message;
        ProviderIdContainer pidc = request->operationContext.get
            (ProviderIdContainer::NAME);
    CIMInstance providerModule = pidc.getModule();
    CIMInstance provider = pidc.getProvider();
    Array <CIMInstance> capabilities = request->capInstances;

    CIMException cimException;
    Boolean responseSent = false;

    //
    //  Set path in each instance, so instances may later be easily
    //  compared
    //  ATTN: this code may be removed once bugzilla 1196 has been fixed
    //
    CIMClass providerClass = _subscriptionRepository->getClass
        (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER,
         true, true, false, CIMPropertyList ());
    CIMClass providerModuleClass = _subscriptionRepository->getClass
        (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDERMODULE,
         true, true, false, CIMPropertyList ());
    provider.setPath (provider.buildPath (providerClass));
    providerModule.setPath (providerModule.buildPath (providerModuleClass));

    //
    //  Get class name, namespace names, and property list
    //  from each capability instance
    //
    Uint32 numCapabilities = capabilities.size ();
    for (Uint32 i = 0; i < numCapabilities; i++)
    {
        CIMName className;
        Array <CIMNamespaceName> namespaceNames;
        CIMPropertyList propertyList;
        Array <CIMInstance> subscriptions;

        try
        {
            String cName;
            capabilities [i].getProperty (capabilities [i].findProperty
                (_PROPERTY_CLASSNAME)).getValue ().get (cName);
            className = CIMName (cName);

            Array <String> nsNames;
            capabilities [i].getProperty (capabilities [i].findProperty
                (_PROPERTY_NAMESPACES)).getValue ().get (nsNames);
            for (Uint32 j = 0; j < nsNames.size (); j++)
            {
                namespaceNames.append (CIMNamespaceName (nsNames [j]));
            }

            Array <String> pNames;
            Array <CIMName> propertyNames;
            Uint32 propertiesIndex = capabilities [i].findProperty
                (_PROPERTY_SUPPORTEDPROPERTIES);
            if (propertiesIndex != PEG_NOT_FOUND)
            {
                CIMValue propertiesValue = capabilities [i].getProperty
                    (propertiesIndex).getValue ();
                //
                //  If the property list is not null, set the property names
                //
                if (!propertiesValue.isNull ())
                {
                    propertiesValue.get (pNames);
                    for (Uint32 k = 0; k < pNames.size (); k++)
                    {
                        propertyNames.append (CIMName (pNames [k]));
                    }
                    propertyList.set (propertyNames);
                }
            }
        }
        catch (Exception & exception)
        {
            //
            //  Error getting information from Capabilities instance
            //
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
               "Exception caught in handling provider enable notification: " +
                exception.getMessage ());

            cimException = CIMException(CIM_ERR_FAILED, exception.getMessage());
            break;
        }
        catch (...)
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
               "Error in handling provider enable notification");

            cimException = PEGASUS_CIM_EXCEPTION_L(
                CIM_ERR_FAILED,
                MessageLoaderParms(
                    "IndicationService.IndicationService.UNKNOWN_ERROR",
                    "Unknown Error"));
            break;
        }

        //
        //  Get matching subscriptions
        //
        subscriptions = _getMatchingSubscriptions (className, namespaceNames,
            propertyList);

        if (subscriptions.size () > 0)
        {
            //
            //  Construct provider class list from input provider and class name
            //
            Array <ProviderClassList> indicationProviders;
            ProviderClassList indicationProvider;
            indicationProvider.provider = provider;
            indicationProvider.providerModule = providerModule;
            indicationProvider.classList.append (className);
            indicationProviders.append (indicationProvider);

            CIMPropertyList requiredProperties;
            String condition;
            String query;
            String queryLanguage;

            for (Uint32 s = 0; s < subscriptions.size (); s++)
            {
                CIMNamespaceName sourceNameSpace;
                Array <CIMName> indicationSubclasses;
                CIMInstance instance = subscriptions [s];
                _getCreateParams
                    (instance.getPath ().getNameSpace (), instance,
                     indicationSubclasses, requiredProperties, sourceNameSpace,
                     condition, query, queryLanguage);

                //
                //  NOTE: These Create requests are not associated with a
                //  user request, so there is no associated authType or userName
                //  The Creator from the subscription instance is used for
                //  userName, and authType is not set
                //
                //  NOTE: the subscriptions in the subscriptions list came from
                //  the IndicationService's internal hash tables, and thus
                //  each instance is known to have a valid Creator property
                //
                String creator = instance.getProperty (instance.findProperty
                    (PEGASUS_PROPERTYNAME_INDSUB_CREATOR)).getValue
                    ().toString ();

                String acceptLangs = String::EMPTY;
                Uint32 propIndex = instance.findProperty
                    (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);
                if (propIndex != PEG_NOT_FOUND)
                {
                    instance.getProperty (propIndex).getValue ().get
                        (acceptLangs);
                }
                String contentLangs = String::EMPTY;
                propIndex = instance.findProperty
                    (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);
                if (propIndex != PEG_NOT_FOUND)
                {
                    instance.getProperty (propIndex).getValue ().get
                        (contentLangs);
                }

                //
                //  Send Create requests
                //
                _sendCreateRequests (indicationProviders,
                    sourceNameSpace, requiredProperties, condition,
                    query, queryLanguage, instance,
                    AcceptLanguages (acceptLangs),
                    ContentLanguages (contentLangs),
                    request,
                    indicationSubclasses,
                    creator);

                //
                //  Response is sent from _handleCreateResponseAggregation
                //  or _handleEnableResponseAggregation
                //
                responseSent = true;

                //
                //  NOTE: When a provider that was previously not serving a 
                //  subscription now serves the subscription due to a provider 
                //  being enabled, a log message is sent, even if there 
                //  were previously other providers serving the subscription
                //

                //
                //  Get Provider Name, Subscription Filter Name and Handler Name
                //
                String logString1 = getProviderLogString (provider);
                String logString2 = _getSubscriptionLogString
                    (subscriptions [s]);

                //
                //  Log a message for each subscription
                //
                Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER,
                    Logger::WARNING, _MSG_PROVIDER_NOW_SERVING_KEY,
                    _MSG_PROVIDER_NOW_SERVING, logString1, logString2);
            }
        }
    }

    //
    //  Send response, if not sent from callback
    //  (for example, if there were no matching subscriptions)
    //
    if (!responseSent)
    {
        CIMNotifyProviderEnableResponseMessage * response =
            new CIMNotifyProviderEnableResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop());

        _enqueueResponse(request, response);
    }

    PEG_METHOD_EXIT ();
}

Boolean IndicationService::_canCreate (
    CIMInstance & instance,
    const CIMNamespaceName & nameSpace)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_canCreate");

    // REVIEW: Derived classes of CIM_IndicationSubscription not
    // handled. It is reasonable for a user to derive from this
    // class and add extra properties.

    // REVIEW: how does the provider manager know to forward
    // requests to this service? Is it by class name? If so,
    // shouldn't the provider use an is-a operator on the new
    // class?

    //
    //  Check all required properties exist
    //  For a property that has a default value, if it does not exist or is
    //  null, add or set property with default value
    //  For a property that has a specified set of valid values, validate
    //
    if (instance.getClassName ().equal (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
    {
        //
        //  Filter and Handler are key properties for Subscription
        //  No other properties are required
        //
        _checkRequiredProperty (instance, _PROPERTY_FILTER, CIMTYPE_REFERENCE,
            _MSG_KEY_PROPERTY);
        _checkRequiredProperty (instance, _PROPERTY_HANDLER, CIMTYPE_REFERENCE,
            _MSG_KEY_PROPERTY);

        //
        //  Validate the Filter and Handler reference properties
        //  Ensure Filter and Handler instances can be retrieved from the
        //  repository
        //
        CIMProperty filterProperty = instance.getProperty
            (instance.findProperty (_PROPERTY_FILTER));
        CIMValue filterValue = filterProperty.getValue ();
        CIMObjectPath filterPath;
        filterValue.get (filterPath);
        CIMInstance filterInstance =
            _subscriptionRepository->getInstance (nameSpace, filterPath,
            true, false, false, CIMPropertyList ());

        CIMProperty handlerProperty = instance.getProperty
            (instance.findProperty (_PROPERTY_HANDLER));
        CIMValue handlerValue = handlerProperty.getValue ();
        CIMObjectPath handlerPath;
        handlerValue.get (handlerPath);
        CIMInstance handlerInstance =
            _subscriptionRepository->getInstance (nameSpace, handlerPath,
            true, false, false, CIMPropertyList ());

        //
        //  Set the key bindings in the subscription instance
        //
        Array <CIMKeyBinding> kb;
        kb.append (CIMKeyBinding (_PROPERTY_FILTER, filterValue));
        kb.append (CIMKeyBinding (_PROPERTY_HANDLER, handlerValue));

        CIMObjectPath instanceRef = instance.getPath ();
        instanceRef.setKeyBindings (kb);
        instanceRef.setNameSpace (nameSpace);
        instance.setPath (instanceRef);

        //
        //  Subscription State, Repeat Notification Policy, and On Fatal Error
        //  Policy properties each has a default value, a corresponding
        //  Other___ property, and a set of valid values
        //
        _checkPropertyWithOther (instance, _PROPERTY_STATE,
            _PROPERTY_OTHERSTATE, (Uint16) _STATE_ENABLED,
            (Uint16) _STATE_OTHER, _validStates, _supportedStates);

        _checkPropertyWithOther (instance, _PROPERTY_REPEATNOTIFICATIONPOLICY,
            _PROPERTY_OTHERREPEATNOTIFICATIONPOLICY, (Uint16) _POLICY_NONE,
            (Uint16) _POLICY_OTHER, _validRepeatPolicies,
            _supportedRepeatPolicies);

        _checkPropertyWithOther (instance, _PROPERTY_ONFATALERRORPOLICY,
            _PROPERTY_OTHERONFATALERRORPOLICY, (Uint16) _ERRORPOLICY_IGNORE,
            (Uint16) _ERRORPOLICY_OTHER, _validErrorPolicies,
            _supportedErrorPolicies);

        //
        //  For each remaining property, verify that if the property exists in
        //  the instance it is of the correct type
        //
        _checkProperty (instance, _PROPERTY_FAILURETRIGGERTIMEINTERVAL,
            CIMTYPE_UINT64);
        _checkProperty (instance, _PROPERTY_LASTCHANGE, CIMTYPE_DATETIME);
        _checkProperty (instance, _PROPERTY_DURATION, CIMTYPE_UINT64);
        _checkProperty (instance, _PROPERTY_STARTTIME, CIMTYPE_DATETIME);
        _checkProperty (instance, _PROPERTY_TIMEREMAINING, CIMTYPE_UINT64);
        _checkProperty (instance, _PROPERTY_REPEATNOTIFICATIONINTERVAL,
            CIMTYPE_UINT64);
        _checkProperty (instance, _PROPERTY_REPEATNOTIFICATIONGAP,
            CIMTYPE_UINT64);
        _checkProperty (instance, _PROPERTY_REPEATNOTIFICATIONCOUNT,
            CIMTYPE_UINT16);
    }
    else  // Filter or Handler
    {
        //
        //  Name, CreationClassName, SystemName, and SystemCreationClassName
        //  are key properties for Filter and Handler
        //  CreationClassName and Name must exist
        //  If others do not exist, add and set to default
        //
        _checkRequiredProperty (instance, _PROPERTY_NAME, CIMTYPE_STRING,
            _MSG_KEY_PROPERTY);
        _checkRequiredProperty (instance, _PROPERTY_CREATIONCLASSNAME,
            CIMTYPE_STRING, _MSG_KEY_PROPERTY);

        _checkPropertyWithDefault (instance, _PROPERTY_SYSTEMNAME,
            System::getFullyQualifiedHostName ());

        _checkPropertyWithDefault (instance, _PROPERTY_SYSTEMCREATIONCLASSNAME,
            System::getSystemCreationClassName ());

        if (instance.getClassName ().equal (PEGASUS_CLASSNAME_INDFILTER))
        {
            //
            //  Query and QueryLanguage properties are required for Filter
            //
            _checkRequiredProperty (instance, _PROPERTY_QUERY, CIMTYPE_STRING,
                _MSG_PROPERTY);
            _checkRequiredProperty (instance, _PROPERTY_QUERYLANGUAGE,
                CIMTYPE_STRING, _MSG_PROPERTY);

            //
            //  Validate the query language is supported
            //
            String queryLanguage;
            instance.getProperty (instance.findProperty
                (_PROPERTY_QUERYLANGUAGE)).getValue ().get (queryLanguage);
            if (queryLanguage != "WQL")
            {
                throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                            queryLanguage);
            }

            //
            //  Default value for Source Namespace is the namespace of the
            //  Filter registration
            //
            CIMNamespaceName sourceNameSpace = CIMNamespaceName
                (_checkPropertyWithDefault (instance, _PROPERTY_SOURCENAMESPACE,
                 nameSpace.getString()));

            //
            //  Validate the query and indication class name
            //  An exception is thrown if the query is invalid or the class
            //  is not an indication class
            //
            String filterQuery = instance.getProperty (instance.findProperty
                (_PROPERTY_QUERY)).getValue ().toString ();
            WQLSelectStatement selectStatement =
                _getSelectStatement (filterQuery);
            CIMName indicationClassName = _getIndicationClassName
                (selectStatement, sourceNameSpace);

            //
            //  Validate properties in the WHERE clause
            //  Properties referenced in the WQL WHERE clause may not be array
            //  properties, and must exist in the indication class referenced
            //  in the FROM clause
            //
            CIMClass indicationClass = _subscriptionRepository->getClass
                (sourceNameSpace, indicationClassName,
                false, false, false, CIMPropertyList ());
            Uint16 numWhereProperties =
                selectStatement.getWherePropertyNameCount ();
            for (Uint32 i = 0; i < numWhereProperties; i++)
            {
                CIMName wherePropertyName =
                    selectStatement.getWherePropertyName (i);
                Uint32 propertyPos = indicationClass.findProperty
                    (wherePropertyName);
                if (propertyPos != PEG_NOT_FOUND)
                {
                    //
                    //  Property exists in class
                    //  Verify it is not an array property
                    //
                    CIMProperty classProperty = indicationClass.getProperty
                        (propertyPos);
                    if (classProperty.isArray ())
                    {
                        String exceptionStr = _MSG_ARRAY_NOT_SUPPORTED_IN_WHERE;
                        PEG_METHOD_EXIT ();
                        throw PEGASUS_CIM_EXCEPTION_L
                            (CIM_ERR_NOT_SUPPORTED, MessageLoaderParms
                                (_MSG_ARRAY_NOT_SUPPORTED_IN_WHERE_KEY,
                                exceptionStr,
                                wherePropertyName.getString ()));
                    }
                }
                else
                {
                    //
                    //  Property does not exist in class
                    //
                    String exceptionStr = _MSG_WHERE_PROPERTY_NOT_FOUND;
                    PEG_METHOD_EXIT ();
                    throw PEGASUS_CIM_EXCEPTION_L
                        (CIM_ERR_INVALID_PARAMETER, MessageLoaderParms
                            (_MSG_WHERE_PROPERTY_NOT_FOUND_KEY,
                            exceptionStr,
                            wherePropertyName.getString (),
                            indicationClassName.getString ()));
                }
            }

            //
            //  Validate properties in the SELECT clause
            //  Properties referenced in the WQL SELECT clause must exist in
            //  the indication class referenced in the FROM clause
            //
            Uint16 numSelectProperties =
                selectStatement.getSelectPropertyNameCount ();
            for (Uint32 j = 0; j < numSelectProperties; j++)
            {
                CIMName selectPropertyName =
                    selectStatement.getSelectPropertyName (j);
                Uint32 propertyPos = indicationClass.findProperty
                    (selectPropertyName);
                if (propertyPos == PEG_NOT_FOUND)
                {
                    //
                    //  Property does not exist in class
                    //
                    String exceptionStr = _MSG_SELECT_PROPERTY_NOT_FOUND;
                    PEG_METHOD_EXIT ();
                    throw PEGASUS_CIM_EXCEPTION_L
                        (CIM_ERR_INVALID_PARAMETER, MessageLoaderParms
                            (_MSG_SELECT_PROPERTY_NOT_FOUND_KEY,
                            exceptionStr,
                            selectPropertyName.getString (),
                            indicationClassName.getString ()));
                }
            }
        }

        //
        //  Currently only two direct subclasses of Indication handler
        //  class are supported -- further subclassing is not currently
        //  supported
        //
        else if ((instance.getClassName ().equal
                  (PEGASUS_CLASSNAME_INDHANDLER_CIMXML)) ||
                 (instance.getClassName ().equal
                  (PEGASUS_CLASSNAME_LSTNRDST_CIMXML)) ||
                 (instance.getClassName ().equal
                  (PEGASUS_CLASSNAME_INDHANDLER_SNMP)))
        {
            _checkPropertyWithOther (instance, _PROPERTY_PERSISTENCETYPE,
                _PROPERTY_OTHERPERSISTENCETYPE, (Uint16) _PERSISTENCE_PERMANENT,
                (Uint16) _PERSISTENCE_OTHER, _validPersistenceTypes,
                _supportedPersistenceTypes);

            //
            //  For remaining property, verify that if the property exists in
            //  the instance it is of the correct type
            //
            _checkProperty (instance, _PROPERTY_OWNER, CIMTYPE_STRING);

            if (instance.getClassName ().equal
                (PEGASUS_CLASSNAME_INDHANDLER_CIMXML) ||
                instance.getClassName ().equal
                (PEGASUS_CLASSNAME_LSTNRDST_CIMXML))
            {
                //
                //  Destination property is required for CIMXML
                //  Handler subclass
                //
                _checkRequiredProperty (instance, _PROPERTY_DESTINATION,
                    CIMTYPE_STRING, _MSG_PROPERTY);
            }

            if (instance.getClassName ().equal
                (PEGASUS_CLASSNAME_INDHANDLER_SNMP))
            {
                //
                //  TargetHost property is required for SNMP
                //  Handler subclass
                //
                _checkRequiredProperty (instance, _PROPERTY_TARGETHOST,
                    CIMTYPE_STRING, _MSG_PROPERTY);

                //
                //  TargetHostFormat property is required for SNMP
                //  Handler subclass
                //
                _checkRequiredProperty (instance, _PROPERTY_TARGETHOSTFORMAT,
                    CIMTYPE_UINT16, _MSG_PROPERTY);

                //
                //  SNMPVersion property is required for SNMP Handler
                //
                _checkRequiredProperty (instance, _PROPERTY_SNMPVERSION,
                    CIMTYPE_UINT16, _MSG_PROPERTY);
                //
                //  For each remaining property, verify that if the property
                //  exists in the instance it is of the correct type
                //
                _checkProperty (instance, _PROPERTY_PORTNUMBER, CIMTYPE_UINT32);
                _checkProperty (instance, _PROPERTY_SNMPSECURITYNAME,
                    CIMTYPE_STRING);
                _checkProperty (instance, _PROPERTY_SNMPENGINEID,
                    CIMTYPE_STRING);
            }
        }

        else
        {
            //
            //  A class not currently served by the Indication Service
            //
            PEG_METHOD_EXIT ();

            // l10n

            // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
            // _MSG_CLASS_NOT_SERVED);

            throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_NOT_SUPPORTED,
            MessageLoaderParms(_MSG_CLASS_NOT_SERVED_KEY, _MSG_CLASS_NOT_SERVED));
        }
    }

    PEG_METHOD_EXIT ();
    return true;
}

void IndicationService::_checkRequiredProperty (
    CIMInstance & instance,
    const CIMName & propertyName,
    const CIMType expectedType,
    const String & message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_checkRequiredProperty");

    Boolean missingProperty = false;

    //
    //  Required property must exist in instance
    //
    if (instance.findProperty (propertyName) == PEG_NOT_FOUND)
    {
        missingProperty = true;
    }
    else
    {
        //
        //  Get the property
        //
        CIMProperty theProperty = instance.getProperty
            (instance.findProperty (propertyName));
        CIMValue theValue = theProperty.getValue ();

        //
        //  Required property must have a non-null value
        //
        if (theValue.isNull ())
        {
            missingProperty = true;
        }
        else
        {
            //
            //  Check that the property value is of the correct type
            //
            if ((theValue.getType () != expectedType) || (theValue.isArray ()))
            {
                if (theValue.isArray ())
                {
                    String exceptionStr = _MSG_INVALID_TYPE;
                    exceptionStr.append (_MSG_ARRAY_OF);
                    exceptionStr.append ("$0");
                    exceptionStr.append (_MSG_FOR_PROPERTY);
                    exceptionStr.append ("$1");

                    PEG_METHOD_EXIT ();
                    throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                        MessageLoaderParms
                            (_MSG_INVALID_TYPE_ARRAY_OF_FOR_PROPERTY_KEY,
                            exceptionStr,
                            cimTypeToString (theValue.getType ()),
                            propertyName.getString ()));
                }
                else
                {
                    String exceptionStr = _MSG_INVALID_TYPE;
                    exceptionStr.append ("$0");
                    exceptionStr.append (_MSG_FOR_PROPERTY);
                    exceptionStr.append ("$1");

                    PEG_METHOD_EXIT ();
                    throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                        MessageLoaderParms
                            (_MSG_INVALID_TYPE_FOR_PROPERTY_KEY,
                            exceptionStr,
                            cimTypeToString (theValue.getType ()),
                            propertyName.getString ()));
                }
            }
        }
    }

    if (missingProperty)
    {

        // l10n

        String exceptionStr = _MSG_MISSING_REQUIRED;
        // exceptionStr.append (propertyName.getString());
        exceptionStr.append ("$0");
        exceptionStr.append (message);

        String message_key;
        if (strcmp(message.getCString(), _MSG_KEY_PROPERTY) == 0)
        {
            message_key = _MSG_KEY_PROPERTY_KEY;
        }
        else if (strcmp(message.getCString(), _MSG_PROPERTY) == 0)
        {
            message_key = _MSG_PROPERTY_KEY;
        }
        else
        {
            message_key = String("");
        }

        PEG_METHOD_EXIT ();

        // l10n
        // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
        //     exceptionStr);

        throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
            MessageLoaderParms(message_key,
            exceptionStr, propertyName.getString()));

    }
    PEG_METHOD_EXIT ();
}

void IndicationService::_checkPropertyWithOther (
    CIMInstance & instance,
    const CIMName & propertyName,
    const CIMName & otherPropertyName,
    const Uint16 defaultValue,
    const Uint16 otherValue,
    const Array <Uint16> & validValues,
    const Array <Uint16> & supportedValues)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_checkPropertyWithOther");

    Uint16 result = defaultValue;

    //
    //  If the property doesn't exist, add it with the default value
    //
    if (instance.findProperty (propertyName) == PEG_NOT_FOUND)
    {
        instance.addProperty (CIMProperty (propertyName,
            CIMValue (defaultValue)));
    }
    else
    {
        //
        //  Get the property
        //
        CIMProperty theProperty = instance.getProperty
            (instance.findProperty (propertyName));
        CIMValue theValue = theProperty.getValue ();

        //
        //  Check that the value is of the correct type
        //
        if ((theValue.getType () != CIMTYPE_UINT16) || (theValue.isArray ()))
        {
            //  L10N TODO DONE -- new throw of exception

            String exceptionStr;
            if (theValue.isArray ())
            {
                MessageLoaderParms parms(
                "IndicationService.IndicationService._MSG_INVALID_TYPE_ARRAY_OF_FOR_PROPERTY",
                "Invalid type array of $0 for property $1",
                cimTypeToString(theValue.getType()),
                propertyName.getString());

                exceptionStr.append(MessageLoader::getMessage(parms));
            }
            else
            {
                MessageLoaderParms parms(
                "IndicationService.IndicationService._MSG_INVALID_TYPE_FOR_PROPERTY",
                "Invalid type $0 for property $1",
                cimTypeToString(theValue.getType()),
                propertyName.getString());

                exceptionStr.append(MessageLoader::getMessage(parms));
            }
            PEG_METHOD_EXIT ();
            throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
                exceptionStr);
        }

        //
        //  If the value is null, set to the default value
        //
        if (theValue.isNull ())
        {
            theProperty.setValue (CIMValue (defaultValue));
        }
        else
        {
            theValue.get (result);

            //
            //  Validate the value
            //
            //  Note: Valid values are defined by the CIM Event Schema MOF
            //
            if (!Contains (validValues, result))
            {
                // l10n

                String exceptionStr = _MSG_INVALID_VALUE;
                // exceptionStr.append (theValue.toString ());
                exceptionStr.append ("$0");
                exceptionStr.append (_MSG_FOR_PROPERTY);
                // exceptionStr.append (propertyName.getString());
                exceptionStr.append ("$1");

                PEG_METHOD_EXIT ();

                // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
                //    exceptionStr);

                throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                    MessageLoaderParms(_MSG_INVALID_VALUE_FOR_PROPERTY_KEY,
                    exceptionStr,
                    theValue.toString(), propertyName.getString()));
            }

            //
            //  Check for valid values that are not supported
            //
            //  Note: Supported values are a subset of the valid values
            //  Some valid values, as defined in the MOF, are not currently
            //  supported by the Pegasus IndicationService
            //
            if (!Contains (supportedValues, result))
            {
                String exceptionStr = _MSG_UNSUPPORTED_VALUE;
                exceptionStr.append ("$0");
                exceptionStr.append (_MSG_FOR_PROPERTY);
                exceptionStr.append ("$1");

                PEG_METHOD_EXIT ();

                throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_NOT_SUPPORTED,
                    MessageLoaderParms (_MSG_UNSUPPORTED_VALUE_FOR_PROPERTY_KEY,
                    exceptionStr,
                    theValue.toString (), propertyName.getString ()));
            }
        }

        //
        //  If the value is Other, the Other
        //  property must exist, value must not be NULL and type must be String
        //
        if (result == otherValue)
        {
            if (instance.findProperty (otherPropertyName) == PEG_NOT_FOUND)
            {
                // l10n

                String exceptionStr = _MSG_MISSING_REQUIRED;
                // exceptionStr.append (otherPropertyName.getString());
                exceptionStr.append ("$0");
                exceptionStr.append (_MSG_PROPERTY);

                PEG_METHOD_EXIT ();

                // l10n

                // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
                //     exceptionStr);

                throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                  MessageLoaderParms(_MSG_PROPERTY_KEY,
                  exceptionStr,
                  otherPropertyName.getString()));
            }
            else
            {
                CIMProperty otherProperty = instance.getProperty
                    (instance.findProperty (otherPropertyName));
                CIMValue theOtherValue = otherProperty.getValue ();
                if (theOtherValue.isNull ())
                {
                    // l10n

                    String exceptionStr = _MSG_MISSING_REQUIRED;
                    // exceptionStr.append (otherPropertyName.getString());
                    exceptionStr.append ("$0");
                    exceptionStr.append (_MSG_PROPERTY);

                    PEG_METHOD_EXIT ();

                    // l10n
                    // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
                    //  exceptionStr);

                    throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                        MessageLoaderParms(_MSG_PROPERTY_KEY,
                        exceptionStr,
                        otherPropertyName.getString()));
                }
                else if (theOtherValue.getType () != CIMTYPE_STRING)
                {
                    //
                    //  Property exists and is not null,
                    //  but is not of correct type
                    //
                    String exceptionStr = _MSG_INVALID_TYPE;
                    exceptionStr.append ("$0");
                    exceptionStr.append (_MSG_FOR_PROPERTY);
                    exceptionStr.append ("$1");

                    PEG_METHOD_EXIT ();
                    throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                        MessageLoaderParms
                            (_MSG_INVALID_TYPE_FOR_PROPERTY_KEY,
                            exceptionStr,
                            cimTypeToString (theOtherValue.getType ()),
                            otherPropertyName.getString ()));
                }
            }
        }

        //
        //  If value is not Other, Other property must not exist
        //  or must be NULL
        //
        else if (instance.findProperty (otherPropertyName) != PEG_NOT_FOUND)
        {
            CIMProperty otherProperty = instance.getProperty
                (instance.findProperty (otherPropertyName));
            CIMValue theOtherValue = otherProperty.getValue ();
            if (!theOtherValue.isNull ())
            {
                // l10n

                // String exceptionStr = otherPropertyName.getString();
                String exceptionStr ("$0");
                exceptionStr.append (_MSG_PROPERTY_PRESENT);
                // exceptionStr.append (propertyName.getString());
                exceptionStr.append ("$1");
                exceptionStr.append (_MSG_VALUE_NOT);
                // exceptionStr.append (CIMValue (otherValue).toString ());
                exceptionStr.append ("$2");

                PEG_METHOD_EXIT ();

                // l10n

                // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
                //  exceptionStr);

                throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                    MessageLoaderParms(_MSG_PROPERTY_PRESENT_BUT_VALUE_NOT_KEY,
                    exceptionStr,
                    otherPropertyName.getString(),
                    propertyName.getString(),
                    CIMValue (otherValue).toString ()));
            }
        }
    }

    PEG_METHOD_EXIT ();
}

String IndicationService::_checkPropertyWithDefault (
    CIMInstance & instance,
    const CIMName & propertyName,
    const String & defaultValue)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_checkPropertyWithDefault");

    String result = defaultValue;

    //
    //  If the property doesn't exist, add it with the default value
    //
    if (instance.findProperty (propertyName) == PEG_NOT_FOUND)
    {
        instance.addProperty (CIMProperty (propertyName,
            CIMValue (defaultValue)));
    }
    else
    {
        //
        //  Get the property
        //
        CIMProperty theProperty = instance.getProperty
            (instance.findProperty (propertyName));
        CIMValue theValue = theProperty.getValue ();

        //
        //  If the value is null, set to the default value
        //
        if (theValue.isNull ())
        {
            theProperty.setValue (CIMValue (defaultValue));
        }
        else if (theValue.getType () != CIMTYPE_STRING)
        {
            //
            //  Property exists and is not null,
            //  but is not of correct type
            //
            String exceptionStr = _MSG_INVALID_TYPE;
            exceptionStr.append ("$0");
            exceptionStr.append (_MSG_FOR_PROPERTY);
            exceptionStr.append ("$1");

            PEG_METHOD_EXIT ();
            throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                MessageLoaderParms
                    (_MSG_INVALID_TYPE_FOR_PROPERTY_KEY,
                    exceptionStr,
                    cimTypeToString (theValue.getType ()),
                    propertyName.getString ()));
        }
        else
        {
            theValue.get (result);
        }
    }

    return result;

    PEG_METHOD_EXIT ();
}

void IndicationService::_checkProperty (
    CIMInstance & instance,
    const CIMName & propertyName,
    const CIMType expectedType)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_checkProperty");

    //
    //  If the property exists, get it
    //
    Uint32 propPos = instance.findProperty (propertyName);
    if (propPos != PEG_NOT_FOUND)
    {
        CIMProperty theProperty = instance.getProperty (propPos);
        CIMValue theValue = theProperty.getValue ();

        //
        //  If the value is not null, check the type
        //
        if (!theValue.isNull ())
        {
            if ((theValue.getType () != expectedType) || (theValue.isArray ()))
            {
                //
                //  Property exists and is not null, but is not of correct type
                //
                if (theValue.isArray ())
                {
                    String exceptionStr = _MSG_INVALID_TYPE;
                    exceptionStr.append (_MSG_ARRAY_OF);
                    exceptionStr.append ("$0");
                    exceptionStr.append (_MSG_FOR_PROPERTY);
                    exceptionStr.append ("$1");

                    PEG_METHOD_EXIT ();
                    throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                        MessageLoaderParms
                            (_MSG_INVALID_TYPE_ARRAY_OF_FOR_PROPERTY_KEY,
                            exceptionStr,
                            cimTypeToString (theValue.getType ()),
                            propertyName.getString ()));
                }
                else
                {
                    String exceptionStr = _MSG_INVALID_TYPE;
                    exceptionStr.append ("$0");
                    exceptionStr.append (_MSG_FOR_PROPERTY);
                    exceptionStr.append ("$1");

                    PEG_METHOD_EXIT ();
                    throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
                        MessageLoaderParms
                            (_MSG_INVALID_TYPE_FOR_PROPERTY_KEY,
                            exceptionStr,
                            cimTypeToString (theValue.getType ()),
                            propertyName.getString ()));
                }
            }
        }
    }

    PEG_METHOD_EXIT ();
}

Boolean IndicationService::_canModify (
    const CIMModifyInstanceRequestMessage * request,
    const CIMObjectPath & instanceReference,
    const CIMInstance & instance,
    CIMInstance & modifiedInstance)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_canModify");

    //
    //  Currently, only modification allowed is of Subscription State
    //  property in Subscription class
    //
    if (!instanceReference.getClassName ().equal
        (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
    {
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
    }

    if (request->includeQualifiers)
    {
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
    }

    //
    //  Request is invalid if property list is null, meaning all properties
    //  are to be updated
    //
    if (request->propertyList.isNull ())
    {
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
    }

    //
    //  Request is invalid if more than one property is specified
    //
    else if (request->propertyList.size () > 1)
    {
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
    }

    //
    //  For request to be valid, zero or one property must be specified
    //  If one property specified, it must be Subscription State property
    //
    else if ((request->propertyList.size () == 1) &&
             (!request->propertyList[0].equal (_PROPERTY_STATE)))
    {
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
    }

    //
    //  Check the SubscriptionState property in the modified instance
    //
    _checkPropertyWithOther (modifiedInstance, _PROPERTY_STATE,
        _PROPERTY_OTHERSTATE, (Uint16) _STATE_ENABLED, (Uint16) _STATE_OTHER,
        _validStates, _supportedStates);

    //
    //  Get creator from instance
    //
    String creator;
    if (!_getCreator (instance, creator))
    {
        //
        //  This instance from the repository is corrupted
        //  L10N TODO DONE -- new throw of exception
        //
        PEG_METHOD_EXIT ();
        //throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED,
            //_MSG_INVALID_INSTANCES);
        MessageLoaderParms parms(_MSG_INVALID_INSTANCES_KEY, 
            _MSG_INVALID_INSTANCES);
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
    }

    //
    //  Current user must be privileged user or instance Creator to modify
    //  NOTE: if authentication was not turned on when instance was created,
    //  instance creator will be String::EMPTY
    //  If creator is String::EMPTY, anyone may modify or delete the
    //  instance
    //
    String currentUser = ((IdentityContainer)request->operationContext.get
        (IdentityContainer :: NAME)).getUserName();
    if ((creator != String::EMPTY) &&
        (!System::isPrivilegedUser (currentUser)) &&
        (currentUser != creator))
    {
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, String::EMPTY);
    }

    return true;
    PEG_METHOD_EXIT ();
}

Boolean IndicationService::_canDelete (
    const CIMObjectPath & instanceReference,
    const CIMNamespaceName & nameSpace,
    const String & currentUser)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_canDelete");

    CIMName superClass;
    CIMName propName;

    //
    //  Get the instance to be deleted from the respository
    //
    CIMInstance instance;

    instance = _subscriptionRepository->getInstance
        (nameSpace, instanceReference);

    //
    //  Get creator from instance
    //
    String creator;
    if (!_getCreator (instance, creator))
    {
        //
        //  This instance from the repository is corrupted
        //  Allow the delete if a Privileged User
        //      (or authentication turned off),
        //  Otherwise disallow as access denied
        //
        if ((!System::isPrivilegedUser (currentUser)) &&
            (currentUser != String::EMPTY))
        {
            PEG_METHOD_EXIT ();
            throw PEGASUS_CIM_EXCEPTION (CIM_ERR_ACCESS_DENIED, String::EMPTY);
        }
    }

    //
    //  Current user must be privileged user or instance Creator to delete
    //  NOTE: if authentication was not turned on when instance was created,
    //  instance creator will be String::EMPTY
    //  If creator is String::EMPTY, anyone may modify or delete the
    //  instance
    //
    if ((creator != String::EMPTY) &&
        (!System::isPrivilegedUser (currentUser)) &&
        (currentUser != creator))
    {
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, String::EMPTY);
    }

    //
    //  Get the class and superclass of the instance to be deleted
    //
    CIMClass refClass;

    refClass = _subscriptionRepository->getClass (nameSpace,
        instanceReference.getClassName (), true, true, false,
        CIMPropertyList ());
    superClass = refClass.getSuperClassName();

    //
    //  If the class or superclass is Filter or Handler, check for
    //  subscription instances referring to the instance to be deleted
    //
    if ((superClass.equal (PEGASUS_CLASSNAME_INDFILTER)) ||
        (superClass.equal (PEGASUS_CLASSNAME_INDHANDLER)) ||
        (instanceReference.getClassName().equal (PEGASUS_CLASSNAME_INDFILTER))
     || (instanceReference.getClassName().equal (PEGASUS_CLASSNAME_INDHANDLER)))
    {
        if ((superClass.equal (PEGASUS_CLASSNAME_INDFILTER)) ||
            (instanceReference.getClassName().equal
            (PEGASUS_CLASSNAME_INDFILTER)))
        {
            propName = _PROPERTY_FILTER;
        }
        else if (superClass.equal (PEGASUS_CLASSNAME_INDHANDLER))
        {
            propName = _PROPERTY_HANDLER;

            //
            //  If deleting transient handler, first delete any referencing
            //  subscriptions
            //
            if (_subscriptionRepository->isTransient (nameSpace,
                instanceReference))
            {
                _deleteReferencingSubscriptions (nameSpace, propName,
                    instanceReference);
                PEG_METHOD_EXIT ();
                return true;
            }
        }

        //
        //  Get all the subscriptions in the same namespace from the respository
        //
        Array <CIMInstance> subscriptions =
            _subscriptionRepository->getSubscriptions (nameSpace);

        CIMValue propValue;

        //
        //  Check each subscription for a reference to the instance to be
        //  deleted
        //
        for (Uint32 i = 0; i < subscriptions.size(); i++)
        {
            //
            //  Get the subscription Filter or Handler property value
            //
            propValue = subscriptions[i].getProperty
                (subscriptions[i].findProperty
                (propName)).getValue();

            CIMObjectPath ref;
            propValue.get (ref);

            //
            //  If the current subscription Filter or Handler is the instance
            //  to be deleted, it may not be deleted
            //
            // ATTN: Can namespaces in the references cause comparison failure?
            //
            if (instanceReference == ref)
            {
                PEG_METHOD_EXIT ();

                // l10n

                // String exceptionStr = _MSG_REFERENCED;
                // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);

                throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED,
                    MessageLoaderParms(_MSG_REFERENCED_KEY, _MSG_REFERENCED));
            }
        }
    }

    PEG_METHOD_EXIT ();
    return true;
}

Array <CIMInstance> IndicationService::_getMatchingSubscriptions (
    const CIMName & supportedClass,
    const Array <CIMNamespaceName> nameSpaces,
    const CIMPropertyList & supportedProperties,
    const Boolean checkProvider,
    const CIMInstance & provider)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getMatchingSubscriptions");

    Array <CIMInstance> matchingSubscriptions;
    Array <CIMInstance> subscriptions;

    subscriptions = _subscriptionTable->getMatchingSubscriptions
        (supportedClass, nameSpaces, checkProvider, provider);

    for (Uint32 i = 0; i < subscriptions.size (); i++)
    {
        Boolean match = true;

        //
        //  If supported properties is null (all properties)
        //  the subscription can be supported
        //
        if (!supportedProperties.isNull ())
        {
            String filterQuery;
            WQLSelectStatement selectStatement;
            CIMName indicationClassName;
            CIMNamespaceName sourceNameSpace;
            CIMPropertyList propertyList;

            //
            //  Get filter properties
            //
            _subscriptionRepository->getFilterProperties
                (subscriptions [i],
                subscriptions [i].getPath ().getNameSpace (),
                filterQuery, sourceNameSpace);
            selectStatement = _getSelectStatement (filterQuery);

            //
            //  Get indication class name from filter query
            //
            indicationClassName = _getIndicationClassName
                (selectStatement, sourceNameSpace);

            //
            //  Get property list from filter query (FROM and
            //  WHERE clauses)
            //
            propertyList = _getPropertyList (selectStatement,
                sourceNameSpace, indicationClassName);

            //
            //  If the subscription requires all properties,
            //  but supported property list does not include all
            //  properties, the subscription cannot be supported
            //
            if (propertyList.isNull ())
            {
                match = false;
                break;
            }
            else
            {
                //
                //  Compare subscription property list
                //  with supported property list
                //
                for (Uint32 j = 0;
                     j < propertyList.size () && match;
                     j++)
                {
                    if (!ContainsCIMName
                        (supportedProperties.getPropertyNameArray(),
                        propertyList [j]))
                    {
                        match = false;
                        break;
                    }
                }
            }
        }

        if (match)
        {
            //
            //  Add current subscription to list
            //
            matchingSubscriptions.append (subscriptions [i]);
        }
    }

    PEG_METHOD_EXIT ();
    return matchingSubscriptions;
}

void IndicationService::_getModifiedSubscriptions (
    const CIMName & supportedClass,
    const Array <CIMNamespaceName> & newNameSpaces,
    const Array <CIMNamespaceName> & oldNameSpaces,
    const CIMPropertyList & newProperties,
    const CIMPropertyList & oldProperties,
    Array <CIMInstance> & newSubscriptions,
    Array <CIMInstance> & formerSubscriptions)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getModifiedSubscriptions");

    Array <CIMInstance> newList;
    Array <CIMInstance> formerList;
    Array <CIMInstance> bothList;

    newSubscriptions.clear ();
    formerSubscriptions.clear ();

    //
    //  For each newly supported namespace, lookup to retrieve list of
    //  subscriptions for the indication class-source namespace pair
    //
    newList = _subscriptionTable->getMatchingSubscriptions
        (supportedClass, newNameSpaces);

    //
    //  For each formerly supported namespace, lookup to retrieve list of
    //  subscriptions for the indication class-source namespace pair
    //
    formerList = _subscriptionTable->getMatchingSubscriptions
        (supportedClass, oldNameSpaces);

    //
    //  Find subscriptions that appear in both lists, and move them to a third
    //  list
    //
    Sint8 found;
    for (Uint32 p = 0; p < newList.size (); p++)
    {
        found = -1;
        for (Uint32 q = 0; q < formerList.size (); q++)
        {
            if (newList [p].identical (formerList [q]))
            {
                found = q;
                bothList.append (newList [p]);
                break;
            }
        }
        if (found >= 0)
        {
            newList.remove (p);
            p--;
            formerList.remove (found);
        }
    }

    //
    //  For indicationClassName-sourceNamespace pair that is now supported, but
    //  previously was not, add to list of newly supported subscriptions if
    //  required properties are now supported
    //
    for (Uint32 n = 0; n < newList.size (); n++)
    {
        String filterQuery;
        WQLSelectStatement selectStatement;
        CIMName indicationClassName;
        CIMNamespaceName sourceNameSpace;
        CIMPropertyList requiredProperties;

        //
        //  Get filter properties
        //
        _subscriptionRepository->getFilterProperties (newList [n],
            newList [n].getPath ().getNameSpace (), filterQuery,
            sourceNameSpace);
        selectStatement = _getSelectStatement (filterQuery);

        //
        //  Get indication class name from filter query (FROM clause)
        //
        indicationClassName = _getIndicationClassName (selectStatement,
            sourceNameSpace);

        //
        //  Get property list from filter query (FROM and WHERE
        //  clauses)
        //
        requiredProperties = _getPropertyList (selectStatement,
            sourceNameSpace, indicationClassName);

        //
        //  Check if required properties are now supported
        //
        if (_inPropertyList (requiredProperties, newProperties))
        {
            newSubscriptions.append (newList [n]);
        }
    }

    //
    //  For indicationClassName-sourceNamespace pair that was previously
    //  supported, but now is not, add to list of formerly supported
    //  subscriptions
    //
    for (Uint32 f = 0; f < formerList.size (); f++)
    {
        formerSubscriptions.append (formerList [f]);
    }

    //
    //  For indicationClassName-sourceNamespace pair that is now supported,
    //  and was also previously supported, add to appropriate list, based on
    //  required properties
    //
    for (Uint32 b = 0; b < bothList.size (); b++)
    {
        String filterQuery;
        WQLSelectStatement selectStatement;
        CIMName indicationClassName;
        CIMNamespaceName sourceNameSpace;
        CIMPropertyList requiredProperties;
        Boolean newMatch = false;
        Boolean formerMatch = false;

        //
        //  Get filter properties
        //
        _subscriptionRepository->getFilterProperties (bothList [b],
            bothList [b].getPath ().getNameSpace (), filterQuery,
            sourceNameSpace);
        selectStatement = _getSelectStatement (filterQuery);

        //
        //  Get indication class name from filter query (FROM clause)
        //
        indicationClassName = _getIndicationClassName (selectStatement,
            sourceNameSpace);

        //
        //  Get property list from filter query (FROM and WHERE
        //  clauses)
        //
        requiredProperties = _getPropertyList (selectStatement,
            sourceNameSpace, indicationClassName);

        //
        //  Check required properties
        //
        newMatch = _inPropertyList (requiredProperties,
            newProperties);
        formerMatch = _inPropertyList (requiredProperties,
            oldProperties);

        //
        //  Add current subscription to appropriate list
        //
        if (newMatch && !formerMatch)
        {
            newSubscriptions.append (bothList [b]);
        }
        else if (!newMatch && formerMatch)
        {
            formerSubscriptions.append (bothList [b]);
        }
    }

    PEG_METHOD_EXIT ();
}

Boolean IndicationService::_inPropertyList (
    const CIMPropertyList & requiredProperties,
    const CIMPropertyList & supportedProperties)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_inPropertyList");

    //
    //  If property list is null (all properties)
    //  all the required properties are supported
    //
    if (supportedProperties.isNull ())
    {
        return true;
    }
    else
    {
        //
        //  If the subscription requires all properties,
        //  but property list does not include all
        //  properties, the required properties cannot be supported
        //
        if (requiredProperties.isNull ())
        {
            return false;
        }
        else
        {
            //
            //  Compare required property list
            //  with property list
            //
            for (Uint32 i = 0; i < requiredProperties.size (); i++)
            {
                if (!ContainsCIMName
                    (supportedProperties.getPropertyNameArray (),
                    requiredProperties[i]))
                {
                    return false;
                }
            }
        }
    }

    PEG_METHOD_EXIT ();
    return true;
}

WQLSelectStatement IndicationService::_getSelectStatement (
    const String & filterQuery) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getSelectStatement");

    WQLSelectStatement selectStatement;

    try
    {
        selectStatement.clear ();

        _mutex.lock(pegasus_thread_self());

        WQLParser::parse (filterQuery, selectStatement);

        _mutex.unlock();
    }
    catch (ParseError & pe)
    {
        String exceptionStr = pe.getMessage ();
        _mutex.unlock();
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, exceptionStr);
    }
    catch (MissingNullTerminator & mnt)
    {
        String exceptionStr = mnt.getMessage ();
        _mutex.unlock();
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, exceptionStr);
    }

    PEG_METHOD_EXIT ();
    return selectStatement;
}

CIMName IndicationService::_getIndicationClassName (
    const WQLSelectStatement & selectStatement,
    const CIMNamespaceName & nameSpaceName) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getIndicationClassName");

    CIMName indicationClassName;
    Array <CIMName> indicationSubclasses;

    indicationClassName = selectStatement.getClassName ();

    //
    //  Validate that class is an Indication class
    //  The Indication Qualifier should exist and have the value True
    //
    Boolean validClass = _subscriptionRepository->validateIndicationClassName
        (indicationClassName, nameSpaceName);

    if (!validClass)
    {
        // l10n
        // String exceptionStr = _MSG_INVALID_CLASSNAME;
        String exceptionStr = String ("Invalid indication class name ");
        // exceptionStr.append (indicationClassName.getString());
        exceptionStr.append ("$0");
        // exceptionStr.append (_MSG_IN_FROM);
        exceptionStr.append (" in FROM clause of ");
        // exceptionStr.append (PEGASUS_CLASSNAME_INDFILTER.getString());
        exceptionStr.append ("$1");
        exceptionStr.append (" ");
        // exceptionStr.append (_PROPERTY_QUERY.getString());
        exceptionStr.append ("$2");
        exceptionStr.append (_MSG_PROPERTY);

        PEG_METHOD_EXIT ();

        // l10n

        // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
        //  exceptionStr);

        throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_INVALID_PARAMETER,
            MessageLoaderParms (
            _MSG_INVALID_CLASSNAME_IN_FROM_PROPERTY_KEY, exceptionStr,
            indicationClassName.getString (),
            PEGASUS_CLASSNAME_INDFILTER.getString (),
            _PROPERTY_QUERY.getString ()));
    }

    PEG_METHOD_EXIT ();
    return indicationClassName;
}

Array <ProviderClassList> IndicationService::_getIndicationProviders (
    const CIMNamespaceName & nameSpace,
    const CIMName & indicationClassName,
    const Array <CIMName> & indicationSubclasses,
    const CIMPropertyList & requiredPropertyList) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getIndicationProviders");

    ProviderClassList provider;
    Array <ProviderClassList> indicationProviders;
    Array <CIMInstance> providerInstances;
    Array <CIMInstance> providerModuleInstances;
    Boolean duplicate = false;

    //
    //  For each indication subclass, get providers
    //
    CIMClass providerClass = _subscriptionRepository->getClass
        (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER,
         true, true, false, CIMPropertyList ());
    CIMClass providerModuleClass = _subscriptionRepository->getClass
        (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDERMODULE,
         true, true, false, CIMPropertyList ());
    for (Uint32 i = 0; i < indicationSubclasses.size (); i++)
    {
        //
        //  Get providers that can serve the subscription
        //
        providerInstances.clear ();
        providerModuleInstances.clear ();
        if (_providerRegManager->getIndicationProviders
                (nameSpace,
                 indicationSubclasses [i],
                 requiredPropertyList,
                 providerInstances,
                 providerModuleInstances))
        {
            PEGASUS_ASSERT (providerInstances.size () ==
                            providerModuleInstances.size ());

            duplicate = false;
            
            //
            //  Merge into list of ProviderClassList structs
            //
            for (Uint32 j = 0; j < providerInstances.size () && !duplicate; j++)
            {
                provider.classList.clear ();
                duplicate = false;

                //
                //  Set path in each instance, so instances may later be easily
                //  compared
                //  ATTN: this code may be removed once bugzilla 1196 has been
                //  fixed
                //  (including getClass calls above)
                //
                providerInstances [j].setPath
                    (providerInstances [j].buildPath (providerClass));
                providerModuleInstances [j].setPath
                    (providerModuleInstances [j].buildPath
                    (providerModuleClass));

                //
                //  See if indication provider is already in list
                //
                for (Uint32 k = 0;
                     k < indicationProviders.size () && !duplicate; k++)
                {
                    if ((providerInstances [j].getPath ().identical
                        (indicationProviders [k].provider.getPath ())) &&
                        (providerModuleInstances [j].getPath ().identical
                        (indicationProviders [k].providerModule.getPath ())))
                    {
                        //
                        //  Indication provider is already in list
                        //  Add subclass to provider's class list
                        //
                        indicationProviders [k].classList.append
                            (indicationSubclasses [i]);
                        duplicate = true;
                    }
                }

                if (!duplicate)
                {
                    //
                    //  Current provider is not yet in list
                    //  Create new list entry
                    //
                    provider.provider = providerInstances [j];
                    provider.providerModule = providerModuleInstances [j];
                    provider.classList.append (indicationSubclasses [i]);
                    indicationProviders.append (provider);
                }
            }  // for each indication provider instance
        }  // if any providers
    }  // for each indication subclass

    PEG_METHOD_EXIT ();
    return indicationProviders;
}

CIMPropertyList IndicationService::_getPropertyList
    (const WQLSelectStatement & selectStatement,
     const CIMNamespaceName & nameSpaceName,
     const CIMName & indicationClassName) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getPropertyList");

    CIMPropertyList propertyList;

    //
    //  Get all the properties referenced in the projection list (SELECT clause)
    //
    propertyList = selectStatement.getSelectPropertyList ();
    if (propertyList.isNull ())
    {
        //
        //  Return null property list for all properties
        //
        return propertyList;
    }
    else
    {
        Array <CIMName> propertyArray;

        //
        //  Get selected property names
        //
        propertyArray = propertyList.getPropertyNameArray ();

        //
        //  Get all the properties referenced in the condition (WHERE clause)
        //
        if (selectStatement.hasWhereClause ())
        {
            CIMPropertyList
            wherePropertyList = selectStatement.getWherePropertyList ();

            //
            //  WHERE property list may not be NULL (may be empty)
            //
            for (Uint32 j = 0; j < wherePropertyList.size(); j++)
            {
                //
                //  Add property name to the list if not already there
                //
                if (!ContainsCIMName (propertyArray, wherePropertyList[j]))
                {
                    propertyArray.append (wherePropertyList[j]);
                }
            }
        }

        return _checkPropertyList (propertyArray, nameSpaceName,
            indicationClassName);
    }
}

CIMPropertyList IndicationService::_checkPropertyList
    (const Array <CIMName> & propertyList,
     const CIMNamespaceName & nameSpaceName,
     const CIMName & indicationClassName) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_checkPropertyList");

    //
    //  Check if list includes all properties in class
    //  If so, must be set to NULL
    //
    CIMClass indicationClass;

    //
    //  Get the indication class object from the repository
    //  Specify localOnly=false because superclass properties are needed
    //  Specify includeQualifiers=false because qualifiers are not needed
    //
    indicationClass = _subscriptionRepository->getClass
        (nameSpaceName, indicationClassName, false, false, false,
         CIMPropertyList ());

    Boolean allProperties = true;
    for (Uint32 i = 0;
         i < indicationClass.getPropertyCount () && allProperties; i++)
    {
        if (!ContainsCIMName (propertyList,
            indicationClass.getProperty (i).getName ()))
        {
            allProperties = false;
        }
    }

    if (allProperties)
    {
        //
        //  Return NULL CIMPropertyList
        //
        PEG_METHOD_EXIT ();
        return CIMPropertyList ();
    }
    else
    {
        PEG_METHOD_EXIT ();
        return CIMPropertyList (propertyList);
    }
}

String IndicationService::_getCondition
    (const String & filterQuery) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getCondition");

    String condition = String::EMPTY;

    //
    //  Get condition substring from filter query
    //
    if (filterQuery.find (_QUERY_WHERE) != PEG_NOT_FOUND)
    {
        condition = filterQuery.subString (filterQuery.find
            (_QUERY_WHERE) + 6);
    }

    PEG_METHOD_EXIT ();
    return condition;
}

void IndicationService::_deleteReferencingSubscriptions (
    const CIMNamespaceName & nameSpace,
    const CIMName & referenceProperty,
    const CIMObjectPath & handler)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_deleteReferencingSubscriptions");

    Array <CIMInstance> deletedSubscriptions;

    //
    //  Delete referencing subscriptions from the repository
    //
    deletedSubscriptions =
        _subscriptionRepository->deleteReferencingSubscriptions
        (nameSpace, referenceProperty, handler);

    //
    //  Send delete request to each provider for each deleted subscription
    //
    for (Uint32 i = 0; i < deletedSubscriptions.size (); i++)
    {
        Array <ProviderClassList> indicationProviders;
        Array <CIMName> indicationSubclasses;
        CIMNamespaceName sourceNamespaceName;

        CIMObjectPath path = deletedSubscriptions [i].getPath ();
        path.setNameSpace (nameSpace);
        deletedSubscriptions [i].setPath (path);

        indicationProviders = _getDeleteParams (nameSpace,
            deletedSubscriptions [i], indicationSubclasses,
            sourceNamespaceName);

        //
        //  Send Delete requests
        //
        //  NOTE: These Delete requests are not associated with a user
        //  request, so there is no associated authType or userName
        //  The Creator from the subscription instance is used for userName,
        //  and authType is not set
        //
        CIMInstance instance = deletedSubscriptions [i];
        String creator = String::EMPTY;
        _getCreator (instance, creator);

// l10n start
        String acceptLangs = String::EMPTY;
        Uint32 propIndex = instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);
        if (propIndex != PEG_NOT_FOUND)
        {
            instance.getProperty (propIndex).getValue ().get (acceptLangs);
        }
        String contentLangs = String::EMPTY;
        propIndex = instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);
        if (propIndex != PEG_NOT_FOUND)
        {
            instance.getProperty (propIndex).getValue ().get (contentLangs);
        }
// l10n end

        CIMObjectPath instanceName =
            deletedSubscriptions [i].getPath ();
        instanceName.setNameSpace (nameSpace);
        deletedSubscriptions [i].setPath (instanceName);
// l10n
        _sendDeleteRequests (indicationProviders, sourceNamespaceName,
            deletedSubscriptions [i],
            AcceptLanguages (acceptLangs),
            ContentLanguages (contentLangs),
            0,  // no request
            indicationSubclasses,
            creator);
    }

    PEG_METHOD_EXIT ();
}

Boolean IndicationService::_isExpired (
    const CIMInstance & instance) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_isExpired");

    Boolean isExpired = true;
    Uint64 timeRemaining = 0;

    //
    //  Get time remaining, if subscription has a duration
    //
    if (_getTimeRemaining (instance, timeRemaining))
    {
        if (timeRemaining > 0)
        {
            isExpired = false;
        }
    }
    else
    {
        //
        //  If there is no duration, the subscription has no expiration date
        //
        isExpired = false;
    }

    PEG_METHOD_EXIT ();
    return isExpired;
}

void IndicationService::_deleteExpiredSubscription (
    CIMObjectPath & subscription)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_deleteExpiredSubscription");

    CIMInstance subscriptionInstance;

    //
    //  Delete instance from repository
    //
    subscriptionInstance = _subscriptionRepository->deleteSubscription
        (subscription);

    //
    //  If a valid instance object was returned, the subscription was
    //  successfully deleted
    //
    if (!subscriptionInstance.isUninitialized ())
    {
        //
        //  If subscription was active, send delete requests to providers
        //  and update hash tables
        //
        Uint16 subscriptionState;
        CIMValue subscriptionStateValue;
        subscriptionStateValue = subscriptionInstance.getProperty
            (subscriptionInstance.findProperty
            (_PROPERTY_STATE)).getValue ();
        subscriptionStateValue.get (subscriptionState);

        if ((subscriptionState == _STATE_ENABLED) ||
            (subscriptionState == _STATE_ENABLEDDEGRADED))
        {
            Array <ProviderClassList> indicationProviders;
            Array <CIMName> indicationSubclasses;
            CIMNamespaceName sourceNamespaceName;

            subscriptionInstance.setPath (subscription);

            indicationProviders = _getDeleteParams
                (subscription.getNameSpace (), subscriptionInstance,
                indicationSubclasses, sourceNamespaceName);

            //
            //  Send Delete requests
            //
            //  NOTE: These Delete requests are not associated with a user
            //  request, so there is no associated authType or userName
            //  The Creator from the subscription instance is used for userName,
            //  and authType is not set
            //
            String creator;
            _getCreator (subscriptionInstance, creator);

            //
            // Get the language tags that were saved with the subscription
            // instance
            //
            String acceptLangs = String::EMPTY;
            Uint32 propIndex = subscriptionInstance.findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);
            if (propIndex != PEG_NOT_FOUND)
            {
                 subscriptionInstance.getProperty (propIndex).getValue ().get
                     (acceptLangs);
            }
            String contentLangs = String::EMPTY;
            propIndex = subscriptionInstance.findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);
            if (propIndex != PEG_NOT_FOUND)
            {
                 subscriptionInstance.getProperty (propIndex).getValue ().get
                     (contentLangs);
            }

// l10n
            subscriptionInstance.setPath (subscription);
            _sendDeleteRequests (indicationProviders,
                sourceNamespaceName, subscriptionInstance,
                AcceptLanguages(acceptLangs),
                ContentLanguages(contentLangs),
                0, // no request
                indicationSubclasses,
                creator);
        }
    }
    else
    {
        //
        //  The subscription may have already been deleted by another thread
        //
    }

    PEG_METHOD_EXIT ();
}

Boolean IndicationService::_getTimeRemaining (
    const CIMInstance & instance,
    Uint64 & timeRemaining) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getTimeRemaining");

    Boolean hasDuration = true;
    timeRemaining = 0;

    //
    //  Calculate time remaining from subscription
    //  start time, subscription duration, and current date time
    //

    //
    //  NOTE: It is assumed that the instance passed to this method is a
    //  subscription instance, and that the Start Time property exists
    //  and has a value
    //

    //
    //  Get Subscription Start Time
    //
    CIMValue startTimeValue;
    CIMDateTime startTime;
    startTimeValue = instance.getProperty
        (instance.findProperty (_PROPERTY_STARTTIME)).getValue ();
    startTimeValue.get (startTime);

    //
    //  Get Subscription Duration
    //
    if (instance.findProperty (_PROPERTY_DURATION) != PEG_NOT_FOUND)
    {
        CIMValue durationValue;
        durationValue = instance.getProperty
            (instance.findProperty (_PROPERTY_DURATION)).getValue ();
        if (durationValue.isNull ())
        {
            hasDuration = false;
        }
        else
        {
            Uint64 duration;
            durationValue.get (duration);

            //
            //  Get current date time, and calculate Subscription Time Remaining
            //
            CIMDateTime currentDateTime = CIMDateTime::getCurrentDateTime ();

            Sint64 difference;
            try
            {
                difference = CIMDateTime::getDifference
                    (startTime, currentDateTime);
            }
            // Check if the date time is out of range.
            catch (DateTimeOutOfRangeException& e)
            {
                PEG_METHOD_EXIT();
                throw e;
            }

            PEGASUS_ASSERT (difference >= 0);
            if (((Sint64) duration - difference) >= 0)
            {
                timeRemaining = (Sint64) duration - difference;
            }
        }
    }
    else
    {
        hasDuration = false;
    }

    PEG_METHOD_EXIT ();
    return hasDuration;
}

void IndicationService::_setTimeRemaining (
    CIMInstance & instance)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_setTimeRemaining");

    Uint64 timeRemaining = 0;
    if (_getTimeRemaining (instance, timeRemaining))
    {
        //
        //  Add or set the value of the property with the calculated value
        //
        if (instance.findProperty (_PROPERTY_TIMEREMAINING) == PEG_NOT_FOUND)
        {
            instance.addProperty (CIMProperty
                (_PROPERTY_TIMEREMAINING, timeRemaining));
        }
        else
        {
            CIMProperty remaining = instance.getProperty
                (instance.findProperty (_PROPERTY_TIMEREMAINING));
            remaining.setValue (CIMValue (timeRemaining));
        }
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_getCreateParams (
    const CIMNamespaceName & nameSpaceName,
    const CIMInstance & subscriptionInstance,
    Array <CIMName> & indicationSubclasses,
    Array <ProviderClassList> & indicationProviders,
    CIMPropertyList & propertyList,
    CIMNamespaceName & sourceNameSpace,
    String & condition,
    String & query,
    String & queryLanguage)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getCreateParams");

    WQLSelectStatement selectStatement;
    CIMName indicationClassName;
    condition = String::EMPTY;
    query = String::EMPTY;
    queryLanguage = String::EMPTY;

    //
    //  Get filter properties
    //
    _subscriptionRepository->getFilterProperties (subscriptionInstance,
        nameSpaceName, query, sourceNameSpace, queryLanguage);
    selectStatement = _getSelectStatement (query);

    //
    //  Get indication class name from filter query (FROM clause)
    //
    indicationClassName = _getIndicationClassName (selectStatement,
        sourceNameSpace);

    //
    //  Get list of subclass names for indication class
    //
    indicationSubclasses = _subscriptionRepository->getIndicationSubclasses
        (sourceNameSpace, indicationClassName);

    //
    //  Get property list from filter query (FROM and WHERE
    //  clauses)
    //
    propertyList = _getPropertyList (selectStatement,
        sourceNameSpace, indicationClassName);

    //
    //  Get indication provider class lists
    //
    indicationProviders = _getIndicationProviders
        (sourceNameSpace, indicationClassName, indicationSubclasses,
         propertyList);

    if (indicationProviders.size () > 0)
    {
        //
        //  Get condition from filter query (WHERE clause)
        //
        if (selectStatement.hasWhereClause ())
        {
            condition = _getCondition (query);
        }
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_getCreateParams (
    const CIMNamespaceName & nameSpaceName,
    const CIMInstance & subscriptionInstance,
    Array <CIMName> & indicationSubclasses,
    CIMPropertyList & propertyList,
    CIMNamespaceName & sourceNameSpace,
    String & condition,
    String & query,
    String & queryLanguage)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getCreateParams");

    WQLSelectStatement selectStatement;
    condition = String::EMPTY;
    query = String::EMPTY;
    queryLanguage = String::EMPTY;

    //
    //  Get filter properties
    //
    _subscriptionRepository->getFilterProperties (subscriptionInstance,
        nameSpaceName, query, sourceNameSpace, queryLanguage);
    selectStatement = _getSelectStatement (query);

    //
    //  Get property list from filter query (FROM and WHERE
    //  clauses)
    //
    CIMName indicationClassName = _getIndicationClassName (selectStatement,
        sourceNameSpace);
    propertyList = _getPropertyList (selectStatement,
        sourceNameSpace, indicationClassName);

    //
    //  Get condition from filter query (WHERE clause)
    //
    if (selectStatement.hasWhereClause ())
    {
        condition = _getCondition (query);
    }

    //
    //  Get indication class name from filter query (FROM clause)
    //
    indicationClassName = _getIndicationClassName (selectStatement,
        sourceNameSpace);

    //
    //  Get list of subclass names for indication class
    //
    indicationSubclasses = _subscriptionRepository->getIndicationSubclasses
            (sourceNameSpace, indicationClassName);

    PEG_METHOD_EXIT ();
}

Array <ProviderClassList> IndicationService::_getDeleteParams (
    const CIMNamespaceName & nameSpaceName,
    const CIMInstance & subscriptionInstance,
    Array <CIMName> & indicationSubclasses,
    CIMNamespaceName & sourceNameSpace)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getDeleteParams");

    String filterQuery;
    WQLSelectStatement selectStatement;
    CIMName indicationClassName;
    CIMPropertyList propertyList;
    Array <ProviderClassList> indicationProviders;

    //
    //  Get filter properties
    //
    _subscriptionRepository->getFilterProperties (subscriptionInstance,
        nameSpaceName, filterQuery, sourceNameSpace);
    selectStatement = _getSelectStatement (filterQuery);

    //
    //  Get indication class name from filter query (FROM clause)
    //
    indicationClassName = _getIndicationClassName (selectStatement,
        sourceNameSpace);

    //
    //  Get list of subclass names for indication class
    //
    indicationSubclasses = _subscriptionRepository->getIndicationSubclasses
        (sourceNameSpace, indicationClassName);

    //
    //  Get property list from filter query (FROM and WHERE
    //  clauses)
    //
    propertyList = _getPropertyList (selectStatement,
        sourceNameSpace, indicationClassName);

    //
    //  Get indication provider class lists from Active Subscriptions table
    //
    ActiveSubscriptionsTableEntry tableValue;
    if (_subscriptionTable->getSubscriptionEntry
        (subscriptionInstance.getPath (), tableValue))
    {
        indicationProviders = tableValue.providers;
    }
    else
    {
        //
        //  Subscription not found in Active Subscriptions table
        //
    }

    PEG_METHOD_EXIT ();
    return indicationProviders;
}

// l10n
void IndicationService::_sendCreateRequests
    (const Array <ProviderClassList> & indicationProviders,
     const CIMNamespaceName & nameSpace,
     const CIMPropertyList & propertyList,
     const String & condition,
     const String & query,
     const String & queryLanguage,
     const CIMInstance & subscription,
     const AcceptLanguages & acceptLangs,
     const ContentLanguages & contentLangs,
     const CIMRequestMessage * origRequest,
     const Array <CIMName> & indicationSubclasses,
     const String & userName,
     const String & authType)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_sendCreateRequests");

    CIMValue propValue;
    Uint16 repeatNotificationPolicy;

    // If there are no providers to accept the subscription, just return
    if (indicationProviders.size() == 0)
    {
        return;
    }

    //
    //  Get repeat notification policy value from subscription instance
    //
    propValue = subscription.getProperty
        (subscription.findProperty
        (_PROPERTY_REPEATNOTIFICATIONPOLICY)).getValue ();
    propValue.get (repeatNotificationPolicy);

    CIMRequestMessage * aggRequest;

    if (origRequest == 0)
    {
        //
        //  Initialize -- no request associated with this create
        //
        aggRequest = 0;
    }
    else
    {
        //
        //  Create Instance, Modify Instance, Provider Registration Change, or
        //  Provider Enable
        //
        switch (origRequest->getType ())
        {
            case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
            {
                CIMCreateInstanceRequestMessage * request =
                    (CIMCreateInstanceRequestMessage *) origRequest;
                CIMCreateInstanceRequestMessage * requestCopy =
                    new CIMCreateInstanceRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
            {
                CIMModifyInstanceRequestMessage * request =
                    (CIMModifyInstanceRequestMessage *) origRequest;
                CIMModifyInstanceRequestMessage * requestCopy =
                    new CIMModifyInstanceRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
            {
                CIMNotifyProviderRegistrationRequestMessage * request =
                    (CIMNotifyProviderRegistrationRequestMessage *) origRequest;
                CIMNotifyProviderRegistrationRequestMessage * requestCopy =
                    new CIMNotifyProviderRegistrationRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE:
            {
                CIMNotifyProviderEnableRequestMessage * request =
                    (CIMNotifyProviderEnableRequestMessage *) origRequest;
                CIMNotifyProviderEnableRequestMessage * requestCopy =
                    new CIMNotifyProviderEnableRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            default:
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                    Tracer::LEVEL2, "Unexpected origRequest type " +
                    String (MessageTypeToString (origRequest->getType ())) +
                    " in _sendCreateRequests");
                break;
            }
        }
    }

    //
    //  Create an aggregate object for the create subscription requests
    //
    IndicationOperationAggregate * operationAggregate =
        new IndicationOperationAggregate (aggRequest, indicationSubclasses);
    operationAggregate->setNumberIssued (indicationProviders.size ());

    //
    //  Send Create request to each provider
    //
    for (Uint32 i = 0; i < indicationProviders.size (); i++)
    {
        //
        //  Create the create subscription request
        //
// l10n
       CIMCreateSubscriptionRequestMessage * request =
            new CIMCreateSubscriptionRequestMessage
                (XmlWriter::getNextMessageId (),
                nameSpace,
                subscription,
                indicationProviders [i].classList,
                propertyList,
                repeatNotificationPolicy,
                query,
                QueueIdStack (_providerManager, getQueueId ()),
                authType,
                userName);

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMCreateSubscriptionRequestMessage * requestCopy =
            new CIMCreateSubscriptionRequestMessage (* request);
        requestCopy->operationContext.insert(ProviderIdContainer 
            (indicationProviders [i].providerModule,
            indicationProviders [i].provider)); 
        operationAggregate->appendRequest (requestCopy);

        request->operationContext.insert(ProviderIdContainer 
            (indicationProviders [i].providerModule,
            indicationProviders [i].provider)); 
        request->operationContext.insert(SubscriptionInstanceContainer
            (subscription));
        request->operationContext.insert(SubscriptionFilterConditionContainer
            (condition,queryLanguage));
        request->operationContext.insert(SubscriptionLanguageListContainer
            (acceptLangs));
        request->operationContext.insert(IdentityContainer(userName));
        request->operationContext.set(ContentLanguageListContainer
            (contentLangs));
        request->operationContext.set(AcceptLanguageListContainer(acceptLangs));

        AsyncOpNode * op = this->get_op ();

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

        SendAsync
            (op,
            _providerManager,
            IndicationService::_aggregationCallBack,
            this,
            operationAggregate);
    }

    PEG_METHOD_EXIT ();
}

// l10n
void IndicationService::_sendModifyRequests
    (const Array <ProviderClassList> & indicationProviders,
     const CIMNamespaceName & nameSpace,
     const CIMPropertyList & propertyList,
     const String & condition,
     const String & query,
     const String & queryLanguage,
     const CIMInstance & subscription,
     const AcceptLanguages & acceptLangs,
     const ContentLanguages & contentLangs,
     const CIMRequestMessage * origRequest,
     const String & userName,
     const String & authType)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_sendModifyRequests");

    CIMValue propValue;
    Uint16 repeatNotificationPolicy;

    // If there are no providers to accept the subscription update, just return
    if (indicationProviders.size() == 0)
    {
        return;
    }

    //
    //  Get repeat notification policy value from subscription instance
    //
    propValue = subscription.getProperty
        (subscription.findProperty
        (_PROPERTY_REPEATNOTIFICATIONPOLICY)).getValue ();
    propValue.get (repeatNotificationPolicy);

    CIMRequestMessage * aggRequest;

    //
    //  Create an aggregate object for the modify subscription requests
    //
    if (origRequest)
    {
        //
        //  Provider Registration Change
        //
        if (origRequest->getType () ==
            CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE)
        {
            CIMNotifyProviderRegistrationRequestMessage * request =
                (CIMNotifyProviderRegistrationRequestMessage *) origRequest;
            CIMNotifyProviderRegistrationRequestMessage * requestCopy =
                new CIMNotifyProviderRegistrationRequestMessage (* request);
            aggRequest = requestCopy;
        }
        else
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
                "Unexpected origRequest type " +
                String (MessageTypeToString (origRequest->getType ())) +
                " in _sendModifyRequests");
        }
    }
    else
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
            "No origRequest in _sendModifyRequests");
    }

    Array <CIMName> indicationSubclasses;  //  empty array -- not needed
    IndicationOperationAggregate * operationAggregate =
        new IndicationOperationAggregate (aggRequest, indicationSubclasses);
    operationAggregate->setNumberIssued (indicationProviders.size ());

    //
    //  Send Modify request to each provider
    //
    for (Uint32 i = 0; i < indicationProviders.size (); i++)
    {
// l10n
        CIMModifySubscriptionRequestMessage * request =
            new CIMModifySubscriptionRequestMessage
                (XmlWriter::getNextMessageId (),
                nameSpace,
                subscription,
                indicationProviders [i].classList,
                propertyList,
                repeatNotificationPolicy,
                query,
                QueueIdStack (_providerManager, getQueueId ()),
                authType,
                userName);

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMModifySubscriptionRequestMessage * requestCopy =
            new CIMModifySubscriptionRequestMessage (* request);
        requestCopy->operationContext.insert(ProviderIdContainer 
            (indicationProviders [i].providerModule,
            indicationProviders [i].provider));
        operationAggregate->appendRequest (requestCopy);

        request->operationContext.insert(ProviderIdContainer 
            (indicationProviders [i].providerModule,
            indicationProviders [i].provider));
        request->operationContext.insert(SubscriptionInstanceContainer
            (subscription));
        request->operationContext.insert(SubscriptionFilterConditionContainer
            (condition,queryLanguage));

        request->operationContext.insert(SubscriptionLanguageListContainer
            (acceptLangs));
        request->operationContext.insert(IdentityContainer(userName));
        request->operationContext.set(ContentLanguageListContainer
            (contentLangs));
        request->operationContext.set(AcceptLanguageListContainer(acceptLangs));

        AsyncOpNode * op = this->get_op ();

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

        SendAsync
            (op,
            _providerManager,
            IndicationService::_aggregationCallBack,
            this,
            operationAggregate);
    }

    PEG_METHOD_EXIT ();
}

// l10n
void IndicationService::_sendDeleteRequests
    (const Array <ProviderClassList> & indicationProviders,
     const CIMNamespaceName & nameSpace,
     const CIMInstance & subscription,
     const AcceptLanguages & acceptLangs,
     const ContentLanguages & contentLangs,
     const CIMRequestMessage * origRequest,
     const Array <CIMName> & indicationSubclasses,
     const String & userName,
     const String & authType)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_sendDeleteRequests");

    // If there are no providers to delete the subscription, just return
    if (indicationProviders.size() == 0)
    {
        return;
    }

    CIMRequestMessage * aggRequest;

    if (origRequest == 0)
    {
        //
        //  Delete a referencing or expired subscription -- no request
        //  associated with this delete
        //
        aggRequest = 0;
    }
    else
    {
        //
        //  Delete Instance, Modify Instance, or Provider Registration Change
        //
        switch (origRequest->getType ())
        {
            case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
            {
                CIMDeleteInstanceRequestMessage * request =
                    (CIMDeleteInstanceRequestMessage *) origRequest;
                CIMDeleteInstanceRequestMessage * requestCopy =
                    new CIMDeleteInstanceRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
            {
                CIMModifyInstanceRequestMessage * request =
                    (CIMModifyInstanceRequestMessage *) origRequest;
                CIMModifyInstanceRequestMessage * requestCopy =
                    new CIMModifyInstanceRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
            {
                CIMNotifyProviderRegistrationRequestMessage * request =
                    (CIMNotifyProviderRegistrationRequestMessage *) origRequest;
                CIMNotifyProviderRegistrationRequestMessage * requestCopy =
                    new CIMNotifyProviderRegistrationRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            default:
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                    Tracer::LEVEL2, "Unexpected origRequest type " +
                    String (MessageTypeToString (origRequest->getType ())) +
                    " in _sendDeleteRequests");
                break;
            }
        }
    }

    //
    //  Create an aggregate object for the delete subscription requests
    //
    IndicationOperationAggregate * operationAggregate =
        new IndicationOperationAggregate (aggRequest, indicationSubclasses);
    operationAggregate->setNumberIssued (indicationProviders.size ());

    //
    //  Send Delete request to each provider
    //
    for (Uint32 i = 0; i < indicationProviders.size (); i++)
    {
// l10n
        CIMDeleteSubscriptionRequestMessage * request =
            new CIMDeleteSubscriptionRequestMessage
                (XmlWriter::getNextMessageId (),
                nameSpace,
                subscription,
                indicationProviders [i].classList,
                QueueIdStack (_providerManager, getQueueId ()),
                authType,
                userName);

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMDeleteSubscriptionRequestMessage * requestCopy =
            new CIMDeleteSubscriptionRequestMessage (* request);
        requestCopy->operationContext.insert(ProviderIdContainer 
            (indicationProviders [i].providerModule,
            indicationProviders [i].provider));
        operationAggregate->appendRequest (requestCopy);

        request->operationContext.insert(ProviderIdContainer 
            (indicationProviders [i].providerModule,
            indicationProviders [i].provider)); 
        request->operationContext.insert(SubscriptionInstanceContainer
            (subscription));
        request->operationContext.insert(SubscriptionLanguageListContainer
            (acceptLangs));
        request->operationContext.insert(IdentityContainer(userName));
        request->operationContext.set(ContentLanguageListContainer
            (contentLangs));
        request->operationContext.set(AcceptLanguageListContainer(acceptLangs));

        AsyncOpNode * op = this->get_op ();

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

        SendAsync
            (op,
            _providerManager,
            IndicationService::_aggregationCallBack,
            this,
            operationAggregate);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_aggregationCallBack (
    AsyncOpNode * op,
    MessageQueue * q,
    void * userParameter)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_aggregationCallBack");

    IndicationService * service = static_cast <IndicationService *> (q);

    AsyncRequest * asyncRequest =
        static_cast <AsyncRequest *> (op->get_request ());
    AsyncReply * asyncReply = static_cast <AsyncReply *> (op->get_response ());

    IndicationOperationAggregate * operationAggregate =
        reinterpret_cast <IndicationOperationAggregate *> (userParameter);
    PEGASUS_ASSERT (operationAggregate != 0);
    PEGASUS_ASSERT (operationAggregate->valid ());

    CIMResponseMessage * response;
    Uint32 msgType = asyncReply->getType ();
    PEGASUS_ASSERT ((msgType == async_messages::ASYNC_LEGACY_OP_RESULT) ||
                    (msgType == async_messages::ASYNC_MODULE_OP_RESULT));

    if (msgType == async_messages::ASYNC_LEGACY_OP_RESULT)
    {
        response = reinterpret_cast <CIMResponseMessage *>
            ((static_cast <AsyncLegacyOperationResult *>
            (asyncReply))->get_result ());
    }
    else if (msgType == async_messages::ASYNC_MODULE_OP_RESULT)
    {
        response = reinterpret_cast <CIMResponseMessage *>
            ((static_cast <AsyncModuleOperationResult *>
            (asyncReply))->get_result ());
    }

    PEGASUS_ASSERT (response != 0);

    delete asyncRequest;
    delete asyncReply;
    op->release ();
    service->return_op (op);

    Boolean isDoneAggregation = operationAggregate->appendResponse (response);
    if (isDoneAggregation)
    {
        service->_handleOperationResponseAggregation (operationAggregate);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleOperationResponseAggregation (
    IndicationOperationAggregate * operationAggregate)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleOperationResponseAggregation");

    switch (operationAggregate->getRequest (0)->getType ())
    {
        case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
        {
            _handleCreateResponseAggregation (operationAggregate);
            break;
        }

        case CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE:
        {
            _handleEnableResponseAggregation (operationAggregate);
            break;
        }

        case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
        {
            _handleModifyResponseAggregation (operationAggregate);
            break;
        }

        case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
        {
            _handleDeleteResponseAggregation (operationAggregate);
            break;
        }

        case CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE:
        {
            _handleDisableResponseAggregation (operationAggregate);
            break;
        }

        default:
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
                "Unexpected request type " + String (MessageTypeToString
                    (operationAggregate->getRequest (0)->getType ())) +
                " in _handleOperationResponseAggregation");
            break;
        }
    }

    //
    //  Requests and responses are deleted in destructor
    //
    delete operationAggregate;

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleCreateResponseAggregation (
    IndicationOperationAggregate * operationAggregate)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleCreateResponseAggregation");

    Array <ProviderClassList> acceptedProviders;
    CIMObjectPath instanceRef;
    CIMException cimException;
    Boolean responseSent = false;

    //
    //  Examine provider responses
    //
    Uint32 accepted = 0;
    for (Uint32 i = 0; i < operationAggregate->getNumberResponses (); i++)
    {
        //
        //  If response is SUCCESS, provider accepted the subscription
        //  Add provider to list of providers that accepted subscription
        //
        CIMResponseMessage * response = operationAggregate->getResponse (i);
        ProviderClassList provider = operationAggregate->findProvider
            (response->messageId);
        if (response->cimException.getCode () == CIM_ERR_SUCCESS)
        {
            //
            //  Find provider from which response was sent
            //
            accepted++;
            acceptedProviders.append (provider);
        }
        else
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3,
                "Provider (" + provider.provider.getPath().toString() +
                ") rejected create subscription: " +
                response->cimException.getMessage ());
        }
    }

    CIMCreateSubscriptionRequestMessage * request =
        (CIMCreateSubscriptionRequestMessage *)
            operationAggregate->getRequest (0);
    if (accepted == 0)
    {
        //
        //  No providers accepted this subscription
        //
        if (operationAggregate->getOrigType () == 0)
        {
            //
            //  For Initialize
            //  Implement the subscription's On Fatal Error Policy
            //  If subscription is not disabled or removed, send alert and
            //  Insert entries into the subscription hash tables
            //
            if (!_subscriptionRepository->reconcileFatalError
                (request->subscriptionInstance))
            {
                //
                //  Insert entries into the subscription hash tables
                //
                _subscriptionTable->insertSubscription
                    (request->subscriptionInstance,
                    acceptedProviders,
                    operationAggregate->getIndicationSubclasses (),
                    request->nameSpace);

#if 0
                //
                //  Send alert
                //
                //
                //  Send NoProviderAlertIndication to handler instances
                //  ATTN: NoProviderAlertIndication must be defined
                //
                Array <CIMInstance> subscriptions;
                subscriptions.append (request->subscriptionInstance);
                CIMInstance indicationInstance = _createAlertInstance
                    (_CLASS_NO_PROVIDER_ALERT, subscriptions);

                Tracer::trace (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
                    "Sending NoProvider Alert for %d subscriptions",
                    subscriptions.size ());
                _sendAlerts (subscriptions, indicationInstance);
#endif

                //
                //  Get Subscription Filter Name and Handler Name
                //
                String logString = _getSubscriptionLogString
                    (request->subscriptionInstance);

                //
                //  Log a message for the subscription
                //
                Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER,
                    Logger::WARNING, _MSG_NO_PROVIDER_KEY, _MSG_NO_PROVIDER,
                    logString);
            }
        }

        else if (operationAggregate->requiresResponse ())
        {
            //
            //  For Create Instance, Modify Instance, Notify Provider Enable or
            //  Notify Provider Registration request, set CIM exception for 
            //  response
            //
            // l10n
            cimException = PEGASUS_CIM_EXCEPTION_L (CIM_ERR_NOT_SUPPORTED,
                MessageLoaderParms (_MSG_NOT_ACCEPTED_KEY, _MSG_NOT_ACCEPTED));
        }
    }

    else
    {
        //
        //  At least one provider accepted the subscription
        //

        Array <ProviderClassList> enableProviders;

        if (operationAggregate->getOrigType () ==
            CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE)
        {
            //
            //  Provider Registration Change
            //
            CIMNotifyProviderRegistrationRequestMessage * origRequest =
                (CIMNotifyProviderRegistrationRequestMessage *)
                    operationAggregate->getOrigRequest ();
            ProviderClassList provider;
            ProviderIdContainer pidc = origRequest->operationContext.get
                (ProviderIdContainer::NAME);
            provider.provider = pidc.getProvider(); 
            provider.providerModule = pidc.getModule();
            provider.classList.append (origRequest->className);

            //
            //  Update the entry in the active subscriptions hash table
            //
            enableProviders = _subscriptionTable->updateProviders
                (request->subscriptionInstance.getPath (), provider, true);

            //
            //  Send Enable message to each provider that accepted
            //  subscription
            //
            if (enableProviders.size () > 0)
            {
                _sendEnable (enableProviders,
                    operationAggregate->getOrigRequest ());

                //
                //  Response is sent from _handleEnableResponseAggregation
                //
                responseSent = true;
            }
        }
        else if (operationAggregate->getOrigType () ==
            CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE)
        {
            //
            //  Provider Enable
            //
            CIMNotifyProviderEnableRequestMessage * origRequest =
                (CIMNotifyProviderEnableRequestMessage *)
                    operationAggregate->getOrigRequest ();
            ProviderClassList provider;
            ProviderIdContainer pidc = origRequest->operationContext.get
                (ProviderIdContainer::NAME);
            provider.provider = pidc.getProvider();
            provider.providerModule = pidc.getModule();
            for (Uint32 j = 0; j < origRequest->capInstances.size (); j++)
            {
                //
                //  If class not yet there, add to provider class list
                //
                String cName;
                origRequest->capInstances [j].getProperty
                    (origRequest->capInstances [j].findProperty
                    (_PROPERTY_CLASSNAME)).getValue ().get (cName);
                CIMName className = CIMName (cName);
                Uint32 classIndex = _subscriptionTable->classInList
                    (className, provider);
                if (classIndex == PEG_NOT_FOUND)
                {
                    provider.classList.append (className);
                }
            }

            //
            //  Update the entry in the active subscriptions hash table
            //
            enableProviders = _subscriptionTable->updateProviders
                (request->subscriptionInstance.getPath (), provider, true);

            //
            //  Send Enable message to each provider that accepted
            //  subscription
            //
            if (enableProviders.size () > 0)
            {
                _sendEnable (enableProviders,
                    operationAggregate->getOrigRequest ());

                //
                //  Response is sent from _handleEnableResponseAggregation
                //
                responseSent = true;
            }
        }
        else if (operationAggregate->getOrigType () ==
            CIM_CREATE_INSTANCE_REQUEST_MESSAGE)
        {
            //
            //  Create Instance -- create the instance in the repository
            //
            CIMCreateInstanceRequestMessage * origRequest =
                (CIMCreateInstanceRequestMessage *)
                    operationAggregate->getOrigRequest ();

            CIMInstance instance = request->subscriptionInstance;
            try
            {
                instanceRef = _subscriptionRepository->createInstance 
                    (request->subscriptionInstance, origRequest->nameSpace, 
                    ((IdentityContainer)origRequest->operationContext.get
                        (IdentityContainer::NAME)).getUserName(), 
                    ((AcceptLanguageListContainer)request->operationContext.get
                        (AcceptLanguageListContainer::NAME)).getLanguages(),
                    ((ContentLanguageListContainer)request->operationContext.get
                        (ContentLanguageListContainer::NAME)).getLanguages(),
                        true);
                instanceRef.setNameSpace 
                    (request->subscriptionInstance.getPath().getNameSpace());
                instance.setPath (instanceRef);
            }
            catch (CIMException & exception)
            {
                cimException = exception;
            }
            catch (Exception & exception)
            {
                cimException = PEGASUS_CIM_EXCEPTION
                    (CIM_ERR_FAILED, exception.getMessage ());
            }

            if (cimException.getCode () == CIM_ERR_SUCCESS)
            {
                //
                //  Insert entries into the subscription hash tables
                //
                enableProviders = _subscriptionTable->insertSubscription
                    (instance,
                    acceptedProviders,
                    operationAggregate->getIndicationSubclasses (),
                    request->nameSpace);

                if (enableProviders.size () > 0)
                {
                    //
                    //  Send Enable message to each provider that accepted 
                    //  subscription
                    //
                    _sendEnable (enableProviders, 
                        operationAggregate->getOrigRequest (), instanceRef);

                    //
                    //  Response is sent from _handleEnableResponseAggregation
                    //
                    responseSent = true;
                }
            }
        }
        else //  Initialize or Modify Instance
        {
            PEGASUS_ASSERT ((operationAggregate->getOrigType () == 0) ||
                            (operationAggregate->getOrigType () ==
                             CIM_MODIFY_INSTANCE_REQUEST_MESSAGE));

            //
            //  Insert entries into the subscription hash tables
            //
            enableProviders = _subscriptionTable->insertSubscription
                (request->subscriptionInstance,
                acceptedProviders,
                operationAggregate->getIndicationSubclasses (),
                request->nameSpace);

            if (enableProviders.size () > 0)
            {
                //
                //  Send Enable message to each provider that accepted 
                //  subscription
                //
                _sendEnable (enableProviders, 
                    operationAggregate->getOrigRequest ());

                //
                //  Response is sent from _handleEnableResponseAggregation
                //
                responseSent = true;
            }
        }
    }

    //
    //  For Create Instance, Modify Instance, Notify Provider Enable or
    //  Notify Provider Registration request,
    //  send response, if not sent from callback
    //  When an enable indications request was required, the response (if 
    //  required) is sent from _handleEnableResponseAggregation, and must not 
    //  be sent from here
    //
    if ((!responseSent) && (operationAggregate->requiresResponse ()))
    {
        CIMResponseMessage * response;
        if (operationAggregate->getOrigType () ==
            CIM_CREATE_INSTANCE_REQUEST_MESSAGE)
        {
            // l10n
            // Note: don't need to set Content-language in the response
            response = new CIMCreateInstanceResponseMessage (
                operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop (),
                instanceRef);
        }

        else if (operationAggregate->getOrigType () == 
            CIM_MODIFY_INSTANCE_REQUEST_MESSAGE)
        {
            // l10n
            // Note: don't need to set Content-language in the response
            //
            response = new CIMModifyInstanceResponseMessage (
                operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        else if (operationAggregate->getOrigType () == 
            CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE)
        {
            response = new CIMNotifyProviderEnableResponseMessage (
                operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        else //  CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE
        {
            PEGASUS_ASSERT (operationAggregate->getOrigType () ==
                            CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE);

            response = new CIMNotifyProviderRegistrationResponseMessage 
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        //
        //  Preserve message key
        //
        response->setKey (operationAggregate->getOrigRequest ()->getKey ());

        //
        //  Set response destination
        //
        response->dest = operationAggregate->getOrigDest ();

        //
        //  Set HTTP method in response from request
        //
        response->setHttpMethod
            (operationAggregate->getOrigRequest ()->getHttpMethod ());

        Base::_enqueueResponse
            (operationAggregate->getOrigRequest (), response);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleEnableResponseAggregation (
    IndicationOperationAggregate * operationAggregate)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleEnableResponseAggregation");

    CIMException cimException;

    //
    //  Examine provider responses
    //
    for (Uint32 i = 0; i < operationAggregate->getNumberResponses (); i++)
    {
        //
        //  If response is not SUCCESS, provider rejected the enable
        //
        CIMResponseMessage * response = operationAggregate->getResponse (i);
        if (response->cimException.getCode () != CIM_ERR_SUCCESS)
        {
            //
            //  Find provider from which response was sent
            //  Log a trace message
            //
            ProviderClassList provider = operationAggregate->findProvider
                (response->messageId);
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3,
                "Provider (" + provider.provider.getPath ().toString() +
                ") rejected enable indications: " +
                response->cimException.getMessage ());

            //
            //  NOTE: if there are multiple non-success responses, the last will
            //  be returned in the response
            //
            cimException = response->cimException;
        }
    }

    //
    //  For Create Instance, Modify Instance, Notify Provider Enable or
    //  Notify Provider Registration request, send response
    //
    if (operationAggregate->requiresResponse ())
    {
        CIMResponseMessage * response;

        if (operationAggregate->getOrigType () ==
            CIM_CREATE_INSTANCE_REQUEST_MESSAGE)
        {
            //
            //  Get the object path of the created instance from the operation 
            //  aggregate object 
            //
            CIMObjectPath instanceRef = operationAggregate->getPath ();

            response = new CIMCreateInstanceResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop (),
                instanceRef);
        }

        else if (operationAggregate->getOrigType () == 
            CIM_MODIFY_INSTANCE_REQUEST_MESSAGE)
        {
            response = new CIMModifyInstanceResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        else if (operationAggregate->getOrigType () == 
            CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE)
        {
            response = new CIMNotifyProviderEnableResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        else //  CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE
        {
            PEGASUS_ASSERT (operationAggregate->getOrigType () ==
                            CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE);

            response = new CIMNotifyProviderRegistrationResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        //
        //  Preserve message key
        //
        response->setKey (operationAggregate->getOrigRequest ()->getKey ());
    
        //
        //  Set response destination
        //
        response->dest = operationAggregate->getOrigDest ();

        //
        //  Set HTTP method in response from request
        //
        response->setHttpMethod 
            (operationAggregate->getOrigRequest ()->getHttpMethod ());
    
        Base::_enqueueResponse 
            (operationAggregate->getOrigRequest (), response);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleModifyResponseAggregation (
    IndicationOperationAggregate * operationAggregate)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleModifyResponseAggregation");

    CIMException cimException;

    //
    //  Examine provider responses
    //
    for (Uint32 i = 0; i < operationAggregate->getNumberResponses (); i++)
    {
        //
        //  If response is not SUCCESS, provider rejected the modification
        //
        CIMResponseMessage * response = operationAggregate->getResponse (i);
        if (response->cimException.getCode () != CIM_ERR_SUCCESS)
        {
            //
            //  Find provider from which response was sent
            //  Log a trace message
            //
            ProviderClassList provider = operationAggregate->findProvider
                (response->messageId);
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3,
                "Provider (" + provider.provider.getPath ().toString() +
                ") rejected modify subscription: " +
                response->cimException.getMessage ());

            //
            //  NOTE: if there are multiple non-success responses, the last will
            //  be returned in the response
            //
            cimException = response->cimException;
        }
    }

    //
    //  Update active subscriptions hash table
    //
    CIMModifySubscriptionRequestMessage * request =
        (CIMModifySubscriptionRequestMessage *)
            operationAggregate->getRequest (0);

    CIMNotifyProviderRegistrationRequestMessage * origRequest =
        (CIMNotifyProviderRegistrationRequestMessage *)
            operationAggregate->getOrigRequest ();

    ProviderIdContainer pidc = origRequest->operationContext.get
        (ProviderIdContainer::NAME);
    CIMInstance provider = pidc.getProvider();

    _subscriptionTable->updateClasses
        (request->subscriptionInstance.getPath (), provider,
        origRequest->className);

    //
    //  For Notify Provider Registration request, send response
    //
    if (operationAggregate->requiresResponse ())
    {
        PEGASUS_ASSERT (operationAggregate->getOrigType () ==
                        CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE);

        CIMResponseMessage * response;

        response = new CIMNotifyProviderRegistrationResponseMessage
            (operationAggregate->getOrigMessageId (),
            cimException,
            operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());

        Base::_enqueueResponse 
            (operationAggregate->getOrigRequest (), response);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleDeleteResponseAggregation (
    IndicationOperationAggregate * operationAggregate)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleDeleteResponseAggregation");

    CIMException cimException;
    Boolean responseSent = false;
    Array <ProviderClassList> checkProviders;

    //
    //  Examine provider responses
    //
    for (Uint32 i = 0; i < operationAggregate->getNumberResponses (); i++)
    {
        //
        //  Find provider from which response was sent and add to list
        //  List will be checked later to determine if Disable Indications
        //  request must be sent
        //
        CIMResponseMessage * response = operationAggregate->getResponse (i);
        ProviderClassList provider = operationAggregate->findProvider
            (response->messageId);
        checkProviders.append (provider);

        //
        //  If response is not SUCCESS, provider rejected the delete
        //
        if (response->cimException.getCode () != CIM_ERR_SUCCESS)
        {
            //
            //  Log a trace message
            //
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3,
                "Provider (" + provider.provider.getPath ().toString() +
                ") rejected delete subscription: " +
                response->cimException.getMessage ());
        }
    }

    //
    //  Update subscription hash tables
    //
    CIMDeleteSubscriptionRequestMessage * request =
        (CIMDeleteSubscriptionRequestMessage *)
            operationAggregate->getRequest (0);

    Array <ProviderClassList> disableProviders;

    if (operationAggregate->getOrigType () ==
        CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE)
    {
        CIMNotifyProviderRegistrationRequestMessage * origRequest =
            (CIMNotifyProviderRegistrationRequestMessage *)
                operationAggregate->getOrigRequest ();

        //
        //  Update the entry in the active subscriptions hash table
        //
        ProviderClassList provider;
        ProviderIdContainer pidc = origRequest->operationContext.get
            (ProviderIdContainer::NAME); 
        provider.provider = pidc.getProvider(); 
        provider.providerModule = pidc.getModule();
        disableProviders = _subscriptionTable->updateProviders
            (request->subscriptionInstance.getPath (), provider, false);
    }

    //
    //  Delete Instance, Modify Instance, or Delete Referencing or Expired
    //  Subscription
    //
    else
    {
        //
        //  Remove entries from the subscription hash tables
        //
        disableProviders = _subscriptionTable->removeSubscription
            (request->subscriptionInstance,
            operationAggregate->getIndicationSubclasses (),
            request->nameSpace,
            checkProviders);
    }

    //
    //  Send Disable Indications requests to any provider no longer in
    //  use
    //
    if (disableProviders.size () > 0)
    {
        _sendDisable (disableProviders, operationAggregate->getOrigRequest ());

        //
        //  Response is sent from _handleDisableResponseAggregation
        //
        responseSent = true;
    }

    //
    //  For Delete Instance, Modify Instance, or Notify Provider Registration
    //  request, send response if not sent from callback
    //  When a disable indicatiosn request was required, the response (if 
    //  required) is sent from _handleDisableResponseAggregation, and must not 
    //  be sent from here
    //
    if ((!responseSent) && (operationAggregate->requiresResponse ()))
    {
        CIMResponseMessage * response;
        if (operationAggregate->getOrigType () ==
            CIM_DELETE_INSTANCE_REQUEST_MESSAGE)
        {
            // l10n
            // Note: don't need to set Content-language in the response
            response = new CIMDeleteInstanceResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        else if (operationAggregate->getOrigType () ==
            CIM_MODIFY_INSTANCE_REQUEST_MESSAGE)
        {
            // l10n
            // Note: don't need to set Content-language in the response
            response = new CIMModifyInstanceResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        else  // CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE
        {
            PEGASUS_ASSERT (operationAggregate->getOrigType () ==
                            CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE);

            response = new CIMNotifyProviderRegistrationResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        //
        //  Preserve message key
        //
        response->setKey (operationAggregate->getOrigRequest ()->getKey ());

        //
        //  Set response destination
        //
        response->dest = operationAggregate->getOrigDest ();

        //
        //  Set HTTP method in response from request
        //
        response->setHttpMethod
            (operationAggregate->getOrigRequest ()->getHttpMethod ());

        Base::_enqueueResponse
            (operationAggregate->getOrigRequest (), response);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleDisableResponseAggregation (
    IndicationOperationAggregate * operationAggregate)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleDisableResponseAggregation");

    CIMException cimException;

    //
    //  Examine provider responses
    //
    for (Uint32 i = 0; i < operationAggregate->getNumberResponses (); i++)
    {
        //
        //  If response is not SUCCESS, provider rejected the disable
        //
        CIMResponseMessage * response = operationAggregate->getResponse (i);
        if (response->cimException.getCode () != CIM_ERR_SUCCESS)
        {
            //
            //  Find provider from which response was sent
            //  Log a trace message
            //
            ProviderClassList provider = operationAggregate->findProvider
                (response->messageId);
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3,
                "Provider (" + provider.provider.getPath ().toString() +
                ") rejected disable indications: " +
                response->cimException.getMessage ());

            //
            //  NOTE: if there are multiple non-success responses, the last will
            //  be returned in the response
            //
            cimException = response->cimException;
        }
    }

    //
    //  For Delete Instance, Modify Instance or Notify Provider Registration
    //  request, send response
    //
    if (operationAggregate->requiresResponse ())
    {
        CIMResponseMessage * response;
        if (operationAggregate->getOrigType () ==
            CIM_DELETE_INSTANCE_REQUEST_MESSAGE)
        {
            response = new CIMDeleteInstanceResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        else if (operationAggregate->getOrigType () ==
            CIM_MODIFY_INSTANCE_REQUEST_MESSAGE)
        {
            response = new CIMModifyInstanceResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        else  // CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE
        {
            PEGASUS_ASSERT (operationAggregate->getOrigType () ==
                            CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE);

            response = new CIMNotifyProviderRegistrationResponseMessage
                (operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        //
        //  Preserve message key
        //
        response->setKey (operationAggregate->getOrigRequest ()->getKey ());
    
        //
        //  Set response destination
        //
        response->dest = operationAggregate->getOrigDest ();

        //
        //  Set HTTP method in response from request
        //
        response->setHttpMethod 
            (operationAggregate->getOrigRequest ()->getHttpMethod ());
    
        Base::_enqueueResponse 
            (operationAggregate->getOrigRequest (), response);
    }

    PEG_METHOD_EXIT ();
}

CIMInstance IndicationService::_createAlertInstance (
    const CIMName & alertClassName,
    const Array <CIMInstance> & subscriptions)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_createAlertInstance");

    CIMInstance indicationInstance (alertClassName);

    //
    //  Add property values for all required properties of CIM_AlertIndication
    //
    indicationInstance.addProperty
        (CIMProperty (_PROPERTY_ALERTTYPE, CIMValue ((Uint16) _TYPE_OTHER)));
    //
    //  ATTN: what should Other Alert Type value be??
    //  Currently using Alert class name
    //
    indicationInstance.addProperty
        (CIMProperty (_PROPERTY_OTHERALERTTYPE, alertClassName.getString()));

    indicationInstance.addProperty
        (CIMProperty (_PROPERTY_PERCEIVEDSEVERITY,
                      CIMValue ((Uint16) _SEVERITY_WARNING)));
    //
    //  ATTN: what should Probable Cause value be??
    //  Currently using Unknown
    //
    indicationInstance.addProperty
        (CIMProperty (_PROPERTY_PROBABLECAUSE,
                      CIMValue ((Uint16) _CAUSE_UNKNOWN)));

    //
    //  Add properties specific to each alert class
    //  ATTN: update once alert classes have been defined
    //  NB: for _CLASS_NO_PROVIDER_ALERT and _CLASS_PROVIDER_TERMINATED_ALERT,
    //  one of the properties will be a list of affected subscriptions
    //  It is for that reason that subscriptions is passed in as a parameter
    //
    if (alertClassName.equal (_CLASS_CIMOM_SHUTDOWN_ALERT))
    {
    }
    else if (alertClassName.equal (_CLASS_NO_PROVIDER_ALERT))
    {
    }
    else if (alertClassName.equal (_CLASS_PROVIDER_TERMINATED_ALERT))
    {
    }

    PEG_METHOD_EXIT ();
    return indicationInstance;
}


#if 0
void IndicationService::_sendAlertsCallBack(AsyncOpNode *op,
    MessageQueue *q,
    void *parm)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_sendAlertsCallBack");

   IndicationService *service =
      static_cast<IndicationService *>(q);
   CIMInstance *_handler =
      reinterpret_cast<CIMInstance *>(parm);

   AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
   AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());
   CIMRequestMessage *request = reinterpret_cast<CIMRequestMessage *>
      ((static_cast<AsyncLegacyOperationStart *>(asyncRequest))->get_action());

   CIMHandleIndicationResponseMessage* response =
      reinterpret_cast<CIMHandleIndicationResponseMessage *>
      ((static_cast<AsyncLegacyOperationResult *>
      (asyncReply))->get_result());

   PEGASUS_ASSERT(response != 0);
   if (response->cimException.getCode() == CIM_ERR_SUCCESS)
   {
   }
   else
   {
   }

   //
   //  ATTN: Check for return value indicating invalid queue ID
   //  If received, need to find Handler Manager Service queue ID
   //  again
   //

// << Mon Jul 15 09:59:16 2002 mdd >> handler is allocated as an element in an array,
// don't delete here.
//   delete _handler;
   delete request;
   delete response;
   delete asyncRequest;
   delete asyncReply;
   op->release();
   service->return_op(op);

    PEG_METHOD_EXIT ();
}


void IndicationService::_sendAlerts (
    const Array <CIMInstance> & subscriptions,
    /* const */ CIMInstance & alertInstance)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_sendAlerts");

    CIMInstance current;
    Boolean duplicate;
    Array <CIMInstance> handlers;

    handlers.clear ();

    PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
        "Sending alert: " + alertInstance.getClassName().getString());

    //
    //  Get list of unique handler instances for all subscriptions in list
    //
    for (Uint32 i = 0; i < subscriptions.size (); i++)
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL4,
            "Alert subscription: " + subscriptions [i].getPath().toString());

        //
        //  Get handler instance
        //
        current = _subscriptionRepository->getHandler (subscriptions [i]);

        //
        //  Merge into list of unique handler instances
        //
        duplicate = false;
        for (Uint32 j = 0; j < handlers.size () && !duplicate; j++)
        {
            if ((current.identical (handlers [j])) &&
                (current.getPath () == handlers [j].getPath ()))
            {
                duplicate = true;
            }
        }

        if (!duplicate)
        {
            handlers.append (current);
        }
    }

    //
    //  Send handle indication request to each handler
    //
    for (Uint32 k = 0; k < handlers.size (); k++)
    {
        CIMHandleIndicationRequestMessage * handler_request =
            new CIMHandleIndicationRequestMessage (
                XmlWriter::getNextMessageId (),
                handlers[k].getPath ().getNameSpace (),
                handlers[k],
                alertInstance,
                QueueIdStack (_handlerService, getQueueId ()));

        AsyncOpNode* op = this->get_op();

        AsyncLegacyOperationStart *async_req =
            new AsyncLegacyOperationStart(
                get_next_xid(),
                op,
                _handlerService,
                handler_request,
                _queueId);

        SendAsync(op,
                  _handlerService,
                  IndicationService::_sendAlertsCallBack,
                  this,
                  (void *)&handlers[k]);

        // <<< Fri Apr  5 06:24:14 2002 mdd >>>
        // AsyncReply *async_reply = SendWait(async_req);
    }

    PEG_METHOD_EXIT ();
}
#endif

void IndicationService::_sendEnable (
    const Array <ProviderClassList> & enableProviders,
    const CIMRequestMessage * origRequest,
    const CIMObjectPath & path)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_sendEnable");

    // If there are no providers to enable, just return
    if (enableProviders.size() == 0)
    {
        return;
    }

    CIMRequestMessage * aggRequest;

    if (origRequest == 0)
    {
        //
        //  Initialize -- no request associated with this enable
        //
        aggRequest = 0;
    }
    else
    {
        //
        //  Create Instance, Modify Instance, Provider Registration Change,
        //  or Provider Enable
        //
        switch (origRequest->getType ())
        {
            case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
            {
                CIMCreateInstanceRequestMessage * request =
                    (CIMCreateInstanceRequestMessage *) origRequest;
                CIMCreateInstanceRequestMessage * requestCopy =
                    new CIMCreateInstanceRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
            {
                CIMModifyInstanceRequestMessage * request =
                    (CIMModifyInstanceRequestMessage *) origRequest;
                CIMModifyInstanceRequestMessage * requestCopy =
                    new CIMModifyInstanceRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
            {
                CIMNotifyProviderRegistrationRequestMessage * request =
                    (CIMNotifyProviderRegistrationRequestMessage *) origRequest;
                CIMNotifyProviderRegistrationRequestMessage * requestCopy =
                    new CIMNotifyProviderRegistrationRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE:
            {
                CIMNotifyProviderEnableRequestMessage * request =
                    (CIMNotifyProviderEnableRequestMessage *) origRequest;
                CIMNotifyProviderEnableRequestMessage * requestCopy =
                    new CIMNotifyProviderEnableRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            default:
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                    Tracer::LEVEL2, "Unexpected origRequest type " +
                    String (MessageTypeToString (origRequest->getType ())) +
                    " in _sendEnable");
                break;
            }
        }
    }

    //
    //  Create an aggregate object for the enable indications requests
    //
    Array <CIMName> indicationSubclasses;  //  empty array -- not needed
    IndicationOperationAggregate * operationAggregate =
        new IndicationOperationAggregate (aggRequest, indicationSubclasses);
    operationAggregate->setNumberIssued (enableProviders.size ());

    //
    //  If original request was to create subscription instance, store the
    //  object path of the created instance in the operation aggregate object 
    //  (because it will be needed for the response)
    //
    if (origRequest->getType () == CIM_CREATE_INSTANCE_REQUEST_MESSAGE)
    {
        operationAggregate->setPath (path);
    }

    //
    //  Send Enable request to each provider
    //
    for (Uint32 i = 0; i < enableProviders.size (); i++)
    {
        //
        //  Create the enable indications request
        //
        CIMEnableIndicationsRequestMessage * request =
            new CIMEnableIndicationsRequestMessage
                (XmlWriter::getNextMessageId (),
                QueueIdStack (_providerManager, getQueueId ()));

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMEnableIndicationsRequestMessage * requestCopy =
            new CIMEnableIndicationsRequestMessage (* request);
        requestCopy->operationContext.insert(ProviderIdContainer
            (enableProviders [i].providerModule,
            enableProviders [i].provider));
        operationAggregate->appendRequest (requestCopy);

        request->operationContext.insert(ProviderIdContainer
            (enableProviders [i].providerModule,
            enableProviders [i].provider));

        AsyncOpNode* op = this->get_op (); 

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

        SendAsync
            (op,
            _providerManager,
            IndicationService::_aggregationCallBack,
            this,
            operationAggregate);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_sendDisable (
    const Array <ProviderClassList> & disableProviders,
    const CIMRequestMessage * origRequest)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_sendDisable");

    // If there are no providers to disable, just return
    if (disableProviders.size() == 0)
    {
        return;
    }

    CIMRequestMessage * aggRequest;

    if (origRequest == 0)
    {
        //
        //  Delete a referencing or expired subscription -- no request
        //  associated with this delete
        //
        aggRequest = 0;
    }
    else
    {
        //
        //  Delete Instance, Modify Instance, or Provider Registration Change
        //
        switch (origRequest->getType ())
        {
            case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
            {
                CIMDeleteInstanceRequestMessage * request =
                    (CIMDeleteInstanceRequestMessage *) origRequest;
                CIMDeleteInstanceRequestMessage * requestCopy =
                    new CIMDeleteInstanceRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
            {
                CIMModifyInstanceRequestMessage * request =
                    (CIMModifyInstanceRequestMessage *) origRequest;
                CIMModifyInstanceRequestMessage * requestCopy =
                    new CIMModifyInstanceRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
            {
                CIMNotifyProviderRegistrationRequestMessage * request =
                    (CIMNotifyProviderRegistrationRequestMessage *) origRequest;
                CIMNotifyProviderRegistrationRequestMessage * requestCopy =
                    new CIMNotifyProviderRegistrationRequestMessage (* request);
                aggRequest = requestCopy;
                break;
            }

            default:
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                    Tracer::LEVEL2, "Unexpected origRequest type " +
                    String (MessageTypeToString (origRequest->getType ())) +
                    " in _sendDisable");
                break;
            }
        }
    }

    //
    //  Create an aggregate object for the disable indications requests
    //
    Array <CIMName> indicationSubclasses;  //  empty array -- not needed
    IndicationOperationAggregate * operationAggregate =
        new IndicationOperationAggregate (aggRequest, indicationSubclasses);
    operationAggregate->setNumberIssued (disableProviders.size ());

    //
    //  Send Disable Indications request to each provider
    //
    for (Uint32 i = 0; i < disableProviders.size (); i++)
    {
        //
        //  Create the disable indications request
        //
        CIMDisableIndicationsRequestMessage * request =
            new CIMDisableIndicationsRequestMessage
                (XmlWriter::getNextMessageId (),
                QueueIdStack (_providerManager, getQueueId ()));

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMDisableIndicationsRequestMessage * requestCopy =
            new CIMDisableIndicationsRequestMessage (* request);
        requestCopy->operationContext.insert(ProviderIdContainer
            (disableProviders [i].providerModule,
            disableProviders [i].provider));
        operationAggregate->appendRequest (requestCopy);

        request->operationContext.insert(ProviderIdContainer
            (disableProviders [i].providerModule,
            disableProviders [i].provider)); 

        AsyncOpNode* op = this->get_op ();

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

        SendAsync
            (op,
            _providerManager,
            IndicationService::_aggregationCallBack,
            this,
            operationAggregate);
    }

    PEG_METHOD_EXIT ();
}

WQLSimplePropertySource IndicationService::_getPropertySourceFromInstance(
    CIMInstance& indicationInstance)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getPropertySourceFromInstance");

    WQLSimplePropertySource source;

    for (Uint32 i=0; i < indicationInstance.getPropertyCount(); i++)
    {
        CIMProperty property = indicationInstance.getProperty(i);
        CIMValue propertyValue = property.getValue();
        CIMType type = property.getType();
        CIMName propertyName = property.getName();

        if (propertyValue.isNull())
        {
            source.addValue(propertyName.getString(), WQLOperand());
        }
        else if (propertyValue.isArray())
        {
            // ATTN: How are arrays handled in WQL?  Ignore them for now.
            // (See Bugzilla 1060)
        }
        else
        {
            switch (type)
            {
            case CIMTYPE_UINT8:
                Uint8 propertyValueUint8;
                propertyValue.get(propertyValueUint8);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueUint8, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMTYPE_UINT16:
                Uint16 propertyValueUint16;
                propertyValue.get(propertyValueUint16);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueUint16, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMTYPE_UINT32:
                Uint32 propertyValueUint32;
                propertyValue.get(propertyValueUint32);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueUint32, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMTYPE_UINT64:
                Uint64 propertyValueUint64;
                propertyValue.get(propertyValueUint64);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueUint64, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMTYPE_SINT8:
                Sint8 propertyValueSint8;
                propertyValue.get(propertyValueSint8);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueSint8, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMTYPE_SINT16:
                Sint16 propertyValueSint16;
                propertyValue.get(propertyValueSint16);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueSint16, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMTYPE_SINT32:
                Sint32 propertyValueSint32;
                propertyValue.get(propertyValueSint32);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueSint32, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMTYPE_SINT64:
                Sint64 propertyValueSint64;
                propertyValue.get(propertyValueSint64);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueSint64, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMTYPE_REAL32:
                Real32 propertyValueReal32;
                propertyValue.get(propertyValueReal32);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueReal32, WQL_DOUBLE_VALUE_TAG));
                break;

            case CIMTYPE_REAL64:
                Real64 propertyValueReal64;
                propertyValue.get(propertyValueReal64);
                source.addValue(propertyName.getString(),
                    WQLOperand(propertyValueReal64, WQL_DOUBLE_VALUE_TAG));
                break;

            case CIMTYPE_BOOLEAN :
                Boolean booleanValue;
                property.getValue().get(booleanValue);
                source.addValue(propertyName.getString(),
                    WQLOperand(booleanValue, WQL_BOOLEAN_VALUE_TAG));
                break;

            case CIMTYPE_CHAR16:
            case CIMTYPE_DATETIME :
            case CIMTYPE_STRING :
                source.addValue(propertyName.getString(),
                    WQLOperand(property.getValue().toString(),
                    WQL_STRING_VALUE_TAG));
                break;
            }
        }
    }

    PEG_METHOD_EXIT ();
    return source;
}

Boolean IndicationService::_getCreator (
    const CIMInstance & instance,
    String & creator) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_getCreator");

    Uint32 creatorIndex = instance.findProperty
        (PEGASUS_PROPERTYNAME_INDSUB_CREATOR);
    if (creatorIndex != PEG_NOT_FOUND)
    {
        CIMValue creatorValue = instance.getProperty
            (creatorIndex).getValue ();
        if (creatorValue.isNull ())
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                Tracer::LEVEL2,
                "Null Subscription Creator property value");

            //
            //  This is a corrupted/invalid instance
            //
            return false;
        }
        else if ((creatorValue.getType () != CIMTYPE_STRING) ||
            (creatorValue.isArray ()))
        {
            String traceString;
            if (creatorValue.isArray ())
            {
                traceString.append ("array of ");
            }
            traceString.append (cimTypeToString (creatorValue.getType ()));
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
               Tracer::LEVEL2,
               "Subscription Creator property value of incorrect type: "
               + traceString);

            //
            //  This is a corrupted/invalid instance
            //
            return false;
        }
        else
        {
            creatorValue.get (creator);
        }
    }
    else
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
            Tracer::LEVEL2,
            "Missing Subscription Creator property");

        //
        //  This is a corrupted/invalid instance
        //
        return false;
    }

    PEG_METHOD_EXIT ();
    return true;
}

Boolean IndicationService::_validateState (
    const Uint16 state) const
{
    //
    //  Validate the value
    //
    if (!Contains (_validStates, state))
    {
        //
        //  This is a corrupted/invalid instance
        //
        return false;
    }

    return true;
}

void IndicationService::_updatePropertyList
    (CIMName & className,
     CIMPropertyList & propertyList,
     Boolean & setTimeRemaining,
     Boolean & startTimeAdded,
     Boolean & durationAdded)
{
    PEG_METHOD_ENTER ( TRC_INDICATION_SERVICE,
        "IndicationService::_updatePropertyList");

    //
    //  A null propertyList means all properties
    //  If the class is Subscription, that includes the Time Remaining property
    //
    if (className.equal (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
    {
        setTimeRemaining = true;
    }
    else
    {
        setTimeRemaining = false;
    }
    startTimeAdded = false;
    durationAdded = false;
    if (!propertyList.isNull ())
    {
        setTimeRemaining = false;
        Array <CIMName> properties = propertyList.getPropertyNameArray ();

        //
        //  Add Creator to property list
        //
        if (!ContainsCIMName (properties,
            PEGASUS_PROPERTYNAME_INDSUB_CREATOR))
        {
            properties.append (PEGASUS_PROPERTYNAME_INDSUB_CREATOR);
        }

        //
        //  If a Subscription and Time Remaining is requested,
        //  Ensure Subscription Duration and Start Time are in property list
        //
        if (className.equal (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
        {
            if (ContainsCIMName (properties, _PROPERTY_TIMEREMAINING))
            {
                setTimeRemaining = true;
                if (!ContainsCIMName (properties, _PROPERTY_STARTTIME))
                {
                    properties.append (_PROPERTY_STARTTIME);
                    startTimeAdded = true;
                }
                if (!ContainsCIMName (properties, _PROPERTY_DURATION))
                {
                    properties.append (_PROPERTY_DURATION);
                    durationAdded = true;
                }
            }
        }
        propertyList.clear ();
        propertyList.set (properties);
    }

    PEG_METHOD_EXIT ();
}

String IndicationService::_getSubscriptionLogString
    (CIMInstance & subscription)
{
    //
    //  Get Subscription Filter Name and Handler Name
    //
    String logString;
    CIMValue filterValue;
    CIMObjectPath filterPath;
    Array <CIMKeyBinding> filterKeyBindings;
    CIMValue handlerValue;
    CIMObjectPath handlerPath;
    Array <CIMKeyBinding> handlerKeyBindings;
    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();
    filterValue.get (filterPath);
    filterKeyBindings = filterPath.getKeyBindings ();
    for (Uint32 i = 0; i < filterKeyBindings.size (); i++)
    {
        if (filterKeyBindings [i].getName ().equal (_PROPERTY_NAME))
        {
            logString.append (filterKeyBindings [i].getValue ());
            logString.append (", ");
            break;
        }
    }
    handlerValue = subscription.getProperty
        (subscription.findProperty
        (_PROPERTY_HANDLER)).getValue ();
    handlerValue.get (handlerPath);
    handlerKeyBindings = handlerPath.getKeyBindings ();
    for (Uint32 j = 0; j < handlerKeyBindings.size (); j++)
    {
        if (handlerKeyBindings [j].getName ().equal (_PROPERTY_NAME))
        {
            logString.append (handlerKeyBindings [j].getValue ());
            break;
        }
    }

    return logString;
}

String IndicationService::getProviderLogString
    (CIMInstance & provider)
{
    String logString;

    logString = provider.getProperty (provider.findProperty
        (_PROPERTY_NAME)).getValue ().toString ();

    return logString;
}

PEGASUS_NAMESPACE_END


