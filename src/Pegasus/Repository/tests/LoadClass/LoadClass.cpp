//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: LoadClass.cpp,v $
// Revision 1.1  2001/01/14 19:53:57  mike
// Initial revision
//
//
//END_HISTORY

#include <Pegasus/Repository/Repository.h>

using namespace Pegasus;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << ": xml-file" << endl;
	exit(1);
    }
    Repository repository("../../../../..");

    try
    {
	ConstClassDecl classDecl = repository.getClass(
	    "root/cimv20", argv[1], true);
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
    }
}
