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
// Modified By: Jenny Yu (jenny_yu@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Repository/AssocClassTable.h>
#include <Pegasus/Repository/AssocInstTable.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
static char * verbose;

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    String assocTablePath;
    const char* tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        assocTablePath = ".";
    }
    else
    {
        assocTablePath = tmpDir;
    }
    assocTablePath.append("/associations.tbl");

    //
    // create class association
    //
    AssocClassTable::append(
        assocTablePath,
	CIMName ("Lineage"),
	CIMName ("Person"),
	CIMName ("parent"),
	CIMName ("Person"),
	CIMName ("child"));

    //
    // delete class association 
    //
    AssocClassTable::deleteAssociation(
        assocTablePath,
        CIMName ("Lineage"));

    //
    // create instance association 
    //
    AssocInstTable::append(
        assocTablePath,
        "A.left=\"x.key=\\\"one\\\"\",right=\"y.key=\\\"two\\\"\"",
        CIMName ("A"),
        "X.key=\"one\"",
        CIMName ("X"),
        CIMName ("left"),
        "Y.key=\"two\"",
        CIMName ("Y"),
        CIMName ("right"));

    //
    // delete instance association 
    //
    AssocInstTable::deleteAssociation(
        assocTablePath,
        CIMObjectPath 
            ("A.left=\"x.key=\\\"one\\\"\",right=\"y.key=\\\"two\\\"\""));

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
