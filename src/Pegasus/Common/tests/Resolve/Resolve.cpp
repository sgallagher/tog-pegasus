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
// Modified By: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//			extend resolve tests for many error cases 22 March 2002
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/*
	This module tests the resolve functions from the class objects.
	Tests defined for valid resolution of classes. This test module sets
	up multiple conditions of classes and subclasses and confirms that
	the classes are resolved correctly for the superclasses and that
	the proper information is propagated to subclasses as part of the
	resolve.
	We have tried to test all of the options of resolution including:
	-resolution of properties and methods.
	-resolution of class qualifiers
	-resolution of property and method qualifiers.
	-options of CIMScope and CIMFlavor
	-resolution of propagation of qualifiers, properties, and methods to
		a subclass.
	-propagation of the values to subclasses.
	
	All tests are confirmed with assertions so that the test should run
	silently.
	NOTE: Problems with the assertions are probably indications of problems
	in the resolution of classes and either the test or the resolution
	needs to be corrected.
	Please, if there is a problem found, do not disable the tests.  We
	prefer if you create a temporary display showing the problem
	and disable the assertion itself so we are warned of the problem during
	testing until the problem is corrected.
*/
#include <cassert>
#include <cstdlib>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define TESTIO
static char* verbose;


/* Macro to Create the equivalent of an assert but without the
   termination.  This can be used as a temporary marker for asserts
   that are not working.  Prints out the error but continues.
   NOTE: This is useful in test programs to keep us aware that we
   have problems without halting the test sequence
*/
#define ASSERTTEMP(COND) \
    do \
    { \
	if (!(COND)) \
	{ \
	    cerr << "TEMP Assert Error TEMP **********"	\
		<<__FILE__ << " " << __LINE__ \
		<< " " << #COND << endl; \
	} \
    } while (0)

