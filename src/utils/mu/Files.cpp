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
// Author: Michael E. Brasher
//
//%=============================================================================

#include "Config.h"
#include "Files.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>

static void _SplitPath(const string& path, vector<string>& components)
{
    char* tmp = new char[path.size() + 1];
    strcpy(tmp, path.c_str());

#if defined (OS_VMS)
    components.push_back (tmp);
#else
    if (path[0] == '/')
	components.push_back("/");

    for (char* p = strtok(tmp, "/"); p != NULL; p = strtok(NULL, "/"))
	components.push_back(p);

    // Fixup the drive letter:

    if (components.size() > 1)
    {
	string s = components[0];

	if (s.size() == 2 && isalpha(s[0]) && s[1] == ':')
	{
	    components[0] += "/" + components[1];
	    components.erase(components.begin() + 1, components.begin() + 2);
	}
    }

    delete [] tmp;
#endif
}

// string.find_last_of() is broken in GNU C++.

static inline size_t _find_last_of(const string& str, char c)
{
    const char* p = strrchr(str.c_str(), c);

    if (p)
	return size_t(p - str.c_str());

    return (size_t)-1;
}

void _SplitPath(
    const string& path,
    string& dirname,
    string& basename)
{
    size_t pos = _find_last_of(path, '/');

#if defined (OS_VMS)
    size_t pos1 = _find_last_of (path, ']');
    if ((pos == (size_t) -1) && (pos1 == (size_t) -1))
#else
    if (pos == (size_t)-1)
#endif
    {
	dirname = ".";
	basename = path;
    }
    else
    {
#if defined (OS_VMS)
      // Did we find a slash?
      if (pos == (size_t) -1)
      {
        // No. Must be a close bracket.
        dirname = path.substr (0, pos1 + 1);
        basename = path.substr (pos1 + 1);
      }
      else
      {
        // Yes.
        dirname = path.substr (0, pos);
        basename = path.substr (pos + 1);
      }
#else
        dirname = path.substr(0, pos);
        basename = path.substr(pos + 1);
#endif
    }
}

bool RemoveFile(const string& path, bool recurse)
{
    if (!IsDir(path))
	return RemoveFile(path);

    if (!recurse)
	return RemoveDir(path);

    vector<string> filenames;

    if (GetDirEntries(path, filenames))
    {
	string save_cwd;
	GetCwd(save_cwd);

	if (!ChangeDir(path))
	    return false;

	for (size_t i = 0; i < filenames.size(); i++)
	    RemoveFile(filenames[i], true);

	if (!ChangeDir(save_cwd))
	    return false;
    }

    return RemoveDir(path);
}

bool MkDirHier(const string& path)
{
    vector<string> components;

    _SplitPath(path, components);

    for (size_t i = 0; i < components.size(); i++)
    {
	if (!IsDir(components[i]))
	{
	    if (!MakeDir(components[i].c_str()))
		return false;
	}

	if (!ChangeDir(components[i]))
	    return false;
    }

    return true;
}
//ATTN: KS 22 Apr 2002 - Put in nonlicensed match function but left the
// old one enabled for the minute until test complete.
// ATTN: KS 22 Apr 2002 P1 Test new and delete old TCL licensed code.
//#define newmatchfunct
#if defined NEWMATCHFUNCT
typedef char MatchChar;

/*
inline Boolean _Equal(MatchChar ch1, MatchChar
     ch2, int nocase)
{
	return ch1 == ch2;
}
*/
static const MatchChar *
_matchrange(const MatchChar *range, MatchChar c, int nocase)
{
  const MatchChar *p = range;
  const MatchChar *rstart = range + 1;
  const MatchChar *rend = 0;
  MatchChar compchar;

  for (rend = rstart; *rend && *rend != ']'; rend++);
  if (*rend == ']') {  // if there is an end to this thing
    for (compchar = *rstart; rstart != rend; rstart++) {
      if (*rstart == c)
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
    int nocase			/* Ignore case if this is true */
    )
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
            if (*pat++ !=*str++)                         // if they don't match
            //if (!_Equal(*pat++, *str++, nocase))         // if they don't match
              done = 1;                                  //   bail.
          }
        }  // end ("pattern is not ambiguous (*)" logic
      } // end logic when pattern and string still have data
    } // end logic when pattern still has data
  } // end main loop
  return res;
}

#else

