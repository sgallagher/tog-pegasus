//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//=============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <cstring>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/SharedPtr.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/ContentLanguageList.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

// Trace files for test purposes
// Will be created in the $(PEGASUS_TMP) directory, or if not set,
// in the current directory
CString FILE1;
CString FILE2;
CString FILE3;
CString FILE4;

// 
// Reads the last trace message from a given trace file and compares the 
// given string with the string read from file
//
// return 0 if the strings match
// return 1 if the strings do not match
//
Uint32 compare(const char* fileName, const char* expectedMessage)
{
    int expectedMessageLength = strlen(expectedMessage);

    // Compute the size of the message in the trace file.  Include the final
    // EOL character added by the Tracer.  This size will be used to seek
    // from the end of the file back to the beginning of the trace message.
    int seekBytes = expectedMessageLength + 1;

#if defined(PEGASUS_OS_TYPE_WINDOWS)
    // Windows converts all '\n' characters to "\r\n" sequences in the trace
    // file.  Increase the seekBytes by the number of '\r' characters added
    // when the message is written to the file.
    for (const char* newlineChar = expectedMessage;
         ((newlineChar = strchr(newlineChar, '\n')) != 0);
         newlineChar++)
    {
        seekBytes++;
    }

    // Count the '\r' character added with the final '\n' written by the Tracer
    seekBytes++;
#endif

    AutoArrayPtr<char> actualMessage(new char[expectedMessageLength + 1]);

    // Read the trace message from the file, minus the message prefix and
    // minus the trailing newline.
    fstream file;
    file.open(fileName, fstream::in);
    if (!file.good())
    {
        return 1;
    }
    file.seekg(-seekBytes, fstream::end);
    file.read(actualMessage.get(), expectedMessageLength);
    file.close();
    actualMessage[expectedMessageLength] = 0;

    // Compare the expected and actual messages
    Uint32 retCode = strcmp(expectedMessage, actualMessage.get());

    /* Diagnostic to determine string differences
    if (retCode)
        cout << "Compare Error: expectedMessage= \n\"" << expectedMessage <<
            "\". actualMessage= \n\"" << actualMessage.get() << "\"" << endl;
    */

    return retCode;
}

//
// Description:
// Trace properties file, level and component are not set
// Should not log a trace message
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test1()
{
    const char* METHOD_NAME = "test1";
    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL2,"%s %d",
        "This message should not appear value=",123));
    PEG_METHOD_EXIT();
    return System::exists(FILE1) ? 1 : 0;
}

//
// Description:
// Trace properties level and component are not set
// Should not log a trace message
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test2()
{
    const char* METHOD_NAME = "test2";
    Tracer::setTraceFile(FILE1);
    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL2,"%s %d",
        "This message should not appear value=",123));
    Uint32 fileSize;
    System::getFileSize(FILE1, fileSize);
    return (fileSize == 0) ? 0 : 1;
}

//
// Description:
// Trace properties component is not set
// Should not log a trace message
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test3()
{
    const char* METHOD_NAME = "test3";
    Tracer::setTraceLevel(Tracer::LEVEL1);
    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL2,"%s",
        "This message should not appear"));
    Uint32 fileSize;
    System::getFileSize(FILE1, fileSize);
    return (fileSize == 0) ? 0 : 1;
}

//
// Description:
// Trace properties file, level and component are set
// should log a trace message
//
// Type:
// Positive 
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test4()
{
    const char* METHOD_NAME = "test4";
    Tracer::setTraceComponents("Config");
    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    return(compare(FILE1,"Entering method test4"));
}

//
// Description:
// Trace component is set to an invalid component
// should not log a trace message
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test5()
{
    const char* METHOD_NAME = "test5";
    Tracer::setTraceComponents("Wrong Component Name");

    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    PEG_METHOD_EXIT();
    return(compare(FILE1,"Entering method test4"));
}

//
// Description:
// Trace level is set to LEVEL 2 and logs a LEVEL 4 message 
// should not log a trace message
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test6()
{
    const char* METHOD_NAME = "test6";
    Tracer::setTraceComponents("Config");
    Tracer::setTraceLevel(Tracer::LEVEL2);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL2,"%s %s",
        "Test Message for Level2 in",METHOD_NAME));
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL2,"%s %s",
        "Test Message for Level2 in",METHOD_NAME));
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL4,"%s",
        "This Message should not appear"));
    return(compare(FILE1,"Test Message for Level2 in test6"));
}

//
// Description:
// Trace level is set to an invalid level
// should not log a trace message
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test7()
{
    const char* METHOD_NAME = "test7";
    Tracer::setTraceLevel(100);
    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    PEG_METHOD_EXIT();
    return(compare(FILE1,"Test Message for Level2 in test6"));
}

