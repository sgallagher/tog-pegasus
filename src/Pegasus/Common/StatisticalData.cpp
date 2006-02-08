//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for Bug#1090
//
//%/////////////////////////////////////////////////////////////////////////////

#include "StatisticalData.h"
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN


// The table on the right represents the mapping from the enumerated types
// in the CIM_CIMOMStatisticalDate class ValueMap versus the internal 
// message type defined in Message.cpp. This conversion is performed by 
// getOpType() in CIMOMStatDataProvider.cpp.
//

String StatisticalData::requestName[] = 
{
                                    // Enumerated     ValueMap Value 
                                    // value from     from class
                                    // internal       CIM_StatisticalData
                                    // message type
                                    // -------------- -------------------
  "GetClass",                       //     0           3  
  "GetInstance",                    //     1           4  
  "IndicationDelivery",             //     2           26  
  "DeleteClass",                    //     3           5  
  "DeleteInstance",                 //     4           6  
  "CreateClass",                    //     5           7  
  "CreateInstance",                 //     6           8
  "ModifyClass",                    //     7           9
  "ModifyInstance",                 //     8          10 
  "EnumerateClasses",               //     9          11  
  "EnumerateClassNames",            //    10          12  
  "EnumerateInstances",             //    11          13  
  "EnumerateInstanceNames",         //    12          14  
  "ExecQuery",                      //    13          15  
  "Associators",                    //    14          16  
  "AssociatorNames",                //    15          17  
  "References",                     //    16          18  
  "ReferenceNames",                 //    17          19  
  "GetProperty",                    //    18          20  
  "SetProperty",                    //    19          21  
  "GetQualifier",                   //    20          22  
  "SetQualifier",                   //    21          23  
  "DeleteQualifier",                //    22          24  
  "EnumerateQualifiers",            //    23          25  
  "InvokeMethod"                    //    24          Not Present
};

const Uint32 StatisticalData::length = NUMBER_OF_TYPES;

StatisticalData* StatisticalData::cur = NULL;

StatisticalData* StatisticalData::current()
{
    if (cur == NULL)
    {
        cur = new StatisticalData();
    }
    return cur;
}

StatisticalData::StatisticalData()
{
    copyGSD = 0;

    for (unsigned int i=0; i<StatisticalData::length; i++)
    {
        numCalls[i] = 0;
        cimomTime[i] = 0;      
        providerTime[i] = 0;
        responseSize[i] = 0;
        requestSize[i] = 0;
    }
}

void StatisticalData::addToValue(Sint64 value, Uint16 type, Uint32 t)
{
    if (type >= NUMBER_OF_TYPES)
    {
         Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
             "StatData: Statistical Data Discarded.  "
                 "Invalid Request Type =  %u", type);
         return;
    }

    if (copyGSD)
    {
        AutoMutex autoMut(_mutex);
        switch (t)
        {
            case PEGASUS_STATDATA_SERVER:
                numCalls[type] += 1;
                cimomTime[type] += value;
                Tracer::trace(TRC_STATISTICAL_DATA, Tracer::LEVEL2,
                    "StatData: SERVER: %s(%d): count = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d; value = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d; total = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d",
                    (const char *)requestName[type].getCString(), type,
                    numCalls[type], value, cimomTime[type]);
                break;
            case PEGASUS_STATDATA_PROVIDER:
                providerTime[type] += value;
                Tracer::trace(TRC_STATISTICAL_DATA, Tracer::LEVEL2,
                    "StatData: PROVIDER: %s(%d): count = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d; value = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d; total = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d",
                    (const char *)requestName[type].getCString(), type,
                    numCalls[type], value, providerTime[type]);
                break;
        case PEGASUS_STATDATA_BYTES_SENT:
                responseSize[type] += value;
                Tracer::trace(TRC_STATISTICAL_DATA, Tracer::LEVEL2,
                    "StatData: BYTES_SENT: %s(%d): count = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d; value = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d; total = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d",
                    (const char *)requestName[type].getCString(), type,
                    numCalls[type], value, responseSize[type]);
                break;
        case PEGASUS_STATDATA_BYTES_READ:
                requestSize[type] += value;
                Tracer::trace(TRC_STATISTICAL_DATA, Tracer::LEVEL2,
                    "StatData: BYTES_READ: %s(%d): count = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d; value = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d; total = %"
                        PEGASUS_64BIT_CONVERSION_WIDTH "d",
                    (const char *)requestName[type].getCString(), type,
                    numCalls[type], value, requestSize[type]);
                break;
        }
    }
}

 void StatisticalData::setCopyGSD(Boolean flag)
{

	copyGSD = flag;
}

PEGASUS_NAMESPACE_END
