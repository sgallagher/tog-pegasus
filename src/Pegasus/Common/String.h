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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_String_h
#define Pegasus_String_h

#include <iostream>
#include <fstream>
#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Char16.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The Pegasus String C++ Class implements the CIM string type.
    This class is based on the general handle/representation pattern
    defined for all the Pegasus objects.  However, it differes from
    most in that it implements "copy on assign" all of the others implement
    "no copy on assign" semantics. The string class uses the Array class and
    implements an array of characters.
*/
class PEGASUS_COMMON_LINKAGE String
{
public:

    /** Default constructor without parameters. This constructor creates a
	null string
	<pre>
	    String test;
	</pre>
    */
    String();

    /// Copy constructor.
    String(const String& x);

    /// Initialize with first n characters from x.
    String(const String& x, Uint32 n);

    /// Initialize with x.
    String(const Char16* x);

    /// Initialize with first n characters of x.
    String(const Char16* x, Uint32 n);

    /// Initialize from a plain old C-String:
    String(const char* x);

    /// Initialize from the first n characters of a plain old C-String:
    String(const char* x, Uint32 n);

    /// String destructor. Used by the representation of the String object
    ~String();

    /** Assign this string with x.
	<pre>
	    String t1 = "abc";
	    String t2 = t1;
	</pre>
    */
    String& operator=(const String& x) { return assign(x); }

    /// Assign this string with Char16 x.
    String& operator=(const Char16* x) { assign(x); return *this; }

    /** Assign this string with String x
    @param x String to assign
    @return Returns the String
    */
    String& assign(const String& x);

    /// Assign this string with x.
    String& assign(const Char16* x);

    /// Assign this string with first n characters of x.
    String& assign(const Char16* x, Uint32 n);

    /// Assign this string with the plain old C-String x.
    String& assign(const char* x);

    /// Assign this string with first n characters of the plain old C-String x.
    String& assign(const char* x, Uint32 n);

    /** clear - Clear this string. After calling clear(), size() will return 0.
	<pre>
	    String test = "abc";
	    test.clear();	// String test is now NULL (length == 0)
	</pre>
    */
    void clear();


    /** reserve - Reserves memory for capacity characters. Notice that this does
    not
	change the size of the string (size() returns what it did before).
	If the capacity of the string is already greater or equal to the
	capacity argument, this method has no effect. After calling reserve(),
	getCapicty() returns a value which is greater or equal to the
	capacity argument.
	@param capacity defines the capacity in characters to reserve.
    */
    void reserve(Uint32 capacity);

    /** Returns the length of the String object.
	@return Length of the string in characters.
	<pre>
	    String s = "abcd";
	    assert(s.size() == 4);
	</pre>
    */
    Uint32 size() const { return _rep.size() - 1; }

    /** getData Returns a pointer to the first character in the 
	null-terminated string string of the String object.
	@return	Pointer to the first character of the String object
    	<pre>
	    String t1 = "abc";
	    const Char16* q = t1.getData();
	</pre>
    */
    const Char16* getData() const { return _rep.getData(); }

    /** AallocateCString - llocates an 8 bit representation of this String 
	object. The user is responsible for freeing the result. If any 
	characters are truncated, a TruncatedCharacter exception is thrown.
	This exception may be suppressed by passing true as the noThrow 
	argument. Extra characters may be allocated at the end of the
	new string by passing a non-zero value to the extraBytes argument.
	
	@param extraBytes Defines the number of extra characters to be
	allocated at the end of the new string. Default is zero.
	
	@param	noThrow If true, no exception will be thrown if characters
	are truncated
	
	@return pointer to the new representation of the string
	
	@exception Throws TruncatedCharacter exception if any characters are
	truncated
	<pre>
	    String test = "abc";
	    char* p = test.allocateCString();
	    ...
	    delete [] p;
	</pre>
    */
    char* allocateCString(Uint32 extraBytes = 0, Boolean noThrow = false) const;

