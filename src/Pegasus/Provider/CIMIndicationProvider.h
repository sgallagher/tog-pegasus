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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMIndicationProvider_h
#define Pegasus_CIMIndicationProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>

#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMIndication.h>
#include <Pegasus/Provider/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
    This class defines the set of methods implemented by an indication 
    provider.  A provider that derives from this class must implement all 
    methods.  The minimal method implementation simply throws the NotSupported 
    exception.  
*/
class PEGASUS_PROVIDER_LINKAGE CIMIndicationProvider : public virtual CIMProvider
{
public:
    /** Constructs a CIMIndicationProvider instance with 
        null values (default constructor).
    */
    CIMIndicationProvider(void);
    /** Destroys the CIMIndicationProvider.
    */
    virtual ~CIMIndicationProvider(void);

    /** Called when the provider is expected to begin generating
        indications.  The handler argument is what the provider uses to deliver
        the indications it generates.
    
        One or more createSubscription calls may precede the call to this method.
        (On start-up, all active subscriptions are given to the provider with
        createSubscription calls before the enableIndications method is called, so
        that the provider knows about all outstanding subscriptions before it
        starts generating indications.)
        @param IndicationResponseHandler 
        @param handler

    */

    virtual void enableIndications(IndicationResponseHandler & handler) = 0;

    /** Tells the provider not to generate any more indications.  After this call,
        the handler given to the provider in the enableIndications method is no
        longer valid, and calling it has undefined results.
    */
    virtual void disableIndications(void) = 0;

    /** Tells the provider to monitor for indications matching the specified
        subscription.  Note that it is not necessary for a simple indication
        provider to take any action on createSubscription, modifySubscription, or
        deleteSubscription.  It may simply generate indications (or not) based on
        the enableIndications and disableIndications calls.
        
        @param context Similar to that in the other provider interfaces.
        Of particular interest to indication providers are the
        SubscriptionInstanceContainer and SubscriptionFilterConditionContainer
        containers.  The SubscriptionInstanceContainer contains the full
        CIM_IndicationSubscription instance (including, for example, more
        information about repeat notification policies).  The
        SubscriptionFilterConditionContainer contains the query string and query
        language from the CIM_IndicationFilter instance.
        
        @param subscriptionName Specifies the object path of the CIM_IndicationSubscription
        instance that implicates this provider.
        
        @param classNames Contains a list of indication class names for
        which this provider has registered as an indication provider that are
        included in the CIM_IndicationFilter for this subscription instance.
        A provider can use this as a medium-level data for controlling which
        indications it generates.
        
        @param propertyList Specifies which properties of the specified class names
        must be included in the indications generated for this subscription.
        
        @param repeatNotificationPolicy Contains the value of the property with the
        same name in the CIM_IndicationSubscription instance.  Most providers
        will probably not support repeat notification policies, and should throw
        a NotSupported exception if the value is anything other than "none".
    */

    virtual void createSubscription(
        const OperationContext & context,
        const CIMObjectPath & subscriptionName,
        const Array<CIMObjectPath> & classNames,
        const CIMPropertyList & propertyList,
        const Uint16 repeatNotificationPolicy) = 0;

    /** Informs the provider that the specified subscription instance has changed.
        The arguments are similar to those for createSubscription.
        
        @param context Similar to that in the other provider interfaces.
        Of particular interest to indication providers are the
        SubscriptionInstanceContainer and SubscriptionFilterConditionContainer
        containers.  The SubscriptionInstanceContainer contains the full
        CIM_IndicationSubscription instance (including, for example, more
        information about repeat notification policies).  The
        SubscriptionFilterConditionContainer contains the query string and query
        language from the CIM_IndicationFilter instance.
        
        @param subscriptionName Specifies the object path of the CIM_IndicationSubscription
        instance that implicates this provider.
        
        @param classNames Contains a list of indication class names for
        which this provider has registered as an indication provider that are
        included in the CIM_IndicationFilter for this subscription instance.
        A provider can use this as a medium-level data for controlling which
        indications it generates.
        
        @param propertyList Specifies which properties of the specified class names
        must be included in the indications generated for this subscription.
        
        @param repeatNotificationPolicy Contains the value of the property with the
        same name in the CIM_IndicationSubscription instance.  Most providers
        will probably not support repeat notification policies, and should throw
        a NotSupported exception if the value is anything other than "none".
    */
    virtual void modifySubscription(
        const OperationContext & context,
        const CIMObjectPath & subscriptionName,
        const Array<CIMObjectPath> & classNames,
        const CIMPropertyList & propertyList,
        const Uint16 repeatNotificationPolicy) = 0;

    /** Informs the provider to stop monitoring for indications matching the
        specified subscription.
        
        @param context Similar to that in the other provider interfaces.
        Of particular interest to indication providers are the
        SubscriptionInstanceContainer and SubscriptionFilterConditionContainer
        containers.  The SubscriptionInstanceContainer contains the full
        CIM_IndicationSubscription instance (including, for example, more
        information about repeat notification policies).  The
        SubscriptionFilterConditionContainer contains the query string and query
        language from the CIM_IndicationFilter instance.
        
        @param subscriptionName Specifies the object path of the CIM_IndicationSubscription
        instance that implicates this provider.
        
        @param classNames Contains a list of indication class names for
        which this provider has registered as an indication provider that are
        included in the CIM_IndicationFilter for this subscription instance.
        A provider can use this as a medium-level data for controlling which
        indications it generates. This parameter contains extra data provided to assist
        those providers doing class-level control of indication generation.
    */
    virtual void deleteSubscription(
        const OperationContext & context,
        const CIMObjectPath & subscriptionName,
        const Array<CIMObjectPath> & classNames) = 0;
};

PEGASUS_NAMESPACE_END

#endif
