//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include "System.h"
#include "Logger.h"
#include "System.h"
#include "Destroyer.h"

#ifdef PEGASUS_OS_HPUX
    #include <syslog.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const Uint32 Logger::TRACE = (1 << 0);
const Uint32 Logger::INFORMATION = (1 << 1);
const Uint32 Logger::WARNING = (1 << 2);
const Uint32 Logger::SEVERE = (1 << 3);
const Uint32 Logger::FATAL = (1 << 4);

LoggerRep* Logger::_rep = 0;
String Logger::_homeDirectory = ".";

// FUTURE-SF-P3-20020517 : This may need to be configurable. At least needs 
// to be set to 0xFF by default always.
Uint32 Logger::_severityMask = 0xFF;      // Set all on by default

Uint32 Logger::_writeControlMask = 0xF;   // Set all on by default

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
#ifndef PEGASUS_OS_HPUX
	// Add test for home directory set.

	// If home directory does not exist, create it.
	CString lgDir = homeDirectory.getCString();

	if (!System::isDirectory(lgDir))
	    System::makeDirectory(lgDir);

	// KS: I put the second test in just in case some trys to create
	// a completly erronous directory.  At least we will get a message
	if (!System::isDirectory(lgDir))
	    cerr << "Logging Disabled";

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

void Logger::put(
    LogFileType logFileType,
    const String& systemId,
    Uint32 severity,
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
    // Test for severity against severity mask to determine
    // if we write this log.
    if ((_severityMask & severity) != 0) 
    {
	if (!_rep)
	   _rep = new LoggerRep(_homeDirectory);

	// Get the Severity String
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

        String logMsg = Formatter::format(formatString,
                arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

        #ifdef PEGASUS_OS_HPUX
            // FUTURE-SF-P3-20020517 : Use the Syslog on HP-UX. Eventually only 
            // certain messages will go to the Syslog and others to the 
            // Pegasus Logger.
            Uint32 syslogLevel = LOG_DEBUG;

            // Map the log levels.
            if (severity & Logger::TRACE) syslogLevel =       LOG_DEBUG;
            if (severity & Logger::INFORMATION) syslogLevel = LOG_INFO;
            if (severity & Logger::WARNING) syslogLevel =     LOG_WARNING;
            if (severity & Logger::SEVERE) syslogLevel =      LOG_ERR;
            if (severity & Logger::FATAL) syslogLevel =       LOG_CRIT;
  
            // Open the syslog.
            // Ignore the systemId and open the log as cimserver
            openlog("cimserver", LOG_PID|LOG_CONS, LOG_DAEMON);

            // Log the message
            syslog(syslogLevel, "%s", (const char*)logMsg.getCString());

            // Close the syslog.
            closelog();
       #else
	    const char* tmp = "";
	    if (severity & Logger::TRACE) tmp =       "TRACE   ";
	    if (severity & Logger::INFORMATION) tmp = "INFO    ";
            if (severity & Logger::WARNING) tmp =     "WARNING ";
	    if (severity & Logger::SEVERE) tmp =      "SEVERE  ";
	    if (severity & Logger::FATAL) tmp =       "FATAL   ";
                _rep->logOf(logFileType) << System::getCurrentASCIITime() 
                 << " " << tmp << logMsg << endl;
       #endif

    }
}

void Logger::clean(const String& directory)
{
    //String logFiles = logsDirectory;
    //logFiles.append("/PegasusTrace.log");
    //cout << "Delete logs in " << logFiles << endl;
    //System::removeFile(logFiles.getCString());
    //for (i = xx; i < yy; i++)
    //(
    //_allocateLogFileName(directory, i)
    //removeFile(
    //}
}

void Logger::setHomeDirectory(const String& homeDirectory)
{
    _homeDirectory = homeDirectory;
}

PEGASUS_NAMESPACE_END
