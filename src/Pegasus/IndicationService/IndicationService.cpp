//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company 
//               (carolann_graves@hp.com)
//
// Modified By:  Ben Heilbronn, Hewlett-Packard Company
//               (ben_heilbronn@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/OperationFlag.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/WQL/WQLSelectStatement.h>

#include "IndicationService.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

IndicationService::IndicationService (
    CIMRepository * repository,
    ProviderRegistrationManager * providerRegManager)
    : Base (PEGASUS_SERVICENAME_INDICATIONSERVICE, 
            MessageQueue::getNextQueueId ()),
         _repository (repository),
         _providerRegManager (providerRegManager)
{
    //
    //  ATTN: This call to _initialize is here temporarily -- in future, it
    //  will be called in response to a CIMSERVICE_START message
    //

    //
    //  Initialize
    //
    _initialize ();
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
    if (req->getType () == async_messages::CIMSERVICE_START)
    {
        //cout << "CIMSERVICE_START message received" << endl;
        req->op->processing ();

        //
        //  Call _initialize ();
        //
        _initialize ();

        handle_CimServiceStart (static_cast <CimServiceStart *> (req));
    }
    else if ( req->getType() == async_messages::ASYNC_LEGACY_OP_START )
    {
        req->op->processing();
        Message *legacy = 
            (static_cast<AsyncLegacyOperationStart *>(req)->get_action());
        handleEnqueue(legacy);
        return;
    }
    else
        Base::_handle_async_request(req);
}

