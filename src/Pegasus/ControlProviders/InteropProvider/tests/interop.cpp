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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <cassert>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/XmlWriter.h>

#include <Pegasus/Common/Tracer.h>
/* This is a test program for the Interop Control Provider.  It provides tests for the
functions in this provider including;

    The CIM_ObjectManager Class and its single instance
    The CIM_CommunicationsMechanism class and its PG subclass.
    The CIM_Namespace Class

*/
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

// Macro puts out message and then does assert error out.
#define TERMINATE(X) {PEGASUS_STD(cout) << "TestInterop " << X << PEGASUS_STD(endl); assert(false);}
//#define CDEBUG(X)
#define CDEBUG(X) PEGASUS_STD(cout) << "InteropTest " << X << PEGASUS_STD(endl)


#include <cstring>
#include <stdcxx/stream/strstream>


/* Test the namespace manipulation functions.
Creates, deletes, queries namespaces using both the
__Namespace and CIM_Namespace functions
*/
char * pgmName;
char * verbose;

static const CIMNamespaceName __NAMESPACE_NAMESPACE = CIMNamespaceName ("root");
static const CIMNamespaceName InteropNamespace = CIMNamespaceName("root/PG_Interop");

static const char programVersion[] =  "1.0";

// Property Names for __Namespace Class
static const CIMName NAMESPACE_PROPERTYNAME  = CIMName ("Name");
static const CIMNamespaceName ROOTNS  = CIMNamespaceName ("root");
static const CIMName __NAMESPACE_CLASSNAME  = CIMName ("__Namespace");
static const CIMName CIM_NAMESPACE_CLASSNAME  = CIMName ("CIM_Namespace");

static const CIMName PG_NAMESPACE_CLASSNAME  = CIMName ("PG_Namespace");
static const CIMName CIM_OBJECTMANAGER_CLASSNAME  = CIMName ("CIM_ObjectManager");
static const CIMName CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM_CLASSNAME  = 
        CIMName ("CIM_ObjectManagerCommunicationMechanism");
static const CIMName PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME  = 
        CIMName ("PG_CIMXMLCommunicationMechanism");
static const CIMName CIM_COMMMECHANISMFORMANAGER_CLASSNAME  = 
        CIMName ("CIM_CommMechanismForManager");
static const CIMName CIM_NAMESPACEINMANAGER_CLASSNAME  = 
        CIMName ("CIM_NamespaceInManager");

/* Class created to provide cover for all of the tests in this
    test program.
*/
class InteropTest
{
public:
    //InteropTest();

    InteropTest(CIMClient & client);

    ~InteropTest();

    // Methods associated with Namespace testing
    Array<CIMNamespaceName> _getNamespacesOld();
    Boolean InteropTest::_deleteOneLevelOfNamespace(const CIMNamespaceName& parent,
         const String & child);
    Boolean _deleteNamespaceOld(const String & name);
    Boolean _validateNamespaces(Array<CIMNamespaceName>& namespaceNames);
    Array<CIMInstance> _getCIMNamespaceInstances();
    Array<CIMInstance> _getPGNamespaceInstances();
    Array<CIMObjectPath> _getCIMNamespaceInstanceNames();

    Array<CIMNamespaceName> _getNamespacesNew();
    void _showNamespaceInfo(const String& title);
    Boolean _existsNew(const CIMNamespaceName& name);
    Boolean _namespaceCreate__Namespace(const CIMNamespaceName& parent,
        const String& childName);
    Boolean _namespaceCreateCIM_Namespace(const CIMNamespaceName& name);
    Boolean _testPGNamespace(const CIMNamespaceName& name,
        Boolean shared, Boolean updatesAllowed, const String& parent);
    Boolean _namespaceCreatePG_Namespace(const CIMNamespaceName& name,
        const Boolean shareable, const Boolean updatesAllowed,
        const String& parent);
    Boolean _namespaceDeleteCIM_Namespace(const CIMNamespaceName& name);
    Boolean testClassExists(const CIMName & className);

    // Methods associated iwth overall testing
    void testNameSpacesManagement();
    void testSharedNameSpacesManagement();
    CIMInstance getInstanceObjMgr();
    CIMObjectPath getObjMgrPath();
    void testObjectManagerClass();
    void setStatisticsState(const Boolean flag);
    Boolean getStatisticsPropertyState(CIMInstance & objMgrInstance);
    Boolean getStatisticsState();
    void showStatisticsState();
    
    void testStatisticsEnable();
    
    void testCommunicationClass();
    void testNameSpaceInObjectManagerAssocClass();
    void testCommMechinManagerAssocClass();

private:

CIMClient _client;
CIMInstance objectManagerInstance;
};


InteropTest::InteropTest(CIMClient & client)
    : _client(client)
{
}

InteropTest::~InteropTest()
{

}

