#include <iostream>
#include "MkDirHierCmd.h"
#include "Files.h"

int MkDirHierCmd(const vector<string>& args)
{
    if (args.size() != 2)
    {
	cerr << args[0] << ": insufficient arguments" << endl;
	return 1;
    }

    if (!MkDirHier(args[1]))
    {
	cerr << args[0] << ": failed" << endl;
	return 1;
    }

    return 0;
}
