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
// $Log: Repository.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:53:59  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Repository/Repository.h>

using namespace Pegasus;
using namespace std;

void test()
{
    Repository r(".");

    const String NAMESPACE = "aa/bb";

    try
    {
	r.createNameSpace(NAMESPACE);
    }
    catch (AlreadyExists&)
    {
	cout << "ignored already exists exception" << endl;
    }

    ClassDecl c("MyClass");
    c.addProperty(Property("count", Uint32(0)));
    c.addProperty(Property("ratio", Real32(1.5)));
    c.addProperty(Property("message", "Hello World"));

    r.createClass(NAMESPACE, c);

    ConstClassDecl cc;
    cc = r.getClass("aa/bb", "MyClass");

    assert(c.identical(cc));
    assert(cc.identical(c));

    // cc.print();
}

int main()
{
    Repository r(".");

    try 
    {
	test();
    }
    catch (Exception& e)
    {
	cout << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
