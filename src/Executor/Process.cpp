#include <stdio.h>
#include <string.h>
#include "Process.h"
#include "Strlcpy.h"

#if defined(PEGASUS_OS_HPUX)
# include <sys/pstat.h>
#endif

//==============================================================================
//
// GetProcessName()
//
//==============================================================================

#if defined(PEGASUS_OS_HPUX)

int GetProcessName(int pid, char name[EXECUTOR_BUFFER_SIZE])
{
    struct pst_status psts;

    if (pstat_getproc(&psts, sizeof(psts), 0, pid) == -1)
        return -1;

    Strlcpy(name, pstru.pst_ucomm, EXECUTOR_BUFFER_SIZE);

    return 0;
}

#elif defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)

int GetProcessName(int pid, char name[EXECUTOR_BUFFER_SIZE])
{
    // Read the process name from the file.

    static char buffer[1024];
    sprintf(buffer, "/proc/%d/stat", pid);
    FILE* is = fopen(buffer, "r");

    if (!is)
        return -1;

    // Read the first line of the file.

    if (fgets(buffer, sizeof(buffer), is) == NULL)
    {
        fclose(is);
        return -1;
    }

    fclose(is);

    // Extract the PID enclosed in parentheses.

    char* start = strchr(buffer, '(');

    if (!start)
        return -1;

    start++;

    char* end = strchr(start, ')');

    if (!end)
        return -1;

    if (start == end)
        return -1;

    *end = '\0';

    Strlcpy(name, start, EXECUTOR_BUFFER_SIZE);

    return 0;
}

#else
# error "not implemented on this platform."
#endif /* PEGASUS_PLATFORM_LINUX_GENERIC_GNU */
