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
// Author: Chuck Carmack (carmack@us.ibm.com)
//
// Modified By:
//      Amit K Arora, IBM (amita@in.ibm.com) for Bug#1090
//      Chip Vincent (cvincent@us.ibm.com)
//      David Dillard, VERITAS Software Corp.
//          (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

// This is a sample provider that test globalization support, including unicode
// support and localized message loads.

// Provider Types Tested:
//
// Instance
// Method
// Indication (just the simple case of an indication with a unicode property and
// a language tag)
// IndicationConsumer

// Testcases:
//
// 1) Round Trip Test: Verifies that a string and char16 containing Unicode
// chars has no characters lost on a round trip.  The client sends a Unicode
// string and char16 in properties for createInstance and modifyInstance.
// We check these against an expected string and char16. If it miscompares then
// some Unicode chars were lost.  The Unicode string and char16 are then saved along
// with the instance sent by the client. When the client does a getInstance it can
// compare the string and char16 to an expected values on the client side.
//
// Test Properties: RoundTripString, RoundTripChar
//
// 2) Resource Bundle Test:  One of the properties on the instances
// returned is a read-only string property where the string is loaded
// from a resource bundle.  The language of the string is determined
// the preferred languages sent by the client in the AcceptLanguages
// header.
//
// Test Property: ResourceBundleString
//
// 3) Content Languages Test.  This tests that the client can set a
// r/w string and associate a content language with that string.  The client
// should be able to retrieve that string with the same associated content
// language tag.
//
// Test Property: ContentLanguageString
//
//
// OVERALL DESIGN NOTE:
//
// We have two localized properties (ResourceBundleString
// and ContentLanguageString).  The first is r/o and the second is r/w.
// The client can set ContentLanguageString with an associated content
// language tag.  The last content language tag set by the client for this
// property is stored in instanceLangs[].
//
// However, when the client gets an instance, the content language in the response
// applies to the whole instance.  So, to decide what language to set for returned
// instance, we do the following:
//
// -- If the client had previously set a content language for
// ContentLanguageString, then we will return that string with the
// content language previously set by the client.  We will try to
// load the ResourceBundleString from the resource bundle in that
// same content language.  If that content language is not supported by
// a resource bundle, this will result in a default string being
// returned to the client in ResourceBundleString.
//
// -- If the client had not previously set a content language for
// ContentLanguageString, or the last set was empty, then we will
// load both properties from the resource bundle based on the
// AcceptLanguages that was set into our thread by Pegasus (this
// is the same as the AcceptLanguages requested by the client)


// l10n TODO
// -- implement test providers for other provider types

#include "LocalizedProvider.h"
#include <cassert>
#include <cstdlib>

// Globalization headers
#include <Pegasus/Common/AcceptLanguages.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>

PEGASUS_USING_STD;

PEGASUS_USING_PEGASUS;

// Class Hierarchy:
// Sample_LocalizedProviderClass is a subclass of Sample_ProviderClass
// Sample_LocalizedProviderSubClass is a subclass of Sample_LocalizedProviderClass

// Our namespace
#define NAMESPACE "root/SampleProvider"

// Class names and references
#define CLASSNAME "Sample_LocalizedProviderClass"
#define SUBCLASSNAME "Sample_LocalizedProviderSubClass"
#define REFERENCE1 NAMESPACE ":Sample_LocalizedProviderClass.Identifier=0"
#define REFERENCE2 NAMESPACE ":Sample_LocalizedProviderClass.Identifier=1"
#define REFERENCE3 NAMESPACE ":Sample_LocalizedProviderSubClass.Identifier=2"

// Properties on the Sample_ProviderClass
#define IDENTIFIER_PROP "Identifier"		// Key

// Properties on the Sample_LocalizedProviderClass
#define ROUNDTRIPSTRING_PROP "RoundTripString"	// Property for round trip test (R/W)
#define ROUNDTRIPCHAR_PROP "RoundTripChar"	// Property for round trip test (R/W)
#define RB_STRING_PROP "ResourceBundleString" // Property for resource bundle test (R/O)
#define CONTENTLANG_PROP "ContentLanguageString" // Property from content language test (R/W)

// Properties on the Sample_LocalizedProviderSubClass
#define TESTSTRING_PROP "TestString"

// The name of the *root* resource bundle.
#ifndef PEGASUS_OS_OS400
// Note: we are using a relative path to $PEGASUS_HOME
#define RESOURCEBUNDLE "provider/localizedProvider/localizedProvider"
#else
// Absolute path on OS/400 (don't want the these resources in "proddata", which
// is the default directory on OS/400)
#define RESOURCEBUNDLE "/cimlib/localizedProvider/localizedProvider"
#endif

// Message IDs and default message strings
#define RB_STRING_MSGID "ResourceBundleString" // ID for property loaded from resource bundle
#define RB_STRING_DFT "ResourceBundleString DEFAULT"

#define CONTENTLANG_STRING_MSGID "ContentLanguageString" // ID for property loaded from resource bundle
#define CONTENTLANG_STRING_DFT "ContentLanguageString DEFAULT"

#define NOT_SUPPORTED_ERROR_MSGID "NotSupportedError" // ID for not supported error msg
#define NOT_SUPPORTED_ERROR_DFT "LocalizedProvider: Operation Not Supported (default message)"

