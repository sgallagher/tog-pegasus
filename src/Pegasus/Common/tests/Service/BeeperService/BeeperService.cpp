//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
#include <fstream>
#include <iostream>
#include <cstdio>
#include <windows.h>
#include <string>
#include <Pegasus/Common/Service.h>

using namespace std;
using namespace Pegasus;

static char _logFileName[1024] = "";

const char SERVICE_NAME[] = "beeper";
const char DISPLAY_NAME[] = "Beeper Service";

class MyServiceHandler : public ServiceHandler
{
public:

    MyServiceHandler()
    {

    }

    virtual int main(int argc, char** argv)
    {
	// Open log file:

	if (*_logFileName == '\0')
	{
	    cerr << argv[0] << ": failed to open log file" << endl;
	    return 1;
	}

	_os.open((char*)_logFileName);

	if (!_os)
	{
	    cerr << argv[0] << ": failed to open " << _logFileName << endl;
	    return 1;
	}

	// Run the service activities:

	_os << "Beeper service starting" << endl;

	for (;;)
	{
	    _os << "Beep!" << endl;
	    Beep(200,200);
	    Sleep(2000);
	}

	return 0;
    }

    virtual void pause()
    {
	_os << "beeper.exe: pause" << endl;
    }

    virtual void resume()
    {
	_os << "beeper.exe: resume" << endl;
    }

    virtual void stop()
    {
	_os << "beeper.exe: stop" << endl;
    }

private:
    ofstream _os;
};

void main(int argc, char** argv)
{
    // Give the log file the same name as the executable:

    {
	strcpy(_logFileName, argv[0]);

	char* dot = strrchr(_logFileName, '.');

	if (dot)
	    strcpy(dot, ".log");
	else
	    strcat(_logFileName, ".log");
    }

    // Look for -term option:

    bool detach = true;

    for (int i = 1; i < argc; i++)
    {
	if (strcmp(argv[i], "-nodetach") == 0)
	    detach = false;
    }

    // Create and run service:

    MyServiceHandler* serviceHandler = new MyServiceHandler();

    Service::run(argc, argv, SERVICE_NAME, serviceHandler, detach);

    // If it drops out, then maybe it wasn't run from the service manager:

    cerr << "This program must be run as a Windows service" << endl;
    exit(1);
}
