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
// Modified By:	Adriano Zanuz (adriano.zanuz@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// WMICollector::
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "WMIType.h"
#include "WMIFlavor.h"
#include "WMIValue.h"
#include "WMIString.h"
#include "WMIProperty.h"
#include "WMICollector.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
WMICollector::WMICollector(bool bLocal)
{
	m_bInitialized = false;
	m_bIsLocalNamespace = false;
	m_bImpersonate = false;
	m_bLocalConnection = bLocal;
}

WMICollector::~WMICollector(void)
{
}

void WMICollector::terminate(void)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::terminate()");

	if (m_bInitialized)
	{
		CoUninitialize();
		m_bInitialized = false;

		if(m_bImpersonate)
		{
			revertToSelf();
			m_bImpersonate = false;
		}
	}

	PEG_METHOD_EXIT();

}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::setup - calls CoInitialize stuff
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::setup()
{
	HRESULT hr;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::setup()");

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMICollector::setup() - return from CoInitializeEx() is %x", hr); 

	m_bInitialized = (SUCCEEDED(hr));

	hr = CoInitializeSecurity(NULL, -1, NULL, NULL,
							  RPC_C_AUTHN_LEVEL_DEFAULT,
							  RPC_C_IMP_LEVEL_IMPERSONATE,
							  NULL,
							  EOAC_NONE,
							  0);
	PEG_METHOD_EXIT();

	return (m_bInitialized);
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::Connect- attempt to establish a WMI connection with the local
//		host
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::Connect(IWbemServices **ppServices)
{

	CComPtr<IWbemLocator>	pLocator;
	CComPtr<IWbemServices>  pServices;
	HRESULT hr;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::Connect()");
	
	// get the Locator object
	hr = pLocator.CoCreateInstance(CLSID_WbemLocator);
	
	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMICollector::Connect() - return from CoCreateInstance() is %x", hr);

	if (SUCCEEDED(hr))	
	{
		if(m_bIsLocalNamespace)
		{
			
			//Does not impersonate if is being called from a client app
			//Impersonate if it is being called from the Mapper service
			if (!m_bLocalConnection)
				logonUser();
 						
			hr = pLocator->ConnectServer(m_bsNamespace, 
										 NULL, 
										 NULL,
										 NULL, 0L, NULL, NULL,
										 &pServices);
		}
		else
		{

			//---------------------------------------------------
			//by Jair
			//adjusting the username in order to add the domain
			//reference

			CMyString sUser, sAux;
			sAux = "\\";
			
			sUser = (LPWSTR)m_bsDomain.m_str;
			sUser += sAux;
			sAux = (LPWSTR)m_bsUserName.m_str;
			sUser += sAux;

			CComBSTR bsUser = (LPCTSTR)sUser;
			//---------------------------------------------------

			hr = pLocator->ConnectServer(m_bsNamespace, 
										 bsUser,
										 m_bsPassword,
										 NULL, 0L, NULL, NULL,
										 &pServices);  
		}

		pLocator.Release();

		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::Connect() - return from ConnectServer() is %x", hr);
		
		if (FAILED(hr))	
		{
			switch(hr)
			{
				case E_ACCESSDENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
				case WBEM_E_ACCESS_DENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
				default: throw CIMException(CIM_ERR_INVALID_NAMESPACE);
			}
		}
	}
	
	if (SUCCEEDED(hr))	
	{
		// set security attributes on pServices
		if(setProxySecurity(pServices))
		{
			pServices.CopyTo(ppServices);
		}
		else	
		{
			*ppServices = NULL;
		}
	}

	PEG_METHOD_EXIT();

	return (SUCCEEDED(hr));
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::getInstanceEnum - fetch a pointer to an enumeration of the
//		instances of sClassname
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::getInstanceEnum(
					 IEnumWbemClassObject **ppInstances,
					 const String & sClassName,
					 Boolean deepInheritance)
{

	HRESULT hr;
	long lFlags = WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY;

	CComPtr<IWbemServices>			pServices;
	CComPtr<IEnumWbemClassObject>	p_inst;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getInstanceEnum()");
	
	bool bConnected = Connect(&pServices);

	if (!bConnected)
	{
		throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, "Connect failed.");
	}

	CMyString p;
	p = sClassName;

	CComBSTR bsClassName = p.Bstr();

	deepInheritance ? lFlags |= WBEM_FLAG_DEEP : lFlags |= WBEM_FLAG_SHALLOW;

	// retrieve instances
	hr = pServices->CreateInstanceEnum(
		bsClassName,
		lFlags,
		NULL,
		&p_inst);

	if (SUCCEEDED(hr))	
	{
		p_inst.CopyTo(ppInstances);

		// set security attributes on *ppInstances
		bool bSecurity = setProxySecurity(*ppInstances);
	}

	else	
	{
		*ppInstances = NULL;
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::getInstanceEnum() - hr value is %x", hr); 

		switch(hr)
		{
			case WBEM_E_ACCESS_DENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_INVALID_CLASS: throw CIMException(CIM_ERR_INVALID_CLASS); break; 
			default: throw CIMException(CIM_ERR_FAILED); 
		}
	}

	PEG_METHOD_EXIT();

	pServices.Release();

	return (SUCCEEDED(hr));
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::getClassEnum - fetch a pointer to an enumeration of the
//		classes satisfying the criteria
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::getClassEnum(
					 IEnumWbemClassObject **ppClasses,
					 const String & sSuperClass,
					 Boolean deepInheritance)
{

	HRESULT hr;
	long lFlags = WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY;
	CComBSTR bsSuperClass = NULL;

	CComPtr<IWbemServices>	pServices;
	CComPtr<IEnumWbemClassObject>	p_class;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getClassEnum()");

	bool bConnected = Connect(&pServices);

	if (!bConnected)
	{
		throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, "Connect failed.");
	}

	//
	// workaround because there is no NULL for String class...
	//
	if (0 != sSuperClass.size())
	{
		CMyString p;
		p = sSuperClass;
		bsSuperClass = p;
	}

	deepInheritance ? lFlags |= WBEM_FLAG_DEEP : lFlags |= WBEM_FLAG_SHALLOW;

	// retrieve classes
	hr = pServices->CreateClassEnum(
		bsSuperClass,
		lFlags,
		NULL,
		&p_class);

	if (SUCCEEDED(hr))	
	{
		p_class.CopyTo(ppClasses);

		// set security attributes on *ppClasses
		bool bSecurity = setProxySecurity(*ppClasses);
	}
	else	
	{
		*ppClasses = NULL;
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::getClassEnum() - hr value is %x", hr);
		
		switch(hr)
		{
			case WBEM_E_ACCESS_DENIED: throw CIMException(CIM_ERR_ACCESS_DENIED); break;
			case WBEM_E_INVALID_CLASS: throw CIMException(CIM_ERR_INVALID_CLASS); break; 
			default: throw CIMException(CIM_ERR_FAILED); 
		}
	}

	PEG_METHOD_EXIT();

	pServices.Release();

	return (SUCCEEDED(hr));
}


/////////////////////////////////////////////////////////////////////////////
// WMICollector::getQueryResult - fetch a point to the enumeration of
//		instances returned from an ExecQuery request 
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::getQueryResult(
					IEnumWbemClassObject **ppInstances, 
					const String &query, 
					const String &queryLanguage)
{
	HRESULT hr;
	
	CComPtr<IWbemServices>	pServices;
	CComPtr<IEnumWbemClassObject> p_inst;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getQueryResult()");

	bool bConnected = Connect(&pServices);

	if (!bConnected)
	{
		throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, "Connect failed.");
	}

	CMyString q; q = query;
	CMyString ql; ql = queryLanguage;
	
	//if QueryLanguage is not WQL, throws an exception error informing
	//that the query language is not supported
	if (stricmp("WQL", (LPCSTR)ql))
	{
		throw CIMException(CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED);
	}

	CComBSTR bsQuery = q.Bstr();
	CComBSTR bsQueryLanguage = ql.Bstr();

	// retrieve query result
	hr = pServices->ExecQuery(
		bsQueryLanguage,
		bsQuery,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY | 
			WBEM_FLAG_ENSURE_LOCATABLE,
		NULL,
		&p_inst);

	if (SUCCEEDED(hr))	
	{
		p_inst.CopyTo(ppInstances);

		// set security attributes on *ppInstances
		bool bSecurity = setProxySecurity(*ppInstances);
	}
	else	
	{
		*ppInstances = NULL;
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::getQueryResult() - hr value is %x", hr); 

		switch(hr)
		{
			case WBEM_E_INVALID_QUERY: throw CIMException(CIM_ERR_INVALID_QUERY); break;
			case WBEM_E_INVALID_QUERY_TYPE: throw CIMException(CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED); break;
			default: throw CIMException(CIM_ERR_FAILED); 
		}
	}

	PEG_METHOD_EXIT();

	pServices.Release();

	return (SUCCEEDED(hr));
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::getObject - fetch a pointer to the object
//		represented by sObjectName
//		Can be either a class or an instance
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::getObject(IWbemClassObject **ppObject, 
								  const String & sObjectName)
{
	HRESULT hr;
	bool bSucceeded;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getObject()");
	
	CComPtr<IWbemServices>	pServices;
	CComPtr<IWbemClassObject>	p_obj;

	bool bConnected = Connect(&pServices);

	if (!bConnected)
	{
		throw CIMException(CIM_ERR_ACCESS_DENIED);
	}

	CMyString s;
	s = sObjectName;
	CComBSTR bsObjectName = s.Bstr();
	LONG lFlags = WBEM_FLAG_USE_AMENDED_QUALIFIERS | WBEM_FLAG_RETURN_WBEM_COMPLETE; 

	// retrieve class object
	hr = pServices->GetObject(
		bsObjectName, lFlags, 
		NULL, &p_obj, NULL);

	bSucceeded = (SUCCEEDED(hr));

	if (SUCCEEDED(hr))
	{
		p_obj.CopyTo(ppObject);

		// set security attributes on result
		bool bSecurity = setProxySecurity(*ppObject);
	}
	else
	{
		*ppObject = NULL;

		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::getObject() - GetObject result is %x", hr); 

		//generate error
		switch(hr)
		{
			case WBEM_E_INVALID_CLASS: throw CIMException(CIM_ERR_INVALID_CLASS); break;
			case WBEM_E_NOT_FOUND: throw CIMException(CIM_ERR_NOT_FOUND); break;
			default: throw CIMException(CIM_ERR_FAILED); 
		}
	}

	PEG_METHOD_EXIT();
	
	pServices.Release();
	
	return (bSucceeded);
}


/////////////////////////////////////////////////////////////////////////////
// getProperties - retrieves selected 
//		properties for a CIMClass or
//		CIMInstance object
//
/////////////////////////////////////////////////////////////////////////////
template<class CONTAINER>
void getProperties(IWbemClassObject *pClass, 
					Boolean localOnly,
					Boolean includeQualifiers,
					Boolean includeClassOrigin,
					const CIMPropertyList& propertyList,								  
					CONTAINER & container)
{
	HRESULT hr = S_OK;
	CMyString sMessage;

	CComBSTR	bsName;			// of the property
	CComVariant	vValue;			// of the property
	long		lFlavor;		// of the property
	CIMTYPE		type;
	CIMProperty property;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"getProperties()");

	Uint32 size = propertyList.size();

	for (Uint32 i = 0; ((i < size) && SUCCEEDED(hr)); i++)
	{
		String sPropName = String(propertyList[i]);
		CMyString sName;

		sName = sPropName;

		bsName.Empty();
		vValue.Clear();
		sMessage = "Get()";

		bsName = sName.Bstr();

		// fetch the property
		hr = pClass->Get(bsName, 0, &vValue, &type, &lFlavor);

		// process each property
		if (SUCCEEDED(hr))
		{

			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"getProperties() - CIMTYPE[%x] - WMITYPE[%x]",
				type, 
				vValue.vt);

			bool bPropagated = (lFlavor & WBEM_FLAVOR_ORIGIN_PROPAGATED) ? true : false;

			try
			{
				property = WMICollector::getProperty(pClass, bsName, vValue, 
										type, includeClassOrigin, 
										includeQualifiers, bPropagated);
			}
			catch( TypeMismatchException & e )
			{
				// ATTN:
				// unsupported for now - do some tracing...
				String s = e.getMessage();
				sMessage = s;

				String sClass = WMICollector::getClassName(pClass);
				CMyString sCls;
				sCls = sClass;
				Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
					"getProperties() - Ignoring invalid type for %s in %s.  %s, unsupported WMI CIM type is %x",
					(LPCTSTR)sName, (LPCTSTR)sCls, 
					(LPCTSTR)sMessage, type);

				continue;
			}
			
			if (bPropagated && localOnly)
			{
				Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
					"getProperties() - Property %s was defined in a superclass", sPropName);
			}

			else
			// try to add it
			{
				try
				{
					container.addProperty(property);
				}
				catch( AlreadyExistsException& e )
				{
					// ignore this
					String s = e.getMessage();
					sMessage = s;
					Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
						"getProperties() - Property %s is already defined. %s", sPropName, (LPCTSTR)sMessage);
				}
				catch( Exception & e )
				{
					// ignore this
					String s = e.getMessage();
					sMessage = s;
					Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
						"getAllProperties() - Ignoring AddedReferenceToClass.  %s", (LPCTSTR)sMessage);

				}
				catch(... ) 
				{
					throw CIMException(CIM_ERR_FAILED);
				}
			}

		}
		else if (WBEM_E_NOT_FOUND == hr)
		{	// we are supposed to keep going...
			sMessage = bsName;
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"getProperties() - %s property not found", (LPCTSTR)sMessage);

			//reset
			hr = S_OK;
		}
	}

	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"getProperties() - %s result is %x", (LPCTSTR)sMessage, hr);
		throw CIMException(CIM_ERR_FAILED);
	}

	PEG_METHOD_EXIT();
}

