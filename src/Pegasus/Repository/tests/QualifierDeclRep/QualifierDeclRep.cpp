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
// $Log: QualifierDeclRep.cpp,v $
// Revision 1.1  2001/01/14 19:53:59  mike
// Initial revision
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

    // Create a namespace:

    const String NAMESPACE = "/zzz";
    const String ABSTRACT = "abstract";
    r.createNameSpace(NAMESPACE);

    // Create a qualifier declaration:

    QualifierDecl q1(ABSTRACT, Boolean(true), Scope::CLASS);
    r.setQualifier(NAMESPACE, q1);

    // Get it back and check to see if it is identical:

    ConstQualifierDecl q2 = r.getQualifier(NAMESPACE, ABSTRACT);

    assert(q1.identical(q2));

    // Remove it now:

    r.deleteQualifier(NAMESPACE, ABSTRACT);

    // Try to get it again (this should fail with a not-found error):

    try
    {
	q2 = r.getQualifier(NAMESPACE, ABSTRACT);
    }
    catch (CimException& e)
    {
	assert(e.getCode() == CimException::NOT_FOUND);
    }

    // Create two qualifiers:

    QualifierDecl q3("q3", Uint32(66), Scope::CLASS);
    QualifierDecl q4("q4", String("Hello World"), Scope::CLASS);

    r.setQualifier(NAMESPACE, q3);
    r.setQualifier(NAMESPACE, q4);

    // Enumerate the qualifier names:

    Array<QualifierDecl> qualifiers = r.enumerateQualifiers(NAMESPACE);

    assert(qualifiers.getSize() == 2);

    for (Uint32 i = 0, n = qualifiers.getSize(); i < n; i++)
    {
	// qualifiers[i].print();

	assert(qualifiers[i].identical(q3) || qualifiers[i].identical(q4));
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
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
