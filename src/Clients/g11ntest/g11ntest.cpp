//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002, 2003 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Chuck Carmack (carmack@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <cassert>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/XmlWriter.h>

#include <Pegasus/Common/AcceptLanguages.h>
#include <Pegasus/Common/ContentLanguages.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define TESTMACRO(EXCTYPE) throw new EXCTYPE;

//#define MYASSERT ASSERTTEMP
#define MYASSERT PEGASUS_ASSERT

static const char* programVersion =  "1.0";

// Array of UTF-16 chars to be sent and received to the server.
// Note: the dbc0/dc01 pair are surrogates
Char16 utf16Chars[] =
        {
        0x6A19,	0x6E96,	0x842C, 0x570B,	0x78BC,
        0x042E, 0x043D, 0x0438, 0x043A, 0x043E, 0x0434,
        0x110B, 0x1172, 0x1102, 0x1165, 0x110F, 0x1169, 0x11AE, 
        0x10E3, 0x10DC, 0x10D8, 0x10D9, 0x10DD, 0x10D3, 0x10D8,
	0xdbc0,	0xdc01, 
        0x05D9, 0x05D5, 0x05E0, 0x05D9, 0x05E7, 0x05D0, 0x05B8, 0x05D3,
        0x064A, 0x0648, 0x0646, 0x0650, 0x0643, 0x0648, 0x062F,
        0x092F, 0x0942, 0x0928, 0x093F, 0x0915, 0x094B, 0x0921,
        0x016A, 0x006E, 0x012D, 0x0063, 0x014D, 0x0064, 0x0065, 0x033D,
        0x00E0, 0x248B, 0x0061, 0x2173, 0x0062, 0x1EA6, 0xFF21, 0x00AA, 0x0325, 0x2173, 0x249C, 0x0063,
        0x02C8, 0x006A, 0x0075, 0x006E, 0x026A, 0x02CC, 0x006B, 0x006F, 0x02D0, 0x0064,
        0x30E6, 0x30CB, 0x30B3, 0x30FC, 0x30C9, 
        0xFF95, 0xFF86, 0xFF7A, 0xFF70, 0xFF84, 0xFF9E, 
        0xC720, 0xB2C8, 0xCF5B, 0x7D71, 0x4E00, 0x78BC,
	0xdbc0,	0xdc01,
        0x00};
                    
/** ErrorExit - Print out the error message as an
    and get out.
    @param - Text for error message
    @return - None, Terminates the program
    @execption - This function terminates the program
    ATTN: Should write to stderr
*/
void ErrorExit(const String& message)
{

    cout << message << endl;
    exit(1);
}


/* Status display of the various steps.  Shows message of function and
time to execute.  Grow this to a class so we have start and stop and time
display with success/failure for each function.
*/
static void testStart(const String& message)
{
    cout << "++++ " << message << " ++++" << endl;

}

static void testEnd(const double elapsedTime)
{
    cout << "In " << elapsedTime << " Seconds\n\n";
}


