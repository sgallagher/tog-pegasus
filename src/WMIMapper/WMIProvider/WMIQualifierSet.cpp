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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:	Barbara Packard (barbara_packard@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "WMIQualifier.h"
#include "WMIString.h"
#include "WMIValue.h"
#include "WMIType.h"
#include "WMIFlavor.h"
#include "WMIQualifierSet.h"

PEGASUS_NAMESPACE_BEGIN

WMIQualifierSet::WMIQualifierSet(const CIMQualifierList & cimQualifierList) : CIMQualifierList(cimQualifierList)
{
}

WMIQualifierSet::WMIQualifierSet(IWbemQualifierSet * pObject)
{
	CComPtr<IWbemQualifierSet> pQualifiers = pObject;

	HRESULT hr;
	CMyString	sMessage;

	CComBSTR	bsName;			// of the qualifier
	CComVariant	vValue;			// of the qualifier
	long		lFlavor;		// of the qualifier
	CIMQualifier	qualifier;
	
 	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIQualifierSet::WMIQualifierSet");

	hr = pQualifiers->BeginEnumeration(0);
	sMessage = "BeginEnumeration()";

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

		try
		{
			add(WMIQualifier(bsName, vValue, lFlavor));
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
		Tracer::trace(TRC_WMIPROVIDER,Tracer::LEVEL3,
			"WMIQualifierSet::WMIQualifierSet - %s result is %x", (LPCTSTR)sMessage, hr);
		throw CIMException(CIM_ERR_FAILED);
	}
	else
	{
		pQualifiers->EndEnumeration();
	}

	PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