void IndicationService::handleEnqueue(Message* message)
{

    switch(message->getType())
    {
	case CIM_GET_INSTANCE_REQUEST_MESSAGE:
	    _handleGetInstanceRequest(message);
	    break;

	case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	    _handleEnumerateInstancesRequest(message);
	    break;

	case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	    _handleEnumerateInstanceNamesRequest(message);
	    break;

	case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	    _handleCreateInstanceRequest(message);
	    break;

	case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	    _handleModifyInstanceRequest(message);
	    break;

	case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	    _handleDeleteInstanceRequest(message);
	    break;

        case CIM_PROCESS_INDICATION_REQUEST_MESSAGE:
            _handleProcessIndicationRequest(message);
            break;

	case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
	    _handleNotifyProviderRegistrationRequest(message);    
	    break;

	case CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE:
	    _handleNotifyProviderTerminationRequest(message);    
	    break;

	default:
            //
            //  A message type not supported by the Indication Service
            //  ATTN-CAKG-P2-20020326: Should a CIM_ERR_NOT_SUPPORTED
            //  response be returned??
            //
	    break;
    }

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
    Array <CIMNamedInstance> activeSubscriptions;
    Array <CIMNamedInstance> noProviderSubscriptions;
    Array <CIMInstance> startProviders;
    Boolean duplicate;

    const char METHOD_NAME [] = "IndicationService::_initialize";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Find required services
    //
    Array <Uint32> pmservices;
    find_services (PEGASUS_SERVICENAME_PROVIDERMANAGER_CPP, 0, 0, &pmservices);
    pegasus_yield ();
    if (pmservices.size () > 0)
    {
        _providerManager = pmservices [0];

        //
        //  ATTN: What should be done if more than one service is found??
        //
    }
    else
    {
        //
        //  ATTN: What should be done if no service is found??
        //
    }

    Array <Uint32> hmservices;
    find_services (PEGASUS_SERVICENAME_HANDLERMANAGER, 0, 0, &hmservices);
    pegasus_yield ();
    if (hmservices.size () > 0)
    {
        _handlerService = hmservices [0];

        //
        //  ATTN: What should be done if more than one service is found??
        //
    }
    else
    {
        //
        //  ATTN: What should be done if no service is found??
        //
    }

    //
    //  FUTURE: Add code to find repository service, if repository becomes a 
    //  service
    //

    //
    //  Make sure subscription classes include Creator property
    //  ATTN-CAKG-P1-20020325: To Be Removed -- the repository is to be 
    //  modified to allow addition of property to an instance, without adding 
    //  property to the class
    //
    _checkClasses ();

    //
    //  Get existing active subscriptions from each namespace in the repository
    //
    activeSubscriptions = _getActiveSubscriptions ();
    noProviderSubscriptions.clear ();

    String condition;
    String queryLanguage;
    CIMPropertyList propertyList;
    Array <ProviderClassList> indicationProviders;

    for (Uint8 i = 0; i < activeSubscriptions.size (); i++)
    {
        //
        //  Check for expired subscription
        //
        if ((activeSubscriptions [i].getInstance ().existsProperty 
               (_PROPERTY_DURATION)) &&
            (_isExpired (activeSubscriptions [i].getInstance ())))
        {
            _deleteExpiredSubscription 
                (activeSubscriptions [i].getInstanceName ().getNameSpace (),
                activeSubscriptions [i].getInstanceName ());
                
            continue;
        }

        _getEnableParams 
            (activeSubscriptions [i].getInstanceName ().getNameSpace (),
            activeSubscriptions [i].getInstance (), indicationProviders,
            propertyList, condition, queryLanguage);

        if (indicationProviders.size () == 0)
        {
            //
            //  There are no providers that can support this subscription
            //  Append this subscription to no provider list
            //
            noProviderSubscriptions.append (activeSubscriptions [i]);
            continue;
        }
    
        //
        //  Send enable request message to each provider
        //  ATTN-CAKG-P3-20020315: These enable requests are not associated 
        //  with a user request, so there is no associated authType or userName
        //  The Creator from the subscription instance is used for userName,
        //  and authType is not set
        //
        CIMInstance instance = activeSubscriptions [i].getInstance ();
        String creator = instance.getProperty (instance.findProperty
            (_PROPERTY_CREATOR)).getValue ().toString ();
        if (!_sendEnableRequests (indicationProviders, 
            activeSubscriptions [i].getInstanceName ().getNameSpace (),
            propertyList, condition, queryLanguage,
            activeSubscriptions [i], creator))
        {
            noProviderSubscriptions.append (activeSubscriptions [i]);
            continue;
        }

        //
        //  Merge provider list into list of unique providers to start
        //
        for (Uint8 j = 0; j < indicationProviders.size (); j++)
        {
            duplicate = false;
            for (Uint8 k = 0; k < startProviders.size () && !duplicate; k++)
            {
                if (indicationProviders [j].provider == startProviders [k])
                {
                    duplicate = true;
                }
            }
    
            if (!duplicate)
            {
                startProviders.append (indicationProviders [j].provider);
            }
        }
    }  // for each active subscription

    //
    //  Send alerts for any subscriptions for which there is no longer any
    //  provider
    //
    if (noProviderSubscriptions.size () > 0)
    {
        //
        //  Send NoProviderAlertIndication to handler instances
        //  ATTN: NoProviderAlertIndication must be defined
        //
        CIMInstance indicationInstance = _createAlertInstance
            (_CLASS_NO_PROVIDER_ALERT, noProviderSubscriptions);

        _sendAlerts (noProviderSubscriptions, indicationInstance);
    }

    //
    //  Send start message to each provider
    //
    for (Uint8 m = 0; m < startProviders.size (); m++)
    {
        //
        //  ATTN: start message has not yet been defined
        //
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_terminate (void)
{
    Array <CIMNamedInstance> activeSubscriptions;
    CIMInstance indicationInstance;

    const char METHOD_NAME [] = "IndicationService::_terminate";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get existing active subscriptions from each namespace in the repository
    //
    activeSubscriptions = _getActiveSubscriptions ();

    //
    //  Create CimomShutdownAlertIndication instance
    //  ATTN: CimomShutdownAlertIndication must be defined
    //
    indicationInstance = _createAlertInstance (_CLASS_CIMOM_SHUTDOWN_ALERT,
        activeSubscriptions);

    //
    //  Send CimomShutdownAlertIndication to each unique handler instance
    //
    _sendAlerts (activeSubscriptions, indicationInstance);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleCreateInstanceRequest (const Message * message)
{
    const char METHOD_NAME [] = 
        "IndicationService::_handleCreateInstanceRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMCreateInstanceRequestMessage* request = 
        (CIMCreateInstanceRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    CIMReference instanceRef;

    CIMInstance instance = request->newInstance.clone ();

    try
    {
        if (_canCreate (instance, request->nameSpace))
        {
            //
            //  Add creator property to Instance
            //  NOTE: userName is only set in the request if authentication 
            //  is turned on
            //
            String currentUser = request->userName;
            try
            {
                instance.addProperty (CIMProperty 
                    (_PROPERTY_CREATOR, currentUser));
            }
            catch (NoSuchProperty & e)
            {
                //
                //  If the property does not exist, add it to the class
                //  ATTN: To Be Removed -- the repository is to be modified to 
                //  allow addition of property to an instance, without adding 
                //  property to class
                //
                CIMClass theClass = _repository->getClass
                    (request->nameSpace, instance.getClassName ());
                theClass.addProperty (CIMProperty (_PROPERTY_CREATOR,
                    CIMValue (String::EMPTY)));
    
                _repository->write_lock ();
    
                try
                {
                    _repository->modifyClass (request->nameSpace,
                        theClass);
                }
                catch (Exception & exception)
                {
                    //
                    //  ATTN: Log a message??
                    //
                }
    
                _repository->write_unlock ();

                instance.addProperty (CIMProperty 
                    (_PROPERTY_CREATOR, currentUser));
            }
    
            //
            //  If the instance is of the CIM_IndicationSubscription class
            //  and subscription state is enabled, determine if any providers
            //  can serve the subscription
            //
            Uint16 subscriptionState;
            String condition;
            String queryLanguage;
            CIMPropertyList requiredProperties;
            Array <ProviderClassList> indicationProviders;
            if (instance.getClassName () == PEGASUS_CLASSNAME_INDSUBSCRIPTION)
            {
                //
                //  Get subscription state
                //
                CIMValue subscriptionStateValue;
                subscriptionStateValue = instance.getProperty
                    (instance.findProperty (_PROPERTY_STATE)).getValue ();
                subscriptionStateValue.get (subscriptionState);

                if ((subscriptionState == _STATE_ENABLED) ||
                    (subscriptionState == _STATE_ENABLEDDEGRADED))
                {
    
                    _getEnableParams (request->nameSpace, instance,
                        indicationProviders, requiredProperties, condition, 
                        queryLanguage);
    
                    if (indicationProviders.size () == 0)
                    {
                        //
                        //  There are no providers that can support this 
                        //  subscription
                        //
                        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                            _MSG_NO_PROVIDERS);
                    }
                }
    
                //
                //  Set Time of Last State Change to current date time
                //  ATTN: need method to get current date time in CIMDateTime 
                //  format
                //
                CIMDateTime currentDateTime = CIMDateTime ();
                if (!instance.existsProperty (_PROPERTY_LASTCHANGE))
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
                if ((subscriptionState == _STATE_ENABLED) ||
                    (subscriptionState == _STATE_ENABLEDDEGRADED))
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
                if (!instance.existsProperty (_PROPERTY_STARTTIME))
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
                instanceRef = _repository->createInstance (request->nameSpace, 
                    instance);
            }
            catch (Exception & exception)
            {
                //
                //  Unlock repository in case of exception
                //
                errorCode = CIM_ERR_FAILED;
                errorDescription = exception.getMessage ();
                _repository->write_unlock ();
                CIMCreateInstanceResponseMessage* response =
                    new CIMCreateInstanceResponseMessage(
                        request->messageId,
                        errorCode,
                        errorDescription,
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
    
                Base::_enqueueResponse (request, response);
    
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                return;
            }
        
            _repository->write_unlock ();
        
            //
            //  If the instance is of the CIM_IndicationSubscription class
            //  and subscription state is enabled, send enable request to 
            //  indication providers
            //
            if (instance.getClassName () == PEGASUS_CLASSNAME_INDSUBSCRIPTION)
            {
                if ((subscriptionState == _STATE_ENABLED) ||
                    (subscriptionState == _STATE_ENABLEDDEGRADED))
                {
                    //
                    //  Send enable request message to each provider
                    //
                    instanceRef.setNameSpace (request->nameSpace);
                    if (!_sendEnableRequests (indicationProviders, 
                        request->nameSpace, requiredProperties, condition, 
                        queryLanguage, 
                        CIMNamedInstance (instanceRef, instance),
                        request->userName, request->authType))
                    {
                        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                            _MSG_NOT_ACCEPTED);
                    }
            
                    //
                    //  Send start message to each provider
                    //
                    for (Uint8 i = 0; i < indicationProviders.size (); i++)
                    {
                        //
                        //  ATTN: start message has not yet been defined
                        //
                    }
                }
            }
        }
    }
    catch (CIMException & exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception & exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMCreateInstanceResponseMessage* response =
        new CIMCreateInstanceResponseMessage(
            request->messageId,
            errorCode,
            errorDescription,
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
    
    Base::_enqueueResponse (request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleGetInstanceRequest (const Message* message)
{
    const char METHOD_NAME [] = "IndicationService::_handleGetInstanceRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMGetInstanceRequestMessage* request = 
        (CIMGetInstanceRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance instance;

    try
    {
        //
        //  Get instance from repository
        //
        instance = _repository->getInstance (request->nameSpace, 
            request->instanceName, request->localOnly, 
            request->includeQualifiers, request->includeClassOrigin, 
            request->propertyList);

        //
        //  Remove Creator property from instance before returning
        //
        instance.removeProperty (instance.findProperty (_PROPERTY_CREATOR));

        //
        //  If a subscription with a duration, calculate subscription time 
        //  remaining, and add property to the instance
        //
        if (request->instanceName.getClassName () == 
            PEGASUS_CLASSNAME_INDSUBSCRIPTION)
        {
            if (instance.existsProperty (_PROPERTY_DURATION))
            {
                _setTimeRemaining (instance);
            }
        }
    }
    catch (CIMException& exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMGetInstanceResponseMessage* response = new CIMGetInstanceResponseMessage
        (request->messageId,
        errorCode,
        errorDescription,
        request->queueIds.copyAndPop(),
        instance);

    //
    //  Preserve message key
    //
    response->setKey (request->getKey ());
    
    //
    //  Set response destination
    //
    response->dest = request->queueIds.top ();
    
    Base::_enqueueResponse (request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleEnumerateInstancesRequest(const Message* message)
{
    const char METHOD_NAME [] = 
        "IndicationService::_handleEnumerateInstancesRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMEnumerateInstancesRequestMessage* request = 
        (CIMEnumerateInstancesRequestMessage*) message;

    Array <CIMNamedInstance> enumInstances;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance cimInstance;

    try
    {
        enumInstances = _repository->enumerateInstances (request->nameSpace, 
            request->className, request->deepInheritance, request->localOnly, 
            request->includeQualifiers, request->includeClassOrigin, 
            request->propertyList);
        
        //
        //  Remove Creator property from instances before returning
        //
        for (Uint8 i = 0; i < enumInstances.size (); i++)
        {
            enumInstances [i].getInstance ().removeProperty 
                (enumInstances [i].getInstance ().findProperty 
                (_PROPERTY_CREATOR));

            //
            //  If a subscription with a duration, calculate subscription time 
            //  remaining, and add property to the instance
            //
            if (request->className == PEGASUS_CLASSNAME_INDSUBSCRIPTION)
            {
                if (enumInstances [i].getInstance ().existsProperty 
                    (_PROPERTY_DURATION))
                {
                    _setTimeRemaining (enumInstances [i].getInstance ());
                }
            }
        }
    }
    catch (CIMException& exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMEnumerateInstancesResponseMessage* response = 
        new CIMEnumerateInstancesResponseMessage(
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop(),
            enumInstances);

    //
    //  Preserve message key
    //
    response->setKey (request->getKey ());
    
    //
    //  Set response destination
    //
    response->dest = request->queueIds.top ();
    
    Base::_enqueueResponse (request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleEnumerateInstanceNamesRequest
    (const Message* message)
{
    const char METHOD_NAME [] = 
        "IndicationService::_handleEnumerateInstancesNamesRequest";
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);
   
    CIMEnumerateInstanceNamesRequestMessage* request =
        (CIMEnumerateInstanceNamesRequestMessage*) message;

    Array<CIMReference> enumInstanceNames;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
        enumInstanceNames = _repository->enumerateInstanceNames 
            (request->nameSpace, request->className);
    }
    catch (CIMException& exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMEnumerateInstanceNamesResponseMessage* response =
        new CIMEnumerateInstanceNamesResponseMessage(
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop(),
            enumInstanceNames);

    // preserve message key
    response->setKey(request->getKey());

    
    //  Set the response destination !!!
    response->dest = request->queueIds.top();
    
    // Note: In this particular case you can call either SendForget
    // OR Base::_enqueueResponse
    //    SendForget(response);
    Base::_enqueueResponse(request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleModifyInstanceRequest (const Message* message)
{
    const char METHOD_NAME [] = 
        "IndicationService::_handleModifyInstanceRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMModifyInstanceRequestMessage* request = 
        (CIMModifyInstanceRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    
    try
    {
        //
        //  Get the instance name
        //
        CIMReference instanceReference = 
            request->modifiedInstance.getInstanceName ();
    
        //
        //  Get instance from repository
        //
        CIMInstance instance = _repository->getInstance
            (request->nameSpace, instanceReference);

        if (_canModify (request, instanceReference, instance))
        {
            //
            //  Check for expired subscription
            //
            if ((instance.existsProperty (_PROPERTY_DURATION)) &&
                (_isExpired (instance)))
            {
                //
                //  Delete the subscription instance
                //
                _deleteExpiredSubscription (request->nameSpace,
                    instanceReference);
    
                String exceptionStr = _MSG_EXPIRED;
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
            }
    
            //
            //  _canModify, above, already checked that propertyList is not 
            //  null, and that numProperties is 0 or 1
            //
            if (request->propertyList.getNumProperties () > 0)
            {
                //
                //  Get current state from instance
                //
                CIMValue subscriptionStateValue;
                Uint16 currentState;
                subscriptionStateValue = instance.getProperty 
                    (instance.findProperty (_PROPERTY_STATE)).getValue ();
                subscriptionStateValue.get (currentState);
        
                //
                //  Get new state 
                //
                Uint16 newState;
                subscriptionStateValue = 
                    request->modifiedInstance.getInstance ().getProperty
                    (request->modifiedInstance.getInstance ().findProperty
                    (_PROPERTY_STATE)).getValue ();
    
                subscriptionStateValue.get (newState);
        
                //
                //  If Subscription State has changed,
                //  Set Time of Last State Change to current date time
                //  ATTN: need method to get current date time in CIMDateTime 
                //  format
                //
                CIMDateTime currentDateTime = CIMDateTime ();
                if (newState != currentState)
                {
                    CIMProperty lastChange = instance.getProperty
                        (instance.findProperty (_PROPERTY_LASTCHANGE));
                    lastChange.setValue (CIMValue (currentDateTime));
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
                    if (startTimeValue.isNull ())
                    {
                        startTimeProperty.setValue (CIMValue (currentDateTime));
                    }
                    else
                    {
                        startTimeValue.get (startTime);
                        if (startTime.isNull ())
                        {
                            startTimeProperty.setValue 
                                (CIMValue (currentDateTime));
                        }
                    }
                }
    
                //
                //  If subscription is to be enabled, determine if there are 
                //  any indication providers that can serve the subscription
                //
                Array <ProviderClassList> indicationProviders;
                CIMPropertyList requiredProperties;
                String condition;
                String queryLanguage;
        
                if (((newState == _STATE_ENABLED) || 
                     (newState == _STATE_ENABLEDDEGRADED))
                    && ((currentState != _STATE_ENABLED) && 
                        (currentState != _STATE_ENABLEDDEGRADED)))
                {
                    //
                    //  Subscription was previously not enabled but is now to 
                    //  be enabled
                    //
                    _getEnableParams (request->nameSpace, instance,
                        indicationProviders, requiredProperties, condition, 
                        queryLanguage);
    
                    if (indicationProviders.size () == 0)
                    {
                        //
                        //  There are no providers that can support this 
                        //  subscription
                        //
                        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                            _MSG_NO_PROVIDERS);
                    }
                }
    
                //
                //  Modify the instance in the repository
                //
                _repository->write_lock ();
    
                try
                {
                    _repository->modifyInstance (request->nameSpace,
                        CIMNamedInstance (instanceReference, 
                        request->modifiedInstance.getInstance ()), 
                        request->includeQualifiers, request->propertyList);
                }
                catch (Exception & exception)
                {
                    //
                    //  Unlock repository in case of exception
                    //
                    _repository->write_unlock ();
    
                    errorCode = CIM_ERR_FAILED;
                    errorDescription = exception.getMessage ();
    
                    CIMModifyInstanceResponseMessage* response =
                        new CIMModifyInstanceResponseMessage (
                            request->messageId,
                            errorCode,
                            errorDescription,
    	                request->queueIds.copyAndPop ());
        
                    //
                    //  Preserve message key
                    //
                    response->setKey (request->getKey ());
    
                    //
                    //  Set response destination
                    //
                    response->dest = request->queueIds.top ();
    
                    Base::_enqueueResponse (request, response);
        
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    return;
                }
                
               _repository->write_unlock ();
    
                //
                //  If subscription is newly enabled, send enable requests
                //  and start providers
                //
                if (((newState == _STATE_ENABLED) || 
                     (newState == _STATE_ENABLEDDEGRADED))
                    && ((currentState != _STATE_ENABLED) && 
                        (currentState != _STATE_ENABLEDDEGRADED)))
                {
                    instanceReference.setNameSpace (request->nameSpace);
                    if (!_sendEnableRequests (indicationProviders, 
                        request->nameSpace, requiredProperties, condition, 
                        queryLanguage,
                        CIMNamedInstance (instanceReference, instance),
                        request->userName, request->authType))
                    {
                        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                            _MSG_NOT_ACCEPTED);
                    }
    
                    //
                    //  Send start message to each provider
                    //
                    for (Uint8 i = 0; i < indicationProviders.size (); i++)
                    {
                        //
                        //  ATTN: start message has not yet been defined
                        //
                    }
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
                    indicationProviders = _getDisableParams 
                        (request->nameSpace, instance);
    
                    //
                    //  Send disable requests
                    //
                    if (indicationProviders.size () > 0)
                    {
                        instanceReference.setNameSpace (request->nameSpace);
                        _sendDisableRequests (indicationProviders, 
                            request->nameSpace,
                            CIMNamedInstance (instanceReference, instance),
                            request->userName, request->authType);
                    }
                }
            }
        }
    }
    catch (CIMException& exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMModifyInstanceResponseMessage* response =
        new CIMModifyInstanceResponseMessage(
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop());

    //
    //  Preserve message key
    //
    response->setKey (request->getKey ());
    
    //
    //  Set response destination
    //
    response->dest = request->queueIds.top ();
    
    Base::_enqueueResponse (request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleDeleteInstanceRequest (const Message* message)
{
    const char METHOD_NAME [] = 
        "IndicationService::_handleDeleteInstanceRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMDeleteInstanceRequestMessage* request = 
        (CIMDeleteInstanceRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
        //
        //  Check if instance may be deleted -- a filter or handler instance 
        //  referenced by a subscription instance may not be deleted
        //
        if (_canDelete (request->instanceName, request->nameSpace,
            request->userName))
        {
            if (request->instanceName.getClassName () == 
                PEGASUS_CLASSNAME_INDSUBSCRIPTION)
            {
                CIMInstance subscriptionInstance;
                Array <ProviderClassList> indicationProviders;

                subscriptionInstance = _repository->getInstance 
                    (request->nameSpace, request->instanceName);

                indicationProviders = _getDisableParams 
                    (request->nameSpace, subscriptionInstance);

                //
                //  Send disable requests
                //
                CIMReference instanceReference = request->instanceName;
                instanceReference.setNameSpace (request->nameSpace);
                _sendDisableRequests (indicationProviders,
                    request->nameSpace, CIMNamedInstance 
                    (instanceReference, subscriptionInstance),
                    request->userName, request->authType);
            }

            //
            //  Delete instance from repository
            //
            _repository->write_lock ();

            try
            {
                _repository->deleteInstance (request->nameSpace, 
                    request->instanceName);
            }
            catch (Exception & exception)
            {
                //
                //  Unlock repository in case of exception
                //
                _repository->write_unlock ();

                errorCode = CIM_ERR_FAILED;
                errorDescription = exception.getMessage ();

                CIMDeleteInstanceResponseMessage* response =
                    new CIMDeleteInstanceResponseMessage (
                        request->messageId,
                        errorCode,
                        errorDescription,
                        request->queueIds.copyAndPop ());

                //
                //  Preserve message key
                //
                response->setKey (request->getKey ());

                //
                //  Set response destination
                //
                response->dest = request->queueIds.top ();

                Base::_enqueueResponse (request, response);

                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                return;
            }

            _repository->write_unlock ();
        }
    }
    catch (CIMException& exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMDeleteInstanceResponseMessage* response =
        new CIMDeleteInstanceResponseMessage(
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop());

    //
    //  Preserve message key
    //
    response->setKey (request->getKey ());
    
    //
    //  Set response destination
    //
    response->dest = request->queueIds.top ();
    
    Base::_enqueueResponse (request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleProcessIndicationRequest (const Message* message)
{
    const char METHOD_NAME [] =
        "IndicationService::_handleProcessIndicationRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMProcessIndicationRequestMessage* request =
        (CIMProcessIndicationRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    CIMProcessIndicationResponseMessage* response =
        new CIMProcessIndicationResponseMessage(
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop());

    String filterQuery;
    Array<String> propertyList;
    Boolean match;

    Array <CIMNamedInstance> matchedSubscriptions;
    CIMNamedInstance handlerNamedInstance;

    WQLSelectStatement selectStatement;

    CIMInstance handler;
    CIMInstance indication = request->indicationInstance;
    
    try
    {
        WQLSimplePropertySource propertySource = _getPropertySourceFromInstance(
            indication);

        for (Uint8 i = 0; i < indication.getPropertyCount(); i++)
            propertyList.append(indication.getProperty(i).getName());

        Array <String> nameSpaces;
        nameSpaces.append (request->nameSpace);
        matchedSubscriptions = _getMatchingSubscriptions(
            indication.getClassName (), nameSpaces, 
            CIMPropertyList (propertyList));

        for (Uint8 i = 0; i < matchedSubscriptions.size(); i++)
        {
            match = true;

            //
            //  Check for expired subscription
            //
            if ((matchedSubscriptions [i].getInstance ().existsProperty
                   (_PROPERTY_DURATION)) &&
                (_isExpired (matchedSubscriptions [i].getInstance ())))
            {
                _deleteExpiredSubscription
                    (matchedSubscriptions [i].getInstanceName ().getNameSpace(),
                     matchedSubscriptions [i].getInstanceName ());
    
                continue;
            }

            _getFilterProperties (
                matchedSubscriptions[i].getInstance (),
                matchedSubscriptions[i].getInstanceName ().getNameSpace (),
                filterQuery);

            selectStatement = _getSelectStatement (filterQuery);

            if (selectStatement.hasWhereClause())
            {
                if (!selectStatement.evaluateWhereClause(&propertySource))
                {
                    match = false;
                }
            }

            if (match)
            {
                 handlerNamedInstance = _getHandler
                     (matchedSubscriptions[i]);

                 CIMRequestMessage * handler_request =
                     new CIMHandleIndicationRequestMessage (
                         XmlWriter::getNextMessageId (),
                         request->nameSpace,
                         handlerNamedInstance.getInstance (),
                         indication,
                         QueueIdStack(_handlerService, getQueueId()));
                
                 AsyncOpNode* op = this->get_op();

                 AsyncLegacyOperationStart *async_req = 
                    new AsyncLegacyOperationStart(
                     get_next_xid(),
                     op,
                     _handlerService,
                     handler_request,
                     _queueId);

                 //AsyncReply *async_reply = SendWait(async_req);
                 SendForget(async_req);

                //
                //  ATTN-CAKG-P1-20020326: Check for error - implement 
                //  subscription's OnFatalErrorPolicy
                //

                 //response = reinterpret_cast<CIMProcessIndicationResponseMessage *>
                 //    ((static_cast<AsyncLegacyOperationResult *>(async_reply))->res);

                 delete async_req;
                 //delete async_reply;
            }
        }
    }
    catch (CIMException& exception)
    {
        response->errorCode = exception.getCode();
        response->errorDescription = exception.getMessage();
    }
    catch (Exception& exception)
    {
        response->errorCode = CIM_ERR_FAILED;
        response->errorDescription = exception.getMessage();
    }

    _enqueueResponse(request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);

    return;
}

void IndicationService::_handleNotifyProviderRegistrationRequest
    (const Message* message)
{
    const char METHOD_NAME [] = 
        "IndicationService::_handleNotifyProviderRegistrationRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);
//cout << "IndicationService::_handleNotifyProviderRegistrationRequest" << endl;

    CIMNotifyProviderRegistrationRequestMessage* request = 
        (CIMNotifyProviderRegistrationRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    CIMInstance provider = request->provider;
    CIMInstance providerModule = request->providerModule;
    String className = request->className;
    Array <String> newNameSpaces = request->newNamespaces;
    Array <String> oldNameSpaces = request->oldNamespaces;
    CIMPropertyList newPropertyNames = request->newPropertyNames;
    CIMPropertyList oldPropertyNames = request->oldPropertyNames;

    Array <CIMNamedInstance> newSubscriptions;
    Array <CIMNamedInstance> formerSubscriptions;
    Array <ProviderClassList> indicationProviders;
    ProviderClassList indicationProvider;

    newSubscriptions.clear ();
    formerSubscriptions.clear ();

    switch (request->operation)
    {
        case OP_CREATE:
        {
//cout << "OP_CREATE" << endl;
            //
            //  Get matching subscriptions
            //
            newSubscriptions = _getMatchingSubscriptions (className, 
                newNameSpaces, newPropertyNames);

            break;
        }

        case OP_DELETE:
        {
//cout << "OP_DELETE" << endl;
            //
            //  Get matching subscriptions
            //
            formerSubscriptions = _getMatchingSubscriptions (className, 
                oldNameSpaces, oldPropertyNames);

            break;
        }

        case OP_MODIFY:
        {
//cout << "OP_MODIFY" << endl;
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
            PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
            throw CIMException (CIM_ERR_NOT_SUPPORTED);
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

//cout << "newSubscriptions.size (): " << newSubscriptions.size () << endl;
        //
        //  Send enable or modify request for each subscription that can newly 
        //  be supported
        //
        for (Uint8 i = 0; i < newSubscriptions.size (); i++)
        {
            _getEnableParams 
                (newSubscriptions [i].getInstanceName ().getNameSpace (), 
                newSubscriptions [i].getInstance (), 
                requiredProperties, condition, queryLanguage);

            //
            //  ATTN-CAKG-P3-20020315: These enable or modify requests are not 
            //  associated with a user request, so there is no associated 
            //  authType or userName
            //  The Creator from the subscription instance is used for 
            //  userName, and authType is not set
            //
            CIMInstance instance = newSubscriptions [i].getInstance ();
            String creator = instance.getProperty (instance.findProperty
                (_PROPERTY_CREATOR)).getValue ().toString ();

            //
            //  Look up the subscription-provider pair in the subscription table
            //  If the entry is there, send a modify request, 
            //  Otherwise, send an enable request
            //
            String tableKey = _generateKey 
                (newSubscriptions [i], indicationProviders [i].provider);
            if (_subscriptionTable.contains (tableKey))
            {
//cout << "table contains key" << endl;
                //
                //  Send modify requests
                //
                _sendModifyRequests (indicationProviders,
                    newSubscriptions [i].getInstanceName ().getNameSpace (), 
                    requiredProperties, condition, queryLanguage, 
                    newSubscriptions [i], creator);
            }
            else
            {
//cout << "table does not contain key" << endl;
                //
                //  Send enable requests
                //
                if (!_sendEnableRequests (indicationProviders,
                    newSubscriptions [i].getInstanceName ().getNameSpace (), 
                    requiredProperties, condition, queryLanguage, 
                    newSubscriptions [i], creator))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED, 
                        _MSG_NOT_ACCEPTED);
                }
    
                //
                //  Send start message to each provider
                //
                for (Uint8 j = 0; j < indicationProviders.size (); j++)
                {
                    //
                    //  ATTN: start message has not yet been defined
                    //
                }
            }
        }
    }

    if (formerSubscriptions.size () > 0)
    {
//cout << "formerSubscriptions.size (): " << formerSubscriptions.size () << endl;
        CIMPropertyList requiredProperties;
        String condition;
        String queryLanguage;

        //
        //  Send disable or modify request for each subscription that can no 
        //  longer be supported
        //
        for (Uint8 i = 0; i < formerSubscriptions.size (); i++)
        {
            //  ATTN-CAKG-P3-20020315: These disable or modify requests are not 
            //  associated with a user request, so there is no associated 
            //  authType or userName
            //  The Creator from the subscription instance is used for userName,
            //  and authType is not set
            CIMInstance instance = formerSubscriptions [i].getInstance ();
            String creator = instance.getProperty (instance.findProperty
                (_PROPERTY_CREATOR)).getValue ().toString ();

            //
            //  Look up the subscription-provider pair in the subscription table
            //  If the entry is there, send a modify request, 
            //  Otherwise, send an enable request
            //
            String tableKey = _generateKey 
                (formerSubscriptions [i], indicationProviders [i].provider);
            if (_subscriptionTable.contains (tableKey))
            {
//cout << "table contains key" << endl;
                _getEnableParams 
                    (formerSubscriptions [i].getInstanceName ().getNameSpace (),
                    formerSubscriptions [i].getInstance (), 
                    requiredProperties, condition, queryLanguage);

                //
                //  Send modify requests
                //
                _sendModifyRequests (indicationProviders,
                    formerSubscriptions [i].getInstanceName ().getNameSpace (), 
                    requiredProperties, condition, queryLanguage, 
                    formerSubscriptions [i], creator);
            }
            else
            {
//cout << "table does not contain key" << endl;
                _sendDisableRequests (indicationProviders,
                    formerSubscriptions [i].getInstanceName ().getNameSpace (),
                    formerSubscriptions [i], creator);
            }
        }

        //
        //  ATTN: Should alert always be sent, or only in the case 
        //  that there are no other providers that can satisfy any
        //  of the subscription indication subclasses??
        //

        //
        //  Create NoProviderAlertIndication instance
        //  ATTN: NoProviderAlertIndication must be defined
        //
        CIMInstance indicationInstance = _createAlertInstance 
            (_CLASS_NO_PROVIDER_ALERT, formerSubscriptions);
    
        //
        //  Send NoProviderAlertIndication to each unique handler instance
        //
        _sendAlerts (formerSubscriptions, indicationInstance);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleNotifyProviderTerminationRequest
    (const Message * message)
{
    Array <CIMNamedInstance> providerSubscriptions;
    CIMInstance indicationInstance;

    const char METHOD_NAME [] = 
        "IndicationService::_handleNotifyProviderTermination";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMNotifyProviderTerminationRequestMessage* request = 
	(CIMNotifyProviderTerminationRequestMessage*) message;

    CIMInstance provider = request->provider;

    //
    //  Get list of affected subscriptions
    //
    providerSubscriptions.clear ();
    providerSubscriptions = _getProviderSubscriptions (provider);

    //
    //  ATTN: Should alert always be sent, or only in the case 
    //  that there are no other providers that can satisfy any
    //  of the subscription indication subclasses??
    //

    //
    //  Create ProviderTerminatedAlertIndication instance
    //  ATTN: ProviderTerminatedAlertIndication must be defined
    //
    indicationInstance = _createAlertInstance 
        (_CLASS_PROVIDER_TERMINATED_ALERT, providerSubscriptions);

    //
    //  Send ProviderTerminatedAlertIndication to each unique handler instance
    //
    _sendAlerts (providerSubscriptions, indicationInstance);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

//
//  ATTN-CAKG-P1-20020325: To Be Removed -- the repository is to be 
//  modified to allow addition of property to an instance, without adding 
//  property to the class
//
void IndicationService::_checkClasses (void)
{
    const char METHOD_NAME [] = "IndicationService::_checkClasses";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get list of namespaces in repository
    //
    Array <String> nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Check for subscription classes in each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {
        //
        //  Make sure subscription classes include Creator property
        //
        try
        {
            CIMClass subscriptionClass = _repository->getClass 
                (nameSpaceNames [i], PEGASUS_CLASSNAME_INDSUBSCRIPTION);
            if (!subscriptionClass.existsProperty (_PROPERTY_CREATOR))
            {
                subscriptionClass.addProperty (CIMProperty (_PROPERTY_CREATOR,
                    CIMValue (String::EMPTY)));

                _repository->write_lock ();

                try
                {
                    _repository->modifyClass (nameSpaceNames [i], 
                        subscriptionClass);
                }
                catch (Exception & exception)
                {
                    //
                    //  ATTN: Log a message??
                    //
                }

                _repository->write_unlock ();
            }
            CIMClass filterClass = _repository->getClass (nameSpaceNames [i], 
                PEGASUS_CLASSNAME_INDFILTER);
            if (!filterClass.existsProperty (_PROPERTY_CREATOR))
            {
                filterClass.addProperty (CIMProperty (_PROPERTY_CREATOR,
                    CIMValue (String::EMPTY)));

                _repository->write_lock ();

                try
                {
                    _repository->modifyClass (nameSpaceNames [i], filterClass);
                }
                catch (Exception & exception)
                {
                    //
                    //  ATTN: Log a message??
                    //
                }

                _repository->write_unlock ();
            }
            CIMClass cimxmlHandlerClass = _repository->getClass 
                (nameSpaceNames [i], PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
            if (!cimxmlHandlerClass.existsProperty (_PROPERTY_CREATOR))
            {
                cimxmlHandlerClass.addProperty (CIMProperty (_PROPERTY_CREATOR,
                    CIMValue (String::EMPTY)));

                _repository->write_lock ();

                try
                {
                    _repository->modifyClass (nameSpaceNames [i], 
                        cimxmlHandlerClass);
                }
                catch (Exception & exception)
                {
                    //
                    //  ATTN: Log a message??
                    //
                }

                _repository->write_unlock ();
            }
            CIMClass snmpHandlerClass = _repository->getClass 
                (nameSpaceNames [i], PEGASUS_CLASSNAME_INDHANDLER_SNMP);
            if (!snmpHandlerClass.existsProperty (_PROPERTY_CREATOR))
            {
                snmpHandlerClass.addProperty (CIMProperty (_PROPERTY_CREATOR,
                    CIMValue (String::EMPTY)));

                _repository->write_lock ();

                try
                {
                    _repository->modifyClass (nameSpaceNames [i], 
                        snmpHandlerClass);
                }
                catch (Exception & exception)
                {
                    //
                    //  ATTN: Log a message??
                    //
                }

                _repository->write_unlock ();
            }
        }
        catch (CIMException e)
        {
            //
            //  Some namespaces may not include the subscription class
            //  In that case, just continue with the next namespace
            //
            if (e.getCode () == CIM_ERR_INVALID_CLASS)
            {
                continue;
            }
            else
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw e;
            }
        }
    }
}

Boolean IndicationService::_canCreate (
    CIMInstance & instance,
    const String & nameSpace)
{
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;
    CIMValue repeatPolicyValue;
    Uint16 repeatNotificationPolicy;
    CIMValue errorPolicyValue;
    Uint16 onFatalErrorPolicy;
    CIMValue persistenceValue;
    Uint16 persistenceType;
    CIMValue nameSpaceValue;
    String sourceNameSpace;

    const char METHOD_NAME [] = "IndicationService::_canCreate";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    // REVIEW: Derived classes of CIM_IndicationSubscription not
    // handled. It is reasonable for a user to derive from this
    // class and add extra properties.

    // REVIEW: how does the provider manager know to forward
    // requests to this service? Is it by class name? If so,
    // shouldn't the provider use an is-a operator on the new
    // class?

    //
    //  Check all required properties exist
    //  For a property that has a default value, if it does not exist,
    //  add property with default value
    //
    if (instance.getClassName () == PEGASUS_CLASSNAME_INDSUBSCRIPTION)
    {
        //
        //  Filter and Handler are key properties for Subscription
        //  No other properties are required
        //
        if (!instance.existsProperty (_PROPERTY_FILTER))
        {
            String exceptionStr = _MSG_MISSING_REQUIRED;
            exceptionStr.append (_PROPERTY_FILTER);
            exceptionStr.append (_MSG_KEY_PROPERTY);
            PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
            throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                exceptionStr);
        }

        if (!instance.existsProperty (_PROPERTY_HANDLER))
        {
            String exceptionStr = _MSG_MISSING_REQUIRED;
            exceptionStr.append (_PROPERTY_HANDLER);
            exceptionStr.append (_MSG_KEY_PROPERTY);
            PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
            throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                exceptionStr);
        }

        //
        //  Default value for Subscription State is Enabled
        //
        if (!instance.existsProperty (_PROPERTY_STATE))
        {
            instance.addProperty (CIMProperty (_PROPERTY_STATE,
                CIMValue ((Uint16) _STATE_ENABLED)));
        }
        else
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = instance.getProperty
                (instance.findProperty (_PROPERTY_STATE)).getValue ();
            subscriptionStateValue.get (subscriptionState);

            //
            //  If Subscription State is Other, Other Subscription State
            //  property must exist
            //
            if ((subscriptionState == _STATE_OTHER) &&
                (!instance.existsProperty (_PROPERTY_OTHERSTATE)))
            {
                String exceptionStr = _MSG_MISSING_REQUIRED;
                exceptionStr.append (_PROPERTY_OTHERSTATE);
                exceptionStr.append (_MSG_PROPERTY);
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                    exceptionStr);
            }

            //
            //  If Subscription State is not Other, 
            //  Other Subscription State property must not exist
            //
            else if (instance.existsProperty (_PROPERTY_OTHERSTATE))
            {
                String exceptionStr = _PROPERTY_OTHERSTATE;
                exceptionStr.append (_MSG_PROPERTY_PRESENT);
                exceptionStr.append (_PROPERTY_STATE);
                exceptionStr.append (_MSG_VALUE_NOT);
                exceptionStr.append (_STATE_OTHER);
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                    exceptionStr);
            }
        }

        //
        //  Default value for Repeat Notification Policy is None
        //
        if (!instance.existsProperty (_PROPERTY_REPEATNOTIFICATIONPOLICY))
        {
            instance.addProperty (CIMProperty 
                (_PROPERTY_REPEATNOTIFICATIONPOLICY, 
                CIMValue ((Uint16) _POLICY_NONE)));
        }

        else
        {
            //
            //  Get Repeat Notification Policy
            //
            repeatPolicyValue = instance.getProperty (instance.findProperty
                (_PROPERTY_REPEATNOTIFICATIONPOLICY)).getValue ();
            repeatPolicyValue.get (repeatNotificationPolicy);

            //
            //  If Repeat Notification Policy is Other, 
            //  Other Repeat Notification Policy property must exist
            //
            if ((repeatNotificationPolicy == _POLICY_OTHER) &&
                (!instance.existsProperty 
                (_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY)))
            {
                String exceptionStr = _MSG_MISSING_REQUIRED;
                exceptionStr.append 
                    (_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY);
                exceptionStr.append (_MSG_PROPERTY);
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                    exceptionStr);
            }

            //
            //  If Repeat Notification Policy is not Other, 
            //  Other Repeat Notification Policy property must not exist
            //
            else if (instance.existsProperty 
                (_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY))
            {
                String exceptionStr = 
                    _PROPERTY_OTHERREPEATNOTIFICATIONPOLICY;
                exceptionStr.append (_MSG_PROPERTY_PRESENT);
                exceptionStr.append (_PROPERTY_REPEATNOTIFICATIONPOLICY);
                exceptionStr.append (_MSG_VALUE_NOT);
                exceptionStr.append (_POLICY_OTHER);
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                    exceptionStr);
            }
        }

        //
        //  Default value for On Fatal Error Policy is Ignore
        //
        if (!instance.existsProperty (_PROPERTY_ONFATALERRORPOLICY))
        {
            instance.addProperty (CIMProperty 
                (_PROPERTY_ONFATALERRORPOLICY, 
                CIMValue ((Uint16) _ERRORPOLICY_IGNORE)));
        }
        else
        {
            //
            //  Get On Fatal Error Policy
            //
            errorPolicyValue = instance.getProperty (instance.findProperty 
                (_PROPERTY_ONFATALERRORPOLICY)).getValue ();
            errorPolicyValue.get (onFatalErrorPolicy);

            //
            //  If On Fatal Error Policy is Other, Other On Fatal Error 
            //  Policy property must exist
            //
            if ((onFatalErrorPolicy == _ERRORPOLICY_OTHER) &&
                (!instance.existsProperty 
                (_PROPERTY_OTHERONFATALERRORPOLICY)))
            {
                String exceptionStr = _MSG_MISSING_REQUIRED;
                exceptionStr.append (_PROPERTY_OTHERONFATALERRORPOLICY);
                exceptionStr.append (_MSG_PROPERTY);
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                    exceptionStr);
            }

            //
            //  If On Fatal Error Policy is not Other, 
            //  Other On Fatal Error Policy property must not exist
            //
            else if (instance.existsProperty 
                (_PROPERTY_OTHERONFATALERRORPOLICY))
            {
                String exceptionStr = _PROPERTY_OTHERONFATALERRORPOLICY;
                exceptionStr.append (_MSG_PROPERTY_PRESENT);
                exceptionStr.append (_PROPERTY_ONFATALERRORPOLICY);
                exceptionStr.append (_MSG_VALUE_NOT);
                exceptionStr.append (_ERRORPOLICY_OTHER);
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                    exceptionStr);
            }
        }

    } 
    else  // Filter or Handler
    {
        //
        //  Name, CreationClassName, SystemName, and SystemCreationClassName
        //  are key properties for Filter and Handler  
        //  CreationClassName and Name must exist
        //  If others do not exist, add and set to default
        //
        if (!instance.existsProperty (_PROPERTY_NAME))
        {
            String exceptionStr = _MSG_MISSING_REQUIRED;
            exceptionStr.append (_PROPERTY_NAME);
            exceptionStr.append (_MSG_KEY_PROPERTY);
            PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
            throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                exceptionStr);
        }

        if (!instance.existsProperty (_PROPERTY_CREATIONCLASSNAME))
        {
            String exceptionStr = _MSG_MISSING_REQUIRED;
            exceptionStr.append (_PROPERTY_CREATIONCLASSNAME);
            exceptionStr.append (_MSG_KEY_PROPERTY);
            PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
            throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                exceptionStr);
        }

        if (!instance.existsProperty (_PROPERTY_SYSTEMNAME))
        {
            //
            //  Set System Name 
            //
            instance.addProperty (CIMProperty (_PROPERTY_SYSTEMNAME,
                System::getFullyQualifiedHostName ()));
        }

        if (!instance.existsProperty (_PROPERTY_SYSTEMCREATIONCLASSNAME))
        {
            //
            //  Set System Creation Class Name 
            //
            instance.addProperty (CIMProperty 
                (_PROPERTY_SYSTEMCREATIONCLASSNAME, 
                System::getSystemCreationClassName ()));
        }

        if (instance.getClassName () == PEGASUS_CLASSNAME_INDFILTER)
        {
            //
            //  Query and QueryLanguage properties are required for Filter
            //
            if (!instance.existsProperty (_PROPERTY_QUERY))
            {
                String exceptionStr = _MSG_MISSING_REQUIRED;
                exceptionStr.append (_PROPERTY_QUERY);
                exceptionStr.append (_MSG_PROPERTY);
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                    exceptionStr);
            }
            if (!instance.existsProperty (_PROPERTY_QUERYLANGUAGE))
            {
                String exceptionStr = _MSG_MISSING_REQUIRED;
                exceptionStr.append (_PROPERTY_QUERYLANGUAGE);
                exceptionStr.append (_MSG_PROPERTY);
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                    exceptionStr);
            }

            //
            //  Default value for Source Namespace is the namespace of the
            //  Filter registration
            //
            if (!instance.existsProperty (_PROPERTY_SOURCENAMESPACE))
            {
                instance.addProperty (CIMProperty 
                    (_PROPERTY_SOURCENAMESPACE, nameSpace));
                sourceNameSpace = nameSpace;
            }
            else
            {
                nameSpaceValue = instance.getProperty (instance.findProperty 
                    (_PROPERTY_SOURCENAMESPACE)).getValue ();
                nameSpaceValue.get (sourceNameSpace);
            }

            //
            //  Validate the query and indication class name
            //  An exception is thrown if the query is invalid or the class
            //  is not an indication class
            //
            String filterQuery = instance.getProperty (instance.findProperty
                (_PROPERTY_QUERY)).getValue ().toString ();
            WQLSelectStatement selectStatement = 
                _getSelectStatement (filterQuery);
            String indicationClassName = _getIndicationClassName 
                (selectStatement, sourceNameSpace);
        }

        //
        //  Currently only two direct subclasses of Indication handler 
        //  class are supported -- further subclassing is not currently 
        //  supported
        //
        else if ((instance.getClassName () == 
                  PEGASUS_CLASSNAME_INDHANDLER_CIMXML) ||
                 (instance.getClassName () == 
                  PEGASUS_CLASSNAME_INDHANDLER_SNMP))
        {
            //
            //  Default value for Persistence Type is Permanent
            //
            if (!instance.existsProperty (_PROPERTY_PERSISTENCETYPE))
            {
                instance.addProperty (CIMProperty 
                    (_PROPERTY_PERSISTENCETYPE, 
                    CIMValue ((Uint16) _PERSISTENCE_PERMANENT)));
            }
            else
            {
                //
                //  Get Persistence Type
                //
                persistenceValue = instance.getProperty 
                    (instance.findProperty 
                    (_PROPERTY_PERSISTENCETYPE)).getValue ();
                persistenceValue.get (persistenceType);

                //
                //  If Persistence Type is Other, Other Persistence Type
                //  property must exist
                //
                if ((persistenceType == _PERSISTENCE_OTHER) &&
                    (!instance.existsProperty 
                    (_PROPERTY_OTHERPERSISTENCETYPE)))
                {
                    String exceptionStr = _MSG_MISSING_REQUIRED;
                    exceptionStr.append (_PROPERTY_OTHERPERSISTENCETYPE);
                    exceptionStr.append (_MSG_PROPERTY);
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                        exceptionStr);
                }

                //
                //  If Persistence Type is not Other, 
                //  Other Persistence Type property must not exist
                //
                else if (instance.existsProperty 
                    (_PROPERTY_OTHERPERSISTENCETYPE))
                {
                    String exceptionStr = _PROPERTY_OTHERPERSISTENCETYPE;
                    exceptionStr.append (_MSG_PROPERTY_PRESENT);
                    exceptionStr.append (_PROPERTY_PERSISTENCETYPE);
                    exceptionStr.append (_MSG_VALUE_NOT);
                    exceptionStr.append (_PERSISTENCE_OTHER);
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                        exceptionStr);
                }
            }

            if (instance.getClassName () == PEGASUS_CLASSNAME_INDHANDLER_CIMXML)
            {
                //
                //  Destination property is required for CIMXML 
                //  Handler subclass
                //
                if (!instance.existsProperty (_PROPERTY_DESTINATION))
                {
                    String exceptionStr = _MSG_MISSING_REQUIRED;
                    exceptionStr.append (_PROPERTY_DESTINATION);
                    exceptionStr.append (_MSG_PROPERTY);
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                        exceptionStr);
                }
            }

            if (instance.getClassName () == PEGASUS_CLASSNAME_INDHANDLER_SNMP)
            {
                //
                //  Trap Destination property is required for SNMP 
                //  Handler subclass
                //
                if (!instance.existsProperty (_PROPERTY_TRAPDESTINATION))
                {
                    String exceptionStr = _MSG_MISSING_REQUIRED;
                    exceptionStr.append (_PROPERTY_TRAPDESTINATION);
                    exceptionStr.append (_MSG_PROPERTY);
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                        exceptionStr);
                }

                //
                //  SNMP Type property is required for SNMP Handler
                //
                if (!instance.existsProperty (_PROPERTY_SNMPTYPE))
                {
                    String exceptionStr = _MSG_MISSING_REQUIRED;
                    exceptionStr.append (_PROPERTY_SNMPTYPE);
                    exceptionStr.append (_MSG_PROPERTY);
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
                        exceptionStr);
                }
            }
        }

        else
        {
            //
            //  A class not currently served by the Indication Service
            //
            PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
            throw CIMException (CIM_ERR_NOT_SUPPORTED);
        }
    }

    return true;
    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

Boolean IndicationService::_canModify (
    const CIMModifyInstanceRequestMessage * request,
    const CIMReference & instanceReference,
    const CIMInstance & instance)
{
    const char METHOD_NAME [] = "IndicationService::_canModify";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Currently, only modification allowed is of Subscription State 
    //  property in Subscription class
    //
    if (instanceReference.getClassName () != PEGASUS_CLASSNAME_INDSUBSCRIPTION)
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_NOT_SUPPORTED);
    }

    if (request->includeQualifiers)
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Request is invalid if property list is null, meaning all properties
    //  are to be updated
    //
    if (request->propertyList.isNull ())
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Request is invalid if more than one property is specified
    //
    else if (request->propertyList.getNumProperties () > 1)
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  For request to be valid, zero or one property must be specified
    //  If one property specified, it must be Subscription State property
    //
    else if ((request->propertyList.getNumProperties () == 1) &&
             (request->propertyList.getPropertyName (0) != _PROPERTY_STATE))
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Get creator from instance
    //
    String creator = instance.getProperty (instance.findProperty 
        (_PROPERTY_CREATOR)).getValue ().toString ();

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
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_ACCESS_DENIED);
    }

    return true;
    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

Boolean IndicationService::_canDelete (
    const CIMReference & instanceReference,
    const String & nameSpace,
    const String & currentUser)
{
    String superClass;
    String propName;

    const char METHOD_NAME [] = "IndicationService::_canDelete";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get the instance to be deleted from the respository
    //
    CIMInstance instance = _repository->getInstance
        (nameSpace, instanceReference);

    //
    //  Get creator from instance
    //
    String creator = instance.getProperty (instance.findProperty 
        (_PROPERTY_CREATOR)).getValue ().toString ();

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
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_ACCESS_DENIED);
    }

    //
    //  Get the class and superclass of the instance to be deleted
    //
    CIMClass refClass = _repository->getClass (nameSpace,
        instanceReference.getClassName());

    superClass = refClass.getSuperClassName();

    //
    //  If the class or superclass is Filter or Handler, check for 
    //  subscription instances referring to the instance to be deleted
    //
    if ((superClass == PEGASUS_CLASSNAME_INDFILTER) ||
        (superClass == PEGASUS_CLASSNAME_INDHANDLER) ||
        (instanceReference.getClassName() == PEGASUS_CLASSNAME_INDFILTER) ||
        (instanceReference.getClassName() == PEGASUS_CLASSNAME_INDHANDLER))
    {
        if ((superClass == PEGASUS_CLASSNAME_INDFILTER) ||
            (instanceReference.getClassName() == PEGASUS_CLASSNAME_INDFILTER))
        {
            propName = _PROPERTY_FILTER;
        }
        else if (superClass == PEGASUS_CLASSNAME_INDHANDLER)
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
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                return true;
            }
        }

        //
        //  Get all the subscriptions from the respository
        //
        Array <CIMNamedInstance> subscriptions = 
            _repository->enumerateInstances (nameSpace, 
                PEGASUS_CLASSNAME_INDSUBSCRIPTION);

        CIMValue propValue;

        //
        //  Check each subscription for a reference to the instance to be 
        //  deleted
        //
        for (Uint8 i = 0; i < subscriptions.size(); i++)
        {
            //
            //  Get the subscription Filter or Handler property value
            //
            propValue = subscriptions[i].getInstance().getProperty
                (subscriptions[i].getInstance().findProperty
                (propName)).getValue();
            
            CIMReference ref;
            propValue.get (ref);

            //
            //  If the current subscription Filter or Handler is the instance 
            //  to be deleted, it may not be deleted
            //
            // ATTN: Can namespaces in the references cause comparison failure?
            //
            if (instanceReference == ref)
            {
                String exceptionStr = _MSG_REFERENCED;
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
            }
        }
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return true;
}


Array <CIMNamedInstance> IndicationService::_getActiveSubscriptions () const
{
    Array <CIMNamedInstance> activeSubscriptions;
    Array <String> nameSpaceNames;
    Array <CIMNamedInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;

    const char METHOD_NAME [] = 
        "IndicationService::_getActiveSubscriptions";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        try
        {
        subscriptions = _repository->enumerateInstances (nameSpaceNames [i], 
            PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        }
        catch (CIMException e)
        {
            //
            //  Some namespaces may not include the subscription class
            //  In that case, just continue with the next namespace
            //
            if (e.getCode () == CIM_ERR_INVALID_CLASS)
            {
                continue;
            }
            else
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw e;
            }
        }
    
        //
        //  Process each subscription
        //
        for (Uint8 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = 
                subscriptions [j].getInstance ().getProperty
                (subscriptions [j].getInstance ().findProperty 
                (_PROPERTY_STATE)).getValue ();
            subscriptionStateValue.get (subscriptionState);

    
            //
            //  Process each enabled subscription
            //
            if ((subscriptionState == _STATE_ENABLED) ||
                (subscriptionState == _STATE_ENABLEDDEGRADED))
            {
                //
                //  CIMNamedInstances returned from repository do not include 
                //  namespace
                //  Set namespace here
                //
                CIMReference instanceName = 
                    subscriptions [j].getInstanceName ();
                instanceName.setNameSpace (nameSpaceNames [i]);
                CIMNamedInstance currentInstance 
                    (instanceName, subscriptions [j].getInstance ());
                activeSubscriptions.append (currentInstance);
            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (activeSubscriptions);
}


Array <CIMNamedInstance> IndicationService::_getMatchingSubscriptions (
    const String & supportedClass,
    const Array <String> nameSpaces,
    const CIMPropertyList & supportedProperties) 
{
    Array <CIMNamedInstance> matchingSubscriptions;
    Array <String> nameSpaceNames;
    Array <CIMNamedInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;

    const char METHOD_NAME [] = 
        "IndicationService::_getMatchingSubscriptions";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        try
        {
        subscriptions = _repository->enumerateInstances (nameSpaceNames [i], 
            PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        }
        catch (CIMException e)
        {
            //
            //  Some namespaces may not include the subscription class
            //  In that case, just continue with the next namespace
            //
            if (e.getCode () == CIM_ERR_INVALID_CLASS)
            {
                continue;
            }
            else
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw e;
            }
        }
    
        //
        //  Process each subscription
        //
        for (Uint8 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = 
                subscriptions [j].getInstance ().getProperty
                (subscriptions [j].getInstance ().findProperty 
                (_PROPERTY_STATE)).getValue ();
            subscriptionStateValue.get (subscriptionState);
    
            //
            //  Process each enabled subscription
            //
            if ((subscriptionState == _STATE_ENABLED) ||
                (subscriptionState == _STATE_ENABLEDDEGRADED))
            {
                String filterQuery;
                WQLSelectStatement selectStatement;
                String indicationClassName;
                Array <String> indicationSubclasses;
                String sourceNameSpace;
                CIMPropertyList propertyList;
                Boolean match;

                //
                //  Get filter properties
                //
                _getFilterProperties (subscriptions [j].getInstance (),
                    nameSpaceNames [i], filterQuery, sourceNameSpace);
                selectStatement = _getSelectStatement (filterQuery);
            
                //
                //  Get indication class name from filter query (FROM clause)
                //
                indicationClassName = _getIndicationClassName (selectStatement,
                    nameSpaceNames [i]);
            
                //
                //  Get list of subclass names for indication class
                //
                indicationSubclasses = _repository->enumerateClassNames 
                    (nameSpaceNames [i], indicationClassName, true);
                indicationSubclasses.append (indicationClassName);

                //
                //  Is current subscription for the supported class and one of
                //  the supported namespaces?
                //
                if ((Contains (indicationSubclasses, supportedClass)) &&
                    (Contains (nameSpaces, sourceNameSpace)))
                {
                    match = true;

                    //
                    //  If supported properties is null (all properties)
                    //  the subscription can be supported
                    //
                    if (!supportedProperties.isNull ())
                    {
                        //
                        //  Get property list from filter query (FROM and 
                        //  WHERE clauses)
                        //
                        if ((selectStatement.getSelectPropertyNameCount () > 0)
                            ||
                            (selectStatement.getWherePropertyNameCount () > 0))
                        {
                            propertyList = _getPropertyList (selectStatement,
                                sourceNameSpace, indicationClassName);
                        }
                
                        //
                        //  If the subscription requires all properties,
                        //  but supported property list does not include all 
                        //  properties, the subscription cannot be supported
                        //
                        if (propertyList.isNull ())
                        {
                            match = false;
                        }
                        else 
                        {
                            //
                            //  Compare subscription property list
                            //  with supported property list
                            //
                            for (Uint8 k = 0; 
                                 k < propertyList.getNumProperties () && match; 
                                 k++)
                            {
                                if (!Contains 
                                    (supportedProperties.getPropertyNameArray(),
                                    propertyList.getPropertyName (k)))
                                {
                                    match = false;
                                }
                            }
                        }
                    }

                    //
                    //  Add current subscription to list
                    //
                    if (match)
                    {
                        //
                        //  CIMNamedInstances returned from repository do not 
                        //  include namespace  
                        //  Set namespace here
                        //
                        CIMReference instanceName = 
                            subscriptions [j].getInstanceName ();
                        instanceName.setNameSpace (nameSpaceNames [i]);
                        CIMNamedInstance currentInstance 
                            (instanceName, subscriptions [j].getInstance ());
                        matchingSubscriptions.append (currentInstance);
                    }
                }  // if subscription includes supported class
            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (matchingSubscriptions);
}

void IndicationService::_getModifiedSubscriptions (
    const String & supportedClass,
    const Array <String> & newNameSpaces,
    const Array <String> & oldNameSpaces,
    const CIMPropertyList & newProperties,
    const CIMPropertyList & oldProperties,
    Array <CIMNamedInstance> & newSubscriptions,
    Array <CIMNamedInstance> & formerSubscriptions)
{
    Array <String> nameSpaceNames;
    Array <CIMNamedInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;

    const char METHOD_NAME [] = 
        "IndicationService::_getModifiedSubscriptions";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    newSubscriptions.clear ();
    formerSubscriptions.clear ();

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        try
        {
        subscriptions = _repository->enumerateInstances (nameSpaceNames [i], 
            PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        }
        catch (CIMException e)
        {
            //
            //  Some namespaces may not include the subscription class
            //  In that case, just continue with the next namespace
            //
            if (e.getCode () == CIM_ERR_INVALID_CLASS)
            {
                continue;
            }
            else
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw e;
            }
        }
    
        //
        //  Process each subscription
        //
        for (Uint8 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = 
                subscriptions [j].getInstance ().getProperty
                (subscriptions [j].getInstance ().findProperty 
                (_PROPERTY_STATE)).getValue ();
            subscriptionStateValue.get (subscriptionState);
    
            //
            //  Process each enabled subscription
            //
            if ((subscriptionState == _STATE_ENABLED) ||
                (subscriptionState == _STATE_ENABLEDDEGRADED))
            {
                String filterQuery;
                WQLSelectStatement selectStatement;
                String indicationClassName;
                Array <String> indicationSubclasses;
                String sourceNameSpace;
                CIMPropertyList requiredProperties;
                Boolean newMatch;
                Boolean formerMatch;

                //
                //  Get filter properties
                //
                _getFilterProperties (subscriptions [j].getInstance (),
                    nameSpaceNames [i], filterQuery, sourceNameSpace);
                selectStatement = _getSelectStatement (filterQuery);
            
                //
                //  Get indication class name from filter query (FROM clause)
                //
                indicationClassName = _getIndicationClassName (selectStatement,
                    nameSpaceNames [i]);
            
                //
                //  Get list of subclass names for indication class
                //
                indicationSubclasses = _repository->enumerateClassNames 
                    (nameSpaceNames [i], indicationClassName, true);
                indicationSubclasses.append (indicationClassName);

                //
                //  Is current subscription for the supported class?
                //
                if (Contains (indicationSubclasses, supportedClass))
                {
                    newMatch = false;
                    formerMatch = false;

                    //
                    //  Get property list from filter query (FROM and WHERE 
                    //  clauses)
                    //
                    if ((selectStatement.getSelectPropertyNameCount () > 0) ||
                        (selectStatement.getWherePropertyNameCount () > 0))
                    {
                        requiredProperties = _getPropertyList (selectStatement,
                            sourceNameSpace, indicationClassName);
                    }

                    //
                    //  If source namespace is now supported, but previously 
                    //  was not, check if required properties are now supported
                    //
                    if ((Contains (newNameSpaces, sourceNameSpace)) &&
                        (!Contains (oldNameSpaces, sourceNameSpace)))
                    {
                        newMatch = _inPropertyList (requiredProperties, 
                            newProperties);
                    }

                    //
                    //  If source namespace was previously supported, but now 
                    //  is not, check if required properties were previously 
                    //  supported
                    //
                    else if ((Contains (oldNameSpaces, sourceNameSpace)) &&
                             (!Contains (newNameSpaces, sourceNameSpace)))
                    {
                        formerMatch = _inPropertyList (requiredProperties, 
                            oldProperties);
                    }

                    //
                    //  If source namespace was previously supported, and still 
                    //  is supported, check required properties 
                    //
                    else if ((Contains (newNameSpaces, sourceNameSpace)) &&
                        (Contains (oldNameSpaces, sourceNameSpace)))
                    {
                        newMatch = _inPropertyList (requiredProperties, 
                            newProperties);
                        formerMatch = _inPropertyList (requiredProperties, 
                            oldProperties);
                    }

                    //
                    //  Add current subscription to appropriate list
                    //
                    if (newMatch && !formerMatch)
                    {
                        //
                        //  CIMNamedInstances returned from repository do not 
                        //  include namespace  
                        //  Set namespace here
                        //
                        CIMReference instanceName = 
                            subscriptions [j].getInstanceName ();
                        instanceName.setNameSpace (nameSpaceNames [i]);
                        CIMNamedInstance currentInstance 
                            (instanceName, subscriptions [j].getInstance ());
                        newSubscriptions.append (currentInstance);
                    }
                    else if (!newMatch && formerMatch)
                    {
                        //
                        //  CIMNamedInstances returned from repository do not 
                        //  include namespace  
                        //  Set namespace here
                        //
                        CIMReference instanceName = 
                            subscriptions [j].getInstanceName ();
                        instanceName.setNameSpace (nameSpaceNames [i]);
                        CIMNamedInstance currentInstance 
                            (instanceName, subscriptions [j].getInstance ());
                        formerSubscriptions.append (currentInstance);
                    }
                }  // if subscription includes supported class
            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

Boolean IndicationService::_inPropertyList (
    const CIMPropertyList & requiredProperties,
    const CIMPropertyList & supportedProperties)
{
    const char METHOD_NAME [] = 
        "IndicationService::_inPropertyList";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);
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
            for (Uint8 i = 0; i < requiredProperties.getNumProperties (); i++)
            {
                if (!Contains (supportedProperties.getPropertyNameArray (), 
                    requiredProperties.getPropertyName (i)))
                {
                    return false;
                }
            }
        }
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return true;
}

Array <CIMNamedInstance> IndicationService::_getProviderSubscriptions (
    const CIMInstance & provider)
{
    Array <CIMNamedInstance> providerSubscriptions;

    const char METHOD_NAME [] = 
        "IndicationService::_getProviderSubscriptions";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Iterate through the subscription table
    //
    for (SubscriptionTable::Iterator i = _subscriptionTable.start (); i; i++)
    {
        //
        //  If provider matches, append subscription to the list
        //
        if (i.value ().provider == provider)
        {
            providerSubscriptions.append (i.value ().subscription);
        }
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (providerSubscriptions);
}

void IndicationService::_getFilterProperties (
    const CIMInstance & subscription,
    const String & nameSpaceName, 
    String & query,
    String & sourceNameSpace,
    String & queryLanguage) 
{
    CIMValue filterValue;
    CIMReference filterReference;
    CIMInstance filterInstance;

    const char METHOD_NAME [] = "IndicationService::_getFilterProperties";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    filterInstance = _repository->getInstance (nameSpaceName, filterReference);

    query = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_QUERY)).getValue ().toString ();

    sourceNameSpace = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_SOURCENAMESPACE)).getValue ().toString ();

    queryLanguage = filterInstance.getProperty
        (filterInstance.findProperty (_PROPERTY_QUERYLANGUAGE)).
        getValue ().toString ();

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_getFilterProperties (
    const CIMInstance & subscription,
    const String & nameSpaceName, 
    String & query,
    String & sourceNameSpace) 
{
    CIMValue filterValue;
    CIMReference filterReference;
    CIMInstance filterInstance;

    const char METHOD_NAME [] = "IndicationService::_getFilterProperties";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    filterInstance = _repository->getInstance (nameSpaceName, filterReference);

    query = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_QUERY)).getValue ().toString ();

    sourceNameSpace = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_SOURCENAMESPACE)).getValue ().toString ();

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_getFilterProperties (
    const CIMInstance & subscription,
    const String & nameSpaceName, 
    String & query) 
{
    CIMValue filterValue;
    CIMReference filterReference;
    CIMInstance filterInstance;

    const char METHOD_NAME [] = "IndicationService::_getFilterProperties";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    filterInstance = _repository->getInstance (nameSpaceName, filterReference);

    query = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_QUERY)).getValue ().toString ();

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

