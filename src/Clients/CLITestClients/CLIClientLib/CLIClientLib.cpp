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
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include "CLIClientLib.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

const String DEFAULT_NAMESPACE = "root/cimv2";

// Character sequences used in help/usage output.

static const char * usage = "This command executes single WBEM Operations.";

String printPropertyList (CIMPropertyList)
{
    return "TBD add code here to print property list";
}

// Note that the following is one long string.
static const char * usageDetails = "Using CLI examples:n \
CLI enumerateinstancenames pg_computersystem  -- enumerateinstances of class\n \
CLI enumerateclassnames -- Enumerate classnames from root\n \
CLI enumerateclasses    -- Enumerate classes with MOF output starting at root\n \
CLI enumerateclasses CIM_Computersystem -- Enumerate classes in MOF starting with \
CIM_Computersystem\n \
CLI getclass CIM_door -a -u guest =p guest - Get class with authentication set and \
user = guest, password = guest.\n \
CLI enumerateclasses -XML -- enumerate classes and output XML rather than MOF. \n";

int OutputFormatInstance(OutputType format, CIMInstance& instance)
{
    if (format == OUTPUT_XML)
        XmlWriter::printInstanceElement(instance, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<Sint8> x;
        MofWriter::appendInstanceElement(x, instance);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }   
    return 0;
}

int OutputFormatClass(OutputType format, CIMClass& myClass)
{
    if (format == OUTPUT_XML)
        XmlWriter::printClassElement(myClass, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<Sint8> x;
        MofWriter::appendClassElement(x, myClass);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }   
    return 0;
}

int OutputFormatObject(OutputType format, CIMObject& myObject)
{
       
    if (myObject.isClass())
    {
        CIMClass c(myObject);
        OutputFormatClass(format, c);
    }
    else if (myObject.isInstance())
    {
        CIMInstance i(myObject);
        OutputFormatInstance(format, i);
    }
    else
        cout << "Error, Object is neither class or instance" << endl;
    return 0;
}

int OutputFormatQualifierDecl(OutputType format, CIMQualifierDecl& myQualifierDecl)
{
    if (format == OUTPUT_XML)
        XmlWriter::printQualifierDeclElement(myQualifierDecl, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<Sint8> x;
        MofWriter::appendQualifierDeclElement(x, myQualifierDecl);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }   
    return 0;
}

int OutputFormatCIMValue(OutputType format, CIMValue& myValue)
{
    if (format == OUTPUT_XML)
        XmlWriter::printValueElement(myValue, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<Sint8> x;
        MofWriter::appendValueElement(x, myValue);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }   
    return 0;
}



int enumerateInstanceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateInstanceNames "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << endl;
    }
    CIMNamespaceName myNameSpace;
    
    if (opts.nameSpace == "")
        myNameSpace.isNull();
    else 
        myNameSpace = opts.nameSpace;
    
    Array<CIMObjectPath> instanceNames = 
        client.enumerateInstanceNames(myNameSpace, opts.className);
    if (opts.summary)
    {
        cout << instanceNames.size() << " instance names of class " << opts.className << " returned." << endl;
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < instanceNames.size(); i++)
                    cout << instanceNames[i].toString() << endl;
    }
    return 0;
}        


