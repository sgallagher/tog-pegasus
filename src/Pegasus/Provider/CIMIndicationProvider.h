//%//-*-c++-*-///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
#include <Pegasus/Common/CIMIndication.h>
#include <Pegasus/Provider/CIMBaseProvider.h>

#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/CIMDateTime.h>

PEGASUS_NAMESPACE_BEGIN

/*
    REVIEW: In CIMClient interfaces, we fully specify a class name using two
    arguments:

	const String& nameSpace
	const String& className

    In the provideIndication() method below, these two are represented like
    this:

	const CIMReference& classReference

    The goal is to represent these:

	namespace name
	class name.
	
    But using this type leaves open the possibility (ambiguity) of specifying
    these components of CIMReference as well:

	host name
	keybinding pairs

    I think there should be consistency between the CIMClient and
    CIMProvider interfaces? Further, I don't think using CIMReference is an
    option for the client, since it would allow the client to pass host names
    and keybinding pairs (which could be detected until run time). Further,
    it is incomptible with the specification.

    REVIEW: WITHIN clauses not supported by WQL.

    REVIEW: Get clarification on use of handler.complete() method.

    REVIEW: How do handlers get cleaned up?

    REVIEW: What do you do with the old handler (you may have cached) when
	you are passed a new handler by any of the methods. Why not just
	have one handler for the whole class rather than passing a new
	handler each time one of the methods of this class is called?

    REVIEW: Why does cancelIndication() need a handler? Why do any of the
	methods except for provideIndication() need a handler?

    REVIEW: Note that Microsoft's WMI only passes the handler (sink) to the
    provideEvents() method.

    REVIEW: Should we split this class into two distinct interfaces (since
	it would appear that polled v.s. non-polled is a development time
	decision and therefore a compile time decission). It may be cleaner
	to resolve polled v.s. non-polled via provider registration.

    REVIEW: Is an asynchronous indication provider asynchronous with respect
	to all indications it might generate or can it be synchronous for some
	and aysynchronous for others? Does synchronicity pertain to the each
	class or just to the provider?
*/