WQLSelectStatement IndicationService::_getSelectStatement (
    const String & filterQuery) const
{
    WQLSelectStatement selectStatement;
    const char METHOD_NAME [] = "IndicationService::_getSelectStatement";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    try
    {
        selectStatement.clear ();
        //
        //  ATTN-CAKG-P1-20020326: this method is not thread safe - it must be 
        //  guarded with mutexes by the caller
        //
        WQLParser::parse (filterQuery, selectStatement);
    }
    catch (ParseError & pe)
    {
        String exceptionStr = pe.getMessage ();
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, exceptionStr);
    }
    catch (MissingNullTerminator & mnt)
    {
        String exceptionStr = mnt.getMessage ();
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, exceptionStr);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return selectStatement;
}

String IndicationService::_getIndicationClassName (
    const WQLSelectStatement & selectStatement,
    const String & nameSpaceName) const
{
    String indicationClassName;
    Array <String> indicationSubclasses;
    const char METHOD_NAME [] = 
        "IndicationService::_getIndicationClassName";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    indicationClassName = selectStatement.getClassName ();

    //
    //  Validate that class is an Indication class
    //  The Indication Qualifier should exist and have the value True
    //
    Boolean validClass = false;
    CIMClass theClass = _repository->getClass
        (nameSpaceName, indicationClassName);
    if (theClass.existsQualifier (_QUALIFIER_INDICATION))
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
        String exceptionStr = _MSG_INVALID_CLASSNAME;
        exceptionStr.append (indicationClassName);
        exceptionStr.append (_MSG_IN_FROM);
        exceptionStr.append (PEGASUS_CLASSNAME_INDFILTER);
        exceptionStr.append (" ");
        exceptionStr.append (_PROPERTY_QUERY);
        exceptionStr.append (_MSG_PROPERTY);
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
            exceptionStr);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (indicationClassName);
}

