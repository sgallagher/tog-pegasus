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
//==============================================================================
//
// Author:  Aruran (aruran.shanmug@in.ibm.com) & Melvin (msolomon@in.ibm.com),
//                                                       IBM for PEP# 241
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef TestStressTestClient_h
#define TestStressTestClient_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Signal.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/TimeValue.h>
#include <signal.h>

#ifdef PEGASUS_OS_TYPE_WINDOWS
 #include <windows.h> /* DWORD etc. */
 typedef DWORD pid_t;
 #include <process.h> /* getpid() and others. */
#elif !defined(PEGASUS_OS_OS400)
 #include <unistd.h>
#endif

#ifdef PEGASUS_PLATFORM_SOLARIS_SPARC_CC
 #include <iostream.h>
#endif

#define SIXTYSECONDS 60
#define MILLISECONDS 1000
#define CHECKUP_INTERVAL 0.8
#define convertmin2millisecs(x) ((x * SIXTYSECONDS * MILLISECONDS))

PEGASUS_NAMESPACE_BEGIN

/** StressTest Client Status types. */
enum CStatus
{
    CLIENT_PASS,
    CLIENT_FAIL,
    CLIENT_UNKNOWN
};

#ifndef PEGASUS_STRESSTESTCLIENT_LINKAGE
# ifdef PEGASUS_OS_TYPE_WINDOWS
#  ifdef PEGASUS_STRESSTESTCLIENT_INTERNAL
#   define PEGASUS_STRESSTESTCLIENT_LINKAGE PEGASUS_EXPORT
#  else
#   define PEGASUS_STRESSTESTCLIENT_LINKAGE PEGASUS_IMPORT
#  endif
# else
#  define PEGASUS_STRESSTESTCLIENT_LINKAGE
# endif
#endif

/** The TestStressTestClient class holds the common functionality for all the
    stress test clients.
*/
class PEGASUS_STRESSTESTCLIENT_LINKAGE TestStressTestClient
{
public:
    /** Constructor. */
    TestStressTestClient();

    /* This method is use to get all the options that are passed through
       command line.
    */
    int GetOptions(
        OptionManager& om,
        int& argc,
        char** argv,
        OptionRow* clientOptionsTable,
        Uint32 clientOptionCount);

    /** This method is used by clients to register client specific required
        options to the option table. All these options are taken as mandatory
        one.
    */
    OptionRow* generateClientOptions(
        OptionRow* clientOptionsTable,
        Uint32 clientOptionCount,
        Uint32& totalOptionCount);

    /** This method is used by the clients to connect to the server. If useSSL
        is true then an SSL connection will be atemped with the userName and
        passWord that is passed in. If localConnection is true a connectLocal
        connection will be attempted. All parameters are required. 
    */
    void connectClient(
        CIMClient *client,
        String host,
        Uint32 portNumber,
        String userName,
        String passWord,
        Boolean useSSL,
        Uint32 timeout,
        Boolean verboseTest);

    /** This method is used by the clients to log information which are
        required for controller reference. It logs the inofrmation with
        Client ID and status of the client in the PID File log file.
    */
    void logInfo(
        String clientId,
        pid_t clientPid,
        int clientStatus,
        String &pidFile);

    /** This method is used to take the client process start time. */
    void startTime();

    /** This method is used to check the time stamp for logging information
        about the success or failure.
    */
    Boolean checkTime();

    /** This method is used to log the information about the client's success
        or failure percentage at a specific interval of time.
    */
    void logErrorPercentage(
        Uint32 successCount,
        Uint32 totalCount,
        pid_t clientPid,
        String &clientLog,
        char client[]);

    /** This method is used to log the informations of client logs to the
        client log file.
    */
    void errorLog(pid_t clientPid, String &clientLog, String &message);

    /** Timer details. */
    Uint64 startMilliseconds;
    Uint64 nowMilliseconds;
    Uint64 nextCheckupInMillisecs;
    struct OptionRow *optionsTable;
    Uint32 optionCount;
};
PEGASUS_NAMESPACE_END
#endif /* TestStressTestClient_h */
