//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Logger_h
#define Pegasus_Logger_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Formatter.h>

PEGASUS_NAMESPACE_BEGIN

class LoggerRep;

/**

*/
class PEGASUS_COMMON_LINKAGE Logger
{
public:

    enum LogFileType 
    { 
	TRACE_LOG, 
	STANDARD_LOG, 
	ERROR_LOG,
	DEBUG_LOG
    };
    enum { NUM_LOGS = 4 };
      
    /** Log file Level - Defines the level of severity of the
        log entry irrespective of which log file it goes into. This is 
        actually a bit mask as defined in logger.cpp.  Thus, it serves both
        as a level of indication of the seriousness and possibly as a mask
        to select what is logged.
        ATTN: The selection test has not been done.
    */

    static const Uint32 TRACE;
    static const Uint32 INFORMATION;
    static const Uint32 WARNING;
    static const Uint32 SEVERE;
    static const Uint32 FATAL;

    /** put - Puts a message to the defined log file
	@param logFileType - Type of log file (Trace, etc.)
	@param systemId  - ID of the system generating the log entry within 
	Pegasus. This is user defined but generally breaks down into major
	Pegasus components.
	@param level Level of severity of the log entry. To be used both t0
	mark the log entry and tested against a mask to determine if log 
	entry should be written.
	@param formatString	Format definition string for the Log. See the 
	Formatter for details.
	@param Arg0 - Arg 9 - Up to 9 arguments representing the variables
	that go into the log entry.
    <pre>
    Logger::put(Logger::TRACE_LOG, "CIMServer", Logger::WARNING,
	"X=$0, Y=$1, Z=$2", 88,  "Hello World", 7.5);
    </pre>
    */
    static void put(
	LogFileType logFileType,
	const String& systemId,
	Uint32 severity,
	const String& formatString,
	const Formatter::Arg& arg0 = Formatter::Arg(),
	const Formatter::Arg& arg1 = Formatter::Arg(),
	const Formatter::Arg& arg2 = Formatter::Arg(),
	const Formatter::Arg& arg3 = Formatter::Arg(),
	const Formatter::Arg& arg4 = Formatter::Arg(),
	const Formatter::Arg& arg5 = Formatter::Arg(),
	const Formatter::Arg& arg6 = Formatter::Arg(),
	const Formatter::Arg& arg7 = Formatter::Arg(),
	const Formatter::Arg& arg8 = Formatter::Arg(),
	const Formatter::Arg& arg9 = Formatter::Arg());

    /** setHomeDirectory
    */
    static void setHomeDirectory(const String& homeDirectory);

    /* clean - Cleans the logger files.
    @param directory Defines the directory that the logs are in
    */
    static void clean(const String& directory);

    /** setSeverityMask
    */
    static void setSeverityMask(const Uint32);

    /** setLogWriteControlMask
    */
    static void setLogWriteControlMask(const Uint32);

private:

    static LoggerRep* _rep;
    static String _homeDirectory;
    static Uint32 _severityMask;
    static Uint32 _writeControlMask;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Logger_h */