/* This command searches the entire namespace and displays names of all instances.
    It is in effect enumerate classes followed by enumerate instances.
   The user may either provide a starting class or not, in which case
   it searches the complete namespace, not simply the defined class.
*/
int enumerateAllInstanceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateClasseNames "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << endl;
    }
    // Added to allow "" string input to represent NULL CIMName.
    CIMName myClassName = CIMName();
    if (opts.className != "")
    {
        myClassName = opts.className;
    }
    
    Array<CIMName> classNames; 
    
    classNames = client.enumerateClassNames(opts.nameSpace,
                                        myClassName,
                                        opts.deepInheritance);
    
    for (Uint32 iClass = 0; iClass < classNames.size(); iClass++)
    {
        if (opts.verboseTest)
        {
            cout << "EnumerateInstanceNames "
                << "Namespace = " << opts.nameSpace
                << ", Class = " << opts.className
                << endl;
        }
        Array<CIMObjectPath> instanceNames = 
        client.enumerateInstanceNames(opts.nameSpace, classNames[iClass]);
        if (opts.summary)
        {
            if (instanceNames.size != 0)
            {
                cout << instanceNames.size() << " instance names of class " << opts.className << " returned." << endl;
            }
        }
        else
        {
            //simply output the list one per line for the moment.
            for (Uint32 i = 0; i < instanceNames.size(); i++)
                        cout << instanceNames[i].toString() << endl;
        }
    }
    return 0;
}        

int enumerateInstances(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateInstances "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << ", localOnly = " << (opts.localOnly? "true" : "false")
            << endl;
    }
    
    Array<CIMInstance> instances; 
    instances = client.enumerateInstances(opts.nameSpace,
                                                   opts.className,
                                                   opts.deepInheritance,
                                                   opts.localOnly,
                                                   opts.includeQualifiers,
                                                   opts.includeClassOrigin);
    if (opts.summary)
    {
        cout << instances.size() << " instances of class " << opts.className << " returned." << endl;
    }
    else
    {
        Uint32 cnt = 0;
        opts.outputFormat.toLower();
    
        for( ; cnt < NUM_OUTPUTS; cnt++ ) 
        {
            if (opts.outputFormat == OutputTable[cnt].OutputName)
                    break;
        }
        // Output the returned instances
        for (Uint32 i = 0; i < instances.size(); i++)
        {
            CIMInstance instance = instances[i];
            // Check Output Format to print results
            OutputFormatInstance(OutputTable[cnt].OutputType, instance);
        }
    }
    return 0;
}

int deleteInstance(CIMClient& client, Options& opts)
{
    client.deleteInstance(opts.nameSpace, opts.className);
    return 0;  
}

int getInstance(CIMClient& client, Options& opts)
{
    CIMInstance cimInstance = client.getInstance(opts.nameSpace,
                                                 opts.cimObjectPath);

    Uint32 cnt = 0;
    for( ; cnt < NUM_OUTPUTS; cnt++ ) 
    {
        if (opts.outputFormat == OutputTable[cnt].OutputName)
                break;
    }
    // Check Output Format to print results
    OutputFormatInstance(OutputTable[cnt].OutputType, cimInstance);
    return 0;
}

int enumerateClassNames(CIMClient& client, Options& opts)
{
    
    if (opts.verboseTest)
    {
        cout << "EnumerateClasseNames "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << endl;
    }
    // Added to allow "" string input to represent NULL CIMName.
    CIMName myClassName = CIMName();
    if (opts.className != "")
    {
        myClassName = opts.className;
    }
    
    Array<CIMName> classNames; 
    
    classNames = client.enumerateClassNames(opts.nameSpace,
                                        myClassName,
                                        opts.deepInheritance);
    
    if (opts.summary)
    {
        cout << classNames.size() << " returned." << endl;
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < classNames.size(); i++)
                cout << classNames[i] << endl;
    }
    return 0;
}

int enumerateClasses(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateClasses "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << ", localOnly = " << (opts.localOnly? "true" : "false")
            << ", includeQualifiers = " << (opts.includeQualifiers? "true" : "false")
            << ", includeClassOrigin = " << (opts.includeClassOrigin? "true" : "false")
            << endl;
    }
    // Added to allow "" string input to represent NULL CIMName.
    CIMName myClassName = CIMName();
    if (opts.className != "")
    {
        myClassName = opts.className;
    }
    
    Array<CIMClass> classes; 
    
    classes = client.enumerateClasses(opts.nameSpace,
                                        myClassName,
                                        opts.deepInheritance,
                                        opts.localOnly,
                                        opts.includeQualifiers,
                                        opts.includeClassOrigin);
    
    if (opts.summary)
    {
        cout << classes.size() << " returned." << endl;
    }
    else
    {
        Uint32 cnt = 0;
        opts.outputFormat.toLower();
    
        for( ; cnt < NUM_OUTPUTS; cnt++ ) 
        {
            if (opts.outputFormat == OutputTable[cnt].OutputName)
                    break;
        }
        // Output the returned instances
        for (Uint32 i = 0; i < classes.size(); i++)
        {
            CIMClass myClass = classes[i];
        
            OutputFormatClass(OutputTable[cnt].OutputType, myClass);
        }
    }
    return 0;
}

