//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////
//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cassert>
#include <strstream>
#include <Pegasus/Common/String.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
#include <Pegasus/Common/Exception.h>

static void TestSplit()
{
    {
	String line = " a 	b \"c d e\" fff  \"\" 	gggg";
	Array<String> fields;
	String::split(line, fields);
	assert(fields.size() == 6);
	assert(fields[0] == "a");
	assert(fields[1] == "b");
	assert(fields[2] == "c d e");
	assert(fields[3] == "fff");
	assert(fields[4] == "");
	assert(fields[5] == "gggg");
    }

    {
	String line = "";
	Array<String> fields;
	String::split(line, fields);
	assert(fields.size() == 0);
    }

    {
	String line = "hello";
	Array<String> fields;
	String::split(line, fields);
	assert(fields.size() == 1);
	assert(fields[0] == "hello");
    }

    {
	String line;
	Array<String> fields;
	fields.append("one");
	fields.append("two \"three\" four");
	fields.append("five");
	String::join(fields, line);
	Boolean result = line == "one \"two \\\"three\\\" four\" five";
	assert(result);

	fields.clear();
	String::split(line, fields);
	assert(fields.size() == 3);
	assert(fields[0] == "one");
	result = fields[1] == "two \"three\" four";
	assert(result);
	assert(fields[2] == "five");
    }
}

int main()
{
    String s1 = "Hello World";
    String s2 = s1;
    String s3(s2);

    assert(String::equal(s1, s3));

    // Test append characters to String
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
	// Test allocateCString
	const char STR0[] = "one two three four";
	String s = STR0;
	char* tmp = s.allocateCString();
	assert(strcmp(tmp, STR0) == 0);
	const char STR1[] = "zero ";
	delete [] tmp;
    }

    {
	// Test appendToCString 
	const char STR0[] = "one two three four";
	String s = STR0;
	const char STR1[] = "zero ";
	char* tmp = new char[strlen(STR1) + s.size() + 1];
	strcpy(tmp, STR1);
	s.appendToCString(tmp);
	assert(strcmp(tmp, "zero one two three four") == 0);
	delete [] tmp;
    }

    {
	// Test appendToCString	with length parameter
	const char STR0[] = "one two three four";
	String s = STR0;
	const char STR1[] = "zero ";
	char* tmp = new char[strlen(STR1) + s.size() + 1];
	strcpy(tmp, STR1);
	s.appendToCString(tmp, 7);
	assert(strcmp(tmp, "zero one two") == 0);
	delete [] tmp;
    }

    {
	// Test remove
	String s = "abcdefg";
	s.remove(3, 3);
	assert(String::equal(s, "abcg"));
	assert(s.size() == 4);

	s = "abcdefg";
	s.remove(3, 4);
	assert(String::equal(s, "abc"));
	assert(s.size() == 3);

	s = "abcdefg";
	s.remove(3);
	assert(String::equal(s, "abc"));
	assert(s.size() == 3);

	s = "abc";
	s.remove(3);
	assert(String::equal(s, "abc"));
	assert(s.size() == 3);

	s = "abc";
	s.remove(0);
	assert(String::equal(s, ""));
	assert(s.size() == 0);

	s = "abc";
	s.remove(0, 1);
	assert(String::equal(s, "bc"));
	assert(s.size() == 2);

	String t1 = "HELLO";
	String t2 = ToLower(t1);
	assert(String::equal(t1, "HELLO"));
	assert(String::equal(t2, "hello"));
    }

    // Test of the different overload operators
    {
	// test the append  overload operator +=;
	String t1 = "one";
	t1 += " two";
    	assert(String::equal(t1, "one two"));
	t1 += ' ';
	t1 += 't';
	t1 += 'h';
	t1 += 'r';
	t1 += "ee";
	assert(String::equal(t1,"one two three"));
	
	// used as example in Doc.
	String test = "abc";
	test += "def";
	assert(test == "abcdef");
    }

    {
	// Test the == overload operator
	String t1 = "one";
	String t2 = "one";
	assert(t1 == "one");
	assert("one" == t1);
	assert(t1 == t2);
	assert(t2 == t1);
	assert(String("one") == "one");

	const char STR0[] = "one two three four";
	String s = STR0;
	char* tmp = s.allocateCString();
	assert(tmp == s);
	assert(s == tmp);
	delete [] tmp;
    }

    {
	// Tests of the + Overload operator
	String t1 = "abc";
	String t2 = t1 + t1;
	assert(t2 == "abcabc");
	t1 = "abc";
	t2 = t1 + "def";
	assert(t2 == "abcdef");

	t1 = "ghi";
	assert(t1 == "ghi");

	// ATTN: the following fails because there
	// is no single character overload operator
	// KS: Apr 2001
	// t2 = t1 + 'k' + 'l' + 'm' + "nop";
	t2 = t1 + "k" + "l" + "m" + "nop";
	assert(t2 == "ghiklmnop");
	assert(String::equal(t2,"ghiklmnop"));

	// add tests for != operator.

	t1 = "abc";
	assert(t1 != "ghi");
	assert(t1 != t2);

	// add tests for other compare operators

	// Operater <
	t1 = "ab";
	t2 = "cde";
	assert(t1 < t2);
	assert(t1 <= t2);
	assert(t2 > t1);
	assert(t2 >=t1);
	t2 = t1;
	assert(t1 <= t2);
	assert(t1 >= t2);

	// comparison and equals
	// the compare is for null term strings.
	// Therefore following does not work
	// the compare operators cover the problem
	// for String objects.
	// assert(String::compare(t1,t2) == -1);


	// Tests for compare with same length
	t1 = "abc";
	t2 = "def";
	assert(t1 < t2);

	// comparison and equals
	// compare is for null term strings
	// therefore following does not work.
	//assert(String::compare(t1,t2) == -1); 
    }

    {
	// Test of the [] operator
	String t1 = "abc";
	Char16 c = t1[1];
	// note c is Char16
	assert(c == 'b');

	//ATTN: test for outofbounds exception
	try
	{
	    c = t1[200];
	}
	catch (OutOfBounds&)
	{
	assert(true);
	}
    }

    {
	// Test the find function
	String t1 = "abcdef";
	String t2 = "cde";
	String t3 = "xyz";
	String t4 = "abc";
	String t5 = "abd";
	String t6 = "defg";
	assert(t1.find('c') == 2);
	assert(t1.find(t2)==2);
	assert(t1.find(t3)==(Uint32)-1);
	assert(t1.find(t4)==0);
	assert(t1.find(t5)==(Uint32)-1);
	assert(t1.find(t6)==(Uint32)-1);
	assert(t1.find("cde")==2);
	assert(t1.find("def")==3);
	assert(t1.find("xyz")==(Uint32)-1);
	assert(t1.find("a") ==0);

	// test for the case where string
	// partly occurs and then later
	// completely occurs
        String s = "this is an apple";
	assert(s.find("apple")==11);
	assert(s.find("appld")==(Uint32)-1);
	assert(s.find("this")==0);
	assert(s.find("t")==0);
	assert(s.find("e")==15);
	s = "a";
	assert(s.find("b")==(Uint32)-1);
	assert(s.find("a")==0);
	assert(s.find(s)==0);
	s = "aaaapple";
	assert(s.find("apple")==3);

	{
	    String nameSpace = "a#b#c";
	    nameSpace.translate('#', '/');
	    assert(nameSpace == "a/b/c");
	}

	TestSplit();
    }


    cout << "+++++ passed all tests" << endl;

    return 0;
}