    /** appendToCString - Append the given String object to a C-string. If the 
	length is not PEG_NOT_FOUND, then the lesser of the the length argument
	and he length of this string is truncated.  Otherwise, the entire string
	is trunctated.  The TruncatedCharacter exception is thrown if any 
	characters are truncated.  
    	@param str Char pointer to the string to append
    	@param length Length to append or PEG_NOT_FOUND (Uint32(-1)
    	@param noThrow - If false, throw the "TruncatedCharacter" exception of
    	any characters are truncated
    	@return void
    	@exception Throws TruncatedCharacter exception of characters are
    	truncated and noThrow parameter is false.
	<pre>
	    const char STR0[] = "one two three four";
	    String s = STR0;
	    const char STR1[] = "zero ";
	    char* tmp = new char[strlen(STR1) + s.size() + 1];
	    strcpy(tmp, STR1);
	    s.appendToCString(tmp, 7);
	    assert(strcmp(tmp, "zero one two") == 0);
	</pre>
    */
    void appendToCString(
	char* str,
	Uint32 length = PEG_NOT_FOUND,
	Boolean noThrow = false) const;

    /** Returns the Ith character of the String object.
	@exception - Throws exception "OutofBounds" if the index
	is outside the length of the string.
	<pre>
	    String t1 = "abc;
	    Char16 c = t1[1];	// character b
	</pre>
    */
    Char16& operator[](Uint32 i);

    /** Returns the Ith character of the String (const version).
	@exception - Throws exception "OutofBounds" if the index
	is outside the length of the string.

    */
    const Char16 operator[](Uint32 i) const;

    /** Append the given character to the string.
        <pre>
	     String s4 = "Hello";
	    s4.append(Char16(0x0000))
	</pre>
    */
    String& append(const Char16& c);

    /// Append n characters from str to this String object.
    String& append(const Char16* str, Uint32 n);

    /// Append the characters of str to this String object.
    String& append(const String& str)
    {
	return append(str.getData(), str.size());
    }

    /** Overload operator += appends the parameter String to this String.
	@param String to append.
	@return This String
	<pre>
	String test = "abc";
	test += "def";
	assert(test == "abcdef");
	</pre>
    */
    String& operator+=(const String& x)
    {
	return append(x);
    }

    /** Append the character given by c to this String object.
	@param c Single character to be appended
	@return String with appended character
    */
    String& operator+=(Char16 c)
    {
	return append(c);
    }

    /** Append the character given by c to this string.
	<pre>
	    String t1 = "abc";
	    t1 += 'd'
	    assert(t1 == "abcd");
	</pre>
    */
    String& operator+=(char c)
    {
	return append(Char16(c));
    }

    /** Remove size characters from the string starting at the given
	position. If size is PEG_NOT_FOUND, then all characters after pos are
	removed.
	@param pos Position in string to start remove
	@param size Number of characters to remove. Default is PEG_NOT_FOUND
	(Uint32(-1) which causes all characters after pos to be removed
	<pre>
	    String s;
	    s = "abc";
	    s.remove(0, 1);
	    assert(String::equal(s, "bc"));
	    assert(s.size() == 2);
	    s.remove(0);
	    assert(String::equal(s, ""));
	    assert(s.size() == 0);
	</pre>
	@exception throws "OutOfBounds" exception if size is greater than
	length of String plus starting position for remove.
    */
    void remove(Uint32 pos, Uint32 size = PEG_NOT_FOUND);

    /** Return a new String which is initialzed with <TT>length</TT>
	characters from this string starting at <TT>pos</TT>.
	@param <TT>pos</TT> is the positon in string to start getting the
	substring.
	@param <TT>length</TT> is the number of characters to get. If length
	is PEG_NOT_FOUND, then all characters after pos are added to the new
	string.
	@return String with the defined substring.
	<pre>
	    s = "abcdefg";
	    s.remove(3);
	    assert(String::equal(s, "abc"));
	</pre>
    */
    String subString(Uint32 pos, Uint32 length = PEG_NOT_FOUND) const;

    /** Find the position of the first occurence of the character c.
	If the character is not found, PEG_NOT_FOUND is returned.
	@param c Char to be found in the String
	@return Position of the character in the string or PEG_NOT_FOUND if not
	found.
    */
    Uint32 find(Char16 c) const;


    /** Find the position of the first occurence of the string object.
	This function finds one string inside another
	If the matching substring is not found, PEG_NOT_FOUND is returned.
	@param s String object to be found in the String
	@return Position of the substring in the String or PEG_NOT_FOUND if not
	found.
    */
    Uint32 find(const String& s) const;

    /** Find substring
	@ param 16 bit character pointer
	@seealso find
	@return Position of the substring in the String or PEG_NOT_FOUND if not
	found.
    */
    Uint32 find(const Char16* s) const;

    /** find substring
	@param s char* to substring
	@return Position of the substring in the String or PEG_NOT_FOUND if not
	found.
    */
    Uint32 find(const char* s) const;

