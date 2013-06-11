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
//%////////////////////////////////////////////////////////////////////////////

/*
    These tests are part of a test program rather than a fixed test because
    a great number of them depend on sequences of operations in the server
    rather than single operations and cannot be tested with fixed comparison
    tests.

    Tests the error responses for pull operations.  This tests the various
    requests that will generate exception error including:
    1. Inclusion of Filter and Filter Language parameters - We do not expect
       use of these parameters today and so block them in the server
    2. Execute Pull or Close operations when there is no Open context.
    3. Bad context on Pull for an open enumeration.  We change the context ID.
    TBD.
    4. Null max object count
    5. Continue on error set - We do not allow this today
*/

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Client/CIMClient.h>

#include <Pegasus/Common/StringConversion.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static Boolean verbose;
#define VCOUT if (verbose) cout

#define ENTRY VCOUT << "Enter "

/*
    Simple pattern match. Pattern may include *
*/

static int _match(const char* pattern, const char* str)
{
    const char* p;
    const char* q;

    /* Now match expression to str. */

    for (p = pattern, q = str; *p && *q; )
    {
        if (*p == '*')
        {
            const char* r;

            p++;

            /* Recursively call to find the shortest match. */

            for (r = q; *r; r++)
            {
                if (_match(p, r) == 0)
                    break;
            }

            q = r;

        }
        else if (*p == *q)
        {
            p++;
            q++;
        }
        else
            return -1;
    }

    /* If src was exhausted but pattern has a single '*' remaining charcters,
     * then match the result.
     */

    if (p[0] == '*' && p[1] == '\0')
        return 0;

    /* If anything left over, then they do not match. */

    if (*p || *q)
        return -1;

    return 0;
}

static int _Match(const String& pattern, const String& str)
{
    return _match(pattern.getCString(), str.getCString());
}