Array <ProviderClassList> 
    IndicationService::_getIndicationProviders (
        const String & nameSpace,
        const String & indicationClassName,
        const Array <String> & indicationSubclasses,
        const CIMPropertyList & requiredPropertyList) const
{
    ProviderClassList provider;
    Array <ProviderClassList> indicationProviders;
    Array <CIMInstance> providerInstances;
    Array <CIMInstance> providerModuleInstances;
    Boolean duplicate = false;

    const char METHOD_NAME [] = "IndicationService::_getIndicationProviders";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  For each indication subclass, get providers
    //
    for (Uint8 i = 0; i < indicationSubclasses.size (); i++)
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
            for (Uint8 j = 0; j < providerInstances.size () && !duplicate; j++)
            {
                provider.classList.clear ();
                duplicate = false;

                //
                //  See if indication provider is already in list
                //
                for (Uint8 k = 0; 
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
        
    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (indicationProviders);
}

CIMPropertyList IndicationService::_getPropertyList 
    (const WQLSelectStatement & selectStatement,
     const String & nameSpaceName,
     const String & indicationClassName) const
{
    Array <String> propertyList;
    String propertyName;

    const char METHOD_NAME [] = "IndicationService::_getPropertyList";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get all the properties referenced in the projection list (SELECT clause)
    //
    Uint32 selectCount = selectStatement.getSelectPropertyNameCount ();
    if (selectCount > 0)
    {
        if (selectStatement.getSelectPropertyName (0) == _QUERY_ALLPROPERTIES)
        {
            //
            //  Return null CIMPropertyList for all properties
            //
            PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
            return CIMPropertyList ();
        }

        for (Uint32 i = 0; i < selectCount; i++)
        {
            propertyName = selectStatement.getSelectPropertyName (i);
            if (!Contains (propertyList, propertyName))
            {
                propertyList.append (propertyName);
            }
        }
    }

    //
    //  Get all the properties referenced in the condition (WHERE clause)
    //
    if (selectStatement.hasWhereClause ())
    {
        Uint32 whereCount = selectStatement.getWherePropertyNameCount ();
        if (whereCount > 0)
        {
            for (Uint32 j = 0; j < whereCount; j++)
            {
                propertyName = selectStatement.getWherePropertyName (j);
                if (!Contains (propertyList, propertyName))
                {
                    propertyList.append (propertyName);
                }
            }
        }
    }

    //
    //  Check if list includes all properties in class
    //  If so, must be set to NULL
    //
    CIMClass indicationClass = _repository->getClass 
        (nameSpaceName, indicationClassName);
    Boolean allProperties = true;
    for (Uint32 k = 0; 
         k < indicationClass.getPropertyCount () && allProperties; k++)
    {
        if (!Contains (propertyList, 
            indicationClass.getProperty (k).getName ()))
        {
            allProperties = false;
        }
    }

    if (allProperties)
    {
        //
        //  Return NULL CIMPropertyList
        //
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        return (CIMPropertyList ());
    }
    else
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        return (CIMPropertyList (propertyList));
    }
}

String IndicationService::_getCondition 
    (const String & filterQuery) const
{
    String condition = String::EMPTY;
    const char METHOD_NAME [] = "IndicationService::_getCondition";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get condition substring from filter query
    //
    if (filterQuery.find (_QUERY_WHERE) != PEG_NOT_FOUND)
    {
        condition = filterQuery.subString (filterQuery.find 
            (_QUERY_WHERE) + 6);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (condition);
}


CIMNamedInstance IndicationService::_getHandler (
    const CIMNamedInstance & subscription) const
{
    CIMValue handlerValue;
    CIMReference handlerRef;
    CIMInstance handlerInstance;
    const char METHOD_NAME [] = "IndicationService::_getHandler";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get Handler reference from subscription instance
    //
    handlerValue = subscription.getInstance ().getProperty 
        (subscription.getInstance ().findProperty
        (_PROPERTY_HANDLER)).getValue ();

    handlerValue.get (handlerRef);

    //
    //  Get Handler instance from the repository
    //
    handlerInstance = _repository->getInstance 
        (subscription.getInstanceName ().getNameSpace (), handlerRef);

    //
    //  Set namespace and create CIMNamedInstance
    //
    handlerRef.setNameSpace 
        (subscription.getInstanceName ().getNameSpace ());
    CIMNamedInstance handlerNamedInstance (handlerRef, handlerInstance);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (handlerNamedInstance);
}

Boolean IndicationService::_isTransient (
    const String & nameSpace,
    const CIMReference & handler) const
{
    CIMValue persistenceValue;
    Uint16 persistenceType;
    const char METHOD_NAME [] = "IndicationService::_isTransient";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get the instance from the respository
    //
    CIMInstance instance = _repository->getInstance (nameSpace, handler);

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
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        return true;
    }
    else
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        return false;
    }
}

