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
// Author:  Aruran, IBM (aruran.shanmug@in.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <../test/StressTests/testClient/TestStressTestClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean verboseTest = false;

Boolean _quit = false;
Boolean _nextCheck = false;
int status = CLIENT_UNKNOWN;    
char clientName[] = "ModelWalkStressClient";

String errorInfo;

/** Signal handler for SIGALRM.
    @param   signum  the alarm identifier */
void endTest(int signum)
{
    PEGASUS_STD(cout) << "\nRecieved interrupt signal SIGINT!\n" << PEGASUS_STD(endl);
    _quit = true;
}

class TestModelWalkStressClient:public TestStressTestClient
{
};

/*//////////////////////////////////////////////////////////////////////////
//  GetNameSpaces
    If no nameSpace is supplied on the command line, this method looks into
    the root and test nameSpaces for the _Namespace class. The names of the 
    instances of that class are retrued by this method.
//////////////////////////////////////////////////////////////////////////*/
Array<CIMNamespaceName> getNameSpaces(
     TestModelWalkStressClient &tmsc,
     CIMClient* client,
     OptionManager &om,
     pid_t clientPid,
     String& clientlog,
     String &clientid,
     int status,
     String &pidfile)
{
    Array<CIMNamespaceName> topNamespaceNames;
    Array<CIMNamespaceName> returnNamespaces;
    String tmpNamespace;

    om.lookupValue("namespace",tmpNamespace);
    if (tmpNamespace != String::EMPTY)
    {
        returnNamespaces.append(CIMNamespaceName (tmpNamespace));
    }
    else
    {
        // Get all namespaces for display using the __Namespaces function.
        CIMName className = "__NameSpace";

        //We have to add any new top level root namespace if added to repository..
        topNamespaceNames.append("root");
        topNamespaceNames.append("test");

        Uint32 start = 0;
        Uint32 end = topNamespaceNames.size();

        // for all new elements in the output array
        for (Uint32 range = start; range < end; range ++)
        {
            // Get the next increment in naming for all name element in the array
            Array<CIMInstance> instances = client->enumerateInstances
                                           (topNamespaceNames[range], className);
            if (status != CLIENT_PASS)
            {
                status = CLIENT_PASS;
                tmsc.logInfo(clientid, clientPid, status, pidfile);
            }
            for (Uint32 i = 0 ; i < instances.size(); i++)
            {
                Uint32 pos;
                // if we find the property and it is a string, use it.
                if ((pos = instances[i].findProperty("name")) != PEG_NOT_FOUND)
                {
                    CIMValue value;
                    String namespaceComponent;
                    value = instances[i].getProperty(pos).getValue();
                    if (value.getType() == CIMTYPE_STRING)
                    {
                        value.get(namespaceComponent);
                        String ns = topNamespaceNames[range].getString();
                        ns.append("/");
                        ns.append(namespaceComponent);
                        returnNamespaces.append(ns);
                    }
                }
            }
        }
    }//else block ends here...

    if (verboseTest)
    {
        errorInfo.clear();
        errorInfo.append("+++++ Successfully Enumerated all Namespaces that have ");
        errorInfo.append("a _NameSpace instance defined for them in the root of test namespaces +++++");
        tmsc.errorLog(clientPid, clientlog, errorInfo);
        errorInfo.clear();
    }
    return returnNamespaces;
}
/*//////////////////////////////////////////////////////////////////////////
//  EnumerateAllQualifiers
    This method enumerates all the qualifiers in each of the nameSpaces
    of the "nameSpacesArray"
//////////////////////////////////////////////////////////////////////////*/
static void enumerateAllQualifiers(
    TestModelWalkStressClient &tmsc,
    CIMClient* client,
    Array<CIMNamespaceName> nameSpacesArray,
    pid_t clientPid,
    String& clientlog,
    String &clientid,
    int status,
    String &pidfile)
{
    Array<CIMQualifierDecl> qualifierDecls;
    qualifierDecls.clear();

    for (Uint32 i=0; i < nameSpacesArray.size();i++)
    {
        try
        {
            qualifierDecls = client->enumerateQualifiers(nameSpacesArray[i]);

            if (status != CLIENT_PASS)
            {
                status = CLIENT_PASS;
                tmsc.logInfo(clientid, clientPid, status, pidfile);
            }
        }
        //the following exceptoins will be caught in the Main method
        catch (CIMException &e)
        {
            errorInfo.clear();
            errorInfo.append("CIMException in enumerateQualifiers : ");
            errorInfo.append(e.getMessage());
            CIMException cimExp(e.getCode(), errorInfo);
            throw cimExp;
        }
        catch (Exception &e)
        {
            errorInfo.clear();
            errorInfo.append("Exception in enumerateQualifiers : ");
            errorInfo.append(e.getMessage());
            Exception exp(errorInfo);
            throw exp;
        }
        catch (...)
        {
            Exception exp("General Exception in enumerateQualifiers ");
            throw exp;
        }
    }

    if (verboseTest)
    {
        errorInfo.clear();
        errorInfo.append("+++++ Successfully Enumerated Qualifiers for Namespaces +++++");
        tmsc.errorLog(clientPid, clientlog, errorInfo);
        errorInfo.clear();
    }
}
/*//////////////////////////////////////////////////////////////////////////
//  EnumerateReferenceNames
    This method enumerates the references to each instance in the array
    "cimInstances" for the "nameSpace" 
//////////////////////////////////////////////////////////////////////////*/
static void enumerateReferenceNames(
    TestModelWalkStressClient &tmsc,
    CIMClient* client, 
    Array<CIMInstance> cimNInstances,
    CIMNamespaceName nameSpace,
    CIMName referenceClass,
    pid_t clientPid,
    String& clientlog,
    String &clientid,
    int status,
    String &pidfile)
{
    String role = String::EMPTY;
    Array<CIMObjectPath> resultObjectPaths;

    for (Uint32 i=0; i < cimNInstances.size(); i++)
    {
        try
        {
            resultObjectPaths = client->referenceNames(
                                                      nameSpace, cimNInstances[i].getPath(),
                                                      referenceClass, role);
            if (status != CLIENT_PASS)
            {
                status = CLIENT_PASS;
                tmsc.logInfo(clientid, clientPid, status, pidfile);
            }
        }
         //the following exceptoins will be caught in the Main method
        catch (CIMException &e)
        {
            errorInfo.clear();
            errorInfo.append("CIMException in enumerateReferenceNames : ");
            errorInfo.append(e.getMessage());
            CIMException cimExp(e.getCode(), errorInfo);
            throw cimExp;
        }
        catch (Exception &e)
        {
            errorInfo.clear();
            errorInfo.append("Exception in enumerateReferenceNames : ");
            errorInfo.append(e.getMessage());
            Exception exp(errorInfo);
            throw exp;
        }
        catch (...)
        {
            Exception exp("General Exception in enumerateReferenceNames ");
            throw exp;
        }
    }
}
/*//////////////////////////////////////////////////////////////////////////
//  EnumerateAssociatorNames
    This method enumerates the associators to each instance in the array
    "cimInstances" for the "nameSpace" 
//////////////////////////////////////////////////////////////////////////*/
static void enumerateAssociatorNames(
    TestModelWalkStressClient &tmsc,
    CIMClient* client,
    Array<CIMInstance> cimNInstances,
    CIMNamespaceName nameSpace,
    CIMName assocClass,
    pid_t clientPid,
    String& clientlog,
    String &clientid,
    int status,
    String &pidfile)
{
    CIMName resultClass = CIMName();
    String role = String::EMPTY;
    String resultRole = String::EMPTY;
    Array<CIMObjectPath> resultObjectPaths;

    for (Uint32 i=0; i < cimNInstances.size(); i++)
    {
        try
        {
            resultObjectPaths = client->associatorNames( nameSpace, cimNInstances[i].getPath(),
                                                         assocClass, resultClass, role, resultRole);
            if (status != CLIENT_PASS)
            {
                status = CLIENT_PASS;
                tmsc.logInfo(clientid, clientPid, status, pidfile);
            }
        }
         //the following exceptoins will be caught in the Main method
        catch (CIMException &e)
        {
            errorInfo.clear();
            errorInfo.append("CIMException in enumerateAssociatorNames : ");
            errorInfo.append(e.getMessage());
            CIMException cimExp(e.getCode(), errorInfo);
            throw cimExp;
        }
        catch (Exception &e)
        {
            errorInfo.clear();
            errorInfo.append("Exception in enumerateAssociatorNames : ");
            errorInfo.append(e.getMessage());
            Exception exp(errorInfo);
            throw exp;
        }
        catch (...)
        {
            Exception exp("General Exception in enumerateAssociatorNames ");
            throw exp;
        }
    }
}
/*//////////////////////////////////////////////////////////////////////////
//  EnumerateInstanceRelatedInfo
    This method enumerates instances, referances (by way of subrotine) and 
    associators (by way of subrotine.Exceptions are caught, modified and 
    re-thrown so that generic exception handling can be used in Main
//////////////////////////////////////////////////////////////////////////*/
static void enumerateInstanceRelatedInfo(
    TestModelWalkStressClient &tmsc,
    CIMClient *client, 
    Array<CIMName> classNames,
    CIMNamespaceName nameSpace,
    pid_t clientPid,
    String& clientlog,
    String &clientid,
    int status,
    String &pidfile )
{
    Boolean deepInheritance = true;
    Boolean localOnly = true;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;
    Array<CIMInstance> cimNInstances;

    for (Uint32 i=0; i < classNames.size(); i++)
    {
        try
        {
            cimNInstances = client->enumerateInstances(
                                                      nameSpace, classNames[i], deepInheritance,
                                                      localOnly, includeQualifiers, includeClassOrigin);
            if (status != CLIENT_PASS)
            {
                status = CLIENT_PASS;
                tmsc.logInfo(clientid, clientPid, status, pidfile);
            }
        }
        //the following exceptoins will be caught in the Main method
        catch (CIMException &e)
        {
            errorInfo.clear();
            errorInfo.append("CIMException thrown from enumerateInstanceRelatedInfo, class = ");
            errorInfo.append(classNames[i].getString());
            errorInfo.append("  ");
            errorInfo.append(e.getMessage());
            CIMException cimExp(e.getCode(), errorInfo);
            throw cimExp;
        }
        catch (Exception &e)
        {
            errorInfo.clear();
            errorInfo.append("Exception thrown from enumerateInstanceRelatedInfo, class = ");
            errorInfo.append(classNames[i].getString());
            errorInfo.append("  ");
            errorInfo.append(e.getMessage());
            Exception exp(errorInfo);
            throw exp;
        }
        catch (...)
        {
            Exception exp("General Exception in EnumerateInstanceRelatedInfo ");
            throw exp;
        }

        enumerateReferenceNames (tmsc, client, cimNInstances, nameSpace, CIMName(),
                                 clientPid, clientlog, clientid, status, pidfile);
        enumerateAssociatorNames (tmsc, client, cimNInstances, nameSpace, CIMName(),
                                  clientPid, clientlog, clientid, status, pidfile);
        if (verboseTest)
        {
            errorInfo.clear();
            errorInfo.append("+++++ Successfully Done Instance Operations on class = ");
		    errorInfo.append(classNames[i].getString());
            tmsc.errorLog(clientPid, clientlog, errorInfo);
            errorInfo.clear();
        }
    }

}
/*//////////////////////////////////////////////////////////////////////////
//  EnumerateClassRelatedInfo
    This method enumerates classes and instances (by way of subrotine)  
//////////////////////////////////////////////////////////////////////////*/
static void enumerateClassRelatedInfo (
    TestModelWalkStressClient &tmsc,
    CIMClient* client,
    OptionManager &om,
    Array<CIMNamespaceName> nameSpacesArray,
    pid_t clientPid,
    String& clientlog,
    String &clientid,
    int status,
    String &pidfile)
{
    Array<CIMName> classNames;
    String tmpClassName;

   
    om.lookupValue("classname",tmpClassName);
    if (tmpClassName != String::EMPTY)
    {
        classNames.append(CIMName(tmpClassName));
        enumerateInstanceRelatedInfo(tmsc, client, classNames, *nameSpacesArray.getData(),
                                     clientPid, clientlog, clientid, status, pidfile);
    }
    else
    {
        Boolean deepInheritance = true;
        CIMName className;
        for (Uint32 i=0; i < nameSpacesArray.size();i++)
        {
            classNames = client->enumerateClassNames(nameSpacesArray[i],
                                                     className, deepInheritance);
            if (status != CLIENT_PASS)
            {
                status = CLIENT_PASS;
                tmsc.logInfo(clientid, clientPid, status, pidfile);
            }
            enumerateInstanceRelatedInfo(tmsc, client, classNames, nameSpacesArray[i],
                                         clientPid, clientlog, clientid, status, pidfile );
        }
    }
      if (verboseTest)
    {
        errorInfo.clear();
        errorInfo.append("+++++ Successfully Enumerated classes in Namespaces +++++");
        tmsc.errorLog(clientPid, clientlog, errorInfo);
        errorInfo.clear();
    }
}
///////////////////////////////////////////////////////////////
//    MAIN
///////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    OptionManager om;
    TestModelWalkStressClient tmsc;
    Uint32 validArg = 0;
    struct OptionRow *newOptionsTable = 0;
    Uint32 newOptionsCount;
    Uint32 cOptionCount =  0;

    //Varriables need to connect to server
    Boolean useSSL;
    String host;
    Uint32 portNumber = 0;
    String portStr;
    String userName;
    String password;
    String help;
    Boolean connectedToHost = false;

    //Varriables needed to do loging and status checking
    String pidfile;
    String clientid;
    pid_t clientpid;
    String clientlog, stopClient;
    char pid_str[15];
    Uint32 successCount=0;        //Number of times the command succeeded.
    Uint32 iteration=0;           //Number of iterations after which logErrorPercentage() is called.
    Uint32 totalCount=0;          //Total number of times the command was executed.

    // timeout
    Uint32 timeout = 30000;

    //This try block includes all the options gathering function
    try
    {
        // client specific options if any
        struct OptionRow *cOptionTable = 0; 
        newOptionsCount = cOptionCount;

        try
        {
            //Generate new option table for this client using OptionManager
            newOptionsTable = tmsc.generateClientOptions(cOptionTable,cOptionCount,newOptionsCount);
            validArg = tmsc.GetOptions(om, argc, argv, newOptionsTable,newOptionsCount);
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

        om.lookupValue("username", userName);

        om.lookupValue("password", password);

        om.lookupIntegerValue("timeout",timeout);

        useSSL =  om.isTrue("ssl");

        om.lookupValue("hostname",host);
        om.lookupValue("port",portStr);

        if (portStr.getCString())
        {
            sscanf (portStr.getCString (), "%u", &portNumber);
        }
        
        //  Setting default ports 
        if (!portNumber)
        {
            if (useSSL)
            {
                portNumber = 5989;
            }
            else
            {
                portNumber = 5988;
            }
        }

        //  default host is localhost.
        if (host == String::EMPTY)
        {
            host = String("localhost");
        }

        //Signal Handling - SIGINT
        signal(SIGINT, endTest);

        om.lookupValue("clientid", clientid);

        om.lookupValue("pidfile", pidfile);

        clientpid = getpid();
       
        om.lookupValue("clientlog", clientlog);

        om.lookupValue("help", help);

    }// end of option Try block
    catch (Exception& e)
    {
        cerr << argv[0] << ": " << e.getMessage() << endl;
        exit(1);
    }
    catch (...)
    {
        cerr << argv[0] << ": Unknown Error gathering options in ModelWalk client  " << endl;
        exit(1);
    }

    /** // Checking whether the user asked for HELP Info...
    if (om.valueEquals("help", "true"))
    {
        String header = "Usage ";
        header.append(argv[0]);
        header.append(" -parameters -n [namespace] -c [classname] ");
        header.append(" -clientid [clientid] -pidfile [pidfile] -clientlog [clientlog]");
        String trailer = "Assumes localhost:5988 if host not specified";
        trailer.append("\nHost may be of the form name or name:port");
        trailer.append("\nPort 5988 assumed if port number missing.");
        om.printOptionsHelpTxt(header, trailer);

        exit(0);
    } */

    try
    {
        //Timer Start
        tmsc.startTime();

        tmsc.logInfo(clientid, clientpid, status, pidfile);

        // connect the client
        CIMClient* client = new CIMClient();

        sprintf(pid_str, "%d", clientpid);

        stopClient = String::EMPTY;
        stopClient.append(FileSystem::extractFilePath(pidfile));
        stopClient.append("STOP_");
        stopClient.append(pid_str);

        // This loop executes till the client gets stop signal from controller
        while (!_quit)
        {
            if (FileSystem::exists(stopClient))
            {
                if (verboseTest)
                {
                    String mes("Ending client: ");
                    mes.append((Uint32)clientpid);
                    tmsc.errorLog(clientpid, clientlog, mes); 
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

            if (!connectedToHost)
            {
                try
                {
                    tmsc.connectClient( client, host, portNumber, userName, password, useSSL,
                                        timeout, verboseTest);
                    connectedToHost = true;
                    
                   //  Client has successfully connected. to server
                   //   update status if previously not Success.
                   //
                   if (status != CLIENT_PASS)
                   {
                       status = CLIENT_PASS;
                       tmsc.logInfo(clientid, clientpid, status, pidfile);
                   }

                }
                catch (CannotConnectException)
                {
                    status = CLIENT_UNKNOWN;
                    tmsc.logInfo(clientid, clientpid, status, pidfile);
                    connectedToHost = false;
                }
                catch (CIMException &e)
                {
                    status = CLIENT_UNKNOWN;
                    tmsc.logInfo(clientid, clientpid, status, pidfile);
                    connectedToHost = false;
                    String mes(e.getMessage());
                    tmsc.errorLog(clientpid, clientlog, mes);
                }
                catch (Exception &e)
                {
                    status = CLIENT_UNKNOWN;
                    tmsc.logInfo(clientid, clientpid, status, pidfile);
                    connectedToHost = false;
                    String mes(e.getMessage());
                    tmsc.errorLog(clientpid, clientlog, mes);
                }
                catch (...)
                {
                    status = CLIENT_UNKNOWN;
                    tmsc.logInfo(clientid, clientpid, status, pidfile);
                    connectedToHost = false;
                    String msg("Error connencting to server in ModleWalk client ");
                    tmsc.errorLog(clientpid, clientlog,msg);
                }
            }// end of if (!connectedToHost)
            iteration++;
            totalCount++;
            if (connectedToHost)
            {
                try
                {
                    Array<CIMNamespaceName> nameSpacesArray;
  
                    // Enumerate all the namespaces here....
                    nameSpacesArray = getNameSpaces(tmsc, client, om,
                        clientpid, clientlog, clientid, status, pidfile);

                    //Enumerate all qualifiers in the namespaces....
                    enumerateAllQualifiers(tmsc, client, nameSpacesArray,
                        clientpid, clientlog, clientid, status, pidfile);

                    // Enumerate all the class related info here....
                    enumerateClassRelatedInfo(tmsc, client, om, nameSpacesArray,
                        clientpid, clientlog, clientid, status, pidfile);

                    successCount++;
                }
                /* This specail catch block in needed so that we will know if the connenction 
                   was lost. We then connect on the next time through the loop*/
                catch (CannotConnectException)
                {
                    status = CLIENT_UNKNOWN;
                    tmsc.logInfo(clientid, clientpid, status, pidfile);
                    connectedToHost = false;
                }
                catch (CIMException &cimE)
                {
                    status = CLIENT_FAIL;
                    tmsc.logInfo(clientid, clientpid, status, pidfile);
                    String mes(cimE.getMessage());
                    tmsc.errorLog(clientpid, clientlog, mes);
                }
                catch (Exception &exp)
                {
                    status = CLIENT_FAIL;
                    tmsc.logInfo(clientid, clientpid, status, pidfile);
                    String mes(exp.getMessage());
                    tmsc.errorLog(clientpid, clientlog, mes);
                }
                catch (...)
                {
                    status = CLIENT_FAIL;
                    tmsc.logInfo(clientid, clientpid, status, pidfile);
                    String err_msg("Unknown Error during ModelWalk Execution");
                    tmsc.errorLog(clientpid, clientlog, err_msg);
                }

                _nextCheck = tmsc.checkTime();
                if (_nextCheck)
                {
                    tmsc.logInfo(clientid, clientpid, status, pidfile);
                    _nextCheck = false;
                }

                /* If verbose is set log success percentage for every 100 iterations.
                   If verbose is not set log success percentage for every 10000 iterations
                */
                if (verboseTest)
                {
                    if (iteration==100)
                    {
                        tmsc.logErrorPercentage(
                            successCount, 
                            totalCount, 
                            clientpid,
                            clientlog,
                            clientName);
                        iteration = 0;
                    }
                }
                else
                {
                    if (iteration==1000)
                    {
                        tmsc.logErrorPercentage(
                            successCount,
                            totalCount,
                            clientpid,
                            clientlog,
                            clientName);
                        iteration = 0;
                    }
                }
            }
        } // end of while (!_quit)
    }
    catch (Exception &exp)
    {
        String exp_str("Exception in ModelWalk client causing it to exit: ");
        exp_str.append(exp.getMessage());
        tmsc.errorLog(clientpid, clientlog, exp_str);

        if (verboseTest)
        {
            PEGASUS_STD(cerr) << exp_str.getCString() << PEGASUS_STD(endl);
        }
    }
    catch (...)
    {
        String exp_str("General Exception in ModelWalk causing it to exit");
        tmsc.errorLog(clientpid, clientlog, exp_str);

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
    if (FileSystem::exists(stopClient))
    {
        // Remove STOP file here 
        FileSystem::removeFile(stopClient);
    }
   
    if (verboseTest)
    {
        errorInfo.clear();
        errorInfo.append("+++++ TestModelWalkStressClient Terminated Normally +++++");
        tmsc.errorLog(clientpid, clientlog, errorInfo);
        errorInfo.clear();
    }
    return 0;
}

