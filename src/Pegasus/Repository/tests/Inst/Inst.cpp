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

#include <fstream>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Repository/InstanceIndexFile.h>

using namespace Pegasus;
using namespace std;

int main(int argc, char** argv)
{
    try
    {
	const char PATH[] = "X.idx";
	const char INSTANCE_NAME[] = "X.key=1666";

	Uint32 i;
	Uint32 j;
	
	assert(InstanceIndexFile::insert(PATH, INSTANCE_NAME, i));
	assert(InstanceIndexFile::lookup(PATH, INSTANCE_NAME, j));
	assert(i == j);

	assert(InstanceIndexFile::remove(PATH, INSTANCE_NAME));
    }

    catch (Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
