#include <iostream>
#include "Files.h"
#include "TouchCmd.h"

int TouchCmd(const vector<string>& args)
{
    if (args.size() < 2)
    {
	cerr << args[0] << ": insufficient arguments" << endl;
	return 1;
    }

    // Build up list of files to touch:

    vector<string> filenames;

    for (size_t i = 1; i < args.size(); i++)
	filenames.push_back(args[i]);

    // Touch the files:

    for (size_t i = 0; i < filenames.size(); i++)
    {
	if (!TouchFile(filenames[i]))
	{
	    cerr << args[0] << ": cannot touch: " << filenames[i] << endl;
	    return 1;
	}
    }

    return 0;
}
