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
// Modified By:  
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3603
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>

#include "IndicationConstants.h"
#include "IndicationService.h"
#include "SubscriptionTable.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

SubscriptionTable::SubscriptionTable (
    SubscriptionRepository * subscriptionRepository)
    : _subscriptionRepository (subscriptionRepository)
{
}

SubscriptionTable::~SubscriptionTable ()
{
}

Boolean SubscriptionTable::getSubscriptionEntry (
    const CIMObjectPath & subscriptionPath,
    ActiveSubscriptionsTableEntry & tableValue) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::getSubscriptionEntry");

    Boolean succeeded = false;
    CIMObjectPath activeSubscriptionsKey = _generateActiveSubscriptionsKey
        (subscriptionPath);
    if (_lockedLookupActiveSubscriptionsEntry
        (activeSubscriptionsKey, tableValue))
    {
        succeeded = true;
    }
    else
    {
        //
        //  Subscription not found in Active Subscriptions table
        //
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
            "Subscription (" + activeSubscriptionsKey.toString () +
            ") not found in ActiveSubscriptionsTable");
    }

    PEG_METHOD_EXIT ();
    return succeeded;
}

Array <CIMInstance> SubscriptionTable::getMatchingSubscriptions (
    const CIMName & supportedClass,
    const Array <CIMNamespaceName> nameSpaces,
    const Boolean checkProvider,
    const CIMInstance & provider) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::getMatchingSubscriptions");

    Array <CIMInstance> matchingSubscriptions;
    Array <CIMInstance> subscriptions;

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
                    CIMObjectPath activeSubscriptionsKey =
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
                        if ((providerInList (provider, tableValue)) ==
                            PEG_NOT_FOUND)
                        {
                            match = false;
                            break;
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

Array <CIMInstance> SubscriptionTable::getAndUpdateProviderSubscriptions (
    const CIMInstance & provider)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::getAndUpdateProviderSubscriptions");

    Array <CIMInstance> providerSubscriptions;

    //
    //  Iterate through the subscription table to find subscriptions served by
    //  the provider
    //  NOTE: updating entries (remove and insert) while iterating through the
    //  table does not work reliably, and it is not clear if that is supposed to
    //  work; for now, the SubscriptionTable first iterates through the
    //  active subscriptions table to find subscriptions served by the
    //  provider, then looks up and updates each affected subscription
    //
    {
        //
        //  Acquire and hold the write lock during the entire
        //  lookup/remove/insert process, allowing competing threads to apply
        //  their logic over a consistent view of the data.
        //  Do not call any other methods that need 
        //  _activeSubscriptionsTableLock.
        //
        WriteLock lock (_activeSubscriptionsTableLock);

        for (ActiveSubscriptionsTable::Iterator i =
            _activeSubscriptionsTable.start (); i; i++)
        {
            //
            //  If provider matches, append subscription to the list
            //
            ActiveSubscriptionsTableEntry tableValue = i.value ();
            for (Uint32 j = 0; j < tableValue.providers.size (); j++)
            {
                if (tableValue.providers [j].provider.getPath ().identical
                    (provider.getPath ()))
                {
                    //
                    //  Add the subscription to the list
                    //
                    providerSubscriptions.append (tableValue.subscription);
                    break;
                }
            }
        }

        //
        //  Look up and update hash table entry for each affected subscription
        //
        for (Uint32 k = 0; k < providerSubscriptions.size (); k++)
        {
            //
            //  Update the entry in the active subscriptions hash table
            //
            CIMObjectPath activeSubscriptionsKey =
                _generateActiveSubscriptionsKey
                    (providerSubscriptions [k].getPath ());
            ActiveSubscriptionsTableEntry tableValue;
            if (_activeSubscriptionsTable.lookup (activeSubscriptionsKey,
                tableValue))
            {
                //
                //  Remove the provider from the list of providers serving the
                //  subscription
                //
                Uint32 providerIndex = providerInList (provider, tableValue);
                if (providerIndex != PEG_NOT_FOUND)
                {
                    tableValue.providers.remove (providerIndex);
                    if (tableValue.providers.size () > 0)
                    {
                        //
                        //  At least one provider is still serving the
                        //  subscription
                        //  Update entry in Active Subscriptions table
                        //
                        _removeActiveSubscriptionsEntry
                            (activeSubscriptionsKey);
                        _insertActiveSubscriptionsEntry
                            (tableValue.subscription, tableValue.providers);
                    }
                    else
                    {
                        //
                        //  If the terminated provider was the only provider
                        //  serving the subscription, implement the
                        //  subscription's On Fatal Error Policy
                        //
                        Boolean removedOrDisabled =
                            _subscriptionRepository->reconcileFatalError
                                (tableValue.subscription);
                        _removeActiveSubscriptionsEntry
                            (activeSubscriptionsKey);
                        if (!removedOrDisabled)
                        {
                            //
                            //  If subscription was not disabled or deleted
                            //  Update entry in Active Subscriptions table
                            //
                            _insertActiveSubscriptionsEntry
                                (tableValue.subscription, tableValue.providers);
                        }
                    }
                }
                else
                {
                    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                        Tracer::LEVEL2,
                        "Provider (" + provider.getPath().toString() +
                        ") not found in list for Subscription (" +
                        activeSubscriptionsKey.toString () +
                        ") in ActiveSubscriptionsTable");
                }
            }
            else
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, 
                    Tracer::LEVEL2,
                    "Subscription (" + activeSubscriptionsKey.toString () +
                    ") not found in ActiveSubscriptionsTable");
                //
                //  The subscription may have been deleted in the mean time
                //  If so, no further update is required
                //
            }
        }
    }

    PEG_METHOD_EXIT ();
    return providerSubscriptions;
}

