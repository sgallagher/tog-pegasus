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
// 		 Ben Heilbronn, Hewlett-Packard Company (ben_heilbronn@hp.com)
// 		 Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//               Dave Rosckes (rosckes@us.ibm.com)
// 		 Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/HashTable.h>
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
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/WQL/WQLSelectStatement.h>

#include "IndicationService.h"

// l10n
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/String.h>

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
         _repository (repository),
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
        // Initialize the Indication Service
        _initialize ();
    }
    catch (Exception & e)
    {
        //
        //  ATTN-CAKG-P3-20020425: Log a message
        //
    }
}

IndicationService::~IndicationService (void)
{
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
	   AcceptLanguages *langs = 
   			new AcceptLanguages(msg->acceptLanguages);	
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

      default:
	 //
	 //  A message type not supported by the Indication Service
	 //  Should not reach here
	 //
	 // << Mon Apr 29 16:29:10 2002 mdd >>
	 PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL3, 
			  "IndicationService::handleEnqueue(msg *) rcv'd unsupported msg " 
			  + String(MessageTypeToString(message->getType())));
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
#ifdef PEGASUS_INDICATION_PERFINST
    Stopwatch stopWatch;
#endif

    Array <CIMInstance> activeSubscriptions;
    Array <CIMInstance> noProviderSubscriptions;
    Array <ProviderClassList> enableProviders;
    Boolean duplicate;
    Boolean warningLogged = false;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_initialize");
 
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
    //  FUTURE: Add code to find repository service, if repository becomes a 
    //  service
    //

    //
    //  Set arrays of valid property values
    //
    _validStates.append (_STATE_UNKNOWN);
    _validStates.append (_STATE_OTHER);
    _validStates.append (_STATE_ENABLED);
    _validStates.append (_STATE_ENABLEDDEGRADED);
    _validStates.append (_STATE_DISABLED);
    _validRepeatPolicies.append (_POLICY_UNKNOWN);
    _validRepeatPolicies.append (_POLICY_OTHER);
    _validRepeatPolicies.append (_POLICY_NONE);
    _validRepeatPolicies.append (_POLICY_SUPPRESS);
    _validRepeatPolicies.append (_POLICY_DELAY);
    _validErrorPolicies.append (_ERRORPOLICY_OTHER);
    _validErrorPolicies.append (_ERRORPOLICY_IGNORE);
    _validErrorPolicies.append (_ERRORPOLICY_DISABLE);
    _validErrorPolicies.append (_ERRORPOLICY_REMOVE);
    _validPersistenceTypes.append (_PERSISTENCE_OTHER);
    _validPersistenceTypes.append (_PERSISTENCE_PERMANENT);
    _validPersistenceTypes.append (_PERSISTENCE_TRANSIENT);

    //
    //  Get existing active subscriptions from each namespace in the repository
    //
    warningLogged = _getActiveSubscriptionsFromRepository (activeSubscriptions);
    noProviderSubscriptions.clear ();

    String condition;
    String queryLanguage;
    CIMPropertyList propertyList;
    Array <ProviderClassList> indicationProviders;

    for (Uint32 i = 0; i < activeSubscriptions.size (); i++)
    {
        //
        //  Check for expired subscription
        //
        if (_isExpired (activeSubscriptions [i]))
        {
            CIMObjectPath path = activeSubscriptions [i].getPath ();
            _deleteExpiredSubscription (path);
                
            continue;
        }

        CIMNamespaceName sourceNameSpace;
        Array <CIMName> indicationSubclasses;
        _getCreateParams 
            (activeSubscriptions [i].getPath ().getNameSpace (),
            activeSubscriptions [i], indicationSubclasses, indicationProviders,
            propertyList, sourceNameSpace, condition, queryLanguage);

        if (indicationProviders.size () == 0)
        {
            //
            //  There are no providers that can support this subscription
            //  Implement the subscription's On Fatal Error Policy
            //  If subscription is not disabled or removed,
            //  Append this subscription to no provider list
            //
            if (!_handleError (activeSubscriptions [i]))
            {
                noProviderSubscriptions.append (activeSubscriptions [i]);
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
            //  Log a message and skip it
            //  L10N TODO DONE -- new log message
            //
            if (!warningLogged)
            {
                //Logger::put (Logger::STANDARD_LOG, System::CIMSERVER, 
                             //Logger::WARNING, _MSG_INVALID_INSTANCES);
				Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, 
                              Logger::WARNING, _MSG_INVALID_INSTANCES_KEY,
					  _MSG_INVALID_INSTANCES);
                warningLogged = true;
            }
            break;
        }

//l10n start            
	// Get the language tags that were saved with the subscription instance 
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

        _sendCreateRequests (indicationProviders, sourceNameSpace,
            propertyList, condition, queryLanguage,
            activeSubscriptions [i], 
            AcceptLanguages(acceptLangs), // l10n
            ContentLanguages(contentLangs),  // 110n
            0,  // initialize -- no request
            indicationSubclasses,
	    creator);

    }  // for each active subscription

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
    Array <CIMInstance> activeSubscriptions;
    CIMInstance indicationInstance;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_terminate");

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
        _checkNonprivilegedAuthorization(request->userName);

        if (_canCreate (instance, request->nameSpace))
        {

            //
            //  If the instance is of the PEGASUS_CLASSNAME_INDSUBSCRIPTION 
            //  class and subscription state is enabled, determine if any 
            //  providers can serve the subscription
            //
            Uint16 subscriptionState;
            String condition;
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
                        queryLanguage);

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
				 MessageLoaderParms(_MSG_NO_PROVIDERS_KEY, _MSG_NO_PROVIDERS));
                    }

                    //
                    //  Send Create request message to each provider
                    //
                    instanceRef = instance.getPath ();
                    instanceRef.setNameSpace (request->nameSpace);

                    CIMValue filterValue = instance.getProperty( 
                        instance.findProperty( _PROPERTY_FILTER ) ).getValue();
                    CIMValue handlerValue = instance.getProperty( 
                        instance.findProperty( _PROPERTY_HANDLER ) ).getValue();

                    Array< CIMKeyBinding > kb;
                    kb.append( CIMKeyBinding( _PROPERTY_FILTER, filterValue ) );
                    kb.append( CIMKeyBinding( _PROPERTY_HANDLER, handlerValue ) );

                    instanceRef.setKeyBindings( kb );

                    instance.setPath (instanceRef);
// l10n 
                    _sendCreateRequests (indicationProviders, 
                        sourceNameSpace, requiredProperties, condition, 
                        queryLanguage, instance,
                        request->acceptLanguages,
                        request->contentLanguages,   
                        request,
                        indicationSubclasses,
                        request->userName, request->authType);

                    //
                    //  Response is sent from _aggregationCallBack 
                    //
                    responseSent = true;
                }
                else
                {
                    //
                    //  Create instance for disabled subscription
                    //
                    instanceRef = _createInstance (request, instance, false);
                }
            }
            else 
            {
                //
                //  Create instance for filter or handler
                //
                instanceRef = _createInstance (request, instance, false);
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
    //  Send response, if it has not already been sent by callback
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

    _repository->read_lock ();

    try
    {
        _checkNonprivilegedAuthorization(request->userName);

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
        instance = _repository->getInstance (request->nameSpace, 
            request->instanceName, request->localOnly, 
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
	    	MessageLoaderParms parms(_MSG_INVALID_INSTANCES_KEY, _MSG_INVALID_INSTANCES);
	    	throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
							 
        }
        instance.removeProperty (instance.findProperty 
            (PEGASUS_PROPERTYNAME_INDSUB_CREATOR));

// l10n start
        //
        //  Remove the language properties from instance before returning
        //
	Uint32 propIndex = instance.findProperty(PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);      
        if (propIndex != PEG_NOT_FOUND)
        {
             instance.removeProperty (propIndex); 
        }   

	propIndex = instance.findProperty(PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);      
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
            _setTimeRemaining (instance);
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

    _repository->read_unlock ();

// l10n
    // Note: setting Content-Language in the response to the contentLanguage
    // in the repository.
    CIMGetInstanceResponseMessage* response = new CIMGetInstanceResponseMessage
        (request->messageId,
        cimException,
        request->queueIds.copyAndPop(),
        instance,
        ContentLanguages(contentLangs));

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


    _repository->read_lock ();

    try
    {
        _checkNonprivilegedAuthorization(request->userName);

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

        enumInstances = _repository->enumerateInstancesForClass
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
        //  Remove Creator and language properties from instances before returning
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
	    propIndex = enumInstances [i].findProperty(PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS);      
            String contentLangs = String::EMPTY;
            if (propIndex != PEG_NOT_FOUND)
            {
 	         enumInstances [i].getProperty(propIndex).getValue().get(contentLangs); 
                 enumInstances [i].removeProperty(propIndex); 
            }

            propIndex = enumInstances [i].findProperty(PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS);
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
                _setTimeRemaining (enumInstances [i]);
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

    _repository->read_unlock ();

// l10n
    // Note: setting Content-Language in the response to the aggregated
    // contentLanguage from the instances in the repository.
    CIMEnumerateInstancesResponseMessage* response = 
        new CIMEnumerateInstancesResponseMessage(
            request->messageId,
            cimException,
            request->queueIds.copyAndPop(),
            returnedInstances,
            ContentLanguages(aggregatedLangs));

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
    PEG_METHOD_ENTER (
        TRC_INDICATION_SERVICE,
        "IndicationService::_handleEnumerateInstancesNamesRequest");
   
    CIMEnumerateInstanceNamesRequestMessage* request =
        (CIMEnumerateInstanceNamesRequestMessage*) message;

    Array<CIMObjectPath> enumInstanceNames;

    CIMException cimException;

    _repository->read_lock ();

    try
    {
        _checkNonprivilegedAuthorization(request->userName);

        enumInstanceNames = _repository->enumerateInstanceNamesForClass
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

    _repository->read_unlock ();

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
        _checkNonprivilegedAuthorization(request->userName);

        //
        //  Get the instance name
        //
        CIMObjectPath instanceReference = 
            request->modifiedInstance.getPath ();
    
        //
        //  Get instance from repository
        //
        _repository->read_lock ();

        CIMInstance instance;

        try
        {
            instance = _repository->getInstance (request->nameSpace, 
                instanceReference);
        } 
        catch (Exception&)
        {
            _repository->read_unlock ();
            PEG_METHOD_EXIT ();
            throw;
        }

        _repository->read_unlock ();

        CIMInstance modifiedInstance = request->modifiedInstance;
        if (_canModify (request, instanceReference, instance, modifiedInstance))
        {
            //
            //  Check for expired subscription
            //
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
                if (!_getState (instance, currentState))
                {
                    //
                    //  This instance from the repository is corrupted
                    //  L10N TODO DONE -- new throw of exception
                    //
                    PEG_METHOD_EXIT ();
                    //throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, 
                        //_MSG_INVALID_INSTANCES);
					MessageLoaderParms parms(_MSG_INVALID_INSTANCES_KEY, _MSG_INVALID_INSTANCES);
		    		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
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
                AcceptLanguages acceptLangs = request->acceptLanguages;
                modifiedInstance.addProperty (CIMProperty 
                    (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS, 
                    acceptLangs.toString()));

	        ContentLanguages contentLangs = request->contentLanguages;
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
                        queryLanguage);
    
                    if (indicationProviders.size () == 0)
                    {
                        //
                        //  There are no providers that can support this 
                        //  subscription
                        //
                        instance.setPath (instanceReference);
                        _handleError (instance);
                        PEG_METHOD_EXIT ();

			// l10n

                        // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
			//  _MSG_NO_PROVIDERS);

			throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_NOT_SUPPORTED,
				 MessageLoaderParms(_MSG_NO_PROVIDERS_KEY, _MSG_NO_PROVIDERS));
                    }
                }

                //
                //  Modify the instance in the repository
                //
                _repository->write_lock ();

                try
                {
                    modifiedInstance.setPath (instanceReference);
                    _repository->modifyInstance (request->nameSpace,
                        modifiedInstance, 
                        request->includeQualifiers, propertyList);

		    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
				"IndicationService::_handleModifyInstanceRequest - Name Space: $0  Instance name: $1",
				request->nameSpace.getString(),
				modifiedInstance.getClassName().getString());
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

               _repository->write_unlock ();
    
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
                        queryLanguage,
                        instance,
                        request->acceptLanguages,
                        request->contentLanguages,  
                        request,
                        indicationSubclasses,
                        request->userName, request->authType);

                    //
                    //  Response is sent from _aggregationCallBack
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
	                    request->acceptLanguages,
    		            request->contentLanguages,  
                            request,
                            indicationSubclasses,
                            request->userName, request->authType);

                        //
                        //  Response is sent from _aggregationCallBack
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
    //  Send response, if it has not already been sent from callback
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
        _checkNonprivilegedAuthorization(request->userName);

        //
        //  Check if instance may be deleted -- a filter or handler instance 
        //  referenced by a subscription instance may not be deleted
        //
        if (_canDelete (request->instanceName, request->nameSpace,
            request->userName))
        {
            //
            //  If a subscription, get the instance from the repository
            //
            CIMInstance subscriptionInstance;
            if (request->instanceName.getClassName ().equal
                (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
            {
                _repository->read_lock ();

                try
                {
                subscriptionInstance = _repository->getInstance 
                    (request->nameSpace, request->instanceName);
                }
                catch (Exception&)
                {
                    _repository->read_unlock ();
                    PEG_METHOD_EXIT ();
                    throw;
                }

                _repository->read_unlock ();
            }

            //
            //  Delete instance from repository
            //
            _repository->write_lock ();

            try
            {
                _repository->deleteInstance (request->nameSpace, 
                    request->instanceName);
		
		Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
			    "IndicationService::_handleDeleteInstanceRequest - Name Space: $0  Instance name: $1",
			    request->nameSpace.getString(),
			    request->instanceName.getClassName().getString());
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

            _repository->write_unlock ();

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

                    //
                    //  Send Delete requests
                    //
// l10n                
                    _sendDeleteRequests (indicationProviders,
                        sourceNamespaceName, subscriptionInstance,
                        request->acceptLanguages,
                        request->contentLanguages,
                        request,
                        indicationSubclasses,
                        request->userName, request->authType);

                    //
                    //  Response is sent from _aggregationCallBack
                    //
                    responseSent = true;
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
    //  Send response, if it has not already been sent from callback
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
                String activeSubscriptionsKey = _generateActiveSubscriptionsKey
                    (request->subscriptionInstanceNames [i]);
                ActiveSubscriptionsTableEntry tableValue;
                if (_lockedLookupActiveSubscriptionsEntry(
                        activeSubscriptionsKey, 
                        tableValue))
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
            if (_isExpired (matchedSubscriptions [i]))
            {
                CIMObjectPath path = matchedSubscriptions [i].getPath ();
                _deleteExpiredSubscription (path);
    
                continue;
            }

            _getFilterProperties (
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
		
                handlerNamedInstance = _getHandler
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
                        String::EMPTY,
                        request->contentLanguages);
               
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
                    "Sending (SendWait) Indication to " + 
                    ((MessageQueue::lookup(_handlerService)) ? 
                    String(((MessageQueue::lookup
                    (_handlerService))->getQueueName())) : 
                    String("BAD queue name")) + 
                    "via CIMHandleIndicationRequestMessage");
		 		 
                AsyncReply *async_reply = SendWait(async_req);

                //
                //  ATTN-CAKG-P1-20020326: Check for error - implement 
                //  subscription's OnFatalErrorPolicy
                //

                response = 
                    reinterpret_cast<CIMProcessIndicationResponseMessage *>
                    ((static_cast<AsyncLegacyOperationResult *>
                    (async_reply))->get_result());

                //
                //  Recipient deletes request
                //

                delete async_reply;
            }
        }
    }
    catch (CIMException& exception)
    {
        response->cimException = exception;
    }
    catch (Exception& exception)
    {
        response->cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                       exception.getMessage());
    }

    _enqueueResponse(request, response);

    PEG_METHOD_EXIT ();

    return;
}

