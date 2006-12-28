#ifdef _GNU_SOURCE
# include <cassert>
# include <cstdio>
# include <pthread.h>
# include "Backtrace.h"
# include <execinfo.h>

static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

void __PegasusBacktrace(const char* file, size_t line)
{
    if (!getenv("__PEGASAUS_EXECUTOR__"))
        return;

    pthread_mutex_lock(&_mutex);

    static const char BACKTRACE_FILE[] = "/tmp/cimbacktrace";

    // Get backtrace:

    void* array[1024];
    size_t size = backtrace(array, 1024);
    char** strings = backtrace_symbols(array, size);

    // Open backtrace file for append.

    FILE* os = fopen(BACKTRACE_FILE, "a");
    assert(os != NULL);

    // Put backtrace header:

    fprintf(os, "====: %s(%u)\n", file, (int)line);

    for (size_t i = 0; i < size; i++)
        fprintf(os, "%s\n", strings[i]);

    free (strings);

    fclose(os);

    pthread_mutex_unlock(&_mutex);
}

#endif /* _GNU_SOURCE */