CIMObjectPath SubscriptionTable::_generateActiveSubscriptionsKey (
    const CIMObjectPath & subscription) const
{
    //
    //  Get filter and handler object paths from subscription Filter and Handler
    //  reference property values
    //
    Array<CIMKeyBinding> subscriptionKB = subscription.getKeyBindings ();
    CIMObjectPath filterPath;
    CIMObjectPath handlerPath;
    for (Uint32 i = 0; i < subscriptionKB.size (); i++)
    {
        if ((subscriptionKB [i].getName () == _PROPERTY_FILTER) &&
            (subscriptionKB [i].getType () == CIMKeyBinding::REFERENCE))
        {
            filterPath = subscriptionKB [i].getValue ();
        }
        if ((subscriptionKB [i].getName () == _PROPERTY_HANDLER) &&
            (subscriptionKB [i].getType () == CIMKeyBinding::REFERENCE))
        {
            handlerPath = subscriptionKB [i].getValue ();
        }
    }

    //
    //  Construct subscription object name for key
    //
    filterPath.setHost (String::EMPTY);
    handlerPath.setHost (String::EMPTY);
    Array <CIMKeyBinding> kb;
    kb.append (CIMKeyBinding (_PROPERTY_FILTER, CIMValue (filterPath)));
    kb.append (CIMKeyBinding (_PROPERTY_HANDLER, CIMValue (handlerPath)));
    CIMObjectPath activeSubscriptionsKey ("", subscription.getNameSpace (),
        subscription.getClassName (), kb);

    return activeSubscriptionsKey;
}

Boolean SubscriptionTable::_lockedLookupActiveSubscriptionsEntry (
    const CIMObjectPath & key,
    ActiveSubscriptionsTableEntry & tableEntry) const
{
    ReadLock lock(_activeSubscriptionsTableLock);

    return (_activeSubscriptionsTable.lookup (key, tableEntry));
}

void SubscriptionTable::_insertActiveSubscriptionsEntry (
    const CIMInstance & subscription,
    const Array <ProviderClassList> & providers)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::_insertActiveSubscriptionsEntry");

    CIMObjectPath activeSubscriptionsKey = _generateActiveSubscriptionsKey
        (subscription.getPath ());
    ActiveSubscriptionsTableEntry entry;
    entry.subscription = subscription;
    entry.providers = providers;

    //
    //  Insert returns true on success, false if duplicate key
    //
    Boolean succeeded = _activeSubscriptionsTable.insert
        (activeSubscriptionsKey, entry);
    PEGASUS_ASSERT (succeeded);

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