/////////////////////////////////////////////////////////////////////////////
// getAllProperties - retrieves all the 
//		properties for a CIMClass or
//		CIMInstance object
//
/////////////////////////////////////////////////////////////////////////////
template<class CONTAINER>
bool getAllProperties(IWbemClassObject *pClass, 
									  long lFlags,
									  Boolean includeQualifiers,
									  Boolean includeClassOrigin,
									  CONTAINER & container)
{
	HRESULT hr;
	CMyString sMessage;

	CComBSTR	bsName;			// of the property
	CComVariant	vValue;			// of the property
	long		lFlavor;		// of the property
	CIMTYPE		type;
	CIMProperty property;

	bool bFound = false;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"getAllProperties()");

	hr = pClass->BeginEnumeration(lFlags);
	sMessage = "BeginEnumeration()";

	if (SUCCEEDED(hr))
	{
		bsName.Empty();
		vValue.Clear();
		sMessage = "Next()";

		hr = pClass->Next(0, &bsName, &vValue, &type, &lFlavor);
	}

	// process each property
	while (SUCCEEDED(hr))
	{
		if (WBEM_S_NO_MORE_DATA == hr)
		{
			break;
		}

		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"getAllProperties() - CIMTYPE[%x] - WMITYPE[%x]",
				type, 
				vValue.vt);

		bFound = true;

		CMyString	sPropName;
		sPropName = bsName;
		CMyString	sMessage;

		bool bPropagated = (lFlavor & WBEM_FLAVOR_ORIGIN_PROPAGATED) ? true : false;

		try
		{
			property = WMICollector::getProperty(pClass, bsName, vValue, 
									type, includeClassOrigin, 
									includeQualifiers, bPropagated);
		}
		catch( TypeMismatchException & e )
		{
			// ATTN:
			// unsupported for now - do some tracing...
			String s = e.getMessage();
			sMessage = s;

			String sClass = WMICollector::getClassName(pClass);
			CMyString sCls;
			sCls = sClass;
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"getAllProperties() - Ignoring invalid type for %s in %s.  %s, unsupported WMI CIM type is %x",
				(LPCTSTR)sPropName, (LPCTSTR)sCls, 
				(LPCTSTR)sMessage, type);

			bsName.Empty();
			vValue.Clear();

			sMessage = "Next()";
			hr = pClass->Next(0, &bsName, &vValue, &type, &lFlavor);

			continue;
		}			
		try
		{
			container.addProperty(property);
		}
		catch( AlreadyExistsException& e )
		{
			// ignore this
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"getAllProperties() - Property %s is already defined", (LPCTSTR)sPropName);
			String s = e.getMessage();
			sMessage = s;
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"getAllProperties() - %s", (LPCTSTR)sMessage);

		}
		catch( Exception & e )
		{
			// ignore this
			String s = e.getMessage();
			sMessage = s;
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"getAllProperties() - Ignoring AddedReferenceToClass.  %s", (LPCTSTR)sMessage);

		}
		catch(... ) 
		{
			throw CIMException(CIM_ERR_FAILED);
		}

		bsName.Empty();
		vValue.Clear();

		sMessage = "Next()";
		hr = pClass->Next(0, &bsName, &vValue, &type, &lFlavor);
	}

	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"getAllProperties() - %s result is %x", (LPCTSTR)sMessage, hr);
		throw CIMException(CIM_ERR_FAILED);
	}
	else
	{
		pClass->EndEnumeration();
	}

	PEG_METHOD_EXIT();

	return bFound;
}

