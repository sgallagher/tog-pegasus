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

#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
/** Test of the namespace functions for the repository.
This test creates a set of namespaces in a local repository
then enumerates them and compares the original
names to the resulting names.
The repository is created is a location determined
by the calling program.  Since this is tied to a
local makefile, typically it is in the same directory as
the makefile.
*/
void test()
{
    CIMRepository r("./repository");

    try
    {
	// Create an array of names
	Array<String> arr1;
	arr1.append("/zzz");
	arr1.append("/xxx/yyy");
	arr1.append("/a/b/c");

	BubbleSort(arr1);

	// create the namespaces
	for (Uint32 i = 0; i < arr1.size(); i++)
	    r.createNameSpace(String(arr1[i]));

	//retrieve the namespaces from rep. as array
	Array<String> arr2 = r.enumerateNameSpaces();

	BubbleSort(arr2);

#if 0
	for (Uint32 i = 0; i < arr2.size(); i++)
	    cout << "===>" << arr2[i] << endl;
#endif

	//confirm that the input and return are equal
	assert(arr1.size() == 3);
	assert(arr2.size() == 3);
	assert(arr1 == arr2);

	// Delete the namespaces test. Put in when delete installed
	//for (Uint32 i = 0; i < arr1.size(); i++)
	//    r.deleteNameSpace(String(arr1[i]));

	//enumerate the namespaces
	//Array<String> arr3 = r.enumerateNameSpaces();
	//assert(arr3.size() == 0);

    }
    catch (AlreadyExists&)
    {
	cout << "ignored already exists exception" << endl;
    }
}

int main()
{
    try 
    {
	test();
    }
    catch (Exception& e)
    {
	cout << e.getMessage() << endl;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
