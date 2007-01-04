#include "Log.h"
#include "Defines.h"
#include "Config.h"
#include <syslog.h>
#include <string.h>
#include <stdarg.h>

static LogLevel _level = LL_INFORMATION;

void OpenLog(const char* ident)
{
    openlog(ident, LOG_PID, LOG_DAEMON);
}

void SetLogLevel(LogLevel level)
{
    _level = level;
}

void Log(LogLevel type, const char *format, ...)
{
    static int _priorities[] =
    {
        LOG_ALERT, // LL_FATAL,
        LOG_CRIT, // LL_SEVERE
        LOG_WARNING, // LL_WARNING
        LOG_NOTICE, // LL_INFORMATION
        LOG_INFO, // LL_TRACE
    };

    // This array maps Pegasus "log levels" to syslog priorities.

    if ((int)type <= (int)_level)
    {
        va_list ap;
        va_start(ap, format);
        vsyslog(_priorities[(int)type], format, ap);
        va_end(ap);
    }
}

//==============================================================================
//
// GetLogLevel()
//
//==============================================================================

void GetLogLevel(int argc, char** argv)
{
    char buffer[EXECUTOR_BUFFER_SIZE];

    if (GetConfigParam(argc, argv, "logLevel", buffer) == 0)
    {
        if (strcasecmp(buffer, "TRACE") == 0)
            SetLogLevel(LL_TRACE);
        else if (strcasecmp(buffer, "INFORMATION") == 0)
            SetLogLevel(LL_INFORMATION);
        else if (strcasecmp(buffer, "WARNING") == 0)
            SetLogLevel(LL_WARNING);
        else if (strcasecmp(buffer, "SEVERE") == 0)
            SetLogLevel(LL_SEVERE);
        else if (strcasecmp(buffer, "FATAL") == 0)
            SetLogLevel(LL_FATAL);
    }
}
