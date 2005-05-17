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
// Modified By: Amit K Arora (amita@in.ibm.com) for Bug# 1081 (mofFormat())
//              Adrian Schuur (schuur@de.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Josephine Eskaline Joyce(jojustin@in.ibm.com) for Bug #1664
//              Amit K Arora (amita@in.ibm.com) for Bug#2926
//
//%/////////////////////////////////////////////////////////////////////////////
#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Common/Tracer.h>
#include "CLIClientLib.h"
#include <Pegasus/Common/PegasusVersion.h>
PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

const String DEFAULT_NAMESPACE = "root/cimv2";


String _toString(Boolean x)
{
    return(x ? "true" : "false");
}
/** Select one item from an array of items presented to
    the user. This preents the list and requests user input for
    the response.
    @param selectList Array<String> list of items from which the
    user is to select one.  Each item should be a printable string.
    @param what String that defines for the output string what type
    of items the select is based on (ex: "Instance Names");
    @return Uint32 representing the item to be selected.
    TODO: Find a way to do a reject.

*/
Uint32 _selectStringItem(const Array<String>& selectList, const String& what)
{
    Uint32 rtn = 0;
    for (Uint32 i = 0 ; i < selectList.size() ; i++)
        cout << i + 1 << ": " << selectList[i] << endl;
    while (rtn < 1 || rtn > selectList.size())
    {
        cout << "Select " << what << " (1.." << selectList.size() << ")? " << flush;
        cin >> rtn;
    }

    return (rtn - 1);
}

/** Allow user to select one instance name. Enumerates names given className and
    requests input of one index
    @param
    @param
    @param className CIMName for the class to enumerate.
    @param instancePath CIMObjectPath of instance found
    @return True if instance provided and the path is in instancePath.  Else False
    and there is nothing in the instancePath
*/
Boolean _selectInstance(CIMClient& client, Options& opts, const CIMName& className,
    CIMObjectPath & instancePath)
{
    Array<CIMObjectPath> instanceNames =
        client.enumerateInstanceNames(opts.nameSpace,
                                      className);
    Array<String> list;
    for (Uint32 i = 0 ; i < instanceNames.size() ; i++)
        list.append(instanceNames[i].toString());

    if (list.size() == 0)
    {
        return(false);
    }
    Uint32 rtn = _selectStringItem(list, "an Instance");

    instancePath = instanceNames[rtn];

    return(true);
}


/** Use the interactive selection mechanism to get the instance if
    the input object is a class AND if the opts.interactive flag is
    set.  This function is used by the association functions because
    just the existence of the object as class is insufficient since
    these functions accept both class and instance input for processing.
    If the tests are passed this function calls the server to enumerate
    the instance names possible and displays them for the user to
    select one.
    @param client CIMClient context for the operation required
    @param opts the context structure for this operaiton
    @param name String of the object that is the target of the request.
    @param instancePath CIMObjectPath of instance selected if return
    is true.  Else, unchanged.
    @return Boolean True if an instance path is to be returned. If nothing
    is selected, returns False.
*/
Boolean _conditionalSelectInstance(CIMClient& client, Options& opts,
    CIMObjectPath & instancePath)
{
    // if class level and interactive set.
    if ((instancePath.getKeyBindings().size() == 0) && opts.interactive)
    {
        // Ask the user to select an instance

        return(_selectInstance(client, opts, CIMName(opts.objectName), instancePath));
    }

    return(true);
}


// Character sequences used in help/usage output.
String buildPropertyListString(CIMPropertyList& pl)
{
    String rtn;
    Array<CIMName> pls = pl.getPropertyNameArray();
    if (pl.isNull())
        return("NULL");

    if (pl.size() == 0)
        return("EMPTY");

    for (Uint32 i = 0 ; i < pls.size() ; i++)
    {
        if (i != 0)
            rtn.append(", ");
        rtn.append(pls[i].getString());
    }
    return(rtn);
}

void printPropertyList(CIMPropertyList& pl)
{
    cout << buildPropertyListString(pl);
}
static const char * version = "2.1";
static const char * usage = "This command executes single CIM Operations.";


// Note that the following is one long string.
static const char * usageDetails = "Using CLI examples:\n \
CLI enumerateinstancenames pg_computersystem  -- enumerateinstances of class\n \
    or \n \
CLI ei pg_computersystem    -- Same as above \n\n \
CLI enumerateclassnames -- Enumerate class names from root/cimv2.\n \
CLI ec -n root -- Enumerate classes from namespace root. \n \
CLI ec -o xml   -- Enumerate classes with XML output starting at root\n \
CLI enumerateclasses CIM_Computersystem -o xml\n    -- Enumerate classes in MOF starting with \
CIM_Computersystem\n \
CLI getclass CIM_door -a -u guest -p guest\n    -- Get class with authentication set and \
    user = guest, password = guest.\n \
CLI rn TST_Person.name=@MIKE@ -n root/sampleprovider -rc TST_Lineage. \n \
CLI ec -o XML -- Enumerate classes and output XML rather than MOF. \n \
CLI getqualifiers -- Get the qualifiers in mof output format\n";

void _displaySummary(Uint32 count, String& description, String item, Options& opts)
{
        cout << count << " " << description
            << " " << item << " returned. ";
        if (opts.repeat > 0)
            cout << opts.repeat;
        if(opts.time && opts.repeat)
            cout << " " << opts.saveElapsedTime;
        cout << endl;
        if ((opts.count) != 29346 && (opts.count != count))
        {
            cout << "Failed count test. Expected= " << opts.count << " Actual rcvd= " << count << endl;
            opts.termCondition = 1;
        }
}

