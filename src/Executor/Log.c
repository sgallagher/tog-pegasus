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
#include <stdio.h>

static enum LogLevel _level = LL_INFORMATION;

/*
**==============================================================================
**
** InitLogLevel()
**
**==============================================================================
*/

void InitLogLevel(void)
{
    char buffer[EXECUTOR_BUFFER_SIZE];

    if (GetConfigParam("logLevel", buffer) == 0)
    {
        SetLogLevel(buffer);
    }
}

/*
**==============================================================================
**
** SetLogLevel()
**
**==============================================================================
*/

int SetLogLevel(const char* logLevel)
{
    if (strcasecmp(logLevel, "TRACE") == 0)
    {
        _level = LL_TRACE;
        return 0;
    }
    else if (strcasecmp(logLevel, "INFORMATION") == 0)
    {
        _level = LL_INFORMATION;
        return 0;
    }
    else if (strcasecmp(logLevel, "WARNING") == 0)
    {
        _level = LL_WARNING;
        return 0;
    }
    else if (strcasecmp(logLevel, "SEVERE") == 0)
    {
        _level = LL_SEVERE;
        return 0;
    }
    else if (strcasecmp(logLevel, "FATAL") == 0)
    {
        _level = LL_FATAL;
        return 0;
    }
    else
    {
        return -1;
    }
}

/*
**==============================================================================
**
** GetLogLevel()
**
**==============================================================================
*/

enum LogLevel GetLogLevel(void)
{
    return _level;
}

/*
**==============================================================================
**
** OpenLog()
**
**==============================================================================
*/

void OpenLog(const char* ident)
{
    int options = LOG_PID;

    openlog(ident, options, LOG_DAEMON);
}

/*
**==============================================================================
**
** Log()
**
**==============================================================================
*/

void Log(enum LogLevel type, const char *format, ...)
{
    /* These definitions are consistent with System::syslog in
       SystemPOSIX.cpp. */
    static int _priorities[] =
    {
        LOG_CRIT, /* LL_FATAL, */
        LOG_ERR, /* LL_SEVERE */
        LOG_WARNING, /* LL_WARNING */
        LOG_INFO, /* LL_INFORMATION */
        LOG_DEBUG, /* LL_TRACE */
    };

    /* This array maps Pegasus "log levels" to syslog priorities. */

    if ((int)type <= (int)_level)
    {
        va_list ap;
        char buffer[EXECUTOR_BUFFER_SIZE];

        va_start(ap, format);
        /* Flawfinder: ignore */
        vsprintf(buffer, format, ap);
        va_end(ap);

        syslog(_priorities[(int)type], "%s", buffer);
    }
}
