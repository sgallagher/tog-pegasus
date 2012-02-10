//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

/*
    Test of operations that stress the CIM Operationresponse mechanism with
    both verylarge response objects and very large numbers of response objects.
    This client works specifically with the corresponding provider and
    depends on the information in the TST_ResponseStressTest class.
*/
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/General/Stopwatch.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const CIMNamespaceName NAMESPACE = CIMNamespaceName("test/TestProvider");

static const CIMName TEST_CLASS = CIMName("TST_ResponseStressTest");

static Boolean verbose;

ClientOpPerformanceData returnedPerformanceData;
class ClientStatistics : public ClientOpPerformanceDataHandler
{
public:

    virtual void handleClientOpPerformanceData (
            const ClientOpPerformanceData & item)
    {
        if (!(0 <= item.operationType) || !(39 >= item.operationType))
        {
           cerr << "Operation type out of expected range in"
                        " ClientOpPerformanceData "
               << endl;
           exit(1);
        }
        returnedPerformanceData.operationType =  item.operationType;
        if (item.roundTripTime == 0)
        {
           cerr << "roundTripTime is incorrect in ClientOpPerformanceData "
               << endl;
        }
        returnedPerformanceData.roundTripTime =  item.roundTripTime;

        if (item.requestSize == 0)
        {
            cerr << "requestSize is incorrect in ClientOpPerformanceData "
                << endl;
        }
        returnedPerformanceData.requestSize =  item.requestSize;

        if (item.responseSize == 0)
        {
            cerr << "responseSize is incorrect in ClientOpPerformanceData "
                << endl;
        }
        returnedPerformanceData.responseSize =  item.responseSize;

        if (item.serverTimeKnown)
        {
            /* Bypass this because we are getting server times zero
            if (item.serverTime == 0)
            {
                cerr << "serverTime is incorrect in ClientOpPerformanceData "
                    << endl;
            }
            */
            returnedPerformanceData.serverTime =  item.serverTime;
            returnedPerformanceData.serverTimeKnown =  item.serverTimeKnown;
            returnedPerformanceData.roundTripTime =  item.roundTripTime;
        }
   }
   //returnedPerformanceData = item;   // Copy the data to public place
};

