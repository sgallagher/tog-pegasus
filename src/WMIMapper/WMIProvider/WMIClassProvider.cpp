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
// Author: Barbara Packard (barbara_packard@hp.com)
//
// Modified By: Paulo Sehn (paulo_sehn@hp.com)
//				Adriano Zanuz (adriano.zanuz@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////
/// WMIClassProvider.cpp: implementation of the WMIClassProvider class.
//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::
//
// ///////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <vector>

#include "WMICollector.h"
#include "WMIBaseProvider.h"
#include "WMIClassProvider.h"

#include "WMIProperty.h"
#include "WMIString.h"
#include "WMIValue.h"
#include "WMIQualifier.h"
#include "WMIQualifierSet.h"
#include "WMIType.h"
#include "WMIException.h"
#include "WMIFlavor.h"

#include "MyString.h"

using namespace std;

PEGASUS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WMIClassProvider::WMIClassProvider(void)
{

	_collector = NULL;
	m_bInitialized = false;
}

WMIClassProvider::~WMIClassProvider(void)
{
	cleanup();

}


/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::getClass
//
/// virtual class CIMClass. From the operations class
// ///////////////////////////////////////////////////////////////////////////
CIMClass WMIClassProvider::getClass(
        const String& nameSpace,
		const String& userName,
		const String& password,
        const String& className,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList)
{

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIClassProvider::getClass()");

	CComPtr<IWbemClassObject> pClass;

	setup(nameSpace,userName,password);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"getClass - localOnly %x, includeQualifiers %x, includeClassOrigin %x", 
		localOnly, includeQualifiers, includeClassOrigin);

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	try 
	{
		if (!(_collector->getObject(&pClass, className)))
		{
			throw CIMException(CIM_ERR_NOT_FOUND);
		}
		else if (_collector->isInstance(pClass))
		{
			throw CIMException(CIM_ERR_INVALID_PARAMETER);
		}
	}
	catch (CIMException &e)
	{
		switch(e.getCode())
		{
			case CIM_ERR_INVALID_CLASS: throw CIMException(CIM_ERR_NOT_FOUND); break;
			default: throw e;
		}
	}

	String superClass = _collector->getSuperClass(pClass);

	CIMName objName = className;

	CIMClass cimClass = CIMClass(objName);

	if (0 != superClass.size())
	{
		CIMName superclassName = superClass;
		cimClass.setSuperClassName(superclassName);
	}

	if (!(_collector->getCIMClass(pClass, 
				cimClass,
				localOnly,
				includeQualifiers,
				includeClassOrigin,
				propertyList)))
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}
	
    PEG_METHOD_EXIT();

	return cimClass;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::enumerateClasses
//
// ///////////////////////////////////////////////////////////////////////////
Array<CIMClass> WMIClassProvider::enumerateClasses(
        const String& nameSpace,
		const String& userName,
		const String& password,
        const String& className,
        Boolean deepInheritance ,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin)
{
	HRESULT hr;
	long lCount = 0;
	DWORD dwReturned;

	CComPtr<IEnumWbemClassObject>	pClassEnum;
	CComPtr<IWbemClassObject>		pClass;

	Array<CIMClass> cimClasses;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIClassProvider::enumerateClasses()");

	setup(nameSpace,userName,password);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"enumerateClasses - deepInheritance %x, localOnly %x, includeQualifiers %x, includeClassOrigin %x", 
		deepInheritance, localOnly, includeQualifiers, includeClassOrigin);
	
	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	if (!(_collector->getClassEnum(&pClassEnum, className, deepInheritance)))
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// set proxy security on pClassEnum
	bool bSecurity = _collector->setProxySecurity(pClassEnum);

	// get the classes and append them to the array

	hr = pClassEnum->Next(WBEM_INFINITE, 1, &pClass, &dwReturned);

	while (SUCCEEDED(hr) && (1 == dwReturned))
	{
		String className = _collector->getClassName(pClass);
		String superClass = _collector->getSuperClass(pClass);

		CIMName objName = className;

		CIMClass tempClass = CIMClass(objName);

		if (0 != superClass.size())
		{
			CIMName superclassName = superClass;
			tempClass.setSuperClassName(superclassName);
		}

		if (_collector->getCIMClass(pClass,
					tempClass,
					localOnly,
					includeQualifiers,
					includeClassOrigin))
		{
			lCount++;

			cimClasses.append(tempClass);
		}

		pClass.Release();
		hr = pClassEnum->Next(WBEM_INFINITE, 1, &pClass, &dwReturned);
	}


	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMIClassProvider::enumerateClasses() - Class count is %d", lCount); 

	if (lCount == 0)
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMIClassProvider::enumerateClasses() - no classes found, hResult value is %x", hr); 
		
		//it is not an error
		//throw CIMException(CIM_ERR_NOT_FOUND);
	}

	PEG_METHOD_EXIT();

	return cimClasses;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::enumerateClassNames