    /** reverseFind - Same as find() but start looking in reverse (last
    character first).
    	@param c Char16 character to find in String.
	@Seealso find
	@return Position of the character in the string or PEG_NOT_FOUND if not
	found.

	NOTE: This function is defined only for char* input, not for
	String.
    */
    Uint32 reverseFind(Char16 c) const;

    /** Converts all characters in this string to lower case.
    */
    void toLower();

    /** Translate any occurences of fromChar to toChar.
    */
    void translate(Char16 fromChar, Char16 toChar);

    /** Method for printing a string.
    */
    void print() const;

    /** Compare the first n characters of the two strings..
    	@param s1 First null-terminated string for the comparison.
	@param s2 Second null-terminated string for the comparison.
	@param n Number of characters to compare.
	@return Return -1 if s1 is lexographically less than s2. If they are
	equavalent return 0. Otherwise return 1.
    */
    static int compare(const Char16* s1, const Char16* s2, Uint32 n);

    /** Just like one above except ignores case differences.
    */
    static int compareNoCase(const char* s1, const char* s2, Uint32 n);

    static int compareNoCase(const char* s1, const char* s2);

    /** Compare two null-terminated strings.
    	@param s1 First null-terminated string for the comparison.
	@param s2 Second null-terminated string for the comparison.
	@return If s1 is less than s2, return -1; if equal return 0;
	otherwise, return 1.

	NOTE: Use the comparison operators <,<= > >= to compare
	String objects.
    */
    static int compare(const Char16* s1, const Char16* s2);

    /** Compare two String objects for equality.
	@param s1 First <TT>String</TT> for comparison.
	@param s2 Second <TT>String</TT> for comparison.

	@return Boolean true if the two strings are equal.
	<pre>
	    String s1 = "Hello World";
	    String s2 = s1;
	    String s3(s2);
	    assert(String::equal(s1, s3));
	</pre>
    */
    static Boolean equal(const String& x, const String& y);

    /// Return true if the two strings are equal.
    static Boolean equal(const String& x, const Char16* y);

    /// Return true if the two strings are equal.
    static Boolean equal(const Char16* x, const String& y);

    /// Return true if the two strings are equal.
    static Boolean equal(const String& x, const char* y);

    /// Return true if the two strings are equal.
    static Boolean equal(const char* x, const String& y);

    static Boolean equalNoCase(const String& x, const String& y);

    /// Convert the plain old C-string to lower case:
    static void toLower(char* str);

    /**	EMPTY - Represent an empty string.
	This member is used to represent empty strings. Using this member
	avoids an expensive construction of an empty string (e.g., String()).
    */
    static const String EMPTY;

private:

    static Uint32 _min(Uint32 x, Uint32 y) { return x < y ? x : y; }

    Array<Char16> _rep;
};

/** String operator ==. Test for equality between two strings of any of the
    types String or char*.
    @return Boolean - True if the strings are equal

*/
inline Boolean operator==(const String& x, const String& y)
{
    return String::equal(x, y);
}

/** String operator ==. Test for equality between two strings

*/
inline Boolean operator==(const String& x, const char* y)
{
    return String::equal(x, y);
}

/** String operator ==. Test for equality between two strings

*/
inline Boolean operator==(const char* x, const String& y)
{
    return String::equal(x, y);
}

/** String operator ==. Test for equality between two strings

*/
inline Boolean operator!=(const String& x, const String& y)
{
    return !String::equal(x, y);
}

PEGASUS_COMMON_LINKAGE PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os,
    const String& x);

/** overload operator +	 - Concatenates String objects.

    <pre>
	String t1 = "abc";
	String t2;
	t2 = t1 + "def"
	assert(t2 == "abcdef");
    </pre>
*/
inline String operator+(const String& x, const String& y)
{
    return String(x).append(y);
}

/** overload operator < - Compares String obects.
    <pre>
	String t1 = "def";
	String t2 = "a";
	assert (t2 < t1);
    </pre>
*/
inline Boolean operator<(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) < 0;
}

/** overload operator <= compares String objects.

*/
inline Boolean operator<=(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) <= 0;
}

/** Overload operator > compares String objects
*/
inline Boolean operator>(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) > 0;
}

/** overload operator >= - Compares String objects
*/
inline Boolean operator>=(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) >= 0;
}

/** Return a version of this string whose characters have been shifted
    to lower case.
*/
PEGASUS_COMMON_LINKAGE String ToLower(const String& str);

