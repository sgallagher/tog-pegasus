//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//             (carolann_graves@hp.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>

#include "IndicationConstants.h"
#include "SubscriptionRepository.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

SubscriptionRepository::SubscriptionRepository (
    CIMRepository * repository)
    : _repository (repository)
{
}

SubscriptionRepository::~SubscriptionRepository ()
{
}

CIMObjectPath SubscriptionRepository::createInstance (
    CIMInstance instance,
    const CIMNamespaceName & nameSpace,
    const String & userName,
    const AcceptLanguages & acceptLanguages,
    const ContentLanguages & contentLanguages,
    Boolean enabled)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::createInstance");

    CIMObjectPath instanceRef;

    //
    //  Add creator property to Instance
    //  NOTE: userName is only set if authentication is turned on
    //
    String currentUser = userName;
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
    // headers in the HTTP message, and may be empty
    AcceptLanguages acceptLangs = acceptLanguages;
    if (instance.findProperty (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS) ==
        PEG_NOT_FOUND)
    {
        instance.addProperty (CIMProperty
            (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS,
            acceptLangs.toString ()));
    }
    else
    {
        CIMProperty langs = instance.getProperty
            (instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS));
        langs.setValue (CIMValue (acceptLangs.toString ()));
    }

    ContentLanguages contentLangs = contentLanguages;
    if (instance.findProperty (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS) ==
        PEG_NOT_FOUND)
    {
        instance.addProperty (CIMProperty
            (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS,
            contentLangs.toString ()));
    }
    else
    {
        CIMProperty langs = instance.getProperty
            (instance.findProperty
            (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS));
        langs.setValue (CIMValue (contentLangs.toString ()));
    }
    // l10n -end

    if ((instance.getClassName ().equal
        (PEGASUS_CLASSNAME_INDSUBSCRIPTION)) ||
        (instance.getClassName ().equal
        (PEGASUS_CLASSNAME_FORMATTEDINDSUBSCRIPTION)))
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
    try
    {
        instanceRef = _repository->createInstance (nameSpace, instance);
    }
    catch (const CIMException &)
    {
        PEG_METHOD_EXIT ();
        throw;
    }
    catch (const Exception & exception)
    {
        PEG_METHOD_EXIT ();
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exception.getMessage ());
    }

    PEG_METHOD_EXIT ();
    return instanceRef;
}

Boolean SubscriptionRepository::getActiveSubscriptions (
    Array <CIMInstance> & activeSubscriptions) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::getActiveSubscriptions");

    Array <CIMNamespaceName> nameSpaceNames;
    Array <CIMInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;
    Boolean invalidInstance = false;

    activeSubscriptions.clear ();

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint32 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        subscriptions = getSubscriptions (nameSpaceNames [i]);

        //
        //  Process each subscription
        //
        for (Uint32 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            if (!getState (subscriptions [j], subscriptionState))
            {
                //
                //  This instance from the repository is corrupted
                //  Skip it
                //
                invalidInstance = true;
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
                CIMObjectPath instanceName = subscriptions [j].getPath ();
                instanceName.setNameSpace (nameSpaceNames [i]);
                subscriptions [j].setPath (instanceName);
                activeSubscriptions.append (subscriptions [j]);
            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

    PEG_METHOD_EXIT ();
    return invalidInstance;
}

Array <CIMInstance> SubscriptionRepository::getAllSubscriptions () const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::getAllSubscriptions");

    Array <CIMNamespaceName> nameSpaceNames;
    Array <CIMInstance> subscriptions;
    Array <CIMInstance> allSubscriptions;

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Get all subscriptions from each namespace in the repository
    //
    for (Uint32 i = 0; i < nameSpaceNames.size (); i++)
    {
        //
        //  Get all subscriptions in current namespace
        //
        subscriptions = getSubscriptions (nameSpaceNames [i]);

        //
        //  Append subscriptions in current namespace to list of all
        //  subscriptions
        //
        allSubscriptions.appendArray (subscriptions);
    }

    PEG_METHOD_EXIT ();
    return allSubscriptions;
}

