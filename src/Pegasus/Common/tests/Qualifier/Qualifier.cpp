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
// Modified By:	Karl Schopmeyer (k.schopemyer@opengroup.org)
//				Mar 2002 - Add more tests for flavors
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/* This program tests the CIMQualifier class and the CIMConstQualifier class
 including the functions in the classes:
 
 It creates qualifiers, tests the scope, value and flavor characteristics.
 ATTN: P3 - KS March 2002 Add more tests for scope, etc.
*/
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
		XmlWriter::printQualifierElement(q1);
		XmlWriter::printQualifierElement(q2);
		XmlWriter::printQualifierElement(q3);
		XmlWriter::printQualifierElement(q4);
	}

	// Note effective march 27 2002, Qualifier no longer get the default flavor from
	// The definition.  Now the defaults come from the declaraction as part of the
	// resolve.  As created, qualifiers have exactly the flavor with which they are
	// defined.	They have no default flavors when there is no explicit definition.
	assert(q4.identical(q1));
	assert(q1.isFlavor(CIMFlavor::TOINSTANCE));
	//assert(!q1.isFlavor(CIMFlavor::TOSUBCLASS));
	//assert(!q1.isFlavor(CIMFlavor::OVERRIDABLE));

	assert(q1.isFlavor(CIMFlavor::TOINSTANCE));
	assert(!q1.isFlavor(CIMFlavor::TOSUBCLASS));
	assert(!q1.isFlavor(CIMFlavor::OVERRIDABLE));

	assert(!q2.isFlavor(CIMFlavor::TOINSTANCE));

	// Test to be sure the defaults are set correctly
	assert(!q2.isFlavor(CIMFlavor::TOSUBCLASS));
	assert(!q2.isFlavor(CIMFlavor::OVERRIDABLE));
	assert(!q2.isFlavor(CIMFlavor::TRANSLATABLE));
	assert(!q2.isFlavor(CIMFlavor::ENABLEOVERRIDE));
	assert(!q2.isFlavor(CIMFlavor::DISABLEOVERRIDE));


	q2.unsetFlavor(CIMFlavor::ALL);
	assert(!q2.isFlavor(CIMFlavor::TOSUBCLASS));
	assert(!q2.isFlavor(CIMFlavor::TOINSTANCE));
	assert(!q2.isFlavor(CIMFlavor::OVERRIDABLE));
	assert(!q2.isFlavor(CIMFlavor::TRANSLATABLE));
	assert(!q2.isFlavor(CIMFlavor::ENABLEOVERRIDE));
	assert(!q2.isFlavor(CIMFlavor::DISABLEOVERRIDE));
	assert(!q2.isFlavor(CIMFlavor::RESTRICTED));

	q2.setFlavor(CIMFlavor::TOSUBCLASS);
	assert(q2.isFlavor(CIMFlavor::TOSUBCLASS));
	assert(!q2.isFlavor(CIMFlavor::OVERRIDABLE));

	q2.unsetFlavor(CIMFlavor::TOSUBCLASS);
	assert(!q2.isFlavor(CIMFlavor::TOSUBCLASS));
	assert(!q2.isFlavor(CIMFlavor::OVERRIDABLE));

	q2.resolveFlavor(CIMFlavor::OVERRIDABLE, false);
	assert(q2.isFlavor(CIMFlavor::OVERRIDABLE));

	q2.setFlavor(CIMFlavor::ALL);
	assert(q2.isFlavor(CIMFlavor::TOSUBCLASS));
	assert(!q2.isFlavor(CIMFlavor::TOINSTANCE));
	assert(q2.isFlavor(CIMFlavor::OVERRIDABLE));
	assert(q2.isFlavor(CIMFlavor::TRANSLATABLE));
	assert(q2.isFlavor(CIMFlavor::ENABLEOVERRIDE));
	assert(q2.isFlavor(CIMFlavor::DISABLEOVERRIDE));
	assert(q2.isFlavor(CIMFlavor::RESTRICTED));


	// ATTN: KS P1 24 March 2002Add test for resloveFlavor here
	q2.unsetFlavor(CIMFlavor::ALL);

	q2.setFlavor( CIMFlavor::TOSUBCLASS	+ CIMFlavor::ENABLEOVERRIDE);

	q2.resolveFlavor (CIMFlavor::DISABLEOVERRIDE | CIMFlavor::RESTRICTED, false);
	assert( q2.isFlavor(CIMFlavor::DISABLEOVERRIDE));
	assert(!q2.isFlavor(CIMFlavor::ENABLEOVERRIDE));
	assert(!q2.isFlavor(CIMFlavor::TOSUBCLASS));
	assert(!q2.isFlavor(CIMFlavor::TOINSTANCE));

	CIMQualifier qual1("qual1", "This is a test");

	CIMQualifier qual3("qual3", "This is a test");
	assert(!qual1.identical(qual3));

	if (verbose)
	{
		XmlWriter::printQualifierElement(q4);
	}

    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
