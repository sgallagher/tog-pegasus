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

#include <cassert>
#include <iostream>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/WQL/WQLSimplePropertySource.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    //
    // Create a property source (a place for the evaluate to get the
    // values of properties from):
    //

    WQLSimplePropertySource source;
    assert(source.addValue("x", Sint32(10), false));
    assert(source.addValue("y", Sint32(20), false));
    assert(source.addValue("z", Real32(1.5), false));

    //
    // Define query:
    //
    
    const char TEXT[] = 
	"SELECT x,y,z\n"
	"FROM MyClass\n "
	"WHERE x > 5 AND y < 20 AND z > 1.0";

    Array<Sint8> text;
    text.append(TEXT, sizeof(TEXT));
    cout << text.getData() << endl;

    // 
    // Parse the text:
    //

    WQLSelectStatement statement;

    try
    {
	WQLParser::parse(text, statement);

	Boolean result = statement.evaluateWhereClause(&source);

	cout << "result=" << result << endl;
    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    statement.print();
}

int main(int argc, char** argv)
{
    test01();
    exit(1);

    //
    // Check arguments:
    //

    if (argc < 2)
    {
	cerr << "Usage: " << argv[0] << " wql-text..." << endl;
	exit(1);
    }

    // 
    // Append all arguments together to from a single string:
    //

    Array<Sint8> text;

    for (int i = 1; i < argc; i++)
    {
	text.append(argv[i], strlen(argv[i]));

	if (i + 1 !=  argc)
	    text.append("\n", 1);
    }

    text.append('\0');

    // PEGASUS_OUT(text.getData());

    // 
    // Parse the text:
    //

    WQLSelectStatement statement;

    try
    {
	WQLParser::parse(text, statement);
    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    statement.print();

    return 0;
}
