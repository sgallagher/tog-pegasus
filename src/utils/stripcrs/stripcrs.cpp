#include <Pegasus/Common/Config.h>
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <direct.h>
#else
# include <unistd.h>
#endif

using namespace std;

bool RemoveFile(const string& path)
{
#ifdef OS_TYPE_WINDOWS
    return _unlink(path.c_str()) == 0;
#else
    return unlink(path.c_str()) == 0;
#endif
}

bool CopyFile(const string& fromFile, const string& toFile)
{
    // Open input file:

    ifstream is(fromFile.c_str() PEGASUS_IOS_BINARY);

    if (!is)
	return false;

    // Open output file:

    ofstream os(toFile.c_str() PEGASUS_IOS_BINARY);

    if (!os)
	return false;

    char c;

    while (is.get(c))
	os.put(c);

    return true;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << " filename" << endl;
	exit(1);
    }

    ifstream is(argv[1] PEGASUS_IOS_BINARY);

    if (!is)
    {
	cerr << argv[0] << ": failed to open \"" << argv[1] << "\"" << endl;
	exit(1);
    }

    const char TEMP_FILE_NAME[] = "stripcrs.tmp";

    ofstream os(TEMP_FILE_NAME PEGASUS_IOS_BINARY);

    if (!os)
    {
	cerr << argv[0] << ": failed to open \"";
	cerr << TEMP_FILE_NAME << "\"" << endl;
	exit(1);
    }

    char c;

    while (is.get(c))
    {
	if (c != '\r')
	    os.put(c);
    }

    is.close();
    os.close();

    if (!CopyFile(TEMP_FILE_NAME, argv[1]))
    {
	cerr << argv[0] << ": failed to copy back file" << endl;
	exit(1);
    }

    RemoveFile(TEMP_FILE_NAME);

    return 0;
}
