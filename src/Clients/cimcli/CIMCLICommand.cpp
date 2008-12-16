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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Threads.h>
#include <Pegasus/Client/CIMClient.h>
#include <Clients/cimcli/CIMCLIClient.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/HostLocator.h>

#ifdef PEGASUS_OS_ZOS
#include <Pegasus/Common/SetFileDescriptorToEBCDICEncoding.h>
#endif


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/////////////////////////////////////////////////////////////////////////
/** getClassName - gets the classname object
 * and puts in into the opts.className holder
 * If rqd, parameter must exist. Otherwise
 * it subsitutes NULL for the string.
 * Also puts the arguement in inputObjectName
 * @param- rqd - true if parameter required
 * @return True if parameter found.
*/
Boolean _getClassNameInput(int argc, char** argv, Options& opts, Boolean rqd)
{
    if (argc > 2)
    {
        opts.className = CIMName(argv[2]);
        opts.inputObjectName = argv[2];
    }
    else
    {
        if (rqd)
        {
                cerr << "Class Name Required. ex. gc CIM_Door" << endl;
                return(false);
        }
        else
        {
                opts.className = CIMName();
                opts.inputObjectName = "";
        }
    }
    return(true);
}
/** getObjectName - gets the classname object
 * and puts in into the opts.className holder
 * If rqd, parameter must exist. Otherwise
 * it subsitutes NULL for the string.
 * Also puts the arguement in inputObjectName
 * @param- rqd - true if parameter required
 * @return True if parameter found.
*/
Boolean _getObjectNameInput(int argc, char** argv, Options& opts, Boolean rqd)
{
    if (argc > 2)
    {
        opts.objectName = argv[2];
        opts.inputObjectName = argv[2];
    }
    else
    {
        if (rqd)
        {
                cerr << "Object Name Required" << endl;
                return(false);
        }
        else
        {
                opts.objectName = "";
                opts.inputObjectName = "";
        }
    }
    return(true);
}

/** _getQualifierNameInput - Gets a single parameter for
 * qualifier
 * @return true if parameter found
*/
Boolean _getQualifierNameInput(int argc, char** argv, Options& opts)
{
    if (argc > 2)
    {
        opts.qualifierName = argv[2];
        opts.inputObjectName = argv[2];
    }
    else
    {
        cerr << "Qualifier Name Required" << endl;
        return(false);
    }
    return(true);
}

Boolean setObjectManagerStatistics(CIMClient & client, Boolean newState)
{

    CIMName gathStatName ("GatherStatisticalData");
    Array<CIMInstance> instancesObjectManager;
    CIMInstance instObjectManager;
    Uint32 prop_num;
    Array<CIMName> plA;
    plA.append(gathStatName);
    CIMPropertyList statPropertyList(plA);
    // Create property list that represents correct request
    // get instance.  Get only the gatherstatitistics property
    instancesObjectManager  =
        client.enumerateInstances(PEGASUS_NAMESPACENAME_INTEROP,
            "CIM_ObjectManager",
            true, false, false, false, statPropertyList);
    PEGASUS_TEST_ASSERT(instancesObjectManager.size() == 1);
    instObjectManager = instancesObjectManager[0];
    // set correct path into instance
    instObjectManager.setPath(instancesObjectManager[0].getPath());

    prop_num = instObjectManager.findProperty(gathStatName);
    PEGASUS_TEST_ASSERT(prop_num != PEG_NOT_FOUND);

    instObjectManager.getProperty(prop_num).setValue(CIMValue(newState));

    client.modifyInstance(PEGASUS_NAMESPACENAME_INTEROP, instObjectManager,
         false, statPropertyList);
    CIMInstance updatedInstance =
        client.getInstance(PEGASUS_NAMESPACENAME_INTEROP,
        instObjectManager.getPath(),
        false, false, false, statPropertyList);
    prop_num = updatedInstance.findProperty(gathStatName);
    PEGASUS_TEST_ASSERT(prop_num != PEG_NOT_FOUND);
    CIMProperty p = updatedInstance.getProperty(prop_num);
    CIMValue v = p.getValue();
    Boolean rtn;
    v.get(rtn);
    //// Need to get it again
    cout << "Updated Status= " << ((rtn)? "true" : "false") << endl;
    return(rtn);
}
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

