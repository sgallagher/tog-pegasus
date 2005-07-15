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
// Author: Jim Wunderlich (Jim_Wunderlich@prodigy.net)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Common/CIMPropertyList.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
static char * verbose;


/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

String repositoryRoot;
CIMRepository *r;
const CIMNamespaceName NS = CIMNamespaceName ("TestCreateClass");
CIMQualifier d(CIMName("description"), String("*REMOVED*"));

const char *ProgName;

void TestOpenRepo(CIMRepository_Mode mode)
{
  if (verbose) cout << ProgName << "-TestOpenRepo()" << endl;
  // -- Create repository and "xyz" namespace:

    r = new CIMRepository(repositoryRoot, mode);


}


void TestCloseRepo()
{
  if (verbose) cout << ProgName << "-TestCloseRepo()" << endl;
  // -- close  repository 

  delete  r;

}

void TestInitRepo()
{
  if (verbose) cout << ProgName << "-TestInitRepo()" << endl;

    try
    {
	r->createNameSpace(NS);
    }
    catch (AlreadyExistsException&)
    {
	// Ignore this!
    }

    // -- Declare the key qualifier:

    r->setQualifier(NS, CIMQualifierDecl(CIMName ("key"),true,CIMScope::PROPERTY));
    r->setQualifier(NS, CIMQualifierDecl(CIMName ("description"),String(),(CIMScope::PROPERTY + CIMScope::CLASS)));
    r->setQualifier(NS, CIMQualifierDecl(CIMName ("junk"),String(),(CIMScope::PROPERTY + CIMScope::CLASS)));



}



void TestCreateClass()
{

  if (verbose) cout << ProgName << "-TestCreateClass()" << endl;
    CIMClass c1(CIMName ("SuperClass"));
    c1.addQualifier(d);
    c1.addProperty(
		   CIMProperty(CIMName ("key"), Uint32(0))
		   .addQualifier(CIMQualifier(CIMName ("key"), true)));

    c1.addProperty(CIMProperty(CIMName ("ratio"), Real32(1.5)));
    c1.addProperty(CIMProperty(CIMName ("message"), String("Hello World")));

    // -- Create the class (get it back and compare):
    r->createClass(NS, c1);
    CIMConstClass cc1;
    cc1 = r->getClass(NS, CIMName ("SuperClass"),true,true, true);
    assert(c1.identical(cc1));
    assert(cc1.identical(c1));

    // -- Now create a sub class (get it back and compare):
	// c22 has one additional property than c1 (junk)

    CIMClass c2(CIMName ("SubClass"), CIMName ("SuperClass"));
	// Add new qualifier that will be local
	CIMQualifier j(CIMName("junk"), String("TestQualifier"));
	c2.addQualifier(j);

    c2.addProperty(CIMProperty(CIMName ("junk"), Real32(66.66)));
    r->createClass(NS, c2);
    CIMConstClass cc2;
    cc2 = r->getClass(NS, CIMName ("SubClass"), false, true, true);
	//XmlWriter::printClassElement(c2);
	//XmlWriter::printClassElement(cc2);
    
	assert(c2.identical(cc2));
    assert(cc2.identical(c2));

    // -- Modify "SubClass" (add new property)

    c2.addProperty(CIMProperty(CIMName ("newProperty"), Uint32(888)));
    r->modifyClass(NS, c2);
    cc2 = r->getClass(NS, CIMName ("SubClass"), false, true, true);
    assert(c2.identical(cc2));
    assert(cc2.identical(c2));
    // should test for this new property on "SubClass" also.

}

void TestEnumerateClass()
{
  if (verbose) cout << ProgName << "-TestEnumerateClass()" << endl;
    // -- Enumerate the class names: expect "SuperClass", "SubClass"

    Array<CIMName> classNames = r->enumerateClassNames(NS, CIMName (), true);
	if (verbose)
	{
	  cout << ProgName << "-Enumerated class names: ";
		for (Uint32 i = 0 ; i < classNames.size(); i++)
		{
		  cout << classNames[i].getString() << ", ";
		}
	  cout << endl;
	}

    BubbleSort(classNames);
    assert(classNames.size() == 2);
    assert(classNames[1] == CIMName ("SuperClass"));
    assert(classNames[0] == CIMName ("SubClass"));

}


