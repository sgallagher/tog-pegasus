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

#ifndef _CLI_CLIENTLIB_H
#define _CLI_CLIENTLIB_H

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/OptionManager.h>
#include <Clients/CLITestClients/CLIClientLib/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

typedef int     CommandID;
typedef int     OutputType;
const OutputType    OUTPUT_XML                  = 1;
const OutputType    OUTPUT_MOF                  = 2;
const OutputType    OUTPUT_TEXT                 = 3;
const OutputType    OUTPUT_FILE                 = 4;

const CommandID ID_EnumerateInstanceNames       = 1;
const CommandID ID_EnumerateInstances           = 2;
const CommandID ID_GetClass                     = 3;
const CommandID ID_GetInstance                  = 4;
const CommandID ID_CreateInstance               = 5;
const CommandID ID_DeleteInstance               = 6;
const CommandID ID_CreateClass                  = 7;
const CommandID ID_DeleteClass                  = 8;
const CommandID ID_EnumerateClasses             = 9;
const CommandID ID_EnumerateClassNames          = 10;
const CommandID ID_GetProperty                  = 11;
const CommandID ID_SetProperty                  = 12;
const CommandID ID_GetQualifier                 = 13;
const CommandID ID_SetQualifier                 = 14;
const CommandID ID_EnumerateQualifiers          = 15;
const CommandID ID_DeleteQualifier              = 16;
const CommandID ID_Associators                  = 17;
const CommandID ID_AssociatorNames              = 18;
const CommandID ID_References                   = 19;
const CommandID ID_ReferenceNames               = 20;
const CommandID ID_InvokeMethod                 = 21;
const CommandID ID_ExecQuery                    = 22;

const CommandID ID_ModifyClass                  = 23;
const CommandID ID_ModifyInstance               = 24;

const CommandID ID_Unknown                      = 25;

struct  OUTPUT_STRUCT
{
    // KS the old form OutputType OutputType causes redefinition error in Linux
    int OutputType;
    const char* OutputName;
};

OUTPUT_STRUCT OutputTable[] =
{
    // Output Type      OutputName              
    {   OUTPUT_XML,     "xml"   },
    {   OUTPUT_MOF,     "mof"   },
    {   OUTPUT_TEXT,    "txt"   }
};
const Uint32 NUM_OUTPUTS = sizeof(OutputTable) / sizeof(OutputTable[0]);
    
struct  CMD_STRUCT
{
    CommandID ID_Command;
    const char* CommandName;
};

// ******** CIM Client Commands
CMD_STRUCT CommandTable[] =
{
        // Command ID                CommandName                
    {   ID_EnumerateInstanceNames,  "enumerateinstancenames"    },
    {   ID_EnumerateInstances,      "enumerateinstances"        },
    {   ID_EnumerateClassNames,     "enumerateclassnames"       },
    {   ID_EnumerateClasses,        "enumerateclasses"          },
    {   ID_GetClass,                "getclass"                  },
    {   ID_GetInstance,             "getinstance"               },
    {   ID_CreateInstance,          "createinstance"            },
    {   ID_DeleteInstance,          "deleteinstance"            },
    {   ID_CreateClass   ,          "createclass"               },
    {   ID_DeleteInstance,          "deleteclass"               },
    {   ID_ModifyInstance,          "modifyclass"               },
    {   ID_DeleteClass,             "deleteinstance"            },
    {   ID_GetProperty,             "getproperty"               },
    {   ID_SetProperty,             "setproperty"               },
    {   ID_GetQualifier,            "getqualifier"              },
    {   ID_SetQualifier,            "setqualifier"              },
    {   ID_EnumerateQualifiers,     "enumeratequalifiers"       },
    {   ID_InvokeMethod,            "InvokeMethod"              },
    {   ID_Associators,             "associators"               },
    {   ID_AssociatorNames,         "associatornames"           },
    {   ID_References,              "references"                },
    {   ID_ReferenceNames,          "referencenames"            },
    {   ID_InvokeMethod,            "InvokeMethod"              },
    {   ID_ExecQuery,               "execquery"                 },
    
    {   ID_Unknown,                 "unknown"                   }
//    { 0,                           NULL,                      }
};

const Uint32 NUM_COMMANDS = sizeof(CommandTable) / sizeof(CommandTable[0]);

// ************* Options Functions

struct  OPTION_STRUCT
{
    String location;
    String nameSpace;
    String className;
    String cimCmd;
    String outputFormat;
    Boolean deepInheritance;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMObjectPath instanceName;
    CIMValue newValue;
    String cimObjectPath;
    String qualifierName;
    CIMQualifierDecl qualifierDeclaration;
    String user;
    String password;
    String propertyName;
    CIMPropertyList propertyList;
    Boolean verboseTest;
    Boolean summary;
};

typedef struct OPTION_STRUCT Options;

void PEGASUS_CLI_LINKAGE printHelpMsg(const char* pgmName, const char* usage, const char* extraHelp, 
                OptionManager& om);

void PEGASUS_CLI_LINKAGE printHelp(char* name, OptionManager& om);

void  PEGASUS_CLI_LINKAGE GetOptions(OptionManager& om, int& argc, char** argv,  const String& testHome);

int PEGASUS_CLI_LINKAGE CheckCommonOptionValues(OptionManager& om, char** argv, Options& opts);

void PEGASUS_CLI_LINKAGE mofFormat(PEGASUS_STD(ostream)& os, const char* text, Uint32 indentSize);

// ************* CIMClient Functions
int PEGASUS_CLI_LINKAGE enumerateClassNames(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateClasses(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE deleteClass(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE getClass(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE deleteInstance(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateInstanceNames(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateInstances(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE getInstance(CIMClient& client, Options& opts);
                                           
int PEGASUS_CLI_LINKAGE getProperty(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE setProperty(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE setQualifier(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE getQualifier(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE deleteQualifier(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateQualifiers(CIMClient& client, Options& opts);


PEGASUS_NAMESPACE_END

#endif