/* Test filterQuery and filterQueryLanguage error responses.
   Should return CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED CIMException
*/
void testFilterErrorResponse(CIMClient& client)
{
    ENTRY << "testFilterErrorResponse" << endl;
    CIMNamespaceName nameSpace = "test/TestProvider";
    CIMName className = "TST_Person";
    CIMObjectPath  cimObjectName("TST_Person.name=\"Mike\"");
    Boolean deepInheritance = true;
    Boolean includeClassOrigin = false;
    Boolean endOfSequence = false;
    Uint32 operationTimeout = 0;
    Boolean continueOnError = false;
    Uint32 maxObjectCount = 9;
    String filterQueryLanguage = "SomeString";
    String filterQuery = "SomeString";
    Array<CIMInstance> cimInstances;
    Array<CIMObjectPath> cimInstancePaths;

    try
    {
        CIMEnumerationContext enumerationContext;
        cimInstances = client.openEnumerateInstances(
            enumerationContext,
            endOfSequence,
            nameSpace,
            className,
            deepInheritance,
            includeClassOrigin,
            CIMPropertyList(),
            filterQueryLanguage,
            filterQuery,
            operationTimeout,
            continueOnError,
            maxObjectCount
            );
        PEGASUS_TEST_ASSERT(false);
    }

    catch (CIMException& e)
    {
        if (e.getCode() != CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED )
        {
            cerr << "CIMException Error: filterErrorResponse "
                << e.getMessage() << endl;
            PEGASUS_TEST_ASSERT(false);
        }
        else
        {
            VCOUT << "filterParameter Error test. Correct exception received"
                  << endl;
        }
    }

    try
    {
        CIMEnumerationContext enumerationContext;
        cimInstancePaths = client.openEnumerateInstancePaths(
            enumerationContext,
            endOfSequence,
            nameSpace,
            className,
            filterQueryLanguage,
            filterQuery,
            operationTimeout,
            continueOnError,
            maxObjectCount
            );
        PEGASUS_TEST_ASSERT(false);
    }
    catch (CIMException& e)
    {
        if (e.getCode() != CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED )
        {
            cerr << "CIMException Error: filterErrorResponse "
                << e.getMessage() << endl;
            PEGASUS_TEST_ASSERT(false);
        }
        else
        {
            VCOUT << "filterParameter Error test. Correct exception received"
                << endl;
        }
    }

    // Test Filter parameters on openReferenceInstances
    String role = "";
    CIMName resultClass = CIMName();
    try
    {
        Boolean includeClassOrigin = false;
        Boolean endOfSequence = false;
        Uint32Arg operationTimeout(0);
        Boolean continueOnError = false;

        CIMEnumerationContext enumerationContext;
        cimInstances = client.openReferenceInstances(
            enumerationContext,
            endOfSequence,
            nameSpace,
            cimObjectName,
            resultClass,
            role,
            includeClassOrigin,
            CIMPropertyList(),
            filterQueryLanguage,
            filterQuery,
            operationTimeout,
            continueOnError,
            maxObjectCount
            );
        PEGASUS_TEST_ASSERT(false);
        }
        catch (CIMException& e)
        {
            if (e.getCode() != CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED )
            {
                cerr << "CIMException Error: filterErrorResponse "
                    << e.getMessage() << " code = " << e.getCode() << endl;
                PEGASUS_TEST_ASSERT(false);
            }
        }

        try
        {
            Boolean endOfSequence = false;
            Uint32Arg operationTimeout(0);
            Boolean continueOnError = false;

            CIMEnumerationContext enumerationContext;
            cimInstancePaths = client.openReferenceInstancePaths(
                enumerationContext,
                endOfSequence,
                nameSpace,
                cimObjectName,
                resultClass,
                role,
                filterQueryLanguage,
                filterQuery,
                operationTimeout,
                continueOnError,
                maxObjectCount
                );
            PEGASUS_TEST_ASSERT(false);
        }
        catch (CIMException& e)
        {
            if (e.getCode() != CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED )
            {
                cerr << "CIMException Error: filterErrorResponse "
                    << e.getMessage() << " code = " << e.getCode() << endl;
                PEGASUS_TEST_ASSERT(false);
            }
        }

        String resultRole = "";
        CIMName assocClass = CIMName();

        try
        {
            Boolean includeClassOrigin = false;
            Boolean endOfSequence = false;
            Uint32Arg operationTimeout(0);
            Boolean continueOnError = false;

            CIMEnumerationContext enumerationContext;
            cimInstances = client.openAssociatorInstances(
                enumerationContext,
                endOfSequence,
                nameSpace,
                cimObjectName,
                assocClass,
                resultClass,
                role,
                resultRole,
                includeClassOrigin,
                CIMPropertyList(),
                filterQueryLanguage,
                filterQuery,
                operationTimeout,
                continueOnError,
                maxObjectCount
            );
            PEGASUS_TEST_ASSERT(false);
            }
            catch (CIMException& e)
            {
                if (e.getCode() != CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED )
                {
                    cerr << "CIMException Error: filterErrorResponse "
                        << e.getMessage() << " code = " << e.getCode() << endl;
                    PEGASUS_TEST_ASSERT(false);
                }
            }

            try
            {
                Boolean endOfSequence = false;
                Uint32Arg operationTimeout(0);
                Boolean continueOnError = false;

                CIMEnumerationContext enumerationContext;
                cimInstancePaths = client.openAssociatorInstancePaths(
                    enumerationContext,
                    endOfSequence,
                    nameSpace,
                    cimObjectName,
                    assocClass,
                    resultClass,
                    role,
                    resultRole,
                    filterQueryLanguage,
                    filterQuery,
                    operationTimeout,
                    continueOnError,
                    maxObjectCount
                    );
                PEGASUS_TEST_ASSERT(false);
                }
                catch (CIMException& e)
                {
                    if (e.getCode() !=
                        CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED )
                    {
                        cerr << "CIMException Error: filterErrorResponse "
                            << e.getMessage() << " code = "
                            << e.getCode() << endl;
                        PEGASUS_TEST_ASSERT(false);
                    }
                }
}
/* Test Error responses for the pull, and close operations with no open
*/
void testEnumContextError(CIMClient& client)
{
    ENTRY << "testEnumContextError" << endl;
    try
    {
        Boolean endOfSequence = false;
        Uint32Arg operationTimeout = 0;
        Uint32 maxObjectCount = 9;
        String filterQueryLanguage = String::EMPTY;
        String filterQuery = String::EMPTY;
        Array<CIMInstance> cimInstances;
        Array<CIMObjectPath> cimInstancePaths;

        CIMEnumerationContext enumerationContext;

        // test for invalid context responses.
        // In our case, we normally get invalid namespace.

        try
        {
            CIMEnumerationContext enumerationContext;
            cimInstances = client.pullInstancesWithPath(
                enumerationContext,
                endOfSequence,
                maxObjectCount);
            VCOUT << "OK: CIMException Error Expected for this operation"
                << endl;
            PEGASUS_TEST_ASSERT(false);
        }
        catch (CIMException& e)
        {
                cerr << "CIMException Error:"
                        " testEnumContextError PullInstancesWithPath "
                    << e.getMessage() << endl;
                PEGASUS_TEST_ASSERT(false);
        }

        // KS_TODO - Currently the error generated is HTTP ERROR 400
        // Temporarily blocked assert
        catch (Exception& e)
        {
            if (e.getMessage() != "Invalid Enumeration Context, unitilialized")
            {
                cerr << "Exception Error: in testEnumContextError Test "
                    << e.getMessage() << endl;
                //// KS_TODO PEGASUS_TEST_ASSERT(false);
            }
        }
        VCOUT << "PullInstancesWithPath context err OK" << endl;

        // test pull with no open. Should return CIMException
        try
        {
            CIMEnumerationContext enumerationContext;
            Array<CIMObjectPath> xx = client.pullInstancePaths(
                enumerationContext,
                endOfSequence,
                maxObjectCount);

            VCOUT << "CIMException Error Expected for this operation" << endl;
            //// KS_TODO PEGASUS_TEST_ASSERT(false);
        }

        catch (CIMException& e)
        {
            cerr << "CIMException Error: " << e.getMessage() << endl;
            PEGASUS_TEST_ASSERT(false);
        }

        catch (Exception& e)
        {
            cerr << "Exception: " << e.getMessage() << endl;
            if (e.getMessage() != "Invalid Enumeration Context, unitilialized")
            {
                cerr << "Error: in testEnumContextError Test "
                    << e.getMessage() << endl;
                //// KS_TODO PEGASUS_TEST_ASSERT(false);
            }
        }
        VCOUT << "PullInstancePaths context err OK" << endl;

        // Test close operation with no open
        try
        {
            CIMEnumerationContext enumerationContext;
            client.closeEnumeration(
                enumerationContext);
            cerr << "CIMException Error Expected for this operation" << endl;
            PEGASUS_TEST_ASSERT(false);
        }

        catch (CIMException& e)
        {
            cerr << "CIMException Error: " << e.getMessage() << endl;
            PEGASUS_TEST_ASSERT(false);
        }

        catch (Exception& e)
        {
            if (e.getMessage() != "Invalid Enumeration Context, unitilialized" )
            {
                cerr << "Error: in testEnumContextError Test "
                     << e.getMessage() << endl;
                //// KS_TODO PEGASUS_TEST_ASSERT(false);
            }
        }

        VCOUT << "CloseEnumeration context err OK" << endl;
        /**
         *     Test EnumerationCount
         */
        try
        {
            CIMEnumerationContext enumerationContext;
            Uint64Arg x = client.enumerationCount(
                enumerationContext );
            PEGASUS_TEST_ASSERT(false);
        }
        catch (CIMException& e)
        {
            cerr << "CIMException Error: " << e.getMessage() << endl;
            PEGASUS_TEST_ASSERT(false);
        }

        catch (Exception& e)
        {
            if (e.getMessage() != "Invalid Enumeration Context, unitilialized")
            {
                cerr << "Error: in testEnumContextError Test "
                     << e.getMessage() << endl;
                //// KS_TODO PEGASUS_TEST_ASSERT(false);
            }
        }

        VCOUT << "enumerationCount context err OK" << endl;
    }
    catch (CIMException& e)
    {
        cerr << "CIMException Error: in testEnumContextError "
             << e.getMessage() << endl;
        PEGASUS_TEST_ASSERT(false);
    }
    catch (Exception& e)
    {
        cerr << "Error: in testEnumContextError Test "
             << e.getMessage() << endl;
        //// KS_TODO PEGASUS_TEST_ASSERT(false);
    }

    VCOUT << "testPullAndCloseEnumContextError passed" << endl;
}