//
// Description:
// Changes the trace file to FILE2 
//
// Type:
// Positive 
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test9()
{
    const char* METHOD_NAME = "test9";
    Tracer::setTraceLevel(Tracer::LEVEL3);
    Tracer::setTraceFile(FILE2);

    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL3,"%s %s",
        "Test Message for Level3 in",METHOD_NAME));
    return(compare(FILE2,"Test Message for Level3 in test9"));
}

//
// Description:
// Passes invalid component in the trace call
// should not log a trace message
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//
// This test not required with change t0
// use and test macros only.

Uint32 test10()
{
    const char* METHOD_NAME = "test10";
    Tracer::setTraceComponents("ALL");
    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    PEG_METHOD_EXIT();
    return(compare(FILE2,"Test Message for Level3 in test9"));
}

//
// Description:
// Implements trace call for Tracer::Level1
// should log a trace message
//
// Type:
// Positive 
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test11()
{
    const char* METHOD_NAME = "test11";
    Tracer::setTraceComponents("ALL");
    Tracer::setTraceLevel(Tracer::LEVEL4);
    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    return(compare(FILE2,"Entering method test11"));
}

//
// Description:
// Implements trace call for Tracer::Level1
// should log a trace message
//
// Type:
// Positive 
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test12()
{
    const char* METHOD_NAME = "test12";
    Tracer::setTraceComponents("ALL");
    Tracer::setTraceLevel(Tracer::LEVEL4);
    PEG_METHOD_ENTER(TRC_CONFIG,METHOD_NAME);
    PEG_METHOD_EXIT();
    return(compare(FILE2,"Exiting method test12"));
}

//
// Description:
// Implements trace call for Tracer::Level2
// should log a trace message
//
// Type:
// Positive 
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test13()
{
    const char* METHOD_NAME = "test13";
    Tracer::setTraceComponents("ALL");
    Tracer::setTraceLevel(Tracer::LEVEL4);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL2,"%s %s",
        "Test Message for Level2 in",METHOD_NAME));
    return(compare(FILE2,"Test Message for Level2 in test13"));
}

//
// Description:
// Implements trace call for Tracer::Level3
// should log a trace message
//
// Type:
// Positive 
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test14()
{
    const char* METHOD_NAME = "test14";
    Tracer::setTraceComponents("ALL");
    Tracer::setTraceLevel(Tracer::LEVEL4);
    Tracer::setTraceFile(FILE3);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL3,"%s %s",
        "Test Message for Level3 in",METHOD_NAME));
    return(compare(FILE3,"Test Message for Level3 in test14"));
}

//
// Description:
// Implements trace call for Tracer::Level4
// should log a trace message
//
// Type:
// Positive 
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test15()
{
    const char* METHOD_NAME = "test15";
    Tracer::setTraceComponents("ALL");
    Tracer::setTraceLevel(Tracer::LEVEL4);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL4,"%s %s",
        "Test Message for Level4 in",METHOD_NAME));
    return(compare(FILE3,"Test Message for Level4 in test15"));
}

//
// Description:
// calls the setTraceComponents with null string
// should log a trace message
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test16()
{
    const char* METHOD_NAME = "test16";
    Tracer::setTraceComponents("");
    Tracer::setTraceLevel(Tracer::LEVEL4);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL4,"%s %s",
    "This Message should not appear in",METHOD_NAME));
    return(compare(FILE3,"Test Message for Level4 in test15"));
}

//
// Description:
// calls the setTraceComponents with one valid and another invalid component
// should log a trace message
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test17()
{
    const char* METHOD_NAME = "test17";
    Tracer::setTraceComponents("InvalidComp");
    Tracer::setTraceLevel(Tracer::LEVEL4);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL4,"%s %s",
    "This Message should not appear in",METHOD_NAME));
    return(compare(FILE3,"Test Message for Level4 in test15"));
}
//
// Description:
// calls the _traceBuffer call
// should log a trace message
//
// Type:
// Positive
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test18()
{
    const char* METHOD_NAME = "test18";
    Tracer::setTraceComponents("Config,InvalidComp");
    Tracer::setTraceLevel(Tracer::LEVEL4);
    PEG_TRACE_CSTRING(TRC_CONFIG,Tracer::LEVEL4,
        "This Message should appear in");
    return(compare(FILE3,"This Message should appear in"));
}

