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
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMScope.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;	 // Flag to control test IO

void test01()
{
    CIMQualifierDecl qual1(
        CIMName ("CIMTYPE"), 
        String(),
        CIMScope::PROPERTY,
        CIMFlavor::TOINSTANCE);

    if(verbose)
		XmlWriter::printQualifierDeclElement(qual1);

    CIMQualifierDecl q2(CIMName ("Abstract"), true, CIMScope::CLASS);
    CIMQualifierDecl q3 = q2;
    
    //
    // Test getName and setName
    //
    assert(qual1.getName() == CIMName ("CIMTYPE"));
    qual1.setName(CIMName ("Aggregate"));
    assert(qual1.getName() == CIMName ("Aggregate"));

    //
    // Test getType 
    //
    assert(qual1.getType() == CIMTYPE_STRING);

    //
    // Test getScope and getFlavor
    //
    assert(qual1.getScope().equal (CIMScope::PROPERTY));
    assert(qual1.getFlavor ().hasFlavor (CIMFlavor::TOINSTANCE));
	// This should be the defaults if nothing specified.
    assert(qual1.getFlavor ().hasFlavor (CIMFlavor::TOSUBCLASS));
    assert(qual1.getFlavor ().hasFlavor (CIMFlavor::OVERRIDABLE));

	CIMQualifierDecl q4(CIMName ("q4"), true, CIMScope::CLASS, 
            CIMFlavor::RESTRICTED);
    assert(!q4.getFlavor ().hasFlavor (CIMFlavor::TOINSTANCE));
    assert(!q4.getFlavor ().hasFlavor (CIMFlavor::TOSUBCLASS));
    assert(q4.getFlavor ().hasFlavor (CIMFlavor::OVERRIDABLE));

	CIMQualifierDecl q5(CIMName ("q5"), true, CIMScope::CLASS, 
            CIMFlavor::DISABLEOVERRIDE);
    assert(!q5.getFlavor ().hasFlavor (CIMFlavor::TOINSTANCE));
    assert(q5.getFlavor ().hasFlavor (CIMFlavor::TOSUBCLASS));
    assert(!q5.getFlavor ().hasFlavor (CIMFlavor::OVERRIDABLE));

    //
    //
    // Test getValue and setValue
    //
    CIMValue v1(String("qualifier1"));
    qual1.setValue(v1);
    CIMValue v2 = qual1.getValue();;
    assert(v1 == v2);

    //
    // Test isArray and getArraySize
    //
    assert(qual1.isArray() == false);
    assert(qual1.getArraySize() == 0);

    //
    // Test clone
    //
    CIMQualifierDecl qualclone = qual1.clone();
    assert(qualclone.identical(qual1));

    //
    // Test toMof 
    //
    Array<char> mofOut;
    MofWriter::appendQualifierDeclElement(mofOut, qual1);

    CIMConstQualifierDecl cq1; 
    cq1 = qualclone;
}

void test02()
{
    //
    // Test CIMConstQualifierDecl class methods
    //
    CIMConstQualifierDecl cq1(
        CIMName ("CIMTYPE"), 
        String(),
        CIMScope::PROPERTY,
        CIMFlavor::TOINSTANCE);

    if(verbose)
		XmlWriter::printQualifierDeclElement(cq1);

    CIMConstQualifierDecl cq2(CIMName ("Abstract"), true, CIMScope::CLASS);
    CIMConstQualifierDecl cq3;
    cq3 = cq2;
    CIMConstQualifierDecl cq4;
  
    //
    // Test getName and setName
    //
    assert(cq1.getName() == CIMName ("CIMTYPE"));

    //
    // Test getType 
    //
    assert(cq1.getType() == CIMTYPE_STRING);

    //
    // Test getScope and getFlavor
    //
    assert(cq1.getScope().equal (CIMScope::PROPERTY));
    assert(cq1.getFlavor ().hasFlavor (CIMFlavor::TOINSTANCE));

    //
    // Test getValue and setValue
    //
    CIMValue v1(String(""));
    CIMValue v2 = cq1.getValue();;
    assert(v1 == v2);

    //
    // Test isArray and getArraySize
    //
    assert(cq1.isArray() == false);
    assert(cq1.getArraySize() == 0);

    //
    // Test clone
    //
    CIMConstQualifierDecl cqclone = cq1.clone();

    //
    // Test identical
    //
    assert(cqclone.identical(cq1) == true);
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    try
    {
        test01();
        test02();
    }
    catch(Exception& e)
    {
        cerr << "Exception: " << e.getMessage() << endl;
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
