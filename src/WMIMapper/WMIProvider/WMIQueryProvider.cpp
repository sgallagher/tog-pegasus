//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By: Jair Santos, Hewlett-Packard Company (jair.santos@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

// WMIQueryProvider.cpp: implementation of the WMIQueryProvider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include "WMICollector.h"
#include "WMIBaseProvider.h"
#include "WMIClassProvider.h"
#include "WMIInstanceProvider.h"
#include "WMIAssociatorProvider.h"
#include "WMIQueryProvider.h"

#include "WMIProperty.h"
#include "WMIString.h"
#include "WMIValue.h"
#include "WMIQualifier.h"
#include "WMIQualifierSet.h"
#include "WMIType.h"
#include "WMIException.h"

//////////////////////////////////////////////////////////////////////////////
// WMIQueryProvider::
//
// ///////////////////////////////////////////////////////////////////////////
PEGASUS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WMIQueryProvider::WMIQueryProvider()
{
	_collector = NULL;
	m_bInitialized = false;

}

WMIQueryProvider::~WMIQueryProvider()
{
	cleanup();

}

/////////////////////////////////////////////////////////////////////////////
// WMIQueryProvider::execQuery
//
// ///////////////////////////////////////////////////////////////////////////
Array<CIMObject> WMIQueryProvider::execQuery(
		const String& nameSpace,
		const String& userName,
		const String& password,
        const String& queryLanguage,
        const String& query,
		const CIMPropertyList& propertyList,
		Boolean includeQualifiers,
		Boolean includeClassOrigin)
{
	HRESULT hr;
	long lCount = 0;
	DWORD dwReturned;
	bool bInst;

	CComPtr<IEnumWbemClassObject>	pObjEnum;
	CComPtr<IWbemClassObject>		pObject;

	Array<CIMObject> objects;
	CIMClass cimclass;
	String className;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIQueryProvider::execQuery()");

	setup(nameSpace,userName,password);

	if (!m_bInitialized)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

	// retrieve results
	if (!(_collector->getQueryResult(&pObjEnum, query, queryLanguage)))
	{
		if (pObjEnum)
			pObjEnum.Release();

		throw CIMException(CIM_ERR_FAILED);
	}

	//set proxy security on pObjEnum
	bool bSecurity = _collector->setProxySecurity(pObjEnum);
	
	//get the results and append them to the array
	hr = pObjEnum->Next(WBEM_INFINITE, 1, &pObject, &dwReturned);
	
	if (SUCCEEDED(hr) && (1 == dwReturned))
	{
		bInst = _collector->isInstance(pObject);
	}

	
	while (SUCCEEDED(hr) && (1 == dwReturned))
	{
		CIMObject cimObj;
		String className2 = _collector->getClassName(pObject);

		if (!(String::equalNoCase(className, className2)))
		{	// first time or
			// changed classes on us, have to get new data...
			// shouldn't happen very often (I hope!)
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
						  "WMIInstanceProvider::execQuery() - classname changed from %s to %s",
						  className.getCString(), className2.getCString());

			className = className2;

			if (bInst)
			{
				cimclass = getCIMClass(
					nameSpace,
					userName,
					password,
					className);
			}
		}

		// now collect the information
		if (bInst)
		{
			CIMInstance tempInst(className);

			if (_collector->getCIMInstance(pObject, tempInst,
											false, includeQualifiers, includeClassOrigin,
											propertyList, 
											true))  //get key properties
			{
				lCount++;

				CIMObjectPath tempRef = tempInst.buildPath(cimclass);
				tempInst.setPath(tempRef);

				objects.append(CIMObject(tempInst));
			}
		}
		else
		{
			// we are collecting a class
			String superClass = _collector->getSuperClass(pObject);
			CIMName objName = className;

			cimclass = CIMClass(objName);

			if (0 != superClass.size())
			{
				CIMName superClassName = superClass;
				cimclass.setSuperClassName(superClassName);
			}

			cimObj = CIMObject(cimclass);

			if (_collector->getCIMObject(pObject, 
				                         cimObj,
										 false, 
										 includeQualifiers, 
										 includeClassOrigin,
										 propertyList))
			{
				lCount++;
				objects.append(cimObj);
			}
		}

		if (pObject)
			pObject.Release();

		hr = pObjEnum->Next(WBEM_INFINITE, 1, &pObject, &dwReturned);
	}

	if (pObjEnum)
		pObjEnum.Release();

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMIQueryProvider::execQuery() - Result count is %d", lCount); 

	if (lCount == 0)
	{
		Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
			"WMIQueryProvider::execQuery() - hResult value is %x", hr); 
	}

	PEG_METHOD_EXIT();

	return objects;
}

PEGASUS_NAMESPACE_END