/*
    General class to test all of the different operations with various
    error scenarios.  The parameters are supplied once and each of the
    various opens can then be tested with the same set of parmeters.
    This avoids the duplicated test code for each open operation and
    each error scenario.
*/
struct testCalls{
    CIMEnumerationContext _enumerationContext;
    CIMNamespaceName _nameSpace;
    CIMName _className;
    CIMObjectPath _cimObjectName;
    Boolean _deepInheritance;
    Boolean _includeClassOrigin;
    Boolean _endOfSequence;
    Uint32 _operationTimeout;
    Boolean _continueOnError;
    Uint32 _maxObjectCount;

    String _filterQueryLanguage;
    String _filterQuery;

    String _role;
    String _resultRole;
    CIMName _assocClass;
    CIMName _resultClass;
    CIMPropertyList _cimPropertyList;

    // Parameters associated with Pulls
    Boolean _expectPullReturnGood;
    CIMClient& _client;

    // Flags on tests for returns
    Boolean _expectOpenReturnGood;
    CIMStatusCode _expectedOpenCIMExceptionCode;
    String _expectedOpenCIMExceptionMessage;

    // Names for the test and the operation being tested
    String _testName;
    String _operationName;

    //  Constructor sets up defaults for all parameters.
    testCalls(CIMClient& client, const char*nameSpace):
        _nameSpace(nameSpace),
        _className(),
        _cimObjectName(),
        _deepInheritance(true),
        _includeClassOrigin(false),
        _operationTimeout(0),
        _continueOnError(false),
        _maxObjectCount(100),
        _filterQueryLanguage(""),
        _filterQuery(""),
        _role(""),
        _resultRole(""),
        _assocClass(CIMName()),
        _resultClass(CIMName()),
        _cimPropertyList(CIMPropertyList()),
        _expectPullReturnGood(true),
        _client(client),
        _expectOpenReturnGood(true),
        _expectedOpenCIMExceptionCode(CIM_ERR_SUCCESS),
        _expectedOpenCIMExceptionMessage(""),
        _testName("unknown"),
        _operationName("unknown")
    {}

