#include "Config.h"
#include <set>
#include <string>
#include <cstdio>
#include <vector>
#include <iostream>
#include <cstddef>
#include <cassert>
#include "DependCmd.h"

using namespace std;

static const char* programName = "";
bool warn = false;

void ErrorExit(const string& message)
{
    fprintf(stderr, "%s: Error: %s\n", programName, message.c_str());
    exit(1);
}

void Warning(const string& message)
{
    fprintf(stderr, "%s: Warning: %s\n", programName, message.c_str());
}

void ProcessOptions(
    int& argc, char**& argv, 
    vector<string>& includePath,
    string& objectDir,
    bool& warn)
{
    int i;

    for (i = 0; i < argc; i++)
    {
	const char* p = argv[i];

	if (*p != '-')
	    break;

	p++;

	if (*p == 'I')
	{
	    if (*++p)
		includePath.push_back(p);
	    else
		ErrorExit("Missing argument for -I option");
	}
	else if (*p == 'O')
	{
	    if (*++p)
		objectDir = p;
	    else
		ErrorExit("Missing argument for -O option");
	}
	else if (*p == 'W' && p[1] == '\0')
	{
	    warn = true;
	}
	else
	    ErrorExit(string("Unknown option: -") + *p);
    }

    argc -= i;
    argv += i;
}

void PrintVector(const vector<string>& v)
{
    for (size_t i = 0; i < v.size(); i++)
	printf("%s\n", v[i].c_str());
}

////////////////////////////////////////////////////////////////////////////////
//
// GetIncludePath():
//
//     Get the include path from an #include directive.
//
////////////////////////////////////////////////////////////////////////////////

bool GetIncludePath(
    const string& fileName,
    size_t lineNumber,
    const char* line, 
    string& path,
    char& openDelim)
{
    if (line[0] == '#')
    {
	const char* p = line + 1;

	// Skip whitespace:

	while (isspace(*p))
	    p++;

	// Check for "include" keyword:

	const char INCLUDE[] = "include";

	if (memcmp(p, INCLUDE, sizeof(INCLUDE) - 1) == 0)
	{
	    // Advance past "include" keyword:

	    p += sizeof(INCLUDE) - 1;

	    // Skip whitespace:

	    while (isspace(*p))
		p++;

	    // Expect opening '"' or '<':

	    if (*p != '"' && *p != '<')
	    {
		char message[64];
		sprintf(message, 
		    "corrupt #include directive at %s(%d)",
		    fileName.c_str(),
		    lineNumber);
		ErrorExit(message);
	    }

	    openDelim = *p++;

	    // Skip whitespace:

	    while (isspace(*p))
		p++;

	    // Store pointer to start of path:

	    const char* start = p;

	    // Look for closing '"' or '>':

	    while (*p && *p != '"' && *p != '>')
		p++;

	    if (*p != '"' && *p != '>')
	    {
		char message[64];
		sprintf(message, 
		    "corrupt #include directive at %s(%d)",
		    fileName.c_str(),
		    lineNumber);
		ErrorExit(message);
	    }

	    // Find end of the path (go backwards, skipping whitespace
	    // between the closing delimiter and the end of the path:

	    if (p == start)
	    {
		char message[64];
		sprintf(message, 
		    "empty path in #include directive at %s(%d)",
		    fileName.c_str(),
		    lineNumber);
		ErrorExit(message);
	    }

	    p--;

	    while (isspace(*p))
		p--;

	    if (p == start)
	    {
		char message[64];
		sprintf(message, 
		    "empty path in #include directive at %s(%d)",
		    fileName.c_str(),
		    lineNumber);
		ErrorExit(message);
	    }

	    path.assign(start, p - start + 1);
	    return true;
	}
    }

    return false;
}

