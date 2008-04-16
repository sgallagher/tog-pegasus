//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//=============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/FileSystem.h>
#include <cstdio>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean verbose = false;

static void _parseFile(const char* fileName)
{
    // cout << "Parsing: " << fileName << endl;

    Buffer text;
    FileSystem::loadFileToMemory(text, fileName);

    XmlParser parser((char*)text.getData());

    XmlEntry entry;

    // Get initial comment and ignore
    parser.next(entry, true);
    // get next comment, check for file Description
    parser.next(entry, true);
    if (!String::equal(entry.text, "Test XML file") )
    {
        throw CIMException(CIM_ERR_FAILED, "Comment Error");
    }
    PEGASUS_ASSERT (parser.getLine () == 2);
    PEGASUS_ASSERT (parser.getStackSize () == 0);
    // Put the Comment back...
    parser.putBack (entry);
    PEGASUS_ASSERT (parser.getLine () == 2);
    PEGASUS_ASSERT (parser.getStackSize () == 0);
    while (parser.next(entry))
    {
        if (verbose) 
        {
            entry.print();
        }
    }
    PEGASUS_ASSERT (parser.next (entry, true) == false);
}

// Test case for 7581: XML parser leaves trailing spaces in entry content.
static void _trailingSpace()
{
    char text[] = "<tag>\r\nvalue  </tag>";
    XmlParser parser(text);
    XmlEntry entry;

    while (parser.next(entry))
    {
        // Make sure content entries have no trailing spaces
        int len = strlen(entry.text);
        if (entry.type == XmlEntry::CONTENT &&
            entry.text[len - 1] == ' ')
        {
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, 
                "Unexpected trailing space in XmlEntry::CONTENT.");
        }
    }
}

int main(int argc, char** argv)
{

    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;
  
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

        _trailingSpace();
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;    
        exit(1);
    }
    }
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
