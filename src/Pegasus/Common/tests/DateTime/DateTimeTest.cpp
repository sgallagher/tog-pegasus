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
// $Log: DateTimeTest.cpp,v $
// Revision 1.2  2001/02/16 02:06:07  mike
// Renamed many classes and headers.
//
// Revision 1.1.1.1  2001/01/14 19:53:45  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/Exception.h>

using namespace Pegasus;
using namespace std;

int main()
{
    CIMDateTime dt;
    dt.set("19991224120000.000000+360");

    dt.clear();
    assert(dt.isNull());

    {
	Boolean bad = false;

	try
	{
	    dt.set("too short");
	}
	catch (BadDateTimeFormat&)
	{
	    bad = true;
	}

	assert(bad);
    }

    {
	Boolean bad = false;

	try
	{
	    dt.set("too short");
	    dt.set("19990132120000.000000+360");
	}
	catch (BadDateTimeFormat&)
	{
	    bad = true;
	}

	assert(bad);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
