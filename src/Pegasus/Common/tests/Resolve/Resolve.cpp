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
// $Log: Resolve.cpp,v $
// Revision 1.2  2001/01/23 01:25:35  mike
// Reworked resolve scheme.
//
// Revision 1.1.1.1  2001/01/14 19:53:48  mike
// Pegasus import
//
//
//END_HISTORY

#include <Pegasus/Common/ClassDecl.h>
#include <Pegasus/Common/DeclContext.h>

using namespace Pegasus;
using namespace std;

void test01()
{
    const String NAMESPACE = "/ttt";

    SimpleDeclContext* context = new SimpleDeclContext;
    ClassDecl class2("YourClass", "");
    context->addClassDecl(NAMESPACE, class2);
    class2.resolve(context, NAMESPACE);
    
    ClassDecl class1("MyClass", "YourClass");

    class1
	.addQualifier(Qualifier("q1", Uint32(55)))
	.addQualifier(Qualifier("q2", "Hello"))
	.addProperty(Property("message", String("Hello")))
	.addProperty(Property("count", Uint32(77)))
	// .addProperty(Property("ref1", Reference("MyClass.key1=\"fred\"")))
	.addMethod(Method("isActive", Type::BOOLEAN)
	    .addParameter(Parameter("hostname", Type::STRING))
	    .addParameter(Parameter("port", Type::UINT32)));

    class1.print();
    class1.resolve(context, NAMESPACE);
}

int main()
{
    try
    {
	test01();
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
