//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WQLParser_h
#define Pegasus_WQLParser_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/WQL/Linkage.h>
#include <Pegasus/WQL/WQLSelectStatement.h>

PEGASUS_NAMESPACE_BEGIN

/** This class provides the parser interface for the WQL parser.

    Here is an example of how to parse a SELECT statement using this
    class:

    <pre>
	const char TEXT[] = "SELECT X,Y FROM MyClass WEHERE X > 10 AND Y < 3";
	Array<Sint8> text(TEXT, sizeof(TEXT));
	WQLSelectStatement selectStatement;

	WQLParser parser;

	try
	{
	    parser.parse(text, selectStatement);
	}
	catch (ParseError&)
	{
	    ...
	}
	catch (MissingNullTerminator&)
	{
	    ...
	}
    </pre>

    NOTE: the text must be NULL terminated or else the MissingNullTerminator
    exception is thrown.
*/
class PEGASUS_WQL_LINKAGE WQLParser
{
public:

    /** Parse the SELECT statement given by the text parameter and initialize
	the statement parameter accordingly.

	@param text null terminated array of characters containing a SELECT
	    statement.
	@param statement object which holds the compiled version of the SELECT
	    statement upon return.
	@exception throws ParseError if text is not a valid SELECT statement.
	@exception throws MissingNullTerminator if text argument is not 
	    terminated with a null. 

	ATTN: this method is NOT thread safe. Needs mutexes.
    */
    static void parse(
	const Array<Sint8>& text,
	WQLSelectStatement& statement);

private:

    WQLParser() { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WQLParser_h */
