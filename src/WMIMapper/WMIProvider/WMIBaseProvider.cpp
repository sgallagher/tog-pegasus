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
// Modified By:	 Adriano Zanuz (adriano.zanuz@hp.com)
//               Jair Santos, Hewlett-Packard Company (jair.santos@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider.cpp: implementation of the WMIBaseProvider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WMICollector.h"
#include "WMIBaseProvider.h"
#include "WMIClassProvider.h"
#include "WMIInstanceProvider.h"
#include "WMIQueryProvider.h"

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

WMIBaseProvider::WMIBaseProvider()
{
}

WMIBaseProvider::~WMIBaseProvider()
{
}

/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::initialize
//
// ///////////////////////////////////////////////////////////////////////////
void WMIBaseProvider::initialize(bool bLocal)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::initialize()");

	initCollector(bLocal);

	PEG_METHOD_EXIT();
}

/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::terminate
//
// ///////////////////////////////////////////////////////////////////////////
void WMIBaseProvider::terminate(void)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::terminate()");
	
	cleanup();

	PEG_METHOD_EXIT();
}


/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::setup
//
/////////////////////////////////////////////////////////////////////////////
void WMIBaseProvider::setup(const String & nameSpace,
							const String & userName,
							const String & password)
{
	m_sNamespace = nameSpace;
	m_sUserName = userName;
	m_sPassword = password;

	if (!m_bInitialized)	
	{
		initCollector();
	}

	if (m_bInitialized)
	{
		_collector->setNamespace(m_sNamespace);

		if (m_sUserName != String::EMPTY)
			_collector->setUserName(m_sUserName);
		
		if (m_sPassword != String::EMPTY)
			_collector->setPassword(m_sPassword);
	}
}

/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::initCollector
//
// ///////////////////////////////////////////////////////////////////////////
void WMIBaseProvider::initCollector(bool bLocal)
{

	if (!m_bInitialized)
	{
		_collector = new WMICollector(bLocal);
		m_bInitialized = _collector->setup();
	}

}

