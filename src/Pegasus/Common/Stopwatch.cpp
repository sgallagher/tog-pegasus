#include <iostream>
#include "Stopwatch.h"

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <time.h>
# include <windows.h>
#endif

PEGASUS_NAMESPACE_BEGIN

Stopwatch::Stopwatch() 
{
    reset();
}

void Stopwatch::reset() 
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    _start = clock();
#else
    _start = 0;
#endif
}

double Stopwatch::getElapsed() const
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return (double)(clock() - (clock_t)_start) / CLOCKS_PER_SEC;
#else
    return 0.0;
#endif
}

void Stopwatch::printElapsed()
{
    std::cout << getElapsed() << " seconds" << std::endl;
}

void Stopwatch::sleep(double seconds)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    Sleep(seconds * 1000);
#endif
}

PEGASUS_NAMESPACE_END
