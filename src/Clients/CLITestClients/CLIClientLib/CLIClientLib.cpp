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
#include <Pegasus/Common/Tracer.h>
#include "CLIClientLib.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

const String DEFAULT_NAMESPACE = "root/cimv2";

// Character sequences used in help/usage output.


String printPropertyList (CIMPropertyList)
{
    return "TBD add code here to print property list";
}

static const char * version = "2.0";
static const char * usage = "This command executes single WBEM Operations.";


// Note that the following is one long string.
static const char * usageDetails = "Using CLI examples:n \
CLI enumerateinstancenames pg_computersystem  -- enumerateinstances of class\n \
    or \n \
CLI ei pg_computersystem    -- Same as above \n\n \
CLI enumerateclassnames -- Enumerate classnames from root/cimv2.\n \
CLI ec /n root -- Enumerate classnames from namespace root. \n \
CLI ec -o xml   -- Enumerate classes with XML output starting at root\n \
CLI enumerateclasses CIM_Computersystem -o xml\n    -- Enumerate classes in MOF starting with \
CIM_Computersystem\n \
CLI getclass CIM_door -a -u guest =p guest\n    -- Get class with authentication set and \
user = guest, password = guest.\n \
CLI ec -o XML -- enumerate classes and output XML rather than MOF. \n \
CLI getqualifiers -- Get the qualifiers in mof output format\n";

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
    /*CIMNamespaceName myNameSpace;
    
    if (opts.nameSpace == "")
        myNameSpace.isNull();
    else 
        myNameSpace = opts.nameSpace;*/
    
    Array<CIMObjectPath> instanceNames = 
        client.enumerateInstanceNames(opts.nameSpace, opts.className);
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
    /****if (opts.className != "")
    {
        myClassName = opts.className;
    }*/
    
    Array<CIMName> classNames; 
    
    classNames = client.enumerateClassNames(opts.nameSpace,
                                        opts.className,
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
            if (instanceNames.size() != 0)
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
        // Output the returned instances
        for (Uint32 i = 0; i < instances.size(); i++)
        {
            CIMInstance instance = instances[i];
            // Check Output Format to print results
            OutputFormatInstance(opts.outputType, instance);
        }
    }
    return 0;
}

int deleteInstance(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteInstance "
            << "Namespace = " << opts.nameSpace
            << ", Object = " << opts.objectName
            << endl;
    }
    client.deleteInstance(opts.nameSpace, opts.objectName);
    return 0;  
}

