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

#include <fstream>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/FileSystem.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static void _processFile(const char* fileName)
{
    Array<char> text;
    FileSystem::loadFileToMemory(text, fileName);
    text.append('\0');

    XmlParser parser((char*)text.getData());
    XmlEntry entry;

    while (parser.next(entry))
	entry.print();
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << " xml-filename" << endl;
	exit(1);
    }

    try 
    { 
	_processFile(argv[1]); 
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;	
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
