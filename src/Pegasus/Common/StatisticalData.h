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

#ifndef STATISTICAL_DATA_H
#define STATISTICAL_DATA_H

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <cstring>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_HAS_PERFINST

#define STAT_GETSTARTTIME \
timeval startTime; \
pegasus_gettimeofday(&startTime);

#define STAT_PMS_PROVIDEREND \
response->endProvider();            \
response->setStartServerTime(request->getStartServerTime()); \
response->setStartProviderTime(startTime);

#define STAT_SERVERSTART request->setStartServerTime(startTime);

#define STAT_SERVEREND \
response->endServer(); \
Array<Sint8> timeOut;  \
timeOut.reserve(10); \
timeOut << response->getTotalTime();      \
message.insert(30, timeOut.getData(), timeOut.size()); \
Uint32 statType = (response->getType() > CIM_GET_CLASS_RESPONSE_MESSAGE)? \
    response->getType() - CIM_GET_CLASS_RESPONSE_MESSAGE: \
    response->getType()-1; \
StatisticalData::current()->addToValue(message.size(), \
    statType, StatisticalData::BYTES_SENT);

#define STAT_SERVEREND_ERROR   response->endServer();


#define STAT_PROVIDERSTART request->startProvider();


#define STAT_PROVIDEREND   request->endProvider();

#define STAT_COPYDISPATCHER response->setStartServerTime(request->getStartServerTime());


// copy request timing info into response
#define STAT_COPYDISPATCHER_REP \
response->setStartServerTime(request->getStartServerTime());   \
response->setStartProviderTime(request->getStartProviderTime());   \
response->setEndProviderTime(request->getEndProviderTime());

#define STAT_BYTESREAD \
Uint32 statType = (request->getType() > CIM_GET_CLASS_RESPONSE_MESSAGE)? \
    request->getType() - CIM_GET_CLASS_RESPONSE_MESSAGE: \
    request->getType()-1; \
StatisticalData::current()->addToValue(contentLength, \
    statType, StatisticalData::BYTES_READ);


#define STAT_SERVERTIME out << "Servertime:  \r\n";
#else
#define STAT_GETSTARTTIME
#define STAT_PMS_PROVIDEREND
#define STAT_SERVERSTART
#define STAT_SERVEREND
#define STAT_SERVEREND_ERROR
#define STAT_PROVIDERSTART
#define STAT_PROVIDEREND
#define STAT_PROVIDEREND_REP
#define STAT_COPYDISPATCHER
#define STAT_BYTESREAD
#define STAT_SERVERTIME
#endif

class PEGASUS_COMMON_LINKAGE StatisticalData
{
   public:
      enum StatRequestType{
         GET_CLASS,
         GET_INSTANCE,
         DELETE_CLASS,
         DELETE_INSTANCE,
         CREATE_CLASS,
         CREATE_INSTANCE,
         MODIFY_CLASS,
         MODIFY_INSTANCE,
         ENUMERATE_CLASSES,
         ENUMERATE_CLASS_NAMES,
         ENUMERATE_INSTANCES,
         ENUMERATE_INSTANCE_NAMES,
         EXEC_QUERY,
         ASSOCIATORS,
         ASSOCIATOR_NAMES,
         REFERENCES,
         REFERENCE_NAMES,
         GET_PROPERTY,
         SET_PROPERTY,
         GET_QUALIFIER,
         SET_QUALIFIER,
         DELETE_QUALIFIER,
         ENUMERATE_QUALIFIERS,
         NUMBER_OF_TYPES
      };

      enum StatDataType{
         SERVER,
         PROVIDER,
         BYTES_SENT,
         BYTES_READ
      };

      static const Uint32 length;
      static StatisticalData* current();

      StatisticalData();

      timeval timestamp;

      Uint64 numCalls[NUMBER_OF_TYPES];
      Uint64 cimomTime[NUMBER_OF_TYPES];
      Uint64 providerTime[NUMBER_OF_TYPES];
      Uint64 responseSize[NUMBER_OF_TYPES];
      Uint64 requestSize[NUMBER_OF_TYPES];
      static StatisticalData* cur;
      void addToValue(Uint64 value, Uint16 type, Uint32 t);
      static String requestName[];

   protected:
      Mutex _mutex;
};


PEGASUS_NAMESPACE_END
#endif

