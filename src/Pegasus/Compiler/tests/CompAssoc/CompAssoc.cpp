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
// Author:
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Common/String.h>
//#include <Pegasus/Compiler/cimmofParser.h>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void TestAssociations(CIMRepository& r)
{
    String nameSpace = "root";
    {
	CIMObjectPath instanceName = CIMObjectPath ("X.key=\"John Smith\"");

	Array<CIMObjectPath> names = r.associatorNames(
	    nameSpace,
	    instanceName,
	    CIMName ("A"),
	    CIMName ("Y"),
	    "left",
	    "right");

	assert(names.size() == 1);
	Boolean cond = names[0] == CIMObjectPath("Y.key=\"John Jones\"");
	assert(cond);
    }

    {
	CIMObjectPath instanceName = CIMObjectPath ("X.key=\"John Smith\"");

	Array<CIMObject> result = r.associators(
	    nameSpace,
	    instanceName,
	    CIMName ("a"),
	    CIMName ("y"),
	    "LEFT",
	    "RIGHT");

	assert(result.size() == 1);

	CIMObjectPath cimReference = result[0].getPath ();
	CIMInstance cimInstance = CIMInstance(result[0]);

	CIMClass tmpClass = r.getClass(nameSpace, cimInstance.getClassName());
	CIMObjectPath tmpInstanceName = cimInstance.buildPath(tmpClass);

	Boolean t = tmpInstanceName == CIMObjectPath("Y.key=\"John Jones\"");
	assert(t);
	// result[0].print();
    }

    {
	CIMObjectPath instanceName = CIMObjectPath ("X.key=\"John Smith\"");

	Array<CIMObjectPath> result = r.referenceNames(
	    nameSpace,
	    instanceName,
	    CIMName ("A"),
	    "left");

	assert(result.size() == 1);

	CIMObjectPath tmp = CIMObjectPath ("A."
	    "left=\"x.key=\\\"John Smith\\\"\","
	    "right=\"y.key=\\\"John Jones\\\"\"");
	
	Boolean cond = (result[0] == tmp);
	assert(cond);
    }

    {
	CIMObjectPath instanceName = CIMObjectPath ("X.key=\"John Smith\"");

	Array<CIMObject> result = r.references(
	    nameSpace,
	    instanceName,
	    CIMName ("A"),
	    "left");

	assert(result.size() == 1);

	CIMClass tmpClass = r.getClass(
	    nameSpace, CIMInstance(result[0]).getClassName());

	CIMObjectPath tmpInstanceName = 
	    CIMInstance(result[0]).buildPath(tmpClass);

	CIMObjectPath tmp = CIMObjectPath ("A."
	    "left=\"x.key=\\\"John Smith\\\"\","
	    "right=\"y.key=\\\"John Jones\\\"\"");
	
	Boolean cond = (tmpInstanceName == tmp);
	assert(cond);
    }

    // Delete all the object we created:
    {
	// First delete the association:

	CIMObjectPath assocInstanceName = CIMObjectPath ("A."
	    "left=\"x.key=\\\"John Smith\\\"\","
	    "right=\"y.key=\\\"John Jones\\\"\"");

	r.deleteInstance(nameSpace, assocInstanceName);
    }
}

int main(int argc, char** argv)
{
    String repositoryRoot;
    const char* tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        repositoryRoot = ".";
    }
    else
    {
        repositoryRoot = tmpDir;
    }
    repositoryRoot.append("/repository");

    try
    {
	CIMRepository r (repositoryRoot);

	// TestAssociations(r);
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
