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
// Modified By: Sushma Fernandes (sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMMethod.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

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

	assert(m1.findQualifier("stuff") != PEG_NOT_FOUND);
	assert(m1.findQualifier("stuff2") != PEG_NOT_FOUND);

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

        // Tests for Parameters
        assert(m1.findParameter("ipaddress") != PEG_NOT_FOUND);
        assert(m1.findParameter("noparam")  == PEG_NOT_FOUND);
        assert(m1.getParameterCount()  == 1);
        CIMParameter cp = m1.getParameter(m1.findParameter("ipaddress"));
        assert(cp.getName() == "ipaddress");
 
        CIMMethod m2("test", CIMType::STRING);
        m2.setName("getVersion");
        assert(m2.getName() == "getVersion");

        m2.setType(CIMType::STRING);
        assert(m2.getType() == CIMType::STRING);

        m2.setClassOrigin("test");
        assert(m2.getClassOrigin() == "test");

        m2.setPropagated(true);
        assert(m2.getPropagated() == true);

        const CIMMethod cm1(m1);
	assert(!cm1.existsQualifier("stuff21"));
	assert(!cm1.existsQualifier("stuf"));
        assert((cm1.getParameterCount() != 3));
        assert(cm1.findParameter("ipaddress") != PEG_NOT_FOUND);
        assert(cm1.findQualifier("stuff") == PEG_NOT_FOUND);
        
        CIMQualifier q = m1.getQualifier(posQualifier);
        CIMConstParameter ccp = cm1.getParameter(cm1.findParameter("ipaddress"));
        assert(cm1.getName() == "getHostName");
        assert(cm1.getType() == CIMType::STRING);
        assert(!(cm1.getClassOrigin() == "test"));
        assert(!cm1.getPropagated() == true);
        assert(!m1.identical(m2));

        // throws OutOfBounds
        try
        {
            CIMConstParameter p = cm1.getParameter(cm1.findParameter("ipaddress"));
        }
        catch(OutOfBounds& e)
        {
        }
        // throws OutOfBounds
        try
        {
            CIMConstQualifier q = cm1.getQualifier(cm1.findQualifier("abstract"));
        }
        catch(OutOfBounds& e)
        {
        }

        if (verbose)
        {
            XmlWriter::printMethodElement(m1);
            XmlWriter::printMethodElement(cm1);
        }
        Array<Sint8> out;
        XmlWriter::appendMethodElement(out, cm1);
        cm1.toMof(out);

        Boolean nullMethod = cm1.isNull(); 
	assert(!nullMethod);

        CIMMethod m3 = m2.clone();
        m3 = cm1.clone();
        
        CIMMethod m4;
        CIMMethod m5(m4);

        CIMConstMethod ccm1("getHostName",CIMType::STRING);
        assert(!(ccm1.getParameterCount() == 3));

        assert(ccm1.getName() == "getHostName");
        assert(ccm1.getType() == CIMType::STRING);
        assert(!(ccm1.getClassOrigin() == "test"));
        assert(!ccm1.getPropagated() == true);
        assert(!(ccm1.getParameterCount() == 3));
        assert(ccm1.getQualifierCount() == 0);
        assert(ccm1.findQualifier("Stuff") == PEG_NOT_FOUND);
        assert(ccm1.findParameter("ipaddress") == PEG_NOT_FOUND);

        if (verbose)
        {
            XmlWriter::printMethodElement(m1);
            XmlWriter::printMethodElement(ccm1);
        }

        XmlWriter::appendMethodElement(out, ccm1);
      
        CIMConstMethod ccm2(ccm1);
        CIMConstMethod ccm3;

        ccm3 = ccm1.clone();
        ccm1 = ccm3;
        assert(ccm1.identical(ccm3));
        assert(ccm1.findQualifier("stuff") == PEG_NOT_FOUND);
        assert(ccm1.findParameter("ipaddress") == PEG_NOT_FOUND);
        
        nullMethod = ccm1.isNull();
        assert(!nullMethod);

        // throws OutOfBounds
        try
        {
            //CIMParameter p = m1.getParameter(m1.findParameter("ipaddress"));
            CIMConstParameter p = ccm1.getParameter(0);
        }
        catch(OutOfBounds& e)
        {
        }
        // throws OutOfBounds
        try
        {
            CIMConstQualifier q = ccm1.getQualifier(0);
        }
        catch(OutOfBounds& e)
        {
        }
        cout << "ALL CLEAR" << endl;
    }
    catch(Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
    }

    // Test for add second qualifier with same name.
    // Should do exception

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
