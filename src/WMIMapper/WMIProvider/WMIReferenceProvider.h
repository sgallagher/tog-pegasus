//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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


// WMIReferenceProvider.h: interface for the WMIReferenceProvider class.
//
//////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WMIReferenceProvider_h
#define Pegasus_WMIReferenceProvider_h

#include <WMIMapper/WMIProvider/WMIBaseProvider.h>

PEGASUS_NAMESPACE_BEGIN

class WMICollector;

class PEGASUS_WMIPROVIDER_LINKAGE WMIReferenceProvider : public WMIBaseProvider  
{
public:
	WMIReferenceProvider();
	virtual ~WMIReferenceProvider();

/////////////////////////////////////////////////////////////////////////////////
// ATTN:
// The  following public methods are not yet implemented
//
/////////////////////////////////////////////////////////////////////////////////
    /// references
	virtual Array<CIMObject> references(
        const String& nameSpace,
        const String& userName,
        const String& password,
        const CIMObjectPath& objectName,
        const String& resultClass = String::EMPTY,
        const String& role = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

    /// referenceNames
    virtual Array<CIMObjectPath> referenceNames(
        const String& nameSpace,
        const String& userName,
        const String& password,
        const CIMObjectPath& objectName,
        const String& resultClass = String::EMPTY,
        const String& role = String::EMPTY);

protected:

private:
	String getReferenceQueryString(
		const CIMObjectPath& objectName, 
		const String& resultClass, 
		const String& role);

};


PEGASUS_NAMESPACE_END

#endif // ifndef Pegasus_WMIReferenceProvider_h