#define ROUND_TRIP_ERROR_MSGID "RoundTripError"	 // ID for round trip error msg
#define ROUND_TRIP_ERROR_DFT "LocalizedProvider: Received bad value for round trip string (default message)"
#define ROUND_TRIP_STRING_DFT "RoundTripString DEFAULT"

// Expected round-trip string from the client.
// Note: the dbc0/dc01 pair are UTF-16 surrogates
static const Char16 roundTripChars[] =
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


// Another UTF-16 string used for testing
// Note: the first 3 chars are taken from section 3.3.2 of the CIM-over-HTTP spec.
// The next 2 chars are a UTF-16 surrogate pair
Char16 hangugo[] = {0xD55C, 0xAD6D, 0xC5B4,
			0xdbc0,
			0xdc01,
'g','l','o','b','a','l',
			0x00};

// Serializes access to the instances arrays during the CIM requests
Mutex mutex;

// Serializes access to the CIMOMHandle
Mutex mutexCH;

// CIMOMHandle we got on initialize
CIMOMHandle _cimom;

// Vars for the indication provider
static IndicationResponseHandler * _handler = 0;
static Boolean _enabled = false;
static Uint32 _nextUID = 0;

// Vars for the indication consumer provider
static Uint8 consumerStatus = 1;

// Constructor - initializes parameters for the MessageLoader that won't
// be changing.
LocalizedProvider::LocalizedProvider() :
	msgParms(RB_STRING_MSGID, RB_STRING_DFT),
	notSupportedErrorParms(NOT_SUPPORTED_ERROR_MSGID, NOT_SUPPORTED_ERROR_DFT),
	contentLangParms(CONTENTLANG_STRING_MSGID, CONTENTLANG_STRING_DFT),
	roundTripErrorParms(ROUND_TRIP_ERROR_MSGID, ROUND_TRIP_ERROR_DFT),
	roundTripString(roundTripChars)
{
	msgParms.msg_src_path = RESOURCEBUNDLE;
	contentLangParms.msg_src_path = RESOURCEBUNDLE;
	notSupportedErrorParms.msg_src_path = RESOURCEBUNDLE;
	roundTripErrorParms.msg_src_path = RESOURCEBUNDLE;
}

LocalizedProvider::~LocalizedProvider(void)
{
}

void LocalizedProvider::initialize(CIMOMHandle & cimom)
{
   AutoMutex autoMut(mutex);

   // Save away the CIMOMHandle
   _cimom = cimom;

   // create default instances

   // Instance 1 - Sample_LocalizedProviderClass
   CIMInstance instance1(CLASSNAME);
   CIMObjectPath reference1(REFERENCE1);

   instance1.addProperty(CIMProperty(IDENTIFIER_PROP, Uint8(0)));
   instance1.addProperty(CIMProperty(ROUNDTRIPSTRING_PROP, String(roundTripChars)));
   instance1.addProperty(CIMProperty(ROUNDTRIPCHAR_PROP, roundTripChars[0]));
   instance1.setPath(reference1);

   _instances.append(instance1);
   _instanceNames.append(reference1);
   _instanceLangs.append(ContentLanguages::EMPTY);

   // Instance 2 - Sample_LocalizedProviderClass
   CIMInstance instance2(CLASSNAME);
   CIMObjectPath reference2(REFERENCE2);

   instance2.addProperty(CIMProperty(IDENTIFIER_PROP, Uint8(1)));
   instance2.addProperty(CIMProperty(ROUNDTRIPSTRING_PROP, String(roundTripChars)));
   instance2.addProperty(CIMProperty(ROUNDTRIPCHAR_PROP, roundTripChars[0]));
   instance2.setPath(reference2);

   _instances.append(instance2);
   _instanceNames.append(reference2);
   _instanceLangs.append(ContentLanguages::EMPTY);

   // Instance 3 - Sample_LocalizedProviderSubClass
   CIMInstance instance3(SUBCLASSNAME);
   CIMObjectPath reference3(REFERENCE3);

   instance3.addProperty(CIMProperty(IDENTIFIER_PROP, Uint8(2)));
   instance3.addProperty(CIMProperty(ROUNDTRIPSTRING_PROP, String(roundTripChars)));
   instance3.addProperty(CIMProperty(ROUNDTRIPCHAR_PROP, roundTripChars[0]));
   instance3.setPath(reference3);

   _instances.append(instance3);
   _instanceNames.append(reference3);
   _instanceLangs.append(ContentLanguages::EMPTY);
}

void LocalizedProvider::terminate(void)
{
	delete this;
}

void LocalizedProvider::getInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
	// begin processing the request
	handler.processing();

	// Get the list of preferred languages that the client requested
	// to be returned in the response.
	AcceptLanguages clientAcceptLangs = getRequestAcceptLanguages(context);

    CIMObjectPath localReference =
        CIMObjectPath(
            String(),
            instanceReference.getNameSpace(),
            instanceReference.getClassName(),
            instanceReference.getKeyBindings());

    CIMInstance foundInstance;
    ContentLanguages foundLang;

	// instance index corresponds to reference index
    {
        AutoMutex autoMut(mutex);

        for(Uint32 i = 0, n = _instances.size(); i < n; i++)
        {
            if(localReference == _instanceNames[i])
            {
                foundInstance = _instances[i].clone();
                foundLang = ContentLanguages(_instanceLangs[i]);

                break;
            }
        }  // end for

        // Load the localized properties and figure out what content
        // language to return, based on the design mentioned above.
        ContentLanguages rtnLangs =
            _loadLocalizedProps(
                clientAcceptLangs,
                foundLang,
                foundInstance);

        // We need to tag the instance we are returning with the
        // the content language.
        _setHandlerLanguages(handler, rtnLangs);

        // deliver requested instance
        handler.deliver(foundInstance);
    }   // mutex unlocks here

    // complete processing the request
    handler.complete();
}

