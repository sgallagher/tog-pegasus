#include <iostream>
#include "Files.h"
#include "MoveCmd.h"

int MoveCmd(const vector<string>& args)
{
    if (args.size() < 3)
    {
	cerr << args[0] << ": wrong number of arguments" << endl;
	return 1;
    }

    vector<string> from;

    for (size_t i = 1; i < args.size() - 1; i++)
	from.push_back(args[i]);

    if (!CopyFiles(from, args[args.size()-1]))
    {
	cerr << args[0] << ": failed to copy file" << endl;
	return 1;
    }

    for (size_t i = 1; i < args.size() - 1; i++)
	RemoveFile(args[i]);

    return 0;
}