void SubscriptionTable::_removeActiveSubscriptionsEntry (
    const CIMObjectPath & key)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::_removeActiveSubscriptionsEntry");

    //
    //  Remove returns true on success, false if not found
    //
    Boolean succeeded = _activeSubscriptionsTable.remove (key);
    PEGASUS_ASSERT (succeeded);

#ifdef PEGASUS_INDICATION_HASHTRACE
    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                      Tracer::LEVEL3,
                      "REMOVED _activeSubscriptionsTable entry: " +
                      key.toString ());
#endif

    PEG_METHOD_EXIT ();
}

String SubscriptionTable::_generateSubscriptionClassesKey (
    const CIMName & indicationClassName,
    const CIMNamespaceName & sourceNamespaceName) const
{
    String subscriptionClassesKey;

    //
    //  Append indication class name to key
    //
    subscriptionClassesKey.append (indicationClassName.getString ());

    //
    //  Append source namespace name to key
    //
    subscriptionClassesKey.append (sourceNamespaceName.getString ());

    return subscriptionClassesKey;
}

Boolean SubscriptionTable::_lockedLookupSubscriptionClassesEntry (
    const String & key,
    SubscriptionClassesTableEntry & tableEntry) const
{
    ReadLock lock(_subscriptionClassesTableLock);

    return (_subscriptionClassesTable.lookup (key, tableEntry));
}

void SubscriptionTable::_insertSubscriptionClassesEntry (
    const CIMName & indicationClassName,
    const CIMNamespaceName & sourceNamespaceName,
    const Array <CIMInstance> & subscriptions)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::_insertSubscriptionClassesEntry");

    String subscriptionClassesKey = _generateSubscriptionClassesKey
        (indicationClassName, sourceNamespaceName);
    SubscriptionClassesTableEntry entry;
    entry.indicationClassName = indicationClassName;
    entry.sourceNamespaceName = sourceNamespaceName;
    entry.subscriptions = subscriptions;

    //
    //  Insert returns true on success, false if duplicate key
    //
    Boolean succeeded = _subscriptionClassesTable.insert
        (subscriptionClassesKey, entry);
    PEGASUS_ASSERT (succeeded);

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

void SubscriptionTable::_removeSubscriptionClassesEntry (
    const String & key)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::_removeSubscriptionClassesEntry");

    //
    //  Remove returns true on success, false if not found
    //
    Boolean succeeded = _subscriptionClassesTable.remove (key);
    PEGASUS_ASSERT (succeeded);

#ifdef PEGASUS_INDICATION_HASHTRACE
    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3,
        "REMOVED _subscriptionClassesTable entry: " + key);
#endif

    PEG_METHOD_EXIT ();
}

void SubscriptionTable::insertSubscription (
    const CIMInstance & subscription,
    const Array <ProviderClassList> & providers,
    const Array <CIMName> & indicationSubclassNames,
    const CIMNamespaceName & sourceNamespaceName)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::insertSubscription");

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
    {
        //
        //  Acquire and hold the write lock during the entire
        //  lookup/remove/insert process, allowing competing threads to apply
        //  their logic over a consistent view of the data.
        //  Do not call any other methods that need 
        //  _subscriptionClassesTableLock.
        //
        WriteLock lock (_subscriptionClassesTableLock);
        for (Uint32 i = 0; i < indicationSubclassNames.size (); i++)
        {
            String subscriptionClassesKey = _generateSubscriptionClassesKey
                (indicationSubclassNames [i], sourceNamespaceName);
            SubscriptionClassesTableEntry tableValue;
            if (_subscriptionClassesTable.lookup (subscriptionClassesKey,
                tableValue))
            {
                //
                //  If entry exists for this IndicationClassName-SourceNamespace
                //  pair, remove old entry and insert new entry
                //
                Array <CIMInstance> subscriptions = tableValue.subscriptions;
                subscriptions.append (subscription);
                _removeSubscriptionClassesEntry (subscriptionClassesKey);
                _insertSubscriptionClassesEntry (indicationSubclassNames [i],
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
                _insertSubscriptionClassesEntry (indicationSubclassNames [i],
                    sourceNamespaceName, subscriptions);
            }
        }
    }

    PEG_METHOD_EXIT ();
}

