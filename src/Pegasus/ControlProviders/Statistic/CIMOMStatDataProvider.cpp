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
// Author: Arthur Pichlkostner
//             (checked in: Markus Mueller sedgewick_de@yahoo.de)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMOMStatDataProvider.h"

PEGASUS_NAMESPACE_BEGIN

CIMOMStatDataProvider::CIMOMStatDataProvider(void)
{
}

CIMOMStatDataProvider::~CIMOMStatDataProvider(void)
{
}

void CIMOMStatDataProvider::initialize(CIMOMHandle & cimom)
{
   for (Uint32 i=0; i<StatisticalData::length; i++){
      char buffer[32];
      sprintf(buffer, "%d", i);
      _references[i] = CIMObjectPath(
        "CIM_CIMOMStatisticalData.InstanceID=\"CIM_CIMOMStatisticalData"+String(buffer)+"\"");
   }

}

void CIMOMStatDataProvider::terminate(void)
{
   delete this;
}

void CIMOMStatDataProvider::getInstance(
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

void CIMOMStatDataProvider::enumerateInstances(
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

void CIMOMStatDataProvider::enumerateInstanceNames(
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

void CIMOMStatDataProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
	throw CIMNotSupportedException("StatisticalData::modifyInstance");
}

void CIMOMStatDataProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
	throw CIMNotSupportedException("StatisticalData::createInstance");
}

void CIMOMStatDataProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
throw CIMNotSupportedException("StatisticalData::deleteInstance");
}

CIMInstance CIMOMStatDataProvider::getInstance(Uint16 type)
{

   StatisticalData* sd = StatisticalData::current();
   char buffer[32];
   sprintf(buffer, "%u", type);

   CIMDateTime cimom_time = toDateTime(sd->cimomTime[type]);
   CIMDateTime provider_time = toDateTime(sd->providerTime[type]);


   CIMInstance requestedInstance("CIM_CIMOMStatisticalData");
   requestedInstance.addProperty(CIMProperty("InstanceID",
      CIMValue("CIM_CIMOMStatisticalData"+String(buffer))));
   requestedInstance.addProperty(CIMProperty("OperationType",
      CIMValue(type)));
   requestedInstance.addProperty(CIMProperty("NumberOfRequests",
      CIMValue(sd->numCalls[type])));
   requestedInstance.addProperty(CIMProperty("CimomElapsedTime",
      CIMValue(cimom_time)));
   requestedInstance.addProperty(CIMProperty("ProviderElapsedTime",
      CIMValue(provider_time)));
   requestedInstance.addProperty(CIMProperty("RequestSize",
      CIMValue(sd->requestSize[type])));
   requestedInstance.addProperty(CIMProperty("ResponseSize",
      CIMValue(sd->responseSize[type])));
   requestedInstance.addProperty(CIMProperty("ElementName",
      CIMValue(StatisticalData::requestName[type])));
   requestedInstance.addProperty( CIMProperty("Description",
      CIMValue(String("CIMOM performance statistics for CIM request type <reqx>"))));
   requestedInstance.addProperty(CIMProperty("Caption",
      CIMValue(String("CIMOM performance statistics for CIM request type <reqx>"))));
   

   return requestedInstance;
}


CIMDateTime CIMOMStatDataProvider::toDateTime(Sint64 date)
{
          
	//break millisecond value into days, hours, minutes, seconds and milliseconds
	//turn each number into a string and append them to each other

	Sint64 ndays = floor (date/86400000000);	//one day = 8.64*10^10 millisecond
	Sint64 rem = date % 86400000000;		//rem_1 is remander of above operation
	char buf_day[8];
	sprintf(buf_day,"%08d",ndays);

	Sint64 nhour = floor (rem/3600000000);	//one hour = 3.6*10^9 milliseconds
	Sint64 rem_2 = rem%3600000000;		//rem_2 is remander of above operation
	char buf_hour[2];
	sprintf(buf_hour,"%02d",nhour);

	Sint64 nmin = floor (rem_2/60000000);  // one minute = 6*10^7
	Sint64 rem_3 = rem_2%60000000;
	char buf_minute[2];
	sprintf(buf_minute,"%02d",nmin);

	Sint64 nsecond = floor (rem_3/1000000);	//one second = 10^6 milliseconds 
	char buf_second[2];
	sprintf(buf_second,"%02d",nsecond);

	Sint64 nmilsec = rem_3%1000000;
	char buf_milsec[6];
	sprintf(buf_milsec,"%06d",nmilsec);
	

	//build result
	char whole[26];
	int dlen = sprintf(whole,"%08s%02s%02s%02s.%06s:000"\
		,buf_day,buf_hour,buf_minute,buf_second,buf_milsec);
	String str(whole);
	CIMDateTime ans(str);
	return ans;

}


PEGASUS_NAMESPACE_END
