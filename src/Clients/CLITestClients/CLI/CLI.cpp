//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
         om.print();
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
    opts.cimObjectPath = "";
    opts.user = "wbemuser";
    opts.password = "wbemuser";
    
    opts.localOnly = false;
    opts.deepInheritance = false;
    opts.includeQualifiers = false;
    opts.includeClassOrigin = false;

    CheckCommonOptionValues(om, argv, opts);

    CIMClient client;
    try
    {
        client.connect(opts.location);
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
                enumerateInstanceNames(client, opts);
                break;

            case ID_EnumerateInstances :
                {
                enumerateInstances(client, opts);
                }
                break;
            case ID_GetInstance :
                getInstance(client, opts);
                break;
                
            case ID_EnumerateClassNames :
                enumerateClassNames(client, opts);
                break;

            case ID_EnumerateClasses :
                {
                enumerateClasses(client, opts);
                }
                break;

            case ID_GetClass :
                getClass(client, opts);
                break;
                
            case ID_CreateInstance :
                {
                }
                break;
            case ID_DeleteInstance :
                {
                    deleteInstance(client, opts);
                }
                break;
                
            case ID_CreateClass :
                break;
            case ID_DeleteClass :
                {
                    deleteClass(client, opts);
                }
                break;
                
            case ID_GetProperty :
                {
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
                    getQualifier(client, opts);
                }
                break;
            case ID_DeleteQualifier :
                {
                    deleteQualifier(client, opts);
                }
                
                
                
            case ID_Unknown :
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
