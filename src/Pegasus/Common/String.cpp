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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cctype>
#include "String.h"
#include "Array.h"
#include "InternalException.h"
#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//
// CString
//
///////////////////////////////////////////////////////////////////////////////

CString::CString()
    : _rep(0)
{
}

CString::CString(const CString& cstr)
{
    _rep = (void*)new char[strlen((char*)cstr._rep)+1];
    strcpy((char*)_rep, (char*)cstr._rep);
}

CString::CString(char* cstr)
    : _rep(cstr)
{
}

CString::~CString()
{
    if (_rep)
        delete [] (char*)_rep;
}

CString& CString::operator=(const CString& cstr)
{
    _rep = (char*)new char[strlen((char*)cstr._rep)+1];
    strcpy((char*)_rep, (char*)cstr._rep);
    return *this;
}

CString::operator const char*() const
{
    return (char*)_rep;
}

///////////////////////////////////////////////////////////////////////////////
//
// String
//
///////////////////////////////////////////////////////////////////////////////

const String String::EMPTY = String();

Uint32 _strnlen(const char* str, Uint32 n)
{
    if (!str)
	throw NullPointer();

    for (Uint32 i=0; i<n; i++)
    {
        if (!*str)
        {
            return i;
        }
    }

    return n;
}

Uint32 _strnlen(const Char16* str, Uint32 n)
{
    if (!str)
	throw NullPointer();

    for (Uint32 i=0; i<n; i++)
    {
        if (!*str)
        {
            return i;
        }
    }

    return n;
}

inline Uint32 _StrLen(const char* str)
{
    if (!str)
	throw NullPointer();

    return strlen(str);
}

inline Uint32 _StrLen(const Char16* str)
{
    if (!str)
	throw NullPointer();

    Uint32 n = 0;

    while (*str++)
	n++;

    return n;
}

class StringRep
{
public:
    StringRep()
    {}
    StringRep(const StringRep& r)
        : c16a(r.c16a)
    {}
    StringRep(const Char16* str)
        : c16a(str, _StrLen(str) + 1)
    {}

    Array<Char16> c16a;
};

String::String()
{
    _rep = new StringRep;
    _rep->c16a.append('\0');
}

String::String(const String& str)
{
    _rep = new StringRep(*str._rep);
}

String::String(const String& str, Uint32 n)
{
    _rep = new StringRep;
    assign(str.getChar16Data(), n);
}

String::String(const Char16* str)
{
    _rep = new StringRep(str);
}

String::String(const Char16* str, Uint32 n)
{
    _rep = new StringRep;
    assign(str, n);
}

String::String(const char* str)
{
    _rep = new StringRep;
    assign(str);
}

String::String(const char* str, Uint32 n)
{
    _rep = new StringRep;
    assign(str, n);
}

String::~String()
{
    delete _rep;
}

String& String::operator=(const String& str)
{
    return assign(str);
}

String& String::assign(const String& str)
{
    _rep->c16a = str._rep->c16a;
    return *this;
}

String& String::assign(const Char16* str)
{
    _rep->c16a.clear();
    _rep->c16a.append(str, _StrLen(str) + 1);
    return *this;
}

String& String::assign(const Char16* str, Uint32 n)
{
    _rep->c16a.clear();
    Uint32 m = _strnlen(str, n);
    _rep->c16a.append(str, m);
    _rep->c16a.append('\0');
    return *this;
}

String& String::assign(const char* str)
{
    _rep->c16a.clear();

    Uint32 n = strlen(str) + 1;
    _rep->c16a.reserveCapacity(n);

    while (n--)
	_rep->c16a.append(*str++);

    return *this;
}

String& String::assign(const char* str, Uint32 n)
{
    _rep->c16a.clear();

    Uint32 _n = _strnlen(str, n);
    _rep->c16a.reserveCapacity(_n + 1);

    while (_n--)
	_rep->c16a.append(*str++);

    _rep->c16a.append('\0');

    return *this;
}

void String::clear()
{
    _rep->c16a.clear();
    _rep->c16a.append('\0');
}

void String::reserveCapacity(Uint32 capacity)
{
    _rep->c16a.reserveCapacity(capacity + 1);
}