/**** NOT TESTED
String _nextToken(String& input, Uint32 start, Char16 separator)
{
    String rtn;
    Uint32 end;
    end = input.find(input.find(start, separator));
    if (end == PEG_NOT_FOUND)
    {
        start = PEG_NOT_FOUND
        rtn = input.subString(start,(input.size() - start);
    }
    else
    {
        rtn = input.subString(start,end);
        start = end + 1;
    }
    return(rtn);
}
*/
/** tokenize an input string into an array of Strings,
 * separating the tokens at the separator character
 * provided
 * @param input String
 * @param separator character
 * @returns Array of separated strings
 * */
Array<String> _tokenize(const String& input, const Char16 separator)
{
    Array<String> tokens;
    if (input.size() != 0)
    {
        Uint32 start = 0;
        Uint32 length = 0;
        Uint32 end = 0;
        while ((end = input.find(start, separator)) != PEG_NOT_FOUND)
        {
            length = end - start;
            tokens.append(input.subString(start, length));
            start += (length + 1);
        }
        if(start < input.size())
        {
            tokens.append(input.subString(start));
        }
    }
    return tokens;
}

Boolean _tokenPair(const String& input, String& key, String& value)
{

    Array<String> pair = _tokenize(input, '=');
    if (pair.size() < 2)
    {
        cout << "Input Parameter error. Expected name=value. Received  " << input << input;
        return(false);
    }
    // If there is more than 1 "=" it is part of the reference and we
    // rebuild the reference.
    if (pair.size() < 2)
    {
        for (Uint32 i = 2 ; i < pair.size() ; i++)
        {
            pair[1].append("=");
            pair[i].append(pair[i]);

        }
    }
    key = pair[0];
    value = pair[1];
    return(true);
}
CIMParamValue _createMethodParamValue(const String& input, const Options& opts)
{
    String key;
    String value;
    if (!_tokenPair(input, key, value))
    {
        cout <<"Error in param parsing with input " << input << endl;
        exit(1);
    }

    if (opts.verboseTest)
    {
        cout << "Name = " << key << ", Value= " << value << endl;
    }
    // ATTN: KS 20030423 P2 This is incomplete since it only allows us to do string input.
    // We don't include the typing information.
    //Array<String> valuePair = _tokenize(pair[1], ':');
    //if (validType(valuePair[0] >= 0)
    //{
    //
    //}

    CIMValue v(value);
    CIMParamValue pv(key, v, false);
    return pv;
}

void outputFormatInstance(const OutputType format, CIMInstance& instance)
{
    cout << "path= " << instance.getPath().toString() << endl;
    if (format == OUTPUT_XML)
        XmlWriter::printInstanceElement(instance, cout);
    else if (format == OUTPUT_MOF)
    {
        // Reset the propagated flag to assure that these entities
        // are all shown in the MOF output.
        for (Uint32 i = 0 ; i < instance.getPropertyCount() ; i++)
        {
            CIMProperty p = instance.getProperty(i);
            p.setPropagated(false);
        }

        Array<char> x;
        MofWriter::appendInstanceElement(x, instance);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }
}
void outputFormatParamValue(const OutputType format, const CIMParamValue& pv)
{
    if (format == OUTPUT_XML)
        XmlWriter::printParamValueElement(pv, cout);

    else if (format == OUTPUT_MOF)
    {
        if (!pv.isUninitialized())
        {
           CIMValue v =  pv.getValue();
           CIMType type = v.getType();
           if (pv.isTyped())
               cout << cimTypeToString (type) << " ";
           else
               cout << "UnTyped ";

           cout << pv.getParameterName() << "="
                << v.toString() << endl;
        }
        else
            cout << "ParamValue not initialized" << endl;
    }
    else
        cout << "Error, Format Definition Error" << endl;
}

void outputFormatClass(const OutputType format, CIMClass& myClass)
{
    if (format == OUTPUT_XML)
        XmlWriter::printClassElement(myClass, cout);
    else if (format == OUTPUT_MOF)
    {
        // Reset the propagated flag to assure that these entities
        // are all shown in the MOF output.
        for (Uint32 i = 0 ; i < myClass.getPropertyCount() ; i++)
        {
            CIMProperty p = myClass.getProperty(i);
            p.setPropagated(false);
        }
        for (Uint32 i = 0 ; i < myClass.getMethodCount() ; i++)
        {
            CIMMethod m = myClass.getMethod(i);
            m.setPropagated(false);
        }
        Array<char> x;
        MofWriter::appendClassElement(x, myClass);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }
    else
        cout << "Error, Format Definition Error" << endl;
}

void outputFormatObject(const OutputType format, const CIMObject& myObject)
{

    if (myObject.isClass())
    {
        CIMClass c(myObject);
        outputFormatClass(format, c);
    }
    else if (myObject.isInstance())
    {
        CIMInstance i(myObject);
        outputFormatInstance(format, i);
    }
    else
        cout << "Error, Object is neither class or instance" << endl;
}

void outputFormatQualifierDecl(const OutputType format, const CIMQualifierDecl& myQualifierDecl)
{
    if (format == OUTPUT_XML)
        XmlWriter::printQualifierDeclElement(myQualifierDecl, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<char> x;
        MofWriter::appendQualifierDeclElement(x, myQualifierDecl);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }
    else
        cout << "Format type error" << endl;
}

void outputFormatCIMValue(const OutputType format, const CIMValue& myValue)
{
    if (format == OUTPUT_XML)
        XmlWriter::printValueElement(myValue, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<char> x;
        MofWriter::appendValueElement(x, myValue);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }
    else
        cout << " Format type error" << endl;
}