//
// ///////////////////////////////////////////////////////////////////////////
Array<CIMName> WMIClassProvider::enumerateClassNames(
        const String& nameSpace,
		const String& userName,
		const String& password,
        const String& className,
        Boolean deepInheritance)
{
	HRESULT hr;
	long lCount = 0;
	DWORD dwReturned;

	CComPtr<IEnumWbemClassObject>	pClassEnum;
	CComPtr<IWbemClassObject>		pClass;

	Array<CIMName> classNames;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIClassProvider::enumerateClassNames()");

	setup(nameSpace,userName,password);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"enumerateClassNames - deepInheritance %x", deepInheritance);
	
	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// retrieve class enumeration object
	if (!(_collector->getClassEnum(&pClassEnum, className, deepInheritance)))
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// set proxy security on pClassEnum
	bool bSecurity = _collector->setProxySecurity(pClassEnum);

	// get the classe names and append them to the array

	hr = pClassEnum->Next(WBEM_INFINITE, 1, &pClass, &dwReturned);

	while (SUCCEEDED(hr) && (1 == dwReturned))
	{
		String className = _collector->getClassName(pClass);
		lCount++;
		CIMName cimclassname = className;
		classNames.append(cimclassname);

		pClass.Release();
		hr = pClassEnum->Next(WBEM_INFINITE, 1, &pClass, &dwReturned);
	}


	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMIClassProvider::enumerateClassNames() - Class count is %d", lCount); 

	if (lCount == 0)
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMIClassProvider::enumerateClassNames() - no classes found, hResult value is %x", hr); 
		//it is not an error
		//throw CIMException(CIM_ERR_NOT_FOUND);
	}

	PEG_METHOD_EXIT();

	return classNames;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::deleteClass
