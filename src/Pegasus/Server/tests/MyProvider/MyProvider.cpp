//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: MyProvider.cpp,v $
// Revision 1.12  2001/04/24 00:00:15  mike
// Ported compiler to use String and Array (rather than STL equivalents)
//
// Revision 1.11  2001/04/18 11:51:33  karl
// get and set property
//
// Revision 1.10  2001/03/30 01:59:14  karl
// clean up get property
//
// Revision 1.9  2001/02/18 21:56:12  karl
// conflicts fix
//
// Revision 1.8  2001/02/18 19:02:18  mike
// Fixed CIM debacle
//
// Revision 1.7  2001/02/18 02:49:01  mike
// Removed ugly workarounds for MSVC++ 5.0 (using SP3 now)
//
// Revision 1.6  2001/02/16 18:17:11  mike
// new
//
// Revision 1.5  2001/02/16 13:05:18  karl
// Clean up and remove an extra include
//
//
//
//END_HISTORY
/* MyProvider Provider	- This provider was written as a simple test provider 
to allow a first demonstration of the interfaces and of instances.  It uses a 
completely artifical class today (Process) and creates a couple of new 
instances on receipt of the instance requests.  It is not intended to ever 
become a real provider.

*/
#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>


using namespace std;

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
	const Array<String>& propertyList = StringArray())
    {
	cout << "MyProvider::getInstance() called" << endl;

	String tmp;
	CIMReference::referenceToInstanceName(instanceName, tmp);
	cout << "instanceName=" << tmp << endl;

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

	CIMReference ref1;
	CIMReference::instanceNameToReference("Process.pid=777",ref1);

	CIMReference ref2;
	CIMReference::instanceNameToReference("Process.pid=888",ref2);

	instanceNames.append(ref1);
	instanceNames.append(ref2);

	return instanceNames;
    }
    virtual CIMValue getProperty(
	    const String& nameSpace,
	    const CIMReference& instanceName,
	    const String& propertyName)
    {

	String tmp;
	CIMReference::referenceToInstanceName(instanceName, tmp);
	cout << "MyProvider::getProperty() called" <<
	    " instanceName= " << tmp <<
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

	    String tmp;
	    CIMReference::referenceToInstanceName(instanceName, tmp);
	    cout << "MyProvider::setProperty() called" <<
		" instanceName= " << tmp << 
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
    // std::cout << "Called PegasusCreateProvider_MyProvider" << std::endl; 
    return new MyProvider;
}

PEGASUS_NAMESPACE_END
