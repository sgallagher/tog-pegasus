//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;
static const char* tmpDir;

/** Test of the namespace functions for the repository.
This test creates a set of namespaces in a local repository
then enumerates them and compares the original
names to the resulting names.
The repository is created is a location determined
by the calling program.  Since this is tied to a
local makefile, typically it is in the same directory as
the makefile.
*/

String repositoryRoot;

void test(CIMRepository_Mode mode)
{

    CIMRepository r (repositoryRoot, mode);

    try
    {
	// Create an array of names
	Array<CIMNamespaceName> arr1;
	arr1.append(CIMNamespaceName ("/zzz"));
	arr1.append(CIMNamespaceName ("/xxx/yyy"));
	arr1.append(CIMNamespaceName ("/a/b/c"));

	BubbleSort(arr1);

	// create the namespaces
	for (Uint32 i = 0; i < arr1.size(); i++)
	    r.createNameSpace(CIMNamespaceName(arr1[i]));

	//retrieve the namespaces from rep. as array
	Array<CIMNamespaceName> arr2 = r.enumerateNameSpaces();

	BubbleSort(arr2);

#if 0
	for (Uint32 i = 0; i < arr2.size(); i++)
	    cout << "===>" << arr2[i] << endl;
#endif

	//confirm that the input and return are equal
	assert(arr1.size() == 3);
	assert(arr2.size() == 4);

	arr1.append(CIMNamespaceName ("root"));
	BubbleSort(arr1);
	assert(arr1 == arr2);

        NameSpaceManager nsm (repositoryRoot);
   
        if (verbose)
            nsm.print(cout);

	// Delete the namespaces test. Put in when delete installed
	for (Uint32 i = 0; i < arr1.size(); i++)
	    r.deleteNameSpace(arr1[i]);

	//enumerate the namespaces
	Array<CIMNamespaceName> arr3 = r.enumerateNameSpaces();
	assert(arr3.size() == 0);

    }
    catch (AlreadyExistsException&)
    {
	cout << "ignored already exists exception" << endl;
    }
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    tmpDir = getenv ("PEGASUS_TMP");

    if (tmpDir == NULL)
    {
        repositoryRoot = ".";
    }
    else
    {
        repositoryRoot = tmpDir;
    }
    repositoryRoot.append("/repository");

    try 
    {
      CIMRepository_Mode mode;
      if (!strcmp(argv[1],"XML") )
	{
	  mode.flag = CIMRepository_Mode::NONE;
	  if (verbose) cout << argv[0]<< ": using XML mode repository" << endl;
	}
      else if (!strcmp(argv[1],"BIN") )
	{
	  mode.flag = CIMRepository_Mode::BIN;
	  if (verbose) cout << argv[0]<< ": using BIN mode repository" << endl;
	}
      else
	{
	  cout << argv[0] << ": invalid argument: " << argv[1] << endl;
	  return 0;
	}
     
	test(mode);
    }
    catch (Exception& e)
    {
	cout << "Exception" << endl;
	cout << argv[0] << " " << argv[1] << " " << e.getMessage() << endl;
	return 1;
    }

    cout << argv[0] << " " << argv[1] << " +++++ passed all tests" << endl;

    return 0;
}
