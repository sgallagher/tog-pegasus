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
// $Log: Array.cpp,v $
// Revision 1.1  2001/01/14 19:53:43  mike
// Initial revision
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include "Str.h"
#include "Int.h"

using namespace Pegasus;
using namespace std;

template<class T>
void Print(const Array<T>& arr)
{
    for (size_t i = 0; i < arr.getSize(); i++)
	cout << arr[i] << endl;
    cout << "-- end" << endl;
}

template<class STR>
void test01(STR*)
{
    Array<STR> arr(3, STR("Hello"));
    assert(arr.getSize() == 3);
    assert(arr[0] == STR("Hello"));
    assert(arr[1] == STR("Hello"));
    assert(arr[2] == STR("Hello"));
}

template<class STR>
void test02(STR*)
{
    Array<STR> arr;
    assert(arr.getSize() == 0);

    arr.append("three");
    arr.append("four");
    arr.prepend("one");
    arr.prepend("zero");
    arr.insert(2, "two");

    assert(arr.getSize() == 5);
    assert(arr[0] == "zero");
    assert(arr[1] == "one");
    assert(arr[2] == "two");
    assert(arr[3] == "three");
    assert(arr[4] == "four");

    arr.remove(2);
    assert(arr.getSize() == 4);
    assert(arr[0] == "zero");
    assert(arr[1] == "one");
    assert(arr[2] == "three");
    assert(arr[3] == "four");
}

template<class T>
void test03(const T*)
{
    Array<T> arr;

    Uint32 tmp1[] = { 1, 2, 3 };

    arr.insert(0, 2);
    arr.insert(0, 1);
    arr.insert(0, 0);
    arr.append(3);
    arr.insert(4, 4);

    Array<T> arr2 = arr;

    Array<T> arr3;
    arr3 = arr2;
    arr = arr3;

    assert(arr.getSize() == 5);
    assert(arr[0] == 0);
    assert(arr[1] == 1);
    assert(arr[2] == 2);
    assert(arr[3] == 3);
    assert(arr[4] == 4);

    arr.remove(4);
    assert(arr.getSize() == 4);
    assert(arr[0] == 0);
    assert(arr[1] == 1);
    assert(arr[2] == 2);
    assert(arr[3] == 3);

    arr.remove(0);
    assert(arr.getSize() == 3);
    assert(arr[0] == 1);
    assert(arr[1] == 2);
    assert(arr[2] == 3);

    arr.remove(0);
    arr.remove(1);
    assert(arr.getSize() == 1);
    assert(arr[0] == 2);

    arr.remove(0);
    assert(arr.getSize() == 0);
}

void test04()
{
    Array<String> arr(3, "Hello");
    assert(arr.getSize() == 3);
    assert(arr[0] == "Hello");
    assert(arr[1] == "Hello");
    assert(arr[2] == "Hello");
}

void test05()
{
    Array<String> arr;
    assert(arr.getSize() == 0);

    arr.append("three");
    arr.append("four");
    arr.prepend("one");
    arr.prepend("zero");
    arr.insert(2, "two");

    assert(arr.getSize() == 5);
    assert(arr[0] == "zero");
    assert(arr[1] == "one");
    assert(arr[2] == "two");
    assert(arr[3] == "three");
    assert(arr[4] == "four");

    arr.remove(2);
    assert(arr.getSize() == 4);
    assert(arr[0] == "zero");
    assert(arr[1] == "one");
    assert(arr[2] == "three");
    assert(arr[3] == "four");
}

int main()
{
    try
    {
	test01((Str*)0);
	test02((Str*)0);
	test01((String*)0);
	test02((String*)0);
	test03((Int*)0);
	test03((int*)0);
	test04();
	test05();
	assert(Int::_count == 0);
	assert(Str::_constructions == Str::_destructions);
    }
    catch(Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