/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::cleanup
//
// ///////////////////////////////////////////////////////////////////////////
void WMIBaseProvider::cleanup()
{
	if (m_bInitialized)
	{
		_collector->terminate();
		delete _collector;
		_collector = NULL;
		m_bInitialized = false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::getCIMInstance - retrieves a CIMInstance object
//
// ///////////////////////////////////////////////////////////////////////////
CIMInstance WMIBaseProvider::getCIMInstance(const String& nameSpace,
												const String& userName,
												const String& password,
												const CIMObjectPath &instanceName, 
												const CIMPropertyList &propertyList)
{

	CIMInstance cimInstance;
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	WMIInstanceProvider provider;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::getCIMInstance()");

	try
	{
		provider.initialize(true);

		cimInstance = provider.getInstance(nameSpace, 
										   userName, 
										   password, 
										   instanceName, 
										   false, 
										   false, 
										   false, 
										   propertyList);
		provider.terminate();
	}
	catch(CIMException& exception)
	{
		provider.terminate();
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(Exception& exception)
	{
		provider.terminate();
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(...)
	{
		provider.terminate();
		throw CIMException(CIM_ERR_FAILED);
	}

    PEG_METHOD_EXIT();

	return cimInstance;
}


/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::getCIMClass - retrieves a CIMClass object
//
// ///////////////////////////////////////////////////////////////////////////
CIMClass WMIBaseProvider::getCIMClass(const String& nameSpace,
										const String& userName,
										const String& password,
										const String& className,
										const CIMPropertyList &propertyList)
{
	CIMClass cimClass;
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	WMIClassProvider provider;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::getCIMClass()");

	try
	{
		provider.initialize(true);

		cimClass = provider.getClass(nameSpace, userName, password, className, false, true, true, propertyList);

		provider.terminate();
	}
	catch(CIMException& exception)
	{
		provider.terminate();
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(Exception& exception)
	{
		provider.terminate();
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(...)
	{
		provider.terminate();
		throw CIMException(CIM_ERR_FAILED);
	}

    PEG_METHOD_EXIT();

	return cimClass;
}

/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::execCIMQuery - retrieves a query result
//
// ///////////////////////////////////////////////////////////////////////////
Array<CIMObject> WMIBaseProvider::execCIMQuery(
	const String& nameSpace,
	const String& userName,
	const String& password,
    const String& queryLanguage,
    const String& query,
	const CIMPropertyList& propertyList,
	Boolean includeQualifiers,
	Boolean includeClassOrigin)
{
	Array<CIMObject> objects;

	CIMInstance cimInstance;
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	WMIQueryProvider provider;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::execCIMQuery()");

	try
	{
		provider.initialize(true);

		objects = provider.execQuery(nameSpace,
					userName,
					password,
					queryLanguage,
					query,
					propertyList,
					includeQualifiers,
					includeClassOrigin);
		
		provider.terminate();
	}
	catch(CIMException& exception)
	{
		provider.terminate();
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(Exception& exception)
	{
		provider.terminate();
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(...)
	{
		provider.terminate();
		throw CIMException(CIM_ERR_FAILED);
	}

	PEG_METHOD_EXIT();

	return objects;
}


//////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::getQueryString - builds the query string from the
//		input parameters for Associator and Reference commands
//
// ///////////////////////////////////////////////////////////////////////////
String WMIBaseProvider::getQueryString(const CIMObjectPath &objectName,
		const String &sQueryCommand,							   
		const String &assocClass, 
		const String &resultClass, 
		const String &role,
		const String &resultRole)
{
	bool hasWHERE = false;
	bool isInst;

	//first we need to get the object name
	String sObjName = getObjectName(objectName);

	// check if is an instance name
	Uint32 pos = sObjName.find(qString(Q_PERIOD));
	isInst = (PEG_NOT_FOUND != pos);

	CMyString sQuery;
	sQuery.Format(CMyString(sQueryCommand), 128, CMyString(sObjName));

	//set up any optional parameters
	if (!((0 == assocClass.size()) && (0 == resultClass.size()) &&
		  (0 == role.size()) && (0 == resultRole.size())))
	{	
		// we have optional parameters, append the appropriate ones
		sQuery += qChar(Q_WHERE);
		hasWHERE = true;

		if (0 != assocClass.size())
		{
			sQuery += qChar(Q_ASSOC_CLS);
			sQuery += assocClass;
		}

		if (0 != resultClass.size())
		{
			sQuery += qChar(Q_RESULT_CLASS);
			sQuery += resultClass;
		}

		if (0 != role.size())
		{
			sQuery += qChar(Q_ROLE);
			sQuery += role;
		}

		if (0 != resultRole.size())
		{
			sQuery += qChar(Q_RESULT_ROLE);
			sQuery += resultRole;
		}
	}

	// check if an instance
	if (!isInst)
	{
		// have a class, add "SchemaOnly"
		if (!hasWHERE)
		{
			sQuery += qChar(Q_WHERE);
		}

		sQuery += qChar(Q_SCHEMA);
	}

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMIBaseProvider::getQueryString() - Query is %s", (LPCTSTR)sQuery); 

	String s = (LPCTSTR)sQuery;
	return s;
}

//////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::getObjectName - extracts the String object name from
//		CIMObjectPath
//		removes namespace
// ///////////////////////////////////////////////////////////////////////////
String WMIBaseProvider::getObjectName( const CIMObjectPath& objectName)
{
	String sObjName;
	bool bHaveReference = false;
	
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::getObjectName()");

	sObjName = objectName.toString();
	
	Tracer::trace("WMIBaseProvider", 400, TRC_WMIPROVIDER, Tracer::LEVEL3,
		sObjName); 

	// and remove the namespace stuff
	//===================================
	Uint32 pos;
	
	//1. Remove the machine name, port before looking for the classname
	if (sObjName.subString(0, 4) != "root") 
	{
		pos = sObjName.find("root");
		
		if (PEG_NOT_FOUND != pos)
		{
			sObjName.remove(0, pos);	
		}
	}

	//2. After ensuring that all stuff before root was removed,
	//   get the class/instance name.
	pos = sObjName.find(qString(Q_COLON));
		
	if (PEG_NOT_FOUND != pos)
	{
		sObjName.remove(0, pos + 1);	
	}
	//===================================

	// Check if has =R".." for a reference instance and
	//	if so, remove the R
	//Uint32 pos = sObjName.find(qString(Q_REF_KEY));
	pos = sObjName.find(qString(Q_REF_KEY));
	bHaveReference = (PEG_NOT_FOUND != pos);

	if (bHaveReference)
	{
		while (PEG_NOT_FOUND != pos)
		{
			sObjName.remove(pos + 1, 1);	//removing R"
			pos = sObjName.find(qString(Q_REF_KEY));
		}
	}

	Tracer::trace("WMIBaseProvider", 420, TRC_WMIPROVIDER, Tracer::LEVEL3, sObjName); 
	PEG_METHOD_EXIT();

	return sObjName;
}

PEGASUS_NAMESPACE_END
