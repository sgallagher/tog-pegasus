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
// $Log: LoadProcessClass.cpp,v $
// Revision 1.4  2001/02/16 13:23:45  karl
// Add providers Directory
//
// Revision 1.3  2001/02/16 02:06:09  mike
// Renamed many classes and headers.
//
// Revision 1.2  2001/01/29 02:58:31  mike
// fixed makefile rule
//
// Revision 1.1  2001/01/29 02:18:56  mike
// new files
//
// Revision 1.1  2001/01/28 23:38:46  mike
// new files
//
// Revision 1.1.1.1  2001/01/14 19:54:05  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Server/ProviderTable.h>
#include <Pegasus/Repository/Repository.h>

using namespace Pegasus;
using namespace std;
/* This program is a temporary solution to installing classes into
the repository until the compiler is installed.

The current version creates
1. The process class for the Process provider in the root/cimv20 namespace
2. The __Namespace class for the Namespace Provider - in the root namespace
*/

const char NAMESPACE[] = "root/cimv20";
const char ROOTNAMESPACE[] = "root";


int main(int argc, char** argv)
{
    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << " repository-root" << endl;
	exit(1);
    }

    try
    {
	Repository r(argv[1]);

	try
	{
	    r.deleteClass(NAMESPACE, "Process");
	}
	catch (Exception&)
	{
	    // Ignore not such class error:
	}

	CIMClass c("Process");

	c.addQualifier(CIMQualifier("provider", "MyProvider"));

	c.addProperty(CIMProperty("pid", Uint32(0))
	    .addQualifier(CIMQualifier("key", true)));

	c.addProperty(CIMProperty("name", Uint32(0)));
	c.addProperty(CIMProperty("age", Uint32(0)));

	r.createClass(NAMESPACE, c);

	cout << "Creating Process class with MyProvider Qualifier" <<endl;

       // delete the __Namespace class if it exists
       try
       {
	   r.deleteClass(NAMESPACE, "__Namespace");
       }
       catch (Exception&)
       {
	   // Ignore no such class error
       }

       // create the __Namespace class
       CIMClass cl("__Namespace");
       cl.addQualifier(Qualifier("provider", "__NamespaceProvider"));
       cl.addProperty(Property("name", String())
	   .addQualifier(Qualifier("key", true)));
       r.createClass(NAMESPACE, cl);
       cout << "Creating __Namespace class with __NamespaceProviderQualfier" 
	   << endl;
       }

       // Now create the _-Namespace class in the root directory also
        // delete the __Namespace class if it exists
       try
       {
	   r.deleteClass(ROOTNAMESPACE, "__Namespace");
       }
       catch (Exception&)
       {
	   // Ignore no such class error
       }

       // create the __Namespace class
       CIMClass cl("__Namespace");
       cl.addQualifier(Qualifier("provider", "__NamespaceProvider"));
       cl.addProperty(Property("name", String())
	   .addQualifier(Qualifier("key", true)));
       r.createClass(ROOTNAMESPACE, cl);
       cout << "Creating __Namespace class with __NamespaceProviderQualfier" 
	   << endl;
       }

    }
    catch(Exception& e)
    {
	std::cerr << "Error: " << e.getMessage() << std::endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