Array <CIMInstance> SubscriptionRepository::getSubscriptions (
    const CIMNamespaceName & nameSpace) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::getSubscriptions");

    Array <CIMInstance> subscriptions;

    //
    //  Get existing subscriptions in specified namespace
    //
    try
    {
        subscriptions = _repository->enumerateInstances
            (nameSpace, PEGASUS_CLASSNAME_INDSUBSCRIPTION);

        //
        //  Process each subscription
        //
        for (Uint32 i = 0; i < subscriptions.size (); i++)
        {
            //
            //  CIMInstances returned from repository do not include
            //  namespace
            //  Set namespace here
            //
            CIMObjectPath instanceName = subscriptions [i].getPath ();
            instanceName.setNameSpace (nameSpace);
            subscriptions [i].setPath (instanceName);
        }
    }
    catch (const CIMException& e)
    {
        //
        //  Some namespaces may not include the subscription class
        //  In that case, just return no subscriptions
        //  Any other exception is an error
        //
        if (e.getCode () != CIM_ERR_INVALID_CLASS)
        {
            PEG_METHOD_EXIT ();
            throw;
        }
    }

    PEG_METHOD_EXIT ();
    return subscriptions;
}

Boolean SubscriptionRepository::getState (
    const CIMInstance & instance,
    Uint16 & state) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::getState");

    Uint32 stateIndex = instance.findProperty (_PROPERTY_STATE);
    if (stateIndex != PEG_NOT_FOUND)
    {
        CIMValue stateValue = instance.getProperty
            (stateIndex).getValue ();
        if (stateValue.isNull ())
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                Tracer::LEVEL2,
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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
               Tracer::LEVEL2,
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
        }
    }
    else
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
            Tracer::LEVEL2,
            "Missing SubscriptionState property");

        //
        //  This is a corrupted/invalid instance
        //
        return false;
    }

    PEG_METHOD_EXIT ();
    return true;
}

CIMInstance SubscriptionRepository::deleteSubscription (
    CIMObjectPath & subscription)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::deleteSubscription");

    CIMInstance subscriptionInstance;
    CIMNamespaceName nameSpace = subscription.getNameSpace ();
    subscription.setNameSpace (CIMNamespaceName ());

    //
    //  Get instance from repository
    //
    try
    {
        subscriptionInstance = _repository->getInstance (nameSpace,
            subscription);
    }
    catch (Exception & exception)
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
            "Exception caught in retrieving subscription (" +
            subscriptionInstance.getPath ().toString () + "): " +
            exception.getMessage ());

        //
        //  If the subscription could not be retrieved, it may already have
        //  been deleted by another thread
        //
        PEG_METHOD_EXIT ();
        return CIMInstance ();
    }

    //
    //  Delete the subscription instance
    //
    try
    {
        _repository->deleteInstance (nameSpace, subscription);
    }
    catch (Exception & exception)
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
            "Exception caught in deleting subscription (" +
            subscriptionInstance.getPath ().toString () + "): " +
            exception.getMessage ());

        //
        //  If the subscription could not be deleted, it may already have
        //  been deleted by another thread
        //
        PEG_METHOD_EXIT ();
        return CIMInstance ();
    }

    //
    //  Reset namespace in object path
    //
    subscription.setNameSpace (nameSpace);

    PEG_METHOD_EXIT ();
    return subscriptionInstance;
}

