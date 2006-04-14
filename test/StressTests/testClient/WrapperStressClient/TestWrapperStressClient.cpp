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
// Author:  Melvin, IBM (msolomon@in.ibm.com) for PEP# 241
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <../test/StressTests/testClient/TestStressTestClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

char clientName[] = "WrapperStressClient";

class TestWrapperStressClient : public TestStressTestClient
{
};

Boolean _quit = false;
Boolean _nextCheck = false;
String errorInfo;

/**
    Signal handler for SIGALRM.
    @param   signum  the alarm identifier
 */
void endTest(int signum)
{
   cout<<"Recieved interupt signal SIGINT!\n"<<endl;
  _quit = true;
}

///////////////////////////////////////////////////////////////
//    MAIN
///////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    OptionManager om;
    TestWrapperStressClient wc;
    pid_t clientPid = getpid();
    Uint32 validArg = 0;
    Boolean verboseTest;

    //Varriables needed to do loging and status checking
    String clientid;
    String pidfile;
    String clientlog, stopClient;
    char pid_str[15];
    int status = CLIENT_UNKNOWN;
    Uint32 successCount=0;        //Number of times the client command succeeded.
    Uint32 iteration=0;           //Number of iterations after which logErrorPercentage() is called.
    Uint32 totalCount=0;          //Total number of times the client command was executed.

    //Variables needed for Command operation
    String command;
    String options;
    String help;

    try
    {
        struct OptionRow *newOptionsTable = 0;
        Uint32 newOptionsCount;

        struct OptionRow cOptionTable[] = {
          {"clientname", "", true, Option::STRING, 0, 0, "clientname",
                                                       "Client name" },
 
          {"options", "", true, Option::STRING, 0, 0, "options",
                             "Corresponding Client program's options" }
        };

        Uint32 cOptionCount = sizeof(cOptionTable) / sizeof(cOptionTable[0]);
        newOptionsCount = cOptionCount;

        try
        {
            //Generate new option table for this client using OptionManager
            newOptionsTable = wc.generateClientOptions(cOptionTable,cOptionCount,newOptionsCount);
            validArg = wc.GetOptions(om, argc, argv, newOptionsTable,newOptionsCount);
        }
        catch (Exception& e)
        {
            cerr << argv[0] << ": " << e.getMessage() << endl;
            exit(1);
        }
        catch (...)
        {
            cerr << argv[0] << ": Error in Options operations " << endl;
            exit(1);
        }

        verboseTest = om.isTrue("verbose");
 
        om.lookupValue("clientid", clientid);

        om.lookupValue("pidfile", pidfile);

        om.lookupValue("clientlog", clientlog);

        om.lookupValue("clientname", command);

        om.lookupValue("options", options);

        om.lookupValue("help", help);
    }// end of option Try block
    catch (Exception& e)
    {
        cerr << argv[0] << ": " << e.getMessage() << endl;
        exit(1);
    }
    catch (...)
    {
        cerr << argv[0] << ": Unknown Error gathering options in Wrapper Client  " << endl;
        exit(1);
    }

    /** // Checking whether the user asked for HELP Info...
    if (om.valueEquals("help", "true"))
    {
        String header = "Usage ";
        header.append(argv[0]);
        header.append(" -parameters -clientname [clientname] -options [options] ");
        header.append(" -clientid [clientid] -pidfile [pidfile] -clientlog [clientlog]");
        String trailer = "Assumes localhost:5988 if host not specified";
        trailer.append("\nHost may be of the form name or name:port");
        trailer.append("\nPort 5988 assumed if port number missing.");
        om.printOptionsHelpTxt(header, trailer);

        exit(0);
    }
	*/

    try
    {
        if (options.getCString())
        {
            command.append(" " + options);
        }
        if(verboseTest)
        {
            errorInfo.append("client command :  ");
            errorInfo.append(command);
            wc.errorLog(clientPid, clientlog, errorInfo);
            errorInfo.clear();
        }

        //Signal Handling - SIGINT
        signal(SIGINT, endTest);

        //Timer Start
        wc.startTime();

        wc.logInfo(clientid, clientPid, status, pidfile);
        sprintf(pid_str, "%d", clientPid);

        stopClient = String::EMPTY;
        stopClient.append(FileSystem::extractFilePath(pidfile));
        stopClient.append("STOP_");
        stopClient.append(pid_str);

        // This loop executes till the client gets stop signal from controller
        while(!_quit)
        {
            if(FileSystem::exists(stopClient))
            {
                if(verboseTest)
                {
                    String mes("Ending client: ");
                    mes.append((Uint32)clientPid);
                    wc.errorLog(clientPid, clientlog, mes);
                }
                break;
            }
           
            #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
                if (!verboseTest)
                    freopen("nul","w",stdout);
            #else 
                if (!verboseTest)
                    freopen("/dev/null","w",stdout);
            #endif

            int i = system (command.getCString()); 

            iteration++;
            totalCount++;
            switch (i)
            {
                case 0:
                    if (status != CLIENT_PASS)
                    {
                        status = CLIENT_PASS;
                        wc.logInfo(clientid, clientPid, status, pidfile);
                    }
                    successCount++;
                    break;

                case 1:
                    status = CLIENT_UNKNOWN;
                    wc.logInfo(clientid, clientPid, status, pidfile);
                    break;

                default:
                    status = CLIENT_FAIL;
                    wc.logInfo(clientid, clientPid, status, pidfile);
                    break;
            }

            _nextCheck = wc.checkTime();
            if (_nextCheck)
            {
                wc.logInfo(clientid, clientPid, status, pidfile);
                _nextCheck = false;
            }

            /* If verbose is set log success percentage for every 100 iterations.
               If verbose is not set log success percentage for every 10000 iterations
            */
            if (verboseTest)
            {
                if (iteration==100)
                {
                    wc.logErrorPercentage(
                        successCount,
                        totalCount,
                        clientPid,
                        clientlog, 
                        clientName);
                    iteration = 0;
                }
            }
            else
            {
                if (iteration==1000)
                {
                    wc.logErrorPercentage(
                        successCount,
                        totalCount,
                        clientPid,
                        clientlog,
                        clientName);
                    iteration = 0;
                }
            }
        }//end of while(!_quit)

    }//end of command execution try block
    catch (Exception &exp)
    {
        String exp_str("Exception in WrapperClient causing it to exit: ");
        exp_str.append(exp.getMessage());
        wc.errorLog(clientPid, clientlog, exp_str);

        if (verboseTest)
        {
            PEGASUS_STD(cerr) << exp_str.getCString() << PEGASUS_STD(endl);
        }
    }
    catch (...)
    {
        String exp_str("General Exception in WrapperClient causing it to exit");
        wc.errorLog(clientPid, clientlog, exp_str);

        if (verboseTest)
        {
            PEGASUS_STD(cerr) << exp_str.getCString() << PEGASUS_STD(endl);
        }
    }

    // second delay before shutdown
#ifndef PEGASUS_PLATFORM_WIN32_IX86_MSVC
           sleep(1);
#else
           Sleep(1000);
#endif
    if(FileSystem::exists(stopClient))
    {
        // Remove STOP file here 
        FileSystem::removeFile(stopClient);
    }
    if (verboseTest)
    {
        errorInfo.append("++++ TestWrapperStressClient Terminated Normally +++++");
        wc.errorLog(clientPid, clientlog, errorInfo);
        errorInfo.clear();
    }
    return 0;
}
