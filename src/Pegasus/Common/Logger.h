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
	ERROR_LOG
    };

    enum { NUM_LOGS = 3 };

    static const Uint32 TRACE;
    static const Uint32 INFORMATIVE;
    static const Uint32 WARNING;
    static const Uint32 SEVERE;
    static const Uint32 FATAL;

    static void put(
	LogFileType logFileType,
	const String& systemId,
	Uint32 level,
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

    static void setHomeDirectory(const String& homeDirectory);

private:

    static LoggerRep* _rep;
    static String _homeDirectory;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Logger_h */
