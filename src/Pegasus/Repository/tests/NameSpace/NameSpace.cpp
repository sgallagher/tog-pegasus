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
// $Log: NameSpace.cpp,v $
// Revision 1.2  2001/02/18 19:02:18  mike
// Fixed CIM debacle
//
// Revision 1.1.1.1  2001/01/14 19:53:58  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

using namespace Pegasus;
using namespace std;

void test()
{
    CIMRepository r(".");

    try
    {
	Array<String> arr1;
	arr1.append("/zzz");
	arr1.append("/xxx/yyy");
	arr1.append("/a/b/c");

	BubbleSort(arr1);

	for (Uint32 i = 0; i < arr1.getSize(); i++)
	    r.createNameSpace(String(arr1[i]));

	Array<String> arr2 = r.enumerateNameSpaces();

	BubbleSort(arr2);

#if 0
	for (Uint32 i = 0; i < arr2.getSize(); i++)
	    cout << "===>" << arr2[i] << endl;
#endif

	assert(arr1.getSize() == 3);
	assert(arr2.getSize() == 3);
	assert(arr1 == arr2);
    }
    catch (AlreadyExists&)
    {
	cout << "ignored already exists exception" << endl;
    }
}

int main()
{
    CIMRepository r(".");

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
