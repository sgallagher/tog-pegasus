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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company 
//               (carolann_graves@hp.com)
// 		 Ben Heilbronn, Hewlett-Packard Company
//               (ben_heilbronn@hp.com)
// 		 Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/WQL/WQLSelectStatement.h>

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
         _repository (repository),
         _providerRegManager (providerRegManager)
{
    try
    {
        //
        //  Initialize
        //
        _initialize ();
    }
    catch (Exception e)
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
    Array <CIMInstance> activeSubscriptions;
    Array <CIMInstance> noProviderSubscriptions;
    Array <ProviderClassList> enableProviders;
    Boolean duplicate;

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
        if (_isExpired (activeSubscriptions [i]))
        {
            CIMObjectPath path = activeSubscriptions [i].getPath ();
            _deleteExpiredSubscription (path);
                
            continue;
        }

        CIMNamespaceName sourceNameSpace;
        _getCreateParams 
            (activeSubscriptions [i].getPath ().getNameSpace (),
            activeSubscriptions [i], indicationProviders,
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
        String creator = instance.getProperty (instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_CREATOR)).getValue ().toString ();
        if (!_sendCreateRequests (indicationProviders, sourceNameSpace,
            propertyList, condition, queryLanguage,
            activeSubscriptions [i], creator))
        {
            //
            //  No providers accepted this subscription
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
        //  Merge provider list into list of unique providers to enable
        //
        for (Uint8 j = 0; j < indicationProviders.size (); j++)
        {
            duplicate = false;
            for (Uint8 k = 0; k < enableProviders.size () && !duplicate; k++)
            {
                if ((indicationProviders [j].provider.identical 
                    (enableProviders [k].provider)))
                {
                    duplicate = true;
                }
            }
    
            if (!duplicate)
            {
                enableProviders.append (indicationProviders [j]);
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
    //  Send Enable message to each provider
    //
    for (Uint8 m = 0; m < enableProviders.size (); m++)
    {
        //
        //  Send Enable message 
        //
        _sendEnable (enableProviders [m]);
    }

    PEG_METHOD_EXIT ();
}

void IndicationService::_terminate (void)
{
    Array <CIMInstance> activeSubscriptions;
    CIMInstance indicationInstance;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_terminate");

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

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleCreateInstanceRequest (const Message * message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_handleCreateInstanceRequest");

    CIMCreateInstanceRequestMessage* request = 
        (CIMCreateInstanceRequestMessage*) message;

    CIMException cimException;

    CIMObjectPath instanceRef;

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
            Array <ProviderClassList> indicationProviders;
            if (instance.getClassName ().equal 
                (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
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
    
                    _getCreateParams (request->nameSpace, instance,
                        indicationProviders, requiredProperties, 
                        sourceNameSpace, condition, queryLanguage);
    
                    if (indicationProviders.size () == 0)
                    {
                        //
                        //  There are no providers that can support this 
                        //  subscription
                        //
                        PEG_METHOD_EXIT ();
                        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                            _MSG_NO_PROVIDERS);
                    }
                }
    
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
                instanceRef = _repository->createInstance (request->nameSpace, 
                    instance);
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
    
                Base::_enqueueResponse (request, response);
    
                PEG_METHOD_EXIT ();
                return;
            }
        
            //
            //  If the instance is of the PEGASUS_CLASSNAME_INDSUBSCRIPTION 
            //  class and subscription state is enabled, send Create request to 
            //  indication providers
            //
            if (instance.getClassName ().equal 
                (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
            {
                if ((subscriptionState == _STATE_ENABLED) ||
                    (subscriptionState == _STATE_ENABLEDDEGRADED))
                {
                    //
                    //  Send Create request message to each provider
                    //
                    instanceRef.setNameSpace (request->nameSpace);
                    instance.setPath (instanceRef);
                    if (!_sendCreateRequests (indicationProviders, 
                        sourceNameSpace, requiredProperties, condition, 
                        queryLanguage, instance,
                        request->userName, request->authType))
                    {
                        PEG_METHOD_EXIT ();
                        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                            _MSG_NOT_ACCEPTED);
                    }
            
                    //
                    //  Send Enable message to each provider
                    //
                    for (Uint8 i = 0; i < indicationProviders.size (); i++)
                    {
                        //
                        //  Send Enable message 
                        //
                        _sendEnable (indicationProviders [i]);
                    }
                }
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
    //  FUTURE: Response should be sent only after Create response
    //  has been received
    //
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
    
    Base::_enqueueResponse (request, response);

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

    _repository->read_lock ();

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
        instance.removeProperty (instance.findProperty 
            (PEGASUS_PROPERTYNAME_INDSUB_CREATOR));

        //
        //  If a subscription with a duration, calculate subscription time 
        //  remaining, and add property to the instance
        //
        if (request->instanceName.getClassName ().equal 
            (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
        {
            _setTimeRemaining (instance);
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

    CIMGetInstanceResponseMessage* response = new CIMGetInstanceResponseMessage
        (request->messageId,
        cimException,
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

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleEnumerateInstancesRequest(const Message* message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_handleEnumerateInstancesRequest");

    CIMEnumerateInstancesRequestMessage* request = 
        (CIMEnumerateInstancesRequestMessage*) message;

    Array <CIMInstance> enumInstances;

    CIMException cimException;
    CIMInstance cimInstance;

    _repository->read_lock ();

    try
    {
        enumInstances = _repository->enumerateInstancesForClass
            (request->nameSpace, request->className, 
             request->deepInheritance, request->localOnly, 
             request->includeQualifiers, request->includeClassOrigin, 
             false, request->propertyList);
        
        //
        //  Remove Creator property from instances before returning
        //
        for (Uint8 i = 0; i < enumInstances.size (); i++)
        {
            enumInstances [i].removeProperty 
                (enumInstances [i].findProperty 
                (PEGASUS_PROPERTYNAME_INDSUB_CREATOR));

            //
            //  If a subscription with a duration, calculate subscription time 
            //  remaining, and add property to the instance
            //
            if (request->className.equal (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
            {
                _setTimeRemaining (enumInstances [i]);
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

    CIMEnumerateInstancesResponseMessage* response = 
        new CIMEnumerateInstancesResponseMessage(
            request->messageId,
            cimException,
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
    
    try
    {
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
        catch (Exception e)
        {
            _repository->read_unlock ();
            PEG_METHOD_EXIT ();
            throw e;
        }

        _repository->read_unlock ();

        if (_canModify (request, instanceReference, instance))
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
    
                String exceptionStr = _MSG_EXPIRED;
                PEG_METHOD_EXIT ();
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
            }
    
            //
            //  _canModify, above, already checked that propertyList is not 
            //  null, and that numProperties is 0 or 1
            //
            CIMInstance modifiedInstance = 
                request->modifiedInstance;
            CIMPropertyList propertyList = request->propertyList;
            if (request->propertyList.size () > 0)
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
                    request->modifiedInstance.getProperty
                    (request->modifiedInstance.findProperty
                    (_PROPERTY_STATE)).getValue ();
    
                subscriptionStateValue.get (newState);
    
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

                //
                //  If subscription is to be enabled, determine if there are 
                //  any indication providers that can serve the subscription
                //
                Array <ProviderClassList> indicationProviders;
                CIMPropertyList requiredProperties;
                CIMNamespaceName sourceNameSpace;
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
                    _getCreateParams (request->nameSpace, instance,
                        indicationProviders, requiredProperties, 
                        sourceNameSpace, condition, queryLanguage);
    
                    if (indicationProviders.size () == 0)
                    {
                        //
                        //  There are no providers that can support this 
                        //  subscription
                        //
                        instance.setPath (instanceReference);
                        _handleError (instance);
                        PEG_METHOD_EXIT ();
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
                    modifiedInstance.setPath (instanceReference);
                    _repository->modifyInstance (request->nameSpace,
                        modifiedInstance, 
                        request->includeQualifiers, propertyList);
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
                    if (!_sendCreateRequests (indicationProviders, 
                        sourceNameSpace, requiredProperties, condition, 
                        queryLanguage,
                        instance,
                        request->userName, request->authType))
                    {
                        PEG_METHOD_EXIT ();
                        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                            _MSG_NOT_ACCEPTED);
                    }
    
                    //
                    //  Send Enable message to each provider
                    //
                    for (Uint8 i = 0; i < indicationProviders.size (); i++)
                    {
                        //
                        //  Send Enable message 
                        //
                        _sendEnable (indicationProviders [i]);
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
                    indicationProviders = _getDeleteParams 
                        (request->nameSpace, instance);
    
                    //
                    //  Send Delete requests
                    //
                    if (indicationProviders.size () > 0)
                    {
                        instanceReference.setNameSpace (request->nameSpace);
                        instance.setPath (instanceReference);
                        _sendDeleteRequests (indicationProviders, 
                            request->nameSpace,
                            instance,
                            request->userName, request->authType);
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
    //  FUTURE: Response should be sent only after Create or Delete
    //  response has been received
    //
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
    
    Base::_enqueueResponse (request, response);

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleDeleteInstanceRequest (const Message* message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_handleDeleteInstanceRequest");

    CIMDeleteInstanceRequestMessage* request = 
        (CIMDeleteInstanceRequestMessage*) message;

    CIMException cimException;

    try
    {
        //
        //  Check if instance may be deleted -- a filter or handler instance 
        //  referenced by a subscription instance may not be deleted
        //
        if (_canDelete (request->instanceName, request->nameSpace,
            request->userName))
        {
            if (request->instanceName.getClassName ().equal
                (PEGASUS_CLASSNAME_INDSUBSCRIPTION))
            {
                CIMInstance subscriptionInstance;
                Array <ProviderClassList> indicationProviders;

                _repository->read_lock ();

                try
                {
                subscriptionInstance = _repository->getInstance 
                    (request->nameSpace, request->instanceName);
                }
                catch (Exception e)
                {
                    _repository->read_unlock ();
                    PEG_METHOD_EXIT ();
                    throw e;
                }

                _repository->read_unlock ();

                indicationProviders = _getDeleteParams 
                    (request->nameSpace, subscriptionInstance);

                //
                //  Send Delete requests
                //
                CIMObjectPath instanceReference = request->instanceName;
                instanceReference.setNameSpace (request->nameSpace);
                subscriptionInstance.setPath (instanceReference);
                _sendDeleteRequests (indicationProviders,
                    request->nameSpace, subscriptionInstance,
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

                Base::_enqueueResponse (request, response);

                PEG_METHOD_EXIT ();
                return;
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
    //  FUTURE: Response should be sent only after Delete response has 
    //  been received
    //
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
    
    Base::_enqueueResponse (request, response);

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleProcessIndicationRequest (const Message* message)
{
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
    PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL4, 
		     "Received Indication " + 
		     indication.getClassName().getString());
    try
    {
        WQLSimplePropertySource propertySource = _getPropertySourceFromInstance(
            indication);

        //
        //  Check if property list contains all properties of class
        //  If so, set to null
        //
        Array <CIMName> propertyNames;
        CIMPropertyList propertyList;
        for (Uint8 i = 0; i < indication.getPropertyCount(); i++)
            propertyNames.append(indication.getProperty(i).getName());
        propertyList = _checkPropertyList (propertyNames, 
            request->nameSpace, indication.getClassName ());

        Array <CIMNamespaceName> nameSpaces;
        nameSpaces.append (request->nameSpace);

        matchedSubscriptions = _getMatchingSubscriptions(
            indication.getClassName (), nameSpaces, propertyList);

        for (Uint8 i = 0; i < matchedSubscriptions.size(); i++)
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
                if (!selectStatement.evaluateWhereClause(&propertySource))
                {
                    match = false;
                }
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

                CIMRequestMessage * handler_request =
                    new CIMHandleIndicationRequestMessage (
                        XmlWriter::getNextMessageId (),
                        request->nameSpace,
                        handlerNamedInstance,
                        formattedIndication,
                        QueueIdStack(_handlerService, getQueueId()));
                
                AsyncOpNode* op = this->get_op();

                AsyncLegacyOperationStart *async_req = 
                    new AsyncLegacyOperationStart(
                    get_next_xid(),
                    op,
                    _handlerService,
                    handler_request,
                    _queueId);

                PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL4, 
                    "Sending (SendForget) Indication to " + 
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

//cout << "newSubscriptions.size (): " << newSubscriptions.size () << endl;
    if (newSubscriptions.size () > 0)
    {
        CIMPropertyList requiredProperties;
        String condition;
        String queryLanguage;

        //
        //  Send Create or Modify request for each subscription that can newly 
        //  be supported
        //
        for (Uint8 i = 0; i < newSubscriptions.size (); i++)
        {
            CIMNamespaceName sourceNameSpace;
            _getCreateParams 
                (newSubscriptions [i].getPath ().getNameSpace (), 
                newSubscriptions [i], 
                requiredProperties, sourceNameSpace, condition, queryLanguage);

            //
            //  NOTE: These Create or Modify requests are not associated with a
            //  user request, so there is no associated authType or userName
            //  The Creator from the subscription instance is used for 
            //  userName, and authType is not set
            //
            CIMInstance instance = newSubscriptions [i];
            String creator = instance.getProperty (instance.findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_CREATOR)).getValue ().toString ();

            //
            //  Look up the subscription-provider pair in the subscription table
            //  If the entry is there, send a Modify request, 
            //  Otherwise, send a Create request
            //
            String tableKey = _generateKey 
                (newSubscriptions [i], indicationProviders [i].provider);
            if (_subscriptionTable.contains (tableKey))
            {
//cout << "case A: table contains key" << endl;
                //
                //  Send Modify requests
                //
                _sendModifyRequests (indicationProviders,
                    newSubscriptions [i].getPath ().getNameSpace (), 
                    requiredProperties, condition, queryLanguage, 
                    newSubscriptions [i], creator);
            }
            else
            {
//cout << "case B: table does not contain key" << endl;
                //
                //  Send Create requests
                //
                if (!_sendCreateRequests (indicationProviders,
                    sourceNameSpace, requiredProperties, condition, 
                    queryLanguage, newSubscriptions [i], creator))
                {
                    PEG_METHOD_EXIT ();
                    throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED, 
                        _MSG_NOT_ACCEPTED);
                }
    
                //
                //  Send Enable message to each provider
                //
                for (Uint8 j = 0; j < indicationProviders.size (); j++)
                {
                    //
                    //  Send Enable message
                    //
                    _sendEnable (indicationProviders [j]);
                }
            }
        }
    }

//cout << "formerSubscriptions.size (): " << formerSubscriptions.size () << endl;
    if (formerSubscriptions.size () > 0)
    {
        CIMPropertyList requiredProperties;
        String condition;
        String queryLanguage;

        //
        //  Send Delete or Modify request for each subscription that can no 
        //  longer be supported
        //
        for (Uint8 i = 0; i < formerSubscriptions.size (); i++)
        {
            //  NOTE: These Delete or Modify requests are not associated with a
            //  user request, so there is no associated authType or userName
            //  The Creator from the subscription instance is used for userName,
            //  and authType is not set
            CIMInstance instance = formerSubscriptions [i];
            String creator = instance.getProperty (instance.findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_CREATOR)).getValue ().toString ();

            //
            //  Look up the subscription-provider pair in the subscription table
            //  If class list contains only the class name from the current
            //  operation, send a Delete request
            //  Otherwise, send a Modify request
            //
            String tableKey = _generateKey 
                (formerSubscriptions [i], indicationProviders [i].provider);
            SubscriptionTableEntry tableValue;
            if (_subscriptionTable.lookup (tableKey, tableValue))
            {
//cout << "table contains key" << endl;
                //
                //  If class list contains only the class name from the current
                //  delete, send a Delete request
                //
                if ((tableValue.classList.size () == 1) &&
                    (tableValue.classList [0].equal (className)))
                {
//cout << "case C: classlist contains only this class" << endl;
                    _sendDeleteRequests (indicationProviders,
                        formerSubscriptions [i].getPath ().getNameSpace (), 
                        formerSubscriptions [i], creator);
                }

                //
                //  Otherwise, send a Modify request
                //
                else
                {
//cout << "case D: classlist contains more" << endl;
                    CIMNamespaceName sourceNameSpace;
                    _getCreateParams 
                        (formerSubscriptions [i].getPath ().getNameSpace (),
                        formerSubscriptions [i], 
                        requiredProperties, sourceNameSpace, condition, 
                        queryLanguage);

                    //
                    //  Send Modify requests
                    //
                    _sendModifyRequests (indicationProviders,
                        formerSubscriptions [i].getPath ().getNameSpace (), 
                        requiredProperties, condition, queryLanguage, 
                        formerSubscriptions [i], creator);
                }
            }
            else
            {
                //
                //  An error condition
                //
                //cout << "Error on delete: table does not contain key" << endl;
            }
        }

        //
        //  NOTE: When a provider that was previously serving a subscription 
        //  no longer serves the subscription due to a provider registration 
        //  change, an alert is always sent, even if there are still other 
        //  providers serving the subscription
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

    for (Uint8 i = 0; i < providers.size (); i++)
    {
        //
        //  Get list of affected subscriptions
        //
        providerSubscriptions.clear ();
        providerSubscriptions = _getProviderSubscriptions (providers [i]);
    
        //
        //  ATTN-CAKG-P2-20020426: For each subscription, if the terminated 
        //  provider was the only provider serving the subscription, 
        //  implement the subscription's On Fatal Error Policy
        //

        //
        //  NOTE: When a provider that was previously serving a subscription 
        //  no longer serves the subscription due to a provider termination, 
        //  an alert is always sent, even if there are still other providers 
        //  serving the subscription
        //
    
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
        _sendAlerts (providerSubscriptions, indicationInstance);
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
                //  Trap Destination property is required for SNMP 
                //  Handler subclass
                //
                _checkRequiredProperty (instance, _PROPERTY_TRAPDESTINATION, 
                    _MSG_PROPERTY);

                //
                //  SNMP Type property is required for SNMP Handler
                //
                _checkRequiredProperty (instance, _PROPERTY_SNMPTYPE, 
                    _MSG_PROPERTY);
            }
        }

        else
        {
            //
            //  A class not currently served by the Indication Service
            //
            PEG_METHOD_EXIT ();
            throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED,
                _MSG_CLASS_NOT_SERVED);
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
        String exceptionStr = _MSG_MISSING_REQUIRED;
        exceptionStr.append (propertyName.getString());
        exceptionStr.append (message);
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
            exceptionStr);
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
                String exceptionStr = _MSG_INVALID_VALUE;
                exceptionStr.append (theValue.toString ());
                exceptionStr.append (_MSG_FOR_PROPERTY);
                exceptionStr.append (propertyName.getString());
                PEG_METHOD_EXIT ();
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
                    exceptionStr);
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
                String exceptionStr = _MSG_MISSING_REQUIRED;
                exceptionStr.append (otherPropertyName.getString());
                exceptionStr.append (_MSG_PROPERTY);
                PEG_METHOD_EXIT ();
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
                    exceptionStr);
            }
            else
            {
                CIMProperty otherProperty = instance.getProperty
                    (instance.findProperty (otherPropertyName));
                CIMValue theOtherValue = otherProperty.getValue ();
                if (theOtherValue.isNull ())
                {
                    String exceptionStr = _MSG_MISSING_REQUIRED;
                    exceptionStr.append (otherPropertyName.getString());
                    exceptionStr.append (_MSG_PROPERTY);
                    PEG_METHOD_EXIT ();
                    throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
                        exceptionStr);
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
                String exceptionStr = otherPropertyName.getString();
                exceptionStr.append (_MSG_PROPERTY_PRESENT);
                exceptionStr.append (propertyName.getString());
                exceptionStr.append (_MSG_VALUE_NOT);
                exceptionStr.append (CIMValue (otherValue).toString ());
                PEG_METHOD_EXIT ();
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
                    exceptionStr);
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

Boolean IndicationService::_canModify (
    const CIMModifyInstanceRequestMessage * request,
    const CIMObjectPath & instanceReference,
    CIMInstance & instance)
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

    _checkPropertyWithOther (instance, _PROPERTY_STATE, _PROPERTY_OTHERSTATE,
        (Uint16) _STATE_ENABLED, (Uint16) _STATE_OTHER, _validStates);

    //
    //  Get creator from instance
    //
    String creator = instance.getProperty (instance.findProperty 
        (PEGASUS_PROPERTYNAME_INDSUB_CREATOR)).getValue ().toString ();

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
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
    }

    _repository->read_unlock ();

    //
    //  Get creator from instance
    //
    String creator = instance.getProperty (instance.findProperty 
        (PEGASUS_PROPERTYNAME_INDSUB_CREATOR)).getValue ().toString ();

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
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
        for (Uint8 i = 0; i < subscriptions.size(); i++)
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
                String exceptionStr = _MSG_REFERENCED;
                PEG_METHOD_EXIT ();
                throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
            }
        }
    }

    PEG_METHOD_EXIT ();
    return true;
}


Array <CIMInstance> IndicationService::_getActiveSubscriptions () const
{
    Array <CIMInstance> activeSubscriptions;
    Array <CIMNamespaceName> nameSpaceNames;
    Array <CIMInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getActiveSubscriptions");

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _getNameSpaceNames ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        subscriptions = _getSubscriptions (nameSpaceNames [i]);

        //
        //  Process each subscription
        //
        for (Uint8 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = 
                subscriptions [j].getProperty
                (subscriptions [j].findProperty 
                (_PROPERTY_STATE)).getValue ();
            subscriptionStateValue.get (subscriptionState);

    
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
    return activeSubscriptions;
}


Array <CIMInstance> IndicationService::_getMatchingSubscriptions (
    const CIMName & supportedClass,
    const Array <CIMNamespaceName> nameSpaces,
    const CIMPropertyList & supportedProperties) 
{
    Array <CIMInstance> matchingSubscriptions;
    Array <CIMNamespaceName> nameSpaceNames;
    Array <CIMInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getMatchingSubscriptions");

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _getNameSpaceNames ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        subscriptions = _getSubscriptions (nameSpaceNames [i]);

        //
        //  Process each subscription
        //
        for (Uint8 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = 
                subscriptions [j].getProperty
                (subscriptions [j].findProperty 
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
                CIMName indicationClassName;
                Array <CIMName> indicationSubclasses;
                CIMNamespaceName sourceNameSpace;
                CIMPropertyList propertyList;
                Boolean match;

                //
                //  Get filter properties
                //
                _getFilterProperties (subscriptions [j],
                    nameSpaceNames [i], filterQuery, sourceNameSpace);
                selectStatement = _getSelectStatement (filterQuery);
            
                //
                //  Get indication class name from filter query (FROM clause)
                //
                indicationClassName = _getIndicationClassName (selectStatement,
                    sourceNameSpace);
            
                //
                //  Get list of subclass names for indication class
                //
                indicationSubclasses = _getIndicationSubclasses
                    (sourceNameSpace, indicationClassName);

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
                                 k < propertyList.size () && match; 
                                 k++)
                            {
                                if (!ContainsCIMName
                                    (supportedProperties.getPropertyNameArray(),
                                    propertyList[k]))
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
                        //  CIMInstances returned from repository do not 
                        //  include namespace  
                        //  Set namespace here
                        //
                        CIMObjectPath instanceName = 
                            subscriptions [j].getPath ();
                        instanceName.setNameSpace (nameSpaceNames [i]);
                        subscriptions [j].setPath (instanceName);
                        matchingSubscriptions.append (subscriptions [j]);
                    }
                }  // if subscription includes supported class
            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

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
    Array <CIMNamespaceName> nameSpaceNames;
    Array <CIMInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getModifiedSubscriptions");

    newSubscriptions.clear ();
    formerSubscriptions.clear ();

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _getNameSpaceNames ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        subscriptions = _getSubscriptions (nameSpaceNames [i]);

        //
        //  Process each subscription
        //
        for (Uint8 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = 
                subscriptions [j].getProperty
                (subscriptions [j].findProperty 
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
                CIMName indicationClassName;
                Array <CIMName> indicationSubclasses;
                CIMNamespaceName sourceNameSpace;
                CIMPropertyList requiredProperties;
                Boolean newMatch;
                Boolean formerMatch;

                //
                //  Get filter properties
                //
                _getFilterProperties (subscriptions [j],
                    nameSpaceNames [i], filterQuery, sourceNameSpace);
                selectStatement = _getSelectStatement (filterQuery);
            
                //
                //  Get indication class name from filter query (FROM clause)
                //
                indicationClassName = _getIndicationClassName (selectStatement,
                    sourceNameSpace);
            
                //
                //  Get list of subclass names for indication class
                //
                indicationSubclasses = _getIndicationSubclasses
                    (sourceNameSpace, indicationClassName);

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
                        //  CIMInstances returned from repository do not 
                        //  include namespace  
                        //  Set namespace here
                        //
                        CIMObjectPath instanceName = 
                            subscriptions [j].getPath ();
                        instanceName.setNameSpace (nameSpaceNames [i]);
                        subscriptions [j].setPath (instanceName);
                        newSubscriptions.append (subscriptions [j]);
                    }
                    else if (!newMatch && formerMatch)
                    {
                        //
                        //  CIMInstances returned from repository do not 
                        //  include namespace  
                        //  Set namespace here
                        //
                        CIMObjectPath instanceName = 
                            subscriptions [j].getPath ();
                        instanceName.setNameSpace (nameSpaceNames [i]);
                        subscriptions [j].setPath (instanceName);
                        formerSubscriptions.append (subscriptions [j]);
                    }
                }  // if subscription includes supported class
            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

    PEG_METHOD_EXIT ();
}

Array <CIMNamespaceName> IndicationService::_getNameSpaceNames (void) const
{
    Array <CIMNamespaceName> nameSpaceNames;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::__getNameSpaceNames");

    _repository->read_lock ();

    try
    {
        nameSpaceNames = _repository->enumerateNameSpaces ();
    }
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
    catch (CIMException e)
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
            for (Uint8 i = 0; i < requiredProperties.size (); i++)
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

    //
    //  Iterate through the subscription table
    //
    for (SubscriptionTable::Iterator i = _subscriptionTable.start (); i; i++)
    {
        //
        //  If provider matches, append subscription to the list
        //
        if (i.value ().provider.identical (provider))
        {
            providerSubscriptions.append (i.value ().subscription);
        }
    }

    PEG_METHOD_EXIT ();
    return providerSubscriptions;
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
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, exceptionStr);
    }
    catch (MissingNullTerminator & mnt)
    {
        String exceptionStr = mnt.getMessage ();
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
        theClass = _repository->getClass (nameSpaceName, indicationClassName);
    }
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
        String exceptionStr = _MSG_INVALID_CLASSNAME;
        exceptionStr.append (indicationClassName.getString());
        exceptionStr.append (_MSG_IN_FROM);
        exceptionStr.append (PEGASUS_CLASSNAME_INDFILTER.getString());
        exceptionStr.append (" ");
        exceptionStr.append (_PROPERTY_QUERY.getString());
        exceptionStr.append (_MSG_PROPERTY);
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, 
            exceptionStr);
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
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
        indicationClass = _repository->getClass (nameSpaceName, 
            indicationClassName);
    }
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
    catch (Exception e)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
        throw e;
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
    for (Uint8 i = 0; i < subscriptions.size (); i++)
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
            Array <ProviderClassList> indicationProviders;
            indicationProviders = _getDeleteParams (nameSpace, 
                subscriptions [i]);

            //
            //  Send Delete requests
            //
            CIMInstance instance = subscriptions [i];
            String creator = instance.getProperty (instance.findProperty
                (PEGASUS_PROPERTYNAME_INDSUB_CREATOR)).getValue ().toString ();
            CIMObjectPath instanceName = 
                subscriptions [i].getPath ();
            instanceName.setNameSpace (nameSpace);
            subscriptions [i].setPath (instanceName);
            _sendDeleteRequests (indicationProviders, nameSpace, 
                subscriptions [i], creator);

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
                //  ATTN-CAKG-P3-20020425: Log a message
                //
            }

            _repository->write_unlock ();
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

    CIMNamespaceName nameSpace = subscription.getNameSpace ();
    subscription.setNameSpace (CIMNamespaceName ());

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
        //  ATTN-CAKG-P3-20020425: Log a message
        //
        //cout << "Exception: " << exception.getMessage () << endl;
    }

    _repository->write_unlock ();

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
    Array <ProviderClassList> & indicationProviders,
    CIMPropertyList & propertyList,
    CIMNamespaceName & sourceNameSpace,
    String & condition,
    String & queryLanguage)
{
    String filterQuery;
    WQLSelectStatement selectStatement;
    CIMName indicationClassName;
    Array <CIMName> indicationSubclasses;
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
    if ((selectStatement.getSelectPropertyNameCount () > 0) ||
        (selectStatement.getWherePropertyNameCount () > 0))
    {
        CIMName indicationClassName = _getIndicationClassName (selectStatement,
            sourceNameSpace);
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

    PEG_METHOD_EXIT ();
}
    
Array <ProviderClassList> IndicationService::_getDeleteParams (
    const CIMNamespaceName & nameSpaceName,
    const CIMInstance & subscriptionInstance)
{
    String filterQuery;
    CIMNamespaceName sourceNameSpace;
    WQLSelectStatement selectStatement;
    CIMName indicationClassName;
    Array <CIMName> indicationSubclasses;
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

    PEG_METHOD_EXIT ();
    return indicationProviders;
}
    

void IndicationService::_sendCreateRequestsCallBack(AsyncOpNode *op, 
						    MessageQueue *q, 
						    void *parm)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_sendCreateRequestsCallBack");

    IndicationService *service = 
      static_cast<IndicationService *>(q);
    struct enableProviderList *epl = 
       reinterpret_cast<struct enableProviderList *>(parm);
    
   AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
   AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());
   CIMRequestMessage *request = reinterpret_cast<CIMRequestMessage *>
      ((static_cast<AsyncLegacyOperationStart *>(asyncRequest))->get_action());
   CIMCreateSubscriptionResponseMessage * response =
      reinterpret_cast 
      <CIMCreateSubscriptionResponseMessage *>
      ((static_cast <AsyncLegacyOperationResult *>
	(asyncReply))->get_result());
   
   PEGASUS_ASSERT(response != 0);
   if (response->cimException.getCode() == CIM_ERR_SUCCESS)
   {
      //cout << "Create accepted: " << response->messageId << endl;
      
      //
      //  Insert entry into subscription table 
      //
      service->_insertEntry(*(epl->cni), epl->pcl->provider, epl->pcl->classList);
      
//      service->_insertEntry (subscription, indicationProviders [i].provider,
//			     indicationProviders [i].classList);
   }
   else
   {
      //
      //  ATTN-CAKG-P3-20020326: Log a message
      //
      
      //cout << "Create rejected: " << response->messageId << endl;
      //cout << "Error code: " << response->cimException.getCode () << endl;
      //cout << response->cimException.getMessage () << endl;
   }
   
   delete epl;
   delete request;
   delete response;
   delete asyncRequest;
   delete asyncReply;
   op->release();
   service->return_op(op);

    PEG_METHOD_EXIT ();
}


Boolean IndicationService::_sendCreateRequests
    (const Array <ProviderClassList> & indicationProviders,
     const CIMNamespaceName & nameSpace,
     const CIMPropertyList & propertyList,
     const String & condition,
     const String & queryLanguage,
     const CIMInstance & subscription,
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

    //
    //  Send Create request to each provider
    //

    for (Uint8 i = 0; i < indicationProviders.size (); i++)
    {
       
       struct enableProviderList *epl = 
           new enableProviderList(indicationProviders[i], 
				 subscription);
       
        CIMCreateSubscriptionRequestMessage * request =
            new CIMCreateSubscriptionRequestMessage
                (XmlWriter::getNextMessageId (),
                nameSpace,
                subscription,
		 epl->pcl->classList, 
		 epl->pcl->provider, 
		 epl->pcl->providerModule,
		 propertyList,
		 repeatNotificationPolicy,
		 condition,
		 queryLanguage,
		 QueueIdStack (_providerManager, getQueueId ()),
		 authType,
		 userName);
//cout << "Create message ID: " << request->messageId << endl;
	
        AsyncOpNode* op = this->get_op(); 

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

	SendAsync(op, 
		  _providerManager, 
		  IndicationService::_sendCreateRequestsCallBack,
		  this, 
		  (void *)epl);
	

        // AsyncReply * async_reply = SendWait (async_req);
    }

    //
    //  FUTURE: Should only return true if at least one provider accepted
    //  the subscription
    //
    //Boolean result = (accepted > 0);
    Boolean result = true;
    return result;

    PEG_METHOD_EXIT ();
}

void IndicationService::_sendModifyRequestsCallBack (
    AsyncOpNode * op,
    MessageQueue * q,
    void * parm)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_sendModifyRequestsCallBack");

    IndicationService *service = static_cast <IndicationService *> (q);
    struct enableProviderList * epl = reinterpret_cast
        <struct enableProviderList *> (parm);
   
    AsyncRequest * asyncRequest = static_cast <AsyncRequest *>
        (op->get_request ());
    AsyncReply * asyncReply = static_cast <AsyncReply *> (op->get_response ());
    CIMRequestMessage * request = reinterpret_cast <CIMRequestMessage *>
        ((static_cast <AsyncLegacyOperationStart *> 
        (asyncRequest))->get_action ());

    //
    //  ATTN: Check for return value indicating invalid queue ID
    //  If received, need to find Provider Manager Service queue ID
    //  again
    //
   
    CIMModifySubscriptionResponseMessage * response = reinterpret_cast
        <CIMModifySubscriptionResponseMessage *>
        ((static_cast <AsyncLegacyOperationResult *>
	(asyncReply))->get_result ());
   
    PEGASUS_ASSERT(response != 0);
    if (response->cimException.getCode () == CIM_ERR_SUCCESS)
    {
        //cout << "Modify accepted: " << response->messageId << endl;
   
        //
        //  Remove old entry from subscription table and insert 
        //  updated entry
        //
        String tableKey = service->_generateKey
            (* (epl->cni), epl->pcl->provider);
        service->_subscriptionTable.remove (tableKey);
        //cout << "Old entry removed: " << tableKey << endl;
        service->_insertEntry (* (epl->cni), epl->pcl->provider, 
            epl->pcl->classList);
    }
    else
    {
        //
        //  ATTN-CAKG-P3-20020326: Log a message
        //  Should the table entry be updated anyway?
        //
        //cout << "Modify rejected: " << response->messageId << endl;
        //cout << "Error code: " << response->cimException.getCode () << endl;
        //cout << response->cimException.getMessage () << endl;
    }

    delete epl;
    delete request;
    delete response;
    delete asyncRequest;
    delete asyncReply;
    op->release ();
    service->return_op (op);
}


void IndicationService::_sendModifyRequests
    (const Array <ProviderClassList> & indicationProviders,
     const CIMNamespaceName & nameSpace,
     const CIMPropertyList & propertyList,
     const String & condition,
     const String & queryLanguage,
     const CIMInstance & subscription,
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

    //
    //  Send Modify request to each provider
    //
    for (Uint8 i = 0; i < indicationProviders.size (); i++)
    {

       struct enableProviderList *epl = 
	  new enableProviderList(indicationProviders[i], 
				 subscription);

       CIMModifySubscriptionRequestMessage * request =
	  new CIMModifySubscriptionRequestMessage(
	     XmlWriter::getNextMessageId (),
	     nameSpace,
	     subscription,
	     epl->pcl->classList, 
	     epl->pcl->provider, 
	     epl->pcl->providerModule,
	     propertyList,
	     repeatNotificationPolicy,
	     condition,
	     queryLanguage,
	     QueueIdStack (_providerManager, getQueueId ()),
	     authType,
	     userName);
//cout << "Modify message ID: " << request->messageId << endl;

        AsyncOpNode* op = this->get_op();

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);

	SendAsync(op, 
		  _providerManager, 
		  IndicationService::_sendModifyRequestsCallBack,
		  this, 
		  (void *)epl);

        // AsyncReply * async_reply = SendWait (async_req);
    }

    PEG_METHOD_EXIT ();
}


void IndicationService::_sendDeleteRequestsCallBack (
    AsyncOpNode * op, 
    MessageQueue * q, 
    void * parm)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_sendDeleteRequestsCallBack");

    IndicationService * service = static_cast <IndicationService *> (q);
    struct enableProviderList * epl = 
        reinterpret_cast <struct enableProviderList *> (parm);

    AsyncRequest * asyncRequest = static_cast <AsyncRequest *>
        (op->get_request ());
    AsyncReply * asyncReply = static_cast <AsyncReply *> (op->get_response ());
    CIMRequestMessage * request = reinterpret_cast <CIMRequestMessage *>
        ((static_cast <AsyncLegacyOperationStart *> 
        (asyncRequest))->get_action ());

    CIMDeleteSubscriptionResponseMessage * response =
        reinterpret_cast <CIMDeleteSubscriptionResponseMessage *>
        ((static_cast <AsyncLegacyOperationResult *>
	(asyncReply))->get_result());
   
    //
    //  ATTN: Check for return value indicating invalid queue ID
    //  If received, need to find Provider Manager Service queue ID again
    //
   
    PEGASUS_ASSERT (response != 0);
    if (response->cimException.getCode () == CIM_ERR_SUCCESS)
    {
        //cout << "Delete accepted: " << response->messageId << endl;
        //
        //  Remove entry from subscription table 
        //
        String tableKey = service->_generateKey 
            (*(epl->cni), epl->pcl->provider);
        service->_subscriptionTable.remove (tableKey);
        //cout << "Entry removed: " << tableKey << endl;
    }
    else
    {
        //
        //  ATTN-CAKG-P3-20020326: Log a message
        //  Should the table entry be removed anyway?
        //
        //cout << "Delete rejected: " << response->messageId << endl;
        //cout << "Error code: " << response->cimException.getCode () << endl;
        //cout << response->cimException.getMessage () << endl;
    }

    delete epl;
    delete request;
    delete response;
    delete asyncRequest;
    delete asyncReply;
    op->release ();
    service->return_op (op);
}


void IndicationService::_sendDeleteRequests
    (const Array <ProviderClassList> & indicationProviders,
     const CIMNamespaceName & nameSpace,
     const CIMInstance & subscription,
     const String & userName,
     const String & authType)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_sendDeleteRequests");

    //
    //  Send Delete request to each provider
    //
    for (Uint8 i = 0; i < indicationProviders.size (); i++)
    {

       struct enableProviderList *epl = 
	  new enableProviderList(indicationProviders[i], 
				 subscription);

        CIMDeleteSubscriptionRequestMessage * request =
            new CIMDeleteSubscriptionRequestMessage
            (XmlWriter::getNextMessageId (),
            nameSpace,
            subscription,
	     epl->pcl->classList,
	     epl->pcl->provider, 
	     epl->pcl->providerModule,
	     QueueIdStack (_providerManager, getQueueId ()),
	     authType,
	     userName);
//cout << "Delete message ID: " << request->messageId << endl;
	
        AsyncOpNode* op = this->get_op();

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
                op,
                _providerManager,
                request,
                _queueId);
	
	SendAsync(op, 
		  _providerManager, 
		  IndicationService::_sendDeleteRequestsCallBack,
		  this, 
		  (void *)epl);

	// <<< Fri Apr  5 06:05:55 2002 mdd >>>
	// AsyncReply * async_reply = SendWait (async_req);
    }

    PEG_METHOD_EXIT ();
}

String IndicationService::_generateKey (
    const CIMInstance & subscription,
    const CIMInstance provider)
{
    String tableKey;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_generateKey");

    //
    //  Append subscription namespace name to key
    //
    tableKey.append (subscription.getPath ().getNameSpace ().getString());

    //
    //  Append subscription filter key values to key
    //
    CIMValue filterVal = subscription.getProperty
        (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();
            
    CIMObjectPath filterRef;
    filterVal.get (filterRef);

    Array <CIMKeyBinding> filterKeyBindings = filterRef.getKeyBindings ();
    for (Uint8 i = 0; i < filterKeyBindings.size (); i++)
    {
        tableKey.append (filterKeyBindings [i].getValue ());
    }

    //
    //  Append subscription handler key values to key
    //
    CIMValue handlerVal = subscription.getProperty
        (subscription.findProperty
        (_PROPERTY_HANDLER)).getValue ();
            
    CIMObjectPath handlerRef;
    handlerVal.get (handlerRef);

    Array <CIMKeyBinding> HandlerKeyBindings = handlerRef.getKeyBindings ();
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

    PEG_METHOD_EXIT ();
    return tableKey;
}

void IndicationService::_insertEntry (
    const CIMInstance & subscription,
    const CIMInstance & provider,
    const Array <CIMName> classList)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_insertEntry");

    String tableKey = _generateKey 
        (subscription, provider);
    SubscriptionTableEntry entry;
    entry.subscription = subscription;
    entry.provider = provider;
    entry.classList = classList;
    _subscriptionTable.insert (tableKey, entry);
//cout << "Entry inserted: " << tableKey << endl;

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
      //cout << "Alert accepted: " << response->messageId << endl;
   }
   else
   {
      //cout << "Alert rejected: " << response->messageId << endl;
      //cout << "Error code: " << response->cimException.getCode () << endl;
      //cout << response->cimException.getMessage () << endl;
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
    for (Uint8 k = 0; k < handlers.size (); k++)
    {
        CIMHandleIndicationRequestMessage * handler_request =
            new CIMHandleIndicationRequestMessage (
                XmlWriter::getNextMessageId (),
                handlers[k].getPath ().getNameSpace (),
                handlers[k],
                alertInstance,
                QueueIdStack (_handlerService, getQueueId ()));
//cout << "Alert message ID: " << handler_request->messageId << endl;

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

void IndicationService::_sendEnableCallBack(AsyncOpNode *op,
					   MessageQueue *q,
					   void *parm)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_sendEnableCallBack");

   IndicationService *service = 
      static_cast<IndicationService *>(q);
   
   AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
   AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());
   CIMEnableIndicationsRequestMessage *request = 
      static_cast<CIMEnableIndicationsRequestMessage *>
      ((static_cast<AsyncLegacyOperationStart *>(asyncRequest))->get_action());
   
//    Message *response =  
//       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

   CIMEnableIndicationsResponseMessage * response =
      reinterpret_cast<CIMEnableIndicationsResponseMessage *>
      ((static_cast <AsyncLegacyOperationResult *>
        (asyncReply))->get_result());

    if (response->cimException.getCode () == CIM_ERR_SUCCESS)
    {
      //cout << "Enable accepted: " << response->messageId << endl;
    }
    else
    {
      //cout << "Enable rejected: " << response->messageId << endl;
      //cout << "Error code: " << response->cimException.getCode () << endl;
      //cout << response->cimException.getMessage () << endl;
    }

   //
   //  ATTN: Check for return value indicating invalid queue ID
   //  If received, need to find Provider Manager Service queue ID
   //  again
   //

   delete request;
   delete response;
   delete asyncRequest;
   delete asyncReply;
   op->release();
   service->return_op(op);

    PEG_METHOD_EXIT ();
}


void IndicationService::_sendEnable (
    const ProviderClassList & enableProvider)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "IndicationService::_sendEnable");
 
    CIMEnableIndicationsRequestMessage * request =
        new CIMEnableIndicationsRequestMessage
            (XmlWriter::getNextMessageId (),
             enableProvider.provider,
             enableProvider.providerModule,
             QueueIdStack (_providerManager, getQueueId ()));
//cout << "Enable message ID: " << request->messageId << endl;

    AsyncOpNode* op = this->get_op (); 

    AsyncLegacyOperationStart * async_req =
        new AsyncLegacyOperationStart
            (get_next_xid (),
            op,
            _providerManager,
            request,
            _queueId);

    SendAsync (op, 
	       _providerManager, 
	       IndicationService::_sendEnableCallBack,
	       this, 
	       NULL);

//    AsyncReply * async_reply = SendWait (async_req);
    // << Wed Apr 10 12:26:16 2002 mdd >>

    PEG_METHOD_EXIT ();
}

WQLSimplePropertySource IndicationService::_getPropertySourceFromInstance(
    CIMInstance& indicationInstance)
{
    Boolean booleanValue;
    WQLSimplePropertySource source;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "IndicationService::_getPropertySourceFromInstance");

    for (Uint8 i=0; i < indicationInstance.getPropertyCount(); i++)
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
    The name of the Trap Destination property for SNMP Mapper Indication 
    Handler subclass
 */
const CIMName IndicationService::_PROPERTY_TRAPDESTINATION = 
              CIMName ("TrapDestination");

/**
    The name of the SNMP Type property for SNMP Indication Handler class
 */
const CIMName IndicationService::_PROPERTY_SNMPTYPE = 
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

const char IndicationService::_MSG_INVALID_VALUE [] =
    "Invalid value ";

const char IndicationService::_MSG_FOR_PROPERTY [] =
    " for property ";

const char IndicationService::_MSG_CLASS_NOT_SERVED [] =
    "The specified class is not served by the Indication Service";

PEGASUS_NAMESPACE_END