/*
   Tests the UTF support in the repository
*/
static void TestUTFRepository( CIMClient& client, Boolean verboseTest )
{
  const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/SampleProvider");

  Boolean deepInheritance = true;
  Boolean localOnly = true;
  Boolean includeQualifiers = true;
  Boolean includeClassOrigin = true;

  try
    {
        String utf16String(utf16Chars);

  	//
  	//  TEST 1 - Create qualifier 
  	//  Create a qualifier with UTF-16 in the qualifier name,
        //  and value
  	//  This will test UTF support in the repository.
  	//

	cout << endl << "REPOSITORY TEST 1: Create Qualifier with UTF-16 chars" << endl;	

        CIMName qualDeclName("UTFTestQualifier");  
//      CIMName qualDeclName(utf16String);    // This will create a file with UTF-8 chars in the name

        //  First, delete the qualifier if it was there from before  
        if (verboseTest)
	    cout << "Deleting the qualifier in case it was there from before" << endl;

        try
        {
       	  client.deleteQualifier(NAMESPACE, qualDeclName);
        }
        catch (CIMException & e)
        {
          if (e.getCode() != CIM_ERR_NOT_FOUND)
            throw e;
        }

        if (verboseTest)
	    cout << "Creating the qualifier" << endl;

        CIMQualifierDecl qualDecl(qualDeclName, 
                     utf16String, 
                     CIMScope::PROPERTY + CIMScope::CLASS,
		     CIMFlavor::OVERRIDABLE);

        client.setQualifier(NAMESPACE, qualDecl);

        if (verboseTest)
	    cout << "Getting the qualifier that was just created" << endl;

        CIMQualifierDecl rtnQualDecl = client.getQualifier(
			NAMESPACE,
			qualDeclName);

        if (verboseTest)
	    cout << "Checking that UTF-16 was preserved" << endl;

        MYASSERT(qualDecl.identical(rtnQualDecl));

  	//
  	//  TEST 2 - Create class 
  	//  Create a class with UTF-16 in the class name,
        //  property names, and qualifiers.
        //
        //  We will use the UTF16 qualifier created above
        //  in this class.
        //
  	//  This will test UTF support in the repository.
  	//

	cout << endl << "REPOSITORY TEST 2: Create Class with UTF-16 chars" << endl;	

        CIMName className("UTFTestClass");
//      CIMName className(utf16String);    // This will create a file with UTF-8 chars in the name

        //  First, delete the class if it was there from before  
        if (verboseTest)
	    cout << "Deleting the class in case it was there from before" << endl;

        try
        {
       	  client.deleteClass(NAMESPACE, className);
        }
        catch (CIMException & e)
        {
          if (e.getCode() != CIM_ERR_NOT_FOUND)
            throw e;
        }
                 
        if (verboseTest)
	    cout << "Creating the class" << endl;

        // How did this get so ugly?
        CIMClass cimClass(className, CIMName ("CIM_ManagedElement"));
        CIMName name1("fred");
        CIMName name2(utf16String);
        CIMValue fred("fred");
        CIMName key("key");

        CIMProperty prop1(name1, fred);
        CIMQualifier qual1(key, true);
        CIMProperty prop2 = prop1.addQualifier(qual1);  
        cimClass.addProperty(prop2);

        CIMProperty prop3(name2, fred);             // UTF16 prop name
        CIMQualifier qual2(qualDecl.getName(),      // UTF16 qualifier
                           qualDecl.getValue(),
                           qualDecl.getFlavor());
        CIMProperty prop4 = prop3.addQualifier(qual2);
        cimClass.addProperty(prop4);

       	client.createClass(NAMESPACE, cimClass);

        if (verboseTest)
	    cout << "Getting the class that was just created" << endl;

        CIMClass rtnClass = client.getClass(
			NAMESPACE,
			className,
			true,
                    true);

        if (verboseTest)
	    cout << "Checking that the UTF-16 was preserved is property name" << endl;

	Uint32 idx = rtnClass.findProperty(name2);  // name2 is UTF16
        MYASSERT(idx != PEG_NOT_FOUND);

        if (verboseTest)
	    cout << "Checking that the UTF-16 was preserved in qualifier" << endl;

	CIMProperty rtnProp = rtnClass.getProperty(idx);
        idx = rtnProp.findQualifier(qualDecl.getName());
        MYASSERT(idx != PEG_NOT_FOUND);

        CIMQualifier rtnQual = rtnProp.getQualifier(idx);
        MYASSERT(rtnQual.getValue() == utf16String);

        //
        // Clean up the repository
        //
        if (verboseTest)
	    cout << endl << "Cleaning up the repository" << endl;

        if (verboseTest)
	    cout << "Delete the qualifier" << endl;

     	client.deleteQualifier(NAMESPACE, qualDeclName);

        if (verboseTest)
	    cout << "Deleting the class" << endl;

        client.deleteClass(NAMESPACE, className);
    }
    catch(Exception& e)
    {
      PEGASUS_STD(cerr) << "Error in TestLocalizedRepository: " << e.getMessage() << PEGASUS_STD(endl);
      throw e;      
    }
}
 
