//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author:  Karl Schopmeyer (k.schopmeyer@opengroup.org) 
//          Mary Hinton (m.hinton@verizon.net)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Clients/CLITestClients/CLIClientLib/CLIClientLib.h>
#include <Pegasus/Common/Constants.h>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments from the command
    // line.

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments from the command
    // line.

    OptionManager om;

    try
    {
        String testHome = ".";
        GetOptions(om, argc, argv, testHome);
        //om.print();
    }
    catch (Exception& e)
    {
        cerr << argv[0] << ": " << e.getMessage() << endl;
        exit(1);
    }
    
    Options opts;
    
    opts.location =   "localhost:5988";
    opts.nameSpace = "root/cimv2";
    opts.cimCmd = "unknown";
    opts.className = "unknown";
    opts.objectName = "unknown";
    opts.outputFormat;
    opts.outputFormatType = OUTPUT_MOF;
    opts.cimObjectPath = "";
    opts.user = String::EMPTY;
    opts.password = String::EMPTY;
    opts.verboseTest = false;
    
    opts.localOnly = false;
    opts.deepInheritance = false;
    opts.includeQualifiers = false;
    opts.includeClassOrigin = false;
    opts.resultClassName = String::EMPTY;
    opts.role = String::EMPTY;
    opts.propertyListText = String::EMPTY;
    opts.propertyName = String::EMPTY;

    CheckCommonOptionValues(om, argv, opts);
    
    /****** Show the args diagnostic display
    cout << "argc = " << argc << endl;
    for (Uint32 i = 0; i < argc; i++)
    {
        cout << "argv[" << i << "] = " << argv[i] << endl;
    }
    */
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


    CIMClient client;
    try
    {
        if (opts.verboseTest)
        {
            cout << "Connecting to " << opts.location
                 << " for User = " << opts.user 
                 << " password = " << opts.password
                 << endl;
        }
        // Take off port number if it is on host name
        Uint32 index = opts.location.find (':');
        String host = opts.location.subString (0, index);
        
        Uint32 portNumber = WBEM_DEFAULT_HTTP_PORT;
        if (index != PEG_NOT_FOUND)
        {
            String portStr = opts.location.subString (index + 1,
                opts.location.size ());
            sscanf (portStr.getCString (), "%u", &portNumber);
        }
        client.connect(host, portNumber, opts.user, opts.password);
    }    
    catch(Exception &e) 
    {
        cerr << "Pegasus Exception: " << e.getMessage() <<
              ". Trying to connect to " << opts.location << endl;
        return 0;
    }                                                       ;

    try
    {
        Uint32 i = 0;
        opts.cimCmd.toLower();
        if (opts.verboseTest)
            cout << "TEST Command = " << opts.cimCmd << endl;
        
        // Find the command
        for( ; i <= NUM_COMMANDS; i++ ) 
        {
            if ((opts.cimCmd == CommandTable[i].CommandName) || 
                (opts.cimCmd == CommandTable[i].ShortCut))

                break;
        }
        // or exit with error
        if ( i > NUM_COMMANDS)
        {
            cout << "Invalid Command - " << argv[1] 
                << " Command name must be first parameter or --c parameter."
                << " \n  ex. cli enumerateclasses\n" 
                << "Enter " << argv[0] << " -h for help."
                << endl;
            exit(1);
        }
        
        switch(CommandTable[i].ID_Command)
            {
            case ID_EnumerateInstanceNames :
                if (argc > 2)
                {
                    opts.className = argv[2];
                }
                enumerateInstanceNames(client, opts);
                break;
                
            case ID_EnumerateAllInstanceNames :
                if (argc > 2)
                {
                    opts.className = argv[2];
                }
                enumerateAllInstanceNames(client, opts);
                break;

            case ID_EnumerateInstances :
                {
                    if (argc > 2)
                    {
                        opts.className = argv[2];
                    }
                    enumerateInstances(client, opts);
                }
                break;
            case ID_GetInstance :
                {
                    if (argc > 2)
                    {
                        opts.className = argv[2];
                    }
                    getInstance(client, opts);
                }
                break;
                
            case ID_EnumerateClassNames :
                {
                    if (argc > 2)
                        opts.className = argv[2];
                    
                    if (argc == 2)
                        opts.className = "";
                    
                    enumerateClassNames(client, opts);
                }
                
                break;

            case ID_EnumerateClasses :
                {
                    if (argc > 2)
                        opts.className = argv[2];
                    
                    if (argc == 2)
                        opts.className = "";
                    
                    enumerateClasses(client, opts);
                }
                break;

            case ID_GetClass :
                if (argc > 2)
                {
                    opts.className = argv[2];
                }
                getClass(client, opts);
                break;
                
            case ID_CreateInstance :
                {
                    cout << "Not Implemented" << endl;
                }
                break;
            case ID_DeleteInstance :
                {
                    if (argc > 2)
                    {
                        opts.className = argv[2];
                    }
                    deleteInstance(client, opts);
                }
                break;
                
            case ID_CreateClass :
                break;
            case ID_DeleteClass :
                {
                    if (argc > 2)
                    {
                        opts.className = argv[2];
                    }
                    deleteClass(client, opts);
                }
                break;
                
            case ID_GetProperty :
                {
                    // KS rewrite all this more organized.  The Get
                    if (argc != 4)
                    {
                        cout << "Usage: cli getproperty instancename propertyname" << endl;
                    }
                    if (argc > 2)
                    {
                        opts.instanceName = argv[2];
                    }
                    if (argc > 3)
                    {
                        opts.propertyName = argv[3];
                    }
                    getProperty(client, opts);
                }
                break;
            case ID_SetProperty :
                
                {
                    if (argc != 5)
                    {
                        cout << "Usage: cli setproperty instancename propertyname value " << endl;
                    }
                    
                    setProperty(client, opts);
                } 
                break;
            case ID_EnumerateQualifiers :
                {
                    enumerateQualifiers(client, opts);
                }
                break;
            case ID_GetQualifier :
                {
                    if (argc > 2)
                    {
                        opts.qualifierName = argv[2];
                    }
                    getQualifier(client, opts);
                }
                break;
            case ID_DeleteQualifier :
                {
                    if (argc > 2)
                    {
                        opts.qualifierName = argv[2];
                    }
                    deleteQualifier(client, opts);
                }
                break;
                
        /* params are
            [IN] <objectName> ObjectName, 
            [IN,OPTIONAL,NULL] <className> ResultClass = NULL, 
            [IN,OPTIONAL,NULL] string Role = NULL, 
            [IN,OPTIONAL] boolean IncludeQualifiers = false, 
            [IN,OPTIONAL] boolean IncludeClassOrigin = false, 
            [IN,OPTIONAL,NULL] string PropertyList [] = NULL 
        */
        case ID_References  :
                {
                    if (argc > 2)
                    {
                        opts.objectName = argv[2];
                    }
                    references(client, opts);
                }
                break;
        case ID_ReferenceNames :
                {
                    if (argc > 2)
                    {
                        opts.objectName = argv[2];
                    }
                    referenceNames(client, opts);
                }
                break;
        case ID_Associators :
                {
                    if (argc > 2)
                    {
                        opts.objectName = argv[2];
                    }
                }
                break;
        case ID_AssociatorNames :
                {
                    if (argc > 2)
                    {
                        opts.objectName = argv[2];
                    }
                    associatorNames(client,opts);
                }
                break;
        case ID_EnumerateNamespaces :
                {
                    opts.className = "__namespace";
                    if (argc > 2)
                    {
                        opts.nameSpace = argv[2];
                    }
                    else
                        opts.nameSpace = "";
                    cout << "KSTEST " << argc << " " << opts.nameSpace << endl;
                    enumerateInstanceNames(client,opts);
                }
                break;
                
            //case ID_Unknown :
            default:
                cout << "Invalid Command. Command name must be first parm or --c parameter."
                    << " \n  ex. cli enumerateclasses\n" 
                    << "Enter " << argv[0] << " -h for help."
                    << endl;
                break;
        }
    }
    catch(Exception& e)
    {
        PEGASUS_STD(cerr) << argv[0] << " Pegasus Exception: " << e.getMessage() 
                <<  ". Cmd = " << opts.cimCmd << " Object = " << opts.className << PEGASUS_STD(endl);
            exit(1);
    }
    
    return 0;
}
