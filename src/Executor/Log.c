/*
//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//%/////////////////////////////////////////////////////////////////////////////
*/
#include "Log.h"
#include "Defines.h"
#include "Config.h"
#include <syslog.h>
#include <string.h>
#include <stdarg.h>
#include "Strlcpy.h"
#include "Strlcat.h"

static enum LogLevel _level = LL_INFORMATION;

void OpenLog(const char* ident, int perror)
{
    int options = LOG_PID;

    if (perror)
        options |= LOG_PERROR;

    openlog(ident, options, LOG_DAEMON);
}

void SetLogLevel(enum LogLevel level)
{
    _level = level;
}

void Log(enum LogLevel type, const char *format, ...)
{
    static int _priorities[] =
    {
        LOG_ALERT, /* LL_FATAL, */
        LOG_CRIT, /* LL_SEVERE */
        LOG_WARNING, /* LL_WARNING */
        LOG_NOTICE, /* LL_INFORMATION */
        LOG_INFO, /* LL_TRACE */
    };
    static const char* _prefix[] =
    {
        "FATAL",
        "SEVERE",
        "WARNING",
        "INFORMATION",
        "TRACE"
    };
    char prefixedFormat[EXECUTOR_BUFFER_SIZE];

    /* Prefix the format with the log level. */

    Strlcpy(prefixedFormat, _prefix[(int)type], sizeof(prefixedFormat));
    Strlcat(prefixedFormat, ": ", sizeof(prefixedFormat));
    Strlcat(prefixedFormat, format, sizeof(prefixedFormat));

    /* This array maps Pegasus "log levels" to syslog priorities. */

    if ((int)type <= (int)_level)
    {
        va_list ap;
        va_start(ap, format);
        vsyslog(_priorities[(int)type], prefixedFormat, ap);
        va_end(ap);
    }
}

/*
**==============================================================================
**
** GetLogLevel()
**
**==============================================================================
*/

void GetLogLevel()
{
    char buffer[EXECUTOR_BUFFER_SIZE];

    if (GetConfigParam("logLevel", buffer) == 0)
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
