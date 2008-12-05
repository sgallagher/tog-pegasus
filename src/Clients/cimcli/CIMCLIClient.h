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

#ifndef _CLI_CLIENTLIB_H
#define _CLI_CLIENTLIB_H

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/OptionManager.h>
#include <Clients/cimcli/Linkage.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN
#define CDEBUG(X) PEGASUS_STD(cout) << "cimcli " << X << PEGASUS_STD(endl)
//#define CDEBUG(X)

// Defined here because apparently not all platforms support max and min.
#define LOCAL_MAX(a, b) ((a > b) ? a : b)
#define LOCAL_MIN(a, b) ((a < b) ? a : b)


static const char MSG_PATH [] = "pegasus/pegasusCLI";

typedef int     CommandID;
typedef int     OutputType;
const OutputType    OUTPUT_XML                  = 1;
const OutputType    OUTPUT_MOF                  = 2;
const OutputType    OUTPUT_TEXT                 = 3;
const OutputType    OUTPUT_FILE                 = 4;
const OutputType    OUTPUT_TABLE                = 5;

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
const CommandID ID_StatisticsOn                 = 28;
const CommandID ID_StatisticsOff                = 29;
const CommandID ID_Unknown                      = 30;

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
    {   OUTPUT_TEXT,    "txt"   },
    {   OUTPUT_TABLE,   "table" }
};
static const Uint32 NUM_OUTPUTS = sizeof(OutputTable) / sizeof(OutputTable[0]);
    
struct  CMD_STRUCT
{
    CommandID ID_Command;
    const char* CommandName;
    int minNumArgs;
    const char* ShortCut;
    const char* msgKey; //Message key for globalization 
    const char* UsageText;
};

// ******** CIM Client Commands
static CMD_STRUCT CommandTable[] =
{
    {ID_EnumerateInstanceNames,  "enumerateInstanceNames", 2 ,"ni",
    "Clients.cimcli.CIMCLIClient.NI_COMMAND_HELP",
    "Enumerate instancenames of <instancename>"},

    {ID_EnumerateAllInstanceNames,"enumallInstanceNames", 2 , "niall",
    "Clients.cimcli.CIMCLIClient.NIALL_COMMAND_HELP" ,
    "Enumerate all instancenames in namespace."}, 

    {ID_EnumerateInstances,      "enumerateInstances",2 ,   "ei",
    "Clients.cimcli.CIMCLIClient.EI_COMMAND_HELP" ,
    "Enumerate instances of <classname>"}, 

    {ID_EnumerateClassNames,     "enumerateClassNames",2 ,  "nc",
    "Clients.cimcli.CIMCLIClient.NC_COMMAND_HELP",
    "Enumerate Class Names of [ <classname> ]"},

    {ID_EnumerateClasses,        "enumerateClasses",2 ,     "ec",
    "Clients.cimcli.CIMCLIClient.EC_COMMAND_HELP",
    "Enumerate classes of [ <classname> ]"},

    {ID_GetClass,                "getClass",2 ,             "gc",
    "Clients.cimcli.CIMCLIClient.GC_COMMAND_HELP",
    "Get class of <classname>"},

    {ID_GetInstance,             "getInstance",   2 ,       "gi",
    "Clients.cimcli.CIMCLIClient.GI_COMMAND_HELP",
    "Get instance of <objectname> | <classname>"},

    {ID_CreateInstance,          "createInstance",2 ,       "ci",
    "Clients.cimcli.CIMCLIClient.CI_COMMAND_HELP",
    "Create one Instance of <Class> *<name=param>"},

    {ID_DeleteInstance,          "deleteInstance",2 ,       "di",
    "Clients.cimcli.CIMCLIClient.DI_COMMAND_HELP",
    "Delete Instance of <objectname> or\n"
    "                            interactive of <className>"},

    {ID_CreateClass   ,          "createClass",   2 ,       "cc",
    "Clients.cimcli.CIMCLIClient.CC_COMMAND_HELP",
    "Not supported"},

    {ID_ModifyInstance,          "modifyInstance",2 ,       "mi",
    "Clients.cimcli.CIMCLIClient.MI_COMMAND_HELP",
    "Not supported"},

    {ID_DeleteClass,             "deleteClass",   2 ,       "dc",
    "Clients.cimcli.CIMCLIClient.DC_COMMAND_HELP",
    "Delete Class of <classname>"},

    {ID_GetProperty,             "getProperty",   2 ,       "gp", 
    "Clients.cimcli.CIMCLIClient.GP_COMMAND_HELP",
    "TBD"},

    {ID_SetProperty,             "setProperty",   2 ,       "sp",
    "Clients.cimcli.CIMCLIClient.SP_COMMAND_HELP",
    "TBD"},

    {ID_GetQualifier,            "getQualifier",  2 ,       "gq",
    "Clients.cimcli.CIMCLIClient.GQ_COMMAND_HELP",
    "Get Qualifier of <qualifiername>"},

    {ID_SetQualifier,            "setQualifier",  2 ,       "sq",
    "Clients.cimcli.CIMCLIClient.SQ_COMMAND_HELP",
    "Not suported"},

    {ID_EnumerateQualifiers,     "enumerateQualifiers",2 ,  "eq",
    "Clients.cimcli.CIMCLIClient.EQ_COMMAND_HELP",
    "Enumerate all Qualifiers"},

    {ID_DeleteQualifier,         "deleteQualifier",  2 ,    "dq",
    "Clients.cimcli.CIMCLIClient.DQ_COMMAND_HELP",
    "Delete qualifer of <qualifiername>"},

    {ID_Associators,             "associators",   2 ,       "a" ,
    "Clients.cimcli.CIMCLIClient.A_COMMAND_HELP",
    "Enumerate Associators of <classname>|<instancename>."},

    {ID_AssociatorNames,         "associatorNames", 2 ,     "an",
    "Clients.cimcli.CIMCLIClient.AN_COMMAND_HELP",
    "Enumerate Associator Names of <classname>|<instancename>"},

    {ID_References,              "references",      2,      "r",
    "Clients.cimcli.CIMCLIClient.R_COMMAND_HELP",
    "Enumerate References of <classname>|<instancename>"},

    {ID_ReferenceNames,          "referenceNames",2 ,       "rn",
    "Clients.cimcli.CIMCLIClient.RN_COMMAND_HELP",
    "Enumerate Reference Names <classname>|<instancename>"},

    {ID_InvokeMethod,            "invokeMethod",  2 ,       "im",
    "Clients.cimcli.CIMCLIClient.IM_COMMAND_HELP",
    "Invoke Method for <object> <method> {<inputParams>}"},

    {ID_ExecQuery,               "execQuery",     2 ,       "xq",
    "Clients.cimcli.CIMCLIClient.XQ_COMMAND_HELP",
    " Execute Query <query-expresssion> [<query-language>]"},

    {ID_EnumerateNamespaces,     "enumerateNamespaces",2 ,  "ns",
    "Clients.cimcli.CIMCLIClient.NS_COMMAND_HELP",
    "Enumerate all namespaces on the server."},

    {ID_StatisticsOn,            "Turn On Statistics",2 ,   "son",
    "Clients.cimcli.CIMCLIClient.SON_COMMAND_HELP",
    "Turn on CIM Server Statistics Gathering"},

    {ID_StatisticsOff,           "Turn Off Statistics",2 ,  "soff",
    "Clients.cimcli.CIMCLIClient.SOFF_COMMAND_HELP",
    "Turn off CIM Server Statistics Gathering"},

    {ID_ShowOptions,             "show command options",2 ,  "?",
    "Clients.cimcli.CIMCLIClient.?_COMMAND_HELP",
    "Show List of Commands"}
};

