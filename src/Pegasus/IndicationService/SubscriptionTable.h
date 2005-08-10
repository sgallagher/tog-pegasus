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
// Modified By: Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3603
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SubscriptionTable_h
#define Pegasus_SubscriptionTable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/IPC.h>

#include "ProviderClassList.h"
#include "SubscriptionRepository.h"

PEGASUS_NAMESPACE_BEGIN

class IndicationService;

/**
    Entry for ActiveSubscriptions table
 */
struct ActiveSubscriptionsTableEntry
{
    CIMInstance subscription;
    Array <ProviderClassList> providers;
};

/**
    Table for active subscription information

    The ActiveSubscriptions table is used to keep track of active subscriptions.
    This table contains an entry for each subscription that is enabled
    (value of SubscriptionState property is enabled).
    Each entry consists of a Subscription instance, and a list of
    ProviderClassList structs representing the providers, if any, currently
    serving each subscription and the relevant indication subclasses served by
    each provider.
    The Key is the object path of the subscription instance.
    Entries are inserted into the table on initialization, when an enabled
    subscription instance is created, or when a subscription instance is
    modified to be enabled.
    Entries are removed from the table when an enabled subscription instance is
    deleted, or when a subscription instance is modified to be disabled.
    Entries are updated (remove followed by insert) when a provider serving a
    subscription is disabled or enabled, or a provider registration change
    occurs.
    The _handleProcessIndicationRequest() function, when a list of
    subscriptions is included in request, looks up each subscription in the
    table.
    The _handleNotifyProviderRegistrationRequest() function, once matching
    subscriptions have been identified, looks up the provider information for
    each matching subscription in the table.
    The _handleModifyInstanceRequest() and _handleDeleteInstanceRequest()
    functions, when sending delete requests to providers, look up the providers
    for the subscription in the table.
    The _handleNotifyProviderTerminationRequest() function, when a provider is
    disabled, iterates through the table to retrieve all active
    subscriptions being served by the provider.
    The terminate() function, when the CIM Server is being shut down, iterates
    through the table to retrieve all active subscriptions.
 */
typedef HashTable <CIMObjectPath,
                   ActiveSubscriptionsTableEntry,
                   EqualFunc <CIMObjectPath>,
                   HashFunc <CIMObjectPath> > ActiveSubscriptionsTable;

/**
    Entry for SubscriptionClasses table
 */
struct SubscriptionClassesTableEntry
{
    CIMName indicationClassName;
    CIMNamespaceName sourceNamespaceName;
    Array <CIMInstance> subscriptions;
};

/**
    Table for subscription classes information

    The SubscriptionClasses Table is used to keep track of active subscriptions
    for each indication subclass-source namespace pair.
    Each entry consists of an indication subclass name, a source namespace name,
    and a list of subscription instances.
    The Key is generated by concatenating the indication subclass name, and the
    source namespace name.
    The _handleProcessIndicationRequest() function, when no list of
    subscriptions is included in the request, looks up matching subscriptions
    in the table, using the class name and namespace name of the indication
    instance.
    The _handleNotifyProviderRegistrationRequest() function, when an indication
    provider registration instance has been created or deleted, looks up
    matching subscriptions in the table, using the class name and namespace
    names from the provider registration instance.
 */
typedef HashTable <String,
                   SubscriptionClassesTableEntry,
                   EqualNoCaseFunc,
                   HashLowerCaseFunc> SubscriptionClassesTable;

/**

    The SubscriptionTable class manages an in-memory cache of indication
    subscription information in two hash tables.  The Active Subscriptions
    table includes all enabled subscriptions along with the providers currently
    serving them.  The Subscription Classes table includes the enabled
    subscription instances for each indication class/namespace pair, based on
    the filter query and sourceNamespace.

    @author  Hewlett-Packard Company

 */

class PEGASUS_SERVER_LINKAGE SubscriptionTable
{
public:

    /**
        Constructs a SubscriptionTable instance.
     */
    SubscriptionTable (
        SubscriptionRepository * subscriptionRepository);

    /**
        Destructs a SubscriptionTable instance.
     */
    ~SubscriptionTable ();

    /**
        Inserts entries (or updates existing entries) for the specified
        subscription in the Active Subscriptions and Subscription Classes
        tables.

        @param   subscription            the subscription instance
        @param   providers               the list of providers
        @param   indicationSubclassNames the list of indication subclass names
        @param   sourceNamespaceName     the source namespace name
     */
    void insertSubscription (
        const CIMInstance & subscription,
        const Array <ProviderClassList> & providers,
        const Array <CIMName> & indicationSubclassNames,
        const CIMNamespaceName & sourceNamespaceName);

