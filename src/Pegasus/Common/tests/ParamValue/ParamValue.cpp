//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMParamValue.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

// #define IO

template<class T>
void test01(const T& x)
{
    CIMValue v(x);
    CIMValue v2(v);
    CIMValue v3;
    v3 = v2;
#ifdef IO
    v3.print();
#endif
    try
    {
	T t;
	v3.get(t);
	assert(t == x);
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }
}

int main()
{
    CIMParameter p1("message", CIMType::STRING);
    p1.addQualifier(CIMQualifier("in", true));
    CIMValue v1("argument_Test");
    CIMParamValue a1(p1, v1);

    CIMParameter p2("message2", CIMType::STRING);
    p2.addQualifier(CIMQualifier("in", true));
    CIMValue v2("argument_Test2");
    CIMParamValue a2(p2, v2);

    CIMParameter p3("message3", CIMType::STRING);
    p3.addQualifier(CIMQualifier("in", true));
    CIMValue v3("argument_Test3");
    CIMParamValue a3(p3, v3);

    CIMParameter p4("message4", CIMType::STRING);
    p4.addQualifier(CIMQualifier("in", true));
    CIMValue v4("argument_Test4");
    CIMParamValue a4(p4, v4);

    Array<CIMParamValue> aa;
    //aa.append(a1);
    //aa.append(a2);
    //aa.append(a3);
    //aa.append(a4);

    aa.append(CIMParamValue(CIMParameter("message1", CIMType::STRING), 
		CIMValue("test1")));

    aa.append(CIMParamValue(CIMParameter("message2", CIMType::UINT8 ), 
		CIMValue(2)));

    aa.append(CIMParamValue(CIMParameter("message3", CIMType::UINT16), 
		CIMValue(200000)));

    aa.append(CIMParamValue(CIMParameter("message4", CIMType::STRING), 
		CIMValue("test4")));
    
    //for (int i=0; i< aa.size(); i++)
    //{
    //	aa[i].print(cout);
    //}

    cout << "+++++ passed all tests" << endl;

    return 0;
}
