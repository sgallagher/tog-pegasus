#include <time.h>
#include <windows.h>
#include <iostream>
#include "Stopwatch.h"

PEGASUS_NAMESPACE_BEGIN

Stopwatch::Stopwatch() 
{
    reset();
}

void Stopwatch::reset() 
{
    _start = clock();
}

double Stopwatch::getElapsed() const
{
    return (double)(clock() - (clock_t)_start) / CLOCKS_PER_SEC;
}

void Stopwatch::printElapsed()
{
    std::cout << getElapsed() << " seconds" << std::endl;
}

void Stopwatch::sleep(double seconds)
{
    Sleep(seconds * 1000);
}

PEGASUS_NAMESPACE_END