//
// ///////////////////////////////////////////////////////////////////////////
void WMIClassProvider::deleteClass(const String& nameSpace,
								   const String& userName,
								   const String& password,
								   const String& className)
{
	HRESULT hr;
	
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIClassProvider::deleteClass()");

	CComPtr<IWbemServices> pServices;

	//Connect to namespace
	setup(nameSpace,userName,password);
	
	bool bConnected = _collector->Connect(&pServices);
	
	if (!bConnected) throw CIMException(CIM_ERR_ACCESS_DENIED);
	
	//Convert the parameters to make the WMI call
	CMyString s;
	s = className;
	CComBSTR bsClassName = s.Bstr();
	LONG lFlags = 0L;

	//Perform the WMI operation
	hr = pServices->DeleteClass(bsClassName,
							    lFlags,
								NULL,
								NULL);

	//Handle the WMI operation result
	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, 
			          Tracer::LEVEL3, 
					  "DeleteClass failed hr=0x%x", 
					  hr);

		switch (hr)
		{
			case WBEM_E_ACCESS_DENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_FAILED: throw CIMException(CIM_ERR_FAILED); break;
			case WBEM_E_INVALID_PARAMETER: throw CIMException(CIM_ERR_FAILED, "WMI Invalid Parameter"); break;
			case WBEM_E_INVALID_CLASS: throw CIMException(CIM_ERR_NOT_FOUND); break;
			case WBEM_E_NOT_FOUND: throw CIMException(CIM_ERR_NOT_FOUND); break;
			case WBEM_E_CLASS_HAS_CHILDREN: throw CIMException(CIM_ERR_CLASS_HAS_CHILDREN); break;
			default: throw CIMException(CIM_ERR_FAILED);
		}
	}

	pServices.Release();

	PEG_METHOD_EXIT();
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::createClass
//
// ///////////////////////////////////////////////////////////////////////////
void WMIClassProvider::createClass(const String& nameSpace,
								   const String& userName,
								   const String& password,
								   const CIMClass& newClass)
{
	bool hasSuperClass = false;
	CMyString tmpStr, tmpStr1; // temporary strings

	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::createClass()");

	setup(nameSpace, userName, password);

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}
	
	try 
	{
		performInitialCheck(newClass);

		if (newClass.getSuperClassName().getString() != String::EMPTY)
		{
			hasSuperClass = true;
		}
	}
	catch (CIMException &e)
	{
		throw e;
	}

	// gets the pointers
	CComPtr<IWbemServices> pServices;
	CComPtr<IWbemClassObject> pNewClass;

	if (!_collector->Connect (&pServices))
	{
		throw CIMException (CIM_ERR_FAILED);
	}

	try 
	{
	    // starts the class creation by name and class qualifiers
		createClassNameAndClassQualifiers(
			newClass, 
			pServices, 
			&pNewClass, 
			hasSuperClass);

		// create properties
		createProperties(
			newClass, 
			pServices, 
			pNewClass);

		// create methods
		createMethods(
			newClass, 
			pServices, 
			pNewClass);
	}
	catch (CIMException &e) 
	{
		throw e;
	}
	// create the class into WMI
	HRESULT hr = pServices->PutClass(pNewClass, 0, NULL, NULL);
	
	if (FAILED(hr))
	{
		tmpStr = newClass.getClassName().getString();
		tmpStr1.Format ("WMI error code returned %x", hr);

		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
			          "WmiClassProvider::createClass() - failed to create the class %s %s", (LPCSTR)tmpStr, (LPCSTR)tmpStr1);
		
		throw CIMException(CIM_ERR_FAILED); 
	}

	pServices.Release();
	pNewClass.Release();
	
	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::modifyClass