Uint32 String::size() const
{
    return _rep->c16a.size() - 1;
}

const Char16* String::getChar16Data() const
{
    return _rep->c16a.getData();
}

CString String::getCString() const
{
    Uint32 n = size() + 1;
    char* str = new char[n];
    char* p = str;
    const Char16* q = getChar16Data();

    for (Uint32 i = 0; i < n; i++)
    {
	Uint16 c = *q++;
	*p++ = char(c);

	//if (c & 0xff00)
	//    truncatedCharacters = true;
    }

    return CString(str);
}

Char16& String::operator[](Uint32 index)
{
    if (index > size())
	throw IndexOutOfBoundsException();

    return _rep->c16a[index];
}

const Char16 String::operator[](Uint32 index) const
{
    if (index > size())
	throw IndexOutOfBoundsException();

    return _rep->c16a[index];
}

String& String::append(const Char16& c)
{
    _rep->c16a.insert(_rep->c16a.size() - 1, c);
    return *this;
}

String& String::append(const Char16* str, Uint32 n)
{
    Uint32 m = _strnlen(str, n);
    _rep->c16a.reserveCapacity(_rep->c16a.size() + m);
    _rep->c16a.remove(_rep->c16a.size() - 1);
    _rep->c16a.append(str, m);
    _rep->c16a.append('\0');
    return *this;
}

String& String::append(const String& str)
{
    return append(str.getChar16Data(), str.size());
}

void String::remove(Uint32 index, Uint32 size)
{
    if (size == PEG_NOT_FOUND)
	size = this->size() - index;

    if (index + size > this->size())
	throw IndexOutOfBoundsException();

    if (size)
	_rep->c16a.remove(index, size);
}

String String::subString(Uint32 index, Uint32 length) const
{
    if (index < size())
    {
	if ((length == PEG_NOT_FOUND) || (length > size() - index))
	    length = size() - index;

	return String(getChar16Data() + index, length);
    }
    else
	return String();
}

Uint32 String::find(Char16 c) const
{
    const Char16* first = getChar16Data();

    for (const Char16* p = first; *p; p++)
    {
	if (*p == c)
	    return  p - first;
    }

    return PEG_NOT_FOUND;
}

Uint32 String::find(Uint32 index, Char16 c) const
{
    const Char16* data = getChar16Data();

    for (Uint32 i = index, n = size(); i < n; i++)
    {
	if (data[i] == c)
	    return i;
    }

    return PEG_NOT_FOUND;
}

Uint32 String::find(const String& s) const
{
    const Char16* pSubStr = s.getChar16Data();
    const Char16* pStr = getChar16Data();
    Uint32 subStrLen = s.size();
    Uint32 strLen = size();

    if (subStrLen > strLen)
    {
        return PEG_NOT_FOUND;
    }

    // loop to find first char match
    Uint32 loc = 0;
    for( ; loc <= (strLen-subStrLen); loc++)
    {
	if (*pStr++ == *pSubStr)  // match first char
	{
	    // point to substr 2nd char
	    const Char16* p = pSubStr + 1;

	    // Test remaining chars for equal
	    Uint32 i = 1;
	    for (; i < subStrLen; i++)
		if (*pStr++ != *p++ )
		    {pStr--; break;} // break from loop
	    if (i == subStrLen)
		return loc;
	}
    }
    return PEG_NOT_FOUND;
}

Uint32 String::reverseFind(Char16 c) const
{
    const Char16* first = getChar16Data();
    const Char16* last = getChar16Data() + size();

    while (last != first)
    {
	if (*--last == c)
	    return last - first;
    }

    return PEG_NOT_FOUND;
}

void String::toLower()
{
    for (Char16* p = &_rep->c16a[0]; *p; p++)
    {
	if (*p <= PEGASUS_MAX_PRINTABLE_CHAR)
	    *p = tolower(*p);
    }
}

int String::compare(const String& s1, const String& s2, Uint32 n)
{
    const Char16* s1c16 = s1.getChar16Data();
    const Char16* s2c16 = s2.getChar16Data();

    while (n--)
    {
	int r = *s1c16++ - *s2c16++;

	if (r)
	    return r;
    }

    return 0;
}