void IndicationService::_handleNotifyProviderRegistrationRequest
    (const Message* message)
{
    PEG_METHOD_ENTER (
        TRC_INDICATION_SERVICE,
        "IndicationService::_handleNotifyProviderRegistrationRequest");

    CIMNotifyProviderRegistrationRequestMessage* request = 
        (CIMNotifyProviderRegistrationRequestMessage*) message;

    CIMException cimException;

    CIMInstance provider = request->provider;
    CIMInstance providerModule = request->providerModule;
    CIMName className = request->className;
    Array <CIMNamespaceName> newNameSpaces = request->newNamespaces;
    Array <CIMNamespaceName> oldNameSpaces = request->oldNamespaces;
    CIMPropertyList newPropertyNames = request->newPropertyNames;
    CIMPropertyList oldPropertyNames = request->oldPropertyNames;

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
                requiredProperties, sourceNameSpace, condition, queryLanguage);

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
            //
            String activeSubscriptionsKey = _generateActiveSubscriptionsKey
                (newSubscriptions [i].getPath ());
            ActiveSubscriptionsTableEntry tableValue;
            if (_lockedLookupActiveSubscriptionsEntry (activeSubscriptionsKey,
                tableValue))
            {
                //
                //  If the provider is already in the subscription's list, 
                //  send a Modify request, otherwise send a Create request
                //
                Uint32 providerIndex = _providerInList (provider, tableValue);
                if (providerIndex != PEG_NOT_FOUND)
                {
                    //
                    //  Send Modify requests
                    //
// l10n
                    _sendModifyRequests (indicationProviders,
                        sourceNameSpace, 
                        requiredProperties, condition, queryLanguage,
                        newSubscriptions [i],
                        AcceptLanguages(acceptLangs),
                        ContentLanguages(contentLangs), 
                        request,
                        creator);
                }
                else
                {
                    //
                    //  Send Create requests
                    //
// l10n                
                    _sendCreateRequests (indicationProviders,
                        sourceNameSpace, requiredProperties, condition, 
                        queryLanguage, newSubscriptions [i], 
                        AcceptLanguages(acceptLangs), 
                        ContentLanguages(contentLangs),  
                        request,
                        indicationSubclasses,
                        creator);
                }
            }  // subscription found in table
            else
            {
                //
                //  Subscription not found in Active Subscriptions table
                //
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                    "Subscription (" + activeSubscriptionsKey +
                    ") not found in ActiveSubscriptionsTable");
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
        CIMClass providerClass = _repository->getClass
            (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER);
        CIMInstance providerCopy = provider.clone ();
        CIMObjectPath path = providerCopy.buildPath (providerClass);
        providerCopy.setPath (path);
        for (Uint32 j = 0; j < newSubscriptions.size (); j++)
        {
            //
            //  Get Provider Name, Subscription Filter Name and Handler Name
            //
            String logString1 = _getProviderLogString (providerCopy);
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
            String activeSubscriptionsKey = _generateActiveSubscriptionsKey
                (formerSubscriptions [i].getPath ());
            ActiveSubscriptionsTableEntry tableValue;
            if (_lockedLookupActiveSubscriptionsEntry (activeSubscriptionsKey,
                tableValue))
            {
                Uint32 providerIndex = _providerInList (provider, tableValue);
                if (providerIndex != PEG_NOT_FOUND)
                {
                    CIMNamespaceName sourceNameSpace;
                    Array <CIMName> indicationSubclasses;
                    _getCreateParams 
                        (formerSubscriptions [i].getPath ().getNameSpace (),
                        formerSubscriptions [i], indicationSubclasses,
                        requiredProperties, sourceNameSpace, condition, 
                        queryLanguage);

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
                    }
    
                    //
                    //  Otherwise, send a Modify request
                    //
                    else
                    {
                        Uint32 classIndex = _classInList (className, 
                            tableValue.providers [providerIndex]);
                        if (classIndex != PEG_NOT_FOUND)
                        {
                        //
                        //  Send Modify requests
                        //
// l10n                    
                        _sendModifyRequests (indicationProviders,
                            sourceNameSpace, 
                            requiredProperties, condition, queryLanguage, 
                            formerSubscriptions [i], 
                            AcceptLanguages(acceptLangs),
                            ContentLanguages(contentLangs),    
                            request,
                            creator);
                        }
                        else
                        {
                            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, 
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
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                    "Subscription (" + activeSubscriptionsKey +
                    ") not found in ActiveSubscriptionsTable");
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
        CIMClass providerClass = _repository->getClass
            (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER);
        CIMInstance providerCopy = provider.clone ();
        CIMObjectPath path = providerCopy.buildPath (providerClass);
        providerCopy.setPath (path);
        for (Uint32 j = 0; j < formerSubscriptions.size (); j++)
        {
            //
            //  Get Provider Name, Subscription Filter Name and Handler Name
            //
            String logString1 = _getProviderLogString (providerCopy);
            String logString2 = _getSubscriptionLogString 
                (formerSubscriptions [j]);

            Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER, 
                Logger::WARNING, _MSG_PROVIDER_NO_LONGER_SERVING_KEY, 
                _MSG_PROVIDER_NO_LONGER_SERVING, logString1, logString2);
        }
    }

    PEG_METHOD_EXIT ();
}

Uint32 IndicationService::_providerInList 
    (const CIMInstance & provider, 
     const ActiveSubscriptionsTableEntry & tableValue)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_providerInList");

    //
    //  Look for the provider in the list
    //
    for (Uint32 i = 0; i < tableValue.providers.size (); i++)
    {
        if (tableValue.providers [i].provider.identical (provider))
        {
            return i;
        }
    }

    return PEG_NOT_FOUND;

    PEG_METHOD_EXIT ();
}

Uint32 IndicationService::_classInList 
    (const CIMName & className, 
     const ProviderClassList & providerClasses)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_classInList");

    //
    //  Look for the class in the list
    //
    for (Uint32 i = 0; i < providerClasses.classList.size (); i++)
    {
        if (providerClasses.classList [i].equal (className))
        {
            return i;
        }
    }

    return PEG_NOT_FOUND;

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleNotifyProviderTerminationRequest
    (const Message * message)
{
    Array <CIMInstance> providerSubscriptions;
    CIMInstance indicationInstance;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_handleNotifyProviderTermination");

    CIMNotifyProviderTerminationRequestMessage* request = 
	(CIMNotifyProviderTerminationRequestMessage*) message;

    Array <CIMInstance> providers = request->providers;

    for (Uint32 i = 0; i < providers.size (); i++)
    {
        //
        //  Get list of affected subscriptions
        //
        //  _getProviderSubscriptions also updates the Active Subscriptions 
        //  hash table, and implements each subscription's On Fatal Error 
        //  policy, if necessary
        //
        providerSubscriptions.clear ();
        providerSubscriptions = _getProviderSubscriptions (providers [i]);
    
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
            CIMClass providerClass = _repository->getClass
                (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER);
            CIMInstance providerCopy = providers [i].clone ();
            CIMObjectPath path = providerCopy.buildPath (providerClass);
            providerCopy.setPath (path);
            for (Uint32 j = 0; j < providerSubscriptions.size (); j++)
            {
                //
                //  Get Provider Name, Subscription Filter Name and Handler Name
                //
                String logString1 = _getProviderLogString (providerCopy);
                String logString2 = _getSubscriptionLogString 
                    (providerSubscriptions [j]);

                Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER, 
                    Logger::WARNING, _MSG_PROVIDER_NO_LONGER_SERVING_KEY, 
                    _MSG_PROVIDER_NO_LONGER_SERVING, logString1, logString2);
            }
        }
    }

    PEG_METHOD_EXIT ();
}

Boolean IndicationService::_handleError (
    const CIMInstance subscription)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_handleError");

    Boolean handleError = false;

    //
    //  Get the value of the On Fatal Error Policy property
    //
    CIMValue errorPolicyValue;
    Uint16 onFatalErrorPolicy;
    errorPolicyValue = subscription.getProperty 
        (subscription.findProperty 
        (_PROPERTY_ONFATALERRORPOLICY)).getValue ();
    errorPolicyValue.get (onFatalErrorPolicy);

    if (errorPolicyValue == _ERRORPOLICY_DISABLE)
    {
        //
        //  FUTURE: Failure Trigger Time Interval should be allowed to pass 
        //  before implementing On Fatal Error Policy
        //
        //  Set the Subscription State to disabled
        //
        _disableSubscription (subscription);
        handleError = true;
    }
    else if (errorPolicyValue == _ERRORPOLICY_REMOVE)
    {
        //
        //  FUTURE: Failure Trigger Time Interval should be allowed to pass 
        //  before implementing On Fatal Error Policy
        //
        //  Delete the subscription
        //
        _deleteSubscription (subscription);
        handleError = true;
    }

    PEG_METHOD_EXIT ();
    return handleError;
}