Array <CIMInstance> SubscriptionRepository::deleteReferencingSubscriptions (
    const CIMNamespaceName & nameSpace,
    const CIMName & referenceProperty,
    const CIMObjectPath & handler)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::deleteReferencingSubscriptions");

    Array <CIMInstance> subscriptions;
    Array <CIMInstance> deletedSubscriptions;

    //
    //  Get all subscriptions in all namespaces
    //
    subscriptions = getAllSubscriptions ();

    //
    //  Check each subscription for a reference to the specified instance
    //
    for (Uint32 i = 0; i < subscriptions.size (); i++)
    {
        //
        //  Get the reference property value from the subscription instance
        //
        CIMValue propValue = subscriptions [i].getProperty
            (subscriptions [i].findProperty (referenceProperty)).getValue ();
        CIMObjectPath ref;
        propValue.get (ref);

        //
        //  If the Handler reference property value includes namespace, check
        //  if it is the namespace of the Handler being deleted.
        //  If the Handler reference property value does not include namespace,
        //  check if the current subscription namespace is the namespace of the
        //  Handler being deleted.
        //
        CIMNamespaceName handlerNS = ref.getNameSpace ();
        if (((handlerNS.isNull ()) &&
            (subscriptions[i].getPath ().getNameSpace () == nameSpace))
            || (handlerNS == nameSpace))
        {
            //
            //  Remove Host and Namespace from reference property value, if
            //  present, before comparing
            //
            CIMObjectPath href ("", CIMNamespaceName (),
                ref.getClassName (), ref.getKeyBindings ());

            //
            //  Remove Host and Namespace from reference of handler instance to
            //  be deleted, if present, before comparing
            //
            CIMObjectPath iref ("", CIMNamespaceName (),
                handler.getClassName (), handler.getKeyBindings ());

            //
            //  If the current subscription references the specified instance,
            //  delete it
            //
            if (iref == href)
            {
                //
                //  Delete referencing subscription instance from repository
                //
                try
                {
                    //
                    //  Namespace and host must not be set in path passed to
                    //  repository
                    //
                    CIMObjectPath path ("", CIMNamespaceName (),
                        subscriptions [i].getPath ().getClassName (),
                        subscriptions [i].getPath ().getKeyBindings ());
                    _repository->deleteInstance
                        (subscriptions [i].getPath ().getNameSpace (), path);
                }
                catch (Exception & exception)
                {
                    //
                    //  Deletion of referencing subscription failed
                    //
                    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                        Tracer::LEVEL2,
                        "Exception caught deleting referencing subscription (" +
                        subscriptions [i].getPath ().toString () + "): " +
                        exception.getMessage ());
                }
 
                deletedSubscriptions.append (subscriptions [i]);
            }
        }
    }

    PEG_METHOD_EXIT ();
    return deletedSubscriptions;
}

CIMInstance SubscriptionRepository::getHandler (
    const CIMInstance & subscription) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::getHandler");

    CIMValue handlerValue;
    CIMObjectPath handlerRef;
    CIMInstance handlerInstance;
    CIMNamespaceName nameSpaceName;

    //
    //  Get Handler reference from subscription instance
    //
    handlerValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_HANDLER)).getValue ();

    handlerValue.get (handlerRef);

    //
    //  Get handler namespace - if not set in Handler reference property value,
    //  namespace is the namespace of the subscription
    //
    nameSpaceName = handlerRef.getNameSpace ();
    if (nameSpaceName.isNull ())
    {
        nameSpaceName = subscription.getPath ().getNameSpace ();
    }

    //
    //  Get Handler instance from the repository
    //
    try
    {
        handlerInstance = _repository->getInstance
            (nameSpaceName, handlerRef, false, false, false,
            CIMPropertyList ());
    }
    catch (const Exception & exception)
    {
        PEG_TRACE_STRING (TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Exception caught trying to get Handler instance (" +
            handlerRef.toString () + "): " +
            exception.getMessage ());
        PEG_METHOD_EXIT ();
        throw;
    }

    //
    //  Set namespace in path in CIMInstance
    //
    handlerRef.setNameSpace (nameSpaceName);
    handlerInstance.setPath (handlerRef);

    PEG_METHOD_EXIT ();
    return handlerInstance;
}

Boolean SubscriptionRepository::isTransient (
    const CIMNamespaceName & nameSpace,
    const CIMObjectPath & handler) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::isTransient");

    CIMValue persistenceValue;
    Uint16 persistenceType;

    //
    //  Get the handler instance from the repository
    //
    CIMInstance instance;

    try
    {
        instance = _repository->getInstance (nameSpace, handler,
            false, false, false, CIMPropertyList ());
    }
    catch (const Exception &)
    {
        PEG_METHOD_EXIT ();
        throw;
    }

    //
    //  Get Persistence Type
    //
    persistenceValue = instance.getProperty (instance.findProperty
        (_PROPERTY_PERSISTENCETYPE)).getValue ();
    persistenceValue.get (persistenceType);

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

