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
// Modified By: Adriano Zanuz (adriano.zanuz@hp.com)
//				Mateus Baur (HPB)
//
//%/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::
//
// ///////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "WMICollector.h"
#include "WMIBaseProvider.h"
#include "WMIClassProvider.h"
#include "WMIInstanceProvider.h"
#include "WMIObjectPath.h"

#include "WMIProperty.h"
#include "WMIString.h"
#include "WMIValue.h"
#include "WMIQualifier.h"
#include "WMIQualifierSet.h"
#include "WMIType.h"
#include "WMIException.h"
#include <mbstring.h>

//using namespace std;

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::
//
// ///////////////////////////////////////////////////////////////////////////
PEGASUS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
WMIInstanceProvider::WMIInstanceProvider(void)
{
	_collector = NULL;
	m_bInitialized = false;
}

WMIInstanceProvider::~WMIInstanceProvider(void)
{
	cleanup();
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::getInstance
//
// ///////////////////////////////////////////////////////////////////////////
CIMInstance WMIInstanceProvider::getInstance(
        const String& nameSpace,
        const String& userName,
        const String& password,
        const CIMObjectPath& instanceName,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin ,
        const CIMPropertyList& propertyList)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::getInstance()");

	CComPtr<IWbemClassObject> pInstance;
	String sClassName = String(instanceName.getClassName());
	String sInstanceName = getObjectName(instanceName);
	CIMInstance cimInstance(sClassName);

	setup(nameSpace, userName, password);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"getInstance - localOnly %x, includeQualifiers %x, includeClassOrigin %x", 
		localOnly, 
		includeQualifiers, 
		includeClassOrigin);

	CMyString p; p = sClassName;
	CMyString q; q = nameSpace;
	CMyString r; r = sInstanceName;

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"getInstance - classname - %s, namespace - %s, instancename - %s", 
		(LPCTSTR)p, (LPCTSTR)q, (LPCTSTR)r);

	if (!m_bInitialized)
	{
		throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Collector initialation failed.");
	}

	// retrieve instance object
	if (!(_collector->getObject(&pInstance, sInstanceName)))
	{		
		throw CIMException(CIM_ERR_NOT_FOUND);
	}
	else if (!(_collector->isInstance(pInstance)))
	{
		throw CIMException(CIM_ERR_INVALID_PARAMETER);
	}

	// Get the instance object.
	if (!_collector->getCIMInstance(pInstance, 
		                            cimInstance,
									localOnly, 
									includeQualifiers,
									includeClassOrigin, 
									propertyList, 
									TRUE)) // need key properties here
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	PEG_METHOD_EXIT();

	return cimInstance;
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::enumerateInstances
//
// ///////////////////////////////////////////////////////////////////////////
Array<CIMInstance> WMIInstanceProvider::enumerateInstances(
        const String& nameSpace,
        const String& userName,
        const String& password,
        const String& className,
        Boolean deepInheritance,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList)
{
	HRESULT hr;
	long lCount = 0;
	DWORD dwReturned;

	CComPtr<IEnumWbemClassObject>	pInstEnum;
	CComPtr<IWbemClassObject>		pInstance;

	Array<CIMInstance> namedInstances;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::enumerateInstances()");

	setup(nameSpace, userName, password);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"enumerateInstances - deepInheritance %x, localOnly %x, includeQualifiers %x, includeClassOrigin %x", 
		deepInheritance, localOnly, includeQualifiers, includeClassOrigin);
	
	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// retrieve instance enumeration object
	if (!(_collector->getInstanceEnum(&pInstEnum, className, deepInheritance)))
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// set proxy security on pInstEnum
	bool bSecurity = _collector->setProxySecurity(pInstEnum);

	CIMClass cimclass = getCIMClass(nameSpace, 
		                            userName,
									password,
									className);

	// Get the instances and append them to the array
	hr = pInstEnum->Next(WBEM_INFINITE, 1, &pInstance, &dwReturned);

	while (SUCCEEDED(hr) && (1 == dwReturned))
	{
		CIMInstance tempInst(className);

		if (_collector->getCIMInstance(pInstance, 
			                           tempInst,
									   localOnly, 
									   includeQualifiers,
									   includeClassOrigin, 
									   propertyList, 
									   TRUE))
		{
			lCount++;

			//build instance path
			//CIMObjectPath tempRef = tempInst.buildPath(cimclass);			
			CComVariant v;
			hr = pInstance->Get(L"__PATH", 
				                0,
								&v,
								NULL,
								NULL);

			WMIObjectPath tempRef(v.bstrVal);
			
			tempInst.setPath(tempRef);

			namedInstances.append(CIMInstance(tempInst));			
		}

		pInstance.Release();
		hr = pInstEnum->Next(WBEM_INFINITE, 1, &pInstance, &dwReturned);
	}

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMIInstanceProvider::enumerateInstances() - Instance count is %d", lCount); 

	if (lCount == 0)
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMIInstanceProvider::enumerateInstances() - hResult value is %x", hr); 
		//it is not an error
		//throw CIMException(CIM_ERR_NOT_FOUND);
	}

	PEG_METHOD_EXIT();

	return namedInstances;
}
	
