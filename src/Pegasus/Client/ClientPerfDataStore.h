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

#ifndef ClientPerfDataStore_h
#define ClientPerfDataStore_h

#include "ClientOpPerformanceDataHandler.h"
#include <Pegasus/Common/CIMOperationType.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Client/Linkage.h>
#include <iostream>
#include <fstream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN
                                                                          
/* ALL UINT32 VALUES MUST BE CHANGED TO UINT64 BEFORE CHECK IN
*/ 


class PEGASUS_CLIENT_LINKAGE ClientPerfDataStore
{
public:


    static ClientPerfDataStore* current();

    ClientPerfDataStore();

    /**Resets all the data members to 0
    */
    void reset();
     
    /**Creates a ClientOpPerformanceData from the current values of of the private data members
    */
    ClientOpPerformanceData createPerfDataSrtuct();

    /**checks the currentMessageID and operationType data members against  
    @param messageID and @param type if the values equea
    true is returned.
    @param messageID
    @return true if @param messageID equals currentMessageID data member
    */
    Boolean checkMessageIDandType(String & messageID, CIMOperationType type);

    /**sets the server time data member
    */
    void setServerTime(Uint32 time);

    /**sets the responsSize data member 
    */
    void setResponseSize(Uint32 size);

    /**sets the requestSize data member
    */
    void setRequestSize(Uint32 size);

    /**sets startNetworkTime data member
    */
    void setStartNetworkTime(void);

    /**sets endNetworkTime data member
    */
    void setEndNetworkTime(CIMDateTime time);


    /** sets validServerTime data member
    */
    void setValidServerTime(Boolean bol);

    /**sets operationType data member by translating message type given by 
    @param type, into a CIMOperationType. That value is ues to set the operationType
    data member and is returned.
    @param type integer repesenting the message type
    @return translation of message type (@param type) into it's oringinal 
    CIM operation (enum CIMOperationType)
    */
    CIMOperationType setOperationType(Uint32 type);

    //void setErrorCondition(Boolean bol);

    Boolean checkMessageID(String id);

    void print();

    void setMessageID(String messageID);

    static ClientPerfDataStore* current_Store;


private:
    CIMOperationType operationType;
    Boolean serverTimeValid;
    //Boolean errorCondition;
    Uint32 serverTime;
    Uint32 requestSize; 
    Uint32 responseSize; 
    String messID;
    CIMDateTime networkEndTime;
    CIMDateTime networkStartTime;
};   
    

PEGASUS_NAMESPACE_END

#endif /* ClientPerfDataStore_h */