void LocalizedProvider::enumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
	// begin processing the request
	handler.processing();

        // The only purpose for this next section of code is to
        // test multi-threading in the MessageLoader when loading messages.
	// Note: for r2.4, MessageLoader uses ICU to load messages, so this
	// test basically tests ICU multi-threading.
        // The g11ntest client pounds on the enumerateInstances
        // request in different languages.  Running multiple
        // g11ntest clients will cause this code to execute
        // in simultaneous threads.
#ifdef PEGASUS_HAS_MESSAGES
	// Message loading (ICU) is enabled.
	String testLang;
        AcceptLanguages testAL;
	MessageLoaderParms testParms(RB_STRING_MSGID, RB_STRING_DFT);
	testParms.msg_src_path = RESOURCEBUNDLE;
	testParms.useThreadLocale = false;
	String testMsg;
	String msgLang;

	for(Uint32 q = 0; q < 10; q++)
	{
	  switch (q % 3)
	  {
	    case 0:
	      testLang = "de";
	      msgLang = "DE";
	      break;

	    case 1:
	      testLang = "fr";
	      msgLang = "FR";
	      break;

	    case 2:
	      testLang = "es";
	      msgLang = "ES";
	      break;
	  }

	  testAL.clear();
	  testAL.insert(AcceptLanguageElement(testLang, float(1.0)));
	  testParms.acceptlanguages = testAL;
	  testMsg = MessageLoader::getMessage(testParms);

	  if (PEG_NOT_FOUND == testMsg.find(msgLang))
	  {
          throw CIMOperationFailedException("Error in ICU multithread test");
	  }
	}  // end for
#endif

        Array<CIMInstance> foundInstances;
        Array<ContentLanguages> foundLangs;

        // Look up the instances
        {
            AutoMutex autoMut(mutex);

            for(Uint32 i = 0, n = _instances.size(); i < n; i++)
            {
                // Since this provider is supporting 2 classes, only return instances
                // of the requested class.
                if (classReference.getClassName().equal(_instanceNames[i].getClassName()))
                {
                    foundInstances.append(_instances[i].clone());
                    foundLangs.append(ContentLanguages(_instanceLangs[i]));
                }
            }

            // Get the list of preferred languages that the client requested
            // to be returned in the response.
            AcceptLanguages clientAcceptLangs = getRequestAcceptLanguages(context);

	    ContentLanguages aggregatedLangs = ContentLanguages::EMPTY;
	    Boolean langMismatch = false;
            Boolean firstInstance = true;

            for(Uint32 j = 0, k = foundInstances.size(); j < k; j++)
            {
               // Load the localized properties and figure out what content
               // language to return for this instance, based on the design
               // mentioned above.
               ContentLanguages rtnLangs = _loadLocalizedProps(clientAcceptLangs,
                                                           foundLangs[j],
                                                           foundInstances[j]);

               // Since we are returning more than one instance, and the
               // content language we are returning applies to all the instances,
               // we need to 'aggregate' the languages of all the instances.
               // If all the instances have the same language, then return
               // that language in the content language to the client.  If there
               // is a language mismatch in the instances, then do not return
               // any ContentLanguages to the client.
               if (firstInstance)
               {
                   // Set the aggregated content language to the first instance lang.
                   aggregatedLangs = rtnLangs;
                   firstInstance = false;
               }
               else if (langMismatch == false && rtnLangs != aggregatedLangs)
               {
                   // A mismatch was found.  Set the aggegrated content lang to empty
                   langMismatch = true;
                   aggregatedLangs = ContentLanguages::EMPTY;
               }

               // deliver instance
               CIMObjectPath localReference = buildRefFromInstance(foundInstances[j]);
               foundInstances[j].setPath(localReference);

               handler.deliver(foundInstances[j]);
            }  // end for

            // Set the aggregated content language into the response
            _setHandlerLanguages(handler, aggregatedLangs);
        } // mutex unlocks here

	// complete processing the request
	handler.complete();
}

void LocalizedProvider::enumerateInstanceNames(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    ObjectPathResponseHandler & handler)
{
	// Not doing any localization here

	// begin processing the request
	handler.processing();

    {
       AutoMutex autoMut(mutex);

       for(Uint32 i = 0, n = _instances.size(); i < n; i++)
       {
           if(classReference.getClassName().equal(_instanceNames[i].getClassName()))
           {
               // deliver reference
               handler.deliver(_instanceNames[i]);
           }
       }
    }  // mutex unlocks here

	// complete processing the request
	handler.complete();
}

