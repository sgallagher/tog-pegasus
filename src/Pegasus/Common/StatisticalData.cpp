//%////////////-*-c++-*-///////////////////////////////////////////////////////////
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
// Author: Arthur Pichlkostner
//             (checked in: Markus Mueller sedgewick_de@yahoo.de)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "StatisticalData.h"

PEGASUS_NAMESPACE_BEGIN

String StatisticalData::requestName[] = {
    "GET_CLASS",
    "GET_INSTANCE",
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
    "ENUMERATE_QUALIFIERS"
      };

const Uint32 StatisticalData::length = NUMBER_OF_TYPES;

StatisticalData* StatisticalData::cur = NULL;

StatisticalData* StatisticalData::current(){
   if (cur == NULL){
      cur = new StatisticalData();
   }
   return cur;
}

StatisticalData::StatisticalData(){
   for (unsigned int i=0; i<StatisticalData::length; i++){
      numCalls[i] = 0;
      cimomTime[i] = 0;      providerTime[i] = 0;
      responseSize[i] = 0;
      requestSize[i] = 0;
   }
}

void StatisticalData::addToValue(Uint64 value, Uint16 type, Uint32 t){
   _mutex.lock( pegasus_thread_self() );
   switch(t){
      case SERVER:      numCalls[type] += 1;
                        cimomTime[type] += value;
                        break;
      case PROVIDER:    providerTime[type] += value;
                        break;
      case BYTES_SENT:  responseSize[type] += value;
                        break;
      case BYTES_READ:  requestSize[type] += value;
                        break;
   }
   _mutex.unlock();
}

PEGASUS_NAMESPACE_END
