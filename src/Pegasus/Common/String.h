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
// Modified By: Karl Schopmeyer(k.schopmeyer@opengroup.org)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_String_h
#define Pegasus_String_h

#include <iostream>
#include <fstream>
#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Char16.h>

PEGASUS_NAMESPACE_BEGIN

class StringRep;

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

    /**	EMPTY - Represent an empty string.
	This member is used to represent empty strings. Using this member
	avoids an expensive construction of an empty string (e.g., String()).
    */
    static const String EMPTY;

    /** Default constructor without parameters. This constructor creates a
	null string
	<pre>
	    String test;
	</pre>
    */
    String();

    /// Copy constructor.
    String(const String& str);

    /// Initialize with first n characters from str.
    String(const String& str, Uint32 n);

    /// Initialize with str.
    String(const Char16* str);

    /// Initialize with first n characters of str.
    String(const Char16* str, Uint32 n);

    /// Initialize from a plain old C-String:
    String(const char* str);

    /// Initialize from the first n characters of a plain old C-String:
    String(const char* str, Uint32 n);

    /// String destructor. Used by the representation of the String object
    ~String();

    /** Assign this string with str.
	<pre>
	    String t1 = "abc";
	    String t2 = t1;
	</pre>
    */
    String& operator=(const String& str);

    /** Assign this string with String str
    @param str String to assign
    @return Returns the String
    */
    String& assign(const String& str);

    /// Assign this string with str.
    String& assign(const Char16* str);

    /// Assign this string with first n characters of str.
    String& assign(const Char16* str, Uint32 n);

    /// Assign this string with the plain old C-String str.
    String& assign(const char* str);

    /// Assign this string with first n characters of the plain old C-String str.
    String& assign(const char* str, Uint32 n);

    /** clear - Clear this string. After calling clear(), size() will return 0.
	<pre>
	    String test = "abc";
	    test.clear();	// String test is now NULL (length == 0)
	</pre>
    */
    void clear();


    /** reserve - Reserves memory for capacity characters. Notice that this
        does not change the size of the string (size() returns what it did
        before).  If the capacity of the string is already greater or equal
        to the capacity argument, this method has no effect.  The capacity
        of a String object has no bearing on its external behavior.  The
        capacity of a String is set only for performance reasons.
	@param capacity defines the capacity in characters to reserve.
    */
    void reserveCapacity(Uint32 capacity);

    /** Returns the length of the String object.
	@return Length of the string in characters.
	<pre>
	    String s = "abcd";
	    assert(s.size() == 4);
	</pre>
    */
    Uint32 size() const;

    /** getData Returns a pointer to the first character in the 
	null-terminated string string of the String object.
	@return	Pointer to the first character of the String object
    	<pre>
	    String t1 = "abc";
	    const Char16* q = t1.getData();
	</pre>
    */
    const Char16* getData() const;

    /** allocateCString - allocates an 8 bit representation of this String 
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
    String& append(const String& str);

    /** Overload operator += appends the parameter String to this String.
	@param String to append.
	@return This String
	<pre>
	String test = "abc";
	test += "def";
	assert(test == "abcdef");
	</pre>
    */
    String& operator+=(const String& str);

    /** Append the character given by c to this String object.
	@param c Single character to be appended
	@return String with appended character
    */
    String& operator+=(Char16 c);

    /** Append the character given by c to this string.
	<pre>
	    String t1 = "abc";
	    t1 += 'd'
	    assert(t1 == "abcd");
	</pre>
    */
    String& operator+=(char c);

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

    /** Same as above but starts searching from the given position. */
    Uint32 find(Uint32 pos, Char16 c) const;

    /** Find the position of the first occurence of the string object.
	This function finds one string inside another
	If the matching substring is not found, PEG_NOT_FOUND is returned.
	@param s String object to be found in the String
	@return Position of the substring in the String or PEG_NOT_FOUND if not
	found.
    */
    Uint32 find(const String& s) const;

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

#ifdef PEGASUS_INTERNALONLY
    // ATTN-RK-P3-20020509: Define case-sensitivity for non-English characters
    /** Converts all characters in this string to lower case.
    */
    void toLower();
#endif

    /** Compare the first n characters of the two strings..
    	@param s1 First null-terminated string for the comparison.
	@param s2 Second null-terminated string for the comparison.
	@param n Number of characters to compare.
	@return Return -1 if s1 is lexographically less than s2. If they are
	equavalent return 0. Otherwise return 1.
    */
    static int compare(const String& s1, const String& s2, Uint32 n);

    /** Compare two null-terminated strings.
    	@param s1 First null-terminated string for the comparison.
	@param s2 Second null-terminated string for the comparison.
	@return If s1 is less than s2, return -1; if equal return 0;
	otherwise, return 1.

	NOTE: Use the comparison operators <,<= > >= to compare
	String objects.
    */
    static int compare(const String& s1, const String& s2);

    /** Just like one above except ignores case differences.
    */
    static int compareNoCase(const char* s1, const char* s2, Uint32 n);

    static int compareNoCase(const char* s1, const char* s2);

