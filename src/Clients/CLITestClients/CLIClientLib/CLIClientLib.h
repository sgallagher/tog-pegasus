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
// Author:  Karl Schopmeyer (k.schopmeyer@opengroup.org)
//          Mary Hinton (m.hinton@verizon.net)
//
// Modified By: Adrian Schuur (schuur&de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _CLI_CLIENTLIB_H
#define _CLI_CLIENTLIB_H

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/OptionManager.h>
#include <Clients/CLITestClients/CLIClientLib/Linkage.h>
#include <Pegasus/Common/Stopwatch.h>

PEGASUS_NAMESPACE_BEGIN
#define CDEBUG(X) PEGASUS_STD(cout) << "CLI " << X << PEGASUS_STD(endl)
//#define CDEBUG(X)
//#define DEBUG(X) Logger::put (Logger::DEBUG_LOG, "CIMOpReqDsptchr", Logger::INFORMATION, "$0", X)

// Defined here because apparently not all platforms support max and min.
#define LOCAL_MAX(a, b) ((a > b) ? a : b)
#define LOCAL_MIN(a, b) ((a < b) ? a : b)


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
const CommandID ID_EnumerateAllInstanceNames    = 25;
const CommandID ID_EnumerateNamespaces          = 26;
const CommandID ID_ShowOptions                  = 27;
const CommandID ID_Unknown                      = 28;

struct  OUTPUT_STRUCT
{
    // KS the old form OutputType OutputType causes redefinition error in Linux
    int OutputType;
    const char* OutputName;
};

static OUTPUT_STRUCT OutputTable[] =
{
    // Output Type      OutputName              
    {   OUTPUT_XML,     "xml"   },
    {   OUTPUT_MOF,     "mof"   },
    {   OUTPUT_TEXT,    "txt"   }
};
static const Uint32 NUM_OUTPUTS = sizeof(OutputTable) / sizeof(OutputTable[0]);
    
struct  CMD_STRUCT
{
    CommandID ID_Command;
    const char* CommandName;
    int minNumArgs;
    const char* ShortCut;
    const char* UsageText;
};

// ******** CIM Client Commands
static CMD_STRUCT CommandTable[] =
{
    // Command ID                CommandName        Min Num Args, ShortCut Name, Usage              
    {ID_EnumerateInstanceNames,  "enumerateinstancenames", 2 ,"ni",
         "Enumerate instancenames <instancename>  " },
    {ID_EnumerateAllInstanceNames,"enumallinstancenames", 2 , "niall",
         "Enumerate all instance names in namespace. CLI niall"   },
    {ID_EnumerateInstances,      "enumerateinstances",2 ,   "ei",
         "Enumerate instances <objectname> X_name.name=@a@"        },
    {ID_EnumerateClassNames,     "enumerateclassnames",2 ,  "nc",
         "Enumerate Class Names. [ <classname> ]"       },
    {ID_EnumerateClasses,        "enumerateclasses",2 ,     "ec",
         "Enumerate classes. [ <classname> ]"          },
    {ID_GetClass,                "getclass",2 ,             "gc",
         "Get class. <classname> (cli gc CIM_Door"                  },
    {ID_GetInstance,             "getinstance",   2 ,       "gi",
         "Get instance <objectname> "  },
    {ID_CreateInstance,          "createinstance",2 ,       "ci", "Create one Instance <Class> *<name=param> "  },
    {ID_DeleteInstance,          "deleteinstance",2 ,       "di", "Delete one Instance <objectname> or interactive <className> "  },
    {ID_CreateClass   ,          "createclass",   2 ,       "cc", "Not supported "  },
    {ID_ModifyInstance,          "modifyclass",   2 ,       "mi", "Not supported "  },
    {ID_DeleteClass,             "deleteinstance",2 ,       "dc", "Delete one Class <classname> "  },
    {ID_GetProperty,             "getproperty",   2 ,       "gp", "TBD "  },
    {ID_SetProperty,             "setproperty",   2 ,       "sp", "TBD "  },
    {ID_GetQualifier,            "getqualifier",  2 ,       "sq", "Get Qualifier <qualifiername> "  },
    {ID_SetQualifier,            "setqualifier",  2 ,       "gc", "Not suported "  },
    {ID_EnumerateQualifiers,     "enumeratequalifiers",2 ,  "eq", "Enumerate all Qualifiers "  },
    {ID_DeleteQualifier,         "deletequalifier",  2 ,    "dq", "Delete qualifer <qualifiername> "  },
    {ID_Associators,             "associators",   2 ,       "a" , 
        "Enumerate Associators <classname>|<instancename>. use -ac, -rc, -rr, -pl options "  },
    {ID_AssociatorNames,         "associatornames", 2 ,     "an", "Enum Assoc names <classname>|<instancename> "  },
    {ID_References,              "references",      2,      "r",  "Enum References <classname>|<instancename> "  },
    {ID_ReferenceNames,          "referencenames",2 ,       "rn", "Enum Reference Names <classname>|<instancename> "  },
    {ID_InvokeMethod,            "InvokeMethod",  2 ,       "im", "im <object> <method> {<inputParams>} "  },
    {ID_ExecQuery,               "execquery",     2 ,       "xq", 
        "Execute Query <query-expresssion> [<query-language>]"  },
    {ID_EnumerateNamespaces,     "enumeratenamespaces",2 ,  "ns",
        "Enumerate all namespaces on the server. "  },
    {ID_ShowOptions,             "show command options",2 ,  "?", "Show List of Commands "  },
   
};

