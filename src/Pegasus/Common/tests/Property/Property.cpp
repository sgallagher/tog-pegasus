//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Karl Schopmeyer(k.schopmeyer@opengroup.org)
//              David Dilard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Common/CIMObjectPath.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
static char * verbose;			// controls test IO

void test01()
{
    CIMProperty pnull;

    assert(pnull.isUninitialized());

    CIMProperty p1(CIMName ("message"), String("Hi There"));
    p1.addQualifier(CIMQualifier(CIMName ("Key"), true));
    p1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
    p1.addQualifier(CIMQualifier(CIMName ("stuff2"), true));
    p1.addQualifier(CIMQualifier(CIMName ("Description"), String("Blah Blah")));
    CIMConstProperty p2 = p1;

    // Test clone
    CIMProperty p1clone = p1.clone();
    CIMProperty p2clone = p2.clone();
   
    // Test print

    if(verbose)
    {
	   XmlWriter::printPropertyElement(p1, cout);
	   XmlWriter::printPropertyElement(p2, cout);
	   XmlWriter::printPropertyElement(p1clone, cout);
	   XmlWriter::printPropertyElement(p2clone, cout);
    }

    // Test toMof
       Array<char> mofOut;
       MofWriter::appendPropertyElement(mofOut, p1);
       MofWriter::appendPropertyElement(mofOut, p2);

    // Test toXml
       Array<char> xmlOut;
       XmlWriter::appendPropertyElement(xmlOut, p1);
       XmlWriter::appendPropertyElement(xmlOut, p2);

    // Test name
        CIMName name;
        name = p1.getName();
        assert(name == CIMName ("message"));
        name = p2.getName();
        assert(name == CIMName ("message"));

    // Test type
        assert(p1.getType() == CIMTYPE_STRING);
        assert(p2.getType() == CIMTYPE_STRING);

    // Test for key qualifier
        Uint32 pos;
        Boolean isKey = false;
        if ((pos = p1.findQualifier (CIMName ("key"))) != PEG_NOT_FOUND)
        {
            CIMValue value;
            value = p1.getQualifier (pos).getValue ();
            if (!value.isNull ())
            {
                value.get (isKey);
            }
        }
        assert (isKey);
        isKey = false;
        if ((pos = p2.findQualifier (CIMName ("key"))) != PEG_NOT_FOUND)
        {
            CIMValue value;
            value = p2.getQualifier (pos).getValue ();
            if (!value.isNull ())
            {
                value.get (isKey);
            }
        }
        assert (isKey);

    // Test getArraySize
        assert(p1.getArraySize() == 0);
        assert(p2.getArraySize() == 0);

    // Test getPropagated
        assert(p1.getPropagated() == false);
        assert(p2.getPropagated() == false);

    // Tests for Qualifiers
	assert(p1.findQualifier(CIMName ("stuff")) != PEG_NOT_FOUND);
	assert(p1.findQualifier(CIMName ("stuff2")) != PEG_NOT_FOUND);
	assert(p1.findQualifier(CIMName ("stuff21")) == PEG_NOT_FOUND);
	assert(p1.findQualifier(CIMName ("stuf")) == PEG_NOT_FOUND);
	assert(p1.getQualifierCount() == 4);

	assert(p2.findQualifier(CIMName ("stuff")) != PEG_NOT_FOUND);
	assert(p2.findQualifier(CIMName ("stuff2")) != PEG_NOT_FOUND);
	assert(p2.findQualifier(CIMName ("stuff21")) == PEG_NOT_FOUND);
	assert(p2.findQualifier(CIMName ("stuf")) == PEG_NOT_FOUND);
	assert(p2.getQualifierCount() == 4);

	assert(p1.findQualifier(CIMName ("stuff")) != PEG_NOT_FOUND);
	assert(p1.findQualifier(CIMName ("stuff2")) != PEG_NOT_FOUND);

	assert(p1.findQualifier(CIMName ("stuff21")) == PEG_NOT_FOUND);
	assert(p1.findQualifier(CIMName ("stuf")) == PEG_NOT_FOUND);

	Uint32 posQualifier;
	posQualifier = p1.findQualifier(CIMName ("stuff"));
	assert(posQualifier != PEG_NOT_FOUND);
	assert(posQualifier < p1.getQualifierCount());

	p1.removeQualifier(posQualifier);
	assert(p1.getQualifierCount() == 3);
	assert(p1.findQualifier(CIMName ("stuff")) == PEG_NOT_FOUND);
	assert(p1.findQualifier(CIMName ("stuff2")) != PEG_NOT_FOUND);

    // Tests for value insertion.
    {
           CIMProperty p1(CIMName ("p1"), String("Hi There"));
           // test for CIMValue and type
           CIMProperty p2(CIMName ("p2"), Uint32(999));
           // test for CIMValue and type

	   //Test getName and setName
	   assert(p1.getName() == CIMName ("p1"));
	   p1.setName(CIMName ("px"));
	   assert(p1.getName() == CIMName ("px"));

	   assert(p2.getName() == CIMName ("p2"));
	   p2.setName(CIMName ("py"));
	   assert(p2.getName() == CIMName ("py"));

	   // Test setValue and getValue
    }
}