    /**
        Updates an entry in the Active Subscriptions table to either add a
        provider to or remove a provider from the list of providers serving the
        subscription.

        @param   subscriptionPath        the subscription object path
        @param   provider                the provider to be added or removed
        @param   addProvider             indicates if adding or removing
                                         provider
     */
    void updateProviders (
        const CIMObjectPath & subscriptionPath,
        const ProviderClassList & provider,
        Boolean addProvider);

    /**
        Updates an entry in the Active Subscriptions table to either add a
        class to or remove a class from the list of indication subclasses served
        by a provider serving the subscription.

        @param   subscriptionPath        the subscription object path
        @param   provider                the provider
        @param   className               the class to be added or removed
     */
    void updateClasses (
        const CIMObjectPath & subscriptionPath,
        const CIMInstance & provider,
        const CIMName & className);

    /**
        Removes entries (or updates entries) for the specified subscription in
        the Active Subscriptions and Subscription Classes tables.

        @param   subscription            the subscription instance
        @param   indicationSubclassNames the list of indication subclass names
        @param   sourceNamespaceName     the source namespace name
        @param   providers               the list of providers that had been
                                         serving the subscription
     */
    void removeSubscription (
        const CIMInstance & subscription,
        const Array <CIMName> & indicationSubclassNames,
        const CIMNamespaceName & sourceNamespaceName,
        const Array <ProviderClassList> & providers);

    /**
        Retrieves the Active Subscriptions table entry for the specified
        subscription.  If this function returns False, the value of tableValue
        is not changed.

        @param   subscriptionPath      the object path of the subscription
        @param   tableValue            the retrieved table entry

        @return   True, if the specified subscription table entry was found;
                  False otherwise
     */
    Boolean getSubscriptionEntry (
        const CIMObjectPath & subscriptionPath,
        ActiveSubscriptionsTableEntry & tableValue) const;

    /**
        Retrieves list of enabled subscription instances in the specified
        namespaces, where the subscription indication class matches or is a
        superclass of the supported class.  If the checkProvider parameter
        value is True, a subscription is only included in the list returned if
        the specified provider accepted the subscription.  If the checkProvider
        parameter value is False, the provider parameter is not used (ignored).

        @param   supportedClass       the supported class
        @param   nameSpaces           the list of supported namespaces
        @param   checkProvider        indicates whether provider acceptance is
                                          checked
        @param   provider             the provider (used if checkProvider True)

        @return   list of CIMInstance subscriptions
     */
    Array <CIMInstance> getMatchingSubscriptions (
        const CIMName & supportedClass,
        const Array <CIMNamespaceName> nameSpaces,
        const Boolean checkProvider = false,
        const CIMInstance & provider = CIMInstance ()) const;

    /**
        Retrieves list of enabled subscription instances in all namespaces,
        that are served by the specified provider.  This function is called
        when a provider is disabled.  In the Active Subscriptions table, the
        specified provider is removed from the list of providers serving the
        subscription.

        Note: this method may call the SubscriptionRepository
        reconcileFatalError() method, which may call the CIMRepository
        modifyInstance() or deleteInstance() method, while a WriteLock is held
        on the Active Subscriptions table.  The determination of whether the
        SubscriptionRepository reconcileFatalError() method must be called (and
        whether the repository must be updated) requires a lookup of the Active
        Subscriptions table to see if any other providers are serving the
        subscription.  The SubscriptionRepository reconcileFatalError() method
        does not need to access the repository to determine the subscription
        policy, but if the policy is Disable or Remove, it does need to call the
        CIMRepository modifyInstance() or deleteInstance() method.  The return
        value from the SubscriptionRepository reconcileFatalError() method (True
        if the subscription was successfully disabled or removed) in turn
        determines whether the entry in the Active Subscriptions table must be
        updated or removed.

        @param   provider          the provider instance

        @return   list of CIMInstance subscriptions
     */
    Array <CIMInstance> getAndUpdateProviderSubscriptions (
        const CIMInstance & provider);

    /**
        Determines if the specified provider is in the list of providers
        serving the subscription.

        @param   provider              the provider instance
        @param   tableValue            the Active Subscriptions Table entry

        @return  The index of the provider in the list, if found;
                 PEG_NOT_FOUND otherwise
    */
    Uint32 providerInList
        (const CIMInstance & provider,
         const ActiveSubscriptionsTableEntry & tableValue) const;

