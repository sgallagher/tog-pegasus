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
// Author: Arthur Pichlkostner
//             (checked in: Markus Mueller sedgewick_de@yahoo.de)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "IBM_CIMOMStatDataProvider.h"

PEGASUS_NAMESPACE_BEGIN

IBM_CIMOMStatDataProvider::IBM_CIMOMStatDataProvider(void)
{
}

IBM_CIMOMStatDataProvider::~IBM_CIMOMStatDataProvider(void)
{
}

void IBM_CIMOMStatDataProvider::initialize(CIMOMHandle & cimom)
{
   for (Uint32 i=0; i<StatisticalData::length; i++){
      char buffer[32];
      sprintf(buffer, "%d", i);
      _references[i] = CIMObjectPath(
        "IBM_CIMOMStatData.InstanceID=\"IBM_CIMOMStatData"+String(buffer)+"\"");
   }

}

void IBM_CIMOMStatDataProvider::terminate(void)
{
   delete this;
}

void IBM_CIMOMStatDataProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
	CIMObjectPath localReference = CIMObjectPath(
		String::EMPTY,
		CIMNamespaceName(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());

	// begin processing the request
	handler.processing();

	// instance index corresponds to reference index
	for(Uint32 i = 0; i < StatisticalData::NUMBER_OF_TYPES; i++)
	{ // cout << "loop" << endl;
		if(localReference == _references[i])
		{
			// deliver requested instance
			handler.deliver(getInstance(i));
			break;
		}
	}

	// complete processing the request
	handler.complete();
}

void IBM_CIMOMStatDataProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    // begin processing the request
	handler.processing();

	// instance index corresponds to reference index
	for(Uint32 i = 0; i < StatisticalData::NUMBER_OF_TYPES; i++)
	{
	   // deliver instance
		handler.deliver(getInstance(i));
	}

	// complete processing the request
	handler.complete();
}

void IBM_CIMOMStatDataProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
	// begin processing the request
	handler.processing();

	for(Uint32 i = 0; i < StatisticalData::NUMBER_OF_TYPES; i++)
	{
		// deliver reference
		handler.deliver(_references[i]);
	}

	// complete processing the request
	handler.complete();
}

void IBM_CIMOMStatDataProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
	throw CIMNotSupportedException("StatisticalData::modifyInstance");
}

void IBM_CIMOMStatDataProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
	throw CIMNotSupportedException("StatisticalData::createInstance");
}

void IBM_CIMOMStatDataProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
throw CIMNotSupportedException("StatisticalData::deleteInstance");
}

CIMInstance IBM_CIMOMStatDataProvider::getInstance(Uint16 type)
{
   StatisticalData* sd = StatisticalData::current();
   char buffer[32];
   sprintf(buffer, "%u", type);

   CIMInstance requestedInstance("IBM_CIMOMStatData");
   requestedInstance.addProperty( CIMProperty("Description",
      CIMValue(String("CIMOM performance statistics for CIM request type <reqx>"))));
   requestedInstance.addProperty(CIMProperty("Caption",
      CIMValue(String("CIMOM performance statistics for CIM request type <reqx>"))));
   requestedInstance.addProperty(CIMProperty("InstanceID",
      CIMValue("IBM_CIMOMStatData"+String(buffer))));
   requestedInstance.addProperty(CIMProperty("Name",
      CIMValue(StatisticalData::requestName[type])));
   requestedInstance.addProperty(CIMProperty("requestType",
      CIMValue(type)));
   requestedInstance.addProperty(CIMProperty("NumberOfRequests",
      CIMValue(sd->numCalls[type])));
   requestedInstance.addProperty(CIMProperty("TotalCimomTime",
      CIMValue(sd->cimomTime[type])));
   requestedInstance.addProperty(CIMProperty("TotalProviderTime",
      CIMValue(sd->providerTime[type])));
   requestedInstance.addProperty(CIMProperty("TotalRequestSize",
      CIMValue(sd->requestSize[type])));
   requestedInstance.addProperty(CIMProperty("TotalResponseSize",
      CIMValue(sd->responseSize[type])));

   return requestedInstance;
}

PEGASUS_NAMESPACE_END