/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::enumerateInstanceNames
//
// ///////////////////////////////////////////////////////////////////////////
Array<CIMObjectPath> WMIInstanceProvider::enumerateInstanceNames(
		const String& nameSpace,
        const String& userName,
        const String& password,
		const String& className)
{
	HRESULT hr;
	long lCount = 0;
	DWORD dwReturned;

	CComPtr<IEnumWbemClassObject>	pInstEnum;
	CComPtr<IWbemClassObject>		pInstance;

	Array<CIMObjectPath> instanceNames;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::enumerateInstanceNames()");

	setup(nameSpace, userName, password);

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// retrieve the instance enumeration object
	if (!(_collector->getInstanceEnum(&pInstEnum, className, FALSE)))
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// set proxy security on pInstEnum
	bool bSecurity = _collector->setProxySecurity(pInstEnum);

	CIMClass cimclass = getCIMClass(nameSpace, 
								    userName,
									password,
									className);

	// Get the names of the instances and send to handler
	hr = pInstEnum->Next(WBEM_INFINITE, 1, &pInstance, &dwReturned);

	while (SUCCEEDED(hr) && (1 == dwReturned))
	{
		CIMInstance tempInst(className);

		if (_collector->getCIMInstance(pInstance, 
			                           tempInst, 
									   FALSE, 
									   FALSE,
									   FALSE))
		{
			lCount++;

			//build instance path
			CComVariant v;
			hr = pInstance->Get(L"__PATH", 0, &v, NULL, NULL);
			
			WMIObjectPath tempRef(v.bstrVal);
			instanceNames.append(tempRef);
		}

		pInstance.Release();
		hr = pInstEnum->Next(WBEM_INFINITE, 1, &pInstance, &dwReturned);
	}

	Tracer::trace(TRC_WMIPROVIDER, 
		          Tracer::LEVEL3,
				  "WMIInstanceProvider::enumerateInstanceNames() - Instance count is %d", 
				  lCount); 

	if (lCount == 0)
	{
		Tracer::trace(TRC_WMIPROVIDER, 
			          Tracer::LEVEL3,
					  "WMIInstanceProvider::enumerateInstanceNames() - hResult value is %x", 
					  hr); 
		
		//it is not an error 
		//throw CIMException(CIM_ERR_NOT_FOUND);
	}
	PEG_METHOD_EXIT();

	return instanceNames;
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::getProperty
//
// ///////////////////////////////////////////////////////////////////////////
CIMValue WMIInstanceProvider::getProperty(
		const String& nameSpace,
        const String& userName,
        const String& password,
		const CIMObjectPath& instanceName,
		const String& propertyName)
{

	CIMInstance cimInstance;
	Array<CIMName> propertyNames;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::getProperty()");

	setup(nameSpace,userName,password);

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	CIMName propName = propertyName;

	propertyNames.append(propName);

	CIMPropertyList propertyList = CIMPropertyList(propertyNames);

	// get the relevant CIMInstance object
	cimInstance = getCIMInstance(nameSpace, 
		                         userName, 
								 password, 
								 instanceName, 
								 propertyList);

	// now fetch the property
	Uint32 pos = cimInstance.findProperty(propName);

	if (PEG_NOT_FOUND == pos)
	{
		throw CIMException(CIM_ERR_NO_SUCH_PROPERTY);
	}

	CIMProperty property = cimInstance.getProperty(pos);

	// and return the value
	CIMValue value = property.getValue();

	PEG_METHOD_EXIT();

	return value;
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::setProperty
//
// ///////////////////////////////////////////////////////////////////////////
void WMIInstanceProvider::setProperty(
		const String& nameSpace,
        const String& userName,
        const String& password,
		const CIMObjectPath& instanceName,
		const String& propertyName,
		const CIMValue& newValue)
{

	CComPtr<IWbemServices>			pServices;
	CComPtr<IWbemClassObject>		pInstance;
	CComVariant						vValue;
	CComBSTR						bsPropName;
	HRESULT							hr;
	String							sInstanceName;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::setProperty()");

	sInstanceName = getObjectName(instanceName);	

	setup(nameSpace, userName, password);

	CMyString prop;	prop = propertyName;
	CMyString q; q = sInstanceName;

	Tracer::trace(TRC_WMIPROVIDER, 
		          Tracer::LEVEL3,
				  "setProperty() - setting property %s in %s", 
				  (LPCTSTR)prop, 
				  (LPCTSTR)q);

	if (!m_bInitialized)
	{
		throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Collector initialization failed.");
	}

	// retrieve instance object
	if (!(_collector->getObject(&pInstance, sInstanceName)))
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}
	else if ((!(_collector->isInstance(pInstance))) || (prop == ""))
	{
		throw CIMException(CIM_ERR_INVALID_PARAMETER);
	}

	//check if property exists
	CIMInstance cimInstance;
	Array<CIMName> propertyNames;
	CIMName propName = propertyName;
	
	propertyNames.append(propName);
	CIMPropertyList propertyList = CIMPropertyList(propertyNames);	
	
	cimInstance = getCIMInstance(nameSpace, 
		                         userName, 
								 password, 
								 instanceName, 
								 propertyList);

	Uint32 pos = cimInstance.findProperty(propName);

	if (PEG_NOT_FOUND == pos)
	{
		throw CIMException(CIM_ERR_NO_SUCH_PROPERTY);
	}

	// check the existing value and type
	bsPropName = prop.Bstr();

	//convert property value from CIMValue to VARIANT
	WMIValue(newValue).getAsVariant(&vValue);

	/*
	//adjust the string value
	if (vValue.vt == VT_BSTR)
	{	
		CMyString strTmp;
		String strAux;

		newValue.get(strAux);
		strTmp = strAux;
		vValue.bstrVal[strTmp.GetLength()] = 0;
	}
	*/

	//update property value
	hr = pInstance->Put(bsPropName, 0, &vValue, 0);

	if (FAILED(hr))
	{
		switch(hr)
		{
		case WBEM_E_TYPE_MISMATCH:
			throw CIMException(CIM_ERR_TYPE_MISMATCH);
		default:
			Tracer::trace(TRC_WMIPROVIDER, 
				          Tracer::LEVEL3,
						  "setProperty() - Put failed, hr = %x", 
						  hr);

			throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "WMI Put property failed.");				
		}
		
	}
	
	//update instance
	_collector->Connect(&pServices);

	hr = pServices->PutInstance(pInstance, 
		                        WBEM_FLAG_UPDATE_ONLY, 
								NULL, 
								NULL);

	pInstance.Release();
	pServices.Release();

	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, 
			          Tracer::LEVEL3,
					  "setProperty() - PutInstance failed, hr = %x", 
					  hr);

		throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "WMI Put instance failed.");
	}
	
	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::modifyInstance
