#include <Pegasus/Common/Config.h>
#include <iostream>
#include "../MyClass/MyClass.h"

class MyClassImpl : public MyClass
{
public:

    virtual void print()
    {
	std::cout << "MyClassImpl::print() called" << std::endl;
    }
};

extern "C" __declspec(dllexport) void* LoadClass()
{
    std::cout << "Called dummy" << std::endl;
    return new MyClassImpl;
}