//
// Description:
// Trace a string.
// Calls the PEG_TRACE macro
// should log a trace message
//
// Type:
// Positive
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test20()
{
    const char* METHOD_NAME = "test20";
    Tracer::setTraceFile(FILE4);
    Tracer::setTraceComponents("ALL");
    Tracer::setTraceLevel(Tracer::LEVEL4);

    PEG_METHOD_ENTER(TRC_CONFIG, METHOD_NAME);
    PEG_TRACE((TRC_CONFIG,Tracer::LEVEL4,
    "Test Message for Level4 in test20"));
    return(compare(FILE4,"Test Message for Level4 in test20"));
}

//
// Description:
// Trace a CIMException.
// Calls the traceCIMException() method
// should log a trace message
//
// Type:
// Positive
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test21()
{
    const char* METHOD_NAME = "test21";
    Tracer::setTraceFile(FILE4);
    Tracer::setTraceComponents("ALL");
    Tracer::setTraceLevel(Tracer::LEVEL4);

    PEG_METHOD_ENTER(TRC_CONFIG, METHOD_NAME);

    // test tracing CIMException
    try
    {
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED, 
            "CIM Exception Message for Level4 in test21.");
    }
    catch (CIMException& e)
    {
        Tracer::traceCIMException(TRC_CONFIG,Tracer::LEVEL4, e);
    }

    return 0;
}

//
// Description:
// Trace a string using macro.
// should log a trace message
//
// Type:
// Positive
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test22()
{
    const char* METHOD_NAME = "test22";
    Tracer::setTraceFile(FILE4);
    Tracer::setTraceComponents("ALL");
    Tracer::setTraceLevel(Tracer::LEVEL4);

    PEG_METHOD_ENTER(TRC_CONFIG, METHOD_NAME);

    PEG_TRACE_STRING(TRC_CONFIG,Tracer::LEVEL4,
                     String("Test message for Level4 in test22."));

    return(compare(FILE4,"Test message for Level4 in test22."));
}

//
// Description:
// Trace a character string using macro.
// should log a trace message
//
// Type:
// Positive
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test23()
{
    const char* METHOD_NAME = "test23";
    Tracer::setTraceFile(FILE4);
    Tracer::setTraceComponents("ALL");
    Tracer::setTraceLevel(Tracer::LEVEL4);

    PEG_METHOD_ENTER(TRC_CONFIG, METHOD_NAME);

    PEG_TRACE_CSTRING(TRC_CONFIG,Tracer::LEVEL4,
                      "Test message for Level4 in test23.");

    return(compare(FILE4,"Test message for Level4 in test23."));
}

// 
// Description:
// Test the getHTTPRequestMessage method.
//
// Type:
// Positive
// Tests with a HTTP Request without a basic authorization header.
// Message is written to trace file without any changes.
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test24()
{
    Tracer::setTraceFile(FILE4);
    Tracer::setTraceComponents("xmlio");
    Tracer::setTraceLevel(Tracer::LEVEL2);

    Uint32 queueId = 18;
    CIMPropertyList propertyList;
    Buffer params;
    AcceptLanguageList al;
    ContentLanguageList cl; 

    XmlWriter::appendClassNameIParameter(
        params, "ClassName", CIMName("testclass"));
    Buffer buffer = XmlWriter::formatSimpleIMethodReqMessage(
        "localhost",
        CIMNamespaceName("test/cimv2"), 
        CIMName ("EnumerateInstanceNames"),
        "12345", 
        HTTP_METHOD__POST,
        "Basic: Authorization AAAAA",
        al,
        cl,
        params);

    SharedArrayPtr<char> reqMsg(Tracer::getHTTPRequestMessage(
            buffer));

    PEG_TRACE((
        TRC_XML_IO, 
        Tracer::LEVEL2,
        "<!-- Request: queue id: %u -->\n%s",
        queueId,
        reqMsg.get()));

    return(compare(FILE4, buffer.getData()));
} 
    
