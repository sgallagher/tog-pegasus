#include <iostream>
#include <fstream>
#include "Files.h"
#include "PrependCmd.h"
#include "Files.h"

static int _Prepend(
    const string& arg0,
    const string& prependFile,
    const string& changeFile)
{

    // -- Open first input file:

    ifstream is1(prependFile.c_str());

    if (!is1)
    {
	cerr << arg0 << ": failed to open \"" << prependFile << "\"" << endl;
	return 1;
    }

    // -- Open second input file:

    ifstream is2(changeFile.c_str());

    if (!is2)
    {
	cerr << arg0 << ": failed to open \"" << changeFile << "\"" << endl;
	return 1;
    }

    // -- Open output temporary file:

    string tmpFileName = changeFile + ".tmp";

    ofstream os(tmpFileName.c_str());

    if (!os)
    {
	cerr << arg0 << ": failed to open \"" << tmpFileName << "\"" << endl;
	return 1;
    }

    // -- Write contents of two input files to the output file:

    string line;

    while (getline(is1, line))
	os << line << endl;

    while (getline(is2, line))
	os << line << endl;

    is1.close();
    is2.close();
    os.close();

    // -- Copy temporary file back over change file:

    if (!CopyFile(tmpFileName, changeFile))
    {
	cerr << arg0 << ": failed to copy file" << endl;
	return 1;
    }

    // -- Remove the temporary file:

    RemoveFile(tmpFileName);

    return 0;
}

int PrependCmd(const vector<string>& args)
{
    // -- Check arguments:

    if (args.size() < 3)
    {
	cerr << args[0] << ": insufficient arguments" << endl;
	return 1;
    }

    // -- Create glob list:

    vector<string> fileNames;

    for (size_t i = 2; i < args.size(); i++)
	Glob(args[i], fileNames);

    // -- For each matching file:

    for (size_t i = 0; i < fileNames.size(); i++)
    {
	int result = _Prepend(args[0], args[1], fileNames[i]);

	if (result != 0)
	    return result;
    }

    return 0;
}