///////////////////////////////////////////////////////////////////
//                                                               //
//     The following section  defines each action function       //
//     ex. getInstance. Parameters are defined in the            //
//     opts structure.  There are not exception catches.         //
//     exception handling is in the main path                    //
///////////////////////////////////////////////////////////////////

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
            << ", deepInheritance = " << _toString(opts.deepInheritance)
            << endl;
    }
    // Added to allow "" string input to represent NULL CIMName.
    CIMName myClassName = CIMName();
    /****if (opts.className != "")
    {
        myClassName = opts.className;
    }*/

    Array<CIMName> classNames;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

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
      String s = "instance names of class";
      _displaySummary(instanceNames.size(), s, opts.className.getString(),opts);
        }
        else
        {
            //simply output the list one per line for the moment.
            for (Uint32 i = 0; i < instanceNames.size(); i++)
                        cout << instanceNames[i].toString() << endl;
        }
    }

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}

int enumerateInstanceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateInstanceNames "
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObjectPath> instanceNames =
        client.enumerateInstanceNames(opts.nameSpace,
                                      opts.className);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "instances names of class";
      _displaySummary(instanceNames.size(), s, opts.className.getString(),opts);
    }
    else
    {
        //Output the list one per line for the moment.
        for (Uint32 i = 0; i < instanceNames.size(); i++)
                    cout << instanceNames[i].toString() << endl;
    }

    return(0);
}

int enumerateInstances(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateInstances "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << _toString(opts.deepInheritance)
            << ", localOnly = " << _toString(opts.localOnly)
            << ", includeQualifiers = " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin = " << _toString(opts.includeClassOrigin)
            << ", PropertyList = " << buildPropertyListString(opts.propertyList)
            << endl;
    }

    Array<CIMInstance> instances;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    instances = client.enumerateInstances( opts.nameSpace,
                                           opts.className,
                                           opts.deepInheritance,
                                           opts.localOnly,
                                           opts.includeQualifiers,
                                           opts.includeClassOrigin,
                                           opts.propertyList );

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "instances of class";
        _displaySummary(instances.size(), s, opts.className.getString(),opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < instances.size(); i++)
        {
            CIMInstance instance = instances[i];
            // Check Output Format to print results
            outputFormatInstance(opts.outputType, instance);
        }
    }

    return(0);
}

int executeQuery(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "ExecQuery "
            << "Namespace = " << opts.nameSpace
            << ", queryLanguage = " << opts.queryLanguage
            << ", query = " << opts.query
            << endl;
    }

    Array<CIMObject> objects;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    objects = client.execQuery( opts.nameSpace,
                                opts.queryLanguage,
                                opts.query );

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "instances of class";
        _displaySummary(objects.size(), s, opts.className.getString(),opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
            outputFormatObject(opts.outputType, objects[i]);

    }

    return(0);
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
    // if request is class only, do this interactively
    // Need to get this into objectpath format before doing the call.
    CIMObjectPath thisObject(opts.objectName);
    if ((thisObject.getKeyBindings().size() == 0) ? true : false)
    {
        // get the instance to delete
        if(!_selectInstance(client, opts, CIMName(opts.objectName), thisObject))
            return(0);
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.deleteInstance(opts.nameSpace,
                          thisObject);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}
/*
NAMESPACE = 1
Class = 2
OBJECT = Object*2
OBJECTNAME = OBJECT*2
INSTANCENAME = OBJECTNAME * 2
METHODNAME = INSTANCENAME * 2
PROPERTYNAME = 2
PROPERTYVALUE=2
INPARAMS = 2
DEEPINHERITANCE = 2
LOCALONLY = 2
ASSOCCLASS = 4
RESULTCLASS = 8
ROLE = 16
RESULTROLE = 32
InCLUDEQUALIFIERS = 64
INCLUDECLASSORIGIN = 128
PROPDERTYLIST = 256
*/

int getInstance(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getInstance "
            << "Namespace = " << opts.nameSpace
            << ", Instance = " << opts.objectName
            << ", localOnly = " << _toString(opts.localOnly)
            << ", includeQualifiers = " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin = " << _toString(opts.includeClassOrigin)
            << ", PropertyList = " << buildPropertyListString(opts.propertyList)
            << endl;
    }

    CIMObjectPath thisObject(opts.objectName);

    if ((thisObject.getKeyBindings().size() == 0) ? true : false)
    {
        // get the instance to delete
        if(!_selectInstance(client, opts, CIMName(opts.objectName),thisObject))
            return(0);
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    CIMInstance cimInstance = client.getInstance(opts.nameSpace,
                                                 thisObject,
                                                 opts.localOnly,
                                                 opts.includeQualifiers,
                                                 opts.includeClassOrigin,
                                                 opts.propertyList);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    // Check Output Format to print results
    if (opts.summary)
    {
        if (opts.time)
            cout << opts.saveElapsedTime << endl;

    }
    else
        outputFormatInstance(opts.outputType, cimInstance);

    return(0);
}

/****
    CIMObjectPath createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance
    );
 ***/
int createInstance(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "createInstance "
            << "Namespace = " << opts.nameSpace
            << ", ClassName = " << opts.className
            << endl;
    }
    // get the class. Exceptions including class_not_found are automatic
    CIMClass thisClass =
        client.getClass(opts.nameSpace, opts.className,false,true,true,CIMPropertyList());

    // Tokenize the parameter pairs
    //Array<keyValuePair> inputs;

    Array<CIMName> propertyNameList;
    Array<String> propertyValueList;

    // ATTN: Need to account for returning key without value here.
    if (opts.extraParams != 0)
    {
        for (Uint32 i = 0 ; i < opts.extraParams.size() ; i++)
        {
            String key;
            String value;
            _tokenPair(opts.extraParams[i], key, value);
            propertyNameList.append(CIMName(key));
            propertyValueList.append(value);
            if (thisClass.findProperty(CIMName(key)) == PEG_NOT_FOUND)
                cout << "Warning property Name not in class: " << opts.extraParams[i] << endl;
        }

        if (opts.verboseTest)
        {
            cout << "Property: " << propertyNameList[propertyNameList.size()]
            << " value: " << propertyValueList[propertyValueList.size()]
            << endl;
        }
    }

    CIMPropertyList myPropertyList(propertyNameList);
    // create the instance with the defined properties
    CIMInstance newInstance = thisClass.buildInstance(true, true, myPropertyList);

    // Now add the parameters from the input. Array.
    //Note that we do NO checking.  Each input parameter is a simple
    //name=value.
    // At this point we also treat them all as strings since we have not
    // defined a means to handle typing.

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    CIMObjectPath rtndPath = client.createInstance(opts.nameSpace,
                                                 newInstance);

    // Need to put values into the parameters.

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    // Check Output Format to print results
    if (opts.summary)
    {
        if (opts.time)
            cout << opts.saveElapsedTime << endl;
    }
    else
    {
        cout << rtndPath.toString() << endl;;
    }

    return(0);
}