void LocalizedProvider::modifyInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance & instanceObject,
    const Boolean includeQualifiers,
    const CIMPropertyList & propertyList,
    ResponseHandler & handler)
{
    CIMObjectPath localReference =
        CIMObjectPath(
            String(),
            instanceReference.getNameSpace(),
            instanceReference.getClassName(),
            instanceReference.getKeyBindings());

    if(localReference.getKeyBindings().size() == 0)
    {
        // try to get the keys from the instance
        localReference = buildRefFromInstance(instanceObject);
    }

    if(localReference.getKeyBindings().size() == 0)
    {
        throw CIMOperationFailedException("cannot determine instance name.");
    }

	// begin processing the request
	handler.processing();

        Uint32 i = 0;

	// instance index corresponds to reference index
        {
           AutoMutex autoMut(mutex);

	   for (Uint32 n = _instances.size(); i < n; i++)
           {
                if(localReference == _instanceNames[i])
                {
                    // We expect the client to send us specific values in the
                    // round trip string and char16 properties.
                    // This is to test that the Unicode characters got to us properly.
                    // (Note: this call can throw an exception)
                    _checkRoundTripString(context, instanceObject);

                    // overwrite existing instance
                    CIMInstance modifiedInstance(instanceObject);

                    modifiedInstance.setPath(localReference);

                    _instances[i] = modifiedInstance;

                    // Get the language that the client tagged to the instance
                    // Note: the ContentLanguageString property is the only r/w string that
                    // can be tagged with a content language.  So the ContentLangauges
                    // for the instance really only applies to that property.
                    ContentLanguages contentLangs = getRequestContentLanguages(context);

                    // Save the language of the ContentLanguageString
                    _instanceLangs[i] = contentLangs;

                    break;
                }
           }  // end for

           if (i == _instanceNames.size())
	     {
              throw CIMObjectNotFoundException(instanceReference.toString());
	     }

        }  // mutex unlocks here

	// complete processing the request
	handler.complete();
}

void LocalizedProvider::createInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance & instanceObject,
    ObjectPathResponseHandler & handler)
{
    CIMObjectPath localReference =
        CIMObjectPath(
            String(),
            instanceReference.getNameSpace(),
            instanceReference.getClassName(),
            instanceReference.getKeyBindings());

    if(localReference.getKeyBindings().size() == 0)
    {
        // try to get the keys from the instance
        localReference = buildRefFromInstance(instanceObject);
    }

    if(localReference.getKeyBindings().size() == 0)
    {
        throw CIMOperationFailedException("cannot determine instance name.");
    }

    {
        AutoMutex autoMut(mutex);

        // instance index corresponds to reference index
        for(Uint32 i = 0, n = _instanceNames.size(); i < n; i++)
        {
            if(localReference == _instanceNames[i])
            {
                // Note: Since localReference is a CIMObjectPath,
                // and that is our "message" here, no need to
                // to localize.
                throw CIMObjectAlreadyExistsException(localReference.toString());
            }
        }

        // begin processing the request
        handler.processing();

        // We expect the client to send us specific values in the
        // round trip string and char16 properties.
        // This is to test that the Unicode characters got to us properly.
        // (Note: this call can throw an exception)
	   _checkRoundTripString(context, instanceObject);

       // update the object's path
       CIMInstance newInstance(instanceObject);

       newInstance.setPath(localReference);

	   // add the new instance to the array
	   _instances.append(newInstance);
	   _instanceNames.append(newInstance.getPath());

       // Get the language that the client tagged to the instance
	   // Note: the ContentLanguageString property is the only r/w string that
	   // can be tagged with a content language.  So the ContentLanguages
	   // for the instance really only applies to that property.
       ContentLanguages contentLangs = getRequestContentLanguages(context);

	   // Save the language of the ContentLanguageString
	   _instanceLangs.append(contentLangs);

	   // deliver the new instance
	   handler.deliver(_instanceNames[_instanceNames.size() - 1]);
    }  // mutex unlocks here

	// complete processing the request
	handler.complete();
}

void LocalizedProvider::deleteInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    ResponseHandler & handler)
{
	// We're not going to support this for instances 0, 1, or 2

    CIMObjectPath localReference =
        CIMObjectPath(
            String(),
            instanceReference.getNameSpace(),
            instanceReference.getClassName(),
            instanceReference.getKeyBindings());

    {
        AutoMutex autoMut(mutex);

        // instance index corresponds to reference index
        Uint32 i = 0;

        for(Uint32 n = _instanceNames.size(); i < n; i++)
        {
            if(localReference == _instanceNames[i])
            {
                if (i < 3)
                {
                    // Throw an exception with a localized message using the
	   	         	// AcceptLanguages set into our thread by Pegasus.
	   	         	// (this equals the AcceptLanguages requested by the client)
                    //  Note: the exception object will load the string for us.
                    throw CIMNotSupportedException(notSupportedErrorParms);
                }

                break;
            }
        }

        if (i == _instanceNames.size())
            throw CIMObjectNotFoundException(instanceReference.toString());

        handler.processing();

	   _instanceNames.remove(i);
	   _instanceLangs.remove(i);
	   _instances.remove(i);
    }  // mutex unlocks here

	handler.complete();
}

/*
void LocalizedProvider::associators(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & associationClass,
    const CIMName & resultClass,
    const String & role,
    const String & resultRole,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    ObjectResponseHandler & handler)
{

}

void LocalizedProvider::associatorNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & associationClass,
    const CIMName & resultClass,
    const String & role,
    const String & resultRole,
    ObjectPathResponseHandler & handler)
{

}

void LocalizedProvider::references(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    ObjectResponseHandler & handler)
{

}

void LocalizedProvider::referenceNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role,
    ObjectPathResponseHandler & handler)
{

}
*/

