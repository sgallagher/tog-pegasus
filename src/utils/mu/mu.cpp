#include <iostream>
#include "Config.h"
#include "CompareCmd.h"
#include "CopyCmd.h"
#include "DependCmd.h"
#include "EchoCmd.h"
#include "MkDirHierCmd.h"
#include "MoveCmd.h"
#include "PwdCmd.h"
#include "RmCmd.h"
#include "RmDirHierCmd.h"
#include "TouchCmd.h"
#include "Files.h"

const char HELP[] = 
"\n"
"MU (MakeUtility) Version 1.0\n"
"\n"
"Usage: mu command arguments ...\n"
"\n"
"Where command is one of the following:\n"
"    rm, rmdirhier, mkdirhier, echo, touch, pwd, copy, move, compare depend\n";

const char* PROGRAM_NAME = "";

int main(int argc, char** argv)
{
    // Process the help option:

    if (argc == 2 && strcmp(argv[1], "-help") == 0 || argc < 2)
    {
	cerr << HELP << endl;
	exit(1);
    }

    // Build up the argument list (expanding as we go)

    vector<string> args;

    args.push_back(argv[1]);

    for (int i = 2; i < argc; i++)
    {
	vector<string> argsOut;

	if (Glob(argv[i], argsOut))
	    args.insert(args.end(), argsOut.begin(), argsOut.end());
	else
	    args.push_back(argv[i]);
    }

    // Execute the command:

    int result = 0;

#if 1
    if (args[0] == "rm")
	result = RmCmd(args);
    else if (args[0] == "rmdirhier")
	result = RmDirHierCmd(args);
    else if (args[0] == "mkdirhier")
	result = MkDirHierCmd(args);
    else if (args[0] == "echo")
	result = EchoCmd(args);
    else if (args[0] == "touch")
	result = TouchCmd(args);
    else if (args[0] == "pwd")
	result = PwdCmd(args);
    else if (args[0] == "copy")
	result = CopyCmd(args);
    else if (args[0] == "move") 
	result = MoveCmd(args);
    else if (args[0] == "compare")
	result = CompareCmd(args);
    else if (args[0] == "depend")
	result = DependCmd(args);
    else
    {
	result = 1;
	cerr << args[0] << ": unknown command: \"" << args[0] << "\"" << endl;
    }
#endif

    exit(result);
    return 0;
}
