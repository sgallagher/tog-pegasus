#include "Config.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include "Files.h"

static void _SplitPath(const string& path, vector<string>& components)
{
    char* tmp = new char[path.size() + 1];
    strcpy(tmp, path.c_str());

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

    if (pos == (size_t)-1)
    {
	dirname = ".";
	basename = path;
    }
    else
    {
	dirname = path.substr(0, pos);
	basename = path.substr(pos + 1);
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

static int _Tcl_StringMatch(
    char *string,		/* String. */
    char *pattern)		/* Pattern, which may contain special
				 * characters. */
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
		if (_Tcl_StringMatch(string, pattern)) {
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

inline bool MatchString(const string& pattern, const string& str)
{
    return _Tcl_StringMatch((char*)str.c_str(), (char*)pattern.c_str()) != 0;
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
	pattern.erase(pattern.begin() + pattern.size() - 1);

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

#ifdef OS_TYPE_WINDOWS
    ifstream is(from_file.c_str(), ios::binary);
#else
    ifstream is(from_file.c_str());
#endif

    if (!is)
	return false;

    // Open output file:

#ifdef OS_TYPE_WINDOWS
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
#ifdef OS_TYPE_WINDOWS
    ifstream is1(filename1.c_str(), ios::binary);
#else
    ifstream is1(filename1.c_str());
#endif

    if (!is1)
	return false;

#ifdef OS_TYPE_WINDOWS
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

    return true;
}
