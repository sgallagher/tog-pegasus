#include <iostream>
#include <cassert>
#include <Pegasus/Common/OptionManager.h>

using namespace std;
using namespace Pegasus;

void test(int& argc, char** argv)
{
    OptionManager om;

    om.registerOption(new Option("host", "", true, Option::STRING,
	Array<String>(), "", "", "host"));

    om.registerOption(new Option("port", "80", false, Option::POSITIVE_INTEGER,
	Array<String>(), "", "", "port"));
	

    Array<String> colors;
    colors.append("red");
    colors.append("green");
    colors.append("blue");

    om.registerOption(
	new Option("color", "", true, Option::STRING, colors, "", "", "color"));

    om.registerOption(new Option("trace", "false", false, Option::BOOLEAN, 
	Array<String>(), "", "", "trace"));


    cout << "=== Default options:" << endl;
    om.print();
    om.mergeCommandLine(argc, argv);

    cout << "=== Selected options:" << endl;
    om.print();
}

int main()
{
    try
    {
	// Emulate a command line:

	char* argv[] = 
	{
	    "main",
	    "-host",
	    "www.opengroup.org",
	    "-port",
	    "8080",
	    "-trace",
	    "-color", 
	    "red",
	    "-one",
	    "two",
	    "-three",
	    "four",
	    "-five",
	    0
	};

	int argc = sizeof(argv) / sizeof(argv[0]) - 1;

	test(argc, argv);

	// The routine should have removed all the processed options
	// and left the following:

	assert(argc == 6);
	assert(strcmp(arg[0], "main") == 0);
	assert(strcmp(arg[1], "-one") == 0);
	assert(strcmp(arg[2], "two") == 0);
	assert(strcmp(arg[3], "-three") == 0);
	assert(strcmp(arg[4], "four") == 0);
	assert(strcmp(arg[5], "-five") == 0);
    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "++++ passed all tests" << endl;

    return 0;
}
