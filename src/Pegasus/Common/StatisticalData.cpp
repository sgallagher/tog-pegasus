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
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for Bug#1090
//
//%/////////////////////////////////////////////////////////////////////////////

#include "StatisticalData.h"
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN

String StatisticalData::requestName[] = 
{
    "GET_CLASS",
    "GET_INSTANCE",
    "EXPORT_INDICATION",
    "DELETE_CLASS",
    "DELETE_INSTANCE",
    "CREATE_CLASS",
    "CREATE_INSTANCE",
    "MODIFY_CLASS",
    "MODIFY_INSTANCE",
    "ENUMERATE_CLASSES",
    "ENUMERATE_CLASS_NAMES",
    "ENUMERATE_INSTANCES",
    "ENUMERATE_INSTANCE_NAMES",
    "EXEC_QUERY",
    "ASSOCIATORS",
    "ASSOCIATOR_NAMES",
    "REFERENCES",
    "REFERENCE_NAMES",
    "GET_PROPERTY",
    "SET_PROPERTY",
    "GET_QUALIFIER",
    "SET_QUALIFIER",
    "DELETE_QUALIFIER",
    "ENUMERATE_QUALIFIERS",
    "INVOKE_METHOD"
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