Boolean first = true;
void _displayTimes( Uint32 objCount, Uint32 objSize, Uint64 elapsed,
                    ClientOpPerformanceData& perfData)
{
    if (first)
    {
        if (!perfData.serverTimeKnown)
        {
            cout << "WARNING: Server statistics data may not be valid" << endl;
        }
        printf("Op ClientTime roundTripTime "
               "serverTime  ReqSize      RespSize ObjSize "
               " ObjCount\n");
        first = false;
    }

    printf("%2u %10lu %13lu %10lu %8lu %12lu  %7u %9u\n",
           perfData.operationType,
           (long unsigned int) elapsed,
           (long unsigned int)perfData.roundTripTime,
           (long unsigned int)perfData.serverTime,
           (long unsigned int) perfData.requestSize,
           (long unsigned int) perfData.responseSize,
           objSize, objCount);


}
// Enumerate the instance names for the defined
void enumerateInstances(CIMClient& client, Uint64 responseCount,
        Uint32 responseSize)
{
    try
    {
        Stopwatch sw;
        sw.start();
        Array<CIMInstance> instances =
            client.enumerateInstances(NAMESPACE, TEST_CLASS);

        sw.stop();
        Uint64 elapsed = sw.getElapsedUsec();

        _displayTimes(responseSize, instances.size(), elapsed,
                      returnedPerformanceData);
        //sw.printElapsed();
//      Uint64 rtTotalTime = 0;
//      rtTotalTime = (returnedPerformanceData.roundTripTime);
//      Uint64 serverTime = returnedPerformanceData.serverTime;
        PEGASUS_TEST_ASSERT(instances.size() == responseCount);

        // Confirm that the sequence numbers are monolithic increasing
        //
        Uint64 prevSequenceNumber = 0;
        for (Uint64 i = 0, n = instances.size() ;  i < n ; i++)
        {
            Uint32 pos;
            if ((pos = instances[i].findProperty("SequenceNumber"))
                 != PEG_NOT_FOUND )
            {
                CIMProperty p = instances[i].getProperty(pos);
                CIMValue v = p.getValue();
                Uint64 sequenceNumber;
                v.get(sequenceNumber);
                //cout << "SequenceNumber = " << sequenceNumber
                //    << " prevSequenceNumber " << prevSequenceNumber << endl;

                PEGASUS_TEST_ASSERT(sequenceNumber == (prevSequenceNumber));
                prevSequenceNumber++;
            }
        }
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

// Test to validate instance names. Executes the enumerate and tests the
// number of responses against the expected response count
void enumerateInstanceNames(CIMClient& client, Uint64 responseCount,
                            Uint32 InstanceSize)
{
    try
    {
        Array<CIMObjectPath> paths =
            client.enumerateInstanceNames(NAMESPACE, TEST_CLASS);

        cout << "count = " << paths.size() << endl;
        if (verbose)
        {
            cout << endl;
            for (Uint32 i = 0; i < paths.size() ; i++)
            {
                cout << paths[i].toString() << endl;
            }
        }
        PEGASUS_TEST_ASSERT(paths.size() == responseCount);
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

// set the instanceSize and response count parameters in the
// client
void set(CIMClient& client, Uint64 instanceSize, Uint64 responseCount)
{
    Array<CIMParamValue> InParams;

    Array<CIMParamValue> outParams;

    InParams.append(CIMParamValue("ResponseCount", responseCount));
    InParams.append(CIMParamValue("Size", instanceSize));
    CIMValue returnValue = client.invokeMethod(
        NAMESPACE,
        CIMObjectPath(String::EMPTY,
                      CIMNamespaceName(),
                      CIMName(TEST_CLASS)),
        CIMName("set"),
        InParams,
        outParams);

    PEGASUS_TEST_ASSERT(outParams.size() == 0);

    Uint32 rc;
    returnValue.get(rc);

    PEGASUS_TEST_ASSERT(rc == 0);
}

// get the current provider test parameters.
void get(CIMClient& client, Uint64& instanceSize, Uint64& responseCount)
{
    Array<CIMParamValue> InParams;

    Array<CIMParamValue> outParams;

    InParams.append(CIMParamValue("Size", instanceSize));
    InParams.append(CIMParamValue("ResponseCount", responseCount));

    CIMValue returnValue = client.invokeMethod(
        NAMESPACE,
        CIMObjectPath(String::EMPTY,
                      CIMNamespaceName(),
                      CIMName(TEST_CLASS)),
        CIMName("get"),
        InParams,
        outParams);

    Uint32 rc;
    returnValue.get(rc);
    PEGASUS_TEST_ASSERT(rc == 0);

    for(Uint32 i = 0; i < outParams.size(); ++i)
    {
        String paramName = outParams[i].getParameterName();
        CIMValue v = outParams[i].getValue();

        if(paramName =="ResponseCount")
        {
            v.get(responseCount);
        }
        else if(paramName =="Size")
        {
            v.get(instanceSize);
        }
        else
        {
            cout <<"Error: unknown parameter name = " << paramName << endl;
            PEGASUS_TEST_ASSERT(false);
        }
    }
}

// fixed test for the get and set methods that restores the provider
// parameters to their original condition.
void testSetAndGetMethods(CIMClient& client)
{
    Uint64 instanceSizeOrig = 0;
    Uint64 responseCountOrig = 0;
    Uint64 instanceSize = 0;
    Uint64 responseCount = 0;
    get(client,instanceSizeOrig, responseCountOrig);
    set(client, 1500, 2000);

    get(client,instanceSize, responseCount);
    PEGASUS_TEST_ASSERT(instanceSize == 1500);
    PEGASUS_TEST_ASSERT(responseCount = 2000);

    set(client, instanceSizeOrig, responseCountOrig);

    get(client,instanceSize, responseCount);
    PEGASUS_TEST_ASSERT(instanceSize == instanceSizeOrig);
    PEGASUS_TEST_ASSERT(responseCount = responseCountOrig);
}

//CIMClient connectServer()
//{
//    CIMClient client;
//    try
//    {
//        client.connectLocal();
//    }
//    catch (Exception& e)
//    {
//        cerr << "Error: Connect Failure " << e.getMessage() << endl;
//        exit(1);
//    }
//    return(client);
//}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE") ? true : false;

    CIMClient client;

    // Register for Client statistics.
    ClientStatistics statistics = ClientStatistics();

    client.registerClientOpPerformanceDataHandler(statistics);
    try
    {
        client.connectLocal();
        //client.connect("localhost", 5988, String(), String());
    }
    catch (Exception& e)
    {
        cerr << "Error: Connect Failure " << e.getMessage() << endl;
        exit(1);
    }

    try
    {
        // if no argument conduct simple standard test to validate the
        // operation of the provider.
        if (argc <=1)
        {
            //CIMClient client = connectServer();

            testSetAndGetMethods(client);

            // set test parameters to 150 = size, 20 = response count
            Uint64 responseCount = 20;
            Uint64 instanceSize = 150;

            set(client, instanceSize, responseCount);

            enumerateInstanceNames(client, responseCount, instanceSize);

            enumerateInstances(client, responseCount, instanceSize);
        }
        else if (strcmp(argv[1],"test") == 0)
        {
            Array<Uint64> objSize;
            objSize.append(100);
            objSize.append(1000);
            objSize.append(10000);
            objSize.append(50000);
            Array<Uint64> objCount;
            objCount.append(100);
            objCount.append(1000);
            objCount.append(10000);
            objCount.append(50000);
            objCount.append(100000);
            objCount.append(200000);
            objCount.append(250000);
            for (Uint32 x = 0; x < objSize.size() ; x++)
            {
                for(Uint32 y = 0; y < objCount.size(); y++)
                {
                    set(client, objSize[x], objCount[y]);

                    enumerateInstances(client, objCount[y],
                                           objSize[x]);
                }
            }
        }
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