/////////////////////////////////////////////////////////////////////////////
// getClassQualifiers - retrieves the
//		class qualifier definitions for a
//		class or instance and adds
//		them to the CIMClass or CIMInstance
//
/////////////////////////////////////////////////////////////////////////////
template<class CONTAINER>
void getClassQualifiers(IWbemClassObject *pClass, 
						CONTAINER & container)
{
	HRESULT hr;
	CMyString sMessage;

	CComPtr<IWbemQualifierSet>	pQualifiers;

	CComBSTR	bsName;			// of the qualifier
	CComVariant	vValue;			// of the qualifier
	long		lFlavor;		// of the qualifier
	bool		bPropagated;		// true if propated from a superclass
	CIMQualifier	qualifier;
	
 	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"getClassQualifiers()");

	// get the qualifiers enumerator
	hr = pClass->GetQualifierSet( & pQualifiers);

	sMessage = "GetQualifier()";

	if (SUCCEEDED(hr))
	{
		hr = pQualifiers->BeginEnumeration(0);
		sMessage = "BeginEnumeration()";
	}

	if (SUCCEEDED(hr))
	{
		bsName.Empty();
		vValue.Clear();
		sMessage = "Next()";

		hr = pQualifiers->Next(0, &bsName, &vValue, &lFlavor);
	}

	// process each qualifier
	while (SUCCEEDED(hr))
	{
		if (WBEM_S_NO_MORE_DATA == hr)
		{
			break;
		}
		
		bPropagated = (lFlavor & WBEM_FLAVOR_ORIGIN_PROPAGATED) ? true : false;

		qualifier = CIMQualifier(WMIString(bsName), WMIValue(vValue), 
							WMIFlavor(lFlavor), bPropagated);
		
		try
		{
			container.addQualifier(qualifier);
		}
		catch (...)
		{
			throw CIMException(CIM_ERR_FAILED);
		}

		bsName.Empty();
		vValue.Clear();

		hr = pQualifiers->Next(0, &bsName, &vValue, &lFlavor);
	}


	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"getClassQualifiers() - %s result is %x", (LPCTSTR)sMessage, hr);
		throw CIMException(CIM_ERR_FAILED);
	}
	else
	{
		pQualifiers->EndEnumeration();
	}

	PEG_METHOD_EXIT();

}