int enumerateClassNames(CIMClient& client, Options& opts)
{

    if (opts.verboseTest)
    {
        cout << "EnumerateClasseNames "
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className
            << ", deepInheritance= " << (opts.deepInheritance? "true" : "false")
            << endl;
    }
    Array<CIMName> classNames;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    classNames = client.enumerateClassNames(opts.nameSpace,
                                        opts.className,
                                        opts.deepInheritance);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "class names";
        _displaySummary(classNames.size(), s,
             opts.className.getString(),opts);
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < classNames.size(); i++)
                cout << classNames[i] << endl;
    }

    return(0);
}

int enumerateClasses(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateClasses "
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className
            << ", deepInheritance= " << _toString(opts.deepInheritance)
            << ", localOnly= " << _toString(opts.localOnly)
            << ", includeQualifiers= " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin= " << _toString(opts.includeClassOrigin)
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

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    classes = client.enumerateClasses(opts.nameSpace,
                                        opts.className,
                                        opts.deepInheritance,
                                        opts.localOnly,
                                        opts.includeQualifiers,
                                        opts.includeClassOrigin);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "classes";
        _displaySummary(classes.size(), s, opts.className.getString(),opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < classes.size(); i++)
        {
            CIMClass myClass = classes[i];
            outputFormatClass(opts.outputType, myClass);
        }
    }

    return(0);
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

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.deleteClass(opts.nameSpace, opts.className);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}

int getClass(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getClass "
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className
            << ", deepInheritance= " << _toString(opts.deepInheritance)
            << ", localOnly= " << _toString(opts.localOnly)
            << ", includeQualifiers= " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin= " << _toString(opts.includeClassOrigin)
            << ", PropertyList= " << buildPropertyListString(opts.propertyList)
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    CIMClass cimClass = client.getClass(opts.nameSpace,
                                        opts.className,
                                        opts.localOnly,
                                        opts.includeQualifiers,
                                        opts.includeClassOrigin,
                                        opts.propertyList);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    outputFormatClass(opts.outputType, cimClass);

    return(0);
}

int getProperty(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getProperty "
            << "Namespace= " << opts.nameSpace
            << ", Instance= " << opts.instanceName.toString()
            << ", propertyName= " << opts.propertyName
            << endl;
    }

    CIMValue cimValue;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    cimValue = client.getProperty( opts.nameSpace,
                                   opts.instanceName,
                                   opts.propertyName);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    // ATTN: TODO: display returned property
    return(0);
}

int setProperty(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "setProperty "
            << "Namespace= " << opts.nameSpace
            << ", Instance= " << opts.instanceName.toString()
            << ", propertyName= " << opts.propertyName
            << ", newValue= " << opts.newValue.toString()
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.setProperty( opts.nameSpace,
                                   opts.instanceName,
                                   opts.propertyName,
                                   opts.newValue);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}

int getQualifier(CIMClient& client, Options& opts)
{
    cout << "entering getQualifier " << endl;
    //if (opts.verboseTest)
    {
        cout << "getQualifier "
            << "Namespace= " << opts.nameSpace
            << ", Qualifier= " << opts.qualifierName
            << endl;
    }

    CIMQualifierDecl cimQualifierDecl;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    cimQualifierDecl = client.getQualifier( opts.nameSpace,
                                   opts.qualifierName);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    // display new qualifier

    outputFormatQualifierDecl(opts.outputType, cimQualifierDecl);

    return(0);
}

int setQualifier(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "setQualifiers "
            << "Namespace= " << opts.nameSpace
            // KS add the qualifier decl here.
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.setQualifier( opts.nameSpace,
                         opts.qualifierDeclaration);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}

int deleteQualifier(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteQualifiers "
            << "Namespace= " << opts.nameSpace
            << " Qualifier= " << opts.qualifierName
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.deleteQualifier( opts.nameSpace,
                            opts.qualifierName);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}
int enumerateQualifiers(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "enumerateQualifiers "
            << "Namespace= " << opts.nameSpace
            << endl;
    }

    Array<CIMQualifierDecl> qualifierDecls;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    qualifierDecls = client.enumerateQualifiers( opts.nameSpace);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
        cout << qualifierDecls.size() << " returned." << endl;
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < qualifierDecls.size(); i++)
        {
            CIMQualifierDecl myQualifierDecl = qualifierDecls[i];
            outputFormatQualifierDecl(opts.outputType, myQualifierDecl);
        }
    }

    return(0);
}

