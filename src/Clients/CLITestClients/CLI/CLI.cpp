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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Clients/CLITestClients/CLIClientLib/CLIClientLib.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
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

    CheckCommonOptionValues(om, argv, opts);
    
    /*cout << "argc = " << argc << endl;
    for (Uint32 i = 0; i < argc; i++)
    {
        cout << "argv[" << i << "] = " << argv[i] << endl;
    }*/
    // if there is still an arg1, assume it is the command name.
    if (argc > 1)
    {
        opts.cimCmd = argv[1];
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
        client.connect(opts.location, opts.user, opts.password);
    }    
    catch(CIMClientException& e)
    {
        cerr << "CIMClientException connecting to : " << opts.location << " "
          << e.getMessage() << endl;
    }
    catch(Exception &e) 
    {
        cerr << "Pegasus Exception: " << e.getMessage() <<
              " connecting to " << opts.location << endl;
    }

    try
    {
        Uint32 i = 0;
        opts.cimCmd.toLower();
        
        for( ; i < NUM_COMMANDS; i++ ) 
        {
            if (opts.cimCmd == CommandTable[i].CommandName)
            break;
        }
                // make sure command exist by checking value of i
        if ( i < NUM_COMMANDS)
        {
            switch(CommandTable[i].ID_Command)
            {
            case ID_EnumerateInstanceNames :
                if (argc > 2)
                {
                    opts.className = argv[2];
                }
                enumerateInstanceNames(client, opts);
                break;

            case ID_EnumerateInstances :
                {
                if (argc < 2)
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
                    {
                        opts.className = argv[2];
                    }
                    enumerateClassNames(client, opts);
                }
                
                break;

            case ID_EnumerateClasses :
                {
                    if (argc > 2)
                    {
                        opts.className = argv[2];
                    }
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
                
                
                
            case ID_Unknown :
                cout << "Invalid Command. Must be first parm or --c parm" << endl;
                break;
            }
        }
    }
    catch(CIMClientException& e)
    {
        PEGASUS_STD(cerr) << argv[0] << " CIMClientException : " << e.getMessage()
              <<  " ClassName = " << opts.className << PEGASUS_STD(endl);
    }
    catch(Exception& e)
    {
        PEGASUS_STD(cerr) << argv[0] << " Pegasus Exception: " << e.getMessage() 
                <<  " ClassName = " << opts.className << PEGASUS_STD(endl);
            exit(1);
    }
    
    return 0;
}
