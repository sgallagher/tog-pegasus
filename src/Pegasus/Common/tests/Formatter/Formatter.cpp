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
// $Log: Formatter.cpp,v $
// Revision 1.1  2001/03/22 23:24:23  mike
// Added new formatter class for formatting log entries and other things.
//
// Revision 1.1.1.1  2001/01/14 19:53:45  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <iostream>
#include <Pegasus/Common/Formatter.h>

using namespace Pegasus;
using namespace std;

int main()
{
    String str = 
	Formatter::format("\\$dollar\\$ X=$0, Y=$1, Z=$2, X=$0", 88, "Hello World", 7.5);

    cout << "str[" << str << "]" << endl;

    cout << "+++++ passed all tests" << endl;

    return 0;
}