void test02()
{
    // Tests for CIMConstProperty methods
        CIMProperty p1(CIMName ("message"), String("Hi There"));
        p1.addQualifier(CIMQualifier(CIMName ("Key"), true));
        p1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
        p1.addQualifier(CIMQualifier(CIMName ("stuff2"), true));
        p1.addQualifier(CIMQualifier(CIMName ("Description"), String("Blah Blah")));
        CIMConstProperty p2 = p1;

        CIMConstProperty cp1 = p1;
        CIMConstProperty cp2 = p2;
        CIMConstProperty cp3(CIMName ("message3"), String("hello"));
        CIMConstProperty cp1clone = cp1.clone();

        if(verbose)
	    XmlWriter::printPropertyElement(cp1, cout);

        Array<char> mofOut;
        MofWriter::appendPropertyElement(mofOut, cp1);
        Array<char> xmlOut;
        XmlWriter::appendPropertyElement(xmlOut, cp1);

        assert(cp1.getName() == CIMName ("message"));
        assert(cp1.getType() == CIMTYPE_STRING);
        Uint32 pos;
        Boolean isKey = false;
        if ((pos = cp1.findQualifier (CIMName ("key"))) != PEG_NOT_FOUND)
        {
            CIMValue value;
            value = cp1.getQualifier (pos).getValue ();
            if (!value.isNull ())
            {
                value.get (isKey);
            }
        }
        assert (isKey);
        assert(cp1.getArraySize() == 0);
        assert(cp1.getPropagated() == false);

	assert(cp1.findQualifier(CIMName ("stuff")) != PEG_NOT_FOUND);
	assert(cp1.findQualifier(CIMName ("stuff2")) != PEG_NOT_FOUND);
	assert(cp1.findQualifier(CIMName ("stuff21")) == PEG_NOT_FOUND);
	assert(cp1.findQualifier(CIMName ("stuf")) == PEG_NOT_FOUND);
	assert(cp1.getQualifierCount() == 4);
 
        try 
        {
            p1.getQualifier(0);
        }
        catch(IndexOutOfBoundsException& e)
        {
			if(verbose)
				cout << "Exception: " << e.getMessage() << endl;	
        }
}
//
// Test propertyList class
//
void test03()
{
    CIMPropertyList list1;
    CIMPropertyList list2;

    Array<CIMName> names;
    names.append(CIMName ("property1"));
    names.append(CIMName ("property2"));
    names.append(CIMName ("property3"));
    list1.set(names);
    list2 = list1;
	Array<CIMName> names1a = list1.getPropertyNameArray();
	assert(names == names1a);
	assert(list2[0] == CIMName("property1"));
	assert(list2[1] == CIMName("property2"));
	assert(list2[2] == CIMName("property3"));

    list1.clear();
	list2.clear();

	// Test use of empty list.  Note that the requirement for
	// property lists assumes that we must be able to distinguish
	// a NULL property list from an empty property list.  The usages
	// are very different.  NULL means ignore.  Empty means use list
	// but no properties in list.
	Array<CIMName> names2;
	list1.set(names2);
	assert(!list1.isNull());
	assert(list1.size() == 0);
	Array<CIMName> names3 = list1.getPropertyNameArray();
	assert(names3.size() == 0);

}

//Test reference type properties
void test04()
{
    // Create reference type property from string input and reference class name
    String p =  "//localhost/root/SampleProvider:TST_PersonDynamic.Name=\"Father\"";
    CIMObjectPath path = p;
        
    String referenceClassName = "TST_Person";
    CIMProperty p1(CIMName ("message"), path, 0, CIMName(referenceClassName));
    assert(!p1.isArray());
    assert(p1.getReferenceClassName() == CIMName(referenceClassName));
    assert(p1.getType() == CIMTYPE_REFERENCE);
    
    CIMValue v1;
    v1 = p1.getValue();
    assert(v1.getType() ==  CIMTYPE_REFERENCE);
    assert(!v1.isNull());
    CIMObjectPath pathout;
    v1.get(pathout);
    assert( pathout == path );

    if(verbose)
        XmlWriter::printPropertyElement(p1, cout);

    // Now create an empty property, one used in class declaration for a reference
    CIMProperty p2(CIMName ("parent"), CIMObjectPath(), 0, CIMName(referenceClassName));
    assert(!p2.isArray());
    assert(p2.getReferenceClassName() == CIMName(referenceClassName));
    assert(p2.getType() == CIMTYPE_REFERENCE);

    CIMValue v2;
    v2 = p2.getValue();
    assert(v2.getType() ==  CIMTYPE_REFERENCE);

    // ATTN: P3 KS 27 Feb 2003. Why does the following test not work. I assume that the value should
    // b e null in this case.
    //assert(v2.isNull());

    CIMObjectPath pathout2;
    v1.get(pathout2);
    // Now compare the paths

    
    if(verbose)
        XmlWriter::printPropertyElement(p2, cout);
}


int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    try
    {
        test01();
        test02();
        test03();
        test04();
    }
    catch (Exception& e)
    {
        cout << "Exception: " << e.getMessage() << endl;
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