void LocalizedProvider::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & objectReference,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParameters,
    MethodResultResponseHandler & handler)
{
    handler.processing();

    String utf16String(roundTripChars);
    String hangugoString(hangugo);
    String expectedString(roundTripChars);
    Char16 expectedChar16 = roundTripChars[1];

    String outString(roundTripChars);
    Char16 outChar16 = roundTripChars[2];

    // Compare the AcceptLanguages from the client with the expected lang
    AcceptLanguages acceptLangs = getRequestAcceptLanguages(context);
    AcceptLanguages AL_DE;
    AL_DE.insert(AcceptLanguageElement("de", float(0.8)));
    if (acceptLangs != AL_DE)
    {
        throw CIMOperationFailedException(acceptLangs.toString());
    }

    // Compare the ContentLanguages from the client with the expected lang
    ContentLanguages contentLangs = getRequestContentLanguages(context);
    ContentLanguages CL_DE("de");
    if (contentLangs != CL_DE)
    {
        throw CIMOperationFailedException(contentLangs.toString());
    }

    // Set the ContentLanguages in the response.  This is just to test
    // that the language tag is passed to the client.
    _setHandlerLanguages(handler, CL_DE);

    if (objectReference.getClassName().equal (CLASSNAME))
    {
	if (methodName.equal ("UTFMethod"))
	{
            // The method was called to test UTF-16 support for input
            // parameters, output parameters, and return value.
	    if( inParameters.size() == 2 )
            {
                String inString;
                Char16 inChar16;

                // Verify that we got the expected UTF-16 chars
                // in the input parameters
	        CIMValue paramVal = inParameters[0].getValue();
                paramVal.get( inString );
 	        if( inString != expectedString )
		{
	            throw CIMInvalidParameterException(roundTripErrorParms);
		}

	        paramVal = inParameters[1].getValue();
                paramVal.get( inChar16 );
 	        if( inChar16 != expectedChar16 )
		{
	            throw CIMInvalidParameterException(roundTripErrorParms);
		}


		// Test CIMOMHandle localization here, for no other reason than this is a good
		// spot for it
		_testCIMOMHandle();

                // Return the UTF-16 chars in the output parameters
                handler.deliverParamValue(
                          CIMParamValue( "outStr",
                                         CIMValue(outString) ) );

                handler.deliverParamValue(
                          CIMParamValue( "outChar16",
                                         CIMValue(outChar16) ) );

                // Return UTF-16 chars in the return string
                handler.deliver( CIMValue( outString ) );
	    }
            else
            {
                throw CIMException(CIM_ERR_FAILED);
            }
	}
        else if (methodName.equal (hangugoString))
        {
            // The method was called that has UTF-16 chars in
            // in the method name.  The purpose of this test
            // is to verify the URI encoding/decoding of UTF-8
            // in the CIMMethod HTTP header.

            // Return UTF-16 chars in the return string
            handler.deliver( CIMValue( hangugoString ) );
        }
	//
	// Methods called by the indication tests
	//
	else if (methodName.equal ("generateIndication"))
	{
	  // This method is used to generate an indication so that the
	  // globalization support can be tested for indications.
	  _generateIndication();

	  handler.deliver(CIMValue((Uint16)1));
	}
	else if (methodName.equal ("getConsumerStatus"))
	{
	  // This method is used to get the status of the
	  // indication consumer provider
	  handler.deliver(CIMValue(consumerStatus));
	}
	else if (methodName.equal ("setDefaultMessageLoading"))
	{
	  // This method is used to change the _useDefaultMsg variable in
	  // the MessageLoader, so that messages can be loaded from the
	  // resource bundles even though $PEGASUS_USE_DEFAULT_MESSAGES is
	  // set. When ICU is used, $PEGASUS_USE_DEFAULT_MESSAGES is set
	  // during make poststarttests so that default messages are returned
	  // by the server and the wbemexec tests can pass (these expect the default
	  // messages).  However, when g11ntest is run, we want messages to be
	  // loaded from the bundles if ICU is enabled.
	  Boolean newSetting;
	  CIMValue newVal = inParameters[0].getValue();
	  newVal.get( newSetting );

          Boolean oldSetting = MessageLoader::_useDefaultMsg;
	  MessageLoader::_useDefaultMsg = newSetting;

	  handler.deliver(CIMValue((Boolean)oldSetting));
	}
        else
        {
            throw CIMMethodNotFoundException(methodName.getString());
        }
   }
   else
   {
       throw CIMObjectNotFoundException(objectReference.getClassName().getString());
   }

   handler.complete();
}

/*
void LocalizedProvider::executeQuery(
    const OperationContext & context,
    const CIMNamespaceName & nameSpace,
    const String & queryLanguage,
    const String & query,
    ObjectResponseHandler & handler)
{

}
*/

void LocalizedProvider::enableIndications(IndicationResponseHandler & handler)
{
  _enabled = true;
  _handler = &handler;
}

void LocalizedProvider::disableIndications(void)
{
  _enabled = false;
  _handler->complete();
}

void LocalizedProvider::createSubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
  // Don't do anything globalization stuff for this now.
  // This can get complicated (see PEP 58)
}

void LocalizedProvider::modifySubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
  // Don't do anything globalization stuff for this now.
  // This can get complicated (see PEP 58)
}