/*  This is the original test written as a simple test of resolution.
*/
void test01()
{
    const String NAMESPACE = "/ttt";
	if(verbose)
		cout << "Test01 " << endl;
	SimpleDeclContext* context = new SimpleDeclContext;

    // Not sure about this one. How do I get NULL as CIMValue
    // This generates an empty string, not NULL
    CIMQualifierDecl q1("q1",String(),CIMScope::CLASS);

	// This qualifier is
    CIMQualifierDecl q2("Abstract", Boolean(true), CIMScope::CLASS , CIMFlavor::NONE);

	// flavors for this one should be enableoverride, and tosubclass 
	CIMQualifierDecl key("key",Boolean(true),(CIMScope::PROPERTY + CIMScope::REFERENCE),
	  CIMFlavor::TOSUBCLASS);

	// Flavor is the defaults. overridable and tosubclass.
    CIMValue v1(CIMType::UINT32, false);
    CIMQualifierDecl q3("q3",v1,CIMScope::CLASS);

	// Flavor should be tosubclass and overridable
    CIMQualifierDecl q4("q4",String(),CIMScope::CLASS, 
		CIMFlavor::TOSUBCLASS | CIMFlavor::ENABLEOVERRIDE);

	// Flavor should be tosubclass and overridable
    CIMQualifierDecl q5("q5",String("Declaration"),CIMScope::CLASS, 
		CIMFlavor::TOSUBCLASS | CIMFlavor::ENABLEOVERRIDE);

	if(verbose)
	{
		XmlWriter::printQualifierDeclElement(q1);
		XmlWriter::printQualifierDeclElement(q2);
		XmlWriter::printQualifierDeclElement(q3);
		XmlWriter::printQualifierDeclElement(q4);
		XmlWriter::printQualifierDeclElement(q5);
		XmlWriter::printQualifierDeclElement(key);
	}

    context->addQualifierDecl(NAMESPACE, q1);
    context->addQualifierDecl(NAMESPACE, q2);
    context->addQualifierDecl(NAMESPACE, q3);
    context->addQualifierDecl(NAMESPACE, q4);
    context->addQualifierDecl(NAMESPACE, q5);
    context->addQualifierDecl(NAMESPACE, key);

	CIMProperty keyProperty("keyProperty", Boolean(true));
	keyProperty
		.addQualifier(CIMQualifier("key", Boolean(true)));


    CIMClass class2("SuperClass", "");

	if(verbose)	cout << "Create Class2 " << endl;

	class2
        .addQualifier(CIMQualifier("Abstract", Boolean(true)))
		.addQualifier(CIMQualifier("q1", String("Hello")))
		.addQualifier(CIMQualifier("q4", String("Goodbye")))
		.addQualifier(CIMQualifier("q5", String("Hello")))
		.addProperty(CIMProperty(keyProperty))
	;
    
    context->addClass(NAMESPACE, class2);
    class2.resolve(context, NAMESPACE);
    
	if(verbose)	cout << "Create Class1 " << endl;

    CIMClass class1("SubClass", "SuperClass");

    class1
        .addQualifier(CIMQualifier("Abstract", Boolean(true)))
        .addQualifier(CIMQualifier("q1", String("Hello")))
		.addQualifier(CIMQualifier("q3", Uint32(55)))
		.addQualifier(CIMQualifier("q5", String("Goodbye")))

	// .addProperty(CIMProperty(keyProperty))
	.addProperty(CIMProperty("count", Uint32(77)))
	// .addProperty(CIMProperty("ref1", Reference("MyClass.key1=\"fred\"")))
	.addMethod(CIMMethod("isActive", CIMType::BOOLEAN)
	    .addParameter(CIMParameter("hostname", CIMType::STRING))
	    .addParameter(CIMParameter("port", CIMType::UINT32)));

    if(verbose)
	{
		XmlWriter::printClassElement(class1);
		XmlWriter::printClassElement(class2);
	}
	if(verbose)	cout << "Resolve class 1 " << endl;

    try{
        class1.resolve(context, NAMESPACE);
		if(verbose)
			cout << "Passed basic resolution test" << endl;

        // Add assertions on the resolution.
        // Abstract did not move to subclass
        // 2. et.
    if(verbose)
	{
		cout << "after resolve " << endl;
		XmlWriter::printClassElement(class1);
		XmlWriter::printClassElement(class2);
	}

    }
    catch (Exception& e)
    {
        cerr << "Test01 - Resolution Error " << e.getMessage() << endl;
    }
	// Test results	in superclass
	assert(class2.findQualifier("Abstract") != PEG_NOT_FOUND);

	// assert results in subclass
	assert(class1.findQualifier("Abstract") != PEG_NOT_FOUND);
	assert(class1.findQualifier("q1") != PEG_NOT_FOUND);
	assert(class1.findQualifier("q3") != PEG_NOT_FOUND);
	assert(class1.findQualifier("q4") != PEG_NOT_FOUND);
	assert(class1.findQualifier("q5") != PEG_NOT_FOUND);

    delete context;
}
/* Test for qualifier and properties resolved to subclass from superclass
	This is a major test of resolution of attributes from a superclass to
	a subclass.
    This test confirms that:
	-subclasses have the following moved correctly from superclasses:
		-properties and the propagated flag set.
		-Qualifiers with toSubclass set.
	
	Confirms that the following is not done:
		-qualifiers with !tosubclass are not propagated.
	
	Confirms classOrigin set correctly for all properties and methods.
	Confirms that the propagated flag set correctly on all propagated
	properties and methods.
	
	Tests to be added:
		-Method propagation
		-Array propagation
	
	All tests are confirmed with assertions. Displays are optional except
	for a few cases where the resolve itself is incorrect and we need reminders
	to fix it.
	
*/
void test02()
{
	if(verbose)
		cout << "Test02 - Resolution of properties from super to subclass " << endl;
    
	const String NAMESPACE = "/ttt";
    Boolean resolved = false;
    SimpleDeclContext* context = new SimpleDeclContext;


	  // flavors for this one should be disable override, restricted.
      CIMQualifierDecl abstract("Abstract", Boolean(true), CIMScope::CLASS , 
		  CIMFlavor::RESTRICTED | CIMFlavor::DISABLEOVERRIDE);

      // flavors for this one should be disableoverride, but tosubclass 
	  CIMQualifierDecl key("key",Boolean(true),
		  (CIMScope::PROPERTY + CIMScope::REFERENCE),
        CIMFlavor::TOSUBCLASS);

	  // Flavors are not to subclass and not overridable
	  CIMQualifierDecl notToSubclass("notToSubclass", Boolean(),
		  (CIMScope::PROPERTY + CIMScope::CLASS),
		  CIMFlavor::RESTRICTED | CIMFlavor::DISABLEOVERRIDE);
	  // same qualities as association qualifier. DisableOverride 
	  CIMQualifierDecl association("associat", Boolean(false),
		  (CIMScope::ASSOCIATION + CIMScope::CLASS),
		  CIMFlavor::DISABLEOVERRIDE);

	  // Qualifier with TOSubclass set and a value and not overridable.
	  CIMQualifierDecl toSubclass("toSubclass", String("default"),
		  (CIMScope::PROPERTY + CIMScope::CLASS),
		  CIMFlavor::TOSUBCLASS);

	  // Qualifier with TOSubclass set and a value.
	  CIMQualifierDecl toSubclassOverridable("toSubclassOverriddable", String("default"),
		  (CIMScope::PROPERTY + CIMScope::CLASS),
		  CIMFlavor::TOSUBCLASS + CIMFlavor::OVERRIDABLE);

	  // Qualifier with Null value. Use to demo value Nulls
	  CIMQualifierDecl nullValue("nullValue", String(),
		  (CIMScope::PROPERTY + CIMScope::CLASS),
		  CIMFlavor::TOSUBCLASS);

	  // Flavors for this qualifier should be enableoverride, tosubclass
	  // Value is empty
      CIMQualifierDecl q1("q1",String(),CIMScope::CLASS,
        CIMFlavor::DEFAULTS);

      CIMQualifierDecl q2("q2",String(),CIMScope::CLASS, CIMFlavor::DEFAULTS);

	  // Qualifier q3 Boolean with value zero
	  CIMValue v1(CIMType::UINT32, false);
      CIMQualifierDecl q3("q3",v1,CIMScope::CLASS, CIMFlavor::DEFAULTS);

	  Array<String> stringArray;
	  stringArray.append("One");
	  stringArray.append("Two");
	  stringArray.append("Three");
      CIMQualifierDecl arrayValue("arrayValue",stringArray,
		  (CIMScope::CLASS + CIMScope::PROPERTY), CIMFlavor::DEFAULTS);

      if(verbose)
	  {
		  XmlWriter::printQualifierDeclElement(q1);
		  XmlWriter::printQualifierDeclElement(q2);
		  XmlWriter::printQualifierDeclElement(q3);
		  XmlWriter::printQualifierDeclElement(abstract);
		  XmlWriter::printQualifierDeclElement(association);
		  XmlWriter::printQualifierDeclElement(key);
		  XmlWriter::printQualifierDeclElement(notToSubclass);
		  XmlWriter::printQualifierDeclElement(toSubclass);
		  XmlWriter::printQualifierDeclElement(toSubclassOverridable);
		  XmlWriter::printQualifierDeclElement(nullValue);
		  XmlWriter::printQualifierDeclElement(arrayValue);
	  }
	// add our test qualifiers to the namespace
    context->addQualifierDecl(NAMESPACE, q1);
    context->addQualifierDecl(NAMESPACE, q2);
    context->addQualifierDecl(NAMESPACE, q3);
    context->addQualifierDecl(NAMESPACE, abstract);
    context->addQualifierDecl(NAMESPACE, association);
    context->addQualifierDecl(NAMESPACE, key);
    context->addQualifierDecl(NAMESPACE, notToSubclass);
    context->addQualifierDecl(NAMESPACE, toSubclass);
    context->addQualifierDecl(NAMESPACE, toSubclassOverridable);

	// Create the superclass and add qualifier, properties and methods
    CIMClass superClass("SuperClass", "");

	// Create property with qualifier that propagates.
	CIMProperty propertyWithQualifier("withQualifier", Boolean(true));
	propertyWithQualifier
		.addQualifier(CIMQualifier("toSubclass", String("default")));
	
	// Create a key property with key qualifier
	CIMProperty keyProperty("keyProperty", Boolean(true));
	keyProperty	
		.addQualifier(CIMQualifier("key", Boolean(true)));
  
	/* ATTN:  Add the following qualifier	to the superclass.  Needs value
		or Null indicator.  Try for Null to see propagation of null
	.addQualifier(CIMQualifier("arrayValue"))
	*/

	// Add our qualifiers and properties to superclass.
	superClass
		// This qualifier should not propagate.
		.addQualifier(CIMQualifier("Abstract", Boolean(true)))
		.addQualifier(CIMQualifier("q1", String("BonJour")))
		.addQualifier(CIMQualifier("notToSubclass", true))
		.addQualifier(CIMQualifier("toSubclass", String("default")))
		.addQualifier(CIMQualifier("toSubclassOverriddable", String("superClass")))
		.addQualifier(CIMQualifier("associat", Boolean(true)))

		.addProperty(CIMProperty(keyProperty))
		.addProperty(CIMProperty("message", String("Hello")))
		.addProperty(CIMProperty("onlyInSuperClass", String("Hello")))
		.addProperty(CIMProperty(propertyWithQualifier))

		// This method to demo propagation of method to subclass
		.addMethod(CIMMethod("methodinSuperclass", CIMType::BOOLEAN)
			.addParameter(CIMParameter("hostname", CIMType::STRING))
			.addParameter(CIMParameter("port", CIMType::UINT32)));

		;

	// ATTN: Add case where property in superclass has
	//   more qualifiers than property in subclass.

	// add the superclass and resolve it.
    context->addClass(NAMESPACE, superClass);
    superClass.resolve(context, NAMESPACE);
    
	// Create the subclass
    CIMClass subClass("SubClass", "SuperClass");

	CIMProperty sndPropertyWithQualifier("sndWithQualifier", Boolean(true));
	sndPropertyWithQualifier
		.addQualifier(CIMQualifier("toSubclass", String("default")));

    subClass
		.addQualifier(CIMQualifier("q1", String("Hello")))
		.addQualifier(CIMQualifier("q3", Uint32(99)))
		.addQualifier(CIMQualifier("toSubclassOverriddable", String("subClass")))

		// the key property should be propagated so do not put in subclass.
		.addProperty(CIMProperty("message", String("Goodbye")))
		.addProperty(CIMProperty("count", Uint32(77)))
		// .addProperty(CIMProperty("ref1", Reference("MyClass.key1=\"fred\"")))

		.addMethod(CIMMethod("isActive", CIMType::BOOLEAN)
			.addParameter(CIMParameter("hostname", CIMType::STRING))
			.addParameter(CIMParameter("port", CIMType::UINT32)));
    if(verbose)
	{
		cout << "Classes before resolution " << endl;
		XmlWriter::printClassElement(superClass);
		XmlWriter::printClassElement(subClass);
	}
    try{
        subClass.resolve(context, NAMESPACE);
        resolved = true;
    }
    catch (Exception& e)
    {
        resolved = false;
        cerr << " Test 02 Did not resolve " << e.getMessage() << endl;
    }
    if(verbose)
	{
		cout << "Classes after resolution " << endl;
		XmlWriter::printClassElement(superClass);
		XmlWriter::printClassElement(subClass);
	}

	// Resolved. Now thoroughly test the results
	assert(resolved);    // Should have correctly resolved.

	// Test the results of the resolve of the subclass and superclass

	// **************************************************8
	// Qualifier Tests on superClass and SubClass
	// **************************************************
	if (verbose) cout << "Tst02 - Test Qualifiers" << endl;
	// Confirm that the qualifiers exist in the superclass that should
	assert(superClass.findQualifier("Abstract") != PEG_NOT_FOUND);
	assert(superClass.findQualifier("ABSTRACT") != PEG_NOT_FOUND);
	assert(superClass.findQualifier("Q3") == PEG_NOT_FOUND);
        assert(superClass.findQualifier("Q1") != PEG_NOT_FOUND);
	assert(superClass.findQualifier("notToSubclass") != PEG_NOT_FOUND);
	assert(superClass.findQualifier("toSubClassOverriddable") != 
            PEG_NOT_FOUND);
	CIMQualifier qt = superClass.getQualifier(superClass.findQualifier("Abstract"));

	// Determine that all required qualifiers exist in the subclass
	assert(subClass.findQualifier("q1") != PEG_NOT_FOUND);
	assert(subClass.findQualifier("Q3") != PEG_NOT_FOUND);
	assert(subClass.findQualifier("toSubClass") != PEG_NOT_FOUND);
	assert(subClass.findQualifier("toSubClassOverriddable") != 
            PEG_NOT_FOUND);
	
	// Confirm that qualifiers that should not be propagated are not.
	assert(subClass.findQualifier("notToSubclass") == PEG_NOT_FOUND);
	assert(subClass.findQualifier("Abstract") == PEG_NOT_FOUND);

	// ATTN: Determine if correct value is in the qualifiers in subclass
	// Need to add a null value qualifier and test its propagation
	// Need to add an array qualifier and test its propagation.
	//Uint32 subclass.findQualifier("Q1"

	// Confirm that the value for tosubclass is still superclass and
	// the value for tosubclassoverride is now subclass

	//	**************************************************
	//	 Property Tests
	// **************************************************

	if (verbose) cout << "Tst02 - Test Properties" << endl;

	// Confirm that correct properties exist in superclass
	assert (superClass.findProperty("message" ) != PEG_NOT_FOUND);
	assert (superClass.findProperty("onlyInSuperclass" ) != PEG_NOT_FOUND);
	assert (superClass.findProperty("withQualifier" ) != PEG_NOT_FOUND);

	//Confirm that correct properties exist in subclass.
	assert (subClass.findProperty("message" ) != PEG_NOT_FOUND);
	assert (subClass.findProperty("count" ) != PEG_NOT_FOUND);
	assert (subClass.findProperty("onlyInSuperclass" ) != PEG_NOT_FOUND);

	// Confirm that all properties in superclass have correct classorigin
	for(Uint32 i = 0; i < superClass.getPropertyCount(); i++)
	{
		CIMProperty p = superClass.getProperty(i);
			assert(p.getClassOrigin() == "SuperClass");
	}
	if (verbose) cout << "Tst02 - Test Properties Move Value" << endl;

	// Determine if we moved the value in the property from superclass.
	{
		Uint32 pos = subClass.findProperty("onlyInSuperclass" );
		assert(pos != PEG_NOT_FOUND);
		
		// It was propagated to subclass. Now get property and test
		CIMProperty p = subClass.getProperty(pos);
		assert(p.getPropagated());

		// Confirm classorigin OK and value not changed.
		assert(p.getClassOrigin() == "SuperClass");
		CIMValue v = p.getValue();
		assert(v.getType() == CIMType::STRING);
		String s;
		v.get(s);
		assert(s == "Hello");     			// Assert correct value moved

		// test that value same in subclass and superclass.
		Uint32 possc = subClass.findProperty("onlyInSuperclass" );
		assert(possc != PEG_NOT_FOUND);
 		CIMProperty psc = subClass.getProperty(pos);
		assert(psc.getPropagated());
		assert(psc.getClassOrigin() == "SuperClass");
		CIMValue vsc = p.getValue();
		assert(vsc.getType() == CIMType::STRING);
		String ssc;
		vsc.get(ssc);
		assert(s == "Hello");     			// Assert correct value moved
		assert (vsc == v);


	}
	// determine if we moved the property with ke qualifier  from superclass
	if (verbose) cout << "Tst02 - Test Properties Move with key" << endl;

	{
		assert (subClass.findProperty("keyProperty") != PEG_NOT_FOUND);
		Uint32 pos = subClass.findProperty("keyProperty" );
		assert(pos != PEG_NOT_FOUND);
		CIMProperty p = subClass.getProperty(pos);
		assert(p.getPropagated());
		assert(p.getClassOrigin() == "SuperClass");
		CIMValue v = p.getValue();
		assert(v.getType() == CIMType::BOOLEAN);
		Boolean b;
		v.get(b);
		assert(b == true);
	}

	// Determine if we moved the qualifier to subclass with the property
	// Note that the identical test won't work since propagated set..
	if (verbose) cout << "Tst02 - Test Properties with qualifier" << endl;

	{
		Uint32 pos = subClass.findProperty("withQualifier" );
		assert(pos != PEG_NOT_FOUND);
		CIMProperty p = subClass.getProperty(pos);
		assert(p.getClassOrigin() == "SuperClass");
		assert(p.getPropagated());
		assert(p.getType() == CIMType::BOOLEAN);
		CIMValue pv = p.getValue();
		assert(pv.getType() == CIMType::BOOLEAN);
		Boolean b;
		pv.get(b);
		assert(b == true);
		assert (p.findQualifier("toSubClass") != PEG_NOT_FOUND);
		
		// Now determine if the value moved.
		assert(p.findQualifier("toSubClass") != PEG_NOT_FOUND);
		Uint32 qpos = p.findQualifier("toSubClass");
		CIMQualifier q = p.getQualifier(qpos);
		CIMValue v = q.getValue();

		assert(v.getType() == CIMType::STRING);
		String s;
		v.get(s);
		assert(s == "default");  // same as value in superclass
	}


	// ************************************************
	// Test the methods propagation here
	// ***********************************************

	if (verbose) cout << "Tst02 - Test Methods" << endl;

	{
		{
		// Test  method in superclass
		// doublecheck the type and that parameters are in place
		assert(superClass.findMethod("methodInSuperclass") != 
                    PEG_NOT_FOUND);
		Uint32 mpos = superClass.findMethod("methodInSuperclass");
		assert(mpos != PEG_NOT_FOUND);
		CIMMethod m = superClass.getMethod(mpos);
		assert(!m.getPropagated()); 				// should not be propagated
		assert(m.getType() == CIMType::BOOLEAN);

		// Now confirm the parameters
        assert(m.findParameter("hostname") != PEG_NOT_FOUND);
        assert(m.findParameter("port") != PEG_NOT_FOUND);

		// Test characteristics of first parameter
		Uint32 ppos = m.findParameter("hostname");
		assert(ppos != PEG_NOT_FOUND);
		CIMParameter mp1 = m.getParameter(m.findParameter("hostname"));
        assert(mp1.getName() == "hostname");
		/* ATTN: KS P3 23 Mar 2002 Want to test values here
		CIMValue vmp1 = mp1.getValue();
		assert(vmp1.getType() == CIMType::Boolean);
		*/
		//assert(p1.getQualifierCount() == 0);

		// Test characteristics of second parameter
		Uint32 ppos2 = m.findParameter("port");
		assert(ppos2 != PEG_NOT_FOUND);
		CIMParameter mp2 = m.getParameter(m.findParameter("port"));
        assert(mp2.getName() == "port");
		assert(mp2.getType() == CIMType::UINT32);

		// Test for second method
		assert(superClass.findMethod("methodInSuperclass") != 
                    PEG_NOT_FOUND);
		}

		// Repeat the above for the subclass and test propagated.
		// ATTN: KS 22 March Complete this P2 - Testing of method propagation
		{
			assert(subClass.findMethod("isActive") != 
                            PEG_NOT_FOUND);
			Uint32 mpos = subClass.findMethod("isActive");
			CIMMethod m = subClass.getMethod(mpos);
			assert(!m.getPropagated()); 				// should not be propagated
			//ATTN: P3-KS-23March 2002 - Tests can be added for parms, etc.
			// Not absolutely necessary.
		}
		// Test for the method propagated from superclass to subclass
		// Confirm that propagated and marked propagated.
		{
			assert(subClass.findMethod("methodInSuperclass") != 
                            PEG_NOT_FOUND);
			Uint32 mpos = subClass.findMethod("methodInSuperclass");
			assert(mpos != PEG_NOT_FOUND);
			CIMMethod m = subClass.getMethod(mpos);
			assert(m.getPropagated()); 				// should not be propagated
			assert(m.getType() == CIMType::BOOLEAN);

			// Now confirm the parameters
			assert(m.findParameter("hostname") != PEG_NOT_FOUND);
			assert(m.findParameter("port") != PEG_NOT_FOUND);

			// Test characteristics of first parameter
			Uint32 ppos = m.findParameter("hostname");
			assert(ppos != PEG_NOT_FOUND);
			CIMParameter mp1 = m.getParameter(m.findParameter("hostname"));
			assert(mp1.getName() == "hostname");
			/* ATTN: Want to test values here
			CIMValue vmp1 = mp1.getValue();
			assert(vmp1.getType() == CIMType::Boolean);
            */
			//assert(p1.getQualifierCount() == 0);
						
			// Test characteristics of second parameter
			Uint32 ppos2 = m.findParameter("port");
			assert(ppos2 != PEG_NOT_FOUND);
			CIMParameter mp2 = m.getParameter(m.findParameter("port"));
			assert(mp2.getName() == "port");
			assert(mp2.getType() == CIMType::UINT32);
		}

	}

    delete context;
}

