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
// $Log: InstanceDecl.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:53:46  mike
// Pegasus import
//
//
//END_HISTORY

#include <Pegasus/Common/InstanceDecl.h>
#include <Pegasus/Common/ClassDecl.h>
#include <Pegasus/Common/DeclContext.h>

using namespace Pegasus;
using namespace std;

void test01()
{
    const String NAMESPACE = "/zzz";

    // Create and populate a declaration context:

    SimpleDeclContext* context = new SimpleDeclContext;

    context->addQualifierDecl(
	NAMESPACE, QualifierDecl("counter", false, Scope::PROPERTY));

    context->addQualifierDecl(
	NAMESPACE, QualifierDecl("min", String(), Scope::PROPERTY));

    context->addQualifierDecl(
	NAMESPACE, QualifierDecl("max", String(), Scope::PROPERTY));

    context->addQualifierDecl(
	NAMESPACE, QualifierDecl("Description", String(), Scope::PROPERTY));

    ClassDecl class1("MyClass");

    class1
	.addProperty(Property("count", Uint32(55))
	    .addQualifier(Qualifier("counter", true))
	    .addQualifier(Qualifier("min", "0"))
	    .addQualifier(Qualifier("max", "1")))
	.addProperty(Property("message", "Hello")
	    .addQualifier(Qualifier("description", "My Message")))
	.addProperty(Property("ratio", Real32(1.5)));

    class1.resolve(context, NAMESPACE);
    context->addClassDecl(NAMESPACE, class1);
    // class1.print();

    InstanceDecl instance1("MyClass");
    instance1.addProperty(Property("message", "Goodbye"));
    instance1.resolve(context, NAMESPACE);
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
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
