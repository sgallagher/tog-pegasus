//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: String.cpp,v $
// Revision 1.2  2001/02/11 05:42:33  mike
// new
//
// Revision 1.1.1.1  2001/01/14 19:53:49  mike
// Pegasus import
//
//
//END_HISTORY


#include <cassert>
#include <strstream>
#include <Pegasus/Common/String.h>

using namespace Pegasus;
using namespace std;

int main()
{
    String s1 = "Hello World";
    String s2 = s1;
    String s3(s2);

    assert(s1 == s3);

    String s4 = "Hello";
    s4.append(Char16(0x0000));
    s4.append(Char16(0x1234));
    s4.append(Char16(0x5678));
    s4.append(Char16(0x9cde));
    s4.append(Char16(0xffff));

    {
	ostrstream os;
	os << s4;
	os.put('\0');
	const char EXPECTED[] = "Hello\\x0000\\x1234\\x5678\\x9CDE\\xFFFF";
	char* tmp = os.str();
	assert(strcmp(EXPECTED, tmp) == 0);
	delete [] tmp;
    }

    {
	const char STR0[] = "one two three four";
	String s = STR0;
	char* tmp = s.allocateCString();
	assert(strcmp(tmp, STR0) == 0);
	const char STR1[] = "zero ";
	delete [] tmp;
    }

    {
	const char STR0[] = "one two three four";
	String s = STR0;
	const char STR1[] = "zero ";
	char* tmp = new char[strlen(STR1) + s.getLength() + 1];
	strcpy(tmp, STR1);
	s.appendToCString(tmp);
	assert(strcmp(tmp, "zero one two three four") == 0);
	delete [] tmp;
    }

    {
	const char STR0[] = "one two three four";
	String s = STR0;
	const char STR1[] = "zero ";
	char* tmp = new char[strlen(STR1) + s.getLength() + 1];
	strcpy(tmp, STR1);
	s.appendToCString(tmp, 7);
	assert(strcmp(tmp, "zero one two") == 0);
	delete [] tmp;
    }

    {
	String s = "abcdefg";
	s.remove(3, 3);
	assert(s == "abcg");
	assert(s.getLength() == 4);

	s = "abcdefg";
	s.remove(3, 4);
	assert(s == "abc");
	assert(s.getLength() == 3);

	s = "abcdefg";
	s.remove(3);
	assert(s == "abc");
	assert(s.getLength() == 3);

	s = "abc";
	s.remove(3);
	assert(s == "abc");
	assert(s.getLength() == 3);

	s = "abc";
	s.remove(0);
	assert(s == "");
	assert(s.getLength() == 0);

	s = "abc";
	s.remove(0, 1);
	assert(s == "bc");
	assert(s.getLength() == 2);

	String t1 = "HELLO";
	String t2 = ToLower(t1);
	assert(t1 == "HELLO");
	assert(t2 == "hello");
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