void IndicationService::_deleteReferencingSubscriptions (
    const String & nameSpace,
    const String & referenceProperty,
    const CIMReference & handler)
{
    Array <CIMNamedInstance> subscriptions;
    const char METHOD_NAME [] = 
        "IndicationService::_deleteReferencingSubscriptions";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get existing subscriptions in the namespace
    //
    subscriptions = _repository->enumerateInstances (nameSpace,
        PEGASUS_CLASSNAME_INDSUBSCRIPTION);

    //
    //  Check each subscription for a reference to the specified instance 
    //
    for (Uint8 i = 0; i < subscriptions.size (); i++)
    {
        //
        //  Get the reference property value from the subscription instance
        //
        CIMValue propValue = subscriptions [i].getInstance ().getProperty
            (subscriptions [i].getInstance ().findProperty
            (referenceProperty)).getValue ();
        CIMReference ref;
        propValue.get (ref);

        //
        //  If the current subscription references the specified instance,
        //  delete it
        //
        if (handler == ref)
        {
            Array <ProviderClassList> indicationProviders;
            indicationProviders = _getDisableParams (nameSpace, 
                subscriptions [i].getInstance ());

            //
            //  Send disable requests
            //
            CIMInstance instance = subscriptions [i].getInstance ();
            String creator = instance.getProperty (instance.findProperty
                (_PROPERTY_CREATOR)).getValue ().toString ();
            CIMReference instanceName = 
                subscriptions [i].getInstanceName ();
            instanceName.setNameSpace (nameSpace);
            CIMNamedInstance currentInstance 
                (instanceName, subscriptions [i].getInstance ());
            _sendDisableRequests (indicationProviders, nameSpace, 
                currentInstance, creator);

            //
            //  Delete referencing subscription instance from repository
            //
            _repository->write_lock ();

            try
            {
                _repository->deleteInstance (nameSpace, 
                    subscriptions [i].getInstanceName ());
            }
            catch (Exception & exception)
            {
                //
                //  ATTN: Log a message??
                //
            }

            _repository->write_unlock ();
        }
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

Boolean IndicationService::_isExpired (
    const CIMInstance & instance) const
{
    const char METHOD_NAME [] = 
        "IndicationService::_isExpired";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Calculate time remaining from subscription
    //  start time, subscription duration, and current date time
    //

    //
    //  NOTE: It is assumed that the instance passed to this method is a 
    //  subscription instance, and that the Subscription Duration and 
    //  Start Time properties exist
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
    CIMValue durationValue;
    Uint64 duration;
    durationValue = instance.getProperty 
        (instance.findProperty (_PROPERTY_DURATION)).getValue ();
    if (durationValue.isNull ())
    {
        //
        //  If there is no duration value set, the subscription has no 
        //  expiration date
        //
        return false;
    }
    else
    {
        durationValue.get (duration);
    }

    //
    //  Get current date time, and determine if subscription has expired
    //  ATTN: need method to calculate difference of two CIMDateTime values
    //
    CIMDateTime currentDateTime = CIMDateTime ();

    if (duration /* - (currentDateTime - startTime) */ > 0)
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        return false;
    }
    else
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        return true;
    }
}