void SubscriptionRepository::getFilterProperties (
    const CIMInstance & subscription,
    String & query,
    CIMNamespaceName & sourceNameSpace,
    String & queryLanguage)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::getFilterProperties");

    CIMValue filterValue;
    CIMObjectPath filterReference;
    CIMInstance filterInstance;
    CIMNamespaceName nameSpaceName;

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    //
    //  Get filter namespace - if not set in Filter reference property value,
    //  namespace is the namespace of the subscription
    //
    nameSpaceName = filterReference.getNameSpace ();
    if (nameSpaceName.isNull ())
    {
        nameSpaceName = subscription.getPath ().getNameSpace ();
    }

    try
    {
        filterInstance = _repository->getInstance (nameSpaceName,
            filterReference);
    }
    catch (const Exception & exception)
    {
        PEG_TRACE_STRING (TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Exception caught trying to get Filter instance (" +
            filterReference.toString () + "): " +
            exception.getMessage ());
        PEG_METHOD_EXIT ();
        throw;
    }

    query = filterInstance.getProperty (filterInstance.findProperty
        (_PROPERTY_QUERY)).getValue ().toString ();

    sourceNameSpace = filterInstance.getProperty (filterInstance.findProperty
        (_PROPERTY_SOURCENAMESPACE)).getValue ().toString ();

    queryLanguage = filterInstance.getProperty
        (filterInstance.findProperty (_PROPERTY_QUERYLANGUAGE)).
        getValue ().toString ();

    PEG_METHOD_EXIT ();
}

void SubscriptionRepository::getFilterProperties (
    const CIMInstance & subscription,
    String & query,
    CIMNamespaceName & sourceNameSpace)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::getFilterProperties");

    CIMValue filterValue;
    CIMObjectPath filterReference;
    CIMInstance filterInstance;
    CIMNamespaceName nameSpaceName;

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    //
    //  Get filter namespace - if not set in Filter reference property value,
    //  namespace is the namespace of the subscription
    //
    nameSpaceName = filterReference.getNameSpace ();
    if (nameSpaceName.isNull ())
    {
        nameSpaceName = subscription.getPath ().getNameSpace ();
    }

    try
    {
        filterInstance = _repository->getInstance (nameSpaceName,
            filterReference);
    }
    catch (const Exception & exception)
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
            "Exception caught in getting filter instance (" +
            filterReference.toString () + "): " +
            exception.getMessage ());
        PEG_METHOD_EXIT ();
        throw;
    }

    query = filterInstance.getProperty (filterInstance.findProperty
        (_PROPERTY_QUERY)).getValue ().toString ();

    sourceNameSpace = filterInstance.getProperty (filterInstance.findProperty
        (_PROPERTY_SOURCENAMESPACE)).getValue ().toString ();

    PEG_METHOD_EXIT ();
}

void SubscriptionRepository::getFilterProperties (
    const CIMInstance & subscription,
    String & query)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::getFilterProperties");

    CIMValue filterValue;
    CIMObjectPath filterReference;
    CIMInstance filterInstance;
    CIMNamespaceName nameSpaceName;

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    //
    //  Get filter namespace - if not set in Filter reference property value,
    //  namespace is the namespace of the subscription
    //
    nameSpaceName = filterReference.getNameSpace ();
    if (nameSpaceName.isNull ())
    {
        nameSpaceName = subscription.getPath ().getNameSpace ();
    }

    try
    {
        filterInstance = _repository->getInstance (nameSpaceName,
            filterReference);
    }
    catch (const Exception & exception)
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
            "Exception caught in getting filter instance (" +
            filterReference.toString () + "): " +
            exception.getMessage ());
        PEG_METHOD_EXIT ();
        throw;
    }

    query = filterInstance.getProperty (filterInstance.findProperty
        (_PROPERTY_QUERY)).getValue ().toString ();

    PEG_METHOD_EXIT ();
}

Boolean SubscriptionRepository::validateIndicationClassName (
    const CIMName & indicationClassName,
    const CIMNamespaceName & nameSpaceName) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::validateIndicationClassName");

    //
    //  Validate that class is an Indication class
    //  The Indication Qualifier should exist and have the value True
    //
    Boolean validClass = false;
    CIMClass theClass;

    try
    {
        theClass = _repository->getClass (nameSpaceName, indicationClassName,
            false, true, false, CIMPropertyList ());
    }
    catch (const Exception & exception)
    {
        PEG_TRACE_STRING (TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Exception caught trying to get indication class (" +
            indicationClassName.getString () + "): " +
            exception.getMessage ());
        PEG_METHOD_EXIT ();
        throw;
    }

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

    PEG_METHOD_EXIT ();
    return validClass;
}