/*Array<CIMObjectPath> referenceNames(
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
            << "Namespace= " << opts.nameSpace
            << ", ObjectPath= " << opts.objectName
            << ", resultClass= " << opts.resultClass
            << ", role= " << opts.role
            << endl;
    }
    // do conditional select of instance if params properly set.
    CIMObjectPath thisObjectPath(opts.objectName);
    if (!_conditionalSelectInstance(client, opts, thisObjectPath))
        return(0);

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObjectPath> referenceNames =
        client.referenceNames( opts.nameSpace,
                               thisObjectPath,
                               opts.resultClass,
                               opts.role);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    /*
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass = CIMName(),
    const String& role = String::EMPTY */

    if (opts.summary)
    {
      String s = "referenceNames";
        _displaySummary(referenceNames.size(),s,
             opts.objectName,opts);
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < referenceNames.size(); i++)
                    cout << referenceNames[i].toString() << endl;
    }

    return(0);
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
            << "Namespace= " << opts.nameSpace
            << ", Object= " << opts.objectName
            << ", resultClass= " << opts.resultClass
            << ", role= " << opts.role
            << ", includeQualifiers= " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin= " << _toString(opts.includeClassOrigin)
            << ", CIMPropertyList= "  << buildPropertyListString(opts.propertyList)
            << endl;
    }

    // do conditional select of instance if params properly set.
    CIMObjectPath thisObjectPath(opts.objectName);
    if (!_conditionalSelectInstance(client, opts, thisObjectPath))
        return(0);

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObject> objects =
        client.references(  opts.nameSpace,
                            thisObjectPath,
                            opts.resultClass,
                            opts.role,
                            opts.includeQualifiers,
                            opts.includeClassOrigin,
                            opts.propertyList);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "references";
        _displaySummary(objects.size(), s, opts.objectName,opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
            outputFormatObject(opts.outputType, objects[i]);
    }

    return(0);
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
            << "Namespace= " << opts.nameSpace
            << ", Object= " << opts.objectName
            << ", assocClass= " << opts.assocClass
            << ", resultClass= " << opts.resultClass
            << ", role= " << opts.role
            << ", resultRole= " << opts.resultRole
            << endl;
    }

    // do conditional select of instance if params properly set.
    CIMObjectPath thisObjectPath(opts.objectName);
    if(!_conditionalSelectInstance(client, opts, thisObjectPath))
        return(0);

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObjectPath> associatorNames =
    client.associatorNames( opts.nameSpace,
                            thisObjectPath,
                            opts.assocClass,
                            opts.resultClass,
                            opts.role,
                            opts.resultRole);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    /*
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass = CIMName(),
    const String& role = String::EMPTY */

    if (opts.summary)
    {
      String s = "associator names";
        _displaySummary(associatorNames.size(), s,
             opts.objectName,opts);
    }
    else
    {
        // Output the list one per line for the moment.
        for (Uint32 i = 0; i < associatorNames.size(); i++)
                    cout << associatorNames[i].toString() << endl;
    }

    return(0);
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
            << "Namespace= " << opts.nameSpace
            << ", Object= " << opts.objectName
            << ", assocClass= " << opts.assocClass
            << ", resultClass= " << opts.resultClass
            << ", role= " << opts.role
            << ", resultRole= " << opts.resultRole
            << ", includeQualifiers= " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin= " << _toString(opts.includeClassOrigin)
            << ", propertyList= " << buildPropertyListString(opts.propertyList)
            << endl;
    }

    // do conditional select of instance if params properly set.
    CIMObjectPath thisObjectPath(opts.objectName);
    if(!_conditionalSelectInstance(client, opts, thisObjectPath))
        return(0);

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObject> objects =
    client.associators( opts.nameSpace,
                        thisObjectPath,
                        opts.assocClass,
                        opts.resultClass,
                        opts.role,
                        opts.resultRole,
                        opts.includeQualifiers,
                        opts.includeClassOrigin,
                        opts.propertyList);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "associators";
        _displaySummary(objects.size(), s, opts.objectName,opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
            outputFormatObject(opts.outputType, objects[i]);
    }

    return(0);
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
     {
         // Display the parameter set if verbose requested.
         if (opts.verboseTest)
         {
             cout << "invokeMethod"
                 << " Namespace= " << opts.nameSpace
                 << ", ObjectName= " << opts.objectName
                 << ", methodName= " << opts.methodName
                 << ", inParams Count= " << opts.inParams.size()
                 << endl;
             for (Uint32 i=0; i< opts.inParams.size(); i++)
                 outputFormatParamValue(opts.outputType, opts.inParams[i]);
        }

         // Create array for output parameters
        CIMValue retValue;
        Array<CIMParamValue> outParams;

        if (opts.time)
        {
            opts.elapsedTime.reset();
            opts.elapsedTime.start();
        }

        // Call invoke method with the parameters
        retValue = client.invokeMethod(opts.nameSpace, opts.objectName,
            opts.methodName, opts.inParams, outParams);

        if (opts.time)
        {
            opts.elapsedTime.stop();

            opts.saveElapsedTime = opts.elapsedTime.getElapsed();
        }

        // Display the return value CIMValue
        cout << "Return Value= ";
        if (opts.outputType == OUTPUT_XML)
            XmlWriter::printValueElement(retValue, cout);
        else
            cout << retValue.toString() << endl;

        // Display any outparms
        for (Uint32 i = 0; i < outParams.size() ; i++)
            outputFormatParamValue(opts.outputType, outParams[i]);

     }

     return(0);
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
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className
            << endl;
    }

    Boolean usingPegasus = true;
    Array<CIMInstance> instances;

    try
    {
        instances = client.enumerateInstances((CIMNamespaceName)(opts.nameSpace),opts.className);
    }
    catch(CIMException &)
    {
        /*if an exception was caught here then we assume we are not useing
        the open pegasus CIMOM. There for we should only check the
        __namespaces class. (Which may only retrun a subset of all namspaces
        */
        usingPegasus = false;
        opts.className = CIMName("__namespace");
        opts.nameSpace = "root/PG_InterOp";
    }

    if (usingPegasus)
    {
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
                    cout << namespaceComponent << endl;
                }
            }
        }

    }




    if (!usingPegasus)
    {
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


        if (opts.time)
        {
            opts.elapsedTime.reset();
            opts.elapsedTime.start();
        }

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

        if (opts.time)
        {
            opts.elapsedTime.stop();

            opts.saveElapsedTime = opts.elapsedTime.getElapsed();
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
    }

    return(0);
}