//
// ///////////////////////////////////////////////////////////////////////////
void WMIClassProvider::modifyClass(const String& nameSpace,
								   const String& userName,
								   const String& password,
								   const CIMClass& modifiedClass)
{
	CMyString tmpStr;
	HRESULT hr;
	
	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WMIClassProvider::modifyClass()");

	setup(nameSpace, userName, password);

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	try
	{
		// check if class doesn't exist
		if (!classAlreadyExists(modifiedClass.getClassName().getString()))
		{
			throw CIMException(CIM_ERR_NOT_FOUND);
		}

		// check if superclass doesn't exist
		if (!classAlreadyExists(modifiedClass.getSuperClassName().getString()))
		{
			// superclass doesn't exist
			throw CIMException(CIM_ERR_INVALID_SUPERCLASS);
		}
	}
	catch (CIMException &e)
	{
		throw e;
	}

	
	// Retrieve class object
	CComPtr<IWbemClassObject> pClass;
	String className = modifiedClass.getClassName().getString();
	
	if (!_collector->getObject(&pClass, className))
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// remove all class methods
	removeMethods(pClass);

	// remove all class properties
	removeProperties(pClass);

	// remove all class qualifiers
	removeQualifiers(pClass);
	
	//
	// recreate the class
	//

	// recreate the class qualifiers
	// get a pointer to work with qualifiers
	CComPtr<IWbemQualifierSet> pQualifier;
	hr = pClass->GetQualifierSet(&pQualifier);

	if (FAILED(hr))
	{
		tmpStr.Format ("WMI error code returned %x", hr);
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
			          "WMIClassProvider::modifyClass() - failed to get a IWbemQualifier pointer %s", (LPCSTR)tmpStr);

		throw CIMException(CIM_ERR_FAILED); 
	}

	for (Uint32 i = 0; i < modifiedClass.getQualifierCount(); i++)
	{
		try 
		{
			WMIQualifier qualifier(modifiedClass.getQualifier(i).clone());
			createQualifier(qualifier, pQualifier);
		}
		catch (CIMException &e)
		{
			throw e;
		}
	}
	
	pQualifier.Release();
	
	// recreate class properties and methods
	CComPtr<IWbemServices> pServices;
	bool bConnected = _collector->Connect(&pServices);
	
	if (!bConnected)
	{
		throw CIMException(CIM_ERR_ACCESS_DENIED);
	}

	try 
	{
		// create properties
		createProperties(
			modifiedClass, 
			pServices, 
			pClass);

		// create methods
		createMethods(
			modifiedClass, 
			pServices, 
			pClass);

	}
	catch (CIMException &e) 
	{
		throw e;
	}
	
	hr = pServices->PutClass(pClass, WBEM_FLAG_UPDATE_ONLY, NULL, NULL);

	if(FAILED(hr))
	{
		switch(hr)
		{
			case E_ACCESSDENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_ACCESS_DENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_CLASS_HAS_CHILDREN: throw CIMException(CIM_ERR_CLASS_HAS_CHILDREN); break;
			case WBEM_E_CLASS_HAS_INSTANCES: throw CIMException(CIM_ERR_CLASS_HAS_INSTANCES); break;
			case WBEM_E_NOT_FOUND: throw CIMException(CIM_ERR_NOT_FOUND);	break;
			case WBEM_E_INVALID_CLASS: throw CIMException(CIM_ERR_INVALID_PARAMETER); break;
			default: throw CIMException(CIM_ERR_FAILED);
		}
	}

	pServices.Release();
	pClass.Release();

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::classAlreadyExists
//
// ///////////////////////////////////////////////////////////////////////////
Boolean WMIClassProvider::classAlreadyExists (const String& className)
{
	CComPtr<IWbemClassObject> pClass;
	CMyString strMsg;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER," WmiClassProvider::classAlreadyExists()");

	strMsg = className;
	
	try
	{
		if (_collector->getObject(&pClass, className))
		{
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"WMIClassProvider::classAlreadyExists() - the class %s already exists", (LPCSTR)strMsg); 

			pClass.Release();
			return true;
		}
	}
	catch(CIMException &e)
	{
		switch(e.getCode())
		{
			case CIM_ERR_NOT_FOUND: return false; break;
			default: throw e;
		}
	}

	pClass.Release();

	PEG_METHOD_EXIT();

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::performInitialCheck
//
// ///////////////////////////////////////////////////////////////////////////
void WMIClassProvider::performInitialCheck(const CIMClass& newClass)
{
	CMyString tmpStr; // temporary strings

	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::performInitialCheck()");
	
	try 
	{
		// check if class already exists
		if (classAlreadyExists(newClass.getClassName().getString()))
		{
			throw CIMException (CIM_ERR_ALREADY_EXISTS);
		}

		// check if newClass has a superclass
		if (newClass.getSuperClassName().getString() != String::EMPTY)
		{
			// verifies if the superclass exists
			if (!classAlreadyExists(newClass.getSuperClassName().getString()))
			{
				// superclass doesn't exist, trace and throw error
				tmpStr = newClass.getSuperClassName().getString();

				Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
					"WMIClassProvider::performInitialCheck() - the superclass %s wasn't yet registered", (LPCSTR) tmpStr); 

				throw CIMException (CIM_ERR_INVALID_SUPERCLASS);
			}
		}
	}
	catch (CIMException &e)
	{
		throw e;
	}

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::createClassNameAndClassQualifiers
//
// ///////////////////////////////////////////////////////////////////////////
void WMIClassProvider::createClassNameAndClassQualifiers(const CIMClass& newClass,
														 IWbemServices *pServices,
														 IWbemClassObject **pNewClass,
														 const bool hasSuperClass)
{
	CMyString tmpStr, tmpStr1;
	HRESULT hr;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::createClassNameAndClassQualifiers ()");

	// if the class has a superclass, we need to spwan a derived
	if (hasSuperClass)
	{
		// get the superclass name
		CComPtr<IWbemClassObject> pSuperClass;
		tmpStr = newClass.getSuperClassName().getString();

		hr = pServices->GetObject(
				tmpStr.Bstr(), 
				NULL, 
				NULL, 
				&pSuperClass, 
				NULL);

		if (FAILED(hr))
		{
			tmpStr1.Format (", WMI error code returned %x", hr);
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
						  "WMIClassProvider::createClassNameAndClassQualifiers() - failed to get a pointer to the SuperClass ", (LPCSTR)tmpStr1);

			throw CIMException(CIM_ERR_FAILED); 
		}

		//Creates the new class
		pSuperClass->SpawnDerivedClass(NULL, pNewClass);
		pSuperClass.Release();
	}
	else
	{
		// we are creating a base class
		hr = pServices->GetObject(NULL, 
								  NULL, 
								  NULL, 
								  pNewClass, 
								  NULL);

		if (FAILED(hr))
		{
			tmpStr.Format (", WMI error code returned %x",hr);
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
						  "WMIClassProvider::createClassNameAndClassQualifiers() - failed to get a pointer to a new WbemClassObject %s", (LPCSTR)tmpStr);
			
			throw CIMException(CIM_ERR_FAILED); 
		}
	}
	
	// create the class name
	CMyString str; 
	str = newClass.getClassName().getString();
	
	CComVariant v; 
	v = str.Bstr();
	
	CMyString Class = "__CLASS";
	
	hr = (*pNewClass)->Put (Class.Bstr(), 0, &v, 0);

	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
			          "WMIClassProvider::createClassNameAndClassQualifiers() - failed to create class %s", (LPCSTR)tmpStr);
		
		throw CIMException(CIM_ERR_FAILED); 
	}

	// get a pointer to work with qualifiers
	CComPtr<IWbemQualifierSet> pNewClassQualifier;
	hr = (*pNewClass)->GetQualifierSet(&pNewClassQualifier);

	if (FAILED(hr))
	{
		tmpStr.Format ("WMI error code returned %x", hr);
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
			          "WMIClassProvider::createClassNameAndClassQualifiers() - failed to get a IWbemQualifier pointer %s", (LPCSTR)tmpStr);
		
		throw CIMException(CIM_ERR_FAILED); 
	}
	
	// check the class qualifiers and create them if they are valid
	// we are taking care of the class qualifiers and not methods/properties qualifiers :D
	for (Uint32 i = 0; i < newClass.getQualifierCount(); i++)
	{
		try 
		{
			WMIQualifier qualifier(newClass.getQualifier(i).clone());
			createQualifier(qualifier, pNewClassQualifier);
		}
		catch (CIMException &e)
		{
			throw e;
		}
	}

	pNewClassQualifier.Release();

	PEG_METHOD_EXIT();

	return;
}


