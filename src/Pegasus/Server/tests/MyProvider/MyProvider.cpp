#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>

using namespace std;

PEGASUS_NAMESPACE_BEGIN

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
	const Array<String>& propertyList = _getStringArray())
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

    void initialize(Repository& repository)
    {
	cout << "MyProvider::initialize() called" << endl;
    }
};

// This is the dynamic entry point into this dynamic module. The name of
// this provider is "MyProvider" which is appened to "PegasusCreateProvider_"
// to form a symbol name. This function is called by the ProviderTable
// to load this provider.

extern "C" PEGASUS_EXPORT CIMProvider* PegasusCreateProvider_MyProvider()
{
    // std::cout << "Called PegasusCreateProvider_MyProvider" << std::endl;
    return new MyProvider;
}

PEGASUS_NAMESPACE_END
