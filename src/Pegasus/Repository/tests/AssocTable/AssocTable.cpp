//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Repository/AssocClassTable.h>
#include <Pegasus/Repository/AssocInstTable.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
    //
    // create class association
    //
    AssocClassTable::append(
        "./associations.tbl",
	"Lineage",
	"Person",
	"parent",
	"Person",
	"child");

    //
    // delete class association 
    //
    AssocClassTable::deleteAssociation(
        "./associations.tbl",
        "Lineage");

    //
    // create instance association 
    //
    AssocInstTable::append(
        "./associations.tbl",
        "A.left=\"x.key=\\\"one\\\"\",right=\"y.key=\\\"two\\\"\"",
        "A",
        "X.key=\"one\"",
        "X",
        "left",
        "Y",
        "right",
        "Y.key=\"two\"");

    //
    // delete instance association 
    //
    AssocInstTable::deleteAssociation(
        "./associations.tbl",
        "A.left=\"x.key=\\\"one\\\"\",right=\"y.key=\\\"two\\\"\"");

    cout << "+++++ passed all tests" << endl;

    return 0;
}
