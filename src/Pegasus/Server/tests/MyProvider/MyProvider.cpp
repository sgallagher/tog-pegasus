//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////
/* MyProvider Provider	- This provider was written as a simple test provider 
to allow a first demonstration of the interfaces and of instances.  It uses a 
completely artifical class today (Process) and creates a couple of new 
instances on receipt of the instance requests.  It is not intended to ever 
become a real provider.

*/
#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static CIMValue* savedCIMValue;

class MyProvider : public CIMProvider
{
public:

    MyProvider()
    {
	// cout << "MyProvider::MyProvider()" << endl;
    }

    virtual ~MyProvider()
    {
	// cout << "MyProvider::~MyProvider()" << endl;
    }

    virtual CIMInstance getInstance(
	const String& nameSpace,
	const CIMReference& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = EmptyStringArray())
    {
	cout << "MyProvider::getInstance() called" << endl;
	cout << "instanceName=" << instanceName << endl;

	CIMInstance instance("Process");
	instance.addProperty(CIMProperty("pid", Uint32(2001)));
	instance.addProperty(CIMProperty("name", "awk"));
	instance.addProperty(CIMProperty("age", Uint32(300)));

	return instance;
    }

    Array<CIMReference> enumerateInstanceNames(
	const String& nameSpace,
	const String& className) 
    {
	Array<CIMReference> instanceNames;

	CIMReference ref1 = "Process.pid=777";
	CIMReference ref2 = "Process.pid=888";

	instanceNames.append(ref1);
	instanceNames.append(ref2);

	return instanceNames;
    }
    virtual CIMValue getProperty(
	    const String& nameSpace,
	    const CIMReference& instanceName,
	    const String& propertyName)
    {

	cout << "MyProvider::getProperty() called" <<
	    " instanceName= " << instanceName <<
	    " CIM Saved Value = " << savedCIMValue->toString() <<
	    " propertyName = " << propertyName << endl;

	
	CIMInstance instance("Process");
	instance.addProperty(CIMProperty("pid", Uint32(2001)));
	instance.addProperty(CIMProperty("name", "awk"));
	instance.addProperty(CIMProperty("age", Uint32(300)));

	//ATTN: Simply return fixed value
	CIMValue result(Uint32(-77));
	return result;

    }
    virtual void setProperty(
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName,
		const CIMValue& newValue)
	{
	    cout << "MyProvider::setProperty() called" << endl;

	    cout << "MyProvider::setProperty() called" <<
		" instanceName= " << instanceName << 
		" Saved CIM Value " << savedCIMValue->toString() <<
		" propertyName = " << propertyName << endl;

	    

	    CIMInstance instance("Process");
	    instance.addProperty(CIMProperty("pid", Uint32(2001)));
	    instance.addProperty(CIMProperty("name", "awk"));
	    instance.addProperty(CIMProperty("age", Uint32(300)));


	    return;

	}



    void initialize(CIMRepository& repository)
    {
	cout << "MyProvider::initialize() called" << endl;
	CIMValue tmp(Uint32(932));
	savedCIMValue = &tmp;
    }
};

// This is the dynamic entry point into this dynamic module. The name of
// this provider is "MyProvider" which is appened to "PegasusCreateProvider_"
// to form a symbol name. This function is called by the ProviderTable
// to load this provider.

extern "C" PEGASUS_EXPORT CIMProvider* 
    PegasusCreateProvider_MyProvider() {
    // PEGASUS_STD(cout) << "Called PegasusCreateProvider_MyProvider" << PEGASUS_STD(endl); 
    return new MyProvider;
}

PEGASUS_NAMESPACE_END