/////////////////////////////////////////////////////////////////////
// The following section manages the input options.  It includes   //
// both the options processor (based on common/options             //
// and the options table.                                          //
/////////////////////////////////////////////////////////////////////

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
        {"count", "29346", false, Option::WHOLE_NUMBER, 0, 0, "count",
                            "Expected count of objects returned if the summary set. Tests this count and display difference. Term nonzero if test fails  "},

        {"debug", "false", false, Option::BOOLEAN, 0, 0, "d",
                            "More detailed debug messages "},

        {"delay", "0", false, Option::WHOLE_NUMBER, 0, 0, "delay",
                            "Delay between connection and request "},

        {"Password", "", false, Option::STRING, 0, 0, "p",
                                        "Defines password for authentication" },

        {"location", "", false, Option::STRING, 0, 0, "l",
                            "specifies system and port (HostName:port). Port is optional" },

        {"User", "", false, Option::STRING, 0, 0, "u",
                                        "Defines User Name for authentication" },

        {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "n",
                            "Specifies namespace to use for operation" },

        {"deepInheritance", "false", false, Option::BOOLEAN, 0, 0, "di",
                            "If set deepInheritance parameter set true "},

        {"localOnly", "true", false, Option::BOOLEAN, 0, 0, "lo",
                            "DEPRECATED. This was used to set LocalOnly. However, default should be true and we cannot use True as default. See !lo "},

        {"!localOnly", "false", false, Option::BOOLEAN, 0, 0, "!lo",
                            "When set, sets LocalOnly = false on operations. DEPRECATED, ! confuses bash. Use -nlo "},

        {"notLocalOnly", "false", false, Option::BOOLEAN, 0, 0, "nlo",
                            "When set, sets LocalOnly = false on operations "},

        {"includeQualifiers", "true", false, Option::BOOLEAN, 0, 0, "iq",
                            "Deprecated. Sets includeQualifiers = True. However, default = true "},

        {"!includeQualifiers", "false", false, Option::BOOLEAN, 0, 0, "!iq",
                            "Sets includeQualifiers = false on operations. DEPRECATED, ! confuses bash. Use -niq"},

        {"notIncludeQualifiers", "false", false, Option::BOOLEAN, 0, 0, "niq",
                            "Sets includeQualifiers = false on operations"},

        // Uses a magic string as shown below to indicate never used.
        {"propertyList", "###!###", false, Option::STRING, 0, 0, "pl",
                            "Defines a propertyNameList. Format is p1,p2,p3 (without spaces). Use \"\" for empty."},

        {"assocClass", "", false, Option::STRING, 0, 0, "ac",
                            "Defines a assocClass string for Associator calls"},

        {"assocRole", "", false, Option::STRING, 0, 0, "ar",
                            "Defines a role string for Associatiors AssocRole parameter"},

        {"role", "", false, Option::STRING, 0, 0, "r",
                            "Defines a role string for reference role parameter"},

        {"resultClass", "", false, Option::STRING, 0, 0, "rc",
                            "Defines a resultClass string for References and Associatiors "},

        {"resultRole", "", false, Option::STRING, 0, 0, "rr",
                            "Defines a role string for associators operation resultRole parameter. "},

        {"inputParameters", "", false, Option::STRING, 0, 0, "ip",
                            "Defines an invokeMethod input parameter list. Format is p1=v1,p2=v2,..,pn=vn (without spaces) "},

        {"filter", "", false, Option::STRING, 0, 0, "f",
                            "defines a filter to use for query. Single String input "},

        // This was never used.  Delete. KS
        //{"substitute", "", false, Option::STRING, 0, 0, "-s",
        //                    "Defines a conditional substition of input parameters. ) "},

        // KS change the output formats to use the enum options function
        // Deprecate this function.
        {"outputformats", "mof", false, Option::STRING, 0,NUM_OUTPUTFORMATS, "o",
                            "Output in xml, mof, txt"},

        {"xmlOutput", "false", false, Option::BOOLEAN, 0,0, "x",
                            "Output objects in xml instead of mof format"},

        {"version", "false", false, Option::BOOLEAN, 0, 0, "-version",
                            "Displays software Version "},

        {"verbose", "false", false, Option::BOOLEAN, 0, 0, "v",
                            "Verbose Display. Includes Detailed Param Input display "},


        {"summary", "false", false, Option::BOOLEAN, 0, 0, "s",
                            "Displays only summary count for enumerations, associators, etc. "},

        {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
                            "Prints help usage message "},

        {"full help", "false", false, Option::BOOLEAN, 0, 0, "-help",
                            "Prints full help message with commands, options, examples "},
        {"help options", "false", false, Option::BOOLEAN, 0, 0, "ho",
                            "Prints list of options "},

        {"help commands", "false", false, Option::BOOLEAN, 0, 0, "hc",
                            "Prints CIM Operation command list "},

        {"connecttimeout", "0", false, Option::WHOLE_NUMBER, 0, 0, "-timeout",
                            "Set the connection timeout in seconds. "},

        {"interactive", "false", false, Option::BOOLEAN, 0, 0, "i",
                            "Interactively ask user to select instances.  Used with associator and reference operations "},

        {"trace", "0", false, Option::WHOLE_NUMBER, 0, 0, "trace",
                            "Set Pegasus Common Components Trace. Sets the Trace level. 0 is off"},

        {"repeat", "0", false, Option::WHOLE_NUMBER, 0, 0, "-r",
                            "Number of times to repeat the function. Zero means one time "},

        {"time", "false", false, Option::BOOLEAN, 0, 0, "-t",
                            "Measure time for the operation and present results. "},

    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    String configFile = "CLI.conf";

    //cout << "Config file from " << configFile << endl;

    if (FileSystem::exists(configFile))
             om.mergeFile(configFile);

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();

}

