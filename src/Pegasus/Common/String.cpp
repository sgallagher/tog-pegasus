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
#include "Exception.h"
#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T String
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

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

String::String()
{
    _rep.append('\0');
}

String::String(const String& str)
    : _rep(str._rep)
{
}

String::String(const String& str, Uint32 n)
{
    assign(str.getData(), n);
}

String::String(const Char16* str)
    : _rep(str, _StrLen(str) + 1)
{
}

String::String(const Char16* str, Uint32 n)
{
    assign(str, n);
}

String::String(const char* str)
{
    assign(str);
}

String::String(const char* str, Uint32 n)
{
    assign(str, n);
}

String::~String()
{
}

String& String::operator=(const String& str)
{
    return assign(str);
}

String& String::operator=(const Char16* str)
{
    return assign(str);
}

String& String::assign(const String& str)
{
    _rep = str._rep;
    return *this;
}

String& String::assign(const Char16* str)
{
    _rep.clear();
    _rep.append(str, _StrLen(str) + 1);
    return *this;
}

String& String::assign(const Char16* str, Uint32 n)
{
    _rep.clear();
    Uint32 m = _strnlen(str, n);
    _rep.append(str, m);
    _rep.append('\0');
    return *this;
}

String& String::assign(const char* str)
{
    _rep.clear();

    Uint32 n = strlen(str) + 1;
    _rep.reserve(n);

    while (n--)
	_rep.append(*str++);

    return *this;
}

String& String::assign(const char* str, Uint32 n)
{
    _rep.clear();

    Uint32 _n = _strnlen(str, n);
    _rep.reserve(_n + 1);

    while (_n--)
	_rep.append(*str++);

    _rep.append('\0');

    return *this;
}

void String::clear()
{
    _rep.clear();
    _rep.append('\0');
}

void String::reserve(Uint32 capacity)
{
    _rep.reserve(capacity + 1);
}

Uint32 String::size() const
{
    return _rep.size() - 1;
}

const Char16* String::getData() const
{
    return _rep.getData();
}

char* String::allocateCString(Uint32 extraBytes, Boolean noThrow) const
{
    Uint32 n = size() + 1;
    char* str = new char[n + extraBytes];
    char* p = str;
    const Char16* q = getData();

    for (Uint32 i = 0; i < n; i++)
    {
	Uint16 c = *q++;
	*p++ = char(c);

	if ((c & 0xff00) && !noThrow)
	    throw TruncatedCharacter();
    }

    return str;
}

void String::appendToCString(
    char* str,
    Uint32 length,
    Boolean noThrow) const
{
    if (!str)
	throw NullPointer();

    Uint32 n = (size() < length)? size() : length;

    char* p = str + strlen(str);
    const Char16* q = getData();

    for (Uint32 i = 0; i < n; i++)
    {
	Uint16 c = *q++;
	*p++ = char(c);

	if ((c & 0xff00) && !noThrow)
	    throw TruncatedCharacter();
    }

    *p = '\0';
}

Char16& String::operator[](Uint32 i)
{
    if (i > size())
	ThrowOutOfBounds();

    return _rep[i];
}

const Char16 String::operator[](Uint32 i) const
{
    if (i > size())
	ThrowOutOfBounds();

    return _rep[i];
}

String& String::append(const Char16& c)
{
    _rep.insert(_rep.size() - 1, c);
    return *this;
}

String& String::append(const Char16* str, Uint32 n)
{
    Uint32 m = _strnlen(str, n);
    _rep.reserve(_rep.size() + m);
    _rep.remove(_rep.size() - 1);
    _rep.append(str, m);
    _rep.append('\0');
    return *this;
}

String& String::append(const String& str)
{
    return append(str.getData(), str.size());
}

String& String::operator+=(const String& str)
{
    return append(str);
}

String& String::operator+=(Char16 c)
{
    return append(c);
}

String& String::operator+=(char c)
{
    return append(Char16(c));
}

void String::remove(Uint32 pos, Uint32 size)
{
    if (size == PEG_NOT_FOUND)
	size = this->size() - pos;

    if (pos + size > this->size())
	ThrowOutOfBounds();

    if (size)
	_rep.remove(pos, size);
}

String String::subString(Uint32 pos, Uint32 length) const
{
    if (pos < size())
    {
	if (length == PEG_NOT_FOUND)
	    length = size() - pos;

	return String(getData() + pos, length);
    }
    else
	return String();
}

Uint32 String::find(Char16 c) const
{
    const Char16* first = getData();

    for (const Char16* p = first; *p; p++)
    {
	if (*p == c)
	    return  p - first;
    }

    return PEG_NOT_FOUND;
}

Uint32 String::find(Uint32 pos, Char16 c) const
{
    const Char16* data = getData();

    for (Uint32 i = pos, n = size(); i < n; i++)
    {
	if (data[i] == c)
	    return i;
    }

    return PEG_NOT_FOUND;
}