void TestCreateInstance1()
{
  if (verbose) cout << ProgName << "-TestCreateInstance1()" << endl;
    // -- Create an instance of each class:

    CIMInstance inst0(CIMName ("SuperClass"));
    inst0.addProperty(CIMProperty(CIMName ("key"), Uint32(1)));
    r->createInstance(NS, inst0);

    CIMInstance inst1(CIMName ("SubClass"));
    inst1.addProperty(CIMProperty(CIMName ("key"), Uint32(2)));
    r->createInstance(NS, inst1);
}

void TestCreateInstance2()
{
  if (verbose) cout << ProgName << "-TestCreateInstance2()" << endl;
    // -- Create an instance of each class:

    CIMInstance inst0(CIMName ("SuperClass"));
    inst0.addProperty(CIMProperty(CIMName ("key"), Uint32(11)));
    r->createInstance(NS, inst0);

    CIMInstance inst1(CIMName ("SubClass"));
    inst1.addProperty(CIMProperty(CIMName ("key"), Uint32(22)));
    r->createInstance(NS, inst1);
}



void TestCreateInstance3()
{
  if (verbose) cout << ProgName << "-TestCreateInstance3()" << endl;
    // -- Create an instance of each class:

    CIMInstance inst0(CIMName ("SuperClass"));
    inst0.addProperty(CIMProperty(CIMName ("key"), Uint32(111)));
    r->createInstance(NS, inst0);

    CIMInstance inst1(CIMName ("SubClass"));
    inst1.addProperty(CIMProperty(CIMName ("key"), Uint32(222)));
    r->createInstance(NS, inst1);
}

void TestCreateInstance4()
{
  if (verbose) cout << ProgName << "-TestCreateInstance4()" << endl;
    // -- Create an instance of each class:

    CIMInstance inst0(CIMName ("SuperClass"));
    inst0.addProperty(CIMProperty(CIMName ("key"), Uint32(1111)));
    r->createInstance(NS, inst0);

    CIMInstance inst1(CIMName ("SubClass"));
    inst1.addProperty(CIMProperty(CIMName ("key"), Uint32(2222)));
    r->createInstance(NS, inst1);
}

void TestCreateInstance5()
{
  if (verbose) cout << ProgName << "-TestCreateInstance5()" << endl;
    // -- Create an instance of each class:

    CIMInstance inst0(CIMName ("SuperClass"));
    inst0.addProperty(CIMProperty(CIMName ("key"), Uint32(11111)));
    r->createInstance(NS, inst0);

    CIMInstance inst1(CIMName ("SubClass"));
    inst1.addProperty(CIMProperty(CIMName ("key"), Uint32(22222)));
    r->createInstance(NS, inst1);
}

void TestEnumerateInstance(Uint32 num)
{
  if (verbose) cout << ProgName << "-TestEnumerateInstance()" << endl;

    // -- Enumerate instances names:
    Array<CIMObjectPath> instanceNames = 
	r->enumerateInstanceNames(NS, CIMName ("SuperClass"));

	if (verbose)
	{
	  cout << ProgName << "-Enumerated instance names: ";
		for (Uint32 i = 0 ; i < instanceNames.size(); i++)
		{
		  cout << instanceNames[i].toString() << ", ";
		}
	  cout << endl;
	}

    assert(instanceNames.size() == num*2);

}

void TestEnumerateInstances(Uint32 num)
{
  if (verbose) cout << ProgName << "-TestEnumerateInstances()" << endl;
    // -- Enumerate instances:

    Array<CIMInstance> namedInstances = r->enumerateInstances(NS, 
        CIMName ("SuperClass"),true,false, true, true);

#ifdef NOTDEF
    // defined out becuase it is very verbose
	if (verbose)
	{
	  cout << ProgName << "-Enumerated instance names: ";
		for (Uint32 i = 0 ; i < namedInstances.size(); i++)
		{
		  cout << ((CIMObject )namedInstances[i]).toString() << ", ";
		}
	  cout << endl;
	}
#endif

    assert(namedInstances.size() == num*2);

}


