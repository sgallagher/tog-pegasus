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
#include <Pegasus/WQL/WQLParser.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
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