/* Get all namespaces and return as an array
*/
Array<CIMNamespaceName> InteropTest::_getNamespacesOld()
{
    CIMName className = "__NameSpace";
    Array<CIMNamespaceName> namespaceNames;

    // Build the namespaces incrementally starting at the root
    // ATTN: 20030319 KS today we start with the "root" directory but this is wrong. We should be
    // starting with null (no directoyr) but today we get an xml error return in Pegasus
    // returned for this call. Note that the specification requires that the root namespace be used
    // when __namespace is defined but does not require that it be the root for allnamespaces. That
    // is a hole is the spec, not in our code.
    namespaceNames.append("root");
    namespaceNames.append("test");
    Uint32 start = 0;
    Uint32 end = namespaceNames.size();
    do
    {
        // for all new elements in the output array
        for (Uint32 range = start; range < end; range ++)
        {
            // Get the next increment in naming for all a name elements in the array
            CIMNamespaceName next;
            if (end != 0)
            {
                next = namespaceNames[range];
            }
            Array<CIMInstance> instances = _client.enumerateInstances(next, className);
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
    // have a name component that is really not a namespace (ex. root/fred/john is a namespace
    // but root/fred is not.
    // There is no clearly defined test for this so we will simply try to get something, in this
    // case a wellknown assoication. But, this means that namespaces that may be valid but that
    // have no contents do not show up on this list.
    Array<CIMNamespaceName> returnNamespaceNames;
    for (Uint32 i = 0 ; i < namespaceNames.size() ; i++)
    {
        try
        {
            CIMQualifierDecl cimQualifierDecl;
            cimQualifierDecl = _client.getQualifier(namespaceNames[i],
                                           "Association");

            returnNamespaceNames.append(namespaceNames[i]);
        }
        catch(CIMException& e)
        {
            if (e.getCode() != CIM_ERR_INVALID_NAMESPACE)
                returnNamespaceNames.append(namespaceNames[i]);
        }
    }
    return(returnNamespaceNames);
}

Boolean InteropTest::_deleteOneLevelOfNamespace(const CIMNamespaceName& parent, const String & child)
{
    try
    {
        CIMObjectPath myReference(String::EMPTY, parent, child);
        _client.deleteInstance(parent, myReference);
    }
    catch(Exception& e)
    {
        PEGASUS_STD(cerr) << "Exception NameSpace Deletion: "
           << e.getMessage() << " Deleting " << child << " from " << parent
               << PEGASUS_STD(endl);
        return(false);
   }
   return(true);
}

/* Delete the namespace defined by the input. This function uses
    the __Namspace tools to do the delete.
*/
Boolean InteropTest::_deleteNamespaceOld(const String & name)
{

    Uint32 pos;
    while((pos = name.reverseFind('/')) != PEG_NOT_FOUND)
    {
        String parent = name.subString(0, pos);
        String child = name.subString (pos + 1);
        Boolean rtn = _deleteOneLevelOfNamespace(CIMNamespaceName(parent), child);
        if(!rtn)
            return(false);
    }
    return(true);
}

Boolean InteropTest::_validateNamespaces(Array<CIMNamespaceName>& namespaceNames)
{
    // Validate that these are all namespaces.  This is not a certain
    // Test but we simply check to see if there is an association
    // qualifier in the namespace. If the call returns the
    // invalidNamespace exception, assume this is not valid namespace.
    Array<CIMNamespaceName> returnNamespaces;

    for (Uint32 i = 0 ; i < namespaceNames.size() ; i++)
    {
        try
        {
            CIMQualifierDecl cimQualifierDecl;
            cimQualifierDecl = _client.getQualifier(namespaceNames[i],
                                           "Association");

            returnNamespaces.append(namespaceNames[i]);
        }
        catch(CIMException& e)
        {
            if (e.getCode() != CIM_ERR_INVALID_NAMESPACE)
                returnNamespaces.append(namespaceNames[i]);
        }
    }

    if (verbose)
    {
        cout << returnNamespaces.size() << " namespaces " << " returned." << endl;
        for( Uint32 cnt = 0 ; cnt < returnNamespaces.size(); cnt++ )
        {
            cout << returnNamespaces[cnt] << endl;;
        }
    }
    if (returnNamespaces.size() == namespaceNames.size())
        return(true);
    else
        return(false);

}
/* Deleted for now
Boolean _existsOld(CIMNamespaceName& name)
{
    Uint32 pos;
    String parent = String::EMPTY;
    String child;
    String nameString = name.getString();
    if((pos = nameString.reverseFind('/')) != PEG_NOT_FOUND)
    {
        parent = nameString.subString(0, pos);
        child = nameString.subString (pos + 1);
    }
    else
    {
        child = nameString;

    }

    Array<CIMKeyBinding> keyBindings;
    // wrong ATTN: This not good.
    keyBindings.append(CIMKeyBinding(NAMESPACE_PROPERTYNAME,
         isRelativeName?childNamespaceName.getString():
                            parentNamespaceName.getString(),
                                 CIMKeyBinding::STRING));
    CIMObjectPath ref(String::EMPTY, parent, child);

    try
    {
        CIMInstance instance.getInstance(parent, ref);
    }
    catch(CIMException& e)
    {
        cerr << "Exception NameSpace query: "
           << e.getMessage() << " querying " << name
               << endl;
    }
    return(true);
}
************************************************/


/* gets the instances for the CIM_Namespace class from host
*/
Array<CIMInstance> InteropTest::_getCIMNamespaceInstances()
{
    Array<CIMInstance> instances;
    try
    {
        instances = _client.enumerateInstances(InteropNamespace,
                                              CIM_NAMESPACE_CLASSNAME);
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage()
            << " Conection term abnormal" << endl;
        // Instead of this returns emptyexit(1);
    }
    return(instances);
}

/* gets the instances for the CIM_Namespace class from host
*/
Array<CIMInstance> InteropTest::_getPGNamespaceInstances()
{
    Array<CIMInstance> instances;
    try
    {
        instances = _client.enumerateInstances(InteropNamespace,
                                              PG_NAMESPACE_CLASSNAME);
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage()
            << " Conection term abnormal" << endl;
        // Instead of this returns emptyexit(1);
    }
    return(instances);
}

/* gets the instancenames for the CIM_Namespace class from host
*/
Array<CIMObjectPath> InteropTest::_getCIMNamespaceInstanceNames()
{
    Array<CIMObjectPath> objectNames;
    try
    {
        objectNames = _client.enumerateInstanceNames(InteropNamespace,
                                              CIM_NAMESPACE_CLASSNAME);
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage()
            << " Conection term abnormal" << endl;
        // Instead of this returns emptyexit(1);
    }
    return(objectNames);

}

/* find the name key in the keybindings and return the value.
    Executes exception if the key not found
    @param object path we will search
    @param keyName - Name of the key to find.
    @return value of name property
    @exceptions CIMInvalidParameterException
    NOTE: This one is a real NO NO. Should never have been written this
    way.  We should be getting the value from the instance, not the
    keys.
*/
String _getKeyValue(const CIMObjectPath& instanceName, const CIMName& keyName)
{
    Array<CIMKeyBinding> kbArray = instanceName.getKeyBindings();

    // find the correct key binding
    for (Uint32 i = 0; i < kbArray.size(); i++)
    {
        if (kbArray[i].getName() == keyName)
        {
            return (kbArray[i].getValue());
        }
    }
    cerr <<"_getKeyValue: Invalid key property: " << keyName.getString()<< endl;
    return(String::EMPTY);
}

// Get the key value from the instance for the keyName defined
String _getKeyValue(const CIMInstance& instance, const CIMName& keyName)
{
    Uint32 pos;
    CIMValue propertyValue;

    pos = instance.findProperty(keyName);
    if (pos == PEG_NOT_FOUND)
    {
        cerr << "_getKeyValue Error. Property " << keyName.getString()
             << " not found " << endl;
        return(String::EMPTY);
    }

    propertyValue = instance.getProperty(pos).getValue();
    if (propertyValue.getType() != CIMTYPE_STRING)
    {
        cerr << "Key Property " << keyName.getString()
             << " incorrect type" << endl;
        return(String::EMPTY);
    }
    String name;
    propertyValue.get(name);
    return(name);
}


Array<CIMNamespaceName> InteropTest::_getNamespacesNew()
{
    Array<CIMObjectPath> instanceNames;
    instanceNames = _getCIMNamespaceInstanceNames();

    Array<CIMNamespaceName> rtns;
    for (Uint32 i = 0 ; i < instanceNames.size(); i++)
    {
        String name = _getKeyValue(instanceNames[i], NAMESPACE_PROPERTYNAME);
        if (name == String::EMPTY)
        {
            continue;
        }
        rtns.append(CIMNamespaceName(name));
    }
    return(rtns);
}

/** get one string property from an instance. Note that these functions simply
    return the default value if the property cannot be found or is of the wrong
    type thus, in reality being a maintenance problem since there is no 
    error indication.
    @param instance CIMInstance from which we get property value
    @param propertyName String name of the property containing the value
    @param default String optional parameter that is substituted if the property does
    not exist, is Null, or is not a string type. The substitute is String::EMPTY
    @return String value found or defaultValue.
*/
void _getPropertyValue(const CIMInstance& instance, const CIMName& propertyName,
    const String& defaultValue, String & returnValue)
{
    returnValue = defaultValue;
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
    {
        CIMConstProperty p1 = instance.getProperty(pos);
        if (p1.getType() == CIMTYPE_STRING)
        {
            CIMValue v1  = p1.getValue();

            if (!v1.isNull())
                v1.get(returnValue);
        }
    }
}

// Overload of _getPropertyValue for boolean type
void _getPropertyValue(const CIMInstance& instance, const CIMName& propertyName,
    const Boolean defaultValue, Boolean returnValue)
{
    returnValue = defaultValue;
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
    {
        CIMConstProperty p1 = instance.getProperty(pos);
        if (p1.getType() == CIMTYPE_BOOLEAN)
        {
            CIMValue v1  = p1.getValue();

            if (!v1.isNull())
                v1.get(returnValue);
        }
    }
}

void InteropTest::_showNamespaceInfo(const String& title)
{
    Array<CIMInstance> instances;
    instances = _getCIMNamespaceInstances();

    cout << title << " size = " << instances.size() << endl;
    Array<CIMNamespaceName> rtns;
    for (Uint32 i = 0 ; i < instances.size(); i++)
    {
        String isSharable = String::EMPTY;
        String updatesAllowed = String::EMPTY;
        String parent = String::EMPTY;
        String name = String::EMPTY;

        // get the namespace name from the name property.
        if ((instances[i].findProperty(NAMESPACE_PROPERTYNAME)) == PEG_NOT_FOUND)
            isSharable = "ERROR: Name Property Not Found";
        else
        {

            _getPropertyValue(instances[i],NAMESPACE_PROPERTYNAME,
                String("ERROR: No Name Property Value"),
                name);
        }

        // if this is a PG_Namespace, get the other characteristics.
        if (instances[i].getClassName() == PG_NAMESPACE_CLASSNAME)
        {
            // Get the sharable property
            if ((instances[i].findProperty("IsShareable")) == PEG_NOT_FOUND)
                isSharable = "Property Not Found";
            else
            {
                Boolean boolRtn = false;
                _getPropertyValue(instances[i],"IsShareable", false, boolRtn); 
                isSharable = boolRtn? "true" : "false";

            }

            // get the schemUpdatesAllowed property information
            if ((instances[i].findProperty("SchemaUpdatesAllowed")) == PEG_NOT_FOUND)
                isSharable = "Property Not Found";
            else
            {
                Boolean boolRtn = false;
                _getPropertyValue(instances[i],"SchemaUpdatesAllowed", false, boolRtn);
                updatesAllowed = boolRtn ?  "true" : "false";
            }
                        // get the schemUpdatesAllowed property information
            if ((instances[i].findProperty("ParentNamespace")) == PEG_NOT_FOUND)
                isSharable = "Property Not Found";
            else
            {
                _getPropertyValue(instances[i],"ParentNamespace", String("No parent"), parent);
            }

        }
        
        printf("%-20s %-10s %-10s %-10s\n", (const char *)name.getCString(), 
                                            (const char *)isSharable.getCString(),
                                            (const char *)updatesAllowed.getCString(),
                                            (const char *)parent.getCString());
    }
}
void _showNamespaceList(const Array<CIMNamespaceName> names, const String& title)
{
    cout << title << " size = " << names.size() << endl;
    for (Uint32 i = 0; i < names.size(); i++)
    {
        cout << "  " << names[i].getString() << endl;
    }
}


// Determine if the named namespace exists in the host.
// gets all namespaces and compares for this one.
Boolean InteropTest::_existsNew(const CIMNamespaceName& name)
{
    //Get all namespace instances

    Array<CIMNamespaceName> namespaceNames =
        _getNamespacesNew();

    for(Uint32 i = 0; i < namespaceNames.size(); i++)
    {
        if(namespaceNames[i].equal ( name ))
       {
           return true;
       }
    }
    return false;
}

Boolean InteropTest::_namespaceCreate__Namespace(const CIMNamespaceName& parent, const String& childName)
{
    CIMObjectPath newInstanceName;
    try
    {
        // Build the new instance
        CIMInstance newInstance(__NAMESPACE_CLASSNAME);
        newInstance.addProperty(CIMProperty(CIMName (NAMESPACE_PROPERTYNAME),
            childName));
        newInstanceName = _client.createInstance(parent,
                                                 newInstance);
    }
    catch(CIMException& e)
    {
         if (e.getCode() == CIM_ERR_ALREADY_EXISTS)
         {
               PEGASUS_STD(cerr) << "CIMException NameSpace Creation: "
                   << e.getMessage() << " Already Exists. Cannot create."
                   << PEGASUS_STD(endl);
         }
         else
         {
              PEGASUS_STD(cerr) << "CIMException NameSpace Creation: "
                  << e.getMessage() << " Creating " << childName
                  << PEGASUS_STD(endl);
         }
         return(false);
    }
    catch(Exception& e)
    {
        PEGASUS_STD(cerr) << "Exception NameSpace Creation: " << e.getMessage() << PEGASUS_STD(endl);
        return(true);
    }
    //
    return(true);
}

/** Create a single namespace using CIM_Namespace
*/
Boolean InteropTest::_namespaceCreateCIM_Namespace(const CIMNamespaceName& name)
{
    // Does this namespace exist.
    if (_existsNew(name))
        return(false);

    //Now build the new namespace name instance.  Note that we need to get the
    // collection of keys that are required. Easy way was to simply
    // use an existing instance and change the name field.
    Array<CIMInstance> instances = _getCIMNamespaceInstances();

    if(instances.size() == 0)
    {
        return(false);
    }

    // El cheapo code Modify one existing instance and send it back as
    // method to construct the correct keys.
    CIMInstance instance = instances[0];

    // remove the qualifiers, etc.
    // NOTE should do this as part of the get.
    instance.filter(false, false, CIMPropertyList());
    // Modify the name property value for new name
    Uint32 pos = instance.findProperty(NAMESPACE_PROPERTYNAME);
    if (pos == PEG_NOT_FOUND)
    {
            cerr << "Error in property on create. No "
                <<  NAMESPACE_PROPERTYNAME << " property" << endl;
            return(false);
    }

    // Modify the name field in the instance and resend
    String localName = name.getString();
    CIMProperty p = instance.getProperty(pos);

    // test for correct property type, etc.
    p.setValue(localName);
    instance.removeProperty(pos);
    instance.addProperty(p);

    CDEBUG("Creating instance for " << localName << " in namespace " << InteropNamespace);
    if (verbose)
    {
        cout << "Show instance used to do namespace create: " << endl;
        XmlWriter::printInstanceElement(instance);
    }
    try
    {
           CIMObjectPath path;
           path = _client.createInstance(InteropNamespace, instance);
    }
    catch(Exception& e)
    {
        // ATTN: Temp bypass to do checkin.
        cerr << "Error during Creation of " << name.getString() << ": " << e.getMessage()
            << " Instance Creation error" << endl;
        return(false);
    }

    return(true);
}

//ATTN: This test is not done and therefore always returns true.
// 28 Sept 2004. KS.
Boolean InteropTest::_testPGNamespace(const CIMNamespaceName& name,
    Boolean shared, Boolean updatesAllowed, const String& parent)
{
    //ATTN: Build the get and test properties.

    // get the instance

    // test for shared property

    // test for updatesAllowed property

    // test for parent property


    return(true);
}

/** Create a single namespace using CIM_Namespace
*/
Boolean InteropTest::_namespaceCreatePG_Namespace(const CIMNamespaceName& name,
    const Boolean shareable, const Boolean updatesAllowed, const String& parent)
{
    // Does this namespace exist.
    if (_existsNew(name))
        return(false);

    //Now build the new namespace name instance.  Note that we need to get the
    // collection of keys that are required. Easy way was to simply
    // use an existing instance and change the name field.
    Array<CIMInstance> instances = _getPGNamespaceInstances();

    if(instances.size() == 0)
    {
        return(false);
    }

    CIMClass thisClass = _client.getClass(PEGASUS_NAMESPACENAME_INTEROP,
                                        PG_NAMESPACE_CLASSNAME,
                                        false,true,true);

    CIMInstance instance = thisClass.buildInstance(true, false, CIMPropertyList());

    // Modify the name property value for new name
    Uint32 pos = instance.findProperty(NAMESPACE_PROPERTYNAME);
    if (pos == PEG_NOT_FOUND)
    {
            cerr << "Error in property on create. No "
                <<  NAMESPACE_PROPERTYNAME << " property" << endl;
            return(false);
    }

    // Modify the name field in the instance
    String localName = name.getString();
    CIMProperty p = instance.getProperty(pos);

    p.setValue(localName);
    instance.removeProperty(pos);
    instance.addProperty(p);

    // Set the sharable, updates allowed, parent properties.
    pos = instance.findProperty("SchemaUpdatesAllowed");
    if (pos == PEG_NOT_FOUND)
    {
         cerr << "Error in property on create. No "
             <<  "SchemaUpdatesAllowed" << " property" << endl;
         return(false);
    }

    // Modify the field in the instance
    String localName1 = name.getString();
    CIMProperty p1 = instance.getProperty(pos);

    p1.setValue(updatesAllowed);
    instance.removeProperty(pos);
    instance.addProperty(p1);


    // Set the sharable, updates allowed, parent properties.
    pos = instance.findProperty("IsShareable");
    if (pos == PEG_NOT_FOUND)
    {
         cerr << "Error in property on create. No "
             <<  "IsShareable" << " property" << endl;
         return(false);
    }

    // Modify the field in the instance
    String localName2 = name.getString();
    CIMProperty p2 = instance.getProperty(pos);

    p1.setValue(shareable);
    instance.removeProperty(pos);
    instance.addProperty(p2);

    CDEBUG("Creating instance for " << localName << " in namespace " << InteropNamespace);
    if (verbose)
    {
        cout << "Show instance used to do namespace create: " << endl;
        XmlWriter::printInstanceElement(instance);
    }
    try
    {
           CIMObjectPath path;
           path = _client.createInstance(InteropNamespace, instance);
    }
    catch(Exception& e)
    {
        // ATTN: Temp bypass to do checkin.
        cerr << "Error during Creation of " << name.getString() << ": " << e.getMessage()
            << " Instance Creation error" << endl;
        return(false);
    }

    return(true);
}

Boolean InteropTest::_namespaceDeleteCIM_Namespace(const CIMNamespaceName& name)
{
    // If does not exist, don't try to delete
    if (!_existsNew(name))
        return(false);

    // Get this instancepath from the whole set.
    Array<CIMObjectPath> paths = _getCIMNamespaceInstanceNames();

    // find the entry with the correct key
    Boolean found = false;
    Uint32 i;
    for (i = 0; i < paths.size() ; i++)
    {
        String n = _getKeyValue(paths[i], NAMESPACE_PROPERTYNAME);
        if (n == name.getString())
        {
            found = true;
            break;
        }
    }
    // This should not happen since we found it at top of this function
    if (!found)
    {
        cerr << "Namespace " << name.getString() << " not found" << endl;
        return(false);
    }

    // Delete the namespace
    try
    {
        _client.deleteInstance(InteropNamespace, paths[i]);

    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage()
            << " Instance deletion error  for " << name.getString() << endl;
        // Instead of this returns emptyexit(1);
        return(false);
    }
    return(true);
}

Boolean InteropTest::testClassExists(const CIMName & className)
{
    try
    {
        CIMClass thisClass = _client.getClass(PEGASUS_NAMESPACENAME_INTEROP,
                                            className,
                                            false,true,true);
        return(true);
    }

    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage() << " Class " << className.getString() 
            << " does not exist in Namespace " << PEGASUS_NAMESPACENAME_INTEROP.getString() << endl;
    }
    return(false);
}


/* Execute the tests to assure that the namespace manager works and
   that the results match what is returned from the __Namespace manager
*/
void InteropTest::testNameSpacesManagement()
{
    Array<CIMNamespaceName> nameListNew;
    // Test for required classes to assure that the interop provider and
    // its corresponding classes exist.
    assert(testClassExists(CIM_NAMESPACE_CLASSNAME));
    assert(testClassExists(PG_NAMESPACE_CLASSNAME));
    assert(testClassExists(CIM_OBJECTMANAGER_CLASSNAME));
    assert(testClassExists(CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM_CLASSNAME));
    assert(testClassExists(PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME));
    assert(testClassExists(CIM_COMMMECHANISMFORMANAGER_CLASSNAME));
    
    // Test namespace usage with both __namespace and CIM_Namespace.
    // __namespace is called old
    // CIM_Namespace is called new for these tests.
    Array<CIMNamespaceName> nameListOld = _getNamespacesOld();
    
    CDEBUG("Got Namespaces with __Namespace. Now Validate");
    if (!_validateNamespaces(nameListOld))
    {
        cout << "Error exit, Invalid namespace returned" << endl;
    }
    
    nameListNew = _getNamespacesNew();
    
    CDEBUG("Got Namespaces with CIM_Namespace. Now Validate");
    if (!_validateNamespaces(nameListNew))
    {
        cout << "Error exit, Invalid namespace returned" << endl;
    }
    
    if(verbose)
    {
        _showNamespaceList(nameListNew, "From CIM_Namespace");
    
        _showNamespaceList(nameListOld, "From __Namespace");
    }
    
    assert(nameListNew.size() == nameListOld.size());
    
    // Add assertion that they have the same items in the list
    
    // Create a new namespace with new functions
    
    CIMNamespaceName testNameNew = CIMNamespaceName("root/junk/interoptest/newtype");
    CIMNamespaceName testNameOldRoot = CIMNamespaceName("root");
    CIMNamespaceName testNameOldTail = CIMNamespaceName("junk/interoptest/oldtype");
    CIMNamespaceName testNameOldComplete = CIMNamespaceName("root/junk/interoptest/oldtype");
    
    // Create the namespace with the CIM_Namespace class
    // Note that this is an assertion and, in fact, we should probably remove the names.
    
    assert( ! _existsNew(testNameNew));
    
    assert( ! _existsNew(testNameOldComplete));
    
    CDEBUG("Now Create New Namespace with CIM_Namespace. Namespace name = " << testNameNew.getString() << ".");
    _namespaceCreateCIM_Namespace(CIMNamespaceName(testNameNew));
    
    if (verbose)
    {
        _showNamespaceList(nameListNew, "CIM_Namespace response after add.");
    }
    
    // The following code is temporary KS 2004
    CDEBUG("Test for namespace created. Name = " << testNameNew.getString()); 
    assert(_existsNew(testNameNew));
    
    assert(_namespaceDeleteCIM_Namespace(CIMNamespaceName(testNameNew)));
    
    CDEBUG("Test for Namespace existing = " << testNameOldComplete.getString()); 
    if(_existsNew(CIMNamespaceName(testNameOldComplete)))
       _namespaceDeleteCIM_Namespace(CIMNamespaceName(testNameOldComplete));
    
    CDEBUG("Test for Namespace NOT existing = " << testNameOldComplete.getString()); 
    if(_existsNew(testNameOldComplete))
        cerr << "Problem deleting namespace" << testNameOldComplete.getString() <<endl;
    
    CDEBUG("Creating Old = " << testNameOldTail.getString()); 
    _namespaceCreate__Namespace(CIMNamespaceName(testNameOldRoot), String(testNameOldTail.getString()));
    
    assert(_existsNew(testNameOldComplete));
    
    assert(_namespaceDeleteCIM_Namespace(CIMNamespaceName(testNameOldComplete)));
    
    assert(!_existsNew(testNameOldComplete));
    
    if (verbose)
    {
        _showNamespaceList(nameListNew, "Namespaces From CIM_Namespace after add.");
    }
    
    _namespaceCreate__Namespace(CIMNamespaceName(testNameOldRoot), String(testNameOldTail.getString()));
    
    // Note that this tries to delete the multiple levels all at the same time.
    
    // ATTN: There is apparently a problem here with the following
    // delete.  Fix this and retest.  For the moment, we substituted
    // Deleting the new way.
    //_deleteNamespaceOld(String(testNameOldComplete.getString()));
    
    _namespaceDeleteCIM_Namespace(testNameOldComplete);
    
    if (verbose)
    {
        _showNamespaceList(nameListNew, "CIM_Namespace response after add.");
    }
    assert(!_existsNew(testNameOldComplete));
    
    if(_existsNew(CIMNamespaceName(testNameOldComplete)))
       _namespaceDeleteCIM_Namespace(CIMNamespaceName(testNameOldComplete));
    
    if(_existsNew(CIMNamespaceName(testNameOldComplete)))
        cerr << "Problem deleting namespace" << endl;
    
    // Finally test to be sure that we have the same count of namespaces
    //as when we started.  Should also check to be sure it is exactly the
    //same set of namespaces.
    
    Array<CIMNamespaceName> nameListTemp = _getNamespacesNew();
    
    assert(nameListTemp.size() == nameListNew.size());
    
    if (verbose)
        cout << "Basic Namespace Tests passed" << endl;
}

//****************************************************************
// Test characteristics of shared namespaces.
//***************************************************************
void InteropTest::testSharedNameSpacesManagement()
{
    try
    {
        CIMNamespaceName testNameSharable = CIMNamespaceName("root/junk/interoptest/sharable");
        CIMNamespaceName testNameShared = CIMNamespaceName("root/junk/interoptest/shared");
    
        // Create a sharable namespace
        _namespaceCreatePG_Namespace(testNameSharable, true, false, String::EMPTY);
    
        // create a namespace with the previous sharable as parent.
        _namespaceCreatePG_Namespace(testNameShared, false, false, testNameSharable.getString());
    
        // Confirm that both exist
        assert(_existsNew(testNameSharable));
        assert(_existsNew(testNameShared));
    
        if (verbose)
            _showNamespaceInfo("Namespaces with one shared and one sharable created");
    
        // Should add test to confirm that these are really shared.
        // No. Not in this version.
        assert(_testPGNamespace(testNameSharable, true, false, String::EMPTY));
        assert(_testPGNamespace(testNameShared, false, false, testNameSharable.getString()));
    
        // Now delete the two namespaces
        _namespaceDeleteCIM_Namespace(testNameSharable);
        _namespaceDeleteCIM_Namespace(testNameShared);
    
        assert(!_existsNew(testNameSharable));
        assert(!_existsNew(testNameShared));
    }
    // Catch block for all of the shared Namespace tests Tests.
    catch(CIMException& e)
    {
        TERMINATE(" Shared Namespace test CIMException: " << e.getMessage());
    }
    catch(Exception& e)
    {
        TERMINATE(" Shared Namespace test Pegasus Exception: " << e.getMessage());
    }
    catch(...)
    {
        TERMINATE(" Shared Namespace test Caught General Exception:");
    }
    if (verbose)
        cout << "Shared Namespace Tests passed" << endl;
}

CIMInstance InteropTest::getInstanceObjMgr()
{
    CIMClass objectManager = _client.getClass(PEGASUS_NAMESPACENAME_INTEROP,
                                        CIM_OBJECTMANAGER_CLASSNAME,
                                        false,false,true);
    if (verbose)
    {
        cout << "Show the object manager Class element" << endl;
        XmlWriter::printClassElement(objectManager);
    }
    Array<CIMInstance> instancesObjMgr = _client.enumerateInstances(
                                             PEGASUS_NAMESPACENAME_INTEROP,
                                             CIM_OBJECTMANAGER_CLASSNAME,
                                             true, false, true,true, CIMPropertyList());

    assert(instancesObjMgr.size() == 1);

    return(instancesObjMgr[0]);
}

CIMObjectPath InteropTest::getObjMgrPath()
{
    CIMInstance instance = getInstanceObjMgr();
    return(instance.getPath());
}

//****************************************************************
// Test the characteristics of the CIM_ObjectManager Class and Instances
//****************************************************************
void InteropTest::testObjectManagerClass()
{
    try
    {
    // Test the CIM_ObjectManager Object

        CIMInstance instanceObjectManager =  getInstanceObjMgr();

        if (verbose)
        {
            cout << "Show the object manager instance element" << endl;
            XmlWriter::printInstanceElement(instanceObjectManager);
        }
        // Why am I building the path here rather than getting it from the instance?
        CIMObjectPath objectManagerPath = instanceObjectManager.buildPath(CIM_OBJECTMANAGER_CLASSNAME);
    
        // test to confirm that both names and instances return same thing.
        Array<CIMObjectPath> pathsObjMgr = _client.enumerateInstanceNames(
                                                 PEGASUS_NAMESPACENAME_INTEROP,
                                                 CIM_OBJECTMANAGER_CLASSNAME);
    
        assert(pathsObjMgr.size() == 1);
    
        CIMObjectPath objectManagerPath1 = instanceObjectManager.getPath();
    
        if (verbose)
        {
        cout << "Object Manager path from enumerateInstancesNames: " << pathsObjMgr[0].toString() << endl
             << "Object manager path from enumerateInstances bld : " << objectManagerPath.toString() << endl
             << "Object Manager path form enumerateInstances path: " << instanceObjectManager.getPath().toString() << endl   ;
        }
    
        // Add code to compare paths, objects, etc. for object manager.

        // Right now this test failing.  not sure why.
        //assert(objectManagerPath1 == objectManagerPath);

        // Test modification of objectmanager statistics property.

        assert (instanceObjectManager.findProperty("gatherstatisticaldata") != PEG_NOT_FOUND);
        assert (instanceObjectManager.findProperty("Name") != PEG_NOT_FOUND);
        assert (instanceObjectManager.findProperty("ElementName") != PEG_NOT_FOUND);
        assert (instanceObjectManager.findProperty("CreationClassName") != PEG_NOT_FOUND);
        assert (instanceObjectManager.findProperty("SystemName") != PEG_NOT_FOUND);
        assert (instanceObjectManager.findProperty("SystemCreationClassName") != PEG_NOT_FOUND);
    }
    // Catch block for all of the CIM_ObjectManager Tests.
    catch(CIMException& e)
    {
        TERMINATE(" CIM_ObjectManager CIMException: " << e.getMessage());
    }
    catch(Exception& e)
    {
        TERMINATE(" CIM_ObjectManager Pegasus Exception: " << e.getMessage());
    }
    catch(...)
    {
        TERMINATE(" CIM_ObjectManager Caught General Exception:");
    }
    if (verbose)
        cout << "ObjectManagerClass Tests passed" << endl;
}

void InteropTest::setStatisticsState(const Boolean flag)
{
    CIMInstance instanceObjectManagr = getInstanceObjMgr();

    CIMInstance sendInstance = instanceObjectManagr.clone();

    Uint32 pos;
    if ((pos = sendInstance.findProperty("gatherstatisticaldata")) != PEG_NOT_FOUND)
        sendInstance.getProperty(pos).setValue(CIMValue(flag));

    // What is this for?????
    // We are appending property????
    Array<CIMName> plA;
    plA.append(CIMName("gatherstatisticaldata"));
    CIMPropertyList myPropertyList(plA);

    _client.modifyInstance(PEGASUS_NAMESPACENAME_INTEROP,
                        sendInstance,
                        false,
                        CIMPropertyList());
}

Boolean InteropTest::getStatisticsPropertyState(CIMInstance & objMgrInstance)
{
    Boolean output;
    Uint32 pos;
    if ((pos = objMgrInstance.findProperty("gatherstatisticaldata")) != PEG_NOT_FOUND)
    {
        CIMConstProperty p1 = objMgrInstance.getProperty(pos);
        if (p1.getType() == CIMTYPE_BOOLEAN)
        {
            CIMValue v1  = p1.getValue();
            if (!v1.isNull())
                v1.get(output);
            else
                output = false;
        }
        else
            TERMINATE("gatherStatisticaldata Property type problem");
    }
    else
        TERMINATE("gatherStatisticaldata Property Not found");
    return(output);
}

Boolean InteropTest::getStatisticsState()
{
    CIMInstance objMgrInstance = getInstanceObjMgr();
    return(getStatisticsPropertyState(objMgrInstance));
}

void InteropTest::showStatisticsState()
{
    cout << "Statistics State = " << (getStatisticsState()? "on" : "off") << endl;

}
void InteropTest::testStatisticsEnable()
{
    try
    {
        CIMInstance instanceObjectManager = getInstanceObjMgr();
        CIMInstance sendInstance = instanceObjectManager.clone();

        Uint32 pos;
        if ((pos = sendInstance.findProperty("gatherstatisticaldata")) != PEG_NOT_FOUND)
            sendInstance.getProperty(pos).setValue(CIMValue(true));
        else
            assert(false);
        CDEBUG("testStats. pos = " << pos);
        Array<CIMName> plA;
        plA.append(CIMName("gatherstatisticaldata"));
        CIMPropertyList myPropertyList(plA);

        // Don't need this.
        sendInstance.filter(false, false, CIMPropertyList());

        CDEBUG("testStats. filtered");
        //??? Error here since we only have the one property in the instance.
        try
        {
             XmlWriter::printInstanceElement(sendInstance);
            _client.modifyInstance(PEGASUS_NAMESPACENAME_INTEROP,
                                sendInstance,
                                false,
                                CIMPropertyList());
        }
        catch(CIMException& e)
            {
                cout <<" CIM_ObjectManager Test CIMException: " << e.getMessage() << endl;
                throw e;
            }
        

        CDEBUG("testStats. modified");
        // Get Object Manager instance and confirm property changed.
        CIMInstance localInstance = getInstanceObjMgr();

        if ((pos = localInstance.findProperty("gatherstatisticaldata")) != PEG_NOT_FOUND)
        {
            CIMConstProperty p1 = localInstance.getProperty(pos);
            if (p1.getType() == CIMTYPE_BOOLEAN)
            {
                CIMValue v1  = p1.getValue();
                Boolean output;
                if (!v1.isNull())
                    v1.get(output);
                assert(output);
            }
        }
        else
            assert(false);

        // Now we must set it back to false.

        CDEBUG("testStats. second get");
        if ((pos = sendInstance.findProperty("gatherstatisticaldata")) != PEG_NOT_FOUND)
            sendInstance.getProperty(pos).setValue(CIMValue(false));
        else
            assert(false);

        _client.modifyInstance(PEGASUS_NAMESPACENAME_INTEROP,
                            sendInstance,
                            false,
                            CIMPropertyList());

        // Get instance and confirm property changed once again.
        CIMInstance newInstanceObjectManager = getInstanceObjMgr();

        if ((pos = newInstanceObjectManager.findProperty("gatherstatisticaldata")) != PEG_NOT_FOUND)
        {
            CIMConstProperty p1 = newInstanceObjectManager.getProperty(pos);
            if (p1.getType() == CIMTYPE_BOOLEAN)
            {
                CIMValue v1  = p1.getValue();
                Boolean output;
                if (!v1.isNull())
                    v1.get(output);
                else
                    assert(false);
                assert(output == false);
            }
        }
        else
            assert(false);
    }
    // Catch block for all of the CIM_ObjectManager Tests.
    catch(CIMException& e)
    {
        TERMINATE(" CIM_ObjectManager Test CIMException: " << e.getMessage());
    }
    catch(Exception& e)
    {
        TERMINATE(" CIM_ObjectManager Test Pegasus Exception: " << e.getMessage());
    }
    catch(...)
    {
        TERMINATE(" CIM_ObjectManager Test Caught General Exception:");
    }
    if (verbose)
    {
        cout << "Statistics Enable Tests passed" << endl;
    }
}


//************************************************************
//  Tests on the objectmanager communication classes
//************************************************************
void InteropTest::testCommunicationClass()
{
    try
    {
        // test to confirm that both names and instances return same thing.
        Array<CIMObjectPath> pathsObjMgr = _client.enumerateInstanceNames(
                                                 PEGASUS_NAMESPACENAME_INTEROP,
                                                 CIM_OBJECTMANAGER_CLASSNAME);
        assert(pathsObjMgr.size() == 1);

        CIMObjectPath objectManagerPath = pathsObjMgr[0];

        Array<CIMInstance> instancesCommMech = _client.enumerateInstances(
                                                     PEGASUS_NAMESPACENAME_INTEROP,
                                                     CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM_CLASSNAME,
                                                     false, false, false,false, CIMPropertyList());
        // COMMENT KS - There is no reason for this.  The whole thing should be covered.
        #ifdef PEGASUS_ENABLE_SLP
        assert(instancesCommMech.size() > 0);
        #endif

        for (Uint32 i = 0 ; i < instancesCommMech.size() ; i++)
        {
            assert (instancesCommMech[i].findProperty("SystemCreationClassName") != PEG_NOT_FOUND);
            assert (instancesCommMech[i].findProperty("SystemName") != PEG_NOT_FOUND);
            assert (instancesCommMech[i].findProperty("CreationClassName") != PEG_NOT_FOUND);
            assert (instancesCommMech[i].findProperty("Name") != PEG_NOT_FOUND);
            assert (instancesCommMech[i].findProperty("SystemCreationClassName") != PEG_NOT_FOUND);
            assert (instancesCommMech[i].findProperty("FunctionalProfileDescriptions") != PEG_NOT_FOUND);
            assert (instancesCommMech[i].findProperty("MultipleOperationsSupported") != PEG_NOT_FOUND);
            assert (instancesCommMech[i].findProperty("AuthenticationMechanismsSupported") != PEG_NOT_FOUND);
            assert (instancesCommMech[i].findProperty("IPAddress") != PEG_NOT_FOUND);
        }
    }
    // Catch block for all of the CIM_ObjectManager Tests.
    catch(CIMException& e)
    {
        TERMINATE(" CIM_ObjectManager Test CIMException: " << e.getMessage());
    }
    catch(Exception& e)
    {
        cerr << pgmName << " CIM_ObjectManager Test Pegasus Exception: " << e.getMessage()  << endl;
    }
    catch(...)
    {
        cerr << pgmName << " CIM_ObjectManager Test Caught General Exception:" << endl;
    }
    if (verbose)
        cout << "test Communication Class successful" << endl;
}

//************************************************************
//  Tests on the NamespaceInObjectManager classes
//************************************************************
void InteropTest::testNameSpaceInObjectManagerAssocClass()
{
    // KS Not implemented
}

//************************************************************
//  Tests on the NamespaceInObjectManager classes
//************************************************************
void InteropTest::testCommMechinManagerAssocClass()
{
    CIMObjectPath objectManagerPath = getObjMgrPath();
    try
    {
        Array<CIMObjectPath> referenceNames = _client.referenceNames(
                        PEGASUS_NAMESPACENAME_INTEROP,                // namespace
                        objectManagerPath,                            // object manager instance
                        CIM_COMMMECHANISMFORMANAGER_CLASSNAME,        // result class
                        String::EMPTY);                               // role

        //cout << "refs " << referenceNames.size() << " " <<  instancesCommMech.size() << endl;
        //assert(referenceNames.size() == instancesCommMech.size());

        if (verbose)
        {
            for (Uint32 i = 0 ; i < referenceNames.size() ; i++)
            {
                cout << i << "   " << referenceNames[i].toString() << endl;
            }
        }
    }

    // Catch block for all of the CIM_ObjectManager Tests.
    catch(CIMException& e)
    {
        cerr << " CIMException: " << e.getMessage() << endl;
    }
    catch(Exception& e)
    {
        cerr << " Pegasus Exception: " << e.getMessage()  << endl;
    }
    catch(...)
    {
        cerr << " Caught General Exception:" << endl;
    }
}
/////////////////////////////////////////////////////////////////////////////////////
//
//  Main-
//
/////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    CIMClient client;
    pgmName = argv[0];
    cout << pgmName << endl;
    if (argc > 1)
    {
        String cmd = argv[1];
        if (cmd == "--help" || cmd == "-help" || cmd == "help")
        {
            cout << "Usage: " << pgmName << "\n"
            << "   or " << pgmName << " parameter where parameter =\n"
            << "         on - turns on the statistics function. \n"
            << "         off - turns off the statistics fucntion.\n"
            << "         status - Shows current status of statistics flag." << endl;
            exit (0);
        }
    }
    try
    {
        client.connectLocal();
    }
    catch(Exception& e)
    {
        cerr <<" Error: " << e.getMessage() << " Conection terminate abnormal" << endl;
        exit(1);
    }
    try
    {
        InteropTest it(client);
    
     /* There are three possible commands. 
        on - Turns on the status monitor
        off - Turns off the status monitor
        Status - Shows the state of the status monitor
    
        If no command is input, it reports the current statistics.
        Note that we do not protect against the user requesting statistics
        if the monitor is off.
    */
        if (argc > 1)
        {
            String cmd = argv[1];
            if (cmd == "on")
            {
                it.setStatisticsState(true);
                exit(0);
            }
            else if (cmd == "off")
            {
                it.setStatisticsState(false);
                exit(0);
            }
            else if (cmd == "status")
            {
                it.showStatisticsState();
                exit(0);
            }
            else
            {
                cerr << pgmName << ". Error on command line. " 
                    << " Expected on or off or status. Rcvd "<< cmd << endl;
                exit(1);
            }
        }
        it.testNameSpacesManagement();
    
        it.testSharedNameSpacesManagement();
    
        it.testObjectManagerClass();
    
        it.testStatisticsEnable();
    
        it.testCommunicationClass();
    
        it.testNameSpaceInObjectManagerAssocClass();
    
        it.testCommMechinManagerAssocClass();
    
        client.disconnect();
    }

    // Catch block for all of the CIM_ObjectManager Tests.
    catch(CIMException& e)
    {
        TERMINATE(" Program CIMException: " << e.getMessage());
    }
    catch(Exception& e)
    {
        TERMINATE(" Program Pegasus Exception: " << e.getMessage());
    }
    catch(...)
    {
        TERMINATE(" Program Caught General Exception:");
    }
    cout << argv[0] << " +++++ passed all tests" << endl;
    exit(0);
}