void TestEnumerateInstancesForClass(Uint32 num)
{
  if (verbose) cout << ProgName << "-TestEnumerateInstancesForClass()" << endl;

    // test the enumerateInstancesFor Class function

    Array<CIMInstance>  namedInstances = r->enumerateInstancesForClass(NS, 
        CIMName ("SuperClass"),true,false, true, true);

#ifdef NOTDEF
    // defined out becuase it is very verbose
	if (verbose)
	{
	  cout << ProgName << "-Enumerated instancs for Class-SuperClass: ";
		for (Uint32 i = 0 ; i < namedInstances.size(); i++)
		{
		  cout << ((CIMObject )namedInstances[i]).toString() << ", ";
		}
	  cout << endl;
	}
#endif

    assert(namedInstances.size() == num);

    namedInstances = r->enumerateInstancesForClass(NS, 
        CIMName ("SubClass"),true,false, true, true);

#ifdef NOTDEF
    // defined out becuase it is very verbose
	if (verbose)
	{
	  cout << ProgName << "-Enumerated instancs for Class-SubClass: ";
		for (Uint32 i = 0 ; i < namedInstances.size(); i++)
		{
		  cout << ((CIMObject )namedInstances[i]).toString() << ", ";
		}
	  cout << endl;
	}
#endif

    assert(namedInstances.size() == num);

}

void TestQualifiers()
{
    // -- Create repository and "xyz" namespace:

    const CIMNamespaceName NS = CIMNamespaceName ("TestQualifiers");

    try
    {
	r->createNameSpace(NS);
    }
    catch (AlreadyExistsException&)
    {
	// Ignore this!
    }

    // -- Construct a qualifier declaration:

    CIMQualifierDecl q(CIMName ("abstract"), true, CIMScope::CLASS);
    r->setQualifier(NS, q);

    CIMQualifierDecl qq = r->getQualifier(NS, CIMName ("abstract"));

    assert(qq.identical(q));
    assert(q.identical(qq));

    // -- Delete the qualifier:

    r->deleteQualifier(NS, CIMName ("abstract"));

    // -- Delete the namespace:

    r->deleteNameSpace(NS);
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    ProgName = argv[0];

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
      CIMRepository_Mode mode;
      CIMRepository_Mode modeother;
      int mixed_mode;

      if (!strcmp(argv[1],"XML") )
	{
	  mode.flag = CIMRepository_Mode::NONE;
	  modeother.flag = CIMRepository_Mode::BIN;
	  mixed_mode = 0;
	  if (verbose) cout << ProgName<< ": Single test using XML mode repository" << endl;
	}
      else if (!strcmp(argv[1],"BIN") )
	{
	  mode.flag = CIMRepository_Mode::BIN;
	  modeother.flag = CIMRepository_Mode::NONE;
	  mixed_mode = 0;
	  if (verbose) cout << ProgName<< ": Single test using BIN mode repository" << endl;
	}
      else if (!strcmp(argv[1],"XMLMIX") )
	{
	  mode.flag = CIMRepository_Mode::NONE;
	  modeother.flag = CIMRepository_Mode::BIN;
	  mixed_mode = 1;
	  if (verbose) cout << ProgName<< ": Mixed test using XML mode repository first" << endl;
	}
      else if (!strcmp(argv[1],"BINMIX") )
	{
	  mode.flag = CIMRepository_Mode::BIN;
	  modeother.flag = CIMRepository_Mode::NONE;
	  mixed_mode = 1;
	  if (verbose) cout << ProgName<< ": Mixed test using BIN mode repository first" << endl;
	}
      else
	{
	  cout << ProgName<< ": invalid argument: " << argv[1] << endl;
	  return 0;
	}

      TestOpenRepo(mode);
      TestInitRepo();
      TestCreateClass();
      TestEnumerateClass();
      TestCreateInstance1();
      if (mixed_mode) 
	{
	  TestCloseRepo();
	  TestOpenRepo(modeother);
	}

      TestCreateInstance2();

      if (mixed_mode) 
	{
	  TestCloseRepo();
	  TestOpenRepo(mode);
	}

      TestCreateInstance3();

      if (mixed_mode) 
	{
	  TestCloseRepo();
	  TestOpenRepo(modeother);
	}
      TestCreateInstance4();

      if (mixed_mode) 
	{
	  TestCloseRepo();
	  TestOpenRepo(mode);
	}

      TestCreateInstance5();
      TestEnumerateInstance(5);
      TestEnumerateInstances(5);      
      TestEnumerateInstancesForClass(5);      
      TestQualifiers();

    }
    catch (const Exception& e)
    {
	cout << argv[0] << " " << argv[1] << " " << e.getMessage() << endl;
	exit(1);
    }

    cout << ProgName << " " << argv[1] << " +++++ passed all tests" << endl;

    return 0;
}
