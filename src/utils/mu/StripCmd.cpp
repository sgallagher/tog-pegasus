#include <iostream>
#include <fstream>
#include "Files.h"
#include "StripCmd.h"
#include "Files.h"

int StripCmd(const vector<string>& args)
{
    // -- Check arguments:

    if (args.size() < 4)
    {
	cerr << args[0] << ": insufficient arguments" << endl;
	return 1;
    }

    string startPattern = args[1];
    string endPattern = args[2];
    string fileName = args[3];

    // -- Open input file:

    ifstream is(fileName.c_str());

    if (!is)
    {
	cerr << args[0] << ": failed to open \"" << fileName << "\"" << endl;
	return 1;
    }

    string tmpFileName = fileName + ".tmp";


    // -- Open output file:

    ofstream os(tmpFileName.c_str());

    if (!os)
    {
	cerr << args[0] << ": failed to open \"" << tmpFileName << "\"" << endl;
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
	cerr << args[0] << ": failed to copy file" << endl;
	return 1;
    }

    // -- Remove the temporary file:

    RemoveFile(tmpFileName);

    return 0;
}