/*
 *----------------------------------------------------------------------
 *
 * Tcl_StringMatch --
 *
 *	See if a particular string MatchStringes a particular pattern.
 *
 * Results:
 *	The return value is 1 if string MatchStringes pattern, and
 *	0 otherwise.  The MatchStringing operation permits the following
 *	special characters in the pattern: *?\[] (see the manual
 *	entry for details on what these mean).
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int _StringMatch(
    char *string,		/* String. */
    char *pattern,		/* Pattern, which may contain special characters*/
    int  nocase)    		/* nocase - Do nocase test Not used.. */
{
    char c2;

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

	/* Check for a "*" as the next pattern character.  It MatchStringes
	 * any substring.  We handle this by calling ourselves
	 * recursively for each postfix of string, until either we
	 * MatchString or we reach the end of the string.
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

	/* Check for a "?" as the next pattern character.  It MatchStringes
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
		if (*pattern == *string) {
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
	 * so we do exact MatchStringing on the character that follows.
	 */

	if (*pattern == '\\') {
	    pattern += 1;
	    if (*pattern == 0) {
		return 0;
	    }
	}

	/* There's no special character.  Just make sure that the next
	 * characters of each string MatchString.
	 */

	if (*pattern != *string) {
	    return 0;
	}

	thisCharOK: pattern += 1;
	string += 1;
    }
}
#endif

inline bool MatchString(const string& pattern, const string& str)
{
    return _StringMatch((char*)str.c_str(), (char*)pattern.c_str(), 0) != 0;
}

static bool _contains_special_chars(const string& str)
{
    const char* p = str.c_str();

    return
	strchr(p, '[') || strchr(p, ']') || strchr(p, '*') || strchr(p, '?');
}

bool Glob(const string& pattern_, vector<string>& fileNames)
{
    // Remove trailing slashes:

    string pattern = pattern_;

    while (pattern.size() > 0 && pattern[pattern.size()-1] == '/')
    {
#ifdef OS_TRU64
	pattern.remove(pattern.size() - 1);
#else
	pattern.erase(pattern.end() - 1);
#endif
    }

    // Split the pattern into directory name and base name:

    string dirname;
    string basename;
    _SplitPath(pattern, dirname, basename);

    if (!_contains_special_chars(basename))
	fileNames.push_back(pattern_);
    else
    {
	// Find all files in the given directory MatchStringing the pattern:

	bool found = false;
	vector<string> filenames;

	if (!GetDirEntries(dirname, filenames))
	    return false;

	for (size_t i = 0; i < filenames.size(); i++)
	{
	    if (MatchString(basename, filenames[i]))
	    {
		found = true;

		if (dirname == ".")
		    fileNames.push_back(filenames[i]);
		else
		    fileNames.push_back(dirname + "/" + filenames[i]);
	    }
	}

	if (!found)
	    return false;
    }

    return true;
}

bool CopyFile(const string& from_file, const string& to_file)
{
    // Open input file:

#ifdef OS_WINDOWS
    ifstream is(from_file.c_str(), ios::binary);
#else
    ifstream is(from_file.c_str());
#endif

    if (!is)
	return false;

    // Open output file:

#ifdef OS_WINDOWS
    ofstream os(to_file.c_str(), ios::binary);
#else
    ofstream os(to_file.c_str());
#endif

    if (!os)
	return false;

    // ATTN: optimize this for speed! Use block-oriented copy approach.
    // Copy the blocks:

    char c;

    while (is.get(c))
	os.put(c);

    return true;
}

bool CopyFiles(const vector<string>& from, const string& to)
{
    // There are two cases. If there is more than one from file, then the
    // to argument must designate a directory. If there is exactly one from
    // file then the to may designate either the destination directory or
    // the new file name.

    if (from.size() > 1)
    {
	if (!IsDir(to))
	    return false;

	bool success = true;

	for (size_t i = 0; i < from.size(); i++)
	{
	    string dirname;
	    string basename;
	    _SplitPath(from[i], dirname, basename);

	    if (!CopyFile(from[i], to + "/" + basename))
		success = false;
	}

	return success;
    }
    else if (from.size() == 1)
    {
	if (IsDir(to))
	{
	    string dirname;
	    string basename;
	    _SplitPath(from[0], dirname, basename);

	    return CopyFile(from[0], to + "/" + basename);
	}
	else
	    return CopyFile(from[0], to);
    }
    else
	return false;
}

bool CompareFiles(
    const string& filename1,
    const string& filename2,
    size_t& offset)
{
#ifdef OS_WINDOWS
    ifstream is1(filename1.c_str(), ios::binary);
#else
    ifstream is1(filename1.c_str());
#endif

    if (!is1)
	return false;

#ifdef OS_WINDOWS
    ifstream is2(filename2.c_str(), ios::binary);
#else
    ifstream is2(filename2.c_str());
#endif

    if (!is2)
	return false;

    char c1;
    char c2;
    offset = 0;

    for (;;)
    {
	bool more1 = is1.get(c1) ? true : false;
	bool more2 = is2.get(c2) ? true : false;

	if (!more1 || !more2)
	    return more1 == more2;

	offset++;

	if (c1 != c2)
	    return false;
    }

#if !defined (OS_VMS)
    return true;
#endif
}