// Test to confirm that invalid qualifier is caught
// Remove q3 from earlier test and confirm that caught
void test04()
{
    const String NAMESPACE = "/ttt";
	Boolean resolved = false;

    SimpleDeclContext* context = new SimpleDeclContext;

    // Not sure about this one. How do I get NULL as CIMValue
    // This generates an empty string, not NULL
    CIMQualifierDecl q1("q1",String(),CIMScope::CLASS);

    CIMQualifierDecl q2("Abstract", Boolean(true), CIMScope::CLASS , CIMFlavor::NONE);
    
    CIMValue v1(CIMType::UINT32, false);
    CIMQualifierDecl q3("q1",v1,CIMScope::CLASS);
	// flavors for this one should be disableoverride, but tosubclass 
	CIMQualifierDecl key("key",Boolean(true),(CIMScope::PROPERTY + CIMScope::REFERENCE),
	  CIMFlavor::TOSUBCLASS);


    context->addQualifierDecl(NAMESPACE, q1);
    context->addQualifierDecl(NAMESPACE, q2);
    context->addQualifierDecl(NAMESPACE, key);

	CIMProperty keyProperty("keyProperty", Boolean(true));
	keyProperty.addQualifier(CIMQualifier("key", Boolean(true)));

    CIMClass class2("SuperClass", "");

	class2.addProperty(CIMProperty(keyProperty));
    
    context->addClass(NAMESPACE, class2);
    class2.resolve(context, NAMESPACE);
    
    CIMClass class1("SubClass", "SuperClass");

    class1
        .addQualifier(CIMQualifier("Abstract", Boolean(true)))
        .addQualifier(CIMQualifier("q1", String("Hello")))
		.addQualifier(CIMQualifier("q3", Uint32(55)))

	.addProperty(CIMProperty("message", String("Hello")))
	.addProperty(CIMProperty("count", Uint32(77)))
	// .addProperty(CIMProperty("ref1", Reference("MyClass.key1=\"fred\"")))
	.addMethod(CIMMethod("isActive", CIMType::BOOLEAN)
	    .addParameter(CIMParameter("hostname", CIMType::STRING))
	    .addParameter(CIMParameter("port", CIMType::UINT32)));

    if(verbose)
	{
		XmlWriter::printClassElement(class1);
		XmlWriter::printClassElement(class2);
	}
    try{
        class1.resolve(context, NAMESPACE);
		resolved = true;
		if(verbose)
			cout << "Passed basic resolution test" << endl;

        // Add assertions on the resolution.
        // Abstract did not move to subclass
        // 2. et.
    if(verbose)
	{
		cout << "after resolve " << endl;
		XmlWriter::printClassElement(class1);
		XmlWriter::printClassElement(class2);
	}

    }
    catch (Exception& e)
    {
        // should catch error here, q3 invalid qualifier.
		if(verbose)
			cout << "Exception correctly detected missing qualifier declaration: " 
			<< e.getMessage() << endl;
    }
	assert(!resolved);

    delete context;
}
// Test05 - Determine if we correctly detect a property qualifier on a class and 
// reject
void test05()
{
	if(verbose)
		cout << "Test05-Detecting a CIMScope errors - exception" 
		<< endl;	
	const String NAMESPACE = "/ttt";
    Boolean resolved = false;
    SimpleDeclContext* context = new SimpleDeclContext;

	CIMQualifierDecl key("key",Boolean(true),(CIMScope::PROPERTY + CIMScope::REFERENCE),
	  CIMFlavor::TOSUBCLASS);

    //Qualifier Association : boolean = false, Scope(class, association), 
    //  Flavor(DisableOverride);

	CIMQualifierDecl association("association",Boolean(true),
		(CIMScope::CLASS + CIMScope::ASSOCIATION), CIMFlavor::TOSUBCLASS);


	CIMQualifierDecl propertyQualifier("propertyQualifier",Boolean(true),
		(CIMScope::PROPERTY + CIMScope::REFERENCE), CIMFlavor::TOSUBCLASS);

	CIMQualifierDecl classQualifier("classQualifier",Boolean(true),
		(CIMScope::CLASS), CIMFlavor::TOSUBCLASS);


	if(verbose)
	{
		XmlWriter::printQualifierDeclElement(propertyQualifier);
		XmlWriter::printQualifierDeclElement(classQualifier);
		XmlWriter::printQualifierDeclElement(key);
		XmlWriter::printQualifierDeclElement(association);
	}
    context->addQualifierDecl(NAMESPACE, key);
    context->addQualifierDecl(NAMESPACE, association);
    context->addQualifierDecl(NAMESPACE, propertyQualifier);
    context->addQualifierDecl(NAMESPACE, classQualifier);

	CIMProperty keyProperty("keyProperty", Boolean(true));
	keyProperty.addQualifier(CIMQualifier("key", Boolean(true)));

	// create class with property only qualifier and no superclass
    CIMClass classWithPropertyQualifier("classWithPropertyQualifier");
    classWithPropertyQualifier
		.addQualifier(CIMQualifier("propertyQualifier", Boolean(true)))

		.addProperty(CIMProperty(keyProperty))
		.addProperty(CIMProperty("message", String("Hello")))

		.addMethod(CIMMethod("isActive", CIMType::BOOLEAN)
			.addParameter(CIMParameter("hostname", CIMType::STRING)));

    resolved = false;
    if(verbose)
		XmlWriter::printClassElement(classWithPropertyQualifier);

	try
	{
        classWithPropertyQualifier.resolve(context, NAMESPACE);
        resolved = true;
		if(verbose)
		{
			cout << "Test05 Error - Should not have resolved class " << endl;
			XmlWriter::printClassElement(classWithPropertyQualifier);

		}
    }
    catch (Exception& e)
    {
        resolved = false;
		if(verbose)
			cout << " Exception is correct. Should not resolve:	" 
				<< e.getMessage() << endl;
	}
	assert(!resolved);

	// Repeat the test with a class property attached to a property

	// Create a property with a qualifier that has scope class
	CIMProperty propertyWithClassQualifier("propertyWithClassQualifier", Boolean(true));
	propertyWithClassQualifier.addQualifier(CIMQualifier("classQualifier", Boolean(true)));
    
    // Create the class with this bad property
	CIMClass classWithBadProperty("classWithBadProperty");
    classWithBadProperty

		.addProperty(CIMProperty(keyProperty))
		.addProperty(CIMProperty(propertyWithClassQualifier))

		.addMethod(CIMMethod("isActive", CIMType::BOOLEAN)
			.addParameter(CIMParameter("hostname", CIMType::STRING)));
	
    if(verbose)
		XmlWriter::printClassElement(classWithBadProperty);

	try
	{
        classWithBadProperty.resolve(context, NAMESPACE);
        resolved = true;
    }
    catch (Exception& e)
    {
        resolved = false;
		if(verbose)
			cout << " Exception is correct. Should not resolve:	" 
				<< e.getMessage() << endl;
	}
	assert(!resolved);

	// Test that an association class passes the qualifier tests


	// Create Properties and references with Key qualifier
    CIMClass classAssoc("classAssoc");
	try
	{
    classAssoc
		.addQualifier(CIMQualifier("Association", Boolean(true)))

		.addProperty(CIMProperty("ref1", 
			CIMObjectPath("YourClass.key1=\"fred\""),0, "refClassName"))
		.addProperty(CIMProperty("ref2", 
			CIMObjectPath("MyClass.key1=\"fred\""), 0, "refClassName" ))
		;
	}
    catch (Exception& e)
	{
		cout << "Class Creation exception: " << e.getMessage() << endl;
		exit(1);
	}
    if(verbose)
		XmlWriter::printClassElement(classAssoc);
	
	try
	{
        classAssoc.resolve(context, NAMESPACE);
        resolved = true;
    }
    catch (Exception& e)
    {
        resolved = false;
		if(verbose)
			cout << " Exception is Error: Should Resolve correctly: " 
				<< e.getMessage() << endl;
	}
	assert(resolved);

    delete context;
    if(verbose) cout << "End Test05" << endl;
}


