//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include "Logger.h"

PEGASUS_NAMESPACE_BEGIN

const Uint32 Logger::TRACE = (1 << 0);
const Uint32 Logger::INFORMATIVE = (1 << 1);
const Uint32 Logger::WARNING = (1 << 2);
const Uint32 Logger::SEVERE = (1 << 3);
const Uint32 Logger::FATAL = (1 << 4);

LoggerRep* Logger::_rep = 0;
String Logger::_homeDirectory = ".";

static char* _allocLogFileName(
    const String& homeDirectory,
    Logger::LogFileType logFileType)
{
    static char* fileNames[] = 
    {
	"trace.log",
	"standard.log",
	"error.log"
    };

    int index = int(logFileType);

    if (index > Logger::NUM_LOGS)
	index = Logger::ERROR_LOG;

    const char* logFileName = fileNames[index];

    String result;
    result.reserve(homeDirectory.size() + 1 + strlen(logFileName));
    result += homeDirectory;
    result += '/';
    result += logFileName;
    return result.allocateCString();
}

class LoggerRep
{
public:

    LoggerRep(const String& homeDirectory)
    {
	char* fileName = _allocLogFileName(homeDirectory, Logger::TRACE_LOG);
	_logs[Logger::TRACE_LOG].open(fileName, ios::app);
	delete [] fileName;

	fileName = _allocLogFileName(homeDirectory, Logger::STANDARD_LOG);
	_logs[Logger::STANDARD_LOG].open(fileName, ios::app);
	delete [] fileName;

	fileName = _allocLogFileName(homeDirectory, Logger::ERROR_LOG);
	_logs[Logger::ERROR_LOG].open(fileName, ios::app);
	delete [] fileName;
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
    Uint32 level,
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
    if (!_rep)
	_rep = new LoggerRep(_homeDirectory);

    _rep->logOf(logFileType) << Formatter::format(formatString,
	arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) << endl;
}

void Logger::setHomeDirectory(const String& homeDirectory)
{
    _homeDirectory = homeDirectory;
}

PEGASUS_NAMESPACE_END
