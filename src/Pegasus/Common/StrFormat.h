//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Michael E. Brasher
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Format_h
#define Pegasus_Format_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/** Builds a string from a variable list of arguments. The format string
    specifies how the subsequent arguments are transformed into a string.
    The format string is scanned from left to right and copied to the output
    string until a '%' character is encountered. The '%' character introduces
    a 'specifier' which indicates how one of subsequent arguments is 
    transformed and copied to the output string. The specifier has the
    following form.

	<pre>
    	"%" argument tag
	</pre>

    That is, a percent character followed a zero-based argument number 
    (indicating which argument to transform) and then by a character 
    tag indicating the type of the argument. For example.

	<pre>
	%0z -- transform the first argument of type char* (given by 'z')
	%3i -- transform the fourth argument of type Sint32 (given by 'i')
	%5L -- transform the sixth argument of type uint64 (given by 'L')
	</pre>

    Like printf(), '%%' is used to specify a percent character.

    Here are the valid tags and their corresponding types.

	<pre>
	o -- boolean (mnemonic: b[o]olean)
	b -- sint8 (mnemonic: [b]yte)
	B -- uint8 (mnemonic: [B]yte)
	s -- sint16 (mnemonic: [s]hort)
	S -- uint16 (mnemonic: [S]hort)
	i -- sint32 (mnemonic: [i]nt)
	I -- uint32 (mnemonic: [I]nt)
	l -- sint64 (mnemonic: [l]ong)
	L -- uint64 (mnemonic: [L]ong)
	f -- real32 (mnemonic: [f]loat)
	d -- real64 (mnemonic: [d]ouble)
	c -- char (mnemonic: [c]har)
	C -- char16 (mnemonic: [C]har)
	z -- char* (mnemonic: [z]tring)
	Z -- String (mnemonic: [Z]tring)
	t -- datetime (menmonic: date[t]ime)
	</pre>

    Consider the following example, which prints the date and day of the week.

	<pre>
	Uint32 month;
	Uint32 day;
	Uint32 year;
	String day_of_week;

	char* str;

	StrFormat(str, 
	    "Month: %0I/%1I/%2I %3Z\n", month, day, year, &day_of_week);
	</pre>

    The argument numbers are essential for performing localization. For 
    example, to put the date in European format, the specifiers may be 
    reordered. This works well since format string are typically obtained 
    from an language localization file. For example:

	<pre>
	StrFormat(str, 
	    "Month: %2I/%0I/%1I %3Z\n", month, day, year, &day_of_week);
	</pre>

    The scheme above has five major advantages over the printf() family of
    routines in the standard library. First, the CIM types are expressly
    supported without ambiguity. For example, the size of integers and longs
    may vary across platforms but StrFormat() specifiers are defined in terms 
    of the actual size of the integer types. Second, StrFormat() supports 
    printing of sixty-four bit integers. Third, it is possible to print String 
    objects without converting them to 'char*' up front. Fourth, StrFormat() 
    supports printing to a string (whereas, the standard library provides 
    no standard mechanism). And finally, arguments may be reordered by the 
    format statement.

    Note that it is an error to use an argument more than once. It is also
    an error to skip over arguments (for every argument, there must be a
    corresponing specifier).

    @param str points to a newly allocated string upon return. The caller
	is responsible for passing the string to free().
    @param format format string as described above.
    @return the number of characters copied to the str argument.
*/
int StrFormat(char*& str, const char* format, ...);

/** Similar to StrFormat() but accepts a va_list rather than a variable
    argument list.
*/
int VaStrFormat(char*& str, const char* format, va_list ap);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Format_h */