/*
   Tests the globalization support of the LocalizedProvider
   for the method operations
*/
static void TestLocalizedMethods( CIMClient& client, Boolean verboseTest )
{
  const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/SampleProvider");
  const CIMName CLASSNAME = CIMName ("Sample_LocalizedProviderClass");
  const String INPARAM1 = "inStr";
  const String INPARAM2 = "inChar16";
  const CIMName METHOD = CIMName("UTFMethod");
  const CIMObjectPath REFERENCE = CIMObjectPath("Sample_LocalizedProviderClass.Identifier=0");

  // Array of UTF-16 chars to be sent and received to the server.
  // Note: the first 3 chars are taken from section 3.3.2 of the CIM-over-HTTP spec.
  // The next 2 chars are a UTF-16 surrogate pair
  Char16 hangugo[] = {0xD55C, 0xAD6D, 0xC5B4,
			0xdbc0,
			0xdc01,
                    'g','l','o','b','a','l',
			0x00};

  try
  {
     // Before we begin, set the language objects to be sent to the provider
     AcceptLanguages AL_DE;
     AL_DE.add(AcceptLanguageElement("de", float(0.8)));
     ContentLanguages CL_DE("de"); 
     client.setRequestAcceptLanguages(AL_DE);
     client.setRequestContentLanguages(CL_DE);

     //
     //  TEST 1 - Invoke Method with UTF-16 in input parameters, output parameters
     //  and return value. 
     //
     //  This will also check that Accept-Language and Content-Language are
     //  are sent to and received from a Method Provider.

     cout << endl << "METHOD TEST 1: Invoke Method with UTF-16 parameters" << endl;	

     // Strings sent to the provider
     String inString(utf16Chars);
     Char16 inChar16 = utf16Chars[1];

     // Expected strings from the provider
     String outString(utf16Chars);
     Char16 outChar16 = utf16Chars[2];
     String expectedRtnString(utf16Chars);

     // Strings returned by the provider 
     String rtnString;
     String outParam1;
     Char16 outParam2;

     Array<CIMParamValue> inParams;
     Array<CIMParamValue> outParams;

     inParams.append( CIMParamValue(  INPARAM1, CIMValue( inString ) ) );
     inParams.append( CIMParamValue(  INPARAM2, CIMValue( inChar16 ) ) );

     if (verboseTest)
        cout << "Invoking the method" << endl;

     CIMValue retValue = client.invokeMethod(
	    NAMESPACE, 
	    REFERENCE, 
	    METHOD,
	    inParams, 
	    outParams);

     if (verboseTest)
        cout << "Checking for UTF-16 chars returned" << endl;

     retValue.get(rtnString); 
     MYASSERT (expectedRtnString == rtnString);

     CIMValue paramVal = outParams[0].getValue();
     paramVal.get( outParam1 );
     MYASSERT (outString == outParam1);

     paramVal = outParams[1].getValue();
     paramVal.get( outParam2 );
     MYASSERT (outChar16 == outParam2);

     if (verboseTest)
        cout << "Checking returned Content-Language = " << CL_DE << endl;

     MYASSERT(CL_DE == client.getResponseContentLanguages());

     //
     //  TEST 2 - Invoke method with UTF-16 in the method name.
     //
     //  This will test the URI encoding and decoding of UTF-8 in the CIMMethod
     //  HTTP header

     cout << endl << "METHOD TEST 2: Invoke Method with UTF-16 method name" << endl;

     String methodName(hangugo);	

     if (verboseTest)
        cout << "Invoking the method" << endl;

     CIMValue retValue1 = client.invokeMethod(
	    NAMESPACE, 
	    REFERENCE, 
	    methodName,
	    inParams, 
	    outParams);

     // Check UTF-16 in the return value just to make sure that the method
     // was called on the provider.
     String expectedRtnString1(hangugo);
     String rtnString1;

     if (verboseTest)
        cout << "Checking for UTF-16 chars returned" << endl;

     retValue1.get(rtnString1); 
     MYASSERT (expectedRtnString1 == rtnString1);	

  }
  catch(Exception& e)
  {
    PEGASUS_STD(cerr) << "Error in TestLocalizedMethod: " << e.getMessage() << PEGASUS_STD(endl);
    throw e;      
  }
}

