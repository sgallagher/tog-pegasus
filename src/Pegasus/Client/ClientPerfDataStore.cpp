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
// Author: Willis White (whiwill@us.ibm.com
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////


#include "ClientPerfDataStore.h"
#include <Pegasus/Common/CIMOperationType.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

ClientPerfDataStore* ClientPerfDataStore::current_Store = NULL;

ClientPerfDataStore* ClientPerfDataStore::current(){
   if (current_Store == NULL){
      current_Store = new ClientPerfDataStore();
   }
   return current_Store;
}

                                                                       
ClientPerfDataStore::ClientPerfDataStore()
{   current_Store = this;
 /*   serverTimeValid = false;
    serverTime = 0;
    networkStartTime = CIMDateTime("00000000000000.000000+000");
    networkEndTime = CIMDateTime("00000000000000.000000+000");
    requestSize = 0; 
    responseSize = 0;
    messID = "";   */

}

void ClientPerfDataStore::reset()
{
    operationType = CIMOPTYPE_INVOKE_METHOD;
    serverTimeValid = false;
    serverTime = 0;                
    networkStartTime = CIMDateTime(); //CIMDateTime("00000000000000.000000-000");
    networkEndTime = CIMDateTime(); //CIMDateTime("00000000000000.000000-000");
    requestSize = 0; 
    responseSize = 0;
    messID = "";

}
     

ClientOpPerformanceData ClientPerfDataStore::createPerfDataSrtuct()
{
    ClientOpPerformanceData * cOPD;
    cOPD = new ClientOpPerformanceData;
    return *cOPD;
}


        
void ClientPerfDataStore::setServerTime(Uint32 time)
{   serverTime = time;
    serverTimeValid = true;
}

        
void ClientPerfDataStore::setResponseSize(Uint32 size)
{ responseSize = size; }
       

void ClientPerfDataStore::setRequestSize(Uint32 size)
{  requestSize = size; }

        
void ClientPerfDataStore::setStartNetworkTime(void)
{ networkStartTime = CIMDateTime::getCurrentDateTime();  }

void ClientPerfDataStore::setEndNetworkTime(CIMDateTime time)
{ networkEndTime = time; }

        
void ClientPerfDataStore::setValidServerTime(Boolean bol)
{   serverTimeValid = bol; }

void ClientPerfDataStore::setMessageID(String messageID)
{ messID = messageID; }

        
CIMOperationType ClientPerfDataStore::setOperationType(Uint32 type)
{   //this function needs to translate message type (type) to CIMOperationType 
   //CIMOperationType oT = CIMOPTYPE_EMPTY;
   //return oT;

    Uint32 in_type, enum_type;
    CIMOperationType cT;


    in_type = type%40;      /* groups request/response message by type ie. getClassRequestMessage 
                                (type = 1) gives the same result as getClassResponseMessage (type = 41)*/

    if (in_type < 3) {
        enum_type = in_type;
    }
    else if((3 < in_type) && (in_type < 25)){
        enum_type = in_type -1;
    }
    else if (in_type == 25) {
        enum_type = 0;
    }
    else{
        //set error
        cout << "error getting type" << endl;
    }
    

    cT = (CIMOperationType)enum_type;
    operationType = cT;
    return cT;
                                
}

//void ClientPerfDataStore::setErrorCondition(Boolean bol)
//{  errorCondition = bol; }

Boolean ClientPerfDataStore::checkMessageIDandType(String & messageID, CIMOperationType type)
{ if(messID == messageID)
    { return true; }
  else
    { return false;}
}
 
void ClientPerfDataStore::print()
{
    cout << " serverTime = " << serverTime << endl;
    cout << " operation type  = " << (Uint32) operationType << endl;
    cout << " network start time is = " << networkStartTime.toString() << endl;
    cout << " network end time = " << networkEndTime.toString() << endl;
    cout << " numberofRequestBytes = " << requestSize << endl;
    cout << " number foRespoonse Bytes = " << responseSize << endl;
    cout << "the message ID is " << messID << endl;
}

    

PEGASUS_NAMESPACE_END