/** Compare two strings but ignore any case differences.
*/
PEGASUS_COMMON_LINKAGE int CompareNoCase(const char* s1, const char* s2);

/** Get the next line from the input file.
*/
PEGASUS_COMMON_LINKAGE Boolean GetLine(PEGASUS_STD(istream)& is, String& line);

/*  This is an internal class to be used by the internal Pegasus
    components only. It provides an easy way to create an 8-bit string
    representation on the fly without calling allocateCString() and
    then worrying about deleting the string. The underscore before the
    class name denotes that this class is internal, unsupported, undocumented,
    and may be removed in future releases.
*/
class _CString
{
public:

    _CString(const String& x)
    {
	_rep = x.allocateCString();
    }

    _CString(const _CString& x)
    {
	_rep = strcpy(new char[strlen(x._rep) + 1], x._rep);
    }

    ~_CString()
    {
	delete [] _rep;
    }

    _CString& operator=(const _CString& x)
    {
	if (this != &x)
	    _rep = strcpy(new char[strlen(x._rep) + 1], x._rep);

	return *this;
    }

    operator const char*() const
    {
	return _rep;
    }

    const char* data() const
    {
	return _rep;
    }

private:

    char* _rep;
};

inline Uint32 _Length(const String& s1) { return s1.size(); }

inline Uint32 _Length(const char* s1) { return strlen(s1); }

inline Uint32 _Length(const char) { return 1; }

template<class S1, class S2>
inline String Cat(const S1& s1, const S2& s2)
{
    String tmp;
    tmp.reserve(_Length(s1) + _Length(s2));
    tmp.append(s1);
    tmp.append(s2);
    return tmp;
}

template<class S1, class S2, class S3>
inline String Cat(const S1& s1, const S2& s2, const S3& s3)
{
    String tmp;
    tmp.reserve(_Length(s1) + _Length(s2) + _Length(s3));
    tmp.append(s1);
    tmp.append(s2);
    tmp.append(s3);
    return tmp;
}

template<class S1, class S2, class S3, class S4>
inline String Cat(const S1& s1, const S2& s2, const S3& s3, const S4& s4)
{
    String tmp;
    tmp.reserve(_Length(s1) + _Length(s2) + _Length(s3) + _Length(s4));
    tmp.append(s1);
    tmp.append(s2);
    tmp.append(s3);
    tmp.append(s4);
    return tmp;
}

template<class S1, class S2, class S3, class S4, class S5>
inline String Cat(
    const S1& s1,
    const S2& s2,
    const S3& s3,
    const S4& s4,
    const S5& s5)
{
    String tmp;

    tmp.reserve(_Length(s1) + _Length(s2) + _Length(s3) + _Length(s4) +
	_Length(s5));

    tmp.append(s1);
    tmp.append(s2);
    tmp.append(s3);
    tmp.append(s4);
    tmp.append(s5);

    return tmp;
}

template<class S1, class S2, class S3, class S4, class S5, class S6>
inline String Cat(
    const S1& s1,
    const S2& s2,
    const S3& s3,
    const S4& s4,
    const S5& s5,
    const S6& s6)
{
    String tmp;

    tmp.reserve(_Length(s1) + _Length(s2) + _Length(s3) + _Length(s4) +
	_Length(s5) + _Length(s6));

    tmp.append(s1);
    tmp.append(s2);
    tmp.append(s3);
    tmp.append(s4);
    tmp.append(s5);
    tmp.append(s6);

    return tmp;
}

PEGASUS_COMMON_LINKAGE const Array<String>& EmptyStringArray();

PEGASUS_COMMON_LINKAGE Boolean Equal(const String& x, const String& y);

inline Boolean Open(PEGASUS_STD(ifstream)& is, const String& path)
{
    char* tmpPath = path.allocateCString();
    is.open(tmpPath);
    delete [] tmpPath;
    return !!is;
}

inline Boolean Open(PEGASUS_STD(ofstream)& os, const String& path)
{
    char* tmpPath = path.allocateCString();
    os.open(tmpPath);
    delete [] tmpPath;
    return !!os;
}

inline Boolean OpenAppend(PEGASUS_STD(ofstream)& os, const String& path)
{
    char* tmpPath = path.allocateCString();
    os.open(tmpPath, PEGASUS_STD(ios::app));
    delete [] tmpPath;
    return !!os;
}

#define PEGASUS_ARRAY_T String
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_String_h */