///////////////////////////////////////////////////////////////////////
//            Main

///////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

#ifdef PEGASUS_OS_ZOS
    // for z/OS set stdout and stderr to EBCDIC
    setEBCDICEncoding(STDOUT_FILENO);
    setEBCDICEncoding(STDERR_FILENO);
#endif

#ifdef PEGASUS_OS_PASE
    // Allow user group name larger than 8 chars in PASE environemnt
    setenv("PASE_USRGRP_LIMITED","N",1);
#endif
    // If no arguments, simply print usage message and terminate.
    MessageLoader::_useProcessLocale = true;

    if (argc == 1)
    {
        showUsage();
        exit(0);
    }

    //****** Show the args diagnostic display
    if (strcmp(argv[1],"displayargs") == 0)
    {
        cout << "argc = " << argc << endl;
        for (int i = 0; i < argc; i++)
            cout << "argv[" << i << "] = " << argv[i] << endl;
    }

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments from the command
    // line.
    OptionManager om;
    Options opts;
    try
    {
        // assume that the config file is local to directory where called.
        String testHome = ".";
        om.setMessagePath("pegasus/pegasusCLI");
        GetOptions(om, argc, argv, testHome);

        // Initialize all of the function input parameters.
        opts.location =  String::EMPTY;
#ifdef PEGASUS_HAS_SSL
        opts.ssl = false;
        opts.clientCert = String::EMPTY;
        opts.clientKey = String::EMPTY;
#endif
        opts.nameSpace = "root/cimv2";
        opts.cimCmd = "unknown";
        opts.className = CIMName();
        opts.objectName = "unknown";

        opts.isXmlOutput = false;
        opts.outputFormatType = OUTPUT_MOF;
        opts.user = String::EMPTY;
        opts.password = String::EMPTY;
        opts.verboseTest = false;

        opts.localOnly = true;
        opts.deepInheritance = false;
        opts.includeQualifiers = true;
        opts.includeClassOrigin = false;
        opts.assocClassName = String::EMPTY;
        opts.assocClass = CIMName();
        opts.resultClassName = String::EMPTY;
        opts.resultClass = CIMName();
        opts.role = String::EMPTY;
        opts.resultRole = String::EMPTY;
        opts.propertyListText = String::EMPTY;
        opts.propertyList.clear();
        opts.propertyName = String::EMPTY;
        opts.methodName = CIMName("unknown");
        opts.delay = 0;
        opts.trace = 0;
        opts.count= 97832;
        opts.repeat = 0;
        opts.time = false;
        opts.termCondition = 0;
        opts.debug = false;
        opts.queryLanguage = "WQL";

        // move any other input parameters left to the extraParams List
        CheckCommonOptionValues(om, argv, opts);

        /* note that this is in error since it assumes a fixed
           number of parameters will be used for all of the commands
           It needs to be expanded to allow for a variable minimum
           number of commands before it picks up any extras
        */
        if (argc > 2)
        {
            for (int i = 2 ; i < argc ; i++ )
                opts.extraParams.append(argv[i]);
        }
    }

    catch(CIMException& e)
    {
        cerr << argv[0] << " Caught CIMException during init: "
             << "\n" << e.getMessage()
             << endl;
        exit(1);
    }

    catch (Exception& e)
    {
        cerr << argv[0] << ": " << e.getMessage() << endl;
        exit(1);
    }
    catch(...)
    {
        cerr << argv[0] << " Caught General Exception During Init:" << endl;
        exit(1);
    }

    // if there is still an arg1, assume it is the command name.
    if (argc > 1)
    {
        opts.cimCmd = argv[1];
    }
    else
    {
        cout << " Command name must be first parameter or --c parameter."
            << " \n  ex. cli enumerateclasses\n"
            << "Enter " << argv[0] << " -h for help."
            << endl;
        exit(1);
    }

    // if the trace option was set initialize the trace function.
    if (opts.trace != 0)
    {
        const char* tmpDir = getenv ("PEGASUS_TMP");
            if (tmpDir == NULL)
            {
                tmpDir = ".";
            }
            String traceFile (tmpDir);
            traceFile.append("/cliTrace.trc");
            Tracer::setTraceFile (traceFile.getCString());
            Tracer::setTraceComponents("ALL");
            Tracer::setTraceLevel(opts.trace);
    }

    // Find the command and save index in cmdIndex
    Uint32 cmdIndex = 0;

    if (opts.verboseTest && opts.debug)
        cout << "TEST Command = " << opts.cimCmd << endl;

    // Find the command or the short cut name
    for( ; cmdIndex < NUM_COMMANDS; cmdIndex++ )
    {
        if ((String::equalNoCase(opts.cimCmd,
                CommandTable[cmdIndex].CommandName))
                ||
                (opts.cimCmd == CommandTable[cmdIndex].ShortCut))
            // Break if found
                    break;
    }

    Stopwatch totalElapsedExecutionTime;

    totalElapsedExecutionTime.start();

    // Now try to open the connection to the cim server
    CIMClient client;
    try
    {
        if (CommandTable[cmdIndex].ID_Command != ID_ShowOptions)
        {
            String host;
            HostLocator addr;
            if (opts.location != String::EMPTY)
            {
                addr.setHostLocator(opts.location);
                if (!addr.isValid())
                {
                    throw InvalidLocatorException(opts.location);
                }
                host = addr.getHost();
            }

            Uint32 portNumber = System::lookupPort( WBEM_HTTP_SERVICE_NAME,
                              WBEM_DEFAULT_HTTP_PORT );

            // Set up SSL port and flag for verbose display
            // if SSL included in build
            String useSSL;
#ifdef PEGASUS_HAS_SSL
            if (opts.ssl)
            {
                portNumber = System::lookupPort( WBEM_HTTPS_SERVICE_NAME,
                              WBEM_DEFAULT_HTTPS_PORT );
            }
            useSSL = " ssl=";
            useSSL.append((opts.ssl)? "true" : "false");
#endif

            if (host != String::EMPTY && addr.isPortSpecified())
            {
                portNumber = addr.getPort();
            }

            //check whether we should use connect() or connectLocal()
            //an empty location option indicates to use connectLocal()
            if (String::equal(host, String::EMPTY))
            {
                if (opts.verboseTest)
                {
                    cout << "Connect with connectLocal" << endl;
                }
                client.connectLocal();

            }
            else
            {
                if (opts.verboseTest)
                {
                    cout << "Connect to " << host
                        << " port=" << portNumber
                        << useSSL
                         << " for User=" << opts.user
                         << endl;
                }
#ifdef PEGASUS_HAS_SSL
                if (opts.ssl) //connect over HTTPS
                {
                    if (!String::equal(opts.clientCert, String::EMPTY)
                            && !String::equal(opts.clientKey, String::EMPTY))
                    {
                        if (opts.verboseTest)
                        {
                            cout << "SSL options "
                                << "Cert = " << opts.clientCert
                                << "clientKey = "  << opts.clientKey << endl;
                        }
                        client.connect(host,
                                       portNumber,
                                       SSLContext("",
                                           opts.clientCert,
                                           opts.clientKey,
                                           NULL, "ssl.rnd"),
                                       opts.user,
                                       opts.password);
                    } else
                    {
                        client.connect(host,
                                       portNumber,
                                       SSLContext("", NULL, "ssl.rnd"),
                                       opts.user,
                                       opts.password);
                    }
                } else //connect over HTTP
                {
                    client.connect(host, portNumber, opts.user, opts.password);
                }
#else
                client.connect(host, portNumber, opts.user, opts.password);
#endif
            }
        }
    }
    catch(Exception &e)
    {
        cerr << "Pegasus Exception: " << e.getMessage() <<
              " Trying to connect to " << opts.location << endl;
        exit(1);
    }

    // Register for Client statistics.
    ClientStatistics statistics = ClientStatistics();
    client.registerClientOpPerformanceDataHandler(statistics);

    if (opts.delay != 0)
    {
        // This was a test because of some delay caused problems.
        Threads::sleep(opts.delay * 1000);
    }

    // If the timeout is not zero, set the timeout for this connection.
    if (opts.connectionTimeout != 0)
    {
        client.setTimeout(opts.connectionTimeout * 1000);
    }

    // Save the total connect time.
    double totalConnectTime = opts.elapsedTime.getElapsed();

    double totalTime = 0;
    Uint32 repeatCount = opts.repeat;
    double maxTime = 0;
    double minTime = 10000000;

    Uint64 serverTotalTime = 0;
    Uint64 maxServerTime = 0;
    Uint64 minServerTime = 10000000;

    Uint64 rtTotalTime = 0;
    Uint64 maxRtTime = 0;
    Uint64 minRtTime = 10000000;

    // Process the input command within a try block.
    try
    {
        // Loop to repeat the command a number of times.
        do
        {
            // or exit with error through default of case logic
            switch(CommandTable[cmdIndex].ID_Command)
                {
                case ID_EnumerateInstanceNames :
                    if (!_getClassNameInput(argc, argv, opts, true))
                        exit(1);
                    enumerateInstanceNames(client, opts);
                    break;

                case ID_EnumerateAllInstanceNames :
                    if (!_getClassNameInput(argc, argv, opts, false))
                        exit(1);
                    enumerateAllInstanceNames(client, opts);
                    break;

                case ID_EnumerateInstances :
                    if (!_getClassNameInput(argc, argv, opts, true))
                        exit(1);
                    enumerateInstances(client, opts);
                    break;
                case ID_GetInstance :
                    if (!_getObjectNameInput(argc, argv, opts, true))
                        exit(1);
                    getInstance(client, opts);
                    break;

                case ID_EnumerateClassNames :
                    if (!_getClassNameInput(argc, argv, opts, false))
                        exit(1);
                    enumerateClassNames(client, opts);
                    break;

                case ID_EnumerateClasses :
                    if (!_getClassNameInput(argc, argv, opts, false))
                        exit(1);
                    enumerateClasses(client, opts);
                    break;

                case ID_GetClass :
                    if (!_getClassNameInput(argc, argv, opts, true))
                        exit(1);
                    getClass(client, opts);
                    break;

                case ID_CreateInstance :
                    if (!_getClassNameInput(argc, argv, opts, true))
                        exit(1);
                    createInstance(client, opts);
                    break;

                case ID_DeleteInstance :
                    if (!_getObjectNameInput(argc, argv, opts, true))
                        exit(1);
                    deleteInstance(client, opts);
                    break;

                case ID_CreateClass :
                    cerr << "CreateClass not implemented" << endl;
                    break;

                case ID_DeleteClass :
                    if (!_getClassNameInput(argc, argv, opts, true))
                        exit(1);
                    deleteClass(client, opts);
                    break;

                case ID_GetProperty :
                    if (argc != 4)
                    {
                        cout << "Usage: cli getproperty <instancename>"
                            "<propertyname>" << endl;
                        exit(1);
                    }

                    opts.instanceName = argv[2];
                    opts.inputObjectName = argv[2];
                    opts.propertyName = argv[3];

                    getProperty(client, opts);
                    break;

                case ID_SetProperty :
                    if (argc != 5)
                        cout <<
                           "Usage: cli setproperty instancename propertyname"
                                " value "
                           << endl;
                    setProperty(client, opts);
                    break;

                case ID_EnumerateQualifiers :
                    enumerateQualifiers(client, opts);
                    break;

                case ID_SetQualifier :
                    cerr << "SetQualifer not implemented" << endl;
                        exit(1);
                    break;

                case ID_GetQualifier :
                    if (!_getQualifierNameInput(argc, argv, opts))
                        exit(1);
                    getQualifier(client, opts);
                    break;

                case ID_DeleteQualifier :
                    if (!_getQualifierNameInput(argc, argv, opts))
                        exit(1);
                    deleteQualifier(client, opts);
                    break;

                /* Reference params are
                    [IN] <objectName> ObjectName,
                    [IN,OPTIONAL,NULL] <className> ResultClass = NULL,
                    [IN,OPTIONAL,NULL] string Role = NULL,
                    [IN,OPTIONAL] boolean IncludeQualifiers = false,
                    [IN,OPTIONAL] boolean IncludeClassOrigin = false,
                    [IN,OPTIONAL,NULL] string PropertyList [] = NULL
                */
                case ID_References  :
                    if (!_getObjectNameInput(argc, argv, opts, true))
                        exit(1);
                    references(client, opts);
                    break;

                case ID_ReferenceNames :
                    if (!_getObjectNameInput(argc, argv, opts, true))
                        exit(1);
                    referenceNames(client, opts);
                    break;

                case ID_Associators :
                    if (!_getObjectNameInput(argc, argv, opts, true))
                        exit(1);
                    associators(client, opts);
                    break;

                case ID_AssociatorNames :
                    if (!_getObjectNameInput(argc, argv, opts, true))
                        exit(1);
                    associatorNames(client,opts);
                    break;

                case ID_EnumerateNamespaces :
                    // Note that the following constants are fixed here.  We
                    // should be getting them from the environment to assure
                    // that others know that we are using them.
                    opts.className = CIMName("CIM_Namespace");
                    if (argc > 2)
                    {
                        opts.nameSpace = argv[2];
                        opts.inputObjectName = argv[2];
                    }
                    else
                        // set nameSpace to interop namespace name
                        opts.nameSpace =
                            PEGASUS_NAMESPACENAME_INTEROP.getString();

                    enumerateNamespaces_Namespace(client,opts);
                    break;

                    /*
                        CIMValue invokeMethod(
                            const CIMNamespaceName& nameSpace,
                            const CIMObjectPath& instanceName,
                            const CIMName& methodName,
                            const Array<CIMParamValue>& inParameters,

                    Array<CIMParamValue>& outParameters
                    */
                case ID_InvokeMethod :
                    if (argc < 4)
                    {
                        cout << "Usage: InvokeMethod requires that object and"
                            " method names be specified.\n"
                            "Input parameters are optional and can be"
                            " specified with the -ip option or as"
                            " additional parameters to this call. "
                            "Enter each input parameter as name=value"
                            " (no spaces around equal sign)."
                            << endl;
                        exit(1);
                    }
                    opts.objectName = argv[2];
                    opts.inputObjectName = argv[2];
                    opts.methodName = CIMName(argv[3]);

                    // If there are any extra arguments they must be parameters
                    // These parameters  can be used in addtion to parameters
                    // ifrom the -ip option setting. Parameters found here must
                    // be key=value pairs or they will generate an exception.
                    if (argc > 4)
                    {
                        // get input params from command line
                        for (Sint32 i = 4 ; i < argc; i++)
                        {
                            CIMParamValue pv;
                            String s = argv[i];
                            pv = _createMethodParamValue(s, opts);
                            opts.inParams.append(pv);
                        }
                    }
                    invokeMethod(client, opts);
                    break;

                case ID_ShowOptions :
                    showUsage();
                    break;

                case ID_ExecQuery:
                    opts.query = argv[2];
                    if (argc==4)
                        opts.queryLanguage = argv[3];
                    executeQuery(client, opts);
                    break;

                case ID_StatisticsOn:
                    setObjectManagerStatistics(client, true);
                    break;

                case ID_StatisticsOff:
                    setObjectManagerStatistics(client, false);
                    break;

                //case ID_Unknown :
                default:
                    cout << "Invalid Command. Command name must be first parm"
                            " or --c parameter."
                        << " \n  ex. cli enumerateclasses\n"
                        << "Enter " << argv[0] << " -h for help."
                        << endl;
                    exit(1);
                    break;
            }
            if (opts.repeat > 0)
            {
                if (opts.verboseTest)
                {
                    cout << "Repetitition " << opts.repeat << endl;
                }
                opts.repeat--;
                if (opts.time)
                {
                    totalTime += opts.saveElapsedTime;
                    maxTime = LOCAL_MAX(maxTime, opts.saveElapsedTime);
                    minTime = LOCAL_MIN(minTime, opts.saveElapsedTime);
                    rtTotalTime += (returnedPerformanceData.roundTripTime);
                    maxRtTime = LOCAL_MAX(maxRtTime,
                            returnedPerformanceData.roundTripTime);
                    minRtTime = LOCAL_MIN(minRtTime,
                            returnedPerformanceData.roundTripTime);

                    if (returnedPerformanceData.serverTimeKnown)
                    {
                        serverTotalTime += (returnedPerformanceData.serverTime);
                        maxServerTime = LOCAL_MAX(maxServerTime,
                                returnedPerformanceData.serverTime);
                        minServerTime = LOCAL_MIN(minServerTime,
                                returnedPerformanceData.serverTime);
                    }
                }
            }
        } while (opts.repeat > 0  );

        if (opts.time)
        {
            if (repeatCount == 0)
            {
                cout << CommandTable[cmdIndex].CommandName
                    << " "
                    << opts.inputObjectName
                    << " Time= "
                    << opts.saveElapsedTime
                    << " Sec "
                    << " SvrTime= "
                    << CIMValue(returnedPerformanceData.serverTime).toString()
                    << " us "
                    << " RtTime= "
                    << CIMValue(returnedPerformanceData.roundTripTime).
                           toString()
                    << " us "
                    << "Req size= "
                    << CIMValue(returnedPerformanceData.requestSize).toString()
                    << " b Resp size= "
                    << CIMValue(returnedPerformanceData.responseSize).toString()
                    << " b"
                    << endl;
            }
            else
            {
                cout << CommandTable[cmdIndex].CommandName
                    << " "
                    << opts.inputObjectName
                    << " Total Time "
                    << totalTime
                    << " for "
                    << repeatCount
                    << " ops. Avg= "
                    << (totalTime * 1000000)/repeatCount
                    << " us min= "
                    << minTime * 1000000
                    << " us max= "
                    << (maxTime * 1000000)
                    << " us SvrTime avg= "
                    << CIMValue(serverTotalTime/repeatCount).toString()
                    << " us SvrTime min= "
                    << CIMValue(minServerTime).toString()
                    << " us SvrTime max= "
                    << CIMValue(maxServerTime).toString()
                    << " us"
                    << " RtTime avg= "
                    << CIMValue(rtTotalTime/repeatCount).toString()
                    << " us RtTime min= "
                    << CIMValue(minRtTime).toString()
                    << " us RtTime max= "
                    << CIMValue(maxRtTime).toString()
                    << " us"
                    << endl;
            }
        }
    }
    catch(CIMException& e)
    {
        cerr << argv[0] << " CIMException: "<<" Cmd= " << opts.cimCmd
            << " Object= " << opts.inputObjectName
             << "\n" << e.getMessage()
             << endl;
        opts.termCondition = 1;
    }
    catch(Exception& e)
    {
        PEGASUS_STD(cerr) << argv[0] << " Pegasus Exception: " << e.getMessage()
                <<  ". Cmd = " << opts.cimCmd
                << " Object = " << opts.inputObjectName
                << PEGASUS_STD(endl);
            opts.termCondition = 1;
    }
    catch(...)
    {
        cerr << argv[0] << " Caught General Exception:" << endl;
        opts.termCondition = 1;
    }

    totalElapsedExecutionTime.stop();

    if (opts.time)
    {
        // if abnormal term, dump all times
        if (opts.termCondition == 1)
        {
            cout << "Exception" << endl;
            cout << "Prev Time " << opts.saveElapsedTime << " Sec" << endl;
            opts.saveElapsedTime = opts.elapsedTime.getElapsed();
            cout << "Last Time " << opts.saveElapsedTime << " Sec" << endl;
            cout << "Total Time " << totalTime << " for "
                << repeatCount << " operations. Avg.= " << totalTime/repeatCount
                << " min= " << minTime << " max= " << maxTime << endl;
        }

        cout << "Total Elapsed Time= " << totalElapsedExecutionTime.getElapsed()
             << " Sec. Terminated at " << System::getCurrentASCIITime() << endl;
    }
    if (opts.delay != 0)
    {
        Threads::sleep(opts.delay * 1000);
    }
    return(opts.termCondition);
}

//PEGASUS_NAMESPACE_END

// END_OF_FILE
