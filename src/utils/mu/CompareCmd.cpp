#include <iostream>
#include "Files.h"
#include "CompareCmd.h"

int CompareCmd(const vector<string>& args)
{
    if (args.size() != 3)
    {
	cerr << args[0] << ": wrong number of arguments" << endl;
	return 1;
    }

    size_t offset = 0;

    if (!CompareFiles(args[1], args[2], offset))
    {
	cerr << args[0] << ": compare failed" << endl;
	return 1;
    }

    return 0;
}