    void setClientParams(CIMObjectPath& path,
                    Boolean deepInheritance,
                    Boolean includeClassOrigin)
    {
        _cimObjectName = path;
        _deepInheritance = deepInheritance;
        _includeClassOrigin = includeClassOrigin;
    }
    void setTestName(String testName)
    {
        _testName = testName;
    }

    void setCIMException(CIMStatusCode code)
    {
        _expectedOpenCIMExceptionCode = code;
    }
    void setCIMExceptionMessage(const char * msg)
    {
        _expectedOpenCIMExceptionMessage = msg;
    }

    // KS_TODo this can be deleted as not used.
    void setDefaultTarget()
    {
        _nameSpace =  "test/TestProvider";
        _className = "TST_Person";
    }

    void startOperationTest(const char* operationName)
    {
        _operationName = operationName;
        VCOUT << "Start "<< _testName << " " << _operationName << endl;
    }

    void continueOperationTest(const char* operationName)
    {
        _operationName = operationName;
        VCOUT << "Start "<< _testName << " " << _operationName
              << "/" << operationName <<endl;
    }

    void displayEnd()
    {
        VCOUT << "End " << _testName << " " << _operationName << endl;
    }
    void testCIMException(CIMException& e)
    {
        if (e.getCode() != _expectedOpenCIMExceptionCode)
        {
            cerr << "CIMException Error: " << e.getCode() << " "
                << e.getMessage() << endl;
            PEGASUS_TEST_ASSERT(false);
        }
        else
        {
            VCOUT << "Correct Exception Code received." << _testName
                  << " " << _operationName << " Exception "
                  << e.getCode() << " " << e.getMessage() << endl;

            // test for correct message if required (i.e. test string not
            // empty

            if (_expectedOpenCIMExceptionMessage != "")
            {
                if (_Match(_expectedOpenCIMExceptionMessage,
                           e.getMessage()) != 0 )
                {
                    cerr << "Received CIMException Message Error: |"
                         << e.getMessage()
                         << "| does not match expected CIMException |"
                         << _expectedOpenCIMExceptionMessage << "|" << endl;
                    PEGASUS_TEST_ASSERT(false);
                }
            }

        }
    }

