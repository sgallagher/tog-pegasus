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
#include <Pegasus/Common/CIMMethod.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main()
{
    try
    {
	CIMMethod m1("getHostName", CIMType::STRING);
	m1.addQualifier(CIMQualifier("stuff", true));
	m1.addQualifier(CIMQualifier("stuff2", true));
	m1.addParameter(CIMParameter("ipaddress", CIMType::STRING));


	// Tests for Qualifiers
	assert(m1.findQualifier("stuff") != PEG_NOT_FOUND);
	assert(m1.findQualifier("stuff2") != PEG_NOT_FOUND);
	assert(m1.findQualifier("stuff21") == PEG_NOT_FOUND);
	assert(m1.findQualifier("stuf") == PEG_NOT_FOUND);
	assert(m1.getQualifierCount() == 2);

	assert(m1.existsQualifier("stuff"));
	assert(m1.existsQualifier("stuff2"));

	assert(!m1.existsQualifier("stuff21"));
	assert(!m1.existsQualifier("stuf"));

	Uint32 posQualifier;
	posQualifier = m1.findQualifier("stuff");
	assert(posQualifier != PEGASUS_NOT_FOUND);
	assert(posQualifier < m1.getQualifierCount());

	m1.removeQualifier(posQualifier);
	assert(m1.getQualifierCount() == 1);
	assert(!m1.existsQualifier("stuff"));
	assert(m1.existsQualifier("stuff2"));


    }
    catch(Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
    }

    // Test for add second qualifier with same name.
    // Should do exception

    cout << "+++++ passed all tests" << endl;

    return 0;
}
