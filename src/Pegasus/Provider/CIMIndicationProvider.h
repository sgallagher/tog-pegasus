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

/**
This class defines the set of methods implemented by an indication provider. A providers that derives
from this class must implement all methods. The minimal method implementation simply throw the
NotSupported exception.

In general, the CIMOM classifies the provider as asynchronous or synchronous depending on the methods
implemented by the provider. An asynchronous indication provider supports provideIndication,
updateIndication, and cancelIndication. A synchronous provider supports checkIndication. A provider
can support both interfaces.
	
The CIMOM first attempts to call provideIndication, given at least once subscription exists.
If the provider does not support the method, the CIMOM assumes the provider generates indications
synchronously using checkIndication. If the provider does not support any of the methods of this
interface, is not considered an indication provider and an error is generated.
*/
class PEGASUS_PROVIDER_LINKAGE CIMIndicationProvider : public virtual CIMBaseProvider
{
public:
	CIMIndicationProvider(void);
	virtual ~CIMIndicationProvider(void);

	/**
	Instructs the provider to begin generating indications of the type specified in
	the classReference parameter.
	
	This method is invoked when the CIMOM has at least one active subscription that
	links the indication type with a handler. Once this method has been invoked, changes
	to the active subscriptions are communicated via the updateIndication method. The
	cancelIndication method is invoked when all active subscriptions have been removed.
	
	For example, assume that two subscriptions exist that point to the following filters.
	
	<pre>
	<code>"SELECT Property1 FROM Sample_Indication WHERE Property1="foo" WITHIN 30000"</code>
	<code>"SELECT Property2 FROM Sample_Indication WHERE Property1="bar" WITHIN 60000"</code>
	</pre>
	
	The contents of the paramters (in string form) to this method might look like the following.
	
	<pre>
	<code>classReference = "localhost/root/cimv2:Sample_Indication"</code>
	<code>minimumInterval = "00000000003000.000000:000" (30 minutes)</code>
	<code>maximumInterval = "00000000006000.000000:000" (60 minutes)</code>
	<code>propertyList = "Property1", "Property2"</code>
	</pre>
	
	NOTE: The WHERE clause is not evaluated by the provider. Providers generate indications according
	to predefined events and are not specified by indication filters. The existence of a filter simply
	notifies a provider that some client is interested in indications of a specified type. The filters
	specify the criteria for indication delivery, not creation.
	
	Assuming the above parameters, the provider should attach to some resource and begin monitoring
	every 30 seconds (optimally). When some predefined event occurs (a circumstance that merits an
	indication), the provider should create and indication containing the properties listed in the
	propertyList parameter. The provider then delivers the indication to the handler associated with the
	indication type and continues monitoring. A call to updateIndication means that the indication
	generation information (minimumInterval, maximumInterval, and propertyList) has changed, and a call
	to cancelIndication notifies the provider to discontinue monitoring (no subscriptions exist).

	NOTE: Under normal circumstances the provider should not call handler.complete() in this method;
	it should be called in cancelIndication. Calling this method implies no more results are available
	and will effectively disable result forwarding for the handler.
		
	@param contex contains security and locale information relevant for the lifetime
	of this operation.
	
	@param classReference provides a fully qualified reference of the indication
	class of interest.
	
	@param minimumInterval specifies the minimum requested indication delivery frequency. This is an
	optional parameter where an interval of zero ("0000000000.000000:000") implies not specified.
	
	@param maximumInterval specifies the maximum requested indication delivery frequency. This is an
	optional parameter where an interval of zero ("0000000000.000000:000") implies not specified.
	
	@param propertyList specifies the properties of interest within the class
	identified by the classReference parameter.
	
	@param handler asynchronusly processes the results of this operation.
	
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
	Instructs the provider update the information regarding the indication of the type specified
	in the classReference parameter.
	
	Once the provideIndication method has been called, the CIMOM communicated significant changes
	to the indication generatation information (minimumInterval, maximumInterval, and propertyList)
	via this method.
	
	Assuming provideIndications was called with the parameters in the sample above, and a new
	subscription is created for the following filter.
	
	<pre>
	<code>"SELECT Property1, Property3 FROM Sample_Indication WHERE Property1="bar" WITHIN 60000"</code>
	</pre>
	
	The contents of the paramters (in string form) to this method might look like the following.
	
	<pre>
	<code>classReference = "localhost/root/cimv2:Sample_Indication"</code>
	<code>minimumInterval = "00000000003000.000000:000" (30 minutes)</code>
	<code>maximumInterval = "00000000006000.000000:000" (60 minutes)</code>
	<code>propertyList = "Property1", "Property2", "Property3"</code>
	</pre>
	
	NOTE: The WHERE clause is not evaluated by the provider. Providers generate indications according
	to predefined events and are not specified by indication filters. The existence of a filter simply
	notifies a provider that some client is interested in indications of a specified type. The filters
	specify the criteria for indication delivery, not creation.
	
	Assuming the above parameters, the provider should adjust add Property3 to any indications
	generated from this point forward.
	
	New subscriptions associated to existing filters do not result in calls to the provider.

	NOTE: Under normal circumstances the provider should not call handler.complete() in this method;
	it should be called in cancelIndication. Calling this method implies no more results are available
	and will effectively disable result forwarding for the handler.
		
	@param context contains security and locale information
	relevant for the lifetime of this operation.
	
	@param classReference provides a fully qualified reference of the indication
	class of interest.
	
	@param minimumInterval specifies the minimum requested indication delivery frequency. This is an
	optional parameter where an interval of zero ("0000000000.000000:000") implies not specified.
	
	@param maximumInterval specifies the maximum requested indication delivery frequency. This is an
	optional parameter where an interval of zero ("0000000000.000000:000") implies not specified.
	
	@param propertyList specifies the properties of interest within the class
	identified by the classReference parameter.
	
	@param handler asynchronusly processes the results of this operation.

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
	Instructs the provider to stop providing indications of the type specified in the
	classReference parameter.
	
	This method is called after provideIndication and implies that either no
	subscriptions exist or that the CIMOM is shutting down. The provider should release
	resources associated with generating indications of the type specified in
	classReference. If a subscription is later created, provideIndications will be
	called again.
	
	Upon completion of this method, the provider should call handler.complete (the CIMOM
	will call it, if necessary) at which point the handle is no longer guaranteed to be
	valid. Usage of the handle after complete is undefined.
	
	@param context contains security and locale information relevant for the lifetime
	of this operation.
	
	@param classReference provides a fully qualified reference of the indication
	class of interest.
	
	@param handler asynchronusly processes the results of this operation.

	@exception NotImplemented
	@exception InvalidArgument
	*/
	virtual void cancelIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		ResponseHandler<CIMIndication> & handler) = 0;

	/**
	Instructs the provider to check the managed resource and immediately generate indications,
	if necessary, and complete.
	
	This method is called periodically by the CIMOM to allow the provider to create indications
	without actively monitoring a resource. Because the method executes on the CIMOM's thread,
	the provider should attempt to complete the method prompty to allow the CIMOM to service
	other providers.
	
	@param context contains security and locale information
	relevant for the lifetime of this operation.
	
	@param classReference provides a fully qualified reference of the indication
	class of interest.
	
	@param The propertyList specifies the properties of interest within the class
	identified by the classReference parameter.
	
	@param handler asynchronusly processes the results of this operation.

	@exception NotImplemented
	@exception InvalidArgument
	*/
	virtual void checkIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		const Array<String> & propertyList,
		ResponseHandler<CIMIndication> & handler) = 0;
};

PEGASUS_NAMESPACE_END

#endif
