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
// Author: 
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


/* Description of the Provider template

This is the template for the version 1providers for Pegasus.
It provides a useful tool to build new providers.

To use the template:
1.   Change all instances of text string **NAMEOFPROVIDER**
     to the name of this provider
*/

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Repository/CIMRepository.h>

using namespace std;

PEGASUS_NAMESPACE_BEGIN

/* Diagnostic tool. Controls Diagnostic output for all
   calls.  Change to DDD(X) X to enable
*/
#define DDD(X) // X
//#define DDD(X) X


static CIMRepository* _repository;

/*  This Section is the provider class created from CIMProvider.
    This section  process each of the CIM_Proivder operation calls
    and returns the results.
    The Template defines all of the reasonable methods.
    ATTN: How do we handle CIM_Provider Operations that are not
    implemented in this particular provider.  We need to:
    
    throw NotSupported("Optional descriptive text");
    
    
*/
class **NAMEOFPROVIDER** : public CIMProvider
{
public:

    **NAMEOFPROVIDER**()
    {
	DDD(cout << "**NAMEOFPROVIDER**::**NAMEOFPROVIDER**()" << endl;)
    }

    virtual ~**NAMEOFPROVIDER**()
    {
       DDD(cout << "**NAMEOFPROVIDER**::~**NAMEOFPROVIDER**()" << endl;)
    }

    // Returns instance based on instanceName.  Since there is only
    // name in the class, returns only that or error if does not exist
    virtual CIMInstance getInstance(
       const String& nameSpace,
       const CIMObjectPath& instanceName,
       Boolean localOnly = true,
       Boolean includeQualifiers = false,
       Boolean includeClassOrigin = false,
       const Array<String>& propertyList = EmptyStringArray())

   {
       DDD(cout << "**NAMEOFPROVIDER**::getInstance() called" << endl;)

       String tmp = instanceName.toString();
       cout << "instanceName=" << tmp << endl;

       // The following is sample conde only

       // Create new instance with received classname
       CIMInstance myInstance(instanceName.getClassName());
       myInstance.addProperty(CIMProperty("name", tmp));

       /////////ADD CODE HERE FOR get Instance ////////////
       return instance;
   }

