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
/// WMIClassProvider.cpp: implementation of the WMIClassProvider class.
//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::
//
// ///////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"

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
        const String& className,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList)
{

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIClassProvider::getClass()");

	CComPtr<IWbemClassObject>		pClass;

//	CIMClass cimClass;

	setup(nameSpace);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"getClass - localOnly %x, includeQualifiers %x, includeClassOrigin %x", 
		localOnly, includeQualifiers, includeClassOrigin);

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	if (!(_collector->getObject(&pClass, className)))
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}
	else if (_collector->isInstance(pClass))
	{
		throw CIMException(CIM_ERR_INVALID_PARAMETER);
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

	setup(nameSpace);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"enumerateClasses - deepInheritance %x, localOnly %x, includeQualifiers %x, includeClassOrigin %x", 
		deepInheritance, localOnly, includeQualifiers, includeClassOrigin);
	
	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// retrieve class enumeration object
	if (!(_collector->getClassEnum(&pClassEnum, className, deepInheritance)))
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

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
		throw CIMException(CIM_ERR_NOT_FOUND);
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

	setup(nameSpace);

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"enumerateClassNames - deepInheritance %x", deepInheritance);
	
	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// retrieve class enumeration object
	if (!(_collector->getClassEnum(&pClassEnum, className, deepInheritance)))
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

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
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	PEG_METHOD_EXIT();

	return classNames;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ATTN:
//	The following public methods have not been implemented
//	yet.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::deleteClass
//
// ///////////////////////////////////////////////////////////////////////////
void WMIClassProvider::deleteClass(const String &nameSpace, 
								   const String &className)
{
	throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::createClass
//
// ///////////////////////////////////////////////////////////////////////////
void WMIClassProvider::createClass(
								   const String & nameSpace,
								   const CIMClass& newClass)
{
	throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

/////////////////////////////////////////////////////////////////////////////
// WMIClassProvider::modifyClass
//
// ///////////////////////////////////////////////////////////////////////////
void WMIClassProvider::modifyClass(
								   const String & nameSpace,
								   const CIMClass& modifiedClass)
{
	throw CIMException(CIM_ERR_NOT_SUPPORTED);
}


PEGASUS_NAMESPACE_END