int String::compare(const String& s1, const String& s2)
{
    const Char16* s1c16 = s1.getChar16Data();
    const Char16* s2c16 = s2.getChar16Data();

    while (*s1c16 && *s2c16)
    {
	int r = *s1c16++ - *s2c16++;

	if (r)
	    return r;
    }

    if (*s2c16)
	return -1;
    else if (*s1c16)
	return 1;

    return 0;
}

int String::compareNoCase(const String& s1, const String& s2)
{
    const Char16* _s1 = s1.getChar16Data();
    const Char16* _s2 = s2.getChar16Data();

    while (*_s1 && *_s2)
    {
        int r;

        if (*_s1 <= PEGASUS_MAX_PRINTABLE_CHAR &&
            *_s2 <= PEGASUS_MAX_PRINTABLE_CHAR)
        {
            r = tolower(*_s1++) - tolower(*_s2++);
        }
        else
        {
            r = *_s1++ - *_s2++;
        }

	if (r)
	    return r;
    }

    if (*_s2)
	return -1;
    else if (*_s1)
	return 1;

    return 0;
}

Boolean String::equal(const String& str1, const String& str2)
{
    return String::compare(str1, str2) == 0;
}

Boolean String::equalNoCase(const String& str1, const String& str2)
{
    if (str1.size() != str2.size())
	return false;

    const Char16* p = str1.getChar16Data();
    const Char16* q = str2.getChar16Data();

    Uint32 n = str1.size();

    while (n--)
    {
	if (*p <= PEGASUS_MAX_PRINTABLE_CHAR &&
            *q <= PEGASUS_MAX_PRINTABLE_CHAR)
	{
	    if (tolower(*p++) != tolower(*q++))
		return false;
	}
	else if (*p++ != *q++)
	    return false;
    }

    return true;
}


// ATTN-RK-P3-20020603: This code is not completely correct
 // Wildcard String matching function that may be useful in the future
// The following code was provided by Bob Blair.

/* _StringMatch Match input MatchString against a GLOB style pattern
       Note that MatchChar is the char type so that this source
       in portable to different string types. This is an internal function
 
  Results: The return value is 1 if string matches pattern, and
 	0 otherwise.  The matching operation permits the following
 	special characters in the pattern: *?\[] (see the manual
 	entry for details on what these mean).
 
  Side effects: None.
 */

/* MatchChar defined as a separate entity because this function source used
    elsewhere was an unsigned char *. Here we use Uint16 to  maintain 16 bit 
    size.
*/
typedef Uint16 MatchChar;

inline Uint16 _ToLower(Uint16 ch)
{
    return ch <= PEGASUS_MAX_PRINTABLE_CHAR ? tolower(char(ch)) : ch;
}

inline Boolean _Equal(MatchChar ch1, MatchChar ch2, int nocase)
{
    if (nocase)
	return _ToLower(ch1) == _ToLower(ch2);
    else
	return ch1 == ch2;
}


static const MatchChar *
_matchrange(const MatchChar *range, MatchChar c, int nocase)
{
  const MatchChar *p = range;
  const MatchChar *rstart = range + 1;
  const MatchChar *rend = 0;
  MatchChar compchar;

  for (rend = rstart; *rend && *rend != ']'; rend++);
  if (*rend == ']') {  // if there is an end to this pattern
    for (compchar = *rstart; rstart != rend; rstart++) {
      if (_Equal(*rstart, c, nocase))
        return ++rend;
      if (*rstart == '-') {
        rstart++;
        if (c >= compchar && c <= *rstart)
          return ++rend;
      }
    }
  }
  return (const MatchChar *)0;
}

