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
// Revision 1.7  2001/04/07 12:01:18  karl
// remove namespace support
//
// Revision 1.6  2001/03/11 23:35:33  mike
// Ports to Linux
//
// Revision 1.4  2001/02/13 02:06:40  mike
// Added renameFile() method.
//
// Revision 1.3  2001/02/11 05:42:33  mike
// new
//
// Revision 1.2  2001/01/14 23:39:04  mike
// fixed broken regression test
//
// Revision 1.1.1.1  2001/01/14 19:53:46  mike
// Pegasus import
//
//
//END_HISTORY

 #ifdef PEGASUS_OS_TYPE_WINDOWS
# include <io.h>
# include <direct.h>
#else
# include <unistd.h>
# include <dirent.h>
#endif


#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdio>


#include <Pegasus/Common/FileSystem.h>

using namespace Pegasus;
using namespace std;

int main()
{
    String path;
    if (!FileSystem::getCurrentDirectory(path))
        return false;
    cout <<  "CWD = " << path << endl;

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
	if (String::equal(paths[i], "CVS"))
	{
	    paths.remove(i);
	    break;
	}
    }

    String realName;
    assert(FileSystem::existsIgnoreCase("filesystem.cpp", realName));
    assert(String::equal(realName, "FileSystem.cpp"));

    assert(FileSystem::existsIgnoreCase(
	"../FileSystem/filesystem.cpp", realName));
    assert(String::equal(realName, "../FileSystem/FileSystem.cpp"));

    BubbleSort(paths);
    assert(paths.getSize() == 3);
    assert(String::equal(paths[0], "a"));
    assert(String::equal(paths[1], "b"));
    assert(String::equal(paths[2], "c"));

    // Test for getCurrentDirectory
    // Go to testdir, test for file "a"
    // Then return and test for file
    {
	String saveDir;
	FileSystem::getCurrentDirectory(saveDir);
	// Should this be ./testdir????
	FileSystem::changeDirectory("testdir");
	assert(FileSystem::exists("a"));
	FileSystem::changeDirectory(saveDir);
	assert(FileSystem::exists("FileSystem.cpp"));
    }
    // Test the Create and delete functions
    {
	String t = "TestDirectory";
	String t1 = "TestDirectory2";
	char* f = "TestFile.txt";
	char* f1 = "TestFile1.txt";

	FileSystem::makeDirectory(t);
	assert(FileSystem::isDirectory(t));
	FileSystem::removeDirectory(t);
	assert(!FileSystem::isDirectory(t));

	// Tests for remove hiearchy command
	/* ATTN: Removed following until next test ks
	// because remove hiearchy does not work yet.
	FileSystem::makeDirectory(t);

	String save_cwd;
	FileSystem::getCurrentDirectory(save_cwd);

	// create some files in new directory
	if (!FileSystem::changeDirectory(t))
	    // ATTN: what is valid error return?
	    return -1;

  	ofstream of1(f);
	of1 << "test" << endl;
	of1.close();
	assert(FileSystem::exists(f));

  	ofstream of2(f1);
	of2 << "test" << endl;
	of2.close();
	assert(FileSystem::exists(f1));
        
	// Create a second level directory
	FileSystem::makeDirectory(t1);


	// Create files in this dir
	if (!FileSystem::changeDirectory(t1))
	    return -1;

	ofstream of3("testfile3.txt");
	of3 << "test" << endl;
	of3.close();

    	ofstream of4("testfile4.txt");
	of4 << "test" << endl;
	of4.close();

	// Go back to top level directory

	FileSystem::changeDirectory(save_cwd);
        assert(FileSystem::isDirectory(t));

	FileSystem::removeDirectoryHier(t);
	// ATTN: Removed until above fixed.
	assert(!FileSystem::isDirectory(t));
	end-of-temp-removal*/
        
    }
    // Test renameFile:
    {
	const char FILE1[] = "file1.txt";
	const char FILE2[] = "file2.txt";

	assert(FileSystem::exists(FILE1));
	assert(!FileSystem::exists(FILE2));
	assert(FileSystem::renameFile(FILE1, FILE2));

	assert(!FileSystem::exists(FILE1));

	assert(FileSystem::exists(FILE2));
	assert(FileSystem::renameFile(FILE2, FILE1));
	assert(FileSystem::exists(FILE1));
	assert(!FileSystem::exists(FILE2));
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
