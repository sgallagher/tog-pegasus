#include <iostream>
#include "Stopwatch.h"
#include "TimeValue.h"

PEGASUS_NAMESPACE_BEGIN

#include <Pegasus/Common/System.h>

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

PEGASUS_NAMESPACE_END