/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::createProperties creates all properties including keys 
//									  add the qualifiers too
// ///////////////////////////////////////////////////////////////////////////
void WMIClassProvider::createProperties(const CIMClass& newClass,
									    IWbemServices *pServices,
										IWbemClassObject *pNewClass)
{
	HRESULT hr;
	CMyString tmpStr, tmpStr1, tmpStr2;
	
	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::createProperties ()");

	// create the properties but don't create the keys again
	CIMProperty keyProp;
	bool isKey = false;

	for (Uint32 i = 0; i < newClass.getPropertyCount(); i++)
	{
		keyProp = newClass.getProperty(i).clone();

		// create the properties
		tmpStr = keyProp.getName().getString();
		try 
		{
			createProperty (keyProp, pNewClass);
		}
		catch (CIMException &e)
		{
			throw e;
		}
		
		// get a pointer to work with qualifiers 
		CComPtr<IWbemQualifierSet> pQual;
		hr = pNewClass->GetPropertyQualifierSet(tmpStr.Bstr(), &pQual);

		if (FAILED(hr))
		{
			tmpStr1.Format ("WMI error code returned %x", hr);
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
						  "WMIClassProvider::createProperties() - failed to get a qualifier pointer for property %s %s", 
						  (LPCSTR)tmpStr, (LPCSTR)tmpStr1);
			
			throw CIMException(CIM_ERR_FAILED); 
		}

		// set the qualifiers to the property
		for (Uint32 j = 0; j < keyProp.getQualifierCount(); j++)
		{
			WMIQualifier qualifier(keyProp.getQualifier(j));
			try 
			{
				createQualifier(qualifier, pQual);
			}
			catch (CIMException &e)
			{
				pQual.Release();
				throw e;
			}
		}

		// set the CLASSORIGIN qualifier if it wasn't set yet
		tmpStr1 = keyProp.getClassOrigin().getString();
		tmpStr2 = "CLASSORIGIN";

		if (tmpStr1.IsEmpty())
		{
			tmpStr1 = newClass.getClassName().getString();
		}

		CComVariant v;
		v.vt = VT_BSTR;
		v.bstrVal = tmpStr1.Bstr();
		WMIFlavor flavor(CIMFlavor::DEFAULTS);

		hr = pQual->Put(tmpStr2.Bstr(), &v, flavor.getAsWMIValue());

		if (FAILED(hr))
		{
			pQual.Release();
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
						  "WMIClassProvider::createProperties () - failed to create CLASSORIGIN, WMI error code returned %h", hr);
			
			throw CIMException(CIM_ERR_FAILED); 
		}

		pQual.Release();
	}

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::createMethods  create methods 
//									  
/////////////////////////////////////////////////////////////////////////////
void WMIClassProvider::createMethods(const CIMClass& newClass,
								     IWbemServices *pServices,
									 IWbemClassObject *pNewClass)
{

	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WMIClassProvider::createMethods ()");

	// create all methods
	for (Uint32 i = 0; i < newClass.getMethodCount(); i++)
	{
		CIMConstMethod method;
		method = newClass.getMethod(i);

		try 
		{
			createMethod (method, pServices, pNewClass);
		}
		catch (CIMException &e)
		{
			throw e;
		}
	}

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::createProperty   creates one property 
//									  doesn't create the qualifiers
/////////////////////////////////////////////////////////////////////////////
void WMIClassProvider::createProperty(const CIMProperty &keyProp, 
									  IWbemClassObject *pNewClass)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::createProperty()");

	// create the property
	CMyString tmpStr;
	tmpStr = keyProp.getName().getString();

	//Get the CIMTYPE of the parameter
	CIMTYPE type = CIMTypeToWMIType(keyProp.getType());

	//If the property is an array, add CIM_FLAG_ARRAY to CIMType
	if (keyProp.isArray())
	{
		type |= CIM_FLAG_ARRAY;
	}

	// add the property to the class
	HRESULT hr = pNewClass->Put(tmpStr.Bstr(), 
								NULL, 
								NULL, 
								type);

	//handle the error, if failed
	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, 
			          Tracer::LEVEL3, 
					  "WMIClassProvider::createProperty () - failed to create a property, WMI error code returned %h", 
					  hr);

		throw CIMException(CIM_ERR_FAILED); 
	}

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::createQualifier  creates a qualifiers 
//									  
/////////////////////////////////////////////////////////////////////////////
void WMIClassProvider::createQualifier (const WMIQualifier &qualifier, 
										IWbemQualifierSet *pQual)
{
	HRESULT hr;
	CMyString tmpStr;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::createQualifier()");

	tmpStr = qualifier.getName().getString();

	WMIValue value (qualifier.getValue());
	WMIFlavor flavor (qualifier.getFlavor());
	CComVariant v;
	value.getAsVariant(&v);

	// key is created using a special call to wmi
	if (!stricmp("key", (LPCSTR)tmpStr))
	{
		hr = pQual->Put(tmpStr.Bstr(), &v, NULL);
	}
	else
	{
		hr = pQual->Put(tmpStr.Bstr(), &v, flavor.getAsWMIValue());
	}

	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
					  "WMIClassProvider::createQualifier () - failed to create a qualifier, WMI error code returned %h", hr);
		
		throw CIMException(CIM_ERR_FAILED); 
	}

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::createMethod  create a method
//									  
/////////////////////////////////////////////////////////////////////////////
void WMIClassProvider::createMethod (CIMConstMethod &method,
									 IWbemServices *pServices,
									 IWbemClassObject *pNewClass)
{
	// the parameters
	HRESULT hr;
	CMyString tmpStr;
	tmpStr = "__PARAMETERS";
	
	CComPtr<IWbemClassObject> pinParameters;
	CComPtr<IWbemClassObject> poutParameters;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::createMethod ()");

	// create the in parameters
	for (Uint32 i = 0; i < method.getParameterCount(); i++)
	{
		CIMConstParameter param;
		param = method.getParameter(i);

		// process only the [in] parameters
		if (param.findQualifier(CIMName("in")) != -1)
		{
			try 
			{
				// gets a pointer to in parameters
				hr = pServices->GetObject(tmpStr.Bstr(),
					                      NULL, 
										  NULL, 
										  &pinParameters, 
										  NULL);

				if (FAILED(hr))
				{
					Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
								  "WMIClassProvider::createMethods () - failed to get a pointer to the inParameters, WMI error code returned %h", hr);
					
					throw CIMException(CIM_ERR_FAILED); 
				}

				createParam (param, pinParameters);
			}
			catch (CIMException &e)
			{
				throw e;
			}
		} 
		else if (param.findQualifier(CIMName("out")) != -1)
		{
			// create the out parameters
			try 
			{
				// gets a pointer to out parameters
				hr = pServices->GetObject(tmpStr.Bstr(), 
					                      NULL, 
										  NULL, 
										  &poutParameters, 
										  NULL);

				if (FAILED(hr))
				{
					Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
								  "WMIClassProvider::createMethods () - failed to get a pointer to the outParameters, WMI error code returned %h", hr);
					
					throw CIMException(CIM_ERR_FAILED); 
				}

				createParam (param, poutParameters);
			}
			catch (CIMException &e)
			{
				throw e;
			}
		}
	}

	// create the method
	tmpStr = method.getName().getString();
	hr = pNewClass->PutMethod(tmpStr.Bstr(), 
		                      NULL, 
							  pinParameters, 
							  poutParameters);

	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
					  "WMIClassProvider::createMethod () - failed to create a method, WMI error code returned %h", hr);
		
		throw CIMException(CIM_ERR_FAILED); 
	}

	pinParameters.Release();
	poutParameters.Release();

	// create the qualifiers for the method
	CComPtr<IWbemQualifierSet> pQual;

	// get a pointer to work with qualifiers 
	hr = pNewClass->GetMethodQualifierSet(tmpStr.Bstr (), &pQual);

	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
					  "WMIClassProvider::createParam () - failed to get a qualifier pointer, WMI error code returned %h", hr);
		throw CIMException(CIM_ERR_FAILED); 
	}

	// create the qualifiers for the method
	for (Uint32 i = 0; i < method.getQualifierCount(); i++)
	{
		CIMQualifier cimQuali;
		cimQuali = method.getQualifier(i).clone();
		WMIQualifier qualifier(cimQuali);

		try 
		{
			createQualifier(qualifier, pQual);
		}
		catch (CIMException &e)
		{
			pQual.Release();
			throw e;
		}
	}

	pQual.Release();
	
	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::createParam  create a parameter
