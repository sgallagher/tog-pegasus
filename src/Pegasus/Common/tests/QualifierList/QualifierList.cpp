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
// $Log: QualifierList.cpp,v $
// Revision 1.1  2001/01/14 19:53:48  mike
// Initial revision
//
//
//END_HISTORY

#include <Pegasus/Common/Qualifier.h>
#include <Pegasus/Common/QualifierList.h>
#include <Pegasus/Common/Property.h>
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

	context->addQualifierDecl(NAMESPACE, QualifierDecl("abstract", 
	    false, Scope::CLASS, Flavor::OVERRIDABLE));

	context->addQualifierDecl(NAMESPACE, QualifierDecl("description", 
	    String(), Scope::CLASS, Flavor::OVERRIDABLE));

	context->addQualifierDecl(NAMESPACE, QualifierDecl("q1", 
	    false, Scope::CLASS, Flavor::OVERRIDABLE | Flavor::TOSUBCLASS));

	context->addQualifierDecl(NAMESPACE, QualifierDecl("q2", 
	    false, Scope::CLASS, Flavor::TOSUBCLASS));

	// Create qualifier list 1:

	QualifierList qualifiers0;
	QualifierList qualifiers1;

	qualifiers1
	    .add(Qualifier("Abstract", true))
	    .add(Qualifier("Description", "Qualifier List 1"))
	    .add(Qualifier("q1", false))
	    .add(Qualifier("q2", false));

	qualifiers1.resolve(
	    context, NAMESPACE, Scope::CLASS, false, qualifiers0);

	// Create qualifier list 2:

	QualifierList qualifiers2;

	qualifiers2
	    .add(Qualifier("Description", "Qualifier List 1"))
	    .add(Qualifier("q1", true, Flavor::OVERRIDABLE));

	qualifiers2.resolve(
	    context, NAMESPACE, Scope::CLASS, false, qualifiers1);
    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