void SubscriptionTable::updateProviders (
    const CIMObjectPath & subscriptionPath,
    const ProviderClassList & provider,
    Boolean addProvider)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::updateProviders");

    CIMObjectPath activeSubscriptionsKey = _generateActiveSubscriptionsKey
        (subscriptionPath);
    ActiveSubscriptionsTableEntry tableValue;
    {
        //
        //  Acquire and hold the write lock during the entire
        //  lookup/remove/insert process, allowing competing threads to apply
        //  their logic over a consistent view of the data.
        //  Do not call any other methods that need 
        //  _activeSubscriptionsTableLock.
        //
        WriteLock lock (_activeSubscriptionsTableLock);
        if (_activeSubscriptionsTable.lookup (activeSubscriptionsKey,
            tableValue))
        {
            Uint32 providerIndex = providerInList (provider.provider, 
                tableValue);
            if (addProvider)
            {
                if (providerIndex == PEG_NOT_FOUND)
                {
                    tableValue.providers.append (provider);
                    _removeActiveSubscriptionsEntry (activeSubscriptionsKey);
                    _insertActiveSubscriptionsEntry (tableValue.subscription,
                        tableValue.providers);
                }
                else
                {
                    CIMInstance p = provider.provider;
                    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                        Tracer::LEVEL2,
                        "Provider " + 
                        IndicationService::getProviderLogString (p) +
                        " already in list for Subscription (" +
                        activeSubscriptionsKey.toString () +
                        ") in ActiveSubscriptionsTable");
                }
            }
            else
            {
                if (providerIndex != PEG_NOT_FOUND)
                {
                    tableValue.providers.remove (providerIndex);
                    _removeActiveSubscriptionsEntry (activeSubscriptionsKey);
                    _insertActiveSubscriptionsEntry (tableValue.subscription,
                        tableValue.providers);
                }
                else
                {
                    CIMInstance p = provider.provider;
                    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL,
                        Tracer::LEVEL2,
                        "Provider " + 
                        IndicationService::getProviderLogString (p) +
                        " not found in list for Subscription (" +
                        activeSubscriptionsKey.toString () +
                        ") in ActiveSubscriptionsTable");
                }
            }
        }
        else
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
                "Subscription (" + activeSubscriptionsKey.toString () +
                ") not found in ActiveSubscriptionsTable");
    
            //
            //  The subscription may have been deleted in the mean time
            //  If so, no further update is required
            //
        }
    }

    PEG_METHOD_EXIT ();
}

void SubscriptionTable::updateClasses (
    const CIMObjectPath & subscriptionPath,
    const CIMInstance & provider,
    const CIMName & className)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::updateClasses");

    CIMObjectPath activeSubscriptionsKey = _generateActiveSubscriptionsKey
        (subscriptionPath);
    ActiveSubscriptionsTableEntry tableValue;

    {
        //
        //  Acquire and hold the write lock during the entire
        //  lookup/remove/insert process, allowing competing threads to apply
        //  their logic over a consistent view of the data.
        //  Do not call any other methods that need 
        //  _activeSubscriptionsTableLock.
        //
        WriteLock lock (_activeSubscriptionsTableLock);
        if (_activeSubscriptionsTable.lookup (activeSubscriptionsKey, 
            tableValue))
        {
            Uint32 providerIndex = providerInList (provider, tableValue);
            if (providerIndex != PEG_NOT_FOUND)
            {
                Uint32 classIndex = classInList (className,
                    tableValue.providers [providerIndex]);
                if (classIndex == PEG_NOT_FOUND)
                {
                    tableValue.providers [providerIndex].classList.append
                        (className);
                }
                else //  classIndex != PEG_NOT_FOUND
                {
                    tableValue.providers [providerIndex].classList.remove
                        (classIndex);
                }

                _removeActiveSubscriptionsEntry (activeSubscriptionsKey);
                _insertActiveSubscriptionsEntry (tableValue.subscription,
                    tableValue.providers);
            }
            else
            {
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, 
                    Tracer::LEVEL2,
                    "Provider (" + provider.getPath ().toString () +
                    ") not found in list for Subscription (" +
                    activeSubscriptionsKey.toString () +
                    ") in ActiveSubscriptionsTable");
            }
        }
        else
        {
            //
            //  Subscription not found in Active Subscriptions table
            //
        }
    }

    PEG_METHOD_EXIT ();
}