// Test for create class with superclass defined but no superclass
void test06()
{
	if(verbose)
		cout << "Test06-Creating class with no superclass - causes exception" 
		<< endl;	
	const String NAMESPACE = "/ttt";
    Boolean resolved = false;
    SimpleDeclContext* context = new SimpleDeclContext;

	CIMQualifierDecl key("key",Boolean(true),(CIMScope::PROPERTY + CIMScope::REFERENCE),
	  CIMFlavor::TOSUBCLASS);

    context->addQualifierDecl(NAMESPACE, key);

	CIMProperty keyProperty("keyProperty", Boolean(true));
	keyProperty.addQualifier(CIMQualifier("key", Boolean(true)));


    CIMClass subClass("SubClass", "SuperClass");
    subClass
		.addProperty(CIMProperty(keyProperty))
		.addProperty(CIMProperty("message", String("Hello")))

		.addMethod(CIMMethod("isActive", CIMType::BOOLEAN)
			.addParameter(CIMParameter("hostname", CIMType::STRING)));
    try
	{
        subClass.resolve(context, NAMESPACE);
        resolved = true;
    }
    catch (Exception& e)
    {
        resolved = false;
		if(verbose)
			cout << " Exception is correct. Should not resolve " << e.getMessage() << endl;
    // ATTN-KS-P3 - Could add test for correct exception message here.  
	// should be INVALID_SUPERCLASS
	}

	assert(!resolved);

    delete context;
}

// Test to confirm that nonoverridable qualifiers cannot be propagated.
void test07()
{
	if (verbose ) cout << "ATTN In process" << endl;
	if(verbose)
		cout << "Test07- Cannot put nonoverridable qualifiers in subclass" 
		<< endl;	

}

//ATTN: KS P1 Mar 7 2002.  Add tests propagation qual, method, propertys as follows:
//  Confirm that qualifiers are propagated correctly based on flavors
//  Confirm that properties and methods are propagated correctly based on flavors
//  

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    
	try
    {
		test01();
        test02();
        //test03();
		test04();
		test05();
		test06(); // Test for no superclass
		test07();   // Confirm noverridable qualifier cannot be in subclass
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