#ifdef PEGASUS_INTERNALONLY
    static int compareNoCase(const String& s1, const String& s2);
#endif

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
    static Boolean equal(const String& str1, const String& str2);

    /** equalNoCase - Compares two strings and returuns true if they
	are equal indpedent of case of the characters.
	@param str1 First String parameter
	@param str2 Second String parameter
	@return true if strings are equal independent of case.
    */
    static Boolean equalNoCase(const String& str1, const String& str2);

    /** match matches a string against a GLOB style pattern.
        Return trues if the String parameter matches the pattern. C-Shell style
	glob matching is used.
        @param str String to be matched against the pattern
        @param pattern Pattern to use in the match
        @return Boolean true if str matches pattern
        The pattern definition is as follows:
        <pre>
        *             Matches any number of any characters
        ?             Match exactly one character
        [chars]       Match any character in chars
        [chara-charb] Match any character in the range between chara and charb
        </pre>
        The literal characters *, ?, [, ] can be included in a string by
        escaping them with backslash "\".  Ranges of characters can be concatenated.
        <pre>
        examples:
        Boolean result = String::match("This is a test", "*is*");
        Boolean works =  String::match("abcdef123", "*[0-9]");
        </pre>
    */
    static Boolean match(const String& str, const String& pattern);

    /** matchNoCase Matches a String against a GLOB style pattern independent
        of case. 
        Returns true if the str parameter matches the pattern. C-Shell style
	glob matching is used. Ignore case in all comparisons. Case is
        ignored in the match.
        @parm str String containing the string to be matched\
        @parm pattern GLOB style patterh to use in the match.
        @return Boolean true if str matches patterh
        @SeeAlso match
    */
    static Boolean matchNoCase(const String& str, const String& pattern);

private:

    StringRep* _rep;
};

/** String operator ==. Test for equality between two strings of any of the
    types String or char*.
    @return Boolean - True if the strings are equal
*/
PEGASUS_COMMON_LINKAGE Boolean operator==(
    const String& str1,
    const String& str2);

/** String operator ==. Test for equality between two strings

*/
PEGASUS_COMMON_LINKAGE Boolean operator==(const String& str1, const char* str2);

/** String operator ==. Test for equality between two strings

*/
PEGASUS_COMMON_LINKAGE Boolean operator==(const char* str1, const String& str2);

/** String operator ==. Test for equality between two strings

*/
PEGASUS_COMMON_LINKAGE Boolean operator!=(
    const String& str1,
    const String& str2);

PEGASUS_COMMON_LINKAGE PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os,
    const String& str);

/** overload operator +	 - Concatenates String objects.

    <pre>
	String t1 = "abc";
	String t2;
	t2 = t1 + "def"
	assert(t2 == "abcdef");
    </pre>
*/
PEGASUS_COMMON_LINKAGE String operator+(const String& str1, const String& str2);

/** overload operator < - Compares String obects.
    <pre>
	String t1 = "def";
	String t2 = "a";
	assert (t2 < t1);
    </pre>
*/
PEGASUS_COMMON_LINKAGE Boolean operator<(
    const String& str1,
    const String& str2);

/** overload operator <= compares String objects.

*/
PEGASUS_COMMON_LINKAGE Boolean operator<=(
    const String& str1,
    const String& str2);

/** Overload operator > compares String objects
*/
PEGASUS_COMMON_LINKAGE Boolean operator>(
    const String& str1,
    const String& str2);

/** overload operator >= - Compares String objects
*/
PEGASUS_COMMON_LINKAGE Boolean operator>=(
    const String& str1,
    const String& str2);

/** Compare two strings but ignore any case differences.
*/
PEGASUS_COMMON_LINKAGE int CompareNoCase(const char* s1, const char* s2);

PEGASUS_COMMON_LINKAGE int EqualNoCase(const char* s1, const char* s2);

#ifdef PEGASUS_INTERNALONLY
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

    _CString(const String& str)
    {
	_rep = str.allocateCString();
    }

    _CString(const _CString& str)
    {
	_rep = strcpy(new char[strlen(str._rep) + 1], str._rep);
    }

    ~_CString()
    {
	delete [] _rep;
    }

    _CString& operator=(const _CString& str)
    {
	if (this != &str)
	    _rep = strcpy(new char[strlen(str._rep) + 1], str._rep);

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
#endif

PEGASUS_NAMESPACE_END

#endif /* Pegasus_String_h */