void IndicationService::_disableSubscription (
    CIMInstance subscription)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_disableSubscription");

    //
    //  Create property list
    //
    CIMPropertyList propertyList;
    Array <CIMName> properties;
    properties.append (_PROPERTY_STATE);
    propertyList = CIMPropertyList (properties);

    //
    //  Set Time of Last State Change to current date time
    //
    CIMInstance instance = subscription;
    CIMDateTime currentDateTime = CIMDateTime::getCurrentDateTime ();
    if (instance.findProperty (_PROPERTY_LASTCHANGE) == PEG_NOT_FOUND)
    {
        instance.addProperty 
            (CIMProperty (_PROPERTY_LASTCHANGE, currentDateTime));
    }
    else 
    {
        CIMProperty lastChange = instance.getProperty 
            (instance.findProperty (_PROPERTY_LASTCHANGE));
        lastChange.setValue (CIMValue (currentDateTime));
    }

    //
    //  Set Subscription State to Disabled
    //
    CIMProperty state = instance.getProperty (instance.findProperty 
        (_PROPERTY_STATE));
    state.setValue (CIMValue (_STATE_DISABLED));

    //
    //  Modify the instance in the repository
    //
    _repository->write_lock ();
   
    try
    {
        _repository->modifyInstance 
            (subscription.getPath ().getNameSpace (),
            subscription, false, propertyList);
    }
    catch (Exception & exception)
    {
        //
        //  ATTN-CAKG-P3-20020425: Log a message
        //
    }

    _repository->write_unlock ();

    PEG_METHOD_EXIT ();
}

void IndicationService::_deleteSubscription (
    const CIMInstance subscription)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_deleteSubscription");

    //
    //  Delete referencing subscription instance from repository
    //
    _repository->write_lock ();

    try
    {
        _repository->deleteInstance 
            (subscription.getPath ().getNameSpace (), 
            subscription.getPath ());
    }
    catch (Exception & exception)
    {
        //
        //  ATTN-CAKG-P3-20020425: Log a message
        //
    }

    _repository->write_unlock ();

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
        _checkRequiredProperty (instance, _PROPERTY_FILTER, _MSG_KEY_PROPERTY);
        _checkRequiredProperty (instance, _PROPERTY_HANDLER, _MSG_KEY_PROPERTY);

        //
        //  Subscription State, Repeat Notificastion Policy, and On Fatal Error
        //  Policy properties each has a default value, a corresponding 
        //  Other___ property, and a set of valid values
        //
        _checkPropertyWithOther (instance, _PROPERTY_STATE, 
            _PROPERTY_OTHERSTATE, (Uint16) _STATE_ENABLED, 
            (Uint16) _STATE_OTHER, _validStates);

        _checkPropertyWithOther (instance, _PROPERTY_REPEATNOTIFICATIONPOLICY,
            _PROPERTY_OTHERREPEATNOTIFICATIONPOLICY, (Uint16) _POLICY_NONE,
            (Uint16) _POLICY_OTHER, _validRepeatPolicies);

        _checkPropertyWithOther (instance, _PROPERTY_ONFATALERRORPOLICY, 
            _PROPERTY_OTHERONFATALERRORPOLICY, (Uint16) _ERRORPOLICY_IGNORE, 
            (Uint16) _ERRORPOLICY_OTHER, _validErrorPolicies);
    } 
    else  // Filter or Handler
    {
        //
        //  Name, CreationClassName, SystemName, and SystemCreationClassName
        //  are key properties for Filter and Handler  
        //  CreationClassName and Name must exist
        //  If others do not exist, add and set to default
        //
        _checkRequiredProperty (instance, _PROPERTY_NAME, _MSG_KEY_PROPERTY);
        _checkRequiredProperty (instance, _PROPERTY_CREATIONCLASSNAME, 
            _MSG_KEY_PROPERTY);

        _checkPropertyWithDefault (instance, _PROPERTY_SYSTEMNAME, 
            System::getFullyQualifiedHostName ());

        _checkPropertyWithDefault (instance, _PROPERTY_SYSTEMCREATIONCLASSNAME, 
            System::getSystemCreationClassName ());

        if (instance.getClassName ().equal (PEGASUS_CLASSNAME_INDFILTER))
        {
            //
            //  Query and QueryLanguage properties are required for Filter
            //
            _checkRequiredProperty (instance, _PROPERTY_QUERY, _MSG_PROPERTY);
            _checkRequiredProperty (instance, _PROPERTY_QUERYLANGUAGE, 
                _MSG_PROPERTY);

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
        }

        //
        //  Currently only two direct subclasses of Indication handler 
        //  class are supported -- further subclassing is not currently 
        //  supported
        //
        else if ((instance.getClassName ().equal 
                  (PEGASUS_CLASSNAME_INDHANDLER_CIMXML)) ||
                 (instance.getClassName ().equal
                  (PEGASUS_CLASSNAME_INDHANDLER_SNMP)))
        {
            _checkPropertyWithOther (instance, _PROPERTY_PERSISTENCETYPE,
                _PROPERTY_OTHERPERSISTENCETYPE, (Uint16) _PERSISTENCE_PERMANENT,
                (Uint16) _PERSISTENCE_OTHER, _validPersistenceTypes);

            if (instance.getClassName ().equal 
                (PEGASUS_CLASSNAME_INDHANDLER_CIMXML))
            {
                //
                //  Destination property is required for CIMXML 
                //  Handler subclass
                //
                _checkRequiredProperty (instance, _PROPERTY_DESTINATION, 
                    _MSG_PROPERTY);
            }

            if (instance.getClassName ().equal 
                (PEGASUS_CLASSNAME_INDHANDLER_SNMP))
            {
		//
                //  TargetHost property is required for SNMP
                //  Handler subclass
                //
                _checkRequiredProperty (instance, _PROPERTY_TARGETHOST,
                    _MSG_PROPERTY);

                //
                //  TargetHostFormat property is required for SNMP
                //  Handler subclass
                //
                _checkRequiredProperty (instance, _PROPERTY_TARGETHOSTFORMAT,
                    _MSG_PROPERTY);

                //
                //  SNMPVersion property is required for SNMP Handler
                //
                _checkRequiredProperty (instance, _PROPERTY_SNMPVERSION,
                    _MSG_PROPERTY);
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
    }

    if (missingProperty)
    {

      // l10n

      String exceptionStr = _MSG_MISSING_REQUIRED;
      // exceptionStr.append (propertyName.getString());
      exceptionStr.append ("$0");
      exceptionStr.append (message);

      String message_key;
      if (strcmp(message.getCString(), _MSG_KEY_PROPERTY) == 0) {
	message_key = _MSG_KEY_PROPERTY_KEY;
      } else if (strcmp(message.getCString(), _MSG_PROPERTY) == 0) {
	message_key = _MSG_PROPERTY_KEY;
      } else {
	message_key = String("");
      }

      PEG_METHOD_EXIT ();
      
      // l10n

      // throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
      //		   exceptionStr);

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
    const Array <Uint16> & validValues)
{
    Uint16 result = defaultValue;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_checkPropertyWithOther");

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
            else{
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
                    MessageLoaderParms(_MSG_INVALID_VALUE_FOR_PROPERTY_KEY, exceptionStr,
				       theValue.toString(), propertyName.getString()));
            }
        }

        //
        //  If the value is Other, the Other
        //  property must exist and value must not be NULL
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
	      //		   exceptionStr);

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
    String result = defaultValue;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_checkPropertyWithDefault");

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
        else
        {
            theValue.get (result);
        }
    }

    return result;

    PEG_METHOD_EXIT ();
}

