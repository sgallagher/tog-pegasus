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
// Authors: Dan Gorey (djgorey@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/CQL/CQLRegularExpression.h>
#include <Pegasus/Common/String.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    //result should return false
    CQLRegularExpression re;

    const String p = "abc";
    const String s = ""; 
    
    assert ( (re.match(s, p)) == false);
    return;
}

void test02()
{
    //result should return false
    CQLRegularExpression re;

    const String p = "";
    const String s = "abc"; 
    
    assert ( (re.match(s, p)) == false);
    return;
}

void test03()
{
    //result should return true
    CQLRegularExpression re;

    const String p = "abc";
    const String s = "abc"; 
    
    assert ( (re.match(s, p)) == true);
    return;
}


void test04()
{
    //result should return false
    CQLRegularExpression re;

    const String p = "abcd";
    const String s = "abc"; 
    
    assert ( (re.match(s, p)) == false);
    return;
}

void test05()
{
    //result should return false
    CQLRegularExpression re;

    const String p = "abc";
    const String s = "abcd"; 
    
    assert ( (re.match(s, p)) == false);
    return;
}

void test06()
{
    //result should return true
    CQLRegularExpression re;

    const String p = "ab.";
    const String s = "ab?"; 
    
    assert ( (re.match(s, p)) == true);
    return;
}

void test07()
{
    //result should return true
    CQLRegularExpression re;
                      
    const String p = ".a.b";
    const String s = "aa!b"; 
    
    assert ( (re.match(s, p)) == true);
    return;
}

void test08()
{
    //result should return true
    CQLRegularExpression re;

    const String p = "\\.ab";
    const String s = ".ab"; 
    
    assert ( (re.match(s, p)) == true);
    return;
}

void test09()
{
    //result should return false
    CQLRegularExpression re;

    const String p = "\\.ab";
    const String s = "\\.ab"; 
    
    assert ( (re.match(s, p)) == false);
    return;
}

void test10()
{
    //result should return true
    CQLRegularExpression re;

    const String p = ".*";
    const String s = "abcd"; 
    assert ( (re.match(s, p)) == true);
    return;
}

void test11()
{
    //result should return true
    CQLRegularExpression re;

    const String p = "\\.*";
    const String s = "......"; 
    
    assert ( (re.match(s, p)) == true);
    return;
}

void test12()
{
    //result should return true
    CQLRegularExpression re;

    const String p = "abcd*";
    const String s = "abcddddd"; 
    
    assert ( (re.match(s, p)) == true);
    return;
}

void test13()
{
    //result should return false
    CQLRegularExpression re;

    const String p = "abcd*";
    const String s = "abcd"; 
    
    assert ( (re.match(s, p)) == false);
    return;
}

void test14()
{
    //result should return true
    CQLRegularExpression re;

    const String p = "ab*cd";
    const String s = "abbbbcd"; 
    
    assert ( (re.match(s, p)) == true);
    return;
}

void test15()
{
    //result should return true
    CQLRegularExpression re;
                      
    const String p = "ab.*cd";
    const String s = "ab123!cd"; 
    
    assert ( (re.match(s, p)) == true);
    return;
}

void test16()
{
    //result should return true
    CQLRegularExpression re;

    const String p = "\\*ab";
    const String s = "*ab"; 
    
    assert ( (re.match(s, p)) == true);
    return;
}

void test17()
{
    //result should return false
    CQLRegularExpression re;

    const String p = ".\\*";
    const String s = "****"; 
    
    assert ( (re.match(s, p)) == false);
    return;
}

void test18()
{
    //result should return true
    CQLRegularExpression re;
    Char16 utf16Chars[] =
        {
        0xD800,0x78BC, 0xDC01, 0x45A3,
        0x00};

    const String utf(utf16Chars);

    assert ( (re.match(utf, utf)) == true);
    return;
}

void test19()
{
    //result should return true
    CQLRegularExpression re;
    Char16 utf16CharsP[] =
        {
        0xD800,0x78BC, 
        0x00};

    String utfPattern(utf16CharsP);
    utfPattern.append("*");
    Char16 utf16CharsS[] =
    {
    0xD800,0x78BC, 0xD800,0x78BC, 0xD800,0x78BC,
    0x00};

    const String utfString(utf16CharsS);


    assert ( (re.match(utfString, utfPattern)) == true);
    return;
}

void test20()
{
    //result should return true
    CQLRegularExpression re;
    const String pattern = ".*";

    Char16 utf16CharsS[] =
    {
    0xD800,0x78BC, 0x00};

    String utfString(utf16CharsS);
    utfString.append("*");


    assert ( (re.match(utfString,pattern)) == true);
    return;
}

void test21()
{
    //result should return true
    CQLRegularExpression re;
    Char16 utf16CharsP[] =
        {
        0xD800,0x78BC, 
        0x00};

    String utfPattern(utf16CharsP);
    utfPattern.append(".*");
    
    Char16 utf16CharsS[] =
    {
    0xD800,0x78BC, 0x00};

    String utfString(utf16CharsS);
    utfString.append("an3s");


    assert ( (re.match(utfString, utfPattern)) == true);
    return;
}





int main(int argc, char** argv)
{
    test01();
    test02();
    test03();
    test04();
    test05();
    test06();
    test07();
    test08();
    test09();
    test10();
    test11();
    test12();
    test13();
    test14();
    test15();
    test16();
    test17();
    test18();
    test19();
    test20();
    test21();
    cout << "+++++ passed all tests" << endl;
    

    return 0;
}
                    