static const Uint32 NUM_COMMANDS = sizeof(CommandTable) / sizeof(CommandTable[0]);

// ************* Options Functions
// The input options used by the individual commands. Note that
// Use of these varies by command.

struct  OPTION_STRUCT
{
    String location;
    String nameSpace;
    CIMName className;
    String objectName;
    String inputObjectName;                 // Name to report as input object on error
    String cimCmd;                          // Command name
    String outputFormat;
    Boolean isXmlOutput;
    int outputFormatType;
    OutputType outputType;
    Boolean deepInheritance;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMObjectPath instanceName;
    CIMValue newValue;
    //String cimObjectPath;
    String qualifierName;
    CIMQualifierDecl qualifierDeclaration;
    String user;
    String password;
    String propertyName;
    CIMPropertyList propertyList;
    String propertyListText;

    // The references and associatior parameters.  We setup both internal and external forms
    String assocClassName;
    CIMName assocClass;
    String resultClassName;
    CIMName resultClass;
    String role;
    String resultRole;

    CIMName methodName;
    Array<CIMParamValue> inParams;

    Boolean verboseTest;
    Boolean summary;
    Uint32 delay;
    Uint32 trace;
	Boolean debug;

    Uint32 count;
    Uint32 repeat;
    Boolean time;
    Stopwatch elapsedTime;
    double saveElapsedTime;
    Uint32 termCondition;
    String queryLanguage;
    String query;
    Uint32 connectionTimeout;    // Connection timeout in seconds
    Array<String> extraParams;   // additional parameter list. depends on command.
    Boolean interactive;
};

typedef struct OPTION_STRUCT Options;

Array<String> PEGASUS_CLI_LINKAGE _tokenize(const String& input, const Char16 separator);

CIMParamValue PEGASUS_CLI_LINKAGE _createMethodParamValue(const String& input, const Options& opts);

void PEGASUS_CLI_LINKAGE showCommands(const char* pgmName);

void PEGASUS_CLI_LINKAGE showUsage(const char* pgmName);

void PEGASUS_CLI_LINKAGE printHelpMsg(const char* pgmName, const char* usage,
     const char* extraHelp, 
     OptionManager& om);

void PEGASUS_CLI_LINKAGE printHelp(char* name, OptionManager& om);

void PEGASUS_CLI_LINKAGE printUsageMsg(const char* pgmName,OptionManager& om);

void  PEGASUS_CLI_LINKAGE GetOptions(OptionManager& om, int& argc, char** argv,
      const String& testHome);

int PEGASUS_CLI_LINKAGE CheckCommonOptionValues(OptionManager& om, char** argv,
     Options& opts);

void PEGASUS_CLI_LINKAGE mofFormat(PEGASUS_STD(ostream)& os, const char* text,
     Uint32 indentSize);

// ************* CIMClient Functions
int PEGASUS_CLI_LINKAGE enumerateClassNames(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateClasses(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE deleteClass(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE getClass(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE deleteInstance(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateInstanceNames(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateAllInstanceNames(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateInstances(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE createInstance(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE getInstance(CIMClient& client, Options& opts);
                                           
int PEGASUS_CLI_LINKAGE getProperty(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE setProperty(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE setQualifier(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE getQualifier(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE deleteQualifier(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateQualifiers(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE referenceNames(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE references(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE associators(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE associatorNames(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE invokeMethod(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE executeQuery(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateNamespaces_Namespace(CIMClient& client, Options& opts);

PEGASUS_NAMESPACE_END

#endif
