//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMProperty.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main()
{
    CIMProperty pnull;

    assert(!pnull);

    CIMProperty p1("message", "Hi There");
    p1.addQualifier(CIMQualifier("Key", true));
    p1.addQualifier(CIMQualifier("stuff", true));
    p1.addQualifier(CIMQualifier("stuff2", true));
    p1.addQualifier(CIMQualifier("Description", "Blah Blah"));
    CIMConstProperty p2 = p1;


    // Test name
        String name;
        name = p1.getName();
        assert(name == "message");

    // Tests for Qualifiers
	assert(p1.findQualifier("stuff") != PEG_NOT_FOUND);
	assert(p1.findQualifier("stuff2") != PEG_NOT_FOUND);
	assert(p1.findQualifier("stuff21") == PEG_NOT_FOUND);
	assert(p1.findQualifier("stuf") == PEG_NOT_FOUND);
	assert(p1.getQualifierCount() == 4);

	assert(p1.existsQualifier("stuff"));
	assert(p1.existsQualifier("stuff2"));

	assert(!p1.existsQualifier("stuff21"));
	assert(!p1.existsQualifier("stuf"));

	Uint32 posQualifier;
	posQualifier = p1.findQualifier("stuff");
	assert(posQualifier >= 0);
	assert(posQualifier < p1.getQualifierCount());

	p1.removeQualifier(posQualifier);
	assert(p1.getQualifierCount() == 3);
	assert(!p1.existsQualifier("stuff"));
	assert(p1.existsQualifier("stuff2"));

   // Tests for value insertion.
   {
           CIMProperty p1("p1", "Hi There");
           // test for CIMValue and type
           CIMProperty p2("p2", Uint32(999));
           // test for CIMValue and type

	   //Test getName and setName
	   assert(p1.getName() == "p1");
	   p1.setName("px");
	   assert(p1.getName() == "px");

	   // Test setValue and getValue


   }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