//
// ///////////////////////////////////////////////////////////////////////////
void WMIInstanceProvider::modifyInstance(
	const String& nameSpace,
    const String& userName,
    const String& password,
	const CIMInstance& modifiedInstance,
	Boolean includeQualifiers,
	const CIMPropertyList& propertylist)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIClassProvider::modifyInstance()");

	HRESULT hr;
    CComPtr<IWbemClassObject> pClass;
    CComPtr<IWbemClassObject> pInstance;

	setup(nameSpace, userName, password);

	Tracer::trace(TRC_WMIPROVIDER, 
		          Tracer::LEVEL3,
				  "ModifyInstance() - nameSpace %s, userName %s",
				  nameSpace.getCString(),
				  userName.getCString());

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// Check if the instance's class is valid.
	String className = modifiedInstance.getClassName().getString();
	
	try
	{
		if (!(_collector->getObject(&pClass, className)))
		{
			throw CIMException(CIM_ERR_INVALID_CLASS);
		}
		else if (_collector->isInstance(pClass))
		{
			throw CIMException(CIM_ERR_INVALID_PARAMETER);
		}

		pClass.Release();
	}
	catch (CIMException &e)
	{
		switch(e.getCode())
		{
			case CIM_ERR_NOT_FOUND: throw CIMException(CIM_ERR_INVALID_CLASS); break;
			default: throw e;
		}
	}
	
	// Get the instance path
	CIMObjectPath objPath = modifiedInstance.getPath();
	
	// Get the name of the instance
	String instanceName = getObjectName(objPath);  // TERRY: WAS: objPath.toString();
	
	// Check if the instance exists
	if (!(_collector->getObject(&pInstance, instanceName)))
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}
	else if (!(_collector->isInstance(pInstance)))
	{
		throw CIMException(CIM_ERR_INVALID_PARAMETER);
	}

	// Set the properties that are into propertylist
	Array<CIMName> listNames;
	listNames = propertylist.getPropertyNameArray();
	
	CMyString str;
	bool foundInArray;
	bool bPropertySet = false;

	for(Uint32 i = 0; i < modifiedInstance.getPropertyCount(); i++)
	{
		CComVariant v;
		CIMProperty property = modifiedInstance.getProperty(i).clone();
		str = property.getName().getString();

		// change only the properties defined into the array
		// if the array is null, change all properties
		if (propertylist.isNull())
		{
			foundInArray = true;
		}
		else
		{
			foundInArray = false;
			for (Uint32 j = 0; (j < listNames.size()) && !foundInArray; j++)
				if (listNames[j].getString() == str)
					foundInArray = true;
		}

		if (foundInArray)
		{
			WMIValue propertyValue = property.getValue();

			try
			{
				propertyValue.getAsVariant(&v);
			}
			catch (CIMException &e)
			{
				throw e;
			}
			
			hr = pInstance->Put(str.Bstr(), 0, &v, 0);
		
			// If we fail to set one property, we must assure 
			// that the others will be processed
			if(SUCCEEDED(hr))
			{
				// Mark that at least one property was set
				bPropertySet = true;
			}
		}
	}		

	// Check if at least one property was set
	// otherwise throw invalid parameter error
	if(!bPropertySet)
	{
		Tracer::trace(TRC_WMIPROVIDER, 
			          Tracer::LEVEL3,
					  "modifyInstance() - Put Failed hr=0x%x.", 
					  hr);

		throw CIMException(CIM_ERR_FAILED);
	}

	// Connect to the server
	CComPtr<IWbemServices> pServices;
	bool bConnected = _collector->Connect(&pServices);

	if (!bConnected)
	{
		throw CIMException(CIM_ERR_ACCESS_DENIED);
	}

	// Write the instance to WMI. 
	hr = pServices->PutInstance(pInstance, 
		                        WBEM_FLAG_UPDATE_ONLY, 
								NULL, 
								NULL);

	if(FAILED(hr))
	{
		switch(hr)
		{
			case E_ACCESSDENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_ACCESS_DENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_NOT_FOUND: throw CIMException(CIM_ERR_NOT_FOUND);	break;
			case WBEM_E_INVALID_CLASS: throw CIMException(CIM_ERR_INVALID_CLASS); break;
			case WBEM_E_INVALID_OBJECT: throw CIMException(CIM_ERR_INVALID_PARAMETER); break;
			default: throw CIMException(CIM_ERR_FAILED);
		}
	}

	pInstance.Release();
	pServices.Release();

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::createInstance
//
// ///////////////////////////////////////////////////////////////////////////
CIMObjectPath WMIInstanceProvider::createInstance(
		const String& nameSpace,
        const String& userName,
        const String& password,
		const CIMInstance& newInstance)
{
	

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::createInstance()");

	HRESULT hr;	
	CComPtr<IWbemClassObject>	pClass;
    CComPtr<IWbemClassObject>	pNewInstance;
	CMyString myStr;

	setup(nameSpace, userName, password);

	Tracer::trace(TRC_WMIPROVIDER, 
		          Tracer::LEVEL3,
				  "createInstance() - nameSpace %s, userName %s",
				  nameSpace.getCString(),
				  userName.getCString());

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// Get the class definition.
	String className = newInstance.getClassName().getString();
	
	try
	{
		if (!(_collector->getObject(&pClass, className)))
		{
			throw CIMException(CIM_ERR_INVALID_CLASS);
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
			case CIM_ERR_NOT_FOUND: throw CIMException(CIM_ERR_INVALID_CLASS); break;
			default: throw e;
		}
	}

    // Create a new instance.
    hr = pClass->SpawnInstance(0, &pNewInstance);
	pClass.Release();

	if(FAILED(hr))
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// Set the properties
	for(Uint32 i = 0; i < newInstance.getPropertyCount(); i++)
	{
		CComVariant v;

		CIMProperty property = newInstance.getProperty(i).clone();
		myStr = property.getName().getString();
		CIMValue propertyValue = property.getValue();

		try
		{
			WMIValue(propertyValue).getAsVariant(&v);
		}
		catch (CIMException &e)
		{
			throw e;
		}
			
		hr = pNewInstance->Put(myStr.Bstr(), 0, &v, 0);

		if(FAILED(hr))
		{
			throw CIMException(CIM_ERR_FAILED);
		}
	}		
	
	// Connect to the server
	CComPtr<IWbemServices>	pServices;
	bool bConnected = _collector->Connect(&pServices);

	if (!bConnected)
	{
		throw CIMException(CIM_ERR_ACCESS_DENIED);
	}
	
	// Write the instance to WMI. 
	CComPtr<IWbemCallResult> pResult;

	hr = pServices->PutInstance(pNewInstance, 
		                        WBEM_FLAG_CREATE_ONLY, 
								NULL, 
								&pResult);

	if(FAILED(hr))
	{
		switch(hr)
		{
			case E_ACCESSDENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_ACCESS_DENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_ALREADY_EXISTS: throw CIMException(CIM_ERR_ALREADY_EXISTS);	break;
			case WBEM_E_INVALID_CLASS: throw CIMException(CIM_ERR_INVALID_CLASS); break;
			default: throw CIMException(CIM_ERR_FAILED);;
		}
	}

	// set proxy security on pResult
	bool bSecurity = _collector->setProxySecurity(pResult);
	
	// Mount the path to return

	// Prepend namespace to path
	String sPath = "//";
	
	// Append the host name if it is local
	if(_collector->isLocalNamespace())
	{
		sPath.append(getHostName());
		sPath.append("/");
	}

	// Append the namespace, colon 
	sPath.append(nameSpace);
	sPath.append(":");
	
	// String conversion
	myStr = sPath;
	CComBSTR bsPath = myStr.Bstr();

	// Get the key bindings
	CComBSTR bsKeyBindings;
	hr = pResult->GetResultString(WBEM_INFINITE, &bsKeyBindings);
	
	if(FAILED(hr))
	{
		switch(hr)
		{
			case E_ACCESSDENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_ACCESS_DENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
            ////////////////////////////////////////////////////////////////////////////
            // TERRY ADDED:
            case WBEM_E_INVALID_OPERATION:
                // GetResultString() may fail on Win2K - get the keys from the CIM instance:
                if (!GetKeyBindingsFromCIMInstance(newInstance, &bsKeyBindings))
                    throw CIMException(CIM_ERR_NOT_SUPPORTED);
                break;
            ////////////////////////////////////////////////////////////////////////////
			default: throw CIMException(CIM_ERR_FAILED);
		}
	}
	
	// Append the key bindings to the path  
	bsPath.Append(bsKeyBindings);
	
	// Return the CIMObjectPath
	myStr = bsPath.m_str;

	//free the objects
	pNewInstance.Release();
	pServices.Release();
	pResult.Release();
	
	PEG_METHOD_EXIT();

	return CIMObjectPath(String((LPCSTR)myStr));
 }

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::deleteInstance
//
// ///////////////////////////////////////////////////////////////////////////
void WMIInstanceProvider::deleteInstance(
	const String& nameSpace,
    const String& userName,
    const String& password,
	const CIMObjectPath& instanceName)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::deleteInstance()");

	CComPtr<IWbemServices> pServices;

	//Connect to namespace
	setup(nameSpace, userName, password);

	bool bConnected = _collector->Connect(&pServices);

	if (!bConnected) throw CIMException(CIM_ERR_ACCESS_DENIED);
	
	//Convert the parameters to make the WMI call
	CMyString str;
	String s = getObjectName(instanceName);
	str = s;

	CComBSTR bsInstanceName = str.Bstr();
	LONG lFlags = 0L;

	//Perform the WMI operation
	HRESULT hr = pServices->DeleteInstance(bsInstanceName,
										   lFlags,
										   NULL,
										   NULL);
	//Handle the WMI operation result
	if (FAILED(hr))
	{
		switch (hr)
		{
			case WBEM_E_ACCESS_DENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_FAILED: throw CIMException(CIM_ERR_FAILED); break;
			case WBEM_E_INVALID_PARAMETER: throw CIMException(CIM_ERR_FAILED, "WMI Invalid Parameter"); break;
			case WBEM_E_INVALID_CLASS: throw CIMException(CIM_ERR_INVALID_CLASS); break;
			case WBEM_E_NOT_FOUND: throw CIMException(CIM_ERR_NOT_FOUND); break;
			default: throw CIMException(CIM_ERR_FAILED);
		}
	}

	pServices.Release();

	PEG_METHOD_EXIT();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::getHostName
//
// ///////////////////////////////////////////////////////////////////////////
String WMIInstanceProvider::getHostName()
{
	DWORD nSize = 255;
    char hostName[256];

    // Get the computer name
	if(GetComputerName(hostName, &nSize))
		return String(hostName);
	
	throw CIMException(CIM_ERR_FAILED);
}

//////////////////////////////////////////////////////////////////////
// TERRY: ADDED: helper function, maybe should go in a utilities or base class?
//////////////////////////////////////////////////////////////////////
bool WMIInstanceProvider::GetKeyBindingsFromCIMInstance(const CIMInstance& newInstance, 
                                                        BSTR* pbsKeyBindings)
{
    // might check for any NULL keys, just returning success always for now:
    bool bSuccess = true;

    String relPath = getObjectName(newInstance.getPath());
    *pbsKeyBindings = SysAllocString((const OLECHAR*)relPath.getChar16Data());

    return bSuccess;
}

PEGASUS_NAMESPACE_END
