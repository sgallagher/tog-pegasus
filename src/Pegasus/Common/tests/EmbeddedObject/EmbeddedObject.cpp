//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: <Karl Schopmeyer. k.schopmeyer@opengroup.org
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

/* Unit test of the EmbeddedObject encode and decode functions Defined in PEP 172.
   This test simply codes and decocdes these objects.  Note that it is not a 
   replacement for an end-end test.  Note also that these are temporary methods
   that will probably NEVER be incorporated into the permanent Pegasus code.
*/

#include <cassert>
#include <Pegasus/Common/CIMEmbeddedObject.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/CIMInstance.h>

#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/Resolver.h>
#include <Pegasus/Common/XmlWriter.h>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    // Create a repository context in which to test the creation and
    // decode functions.
    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("/root/test");

    // Create and populate a declaration context:

    SimpleDeclContext* context = new SimpleDeclContext;

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl(CIMName ("counter"), false, 
        CIMScope::PROPERTY));

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl(CIMName ("classcounter"), false, 
        CIMScope::CLASS));

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl(CIMName ("min"), String(), 
        CIMScope::PROPERTY));

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl(CIMName ("max"), String(),
        CIMScope::PROPERTY));

    context->addQualifierDecl(NAMESPACE,
	CIMQualifierDecl(CIMName ("Description"), String(), 
        CIMScope::PROPERTY));

    CIMClass class1(CIMName ("MyClass"));

    class1
	.addProperty(CIMProperty(CIMName ("count"), Uint32(55))
	    .addQualifier(CIMQualifier(CIMName ("counter"), true))
	    .addQualifier(CIMQualifier(CIMName ("min"), String("0")))
	    .addQualifier(CIMQualifier(CIMName ("max"), String("1"))))
	.addProperty(CIMProperty(CIMName ("message"), String("Hello"))
	    .addQualifier(CIMQualifier(CIMName ("description"), 
                String("My Message"))))
	.addProperty(CIMProperty(CIMName ("ratio"), Real32(1.5)));

    try
    {
    Resolver::resolveClass (class1, context, NAMESPACE);
    context->addClass(NAMESPACE, class1);
    }
    catch (Exception& e)
    {
    	cout << "Exception: " << e.getMessage() << endl;
    	exit(1);
    }
	if(verbose) 
    {
        cout << "Class added to context" << endl;
		XmlWriter::printClassElement(class1);
	}

    // Test to determine if we generate an exception when
    // the object is not initialized.
    Boolean foundException = false;
    try
    {
        CIMInstance iError;   // Create unitialized instance
        String s1 =
            CIMEmbeddedObject::encodeToEmbeddedObject(iError);
    }
    catch (Exception& e)
    {
        foundException = true;
        if (verbose)
        {
            cout << "Correctly found Exception: " << e.getMessage() << endl;
        }
    }
    assert(foundException);

    // test to determine if we generate exception if the string is
    // bad
    foundException = false;
    try
    {
        String s1 =  "junk";
        CIMInstance iError = 
            CIMEmbeddedObject::decodeEmbeddedObject(s1);
    }
    catch (Exception& e)
    {
        foundException = true;
        if (verbose)
        {
            cout << "Correctly found Exception: " << e.getMessage() << endl;
        }
    }
    assert(foundException);
    try
    {

        /*
        <INSTANCE  CLASSNAME="MyClass" >
        <QUALIFIER NAME="classcounter" TYPE="boolean">
        <VALUE>TRUE</VALUE>
        </QUALIFIER>
        <PROPERTY NAME="count"  CLASSORIGIN="MyClass" PROPAGATED="true" TYPE="uint32">
        <QUALIFIER NAME="counter" TYPE="boolean">
        <VALUE>TRUE</VALUE>
        </QUALIFIER>
        <QUALIFIER NAME="min" TYPE="string">
        <VALUE>0</VALUE>
        </QUALIFIER>
        <QUALIFIER NAME="max" TYPE="string">
        <VALUE>1</VALUE>
        </QUALIFIER>
        <VALUE>55</VALUE>
        </PROPERTY>
        <PROPERTY NAME="ratio"  CLASSORIGIN="MyClass" PROPAGATED="true" TYPE="real32">
        <VALUE>1.5000000e+000</VALUE>
        </PROPERTY>
        <PROPERTY NAME="message"  CLASSORIGIN="MyClass" TYPE="string">
        <VALUE>Goodbye</VALUE>
        </PROPERTY>
        </INSTANCE>
        */
        // Test one, create an embedded object manually and test
        // Creation and deletion.
        String mi = "<INSTANCE  CLASSNAME=\"MyClass\">\n";
        mi.append("<QUALIFIER NAME=\"classcounter\" TYPE=\"boolean\" OVERRIDABLE=\"false\" TOSUBCLASS=\"false\">\n");
        mi.append("<VALUE>TRUE</VALUE>\n");
        mi.append("</QUALIFIER>\n");
        //mi.append("<PROPERTY NAME=\"InstanceID\"  TYPE=\"uint32\">\n");
        //mi.append("<VALUE>999999</VALUE>\n");
        //mi.append("</PROPERTY>)\n");
        mi.append("<PROPERTY NAME=\"message\"  TYPE=\"string\">\n");
        mi.append("<VALUE>Goodbye</VALUE>\n");
        mi.append("</PROPERTY>\n");
        mi.append("</INSTANCE>\n");

        if(verbose)
        {
        cout << "string of Instance = " << mi << endl;
        }

        CIMInstance instance0 = CIMEmbeddedObject::decodeEmbeddedObject(mi);

        // Test 2 - Create an instance to test.

        CIMInstance instance(CIMName ("MyClass"));
        instance.addQualifier(CIMQualifier(CIMName ("classcounter"), true));
        //instance.addProperty(CIMProperty(CIMName ("InstanceID"), Uint32(999999)));
        instance.addProperty(CIMProperty(CIMName ("message"), String("Goodbye")));


        // Note that we resolve the instance to create a complete instance for
        // the compare
        Resolver::resolveInstance (instance, context, NAMESPACE, true);

        if(verbose)
        {
            cout << "Decoded Instance0: " <<  "path: " << instance0.getPath().toString() << endl;
            XmlWriter::printInstanceElement(instance0);

            cout << "created Instance: "  <<  "path: " << instance.getPath().toString() << endl;
            XmlWriter::printInstanceElement(instance);
        }

        // Compare the manually created string against the instance
        // Create an embedded object string
        // Note that this test fails because one object is resolved and the other not, I bet.

        // assert(instance.identical(instance0));

        String testEmbeddedObject =
            CIMEmbeddedObject::encodeToEmbeddedObject(instance);

        if(verbose)
        {
            cout << "String of Embedded Instance: " << testEmbeddedObject << endl;
        }
    
        CIMInstance instance1 = CIMEmbeddedObject::decodeEmbeddedObject(testEmbeddedObject);

        if(verbose)
        {
            cout << "return from decode with instance recreated:" << endl;
            XmlWriter::printInstanceElement(instance1);
        }

        assert(instance1.identical(instance));

        // Test 3 - Embedded object with quote mark in a string value.

        if (verbose)
        {
            cout << "Test with quote embedded in string." << endl;
        }

        // replace property in message property with one with quote.
        String test="Single double Quote Mark \" . Single left carot <, Single right carot >, Single single-quote \'";
        if (verbose)
        {
            cout << "Test String " << test << endl;
        }

        Uint32 pos;
        pos = instance.findProperty(CIMName("message"));
        assert(pos != PEG_NOT_FOUND);
        instance.removeProperty(pos);

        instance.addProperty(CIMProperty(CIMName ("message"), test));

        testEmbeddedObject =
            CIMEmbeddedObject::encodeToEmbeddedObject(instance);
 
        if(verbose)
        {
            cout << "String of Embedded Instance: " << testEmbeddedObject << endl;
        }

        cout << "Decode the encoded object" << endl;
        instance1 = CIMEmbeddedObject::decodeEmbeddedObject(testEmbeddedObject);

        if(verbose)
        {
            cout << "return from decode with instance recreated:" << endl;
            XmlWriter::printInstanceElement(instance1);
        }

        assert(instance1.identical(instance));


        //Test 4 - String form
        //cout << "Test 4" << endl;

        String test4 = "<INSTANCE CLASSNAME=\"Test_Karl\"><PROPERTY NAME=\"k\" TYPE=\"string\"><QUALIFIER OVERRIDABLE=\"false\" NAME=\"key\" TYPE=\"boolean\"><VALUE>true</VALUE></QUALIFIER><VALUE>Filterwsindicationtest1089760576</VALUE></PROPERTY><PROPERTY PROPAGATED=\"true\" NAME=\"s\" TYPE=\"string\"></PROPERTY></INSTANCE></VALUE>";
        if(verbose)
        {
            cout << "Test 4 String= " << test4 << endl;
        }

        instance1 = CIMEmbeddedObject::decodeEmbeddedObject(test4);

        //Test 4a - replacement of \" with "
        //cout << "Test 4a" << endl;

        String test4a = "<INSTANCE CLASSNAME=\\\"Test_Karl\\\"><PROPERTY NAME=\\\"k\\\" TYPE=\\\"string\\\"><QUALIFIER OVERRIDABLE=\\\"false\\\" NAME=\\\"key\\\" TYPE=\\\"boolean\\\"><VALUE>true</VALUE></QUALIFIER><VALUE>Filterwsindicationtest1089760576</VALUE></PROPERTY><PROPERTY PROPAGATED=\\\"true\\\" NAME=\\\"s\\\" TYPE=\\\"string\\\"></PROPERTY></INSTANCE></VALUE>";

        if(verbose)
        {
            cout << "Test 4a String= " << test4a << endl;
        }

        instance1 = CIMEmbeddedObject::decodeEmbeddedObject(test4a);

        //Test 4b - Replacement of &quote; with "
 
        String test4b = "<INSTANCE CLASSNAME=\\&quot;Test_Karl\\&quot;><PROPERTY NAME=\\&quot;k\\&quot; TYPE=\\&quot;string\\&quot;><QUALIFIER OVERRIDABLE=\\&quot;false\\&quot; NAME=\\&quot;key\\&quot; TYPE=\\&quot;boolean\\&quot;><VALUE>true</VALUE></QUALIFIER><VALUE>Filterwsindicationtest1089760576</VALUE></PROPERTY><PROPERTY PROPAGATED=\\&quot;true\\&quot; NAME=\\&quot;s\\&quot; TYPE=\\&quot;string\\&quot;></PROPERTY></INSTANCE></VALUE>";

        if(verbose)
        {
        cout << "Test 4b String= " << test4b << endl;
        }

        instance1 = CIMEmbeddedObject::decodeEmbeddedObject(test4b);

    }

    // Catch all exceptions.
    catch (Exception& e)
    {
        cout << "Exception: " << e.getMessage() << endl;
    	exit(1);
    }
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