FILE* FindFile(
    const vector<string>& includePath, 
    const string& path, 
    char openDelim, 
    string& fullPath)
{
    // If the opening delimiter was '"', then check the current
    // directory first:

    if (openDelim == '"')
    {
	FILE* fp = fopen(path.c_str(), "rb");

	if (fp)
	{
	    fullPath = path;
	    return fp;
	}
    }

    // Search the include path for the file:

    vector<string>::const_iterator first = includePath.begin();
    vector<string>::const_iterator last = includePath.end();

    for (; first != last; first++)
    {
	fullPath = *first;
	fullPath += '/';
	fullPath += path;

	FILE* fp = fopen(fullPath.c_str(), "rb");

	if (fp)
	    return fp;
    }

    return NULL;
}

void PrintDependency(
    const string& objectFileName,
    const string& fileName)
{
    printf("%s: %s\n\n", objectFileName.c_str(), fileName.c_str());
}

void ProcessFile(
    const string& objectFileName, 
    const string& fileName, 
    FILE* fp,
    const vector<string>& includePath,
    size_t nesting,
    set<string>& cache)
{
    PrintDependency(objectFileName, fileName);

    if (nesting == 100)
    {
	ErrorExit(
	    "Infinite include file recursion? nesting level reached 100");
    }

    assert(fp != NULL);

    // For each line in the file:

    char line[4096];
    size_t lineNumber = 1;

    for (; fgets(line, sizeof(line), fp) != NULL; lineNumber++)
    {
	// Check for include directive:

	string path;
	char openDelim;

	if (line[0] == '#' && 
	    GetIncludePath(fileName, lineNumber, line, path, openDelim))
	{
	    // ATTN: danger! not distinguising between angle brack delimited
	    // and quote delimited paths!
	    
	    set<string>::const_iterator pos = cache.find(path);

	    if (pos != cache.end())
		continue;

	    cache.insert(path);

	    string fullPath;
	    FILE* fp = FindFile(includePath, path, openDelim, fullPath);

	    if (!fp)
	    {
		if (warn)
		    Warning("header file not found: " + path);
	    }
	    else
	    {
		ProcessFile(objectFileName, fullPath, fp, includePath, 
		    nesting + 1, cache);
	    }
	}
    }

    fclose(fp);
}

int DependCmdMain(int argc, char** argv)
{
    // Check arguments:

    if (argc == 1)
    {
	fprintf(stderr, 
	    "Usage: %s [-W]? [-Oobject_dir]? [-Iinclude_dir]* source_files...\n"
	    "Where: \n"
	    "    -W - warn doube include files which cannot be found\n"
	    "    -O - prepend this directory to object files\n"
	    "    -I - search this directory for header files\n",
	    argv[0]);
	exit(1);
    }

    // Extract the program name:

    programName = argv[0];
    argc--;
    argv++;

    // Process all options:

    vector<string> includePath;
    string objectDir;
    ProcessOptions(argc, argv, includePath, objectDir, warn);

    // There must be at least one source file; print error if not:

    if (argc < 1)
	ErrorExit("no source files given");

    // Process each file:

    for (int i = 0; i < argc; i++)
    {
	string fileName = argv[i];

	// Open the file:

	FILE* fp = fopen(argv[i], "rb");

	if (fp == NULL)
	{
	    string message = "failed to open file: \"" + fileName + "\"";
	    ErrorExit(message);
	}

	const char* start = fileName.c_str();
	const char* dot = strrchr(start, '.');

	if (strcmp(dot, ".cpp") != 0)
	    ErrorExit("bad extension: must be \".cpp\": " + fileName);

	string objectFileName;

	if (objectDir.size())
	{
	    objectFileName = objectDir;
	    objectFileName += '/';
	}

	objectFileName.append(start, dot - start);
	objectFileName += ".obj";

	set<string> cache;

	ProcessFile(objectFileName, fileName, fp, includePath, 0, cache);
    }

    return 0;
}

int DependCmd(const vector<string>& args)
{
    // Dummy up argc/argv structures and call DependCmdMain():

    int argc = args.size();
    char** argv = new char*[args.size()];

    for (int i = 0; i < argc; i++)
	argv[i] = (char*)args[i].c_str();

    int result = DependCmdMain(argc, argv);

    delete [] argv;

    return result;
}
