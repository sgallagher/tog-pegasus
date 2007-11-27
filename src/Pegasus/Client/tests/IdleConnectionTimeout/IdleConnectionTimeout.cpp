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
//%/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Client/CIMClient.h>

#define NUM_THREADS 2
#define PASSED true
#define FAILED false
#define OSINFO_NAMESPACE CIMNamespaceName ("root/cimv2")
#define OSINFO_CLASSNAME CIMName ("PG_OperatingSystem")

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean test2CaughtException;
Boolean connectLocal;
char * testUseridPtr;
char * testPasswdPtr;
char testUserid[256];
char testPasswd[256];
char testHostname[256];

////////////////////////////////////////////////////////////////////////////////
//
// Thread Parameters Class
//
////////////////////////////////////////////////////////////////////////////////
class T_Parms{
   public:
    AutoPtr<CIMClient> client;
    Uint32 duration;
    Uint32 uniqueID;
};


ThreadReturnType PEGASUS_THREAD_CDECL _runningThd(void *parm)
{
    Thread *my_thread = (Thread *)parm;
    T_Parms *parms = (T_Parms *)my_thread->get_parm();
    CIMClient *client = parms->client.get();
    Uint32 duration = parms->duration;
    Uint32 uniqueID = parms->uniqueID;

    for(Uint32 i = 0; i <= duration; i++)
    {
        Threads::sleep(1000);
        CIMClass tmpClass = 
            client->getClass (OSINFO_NAMESPACE, OSINFO_CLASSNAME);
    }

    my_thread->exit_self((ThreadReturnType)0);
    return 0;
}

ThreadReturnType PEGASUS_THREAD_CDECL _idleThd(void *parm)
{
    Thread *my_thread = (Thread *)parm;
    T_Parms *parms = (T_Parms *)my_thread->get_parm();
    CIMClient *client = parms->client.get();
    Uint32 duration = parms->duration;
    Uint32 uniqueID = parms->uniqueID;

    try
    {
        CIMClass tmpClass = 
            client->getClass (OSINFO_NAMESPACE, OSINFO_CLASSNAME);

        cout << "Test 2 of 2: Begin " << duration 
            << " second idle period..." << endl;
        Threads::sleep(duration*1000);

        CIMClass tmpClass2 = 
            client->getClass (OSINFO_NAMESPACE, OSINFO_CLASSNAME);
    }
    catch(Exception& e)
    {
        if (String::equal(e.getMessage(),"Connection closed by CIM Server."))
        {
            test2CaughtException = true;
            cout << "Expected error: " << e.getMessage() << endl;
        }
        else
        {
            cerr << "Error: " << e.getMessage() << endl;
        }
    }

    my_thread->exit_self((ThreadReturnType)0);
    return 0;
}

Thread * _runTestThreads(
    CIMClient* client,
    ThreadReturnType (PEGASUS_THREAD_CDECL *_executeFn)(void *),
    const Uint32 duration,
    Uint32 uniqueID)
{
    // package parameters, create thread and run...
    AutoPtr<T_Parms> parms(new T_Parms());
    parms->client.reset(client);
    parms->duration = duration;
    parms->uniqueID = uniqueID;
    AutoPtr<Thread> t(new Thread(_executeFn, (void*)parms.release(), false));
    // zzzzz... (1 second) zzzzz...
    Threads::sleep(1000);
    t->run();
    return t.release();
}

// _test1 verifies that a CIM Operation on a client connection succeeds
// if there is no other server activity, even if the idleConnectionTimeout
// period has been exceeded. In this case, no exception should occur for the
// second call to getClass().
Boolean _test1(int duration, Boolean connectLocal)
{
    try
    {
        CIMClient client;

        if (connectLocal)
        {
            client.connectLocal();
        }
        else
        {
            cout << "Connecting to " << System::getHostName() << ":5988" 
                << endl;
            client.connect(System::getHostName(), 5988, 
                testUserid, testPasswd);
        }

        CIMClass tmpClass = 
            client.getClass (OSINFO_NAMESPACE, OSINFO_CLASSNAME);

        cout << "Test 1 of 2: Begin " << duration 
            << " second idle period..." << endl;
        Threads::sleep(duration*1000);

        CIMClass tmpClass2 = 
            client.getClass (OSINFO_NAMESPACE, OSINFO_CLASSNAME);
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        return FAILED;
    }
    cout << "Test 1 success." << endl;
    return PASSED;
}

