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
// WMIBaseProvider.cpp: implementation of the WMIBaseProvider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//#include <Pegasus/Common/CIMNamedInstance.h>

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
void WMIBaseProvider::initialize(void)
{
	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::initialize()");

	initCollector();


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
// ///////////////////////////////////////////////////////////////////////////
void WMIBaseProvider::setup(const String & nameSpace)
{
	m_sNamespace = nameSpace;

	if (!m_bInitialized)	
	{
		initCollector();
	}

	if (m_bInitialized)
	{
		_collector->setNamespace(m_sNamespace);
	}
}

/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::initCollector
//
// ///////////////////////////////////////////////////////////////////////////
void WMIBaseProvider::initCollector()
{

	if (!m_bInitialized)	
	{
		_collector = new WMICollector( );
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
CIMInstance WMIBaseProvider::getCIMInstance(const String &nameSpace,
												const CIMObjectPath &instanceName, 
												const CIMPropertyList &propertyList)
{

	CIMInstance cimInstance;
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::getCIMInstance()");

	try
	{
		WMIInstanceProvider provider;

		provider.initialize();

		cimInstance = provider.getInstance(nameSpace, instanceName, false, false, false, propertyList);

		provider.terminate();
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(...)
	{
		throw CIMException(CIM_ERR_FAILED);
	}

    PEG_METHOD_EXIT();

	return cimInstance;
}


/////////////////////////////////////////////////////////////////////////////
// WMIBaseProvider::getCIMClass - retrieves a CIMClass object
//
// ///////////////////////////////////////////////////////////////////////////
CIMClass WMIBaseProvider::getCIMClass(const String &nameSpace, const String &className,
										  const CIMPropertyList &propertyList)
{
	CIMClass cimClass;
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::getCIMClass()");

	try
	{
		WMIClassProvider provider;

		provider.initialize();

		cimClass = provider.getClass(nameSpace, className, false, true, true, propertyList);

		provider.terminate();
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(...)
	{
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

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::execCIMQuery()");

	try
	{
		WMIQueryProvider provider;

		provider.initialize();

		objects = provider.execQuery(nameSpace,
					queryLanguage,
					query,
					propertyList,
					includeQualifiers,
					includeClassOrigin);

		provider.terminate();
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
		throw PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	catch(...)
	{
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
	String s;
	bool hasWHERE = false;
	bool isInst;

	String sObjName;
	CMyString sName;
	CMyString sQCmnd;

	CMyString sQuery;

	sQCmnd = sQueryCommand;

	//first we need to get the object name
	sObjName = getObjectName(objectName);

	// check if is an instance name
	Uint32 pos = sObjName.find(qString(Q_PERIOD));
	isInst = (PEG_NOT_FOUND != pos);

	sName = sObjName;
	sQuery.Format(sQCmnd, 128, sName);

	//set up any optional parameters
	if (!(	(0 == assocClass.size()) && (0 == resultClass.size()) &&
			(0 == role.size()) && (0 == resultRole.size()) ))
	{	
		// we have optional parameters, append the appropriate ones
		sQuery += qChar(Q_WHERE);
		hasWHERE = true;

		if (0 != assocClass.size())
		{
			sQuery += qChar(Q_ASSOC_CLS);
			sName = assocClass;
			sQuery += sName;
		}

		if (0 != resultClass.size())
		{
			sQuery += qChar(Q_RESULT_CLASS);
			sName = resultClass;
			sQuery += sName;
		}

		if (0 != role.size())
		{
			sQuery += qChar(Q_ROLE);
			sName = role;
			sQuery += sName;
		}

		if (0 != resultRole.size())
		{
			sQuery += qChar(Q_RESULT_ROLE);
			sName = resultRole;
			sQuery += sName;
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

	s = (LPCTSTR)sQuery;
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
	Uint32 i, size, refCount;

	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIBaseProvider::getObjectName()");

	// we will need the keys to massage the object name string
	KeyBindingArray keys = objectName.getKeyBindings();
	size = keys.size();

	for (i=0, refCount=0; i<size; i++)
	{
		KeyBinding key = keys[i];

		if (KeyBinding::REFERENCE == key.getType())
		{
			refCount++;
		}
	}

	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
		"WMIBaseProvider::getObjectName() - Reference count is %x", refCount); 
	
	sObjName = objectName.toString();  //false);	//no host name
	Tracer::trace("WMIBaseProvider", 400, TRC_WMIPROVIDER, Tracer::LEVEL3,
		sObjName); 

	// and remove the namespace stuff
	Uint32 pos = sObjName.find(qString(Q_COLON));
	
	if (PEG_NOT_FOUND != pos)
	{
		sObjName.remove(0, pos+1);
	}

	// Check if has =R".." for a reference instance and
	//	if so, remove the R
	pos = sObjName.find(qString(Q_REF_KEY));
	bHaveReference = (PEG_NOT_FOUND != pos);

	if (bHaveReference)
	{
		while (PEG_NOT_FOUND != pos)
		{
			sObjName.remove(pos+1, 2);	//removing R"
			pos = sObjName.find(qString(Q_REF_KEY));
		}

		// now remove extra quotes and \"
		pos = sObjName.find(qString(Q_SLASH_QUOTE));
		while (PEG_NOT_FOUND != pos)
		{
			sObjName.remove(pos+1, 1);	// removing \ from \"
			pos = sObjName.find(qString(Q_SLASH_QUOTE));
		}
	}

	Tracer::trace("WMIBaseProvider", 420, TRC_WMIPROVIDER, Tracer::LEVEL3,
		sObjName); 
	PEG_METHOD_EXIT();

	return sObjName;
}

PEGASUS_NAMESPACE_END