/* Remap a long string into a string that can be positioned on a line
   starting at pos and with length but broken into multiple lines.
   The input string is recreated and filled from the left so that the
   returned string can be output as a multiline string starting at pos.
*/
String formatLongString (const char * input, Uint32 pos, Uint32 length)
{
    String output;
    String work = input;
    Array<String> list;

    // create the fill string starting with the newline character
    String fill;
    fill.append("\n");
    for (Uint32 i = 0; i < pos; i++)
        fill.append (" ");

    list = _tokenize(work, ' ');

    for (Uint32 i = 0 ; i < list.size() ; i++)
    {
        // move a single word and either a space or create new line
        if (((output.size() % length) + list[i].size()) >= (length))
            output.append(fill);
        else
            output.append(" ");

        output.append(list[i]);
    }
    return(output);
}

void showUsage(const char* pgmName)
{
    cout << "Usage: " << pgmName << "<command> <CIMObject> <Options> *<extra parameters>" << endl
        << "    -hc    for <command> set and <CimObject> for each command\n"
        << "    -ho    for <Options> set\n"
        << "    -h     for this summary\n"
        << "    --help for full help" << endl;
}
/* showCommands - Display the list of operation commands.
*/
const char * helpSummary = " -h for all help, -hc for commands, -ho for options";
void showCommands(const char* pgmName)
{
    for( Uint32 i = 0; i < NUM_COMMANDS; i++ )
    {

        String txtFormat = formatLongString(CommandTable[i].UsageText,28 ,75 - 28 );

        printf("%-5s %-21s",CommandTable[i].ShortCut, CommandTable[i].CommandName);
        cout << txtFormat << endl;
    }
    cout << helpSummary << endl;
}
void showVersion(const char* pgmName, OptionManager& om)
{
    cout << endl << pgmName << " V"<< version << " "
        " using Pegasus version " << PEGASUS_PRODUCT_VERSION << endl;
}

void showOptions(const char* pgmName, OptionManager& om)
{

    String optionsTrailer = "Options vary by command consistent with CIM Operations";
    cout << "The options for this command are:" << endl;
    om.printOptionsHelpTxt(usage, optionsTrailer);
    //om.printHelp(const char* pgmName, OptionManager om);
}

/* PrintHelpMsg - This is temporary until we expand the options manager to allow
   options help to be defined with the OptionRow entries and presented from
   those entries.
*/
void printHelpMsg(const char* pgmName, const char* usage, const char* extraHelp,
                OptionManager& om)
{
    showUsage(pgmName);

    showVersion(pgmName, om);

    showOptions(pgmName, om);

    showCommands(pgmName);

    cout << endl;

    cout << extraHelp << endl;
}

void printUsageMsg(const char* pgmName,OptionManager& om)
{
    printHelpMsg(pgmName, usage, usageDetails, om);
}

