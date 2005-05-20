//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Sushma Fernandes (Hewlett-Packard Company)
//              sushma_fernandes@hp.com
// Modified By: Dave Rosckes (IBM)
//              rosckes@us.ibm.com
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP101
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cstring>
#include "Logger.h"
#include "System.h"
#include <Pegasus/Common/MessageLoader.h> //l10n

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const Uint32 Logger::TRACE = (1 << 0);
const Uint32 Logger::INFORMATION = (1 << 1);
const Uint32 Logger::WARNING = (1 << 2);
const Uint32 Logger::SEVERE = (1 << 3);
const Uint32 Logger::FATAL = (1 << 4);

static char const* LOGLEVEL_LIST[] =
{
    "TRACE",
    "INFORMATION",
    "WARNING",
    "SEVERE",
    "FATAL"
};


LoggerRep* Logger::_rep = 0;
String Logger::_homeDirectory = ".";

const Uint32 Logger::_NUM_LOGLEVEL = 5;

// Set separator
const char Logger::_SEPARATOR = '@';

Uint32 Logger::_severityMask;

Uint32 Logger::_writeControlMask = 0xF;   // Set all on by default

// Set the return codes
const Boolean Logger::_SUCCESS = 1;
const Boolean Logger::_FAILURE = 0;

/* _allocLogFileName. Allocates the name from a name set.
    Today this is static.  However, it should be completely
    configerable and driven from the config file so that
    Log organization and names are open.
    ATTN: rewrite this so that names, choice to do logs and
    mask for level of severity are all driven from configuration
    input.
*/
static CString _allocLogFileName(
    const String& homeDirectory,
    Logger::LogFileType logFileType)
{
    static const char* fileNames[] =
    {
        "PegasusTrace.log",
        "PegasusStandard.log",
        "PegasusError.log",
        "PegasusDebug.log"
    };

    int index = int(logFileType);

    if (index > Logger::NUM_LOGS)
        index = Logger::ERROR_LOG;

    const char* logFileName = fileNames[index];

    String result;
    result.reserveCapacity(homeDirectory.size() + 1 + strlen(logFileName));
    result.append(homeDirectory);
    result.append('/');
    result.append(logFileName);
    return result.getCString();
}

class LoggerRep
{
public:

    LoggerRep(const String& homeDirectory)
    {
#if !defined(PEGASUS_USE_SYSLOGS)
        // Add test for home directory set.

        // If home directory does not exist, create it.
        CString lgDir = homeDirectory.getCString();

        if (!System::isDirectory(lgDir))
            System::makeDirectory(lgDir);

        // KS: I put the second test in just in case some trys to create
        // a completly erronous directory.  At least we will get a message
        if (!System::isDirectory(lgDir)){
           //l10n
           //cerr << "Logging Disabled";
           MessageLoaderParms parms("Common.Logger.LOGGING_DISABLED",
               "Logging Disabled");

           cerr << MessageLoader::getMessage(parms);
        }

        CString fileName = _allocLogFileName(homeDirectory, Logger::TRACE_LOG);
        _logs[Logger::TRACE_LOG].open(fileName, ios::app);

        fileName = _allocLogFileName(homeDirectory, Logger::STANDARD_LOG);
        _logs[Logger::STANDARD_LOG].open(fileName, ios::app);

        fileName = _allocLogFileName(homeDirectory, Logger::ERROR_LOG);
        _logs[Logger::ERROR_LOG].open(fileName, ios::app);

        fileName = _allocLogFileName(homeDirectory, Logger::DEBUG_LOG);
        _logs[Logger::DEBUG_LOG].open(fileName, ios::app);
#endif

    }

    ostream& logOf(Logger::LogFileType logFileType)
    {
        int index = int(logFileType);

        if (index > int(Logger::ERROR_LOG))
            index = Logger::ERROR_LOG;

        return _logs[index];
    }

private:

    ofstream _logs[int(Logger::NUM_LOGS)];
};