    Boolean openEnumerateInstances()
    {
        startOperationTest("openEnumerateInstances");

        try
        {
            Array<CIMInstance> cimInstances = _client.openEnumerateInstances(
                _enumerationContext,
                _endOfSequence,
                _nameSpace,
                _className,
                _deepInheritance,
                _includeClassOrigin,
                _cimPropertyList,
                _filterQueryLanguage,
                _filterQuery,
                _operationTimeout,
                _continueOnError,
                _maxObjectCount
                );
            PEGASUS_TEST_ASSERT(_expectOpenReturnGood);
        }
        catch (CIMException& e)
        {
            testCIMException(e);
        }
        displayEnd();
        return true;
    }


    Boolean openEnumerateInstancePaths()
    {

        startOperationTest("openEnumerateInstancePaths");
        CIMEnumerationContext enumerationContext;
        try
        {
            Array<CIMObjectPath> cimPaths =
                _client.openEnumerateInstancePaths(
                _enumerationContext,
                _endOfSequence,
                _nameSpace,
                _className,
                _filterQueryLanguage,
                _filterQuery,
                _operationTimeout,
                _continueOnError,
                _maxObjectCount);
            PEGASUS_TEST_ASSERT(_expectOpenReturnGood);
        }
        catch (CIMException& e)
        {
            testCIMException(e);
        }
        displayEnd();
        return true;
    }

    Boolean openReferenceInstances()
    {
        startOperationTest("openReferenceInstances");

        Array<CIMInstance> cimInstances;
        try
        {
            cimInstances = _client.openReferenceInstances(
                _enumerationContext,
                _endOfSequence,
                _nameSpace,
                _cimObjectName,
                _resultClass,
                _role,
                _includeClassOrigin,
                _cimPropertyList,
                _filterQueryLanguage,
                _filterQuery,
                _operationTimeout,
                _continueOnError,
                _maxObjectCount);
            PEGASUS_TEST_ASSERT(_expectOpenReturnGood);
        }
        catch (CIMException& e)
        {
            testCIMException(e);
        }
        displayEnd();
        return true;
    }

