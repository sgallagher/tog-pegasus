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
// Modified By: Adrian Schuur
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
	Array<CIMNamespaceName> arro;

	arr1.append(CIMNamespaceName("root/baseWS"));
	arr1.append(CIMNamespaceName("root/base/v2WS"));
	arr1.append(CIMNamespaceName("root/base/v3RS"));
	arr1.append(CIMNamespaceName("root/base/v4RS"));

        for (int i=0,m=arr1.size(); i<m; i++)
           arro.append(arr1[i]);

	// create the namespaces

        CIMRepository::NameSpaceAttributes nsa;
        CIMRepository::NameSpaceAttributes nsa1;
        Boolean failed=false;

	r.createNameSpace(arr1[0],nsa); // not shareable

        nsa.clear();
        nsa.insert("parent",arr1[0].getString());
        nsa.insert("shareable","true");
        nsa.insert("updatesAllowed","true");

        failed=false;
        try {
	   r.createNameSpace(arr1[1],nsa);
        }
        catch (const Exception &) {
           nsa1.insert("shareable","true");     // make shareable
	   r.modifyNameSpace(arr1[0],nsa1);
	   r.createNameSpace(arr1[1],nsa);      // try again
           failed=true;
        }
	assert(failed == true);

        nsa.clear();
        nsa.insert("parent",arr1[1].getString());
        nsa.insert("shareable","true");
        nsa.insert("updatesAllowed","false");
	r.createNameSpace(arr1[2],nsa);

        nsa.clear();
        nsa.insert("parent",arr1[2].getString());
        nsa.insert("shareable","true");
        nsa.insert("updatesAllowed","false");
	r.createNameSpace(arr1[3],nsa);

        failed=false;
        try {
           nsa1.clear();
           nsa1.insert("shareable","false");
           r.modifyNameSpace(arr1[2],nsa1);     // try to namespace not shareable
        }
        catch (const Exception &) {
           failed=true;
        }
	assert(failed == true);

        try {                                   // try to delete
           r.deleteNameSpace(arro[1]);
        }
        catch (const Exception &) {
           failed=true;
        }
	assert(failed == true);


	Array<CIMNamespaceName> arr2 = r.enumerateNameSpaces();

	BubbleSort(arr1);
	BubbleSort(arr2);

	//confirm that the input and return are equal
	assert(arr1.size() == 4);
	assert(arr2.size() == 5);

	arr1.append(CIMNamespaceName ("root"));
	BubbleSort(arr1);
	assert(arr1 == arr2);

        NameSpaceManager nsm (repositoryRoot);

        if (verbose)
            nsm.print(cout);

	CIMRepository::NameSpaceAttributes attributes;

	if (verbose) for (Uint32 i = 0; i < arr1.size(); i++) {
	    r.getNameSpaceAttributes(arr1[i],attributes);
            cout<<"-----------------"<<endl;
            for (CIMRepository::NameSpaceAttributes::Iterator i = attributes.start(); i; i++)
	       cout<<"--- "<<i.key()<<": "<<i.value()<<endl;
	}

	// Delete the namespaces test. Put in when delete installed

	arro.prepend(CIMNamespaceName ("root"));
	for (int i = arro.size()-1; i>=0;  i--) {
            if (verbose) cout<<"--- delete "<<arro[i].getString()<<endl;
	    r.deleteNameSpace(arro[i]);
        }

	//enumerate the namespaces
	Array<CIMNamespaceName> arr3 = r.enumerateNameSpaces();
        if (verbose) cout<<"--- arr3.size(): "<<arr3.size()<<endl;
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
