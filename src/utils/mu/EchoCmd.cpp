#include <iostream>
#include "EchoCmd.h"

int EchoCmd(const vector<string>& args)
{
    for (size_t i = 1; i < args.size(); i++)
    {
	cout << args[i];

	if (i + 1 != args.size())
	    cout << ' ';
    }

    cout << endl;

    return 0;
}
