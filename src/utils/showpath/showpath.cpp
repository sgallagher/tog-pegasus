#include <Pegasus/Common/Config.h>
#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 1 && argc != 2)
    {
	cerr << "Usgae: " << argv[0] << " patter" << endl;
	exit(1);
    }

    char* path = getenv("PATH");

    if (path)
	path = strdup(path);

    for (char* p = strtok(path, ";"); p; p = strtok(NULL, ";"))
    {
	if (argc == 1 || (argc == 2 && strstr(p, argv[1])))
	    cout << p << endl;
    }

    return 0;
}