int CheckCommonOptionValues(OptionManager& om, char** argv, Options& opts)
{
    // Catch the verbose and debug options first so they can control other
    // processing
    Boolean verboseTest = (om.valueEquals("verbose", "true")) ? true :false;
    Boolean debug = (om.valueEquals("debug", "true")) ? true :false;

    if (verboseTest)
        opts.verboseTest = verboseTest;

    if (debug)
    {
        opts.debug= debug;
    }

    // Base code for parameter substition.  Dispabled until complete
    /*{
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
    }*/


    if (om.isTrue("full help"))
    {
                printHelpMsg(argv[0], usage, usageDetails, om);
                exit(0);
    }

    if (om.isTrue("help"))
    {
                showUsage(argv[0]);
                exit(0);
    }
    if (om.isTrue("version"))
    {
        showVersion(argv[0], om);
        exit(0);
    }

    if (om.isTrue("help options"))
    {
        showOptions(argv[0], om);
        exit(0);
    }

    if (om.isTrue("help commands"))
    {
        showCommands(argv[0]);
        exit(0);
    }

    // Establish the namespace from the input parameters
    //String nameSpace;
    if(om.lookupValue("namespace", opts.nameSpace))
    {
        if (verboseTest && debug)
            cout << "Namespace = " << opts.nameSpace << endl;
    }

    String temprole;
    if(om.lookupValue("role", temprole))
    {
        // we need to deliver String::EMPTY when no param.
        if (temprole != "")
            opts.role = temprole;
        if (verboseTest && debug && temprole != "")
           cout << "role = " << opts.role << endl;
    }

    String tempResultRole;
    if(om.lookupValue("resultRole", tempResultRole))
    {
        // we need to deliver String::EMPTY when no param.
        if (tempResultRole != "")
            opts.resultRole = tempResultRole;
        if (verboseTest && debug && tempResultRole != "")
           cout << "resultRole= " << opts.resultRole << endl;
    }

    // Get value for location, i.e. host, etc.
    om.lookupValue("location", opts.location);

    // Assign the result class
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
       if (verboseTest && debug && opts.resultClassName != "")
           cout << "resultClassName = " << opts.resultClassName << endl;
    }

    if(om.lookupValue("assocClass", opts.assocClassName))
    {
       if (verboseTest && debug && opts.assocClassName != "")
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

    // Evaluate connectiontimeout option.
    opts.connectionTimeout = 0;
    if(!om.lookupIntegerValue("connecttimeout", opts.connectionTimeout))
    {
            opts.connectionTimeout = 0;
    }

    if (verboseTest && debug && opts.connectionTimeout != 0)
    {
        cout << "Connection Timeout= " << opts.connectionTimeout << " Seconds" << endl;
    }

    if(!om.lookupIntegerValue("delay", opts.delay))
    {
            opts.delay = 0;
    }

    if (verboseTest && debug && opts.delay != 0)
    {
        cout << "delay= " << opts.delay << " Seconds" << endl;
    }

    // Set the interactive request flag based on input
    opts.interactive = om.isTrue("interactive");
    if (om.isTrue("interactive")  && verboseTest && debug)
        cout << "interactive request" << endl;

    // set the deepInheritance flag based on input
    opts.deepInheritance = om.isTrue("deepInheritance");
    if (om.isTrue("deepInheritance")  && verboseTest && debug)
        cout << "deepInteritance set" << endl;

    // process localOnly and !localOnly parameters
    opts.localOnly = om.isTrue("localOnly");
    if (om.isTrue("!localOnly") || om.isTrue("notNocalOnly"))
    {
        opts.localOnly = false;
    }

    // Use two options for the not command because the ! confuses bash
    // Either is legal and they do the same thing.
    // Used the not version because the DMTF and pegasus default is true
    if (verboseTest && debug && (om.isTrue("!localOnly") || om.isTrue("notLocalOnly")))
    {
        cout << "localOnly= " << _toString(opts.localOnly) << endl;;
    }

    // Process includeQualifiers and !includeQualifiers
    opts.includeQualifiers = om.isTrue("includeQualifiers");

    if (om.isTrue("!includeQualifiers") || om.isTrue("notIncludeQualifiers"))
    {
        opts.includeQualifiers = false;
    }

    if (verboseTest && debug && (om.isTrue("!includeQualifiers") || om.isTrue("notIncludeQualifiers" )))
    {
        cout << "includeQualifiers = " << _toString(opts.includeQualifiers) << endl;
    }

    opts.includeClassOrigin = om.isTrue("includeClassOrigin");
    if (om.isTrue("includeClassOrigin")  && verboseTest && debug)
        cout << "includeClassOrigin set" << endl;

    opts.time = om.isTrue("time");
    if (om.isTrue("time")  && verboseTest && debug)
        cout << "time set" << endl;

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
    if (verboseTest && debug && opts.trace != 0)
    {
        cout << "Pegasus Trace set to  Level  " << opts.trace << endl;
    }

    opts.summary = om.isTrue("summary");

    // get User name and password if set.
     if(om.lookupValue("User", opts.user))
     {
         if (opts.user.size() == 0)
             opts.user = String::EMPTY;
         if (debug && verboseTest)
             cout << "User= " << opts.user << endl;
     }

     if(om.lookupValue("Password", opts.password))
     {
         if (opts.password.size() == 0)
             opts.password = String::EMPTY;
         if (debug && verboseTest)
             cout << "Password= " << opts.password << endl;
     }

    // Create a variable with the format output and a correponding type.
    // Suggest we might change this whole thing to the option type that
    // mike used in the example of colors so that  you could do -red -blue
    // or in our case -mof -xml, etc.

     opts.isXmlOutput = om.isTrue("xmlOutput");
     if (opts.isXmlOutput  && debug && verboseTest)
         cout << "xmlOutput set" << endl;

    if(om.lookupValue("outputformats", opts.outputFormat))
     {
        if (debug && verboseTest)
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

    if(!om.lookupIntegerValue("repeat", opts.repeat))
    {
            opts.repeat = 0;

    }
    if (debug && verboseTest)
    {
        cout << "Repeat Count= " << opts.repeat << endl;
    }

    if(!om.lookupIntegerValue("count", opts.count))
    {
            opts.count = 29346;
    }
    if (debug && verboseTest)
    {
        if (opts.count != 29346)
        {
        cout << "Comparison Count= " << opts.count << endl;
        }
    }

    /*  Property List parameter.
        Separate an input stream into an array of Strings
        Two special situations, empty list and NULL list
        Use NULL when there is no list.
        Use empty if
    */
    {
        String properties;
        if(om.lookupValue("propertyList", properties))
        {
            if (properties == "###!###")
            {
                opts.propertyList.clear();
            }
            else
            {
                Array<CIMName> pList;
                Array<String> pListString =  _tokenize(properties, ',');

                for (Uint32 i = 0 ; i < pListString.size(); i++)
                {
                    pList.append(CIMName(pListString[i]));
                }
                opts.propertyList.set(pList);
            }
            if (debug && verboseTest && properties != "###!###")
            {
                cout << "PropertyList= ";
                printPropertyList(opts.propertyList);
                cout << endl;
            }
        }
    }

    /* Method input parameters processing.  Process as one
       string containing multiple parameters in the form
       name=value
       ATTN: KS 20030426 Note that we have not accounted for
       the typing on parameters.
    */
    String inputParameters;
    if(om.lookupValue("inputParameters", inputParameters))
    {
        // first tokenization is the ','
        Array<String> pList =  _tokenize(inputParameters, ',');
        for (Uint32 i = 0 ; i< pList.size() ; i++)
        {
            CIMParamValue pv;
            pv = _createMethodParamValue(pList[i], opts);
            opts.inParams.append(pv);
        }
    }
    return 0;
}


//------------------------------------------------------------------------------
//
// mofFormat
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

/* Format the output stream for indented MOF format
*/
void mofFormat(
    PEGASUS_STD(ostream)& os,
    const char* text,
    Uint32 indentSize)
{
    char* var = new char[strlen(text)+1];
    char* tmp = strcpy(var, text);
    Uint32 count = 0;
    Uint32 indent = 0;
    Boolean quoteState = false;
    Boolean qualifierState = false;
    char c;
    char prevchar = 0;
    while ((c = *tmp++))
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
    delete [] var;
}

PEGASUS_NAMESPACE_END
// END_OF_FILE
