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
// $Log: Abstract.cpp,v $
// Revision 1.1  2001/01/14 19:53:43  mike
// Initial revision
//
//
//END_HISTORY

#include <Pegasus/Common/ClassDecl.h>
#include <Pegasus/Common/DeclContext.h>

using namespace Pegasus;
using namespace std;

int main()
{
    try
    {
	const String NAMESPACE = "/zzz";

	// Create and populate a declaration context:

	SimpleDeclContext* context = new SimpleDeclContext;

	context->addQualifierDecl(NAMESPACE, QualifierDecl(
	    "abstract", false, Scope::CLASS, Flavor::OVERRIDABLE));

	// Create some classes:

	ClassDecl class1("PeskySuperClass");
	class1.addQualifier(Qualifier("abstract", true));

	ClassDecl class2("Class", "PeskySuperClass");

	class1.resolve(context, NAMESPACE);
	context->addClassDecl(NAMESPACE, class1);

	class2.resolve(context, NAMESPACE);
	context->addClassDecl(NAMESPACE, class2);

	// class1.print();
	// class2.print();
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
