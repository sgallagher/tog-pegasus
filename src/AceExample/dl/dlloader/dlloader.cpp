#include <cstdlib>
#include <cassert>
#include <ace/OS.h>
#include "../MyClass/MyClass.h"

typedef void* (*LoadClassFunc)();

int main (int argc, char *argv[])
{
    const char PATH[] = "dlmodule";

    // Load the dynamic library:

    ACE_SHLIB_HANDLE h = ACE_OS::dlopen(PATH);

    if (!h)
    {
	 cerr << "Failed to load dlmodule.dll" << endl;
	 exit(1);
    }

    // Lookup LoadClass() function within that library:

    LoadClassFunc func = (LoadClassFunc)ACE_OS::dlsym(h, "LoadClass");

    if (!func)
    {
	 cerr << "Failed to find dummy() function" << endl;
	 exit(1);
    }

    // Invoke LoadClass function to obtain MyClass implementation instance:

    MyClass* myClass = (MyClass*)func();
    assert(myClass);

    myClass->print();

    return 0;
}