int deleteClass(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteClasses "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << endl;
    }
    
    client.deleteClass(opts.nameSpace, opts.className);
    return 0;  
}

int getClass(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getClass "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << ", localOnly = " << (opts.localOnly? "true" : "false")
            << ", includeQualifiers = " << (opts.includeQualifiers? "true" : "false")
            << ", includeClassOrigin = " << (opts.includeClassOrigin? "true" : "false")
            << " PropertyList = " << " ADD property list print HERE KSTEST"
            << endl;
    }
    CIMClass cimClass = client.getClass(opts.nameSpace,
                                        opts.className,
                                        opts.localOnly,
                                        opts.includeQualifiers,
                                        opts.includeClassOrigin,
                                        opts.propertyList);
    Uint32 cnt = 0;
    opts.outputFormat.toLower();

    // KS this should be moved to a common place. and saved with opts.
    for( ; cnt < NUM_OUTPUTS; cnt++ ) 
    {
        if (opts.outputFormat == OutputTable[cnt].OutputName)
                break;
    }
    
    OutputFormatClass(OutputTable[cnt].OutputType, cimClass);
    return 0;
}

int getProperty(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getProperty "
            << "Namespace = " << opts.nameSpace
            << ", Instance = " << opts.instanceName.toString()
            << ", propertyName = " << opts.propertyName
            << endl;
    }
    
    CIMValue cimValue;
    cimValue = client.getProperty( opts.nameSpace,
                                   opts.instanceName,
                                   opts.propertyName);
                                   
    // display returned property
    return 0;
}


int setProperty(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "setProperty "
            << "Namespace = " << opts.nameSpace
            << ", Instance = " << opts.instanceName.toString()
            << ", propertyName = " << opts.propertyName
            // KS The new value goes here
            << endl;
    }
    
    client.setProperty( opts.nameSpace,
                                   opts.instanceName,
                                   opts.propertyName,
                                   opts.newValue);
                                   

    return 0;
}

int getQualifier(CIMClient& client, Options& opts)
{
    cout << "entering getQualifier " << endl;
    //if (opts.verboseTest)
    {
        cout << "getQualifier "
            << "Namespace = " << opts.nameSpace
            << ", Qualifier = " << opts.qualifierName
            << endl;
    }
    
    
    CIMQualifierDecl cimQualifierDecl;
    cimQualifierDecl = client.getQualifier( opts.nameSpace,
                                   opts.qualifierName);
                                   
    // display new qualifier
    
    Uint32 cnt = 0;
    opts.outputFormat.toLower();

    for( ; cnt < NUM_OUTPUTS; cnt++ ) 
    {
        if (opts.outputFormat == OutputTable[cnt].OutputName)
                break;
    }
    
    OutputFormatQualifierDecl(OutputTable[cnt].OutputType, cimQualifierDecl);
    
    return 0;
}
int setQualifier(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "setQualifiers "
            << "Namespace = " << opts.nameSpace
            // KS add the qualifier decl here.
            << endl;
    }
    client.setQualifier( opts.nameSpace,
                         opts.qualifierDeclaration);
    return 0;
}
int deleteQualifier(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteQualifiers "
            << "Namespace = " << opts.nameSpace
            << " Qualifier " << opts.qualifierName
            << endl;
    }
    client.deleteQualifier( opts.nameSpace,
                            opts.qualifierName);
                                   
    return 0;
}
int enumerateQualifiers(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "enumerateQualifiers "
            << "Namespace = " << opts.nameSpace
            << endl;
    }
  
    Array<CIMQualifierDecl> qualifierDecls;
    qualifierDecls = client.enumerateQualifiers( opts.nameSpace);
    
    Uint32 cnt = 0;
    opts.outputFormat.toLower();
    if (opts.summary)
    {
        cout << qualifierDecls.size() << " returned." << endl;
    }
    else
    {
        for( ; cnt < NUM_OUTPUTS; cnt++ ) 
        {
            if (opts.outputFormat == OutputTable[cnt].OutputName)
                    break;
        }
        // Output the returned instances
        for (Uint32 i = 0; i < qualifierDecls.size(); i++)
        {
            CIMQualifierDecl myQualifierDecl = qualifierDecls[i];
        
            OutputFormatQualifierDecl(OutputTable[cnt].OutputType, myQualifierDecl);
    }
    }
                                  
    return 0;
}

