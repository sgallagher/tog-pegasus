#include <iostream>
#include <fstream>
#include "Files.h"
#include "StripCmd.h"
#include "Files.h"

static int _Strip(
    const string& arg0,
    const string& startPattern,
    const string& endPattern,
    const string& fileName)
{
    // -- Open input file:

    ifstream is(fileName.c_str());

    if (!is)
    {
	cerr << arg0 << ": failed to open \"" << fileName << "\"" << endl;
	return 1;
    }

    string tmpFileName = fileName + ".tmp";


    // -- Open output file:

    ofstream os(tmpFileName.c_str());

    if (!os)
    {
	cerr << arg0 << ": failed to open \"" << tmpFileName << "\"" << endl;
	return 1;
    }

    // -- Strip out the unwanted lines:

    bool inside = false;
    bool foundStart = false;
    bool foundEnd = false;
    string line;

    while (getline(is, line))
    {
	if (!foundStart && line.substr(0, startPattern.size()) == startPattern)
	{
	    foundStart = true;
	    inside = true;
	}

	if (!inside)
	    os << line << endl;

	if (!foundEnd && line.substr(0, endPattern.size()) == endPattern)
	{
	    foundEnd = true;
	    inside = false;
	}
    }

    is.close();
    os.close();

    if (!foundStart || !foundEnd)
    {
	RemoveFile(tmpFileName);
	return 0;
    }


    // -- Copy the temporary file back over the original:

    if (!CopyFile(tmpFileName, fileName))
    {
	cerr << arg0 << ": failed to copy file" << endl;
	return 1;
    }

    // -- Remove the temporary file:

    RemoveFile(tmpFileName);

    return 0;
}

int StripCmd(const vector<string>& args)
{
    // -- Check arguments:

    if (args.size() < 4)
    {
	cerr << args[0] << ": insufficient arguments" << endl;
	return 1;
    }

    // -- Create complete glob list:

    vector<string> fileNames;

    for (size_t i = 3; i < args.size(); i++)
	Glob(args[i], fileNames);

    for (size_t i = 0; i < fileNames.size(); i++)
    {
	int result = _Strip(args[0], args[1], args[2], fileNames[i]);

	if (result != 0)
	    return result;
    }

    return 0;
}