////////////////////////////////////////////////////////////////////////////
// WMICollector::getCIMObject - set up a getCIMObject structure
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::getCIMObject(IWbemClassObject *pObject, 
							CIMObject & cimObj,
							Boolean localOnly,
							Boolean includeQualifiers,
							Boolean includeClassOrigin,
							const CIMPropertyList& propertyList,
							Boolean getKeyProperties)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getCIMObject()");

	// first get the qualifiers if wanted
	if (includeQualifiers)
	{
		getClassQualifiers(pObject, cimObj);
	}

	// then the properties
	getObjectProperties(pObject, cimObj, localOnly, 
										includeQualifiers, includeClassOrigin, 
										propertyList, getKeyProperties);

	PEG_METHOD_EXIT();
	return true;

}

////////////////////////////////////////////////////////////////////////////
// WMICollector::getCIMInstance - set up a CIMInstance structure
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::getCIMInstance(IWbemClassObject *pObject, 
							CIMInstance & cimInst,
							Boolean localOnly,
							Boolean includeQualifiers,
							Boolean includeClassOrigin,
							const CIMPropertyList& propertyList,
							Boolean getKeyProperties)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getCIMInstance()");

	CIMObject cimObj = cimInst;

	getCIMObject(pObject, cimObj, localOnly, includeQualifiers,
					includeClassOrigin, propertyList, getKeyProperties);

	CIMInstance newInstance(cimObj);
	cimInst = newInstance;

	PEG_METHOD_EXIT();
	return true;

}


