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
    assert(source.addValue("x", WQLOperand(10, WQL_INTEGER_VALUE_TAG)));
    assert(source.addValue("y", WQLOperand(20, WQL_INTEGER_VALUE_TAG)));
    assert(source.addValue("z", WQLOperand(1.5, WQL_DOUBLE_VALUE_TAG)));

    //
    // Define query:
    //
    
    const char TEXT[] = 
	"SELECT x,y,z\n"
	"FROM MyClass\n"
	"WHERE x > 5 AND y < 25 AND z > 1.2";

    Array<Sint8> text;
    text.append(TEXT, sizeof(TEXT));
    // cout << text.getData() << endl;

    // 
    // Parse the text:
    //

    WQLSelectStatement statement;

    try
    {
	WQLParser::parse(text, statement);
	// statement.print();
	// Boolean result = statement.evaluateWhereClause(&source);
	// cout << "result=" << result << endl;
    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
	exit(1);
    }
}

int main(int argc, char** argv)
{
    test01();

    cout << "+++++ passed all tests" << endl;
    return 0;
}