void LocalizedProvider::deleteSubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames)
{
  // Don't do anything globalization stuff for this now.
  // This can get complicated (see PEP 58)
}


// CIMIndicationConsumerProvider interface
void LocalizedProvider::consumeIndication(const OperationContext& context,
					const String & url,
					const CIMInstance& indication)
{
  // Verify that the utf-16 string and character values got here
  Uint32 pos = indication.findProperty("UnicodeStr");
  if (pos == (Uint32)PEG_NOT_FOUND)
  {
    consumerStatus = 2;
    return;
  }

  CIMValue val = indication.getProperty(pos).getValue();
  String utf16;
  val.get(utf16);
  if (utf16 != String(roundTripChars))
  {
    consumerStatus = 3;
    return;
  }

  pos = indication.findProperty("UnicodeChar");
  if (pos == PEG_NOT_FOUND)
  {
    consumerStatus = 4;
    return;
  }

  val = indication.getProperty(pos).getValue();
  Char16 char16;
  val.get(char16);
  if (char16 != roundTripChars[0])
  {
    consumerStatus = 5;
    return;
  }

  // Verify that the Content-Language of the indication here
  ContentLanguages expectedCL("x-world");

  ContentLanguageListContainer cntr =
    context.get(ContentLanguageListContainer::NAME);
  ContentLanguages cl = cntr.getLanguages();
  if (cl != expectedCL)
  {
    consumerStatus = 6;
    return;
  }

  // Set the consumer status to success
  consumerStatus = 0;
}


void LocalizedProvider::_checkRoundTripString(const OperationContext & context,
					      const CIMInstance& instanceObject)
{
    // Get the round trip string sent by the client
    String roundTripStringProp;
    instanceObject.getProperty(instanceObject.findProperty(ROUNDTRIPSTRING_PROP)).
            getValue().
            get(roundTripStringProp);

    // Get the round trip char16 sent by the client
    Char16 roundTripCharProp;
    instanceObject.getProperty(instanceObject.findProperty(ROUNDTRIPCHAR_PROP)).
            getValue().
            get(roundTripCharProp);

    // Now compare the string and char16 from the client to the ones we expect
    // This checks that Unicode chars were not lost
    if ((roundTripString != roundTripStringProp) ||
        (roundTripChars[0] != roundTripCharProp))
    {
	// A miscompare.  The Unicode was not preserved from the
	// client, through Pegasus, down to us.

	// Throw an exception with a localized message using the
	// AcceptLanguages set into our thread by Pegasus.
	// (this equals the AcceptLanguages requested by the client)
	//  Note: the exception object will load the string for us.
	throw CIMInvalidParameterException(roundTripErrorParms);
    }
}

AcceptLanguages LocalizedProvider::getRequestAcceptLanguages(const OperationContext & context)
{
	// Get the client's list of preferred languages for the response
	AcceptLanguageListContainer al_container =
		(AcceptLanguageListContainer)context.get(AcceptLanguageListContainer::NAME);
	return al_container.getLanguages();
}


ContentLanguages LocalizedProvider::getRequestContentLanguages(	const OperationContext & context)
{
    // Get the language that the client sent in the request
    ContentLanguageListContainer cl_container =
		(ContentLanguageListContainer)context.get(ContentLanguageListContainer::NAME);
    return cl_container.getLanguages();
}

CIMObjectPath LocalizedProvider::buildRefFromInstance(const CIMInstance& instanceObject)
{
    CIMObjectPath cimObjectPath =
        CIMObjectPath(
            String(),
            NAMESPACE,
            instanceObject.getClassName());

    Array<CIMKeyBinding> keys;

    Uint32 pos = instanceObject.findProperty(IDENTIFIER_PROP);

    if(pos != PEG_NOT_FOUND)
    {
        CIMConstProperty cimProperty = instanceObject.getProperty(pos);

        keys.append(CIMKeyBinding(cimProperty.getName(), cimProperty.getValue()));
    }
    else
    {
        throw CIMPropertyNotFoundException(IDENTIFIER_PROP);
    }

    cimObjectPath.setKeyBindings(keys);

    return(cimObjectPath);
}

