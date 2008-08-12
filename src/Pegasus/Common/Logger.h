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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Logger_h
#define Pegasus_Logger_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Formatter.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN

class LoggerRep;

/** This class provides the interface for writing log records to the log.
*/
class PEGASUS_COMMON_LINKAGE Logger
{
public:

    enum LogFileType
    {
        TRACE_LOG,
        STANDARD_LOG,
        AUDIT_LOG, // Use only if PEGASUS_ENABLE_AUDIT_LOGGER is defined
        ERROR_LOG
    };

    enum { NUM_LOGS = 4 };

    /** Log file Level - Defines the loglevel of the log entry irrespective of
        which log file it goes into. This is actually a bit mask as defined in
        logger.cpp. Thus, it serves both as a level of indication of the
        seriousness and possibly as a mask to select what is logged.
        ATTN: The selection test has not been done.
    */
    static const Uint32 TRACE;
    static const Uint32 INFORMATION;
    static const Uint32 WARNING;
    static const Uint32 SEVERE;
    static const Uint32 FATAL;

    /** Puts a message to the defined log file
        @param logFileType - Type of log file (Trace, etc.)
        @param systemId  - ID of the system generating the log entry within
        Pegasus. This is user defined but generally breaks down into major
        Pegasus components.
        @param level logLevel of the log entry. To be used both t0
        mark the log entry and tested against a mask to determine if log
        entry should be written.
        @param formatString     Format definition string for the Log. See the
        Formatter for details.
        @param Arg0 - Arg 9 - Up to 9 arguments representing the variables
        that go into the log entry.
        <pre>
        Logger::put(Logger::TRACE_LOG, System::CIMSERVER, Logger::WARNING,
            "X=$0, Y=$1, Z=$2", 88,  "Hello World", 7.5);
        </pre>
    */
    static void put(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const String& formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1,
        const Formatter::Arg& arg2,
        const Formatter::Arg& arg3,
        const Formatter::Arg& arg4 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg5 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg6 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg7 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg8 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg9 = Formatter::DEFAULT_ARG);

    /** Optimized zero-argument form of put().
    */
    static void put(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const String& formatString);

    /** Optimized one-argument form of put().
    */
    static void put(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const String& formatString,
        const Formatter::Arg& arg0);

    /** Optimized two-argument form of put().
    */
    static void put(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const String& formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1);

    /** Optimized three-argument form of put().
    */
    static void put(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const String& formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1,
        const Formatter::Arg& arg2);

    /** put_l - Puts a localized message to the defined log file
        @param logFileType - Type of log file (Trace, etc.)
        @param systemId  - ID of the system generating the log entry within
        Pegasus. This is user defined but generally breaks down into major
        Pegasus components.
        @param level logLevel of the log entry. To be used both t0
        mark the log entry and tested against a mask to determine if log
        entry should be written.
        @param messageId Message ID of the format string to load from
        the resource bundle.
        @param formatString     Default format definition string. See the
        Formatter for details.  This will be used as the default format string
        in case the resource bundle cannot be found.
        @param Arg0 - Arg 9 - Up to 9 arguments representing the variables
        that go into the log entry.
        <pre>
        Logger::put(Logger::TRACE_LOG, System::CIMSERVER, Logger::WARNING,
            "X=$0, Y=$1, Z=$2", 88,  "Hello World", 7.5);
        </pre>
    */
    static void put_l(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const char* messageId,
        const char* formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1,
        const Formatter::Arg& arg2,
        const Formatter::Arg& arg3,
        const Formatter::Arg& arg4 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg5 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg6 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg7 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg8 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg9 = Formatter::DEFAULT_ARG);

    /** Optimized zero-argument form of put_l().
    */
    static void put_l(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const char* messageId,
        const char* formatString);

    /** Optimized one-argument form of put_l().
    */
    static void put_l(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const char* messageId,
        const char* formatString,
        const Formatter::Arg& arg0);

    /** Optimized two-argument form of put_l().
    */
    static void put_l(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const char* messageId,
        const char* formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1);

    /** Optimized three-argument form of put_l().
    */
    static void put_l(
        LogFileType logFileType,
        const String& systemId,
        Uint32 logLevel,
        const char* messageId,
        const char* formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1,
        const Formatter::Arg& arg2);

    /** Puts a message to the define log. Should only be used for trace type
        logs.
    */
    static void trace(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const String& formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1,
        const Formatter::Arg& arg2,
        const Formatter::Arg& arg3,
        const Formatter::Arg& arg4 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg5 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg6 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg7 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg8 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg9 = Formatter::DEFAULT_ARG);

    /** Optimized three-argument form of trace().
    */
    static void trace(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const String& formatString);

    /** Optimized one-argument form of trace().
    */
    static void trace(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const String& formatString,
        const Formatter::Arg& arg0);

    /** Optimized two-argument form of trace().
    */
    static void trace(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const String& formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1);

    /** Optimized three-argument form of trace().
    */
    static void trace(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const String& formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1,
        const Formatter::Arg& arg2);

    /** Puts a localized message to the log. Should only be used
        for trace type logs.
    */
    static void trace_l(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const char* messageId,
        const String& formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1,
        const Formatter::Arg& arg2,
        const Formatter::Arg& arg3,
        const Formatter::Arg& arg4 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg5 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg6 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg7 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg8 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg9 = Formatter::DEFAULT_ARG);

    /** Optimized zero-argument form of trace_l().
    */
    static void trace_l(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const char* messageId,
        const String& formatString);

    /** Optimized one-argument form of trace_l().
    */
    static void trace_l(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const char* messageId,
        const String& formatString,
        const Formatter::Arg& arg0);

    /** Optimized two-argument form of trace_l().
    */
    static void trace_l(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const char* messageId,
        const String& formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1);

    /** Optimized three-argument form of trace_l().
    */
    static void trace_l(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        const char* messageId,
        const String& formatString,
        const Formatter::Arg& arg0,
        const Formatter::Arg& arg1,
        const Formatter::Arg& arg2);

    /** setHomeDirectory
    */
    static void setHomeDirectory(const String& homeDirectory);

    /** setlogLevelMask
    */
    static void setlogLevelMask(const String logLevelList);

    /** Returns true if the given string is one of the legal log levels.
    */
    static Boolean isValidlogLevel(const String logLevel);

    /** Tests if a log entry would be created by this call before
        the logger is called.  This function is intended to be used
        within the server for high usage log entries to avoid the
        overhead of doing the call when no log is created.
        @param logLevel Uint32 defining the level of the log to be
        executed.
        <p><b>Example:</b>
        <pre>
        if (Logger::wouldLog(Logger::TRACE))
        {
            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER,
                Logger::TRACE, "HTTPMessage - HTTP header name: $0  "
                "HTTP header value: $1" ,name,value);
        }
        </pre>
    */
    static Boolean wouldLog(Uint32 logLevel)
    {
        return (_severityMask & logLevel) != 0;
    }

private:

    static LoggerRep* _rep;
    static String _homeDirectory;
    static Uint32 _severityMask;

    static const Uint32 _NUM_LOGLEVEL;

    static void _putInternal(
        LogFileType logFileType,
        const String& systemId,
        const Uint32 logComponent,
        Uint32 logLevel,
        const String& formatString,
        const char* messageId,
        const Formatter::Arg& arg0 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg1 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg2 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg3 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg4 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg5 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg6 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg7 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg8 = Formatter::DEFAULT_ARG,
        const Formatter::Arg& arg9 = Formatter::DEFAULT_ARG);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Logger_h */
