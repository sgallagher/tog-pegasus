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
// $Author: karl $
//
// $Log: ProviderTemplate.CPP,v $
// Revision 1.1  2001/02/26 10:27:12  karl
// import
//
//
//
//END_HISTORY
/* Description of the Provider 

This is the template for the version 1
providers for Pegasus.

It provides a useful tool to build
new providers.
    
*/

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Repository/CIMRepository.h>

using namespace std;

PEGASUS_NAMESPACE_BEGIN

static CIMRepository* _repository;

class **NAMEOFPROVIDER** : public CIMProvider
{
public:

    **NAMEOFPROVIDER**()
    {
	// cout << "__NamespaceProvider::__NamespaceProvider()" << endl;
    }

    virtual ~**NAMEOFPROVIDER**()
    {
       // cout << "**NAMEOFPROVIDER**::~**NAMEOFPROVIDER**()" << endl;
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
	   cout << "**NAMEOFPROVIDER**::getInstance() called" << endl;
    
	   String tmp;
	   CIMReference::referenceToInstanceName(instanceName, tmp);
	   cout << "instanceName=" << tmp << endl;
    
	   CIMInstance instance("__Namespace");
	   instance.addProperty(CIMProperty("name", tmp));
    	   /////////ADD CODE HERE FOR get Instance ////////////
	   return instance;
       }

    virtual void createInstance(
	const String& nameSpace,
	CIMInstance& myInstance)
	{
	    cout << "**NAMEOFPROVIDER**::createInstance() called" << endl;	
	    // find property "name"
 	    Uint32 i = myInstance.findProperty("name");
	    if (i == -1)
		{
		cout << "Property name not found" << endl;
		return;
		}
	    /////// ADD CODE HERE for create instance /////////
 	    return;
	}


   virtual Array<CIMReference> enumerateInstanceNames(
       const String& nameSpace,
       const String& className) 
       {
 		  
	   // ////////ADD CODE HERE FOR Enumerate Instance Names/////
	    return instanceRefs;
       }

    virtual Array<CIMReference> enumerateInstances(
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = StringArray()) = 0;

	{
		  
	   // ////ADD CODE HERE FOR Enumerate Instance Names/////
	   return(NEEDSRETURNVALUE);
	}


    virtual CIMValue getProperty(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName) = 0;
	{
	    // ///ADD CODE HERE //
	    return(NEEDSRETURNVALUE:
	    }

    virtual void setProperty(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName,
	const CIMValue& newValue = CIMValue()) = 0;
    {
	// ////ADD CODE HERE /////
	return;
    }


/////////////THE FOLLOWING SECTION INITIALIZES THE PROVIDER /////////

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
       cout << "**NAMEOFPROVIDER**::initialize() called" << endl;
   }
};

// This is the dynamic entry point into this dynamic module. The name of
// this provider is "MyProvider" which is appened to "PegasusCreateProvider_"
// to form a symbol name. This function is called by the ProviderTable
// to load this provider.

// NOTE: The name of the provider must be correct to be loadable.

extern "C" PEGASUS_EXPORT CIMProvider* 
	PegasusCreateProvider_**NAMEOFPROVIDER**() {
   std::cout << "Called PegasusCreateProvider_**NAMEOFPROVIDER**" << std::endl; 
    return new **NAMEOFPROVIDER**_**NAMEOFPROVIDER**;
}

PEGASUS_NAMESPACE_END