void IndicationService::_deleteExpiredSubscription (
    const String & nameSpace,
    const CIMReference & subscription)
{
    const char METHOD_NAME [] = 
        "IndicationService::_deleteExpiredSubscription";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

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
        //
        //  ATTN: Log a message??
        //
    }

    _repository->write_unlock ();

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_setTimeRemaining (
    CIMInstance & instance)
{
    const char METHOD_NAME [] = 
        "IndicationService::_setTimeRemaining";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Calculate time remaining from subscription
    //  start time, subscription duration, and current date time
    //

    //
    //  NOTE: It is assumed that the instance passed to this method is a 
    //  subscription instance, and that the Subscription Duration and 
    //  Start Time properties exist
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
    CIMValue durationValue;
    Uint64 duration;
    durationValue = instance.getProperty 
        (instance.findProperty (_PROPERTY_DURATION)).getValue ();
    durationValue.get (duration);

    //
    //  Get current date time, and calculate Subscription Time Remaining
    //  ATTN: need method to calculate difference of two CIMDateTime values
    //
    CIMDateTime currentDateTime = CIMDateTime ();
    Uint64 timeRemaining = duration /* - (currentDateTime - startTime) */ ;

    //
    //  Add or set the value of the property with the calculated value
    //
    if (!instance.existsProperty (_PROPERTY_TIMEREMAINING))
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

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_getEnableParams (
    const String & nameSpaceName,
    const CIMInstance & subscriptionInstance,
    Array <ProviderClassList> & indicationProviders,
    CIMPropertyList & propertyList,
    String & condition,
    String & queryLanguage)
{
    String filterQuery;
    WQLSelectStatement selectStatement;
    String indicationClassName;
    Array <String> indicationSubclasses;
    String sourceNameSpace;
    condition = String::EMPTY;
    queryLanguage = String::EMPTY;
    const char METHOD_NAME [] = "IndicationService::_getEnableParams";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

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
        nameSpaceName);
    
    //
    //  Get list of subclass names for indication class
    //
    indicationSubclasses = _repository->enumerateClassNames (nameSpaceName, 
        indicationClassName, true);
    indicationSubclasses.append (indicationClassName);
    
    //
    //  Get property list from filter query (FROM and WHERE 
    //  clauses)
    //
    if ((selectStatement.getSelectPropertyNameCount () > 0) ||
        (selectStatement.getWherePropertyNameCount () > 0))
    {
        propertyList = _getPropertyList (selectStatement,
            sourceNameSpace, indicationClassName);
    }

    //
    //  Get indication provider class lists
    //
    indicationProviders = _getIndicationProviders 
        (sourceNameSpace, indicationClassName, indicationSubclasses, 
         propertyList);
//cout << "Indication Providers to enable: " << indicationProviders.size () 
//<< endl;

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

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}
    