int getInstance(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getInstance "
            << "Namespace = " << opts.nameSpace
            << ", Instance = " << opts.objectName
            << endl;
    }
    
    CIMInstance cimInstance = client.getInstance(opts.nameSpace,
                                                 opts.objectName);

    // Check Output Format to print results
    OutputFormatInstance(opts.outputType, cimInstance);
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
    Array<CIMName> classNames; 
    
    classNames = client.enumerateClassNames(opts.nameSpace,
                                        opts.className,
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
    /*
    CIMName myClassName = CIMName();
    if (opts.className != "")
    {
        myClassName = opts.className;
    }
    */
    Array<CIMClass> classes; 
    
    classes = client.enumerateClasses(opts.nameSpace,
                                        opts.className,
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
        // Output the returned instances
        for (Uint32 i = 0; i < classes.size(); i++)
        {
            CIMClass myClass = classes[i];
            OutputFormatClass(opts.outputType, myClass);
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
    
    OutputFormatClass(opts.outputType, cimClass);
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
    
    OutputFormatQualifierDecl(opts.outputType, cimQualifierDecl);
    
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
    
    if (opts.summary)
    {
        cout << qualifierDecls.size() << " returned." << endl;
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < qualifierDecls.size(); i++)
        {
            CIMQualifierDecl myQualifierDecl = qualifierDecls[i];
        
            OutputFormatQualifierDecl(opts.outputType, myQualifierDecl);
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
            << ", resultClass = " << opts.resultClass
            << ", role = " << opts.role
            << endl;
    }
    Array<CIMObjectPath> referenceNames = 
    
    client.referenceNames( opts.nameSpace, opts.objectName, opts.resultClass, opts.role);
    
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
            << ", resultClass = " << opts.resultClass
            << ", role = " << opts.role
            << ", includeQualifiers = " << opts.includeQualifiers
            << ", includeClassOrigin = " << opts.includeClassOrigin
            << ", CIMPropertyList = "  << printPropertyList(opts.propertyList)
            << endl;
    }
    Array<CIMObject> objects =  
    client.references( opts.nameSpace, opts.objectName,
        opts.resultClass,
        opts.role,
        opts.includeQualifiers,
        opts.includeClassOrigin,
        opts.propertyList);
    
    if (opts.summary)
    {
        cout << objects.size() << " returned." << endl;
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
        {
            OutputFormatObject(opts.outputType, objects[i]);
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
            << ", assocClass = " << opts.assocClass
            << ", resultClass = " << opts.resultClass
            << ", role = " << opts.role
            << ", resultRole = " << opts.resultRole
            << endl;
    }
    Array<CIMObjectPath> associatorNames = 
    
    client.associatorNames( opts.nameSpace, opts.objectName,
        opts.assocClass,
        opts.resultClass,
        opts.role,
        opts.resultRole);
    
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
        // Output the list one per line for the moment.
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
            << ", assocClass = " << opts.assocClass
            << ", resultClass = " << opts.resultClass
            << ", role = " << opts.role
            << ", resultRole = " << opts.resultRole
            << ", includeQualifiers = " << ((opts.includeQualifiers)? "true" : "false")
            << ", includeClassOrigin = " << ((opts.includeClassOrigin)? "true" : "false")
            << ", propertyList = " << printPropertyList(opts.propertyList)
            << endl;
    }
    Array<CIMObject> objects =  
    client.associators( opts.nameSpace, opts.objectName,
        opts.assocClass,
        opts.resultClass,
        opts.role,
        opts.resultRole,
        opts.includeQualifiers,
        opts.includeClassOrigin,
        opts.propertyList);
    
    if (opts.summary)
    {
        cout << objects.size() << "  associators returned." << endl;
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
        {
            OutputFormatObject(opts.outputType, objects[i]);
        }
    }
    return 0;
} 

                /*
                    CIMValue invokeMethod(
                    	const CIMNamespaceName& nameSpace,
                    	const CIMObjectPath& instanceName,
                    	const CIMName& methodName,
                    	const Array<CIMParamValue>& inParameters,
	                    Array<CIMParamValue>& outParameters
                 */

 int invokeMethod(CIMClient& client, Options& opts)
 {
     if (opts.verboseTest)
     {
         cout << "Associators "
             << "Namespace = " << opts.nameSpace
             << ", Object = " << opts.objectName
             << ", methodName = " << opts.methodName
             << endl;
         CIMValue retValue;
         Array<CIMParamValue> inParms;
         Array<CIMParamValue> outParms;

         retValue = client.invokeMethod(opts.nameSpace, opts.objectName,
             opts.methodName, inParms, outParms);

         // Now display the CIMValue


     }
     return 0;
 }
 

 /* Enumerate the Namespaces.  This function is based on using the __Namespace class
    and either returns all namespaces or simply the ones starting at the namespace input
    as the namespace variable.
    It assumes that the input classname is __Namespace.
 */
int enumerateNamespaces_Namespace(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateNamespaces "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << endl;
    }
    Array<CIMNamespaceName> namespaceNames;
    
    // Build the namespaces incrementally starting at the root
    // ATTN: 20030319 KS today we start with the "root" directory but this is wrong. We should be
    // starting with null (no directoyr) but today we get an xml error return in Pegasus
    // returned for this call. Note that the specification requires that the root namespace be used
    // when __namespace is defined but does not require that it be the root for allnamespaces. That
    // is a hole is the spec, not in our code.
    namespaceNames.append(opts.nameSpace);
    Uint32 start = 0;
    Uint32 end = namespaceNames.size();
    do
    {
        // for all new elements in the output array
        for (Uint32 range = start; range < end; range ++)
        {
            // Get the next increment in naming for all a name element in the array
            Array<CIMInstance> instances = client.enumerateInstances(namespaceNames[range],opts.className);
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
                        String ns = namespaceNames[range].getString();
                        ns.append("/");
                        ns.append(namespaceComponent);
                        namespaceNames.append(ns);
                    }
                }
            }
            start = end;
            end = namespaceNames.size();
        }
    }
    while (start != end);
    // Validate that all of the returned entities are really namespaces. It is legal for us to
    // have an name component that is really not a namespace (ex. root/fred/john is a namespace
    // but root/fred is not.
    // There is no clearly defined test for this so we will simply try to get something, in this
    // case a wellknown assoication
    Array<CIMNamespaceName> returnNamespaces;

    for (Uint32 i = 0 ; i < namespaceNames.size() ; i++)
    {
        try
        {
            CIMQualifierDecl cimQualifierDecl;
            cimQualifierDecl = client.getQualifier(namespaceNames[i],
                                           "Association");

            returnNamespaces.append(namespaceNames[i]);
        }
        catch(CIMException& e)
        {
            if (e.getCode() != CIM_ERR_INVALID_NAMESPACE)
                returnNamespaces.append(namespaceNames[i]);
        }
    }

    if (opts.summary)
    {
        cout << returnNamespaces.size() << " namespaces " << " returned." << endl;
    }
    else
    {
        for( Uint32 cnt = 0 ; cnt < returnNamespaces.size(); cnt++ ) 
        {
            cout << returnNamespaces[cnt] << endl;;
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
        /*{"authenticate", "false" , false, Option::BOOLEAN, 0, 0, "a",
                                        "Defines whether user authentication is used" },*/
        {"User", "", false, Option::STRING, 0, 0, "u",
                                        "Defines User Name for authentication" },
        
        {"Password", "", false, Option::STRING, 0, 0, "p",
                                        "Defines password for authentication" },
        
        {"location", "localhost:5988", false, Option::STRING, 0, 0, "l",
                            "specifies system and port (HostName:port). Port is optional" },
        
        {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "n",
                            "Specifies namespace to use for operation" },
        
        {"deepInheritance", "false", false, Option::BOOLEAN, 0, 0, "di",
                            "If set deepInheritance parameter set true "},
        
        {"localOnly", "false", false, Option::BOOLEAN, 0, 0, "lo",
                            "If set localonly parameter set true "},
        
        {"includeQualifiers", "false", false, Option::BOOLEAN, 0, 0, "iq",
                            "If set sets includeQualifiers option true "},
        
        {"includeClassOrigin", "false", false, Option::BOOLEAN, 0, 0, "ic",
                            "If set includeClassOriginOption set toTrue"},

        {"propertyList", "", false, Option::STRING, 0, 0, "pl",
                            "Defines a propertyNameList. Format is p1,p2,p3 (without spaces) "},
        
        {"assocClass", "", false, Option::STRING, 0, 0, "ac",
                            "Defines a assocClass string for Associator calls"},
        
        {"AssocRole", "", false, Option::STRING, 0, 0, "ar",
                            "Defines a role string for Associatiors AssocRole parameter"},

        
        {"resultClass", "", false, Option::STRING, 0, 0, "rc",
                            "Defines a resultClass string for References and Associatiors "},
        
        {"resultRole", "", false, Option::STRING, 0, 0, "rr",
                            "Defines a role string for References, etc. "},

        {"inputParameters", "", false, Option::STRING, 0, 0, "ip",
                            "Defines an invokeMethod input parameter list. Format is p1=v1,p2=v2,..,pn=vn (without spaces) "},
        
        {"outputParameters", "", false, Option::STRING, 0, 0, "op",
                            "Defines an invokeMethod output parameter list. Format is p1=v1,p2=v2,..,pn=vn (without spaces) "},
        
        {"filter", "", false, Option::STRING, 0, 0, "f",
                            "defines a filter to use for query. One String input "},

        {"substitute", "", false, Option::STRING, 0, 0, "-s",
                            "Defines a conditional substition of input parameters. ) "},
        
        // KS change the output formats to use the enum options function
        // Deprecate this function.
        {"outputformats", "mof", false, Option::STRING, 0,NUM_OUTPUTFORMATS, "o",
                            "Output in xml, mof, txt"},
        
        {"xmlOutput", "false", false, Option::BOOLEAN, 0,0, "x",
                            "Output objects in xml instead of mof format"},

        {"version", "false", false, Option::BOOLEAN, 0, 0, "-v",
                            "Displays software Version "},
        
        {"verbose", "false", false, Option::BOOLEAN, 0, 0, "v",
                            "Verbose Display. Includes Detailed Param Input display "},
        
        
        {"summary", "false", false, Option::BOOLEAN, 0, 0, "s",
                            "Displays only summary count for enumerations, associators, etc. "},

        {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
                            "Prints help message with command line options "},


        {"debug", "false", false, Option::BOOLEAN, 0, 0, "d", 
                            "Not Implemented "},
        
        {"trace", "0", false, Option::WHOLE_NUMBER, 0, 0, "trace", 
                            "Set Pegasus Common Components Trace. Sets the Trace level. 0 is off"},

        {"delay", "0", false, Option::WHOLE_NUMBER, 0, 0, "delay", 
                            "Delay between connection and request "},
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

/* PrintHelpMsg - This is temporary until we expand the options manager to allow
   options help to be defined with the OptionRow entries and presented from
   those entries.
*/
void printHelpMsg(const char* pgmName, const char* usage, const char* extraHelp, 
                OptionManager& om)
{
    String header = "Usage: cli <CIMOperationName> <CIMOperationObject> <Options>";
    String header2 = "  Execute the <CimOperationName> on the <CIMOperationObject> with <Options>"; 
    
    String optionsTrailer = "Options vary by command consistent with CIM Operations";
    cout << endl << pgmName << " V"<< version << " " << header << endl <<header2 << endl;
    cout << endl;
    cout << "The options for this command are:" << endl;
    om.printOptionsHelpTxt(usage, optionsTrailer);
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

    {
        String target;
        if (om.lookupValue("substitution", target))
        {
            // Get directory for file
            Array<String> fileList;
            String fileName = "/conditions.txt";
            String temp = ".";
            temp.append(fileName);
            fileList.append(temp);
            const char* tmpDir = getenv ("PEGASUS_HOME");
            if (tmpDir != NULL)
            {
                temp = tmpDir;
                temp.append(fileName);
                fileList.append(temp);
            }
            /*

            // get filename
            FILE* fp;
            for (Uint32 i = 0; i < fileList.size(); i++)
            {
                fp = fopen(fileList[i].getCString(), "rb");
                if (fp != NULL)
                {
                }
            }
            {
            } while (fp != NULL && )
            FILE* fp = fopen(traceFile.getCString(), "rb");

            if (fp == NULL)
            {
                FILE* fp = fopen(traceFile.getCString(), "rb");
    
                if (fp == NULL)
                {
                    string message = "failed to open file: \"" + fileName + "\"";
                    ErrorExit(message);
                }
            }

            int argc = args.size();
            char** argv = new char*[args.size()];

            for (int i = 0; i < argc; i++)
            argv[i] = (char*)args[i].c_str();

            int result = DependCmdMain(argc, argv);

            delete [] argv;
            
            //////////////////////////
            
            reak a configuration line up into tokens.  Returns tokenized string
00184 // if the line is a valid one, i.e. the 2nd word is '=' and there is at 
00185 // least one more word after the equal.
00186 char *command_tokenize(char *newcmd, int *argc, char *argv[]) {
00187   char *cmd, *eqloc;
00188 
00189   // make a new string with a ' = ' instead of '='
00190   if(!(eqloc = strchr(newcmd,'=')))
00191     return NULL;
00192     
00193   cmd = new char[strlen(newcmd) + 3];
00194   strncpy(cmd,newcmd,(eqloc - newcmd));
00195   strcpy(cmd + (eqloc - newcmd)," = ");
00196   strcat(cmd,eqloc + 1);
00197 
00198   *argc = 0;
00199 
00200   argv[*argc] = strtok(cmd," ,;\t\n");
00201   if (argv[*argc] == NULL) {
00202     delete [] cmd;
00203     return NULL;
00204   }
00205 
00206   // see if the first token starts with '#'
00207   if(!strncmp(argv[0],"#",1)) {
00208     delete [] cmd;
00209     return NULL;
00210   }
00211 
00212   (*argc)++;
00213 
00214   // break up the rest of the string
00215   while ((argv[*argc] = strtok(NULL," ,;\t\n")) != NULL)
00216     (*argc)++;
00217 
00218   // make sure the 2nd word is "=", and there are 3 words or more
00219   if(*argc < 3 || strcmp(argv[1],"=")) {
00220     delete [] cmd;
00221     return NULL;
00222   }
00223 
00224   return cmd;
00225 }
00226 
00227 
00228 // break a configuration line up into tokens.
00229 char *str_tokenize(char *newcmd, int *argc, char *argv[]) {
00230   char *cmd;
00231 
00232   cmd = stringdup(newcmd);
00233   *argc = 0;
00234 
00235   // initialize tokenizing calls
00236   argv[*argc] = strtok(cmd," ,;\t\n");
00237 
00238   // loop through words until end-of-string, or comment character, found
00239   while(argv[*argc] != NULL) {
00240     // see if the token starts with '#'
00241     if(argv[*argc][0] == '#') {
00242       delete [] cmd;
00243       return argv[0];
00244     } else {
00245       (*argc)++;                // another token in list
00246     }
00247     
00248     // scan for next token
00249     argv[*argc] = strtok(NULL," ,;\t\n");
00250   }
00251 
00252   return (*argc > 0 ? argv[0] : (char *) NULL);

            */
        }
    } 


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

    if(om.lookupValue("className", opts.classNameString))
    {
        if (opts.classNameString != "")
        {
            opts.className = opts.classNameString;
        }
        if (verboseTest)
           cout << "Class Name = " << opts.className << endl;
    }

    /*if(om.lookupValue("cimObjectPath", opts.cimObjectPath))
    {
       if (verboseTest)
           cout << "CIM ObjectPath = " << opts.cimObjectPath << endl;
    }*/
    String temprole;
    if(om.lookupValue("role", temprole))
    {
        // we need to deliver String::EMPTY when no param.
        if (temprole != "")
            opts.role = temprole;
        if (verboseTest)
           cout << "role = " << opts.role << endl;
    }

    String tempResultRole;
    if(om.lookupValue("resultRole", tempResultRole))
    {
        // we need to deliver String::EMPTY when no param.
        if (tempResultRole != "")
            opts.role = tempResultRole;
        if (verboseTest)
           cout << "resultrole = " << opts.resultRole << endl;
    }

    om.lookupValue("location", opts.location);

    if(om.lookupValue("resultClass", opts.resultClassName))
    {
       
       if (opts.resultClassName != "")
       {
           //Covers fact that assigning to CIMName can cause exception.
           try
           {
               opts.resultClass = opts.resultClassName;
           }
           catch(Exception& e)
           {
               cout << "Error in Result Class. Exception " << e.getMessage() << endl;
               exit(1);
           }
       }
       if (verboseTest)
           cout << "resultClassName = " << opts.resultClassName << endl;
    }


    if(!om.lookupIntegerValue("delay", opts.delay))
    {
            opts.delay = 0;
    }
    if (verboseTest && opts.delay != 0)
    {
        cout << "delay set to " << opts.delay << " Seconds" << endl;
    }


    if(om.lookupValue("assocClass", opts.assocClassName))
    {
       if (verboseTest)
           cout << "assocClassName = " << opts.assocClassName << endl;
       if (opts.assocClassName != "")
       {
           try
           {
               opts.assocClass = opts.assocClassName;
           }
           catch(Exception& e)
           {
               cout << "Error in assoc Class. Exception " << e.getMessage() << endl;
               exit(1);
           }
       }
    }

    opts.deepInheritance = om.isTrue("deepInheritance");
    if (om.isTrue("deepInheritance")  & verboseTest)
        cout << "deepInteritance set" << endl;
    
    opts.localOnly = om.isTrue("localOnly");
    if (om.isTrue("localOnly")  & verboseTest)
        cout << "localOnly set" << endl;
    
    opts.includeQualifiers = om.isTrue("includeQualifiers");
    if (om.isTrue("includeQualifiers")  & verboseTest)
        cout << "includeQualifiers set" << endl;
    
    opts.includeClassOrigin = om.isTrue("includeClassOrigin");
    if (om.isTrue("includeClassOrigin")  & verboseTest)
        cout << "includeClassOrigin set" << endl;

    if(!om.lookupIntegerValue("trace", opts.trace))
    {
            opts.trace = 0;
    }
    else
    {
        Uint32 traceLevel = 0;
        switch (opts.trace)
        {
            case 0:             // This covers the default.
                break;
            case 1 :
                traceLevel = Tracer::LEVEL1;
                break;
            case 2 :
                traceLevel = Tracer::LEVEL2;
                break;
            case 3 :
                traceLevel = Tracer::LEVEL3;
                break;
            case 4 :
                traceLevel = Tracer::LEVEL4;
                break;
            default:
                cout << "Illegal value for Trace. Max = 4" << endl;
        }
        opts.trace = traceLevel;
    }
    if (verboseTest && opts.trace != 0)
    {
        cout << "Pegasus Trace set to  Level  " << opts.trace << endl;
    }


    opts.summary = om.isTrue("summary");
    
    // get User name and password if set.
     if(om.lookupValue("User", opts.user))
     {
         if (opts.user.size() == 0)
             opts.user = String::EMPTY;
         if (verboseTest)
             cout << "User = " << opts.user << endl;
     }
     
     if(om.lookupValue("Password", opts.password))
     {
         if (opts.password.size() == 0)
             opts.password = String::EMPTY;
         if (verboseTest)
             cout << "Password = " << opts.password << endl;
     }
    
    // Create a variable with the format output and a correponding type.
    // Suggest we might change this whole thing to the option type that
    // mike used in the example of colors so that  you could do -red -blue
    // or in our case -mof -xml, etc.
    
     opts.isXmlOutput = om.isTrue("xmlOutput");
     if (opts.isXmlOutput  & verboseTest)
         cout << "xmlOutput set" << endl;

    if(om.lookupValue("outputformats", opts.outputFormat))
     {
        if (verboseTest)
            cout << "Output Format = " << opts.outputFormat << endl;
     }

    // Get the output format parameter and save it
    Uint32 cnt = 0;
    opts.outputFormat.toLower();

    for( ; cnt < NUM_OUTPUTS; cnt++ ) 
    {
        if (opts.outputFormat == OutputTable[cnt].OutputName)
                break;
    }
    // Note that this makes no notice of a not found
    if (cnt != NUM_OUTPUTS)
    {
        opts.outputFormatType = cnt;
        opts.outputType = OutputTable[cnt].OutputType;
    }

    // Separate the Property List if any provided
    {
        String p;
        Array<CIMName> pList;
        if(om.lookupValue("propertyList", p))
        {
            if (p.size() != 0)
            {
                Uint32 start = 0;
                Uint32 length = 0;
                Uint32 end = 0;
                while ((end = p.find(start, ',')) != PEG_NOT_FOUND)
                {
                    length = end - start;
                    pList.append(p.subString(start, length));
                    start += (length + 1);
                }
                if(start < p.size())
                {
                    pList.append(p.subString(start));
                }
            }
            if (verboseTest)
            {
                cout << "Property List =";
                for (Uint32 i = 0 ; i < pList.size() ; i++)
                    cout << " " << pList[i] << endl;
            }
            opts.propertyList.set(pList);
        }
    }


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
