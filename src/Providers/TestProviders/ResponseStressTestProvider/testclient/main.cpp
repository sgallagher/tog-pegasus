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
    Test of  the ResponseStress Provider with operations that stress the
    CIM Operationresponse mechanism with both verylarge response objects and
    very large numbers of response objects.
    This client works specifically with the corresponding provider and
    depends on the information in the TST_ResponseStressTest class.
    It is only a test of the providers capability to execute its options.
*/
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/General/Stopwatch.h>
#include <Pegasus/Common/Pegasus_inl.h>
#include <Pegasus/General/Stopwatch.h>
#include <cstdarg>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define VCOUT if (verbose) cout

static const CIMNamespaceName NAMESPACE = CIMNamespaceName("test/TestProvider");

static const CIMName TEST_CLASS = CIMName("TST_ResponseStressTestCxx");

static Boolean verbose;

/*
    This structure encapsulates all of the parameters for the get and
    set methods for the ResponseStressTest Class with functions for
    setting, getting, displaying  and comparing them.
*/
typedef struct MethodParameters{
    MethodParameters() :  _responseCount(0),_instanceSize(0),
        _countToFail(0), _failureStatusCode(CIM_ERR_SUCCESS), _delay(0)
    {}

    // Test with responseCount and instanceSize required parameters
    // and others optional
    MethodParameters(Uint64 responseCount, Uint64 instanceSize,
        Uint64 countToFail = 0, Uint32 failureStatusCode = 0, Uint32 delay = 0)
       :  _responseCount(responseCount),
        _instanceSize(instanceSize),
        _countToFail(countToFail),
        _failureStatusCode((CIMStatusCode)failureStatusCode),
        _delay(delay) {}

    MethodParameters(Uint64 responseCount, Uint64 instanceSize,
        Uint64 countToFail, CIMStatusCode failureStatusCode, Uint32 delay = 0)
       :  _responseCount(responseCount),
        _instanceSize(instanceSize),
        _countToFail(countToFail),
        _failureStatusCode(failureStatusCode),
        _delay(delay) {}

    bool equal(const MethodParameters& mp)
    {
        if (_responseCount != mp._responseCount)
        {
            VCOUT << "Compare fail responseCount" << endl;
            return false;
        }
        else if(_instanceSize != mp._instanceSize)
        {
            VCOUT << "Compare fail instanceSize" << endl;
            return false;
        }
        else if(_countToFail != mp._countToFail)
        {
            VCOUT << "Compare fail countToFail" << endl;
            return false;
        }
        else if(_failureStatusCode != mp._failureStatusCode)
        {
            VCOUT << "Compare fail failureStatusCode" << endl;
            return false;
        }
        else if (_delay != mp._delay)
        {
            VCOUT << "Compare Delay parmaeters" << endl;
            return false;
        }
        return true;
    }

    void setResponseCount(Uint64 x)
    {
        _responseCount = x;
    }
    void setCountToFail(Uint64 x)
    {
        _countToFail = x;
    }
    void setFailureStatusCode(CIMStatusCode x)
    {
        _failureStatusCode = x;
    }
    void setFailureStatusCode(Uint32 x)
    {
        _failureStatusCode = (CIMStatusCode)x;
    }

    void setDelay(Uint32 x)
    {
        _delay = x;
    }
    String toString()
    {
        String rtn;
        rtn.appendPrintf("responseCount=%llu instanceSize=%llu "
            "countToFail=%llu _failureStatusCond=%u (%s) _delay=%u",
            _responseCount, _instanceSize, _countToFail,
            (Uint32)_failureStatusCode,
            cimStatusCodeToString(_failureStatusCode),
            _delay);
        return rtn;
    }

    Uint64 _responseCount;
    Uint64 _instanceSize;
    Uint64 _countToFail;
    CIMStatusCode _failureStatusCode;
    Uint32 _delay;

} methodParameters;

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
void enumerateInstances(CIMClient& client, const methodParameters& mp)
{
    Stopwatch sw;
    sw.start();
    Array<CIMInstance> instances =
        client.enumerateInstances(NAMESPACE, TEST_CLASS);

    sw.stop();
    Uint64 elapsed = sw.getElapsedUsec();

    _displayTimes(mp._responseCount, instances.size(), elapsed,
                  returnedPerformanceData);
    //sw.printElapsed();
//      Uint64 rtTotalTime = 0;
//      rtTotalTime = (returnedPerformanceData.roundTripTime);
//      Uint64 serverTime = returnedPerformanceData.serverTime;
    PEGASUS_TEST_ASSERT(instances.size() == mp._responseCount);

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

// Test to validate instance names. Executes the enumerate and tests the
// number of responses against the expected response count
void enumerateInstanceNames(CIMClient& client, const methodParameters& mp)
{
    Array<CIMObjectPath> paths =
        client.enumerateInstanceNames(NAMESPACE, TEST_CLASS);

    VCOUT << "count = " << paths.size() << endl;
    if (verbose)
    {
        cout << endl;
        for (Uint32 i = 0; i < paths.size() ; i++)
        {
            cout << paths[i].toString() << endl;
        }
    }
    PEGASUS_TEST_ASSERT(paths.size() == mp._responseCount);
}

// set the instanceSize and response count parameters in the
// client
void set(CIMClient& client, const methodParameters& mp)
{
    Array<CIMParamValue> InParams;

    Array<CIMParamValue> outParams;

    InParams.append(CIMParamValue("ResponseCount", mp._responseCount));
    InParams.append(CIMParamValue("Size", mp._instanceSize));
    InParams.append(CIMParamValue("CountToFail", mp._countToFail));
    InParams.append(CIMParamValue("FailureStatusCode",
        (Uint32)mp._failureStatusCode));
    InParams.append(CIMParamValue("Delay", mp._delay));

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

// Issue the reset invoke method operation and wait for good response.
void reset(CIMClient& client)
{
    Array<CIMParamValue> InParams;
    Array<CIMParamValue> outParams;

    CIMValue returnValue = client.invokeMethod(
        NAMESPACE,
        CIMObjectPath(String::EMPTY,
            CIMNamespaceName(),
            CIMName(TEST_CLASS)),
        CIMName("reset"),
        InParams,
        outParams);

    Uint32 rc;
    returnValue.get(rc);
    PEGASUS_TEST_ASSERT(rc == 0);
}
// get the current provider test parameters.
void get(CIMClient& client, methodParameters& mp)
{
    Array<CIMParamValue> InParams;
    Array<CIMParamValue> outParams;

//  InParams.append(CIMParamValue("Size", instanceSize));
//  InParams.append(CIMParamValue("ResponseCount", responseCount));

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
            v.get(mp._responseCount);
        }
        else if(paramName =="Size")
        {
            v.get(mp._instanceSize);
        }
        else if(paramName =="CountToFail")
        {
            v.get(mp._countToFail);
        }
        else if(paramName =="FailureStatusCode")
        {
            Uint32 x;
            v.get(x);
            mp._failureStatusCode = (CIMStatusCode)x;
        }
        else if(paramName =="Delay")
        {
            v.get(mp._delay);
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
    // This should match the definition in the provider as compiled
    methodParameters orig(5, 100, 0, 0, 0);
    VCOUT << "orig = " << orig.toString() << endl;

    methodParameters rtn;
    get(client, rtn);
    VCOUT << "getRtn = " << rtn.toString() << endl;

    // This confirms that our orig is really the provider defaults.
    if (!rtn.equal(orig))
    {
        cerr << "Error in test of default method parameters"
            << " expected " << orig.toString()
            << " received " << rtn.toString()
            << ". Difference Ignored" << endl;
    }

    // Test changes to count and size
    methodParameters test1(1500, 2000);
    set(client, test1);

    methodParameters test1Rtn = test1;
    get(client, test1Rtn);
    PEGASUS_TEST_ASSERT(test1.equal(test1Rtn));

    // set back to origin
    set(client, orig);

    methodParameters test2Rtn;
    get(client, test2Rtn);
    PEGASUS_TEST_ASSERT(test2Rtn.equal(orig));

    // assumes provider set to orig.
    methodParameters testFailureParams = orig;
    testFailureParams.setFailureStatusCode(CIM_ERR_FAILED);
    testFailureParams.setCountToFail(4);

    set(client, testFailureParams);
    methodParameters testFailureParamsRtn = testFailureParams;
    get(client, testFailureParamsRtn);
    PEGASUS_TEST_ASSERT(testFailureParams.equal(testFailureParamsRtn));

    // reset to the original parameters.
    set(client, orig);

    get(client, rtn);
    VCOUT << "getRtn = " << rtn.toString() << endl;

    // test setting delay parameter
    methodParameters testDelayParams = orig;
    testDelayParams.setDelay(10);
    testDelayParams.setResponseCount(290);

    methodParameters testDelayParamsRtn = testDelayParams;
    set(client, testDelayParams);
    get(client,testDelayParamsRtn);
    PEGASUS_TEST_ASSERT(testDelayParamsRtn._delay == 10);
    VCOUT << "testDelayParams" << testDelayParamsRtn.toString() << endl;
    PEGASUS_TEST_ASSERT(testDelayParams.equal(testDelayParamsRtn));

    // test if this actually delays. Delay should be about 10 seconds.
    VCOUT << "Expect a delay of about 20 sec here" << endl;
    Stopwatch timer;

    timer.start();
    enumerateInstanceNames(client, testDelayParams);
    timer.stop();

    // test if delay gt 9 sec.
    PEGASUS_TEST_ASSERT(timer.getElapsed() > (double)9);

    // This confirms that our orig is really the provider defaults.

    // reset to the original parameters.
    set(client, orig);
    get(client, rtn);
    VCOUT << "getRtn = " << rtn.toString() << endl;
    PEGASUS_TEST_ASSERT(rtn.equal(orig));

    VCOUT << "Passed testSetAndGetMethods." << endl;
}

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
            methodParameters mp(20,150);

            set(client, mp);

            enumerateInstanceNames(client, mp);

            enumerateInstances(client, mp);
        }
        // This is a real stress test and should be only used for manual
        // test, not the nightly tests.
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
                    methodParameters mp(objSize[x], objCount[y]);
                    set(client, mp);

                    enumerateInstances(client, mp);
                }
            }
        }

    }
    catch (Exception& e)
    {
        cerr << "Error: Connect or standard tests " << e.getMessage() << endl;
        exit(1);
    }

    VCOUT << "Start Error reponse Tests. " << endl;
    // test to confirm that the countToFailure and failureStatusCode
    // work. This definition should fail after 30 objects returned.
    methodParameters mpFail(20,150,10, CIM_ERR_FAILED);

    set(client, mpFail);
    try
    {
        enumerateInstances(client, mpFail);
        PEGASUS_TEST_ASSERT(false);    // should not reach here.
    }
    catch (CIMException& e)
    {
        if (e.getCode() != CIM_ERR_FAILED)
        {
            cout << "ERROR: expected CIM_ERR_FAILED. got " << e.getCode()
                << e.getMessage() << endl;
            PEGASUS_TEST_ASSERT(false);
        }
    }

    try
    {
        enumerateInstanceNames(client, mpFail);
        PEGASUS_TEST_ASSERT(false);
    }
    catch (CIMException& e)
    {
        if (e.getCode() != CIM_ERR_FAILED)
        {
            cout << "ERROR: expected CIM_ERR_FAILED. got " << e.getCode()
                << e.getMessage() << endl;
            PEGASUS_TEST_ASSERT(false);
        }
    }

    mpFail.setCountToFail(155);    // Should not fail
    set(client, mpFail);

    try
    {
        enumerateInstanceNames(client, mpFail);
    }
    catch (CIMException& e)
    {
        cout << "ERROR: Count should be greater than number of resposnes.got "
             << e.getCode() << e.getMessage() << endl;
        PEGASUS_TEST_ASSERT(false);
    }

    // reset provider to default in case another test.
    reset(client);
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