    Boolean openReferenceInstancePaths()
    {
        startOperationTest("openReferenceInstancePaths");

        Array<CIMObjectPath> cimInstancePaths;
        try
        {
            cimInstancePaths = _client.openReferenceInstancePaths(
                _enumerationContext,
                _endOfSequence,
                _nameSpace,
                _cimObjectName,
                _resultClass,
                _role,
                _filterQueryLanguage,
                _filterQuery,
                _operationTimeout,
                _continueOnError,
                _maxObjectCount);
            PEGASUS_TEST_ASSERT(_expectOpenReturnGood);
        }
        catch (CIMException& e)
        {
            testCIMException(e);
        }
        displayEnd();
        return true;
    }

    Boolean openAssociatorInstances()
    {
        startOperationTest("openAssociatorInstances");

        try
        {
            Array<CIMInstance> cimInstances = _client.openAssociatorInstances(
                _enumerationContext,
                _endOfSequence,
                _nameSpace,
                _cimObjectName,
                _assocClass,
                _resultClass,
                _role,
                _resultRole,
                _includeClassOrigin,
                _cimPropertyList,
                _filterQueryLanguage,
                _filterQuery,
                _operationTimeout,
                _continueOnError,
                _maxObjectCount);
                PEGASUS_TEST_ASSERT(_expectOpenReturnGood);
        }
        catch (CIMException& e)
        {
            testCIMException(e);
        }
        displayEnd();
        return true;
    }

    Boolean openAssociatorInstancePaths()
    {
        startOperationTest("openAssociatorInstancePaths");

        Array<CIMObjectPath> cimInstancePaths;
        try
        {
            cimInstancePaths = _client.openAssociatorInstancePaths(
                _enumerationContext,
                _endOfSequence,
                _nameSpace,
                _cimObjectName,
                _assocClass,
                _resultClass,
                _role,
                _resultRole,
                _filterQueryLanguage,
                _filterQuery,
                _operationTimeout,
                _continueOnError,
                _maxObjectCount);
            PEGASUS_TEST_ASSERT(_expectOpenReturnGood);
        }
        catch (CIMException& e)
        {
            testCIMException(e);
        }
        displayEnd();
        return true;
    }

    Boolean pullInstancesWithPath()
    {
        continueOperationTest("pullInstancesWithPath");
        try
        {
            Array<CIMInstance> cimInstances = _client.pullInstancesWithPath(
                _enumerationContext,
                _endOfSequence,
                _maxObjectCount);

            PEGASUS_TEST_ASSERT(_expectPullReturnGood);
        }
        catch (CIMException& e)
        {
            testCIMException(e);
        }
        displayEnd();
        return true;
    }

    Boolean pullInstancePaths()
    {
        continueOperationTest("pullInstancePaths");
        try
        {
            Array<CIMObjectPath> cimInstances = _client.pullInstancePaths(
                _enumerationContext,
                _endOfSequence,
                _maxObjectCount);

            PEGASUS_TEST_ASSERT(_expectPullReturnGood);
        }
        catch (CIMException& e)
        {
            testCIMException(e);
        }
        displayEnd();
        return true;
    }

    void executeEnumerateCalls()
    {
        VCOUT << "execute tests for EnumerateFunctions" << endl;
        openEnumerateInstances();
        openEnumerateInstancePaths();
    }

    void executeAssociationCalls()
    {
        VCOUT << "execute tests for AssociateFunctions" << endl;
        openReferenceInstances();
        openReferenceInstancePaths();

        openAssociatorInstances();
        openAssociatorInstancePaths();
    }

    void executeAllOpenCalls()
    {
        VCOUT << "Execute tests for all open functions" << endl;
        executeEnumerateCalls();
        executeAssociationCalls();
    }

}; // End of Class testCalls

