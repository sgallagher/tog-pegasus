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
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/FileSystem.h>
#include <cstdio>

using namespace Pegasus;
using namespace std;

static void _parseFile(const char* fileName)
{
    // cout << "Parsing: " << fileName << endl;

    Array<char> text;
    FileSystem::loadFileToMemory(text, fileName);
    text.append('\0');

    XmlParser parser((char*)text.getData());

    try
    {
	XmlEntry entry;

	while (parser.next(entry))
	    ; // entry.print();
    }
    catch (Exception& e)
    {
	cout << fileName << ": " << e.getMessage() << endl;
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
	cerr << "Usage: " << argv[0] << " xml-filename ..." << endl;
	exit(1);
    }

    for (Uint32 i = 1; i < Uint32(argc); i++)
    {
	try 
	{ 
	    _parseFile(argv[i]); 
	}
	catch(Exception& e)
	{
	    cerr << "Error: " << e.getMessage() << endl;	
	    exit(1);
	}
    }

    return 0;
}