CIMObjectPath IndicationService::_createInstance (
    CIMCreateInstanceRequestMessage * request,
    CIMInstance instance,
    Boolean enabled)
{
    CIMObjectPath instanceRef;
    CIMException cimException;

    //
    //  Add creator property to Instance
    //  NOTE: userName is only set in the request if authentication 
    //  is turned on
    //
    String currentUser = request->userName;
    if (instance.findProperty (PEGASUS_PROPERTYNAME_INDSUB_CREATOR) == 
        PEG_NOT_FOUND)
    {
        instance.addProperty (CIMProperty 
            (PEGASUS_PROPERTYNAME_INDSUB_CREATOR, currentUser));
    }
    else 
    {
        CIMProperty creator = instance.getProperty 
            (instance.findProperty 
            (PEGASUS_PROPERTYNAME_INDSUB_CREATOR));
        creator.setValue (CIMValue (currentUser));
    }
    
    // l10n
    // Add the language properties to the Instance
    // Note:  These came from the Accept-Language and Content-Language
    // headers in the HTTP messages, and may be empty.
    AcceptLanguages acceptLangs = request->acceptLanguages;
    if (instance.findProperty (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS) == 
        PEG_NOT_FOUND)
    {
        instance.addProperty (CIMProperty 
            (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS, 
            acceptLangs.toString()));
    }
    else 
    {
        CIMProperty langs = instance.getProperty 
            (instance.findProperty 
            (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS));
        langs.setValue (CIMValue (acceptLangs.toString()));
    } 

    ContentLanguages contentLangs = request->contentLanguages;
    if (instance.findProperty (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS) == 
        PEG_NOT_FOUND)
    {
        instance.addProperty (CIMProperty 
            (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS, 
            contentLangs.toString()));
    }
    else 
    {
        CIMProperty langs = instance.getProperty 
            (instance.findProperty 
            (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS));
        langs.setValue (CIMValue (contentLangs.toString()));
    }                                   
    // l10n -end

    if (instance.getClassName ().equal 
        (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
    {
        //
        //  Set Time of Last State Change to current date time
        //
        CIMDateTime currentDateTime = 
            CIMDateTime::getCurrentDateTime ();
        if (instance.findProperty (_PROPERTY_LASTCHANGE) == 
            PEG_NOT_FOUND)
        {
            instance.addProperty 
                (CIMProperty (_PROPERTY_LASTCHANGE, currentDateTime));
        }
        else 
        {
            CIMProperty lastChange = instance.getProperty 
                (instance.findProperty (_PROPERTY_LASTCHANGE));
            lastChange.setValue (CIMValue (currentDateTime));
        }
    
        CIMDateTime startDateTime;
        if (enabled)
        {
            startDateTime = currentDateTime;
        }
        else
        {
            //
            //  If subscription is not enabled, set Subscription
            //  Start Time to null CIMDateTime value
            //
            startDateTime = CIMDateTime ();
        }

        //
        //  Set Subscription Start Time
        //
        if (instance.findProperty (_PROPERTY_STARTTIME) == 
            PEG_NOT_FOUND)
        {
            instance.addProperty 
                (CIMProperty (_PROPERTY_STARTTIME, startDateTime));
        }
        else 
        {
            CIMProperty startTime = instance.getProperty 
                (instance.findProperty (_PROPERTY_STARTTIME));
            startTime.setValue (CIMValue (startDateTime));
        }
    }

    //
    //  Create instance in repository
    //
    _repository->write_lock ();

    try 
    {
        instanceRef = _repository->createInstance (
            request->nameSpace, 
            instance);

        Logger::put (
            Logger::STANDARD_LOG, 
            System::CIMSERVER, 
            Logger::TRACE,
            "IndicationService::_handleCreateInstanceRequest - Name Space: $0  Instance name: $1",
            request->nameSpace.getString (),
            instance.getClassName ().getString ());
    }
    catch (CIMException & exception)
    {
        _repository->write_unlock ();
        cimException = exception;
        throw cimException;
    }
    catch (Exception & exception)
    {
        _repository->write_unlock ();
        cimException = PEGASUS_CIM_EXCEPTION 
            (CIM_ERR_FAILED, exception.getMessage ());
        throw cimException;
    }

    _repository->write_unlock ();
    return instanceRef;
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
        _validStates);

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
		MessageLoaderParms parms(_MSG_INVALID_INSTANCES_KEY, _MSG_INVALID_INSTANCES);
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
    }

    //
    //  Current user must be privileged user or instance Creator to modify
    //  NOTE: if authentication was not turned on when instance was created,
    //  instance creator will be String::EMPTY
    //  If creator is String::EMPTY, anyone may modify or delete the 
    //  instance
    //
    String currentUser = request->userName;
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
    CIMName superClass;
    CIMName propName;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_canDelete");

    //
    //  Get the instance to be deleted from the respository
    //
    CIMInstance instance;

    _repository->read_lock ();

    try
    {
        instance = _repository->getInstance (nameSpace, instanceReference);
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

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
    _repository->read_lock ();

    try
    {
        refClass = _repository->getClass (nameSpace,
            instanceReference.getClassName ());
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

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
            if (_isTransient (nameSpace, instanceReference))
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
        Array <CIMInstance> subscriptions = _getSubscriptions (nameSpace);

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


Boolean IndicationService::_getActiveSubscriptionsFromRepository (
    Array <CIMInstance> & activeSubscriptions) const
{
    Array <CIMNamespaceName> nameSpaceNames;
    Array <CIMInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;
    Boolean warningLogged = false;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_getActiveSubscriptionsFromRepository");

    activeSubscriptions.clear ();

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _getNameSpaceNames ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint32 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        subscriptions = _getSubscriptions (nameSpaceNames [i]);

        //
        //  Process each subscription
        //
        for (Uint32 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            if (!_getState (subscriptions [j], subscriptionState))
            {
                //
                //  This instance from the repository is corrupted
                //  Skip it
                //  L10N TODO DONE -- new log message
                //
                if (!warningLogged)
                {
                    //Logger::put (Logger::STANDARD_LOG, System::CIMSERVER, 
                                 //Logger::WARNING, _MSG_INVALID_INSTANCES);
                    Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, 
                                 Logger::WARNING, 
                                 _MSG_INVALID_INSTANCES_KEY,
                                 _MSG_INVALID_INSTANCES);
                    warningLogged = true;
                }
                break;
            }

            //
            //  Process each enabled subscription
            //
            if ((subscriptionState == _STATE_ENABLED) ||
                (subscriptionState == _STATE_ENABLEDDEGRADED))
            {
                //
                //  CIMInstances returned from repository do not include 
                //  namespace
                //  Set namespace here
                //
                CIMObjectPath instanceName = 
                    subscriptions [j].getPath ();
                instanceName.setNameSpace (nameSpaceNames [i]);
                subscriptions [j].setPath (instanceName);
                activeSubscriptions.append (subscriptions [j]);
            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

    PEG_METHOD_EXIT ();
    return warningLogged;
}

Array <CIMInstance> IndicationService::_getActiveSubscriptions ()
{
    Array <CIMInstance> activeSubscriptions;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getActiveSubscriptions");

    // Do not call any other methods that need _activeSubscriptionsTableLock
    ReadLock lock(_activeSubscriptionsTableLock);

    //
    //  Iterate through the subscription table
    //
    for (ActiveSubscriptionsTable::Iterator i =
        _activeSubscriptionsTable.start (); i; i++)
    {
        //
        //  Append subscription to the list
        //
        activeSubscriptions.append (i.value ().subscription);
    }

    PEG_METHOD_EXIT ();
    return activeSubscriptions;
}


Array <CIMInstance> IndicationService::_getMatchingSubscriptions (
    const CIMName & supportedClass,
    const Array <CIMNamespaceName> nameSpaces,
    const CIMPropertyList & supportedProperties,
    const Boolean checkProvider,
    const CIMInstance & provider) 
{
    Array <CIMInstance> matchingSubscriptions;
    Array <CIMInstance> subscriptions;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getMatchingSubscriptions");

    for (Uint32 i = 0; i < nameSpaces.size (); i++)
    {
        //
        //  Look up the indicationClass-sourceNamespace pair in the 
        //  Subscription Classes table
        //
        String subscriptionClassesKey = _generateSubscriptionClassesKey 
            (supportedClass, nameSpaces [i]);
        SubscriptionClassesTableEntry tableValue;
        if (_lockedLookupSubscriptionClassesEntry (subscriptionClassesKey, 
            tableValue))
        {
            subscriptions = tableValue.subscriptions;
            for (Uint32 j = 0; j < subscriptions.size (); j++)
            {
                Boolean match = true;

                if (checkProvider)
                {
                    //
                    //  Check if the provider who generated this indication 
                    //  accepted this subscription
                    //
                    String activeSubscriptionsKey = 
                        _generateActiveSubscriptionsKey
                        (subscriptions [j].getPath ());
                    ActiveSubscriptionsTableEntry tableValue;
                    if (_lockedLookupActiveSubscriptionsEntry 
                        (activeSubscriptionsKey, tableValue))
                    {
                        //
                        //  If provider is not in list, it did not accept the
                        //  subscription
                        //
                        if ((_providerInList (provider, tableValue)) == 
                            PEG_NOT_FOUND)
                        {
                            match = false;
                            break;
                        }
                    }
                }

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
                    _getFilterProperties (subscriptions [j],
                        nameSpaces [i], filterQuery, sourceNameSpace);
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
                        for (Uint32 k = 0; 
                             k < propertyList.size () && match; 
                             k++)
                        {
                            if (!ContainsCIMName
                                (supportedProperties.getPropertyNameArray(),
                                propertyList[k]))
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
                    matchingSubscriptions.append (subscriptions [j]);
                }
            }
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
    Array <CIMInstance> newList;
    Array <CIMInstance> formerList;
    Array <CIMInstance> bothList;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getModifiedSubscriptions");

    newSubscriptions.clear ();
    formerSubscriptions.clear ();

    //
    //  For each newly supported namespace, lookup to retrieve list of 
    //  subscriptions for the indication class-source namespace pair
    //
    for (Uint32 i = 0; i < newNameSpaces.size (); i++)
    {
        //
        //  Look up the indicationClass-sourceNamespace pair in the
        //  Subscription Classes table
        //
        String subscriptionClassesKey = _generateSubscriptionClassesKey
            (supportedClass, newNameSpaces [i]);
        SubscriptionClassesTableEntry tableValue;
        if (_lockedLookupSubscriptionClassesEntry (subscriptionClassesKey,
            tableValue))
        {
            for (Uint32 j = 0; j < tableValue.subscriptions.size (); j++)
                newList.append (tableValue.subscriptions [j]);
        }
    }

    //
    //  For each formerly supported namespace, lookup to retrieve list of 
    //  subscriptions for the indication class-source namespace pair
    //
    for (Uint32 k = 0; k < oldNameSpaces.size (); k++)
    {
        //
        //  Look up the indicationClass-sourceNamespace pair in the
        //  Subscription Classes table
        //
        String subscriptionClassesKey = _generateSubscriptionClassesKey
            (supportedClass, oldNameSpaces [k]);
        SubscriptionClassesTableEntry tableValue;
        if (_lockedLookupSubscriptionClassesEntry (subscriptionClassesKey,
            tableValue))
        {
            for (Uint32 m = 0; m < tableValue.subscriptions.size (); m++)
                formerList.append (tableValue.subscriptions [m]);
        }
    }

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
        _getFilterProperties (newList [n],
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
        _getFilterProperties (bothList [b],
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

Array <CIMNamespaceName> IndicationService::_getNameSpaceNames (void) const
{
    Array <CIMNamespaceName> nameSpaceNames;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getNameSpaceNames");

    _repository->read_lock ();

    try
    {
        nameSpaceNames = _repository->enumerateNameSpaces ();
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

    PEG_METHOD_EXIT ();
    return nameSpaceNames;
}

Array <CIMInstance> IndicationService::_getSubscriptions (
    const CIMNamespaceName & nameSpaceName) const
{
    Array <CIMInstance> subscriptions;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getSubscriptions");

    //
    //  Get existing subscriptions in current namespace
    //
    _repository->read_lock ();

    try
    {
        subscriptions = _repository->enumerateInstances
            (nameSpaceName, PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    }
    catch (CIMException& e)
    {
        //
        //  Some namespaces may not include the subscription class
        //  In that case, just return no subscriptions
        //  Any other exception is an error
        //
        if (e.getCode () != CIM_ERR_INVALID_CLASS)
        {
            _repository->read_unlock ();
            PEG_METHOD_EXIT ();
            throw e;
        }
    }

    _repository->read_unlock ();

    PEG_METHOD_EXIT ();
    return subscriptions;
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
                if (!ContainsCIMName (supportedProperties.getPropertyNameArray (), 
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

Array <CIMInstance> IndicationService::_getProviderSubscriptions (
    const CIMInstance & provider)
{
    Array <CIMInstance> providerSubscriptions;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getProviderSubscriptions");

    // Do not call any other methods that need _activeSubscriptionsTableLock
    WriteLock lock(_activeSubscriptionsTableLock);

    //
    //  Iterate through the subscription table
    //
    for (ActiveSubscriptionsTable::Iterator i =
        _activeSubscriptionsTable.start (); i; i++)
    {
        //
        //  If provider matches, append subscription to the list
        //
        for (Uint32 j = 0; j < i.value ().providers.size (); j++)
        {
            ActiveSubscriptionsTableEntry tableValue = i.value ();
            if (tableValue.providers [j].provider.identical (provider))
            {
                //
                //  Add the subscription to the list
                //
                providerSubscriptions.append (tableValue.subscription);

                //
                //  Remove the provider from the list of providers serving the 
                //  subscription
                //
                tableValue.providers.remove (j);
                _removeActiveSubscriptionsEntry (i.key ());

                if (tableValue.providers.size () > 0)
                {
                    //
                    //  Insert updated entry into Active Subscriptions table
                    //
                    _insertActiveSubscriptionsEntry (tableValue.subscription,
                        tableValue.providers);
                }
                else
                {
                    //
                    //  If the terminated provider was the only provider serving
                    //  the subscription, implement the subscription's On Fatal 
                    //  Error Policy
                    //
                    _handleError (tableValue.subscription);
                }
            }
        }
    }

    PEG_METHOD_EXIT ();
    return providerSubscriptions;
}

Boolean IndicationService::_providerInUse (
    const CIMInstance & provider)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_providerInUse");

    //
    // Do not call any other methods that need _activeSubscriptionsTableLock
    //
    ReadLock lock (_activeSubscriptionsTableLock);

    //
    //  Iterate through the subscription table
    //
    for (ActiveSubscriptionsTable::Iterator i =
        _activeSubscriptionsTable.start (); i; i++)
    {
        //
        //  If provider matches, return true
        //
        for (Uint32 j = 0; j < i.value ().providers.size (); j++)
        {
            ActiveSubscriptionsTableEntry tableValue = i.value ();
            if (tableValue.providers [j].provider.identical (provider))
            {
                return true;
            }
        }
    }

    PEG_METHOD_EXIT ();
    return false;
}

void IndicationService::_getFilterProperties (
    const CIMInstance & subscription,
    const CIMNamespaceName & nameSpaceName, 
    String & query,
    CIMNamespaceName & sourceNameSpace,
    String & queryLanguage) 
{
    CIMValue filterValue;
    CIMObjectPath filterReference;
    CIMInstance filterInstance;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getFilterProperties");

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    _repository->read_lock ();

    try
    {
        filterInstance = _repository->getInstance (nameSpaceName, 
            filterReference);
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

    query = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_QUERY)).getValue ().toString ();

    sourceNameSpace = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_SOURCENAMESPACE)).getValue ().toString ();

    queryLanguage = filterInstance.getProperty
        (filterInstance.findProperty (_PROPERTY_QUERYLANGUAGE)).
        getValue ().toString ();

    PEG_METHOD_EXIT ();
}

void IndicationService::_getFilterProperties (
    const CIMInstance & subscription,
    const CIMNamespaceName & nameSpaceName, 
    String & query,
    CIMNamespaceName & sourceNameSpace) 
{
    CIMValue filterValue;
    CIMObjectPath filterReference;
    CIMInstance filterInstance;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getFilterProperties");

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    _repository->read_lock ();

    try
    {
        filterInstance = _repository->getInstance (nameSpaceName, 
            filterReference);
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

    query = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_QUERY)).getValue ().toString ();

    sourceNameSpace = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_SOURCENAMESPACE)).getValue ().toString ();

    PEG_METHOD_EXIT ();
}

void IndicationService::_getFilterProperties (
    const CIMInstance & subscription,
    const CIMNamespaceName & nameSpaceName, 
    String & query) 
{
    CIMValue filterValue;
    CIMObjectPath filterReference;
    CIMInstance filterInstance;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getFilterProperties");

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    _repository->read_lock ();

    try
    {
        filterInstance = _repository->getInstance (nameSpaceName, 
            filterReference);
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

    query = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_QUERY)).getValue ().toString ();

    PEG_METHOD_EXIT ();
}

WQLSelectStatement IndicationService::_getSelectStatement (
    const String & filterQuery) const
{
    WQLSelectStatement selectStatement;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getSelectStatement");

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
    CIMName indicationClassName;
    Array <CIMName> indicationSubclasses;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getIndicationClassName");

    indicationClassName = selectStatement.getClassName ();

    //
    //  Validate that class is an Indication class
    //  The Indication Qualifier should exist and have the value True
    //
    Boolean validClass = false;
    CIMClass theClass;
    _repository->read_lock ();

    try
    {
        theClass = _repository->getClass (
            nameSpaceName,
            indicationClassName,
            false,
            true,
            false,
            CIMPropertyList());
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

    if (theClass.findQualifier (_QUALIFIER_INDICATION) != PEG_NOT_FOUND)
    {
        CIMQualifier theQual = theClass.getQualifier (theClass.findQualifier 
            (_QUALIFIER_INDICATION));
        CIMValue theVal = theQual.getValue ();
        if (!theVal.isNull ())
        {
            Boolean indicationClass;
            theVal.get (indicationClass);
            validClass = indicationClass;
        }
    }

    if (!validClass)
    {
      // l10n

        // String exceptionStr = _MSG_INVALID_CLASSNAME;
	    String exceptionStr = String("Invalid indication class name ");
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
				          _MSG_INVALID_CLASSNAME_IN_FROM_PROPERTY_KEY,
				          exceptionStr, 
				          indicationClassName.getString(), 
				          PEGASUS_CLASSNAME_INDFILTER.getString(), 
				          _PROPERTY_QUERY.getString()));
    }

    PEG_METHOD_EXIT ();
    return indicationClassName;
}

Array <CIMName> IndicationService::_getIndicationSubclasses (
        const CIMNamespaceName & nameSpace,
        const CIMName & indicationClassName) const
{
    Array <CIMName> indicationSubclasses;

    const char METHOD_NAME [] = "IndicationService::_getIndicationSubclasses";

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getIndicationSubclasses");

    _repository->read_lock ();

    try
    {
        indicationSubclasses = _repository->enumerateClassNames
            (nameSpace, indicationClassName, true);
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

    indicationSubclasses.append (indicationClassName);

    PEG_METHOD_EXIT ();
    return indicationSubclasses;
}

Array <ProviderClassList> 
    IndicationService::_getIndicationProviders (
        const CIMNamespaceName & nameSpace,
        const CIMName & indicationClassName,
        const Array <CIMName> & indicationSubclasses,
        const CIMPropertyList & requiredPropertyList) const
{
    ProviderClassList provider;
    Array <ProviderClassList> indicationProviders;
    Array <CIMInstance> providerInstances;
    Array <CIMInstance> providerModuleInstances;
    Boolean duplicate = false;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getIndicationProviders");

    //
    //  For each indication subclass, get providers
    //
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
            
            //
            //  Merge into list of ProviderClassList structs 
            //
            for (Uint32 j = 0; j < providerInstances.size () && !duplicate; j++)
            {
                provider.classList.clear ();
                duplicate = false;

                //
                //  See if indication provider is already in list
                //
                for (Uint32 k = 0; 
                     k < indicationProviders.size () && !duplicate; k++)
                {
                    if ((providerInstances [j].identical 
                        (indicationProviders [k].provider)) &&
                        (providerModuleInstances [j].identical
                        (indicationProviders [k].providerModule)))
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
    CIMPropertyList propertyList;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getPropertyList");

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
    _repository->read_lock ();

    try
    {
        //
        //  Get the indication class object from the repository
        //  Specify localOnly=false because superclass properties are needed
        //  Specify includeQualifiers=false because qualifiers are not needed
        //
        indicationClass = _repository->getClass (nameSpaceName, 
            indicationClassName, false, false);
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

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
    String condition = String::EMPTY;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getCondition");

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


CIMInstance IndicationService::_getHandler (
    const CIMInstance & subscription) const
{
    CIMValue handlerValue;
    CIMObjectPath handlerRef;
    CIMInstance handlerInstance;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_getHandler");

    //
    //  Get Handler reference from subscription instance
    //
    handlerValue = subscription.getProperty 
        (subscription.findProperty
        (_PROPERTY_HANDLER)).getValue ();

    handlerValue.get (handlerRef);

    //
    //  Get Handler instance from the repository
    //
    _repository->read_lock ();

    try
    {
        handlerInstance = _repository->getInstance 
            (subscription.getPath ().getNameSpace (), handlerRef);
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

    //
    //  Set namespace in path in CIMInstance
    //
    handlerRef.setNameSpace 
        (subscription.getPath ().getNameSpace ());
    handlerInstance.setPath (handlerRef);

    PEG_METHOD_EXIT ();
    return handlerInstance;
}

Boolean IndicationService::_isTransient (
    const CIMNamespaceName & nameSpace,
    const CIMObjectPath & handler) const
{
    CIMValue persistenceValue;
    Uint16 persistenceType;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_isTransient");

    //
    //  Get the handler instance from the respository
    //
    CIMInstance instance;
    _repository->read_lock ();

    try
    {
        instance = _repository->getInstance (nameSpace, handler);
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

    //
    //  Get Persistence Type
    //
    persistenceValue = instance.getProperty (instance.findProperty
        (_PROPERTY_PERSISTENCETYPE)).getValue ();
    persistenceValue.get (persistenceType);

    //
    //  If Persistence Type is Other, Other Persistence Type
    //  property must exist
    //
    if (persistenceType == _PERSISTENCE_TRANSIENT)
    {
        PEG_METHOD_EXIT ();
        return true;
    }
    else
    {
        PEG_METHOD_EXIT ();
        return false;
    }
}

void IndicationService::_deleteReferencingSubscriptions (
    const CIMNamespaceName & nameSpace,
    const CIMName & referenceProperty,
    const CIMObjectPath & handler)
{
    Array <CIMInstance> subscriptions;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_deleteReferencingSubscriptions");

    //
    //  Get existing subscriptions in the namespace
    //
    subscriptions = _getSubscriptions (nameSpace);

    //
    //  Check each subscription for a reference to the specified instance 
    //
    for (Uint32 i = 0; i < subscriptions.size (); i++)
    {
        //
        //  Get the reference property value from the subscription instance
        //
        CIMValue propValue = subscriptions [i].getProperty
            (subscriptions [i].findProperty
            (referenceProperty)).getValue ();
        CIMObjectPath ref;
        propValue.get (ref);

        //
        //  If the current subscription references the specified instance,
        //  delete it
        //
        if (handler == ref)
        {
            //
            //  Delete referencing subscription instance from repository
            //
            _repository->write_lock ();

            try
            {
                //
                //  Namespace and host must not be set in path passed to 
                //  repository
                //
                CIMObjectPath path ("", CIMNamespaceName (),
                    subscriptions [i].getPath ().getClassName(),
                    subscriptions [i].getPath ().getKeyBindings());
                _repository->deleteInstance (nameSpace, path);
            }
            catch (Exception & exception)
            {
                //
                //  Deletion of referencing subscription failed
                //
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                    "Exception caught in deleting referencing subscription (" +
                    subscriptions[i].getPath().toString() + "): " + 
                    exception.getMessage ());
            }
            _repository->write_unlock ();

            Array <ProviderClassList> indicationProviders;
            Array <CIMName> indicationSubclasses;
            CIMNamespaceName sourceNamespaceName;

            CIMObjectPath path = subscriptions [i].getPath ();
            path.setNameSpace (nameSpace);
            subscriptions [i].setPath (path);

            indicationProviders = _getDeleteParams (nameSpace, 
                subscriptions [i], indicationSubclasses, sourceNamespaceName);

            //
            //  Send Delete requests
            //
            //  NOTE: These Delete requests are not associated with a user 
            //  request, so there is no associated authType or userName
            //  The Creator from the subscription instance is used for userName,
            //  and authType is not set
            //
            CIMInstance instance = subscriptions [i];
            String creator = String::EMPTY;
            _getCreator (instance, creator);

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

            CIMObjectPath instanceName = 
                subscriptions [i].getPath ();
            instanceName.setNameSpace (nameSpace);
            subscriptions [i].setPath (instanceName);
// l10n  
            _sendDeleteRequests (indicationProviders, sourceNamespaceName, 
                subscriptions [i], 
                AcceptLanguages(acceptLangs),
                ContentLanguages(contentLangs),
                0,  // no request
                indicationSubclasses,
                creator);
        }
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
    Array <ProviderClassList> indicationProviders;
    CIMNamespaceName nameSpace = subscription.getNameSpace ();
    subscription.setNameSpace (CIMNamespaceName ());

    //
    //  Get instance from repository
    //
    _repository->read_lock ();

    try
    {
        subscriptionInstance = _repository->getInstance 
            (nameSpace, subscription);
    }
    catch (Exception&)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw;
    }

    _repository->read_unlock ();

    //
    //  Delete the subscription instance
    //
    _repository->write_lock ();

    try
    {
        _repository->deleteInstance (nameSpace, subscription);
    }
    catch (Exception & exception)
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
            "Exception caught in deleting expired subscription (" +
            subscriptionInstance.getPath().toString() + "): " +
            exception.getMessage ());
    }

    _repository->write_unlock ();

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
        Array <CIMName> indicationSubclasses;
        CIMNamespaceName sourceNamespaceName;

        subscription.setNameSpace (nameSpace);
        subscriptionInstance.setPath (subscription);

        indicationProviders = _getDeleteParams 
            (nameSpace, subscriptionInstance,
            indicationSubclasses, sourceNamespaceName);

        //
        //  Send Delete requests
        //
        //  NOTE: These Delete requests are not associated with a user request,
        //  so there is no associated authType or userName
        //  The Creator from the subscription instance is used for userName,
        //  and authType is not set
        //
        String creator;
        _getCreator (subscriptionInstance, creator);

        //
        // Get the language tags that were saved with the subscription instance 
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
                // ATTN: P4 SF 10/15/2002 May need to handle this error more
                // appropriately.
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
    String & queryLanguage)
{
    String filterQuery;
    WQLSelectStatement selectStatement;
    CIMName indicationClassName;
    condition = String::EMPTY;
    queryLanguage = String::EMPTY;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getCreateParams");

    //
    //  Get filter properties
    //
    _getFilterProperties (subscriptionInstance, nameSpaceName, 
        filterQuery, sourceNameSpace, queryLanguage);
    selectStatement = _getSelectStatement (filterQuery);
    
    //
    //  Get indication class name from filter query (FROM clause)
    //
    indicationClassName = _getIndicationClassName (selectStatement,
        sourceNameSpace);
    
    //
    //  Get list of subclass names for indication class
    //
    indicationSubclasses = _getIndicationSubclasses (sourceNameSpace,
        indicationClassName);

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
            condition = _getCondition (filterQuery);
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
    String & queryLanguage)
{
    String filterQuery;
    WQLSelectStatement selectStatement;
    condition = String::EMPTY;
    queryLanguage = String::EMPTY;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getCreateParams");

    //
    //  Get filter properties
    //
    _getFilterProperties (subscriptionInstance, nameSpaceName, 
        filterQuery, sourceNameSpace, queryLanguage);
    selectStatement = _getSelectStatement (filterQuery);
    
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
        condition = _getCondition (filterQuery);
    }

    //
    //  Get indication class name from filter query (FROM clause)
    //
    indicationClassName = _getIndicationClassName (selectStatement,
        sourceNameSpace);
    
    //
    //  Get list of subclass names for indication class
    //
    indicationSubclasses = _getIndicationSubclasses (sourceNameSpace,
        indicationClassName);

    PEG_METHOD_EXIT ();
}
    
Array <ProviderClassList> IndicationService::_getDeleteParams (
    const CIMNamespaceName & nameSpaceName,
    const CIMInstance & subscriptionInstance,
    Array <CIMName> & indicationSubclasses,
    CIMNamespaceName & sourceNameSpace)
{
    String filterQuery;
    WQLSelectStatement selectStatement;
    CIMName indicationClassName;
    CIMPropertyList propertyList;
    Array <ProviderClassList> indicationProviders;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getDeleteParams");

    //
    //  Get filter properties
    //
    _getFilterProperties (subscriptionInstance, nameSpaceName, 
        filterQuery, sourceNameSpace);
    selectStatement = _getSelectStatement (filterQuery);
    
    //
    //  Get indication class name from filter query (FROM clause)
    //
    indicationClassName = _getIndicationClassName (selectStatement,
        sourceNameSpace);
    
    //
    //  Get list of subclass names for indication class
    //
    indicationSubclasses = _getIndicationSubclasses (sourceNameSpace,
        indicationClassName);

    //
    //  Get property list from filter query (FROM and WHERE 
    //  clauses)
    //
    propertyList = _getPropertyList (selectStatement,
        sourceNameSpace, indicationClassName);

    //
    //  Get indication provider class lists from Active Subscriptions table
    //
    String activeSubscriptionsKey = _generateActiveSubscriptionsKey
        (subscriptionInstance.getPath ());
    ActiveSubscriptionsTableEntry tableValue;
    if (_lockedLookupActiveSubscriptionsEntry (activeSubscriptionsKey, 
                    tableValue))
    {
        indicationProviders = tableValue.providers;
    }
    else
    {
        //
        //  Subscription not found in Active Subscriptions table
        //
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
            "Subscription (" + activeSubscriptionsKey +
            ") not found in ActiveSubscriptionsTable");
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
     const String & queryLanguage,
     const CIMInstance & subscription,
     const AcceptLanguages & acceptLangs,
     const ContentLanguages & contentLangs,
     const CIMRequestMessage * origRequest,
     const Array <CIMName> & indicationSubclasses,
     const String & userName,
     const String & authType)
{
    CIMValue propValue;
    Uint16 repeatNotificationPolicy;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_sendCreateRequests");

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
        //  Create Instance, Modify Instance, or Provider Registration Change
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

            default:
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                    "Unexpected origRequest type " +
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
                indicationProviders [i].provider, 
                indicationProviders [i].providerModule,
                propertyList,
                repeatNotificationPolicy,
                condition,
                queryLanguage,
                QueueIdStack (_providerManager, getQueueId ()),
                authType,
                userName,
                contentLangs,
                acceptLangs);

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMCreateSubscriptionRequestMessage * requestCopy =
            new CIMCreateSubscriptionRequestMessage (* request);
        operationAggregate->appendRequest (requestCopy);

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
     const String & queryLanguage,
     const CIMInstance & subscription,
     const AcceptLanguages & acceptLangs,
     const ContentLanguages & contentLangs,
     const CIMRequestMessage * origRequest,
     const String & userName,
     const String & authType)
{
    CIMValue propValue;
    Uint16 repeatNotificationPolicy;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_sendModifyRequests");

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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                "Unexpected origRequest type " +
                String (MessageTypeToString (origRequest->getType ())) +
                " in _sendModifyRequests");
        }
    }
    else
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
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
                indicationProviders [i].provider, 
                indicationProviders [i].providerModule,
                propertyList,
                repeatNotificationPolicy,
                condition,
                queryLanguage,
                QueueIdStack (_providerManager, getQueueId ()),
                authType,
                userName,
                contentLangs,
                acceptLangs);

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMModifySubscriptionRequestMessage * requestCopy =
            new CIMModifySubscriptionRequestMessage (* request);
        operationAggregate->appendRequest (requestCopy);

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
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                    "Unexpected origRequest type " +
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
                indicationProviders [i].provider, 
                indicationProviders [i].providerModule,
                QueueIdStack (_providerManager, getQueueId ()),
                authType,
                userName,
                contentLangs,
                acceptLangs);

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMDeleteSubscriptionRequestMessage * requestCopy =
            new CIMDeleteSubscriptionRequestMessage (* request);
        operationAggregate->appendRequest (requestCopy);

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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                "Unexpected request type " +
                String (MessageTypeToString 
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

    Array <ProviderClassList> enableProviders;
    CIMObjectPath instanceRef;
    CIMException cimException;

    //
    //  Examine provider responses
    //
    Uint32 accepted = 0;
    for (Uint32 i = 0; i < operationAggregate->getNumberResponses (); i++)
    {
        //
        //  If response is SUCCESS, provider accepted the subscription
        //  Add provider to list of providers to enable
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
            enableProviders.append (provider);
        }
        else
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL3, 
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
            //  If subscription is not disabled or removed, send alert
            //
            if (!_handleError (request->subscriptionInstance))
            {
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
            //  For Create Instance or Modify Instance
            //  set CIM exception for response
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
            provider.provider = origRequest->provider;
            provider.providerModule = origRequest->providerModule;
            provider.classList.append (origRequest->className);

            //
            //  Update the entry in the active subscriptions hash table
            //
            String activeSubscriptionsKey = _generateActiveSubscriptionsKey
                (request->subscriptionInstance.getPath ());
            ActiveSubscriptionsTableEntry tableValue;
            if (_lockedLookupActiveSubscriptionsEntry (activeSubscriptionsKey,
                tableValue))
            {
                tableValue.providers.append (provider);
                {
                    WriteLock lock (_activeSubscriptionsTableLock);
                    _removeActiveSubscriptionsEntry (activeSubscriptionsKey);
                    _insertActiveSubscriptionsEntry (tableValue.subscription, 
                        tableValue.providers);
                }
            }
            else
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                    "Subscription (" + activeSubscriptionsKey +
                    ") not found in ActiveSubscriptionsTable");
                //
                //  The subscription may have been deleted in the mean time
                //  If so, no further update is required
                //
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
                instanceRef = _createInstance 
                    (origRequest, request->subscriptionInstance, true);
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
                _insertToHashTables (instance, 
                    enableProviders,
                    operationAggregate->getIndicationSubclasses (), 
                    request->nameSpace);

                //
                //  Send Enable message to each provider that accepted 
                //  subscription
                //
                _sendEnable (enableProviders, 
                    operationAggregate->getOrigRequest ());
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
            _insertToHashTables (request->subscriptionInstance, 
                enableProviders,
                operationAggregate->getIndicationSubclasses (), 
                request->nameSpace);

            //
            //  Send Enable message to each provider that accepted subscription
            //
            _sendEnable (enableProviders, 
                operationAggregate->getOrigRequest ());
        }
    }

    //
    //  For Create Instance or Modify Instance, send response
    //
    if (operationAggregate->requiresResponse ())
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

        else  // CIM_MODIFY_INSTANCE_REQUEST_MESSAGE
        {
            // l10n
            // Note: don't need to set Content-language in the response
            //
            response = new CIMModifyInstanceResponseMessage (
                operationAggregate->getOrigMessageId (),
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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL3, 
                "Provider (" + provider.provider.getPath ().toString() +
                ") rejected enable indications: " +
                response->cimException.getMessage ());
        }
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleModifyResponseAggregation (
    IndicationOperationAggregate * operationAggregate)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleModifyResponseAggregation");

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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL3, 
                "Provider (" + provider.provider.getPath ().toString() +
                ") rejected modify subscription: " +
                response->cimException.getMessage ());
        }
    }

    //
    //  Update subscription hash tables
    //
    CIMModifySubscriptionRequestMessage * request =
        (CIMModifySubscriptionRequestMessage *) 
            operationAggregate->getRequest (0);

    CIMNotifyProviderRegistrationRequestMessage * origRequest =
        (CIMNotifyProviderRegistrationRequestMessage *) 
            operationAggregate->getOrigRequest ();

    String activeSubscriptionsKey = _generateActiveSubscriptionsKey
        (request->subscriptionInstance.getPath ());
    ActiveSubscriptionsTableEntry tableValue;
    if (_lockedLookupActiveSubscriptionsEntry (activeSubscriptionsKey,
        tableValue))
    {
        Uint32 providerIndex = _providerInList (origRequest->provider, 
            tableValue);
        if (providerIndex != PEG_NOT_FOUND)
        {
            Uint32 classIndex = _classInList (origRequest->className,
                tableValue.providers [providerIndex]);
            if (classIndex == PEG_NOT_FOUND)
            {
                tableValue.providers [providerIndex].classList.append
                    (origRequest->className);
            }
            else //  classIndex != PEG_NOT_FOUND
            {
                tableValue.providers [providerIndex].classList.remove
                    (classIndex);
            }
            {
            WriteLock lock(_activeSubscriptionsTableLock);
            _removeActiveSubscriptionsEntry
                (activeSubscriptionsKey);
            _insertActiveSubscriptionsEntry
                (tableValue.subscription, tableValue.providers);
            }
        }
        else
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                "Provider (" + origRequest->provider.getPath().toString() +
                ") not found in list for Subscription (" +
                activeSubscriptionsKey +
                ") in ActiveSubscriptionsTable");
        }
    }
    else
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
            "Subscription (" + activeSubscriptionsKey +
            ") not found in ActiveSubscriptionsTable");
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleDeleteResponseAggregation (
    IndicationOperationAggregate * operationAggregate)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_handleDeleteResponseAggregation");

    CIMException cimException;
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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL3,
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

    if (operationAggregate->getOrigType () ==
        CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE)
    {
        CIMNotifyProviderRegistrationRequestMessage * origRequest =
            (CIMNotifyProviderRegistrationRequestMessage *) 
                operationAggregate->getOrigRequest ();

        //
        //  Update the entry in the active subscriptions hash table
        //
        String activeSubscriptionsKey = _generateActiveSubscriptionsKey
            (request->subscriptionInstance.getPath ());
        ActiveSubscriptionsTableEntry tableValue;
        if (_lockedLookupActiveSubscriptionsEntry (activeSubscriptionsKey,
            tableValue))
        {
            Uint32 providerIndex = _providerInList (origRequest->provider, 
                tableValue);
            if (providerIndex != PEG_NOT_FOUND)
            {
                tableValue.providers.remove (providerIndex);
                {
                WriteLock lock (_activeSubscriptionsTableLock);
                _removeActiveSubscriptionsEntry (activeSubscriptionsKey);
                _insertActiveSubscriptionsEntry (tableValue.subscription,
                    tableValue.providers);
                }
            }
            else
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                    "Provider (" + origRequest->provider.getPath().toString() +
                    ") not found in list for Subscription (" +
                    activeSubscriptionsKey +
                    ") in ActiveSubscriptionsTable");
            }
        }
        else
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                "Subscription (" + activeSubscriptionsKey +
                ") not found in ActiveSubscriptionsTable");
        }
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
        _removeFromHashTables (request->subscriptionInstance, 
            operationAggregate->getIndicationSubclasses (), 
            request->nameSpace);
    }

    //
    //  Check each provider to see if it's still in use by any subscription
    //
    Array <ProviderClassList> disableProviders;
    for (Uint32 j = 0; j < checkProviders.size (); j++)
    {
        if (!_providerInUse (checkProviders [j].provider))
        {
            disableProviders.append (checkProviders [j]);
        }
    }

    //
    //  Send Disable Indications requests to any provider no longer in 
    //  use
    //
    if (disableProviders.size () > 0)
    {
        _sendDisable (disableProviders, operationAggregate->getOrigRequest ());
    }

    //
    //  For Delete Instance or Modify Instance, send response
    //
    if (operationAggregate->requiresResponse ())
    {
        CIMResponseMessage * response;
        if (operationAggregate->getOrigType () ==
            CIM_DELETE_INSTANCE_REQUEST_MESSAGE)
        {
            // l10n
            // Note: don't need to set Content-language in the response
            response = new CIMDeleteInstanceResponseMessage (
                operationAggregate->getOrigMessageId (),
                cimException,
                operationAggregate->getOrigRequest ()->queueIds.copyAndPop ());
        }

        else  // CIM_MODIFY_INSTANCE_REQUEST_MESSAGE
        {
            // l10n
            // Note: don't need to set Content-language in the response
            //
            response = new CIMModifyInstanceResponseMessage (
                operationAggregate->getOrigMessageId (),
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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL3, 
                "Provider (" + provider.provider.getPath ().toString() +
                ") rejected disable indications: " +
                response->cimException.getMessage ());
        }
    }

    PEG_METHOD_EXIT ();
}

