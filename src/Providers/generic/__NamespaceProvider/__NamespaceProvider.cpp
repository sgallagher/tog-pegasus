//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: __NamespaceProvider.cpp,v $
// Revision 1.1  2001/02/24 16:59:12  karl
// move namespaceprovider to generic directory
//
// Revision 1.8  2001/02/21 01:53:08  karl
// Namespaces stuff
//
// Revision 1.7  2001/02/19 01:53:16  mike
// Missing "()" in call to getSize().
//
// Revision 1.6  2001/02/18 23:08:53  karl
// Version with bug causing Reference conversion exception
//
// Revision 1.5  2001/02/18 19:02:18  mike
// Fixed CIM debacle
//
// Revision 1.4  2001/02/18 02:49:01  mike
// Removed ugly workarounds for MSVC++ 5.0 (using SP3 now)
//
// Revision 1.3  2001/02/16 18:03:32  mike
// fixed compiler errors
//
// Revision 1.2  2001/02/16 13:23:45  karl
// Add providers Directory
//
// Revision 1.1  2001/02/15 21:22:58  karl
// __Namespace CIMProvider import
//
//
//END_HISTORY
/* __Namespace CIMProvider

*/

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Repository/CIMRepository.h>

using namespace std;

PEGASUS_NAMESPACE_BEGIN

static CIMRepository* _repository;

const char INSTANCENAME[] = "__Namespace.name=";

class __NamespaceProvider : public CIMProvider
{
public:

    __NamespaceProvider()
    {
	// cout << "__NamespaceProvider::__NamespaceProvider()" << endl;
    }

    virtual ~__NamespaceProvider()
    {
       // cout << "__NamespaceProvider::~__NamespaceProvider()" << endl;
    }

    // Returns instance based on instanceName.  Since there is only
    // name in the class, returns only that or error if does not exist
    virtual CIMInstance getInstance(
       const String& nameSpace,
       const CIMReference& instanceName,
       Boolean localOnly = true,
       Boolean includeQualifiers = false,
       Boolean includeClassOrigin = false,
       const Array<String>& propertyList = StringArray())
   {
       cout << "__NamespaceProvider::getInstance() called" << endl;

       String tmp;
       CIMReference::referenceToInstanceName(instanceName, tmp);
       cout << "instanceName=" << tmp << endl;

       CIMInstance instance("__Namespace");
       instance.addProperty(CIMProperty("name", tmp));

       return instance;
   }


    virtual void createInstance(
     const String& nameSpace,
     CIMInstance& myInstance)
    {
	cout << "Create namespace called" << endl;
	// find property "name"
	// How do I throw exception here.???
	Uint32 i = myInstance.findProperty("name");
	if (i == -1)
	    {
	    cout << "Property name not found" << endl;
	    return;
	    }
	// ATTN: Only allow creation of namespaces if the current namespace
	// is root.  Is this important.  Have not thought this out but 
	// seems logical

	// get property "name"
	CIMProperty myProperty = myInstance.getProperty(i);
	// get value from property "name"
	// This is new namespace name.

	CIMValue myValue = myProperty.getValue();

	CIMType myType = myValue.getType();

	String myName;
	myValue.get(myName);

	// QUESTION: Is there a display for CIMType
	cout << myType << " " << myName << endl;
	//PEGASUS_ASSERT (myType == CimSTRING);
	// check if namespace already exists


	Array<String> ns;

	// ATTN: Does this throw an exception?
	ns = _repository->enumerateNameSpaces();
	for (Uint32 i = 0, n = ns.getSize(); i < n; i++)
	    {
       	cout <<"Loop " << ns[i] << " " << n << " " << myName << endl;  
		if (ns[i] == myName)
		     throw CimException(CimException::ALREADY_EXISTS);
	    }


	// create new namespace
	cout << "Here create new Namespace " << myName << endl;
	_repository->createNameSpace(myName);

	return;
    }


   Array<CIMReference> enumerateInstanceNames(
       const String& nameSpace,
       const String& className) 
   {
       Array<CIMReference> instanceRefs;
       Array<String> instanceName;
       Array<String> ns;
              
       // ATTN: Does this throw an exception?
       ns = _repository->enumerateNameSpaces();
 
       // Create an instance name from namespace names
       // ATTN: Legal to append to String in Array?
       for (Uint32 i = 0; i < ns.getSize(); i++)
       {
	    instanceName.append("__Namespace.name=\"");
	    instanceName[i].append(ns[i]);
	    instanceName[i].append("\"");
       } 

       // Convert to references here so can return references
       CIMReference ref;
       try
       {
	   for (Uint32 i = 0; i < instanceName.getSize(); i++)
	   {
		// Convert instance names to References
	       CIMReference::instanceNameToReference(instanceName[i], ref);
	       instanceRefs.append(ref);
	   }
       }
       catch(Exception& e)
       {
           cout << "__Namespace Provider Exception ";
	   cout << e.getMessage() <<  endl; 
       }

       //ATTN: How dow we return error codes from the provider.
       // In the case of the exception above, we should be returning
       // some sort of error code.
       return instanceRefs;
   }

/** initialize - Standard initialization funciton for the
   provider.  This is required for each provider.
   
   NOTE: For the moment, the pointer to the repository is provided
   with the call.  This will be changed in the future for the provider
   interface.  However, this is really a service extension and therefore
   needs this information.
   */
   void initialize(CIMRepository& repository)
   {
       // derefence repository pointer and save for later.
        _repository = &repository;
       cout << "__NamespaceProvider::initialize() called" << endl;
   }
};

// This is the dynamic entry point into this dynamic module. The name of
// this provider is "MyProvider" which is appened to "PegasusCreateProvider_"
// to form a symbol name. This function is called by the ProviderTable
// to load this provider.

// NOTE: The name of the provider must be correct to be loadable.

extern "C" PEGASUS_EXPORT CIMProvider* 
	PegasusCreateProvider___NamespaceProvider() {
   std::cout << "Called PegasusCreateProvider___NamespaceProvider" << std::endl; 
    return new __NamespaceProvider;
}

PEGASUS_NAMESPACE_END



