#include <iostream>
#include <cassert>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Stopwatch.h>

using namespace std;
using namespace Pegasus;

int main()
{
    TimeValue tv1 = TimeValue::getCurrentTime();
    System::sleep(5);
    TimeValue tv2 = TimeValue::getCurrentTime();

    Uint32 milliseconds = tv2.toMilliseconds() - tv1.toMilliseconds();

    // cout << "milliseconds=" << milliseconds << endl;

    assert(milliseconds >= 4500 && milliseconds <= 5500);

    cout << "+++++ passed all tests" << endl;

    return 0;
}
