//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMFlavor.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;


int main(int argc, char** argv)
{
    // get the output display flag.
    Boolean verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;

    try
    {
	CIMQualifier q1("Description", "Hello", CIMFlavor::TOINSTANCE);
	// This one sets the defaults overridable and tosubclass
	CIMQualifier q2("Abstract", true);
	CIMConstQualifier q3 = q1;
	CIMConstQualifier q4;
	q4 = q3;

	if (verbose)
	{
		q1.print();
		q2.print();
		q3.print();
		q4.print();
	}

	assert(q4.identical(q1));
	assert(q1.isFlavorToInstance());
	assert(!q1.isFlavorToSubclass());
	assert(!q1.isFlavorOverridable());

	assert(q1.isFlavorToInstance());
	assert(!q1.isFlavorToSubclass());
	assert(!q1.isFlavorOverridable());

	assert(!q2.isFlavor(CIMFlavor::TOINSTANCE));

	// Test to be sure the defaults are set correctly
	assert(q2.isFlavor(CIMFlavor::TOSUBCLASS));
	assert(q2.isFlavor(CIMFlavor::OVERRIDABLE));
	assert(q2.isFlavorToSubclass());
	assert(q2.isFlavorOverridable());

	assert(q3.isFlavorToInstance());
	assert(!q3.isFlavorToSubclass());
	assert(!q3.isFlavorOverridable());

	Uint32 f = q3.getFlavor();


	CIMQualifier qual1("qual1", "This is a test");

	CIMQualifier qual3("qual3", "This is a test");
	assert(!qual1.identical(qual3));

	if (verbose)
	{
		q4.print();
	}

    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
