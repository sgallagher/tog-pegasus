#include <iostream>
#include "Stopwatch.h"
#include "TimeValue.h"

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_OS_TYPE_WINDOWS
#   include <windows.h>
#elif PEGASUS_OS_TYPE_UNIX
#   include <unistd.h>
#else
#   error "Unsupported platform"
#endif

Stopwatch::Stopwatch() 
{
    reset();
}

void Stopwatch::reset() 
{
    _start = TimeValue::getCurrentTime();
}

double Stopwatch::getElapsed() const
{
    Uint32 stop = TimeValue::getCurrentTime().toMilliseconds();
    Uint32 start = _start.toMilliseconds();
    return double(stop - start) / 1000.0;
}

void Stopwatch::printElapsed()
{
    std::cout << getElapsed() << " seconds" << std::endl;
}

void Stopwatch::sleep(double seconds)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    Sleep(seconds * 1000);
#elif PEGASUS_OS_TYPE_UNIX
    sleep(seconds);
#else
#   error "Unsupported platform"
#endif
}

PEGASUS_NAMESPACE_END
