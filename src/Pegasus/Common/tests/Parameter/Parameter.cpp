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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMParameter.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
static char * verbose;	  // controls IO from test

void test01()
{
    CIMParameter p;
    assert(p.isNull());

    CIMParameter p1("message", CIMTYPE_STRING);
    p1.addQualifier(CIMQualifier("in", true));
	if(verbose)
		XmlWriter::printParameterElement(p1, cout);

    CIMParameter p2("message2", CIMTYPE_STRING);
    p2.addQualifier(CIMQualifier("in", true));
	if(verbose)
		XmlWriter::printParameterElement(p2, cout);

    CIMParameter p3("message3", CIMTYPE_STRING);
    p3.setName("message3a");
    assert(p3.getName() == "message3a");
	if(verbose)
		XmlWriter::printParameterElement(p3, cout);
    assert(p3.getType() == CIMTYPE_STRING);;

    //
    // clone
    //
    CIMParameter p1clone = p1.clone();
	if(verbose)
		XmlWriter::printParameterElement(p1clone, cout);

    //
    // toMof
    //
    Array<Sint8> mofOut;
	if(verbose)
		MofWriter::appendParameterElement(mofOut, p1);

    //
    // toXml
    //
    Array<Sint8> xmlOut;
	if(verbose)
		XmlWriter::appendParameterElement(xmlOut, p1);

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
    assert(!q1.isNull());
    CIMConstQualifier q1const = p1.getQualifier(0);
    assert(!q1const.isNull());

    //
    // isArray
    //
    assert(p1.isArray() == false);

    //
    // getArraySize
    //
    assert(p1.getArraySize() == 0);

    //
    // test CIMConstParameter methods
    //
    CIMConstParameter cp1 = p1;
    CIMConstParameter cp2 = p2;
    CIMConstParameter cp3 = p3;
    CIMConstParameter cp4("message4", CIMTYPE_STRING);

	if(verbose)
		XmlWriter::printParameterElement(cp1, cout);

    assert(cp3.getName() == "message3a");
    assert(cp3.getType() == CIMTYPE_STRING);;

    const CIMParameter cp1clone = cp1.clone();
	if(verbose)
		XmlWriter::printParameterElement(cp1clone, cout);

    XmlWriter::appendParameterElement(xmlOut, cp1);

    assert(cp1.identical(cp1) == true);
    assert(cp1.identical(cp2) == false);
    assert(cp1.isArray() == false);
    assert(cp1.getArraySize() == 0);
    assert(cp1.getQualifierCount() == 1);
    assert(cp1.findQualifier("in") != PEG_NOT_FOUND);
    CIMConstQualifier cq1 = cp1.getQualifier(0);
    assert(!cq1.isNull());
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    try
    {
        test01();
    }
    catch (Exception& e)
    {
        cout << "Exception: " << e.getMessage() << endl;
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