String IndicationService::_generateActiveSubscriptionsKey (
    const CIMObjectPath & subscriptionRef)
{
    String activeSubscriptionsKey;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_generateActiveSubscriptionsKey");

    //
    //  Append subscription namespace name to key
    //
    activeSubscriptionsKey.append 
        (subscriptionRef.getNameSpace ().getString());

    //
    //  Get filter and handler key bindings from subscription reference
    //
    Array<CIMKeyBinding> subscriptionKB = subscriptionRef.getKeyBindings ();
    Array<CIMKeyBinding> filterKB;
    Array<CIMKeyBinding> handlerKB;
    for (Uint32 i = 0; i < subscriptionKB.size (); i++)
    {
        if ((subscriptionKB [i].getName () == _PROPERTY_FILTER) &&
            (subscriptionKB [i].getType () == CIMKeyBinding::REFERENCE))
        {
            CIMObjectPath filterRef (subscriptionKB [i].getValue ());
            filterKB = filterRef.getKeyBindings ();
        }
        if ((subscriptionKB [i].getName () == _PROPERTY_HANDLER) &&
            (subscriptionKB [i].getType () == CIMKeyBinding::REFERENCE))
        {
            CIMObjectPath handlerRef (subscriptionKB [i].getValue ());
            handlerKB = handlerRef.getKeyBindings ();
        }
    }

    //
    //  Append subscription filter key values to key
    //
    for (Uint32 j = 0; j < filterKB.size (); j++)
    {
        activeSubscriptionsKey.append (filterKB [j].getValue ());
    }

    //
    //  Append subscription handler key values to key
    //
    for (Uint32 k = 0; k < handlerKB.size (); k++)
    {
        activeSubscriptionsKey.append (handlerKB [k].getValue ());
    }

    PEG_METHOD_EXIT ();
    return activeSubscriptionsKey;
}