    virtual void createInstance(
	const String& nameSpace,
	CIMInstance& myInstance)
    {
	DDD(cout << "**NAMEOFPROVIDER**::createInstance() called" << endl;)	
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


   virtual Array<CIMObjectPath> enumerateInstanceNames(
       const String& nameSpace,
       const String& className)

   {
       DDD(cout << "**NAMEOFPROVIDER**::EnumerateInstanceNames()" << endl;) 
       Array<CIMObjectPath> instanceNameRefs;
      
       // ////////ADD CODE HERE FOR Enumerate Instance Names/////

       // Return the Array of CIMObjectPath Generated
	return instanceNameRefs;
   }

   /* Process enumerateInstances.
       ATTN: What do we do about the NULL option on the
       propertyList
   */
   virtual Array<CIMInstance> enumerateInstances(
       const String& nameSpace,
       const String& className,
       Boolean deepInheritance = true,
       Boolean localOnly = true,
       Boolean includeQualifiers = false,
       Boolean includeClassOrigin = false,
       const Array<String>& propertyList = EmptyStringArray())

    {
	DDD(cout << "**NAMEOFPROVIDER**::enumerateInstance()" << endl;)

	Array<CIMObjectPath> instanceNameRefs;

	// ////////ADD CODE HERE FOR Enumerate Instance Names/////

	// Return the Array of CIMObjectPath Generated
	 return instanceNameRefs;
	      
    }

   virtual void modifyInstance(
	const String& nameSpace,
	const CIMInstance& modifiedInstance)

    {
	DDD(cout << "**NAMEOFPROVIDER**::modifyInstance()" << endl;)	

	///// ADD CODE FOR modifyInstance here ///
	return;
    }
    /* Process the getProperty
    ATTN: there was  = 0 on propertyName. WHY
    */
    virtual CIMValue getProperty(
	const String& nameSpace,
	const CIMObjectPath& instanceName,
	const String& propertyName)
    {
	DDD(cout << "**NAMEOFPROVIDER**::getProperty() called"
		 << "instanceName= "	<< instanceName.ToString();
		 " propertyName = " << propertyName << endl;)
	CIMValue returnValue;


	// ///ADD CODE HERE  for get property//
	return(returnValue);
     }

    /* Process setProperty method.
        ATTN: What about the newValue Default
    */
    virtual void setProperty(
	const String& nameSpace,
	const CIMObjectPath& instanceName,
	const String& propertyName,
	const CIMValue& newValue = CIMValue())
    {
	DDD(cout << "**NAMEOFPROVIDER**::setProperty() called"
		 << "instanceName "	<< instanceName.toString();
		 " propertyName = " << propertyName <<
	         " newValue= " << newValue.ToString() endl;)
	
	// ////ADD CODE HERE for set property/////
	return;
    }

    /* Process invokeMethod
       ATTN: what about the default on parameters
    */
    virtual CIMValue invokeMethod(
	const String& nameSpace,
	const CIMObjectPath& instanceName,
	const String& methodName,
	const Array<CIMValue>& inParameters,
	Array<CIMValue>& outParameters)
    {
	DDD(cout << "**NAMEOFPROVIDER**::getProperty() called" << endl;)

	CIMValue returnValue

	return(returnValue);
    }

    Array<CIMInstance> execQuery(
	const String& queryLanguage,
	const String& query) 
    { 
	DDD(cout << "**NAMEOFPROVIDER**::execQuery() called" << endl;)
	
	throw NotSupported("execQuery");
	return Array<CIMInstance>();
    }
    
    Array<CIMInstance> associators(
	const String& nameSpace,
	const CIMObjectPath& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole,
	Boolean includeQualifiers,
	Boolean includeClassOrigin,
	const Array<String>& propertyList)
    {
	DDD(cout << "**NAMEOFPROVIDER**::associators() called" << endl;)

	throw NotSupported("associators");
	return Array<CIMInstance>();
    }
    
    Array<CIMObjectPath> associatorNames(
	const String& nameSpace,
	const CIMObjectPath& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole)
    { 
	DDD(cout << "**NAMEOFPROVIDER**::associatorNames() called" << endl;)
	
	throw NotSupported("associatorNames");
	return Array<CIMObjectPath>();
    }
    
    Array<CIMInstance> references(
	const String& nameSpace,
	const CIMObjectPath& objectName,
	const String& resultClass,
	const String& role,
	Boolean includeQualifiers,
	Boolean includeClassOrigin,
	const Array<String>& propertyList)
    {
	DDD(cout << "**NAMEOFPROVIDER**::references() called" << endl;)
	
	throw NotSupported("references");
	return Array<CIMInstance>();
    }
    
    Array<CIMObjectPath>referenceNames(
	const String& nameSpace,
	const CIMObjectPath& objectName,
	const String& resultClass,
	const String& role)
    { 
	DDD(cout << "**NAMEOFPROVIDER**::referenceNames() called" << endl;)
	
	throw NotSupported("referenceNames");
	return Array<CIMObjectPath>();
    }


/////////////THE FOLLOWING SECTION INITIALIZES THE PROVIDER /////////

/** initialize - Standard initialization funciton for the
   provider.  This is required for each provider.
   
   NOTE: For the moment, the pointer to the repository is provided
   with the call.  This will be changed in the future for the provider
   interface.  However, this is really a service extension and therefore
   needs this information.
   */
   void initialize(CIMOMHandle& cimomHandle)
   {
       // derefence repository pointer and save for later.
        _repository = cimomHandle->getRepository();
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
   DDD(cout << "Called PegasusCreateProvider_**NAMEOFPROVIDER**" << 
	    std::endl;)

   return new **NAMEOFPROVIDER**;
}

PEGASUS_NAMESPACE_END