void IndicationService::_getEnableParams (
    const String & nameSpaceName,
    const CIMInstance & subscriptionInstance,
    CIMPropertyList & propertyList,
    String & condition,
    String & queryLanguage)
{
    String filterQuery;
    WQLSelectStatement selectStatement;
    String sourceNameSpace;
    condition = String::EMPTY;
    queryLanguage = String::EMPTY;
    const char METHOD_NAME [] = "IndicationService::_getEnableParams";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

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
    if ((selectStatement.getSelectPropertyNameCount () > 0) ||
        (selectStatement.getWherePropertyNameCount () > 0))
    {
        String indicationClassName = _getIndicationClassName (selectStatement,
            nameSpaceName);
        propertyList = _getPropertyList (selectStatement,
            sourceNameSpace, indicationClassName);
    }

    //
    //  Get condition from filter query (WHERE clause)
    //
    if (selectStatement.hasWhereClause ())
    {
        condition = _getCondition (filterQuery);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}
    
Array <ProviderClassList> IndicationService::_getDisableParams (
    const String & nameSpaceName,
    const CIMInstance & subscriptionInstance)
{
    String filterQuery;
    String sourceNameSpace;
    WQLSelectStatement selectStatement;
    String indicationClassName;
    Array <String> indicationSubclasses;
    CIMPropertyList propertyList;
    Array <ProviderClassList> indicationProviders;

    const char METHOD_NAME [] = "IndicationService::_getDisableParams";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

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
        nameSpaceName);
    
    //
    //  Get list of subclass names for indication class
    //
    indicationSubclasses = _repository->enumerateClassNames (nameSpaceName, 
        indicationClassName, true);
    indicationSubclasses.append (indicationClassName);
    
    //
    //  Get property list from filter query (FROM and WHERE 
    //  clauses)
    //
    if ((selectStatement.getSelectPropertyNameCount () > 0) ||
        (selectStatement.getWherePropertyNameCount () > 0))
    {
        propertyList = _getPropertyList (selectStatement,
            sourceNameSpace, indicationClassName);
    }

    //
    //  Get indication provider class lists
    //
    indicationProviders = _getIndicationProviders 
        (sourceNameSpace, indicationClassName, indicationSubclasses, 
         propertyList);
//cout << "Indication Providers to disable: " << indicationProviders.size () 
//<< endl;

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return indicationProviders;
}
    
Boolean IndicationService::_sendEnableRequests
    (const Array <ProviderClassList> & indicationProviders,
     const String & nameSpace,
     const CIMPropertyList & propertyList,
     const String & condition,
     const String & queryLanguage,
     const CIMNamedInstance & subscription,
     const String & userName,
     const String & authType)
{
    CIMValue propValue;
    Uint16 repeatNotificationPolicy;
    const char METHOD_NAME [] = "IndicationService::_sendEnableRequests";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get repeat notification policy value from subscription instance
    //
    propValue = subscription.getInstance ().getProperty 
        (subscription.getInstance ().findProperty 
        (_PROPERTY_REPEATNOTIFICATIONPOLICY)).getValue ();
    propValue.get (repeatNotificationPolicy);

    //
    //  Send enable request to each provider
    //
    Uint8 accepted = 0;
    for (Uint8 i = 0; i < indicationProviders.size (); i++)
    {
        CIMCreateSubscriptionRequestMessage * request =
            new CIMCreateSubscriptionRequestMessage
                (XmlWriter::getNextMessageId (),
                nameSpace,
                subscription.getInstance (),
                indicationProviders [i].classList,
                indicationProviders [i].provider,
                indicationProviders [i].providerModule,
                propertyList,
                repeatNotificationPolicy,
                condition,
                queryLanguage,
                QueueIdStack (_providerManager, getQueueId ()),
                authType,
                userName);

        AsyncOpNode* op = this->get_op(); 

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

        AsyncReply * async_reply = SendWait (async_req);

        //
        //  ATTN: Check for return value indicating invalid queue ID
        //  If received, need to find Provider Manager Service queue ID
        //  again
        //

        CIMCreateSubscriptionResponseMessage * response =
            reinterpret_cast 
            <CIMCreateSubscriptionResponseMessage *>
            ((static_cast <AsyncLegacyOperationResult *>
            (async_reply))->get_result());

        if (response->errorCode == CIM_ERR_SUCCESS)
        {
            //cout << "Enable accepted" << endl;
            accepted++;

            //
            //  Insert entry into subscription table 
            //
            _insertEntry (subscription, indicationProviders [i].provider,
                indicationProviders [i].classList);
        }
        else
        {
            //
            //  ATTN-CAKG-P3-20020326: Any action required?
            //  Should a message be logged?
            //

            //cout << "Enable rejected" << endl;
            //cout << "Error code: " << response->errorCode << endl;
            //cout << response->errorDescription << endl;
        }

        //
        //  ATTN: This is temporary... because requests are not accepted yet
        //
        _insertEntry (subscription, indicationProviders [i].provider,
            indicationProviders [i].classList);

        delete async_req;
        delete async_reply;
    }

    //
    //  ATTN-CAKG-P1-20020326: Temporarily returning true (for testing purposes)
    //  although there are no indication providers functioning
    //
    //Boolean result = (accepted > 0);
    Boolean result = true;
    return result;

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}


