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
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Vijay Eli, vijayeli@in.ibm.com, for #3101
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cstdlib>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/Resolver.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
int main(int argc, char** argv)
{
    // get the output display flag.
    Boolean verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;

    try
    {
	const String NAMESPACE = "/zzz";
	
	// Create and populate a declaration context:

	SimpleDeclContext* context = new SimpleDeclContext;

	context->addQualifierDecl(NAMESPACE, CIMQualifierDecl(
	    CIMName ("abstract"), false, CIMScope::CLASS, 
            CIMFlavor::OVERRIDABLE));

	// Create some classes:

	CIMClass class1(CIMName ("PeskySuperClass"));
	class1.addQualifier(CIMQualifier(CIMName ("abstract"), true));

	CIMClass class2(CIMName ("Class"), CIMName ("PeskySuperClass"));

	Resolver::resolveClass (class1, context, NAMESPACE);
	context->addClass(NAMESPACE, class1);

	Resolver::resolveClass (class2, context, NAMESPACE);
	context->addClass(NAMESPACE, class2);

	// class1.print();
	// class2.print();

        // Create some methods:
        CIMMethod meth1(CIMName ("getHostName"), CIMTYPE_STRING);
        CIMConstMethod meth2(CIMName ("test"), CIMTYPE_STRING);
        Resolver::resolveMethod ( meth1, context, NAMESPACE, meth2);

	delete context;
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