Array <CIMName> SubscriptionRepository::getIndicationSubclasses (
        const CIMNamespaceName & nameSpace,
        const CIMName & indicationClassName) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::getIndicationSubclasses");

    Array <CIMName> indicationSubclasses;

    try
    {
        indicationSubclasses = _repository->enumerateClassNames
            (nameSpace, indicationClassName, true);
    }
    catch (const Exception &)
    {
        PEG_METHOD_EXIT ();
        throw;
    }

    indicationSubclasses.append (indicationClassName);

    PEG_METHOD_EXIT ();
    return indicationSubclasses;
}

Boolean SubscriptionRepository::reconcileFatalError (
    const CIMInstance subscription)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::reconcileFatalError");

    Boolean removeOrDisable = false;

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
        removeOrDisable = true;
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
        removeOrDisable = true;
    }

    PEG_METHOD_EXIT ();
    return removeOrDisable;
}

CIMClass SubscriptionRepository::getClass (
    const CIMNamespaceName & nameSpaceName,
    const CIMName & className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList & propertyList) const
{
    try
    {
        return _repository->getClass (nameSpaceName, className, localOnly,
            includeQualifiers, includeClassOrigin, propertyList);
    }
    catch (const Exception & exception)
    {
        PEG_TRACE_STRING (TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Exception caught trying to get class (" +
            className.getString () + "): " +
            exception.getMessage ());
        throw;
    }
}

CIMInstance SubscriptionRepository::getInstance (
    const CIMNamespaceName & nameSpace,
    const CIMObjectPath & instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    return _repository->getInstance (nameSpace, instanceName, localOnly,
        includeQualifiers, includeClassOrigin, propertyList);
}

void SubscriptionRepository::modifyInstance (
    const CIMNamespaceName & nameSpace,
    const CIMInstance & modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList & propertyList)
{
    _repository->modifyInstance (nameSpace, modifiedInstance,
        includeQualifiers, propertyList);
}

void SubscriptionRepository::deleteInstance (
    const CIMNamespaceName & nameSpace,
    const CIMObjectPath & instanceName)
{
    _repository->deleteInstance (nameSpace, instanceName);
}

Array <CIMInstance> SubscriptionRepository::enumerateInstancesForClass (
    const CIMNamespaceName & nameSpace,
    const CIMName & className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    Boolean includeInheritance,
    const CIMPropertyList & propertyList)
{
    return _repository->enumerateInstancesForClass (nameSpace, className,
        deepInheritance, localOnly, includeQualifiers, includeClassOrigin,
        false, propertyList);
}

Array <CIMObjectPath> SubscriptionRepository::enumerateInstanceNamesForClass (
    const CIMNamespaceName & nameSpace,
    const CIMName & className,
    Boolean includeInheritance)
{
    return _repository->enumerateInstanceNamesForClass (nameSpace, className,
        false);
}

void SubscriptionRepository::_disableSubscription (
    CIMInstance subscription)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::_disableSubscription");

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
    try
    {
        _repository->modifyInstance
            (subscription.getPath ().getNameSpace (),
            subscription, false, propertyList);
    }
    catch (Exception & exception)
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
           "Exception caught in attempting to disable a subscription: " +
            exception.getMessage ());
    }

    PEG_METHOD_EXIT ();
}

void SubscriptionRepository::_deleteSubscription (
    const CIMInstance subscription)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionRepository::_deleteSubscription");

    //
    //  Delete subscription instance from repository
    //
    try
    {
        _repository->deleteInstance
            (subscription.getPath ().getNameSpace (),
            subscription.getPath ());
    }
    catch (Exception & exception)
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
           "Exception caught in attempting to delete a subscription: " +
            exception.getMessage ());
    }

    PEG_METHOD_EXIT ();
}

PEGASUS_NAMESPACE_END
