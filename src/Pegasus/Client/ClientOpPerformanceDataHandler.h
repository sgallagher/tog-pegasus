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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef ClientOpPerformanceDataHandler_h
#define ClientOpPerformanceDataHandler_h


#include <Pegasus/Common/CIMOperationType.h> 
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN



struct PEGASUS_CLIENT_LINKAGE ClientOpPerformanceData
{
   /** Identifies operation type for statistical information being
       provided
    */
    CIMOperationType operationType;

    /** serverTimeKnown is true if the CIM server has sent back
    a valid WBEMServerResponseTime header field.
    */
    Boolean serverTimeKnown;

    /** serverTime is the number of micro seconds the CIM Server
     has taken to process request.
    */
    Uint64 serverTime;

    /** roundTripTime is the number of micro seconds a request/response
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

     

/** A sub-class of the ClientOpPerformanceDataHandler class is registered by 
the client app. This sub-class should have the same scope as the CIMClient object
that is registering it. 
*/                                                                           
class PEGASUS_CLIENT_LINKAGE ClientOpPerformanceDataHandler
{
public:

    /**Callback method held by the ClientOpPerformanceDataHandler class. It is 
    called by the CIMClient library for each operation performed against the 
    server. The client application implements this method in a derived class 
    of this class, and does whatever it wants to do with the 
    raw data item it gets as an argument. For instance, it can accumulate the 
    single values to allow calculating the average across a number of operations.
    Exceptions thrown by this method are not handled by the CIMClient code. The
    exception will appear to the client application and causes the server respones 
    information not to be delivered. 
    @param item The client statistics data item being delivered.
    */
    
    virtual void handleClientOpPerformanceData (const ClientOpPerformanceData & item) = 0;
}; 


/*

The following example shows how the callback function can be used by a client application


//   An implementation of a client statistics catcher that prints all the data in the 
//   ClientOpPerformanceData object

class ClientStatisticsAccumulator : public ClientOpPerformanceDataHandler
{
public:

    virtual void handleClientOpPerformanceData (const ClientOpPerformanceData & item)
        {
            cout << "This is the client app talking" << endl;
            cout << "operationType is " << (Uint32)item.operationType << endl;
            cout << "serverTime is " << (Uint32)item.serverTime << endl;
            cout << "roundTripTime is " << (Uint32)item.roundTripTime << endl;
            cout << "requestSize is " << (Uint32)item.requestSize << endl;
            cout << "responseSize is " << (Uint32)item.responseSize << endl;
            if (item.serverTimeKnown) {
                cout << "serverTimeKnow is true" << endl;
            }
            else{
                cout << "serverTimeKnow is false" << endl;
            }
        }
};




int main(int argc, char** argv)
{
   // Establish the namespace from the input parameters
   String nameSpace = "root/cimv2";

   //Get hostname
   String location = "localhost";

   //Get port number
   Uint32 port = 5988;

   //Get user name and password
   String userN = String::EMPTY;
   String passW = String::EMPTY;
   

   //The next sectoin of code connects to the server

   String className = "PG_ComputerSystem";
   CIMClient client;
                                          //NOTE: ClientStatisticsAccumulator variable must have the same 
                                          //      scope as the CIMClient variable    
   ClientStatisticsAccumulator accumulator = ClientStatisticsAccumulator();

   try
   {
      client.connect(location, port, userN, passW);
   }

   catch (Exception& e)
   {
      cerr << argv[0] << " Exception connecting to : " << location << endl;
      cerr << e.getMessage() << endl;
      exit(1);
   }
                
    Array<CIMObjectPath> instances;

   ///////////////////////////////////////////////////
   // Register callback and EnumerateInstances 
   /////////////////////////////////////////////////////
     
   client.registerClientOpPerformanceDataHandler(accumulator);

     
   //issue command to use callback  
        
   try
   {     
      instances = client.enumerateInstanceNames(nameSpace,
                                                className);
   }
   catch (Exception& e)
   {
      cerr << "Exception : " << e.getMessage() << endl;
      exit(1);
   }    


   return 0;
}

*/


PEGASUS_NAMESPACE_END

#endif /* ClientOpPerformanceDataHandler_h */