Boolean IndicationService::_lockedLookupActiveSubscriptionsEntry (
    const String & key,
    ActiveSubscriptionsTableEntry & tableEntry)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_lockedLookupActiveSubscriptionsEntry");

    ReadLock lock(_activeSubscriptionsTableLock);

    return (_activeSubscriptionsTable.lookup (key, tableEntry));

    PEG_METHOD_EXIT ();
}

void IndicationService::_insertActiveSubscriptionsEntry (
    const CIMInstance & subscription,
    const Array <ProviderClassList> & providers)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_insertActiveSubscriptionsEntry");

    String activeSubscriptionsKey = _generateActiveSubscriptionsKey 
        (subscription.getPath ());
    ActiveSubscriptionsTableEntry entry;
    entry.subscription = subscription;
    entry.providers = providers;

    _activeSubscriptionsTable.insert (activeSubscriptionsKey, entry);

#ifdef PEGASUS_INDICATION_HASHTRACE
    String traceString;
    traceString.append (activeSubscriptionsKey);
    traceString.append (" Providers: ");
    for (Uint32 i = 0; i < providers.size (); i++)
    {
        String providerName = providers [i].provider.getProperty 
            (providers [i].provider.findProperty 
            (_PROPERTY_NAME)).getValue ().toString ();
        traceString.append (providerName);
        traceString.append ("  Classes: ");
        for (Uint32 j = 0; j < providers[i].classList.size (); j++)
        {
             traceString.append (providers[i].classList[j].getString());   
             traceString.append ("  ");
        }
    }
    
    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3, 
        "INSERTED _activeSubscriptionsTable entry: " + traceString);