/*
    Array<CIMObjectPath> referenceNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY

*/
int referenceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "ReferenceNames "
            << "Namespace = " << opts.nameSpace
            << ", ObjectPath = " << opts.objectName
            << ", resultClass = " << opts.resultClassName
            << ", role = " << opts.role
            << endl;
    }
    Array<CIMObjectPath> referenceNames = 
    
    client.referenceNames( opts.nameSpace, opts.objectName);
    
    /*
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY */
    
    if (opts.summary)
    {
        cout << referenceNames.size() << " returned." << endl;
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < referenceNames.size(); i++)
                    cout << referenceNames[i].toString() << endl;
    }
    return 0;
}        


/****
     Array<CIMObject> references(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );
    */
int references(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "References "
            << "Namespace = " << opts.nameSpace
            << ", Object = " << opts.objectName
            << ", resultClass = " << opts.resultClassName
            << ", role = " << opts.role
            << ", includeQualifiers = " << opts.includeQualifiers
            << ", includeClassOrigin = " << opts.includeClassOrigin
            << ", CIMPropertyList = "  << printPropertyList(opts.propertyList)
            << endl;
    }
    Array<CIMObject> objects =  
    client.references( opts.nameSpace, opts.objectName);
    
    if (opts.summary)
    {
        cout << objects.size() << " returned." << endl;
    }
    else
    {
        Uint32 cnt = 0;
        opts.outputFormat.toLower();
    
        for( ; cnt < NUM_OUTPUTS; cnt++ ) 
        {
            if (opts.outputFormat == OutputTable[cnt].OutputName)
                    break;
        }
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
        {
            CIMObject object = objects[i];
            // Check Output Format to print results
            OutputFormatObject(OutputTable[cnt].OutputType, object);
        }
    }
    return 0;
}        

/*
    Array<CIMObjectPath> associatorNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& assocClass = CIMName(),
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY
    );

*/
int associatorNames(CIMClient& client, Options& opts)
{
    
    if (opts.verboseTest)
    {
        cout << "associatorNames "
            << "Namespace = " << opts.nameSpace
            << ", Object = " << opts.objectName
            << ", resultClass = " << opts.resultClassName
            << ", role = " << opts.role
            << endl;
    }
    Array<CIMObjectPath> associatorNames = 
    
    client.associatorNames( opts.nameSpace, opts.objectName);
    
    /*
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY */
    
    if (opts.summary)
    {
        cout << associatorNames.size() << " returned." << endl;
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < associatorNames.size(); i++)
                    cout << associatorNames[i].toString() << endl;
    }
    return 0;
}        


/****
    Array<CIMObject> associators(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& assocClass = CIMName(),
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );
    */
