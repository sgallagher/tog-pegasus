#include <iostream>
#include "PwdCmd.h"
#include "Files.h"

int PwdCmd(const vector<string>& args)
{
    if (args.size() != 1)
    {
	cerr << args[0] << ": extraneous arguments" << endl;
	return 1;
    }

    string path;

    if (!GetCwd(path))
    {
	cerr << args[0] << ": failed to access working directory" << endl;
	return 1;
    }

    cout << path << endl;

    return 0;
}