/**
    This class defines the set of methods implemented by an indication
    provider.

    A provider that derives from this class must implement all methods. The
    minimal method implementation simply throws the NotSupported exception.

    In general, the CIMOM classifies the provider as asynchronous or
    synchronous depending on the methods implemented by the provider. An
    asynchronous indication provider supports these methods:

	<pre>
	provideIndication()
	updateIndication()
	cancelIndication()
	</pre>
	
    Whereas, a synchronous provider supports this method:

	<pre>
	checkIndication()
	</pre>

    The CIMOM first attempts to call provideIndication(), given at least once
    subscription exists.  If the provider does not support this method, the
    CIMOM assumes the provider generates indications synchronously using
    checkIndication(). If the provider does not support any of the methods of
    this interface, it is not considered an indication provider and an error
    is generated.
*/
class PEGASUS_PROVIDER_LINKAGE CIMIndicationProvider
    : public virtual CIMBaseProvider
{
public:

    CIMIndicationProvider(void);

    virtual ~CIMIndicationProvider(void);

    /**
	Instructs the provider to begin generating indications of the type
	specified in the classReference parameter.
	
	This method is invoked when the CIMOM has at least one active
	subscription that links the indication type with a handler. Once
	this method has been invoked, changes to the active subscriptions
	are communicated via the updateIndication() method. The
	cancelIndication() method is invoked when all active subscriptions
	have been removed.
	
	For example, assume that two subscriptions exist that point to the
	following filters.

	<pre>
	    SELECT Property1 FROM Sample_Indication
	    WHERE Property1="foo" WITHIN 30000"

	    SELECT Property2 FROM Sample_Indication
	    WHERE Property1="bar" WITHIN 60000"
	</pre>
	
	The contents of the paramters (in string form) to this method might
	look like the following.
	
	<pre>
	    classReference = "localhost/root/cimv2:Sample_Indication"
	    minimumInterval = "00000000003000.000000:000" (30 minutes)
	    maximumInterval = "00000000006000.000000:000" (60 minutes)
	    propertyList = "Property1", "Property2"
	</pre>
	
	NOTE: The WHERE clause is not evaluated by the provider. Providers
	generate indications according to predefined events and are not
	specified by indication filters. The existence of a filter simply
	notifies a provider that some client is interested in indications
	of a specified type. The filters specify the criteria for indication
	delivery, not creation. In other words, the filter is resolved by
	CIMOM, not the provider.
	
	Assuming the above parameters, the provider should attach to some
	resource and begin monitoring every 30 seconds (optimally). When
	some predefined event occurs (a circumstance that merits an
	indication), the provider should create an indication containing
	the properties listed in the propertyList parameter. The provider
	then delivers the indication to the handler associated with the
	indication type and continues monitoring. A call to
	updateIndication() means that the indication generation information
	(minimumInterval, maximumInterval, and propertyList) has changed,
	and a call to cancelIndication() notifies the provider to
	discontinue monitoring (no subscriptions exist).

	NOTE: Under normal circumstances the provider should not call
	handler.complete() in this method; it should be called in
	cancelIndication(). Calling this method implies no more results are
	available and will effectively disable result forwarding for the
	handler.
		
	@param contex contains security and locale information relevant
	    for the lifetime of this operation.
	
	@param classReference provides a fully qualified reference of the
	    indication class of interest.
	
	@param minimumInterval specifies the minimum requested indication
	    delivery frequency. This is an optional parameter where an
	    interval of zero ("0000000000.000000:000") implies not
	    specified.
	
	@param maximumInterval specifies the maximum requested indication
	    delivery frequency. This is an optional parameter where an
	    interval of zero ("0000000000.000000:000") implies not
	    specified.
	
	@param propertyList specifies the properties of interest within the
	    class identified by the classReference parameter.
	
	@param handler asynchronusly processes the results of this
	operation.
	
	@exception NotImplemented
	@exception InvalidArgument
    */
    virtual void provideIndication(
	    const OperationContext & context,
	    const CIMReference & classReference,
	    const CIMDateTime & minimumInterval,
	    const CIMDateTime & maximumInterval,
	    const Array<String> & propertyList,
	    ResponseHandler<CIMIndication> & handler) = 0;

    /**
	Instructs the provider to update the information regarding the
	indication of the type specified in the classReference parameter.
	
	Once the provideIndication method has been called, the CIMOM
	communicates significant changes about the indication generatation
	information (minimumInterval, maximumInterval, and propertyList)
	via this method.
	
	Assuming provideIndications() was called with the parameters in the
	sample above, and a new subscription is created for the following
	filter.
	
	<pre>
	    SELECT Property1, Property3 FROM Sample_Indication
	    WHERE Property1="bar" WITHIN 60000
	</pre>
	
	The contents of the paramters (in string form) to this method might
	look like the following.
	
	<pre>
	    classReference = "localhost/root/cimv2:Sample_Indication"
	    minimumInterval = "00000000003000.000000:000" (30 minutes)
	    maximumInterval = "00000000006000.000000:000" (60 minutes)
	    propertyList = "Property1", "Property2", "Property3"
	</pre>
	
	NOTE: The WHERE clause is not evaluated by the provider. Providers
	generate indications according to predefined events and are not
	specified by indication filters. The existence of a filter simply
	notifies a provider that some client is interested in indications
	of a specified type. The filters specify the criteria for
	indication delivery, not creation. In other words, indications may
	be created by this provider and later discarded by the CIMOM
	(while evaluating the filter).
	
	Assuming the above parameters, the provider should adjust add
	Property3 to any indications generated from this point forward.
	
	Subsequent subscriptions associated with existing filters do not
	result in calls to the provider.

	NOTE: Under normal circumstances the provider should not call
	handler.complete() in this method; it should be called in
	cancelIndication(). Calling the handler.complete() method
	implies no more results are available and will effectively
	disable result forwarding for the handler.
		
	@param context contains security and locale information
	relevant for the lifetime of this operation.
	
	@param classReference provides a fully qualified reference of the
	    indication class of interest.
	
	@param minimumInterval specifies the minimum requested indication
	    delivery frequency. This is an optional parameter where an
	    interval of zero ("0000000000.000000:000") implies not
	    specified.
	
	@param maximumInterval specifies the maximum requested indication
	    delivery frequency. This is an optional parameter where an
	    interval of zero ("0000000000.000000:000") implies not
	    specified.
	
	@param propertyList specifies the properties of interest within
	    the class identified by the classReference parameter.
	
	@param handler asynchronusly processes the results of this
	    operation.

	@exception NotImplemented
	@exception InvalidArgument
    */
    virtual void updateIndication(
	    const OperationContext & context,
	    const CIMReference & classReference,
	    const CIMDateTime & minimumInterval,
	    const CIMDateTime & maximumInterval,
	    const Array<String> & propertyList,
	    ResponseHandler<CIMIndication> & handler) = 0;

    /**
	Instructs the provider to stop providing indications of the type
	specified in the classReference parameter.
	
	This method is called after provideIndication() and implies that
	either no subscriptions exist or that the CIMOM is shutting down.
	The provider should release resources associated with generating
	indications of the type specified in classReference. If a
	subscription is later created, provideIndications will be called
	again.

	Upon completion of this method, the provider should call
	handler.complete (the CIMOM will call it, if necessary) at which
	point the handle is no longer guaranteed to be valid. Usage of the
	handle after complete is undefined.
	
	@param context contains security and locale information relevant
	    for the lifetime of this operation.
	
	@param classReference provides a fully qualified reference of the
	    indication class of interest.
	
	@param handler asynchronusly processes the results of this
	    operation.

	@exception NotImplemented
	@exception InvalidArgument
    */
    virtual void cancelIndication(
	    const OperationContext & context,
	    const CIMReference & classReference,
	    ResponseHandler<CIMIndication> & handler) = 0;

    /**
	Instructs the provider to check the managed resource and
	immediately generate indications, if necessary, and complete.

	Note that this method is used (to the exclusion of the others) for
	synchronous ("polled") indications.
	
	This method is called periodically by the CIMOM to allow the
	provider to create indications without actively monitoring a
	resource.

	Note the CIMOM frequency wherewith the CIMOM invokes this method
	is related to the "intervals" defined by the filter.
	
	Because the method executes on the CIMOM's thread, the provider
	should attempt to complete the method prompty to allow the CIMOM
	to service other providers.

	REVIEW: the above statement we hope is in error. This method
	must not be invoked on the CIMOM's thread.
	
	@param context contains security and locale information
	relevant for the lifetime of this operation.
	
	@param classReference provides a fully qualified reference of the
	    indication class of interest.
	
	@param The propertyList specifies the properties of interest
	    within the class identified by the classReference parameter.
	
	@param handler asynchronusly processes the results of this
	    operation.

	@exception NotImplemented
	@exception InvalidArgument

	Note the absense of interval parameters as these are processed by
	the CIMOM itself.
    */
    virtual void checkIndication(
	    const OperationContext & context,
	    const CIMReference & classReference,
	    const Array<String> & propertyList,
	    ResponseHandler<CIMIndication> & handler) = 0;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMIndicationProvider_h */