int associators(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "Associators "
            << "Namespace = " << opts.nameSpace
            << ", Object = " << opts.objectName
            << endl;
    }
    Array<CIMObject> objects =  
    client.associators( opts.nameSpace, opts.objectName);
    
    if (opts.summary)
    {
        cout << objects.size() << "  associators returned." << endl;
    }
    else
    {
        Uint32 cnt = 0;
        opts.outputFormat.toLower();
    
        for( ; cnt < NUM_OUTPUTS; cnt++ ) 
        {
            if (opts.outputFormat == OutputTable[cnt].OutputName)
                    break;
        }
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
        {
            CIMObject object = objects[i];
            // Check Output Format to print results
            OutputFormatObject(OutputTable[cnt].OutputType, object);
        }
    }
    return 0;
}        


void GetOptions(
    OptionManager& om,
    int& argc,
    char** argv,
    const String& testHome)
{
    static const char* outputFormats[] = { "xml", "mof", "txt"};
    static const Uint32 NUM_OUTPUTFORMATS = sizeof(outputFormats) /
                                            sizeof(outputFormats[0]);

    static OptionRow optionsTable[] =
        //optionname defaultvalue rqd  type domain domainsize clname hlpmsg
    {
        {"authenticate", "false" , false, Option::BOOLEAN, 0, 0, "a",
                                        "Defines whether user authentication is used" },
        {"User", "unknown", false, Option::STRING, 0, 0, "u",
                                        "Defines User Name for authentication" },
        
        {"Password", "unknown", false, Option::STRING, 0, 0, "p",
                                        "Defines password for authentication" },
        
        // We don't need this one. Force command to first param since always used
        {"cimCmd", "unknown", false, Option::STRING, 0, 0, "-c",
                                        "specifies CIM Command to use" },
        
        {"className", "", false, Option::STRING, 0, 0, "c",
                                        "ClassName to use" },

        {"cimObjectPath", "", false, Option::STRING, 0, 0, "-p",
                                        "CIMObjectPath to use" },
        
        {"location", "localhost:5988", false, Option::STRING, 0, 0, "l",
                                        "specifies system and port" },
        
        {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "n",
                                        "specifies namespace to use for operation" },
        
        // KS change the output formats to use the enum options function
        {"outputformats", "mof", false, Option::STRING, 0,
                                         NUM_OUTPUTFORMATS, "o",
                                        "Output in xml, mof, txt"},
        
        {"deepInheritance", "false", false, Option::BOOLEAN, 0, 0, "di",
                                        "If set does deep enumeration "},
        
        {"localOnly", "false", false, Option::BOOLEAN, 0, 0, "lo",
                                        "If set sets localonly param true "},
        
        {"includeQualifiers", "false", false, Option::BOOLEAN, 0, 0, "iq",
                                        "If set sets includeQualifiers option "},
        
        {"includeClassOrigin", "false", false, Option::BOOLEAN, 0, 0, "ic",
                                        "If set includeClassOriginOption True"},

        {"propertyList", "unknown", false, Option::STRING, 0, 0, "pl",
                                        "This parameter defines a propertyNameList "},
        
        {"resultClass", "unknown", false, Option::STRING, 0, 0, "rc",
                                        "This parameter defines a resultClass string for References, etc. "},
        
        {"role", "unknown", false, Option::STRING, 0, 0, "role",
                                        "This parameter defines a role string for References, etc. "},
        
        {"version", "false", false, Option::BOOLEAN, 0, 0, "-v",
                                        "Displays software Version "},
        
        {"verbose", "false", false, Option::BOOLEAN, 0, 0, "v",
                                        "Verbose Display. Includes Detailed Param Input display "},
        
        
        {"summary", "false", false, Option::BOOLEAN, 0, 0, "s",
                                        "Displays only summary count for enums "},

        {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
                            "Prints help message with command line options "},

        {"debug", "false", false, Option::BOOLEAN, 0, 0, "d", 
                     "Not Used "},
    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    //We want to make this code common to all of the commands

    String configFile = "CLI.conf";

    //cout << "Config file from " << configFile << endl;

    if (FileSystem::exists(configFile))
             om.mergeFile(configFile);

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();

}

void printHelp(char* name, OptionManager& om)
{
    String header = "Usage: cli <CIMOperationName> <CIMOperationParameters> <Options>";
    String trailer = "Trailer";
    header.append(name);

    om.printOptionsHelpTxt(header, trailer);
    
    cout << "Possible CIMOperations are:" << endl;
    for( Uint32 i = 0; i < NUM_COMMANDS; i++ ) 
    {
        cout << CommandTable[i].CommandName << ", ";
    }
}

/* PrintHelp - This is temporary until we expand the options manager to allow
   options help to be defined with the OptionRow entries and presented from
   those entries.
*/
void printHelpMsg(const char* pgmName, const char* usage, const char* extraHelp, 
                OptionManager& om)
{
    cout << endl << pgmName << endl;
    cout << "Usage: " << pgmName << endl << usage << endl;
    cout << endl;
    om.printOptionsHelpTxt(usage, extraHelp);
    //om.printHelp(const char* pgmName, OptionManager om);
    
    cout << "\nPossible CIMOperations are\n     name      (short Cut Name) Usage: \n";
    for( Uint32 i = 0; i < NUM_COMMANDS; i++ ) 
    {
        cout << "    " << CommandTable[i].CommandName 
             << " ( " << CommandTable[i].ShortCut << " ) "
             << CommandTable[i].UsageText << endl;
    }
    cout << endl;
    
    cout << extraHelp << endl;

}

int CheckCommonOptionValues(OptionManager& om, char** argv, Options& opts) 
{
    // Check to see if user asked for help (-h otpion):
    Boolean verboseTest = (om.valueEquals("verbose", "true")) ? true :false;
    if (verboseTest)
        opts.verboseTest = verboseTest;
    
    if (om.isTrue("help"))
    {
                printHelpMsg(argv[0], usage, usageDetails, om);
                exit(0);
    }

    // Establish the namespace from the input parameters
    //String nameSpace;
    if(om.lookupValue("namespace", opts.nameSpace))
    {
        if (verboseTest)
            cout << "Namespace = " << opts.nameSpace << endl;
    }

    if(om.lookupValue("cimCmd", opts.cimCmd))
    {
       if (verboseTest)
           cout << "CIM command = " << opts.cimCmd << endl;
    }
    if(om.lookupValue("className", opts.className))
    {
       if (verboseTest)
           cout << "Class Name = " << opts.className << endl;
    }

    if(om.lookupValue("cimObjectPath", opts.cimObjectPath))
    {
       if (verboseTest)
           cout << "CIM ObjectPath = " << opts.cimObjectPath << endl;
    }
    
    if(om.lookupValue("role", opts.role))
    {
       if (verboseTest)
           cout << "role = " << opts.role << endl;
    }
    
    if(om.lookupValue("resultClassName", opts.resultClassName))
    {
       if (verboseTest)
           cout << "resultClassName = " << opts.resultClassName << endl;
    }
    
    opts.deepInheritance = om.isTrue("deepInheritance");
    if (om.isTrue("deepInheritance")  & verboseTest)
    {
        cout << "deepInteritance set" << endl;
    }
    
    opts.localOnly = om.isTrue("localOnly");
    
    opts.includeQualifiers = om.isTrue("includeQualifiers");
    
    opts.includeClassOrigin = om.isTrue("includeClassOrigin");

    opts.summary = om.isTrue("summary");
    /*
    Boolean activeTest = false;
    if (om->valueEquals("active", "true"))
                 activeTest = true;
    */
    // Now develop the target instname from the arglist
    // For this one, at least one arguement is required
    // Not sure if this is true.
    // Think we should be able to get by with no argument.
    if (om.valueEquals("help", "true"))
    {
        String header = "Usage ";
        String trailer = "";
        om.printOptionsHelpTxt(header, trailer);
        exit(0);
    }

    if(om.lookupValue("location", opts.location))
    {
        if (verboseTest)
            cout << "Location = " << opts.location << endl;
    }
    
    Boolean authenticate = (om.isTrue("authenticate"));
    {
        if (verboseTest)
            cout << "authenticate = " << (authenticate? "true": "false") << endl;
    }
                         
    // Don't get user and password if not authenticating.
    if (authenticate)
    {
         if(om.lookupValue("User", opts.user))
         {
             if (verboseTest)
                 cout << "User = " << opts.user << endl;
         }
         
         if(om.lookupValue("Password", opts.password))
         {
             if (verboseTest)
                 cout << "Password = " << opts.password << endl;
         }
    }
    
    // Create a variable with the format output and a correponding type.
    // Suggest we might change this whole thing to the option type that
    // mike used in the example of colors so that  you could do -red -blue
    // or in our case -mof -xml, etc.
    
    if(om.lookupValue("outputformats", opts.outputFormat))
     {
        if (verboseTest)
            cout << "Output Format = " << opts.outputFormat << endl;
     }

    
    Uint32 cnt = 0;
    opts.outputFormat.toLower();

    for( ; cnt < NUM_OUTPUTS; cnt++ ) 
    {
        if (opts.outputFormat == OutputTable[cnt].OutputName)
                break;
    }
    // Note that this makes no notice of a not found
    if (cnt != NUM_OUTPUTS)
        opts.outputFormatType = cnt;
    if (om.lookupValue("role", opts.role))
    {
        if (verboseTest)
            cout << "Role = " << opts.role << endl;
    }
   //REVIEW add the assoc class, etc stuff.
    
    return 0;
}


//------------------------------------------------------------------------------
//
// _indent()
//
//------------------------------------------------------------------------------

static void _indent(PEGASUS_STD(ostream)& os, Uint32 level, Uint32 indentSize)
{
    Uint32 n = level * indentSize;
    if (n > 50)
    {
    cout << "Jumped Ship " << level << " size " << indentSize << endl;
    exit(1);
    }

    for (Uint32 i = 0; i < n; i++)
        os << ' ';
}
void mofFormat(
    PEGASUS_STD(ostream)& os, 
    const char* text, 
    Uint32 indentSize)
{
    char* tmp = strcpy(new char[strlen(text) + 1], text);
    Uint32 count = 0;
    Uint32 indent = 0;
    Boolean quoteState = false;
    Boolean qualifierState = false;
    char c;
    char prevchar;
    while (c = *tmp++)
    {
        count++;
        // This is too simplistic and must move to a token based mini parser
        // but will do for now. One problem is tokens longer than 12 characters that
        // overrun the max line length.
        switch (c)
        {
            case '\n':
                os << Sint8(c);
                prevchar = c;
                count = 0 + (indent * indentSize);
                _indent(os, indent, indentSize);   
                break;

            case '\"':   // quote 
                os << Sint8(c);
                prevchar = c;
                quoteState = !quoteState;
                break;

            case ' ':
                os << Sint8(c);
                prevchar = c;
                if (count > 66)
                {
                    if (quoteState)
                    {   
                        os << "\"\n";
                        _indent(os, indent + 1, indentSize);   
                        os <<"\"";
                    }
                    else
                    {
                        os <<"\n";
                        _indent(os, indent + 1,  indentSize);   
                    }
                    count = 0 + ((indent + 1) * indentSize);
                }
                break;
            case '[':
                if (prevchar == '\n')
                {
                    indent++;
                    _indent(os, indent,  indentSize);
                    qualifierState = true;
                }
                os << Sint8(c);
                prevchar = c;
                break;

            case ']':
                if (qualifierState)
                {
                    if (indent > 0)
                        indent--;
                    qualifierState = false;
                }
                os << Sint8(c);
                prevchar = c;
                break;

            default:
                os << Sint8(c);
                prevchar = c;
        }

    }
}

PEGASUS_NAMESPACE_END
