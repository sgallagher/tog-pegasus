#include <vector>
#include <cassert>
#include <iostream>
#include "RmDirHierCmd.h"
#include "Files.h"

int RmDirHierCmd(const vector<string>& args)
{
    if (args.size() < 2)
    {
	cerr << args[0] << ": insufficient arguments" << endl;
	return 1;
    }

    // Build up list of files to remove:

    vector<string> filenames;

    for (size_t i = 1; i < args.size(); i++)
	filenames.push_back(args[i]);

    // Remove the files:
    // Note that this calls the RemoveFile with true
    // which causes that command to remove complete
    // hiearchies, not just individual files.

    for (size_t i = 0; i < filenames.size(); i++)
	RemoveFile(filenames[i], true);

    return 0;
}
