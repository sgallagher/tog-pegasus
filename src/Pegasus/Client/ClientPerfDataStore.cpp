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
// Author: Willis White (whiwill@us.ibm.com)
//
// Modified By: Aruran, IBM(ashanmug@in.ibm.com) for Bug# 3674
//
//%/////////////////////////////////////////////////////////////b////////////////


#include "ClientPerfDataStore.h"
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_STD;

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

ClientPerfDataStore* ClientPerfDataStore::current_Store = NULL;

ClientPerfDataStore* ClientPerfDataStore::Instance(){
   static ClientPerfDataStore current_Store; 
   return &current_Store;
}

ClientPerfDataStore::ClientPerfDataStore()
{
}

void ClientPerfDataStore::reset()
{
    _operationType = CIMOPTYPE_INVOKE_METHOD;
    _serverTimeKnown = false;
    _errorCondition = false;
    _serverTime = 0;                
    _networkStartTime = TimeValue(); 
    _networkEndTime = TimeValue(); 
    _requestSize = 0; 
    _responseSize = 0;
    _messID = "";

}
     

ClientOpPerformanceData ClientPerfDataStore::createPerfDataStruct()
{
    ClientOpPerformanceData _ClientOpPerfData_obj;
    _ClientOpPerfData_obj.roundTripTime = _networkEndTime.toMilliseconds()-_networkStartTime.toMilliseconds();
    _ClientOpPerfData_obj.operationType = _operationType;
    _ClientOpPerfData_obj.requestSize = _requestSize;
    _ClientOpPerfData_obj.responseSize = _responseSize;
    _ClientOpPerfData_obj.serverTimeKnown = _serverTimeKnown;
    if (_serverTimeKnown) {
        _ClientOpPerfData_obj.serverTime = _serverTime;
    }
    return _ClientOpPerfData_obj;
}  


        
void ClientPerfDataStore::setServerTime(Uint32 time)
{   _serverTime = time;
    _serverTimeKnown = true;
}

        
void ClientPerfDataStore::setResponseSize(Uint64 size)
{ _responseSize = size; }
       

void ClientPerfDataStore::setRequestSize(Uint64 size)
{  _requestSize = size; }

        
void ClientPerfDataStore::setStartNetworkTime(void)
{ _networkStartTime = TimeValue::getCurrentTime();  }


void ClientPerfDataStore::setEndNetworkTime(TimeValue time)
{ _networkEndTime = time; }

        
void ClientPerfDataStore::setServerTimeKnown(Boolean bol)
{   _serverTimeKnown = bol; }


void ClientPerfDataStore::setMessageID(String messageID)
{ _messID = messageID; }
       

void ClientPerfDataStore::setOperationType(Uint32 type)
{   
  _operationType = Message::convertMessageTypetoCIMOpType(type);                             
}


Boolean ClientPerfDataStore::checkMessageIDandType(const String& messageID, Uint32 type)
{ if(_messID != messageID)
  {
    _errorCondition = true;
    return false; 
  }
  
  if (_operationType != Message::convertMessageTypetoCIMOpType(type)) 
  {
    _errorCondition = true;
    return false; 
  }

  return true;
}
 
String ClientPerfDataStore::toString() const
{
    Array<char> out;
    /*XMLWriter::append(out, String(" serverTime = ");
    XMLWriter::append(out, _serverTime);
     << "\r\n";  */
    out << " operation type  = " << (Uint32)_operationType << "\r\n";
    out << " network start time is = " << _networkStartTime.toMilliseconds() << "\r\n";
    out << " network end time = " << _networkEndTime.toMilliseconds() << "\r\n";
    out << " numberofRequestBytes = " << (Uint32)_requestSize << "\r\n";
    out << " number foRespoonse Bytes = " << (Uint32)_responseSize << "\r\n";
    out << "the message ID is " << _messID << "\r\n";
    if (_errorCondition) {
        out << "the error condition is true " << "\r\n";
    }
    else{
        out << " he error condition is false" << "\r\n";
    }
    if (_classRegistered) {
        out << "there is a class registered" << "\r\n";
    }
    else{
        out << "no class is registered" << "\r\n";
    }
    if (_serverTimeKnown) {
        out << "_serverTimeKnown is true" << "\r\n";
    }
    else{
        out << "_serverTimeKnown is false" << "\r\n";
    }

    //return a Pegasus String constructed form the array "out"
    return (String(out.getData(), out.size()));
}
    
 

Boolean ClientPerfDataStore::getStatError() const
{
    return _errorCondition;
}

void ClientPerfDataStore::setClassRegistered(Boolean bol)
{
    _classRegistered = bol;
}

Boolean ClientPerfDataStore::isClassRegistered() const
{
    return _classRegistered;
}

PEGASUS_NAMESPACE_END



