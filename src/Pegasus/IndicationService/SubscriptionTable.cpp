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

Array <CIMInstance> SubscriptionTable::getActiveSubscriptions () 
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::getActiveSubscriptions");

    Array <CIMInstance> activeSubscriptions;

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

Boolean SubscriptionTable::getSubscriptionEntry (
    const CIMObjectPath & subscriptionPath,
    ActiveSubscriptionsTableEntry & tableValue) 
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::getSubscriptionEntry");

    Boolean succeeded = false;
    String activeSubscriptionsKey = _generateActiveSubscriptionsKey
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
            "Subscription (" + activeSubscriptionsKey +
            ") not found in ActiveSubscriptionsTable");
    }

    PEG_METHOD_EXIT ();
    return succeeded;
}

Array <CIMInstance> SubscriptionTable::getMatchingSubscriptions (
    const CIMName & supportedClass,
    const Array <CIMNamespaceName> nameSpaces,
    const Boolean checkProvider,
    const CIMInstance & provider)
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

Array <CIMInstance> SubscriptionTable::getProviderSubscriptions (
    const CIMInstance & provider)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::getProviderSubscriptions");

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
        // Do not call any other methods that need _activeSubscriptionsTableLock
        //
        ReadLock lock (_activeSubscriptionsTableLock);

        CIMClass providerClass = _subscriptionRepository->getClass
            (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER);
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
    }

    //
    //  Look up and update hash table entry for each affected subscription
    //
    for (Uint32 k = 0; k < providerSubscriptions.size (); k++)
    {
        //
        //  Update the entry in the active subscriptions hash table
        //
        String activeSubscriptionsKey = _generateActiveSubscriptionsKey
            (providerSubscriptions [k].getPath ());
        ActiveSubscriptionsTableEntry tableValue;
        if (_lockedLookupActiveSubscriptionsEntry (activeSubscriptionsKey,
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
                    WriteLock lock (_activeSubscriptionsTableLock);
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
                    if (!_subscriptionRepository->reconcileFatalError 
                        (tableValue.subscription))
                    {
                        //
                        //  If subscription was not disabled or deleted
                        //  Update entry in Active Subscriptions table
                        //
                        WriteLock lock (_activeSubscriptionsTableLock);
                        _removeActiveSubscriptionsEntry 
                            (activeSubscriptionsKey);
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
                    activeSubscriptionsKey +
                    ") in ActiveSubscriptionsTable");
            }
        }
        else
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2, 
                "Subscription (" + activeSubscriptionsKey +
                ") not found in ActiveSubscriptionsTable");
            //
            //  The subscription may have been deleted in the mean time
            //  If so, no further update is required
            //
        }
    }

    PEG_METHOD_EXIT ();
    return providerSubscriptions;
}

Boolean SubscriptionTable::_providerInUse (
    const CIMInstance & provider) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::_providerInUse");

    //
    //  The caller must acquire a lock on the Active Subscriptions table
    //  before calling
    //

    //
    //  Iterate through the subscription table
    //
    CIMClass providerClass = _subscriptionRepository->getClass
        (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER);
    for (ActiveSubscriptionsTable::Iterator i =
        _activeSubscriptionsTable.start (); i; i++)
    {
        //
        //  If provider matches, return true
        //
        for (Uint32 j = 0; j < i.value ().providers.size (); j++)
        {
            ActiveSubscriptionsTableEntry tableValue = i.value ();
            if (tableValue.providers [j].provider.getPath ().identical 
                (provider.getPath ()))
            {
                PEG_METHOD_EXIT ();
                return true;
            }
        }
    }

    PEG_METHOD_EXIT ();
    return false;
}

String SubscriptionTable::_generateActiveSubscriptionsKey (
    const CIMObjectPath & subscription) const
{
    String activeSubscriptionsKey;

    //
    //  Append subscription namespace name to key
    //
    activeSubscriptionsKey.append 
        (subscription.getNameSpace ().getString());

    //
    //  Get filter and handler key bindings from subscription reference
    //
    Array<CIMKeyBinding> subscriptionKB = subscription.getKeyBindings ();
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

    return activeSubscriptionsKey;
}

Boolean SubscriptionTable::_lockedLookupActiveSubscriptionsEntry (
    const String & key,
    ActiveSubscriptionsTableEntry & tableEntry)
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

void SubscriptionTable::_removeActiveSubscriptionsEntry (
    const String & key)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::_removeActiveSubscriptionsEntry");

    _activeSubscriptionsTable.remove (key);
#ifdef PEGASUS_INDICATION_HASHTRACE
    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, 
                      Tracer::LEVEL3, 
                      "REMOVED _activeSubscriptionsTable entry: " + key);
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
    SubscriptionClassesTableEntry & tableEntry)
{
    ReadLock lock(_subscriptionClassesTableLock);

    return (_subscriptionClassesTable.lookup (key, tableEntry));
}

void SubscriptionTable::_lockedInsertSubscriptionClassesEntry (
    const CIMName & indicationClassName,
    const CIMNamespaceName & sourceNamespaceName,
    const Array <CIMInstance> & subscriptions)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::_lockedInsertSubscriptionClassesEntry");

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

void SubscriptionTable::_lockedRemoveSubscriptionClassesEntry (
    const String & key)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::_lockedRemoveSubscriptionClassesEntry");

    WriteLock lock(_subscriptionClassesTableLock);

    _subscriptionClassesTable.remove (key);