// Parse Hostname input into name and port number
Boolean parseHostName(char* arg, String& hostName, Uint32& port)
{
    port = 5988;
    String argv = arg;
    hostName = argv;

    Uint32 pos;
    if (!((pos = argv.reverseFind(':')) == PEG_NOT_FOUND))
    {
        Uint64 temp;
        if (StringConversion::decimalStringToUint64(
            hostName.subString(pos+1).getCString(), temp)
            &&
            StringConversion::checkUintBounds(temp,CIMTYPE_UINT32))
        {
            hostName.remove(pos);
            port = (Uint32)temp;
        }
        else
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    CIMClient client;

    // Try to connect.  If there is an optional argument use it as the
    // hostname, optional(port) to connect.
    try
    {
        if (argc == 2)
        {
            Uint32 port;
            String hostName;
            if (!parseHostName(argv[1], hostName, port))
            {
                cerr << "Invalid hostName input " << argv[1]
                     << "format is hostname[:port]" << endl;
                return 1;
            }

            VCOUT << "Connect hostName = " << hostName << " port = "
                  << port << endl;
            client.connect(hostName, port, "", "");
        }
        else
        {
            VCOUT << "connectLocal" << endl;
            client.connectLocal();
        }
    }

    catch (CIMException& e)
    {
            cerr << "CIMException Error: in connect " << e.getMessage() << endl;
            PEGASUS_TEST_ASSERT(false);
    }
    catch (Exception& e)
    {
        cerr << "Error: in connect " << e.getMessage() << endl;
        PEGASUS_TEST_ASSERT(false);
    }

    testCalls tc(client, "test/TestProvider");
    tc.setTestName("Invalid Classname test");
    tc.setCIMException(CIM_ERR_INVALID_CLASS);
    tc._className =  "junk";
    tc.executeEnumerateCalls();
    VCOUT << "InvalidTargetClass test passed" << endl;

    // Test invalid Object Path
    tc.setTestName("Invalid Classname in associator call");
    tc._cimObjectName = "junk.id=1";
    tc.setCIMException(CIM_ERR_INVALID_PARAMETER);
    tc.executeAssociationCalls();

    // test Filter parameters on all operations

    tc.setTestName("Filter Parameter test- Using filterQuery");
    tc._className =  "CMPI_TEST_Person";
    tc._cimObjectName = "CMPI_TEST_Person.name=\"Melvin\"";
    tc._filterQuery="abc";
    tc.setCIMException(CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED);
    tc.executeAllOpenCalls();

    tc.setTestName("Filter Parameter test- Using filter and Language");
    tc._filterQueryLanguage="WQL";
    tc.setCIMException(CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED);
    tc.executeAllOpenCalls();

    tc.setTestName("Filter Parameter test- Using filterQuery Language Only");
    tc._filterQuery="";
    tc.setCIMException(CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED);
    tc.executeAllOpenCalls();

    tc.setTestName("MaxObjectCount GT max allowed by Server");
    tc._filterQuery="";
    tc._filterQueryLanguage="";
    tc._maxObjectCount = 1000000;
    tc.setCIMException(CIM_ERR_INVALID_PARAMETER);
    tc.executeAllOpenCalls();

    // ContinueOnError test. Should always error out
    tc.setTestName("ContinueOnError should always return exception");
    tc._maxObjectCount = 100;
    tc._continueOnError = true;
    tc.setCIMException(CIM_ERR_NOT_SUPPORTED);
    tc.setCIMExceptionMessage("*ContinueOnError Not supported");
    tc.executeAllOpenCalls();
    tc.setCIMExceptionMessage("");

    // Pull with no open. NOTE: This does not really work for us becuase
    // the client generates Pegasus::InvalidNamespaceNameException since
    // there is no namespace from an open operation.  Probably the only way
    // to really test this is with the wbemexec tool.
    // KS_TODO - Create a corresponding test with static tests.

//  testCalls tc1(client, "test/TestProvider");
//  tc1.setTestName("Pull with no open");
//  tc1._className =  "CMPI_TEST_Person";
//  tc1._cimObjectName = "CMPI_TEST_Person.name=\"Melvin\"";
//  tc1.pullInstancesWithPath();

    // KS_TODO - The following is incorrect in that I am using what should
    // be a private function to mess with the EnumerationContext. Need
    // to fix this and also clean up the CIMEnumerationContext.h file.
    testCalls tc2(client, "test/TestProvider");
    tc2.setTestName("est TPull with invalid Enumeration Context");
    tc2._className =  "CMPI_TEST_Person";
    tc2._cimObjectName = "CMPI_TEST_Person.name=\"Melvin\"";
    tc2.openEnumerateInstances();
    tc2.setCIMException(CIM_ERR_INVALID_ENUMERATION_CONTEXT);
    tc2._enumerationContext.clearEnumeration();
    tc2.pullInstancesWithPath();

    // repeat test for EnumerateInstancePaths
    tc2.openEnumerateInstancePaths();
    tc2._enumerationContext.clearEnumeration();
    tc2.pullInstancePaths();

    tc2.openReferenceInstances();
    tc2._enumerationContext.clearEnumeration();
    tc2.pullInstancesWithPath();

    tc2.openReferenceInstancePaths();
    tc2._enumerationContext.clearEnumeration();
    tc2.pullInstancePaths();

    tc2.openAssociatorInstances();
    tc2._enumerationContext.clearEnumeration();
    tc2.pullInstancesWithPath();

    tc2.openAssociatorInstancePaths();
    tc2._enumerationContext.clearEnumeration();
    tc2.pullInstancePaths();

    // Test invalid pull type for enumeration.
    // KS_TODO - This one is incorrect I think.  Check to see if it
    // is correct that we are returning the Enum Context error.
    testCalls tc3(client, "test/TestProvider");
    tc3.setTestName("Open followed by Incorrect Pull operation");
    tc3._className =  "CMPI_TEST_Person";
    tc3._cimObjectName = "CMPI_TEST_Person.name=\"Melvin\"";
    tc3.openEnumerateInstances();
    tc3.setCIMException(CIM_ERR_INVALID_ENUMERATION_CONTEXT);
    tc3.pullInstancePaths();

    // Test for a complete sequence that works.
    testCalls tcgood(client, "test/TestProvider");
    tcgood.setTestName("Good complete open/pull response");
    tcgood._className =  "CMPI_TEST_Person";
    tcgood._cimObjectName = "CMPI_TEST_Person.name=\"Melvin\"";
    tcgood.openEnumerateInstances();
    while(tcgood._endOfSequence == false)
    {
        tcgood.pullInstancesWithPath();
    }

    // test for exception returns on pull and close where there is no open
    //// TODO Problems with this Gens wrong error and dies in
    //// linkable testEnumContextError(client);

    /*  Interoperation Timeout tests.  Note that these tests cannot
        completely test since we have no way to test if the EnumerationContext
        is actually released.
    */
    // test openEnumerateInstances and pull after timeout.
    //testCalls tc(client, "test/TestProvider");
    tc.setTestName("Interoperation Timeout upon Pull");
    tc._className =  "CMPI_TEST_Person";
    tc._maxObjectCount = 1;
    tc._operationTimeout = 7;

    // execute the open call and then wait past timer to test for timeout
    tc.openEnumerateInstances();
    VCOUT << "Wait for open operation to timeout" << endl;
    sleep(10);

    tc.setCIMException(CIM_ERR_INVALID_ENUMERATION_CONTEXT);
    tc.pullInstancesWithPath();
    VCOUT << "Interoperation Timeout test passed. Note that we"
             " cannot do complete test without enum context table inspsection"
          << endl;

    // KS_TODO - Extend this to other test options
    // KS_TODO - FInd some way to get info on the Number of Enumeration
    //           context entries to complete test.


    // KS_TODO tests we have not yet built
    // 2. EnumerationContextError on close - Execute Close after open
    //    with changed context.
    // 3. More error on input of invalid interoperation times and
    //    maxobjectCounts.
    // 4. Invalid roles, etc. on association requests.

    cout << argv[0] <<" +++++ passed all tests" << endl;
    return 0;
}
