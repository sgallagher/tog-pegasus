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

#define NEED_STRING_EQUAL

#include <Pegasus/Common/PegasusAssert.h>
#include <iostream>
#include <cctype>
#include <Pegasus/Common/Buffer.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

int main(int argc, char** argv)
{
    // Test Buffer()
    {
    Buffer b;
    PEGASUS_TEST_ASSERT(b.size() == 0);
    }

    // Test Buffer(const Buffer&)
    {
    Buffer b1("abcdefghijklmnopqrstuvwxyz", 26);
    Buffer b2 = b1;
    PEGASUS_TEST_ASSERT(b2.size() == 26);
    PEGASUS_TEST_ASSERT(strcmp(b2.getData(),
                               "abcdefghijklmnopqrstuvwxyz") == 0);
    }

    // Test Buffer(const char*, size_t)
    {
    Buffer b1("abcdefghijklmnopqrstuvwxyz", 26);
    PEGASUS_TEST_ASSERT(b1.size() == 26);
    PEGASUS_TEST_ASSERT(strcmp(b1.getData(),
                               "abcdefghijklmnopqrstuvwxyz") == 0);
    }

    // Test operator=(const Buffer& x)
    {
    Buffer b1("abcdefghijklmnopqrstuvwxyz", 26);
    Buffer b2;
    b2 = b1;
    PEGASUS_TEST_ASSERT(b2.size() == 26);
    PEGASUS_TEST_ASSERT(strcmp(b2.getData(),
                               "abcdefghijklmnopqrstuvwxyz") == 0);
    }

    // Test swap(Buffer& x)
    {
    Buffer b1("abcdefghijklmnopqrstuvwxyz", 26);
    Buffer b2;
    b2.swap(b1);
    PEGASUS_TEST_ASSERT(b1.size() == 0);
    PEGASUS_TEST_ASSERT(b2.size() == 26);
    PEGASUS_TEST_ASSERT(strcmp(b2.getData(),
                               "abcdefghijklmnopqrstuvwxyz") == 0);
    }

    // Test size(), append(), and remove()
    {
    Buffer b("abcd", 4);
    b.append("efg", 3);
    b.append("hijk", 4);
    b.append("lmnop", 5);
    b.append("qrstuv", 6);
    b.append("wxyz", 4);
    PEGASUS_TEST_ASSERT(b.size() == 26);
    PEGASUS_TEST_ASSERT(strcmp(b.getData(), "abcdefghijklmnopqrstuvwxyz") == 0);

    b.remove(0, 4);
    PEGASUS_TEST_ASSERT(strcmp(b.getData(), "efghijklmnopqrstuvwxyz") == 0);

    b.remove(0, 3);
    PEGASUS_TEST_ASSERT(strcmp(b.getData(), "hijklmnopqrstuvwxyz") == 0);

    b.remove(15, 4);
    PEGASUS_TEST_ASSERT(strcmp(b.getData(), "hijklmnopqrstuv") == 0);

    b.remove(4, 5);
    PEGASUS_TEST_ASSERT(strcmp(b.getData(), "hijkqrstuv") == 0);

    b.remove(4, 6);
    PEGASUS_TEST_ASSERT(strcmp(b.getData(), "hijk") == 0);

    b.remove(0, 4);
    PEGASUS_TEST_ASSERT(b.size() == 0);
    }

    // Test get() and set()
    {
    Buffer b("abcdefghijklmnopqrstuvwxyz", 26);

    for (size_t i = 0; i < 26; i++)
    {
        PEGASUS_TEST_ASSERT(b[i] == char(i + 'a'));
        PEGASUS_TEST_ASSERT(b.get(i) == char(i + 'a'));
    }

    for (size_t i = 0; i < 26; i++)
    {
        int c = toupper(b[i]);
        b.set(i, c);
    }

    for (size_t i = 0; i < 26; i++)
    {
        PEGASUS_TEST_ASSERT(b[i] == char(i + 'A'));
        PEGASUS_TEST_ASSERT(b.get(i) == char(i + 'A'));
    }
    }


    // Test reserveCapacity() and append()
    {
    Buffer b;

    for (size_t i = 0; i < 26; i++)
    {
        b.reserveCapacity(i+1);
        b.append(char('a' + i));
    }

    PEGASUS_TEST_ASSERT(b.size() == 26);
    PEGASUS_TEST_ASSERT(b.capacity() >= 26);
    PEGASUS_TEST_ASSERT(strcmp(b.getData(), "abcdefghijklmnopqrstuvwxyz") == 0);
    }

    // Test grow()
    {
    Buffer b;
    b.grow(7, 'A');
    b.grow(7, 'B');
    b.grow(7, 'C');
    b.grow(7, '\0');

    PEGASUS_TEST_ASSERT(b.size() == 28);
    PEGASUS_TEST_ASSERT(memcmp(
        b.getData(), "AAAAAAABBBBBBBCCCCCCC\0\0\0\0\0\0\0", 28) == 0);
    }

    // Test append(char,char,char,char) and
    // append(char,char,char,char,char,char,char,char)
    {
    Buffer b;
    b.append('X', 'X', 'X', 'X');
    b.append('Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y');
    b.append('X', 'X', 'X', 'X');
    b.append('Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y');
    PEGASUS_TEST_ASSERT(strcmp(b.getData(), "XXXXYYYYYYYYXXXXYYYYYYYY") == 0);
    }

    // Test clear()
    {
    Buffer b1;
    b1.clear();
    PEGASUS_TEST_ASSERT(b1.size() == 0);

    Buffer b2;
    b2.append("abcdefghijklmnopqrstuvwxyz", 26);
    b2.clear();
    PEGASUS_TEST_ASSERT(b2.size() == 0);
    }

    // Test a large buffer (1 megabyte).
    {
    Buffer b;
    const size_t MEGABYTE = 1024 * 1024;

    for (size_t i = 0; i < MEGABYTE; i++)
        b.append('A' + (i % 26));

    PEGASUS_TEST_ASSERT(b.size() == MEGABYTE);

    for (size_t i = 0; i < MEGABYTE; i++)
        PEGASUS_TEST_ASSERT(b[i] == 'A' + int(i % 26));

    Buffer b2 = b;
    PEGASUS_TEST_ASSERT(b2.size() == MEGABYTE);
    b2.clear();
    b.clear();
    PEGASUS_TEST_ASSERT(b2.size() == 0);
    PEGASUS_TEST_ASSERT(b.size() == 0);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
