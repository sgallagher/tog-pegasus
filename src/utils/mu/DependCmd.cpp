#include "Config.h"
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <fstream>
#include "Files.h"

using namespace std;

//------------------------------------------------------------------------------
//
// GetFileName()
//
//	Extract the file name part of a line of the form:
//
//		#line 1 "D:<filenaem>"
//
//	Translate all backslashes to forward slashes.
//	Escape all spaces.
//	
//------------------------------------------------------------------------------

static string GetFileName(const char* line)
{
    const char* first = strchr(line, '"') + 1;
    const char* last = strrchr(line, '"');

    string fileName;

    while (first != last)
    {
	char c = *first++;

	if (c == '\\' && *first == '\\')
	{
	    fileName += '/';
	    first++;
	}
	else if (c == ' ')
	{
	    fileName += '\\';
	    fileName += c;
	}
	else
	    fileName += c;
    }

    return fileName;
}

int DependCmd(const vector<string>& args)
{
    // Check arguments:

    if (args.size() != 4)
    {
	cerr << "Usage: depend input-file output-file obj-prefix" << endl;
	return 1;
    }

    // Translate backslashes to forward slashes:

    string objectPrefix = args[3];

    for (size_t i = 0; i < objectPrefix.size(); i++)
    {
	if (objectPrefix[i] == '\\')
	    objectPrefix[i] = '/';
    }

    // Open the input file:

    FILE* fp = fopen(args[1].c_str(), "rt");

    if (fp == NULL)
    {
	cerr << args[0] << ": failed to open " << args[1] << endl;
	return 1;
    }

    // Open the output file:

    ofstream os(args[2].c_str(), ios::app);

    if (!os)
    {
	cerr << args[0] << ": failed to open " << args[2] << endl;
	return 1;
    }

    // For each line:

    string objectFileName;
    char line[16 * 1024];

    while (fgets(line, sizeof(line), fp) != NULL)
    {
	if (line[0] == '#' && memcmp(line, "#line 1 ", 8) == 0)
	{
	    string fileName = GetFileName(line);

	    if (objectFileName.size())
	    {
		os << objectPrefix << objectFileName;
		os << ": " << fileName << endl;
	    }
	    else
	    {
		size_t pos = fileName.find(".cpp");
		assert(pos != (size_t)-1);

		objectFileName = fileName.substr(0, pos);
		objectFileName += ".obj";
	    }
	}
    }

    fclose(fp);

    // Remove the input file:

    RemoveFile(args[1]);
    return 0;
}
