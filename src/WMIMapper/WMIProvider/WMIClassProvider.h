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

#ifndef Pegasus_WMIClassProvider_h
#define Pegasus_WMIClassProvider_h

#include <WMIMapper/WMIProvider/WMIBaseProvider.h>

PEGASUS_NAMESPACE_BEGIN


class WMICollector; 

class PEGASUS_WMIPROVIDER_LINKAGE WMIClassProvider : public WMIBaseProvider
{
public:
	WMIClassProvider(void);
	virtual ~WMIClassProvider(void);

    /// virtual class CIMClass. From the operations class
    virtual CIMClass getClass(
        const String& nameSpace,
        const String& className,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());	

/////////////////////////////////////////////////////////////////////////////////
// ATTN:
// The  following public methods are not yet implemented
//
/////////////////////////////////////////////////////////////////////////////////
	// deleteClass
	virtual void deleteClass(
		const String& nameSpace, 
		const String& className);

	// createClass
	virtual void createClass(
		const String& nameSpace, 
		const CIMClass& newClass);

	// modifyClass
	virtual void modifyClass(
		const String& nameSpace, 
		const CIMClass& modifiedClass);

    /// enumerateClasses
    virtual Array<CIMClass> enumerateClasses(
        const String& nameSpace,
        const String& className = String::EMPTY,
        Boolean deepInheritance = false,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false);

    /// enumerateClassNames
    virtual Array<CIMName> enumerateClassNames(
        const String& nameSpace,
        const String& className = String::EMPTY,
        Boolean deepInheritance = false);

protected:
	

private:

};


PEGASUS_NAMESPACE_END

#endif // ifndef Pegasus_WMIClassProvider_h