void Logger::_putInternal(
    LogFileType logFileType,
    const String& systemId,
    const Uint32 logComponent, // TODO: Support logComponent mask in future release
    Uint32 logLevel,
    const String& formatString,
    const String& messageId,  // l10n
    const Formatter::Arg& arg0,
    const Formatter::Arg& arg1,
    const Formatter::Arg& arg2,
    const Formatter::Arg& arg3,
    const Formatter::Arg& arg4,
    const Formatter::Arg& arg5,
    const Formatter::Arg& arg6,
    const Formatter::Arg& arg7,
    const Formatter::Arg& arg8,
    const Formatter::Arg& arg9)
{
    // Test for logLevel against severity mask to determine
    // if we write this log.
    if ((_severityMask & logLevel) != 0)
    {
        if (!_rep)
           _rep = new LoggerRep(_homeDirectory);

        // Get the logLevel String
        // This converts bitmap to string based on highest order
        // bit set
        // ATTN: KS Fix this more efficiently.
        static const char* svNames[] =
        {
            "TRACE   ",
            "INFO    ",
            "WARNING ",
            "SEVERE  ",
            "FATAL   "
        };
        // NUM_LEVELS = 5
        int sizeSvNames = sizeof(svNames) / sizeof(svNames[0]) - 1;

// l10n start
        // The localized message to be sent to the system log.
        String localizedMsg;

        // If the caller specified a messageId, then load the localized
        // message in the locale of the server process.
        if (messageId != String::EMPTY)
        {
            // A message ID was specified.  Use the MessageLoader.
            MessageLoaderParms msgParms(messageId, formatString);
            msgParms.useProcessLocale = true;
            msgParms.arg0 = arg0;
            msgParms.arg1 = arg1;
            msgParms.arg2 = arg2;
            msgParms.arg3 = arg3;
            msgParms.arg4 = arg4;
            msgParms.arg5 = arg5;
            msgParms.arg6 = arg6;
            msgParms.arg7 = arg7;
            msgParms.arg8 = arg8;
            msgParms.arg9 = arg9;

            localizedMsg = MessageLoader::getMessage(msgParms);
        }
        else
        {  // No message ID.  Use the Pegasus formatter
              localizedMsg = Formatter::format(formatString,
                arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        }
// l10n end

#if defined(PEGASUS_USE_SYSLOGS)

        // Log the message
        System::syslog(System::CIMSERVER, logLevel, localizedMsg.getCString());

#else

        // Prepend the systemId to the incoming message
        String messageString(systemId);
        messageString.append(": ");
        messageString.append(localizedMsg);  // l10n

        const char* tmp = "";
        if (logLevel & Logger::TRACE) tmp =       "TRACE   ";
        if (logLevel & Logger::INFORMATION) tmp = "INFO    ";
        if (logLevel & Logger::WARNING) tmp =     "WARNING ";
        if (logLevel & Logger::SEVERE) tmp =      "SEVERE  ";
        if (logLevel & Logger::FATAL) tmp =       "FATAL   ";

        _rep->logOf(logFileType) << System::getCurrentASCIITime()
           << " " << tmp << (const char *)messageString.getCString() << endl;

#endif
    }
}

#if 1
void Logger::put(
    LogFileType logFileType,
    const String& systemId,
    Uint32 logLevel,
    const String& formatString,
    const Formatter::Arg& arg0,
    const Formatter::Arg& arg1,
    const Formatter::Arg& arg2,
    const Formatter::Arg& arg3,
    const Formatter::Arg& arg4,
    const Formatter::Arg& arg5,
    const Formatter::Arg& arg6,
    const Formatter::Arg& arg7,
    const Formatter::Arg& arg8,
    const Formatter::Arg& arg9)
{
    Uint32 logComponent = 0;

    Logger::_putInternal(logFileType, systemId, logComponent, logLevel,
        formatString, String::EMPTY, arg0, arg1, arg2, arg3, arg4, arg5, arg6,
        arg7, arg8, arg9);
}
#endif

void Logger::put(
    LogFileType logFileType,
    const String& systemId,
    Uint32 logLevel,
    const String& formatString)
{
    Uint32 logComponent = 0;

    Logger::_putInternal(logFileType, systemId, logComponent, logLevel,
        formatString, String::EMPTY);
}

void Logger::put(
    LogFileType logFileType,
    const String& systemId,
    Uint32 logLevel,
    const String& formatString,
    const Formatter::Arg& arg0)
{
    Uint32 logComponent = 0;

    Logger::_putInternal(logFileType, systemId, logComponent, logLevel,
        formatString, String::EMPTY, arg0);
}

// l10n
#if 1
void Logger::put_l(
    LogFileType logFileType,
    const String& systemId,
    Uint32 logLevel,
    const String& messageId,  // l10n
    const String& formatString,
    const Formatter::Arg& arg0,
    const Formatter::Arg& arg1,
    const Formatter::Arg& arg2,
    const Formatter::Arg& arg3,
    const Formatter::Arg& arg4,
    const Formatter::Arg& arg5,
    const Formatter::Arg& arg6,
    const Formatter::Arg& arg7,
    const Formatter::Arg& arg8,
    const Formatter::Arg& arg9)
{
    Uint32 logComponent = 0;

    Logger::_putInternal(logFileType, systemId, logComponent, logLevel,
        formatString, messageId, arg0, arg1, arg2, arg3, arg4, arg5,
        arg6, arg7, arg8, arg9);
}
#endif

void Logger::put_l(
     LogFileType logFileType,
     const String& systemId,
     Uint32 logLevel,
     const String& messageId,
     const String& formatString)
{
    Uint32 logComponent = 0;

    Logger::_putInternal(logFileType, systemId, logComponent, logLevel,
        formatString, messageId);
}

void Logger::put_l(
     LogFileType logFileType,
     const String& systemId,
     Uint32 logLevel,
     const String& messageId,
     const String& formatString,
     const Formatter::Arg& arg0)
{
    Uint32 logComponent = 0;

    Logger::_putInternal(logFileType, systemId, logComponent, logLevel,
        formatString, messageId, arg0);
}

void Logger::trace(
    LogFileType logFileType,
    const String& systemId,
    const Uint32 logComponent,
    const String& formatString,
    const Formatter::Arg& arg0,
    const Formatter::Arg& arg1,
    const Formatter::Arg& arg2,
    const Formatter::Arg& arg3,
    const Formatter::Arg& arg4,
    const Formatter::Arg& arg5,
    const Formatter::Arg& arg6,
    const Formatter::Arg& arg7,
    const Formatter::Arg& arg8,
    const Formatter::Arg& arg9)
{
    Uint32 logLevel = Logger::TRACE;

    Logger::_putInternal(
        logFileType,
        systemId,
        logComponent,
        logLevel,
        formatString,
// l10n
        String::EMPTY,
        arg0,
        arg1,
        arg2,
        arg3,
        arg4,
        arg5,
        arg6,
        arg7,
        arg8,
        arg9);
}

// l10n
void Logger::trace_l(
    LogFileType logFileType,
    const String& systemId,
    const Uint32 logComponent,
    const String& messageId,
    const String& formatString,
    const Formatter::Arg& arg0,
    const Formatter::Arg& arg1,
    const Formatter::Arg& arg2,
    const Formatter::Arg& arg3,
    const Formatter::Arg& arg4,
    const Formatter::Arg& arg5,
    const Formatter::Arg& arg6,
    const Formatter::Arg& arg7,
    const Formatter::Arg& arg8,
    const Formatter::Arg& arg9)
{
    Uint32 logLevel = Logger::TRACE;

    Logger::_putInternal(
        logFileType,
        systemId,
        logComponent,
        logLevel,
        formatString,
        messageId,
        arg0,
        arg1,
        arg2,
        arg3,
        arg4,
        arg5,
        arg6,
        arg7,
        arg8,
        arg9);
}

void Logger::setHomeDirectory(const String& homeDirectory)
{
    _homeDirectory = homeDirectory;
}

////////////////////////////////////////////////////////////////////////////////
// Set logLevel.
////////////////////////////////////////////////////////////////////////////////
void Logger::setlogLevelMask( const String logLevelList )
{
    Uint32 position          = 0;
    Uint32 logLevelType = 0;
    String logLevelName      = logLevelList;

    // Check if logLevel has been specified
    if (logLevelName != String::EMPTY)
    {
        // initialise _severityMask
        _severityMask = 0;

        // Set logLevelType to indicate the level of logging
        // required by the user.
        if (String::equalNoCase(logLevelName,"TRACE"))
        {
            logLevelType =  Logger::TRACE;
        }
        else if (String::equalNoCase(logLevelName,"INFORMATION"))
        {
            logLevelType =  Logger::INFORMATION;
        }
        else if (String::equalNoCase(logLevelName,"WARNING"))
        {
            logLevelType = Logger::WARNING;
        }
        else if (String::equalNoCase(logLevelName,"SEVERE"))
        {
            logLevelType = Logger::SEVERE;
        }
        else if (String::equalNoCase(logLevelName,"FATAL"))
        {
            logLevelType = Logger::FATAL;
        }
        // Setting _severityMask.  NOTE:  When adding new logLevels
        // it is essential that they are adding in ascending order
        // based on priority.  Once a case statement is true we will
        // continue to set all following log levels with a higher
        // priority.
        switch(logLevelType)
        {
            case Logger::TRACE:
                  _severityMask |= Logger::TRACE;
            case Logger::INFORMATION:
                  _severityMask |= Logger::INFORMATION;
            case Logger::WARNING:
                  _severityMask |= Logger::WARNING;
            case Logger::SEVERE:
                  _severityMask |= Logger::SEVERE;
            case Logger::FATAL:
                  _severityMask |= Logger::FATAL;
        }
    }
    else
    {
        // Property logLevel not specified, set default value.
        _severityMask = ~Logger::TRACE;
    }
    return ;
}

Boolean Logger::isValidlogLevel(
    const String logLevel)
{
    // Validate the logLevel and modify the logLevel argument
    // to reflect the invalid logLevel

    Uint32    position=0;
    Uint32    index=0;
    String    logLevelName = String::EMPTY;
    Boolean   validlogLevel=false;
    Boolean   retCode=true;

    logLevelName = logLevel;

    if (logLevelName != String::EMPTY)
    {
        // Lookup the index for logLevel name in _logLevel_LIST
        index = 0;
        validlogLevel = false;

        while (index < _NUM_LOGLEVEL)
        {
            if (String::equalNoCase(logLevelName, LOGLEVEL_LIST[index]))
            {
                // Found logLevel, break from the loop
                validlogLevel = true;
                break;
            }
            else
            {
                index++;
            }
        }
    }
    else
    {
        // logLevels is empty, it is a valid value so return true
        return _SUCCESS;
    }

    return validlogLevel;
}


PEGASUS_NAMESPACE_END