static int
_StringMatch( 
    const MatchChar *testString, 
    const MatchChar *pattern,
    int nocase ) 		/* Ignore case if this is true */
{
  const MatchChar *pat = pattern;
  const MatchChar *str = testString;
  unsigned int done = 0;
  unsigned int res = 0;  // the result: 1 == match

  while (!done) { // main loop walks through pattern and test string
    //cerr << "Comparing <" << *pat << "> and <" << *str << ">" << endl;
    if (!*pat) {                                         //end of pattern
      done = 1;                                          // we're done
      if (!*str)                                         //end of test, too?
        res = 1;                                         // then we matched
    } else {                                             //Not end of pattern
      if (!*str) {                                       // but end of test
        done = 1;                                        // We're done
        if (*pat == '*')                                 // If pattern openends
          res = 1;                                       //  then we matched
      } else {                                           //Not end of test
        if (*pat == '*') {                               //Ambiguuity found
          if (!*++pat) {                                 //and it ends pattern
            done = 1;                                    //  then we're done
            res = 1;                                     //  and match
          } else {                                       //if it doesn't end
            while (!done) {                              //  until we're done
              if (_StringMatch(str, pat, nocase)) {      //  we recurse
                done = 1;                                //if it recurses true
                res = 1;                                 //  we done and match
              } else {                                   //it recurses false
                if (!*str)                               // see if test is done
                  done = 1;                              //  yes: we done
                else                                     // not done:
                  str++;                                 //   keep testing
              } // end test on recursive call
            } // end looping on recursive calls
          } // end logic when pattern is ambiguous
        } else {                                         //pattern not ambiguus
          if (*pat == '?') {                             //pattern is 'any'
            pat++, str++;                                //  so move along
          } else if (*pat == '[') {                      //see if it's a range
            pat = _matchrange(pat, *str, nocase);         // and is a match
            if (!pat) {                                  //It is not a match
              done = 1;                                  //  we're done
              res = 0;                                   //  no match
            } else {                                     //Range matches
              str++, pat++;                              //  keep going
            }
          } else {               // only case left is individual characters
            if (!_Equal(*pat++, *str++, nocase))         // if they don't match
              done = 1;                                  //   bail.
          }
        }  // end ("pattern is not ambiguous (*)" logic
      } // end logic when pattern and string still have data
    } // end logic when pattern still has data
  } // end main loop
  return res;
}


Boolean String::match(const String& str, const String& pattern)
{
    return _StringMatch(
	(Uint16*)str.getChar16Data(), (Uint16*)pattern.getChar16Data(), 0) != 0;
}

Boolean String::matchNoCase(const String& str, const String& pattern)
{
    return _StringMatch(
	(Uint16*)str.getChar16Data(), (Uint16*)pattern.getChar16Data(), 1) != 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// String-related functions
//
///////////////////////////////////////////////////////////////////////////////

Boolean operator==(const String& str1, const String& str2)
{
    return String::equal(str1, str2);
}

Boolean operator==(const String& str1, const char* str2)
{
    return String::equal(str1, str2);
}

Boolean operator==(const char* str1, const String& str2)
{
    return String::equal(str1, str2);
}

Boolean operator!=(const String& str1, const String& str2)
{
    return !String::equal(str1, str2);
}

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os, const String& str)
{
    for (Uint32 i = 0, n = str.size(); i < n; i++)
    {
        Uint16 code = str[i];

        if (code > 0 && code <= PEGASUS_MAX_PRINTABLE_CHAR)
        {
            os << char(code);
        }
        else
        {
            // Print in hex format:
            char buffer[8];
            sprintf(buffer, "\\x%04X", code);
            os << buffer;
        }
    }

    return os;
}

String operator+(const String& str1, const String& str2)
{
    return String(str1).append(str2);
}

Boolean operator<(const String& str1, const String& str2)
{
    return String::compare(str1, str2) < 0;
}

Boolean operator<=(const String& str1, const String& str2)
{
    return String::compare(str1, str2) <= 0;
}

Boolean operator>(const String& str1, const String& str2)
{
    return String::compare(str1, str2) > 0;
}

Boolean operator>=(const String& str1, const String& str2)
{
    return String::compare(str1, str2) >= 0;
}

int CompareNoCase(const char* s1, const char* s2)
{
    while (*s1 && *s2)
    {
	int r = tolower(*s1++) - tolower(*s2++);

	if (r)
	    return r;
    }

    if (*s2)
	return -1;
    else if (*s1)
	return 1;

    return 0;
}

PEGASUS_NAMESPACE_END