void SubscriptionTable::removeSubscription (
    const CIMInstance & subscription,
    const Array <CIMName> & indicationSubclassNames,
    const CIMNamespaceName & sourceNamespaceName,
    const Array <ProviderClassList> & providers)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::removeSubscription");

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
    {
        //
        //  Acquire and hold the write lock during the entire
        //  lookup/remove/insert process, allowing competing threads to apply
        //  their logic over a consistent view of the data.
        //  Do not call any other methods that need 
        //  _subscriptionClassesTableLock.
        //
        WriteLock lock (_subscriptionClassesTableLock);
        for (Uint32 i = 0; i < indicationSubclassNames.size (); i++)
        {
            String subscriptionClassesKey = _generateSubscriptionClassesKey
                (indicationSubclassNames [i], sourceNamespaceName);
            SubscriptionClassesTableEntry tableValue;
            if (_subscriptionClassesTable.lookup (subscriptionClassesKey,
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
                _removeSubscriptionClassesEntry (subscriptionClassesKey);
    
                //
                //  If there are still subscriptions in the list, insert the
                //  new entry
                //
                if (subscriptions.size () > 0)
                {
                    _insertSubscriptionClassesEntry (
                        indicationSubclassNames [i],
                        sourceNamespaceName, subscriptions);
                }
            }
            else
            {
                //
                //  Entry not found in Subscription Classes table
                //
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, 
                    Tracer::LEVEL2,
                    "Indication subclass and namespace (" + 
                    subscriptionClassesKey +
                    ") not found in SubscriptionClassesTable");
            }
        }
    }

    PEG_METHOD_EXIT ();
}

Uint32 SubscriptionTable::providerInList
    (const CIMInstance & provider,
     const ActiveSubscriptionsTableEntry & tableValue) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::providerInList");

    //
    //  Look for the provider in the list
    //
    for (Uint32 i = 0; i < tableValue.providers.size (); i++)
    {
        if (tableValue.providers [i].provider.getPath ().identical
            (provider.getPath ()))
        {
            PEG_METHOD_EXIT ();
            return i;
        }
    }

    PEG_METHOD_EXIT ();
    return PEG_NOT_FOUND;
}


Uint32 SubscriptionTable::classInList
    (const CIMName & className,
     const ProviderClassList & providerClasses) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "SubscriptionTable::classInList");

    //
    //  Look for the class in the list
    //
    for (Uint32 i = 0; i < providerClasses.classList.size (); i++)
    {
        if (providerClasses.classList [i].equal (className))
        {
            PEG_METHOD_EXIT ();
            return i;
        }
    }

    PEG_METHOD_EXIT ();
    return PEG_NOT_FOUND;
}

void SubscriptionTable::clear ()
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "SubscriptionTable::clear");

    {
        WriteLock lock (_activeSubscriptionsTableLock);
        _activeSubscriptionsTable.clear ();
    }
    {
        WriteLock lock (_subscriptionClassesTableLock);
        _subscriptionClassesTable.clear ();
    }

    PEG_METHOD_EXIT ();
}

PEGASUS_NAMESPACE_END