Uint32 String::find(const String& s) const
{
    const Char16* pSubStr = s.getData();
    const Char16* pStr = getData();
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

Uint32 String::find(const Char16* s) const
{
    return find(String(s));
}

Uint32 String::find(const char* s) const
{
    return find(String(s));
}

Uint32 String::reverseFind(Char16 c) const
{
    const Char16* first = getData();
    const Char16* last = getData() + size();

    while (last != first)
    {
	if (*--last == c)
	    return last - first;
    }

    return PEG_NOT_FOUND;
}

void String::toLower()
{
    for (Char16* p = &_rep[0]; *p; p++)
    {
#ifdef PEGASUS_HAS_EBCDIC
	if (*p <= 255)
#else
	if (*p <= 127)
#endif
	    *p = tolower(*p);
    }
}

void String::toLower(char* str)
{
    while (*str)
	tolower(*str++);
}

void String::translate(Char16 fromChar, Char16 toChar)
{
    for (Char16* p = &_rep[0]; *p; p++)
    {
	if (*p == fromChar)
	    *p = toChar;
    }
}

void String::print() const
{
    cout << *this << endl;
}

int String::compare(const Char16* s1, const Char16* s2, Uint32 n)
{
    while (n--)
    {
	int r = *s1++ - *s2++;

	if (r)
	    return r;
    }

    return 0;
}

int String::compare(const Char16* s1, const Char16* s2)
{
    while (*s1 && *s2)
    {
	int r = *s1++ - *s2++;

	if (r)
	    return r;
    }

    if (*s2)
	return -1;
    else if (*s1)
	return 1;

    return 0;
}

int String::compareNoCase(const char* s1, const char* s2, Uint32 n)
{
    while (n--)
    {
	int r = tolower(*s1++) - tolower(*s2++);

	if (r)
	    return r;
    }

    return 0;
}

int String::compareNoCase(const char* s1, const char* s2)
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

int String::compareNoCase(const String& s1, const String& s2)
{
    const Char16* _s1 = s1.getData();
    const Char16* _s2 = s2.getData();

    while (*_s1 && *_s2)
    {
        int r;

#ifdef PEGASUS_HAS_EBCDIC
        if (*_s1 <= 255 && *_s2 <= 255)
#else
        if (*_s1 <= 127 && *_s2 <= 127)
#endif
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
    if (str1.size() != str2.size())
	return false;

    return String::compare(str1.getData(), str2.getData(), str1.size()) == 0;
}

Boolean String::equal(const String& str1, const Char16* str2)
{
    if (str1.size() != _StrLen(str2))
	return false;

    return String::compare(str1.getData(), str2, str1.size()) == 0;
}

Boolean String::equal(const Char16* str1, const String& str2)
{
    return equal(str2, str1);
}

Boolean String::equal(const String& str1, const char* str2)
{
    return equal(str1, String(str2));
}

Boolean String::equal(const char* str1, const String& str2)
{
    return equal(String(str1), str2);
}

Boolean String::equalNoCase(const String& str1, const String& str2)
{
    if (str1.size() != str2.size())
	return false;

    const Char16* p = str1.getData();
    const Char16* q = str2.getData();

    Uint32 n = str1.size();

    while (n--)
    {
#ifdef PEGASUS_HAS_EBCDIC
	if (*p <= 255 && *q <= 255)
#else
	if (*p <= 127 && *q <= 127)
#endif
	{
	    if (tolower(*p++) != tolower(*q++))
		return false;
	}
	else if (*p++ != *q++)
	    return false;
    }

    return true;
}

//#define NEWMATCHFUNCTION
#if defined NEWMATCHFUNCTION
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
#ifdef PEGASUS_HAS_EBCDIC
    return ch <= 255 ? tolower(char(ch)) : ch;
#else
    return ch <= 127 ? tolower(char(ch)) : ch;
#endif
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
              res = 1;                                   //  no match
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

#else
////////////////////////////////////////////////////////////////////////////////
//
// String matching routines borrowed from Tcl 8.0:
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// This software is copyrighted by the Regents of the University of
// California, Sun Microsystems, Inc., and other parties.  The following
// terms apply to all files associated with the software unless explicitly
// disclaimed in individual files.
// 
// The authors hereby grant permission to use, copy, modify, distribute,
// and license this software and its documentation for any purpose, provided
// that existing copyright notices are retained in all copies and that this
// notice is included verbatim in any distributions. No written agreement,
// license, or royalty fee is required for any of the authorized uses.
// Modifications to this software may be copyrighted by their authors
// and need not follow the licensing terms described here, provided that
// the new terms are clearly indicated on the first page of each file where
// they apply.
// 
// IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
// FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
// ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
// DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// 
// THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
// IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
// NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
// MODIFICATIONS.
// 
// GOVERNMENT USE: If you are acquiring this software on behalf of the
// U.S. government, the Government shall have only "Restricted Rights"
// in the software and related documentation as defined in the Federal 
// Acquisition Regulations (FARs) in Clause 52.227.19 (c) (2).  If you
// are acquiring the software on behalf of the Department of Defense, the
// software shall be classified as "Commercial Computer Software" and the
// Government shall have only "Restricted Rights" as defined in Clause
// 252.227-7013 (c) (1) of DFARs.  Notwithstanding the foregoing, the
// authors grant the U.S. Government and others acting in its behalf
// permission to use and distribute the software in accordance with the
// terms specified in this license. 
//
////////////////////////////////////////////////////////////////////////////////


/*
 *----------------------------------------------------------------------
 *
 * Tcl_StringMatch --
 *
 *	See if a particular string matches a particular pattern.
 *
 * Results:
 *	The return value is 1 if string matches pattern, and
 *	0 otherwise.  The matching operation permits the following
 *	special characters in the pattern: *?\[] (see the manual
 *	entry for details on what these mean).
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

typedef Uint16 MatchChar;

inline Uint16 _ToLower(Uint16 ch)
{
#ifdef PEGASUS_HAS_EBCDIC
    return ch <= 255 ? tolower(char(ch)) : ch;
#else
    return ch <= 127 ? tolower(char(ch)) : ch;
#endif
}

inline Boolean _Equal(Uint16 ch1, Uint16 ch2, int nocase)
{
    if (nocase)
	return _ToLower(ch1) == _ToLower(ch2);
    else
	return ch1 == ch2;
}

int _StringMatch(
    MatchChar *string,		/* String. */
    MatchChar *pattern,		/* Pattern, which may contain special
				 * characters. */
    int nocase)			/* Ignore case if this is true */
{
    MatchChar c2;

    while (1) {
	/* See if we're at the end of both the pattern and the string.
	 * If so, we succeeded.  If we're at the end of the pattern
	 * but not at the end of the string, we failed.
	 */
	
	if (*pattern == 0) {
	    if (*string == 0) {
		return 1;
	    } else {
		return 0;
	    }
	}
	if ((*string == 0) && (*pattern != '*')) {
	    return 0;
	}

	/* Check for a "*" as the next pattern character.  It matches
	 * any substring.  We handle this by calling ourselves
	 * recursively for each postfix of string, until either we
	 * match or we reach the end of the string.
	 */
	
	if (*pattern == '*') {
	    pattern += 1;
	    if (*pattern == 0) {
		return 1;
	    }
	    while (1) {
		if (_StringMatch(string, pattern, nocase)) {
		    return 1;
		}
		if (*string == 0) {
		    return 0;
		}
		string += 1;
	    }
	}
    
	/* Check for a "?" as the next pattern character.  It matches
	 * any single character.
	 */

	if (*pattern == '?') {
	    goto thisCharOK;
	}

	/* Check for a "[" as the next pattern character.  It is followed
	 * by a list of characters that are acceptable, or by a range
	 * (two characters separated by "-").
	 */
	
	if (*pattern == '[') {
	    pattern += 1;
	    while (1) {
		if ((*pattern == ']') || (*pattern == 0)) {
		    return 0;
		}
		if (_Equal(*pattern, *string, nocase)) {
		    break;
		}
		if (pattern[1] == '-') {
		    c2 = pattern[2];
		    if (c2 == 0) {
			return 0;
		    }
		    if ((*pattern <= *string) && (c2 >= *string)) {
			break;
		    }
		    if ((*pattern >= *string) && (c2 <= *string)) {
			break;
		    }
		    pattern += 2;
		}
		pattern += 1;
	    }
	    while (*pattern != ']') {
		if (*pattern == 0) {
		    pattern--;
		    break;
		}
		pattern += 1;
	    }
	    goto thisCharOK;
	}
    
	/* If the next pattern character is '/', just strip off the '/'
	 * so we do exact matching on the character that follows.
	 */
	
	if (*pattern == '\\') {
	    pattern += 1;
	    if (*pattern == 0) {
		return 0;
	    }
	}

	/* There's no special character.  Just make sure that the next
	 * characters of each string match.
	 */
	
	if (!_Equal(*pattern, *string, nocase)) {
	    return 0;
	}

	thisCharOK: pattern += 1;
	string += 1;
    }
}
#endif

Boolean String::match(const String& str, const String& pattern)
{
    return _StringMatch(
	(Uint16*)str.getData(), (Uint16*)pattern.getData(), 0) != 0;
}

Boolean String::matchNoCase(const String& str, const String& pattern)
{
    return _StringMatch(
	(Uint16*)str.getData(), (Uint16*)pattern.getData(), 1) != 0;
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

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os, const String& str1)
{
    for (Uint32 i = 0, n = str1.size(); i < n; i++)
	os << str1[i];

    return os;
}

String operator+(const String& str1, const String& str2)
{
    return String(str1).append(str2);
}

Boolean operator<(const String& str1, const String& str2)
{
    return String::compare(str1.getData(), str2.getData()) < 0;
}

Boolean operator<=(const String& str1, const String& str2)
{
    return String::compare(str1.getData(), str2.getData()) <= 0;
}

Boolean operator>(const String& str1, const String& str2)
{
    return String::compare(str1.getData(), str2.getData()) > 0;
}

Boolean operator>=(const String& str1, const String& str2)
{
    return String::compare(str1.getData(), str2.getData()) >= 0;
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

int EqualNoCase(const char* s1, const char* s2)
{
    return CompareNoCase(s1, s2) == 0;
}

PEGASUS_NAMESPACE_END
