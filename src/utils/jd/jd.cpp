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
#include <direct.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cctype>
#include <vector>

using namespace std;

const char* argv0;

static string _cwd;
static int _drive;

bool _RemoveFile(const string& path)
{
    return _unlink(path.c_str()) == 0;
}

bool _ChangeDir(const string& path)
{
    if (path.size() >= 2)
    {
	if (isalpha(path[0]) && path[1] == ':')
	{
	    int drive = tolower(path[0]) - 'a';
	    _chdrive(drive);
	}
    }

    return _chdir(path.c_str()) == 0;
}

bool _GetCwd(string& path)
{
    char* tmp = _getcwd(NULL, 0);

    if (!tmp)
	return false;

    path = tmp;
    delete [] tmp;
    return true;
}

bool _MakeDir(const string& path)
{
    return _mkdir(path.c_str()) == 0;
}

void _Split(const string& path, vector<string>& components)
{
    char *p = strdup(path.c_str());
    char *porig = p;

    for (p = strtok(p, ";"); p; p = strtok(NULL, ";"))
	components.push_back(p);

    free(porig);
}


void _WriteLastBatchFile()
{
    ofstream os("c:\\temp\\jdlast.bat");

    if (!os)
    {
	cerr << argv0 << ": cannot open \"C:\\temp\\jdlast.bat\"" << endl;
	exit(1);
    }

    os << "@echo off" << endl;
    os << (char)(_drive + 'A' - 1) << ":" << endl;
    os << "cd " << _cwd << endl;
}

void _CreateBatchFileAndExit(const string& path_)
{
    string path = path_;

    for (size_t i = 0; i < path.size(); i++)
    {
	if (path[i] == '/')
	    path[i] = '\\';
    }

    {
	ofstream os("c:\\temp\\jdtmp.bat");

	if (!os)
	{
	    cerr << argv0 << ": cannot open \"C:\\temp\\jdtmp.bat\"" << endl;
	    exit(1);
	}

	os << "@echo off" << endl;

	if (path.size() >= 2 && isalpha(path[0]) && path[1] == ':')
	    os << path.substr(0, 2) << endl;

	os << "cd " << path << endl;
    }

    // Build batch file to restore CWD with "jd -"

    _WriteLastBatchFile();

    cout << path << endl;
    exit(0);
}

const string ALIAS_DIR = "C:/jdalias"; 

bool _IsLegalAlias(const string& alias)
{
    for (size_t i = 0; i < alias.size(); i++)
    {
	char c = alias[i];

	if (!isalnum(c) && c != '_' && c != '-')
	    return false;
    }

    return true;
}

void _AliasCurrentDirectory(const string& alias)
{
    if (!_IsLegalAlias(alias))
    {
	cerr << argv0 << ": aliases may only contain these characters: ";
	cerr << "[A-Za-z0-9_-]" << endl;
    }

    // Create JD home directory if it does not exist:

    _MakeDir(ALIAS_DIR);

    // Get the current working directory:

    string cwd;

    if (!_GetCwd(cwd))
    {
	cerr << argv0 << ": cannot get current directory" << endl;
	exit(1);
    }

    // Create file to hold path:	    

    string file_name = ALIAS_DIR + "/" + alias;

    ofstream os(file_name.c_str());

    if (!os)
    {
	cerr << argv0 << ": failed to open \"" << file_name << "\"" << endl;
	exit(1);
    }

    os << cwd << endl;

    cout << "created alias \"" << alias << "\"" << endl;

    exit(1);
}

void _ChangeToAliasedDirectory(const string& alias)
{
    if (!_IsLegalAlias(alias))
	return;

    string file_name = ALIAS_DIR + "/" + alias;

    ifstream is(file_name.c_str());

    if (!is)
	return;

    string path;

    if (!getline(is, path))
	return;

    if (_ChangeDir(path))
	_CreateBatchFileAndExit(path);
}

void _DeleteDirectoryAlias(const string& alias)
{
    if (!_IsLegalAlias(alias))
    {
	cerr << argv0 << ": aliases may only contain these characters: ";
	cerr << "[A-Za-z0-9_-]" << endl;
    }

    string file_name = ALIAS_DIR + "/" + alias;

    if (!_RemoveFile(file_name))
    {
	cerr << "no such alias: \"" << alias << "\"" << endl;
	exit(1);
    }

    exit(1);
}

int main(int argc, char** argv)
{
    argv0 = argv[0];

    // Check for -a option (diretory aliasing)

    if (argc == 3 && string(argv[1]) == "-a")
    {
	_AliasCurrentDirectory(argv[2]);
	exit(1);
    }

    // Check for -d option (delete diretory alias)

    if (argc == 3 && string(argv[1]) == "-d")
    {
	_DeleteDirectoryAlias(argv[2]);
	exit(1);
    }

    // Check arguments:

    if (argc > 2)
    {
	cerr << "Usage: " << argv[0] << " path" << endl;
	exit(1);
    }

    // Save the current working directory:

    if (!_GetCwd(_cwd))
    {
	cerr << argv0 << ": cannot access current directory" << endl;
	exit(1);
    }

    _drive = _getdrive();

    // If only one argument, change to home:

    if (argc == 1)
    {
	const char* home = getenv("HOME");

	if (!home)
	{
	    cerr << "HOME environment variable not defined" << endl;
	    exit(1);
	}

	if (!_ChangeDir(home))
	{
	    cerr << "Directory given by HOME environment variable does not ";
	    cerr << "exist: \"" << home << "\"" << endl;
	    exit(1);
	}

	_CreateBatchFileAndExit(home);
    }

    // If user typed "jd -"

    if (string(argv[1]) == "-")
	exit(2);

    // Attempt to change to directory:

    string path = argv[1];

    if (_ChangeDir(path))
	_CreateBatchFileAndExit(path);
    else
    {
	const char* jdpath = getenv("JDPATH");

	if (!jdpath)
	{
	    cerr << "JDPATH environment variable not defined" << endl;
	    exit(1);
	}

	vector<string> components;

	_Split(jdpath, components);

	for (size_t i = 0; i < components.size(); i++)
	{
	    string tmp = components[i] + "/" + path;

	    if (_ChangeDir(tmp))
		_CreateBatchFileAndExit(tmp);
	}
    }

    // Finally check to see if path is an alias:

    _ChangeToAliasedDirectory(path);

    // Not found:

    cerr << argv[0] << ": " << path << " does not exist" << endl;
    exit(1);

    return 0;
}
