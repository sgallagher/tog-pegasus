#include <iostream>
#include <cassert>
#include <Pegasus/Common/OptionManager.h>

using namespace std;
using namespace Pegasus;

static char* colors[] = { "red", "green", "blue" };
static const Uint32 NCOLORS = sizeof(colors) / sizeof(colors[0]);

//struct OptionRow
//{
//    const char* optionName;
//    const char* defaultValue;
//    int required;
//    Option::Type type;
//    char** domain;
//    Uint32 domainSize;
//    const char* commandLineOptionName;
//};

static struct OptionRow options[] =
{
    {"host", "", true, Option::STRING, 0, 0, "h"},
    {"port", "80", false, Option::WHOLE_NUMBER, 0, 0, "p"},
    {"color", "red", false, Option::STRING, colors, NCOLORS, "c"},
    {"trace", "false", false, Option::BOOLEAN, 0, 0, "t"},
    {"message", "Hello World", false, Option::STRING, 0, 0, "m"},
    {"falsetest","false",false, Option::BOOLEAN, 0, 0, "f",}
};

static const Uint32 NUM_OPTIONS = sizeof(options) / sizeof(options[0]);

void test01(int& argc, char** argv)
{
    OptionManager om;
    om.registerOptions(options, NUM_OPTIONS);

    // cout << "=== Default options:" << endl;
    om.print();
    om.mergeCommandLine(argc, argv);

    // cout << "=== Selected options:" << endl;
    om.print();

    // Check for missing required option:
    om.checkRequiredOptions();

    assert(om.valueEquals("trace","true"));
    assert(om.valueEquals("falsetest","false"));
}

void test02()
{
    OptionManager om;
    om.registerOptions(options, NUM_OPTIONS);

    om.mergeFile("config.dat");
    om.print();

    // Check for missing required option:
    om.checkRequiredOptions();
}

int main()
{
    try
    {
	// Emulate a command line:

	char* argv[] = 
	{
	    "main", "-h", "www.opengroup.org", "-p", "8080", "-t", "-c", "blue",
	    "-one", "two", "-three", "four", "-five", 0
	};
	int argc = sizeof(argv) / sizeof(argv[0]) - 1;
	test01(argc, argv);

	// The routine should have removed all the processed options
	// and left the following:

	assert(argc == 6);
	assert(strcmp(argv[0], "main") == 0);
	assert(strcmp(argv[1], "-one") == 0);
	assert(strcmp(argv[2], "two") == 0);
	assert(strcmp(argv[3], "-three") == 0);
	assert(strcmp(argv[4], "four") == 0);
	assert(strcmp(argv[5], "-five") == 0);

	// Test 2:
	test02();

    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "++++ passed all tests" << endl;

    return 0;
}
