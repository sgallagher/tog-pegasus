
//==============================================================================
//
// GetCurrentTime()
//
//     Get microseconds (usec) ellapsed since epoch.
//
//==============================================================================

uint64 GetCurrentTime()
{
    // ATTN: delete this function?
    struct timeval  tv;
    struct timezone ignore;
    gettimeofday(&tv, &ignore);

    return uint64(tv.tv_sec) * uint64(1000000) + uint64(tv.tv_usec);
}