#endif

    PEG_METHOD_EXIT ();
}

void IndicationService::_removeActiveSubscriptionsEntry (
    const String & key)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_removeActiveSubscriptionsEntry");

    _activeSubscriptionsTable.remove (key);
#ifdef PEGASUS_INDICATION_HASHTRACE
    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, 
                      Tracer::LEVEL3, 
                      "REMOVED _activeSubscriptionsTable entry: " + key);
#endif

    PEG_METHOD_EXIT ();
}

String IndicationService::_generateSubscriptionClassesKey (
    const CIMName & indicationClassName,
    const CIMNamespaceName & sourceNamespaceName)
{
    String subscriptionClassesKey;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_generateSubscriptionClassesKey");

    //
    //  Append indication class name to key
    //
    subscriptionClassesKey.append (indicationClassName.getString ());

    //
    //  Append source namespace name to key
    //
    subscriptionClassesKey.append (sourceNamespaceName.getString ());

    PEG_METHOD_EXIT ();
    return subscriptionClassesKey;
}

Boolean IndicationService::_lockedLookupSubscriptionClassesEntry (
    const String & key,
    SubscriptionClassesTableEntry & tableEntry)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_lockedLookupSubscriptionClassesEntry");

    ReadLock lock(_subscriptionClassesTableLock);

    return (_subscriptionClassesTable.lookup (key, tableEntry));

    PEG_METHOD_EXIT ();
}

void IndicationService::_lockedInsertSubscriptionClassesEntry (
    const CIMName & indicationClassName,
    const CIMNamespaceName & sourceNamespaceName,
    const Array <CIMInstance> & subscriptions)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_lockedInsertSubscriptionClassesEntry");

    String subscriptionClassesKey = _generateSubscriptionClassesKey
        (indicationClassName, sourceNamespaceName);
    SubscriptionClassesTableEntry entry;
    entry.indicationClassName = indicationClassName;
    entry.sourceNamespaceName = sourceNamespaceName;
    entry.subscriptions = subscriptions;
    {
        WriteLock lock(_subscriptionClassesTableLock);
        _subscriptionClassesTable.insert (subscriptionClassesKey, entry);
    }

#ifdef PEGASUS_INDICATION_HASHTRACE
    String traceString;
    traceString.append (subscriptionClassesKey);
    traceString.append (" Subscriptions: ");
    for (Uint32 i = 0; i < subscriptions.size (); i++)
    {
        traceString.append (subscriptions [i].getPath ().toString());   
        traceString.append ("  ");
    }
    
    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3, 
        "INSERTED _subscriptionClassesTable entry: " + traceString);
#endif

    PEG_METHOD_EXIT ();
}

void IndicationService::_lockedRemoveSubscriptionClassesEntry (
    const String & key)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "IndicationService::_lockedRemoveSubscriptionClassesEntry");

    WriteLock lock(_subscriptionClassesTableLock);

    _subscriptionClassesTable.remove (key);

#ifdef PEGASUS_INDICATION_HASHTRACE
    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3, 
        "REMOVED _subscriptionClassesTable entry: " + key);
#endif

    PEG_METHOD_EXIT ();
}

void IndicationService::_insertToHashTables (
    const CIMInstance & subscription,
    const Array <ProviderClassList> & providers,
    const Array <CIMName> & indicationSubclassNames,
    const CIMNamespaceName & sourceNamespaceName)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_insertToHashTables");

    //
    //  Insert entry into active subscriptions table 
    //
    {
        WriteLock lock(_activeSubscriptionsTableLock);
        _insertActiveSubscriptionsEntry (subscription, providers);
    }

    //
    //  Insert or update entries in subscription classes table 
    //
    for (Uint32 i = 0; i < indicationSubclassNames.size (); i++)
    {
        String subscriptionClassesKey = _generateSubscriptionClassesKey
            (indicationSubclassNames [i], sourceNamespaceName);
        SubscriptionClassesTableEntry tableValue;
        if (_lockedLookupSubscriptionClassesEntry (subscriptionClassesKey,
            tableValue))
        {
            //
            //  If entry exists for this IndicationClassName-SourceNamespace 
            //  pair, remove old entry and insert new entry
            //
            Array <CIMInstance> subscriptions = tableValue.subscriptions;
            subscriptions.append (subscription);
            _lockedRemoveSubscriptionClassesEntry (subscriptionClassesKey);
            _lockedInsertSubscriptionClassesEntry (indicationSubclassNames [i],
                sourceNamespaceName, subscriptions);
        }
        else
        {
            //
            //  If no entry exists for this 
            //  IndicationClassName-SourceNamespace pair, insert new entry
            //
            Array <CIMInstance> subscriptions;
            subscriptions.append (subscription);
            _lockedInsertSubscriptionClassesEntry (indicationSubclassNames [i],
                sourceNamespaceName, subscriptions);
        }
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_removeFromHashTables (
    const CIMInstance & subscription,
    const Array <CIMName> & indicationSubclassNames,
    const CIMNamespaceName & sourceNamespaceName)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_removeFromHashTables");

    //
    //  Remove entry from active subscriptions table 
    //
    {
        WriteLock lock(_activeSubscriptionsTableLock);

        _removeActiveSubscriptionsEntry (
            _generateActiveSubscriptionsKey (subscription.getPath ()));
    }

    //
    //  Remove or update entries in subscription classes table 
    //
    for (Uint32 i = 0; i < indicationSubclassNames.size (); i++)
    {
        String subscriptionClassesKey = _generateSubscriptionClassesKey
            (indicationSubclassNames [i], sourceNamespaceName);
        SubscriptionClassesTableEntry tableValue;
        if (_lockedLookupSubscriptionClassesEntry (subscriptionClassesKey,
            tableValue))
        {
            //
            //  If entry exists for this IndicationClassName-SourceNamespace 
            //  pair, remove subscription from the list
            //
            Array <CIMInstance> subscriptions = tableValue.subscriptions;
            for (Uint32 j = 0; j < subscriptions.size (); j++)
            {
                if (subscriptions [j].getPath().identical 
                   (subscription.getPath()))
                {
                    subscriptions.remove (j);
                }
            }

            //
            //  Remove the old entry
            //
            _lockedRemoveSubscriptionClassesEntry (subscriptionClassesKey);

            //
            //  If there are still subscriptions in the list, insert the 
            //  new entry
            //
            if (subscriptions.size () > 0)
            {
                _lockedInsertSubscriptionClassesEntry (
                    indicationSubclassNames [i],
                    sourceNamespaceName, subscriptions);
            }
        }
        else
        {
            //
            //  Entry not found in Subscription Classes table
            //
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                "Indication subclass and namespace (" + subscriptionClassesKey +
                ") not found in SubscriptionClassesTable");
        }
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
    CIMInstance current;
    Boolean duplicate;
    Array <CIMInstance> handlers; 
    
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_sendAlerts");

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
        current = _getHandler (subscriptions [i]);

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
    const CIMRequestMessage * origRequest)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_sendEnable");

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
        //  Create Instance, Modify Instance, or Provider Registration Change
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

            default:
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                    "Unexpected origRequest type " +
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
                enableProviders [i].provider,
                enableProviders [i].providerModule,
                QueueIdStack (_providerManager, getQueueId ()));

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMEnableIndicationsRequestMessage * requestCopy =
            new CIMEnableIndicationsRequestMessage (* request);
        operationAggregate->appendRequest (requestCopy);

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
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE, Tracer::LEVEL2, 
                    "Unexpected origRequest type " +
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
                disableProviders [i].provider,
                disableProviders [i].providerModule,
                QueueIdStack (_providerManager, getQueueId ()));

        //
        //  Store a copy of the request in the operation aggregate instance
        //
        CIMDisableIndicationsRequestMessage * requestCopy =
            new CIMDisableIndicationsRequestMessage (* request);
        operationAggregate->appendRequest (requestCopy);

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
    Boolean booleanValue;
    WQLSimplePropertySource source;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getPropertySourceFromInstance");

    for (Uint32 i=0; i < indicationInstance.getPropertyCount(); i++)
    {
        CIMProperty property = indicationInstance.getProperty(i);
        CIMValue propertyValue = property.getValue();
        CIMType type = property.getType();
        CIMName propertyName = property.getName();

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
                break;                break;

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
                property.getValue().get(booleanValue);
                source.addValue(propertyName.getString(),
                    WQLOperand(booleanValue, WQL_BOOLEAN_VALUE_TAG));
                break;

            case CIMTYPE_CHAR16:
            case CIMTYPE_STRING :
                source.addValue(propertyName.getString(),
                    WQLOperand(property.getValue().toString(),
                    WQL_STRING_VALUE_TAG));
                break;

            case CIMTYPE_DATETIME :
                source.addValue (propertyName.getString(),
                    WQLOperand ());
                break;
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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, 
                Tracer::LEVEL4, 
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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, 
               Tracer::LEVEL4, 
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
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE, 
            Tracer::LEVEL4, 
            "Missing Subscription Creator property");

        //
        //  This is a corrupted/invalid instance
        //
        return false;
    }

    PEG_METHOD_EXIT ();
    return true;
}

Boolean IndicationService::_getState (
    const CIMInstance & instance,
    Uint16 & state) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_getState");

    Uint32 stateIndex = instance.findProperty (_PROPERTY_STATE);
    if (stateIndex != PEG_NOT_FOUND)
    {
        CIMValue stateValue = instance.getProperty 
            (stateIndex).getValue ();
        if (stateValue.isNull ())
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, 
                Tracer::LEVEL4, 
                "Null SubscriptionState property value");

            //
            //  This is a corrupted/invalid instance
            //
            return false;
        }
        else if ((stateValue.getType () != CIMTYPE_UINT16) ||
            (stateValue.isArray ()))
        {
            String traceString;
            if (stateValue.isArray ())
            {
                traceString.append ("array of ");
            }
            traceString.append (cimTypeToString (stateValue.getType ()));
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE, 
               Tracer::LEVEL4, 
               "SubscriptionState property value of incorrect type: "
               + traceString);

            //
            //  This is a corrupted/invalid instance
            //
            return false;
        }
        else
        {
            stateValue.get (state);

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
        }
    }
    else
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE, 
            Tracer::LEVEL4, 
            "Missing SubscriptionState property");

        //
        //  This is a corrupted/invalid instance
        //
        return false;
    }

    PEG_METHOD_EXIT ();
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
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, 
        "IndicationService::_getSubscriptionLogString");

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

    PEG_METHOD_EXIT ();
    return logString;
}

String IndicationService::_getProviderLogString
    (CIMInstance & provider)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, 
        "IndicationService::_getProviderLogString");

    String logString;
    
    logString = provider.getProperty (provider.findProperty 
        (_PROPERTY_NAME)).getValue ().toString ();

    PEG_METHOD_EXIT ();
    return logString;
}

//
//  Class names
//

/**
    The name of the CIMOM Shutdown alert indication class
 */
//
//  ATTN: Update once CimomShutdownAlertIndication has been defined
//
const CIMName IndicationService::_CLASS_CIMOM_SHUTDOWN_ALERT =
              CIMName ("CIM_AlertIndication");