/////////////////////////////////////////////////////////////////////////////
// WMICollector::getCIMClass - set up a 
//		CIMClass structure
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::getCIMClass(IWbemClassObject *pObject,
		CIMClass & cimClass,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getCIMClass()");

	CIMObject cimObj = cimClass;

	getCIMObject(pObject, cimObj, localOnly, includeQualifiers,
					includeClassOrigin, propertyList);

	CIMClass newClass(cimObj);
	cimClass = newClass;

	PEG_METHOD_EXIT();
	return true;

}


/////////////////////////////////////////////////////////////////////////////
// WMICollector::getClassName - return the value of the __CLASS
//		property of pObject 
//
/////////////////////////////////////////////////////////////////////////////
String WMICollector::getClassName(IWbemClassObject *pObject)
{ 
	CComBSTR bsClass = "__CLASS";

	return getStringProperty(pObject, bsClass);

}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::getRelativePath - return the value of the __RELPATH
//		property of pClass
//
/////////////////////////////////////////////////////////////////////////////
String WMICollector::getRelativePath(IWbemClassObject *pObject)
{
	CComBSTR bsRelPath = "__RELPATH";

	return getStringProperty(pObject, bsRelPath);
	
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::getSuperClass - return the value of the __SUPERCLASS
//		property of pClass
//
/////////////////////////////////////////////////////////////////////////////
String WMICollector::getSuperClass(IWbemClassObject *pClass)
{
	CComBSTR bsSuperClass = "__SUPERCLASS";

	return getStringProperty(pClass, bsSuperClass);
	
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::isInstance - returns true if pObject represents a
//		WMI instance - false if it is a class
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::isInstance(IWbemClassObject *pObject)
{
	String sClass = getClassName(pObject);
	String sRelPath = getRelativePath(pObject);

	return (!String::equal(sClass, sRelPath));
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::getStringProperty - helper function to retrieve specific
//		string properties from a class or instance object 
//
/////////////////////////////////////////////////////////////////////////////
String WMICollector::getStringProperty(IWbemClassObject *pObject, 
									   const CComBSTR &bsPropertyName)
{

	HRESULT hr;

	CComVariant	vValue;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getStringProperty()");

	CComBSTR bs;
	CMyString s;

	s = "";

	CMyString name;
	name = bsPropertyName;

	hr = pObject->Get(bsPropertyName, 0, &vValue, NULL, NULL);
	if (SUCCEEDED(hr))
	{
		if (VT_NULL != vValue.vt)
		{	// there is a value... if not we will return an empty string...
			bs = vValue.bstrVal;

			if (0 != bs.Length())
			{
				s = bs;
			}
		}

		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::getStringProperty() - Value for %s is %s", 
			(LPCTSTR)name, (LPCTSTR)s);
	}
	else
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::getStringProperty() - get result for %s is %x", 
			(LPCTSTR)name, hr);
	}

	PEG_METHOD_EXIT();

	if (VT_NULL == vValue.vt)
	{
		return String::EMPTY;
	}
	else
	{
		return String((LPCTSTR)s);
	}
	
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::getObjectProperties - retrieves the 
//		object property definitions and values, if any,
//		and adds them to the CIMObject instance
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::getObjectProperties(IWbemClassObject *pObject, 
									  CIMObject & cimObj,
									  Boolean localOnly,
									  Boolean includeQualifiers,
									  Boolean includeClassOrigin,
									  const CIMPropertyList& propertyList,
									  Boolean bGetKeyProperties)
{
 	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getObjectProperties()");

	long lFlags = (localOnly) ? WBEM_FLAG_LOCAL_ONLY : WBEM_FLAG_NONSYSTEM_ONLY;

	if (propertyList.isNull())
	{	// we want all the properties...
		getAllProperties(pObject, lFlags, 
						includeQualifiers, includeClassOrigin, 
						cimObj);
	}

	else if (0 != propertyList.size())
	{	// just get the ones requested
		getProperties(pObject, localOnly, includeQualifiers, 
						includeClassOrigin, propertyList, cimObj);
	}

	// else we have an empty list and don't want any

	// if being called from getInstance, need to be sure that 
	//	the key properties are retrieved...
	if (bGetKeyProperties)
	{
		getAllProperties(pObject, WBEM_FLAG_KEYS_ONLY,
						includeQualifiers, includeClassOrigin, 
						cimObj);
	}

	PEG_METHOD_EXIT();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::getProperty
//		create a CIMProperty object from
//		WMI data 
//
/////////////////////////////////////////////////////////////////////////////
CIMProperty WMICollector::getProperty(IWbemClassObject *pClass, const CComBSTR &bsName, 
									  const CComVariant &vValue, CIMTYPE type,
									  Boolean includeClassOrigin, Boolean includeQualifiers,
									  Boolean bPropagated)
{
	HRESULT hr = S_OK;
	CMyString sMessage;
	CIMProperty property;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMICollector::getProperty()");

	CMyString sPropName;
	sPropName = bsName;
	long iSize = sPropName.GetLength();

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMICollector::getProperty() - Property Name is %s, type is %x, CIMTYPE is %x", 
		(LPCTSTR)sPropName, vValue.vt, type);

	// put this in to check XP - @bbp
	if (VT_BSTR == vValue.vt)
	{
		CComBSTR bs;
		bs.Empty();
		bs.Append(vValue.bstrVal);
		sPropName = bs;
		iSize = sPropName.GetLength();
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::getProperty() - Property Value is %s, size is %x", 
			(LPCTSTR)sPropName, iSize);
	}

	CComPtr<IWbemQualifierSet> pQualifiers;

	// get the qualifier set for the properties
	sMessage = "GetPropertyQualifierSet()";
	hr = pClass->GetPropertyQualifierSet(bsName, &pQualifiers);

	if (SUCCEEDED(hr))
	{
		property = WMIProperty(bsName, vValue, type, pQualifiers, includeQualifiers);

		// set the property qualifier
		property.setPropagated(bPropagated);
	}

	// check if requested ClassOrigin qualifier
	// ATTN:
	// This is a bug in Pegasus.  It will put it
	//	the current class for GetClass if it isn't
	//	there.
	if ((includeClassOrigin /*|| bPropagated*/) && SUCCEEDED(hr))
	{
		CComBSTR		bsClassOrigin;
		sMessage = "GetPropertyOrigin()";

		hr = pClass->GetPropertyOrigin(bsName, & bsClassOrigin);

		if (SUCCEEDED(hr))
		{
			property.setClassOrigin(WMIString(bsClassOrigin));
		}
	}

	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::getProperty() - %s result is %x", (LPCTSTR)sMessage, hr);
		throw CIMException(CIM_ERR_FAILED);
	}

	PEG_METHOD_EXIT();
	return property;
}


