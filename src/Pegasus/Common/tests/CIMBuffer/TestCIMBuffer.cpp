//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMBuffer.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test1()
{
    CIMBuffer cb(1024);
    const Uint32 N = 10000;

    for (Uint32 i = 0; i < N; i++)
    {
        cb.putUint32(i);
    }

    cb.rewind();

    for (Uint32 i = 0; i < N; i++)
    {
        Uint32 x = 0;
        PEGASUS_TEST_ASSERT(cb.getUint32(x));
        PEGASUS_TEST_ASSERT(x == i);
    }
}

void test2()
{
    CIMBuffer cb(1024);
    const Uint32 N = 10000;

    for (Uint32 i = 0; i < N; i++)
    {
        char buffer[32];
        sprintf(buffer, "%u", i);
        cb.putString(buffer);
    }

    cb.rewind();

    for (Uint32 i = 0; i < N; i++)
    {
        String x;
        PEGASUS_TEST_ASSERT(cb.getString(x));
        char buffer[32];
        sprintf(buffer, "%u", i);
        PEGASUS_TEST_ASSERT(x == buffer);
    }
}

void test3()
{
    CIMBuffer cb(1024);
    const Uint32 N = 10000;

    Array<Uint32> x1;

    for (Uint32 i = 0; i < N; i++)
    {
        x1.append(i);
    }

    cb.putUint32A(x1);

    cb.rewind();
    Array<Uint32> x2;
    PEGASUS_TEST_ASSERT(cb.getUint32A(x2));

    PEGASUS_TEST_ASSERT(x2.size() == N);

    for (Uint32 i = 0; i < N; i++)
    {
        PEGASUS_TEST_ASSERT(x1[i] == x2[i]);
    }
}

void test4()
{
    CIMBuffer cb(1024);
    CIMValue a1;
    CIMValue a2;
    CIMValue a3;
    CIMValue b1;
    CIMValue b2;
    CIMValue b3;

    a1.set(String("Hello World"));
    a2.set(Boolean(false));
    a3.set(Uint32(12345));

    cb.putValue(a1);
    cb.putValue(a2);
    cb.putValue(a3);

    cb.rewind();

    PEGASUS_TEST_ASSERT(cb.getValue(b1));
    PEGASUS_TEST_ASSERT(cb.getValue(b2));
    PEGASUS_TEST_ASSERT(cb.getValue(b3));

    PEGASUS_TEST_ASSERT(a1 == b1);
    PEGASUS_TEST_ASSERT(a2 == b2);
    PEGASUS_TEST_ASSERT(a3 == b3);
}

void test5()
{
    CIMBuffer cb(1024);

    CIMObjectPath cop1("Class.Key1=\"hello\",Key2=true,Key3=12345");
    cb.putObjectPath(cop1);
    cb.rewind();
    CIMObjectPath cop2;

    PEGASUS_TEST_ASSERT(cb.getObjectPath(cop2));

    PEGASUS_TEST_ASSERT(cop1 == cop2);
    PEGASUS_TEST_ASSERT(cop1.toString() == cop2.toString());
}

void test6()
{
    CIMBuffer cb(1024);

    CIMInstance ci1("MyClass");
    ci1.addProperty(CIMProperty("Number", Uint32(12345)));
    ci1.addProperty(CIMProperty("Flag", Boolean(true)));
    ci1.addProperty(CIMProperty("Message", String("Hello")));
    cb.putInstance(ci1);

    cb.rewind();
    CIMInstance ci2;
    PEGASUS_TEST_ASSERT(cb.getInstance(ci2));

    PEGASUS_TEST_ASSERT(ci1.identical(ci2));
}

void test7()
{
    CIMBuffer cb(1024);

    CIMObjectPath cop1;
    cb.putObjectPath(cop1);
    cb.rewind();
    CIMObjectPath cop2;

    PEGASUS_TEST_ASSERT(cb.getObjectPath(cop2));

    PEGASUS_TEST_ASSERT(cop1 == cop2);
    PEGASUS_TEST_ASSERT(cop1.toString() == cop2.toString());
}

int main(int argc, char** argv)
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();

    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
}
