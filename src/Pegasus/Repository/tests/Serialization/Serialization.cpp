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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Repository/Serialization.h>
#include <Pegasus/Common/PegasusAssert.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static void _dump(const char* s, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        char c = s[i];

        if (isprint(c))
            printf("%c", c);
        else
            printf("[%02X]", (unsigned char)c);
    }

    printf("\n");
}

void test()
{
    // Create embedded instance:

    CIMInstance emb("EmbClass");
    emb.addProperty(CIMProperty("Key", Uint32(777)));

    // Create instance:

    CIMInstance ci1("MyClass");
    CIMObjectPath cop("MyClass.Key=7777");
    ci1.setPath(cop);
    ci1.addProperty(CIMProperty("Key", Uint32(7777)));
    ci1.addProperty(CIMProperty("Message", String("Hello World!")));
    ci1.addProperty(CIMProperty("Count", Uint32(1234)));
    ci1.addProperty(CIMProperty("Flag", Boolean(true)));

    Array<String> colors;
    colors.append("Red");
    colors.append("Green");
    colors.append("Blue");
    ci1.addProperty(CIMProperty("Colors", colors));

    ci1.addProperty(CIMProperty("Ref1", 
        CIMObjectPath("RefClass.Key1=99,Key2=\"Hello\",Key3=False")));

    ci1.addProperty(CIMProperty("Emb1", emb));

    // Serialize instance:

    Buffer out;
    {
        Array<String> tbl;
        SerializeInstance(tbl, out, ci1);
        SerializeInstance(tbl, out, ci1);
    }

    // _dump(out.getData(), out.size());

    // Deserialize instance:

    CIMInstance ci2;
    {
        Buffer in(out);
        Array<String> tbl;
        size_t pos = 0;

        if (DeserializeInstance(tbl, in, pos, ci2) != 0)
            PEGASUS_TEST_ASSERT(0);
    }

    // Check instance:

    PEGASUS_TEST_ASSERT(ci1.identical(ci2));

    // Print
#if 0
    {
        CIMObject co1(ci1);
        cout << co1.toString() << endl;
        printf("===========================================\n");
        CIMObject co2(ci2);
        cout << co2.toString() << endl;
    }
#endif
}

int main(int argc, char** argv)
{
    try
    {
        test();
    }
    catch (...)
    {
        PEGASUS_TEST_ASSERT(0);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
