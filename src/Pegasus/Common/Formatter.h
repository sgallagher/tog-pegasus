//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author: Mike Brasher
//
// $Log: Formatter.h,v $
// Revision 1.1  2001/03/22 23:24:23  mike
// Added new formatter class for formatting log entries and other things.
//
//
//END_HISTORY

#ifndef Pegasus_Formatter_h
#define Pegasus_Formatter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

/**

*/
class PEGASUS_COMMON_LINKAGE Formatter
{
public:

    class Arg
    {
    public:

	enum Type { VOID, STRING, INTEGER, REAL };

	Arg() : _type(VOID)
	{
	}

	Arg(const String& x) : _string(x), _type(STRING)
	{
	}

	Arg(const char* x) : _string(x), _type(STRING)
	{
	}

	Arg(Sint32 x) : _integer(x), _type(INTEGER)
	{
	}

	Arg(Real64 x) : _real(x), _type(REAL)
	{
	}

	Type _type;

	String toString() const;

    private:

	String _string;

	union
	{
	    Sint32 _integer;
	    Real64 _real;
	};
    };

    static String format(
	const String& formatString,
	const Arg& arg0 = Arg(),
	const Arg& arg1 = Arg(),
	const Arg& arg2 = Arg(),
	const Arg& arg3 = Arg(),
	const Arg& arg4 = Arg(),
	const Arg& arg5 = Arg(),
	const Arg& arg6 = Arg(),
	const Arg& arg7 = Arg(),
	const Arg& arg8 = Arg(),
	const Arg& arg9 = Arg());
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Formatter_h */