#ifdef PEGASUS_INDICATION_HASHTRACE
    PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL3, 
        "REMOVED _subscriptionClassesTable entry: " + key);
#endif

    PEG_METHOD_EXIT ();
}

Array <ProviderClassList> SubscriptionTable::insertSubscription (
    const CIMInstance & subscription,
    const Array <ProviderClassList> & providers,
    const Array <CIMName> & indicationSubclassNames,
    const CIMNamespaceName & sourceNamespaceName)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::insertSubscription");

    Array <ProviderClassList> enableProviders;

    //
    //  Insert entry into active subscriptions table 
    //
    {
        WriteLock lock(_activeSubscriptionsTableLock);

        //
        //  If provider is not yet in the table, add to list of 
        //  providers to enable
        //
        for (Uint32 i = 0; i < providers.size (); i++)
        {
            if (!_providerInUse (providers [i].provider))
            {
                enableProviders.append (providers [i]);
            }
        }
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
    return enableProviders;
}

Array <ProviderClassList> SubscriptionTable::updateProviders (
    const CIMObjectPath & subscriptionPath,
    const ProviderClassList & provider,
    Boolean addProvider)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::updateProviders");

    Array <ProviderClassList> providers;

    String activeSubscriptionsKey = _generateActiveSubscriptionsKey
        (subscriptionPath);
    ActiveSubscriptionsTableEntry tableValue;
    if (_lockedLookupActiveSubscriptionsEntry (activeSubscriptionsKey,
        tableValue))
    {
        Uint32 providerIndex = providerInList (provider.provider, tableValue);
        if (addProvider)
        {
            if (providerIndex == PEG_NOT_FOUND)
            {
                tableValue.providers.append (provider);
            }
            else
            {
                CIMInstance p = provider.provider;
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, 
                    Tracer::LEVEL2, 
                    "Provider " + IndicationService::getProviderLogString (p) +
                    " already in list for Subscription (" +
                    activeSubscriptionsKey +
                    ") in ActiveSubscriptionsTable");
            }
        }
        else
        {
            if (providerIndex != PEG_NOT_FOUND)
            {
                tableValue.providers.remove (providerIndex);
            }
            else
            {
                CIMInstance p = provider.provider;
                PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, 
                    Tracer::LEVEL2, 
                    "Provider " + IndicationService::getProviderLogString (p) +
                    " not found in list for Subscription (" +
                    activeSubscriptionsKey +
                    ") in ActiveSubscriptionsTable");
            }
        }
        {
            WriteLock lock (_activeSubscriptionsTableLock);
            _removeActiveSubscriptionsEntry (activeSubscriptionsKey);
            if (!_providerInUse (provider.provider))
            {
                providers.append (provider);
            }
            _insertActiveSubscriptionsEntry (tableValue.subscription, 
                tableValue.providers);
        }
    }
    else
    {
        PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2, 
            "Subscription (" + activeSubscriptionsKey +
            ") not found in ActiveSubscriptionsTable");

        //
        //  The subscription may have been deleted in the mean time
        //  If so, no further update is required
        //
    }

    PEG_METHOD_EXIT ();
    return providers;
}

void SubscriptionTable::updateClasses (
    const CIMObjectPath & subscriptionPath,
    const CIMInstance & provider,
    const CIMName & className)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::updateClasses");

    String activeSubscriptionsKey = _generateActiveSubscriptionsKey
        (subscriptionPath);
    ActiveSubscriptionsTableEntry tableValue;

    if (getSubscriptionEntry (subscriptionPath, tableValue))
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
        }
        else
        {
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2,
                "Provider (" + provider.getPath ().toString () +
                ") not found in list for Subscription (" +
                activeSubscriptionsKey +
                ") in ActiveSubscriptionsTable");
        }
    }
    else
    {
        //
        //  Subscription not found in Active Subscriptions table
        //
    }

    {
        WriteLock lock (_activeSubscriptionsTableLock);
        _removeActiveSubscriptionsEntry (activeSubscriptionsKey);
        _insertActiveSubscriptionsEntry (tableValue.subscription, 
            tableValue.providers);
    }

    PEG_METHOD_EXIT ();
}

Array <ProviderClassList> SubscriptionTable::removeSubscription (
    const CIMInstance & subscription,
    const Array <CIMName> & indicationSubclassNames,
    const CIMNamespaceName & sourceNamespaceName,
    const Array <ProviderClassList> & providers)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::removeSubscription");

    Array <ProviderClassList> disableProviders;

    //
    //  Remove entry from active subscriptions table 
    //
    {
        WriteLock lock(_activeSubscriptionsTableLock);

        _removeActiveSubscriptionsEntry (
            _generateActiveSubscriptionsKey (subscription.getPath ()));

        for (Uint32 i = 0; i < providers.size (); i++)
        {
            if (!_providerInUse (providers [i].provider))
            {
                disableProviders.append (providers [i]);
            }
        }
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
            PEG_TRACE_STRING (TRC_INDICATION_SERVICE_INTERNAL, Tracer::LEVEL2, 
                "Indication subclass and namespace (" + subscriptionClassesKey +
                ") not found in SubscriptionClassesTable");
        }
    }

    PEG_METHOD_EXIT ();
    return disableProviders;
}

Uint32 SubscriptionTable::providerInList 
    (const CIMInstance & provider, 
     const ActiveSubscriptionsTableEntry & tableValue) const
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "SubscriptionTable::providerInList");

    CIMClass providerClass = _subscriptionRepository->getClass
        (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER);

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