/////////////////////////////////////////////////////////////////////////////
// WMICollector::setNamespace
//	 
//
/////////////////////////////////////////////////////////////////////////////
void WMICollector::setNamespace(const String & sNamespace)
{
	CMyString s; s = sNamespace;
	CMyString sName;

	String sLower = sNamespace;
	sLower.toLower();

	String first = "root";
	Uint32 pos = sLower.find(first);
	
	m_bIsLocalNamespace = (0 == pos);

	if (m_bIsLocalNamespace)
	{
		CMyString sFormat;
		sFormat.Format("Namespace %s is local", 40, s);
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::setNamespace() - %s", (LPCTSTR)sFormat);
		sName = s;
	}
	else
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMICollector::setNamespace() - Namespace %s is remote", (LPCTSTR)s);

		// by Jair 
		// adjust the namespace to accept DNS fully qualified names
		// and IP addresses.
		String str = sNamespace;
		for (Uint32 i = 0; i < pos - 1; i++)
		{
			if (str[i] == '/') 
			{
				str[i] = '.';
			}
			else if (str[i] == '_')
			{
				if ((str[i + 1] >= '0') && 
					(str[i + 1] <= '9'))
				{
					if ((i == 0) || (str[i - 1] == '.'))
					{
						str.remove(i, 1);
						pos--;
					}
				}
			}
		}

		//prepend "//" to namespace
		sName = "//";
		s = str;
		sName += s;

	}

	m_bsNamespace = (LPCTSTR)sName;
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::setUserName
//	 
//
/////////////////////////////////////////////////////////////////////////////
void WMICollector::setUserName(const String & sUserName)
{
	CMyString s;
	s = sUserName;

	LPTSTR pszUserName = (LPTSTR)(LPCTSTR)s;
	LPTSTR pszDomain = ".";
	
	//Check if there is a slash or a backslash in the
	//username and then separate the domain and the username
	bool bSlashInStr = false;
	LPTSTR psz = pszUserName;
	while ( *psz )
	{
		if ( (*psz == '\\') || (*psz == '/') )
			bSlashInStr = true;
		psz++;
	}

	if(bSlashInStr)
	{
		pszDomain = pszUserName;
		while ( (*pszUserName != '\\') && (*pszUserName != '/') )
			++pszUserName;
		*pszUserName++ = '\0'; // terminate domain name
	}

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"WMICollector::setUserName() - UserName %s, Domain %s", pszUserName, pszDomain);

	m_bsUserName = pszUserName;
	m_bsDomain = pszDomain;
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::setPassword
//	 
//
// ///////////////////////////////////////////////////////////////////////////
void WMICollector::setPassword(const String & sPassword)
{
	CMyString s;
	s = sPassword;

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"WMICollector::setPassword() - Password %s", (LPCTSTR)s);

	m_bsPassword = (LPCTSTR)s;
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::setProxySecurity
//	 
//
/////////////////////////////////////////////////////////////////////////////
bool WMICollector::setProxySecurity(IUnknown * pProxy)
{
	HRESULT hr;

	if(m_bIsLocalNamespace)
	{
		hr = CoSetProxyBlanket(
			pProxy,
			RPC_C_AUTHN_WINNT,    // NTLM authentication service
			RPC_C_AUTHZ_NONE,     // default authorization service...
			NULL,                 // no mutual authentication
			RPC_C_AUTHN_LEVEL_PKT,      // authentication Tracer::LEVEL
			RPC_C_IMP_LEVEL_IMPERSONATE, // impersonation Tracer::LEVEL
			NULL,                
			EOAC_NONE);          // no special capabilities
	}
	else
	{
		// set security attributes on pProxy
		COAUTHIDENTITY authident;
			
		memset((void *)&authident,0,sizeof(COAUTHIDENTITY));

		authident.UserLength = (ULONG)m_bsUserName.Length();
		authident.User = (LPWSTR)m_bsUserName.m_str;
		authident.PasswordLength = (ULONG)m_bsPassword.Length();
		authident.Password = (LPWSTR)m_bsPassword.m_str;
		authident.DomainLength = (ULONG)m_bsDomain.Length();
		authident.Domain = (LPWSTR)m_bsDomain.m_str;
		authident.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;

		hr = CoSetProxyBlanket(
				pProxy,
				RPC_C_AUTHN_WINNT,    // NTLM authentication service
				RPC_C_AUTHZ_NONE,     // default authorization service...
				NULL,                 // no mutual authentication
				RPC_C_AUTHN_LEVEL_PKT,      // authentication Tracer::LEVEL
				RPC_C_IMP_LEVEL_IMPERSONATE,    // impersonation Tracer::LEVEL
				&authident, 
				EOAC_NONE);          // no special capabilities
	}
	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::logonUser
//	 
//
/////////////////////////////////////////////////////////////////////////////
void WMICollector::logonUser()
{
	CMyString sUserName, sDomain, sPassword;

	sUserName = (LPWSTR)m_bsUserName.m_str;
	sDomain   = (LPWSTR)m_bsDomain.m_str;
	sPassword = (LPWSTR)m_bsPassword.m_str;

	LPTSTR pszUserName  = (LPTSTR)(LPCTSTR)sUserName;
	LPTSTR pszDomain    = (LPTSTR)(LPCTSTR)sDomain;
	LPTSTR pszPassword  = (LPTSTR)(LPCTSTR)sPassword;

    //Logon and impersonate the user
	if(!RevertToSelf())
	{
		DWORD error = GetLastError();
		
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"WMICollector::logonUser() - return from RevertToSelf() is %d", error);

		throw CIMException(CIM_ERR_ACCESS_DENIED, "RevertToSelf()");
	}

	HANDLE htok = 0;
	if(!LogonUser(pszUserName,
		          pszDomain,
				  pszPassword,
				  LOGON32_LOGON_INTERACTIVE,
				  LOGON32_PROVIDER_DEFAULT,
				  &htok))
	{
		DWORD error = GetLastError();
	
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"WMICollector::logonUser() - return from LogonUser() is %d", error);
				
		throw CIMException(CIM_ERR_ACCESS_DENIED, "LogonUser()");
	}

	if(!ImpersonateLoggedOnUser(htok))
	{
		DWORD error = GetLastError();

		throw CIMException(CIM_ERR_ACCESS_DENIED, "ImpersonateLoggedOnUser()");
	}

	m_bImpersonate = true;
}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::revertToSelf
//	 
//
/////////////////////////////////////////////////////////////////////////////
void WMICollector::revertToSelf()
{
	if(!RevertToSelf())
	{
		throw CIMException(CIM_ERR_FAILED, "RevertToSelf()");
	}
}

PEGASUS_NAMESPACE_END