//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Logger.cpp,v $
// Revision 1.2  2001/03/23 01:00:46  mike
// More logging capabilities.
//
// Revision 1.1  2001/03/23 00:56:33  mike
// New regression test for Logger
//
//
//END_HISTORY

#include <cassert>
#include <iostream>
#include <Pegasus/Common/Logger.h>

using namespace Pegasus;
using namespace std;

// ATTN-B: Complete this test by reopening the log and making sure it
// contains what we expect.

int main()
{
    Logger::setHomeDirectory("./logs");

    Logger::put(
	Logger::TRACE_LOG,
	"LoggerTest",
	Logger::WARNING,
	"X=$0, Y=$1, Z=$2", 
	88, 
	"Hello World", 
	7.5);

    cout << "+++++ passed all tests" << endl;

    return 0;
}