//									  
/////////////////////////////////////////////////////////////////////////////
void WMIClassProvider::createParam(const CIMConstParameter &param, 
								   IWbemClassObject *pNewClass)
{
	
	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::createParam ()");
	
	// get the parameter name
	CMyString tmpStr;
	tmpStr = param.getName().getString();

	//Get the CIMTYPE of the parameter
	CIMTYPE type = CIMTypeToWMIType(param.getType());

	//If the property is an array, add CIM_FLAG_ARRAY to CIMType
	if (param.isArray())
	{
		type |= CIM_FLAG_ARRAY;
	}

	// add the property to the class
	HRESULT hr = pNewClass->Put(tmpStr.Bstr(), 
								NULL, 
								NULL, 
								type);

	//handle the error, if failed
	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
					  "WMIClassProvider::createParam () - failed to create a parameter, WMI error code returned %h", 
					  hr);

		throw CIMException(CIM_ERR_FAILED); 
	}

	// create the qualifiers for this parameter
	CComPtr<IWbemQualifierSet> pQual;

	// get a pointer to work with qualifiers 
	hr = pNewClass->GetPropertyQualifierSet(tmpStr.Bstr(), &pQual);
	
	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3, 
					  "WMIClassProvider::createParam () - failed to get a qualifier pointer, WMI error code returned %h", hr);
		
		throw CIMException(CIM_ERR_FAILED); 
	}

	// create the qualifiers for this parameter
	for (Uint32 i = 0; i < param.getQualifierCount(); i++)
	{
		CIMQualifier cimQuali;
		cimQuali = param.getQualifier(i).clone();
		WMIQualifier qualifier (cimQuali);

		try 
		{
			createQualifier (qualifier, pQual);
		}
		catch (CIMException &e)
		{
			pQual.Release();
			throw e;
		}
	}
	
	pQual.Release();

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::removeProperties  remove the class methods
//									  
/////////////////////////////////////////////////////////////////////////////
void WMIClassProvider::removeMethods(IWbemClassObject *pClass) 
{
	HRESULT hr;
	vector<CComBSTR> vecBsMethodNames;
	CComBSTR bsMethodName;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::removeMethods()");
	
	hr = pClass->BeginMethodEnumeration(0L);
	
	if (FAILED(hr))
	{
		throw CIMException(CIM_ERR_FAILED); 
	}
		
	while (pClass->NextMethod(NULL, &bsMethodName, NULL, NULL) != WBEM_S_NO_MORE_DATA)
	{
		vecBsMethodNames.push_back(bsMethodName);
	}	
	
	hr = pClass->EndMethodEnumeration();
	
	if (FAILED(hr))
	{
		throw CIMException(CIM_ERR_FAILED); 
	}
	
	// For each method...
	for (long i = 0; i < vecBsMethodNames.size(); i++) 
	{
		// Delete the method
		hr = pClass->DeleteMethod(vecBsMethodNames[i]);

		SysFreeString(vecBsMethodNames[i]);
				
		if (FAILED(hr))
		{
			throw CIMException(CIM_ERR_FAILED);
		}
	}

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::removeProperties  remove the class properties
//									  
/////////////////////////////////////////////////////////////////////////////
void WMIClassProvider::removeProperties (IWbemClassObject *pClass) 
{
	HRESULT hr;
	SAFEARRAY *psaPropNames = NULL;
	CComBSTR bsPropertyName;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::removeProperties()");

	// Get class properties names
	hr = pClass->GetNames(NULL, WBEM_FLAG_LOCAL_ONLY, NULL, &psaPropNames);

	if(FAILED(hr))
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// Get the number of properties
	long lLower, lUpper; 

	SafeArrayGetLBound(psaPropNames, 1, &lLower);
	SafeArrayGetUBound(psaPropNames, 1, &lUpper);

	// For each property...
	for (long i = lLower; i <= lUpper; i++) 
	{
		// Get the property
		hr = SafeArrayGetElement(
			psaPropNames, 
			&i, 
			&bsPropertyName);
			
		if(FAILED(hr))
		{
			throw CIMException(CIM_ERR_FAILED);
		}

		// Delete the property
		hr = pClass->Delete(bsPropertyName);

		if (FAILED(hr))
		{
			throw CIMException(CIM_ERR_FAILED); 
		}

		SysFreeString(bsPropertyName);
	}
	
	SafeArrayDestroy(psaPropNames);

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::removeQualifiers  remove the class qualifiers
//									  
/////////////////////////////////////////////////////////////////////////////
void WMIClassProvider::removeQualifiers(IWbemClassObject *pClass) 
{
	HRESULT hr;
	CComPtr<IWbemQualifierSet> pQualifier;
	SAFEARRAY *psaQualifNames = NULL;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER, "WmiClassProvider::removeQualifiers()");
	
	// Get a pointer to work with qualifiers
	hr = pClass->GetQualifierSet(&pQualifier);

	if (FAILED(hr))
	{
		throw CIMException(CIM_ERR_FAILED); 
	}

	// Get all qualifiers names
	hr = pQualifier->GetNames(0L, &psaQualifNames);

	if (FAILED(hr))
	{
		throw CIMException(CIM_ERR_FAILED); 
	}
	
	// Get the number of qualifiers
	long lLower, lUpper; 

	SafeArrayGetLBound(psaQualifNames, 1, &lLower);
	SafeArrayGetUBound(psaQualifNames, 1, &lUpper);

	// For each qualifier...
	CComBSTR bsQualifierName;

	for (long i = lLower; i <= lUpper; i++) 
	{
		// Get the qualifier
		hr = SafeArrayGetElement(
			psaQualifNames, 
			&i, 
			&bsQualifierName);
		
		if (FAILED(hr))
		{
			throw CIMException(CIM_ERR_FAILED); 
		}
					
		// Delete the qualifier
		hr = pQualifier->Delete(bsQualifierName);
		if (FAILED(hr))
		{
			throw CIMException(CIM_ERR_FAILED);
		}
		
		SysFreeString(bsQualifierName);
	}
	
	SafeArrayDestroy(psaQualifNames);

	pQualifier.Release();
		
	PEG_METHOD_EXIT();

	return;
}

PEGASUS_NAMESPACE_END