//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

/* __Namespace CIMProvider
    The __Namespace Provider provides responses to the CIM Operations defined in
    the DMTF docuument CIM Operations over HTTP (Section 2.5),
    This provider implements 3 functions:
    Create namespace when it receives a create __Namespace instance.
    Delete namesapce when it receives a delete __Namespace instance
    Enumerate namespace when it recieves an enumerate __Namespaces or
	enumerate __Namespace names

    NOTE: It is the intention of the DMTF to depricate the __Namespace
    functions so that eventually this provider will be depricated and
    removed.
*/

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define DDD(X) //X

static CIMRepository* _repository;


const char INSTANCENAME[] = "__Namespace.name=";

class __NamespaceProvider : public CIMProvider
{
public:

    __NamespaceProvider()
    {
	DDD(cout << "__NamespaceProvider::__NamespaceProvider()" << endl;)
    }

    virtual ~__NamespaceProvider()
    {
       DDD(cout << "__NamespaceProvider::~__NamespaceProvider()" << endl;)
    }

    // Returns instance based on instanceName.  Since there is only
    // name in the class, returns only that or error if does not exist
    virtual CIMInstance getInstance(
       const String& nameSpace,
       const CIMReference& instanceName,
       Boolean localOnly = true,
       Boolean includeQualifiers = false,
       Boolean includeClassOrigin = false,
       const Array<String>& propertyList = EmptyStringArray())
   {
       cout << "__NamespaceProvider::getInstance() called" << endl;

       String tmp = instanceName.toString();
       cout << "instanceName=" << tmp << endl;

       CIMInstance instance("__Namespace");
       instance.addProperty(CIMProperty("name", tmp));

       return instance;
   }

    /** createInstance -- Creates a new namespace
    */
    virtual void createInstance(
	const String& nameSpace,
	CIMInstance& myInstance)
	{
	    cout << "Create namespace called" << endl;

	    // find property "name"
	    Uint32 i = myInstance.findProperty("name");
	    if (i == PEG_NOT_FOUND)
		{
		    throw CIMException(CIMException::INVALID_PARAMETER);
		    return;
		}
	    // ATTN: Only allow creation of namespaces if the current namespace
	    // is root.  Is this important.  Have not thought this out but
	    // seems logical. Wrong.  This must be corrected in the future.
	    //if (nameSpace != "root")
	    //        throw CIMException(CIMException::INVALID_NAMESPACE);
	    //        return;

	    // get property "name"
	    CIMProperty myProperty = myInstance.getProperty(i);

	    // get value from property "name"
	    // This is new namespace name. 
	    CIMValue myValue = myProperty.getValue();

	    CIMType myType = myValue.getType();
	    String myName;
	    myValue.get(myName);
	    //PEGASUS_ASSERT (myType == CimSTRING);

	    // check if namespace already exists
	    Array<String> ns;

	    // ATTN: Does this throw an exception?
	    ns = _repository->enumerateNameSpaces();
	    for (Uint32 i = 0, n = ns.size(); i < n; i++)
		{
		cout <<"DEBUG Loop " << ns[i] << " " << n << " " << myName <<
			endl;
		 if (String::equal(ns[i], myName))
			 throw CIMException(CIMException::ALREADY_EXISTS);
		}

	    // create new namespace
	    try
	    {
		_repository->createNameSpace(myName);
	    }
	    catch(Exception& e)
	    {
		// ATTN: Not sure how to handle this error
		cout << "__Namespace Provider Exception ";
		cout << e.getMessage() <<  endl;
	    }

	    return;
	}

    /** deleteInstance - Deletes the Namespace represented
	by the instance
	@param - InstanceReferenc
    */
    virtual void deleteInstance(
	const String& nameSpace,
	const CIMReference& instanceName)
    {
	DDD(cout << "__NamespaceProvider::deleteInstance" << endl;)
        throw CIMException(CIMException::NOT_SUPPORTED);
	// _repository->deleteNameSpace();

    }

   /** enumerateInstanceNames - Enumerates all of the
       namespace names in the repository
   */
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
       for (Uint32 i = 0; i < ns.size(); i++)
       {
	    instanceName.append("__Namespace.name=\"");
	    instanceName[i].append(ns[i]);
	    instanceName[i].append("\"");
       }

       // Convert to references here so can return references
       CIMReference ref;
       try
       {
	   for (Uint32 i = 0; i < instanceName.size(); i++)
	   {
		// Convert instance names to References
	       instanceRefs.append(instanceName[i]);
	   }
       }
       catch(Exception& e)
       {
           // ATTN: Not sure how to handle this error
	   cout << "__Namespace Provider Exception ";
	   cout << e.getMessage() <<  endl;
       }

       //ATTN: How dow we return error codes from the provider.
       // In the case of the exception above, we should be returning
       // some sort of error code.
       return instanceRefs;
   }

    /** initialize - Standard initialization function for the
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
       DDD(cout << "__NamespaceProvider::initialize() called" << endl;)
   }
};

// This is the dynamic entry point into this dynamic module. The name of
// this provider is "MyProvider" which is appened to "PegasusCreateProvider_"
// to form a symbol name. This function is called by the ProviderTable
// to load this provider.

// NOTE: The name of the provider must be correct to be loadable.

extern "C" PEGASUS_EXPORT CIMProvider*
	PegasusCreateProvider___NamespaceProvider() 
{
   DDD(PEGASUS_STD(cout) <<  \
        "Called PegasusCreateProvider___NamespaceProvider" << \
	PEGASUS_STD(endl);)
    return new __NamespaceProvider;
}

PEGASUS_NAMESPACE_END




