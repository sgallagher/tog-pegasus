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
// $Log: TestClasses.cpp,v $
// Revision 1.1  2001/01/14 19:54:00  mike
// Initial revision
//
//
//END_HISTORY

#include <fstream>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Repository/Repository.h>

using namespace Pegasus;
using namespace std;

void TestClasses(const String& repositoryRoot)
{
    String PATH = repositoryRoot;
    PATH += "/repository/root#cimv20/classes";

    Repository repository(repositoryRoot);

    // Enumerate all the classes in the repository:

    Array<String> classNames = repository.enumerateClassNames(
	"root/cimv20", String(), true);

    for (Uint32 i = 0; i < classNames.getSize(); i++)
    {
	// cout << classNames[i] << endl;

	try
	{
	    //------------------------------------------------------------------
	    // Get the class:
	    //------------------------------------------------------------------

	    ConstClassDecl classDecl = repository.getClass(
		"root/cimv20", classNames[i]);

	    //------------------------------------------------------------------
	    // Save to tmpfile:
	    //------------------------------------------------------------------

	    Array<Sint8> out;
	    classDecl.toXml(out);

#ifdef PEGASUS_COMPILER_MSVC
	    ofstream os("tmpfile", ios::binary);
#else
	    ofstream os("tmpfile");
#endif

	    if (!os)
	    {
		cerr << "failed to open tmpfile" << endl;
		exit(1);
	    }

	    out.append('\0');
	    XmlWriter::indentedPrint(os, out.getData(), 2);
	    os.close();

	    //------------------------------------------------------------------
	    // Compare it with the original:
	    //------------------------------------------------------------------

	    String superClassName = classDecl.getSuperClassName();
	    String path = PATH;
	    path += '/';
	    path += classDecl.getClassName();
	    path += '.';
	    if (superClassName.getLength())
		path += superClassName;
	    else
		path += "#";

	    if (!FileSystem::compare("tmpfile", path))
	    {
		cerr << "compare() failed: \"tmpfile\" and ";
		cerr << "\"" << path << "\"" << endl;
		exit(1);
	    }
	}
	catch (Exception& e)
	{
	    cout << classNames[i] << ": " << e.getMessage() << endl;
	    exit(1);
	}
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << " repository-root" << endl;
	exit(1);
    }

    try
    {
	TestClasses(argv[1]);
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