ContentLanguages LocalizedProvider::_loadLocalizedProps(
                                             AcceptLanguages & acceptLangs,
                                             ContentLanguages & contentLangs,
                                             CIMInstance & instance)
{
	// The content languages to be returned to the client
	ContentLanguages rtnLangs = ContentLanguages::EMPTY;

	// Attempt to match one of the accept languages from the client with
	// the last content language saved for ContentLanguageString
	// (Note:  Using the AcceptLanguages iterator allows us
	// to scan the client's accept languages in preferred order)
	AcceptLanguageElement ale = AcceptLanguageElement::EMPTY;
	ContentLanguageElement cle = ContentLanguageElement::EMPTY;

	Boolean matchFound = false;
	acceptLangs.itrStart();
	while((ale = acceptLangs.itrNext()) != AcceptLanguageElement::EMPTY)
	{
		contentLangs.itrStart();
		while((cle = contentLangs.itrNext()) != ContentLanguageElement::EMPTY)
		{
			if (ale.getTag() == cle.getTag())
			{
				matchFound = true;
				break;
			}
		}
	}

	if (!matchFound)
	{
		// We have not found a match.
		// Load both localized properties from the resource bundle, using
		// the AcceptLanguages that was set into our thread by Pegasus.
		// (this equals the AcceptLanguages requested by the client)

		// First, load the ResourceBundleString and put the string into
		// the instance we are returning.
		rtnLangs = _addResourceBundleProp(instance);

		// Load the ContentLanguageString from the resource bundle
		// into the instance we are returning.
		ContentLanguages clPropLangs = _addContentLanguagesProp(instance);

		// The calls above returned the ContentLanguages for the language
		// of the resource bundle that was found.
		// If the two resource bundle langs don't match then
		// we have a bug in our resource bundles.
		if (rtnLangs != clPropLangs)
		{
			throw new CIMOperationFailedException(String::EMPTY);
		}

                // If we received an empty ContentLanguages then no resource bundle
                // was found.
		// In this case, set the ContentLanguages to "en".
                // Note: we are doing this for the enumerate instances aggregation test
                // when ICU is not being used.  In 'real' code, the empty
                // ContentLanguages would be returned.
                if (rtnLangs == ContentLanguages::EMPTY)
                {
                    rtnLangs = ContentLanguages("en");
                }
	}
	else
	{
		// We have found a match.  Attempt
		// to load ResourceBundleString using the matching
		// content language.
		AcceptLanguages tmpLangs;
		tmpLangs.insert(ale);
		(void)_addResourceBundleProp(tmpLangs, instance);

		// Send the matching content language back to the client.
		rtnLangs.append(cle);
	}

	return rtnLangs;
}

ContentLanguages LocalizedProvider::_addResourceBundleProp(
                                                  AcceptLanguages & acceptLangs,
                                                  CIMInstance & instance)
{
	// Get the string out of the resource bundle in one
	// of the languages requested by the caller of this function.
	msgParms.acceptlanguages = acceptLangs;

	String localizedMsgProp = MessageLoader::getMessage(msgParms);

	// Replace the string into the instance
	_replaceRBProperty(instance, localizedMsgProp);

	// The MessageLoader set the contentlanguages member
	// of msgParms to the language that it found for the message.
	return msgParms.contentlanguages;
}


ContentLanguages LocalizedProvider::_addResourceBundleProp(
                                                  CIMInstance & instance)
{
	// Get the ResourceBundleString of the resource bundle using the
	// AcceptLanguages set into our thread by Pegasus.
	// (this equals the AcceptLanguages requested by the client)

	// First, need to clear any old AcceptLanguages in the message loader
	// parms because that will override the thread's AcceptLanguages.
	msgParms.acceptlanguages = AcceptLanguages::EMPTY;
	msgParms.useThreadLocale = true;  // Don't really need to do this
					// because the default is true

	// Load the string from the resource bundle
	String localizedMsgProp = MessageLoader::getMessage(msgParms);

	// Replace the string into the instance
	_replaceRBProperty(instance, localizedMsgProp);

	// The MessageLoader set the contentlanguages member
	// of msgParms to the language that it found for the message.
	return msgParms.contentlanguages;
}


void LocalizedProvider::_replaceRBProperty(CIMInstance & instance, String newProp)
{
	// Remove the old string property
	Uint32 index = instance.findProperty(RB_STRING_PROP);
	if (index != PEG_NOT_FOUND)
	{
		instance.removeProperty(index);
	}

	// Add the localized string property to the instance
	instance.addProperty(CIMProperty(RB_STRING_PROP, newProp));
}


ContentLanguages LocalizedProvider::_addContentLanguagesProp(CIMInstance & instance)
{
	// Get the ContentLanguageString out of the resource bundle using the
	// AcceptLanguages set into our thread by Pegasus.
	// (this equals the AcceptLanguages requested by the client)

	// First, need to clear any old AcceptLanguages in the message loader
	// parms because that will override the thread's AcceptLanguages.
	contentLangParms.acceptlanguages = AcceptLanguages::EMPTY;
	contentLangParms.useThreadLocale = true;  // Don't really need to do this
						// because the default is true

	// Load the string from the resource bundle
	String localizedMsgProp = MessageLoader::getMessage(contentLangParms);

	// Remove the old string property
	Uint32 index = instance.findProperty(CONTENTLANG_PROP);
	if (index != PEG_NOT_FOUND)
	{
		instance.removeProperty(index);
	}

	// Add the localized string property to the instance
	instance.addProperty(CIMProperty(CONTENTLANG_PROP, localizedMsgProp));

	// The MessageLoader set the contentlanguages member
	// of msgParms to the language that it found for the message.
	return contentLangParms.contentlanguages;
}

void LocalizedProvider::_setHandlerLanguages(ResponseHandler & handler,
                                            ContentLanguages & langs)
{
    OperationContext context;
    context.insert(ContentLanguageListContainer(langs));
    handler.setContext(context);
}


