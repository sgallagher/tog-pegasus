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
// Modified By:
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

#include "WMIProperty.h"
#include "WMIString.h"
#include "WMIValue.h"
#include "WMIQualifier.h"
#include "WMIQualifierSet.h"
#include "WMIType.h"
#include "WMIException.h"

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
        const CIMObjectPath& instanceName,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin ,
        const CIMPropertyList& propertyList)
{
	CComPtr<IWbemClassObject>		pInstance;

	String sClassName = String(instanceName.getClassName());
	String sInstanceName;

	sInstanceName = getObjectName(instanceName);

	CIMInstance cimInstance(sClassName);

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::getInstance()");

	setup(nameSpace);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"getInstance - localOnly %x, includeQualifiers %x, includeClassOrigin %x", 
		localOnly, includeQualifiers, includeClassOrigin);

	CMyString p;
	p = sClassName;
	CMyString q;
	q = nameSpace;
	CMyString r;
	r = sInstanceName;

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

	if (!_collector->getCIMInstance(pInstance, cimInstance,
									localOnly, includeQualifiers,
									includeClassOrigin, propertyList, 
									TRUE	// need key properties here
									))
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

	setup(nameSpace);

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
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	CIMClass cimclass = getCIMClass(
		nameSpace, 
		className);


	// Get the instances and append them to the array

	hr = pInstEnum->Next(WBEM_INFINITE, 1, &pInstance, &dwReturned);

	while (SUCCEEDED(hr) && (1 == dwReturned))
	{
		CIMInstance tempInst(className);
		if (_collector->getCIMInstance(pInstance, tempInst,
										localOnly, includeQualifiers,
										includeClassOrigin, propertyList, TRUE))
		{
			lCount++;

			CIMObjectPath tempRef = tempInst.buildPath(cimclass);
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
		throw CIMException(CIM_ERR_NOT_FOUND);
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
		const String& className)
{
	HRESULT hr;
	long lCount = 0;
	DWORD dwReturned;

	CComPtr<IEnumWbemClassObject>	pInstEnum;
	CComPtr<IWbemClassObject>		pInstance;

	Array<CIMObjectPath> instanceNames;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::enumerateInstanceNames()");

	setup(nameSpace);

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// retrieve the instance enumeration object
	if (!(_collector->getInstanceEnum(&pInstEnum, className, FALSE)))
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	CIMClass cimclass = getCIMClass(
		nameSpace, 
		className);


	// Get the names of the instances and send to handler

	hr = pInstEnum->Next(WBEM_INFINITE, 1, &pInstance, &dwReturned);

	while (SUCCEEDED(hr) && (1 == dwReturned))
	{
		CIMInstance tempInst(className);
		if (_collector->getCIMInstance(pInstance, tempInst, FALSE, FALSE,
										FALSE))
		{
			lCount++;

			CIMObjectPath tempRef = tempInst.buildPath(cimclass);

			tempRef.setNameSpace(nameSpace);
			instanceNames.append(tempRef);
		}

		pInstance.Release();
		hr = pInstEnum->Next(WBEM_INFINITE, 1, &pInstance, &dwReturned);
	}

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMIInstanceProvider::enumerateInstanceNames() - Instance count is %d", lCount); 

	if (lCount == 0)
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMIInstanceProvider::enumerateInstanceNames() - hResult value is %x", hr); 
		throw CIMException(CIM_ERR_NOT_FOUND);
	}
	PEG_METHOD_EXIT();

	return instanceNames;
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::getProperty
//
// ///////////////////////////////////////////////////////////////////////////
CIMValue WMIInstanceProvider::getProperty(
		const String & nameSpace,
		const CIMObjectPath & instanceName,
		const String & propertyName)
{

	CIMInstance cimInstance;
	Array<CIMName> propertyNames;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::getProperty()");

	setup(nameSpace);

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	CIMName propName = propertyName;


	propertyNames.append(propName);

	CIMPropertyList propertyList = CIMPropertyList(propertyNames);

	// get the relevant CIMInstance object
	cimInstance = getCIMInstance(nameSpace, instanceName, propertyList);

	// now fetch the property
	Uint32 pos = cimInstance.findProperty(propName);

	if (PEG_NOT_FOUND == pos)
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	CIMProperty property = cimInstance.getProperty(pos);

	// and return the value
	CIMValue value = property.getValue();

	PEG_METHOD_EXIT();

	return value;
}


///////////////////////////////////////////////////////////////////////////
//		private methods
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::setProperty
//
// ///////////////////////////////////////////////////////////////////////////
void WMIInstanceProvider::setProperty(
		const String & nameSpace,
		const CIMObjectPath & instanceName,
		const String & propertyName,
		const CIMValue& newValue)
{

	CComPtr<IWbemClassObject>		pInstance;
	CComVariant						vValue;
	CComBSTR						bsPropName;

	HRESULT hr;

	String sInstanceName;

	sInstanceName = getObjectName(instanceName);

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIInstanceProvider::setProperty()");

	setup(nameSpace);

	CMyString prop;
	prop = propertyName;

	CMyString q;
	q = sInstanceName;

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"setProperty() - setting property %s in %s", (LPCTSTR)prop, (LPCTSTR)q);

	if (!m_bInitialized)
	{
		throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Collector initialization failed.");
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

	// check the existing value and type
	bsPropName = prop.Bstr();

	vValue = WMIValue(newValue).toVariant();
	hr = pInstance->Put(bsPropName, 0, &vValue, 0);

	if (FAILED(hr))
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"setProperty() - Put failed, hr = %x", hr);

		throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "WMI Put property failed.");
	}

	throw CIMException(CIM_ERR_NOT_SUPPORTED);

	PEG_METHOD_EXIT();

}

/////////////////////////////////////////////////////////////////////////////////
// ATTN:
// The  following public methods are not yet implemented
//
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::modifyInstance
//
// ///////////////////////////////////////////////////////////////////////////
void WMIInstanceProvider::modifyInstance(
	const String & nameSpace,
	const CIMInstance& modifiedInstance,
	Boolean includeQualifiers,
	const CIMPropertyList & propertylist)
{
	throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::createInstance
//
// ///////////////////////////////////////////////////////////////////////////
CIMObjectPath WMIInstanceProvider::createInstance(
		const String & nameSpace,
		const CIMInstance & newInstance)
{
	throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

/////////////////////////////////////////////////////////////////////////////
// WMIInstanceProvider::deleteInstance
//
// ///////////////////////////////////////////////////////////////////////////
void WMIInstanceProvider::deleteInstance(
	const String & nameSpace,
	const CIMObjectPath & instanceName)
{
	throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

PEGASUS_NAMESPACE_END
