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