// _test2 verifies that a CIM Operation on a client connection does not
// succeed when the idleConnectionTimeout period is exceeded (_idleThd) *and*
// there is concurrent server activity (_runningThd) so that the Monitor
// will wake up and check for timeouts. In this case, the second call to
// getClass() in _idleThd() should receive an exception because the
// connection has been closed due to the timeout.
Boolean _test2(int duration, Boolean connectLocal)
{
    try
    {
        Array<CIMClient *> clientConnections;

        // declare the clients
        CIMClient * tmpClient;
        for(Uint32 i = 0; i < NUM_THREADS; i++)
        {
            tmpClient = new CIMClient();
            clientConnections.append(tmpClient);
        }

        // connect the clients
        for(Uint32 i = 0; i < NUM_THREADS; i++)
        {
            if (connectLocal)
            {
                clientConnections[i]->connectLocal();
            }
            else
            {
                cout << "Connecting to " << System::getHostName() << ":5988" 
                    << endl;
                clientConnections[i]->connect(System::getHostName(), 5988, 
                    testUserid, testPasswd);
            }
        }

        // run tests
        Array<Thread *> clientThreads;

        test2CaughtException = false;

        clientThreads.append(
            _runTestThreads(clientConnections[0], _runningThd, duration, 0));

        clientThreads.append(
            _runTestThreads(clientConnections[1], _idleThd, duration, 1));

        // wait for threads to terminate
        for(Uint32 i=0; i< clientThreads.size(); i++)
        {
            clientThreads[i]->join();
        }

        // clean up connections
        for(Uint32 i=0; i< clientConnections.size(); i++)
        {
            if(clientConnections[i])
                delete clientConnections[i];
        }
        // clean up threads
        for(Uint32 i=0; i < clientThreads.size(); i++)
        {
            if(clientThreads[i])
                delete clientThreads[i];
        }
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        return FAILED;
    }

    // We except the exception in this case, so if it was caught
    // then the test passed.
    return test2CaughtException;
}

int main(int argc, char** argv)
{
    int duration = 0;

    if (argc != 2)
    {
        cerr 
            << "Invalid argument count, only parameter is sleep time in seconds"
            << endl;
        return 1;
    }

    const char * optOne = argv[1];
    duration = atoi(optOne);

    cout << "Testing connectLocal()" << endl;

    if (_test1(duration, true) == FAILED)
    {
        cerr << argv[0] << "----- _test1() localConnect failed" << endl;
        return 1;
    }

    if (_test2(duration, true) == FAILED)
    {
        cerr << argv[0] << "----- _test2() localConnect failed" << endl;
        return 1;
    }

    if ((testUseridPtr = getenv("PEGASUS_TEST_USER_ID")) &&
        (testPasswdPtr = getenv("PEGASUS_TEST_USER_PASS")))
    {

        strncpy(testUserid, testUseridPtr, sizeof(testUserid));
        strncpy(testPasswd, testPasswdPtr, sizeof(testPasswd));

        cout << "Testing connect() with user: " 
            << testUserid << ", passwd: " << testPasswd << endl;

        if (_test1(duration, false) == FAILED)
        {
            cerr << argv[0] << "----- _test1() connect failed" << endl;
            return 1;
        }

        if (_test2(duration, false) == FAILED)
        {
            cerr << argv[0] << "----- _test2() connect failed" << endl;
            return 1;
        }
    }
    else
    {
        cout << "Skipping connect() test, PEGASUS_TEST_USER_ID and "
            "PEGASUS_TEST_USER_PASS not defined." << endl;
    }

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);
    return 0;
}
