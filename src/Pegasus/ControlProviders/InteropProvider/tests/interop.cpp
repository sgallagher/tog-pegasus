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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: 
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

/* This is a tetprogram for namespace manipulation.

*/
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/* Test the namespace manipulation functions.
Creates, deletes, quieries namespaces using both the
__Namespace and CIM_Namespace functions
*/

static const CIMNamespaceName __NAMESPACE_NAMESPACE = CIMNamespaceName ("root");
static const CIMNamespaceName InteropNamespace = CIMNamespaceName("root/cimv2");

static char* programVersion =  "1.0";

// Property Names for __Namespace Class
static const CIMName NAMESPACE_PROPERTYNAME  = CIMName ("Name");
static const CIMNamespaceName ROOTNS  = CIMNamespaceName ("root");
static const CIMName __NAMESPACE_CLASSNAME  = CIMName ("__Namespace");
static const CIMName CIM_NAMESPACE_CLASSNAME  = CIMName ("CIM_Namespace");

static char * verbose;

/* Get all namespaces and return as an array
*/
Array<CIMNamespaceName> _getNamespacesOld( CIMClient& client)
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
            // Get the next increment in naming for all a name element in the array
            CIMNamespaceName next;
            if (end != 0)
            {
                next = namespaceNames[range]; 
            }
            Array<CIMInstance> instances = client.enumerateInstances(next, className);
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
            cimQualifierDecl = client.getQualifier(namespaceNames[i],
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

Boolean _deleteOneLevelOfNamespace(CIMClient& client, CIMNamespaceName& parent,String & child)
{
    
    try
    {
        CIMObjectPath myReference(String::EMPTY, parent, child);
        client.deleteInstance(parent, myReference);
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
Boolean _deleteNamespaceOld(CIMClient& client, String & name)
{
    
    Uint32 pos;
    while((pos = name.reverseFind('/')) != PEG_NOT_FOUND)    
    {
        String parent = name.subString(0, pos);
        String child = name.subString (pos + 1);
        Boolean rtn = _deleteOneLevelOfNamespace(client, CIMNamespaceName(parent), child);
        if(!rtn)
            return(false);
    }
    return(true);
}

Boolean _validateNamespaces(CIMClient& client, Array<CIMNamespaceName>& namespaceNames)
{
    // Validate that these are all namespaces.  This is not a certain
    // Test but we simply check to see if there is an association 
    // qualifier in the namespace.
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

    cout << returnNamespaces.size() << " namespaces " << " returned." << endl;
    if (verbose)
    {
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
/* Delete for now
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
Array<CIMInstance> _getCIMNamespaceInstances(CIMClient& client)
{
    Array<CIMInstance> instances;
    try
    {
        instances = client.enumerateInstances(InteropNamespace, 
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

/* gets the instancenames for the CIM_Namespace class from host
*/
Array<CIMObjectPath> _getCIMNamespaceInstanceNames(CIMClient& client)
{
    Array<CIMObjectPath> objectNames;
    try
    {
        objectNames = client.enumerateInstanceNames(InteropNamespace, 
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


Array<CIMNamespaceName> _getNamespacesNew(CIMClient& client)
{
    Array<CIMObjectPath> instanceNames;
    instanceNames = _getCIMNamespaceInstanceNames(client);

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

void _showNamespaceList(const Array<CIMNamespaceName> names, const String title)
{
    cout << title << " size = " << names.size() << endl;
    for (Uint32 i = 0; i < names.size(); i++)
    {
        cout << "  " << names[i].getString() << endl;
    }
}
// Determine if the named namespace exists in the host.
// gets all namespaces and compares for this one.
Boolean _existsNew(CIMClient& client, CIMNamespaceName& name)
{
    //Get all namespace instances
    
    Array<CIMNamespaceName> namespaceNames = 
        _getNamespacesNew(client);

    for(Uint32 i = 0; i < namespaceNames.size(); i++)
    {
        if(namespaceNames[i].equal ( name ))
       {
           return true;
       }
    }
    return false;
}
Boolean _namespaceCreateOld(CIMClient& client, CIMNamespaceName& parent, String& name)
{
    CIMObjectPath newInstanceName;
    try
    {
        // Build the new instance
        CIMInstance newInstance(__NAMESPACE_CLASSNAME);
        newInstance.addProperty(CIMProperty(CIMName (NAMESPACE_PROPERTYNAME), 
            name));
        newInstanceName = client.createInstance(parent,
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
                  << e.getMessage() << " Creating " << name
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

Boolean _namespaceCreateNew(CIMClient& client, CIMNamespaceName& name)
{
    // First get the existing namespaces.
    if (_existsNew(client, name))
        return(false);

    Array<CIMInstance> instances = _getCIMNamespaceInstances(client);
    // ask if empty here
    // El cheapo. Modify one instance and send it back to get
    // the correct keys.
    CIMInstance instance = instances[0];
    // Modify the name property value for new name
    Uint32 pos = instance.findProperty(NAMESPACE_PROPERTYNAME);
    if (pos == PEG_NOT_FOUND)
    {
            cerr << "Error in property on create. No " 
                <<  NAMESPACE_PROPERTYNAME << " property" << endl;
            return(false);
    }
    // Modify the name field in the instance and resent
    String localName = name.getString();
    CIMProperty p = instance.getProperty(pos);
    // ATTN XmlWriter::printInstanceElement(instance);
    // test for correct property type, etc.
    p.setValue(localName);
    instance.removeProperty(pos);
    instance.addProperty(p);
    
    // ATTN XmlWriter::printInstanceElement(instance);
    try
    {
           CIMObjectPath path;
           path = client.createInstance(InteropNamespace, instance);
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage() 
            << " Instance Creation error" << endl;
        // Instead of this returns emptyexit(1);
        return(false);
    }

    return(true);
}
Boolean _namespaceDeleteNew(CIMClient& client, CIMNamespaceName& name)
{
    // If does not exist, don't try to delete
    if (!_existsNew(client, name))
        return(false);

    // Get this instancepath from the whole set.
    Array<CIMObjectPath> paths = _getCIMNamespaceInstanceNames(client);

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
        client.deleteInstance(InteropNamespace, paths[i]);

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

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    // ATTN:cout << argv[0] << endl;

    CIMClient client;
    String host = "localhost";
    Uint32 portNumber = 5988;
    String userName = String::EMPTY;
    String password = String::EMPTY;
    
    try
    {
        client.connect (host, portNumber,
                        userName, password);
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage() << " Conection term abnormal" << endl;
        exit(1);
    }

    Array<CIMNamespaceName> nameListOld = _getNamespacesOld(client);


    if (!_validateNamespaces(client, nameListOld))
    {
        cout << "Error exit, Invalid namespace returned" << endl;
    }
    
    Array<CIMNamespaceName> nameListNew = _getNamespacesNew(client);

    if (!_validateNamespaces(client, nameListNew))
    {
        cout << "Error exit, Invalid namespace returned" << endl;
    }

    if(verbose)
    {
        _showNamespaceList(nameListNew, "From CIM_Namespace");
    
        _showNamespaceList(nameListOld, "From __Namespace");
    }

    //assert(nameListNew.size() == nameListOld.size());

    // Add assertion that they have the same items in the list

    // Create a new namespace with new functions

    CIMNamespaceName testName = CIMNamespaceName("root/karl/junk/newtype");
    _namespaceCreateNew(client, CIMNamespaceName(testName));

    _showNamespaceList(nameListNew, "From CIM_Namespace after add.");

    assert(_existsNew(client, testName));

    assert(_namespaceDeleteNew(client, CIMNamespaceName(testName)));

    assert(!_existsNew(client, testName));

    

    if(_existsNew(client, CIMNamespaceName("root/karl/junk/oldtype")))
       _namespaceDeleteNew(client, CIMNamespaceName("root/karl/junk/oldtype"));

    if(_existsNew(client, CIMNamespaceName("root/karl/junk/oldtype")))
        cerr << "Problem deleting namespace" << endl;

    _namespaceCreateOld(client, CIMNamespaceName("root"), String("karl/junk/oldtype"));

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}

