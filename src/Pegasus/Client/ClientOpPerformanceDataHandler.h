//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef ClientOpPerformanceDataHandler_h
#define ClientOpPerformanceDataHandler_h


#include <Pegasus/Common/CIMOperationType.h> 
#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN
                                                                          

struct ClientOpPerformanceData
{
   /** Identifys operation statistical information is for
    */
    CIMOperationType operationType;

    /**serverTimeValid is true if the CIM server has sent back
    a valid number in the WBEMServerResponseTime  header
    field of the response.
    */
    Boolean serverTimeValid;

    /**serverTime is the number of micro seconds the CIM Server
     has taken to process request.
    */
    Uint64 serverTime;

    /**roundTripTime is the number of micro seconds a request/response
    has spent in the network and server combined. roundTripTime includes
    serverTime
    */
    Uint64 roundTripTime;

    /**requestSize is the request message size in bytes.
    */
    Uint64 requestSize; 

    /**responseSize is the response message size in bytes.
    */
    Uint64 responseSize; 
 };
    

PEGASUS_NAMESPACE_END

#endif /* ClientOpPerformanceDataHandler_h */

