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
#include <Pegasus/Common/CIMParameter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    CIMParameter p;
    assert(!p);

    CIMParameter p1("message", CIMType::STRING);
    p1.addQualifier(CIMQualifier("in", true));
    p1.print(cout);

    CIMParameter p2("message2", CIMType::STRING);
    p2.addQualifier(CIMQualifier("in", true));
    p2.print(cout);

    CIMParameter p3("message3", CIMType::STRING);
    p3.setName("message3a");
    assert(p3.getName() == "message3a");
    p3.print(cout);
    assert(p3.getType() == CIMType::STRING);;

    //
    // clone
    //
    CIMParameter p1clone = p1.clone();
    p1clone.print(cout);

    //
    // toMof
    //
    Array<Sint8> mofOut;
    p1.toMof(mofOut);

    //
    // toXml
    //
    Array<Sint8> xmlOut;
    p1.toXml(xmlOut);

    //
    // identical
    //
    Boolean same;
    same  = p1clone.identical(p1);
    assert(same);

    //
    // not identical
    //
    same = p1clone.identical(p2);
    assert(!same);

    //
    // get qualifier count
    //
    assert(p1.getQualifierCount() == 1);
    assert(p2.getQualifierCount() == 1);
    assert(p3.getQualifierCount() == 0);
 
    //
    // find qualifier
    //
    assert(p1.findQualifier("in") != PEG_NOT_FOUND);
    assert(p2.findQualifier("in") != PEG_NOT_FOUND);
    assert(p2.findQualifier("none") == PEG_NOT_FOUND);
    assert(p3.findQualifier("none") == PEG_NOT_FOUND);

    //
    // get qualifiers
    //
    CIMQualifier q1 = p1.getQualifier(0);
    assert(q1);
    CIMConstQualifier q1const = p1.getQualifier(0);
    assert(q1const);

    //
    // isArray
    //
    assert(p1.isArray() == false);

    //
    // getAraySize
    //
    assert(p1.getAraySize() == 0);

    //
    // test CIMConstParameter methods
    //
    CIMConstParameter cp1 = p1;
    CIMConstParameter cp2 = p2;
    CIMConstParameter cp3 = p3;
    CIMConstParameter cp4("message4", CIMType::STRING);

    cp1.print(cout);

    assert(cp3.getName() == "message3a");
    assert(cp3.getType() == CIMType::STRING);;

    const CIMParameter cp1clone = cp1.clone();
    cp1clone.print(cout);

    cp1.toXml(xmlOut);

    assert(cp1.identical(cp1) == true);
    assert(cp1.identical(cp2) == false);
    assert(cp1.isArray() == false);
    assert(cp1.getAraySize() == 0);
    assert(cp1.getQualifierCount() == 1);
    assert(cp1.findQualifier("in") != PEG_NOT_FOUND);
    CIMConstQualifier cq1 = cp1.getQualifier(0);
    assert(cq1);
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