/**
    The name of the No Provider alert indication class
 */
//
//  ATTN: Update once NoProviderAlertIndication has been defined
//
const CIMName IndicationService::_CLASS_NO_PROVIDER_ALERT =
              CIMName ("CIM_AlertIndication");

/**
    The name of the CIMOM shutdown alert indication class
 */
//
//  ATTN: Update once ProviderTerminatedAlertIndication has been defined
//
const CIMName IndicationService::_CLASS_PROVIDER_TERMINATED_ALERT =
              CIMName ("CIM_AlertIndication");


//
//  Property names
//

/**
    The name of the filter reference property for indication subscription class
 */
const CIMName IndicationService::_PROPERTY_FILTER = CIMName ("Filter");

/**
    The name of the handler reference property for indication subscription class
 */
const CIMName IndicationService::_PROPERTY_HANDLER = CIMName ("Handler");

/**
    The name of the subscription state property for indication subscription 
    class
 */
const CIMName IndicationService::_PROPERTY_STATE = 
              CIMName ("SubscriptionState");

/**
    The name of the Other Subscription State property for Indication 
    Subscription class
 */
const CIMName IndicationService::_PROPERTY_OTHERSTATE = 
              CIMName ("OtherSubscriptionState");

/**
    The name of the repeat notification policy property for indication
    subscription class
 */
const CIMName IndicationService::_PROPERTY_REPEATNOTIFICATIONPOLICY =
              CIMName ("RepeatNotificationPolicy");

/**
    The name of the other repeat notification policy property for
    indication subscription class
 */
const CIMName IndicationService::_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY =
              CIMName ("OtherRepeatNotificationPolicy");

/**
    The name of the repeat notification interval property for indication
    subscription class
 */
const CIMName IndicationService::_PROPERTY_REPEATNOTIFICATIONINTERVAL =
              CIMName ("RepeatNotificationInterval");

/**
    The name of the repeat notification gap property for indication
    subscription class
 */
const CIMName IndicationService::_PROPERTY_REPEATNOTIFICATIONGAP =
              CIMName ("RepeatNotificationGap");

/**
    The name of the repeat notification count property for indication
    subscription class
 */
const CIMName IndicationService::_PROPERTY_REPEATNOTIFICATIONCOUNT =
              CIMName ("RepeatNotificationCount");

/**
    The name of the On Fatal Error Policy property for Indication Subscription 
    class
 */
const CIMName IndicationService::_PROPERTY_ONFATALERRORPOLICY = 
              CIMName ("OnFatalErrorPolicy");

/**
    The name of the Other On Fatal Error Policy property for Indication 
    Subscription class
 */
const CIMName IndicationService::_PROPERTY_OTHERONFATALERRORPOLICY = 
              CIMName ("OtherOnFatalErrorPolicy");

/**
    The name of the Time Of Last State Change property for Indication 
    Subscription class
 */
const CIMName IndicationService::_PROPERTY_LASTCHANGE = 
              CIMName ("TimeOfLastStateChange");

/**
    The name of the Subscription Start Time property for Indication 
    Subscription class
 */
const CIMName IndicationService::_PROPERTY_STARTTIME = 
              CIMName ("SubscriptionStartTime");

/**
    The name of the Subscription Duration property for Indication 
    Subscription class
 */
const CIMName IndicationService::_PROPERTY_DURATION = 
              CIMName ("SubscriptionDuration");

/**
    The name of the Subscription Time Remaining property for Indication 
    Subscription class
 */
const CIMName IndicationService::_PROPERTY_TIMEREMAINING = 
              CIMName ("SubscriptionTimeRemaining");

/**
    The name of the query property for indication filter class
 */
const CIMName IndicationService::_PROPERTY_QUERY = CIMName ("Query");

/**
    The name of the query language property for indication filter class
 */
const CIMName IndicationService::_PROPERTY_QUERYLANGUAGE = 
              CIMName ("QueryLanguage");

/**
    The name of the Source Namespace property for indication filter class
 */
const CIMName IndicationService::_PROPERTY_SOURCENAMESPACE = 
              CIMName ("SourceNamespace");

/**
    The name of the name property for indication filter and indications handler     classes
 */
const CIMName IndicationService::_PROPERTY_NAME = CIMName ("Name");

/**
    The name of the creation class name property for indication filter and 
    indications handler classes
 */
const CIMName IndicationService::_PROPERTY_CREATIONCLASSNAME = 
              CIMName ("CreationClassName");

/**
    The name of the system name property for indication filter and indications 
    handler classes
 */
const CIMName IndicationService::_PROPERTY_SYSTEMNAME = 
              CIMName ("SystemName");

/**
    The name of the system creation class name property for indication filter 
    and indications handler classes
 */
const CIMName IndicationService::_PROPERTY_SYSTEMCREATIONCLASSNAME = 
              CIMName ("SystemCreationClassName");

/**
    The name of the Persistence Type property for Indication Handler class
 */
const CIMName IndicationService::_PROPERTY_PERSISTENCETYPE = 
              CIMName ("PersistenceType");

/**
    The name of the Other Persistence Type property for Indication Handler 
    class
 */
const CIMName IndicationService::_PROPERTY_OTHERPERSISTENCETYPE = 
              CIMName ("OtherPersistenceType");

/**
    The name of the Destination property for CIM XML Indication Handler 
    subclass
 */
const CIMName IndicationService::_PROPERTY_DESTINATION = 
              CIMName ("Destination");

/**
    The name of the TargetHost property for SNMP Mapper Indication 
    Handler subclass
 */
const CIMName IndicationService::_PROPERTY_TARGETHOST = 
              CIMName ("TargetHost");

/**
    The name of the TargetHostFormat property for SNMP Indication Handler subclass
 */
const CIMName IndicationService::_PROPERTY_TARGETHOSTFORMAT = 
              CIMName ("TargetHostFormat");

/**
    The name of the SNMPVersion property for SNMP Indication Handler class
 */
const CIMName IndicationService::_PROPERTY_SNMPVERSION =
              CIMName ("SNMPVersion");

/**
    The name of the Alert Type property for Alert Indication class
 */
const CIMName IndicationService::_PROPERTY_ALERTTYPE = CIMName ("AlertType");

/**
    The name of the Other Alert Type property for Alert Indication class
 */
const CIMName IndicationService::_PROPERTY_OTHERALERTTYPE = 
              CIMName ("OtherAlertType");

/**
    The name of the Perceived Severity property for Alert Indication class
 */
const CIMName IndicationService::_PROPERTY_PERCEIVEDSEVERITY = 
              CIMName ("PerceivedSeverity");

/**
    The name of the Probable Cause property for Alert Indication class
 */
const CIMName IndicationService::_PROPERTY_PROBABLECAUSE = 
              CIMName ("ProbableCause");

/**
    The name of the Provider Name property for Provider class
 */
const CIMName IndicationService::_PROPERTY_PROVIDERNAME = CIMName ("Name");

/**
    The name of the Provider Module Name property for Provider class
 */
const CIMName IndicationService::_PROPERTY_PROVIDERMODULENAME = 
              CIMName ("ProviderModuleName");


//
//  Qualifier names
//

/**
    The name of the Indication qualifier for classes
 */
const CIMName IndicationService::_QUALIFIER_INDICATION = CIMName ("INDICATION");


//
//  Other literal values
//

/**
    The WHERE keyword in WQL
 */
const char   IndicationService::_QUERY_WHERE []         = "WHERE";

/**
    The string representing the asterisk all properties symbol in WQL
 */
const char   IndicationService::_QUERY_ALLPROPERTIES [] = "*";

/**
    A zero value CIMDateTime interval
 */
const char IndicationService::_ZERO_INTERVAL_STRING [] = 
    "00000000000000.000000:000";

//
//  Message substrings used in exception messages
//

// l10n

// some have been commented out and put directly in the code for localization

// this one is tricky because it is used in _checkRequiredProperty with the two below
const char IndicationService::_MSG_MISSING_REQUIRED [] = "Missing required ";

const char IndicationService::_MSG_KEY_PROPERTY [] = " key property";
const char IndicationService::_MSG_KEY_PROPERTY_KEY [] = 
   "IndicationService.IndicationService._MSG_KEY_PROPERTY";

const char IndicationService::_MSG_PROPERTY [] = " property";
const char IndicationService::_MSG_PROPERTY_KEY [] = 
   "IndicationService.IndicationService._MSG_PROPERTY";

const char IndicationService::_MSG_PROPERTY_PRESENT [] = 
    " property present, but ";

const char IndicationService::_MSG_VALUE_NOT [] = " value not ";

const char IndicationService::_MSG_PROPERTY_PRESENT_BUT_VALUE_NOT_KEY [] = 
   "IndicationService.IndicationService._MSG_PROPERTY_PRESENT_BUT_VALUE_NOT";


const char IndicationService::_MSG_NO_PROVIDERS [] = 
    "There are no providers capable of serving the subscription";
const char IndicationService::_MSG_NO_PROVIDERS_KEY [] = 
    "IndicationService.IndicationService._MSG_NO_PROVIDERS";

const char IndicationService::_MSG_NOT_ACCEPTED [] = 
    "No providers accepted the subscription";
const char IndicationService::_MSG_NOT_ACCEPTED_KEY [] = 
    "IndicationService.IndicationService._MSG_NOT_ACCEPTED";

const char IndicationService::_MSG_INVALID_CLASSNAME [] = 
    "Invalid indication class name ";

const char IndicationService::_MSG_IN_FROM [] = " in FROM clause of ";

const char IndicationService::_MSG_INVALID_CLASSNAME_IN_FROM_PROPERTY_KEY [] = 
    "IndicationService.IndicationService._MSG_INVALID_CLASSNAME_IN_FROM_PROPERTY";

const char IndicationService::_MSG_EXPIRED [] = 
    "Expired subscription may not be modified; has been deleted";
const char IndicationService::_MSG_EXPIRED_KEY [] = 
    "IndicationService.IndicationService._MSG_EXPIRED";

const char IndicationService::_MSG_REFERENCED [] = 
    "A Filter or Handler referenced by a subscription may not be deleted";
const char IndicationService::_MSG_REFERENCED_KEY [] = 
    "IndicationService.IndicationService._MSG_REFERENCED";


const char IndicationService::_MSG_INVALID_VALUE [] =
    "Invalid value ";

const char IndicationService::_MSG_INVALID_TYPE [] =
    "Invalid type ";

const char IndicationService::_MSG_FOR_PROPERTY [] =
    " for property ";

const char IndicationService::_MSG_ARRAY_OF [] =
    " array of ";

const char IndicationService::_MSG_INVALID_VALUE_FOR_PROPERTY_KEY [] =
    "IndicationService.IndicationService._MSG_INVALID_VALUE_FOR_PROPERTY";

const char IndicationService::_MSG_CLASS_NOT_SERVED [] =
    "The specified class is not served by the Indication Service";

const char IndicationService::_MSG_CLASS_NOT_SERVED_KEY [] =
    "IndicationService.IndicationService._MSG_CLASS_NOT_SERVED";

const char IndicationService::_MSG_INVALID_INSTANCES [] =
    "One or more invalid Subscription instances were ignored";

const char IndicationService::_MSG_INVALID_INSTANCES_KEY [] =
    "IndicationService.IndicationService.INVALID_SUBSCRIPTION_INSTANCES_IGNORED";

const char IndicationService::_MSG_PROVIDER_NO_LONGER_SERVING [] =
    "Provider ($0) is no longer serving subscription ($1)";

const char IndicationService::_MSG_PROVIDER_NO_LONGER_SERVING_KEY [] =
    "IndicationService.IndicationService._MSG_PROVIDER_NO_LONGER_SERVING";

const char IndicationService::_MSG_PROVIDER_NOW_SERVING [] =
    "Provider ($0) is now serving subscription ($1)";

const char IndicationService::_MSG_PROVIDER_NOW_SERVING_KEY [] =
    "IndicationService.IndicationService._MSG_PROVIDER_NOW_SERVING";

const char IndicationService::_MSG_NO_PROVIDER [] =
    "Subscription ($0) has no provider";

const char IndicationService::_MSG_NO_PROVIDER_KEY [] =
    "IndicationService.IndicationService._MSG_NO_PROVIDER";

const char IndicationService::_MSG_NON_PRIVILEGED_ACCESS_DISABLED [] =
    "User ($0) is not authorized to perform this operation.";

const char IndicationService::_MSG_NON_PRIVILEGED_ACCESS_DISABLED_KEY [] =
    "IndicationService.IndicationService._MSG_NON_PRIVILEGED_ACCESS_DISABLED";

PEGASUS_NAMESPACE_END
