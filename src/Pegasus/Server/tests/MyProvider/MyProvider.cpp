#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/Provider.h>

using namespace std;

PEGASUS_NAMESPACE_BEGIN

class MyProvider : public Provider
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

    virtual InstanceDecl getInstance(
	const String& nameSpace,
	const Reference& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray())
    {
	cout << "MyProvider::getInstance() called" << endl;

	String tmp;
	Reference::referenceToInstanceName(instanceName, tmp);
	cout << "instanceName=" << tmp << endl;

	InstanceDecl instance("Process");
	instance.addProperty(Property("pid", Uint32(2001)));
	instance.addProperty(Property("name", "awk"));
	instance.addProperty(Property("age", Uint32(300)));

	return instance;
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

extern "C" PEGASUS_EXPORT Provider* PegasusCreateProvider_MyProvider()
{
    // std::cout << "Called PegasusCreateProvider_MyProvider" << std::endl;
    return new MyProvider;
}

PEGASUS_NAMESPACE_END