static const Uint32 NUM_COMMANDS =
    sizeof(CommandTable) / sizeof(CommandTable[0]);

// ************* Options Functions
// The input options used by the individual commands. Note that
// Use of these varies by command.

struct  OPTION_STRUCT
{
    String location;
#ifdef PEGASUS_HAS_SSL
    Boolean ssl;
    String clientCert;
    String clientKey;
#endif
    String nameSpace;
    CIMName className;
    String objectName;
    String inputObjectName;       // Name to report as input object on error
    String cimCmd;                // Command name
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

    // The references and associatior parameters.
    // We setup both internal and external forms
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
    Array<String> extraParams;   // additional param list. depends on command.
    Boolean interactive;
};

typedef struct OPTION_STRUCT Options;

//
//  String entries for each column in the output
//
typedef Array <String> ColumnEntry;

Array<String> PEGASUS_CLI_LINKAGE _tokenize(const String& input,
                                            const Char16 separator);

CIMParamValue PEGASUS_CLI_LINKAGE _createMethodParamValue(const String& input, 
                                                          const Options& opts);

void PEGASUS_CLI_LINKAGE _printTables(const Array<Uint32>& maxColumnWidth,
                                      const Array<ColumnEntry>& outputTable,
                                      PEGASUS_STD(ostream)& outPrintWriter);

void PEGASUS_CLI_LINKAGE showCommands(const char* pgmName);

void PEGASUS_CLI_LINKAGE showUsage();

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

void PEGASUS_CLI_LINKAGE tableFormat(PEGASUS_STD(ostream)& outPrintWriter,
    const Array<CIMInstance>& instances);

String PEGASUS_CLI_LINKAGE loadMessage(
     const char* key,
     const char* defMessage);

// ************* CIMClient Functions
int PEGASUS_CLI_LINKAGE enumerateClassNames(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateClasses(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE deleteClass(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE getClass(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE deleteInstance(CIMClient& client, Options& opts);

int PEGASUS_CLI_LINKAGE enumerateInstanceNames(CIMClient& client,
                                               Options& opts);

int PEGASUS_CLI_LINKAGE enumerateAllInstanceNames(CIMClient& client,
                                                  Options& opts);

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

int PEGASUS_CLI_LINKAGE enumerateNamespaces_Namespace(CIMClient& client,
                                                      Options& opts);

PEGASUS_NAMESPACE_END

#endif