/*
   Tests the globalization support of the LocalizedProvider
   for the instance operations
*/
static void TestLocalizedInstances( CIMClient& client, Boolean verboseTest )
{
  const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/SampleProvider");
  const CIMName CLASSNAME = CIMName ("Sample_LocalizedProviderClass");
  const CIMName SUBCLASSNAME = CIMName ("Sample_LocalizedProviderSubClass");
  const String INSTANCE0 = "instance 0Sample_LocalizedProviderClass";
  const String INSTANCE1 = "instance 1Sample_LocalizedProviderClass";
  const String RBPROP = "ResourceBundleString";
  const String ROUNDTRIPSTRINGPROP = "RoundTripString";
  const String ROUNDTRIPCHARPROP = "RoundTripChar";
  const String CLPROP = "ContentLanguageString";
  const String TESTSTRINGPROP = "TestString";
  const String IDPROP = "Identifier";

  ContentLanguages CL_DE("de");
  String expectedDEString = "ResourceBundleString DE";
  ContentLanguages CL_FR("fr");
  String expectedFRString = "ResourceBundleString FR";		
  ContentLanguages CL_Dft = ContentLanguages::EMPTY;
  ContentLanguages CL_EN("en");
  String expectedDftString = "ResourceBundleString DEFAULT";

  ContentLanguages CL_HOMER("x-homer");
  ContentLanguages CL_ES("es");  
		
  String expectedUTF16String(utf16Chars);			

  try
    {
      const CIMName className = CLASSNAME;
      Boolean deepInheritance = false;
      Boolean localOnly = false;
      Boolean includeQualifiers = false;
      Boolean includeClassOrigin = false;
      Uint32 testRepeat = 1;

      if (verboseTest)
          cout << "Getting class: " << CLASSNAME << endl;	

      CIMClass sampleClass = client.getClass(
			NAMESPACE,
			CLASSNAME,
			localOnly,
			includeQualifiers,
			includeClassOrigin);

      CIMClass sampleSubClass = client.getClass(
			NAMESPACE,
			SUBCLASSNAME,
			localOnly,
			includeQualifiers,
			includeClassOrigin);
	
      // Note:  When Pegasus has external message files enabled,
      // then the provider has message files for fr, de,
      // and es, but no root message file.  The missing root
      // message file will cause us to test the default message
      // strings compiled into the code.	
 
      for (Uint32 i = 0; i < testRepeat; i++)        // repeat the test x time
      {
          //
          //  TEST 1 - Enumerate Instances of the base class, deepInheritance == false.  
          //  de is supported by the provider, and is the most preferred by us.
          //
	  	
          AcceptLanguages acceptLangs1;
          acceptLangs1.add(AcceptLanguageElement("fr", float(0.5)));
          acceptLangs1.add(AcceptLanguageElement("de", float(0.8))); 
          acceptLangs1.add(AcceptLanguageElement("es", float(0.4)));    	  	
          client.setRequestAcceptLanguages(acceptLangs1);
        	
	  cout << endl << "INSTANCE TEST 1: Enumerate Instances with AcceptLanguages = " << acceptLangs1 << endl;	        	
        	
          Array<CIMInstance> cimNInstances =
                              client.enumerateInstances(NAMESPACE,  className, deepInheritance,
                                                        localOnly,  includeQualifiers,
                                                        includeClassOrigin );
			
          if (verboseTest)	      
              cout << "Found " << cimNInstances.size() << " Instances of " << className << endl;

// Note: this should really be 2 because there are 2 instances of the base class,
// but Pegasus ignores deepInheritance == false
          MYASSERT(cimNInstances.size() == 3);

#ifdef PEGASUS_HAS_MESSAGES	
          if (verboseTest)
              cout << "Checking expected response ContentLanguages: " << CL_DE << endl;

          MYASSERT(CL_DE == client.getResponseContentLanguages());
#else
          if (verboseTest)
              cout << "Checking expected response ContentLanguages: " << CL_EN << endl;

          MYASSERT(CL_EN == client.getResponseContentLanguages());
#endif		

          if (verboseTest)
              cout << "Checking the returned string in each instance" << endl;

          for (Uint32 k = 0; k < cimNInstances.size(); k++)
          {
              String enumString;
              cimNInstances[k].getProperty (cimNInstances[k].findProperty(RBPROP)).
                      getValue().
                      get(enumString);

#ifdef PEGASUS_HAS_MESSAGES        	    
              MYASSERT(expectedDEString == enumString);  
#else
              MYASSERT(expectedDftString == enumString);
#endif			      	        			
          }

          //
          //  TEST 2 - Aggregation Test - Enumerate Instances starting at the base class,
          //  with deep inheritance, where each instance has the same language.  
          //  This will test that the languages of the instances are aggregated
          //  into the Content-Language of the whole response.
          //  
          //  (tests ContentLanguage aggregation in CIMOperationRequestDispatcher)

	  cout << endl << "INSTANCE TEST 2: Enumerate Instances with Content-Language match"  << endl; 	

          AcceptLanguages acceptLangs2;
          acceptLangs2.add(AcceptLanguageElement("de", float(0.8))); 
	  	
          client.setRequestAcceptLanguages(acceptLangs2);
        	
          Array<CIMInstance> cimNInstances2 =
                              client.enumerateInstances(NAMESPACE,  className, true,
                                                        false,  includeQualifiers,
                                                        includeClassOrigin);
			
          if (verboseTest)	      
              cout << "Found " << cimNInstances2.size() << " Instances of " << className << endl;

          MYASSERT(cimNInstances2.size() == 3);

#ifdef PEGASUS_HAS_MESSAGES	
          if (verboseTest)
              cout << "Checking expected response ContentLanguages: " << CL_DE << endl;

          MYASSERT(CL_DE == client.getResponseContentLanguages());
#else
          if (verboseTest)
              cout << "Checking expected response ContentLanguages: " << CL_EN << endl;

          MYASSERT(CL_EN == client.getResponseContentLanguages());
#endif	

          //
          //  TEST 3 - Aggregation Test - Enumerate Instances with deep inheritance, where
          //  the instances do not have the same language.  Since the languages of the instances
          //  do not match, we expect the ContentLanguage of the whole response to be empty.
          //  This will test that the languages of the instances are aggregated
          //  into the Content-Language of the whole response.  
          //
          //  We will modify the language of the sub-class instance, and then do the enumerate.
          //
          //  (tests ContentLanguage aggregation in CIMOperationRequestDispatcher)
          //

	  cout << endl << "INSTANCE TEST 3: Enumerate Instances with Content-Language mismatch" << endl;

          //  First, find the instance of the sub-class
          Uint32 i = 0;
          for (; i < cimNInstances2.size(); i++)
          {
              if (cimNInstances2[i].getClassName() == SUBCLASSNAME)
              {
                  break;
              }
          }

          MYASSERT(i != cimNInstances2.size());

          if (verboseTest)    			    
              cout << "Modifying the language of an instance of the subclass to " << CL_HOMER << endl;

          client.setRequestContentLanguages(CL_HOMER);

          client.modifyInstance(NAMESPACE,
                                  cimNInstances2[i],
                                  includeQualifiers);

          //  Enumerate the instances starting at the base class, with deep inheritance.  
          AcceptLanguages acceptLangs3;
          acceptLangs3.add(AcceptLanguageElement("x-homer", float(0.8))); 
	  	
          client.setRequestAcceptLanguages(acceptLangs3);
        	
          Array<CIMInstance> cimNInstances3 =
                              client.enumerateInstances(NAMESPACE,  className, true,
                                                        false,  includeQualifiers,
                                                        includeClassOrigin );
			
          if (verboseTest)	      
              cout << "Found " << cimNInstances3.size() << " Instances of " << className << endl;

          MYASSERT(cimNInstances3.size() == 3);

          if (verboseTest)
              cout << "Checking expected empty response ContentLanguages" << endl;

          MYASSERT(ContentLanguages::EMPTY == client.getResponseContentLanguages());

          //
          //  TEST 4 - Get Instance, check default language returned.
          //  None of our preferred languages is supported by the 
          //  provider, and there is no root message file.  Expect
          //  the compiled-in default strings from the provider.
          //	    
	    
          AcceptLanguages acceptLangs4;
          acceptLangs4.add(AcceptLanguageElement("x-martian",float(0.8)));
          acceptLangs4.add(AcceptLanguageElement("x-pig-latin", float(0.1))); 
          acceptLangs4.add(AcceptLanguageElement("x-men", float(0.4)));  	  	
          client.setRequestAcceptLanguages(acceptLangs4);	    

          cout << endl << "INSTANCE TEST 4: Get Instance with AcceptLanguages = " << acceptLangs4 << endl;	        		    

          //  Find an instance of the base-class from the first enumerate
          Uint32 j = 0;
          for (; j < cimNInstances.size(); j++)
          {
              if (cimNInstances[j].getClassName() == CLASSNAME)
              {
                  break;
              }
          }

          MYASSERT(j != cimNInstances.size());
		
          // Get the instance.  Note, baseInstance is used later
          CIMInstance baseInstance = client.getInstance(
				NAMESPACE,
				cimNInstances[j].buildPath(sampleClass),
				localOnly,
				includeQualifiers,
				includeClassOrigin);

          String returnedString;
          Char16 returnedChar16;

          baseInstance.getProperty (baseInstance.findProperty(RBPROP)).
                getValue().
                get(returnedString);

          if (verboseTest)
             cout << "Checking for default language returned" << endl;

          MYASSERT(expectedDftString == returnedString); 
		
          //
          //  TEST 5 - Get Instance, check fr language returned.
          //  fr is the lowest preferred language from us, but
          //  is the only one in the list supported by the provider.
          //	   		
	  	
          AcceptLanguages acceptLangs5;
          acceptLangs5.add(AcceptLanguageElement("x-martian", float(0.8)));
          acceptLangs5.add(AcceptLanguageElement("fr", float(0.1))); 
          acceptLangs5.add(AcceptLanguageElement("x-men", float(0.4)));  	  	
          client.setRequestAcceptLanguages(acceptLangs5);	  		
		
          cout << endl << "INSTANCE TEST 5: Get Instance with AcceptLanguages = " << acceptLangs5 << endl;	        		    		
		
          CIMInstance instance5 = client.getInstance(
				NAMESPACE,
				cimNInstances[1].buildPath(sampleClass),
				localOnly,
				includeQualifiers,
				includeClassOrigin);

          instance5.getProperty (instance5.findProperty(RBPROP)).
               getValue().
               get(returnedString);		

#ifdef PEGASUS_HAS_MESSAGES	
          if (verboseTest)	
                cout << "Checking for fr returned" << endl;
	
          MYASSERT(CL_FR == client.getResponseContentLanguages());
          MYASSERT(expectedFRString == returnedString); 		
#else
          if (verboseTest)
                cout << "Checking for default language returned" << endl;

          MYASSERT(CL_EN == client.getResponseContentLanguages());
          MYASSERT(expectedDftString == returnedString); 		
#endif			 
	
          //
          //  TEST 6 - Round trip Test. 
          //  Create Instance followed by Get Instance.
          //
          //  Create with an fr string and expect that
          //  the fr string is preserved on get instance.
          //  (fr is one of the supported languages of the provider)
          //
          //  Create with a UTF-16 string and char16 and expect that they
          //  are preserved on get instance
          //	  	
	
          String oui = "Oui";

          CIMInstance frInstance(CLASSNAME);
          frInstance.addProperty(CIMProperty(	CIMName(CLPROP), oui));
          frInstance.addProperty(CIMProperty(	CIMName(ROUNDTRIPSTRINGPROP), String(utf16Chars)));
          frInstance.addProperty(CIMProperty(	CIMName(ROUNDTRIPCHARPROP), utf16Chars[0]));
          frInstance.addProperty(CIMProperty(	CIMName(IDPROP), Uint8(3)));				

          CIMObjectPath frInstanceName = frInstance.buildPath(sampleClass);
          frInstance.setPath(frInstanceName);
	    
          client.setRequestContentLanguages(CL_FR);
	    
          cout << endl << "INSTANCE TEST 6: Create Instance with ContentLanguages = " << CL_FR
                       << "  and UTF-16 string." << endl;	
        		
          if (verboseTest)    			    
              cout << "Creating the instance" << endl;		    

          client.createInstance(NAMESPACE, frInstance);
	
          AcceptLanguages acceptLangs6;
          acceptLangs6.add(AcceptLanguageElement("x-martian", float(0.8)));
          acceptLangs6.add(AcceptLanguageElement("fr", float(0.1))); 
          acceptLangs6.add(AcceptLanguageElement("x-men", float(0.4)));  	  	
          client.setRequestAcceptLanguages(acceptLangs6);		

          if (verboseTest)
                cout << "Getting the instance just created, using AcceptLanguages = " << acceptLangs6
 		     << endl;	   	
    						
          CIMInstance instance6 = client.getInstance(
                  NAMESPACE,
                  frInstanceName,
                  localOnly,
                  includeQualifiers,
                  includeClassOrigin);	    						

          instance6.getProperty (instance6.findProperty(ROUNDTRIPSTRINGPROP)).
                  getValue().
                  get(returnedString);

          instance6.getProperty (instance6.findProperty(ROUNDTRIPCHARPROP)).
                  getValue().
                  get(returnedChar16);

          if (verboseTest)
                  cout << "Checking the UTF-16 chars were preserved" << endl;	 

          MYASSERT(expectedUTF16String == returnedString);
          MYASSERT(utf16Chars[0] == returnedChar16);  
		
          instance6.getProperty (	instance6.findProperty(CLPROP)).
                  getValue().
                  get(returnedString);

          if (verboseTest)
                  cout << "Checking for fr returned" << endl;

          MYASSERT(oui == returnedString); 
          MYASSERT(CL_FR == client.getResponseContentLanguages());

          // Delete the instance for the next pass
          if (verboseTest)
               cout << "Deleting the instance" << endl;

          client.deleteInstance(
                NAMESPACE,
                frInstanceName);		
	        
          //
          //  TEST 7 - Round trip Test. 
          //  Modify Instance followed by Get Instance.
          //
          //  Modify an instance of the base class with an x-homer language
          //  string and expect that the x-homer string is preserved on get
          //  instance.
          //  (x-homer is not one of the supported languages of the provider
          //   but still should be returned when asked for)
          //
          //  Modify baseInstance with a UTF-16 string and char16 and expect that 
          //  they are preserved on get instance
          //	  	        
	        	
          String doh = "doh!";
          baseInstance.removeProperty (baseInstance.findProperty(CLPROP));
          baseInstance.addProperty(CIMProperty(CIMName(CLPROP), doh));

          baseInstance.removeProperty (baseInstance.findProperty(ROUNDTRIPSTRINGPROP));		
          baseInstance.addProperty(CIMProperty(CIMName(ROUNDTRIPSTRINGPROP), String(utf16Chars)));	

          baseInstance.removeProperty (baseInstance.findProperty(ROUNDTRIPCHARPROP));		
          baseInstance.addProperty(CIMProperty(CIMName(ROUNDTRIPCHARPROP),utf16Chars[0]));			
		
          client.setRequestContentLanguages(CL_HOMER);
	    
          cout << endl << "INSTANCE TEST 7: Modify Instance with ContentLanguages = " << 
                 CL_HOMER << "  and UTF-16 string." << endl;	   	    
	    
          if (verboseTest)
                 cout << "Modifying the instance" << endl;

          client.modifyInstance(NAMESPACE,
                                  baseInstance,
                                  includeQualifiers);
	    			
          AcceptLanguages acceptLangs7;
          acceptLangs7.add(AcceptLanguageElement("x-homer", float(0.8)));		
          client.setRequestAcceptLanguages(acceptLangs7);
	    
          if (verboseTest)
                cout << "Getting the instance just modified, using AcceptLanguages = " << acceptLangs7
                     << endl;	 	    
	    	
          CIMInstance instance7 = client.getInstance(
                                                       NAMESPACE,
                                                       baseInstance.buildPath(sampleClass),
                                                       localOnly,
                                                       includeQualifiers,
                                                       includeClassOrigin);
					 
          instance7.getProperty (
                instance7.findProperty(ROUNDTRIPSTRINGPROP)).
                getValue().
                get(returnedString);

          instance7.getProperty (
                instance7.findProperty(ROUNDTRIPCHARPROP)).
                getValue().
                get(returnedChar16);
        	  
          if (verboseTest)     		
                cout << "Checking the UTF-16 chars were preserved" << endl;    		
        	       						
          MYASSERT(expectedUTF16String == returnedString);
          MYASSERT(utf16Chars[0] == returnedChar16);
		
          instance7.getProperty (
                instance7.findProperty(CLPROP)).
                getValue().
                get(returnedString);
        	
          if (verboseTest)
                cout << "Checking for x-homer returned" << endl;	
         	    
          MYASSERT(returnedString == doh); 
          MYASSERT(CL_HOMER == client.getResponseContentLanguages());	
		
          //
          //  TEST 8 - Exception test. 
          //  Expect an exception to be returned from the server
          //  with a localized message in es locale.
          //
          //  Note: the provider will throw a not-supported exception
          //  on delete instance requests.
          //
	  	
          AcceptLanguages acceptLangs8;
          acceptLangs8.add(AcceptLanguageElement("es", float(1.0)));
          acceptLangs8.add(AcceptLanguageElement("fr", float(0.9))); 
          client.setRequestAcceptLanguages(acceptLangs8);			  	
	  	
          cout << endl << "INSTANCE TEST 8: Delete Instance with AcceptLanguages = " << 
                            acceptLangs8 << endl;	   	  	
	  	
          // Try to delete baseInstance, expect an exception
          if (verboseTest)
                cout << "Deleting the instance, expecting to get a CIMException" << endl;

          Boolean gotException = false;
          try 
          {
              client.deleteInstance(
                                    NAMESPACE,
                                    baseInstance.buildPath(sampleClass)); 	  	
          } catch (CIMException & ce)
          {
              if (verboseTest)
                  cout << "Got the CIMException" << endl;
              String message = ce.getMessage();	
	  		
#ifdef PEGASUS_HAS_MESSAGES
              if (verboseTest)		
                  cout << "Checking for es returned in the exception" << endl;	
              MYASSERT(CL_ES == client.getResponseContentLanguages());
              Uint32 n = message.find("ES");
              MYASSERT(n != PEG_NOT_FOUND);			
#else
              if (verboseTest)
                  cout << "Checking for default language returned in the exception" << endl;
              MYASSERT(CL_Dft == client.getResponseContentLanguages());
              Uint32 n = message.find("default");
              MYASSERT(n != PEG_NOT_FOUND);				
#endif		  		
              gotException = true;	
          }		
  		
          MYASSERT(gotException == true);

        }  // end for
    }
    catch(Exception& e)
    {
      PEGASUS_STD(cerr) << "Error in TestLocalizedInstances: " << e.getMessage() << PEGASUS_STD(endl);
      throw e;      
    }
}
// l10n end


