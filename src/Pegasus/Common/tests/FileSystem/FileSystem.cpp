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
// Author:
//
// $Log: FileSystem.cpp,v $
// Revision 1.2  2001/01/14 23:39:04  mike
// fixed broken regression test
//
// Revision 1.1.1.1  2001/01/14 19:53:46  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <iostream>
#include <Pegasus/Common/FileSystem.h>

using namespace Pegasus;
using namespace std;

int main()
{
    assert(FileSystem::exists("FileSystem.cpp"));
    assert(!FileSystem::exists("NoSuchFile.dat"));
    // assert(!FileSystem::canExecute("FileSytem.cpp"));
    assert(!FileSystem::canRead("NoSuchFile.dat"));
    assert(!FileSystem::canWrite("NoSuchFile.dat"));
    // assert(!FileSystem::canRead("NoCanRead.dat"));
    assert(FileSystem::isDirectory(".."));
    assert(FileSystem::isDirectory("."));
    assert(!FileSystem::isDirectory("FileSystem.cpp"));

    Array<String> paths;
    assert( FileSystem::getDirectoryContents("./testdir", paths) );


    for (Uint32 i = 0; i < paths.getSize(); i++)
    {
	if (paths[i] == "CVS")
	{
	    paths.remove(i);
	    break;
	}
    }

    BubbleSort(paths);
    assert(paths.getSize() == 3);
    assert(paths[0] == "a");
    assert(paths[1] == "b");
    assert(paths[2] == "c");

    cout << "+++++ passed all tests" << endl;

    return 0;
}
