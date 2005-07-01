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
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for Bug#1939
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMOMStatDataProvider.h"

#include <Pegasus/Common/PegasusVersion.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

CIMOMStatDataProvider::CIMOMStatDataProvider()
{
   for (Uint32 i=0; i<StatisticalData::length; i++){
      char buffer[32];
      sprintf(buffer, "%d", i);
      _references[i] = CIMObjectPath(
        "CIM_CIMOMStatisticalData.InstanceID=\"CIM_CIMOMStatisticalData"+String(buffer)+"\"");
   }

}

CIMOMStatDataProvider::~CIMOMStatDataProvider()
{
}

void CIMOMStatDataProvider::initialize(CIMOMHandle & cimom)
{
   _cimom = cimom;
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
	{  //cout << "this is what we are looking at " << _references[i].toString() <<endl <<endl;
		if(localReference == _references[i])
		{
			// deliver requested instance
			handler.deliver(getInstance(i, instanceReference));
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
		handler.deliver(getInstance(i, classReference));
		
	}

//	printf("have all the instances\n");
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

CIMInstance CIMOMStatDataProvider::getInstance(Uint16 type, CIMObjectPath cimRef)
{

   StatisticalData* sd = StatisticalData::current();
   char buffer[32];
   sprintf(buffer, "%u", type);

   checkObjectManager();

   CIMDateTime cimom_time = CIMDateTime((sd->cimomTime[type]), true);
   CIMDateTime provider_time = CIMDateTime((sd->providerTime[type]), true);
   Uint16 mof_type = getOpType(type);



   CIMInstance requestedInstance("CIM_CIMOMStatisticalData");
   requestedInstance.addProperty(CIMProperty("InstanceID",
      CIMValue("CIM_CIMOMStatisticalData"+String(buffer))));
   requestedInstance.addProperty(CIMProperty("OperationType",
      CIMValue(mof_type)));
   requestedInstance.addProperty(CIMProperty("NumberOfOperations",
      CIMValue((Uint64)sd->numCalls[type])));
   requestedInstance.addProperty(CIMProperty("CimomElapsedTime",
      CIMValue(cimom_time)));
   requestedInstance.addProperty(CIMProperty("ProviderElapsedTime",
      CIMValue(provider_time)));
   requestedInstance.addProperty(CIMProperty("RequestSize",
      CIMValue((Uint64)sd->requestSize[type])));
   requestedInstance.addProperty(CIMProperty("ResponseSize",
      CIMValue((Uint64)sd->responseSize[type])));
   requestedInstance.addProperty(CIMProperty("ElementName",
      CIMValue(StatisticalData::requestName[type])));
   requestedInstance.addProperty( CIMProperty("Description",
      CIMValue(String("CIMOM performance statistics for CIM request "))));
   requestedInstance.addProperty(CIMProperty("Caption",
      CIMValue(String("CIMOM performance statistics for CIM request"))));

   requestedInstance.setPath(_references[type]);

//cout << "at the end of getinstance of CIMOMStat" << endl;
   return requestedInstance;
}

/*CIMDateTime CIMOMStatDataProvider::toDateTime(Sint64 date)
{
        //break millisecond value into days, hours, minutes, seconds and milliseconds
        //turn each number into a string and append them to each other

   	const Sint64 oneDay = Sint64(864) * 100000000;

        Sint64 ndays = date/oneDay;        //one day = 8.64*10^10 millisecond
        Sint64 rem = date % oneDay;                //rem_1 is remander of above operation
        char buf_day[9];
        sprintf(buf_day,"%08d",(int)ndays);

String test = String(buf_day);


        Sint64 nhour = rem/PEGASUS_UINT64_LITERAL(3600000000);  //one hour = 3.6*10^9 milliseconds
        Sint64 rem_2 = rem%PEGASUS_UINT64_LITERAL(3600000000);    //rem_2 is remander of above operation
        char buf_hour[3];
        sprintf(buf_hour,"%02d",(int)nhour);

String hour = String(buf_hour);
String dh = test.append(String(buf_hour));
//printf("this is test now after append\n");// %s\n", test.getCString());

        Sint64 nmin = rem_2/60000000;  // one minute = 6*10^7
        Sint64 rem_3 = rem_2%60000000;
        char buf_minute[3];
        sprintf(buf_minute,"%02d",(int)nmin);

String dhm = dh.append(String(buf_minute));
//printf("after second append this is test %s\n", test.getCString());

        Sint64 nsecond = rem_3/1000000; //one second = 10^6 milliseconds
        char buf_second[3];
        sprintf(buf_second,"%02d",(int)nsecond);

String dhms = dhm.append(String(buf_second));
//printf("after third append this is test\n");// %s \n",test.getCString());


        Sint64 nmilsec = rem_3%1000000;
        char buf_milsec[20];
        sprintf(buf_milsec,".%06d:000",(int)nmilsec);

String dhmsm = dhms.append(String(buf_milsec));
    //cout << "String for datetime= " << dhmsm << endl;
    CIMDateTime ans;
    try
    {
        ans.set(dhmsm);
    }
    catch(Exception& e)
    {
        cout << "Error in string convert of " << dhmsm << " " << e.getMessage() << endl;;
        ans.clear();
    }
//cout<<"this is being passed back for toDateTime" << ans.toString() << endl;

        return ans;

} */


void CIMOMStatDataProvider::checkObjectManager()
{
	
	StatisticalData* sData = StatisticalData::current();

	if (!sData->copyGSD)
	{  //set all values to 0 if CIM_ObjectManager is False

		for (Uint16 i=0; i<StatisticalData::length; i++)
		{
		  sData->numCalls[i] = 0;
		  sData->cimomTime[i] = 0;      
		  sData->providerTime[i] = 0;
		  sData->responseSize[i] = 0;
		  sData->requestSize[i] = 0;
		}
//		printf("just set all tha values of StatistcalData to 0\n");

	}

//	printf("at the end of the checkObjectManager func\n");
}




Uint16 CIMOMStatDataProvider::getOpType(Uint16 type)
{
	Uint16 ty, outType;
// the rang of operation types is defined in Message.cpp as 1-113. For operation types
// with values grater then 39 fourty is subtracted (in StatisticalData.h and Message.cpp

// This conversion makes make the OperationType attribute of the CIM_CIMOMStatisticalData
// instances agree with DMTF spec.
		


	ty = type;

 	if ((ty==0) || (ty==1))
 		{outType = type+3;
	//	printf("ty was 0 or 1\n");
	}
 
 	else if ((ty >=3) && (ty <= 23))
 		{outType = ty + 2;
	//	printf("ty was between 3 and 23\n");
	}
 
 	else if ((ty == 2) || ((ty > 23) && (ty < 73)))
 		{ outType = 1;
	//	printf("ty was between 32 and 73 or it was 2\n");
	}
 
 	else 
 	{ outType = 0;
	//printf("ty was out of this world\n");
	}

//	printf("about to return form getOpType\n");
	return outType;
}


PEGASUS_NAMESPACE_END
