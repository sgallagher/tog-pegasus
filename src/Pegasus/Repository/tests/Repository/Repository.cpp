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
// Revision 1.5  2001/02/18 19:02:18  mike
// Fixed CIM debacle
//
// Revision 1.4  2001/02/18 03:56:01  mike
// Changed more class names (e.g., ConstClassDecl -> CIMConstClass)
//
// Revision 1.3  2001/02/16 02:06:09  mike
// Renamed many classes and headers.
//
// Revision 1.2  2001/02/13 07:00:18  mike
// Added partial createInstance() method to repository.
//
// Revision 1.1.1.1  2001/01/14 19:53:59  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

using namespace Pegasus;
using namespace std;

void test01()
{
    CIMRepository r(".");

    const String NAMESPACE = "aa/bb";

    try
    {
	r.createNameSpace(NAMESPACE);
    }
    catch (AlreadyExists&)
    {
	// Ignore this!
    }

    CIMClass c("MyClass");

    r.setQualifier(NAMESPACE, CIMQualifierDecl("key", true, CIMScope::PROPERTY));

    c.addProperty(
	CIMProperty("key", Uint32(0))
	    .addQualifier(CIMQualifier("key", true)))
	.addProperty(CIMProperty("ratio", Real32(1.5)))
	.addProperty(CIMProperty("message", "Hello World"));

    r.createClass(NAMESPACE, c);

    CIMConstClass cc;
    cc = r.getClass("aa/bb", "MyClass");

    assert(c.identical(cc));
    assert(cc.identical(c));

    // cc.print();
}

void test02()
{
    CIMRepository r(".");

    const String NAMESPACE = "aa/bb";

    try
    {
	r.createNameSpace(NAMESPACE);
    }
    catch (AlreadyExists&)
    {
	// Ignore this!
    }

    CIMInstance inst("MyClass");
    inst.addProperty(CIMProperty("key", Uint32(0)));

    r.createInstance(NAMESPACE, inst);
}

int main()
{
    CIMRepository r(".");

    try 
    {
	test01();
	test02();
    }
    catch (Exception& e)
    {
	cout << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
