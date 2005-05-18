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
// Modified By:  Jim Wunderlich (Jim_Wunderlich@prodigy.net)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/XmlStreamer.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
Boolean verbose;

String repositoryRoot;
bool trace = false;
size_t failures = 0;

void PutClass(const char* filename, const CIMClass& cimClass)
{
  //    Array<Sint8> out;
    Array<char> out;
    XmlStreamer stream;
    stream.encode(out, cimClass);
    out.append('\0');

    FILE* fp = fopen(filename, "wb");
    assert(fp != NULL);
    fputs(out.getData(), fp);
    fclose(fp);
}

void CompareClasses(
    CIMRepository& r1,
    CIMRepository& r2,
    const CIMNamespaceName& namespaceName)
{
    Array<CIMName> classNames1 = r1.enumerateClassNames(namespaceName);
    Array<CIMName> classNames2 = r2.enumerateClassNames(namespaceName);
    BubbleSort(classNames1);
    BubbleSort(classNames2);


    assert(classNames1 == classNames2);

    for (size_t i = 0; i < classNames1.size(); i++)
    {
	CIMClass class1 = r1.getClass(namespaceName, classNames1[i]);
	CIMClass class2 = r2.getClass(namespaceName, classNames2[i]);

	if (verbose)
	{
	    cout << "testing class " << namespaceName.getString() << "/";
	    cout << classNames1[i].getString() << "..." << endl;
	}

	if (!class1.identical(class2))
	{
	    PutClass("file1", class1);
	    PutClass("file2", class2);

	    cout << "========================================================="; 
	    cout << "========================================================="; 
	    cout << endl;
	    cout << "ERROR: not identical! - ";


	    cout << "ERROR FOUND testing class: " << namespaceName.getString();
	    cout << "/";
	    cout << classNames1[i].getString();

	    cout << " .... differences follow:" << endl << endl;

	    system("diff file1 file2");

	    if (verbose) 
	      {
		XmlWriter::printClassElement(class1, cout);
		XmlWriter::printClassElement(class2, cout);
	      }
	    failures++;
	}
    }
}

void CompareInstances(
    CIMRepository& r1,
    CIMRepository& r2,
    const CIMNamespaceName& namespaceName)
{
    Array<CIMName> classNames1 = r1.enumerateClassNames(namespaceName);
    Array<CIMName> classNames2 = r2.enumerateClassNames(namespaceName);
    BubbleSort(classNames1);
    BubbleSort(classNames2);
    assert(classNames1 == classNames2);

    for (size_t i = 0; i < classNames1.size(); i++)
    {
	Array<CIMObjectPath> objectPaths1 = r1.enumerateInstanceNames(
	    namespaceName, classNames1[i]);
	Array<CIMObjectPath> objectPaths2 = r2.enumerateInstanceNames(
	    namespaceName, classNames2[i]);
	// BubbleSort(objectPaths1);
	// BubbleSort(objectPaths2);
	assert(objectPaths1 == objectPaths2);

	for (size_t i = 0; i < objectPaths2.size(); i++)
	{
	    CIMInstance inst1 = r1.getInstance(namespaceName, objectPaths1[i]);
	    CIMInstance inst2 = r2.getInstance(namespaceName, objectPaths2[i]);

	    if (verbose)
	    {
		cout << "testing instance " << namespaceName.getString() << "/";
		cout << objectPaths1[i].toString() << "..." << endl;
	    }

	    assert(inst1.identical(inst2));
	}
    }
}

void CompareQualifiers(
    CIMRepository& r1,
    CIMRepository& r2,
    const CIMNamespaceName& namespaceName)
{
    Array<CIMQualifierDecl> quals1 = r1.enumerateQualifiers(namespaceName);
    Array<CIMQualifierDecl> quals2 = r2.enumerateQualifiers(namespaceName);
    assert(quals1.size() == quals2.size());

    for (size_t i = 0; i < quals2.size(); i++)
    {
	if (verbose)
	{
	    cout << "testing qualifier " << namespaceName.getString() << "/";
	    cout << quals2[i].getName().getString() << "..." << endl;
	}

	assert(quals1[i].identical(quals2[i]));
    }
}

void Compare(
    const String& repositoryRoot1,
    const String& repositoryRoot2)
{
    //
    // Create repositories:
    //

    CIMRepository r1(repositoryRoot1);
    CIMRepository r2(repositoryRoot2);

    //
    // Compare the namespaces.
    //

    Array<CIMNamespaceName> nameSpaces1 = r1.enumerateNameSpaces();
    Array<CIMNamespaceName> nameSpaces2 = r2.enumerateNameSpaces();
    BubbleSort(nameSpaces1);
    BubbleSort(nameSpaces2);
    assert(nameSpaces1 == nameSpaces2);

    //
    // Compare classes in each namespace:
    //

    for (size_t i = 0; i < nameSpaces1.size(); i++)
    {
	CompareQualifiers(r1, r2, nameSpaces1[i]);
	CompareClasses(r1, r2, nameSpaces1[i]);
	CompareInstances(r1, r2, nameSpaces1[i]);
    }
}

int main(int argc, char** argv)
{

    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;
    if (verbose) cout << argv[0] << ":" << endl;

    //
    // Usage:
    //


    if (argc != 3)
    {
	fprintf(stderr, 
	    "Usage: %s repository-root-1 repository-root-2\n", __FILE__);
	exit(1);
    }

    //
    // Extract repository roots:
    //

    try 
    {
	Compare(argv[1], argv[2]);
    }
    catch (Exception& e)
    {
        cout << argv[0] << " " << e.getMessage() << endl;
	exit(1);
    }

    if (!failures)
      cout << argv[0] << ": +++++ passed all tests" << endl;
    else
      cerr << argv[0] << ": +++++ There were " << failures << " failures" << endl;

    return 0;
}
