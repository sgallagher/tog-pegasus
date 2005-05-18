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
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const char* tmpDir;
static char * verbose;
String repositoryRoot;

void test(CIMRepository_Mode mode)
{

  CIMRepository r (repositoryRoot, mode);

    // Create a namespace:

    const String NAMESPACE = "/zzz";
    const String ABSTRACT = "abstract";
    r.createNameSpace(NAMESPACE);

    // Create a qualifier declaration:

    CIMQualifierDecl q1(ABSTRACT, Boolean(true), CIMScope::CLASS);
    r.setQualifier(NAMESPACE, q1);

    // Get it back and check to see if it is identical:

    CIMConstQualifierDecl q2 = r.getQualifier(NAMESPACE, ABSTRACT);

    assert(q1.identical(q2));

    // Remove it now:

    r.deleteQualifier(NAMESPACE, ABSTRACT);

    // Try to get it again (this should fail with a not-found error):

    try
    {
	q2 = r.getQualifier(NAMESPACE, ABSTRACT);
    }
    catch (CIMException& e)
    {
	assert(e.getCode() == CIM_ERR_NOT_FOUND);
    }

    // Create two qualifiers:

    CIMQualifierDecl q3(CIMName ("q3"), Uint32(66), CIMScope::CLASS);
    CIMQualifierDecl q4(CIMName ("q4"), String("Hello World"), CIMScope::CLASS);

    r.setQualifier(NAMESPACE, q3);
    r.setQualifier(NAMESPACE, q4);

    // Enumerate the qualifier names:

    Array<CIMQualifierDecl> qualifiers = r.enumerateQualifiers(NAMESPACE);
    assert(qualifiers.size() == 2);

    for (Uint32 i = 0, n = qualifiers.size(); i < n; i++)
    {
	// qualifiers[i].print();
	assert(qualifiers[i].identical(q3) || qualifiers[i].identical(q4));
    }
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    tmpDir = getenv ("PEGASUS_TMP");
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
      CIMRepository_Mode mode;
      if (!strcmp(argv[1],"XML") )
	{
	  mode.flag = CIMRepository_Mode::NONE;
	  if (verbose) cout << argv[0]<< ": using XML mode repository" << endl;
	}
      else if (!strcmp(argv[1],"BIN") )
	{
	  mode.flag = CIMRepository_Mode::BIN;
	  if (verbose) cout << argv[0]<< ": using BIN mode repository" << endl;
	}
      else
	{
	  cout << argv[0] << ": invalid argument: " << argv[1] << endl;
	  return 0;
	}

      test(mode);
    }
    catch (Exception& e)
    {
	cout << argv[0] << " " << argv[1] << " " << e.getMessage() << endl;
	exit(1);
    }

    cout << argv[0] << " " << argv[1] << " +++++ passed all tests" << endl;

    return 0;
}