    /**
        Determines if the specified class is in the list of indication
        subclasses served by the specified provider, serving the subscription.

        @param   className             the class name
        @param   providerClasses       a provider serving the subscription,
                                           with the indication classes served

        @return  The index of the class name in the list, if found;
                 PEG_NOT_FOUND otherwise
    */
    Uint32 classInList
        (const CIMName & className,
         const ProviderClassList & providerClasses) const;

    /**
        Removes all entries from the Active Subscriptions and Subscription
        Classes tables.
     */
    void clear ();


private:

    /**
        Generates a unique CIMObjectPath key for the Active Subscriptions table
        from the subscription object path.

        @param   subscription          the subscription object path

        @return  the generated key
     */
    CIMObjectPath _generateActiveSubscriptionsKey (
        const CIMObjectPath & subscription) const;

    /**
        Locks the _activeSubscriptionsTableLock for read access and looks
        up an entry in the Active Subscriptions table.

        @param   key                   the hash table key
        @param   tableEntry            the table entry retrieved

        @return  true if the key is found in the table; false otherwise
     */
    Boolean _lockedLookupActiveSubscriptionsEntry (
        const CIMObjectPath & key,
        ActiveSubscriptionsTableEntry & tableEntry) const;

    /**
        Inserts an entry into the Active Subscriptions table.  The caller
        must first lock the _activeSubscriptionsTableLock for write access.

        @param   subscription          the subscription instance
        @param   providers             the list of providers
     */
    void _insertActiveSubscriptionsEntry (
        const CIMInstance & subscription,
        const Array <ProviderClassList> & providers);

    /**
        Removes an entry from the Active Subscriptions table.  The caller
        must first lock the _activeSubscriptionsTableLock for write access.

        @param   key                   the key of the entry to remove
     */
    void _removeActiveSubscriptionsEntry (
        const CIMObjectPath & key);

    /**
        Generates a unique String key for the Subscription Classes table from
        the indication class name and source namespace name.

        @param   indicationClassName   the indication class name
        @param   sourceNamespaceName   the source namespace name

        @return  the generated key
     */
    String _generateSubscriptionClassesKey (
        const CIMName & indicationClassName,
        const CIMNamespaceName & sourceNamespaceName) const;

    /**
        Locks the _subscriptionClassesTableLock for read access and looks
        up an entry in the Subscription Classes table.

        @param   key                   the hash table key
        @param   tableEntry            the table entry retrieved

        @return  true if the key is found in the table; false otherwise
     */
    Boolean _lockedLookupSubscriptionClassesEntry (
        const String & key,
        SubscriptionClassesTableEntry & tableEntry) const;

    /**
        Inserts an entry into the Subscription Classes table.  The caller must
        first lock the _subscriptionClassesTableLock for write access.

        @param   indicationClassName   the indication class name
        @param   sourceNamespaceName   the source namespace name
        @param   subscriptions         the list of subscription instances
     */
    void _insertSubscriptionClassesEntry (
        const CIMName & indicationClassName,
        const CIMNamespaceName & sourceNamespaceName,
        const Array <CIMInstance> & subscriptions);

    /**
        Removes an entry from the Subscription Classes table.  The caller must
        first lock the _subscriptionClassesTableLock for write access.

        @param   key                   the key of the entry to remove
     */
    void _removeSubscriptionClassesEntry (
        const String & key);

    /**
        Active Subscriptions information table.  Access to this table is
        controlled by the _activeSubscriptionsTableLock.
     */
    ActiveSubscriptionsTable _activeSubscriptionsTable;

    /**
        A lock to control access to the _activeSubscriptionsTable.  Before
        accessing the _activeSubscriptionsTable, one must first lock this for
        read access.  Before updating the _activeSubscriptionsTable, one must
        first lock this for write access.
     */
    mutable ReadWriteSem _activeSubscriptionsTableLock;

    /**
        Subscription Classes information table.  Access to this table is
        controlled by the _subscriptionClassesTableLock.
     */
    SubscriptionClassesTable _subscriptionClassesTable;

    /**
        A lock to control access to the _subscriptionClassesTable.  Before
        accessing the _subscriptionClassesTable, one must first lock this for
        read access.  Before updating the _subscriptionClassesTable, one must
        first lock this for write access.
     */
    mutable ReadWriteSem _subscriptionClassesTableLock;

    SubscriptionRepository * _subscriptionRepository;
};

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_SubscriptionTable_h */
