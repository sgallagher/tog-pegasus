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
// Author: Sushma Fernandes (sushma_fernandes@hp.com)
//
// Modified By: Jenny Yu (jenny_yu@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <cstring>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

// If Windows platform then set the EOF_CHAR to 2
#if defined(PEGASUS_OS_TYPE_WINDOWS)
    #define EOF_CHAR 2
#else
    #define EOF_CHAR 1
#endif

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
Uint32 compare(const char* fileName, const char* compareStr)
{
    Uint32 count=0;
    Uint32 retCode=0;
    fstream file;
    Uint32 size= static_cast<Uint32>(strlen(compareStr));
    AutoArrayPtr<char> readStr(new char[size+EOF_CHAR+1]);

    file.open(fileName,fstream::in);
    if (!file.good())
    {
      return 1;
    }
    file.seekg((Sint32) -(static_cast<Sint32>(size)+EOF_CHAR),fstream::end);
    memset(readStr.get(), 0, (size+EOF_CHAR+1)*sizeof(char));
    file.read(readStr.get(),size+EOF_CHAR);
    (readStr.get())[size]='\0';
    retCode=strcmp(compareStr,readStr.get());
    file.close();
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
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL2,"%s %d",
	"This message should not appear value=",123);
    PEG_METHOD_EXIT();
    return(compare(FILE1,""));
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
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL2,"%s %d",
	"This message should not appear value=",123);
    return(compare(FILE1,"This message should not appear value=123"));
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
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL2,"%s",
	"This message should not appear");
    return(compare(FILE1,"This message should not appear"));
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
    Tracer::traceExit(__FILE__,__LINE__,TRC_CONFIG,METHOD_NAME);
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
    Tracer::trace(TRC_CONFIG,Tracer::LEVEL2,"%s %s",
	"Test Message for Level2 in",METHOD_NAME);
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL2,"%s %s",
	"Test Message for Level2 in",METHOD_NAME);
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL4,"%s",
	"This Message should not appear");
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
    Tracer::traceExit(__FILE__,__LINE__,TRC_CONFIG,METHOD_NAME);
    return(compare(FILE1,"Test Message for Level2 in test6"));
}

//
// Description:
// Trace level is set to LEVEL1 for a non entry/exit message
// should not log a trace message, should log an error
//
// Type:
// Negative 
//
// return 0 if the test passed
// return 1 if the test failed
//
Uint32 test8()
{
    const char* METHOD_NAME = "test8";
    Tracer::setTraceLevel(Tracer::LEVEL1);
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL1,"%s",
	"Test Message for Level4");
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
    Tracer::trace(TRC_CONFIG,Tracer::LEVEL3,"%s %s",
	"Test Message for Level3 in",METHOD_NAME);
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

Uint32 test10()
{
    const char* METHOD_NAME = "test10";
    Tracer::setTraceComponents("ALL");
    Tracer::traceExit(__FILE__,__LINE__,Uint32(-1),METHOD_NAME);
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
    Tracer::traceExit(__FILE__,__LINE__,TRC_CONFIG,METHOD_NAME);
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
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL2,"%s %s",
	"Test Message for Level2 in",METHOD_NAME);
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
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL3,"%s %s",
	"Test Message for Level3 in",METHOD_NAME);
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
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL4,"%s %s",
	"Test Message for Level4 in",METHOD_NAME);
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
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL4,"%s %s",
	"This Message should not appear in",METHOD_NAME);
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
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL4,"%s %s",
	"This Message should not appear in",METHOD_NAME);
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
    Tracer::traceBuffer(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL4,
        "This Message should appear in",4);
    Tracer::traceBuffer(TRC_CONFIG,Tracer::LEVEL4,
        "This Message should appear in",4);
    return(compare(FILE3,"This"));
}

//
// Description:
// calls the isValid call 
// should not log a trace message
//
// Type:
// Positive
//
// return 0 if the test passed
// return 1 if the test failed
//

Uint32 test19()
{
    const char* METHOD_NAME = "test18";
    Tracer::setTraceComponents("Config,InvalidComp");
    Tracer::setTraceLevel(Tracer::LEVEL4);
    Tracer::traceBuffer(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL4,
        "This Message should appear in",4);
    Tracer::traceBuffer(TRC_CONFIG,Tracer::LEVEL4,
        "This Message should appear in",4);
    return(compare(FILE3,"This"));
}

//
// Description:
// Trace a string.
// Calls the _traceString() method
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
    Tracer::trace(__FILE__,__LINE__,TRC_CONFIG,Tracer::LEVEL4,
	"Test Message for Level4 in test20");
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

    PEG_TRACE_STRING(TRC_CONFIG,Tracer::LEVEL4,"Test message for Level4 in test22.");

    return(compare(FILE4,"Test message for Level4 in test22."));
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
    if (test1() == 0)
    {
       cout << "Tracer test (test1) failed" << endl;
       exit(1);
    }
    if (test2() == 0)
    {
       cout << "Tracer test (test2) failed" << endl;
       exit(1);
    }
    if (test3() == 0)
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
    if (test8() != 0)
    {
       cout << "Tracer test (test8) failed" << endl;
       exit(1);
    }
    if (test9() != 0)
    {
       cout << "Tracer test (test9) failed" << endl;
       exit(1);
    }
    if (test10() != 0)
    {
       cout << "Tracer test (test10) failed" << endl;
       exit(1);
    }
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
    if (test19() != 0)
    {
       cout << "Tracer test (test19) failed" << endl;
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
    cout << argv[0] << " +++++ passed all tests" << endl;
    System::removeFile(FILE1);
    System::removeFile(FILE2);
    System::removeFile(FILE3);
    System::removeFile(FILE4);
    return 0;
#endif
}

