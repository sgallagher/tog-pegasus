#include <sys/time.h>
#include "Time.h"

//==============================================================================
//
// GetTimeStamp()
//
//     Get microseconds (usec) ellapsed since epoch.
//
//==============================================================================

TimeStamp GetTimeStamp()
{
    // ATTN: delete this function?
    struct timeval  tv;
    struct timezone ignore;
    gettimeofday(&tv, &ignore);

    return TimeStamp(tv.tv_sec) * TimeStamp(1000000) + TimeStamp(tv.tv_usec);
}