void LocalizedProvider::_testCIMOMHandle()
{
  // This function tests the localization support in the CIMOMHandle.

  // Save away the AcceptLanguages currently in our thread
  AcceptLanguages * pal = Thread::getLanguages();
  AcceptLanguages saveAL;
  if (pal != NULL)
  {
    saveAL = *pal;
  }

  OperationContext requestCtx;

  // AcceptLanguages we are sending in the request to CIMOMHandle
  AcceptLanguages requestAL;
  requestAL.insert(AcceptLanguageElement("x-horse", float(1.0)));
  requestAL.insert(AcceptLanguageElement("x-cow", float(0.4)));
  requestAL.insert(AcceptLanguageElement("x-gekko", float(0.2)));
  requestAL.insert(AcceptLanguageElement("mi", float(0.9)));
  requestAL.insert(AcceptLanguageElement("es", float(0.8)));

  //------------------------------------------------------------
  // TEST 1
  // Do a getInstance to our instance provider, asking for the Spanish
  // instance in AcceptLanguages.  Check that the ContentLanguages
  // of the response indicates that Spanish was returned.
  // NOTE - this test uses the Thread language to request Spanish.
  //------------------------------------------------------------

  // Set the requested AcceptLanguages into our thread.
  // Note: not a memory leak because the old AcceptLanguages in the
  // Thread will be deleted for us.
  Thread::setLanguages(new AcceptLanguages(requestAL));

  {
    AutoMutex automut(mutexCH);

    _cimom.getInstance(requestCtx,
		     CIMNamespaceName(NAMESPACE),
		     _instanceNames[0],
		     false,
		     true,
		     true,
		     CIMPropertyList());

    // Restore the original AcceptLanguages into our thread
    Thread::setLanguages(new AcceptLanguages(saveAL));

    _validateCIMOMHandleResponse(String("es")); // uses _cimom
  }  // mutex unlocks here

  //------------------------------------------------------------
  // TEST 2
  // Do a getInstance to our instance provider, asking for the French
  // instance in AcceptLanguages.  Check that the ContentLanguages
  // of the response indicates that French was returned.
  // NOTE - this test sets the AcceptLanguages in the request
  // OperationContext
  //------------------------------------------------------------

  requestAL.clear();
  requestAL.insert(AcceptLanguageElement("x-horse", float(1.0)));
  requestAL.insert(AcceptLanguageElement("x-cow", float(0.4)));
  requestAL.insert(AcceptLanguageElement("x-gekko", float(0.2)));
  requestAL.insert(AcceptLanguageElement("fr", float(0.9)));
  requestAL.insert(AcceptLanguageElement("es", float(0.8)));

  requestCtx.insert(AcceptLanguageListContainer(requestAL));

  {
    AutoMutex automut(mutexCH);

    _cimom.getInstance(requestCtx,
		     CIMNamespaceName(NAMESPACE),
		     _instanceNames[0],
		     false,
		     true,
		     true,
		     CIMPropertyList());

    _validateCIMOMHandleResponse(String("fr")); // uses _cimom
  } // mutex unlocks here
}

void LocalizedProvider::_validateCIMOMHandleResponse(String expectedLang)
{
  // Get the ContentLanguages of the response
  OperationContext responseCtx = _cimom.getResponseContext();
  ContentLanguages responseCL;
  try
  {
    ContentLanguageListContainer cl_cntr  =
      (ContentLanguageListContainer)responseCtx.get(ContentLanguageListContainer::NAME);
    responseCL = cl_cntr.getLanguages();
  }
  catch (Exception &)
  {
    // No ContentLanguageListContainer found.
    // If ICU is enabled, then we must get a ContentLanguages in the response.
#if defined PEGASUS_HAS_MESSAGES
    throw CIMOperationFailedException("Did not get ContentLanguageListContainer from CIMOMHandle");
#endif
  }

  // Figure out what ContentLanguage we expect
  ContentLanguages expectedCL("en");  // default to en because this provider sets
                                      // C-L to en by default on getInstance
#if defined PEGASUS_HAS_MESSAGES
  // If ICU is being used, then we expect back a language other than the default en.
  expectedCL = ContentLanguages(expectedLang);
#endif

  // Now, the compare
  if (expectedCL != responseCL)
  {
    // A miscompare
    String msg("ContentLanguage miscompare in CIMOMHandle test: ");
    msg.append(expectedCL.toString());
    msg.append(" != ");
    msg.append(responseCL.toString());
    throw CIMOperationFailedException(msg);
  }
}

void LocalizedProvider::_generateIndication()
{
  if (_enabled)
    {
      // Reset the consumer status to indication-not-received
      consumerStatus = 1;

      // Create the indication
      CIMInstance indicationInstance (CIMName("LocalizedProvider_TestIndication"));

      CIMObjectPath path ;
      path.setNameSpace("root/SampleProvider");
      path.setClassName("LocalizedProvider_TestIndication");

      indicationInstance.setPath(path);

      char buffer[32];
      sprintf(buffer, "%d", _nextUID++);
      indicationInstance.addProperty
	(CIMProperty ("IndicationIdentifier",String(buffer)));

      CIMDateTime currentDateTime = CIMDateTime::getCurrentDateTime ();
      indicationInstance.addProperty
	(CIMProperty ("IndicationTime", currentDateTime));

      Array<String> correlatedIndications;
      indicationInstance.addProperty
	(CIMProperty ("CorrelatedIndications", correlatedIndications));

      // Put the UTF-16 properties into the indication.  The listener will
      // verify that it gets there.
      indicationInstance.addProperty
	(CIMProperty ("UnicodeStr",String(roundTripChars)));
      indicationInstance.addProperty
	(CIMProperty ("UnicodeChar",roundTripChars[0]));

      CIMIndication cimIndication (indicationInstance);

      // Tag this indication with a language.  The listener will verify this.
      OperationContext ctx;
      ContentLanguages cl("x-world");
      ctx.insert(ContentLanguageListContainer(cl));

      // deliver the indication
      _handler->deliver (ctx, indicationInstance);
    }
}