///////////////////////////////////////////////////////////////
//    OPTION MANAGEMENT
///////////////////////////////////////////////////////////////

/** GetOptions function - This function defines the Options Table
    and sets up the options from that table using the option manager.
    const char* optionName;
    const char* defaultValue;
    int required;
    Option::Type type;
    char** domain;
    Uint32 domainSize;
    const char* commandLineOptionName;
    const char* optionHelpMessage;

*/
void GetOptions(
    OptionManager& om,
    int& argc,
    char** argv,
    const String& pegasusHome)
{
    static struct OptionRow optionsTable[] =
        //     optionname defaultvalue rqd  type domain domainsize clname hlpmsg
    {

		 {"repeat", "1", false, Option::WHOLE_NUMBER, 0, 0, "r",
		 		       "Specifies a Repeat Count Entire test repeated this many times" },

		 {"version", "false", false, Option::BOOLEAN, 0, 0, "version",
		 		 		 "Displays TestClient Version "},

		 {"verbose", "false", false, Option::BOOLEAN, 0, 0, "verbose",
		 		 		 "If set, outputs extra information "},

		 {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
		 		     "Prints help message with command line options "},
		 		     
		 {"debug", "false", false, Option::BOOLEAN, 0, 0, "d",
		              "Not Used "},

		 {"local", "false", false, Option::BOOLEAN, 0, 0, "local",
		 		 		 "Use local connection mechanism"},
		 {"user", "", false, Option::STRING, 0, 0, "user",
		 		 		 "Specifies user name" },

		 {"password", "", false, Option::STRING, 0, 0, "password",
		 		 		 "Specifies password" }

    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    String configFile = pegasusHome + "/cimserver.conf";

    cout << "Config file from " << configFile << endl;

    if (FileSystem::exists(configFile))
		 om.mergeFile(configFile);

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();
}


///////////////////////////////////////////////////////////////
//    MAIN
///////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

  // char connection[50] = "localhost:5988";
  char *address_string = NULL;

    Uint32 repetitions = 1;

    // Get environment variables:

    String pegasusHome;
    pegasusHome = "/";
    // GetEnvironmentVariables(argv[0], pegasusHome);

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    OptionManager om;

    try
    {
		 GetOptions(om, argc, argv, pegasusHome);
    }
    catch (Exception& e)
    {
		 cerr << argv[0] << ": " << e.getMessage() << endl;
		 exit(1);
    }

    // Check to see if user asked for help (-h otpion):
    if (om.valueEquals("help", "true"))
    {
                String header = "Usage ";
                header.append(argv[0]);
                header.append(" -parameters host [host]");

                String trailer = "Assumes localhost:5988 if host not specified";
                trailer.append("\nHost may be of the form name or name:port");
                trailer.append("\nPort 5988 assumed if port number missing.");
                om.printOptionsHelpTxt(header, trailer);

		 exit(0);
    }
    if (om.valueEquals("version","true"))
    {
        cout << argv[0] << " version " <<programVersion <<  endl;
        //cout << argv[0] << endl;
        exit(0);
    }

	String userName;
    om.lookupValue("user", userName);
    if (userName != String::EMPTY)
    {
       cout << "Username = " << userName << endl;
    }
    
    Boolean verboseTest = om.isTrue("verbose");

    String password;
    om.lookupValue("password", password);
    if (password != String::EMPTY)
    {
       cout << "password = " << password << endl;
    }

	// Set up number of test repetitions.  Will repeat entire test this number of times
	// Default is zero
	String repeats;
	Uint32 repeatTestCount = 0;
	/* ATTN: KS P0 Test and fix function added to Option Manager
	*/
	if (!om.lookupIntegerValue("repeat", repeatTestCount))
	    repeatTestCount = 1;
	
    if(verboseTest)
		cout << "Test repeat count " << repeatTestCount << endl;

    Boolean localConnection = (om.valueEquals("local", "true"))? true: false;
    cout << "localConnection " << (localConnection ? "true" : "false") << endl;

    Array<String> connectionList;
    if (argc > 1 && !localConnection)
		 for (Sint32 i = 1; i < argc; i++)
		     connectionList.append(argv[i]);

	 if(argc < 2)
      connectionList.append("localhost:5988");

    // Expand host to add port if not defined

	// Show the connectionlist
    cout << "Connection List size " << connectionList.size() << endl;
    for (Uint32 i = 0; i < connectionList.size(); i++)
	cout << "Connection " << i << " address " << connectionList[i] << endl;

    cout << "==========START GLOBALIZATION TEST============" << endl;
#ifdef PEGASUS_HAS_MESSAGES	
	cout << "NOTE: Pegasus supports ICU resource bundles.  Expecting the provider to return translated strings loaded from resource bundles." << endl;  
#else
	cout << "NOTE: Pegasus does not support ICU resource bundles.  Expecting the provider to return untranslated hardcoded strings." << endl;
#endif

    for(Uint32 numTests = 1; numTests <= repeatTestCount; numTests++)
	{
		cout << "Test Repetition # " << numTests << endl;
		for (Uint32 i = 0; i < connectionList.size(); i++)
		{
			cout << "Start Try Block" << endl;
		  try
		  {
		     cout << "Set Stopwatch" << endl;
		     Stopwatch elapsedTime;
		     cout << "Create client" << endl;
		     CIMClient client;
		     client.setTimeout(360 * 1000);
		     cout << "Client created" << endl;

             //
             //  Get host and port number from connection list entry
             //
             Uint32 index = connectionList[i].find (':');
             String host = connectionList[i].subString (0, index);
             Uint32 portNumber = 0;
             if (index != PEG_NOT_FOUND)
             {
	             String portStr = connectionList[i].subString 
                             (index + 1, connectionList[i].size ());
	               sscanf (portStr.getCString (), "%u", &portNumber);
             }


			 if (om.isTrue("local"))
			 {
			   	 cout << "Using local connection mechanism " << endl;
     		     client.connectLocal();
			 }
			 else
			 {
			    cout << "Connecting to " << connectionList[i] << endl;
                client.connect (host, portNumber,
                                   userName, password);
			 }
			 cout << "Client Connected" << endl;

			 testStart("Test Instance Operations");
			 elapsedTime.reset();
			 TestLocalizedInstances(client, verboseTest);
			 testEnd(elapsedTime.getElapsed());

			 testStart("Test Method Operations");
			 elapsedTime.reset();
			 TestLocalizedMethods(client, verboseTest);
			 testEnd(elapsedTime.getElapsed());

			 testStart("Test Class and Qualifier Operations");
			 elapsedTime.reset();
			 TestUTFRepository(client, verboseTest);
			 testEnd(elapsedTime.getElapsed());

			 client.disconnect();
		  }
		  catch(Exception& e)
		  {
			   PEGASUS_STD(cerr) << "Error: " << e.getMessage() <<
			     PEGASUS_STD(endl);
			   exit(1);
		  }
		}
	}
	
    PEGASUS_STD(cout) << "+++++ "<< argv[0] << " Terminated Normally" << PEGASUS_STD(endl);
    return 0;
}