void IndicationService::_sendModifyRequests
    (const Array <ProviderClassList> & indicationProviders,
     const String & nameSpace,
     const CIMPropertyList & propertyList,
     const String & condition,
     const String & queryLanguage,
     const CIMNamedInstance & subscription,
     const String & userName,
     const String & authType)
{
    CIMValue propValue;
    Uint16 repeatNotificationPolicy;
    const char METHOD_NAME [] = "IndicationService::_sendModifyRequests";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get repeat notification policy value from subscription instance
    //
    propValue = subscription.getInstance ().getProperty 
        (subscription.getInstance ().findProperty
        (_PROPERTY_REPEATNOTIFICATIONPOLICY)).getValue ();
    propValue.get (repeatNotificationPolicy);

    //
    //  Send modify request to each provider
    //
    for (Uint8 i = 0; i < indicationProviders.size (); i++)
    {
        CIMModifySubscriptionRequestMessage * request =
            new CIMModifySubscriptionRequestMessage
                (XmlWriter::getNextMessageId (),
                nameSpace,
                subscription.getInstance (),
                indicationProviders [i].classList,
                indicationProviders [i].provider,
                indicationProviders [i].providerModule,
                propertyList,
                repeatNotificationPolicy,
                condition,
                queryLanguage,
                QueueIdStack (_providerManager, getQueueId ()),
                authType,
                userName);

        AsyncOpNode* op = this->get_op();

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

        AsyncReply * async_reply = SendWait (async_req);

        //
        //  ATTN: Check for return value indicating invalid queue ID
        //  If received, need to find Provider Manager Service queue ID
        //  again
        //

        CIMModifySubscriptionResponseMessage * response =
            reinterpret_cast
            <CIMModifySubscriptionResponseMessage *>
            ((static_cast <AsyncLegacyOperationResult *>
            (async_reply))->get_result());

        //
        //  ATTN-CAKG-P2-20020326: Do we need to look at the response?
        //  Indication providers may ignore modify requests, so I don't think
        //  we care whether they accept or reject the modification...
        //

        //
        //  Remove old entry from subscription table and insert 
        //  updated entry
        //
        String tableKey = _generateKey 
            (subscription, indicationProviders [i].provider);
        _subscriptionTable.remove (tableKey);
        _insertEntry (subscription, indicationProviders [i].provider,
            indicationProviders [i].classList);

        delete async_req;
        delete async_reply;
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_sendDisableRequests
    (const Array <ProviderClassList> & indicationProviders,
     const String & nameSpace,
     const CIMNamedInstance & subscription,
     const String & userName,
     const String & authType)
{
    const char METHOD_NAME [] = "IndicationService::_sendDisableRequests";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Send disable request to each provider
    //
    for (Uint8 i = 0; i < indicationProviders.size (); i++)
    {
        CIMDeleteSubscriptionRequestMessage * request =
            new CIMDeleteSubscriptionRequestMessage
            (XmlWriter::getNextMessageId (),
            nameSpace,
            subscription.getInstance (),
            indicationProviders [i].classList,
            indicationProviders [i].provider,
            indicationProviders [i].providerModule,
            QueueIdStack (_providerManager, getQueueId ()),
            authType,
            userName);

        AsyncOpNode* op = this->get_op();

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

        AsyncReply * async_reply = SendWait (async_req);

        //
        //  ATTN: Check for return value indicating invalid queue ID
        //  If received, need to find Provider Manager Service queue ID
        //  again
        //

        CIMDeleteSubscriptionResponseMessage * response =
            reinterpret_cast
            <CIMDeleteSubscriptionResponseMessage *>
            ((static_cast <AsyncLegacyOperationResult *>
            (async_reply))->get_result());

        //
        //  ATTN-CAKG-P2-20020326: Do we need to look at the response?
        //  I don't think there is any action to take if the disable is
        //  rejected (perhaps log a message?)
        //

        //
        //  Remove entry from subscription table 
        //
        String tableKey = _generateKey 
            (subscription, indicationProviders [i].provider);
        _subscriptionTable.remove (tableKey);

        delete async_req;
        delete async_reply;
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

String IndicationService::_generateKey (
    const CIMNamedInstance & subscription,
    const CIMInstance provider)
{
    String tableKey;
    const char METHOD_NAME [] = "IndicationService::_generateKey";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Append subscription namespace name to key
    //
    tableKey.append (subscription.getInstanceName ().getNameSpace ());

    //
    //  Append subscription filter key values to key
    //
    CIMValue filterVal = subscription.getInstance ().getProperty
        (subscription.getInstance ().findProperty
        (_PROPERTY_FILTER)).getValue ();
            
    CIMReference filterRef;
    filterVal.get (filterRef);

    Array <KeyBinding> filterKeyBindings = filterRef.getKeyBindings ();
    for (Uint8 i = 0; i < filterKeyBindings.size (); i++)
    {
        tableKey.append (filterKeyBindings [i].getValue ());
    }

    //
    //  Append subscription handler key values to key
    //
    CIMValue handlerVal = subscription.getInstance ().getProperty
        (subscription.getInstance ().findProperty
        (_PROPERTY_HANDLER)).getValue ();
            
    CIMReference handlerRef;
    handlerVal.get (handlerRef);

    Array <KeyBinding> HandlerKeyBindings = handlerRef.getKeyBindings ();
    for (Uint8 i = 0; i < HandlerKeyBindings.size (); i++)
    {
        tableKey.append (HandlerKeyBindings [i].getValue ());
    }

    //
    //  Append provider key values to key
    //
    String providerName = provider.getProperty (provider.findProperty
        (_PROPERTY_PROVIDERNAME)).getValue ().toString ();
    String providerModuleName = provider.getProperty (provider.findProperty
        (_PROPERTY_PROVIDERMODULENAME)).getValue ().toString ();
    tableKey.append (providerName);
    tableKey.append (providerModuleName);

    //cout << "Generated Key: " << tableKey << endl;
    //cout << endl;
    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (tableKey);
}

void IndicationService::_insertEntry (
    const CIMNamedInstance & subscription,
    const CIMInstance & provider,
    const Array <String> classList)
{
    const char METHOD_NAME [] = "IndicationService::_insertEntry";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    String tableKey = _generateKey 
        (subscription, provider);
    SubscriptionTableEntry entry;
    entry.subscription = subscription;
    entry.provider = provider;
    entry.classList = classList;
    _subscriptionTable.insert (tableKey, entry);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

CIMInstance IndicationService::_createAlertInstance (
    const String & alertClassName,
    const Array <CIMNamedInstance> & subscriptions)
{
    const char METHOD_NAME [] = "IndicationService::_createAlertInstance";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMInstance indicationInstance (alertClassName);

    // 
    //  Add property values for all required properties of CIM_AlertIndication
    // 
    indicationInstance.addProperty 
        (CIMProperty (_PROPERTY_ALERTTYPE, CIMValue ((Uint16) _TYPE_OTHER)));
    //
    // ATTN: what should Other Alert Type value be??
    //
    indicationInstance.addProperty 
        (CIMProperty (_PROPERTY_OTHERALERTTYPE, alertClassName));

    indicationInstance.addProperty 
        (CIMProperty (_PROPERTY_PERCEIVEDSEVERITY,
                      CIMValue ((Uint16) _SEVERITY_WARNING)));
    //
    // ATTN: what should Probable Cause value be??
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
    if (alertClassName == _CLASS_CIMOM_SHUTDOWN_ALERT)
    {
    }
    else if (alertClassName == _CLASS_NO_PROVIDER_ALERT)
    {
    }
    else if (alertClassName == _CLASS_PROVIDER_TERMINATED_ALERT)
    {
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (indicationInstance);
}

void IndicationService::_sendAlerts (
    const Array <CIMNamedInstance> & subscriptions,
    /* const */ CIMInstance & alertInstance)
{
    CIMNamedInstance current;
    Boolean duplicate;
    Array <CIMNamedInstance> handlers;
    const char METHOD_NAME [] = "IndicationService::_sendAlerts";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    handlers.clear ();

    //
    //  Get list of unique handler instances for all subscriptions in list
    //
    for (Uint8 i = 0; i < subscriptions.size (); i++)
    {
        //
        //  Get handler instance
        //
        current = _getHandler (subscriptions [i]);

        //
        //  Merge into list of unique handler instances
        //
        duplicate = false;
        for (Uint8 j = 0; j < handlers.size () && !duplicate; j++)
        {
            if ((current.getInstance () == handlers [j].getInstance ()) &&
                (current.getInstanceName () == handlers [j].getInstanceName ()))
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
    for (Uint8 k = 0; k < handlers.size (); k++)
    {
        CIMHandleIndicationRequestMessage * handler_request =
            new CIMHandleIndicationRequestMessage (
                XmlWriter::getNextMessageId (),
                handlers [k].getInstanceName ().getNameSpace (),
                handlers [k].getInstance (),
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

        AsyncReply *async_reply = SendWait(async_req);

        //
        //  ATTN: Check for return value indicating invalid queue ID
        //  If received, need to find Provider Manager Service queue ID
        //  again
        //

        CIMHandleIndicationResponseMessage* response = 
            reinterpret_cast<CIMHandleIndicationResponseMessage *>
            ((static_cast<AsyncLegacyOperationResult *>
            (async_reply))->get_result());

        delete async_req;
        delete async_reply;
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

WQLSimplePropertySource IndicationService::_getPropertySourceFromInstance(
    CIMInstance& indicationInstance)
{
    Boolean booleanValue;
    WQLSimplePropertySource source;

    for (Uint8 i=0; i < indicationInstance.getPropertyCount(); i++)
    {
        CIMProperty property = indicationInstance.getProperty(i);
        CIMValue propertyValue = property.getValue();
        CIMType type = property.getType();
        String propertyName = property.getName();

        switch (type)
        {
            case CIMType::UINT8:
                Uint8 propertyValueUint8;
                propertyValue.get(propertyValueUint8);
                source.addValue(propertyName,
                    WQLOperand(propertyValueUint8, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::UINT16:
                Uint16 propertyValueUint16;
                propertyValue.get(propertyValueUint16);
                source.addValue(propertyName,
                    WQLOperand(propertyValueUint16, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::UINT32:
                Uint32 propertyValueUint32;
                propertyValue.get(propertyValueUint32);
                source.addValue(propertyName,
                    WQLOperand(propertyValueUint32, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::UINT64:
                Uint64 propertyValueUint64;
                propertyValue.get(propertyValueUint64);
                source.addValue(propertyName,
                    WQLOperand(propertyValueUint64, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::SINT8:
                Sint8 propertyValueSint8;
                propertyValue.get(propertyValueSint8);
                source.addValue(propertyName,
                    WQLOperand(propertyValueSint8, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::SINT16:
                Sint16 propertyValueSint16;
                propertyValue.get(propertyValueSint16);
                source.addValue(propertyName,
                    WQLOperand(propertyValueSint16, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::SINT32:
                Sint32 propertyValueSint32;
                propertyValue.get(propertyValueSint32);
                source.addValue(propertyName,
                    WQLOperand(propertyValueSint32, WQL_INTEGER_VALUE_TAG));
                break;                break;

            case CIMType::SINT64:
                Sint64 propertyValueSint64;
                propertyValue.get(propertyValueSint64);
                source.addValue(propertyName,
                    WQLOperand(propertyValueSint64, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::REAL32:
                Real32 propertyValueReal32;
                propertyValue.get(propertyValueReal32);
                source.addValue(propertyName,
                    WQLOperand(propertyValueReal32, WQL_DOUBLE_VALUE_TAG));
                break;

            case CIMType::REAL64:
                Real64 propertyValueReal64;
                propertyValue.get(propertyValueReal64);
                source.addValue(propertyName,
                    WQLOperand(propertyValueReal64, WQL_DOUBLE_VALUE_TAG));
                break;

            case CIMType::BOOLEAN :
                property.getValue().get(booleanValue);
                source.addValue(propertyName,
                    WQLOperand(booleanValue, WQL_BOOLEAN_VALUE_TAG));
                break;

            case CIMType::CHAR16:
            case CIMType::STRING :
                source.addValue(propertyName,
                    WQLOperand(property.getValue().toString(),
                    WQL_STRING_VALUE_TAG));
                break;

            case CIMType::NONE :
                source.addValue(propertyName,
                    WQLOperand());
                break;
        }
    }

    return (source);
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
const char   IndicationService::_CLASS_CIMOM_SHUTDOWN_ALERT [] =
                 "CIM_AlertIndication";

/**
    The name of the No Provider alert indication class
 */
//
//  ATTN: Update once NoProviderAlertIndication has been defined
//
const char   IndicationService::_CLASS_NO_PROVIDER_ALERT [] =
                 "CIM_AlertIndication";

/**
    The name of the CIMOM shutdown alert indication class
 */
//
//  ATTN: Update once ProviderTerminatedAlertIndication has been defined
//
const char   IndicationService::_CLASS_PROVIDER_TERMINATED_ALERT [] =
                 "CIM_AlertIndication";


//
//  Property names
//

/**
    The name of the filter reference property for indication subscription class
 */
const char   IndicationService::_PROPERTY_FILTER []     = "Filter";

/**
    The name of the handler reference property for indication subscription class
 */
const char   IndicationService::_PROPERTY_HANDLER []     = "Handler";

/**
    The name of the subscription state property for indication subscription 
    class
 */
const char   IndicationService::_PROPERTY_STATE []      = 
                 "SubscriptionState";

/**
    The name of the Other Subscription State property for Indication 
    Subscription class
 */
const char   IndicationService::_PROPERTY_OTHERSTATE [] = 
                 "OtherSubscriptionState";

/**
    The name of the repeat notification policy property for indication
    subscription class
 */
const char   IndicationService::_PROPERTY_REPEATNOTIFICATIONPOLICY [] =
                 "RepeatNotificationPolicy";

/**
    The name of the other repeat notification policy property for
    indication subscription class
 */
const char   IndicationService::_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY []
                 = "OtherRepeatNotificationPolicy";

/**
    The name of the repeat notification interval property for indication
    subscription class
 */
const char   IndicationService::_PROPERTY_REPEATNOTIFICATIONINTERVAL [] =
                 "RepeatNotificationInterval";

/**
    The name of the repeat notification gap property for indication
    subscription class
 */
const char   IndicationService::_PROPERTY_REPEATNOTIFICATIONGAP [] =
                 "RepeatNotificationGap";

/**
    The name of the repeat notification count property for indication
    subscription class
 */
const char   IndicationService::_PROPERTY_REPEATNOTIFICATIONCOUNT [] =
                 "RepeatNotificationCount";

/**
    The name of the On Fatal Error Policy property for Indication Subscription 
    class
 */
const char   IndicationService::_PROPERTY_ONFATALERRORPOLICY [] = 
                 "OnFatalErrorPolicy";

/**
    The name of the Other On Fatal Error Policy property for Indication 
    Subscription class
 */
const char   IndicationService::_PROPERTY_OTHERONFATALERRORPOLICY [] = 
                 "OtherOnFatalErrorPolicy";

/**
    The name of the Time Of Last State Change property for Indication 
    Subscription class
 */
const char   IndicationService::_PROPERTY_LASTCHANGE []      = 
                 "TimeOfLastStateChange";

/**
    The name of the Subscription Start Time property for Indication 
    Subscription class
 */
const char   IndicationService::_PROPERTY_STARTTIME []      = 
                 "SubscriptionStartTime";

/**
    The name of the Subscription Duration property for Indication 
    Subscription class
 */
const char   IndicationService::_PROPERTY_DURATION []      = 
                 "SubscriptionDuration";

/**
    The name of the Subscription Time Remaining property for Indication 
    Subscription class
 */
const char   IndicationService::_PROPERTY_TIMEREMAINING []      = 
                 "SubscriptionTimeRemaining";

/**
    The name of the query property for indication filter class
 */
const char   IndicationService::_PROPERTY_QUERY []      = "Query";

/**
    The name of the query language property for indication filter class
 */
const char   IndicationService::_PROPERTY_QUERYLANGUAGE [] = 
                 "QueryLanguage";

/**
    The name of the Source Namespace property for indication filter class
 */
const char   IndicationService::_PROPERTY_SOURCENAMESPACE [] = 
                 "SourceNamespace";

/**
    The name of the name property for indication filter and indications handler     classes
 */
const char   IndicationService::_PROPERTY_NAME []       = "Name";

/**
    The name of the creation class name property for indication filter and 
    indications handler classes
 */
const char   IndicationService::_PROPERTY_CREATIONCLASSNAME [] = 
             "CreationClassName";

/**
    The name of the system name property for indication filter and indications 
    handler classes
 */
const char   IndicationService::_PROPERTY_SYSTEMNAME [] = "SystemName";

/**
    The name of the system creation class name property for indication filter 
    and indications handler classes
 */
const char   IndicationService::_PROPERTY_SYSTEMCREATIONCLASSNAME [] = 
             "SystemCreationClassName";

/**
    The name of the Persistence Type property for Indication Handler class
 */
const char   IndicationService::_PROPERTY_PERSISTENCETYPE [] = 
                 "PersistenceType";

/**
    The name of the Other Persistence Type property for Indication Handler 
    class
 */
const char   IndicationService::_PROPERTY_OTHERPERSISTENCETYPE [] = 
                 "OtherPersistenceType";

/**
    The name of the Destination property for CIM XML Indication Handler 
    subclass
 */
const char   IndicationService::_PROPERTY_DESTINATION [] = "Destination";

/**
    The name of the Trap Destination property for SNMP Mapper Indication 
    Handler subclass
 */
const char   IndicationService::_PROPERTY_TRAPDESTINATION [] = 
                 "TrapDestination";

/**
    The name of the SNMP Type property for SNMP Indication Handler class
 */
const char   IndicationService::_PROPERTY_SNMPTYPE [] = "SNMPVersion";

/**
    The name of the Alert Type property for Alert Indication class
 */
const char   IndicationService::_PROPERTY_ALERTTYPE [] = "AlertType";

/**
    The name of the Other Alert Type property for Alert Indication class
 */
const char   IndicationService::_PROPERTY_OTHERALERTTYPE [] = 
             "OtherAlertType";

/**
    The name of the Perceived Severity property for Alert Indication class
 */
const char   IndicationService::_PROPERTY_PERCEIVEDSEVERITY [] = 
             "PerceivedSeverity";

/**
    The name of the Probable Cause property for Alert Indication class
 */
const char   IndicationService::_PROPERTY_PROBABLECAUSE [] = 
             "ProbableCause";

/**
    The name of the Provider Name property for Provider class
 */
const char   IndicationService::_PROPERTY_PROVIDERNAME [] = "Name";

/**
    The name of the Provider Module Name property for Provider class
 */
const char   IndicationService::_PROPERTY_PROVIDERMODULENAME [] = 
             "ProviderModuleName";

/**
    The name of the Creator property for a class
 */
const char   IndicationService::_PROPERTY_CREATOR [] = 
             "Creator";

//
//  Qualifier names
//

/**
    The name of the Indication qualifier for classes
 */
const char   IndicationService::_QUALIFIER_INDICATION []     = "INDICATION";


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


//
//  Message substrings used in exception messages
//

const char IndicationService::_MSG_MISSING_REQUIRED [] = "Missing required ";

const char IndicationService::_MSG_KEY_PROPERTY [] = " key property";

const char IndicationService::_MSG_PROPERTY [] = " property";

const char IndicationService::_MSG_PROPERTY_PRESENT [] = 
    " property present, but ";

const char IndicationService::_MSG_VALUE_NOT [] = " value not ";

const char IndicationService::_MSG_NO_PROVIDERS [] = 
    "There are no providers capable of serving the subscription";

const char IndicationService::_MSG_NOT_ACCEPTED [] = 
    "No providers accepted the subscription";

const char IndicationService::_MSG_INVALID_CLASSNAME [] = 
    "Invalid indication class name ";

const char IndicationService::_MSG_IN_FROM [] = " in FROM clause of ";

const char IndicationService::_MSG_EXPIRED [] = 
    "Expired subscription may not be modified; has been deleted";

const char IndicationService::_MSG_REFERENCED [] = 
    "A Filter or Handler referenced by a subscription may not be deleted";


PEGASUS_NAMESPACE_END