// 
// Description:
// Test the getHTTPRequestMessage method.
//
// Type:
// Positive
// Tests with a HTTP Request that contains a Basic authorization header.
// The user/password info in the message is suppressed before writing it to 
// the trace file.
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test25()
{   
    Tracer::setTraceFile(FILE4);
    Tracer::setTraceComponents("xmlio");
    Tracer::setTraceLevel(Tracer::LEVEL2);

    Uint32 queueId = 18;
    CIMPropertyList propertyList;
    Buffer params;
    AcceptLanguageList al;
    ContentLanguageList cl;
    String authHeader = "Authorization: Basic ABCDEABCDE==";
    String MSGID = "32423424";

    XmlWriter::appendClassNameIParameter(
        params, 
        "ClassName", 
        CIMName("testclass"));
    Buffer buffer = XmlWriter::formatSimpleIMethodReqMessage(
        "localhost",
        CIMNamespaceName("test/cimv2"), 
        CIMName ("EnumerateInstanceNames"),
        MSGID, 
        HTTP_METHOD__POST,
        authHeader,
        al,
        cl,
        params);

    PEG_TRACE((
        TRC_XML_IO, 
        Tracer::LEVEL2,
        "<!-- Request: queue id: %u -->\n%s",
        queueId,
        Tracer::getHTTPRequestMessage(
            buffer).get()));
    
    String testStr(buffer.getData());
    Uint32 pos = testStr.find("ABCDEABCDE==");
    
    for ( Uint32 i = pos; i < pos+strlen("ABCDEABCDE=="); i++)
        testStr[i] = 'X';

    return(compare(FILE4, testStr.getCString()));
}

int main(int argc, char** argv)
{

// Execute the tests only if trace calls are included

#ifdef PEGASUS_REMOVE_TRACE
    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
#else

    const char* tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        tmpDir = ".";
    }
    String f1 (tmpDir);
    f1.append("/testtracer1.trace");
    FILE1 = f1.getCString();
    String f2 (tmpDir);
    f2.append("/testtracer2.trace");
    FILE2 = f2.getCString();
    String f3 (tmpDir);
    f3.append("/testtracer3.trace");
    FILE3 = f3.getCString();
    String f4 (tmpDir);
    f4.append("/testtracer4.trace");
    FILE4 = f4.getCString();

    System::removeFile(FILE1);
    System::removeFile(FILE2);
    System::removeFile(FILE3);
    System::removeFile(FILE4);
    if (test1() != 0)
    {
       cout << "Tracer test (test1) failed" << endl;
       exit(1);
    }
    if (test2() != 0)
    {
       cout << "Tracer test (test2) failed" << endl;
       exit(1);
    }
    if (test3() != 0)
    {
       cout << "Tracer test (test3) failed" << endl;
       exit(1);
    }
    if (test4() != 0)
    {
       cout << "Tracer test (test4) failed" << endl;
       exit(1);
    }
    if (test5() != 0)
    {
       cout << "Tracer test (test5) failed" << endl;
       exit(1);
    }
    if (test6() != 0)
    {
       cout << "Tracer test (test6) failed" << endl;
       exit(1);
    }
    if (test7() != 0)
    {
       cout << "Tracer test (test7) failed" << endl;
       exit(1);
    }
    if (test9() != 0)
    {
       cout << "Tracer test (test9) failed" << endl;
       exit(1);
    }
    /*************************** 
       Test 10 bypassed when tests changed to
       use macros.  It did an invalid call which is
       not possible with macros

    if (test10() != 0)
    {
       cout << "Tracer test (test10) failed" << endl;
       exit(1);
    }
    ******************************/
    if (test11() != 0)
    {
       cout << "Tracer test (test11) failed" << endl;
       exit(1);
    }
    if (test12() != 0)
    {
       cout << "Tracer test (test12) failed" << endl;
       exit(1);
    }
    if (test13() != 0)
    {
       cout << "Tracer test (test13) failed" << endl;
       exit(1);
    }
    if (test14() != 0)
    {
       cout << "Tracer test (test14) failed" << endl;
       exit(1);
    }
    if (test15() != 0)
    {
       cout << "Tracer test (test15) failed" << endl;
       exit(1);
    }
    if (test16() != 0)
    {
       cout << "Tracer test (test16) failed" << endl;
       exit(1);
    }
    if (test17() != 0)
    {
       cout << "Tracer test (test17) failed" << endl;
       exit(1);
    }
    if (test18() != 0)
    {
       cout << "Tracer test (test18) failed" << endl;
       exit(1);
    }
    if (test20() != 0)
    {
       cout << "Tracer test (test20) failed" << endl;
       exit(1);
    }
    if (test21() != 0)
    {
       cout << "Tracer test (test21) failed" << endl;
       exit(1);
    }
    if (test22() != 0)
    {
       cout << "Tracer test (test22) failed" << endl;
       exit(1);
    }
    if (test23() != 0)
    {
       cout << "Tracer test (test23) failed" << endl;
       exit(1);
    }
    if (test24() != 0)
    {
       cout << "Tracer test (test24) failed" << endl;
       exit(1);
    }

    if (test25() != 0)
    {
       cout << "Tracer test (test25) failed" << endl;
       exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;
    System::removeFile(FILE1);
    System::removeFile(FILE2);
    System::removeFile(FILE3);
    System::removeFile(FILE4);
    return 0;
#endif
}
