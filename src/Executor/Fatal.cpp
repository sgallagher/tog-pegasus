#include <stdarg.h>
#include <syslog.h>
#include <stdio.h>
#include "Fatal.h"
#include "Log.h"
#include "Exit.h"

extern const char* globalArg0;

//==============================================================================
//
// Fatal()
//
//     Report fatal errors. The callar set fatal_file and fatal_line before
//     calling this function. Note that since this is a single threaded
//     application, there is no attempt to synchronize access to these
//     globals.
//
//==============================================================================

void Fatal(const char* file, size_t line, const char* format, ...)
{
    Log(LL_FATAL, "%s(%d): Fatal() called", file, int(line));

    // Print to syslog.
    {
        va_list ap;
        va_start(ap, format);
        vsyslog(LOG_CRIT, format, ap);
        va_end(ap);
    }

    // Print to stderr.
    {
        fprintf(stderr, "%s: %s(%d): ", globalArg0, file, (int)line);
        va_list ap;
        va_start(ap, format);
        /* Flawfinder: ignore */
        vfprintf(stderr, format, ap);
        va_end(ap);
        fputc('\n', stderr);
    }

    Exit(1);
}
