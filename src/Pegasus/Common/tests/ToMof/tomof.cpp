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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

// #define IO
char * verbose;

template<class T>
void test01(const T& x)
{
    CIMValue v(x);
    CIMValue v2(v);
    CIMValue v3;
    v3 = v2;
    if (verbose)
        XmlWriter::printValueElement(v3, cout);

    Array<char> tmp1;
    MofWriter::appendValueElement(tmp1, v3);

    tmp1.append('\0');
    if (verbose)
        cout << "\nMOF = " << tmp1.getData() << PEGASUS_STD(endl);
    try
    {
	T t;
	v3.get(t);
	assert(t == x);
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    // Test values

    Array<Uint32> aa;
    CIMValue vv(aa);

    test01(Boolean(true));
    test01(Boolean(false));
    test01(Char16('Z'));
    test01(Uint8(77));
    test01(Sint8(-77));
    test01(Sint16(77));
    test01(Uint16(-77));
    test01(Sint32(77));
    test01(Uint32(-77));
    test01(Sint64(77));
    test01(Uint64(-77));
    test01(Real32(1.5));
    test01(Real64(55.5));
    test01(Uint64(123456789));
    test01(Sint64(-123456789));
    test01(CIMObjectPath("MyClass.key1=20,key2=\"my name\""));

    Array<String> arr;
    arr.append("One");
    arr.append("Two");
    arr.append("Three");
    CIMValue v(arr);

    // Test Properties
    {
    CIMProperty p1(CIMName ("message"), String("Hi There"));
    Array<char> tmp;
    MofWriter::appendPropertyElement(tmp, p1);
    tmp.append('\0');
    if (verbose)
        cout << "\nProperty MOF = " << tmp.getData() << PEGASUS_STD(endl);
    }
    {
    CIMProperty p2(CIMName ("message"), String("test"));
    Array<char> tmp;
    MofWriter::appendPropertyElement(tmp, p2);
    tmp.append('\0');
    if (verbose)
        cout << "\nProperty MOF = " << tmp.getData() << PEGASUS_STD(endl);
    }

    // Test CimClass none Association
    {
        if (verbose)
           cout << "Class test\n";
	CIMClass class1(CIMName ("MyClass"), CIMName ("YourClass"));

	class1
	    .addQualifier(CIMQualifier(CIMName ("q1"), Uint32(55)))
	    .addQualifier(CIMQualifier(CIMName ("q2"), String("Hello")))
	    .addProperty(CIMProperty(CIMName ("message"), String("Hello")))
	    .addProperty(CIMProperty(CIMName ("count"), Uint32(77)))
	    .addMethod(CIMMethod(CIMName ("isActive"), CIMTYPE_BOOLEAN)
		.addParameter(CIMParameter(CIMName ("hostname"), 
                    CIMTYPE_STRING))
		.addParameter(CIMParameter(CIMName ("port"), CIMTYPE_UINT32)));
        if (verbose)
            XmlWriter::printClassElement(class1);
	Array<char> tmp;
	MofWriter::appendClassElement(tmp, class1);
	tmp.append('\0');
        if (verbose)
	    cout << "\nClass MOF = " << tmp.getData() << PEGASUS_STD(endl);

    }
    
    
    // Test CimClass This is not really an association class.
    {
	
        if (verbose)
            cout << "Class test\n";
	CIMClass class1(CIMName ("MyClass"), CIMName ("YourClass"));

	class1
	    .addQualifier(CIMQualifier(CIMName ("association"), true))
	    .addQualifier(CIMQualifier(CIMName ("q1"), Uint32(55)))
	    .addQualifier(CIMQualifier(CIMName ("q2"), String("Hello")))
	    .addProperty(CIMProperty(CIMName ("message"), String("Hello")))
	    .addProperty(CIMProperty(CIMName ("count"), Uint32(77)))
	    .addMethod(CIMMethod(CIMName ("isActive"), CIMTYPE_BOOLEAN)
		.addParameter(CIMParameter(CIMName ("hostname"), 
                    CIMTYPE_STRING))
		.addParameter(CIMParameter(CIMName ("port"), CIMTYPE_UINT32)));
        if (verbose)
            XmlWriter::printClassElement(class1);
	Array<char> tmp;
	MofWriter::appendClassElement(tmp, class1);
	tmp.append('\0');
        if (verbose)
	    cout << "\nClass MOF = " << tmp.getData() << PEGASUS_STD(endl);

    }
    // Test CimClass This is not really an association class.
    /* 
    [abstract]
	class class1
	{
	[read]
	string DriveLetter;
	[read, Units("KiloBytes")]
	sint32 RawCapacity = 0;
	[write]
	string VolumeLabel;
	boolean NoParmsMethod();
	[Dangerous]
	boolean OneParmMethod([in] boolean FastFormat);
	}; */
    {
        if (verbose)
	    cout << "Class test of class1\n";

	CIMClass class1(CIMName ("MyClass"), CIMName ("YourClass"));

	class1
	    .addQualifier(CIMQualifier(CIMName ("abstract"), true))
    	    .addQualifier(CIMQualifier(CIMName ("description"), 
	       String("This is a Description of my class. "
	       "This is part 2 of the string to make it longer. "
	       "This is part 3 of the same string for nothing.")
				       
				       ))

	    .addProperty(CIMProperty(CIMName ("DriveLetter"), String(""))
		.addQualifier(CIMQualifier(CIMName ("read"), true)))

	    .addProperty(CIMProperty(CIMName ("RawCapacity"),Sint32(99))
		.addQualifier(CIMQualifier(CIMName ("read"), true))
		.addQualifier(CIMQualifier(CIMName ("Units"), 
                    String("KiloBytes"))))

	    .addProperty(CIMProperty(CIMName ("VolumeLabel"), String(" ")))
	
    	    .addMethod(CIMMethod(CIMName ("NoParmsMethod"), CIMTYPE_BOOLEAN))
	    
	    .addMethod(CIMMethod(CIMName ("OneParmmethod"), CIMTYPE_BOOLEAN)
	        .addParameter(CIMParameter(CIMName ("FastFormat"), 
                    CIMTYPE_BOOLEAN)
	        	   .addQualifier(CIMQualifier(CIMName ("Dangerous"), 
                               true))
			     )
		      )

	    .addMethod(CIMMethod(CIMName ("TwoParmMethod"), CIMTYPE_BOOLEAN)
		.addParameter(CIMParameter(CIMName ("FirstParam"), 
                    CIMTYPE_BOOLEAN)
			   .addQualifier(CIMQualifier(CIMName ("Dangerous"), 
                               true))
			   .addQualifier(CIMQualifier(CIMName ("in"),true)) 	     
			    )
		.addParameter(CIMParameter(CIMName ("SecondParam"), 
                    CIMTYPE_BOOLEAN)
			    .addQualifier(CIMQualifier(CIMName ("Dangerous"), 
                                true))
			    .addQualifier(CIMQualifier(CIMName ("in"),true)) 	     
			    )

		     )
                     ;
        if (verbose)
            XmlWriter::printClassElement(class1);
	//Array<char> tmp;
	//MofWriter::appendClassElement(tmp, class1);
	//tmp.append('\0');

        if (verbose)
	    MofWriter::printClassElement(class1);
    }

    {
    /*
    Qualifier flavors are indicated by an optional clause after the qualifier
    and preceded by a colon.
    They consist of some combination of the key words EnableOverride,
    DisableOverride,
    ToSubclass and Restricted, indicating the applicable propagation
    and override rules.
    
    Handle is designated as writable for the Process class and for every subclass of
    this class.
    class CIM_Process:CIM_LogicalElement
	{
	uint32 Priority;
	[Write(true):DisableOverride ToSubclass]
	string Handle;
	};
     */
    }

    //Qualifier declaration Test
    {
	try
	{
	    CIMQualifierDecl qual1(
		CIMName ("CIMTYPE"), 
		String(),
		CIMScope::PROPERTY,
		CIMFlavor::TOINSTANCE);

	    //Qualifier Abstract : boolean = false, Scope(class, association, 
	    //indication), Flavor(DisableOverride, Restricted);

	    CIMQualifierDecl q2(
		CIMName ("Abstract"),
		true, 
		CIMScope::CLASS,
		CIMFlavor::OVERRIDABLE)
	    ;

            if (verbose)
	        XmlWriter::printQualifierDeclElement(qual1);
	    Array<char> tmp;
	    MofWriter::appendQualifierDeclElement(tmp, qual1);
	    tmp.append('\0');
            if (verbose)
	        cout << "Qualifier Test\n" << tmp.getData() << "\n\n";
            if (verbose)
	        XmlWriter::printQualifierDeclElement(q2);
	    Array<char> tmp1;
	    MofWriter::appendQualifierDeclElement(tmp1, q2);
	    tmp1.append('\0');
            if (verbose)
	        cout << "Qualifier Test\n" << tmp1.getData() << "\n\n";
	}
	catch(Exception& e)
	{
	    cerr << "Exception: " << e.getMessage() << endl;
	}
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
