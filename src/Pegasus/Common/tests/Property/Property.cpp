//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
static char * verbose;			// controls test IO

void test01()
{
    CIMProperty pnull;

    assert(pnull.isNull());

    CIMProperty p1("message", String("Hi There"));
    p1.addQualifier(CIMQualifier("Key", true));
    p1.addQualifier(CIMQualifier("stuff", true));
    p1.addQualifier(CIMQualifier("stuff2", true));
    p1.addQualifier(CIMQualifier("Description", String("Blah Blah")));
    CIMConstProperty p2 = p1;

    // Test clone
    CIMProperty p1clone = p1.clone(true);
    CIMProperty p2clone = p2.clone(true);
   
    // Test print

    if(verbose)
    {
	   XmlWriter::printPropertyElement(p1, cout);
	   XmlWriter::printPropertyElement(p2, cout);
	   XmlWriter::printPropertyElement(p1clone, cout);
	   XmlWriter::printPropertyElement(p2clone, cout);
    }

    // Test toMof
       Array<Sint8> mofOut;
       MofWriter::appendPropertyElement(mofOut, p1);
       MofWriter::appendPropertyElement(mofOut, p2);

    // Test toXml
       Array<Sint8> xmlOut;
       XmlWriter::appendPropertyElement(xmlOut, p1);
       XmlWriter::appendPropertyElement(xmlOut, p2);

    // Test name
        String name;
        name = p1.getName();
        assert(name == "message");
        name = p2.getName();
        assert(name == "message");

    // Test type
        assert(p1.getType() == CIMType::STRING);
        assert(p2.getType() == CIMType::STRING);

    // Test isKey
        assert(p1.isKey() == true);
        assert(p2.isKey() == true);

    // Test getArraySize
        assert(p1.getArraySize() == 0);
        assert(p2.getArraySize() == 0);

    // Test getPropagated
        assert(p1.getPropagated() == false);
        assert(p2.getPropagated() == false);

    // Tests for Qualifiers
	assert(p1.findQualifier("stuff") != PEG_NOT_FOUND);
	assert(p1.findQualifier("stuff2") != PEG_NOT_FOUND);
	assert(p1.findQualifier("stuff21") == PEG_NOT_FOUND);
	assert(p1.findQualifier("stuf") == PEG_NOT_FOUND);
	assert(p1.getQualifierCount() == 4);

	assert(p2.findQualifier("stuff") != PEG_NOT_FOUND);
	assert(p2.findQualifier("stuff2") != PEG_NOT_FOUND);
	assert(p2.findQualifier("stuff21") == PEG_NOT_FOUND);
	assert(p2.findQualifier("stuf") == PEG_NOT_FOUND);
	assert(p2.getQualifierCount() == 4);

	assert(p1.existsQualifier("stuff"));
	assert(p1.existsQualifier("stuff2"));

	assert(!p1.existsQualifier("stuff21"));
	assert(!p1.existsQualifier("stuf"));

	Uint32 posQualifier;
	posQualifier = p1.findQualifier("stuff");
	assert(posQualifier != PEGASUS_NOT_FOUND);
	assert(posQualifier < p1.getQualifierCount());

	p1.removeQualifier(posQualifier);
	assert(p1.getQualifierCount() == 3);
	assert(!p1.existsQualifier("stuff"));
	assert(p1.existsQualifier("stuff2"));

    // Tests for value insertion.
    {
           CIMProperty p1("p1", String("Hi There"));
           // test for CIMValue and type
           CIMProperty p2("p2", Uint32(999));
           // test for CIMValue and type

	   //Test getName and setName
	   assert(p1.getName() == "p1");
	   p1.setName("px");
	   assert(p1.getName() == "px");

	   assert(p2.getName() == "p2");
	   p2.setName("py");
	   assert(p2.getName() == "py");

	   // Test setValue and getValue
    }
}

void test02()
{
    // Tests for CIMConstProperty methods
        CIMProperty p1("message", String("Hi There"));
        p1.addQualifier(CIMQualifier("Key", true));
        p1.addQualifier(CIMQualifier("stuff", true));
        p1.addQualifier(CIMQualifier("stuff2", true));
        p1.addQualifier(CIMQualifier("Description", String("Blah Blah")));
        CIMConstProperty p2 = p1;

        CIMConstProperty cp1 = p1;
        CIMConstProperty cp2 = p2;
        CIMConstProperty cp3("message3", "hello");
        CIMConstProperty cp1clone = cp1.clone(true);

        if(verbose)
	    XmlWriter::printPropertyElement(cp1, cout);

        Array<Sint8> mofOut;
        MofWriter::appendPropertyElement(mofOut, cp1);
        Array<Sint8> xmlOut;
        XmlWriter::appendPropertyElement(xmlOut, cp1);

        assert(cp1.getName() == "message");
        assert(cp1.getType() == CIMType::STRING);
        assert(cp1.isKey() == true);
        assert(cp1.getArraySize() == 0);
        assert(cp1.getPropagated() == false);

	assert(cp1.findQualifier("stuff") != PEG_NOT_FOUND);
	assert(cp1.findQualifier("stuff2") != PEG_NOT_FOUND);
	assert(cp1.findQualifier("stuff21") == PEG_NOT_FOUND);
	assert(cp1.findQualifier("stuf") == PEG_NOT_FOUND);
	assert(cp1.getQualifierCount() == 4);
 
        try 
        {
            p1.getQualifier(0);
        }
        catch(OutOfBounds& e)
        {
			if(verbose)
				cout << "Exception: " << e.getMessage() << endl;	
        }
}

void test03()
{
    CIMPropertyList list1;
    CIMPropertyList list2;

    Array<String> names;
    names.append("property1");
    names.append("property2");
    names.append("property3");
    list1.set(names);
    list2 = list1;

    list1.clear();
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    try
    {
        test01();
        test02();
        test03();
    }
    catch (Exception& e)
    {
        cout << "Exception: " << e.getMessage() << endl;
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
