//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMScope.h>
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    CIMQualifierDecl qual1(
        "CIMTYPE", 
        String(),
        CIMScope::PROPERTY,
        CIMFlavor::TOINSTANCE);

    qual1.print();

    CIMQualifierDecl q2("Abstract", true, CIMScope::CLASS);
    CIMQualifierDecl q3 = q2;
    
    //
    // Test getName and setName
    //
    assert(qual1.getName() == "CIMTYPE");
    qual1.setName("Aggregate");
    assert(qual1.getName() == "Aggregate");

    //
    // Test getType 
    //
    assert(qual1.getType() == CIMType::STRING);

    //
    // Test getScope and getFlavor
    //
    assert(qual1.getScope() == CIMScope::PROPERTY);
    assert(qual1.getFlavor() == CIMFlavor::TOINSTANCE);

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
    assert(qualclone == qual1);

    //
    // Test toMof 
    //
    Array<Sint8> mofOut;
    qual1.toMof(mofOut);

    CIMConstQualifierDecl cq1; 
    cq1 = qualclone;
}

void test02()
{
    //
    // Test CIMConstQualifierDecl class methods
    //
    CIMConstQualifierDecl cq1(
        "CIMTYPE", 
        String(),
        CIMScope::PROPERTY,
        CIMFlavor::TOINSTANCE);

    cq1.print();

    CIMConstQualifierDecl cq2("Abstract", true, CIMScope::CLASS);
    CIMConstQualifierDecl cq3;
    cq3 = cq2;
    CIMConstQualifierDecl cq4;
  
    //
    // Test getName and setName
    //
    assert(cq1.getName() == "CIMTYPE");

    //
    // Test getType 
    //
    assert(cq1.getType() == CIMType::STRING);

    //
    // Test getScope and getFlavor
    //
    assert(cq1.getScope() == CIMScope::PROPERTY);
    assert(cq1.getFlavor() == CIMFlavor::TOINSTANCE);

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
    assert(cqclone == cq1);

    //
    // Test identical
    //
    assert(cqclone.identical(cq1) == true);
}

int main()
{
    try
    {
        test01();
        test02();
    }
    catch(Exception& e)
    {
        cerr << "Exception: " << e.getMessage() << endl;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
