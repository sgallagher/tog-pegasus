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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
static char * verbose;

void test01()
{
    CIMParamValue pv;
    assert(pv.isUninitialized());

    String p1("message");
    CIMValue v1(String("argument_Test"));
    CIMParamValue a1(p1, v1);

//  Test call to CIMParamValue::setValue(CIMValue& value)
//  this test uses the above values for a1, p1 and v1.
    CIMParamValue a7(p1, v1);
    CIMValue v7(String("argument_Test7"));
    a7.setValue( v7);
    assert(a1.getValue().toString() != a7.getValue().toString());
    assert(a1.getParameterName() == a7.getParameterName());

//  Test call to CIMParamValue::setParameterName(String& parameterName)
//  this test uses the above values for a1, p1 and v1.
    CIMParamValue a8(p1, v1);
    String p8("message8");
    a8.setParameterName( p8);
    assert(a1.getValue().toString() == a8.getValue().toString());
    assert(a1.getParameterName() != a8.getParameterName());
    assert(a8.isTyped());
    a8.setIsTyped(false);
    assert(!a8.isTyped());

    String p2("message2");
    CIMValue v2(String("argument_Test2"));
    CIMParamValue a2(p2, v2);

    String p3("message3");
    CIMValue v3(String("argument_Test3"));
    CIMParamValue a3(p3, v3);

    String p4("message4");
    CIMValue v4(String("argument_Test4"));
    CIMParamValue a4(p4, v4);
    CIMParamValue a5 = a4;

    String p6("message6");
    CIMValue v6(String("argument_Test6"));
    CIMParamValue a6(p6, v6);

    Array<CIMParamValue> aa;
    aa.append(a1);
    aa.append(a2);

    aa.append(CIMParamValue("message3", CIMValue(200000)));

    aa.append(CIMParamValue("message4", CIMValue(String("test4"))));
    
    //
    // clone
    //
    CIMParamValue a4clone = a4.clone();
    aa.append(a4clone);

    if (verbose)
    {
        for (Uint32 i=0; i< aa.size(); i++)
        {
            XmlWriter::printParamValueElement(aa[i], cout);
        }
    }

    //
    // toXml
    //
    Array<char> xmlOut;
    XmlWriter::appendParamValueElement(xmlOut, a4clone);
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
