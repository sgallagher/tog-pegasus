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
// $Log: ClassDeclRep.cpp,v $
// Revision 1.6  2001/02/26 04:33:30  mike
// Fixed many places where cim names were be compared with operator==(String,String).
// Changed all of these to use CIMName::equal()
//
// Revision 1.5  2001/02/19 01:47:17  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.4  2001/02/18 19:02:18  mike
// Fixed CIM debacle
//
// Revision 1.3  2001/02/18 03:56:01  mike
// Changed more class names (e.g., ConstClassDecl -> ConstCIMClass)
//
// Revision 1.2  2001/02/16 02:06:09  mike
// Renamed many classes and headers.
//
// Revision 1.1.1.1  2001/01/14 19:53:56  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/FileSystem.h>

using namespace Pegasus;
using namespace std;

void Test01()
{
    CIMRepository r(".");

    // Create a namespace:

    const String NAMESPACE = "/zzz";
    r.createNameSpace(NAMESPACE);

    // Create a qualifier (and read it back):

    CIMQualifierDecl q1("abstract", false, CIMScope::CLASS);
    r.setQualifier(NAMESPACE, q1);

    CIMConstQualifierDecl q2 = r.getQualifier(NAMESPACE, "abstract");
    assert(q1.identical(q2));

    // Create two simple classes:

    CIMClass class1("Class1");
    class1.addQualifier(CIMQualifier("abstract", true));
    CIMClass class2("Class2", "Class1");

    r.createClass(NAMESPACE, class1);
    r.createClass(NAMESPACE, class2);

    // Enumerate the class names:

    Array<String> classNames = 
	r.enumerateClassNames(NAMESPACE, String::EMPTY, true);

    BubbleSort(classNames);

    // Print(classNames);

    assert(classNames.getSize() == 2);
    assert(CIMName::equal(classNames[0], "Class1"));
    assert(CIMName::equal(classNames[1], "Class2"));

    // Attempt to delete Class1. It should fail since the class has
    // children.

    try
    {
	r.deleteClass(NAMESPACE, "Class1");
    }
    catch (CimException& e)
    {
	assert(e.getCode() == CimException::CLASS_HAS_CHILDREN);
    }

    // Delete all classes created here:

    r.deleteClass(NAMESPACE, "Class2");
    r.deleteClass(NAMESPACE, "Class1");

    // Be sure the class files are really gone:

    Array<String> tmp;
    assert(FileSystem::getDirectoryContents("./repository/#zzz/classes", tmp));
    assert(tmp.getSize() == 0);
}

int main()
{
    try 
    {
	Test01();
    }
    catch (Exception& e)
    {
	cout << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
