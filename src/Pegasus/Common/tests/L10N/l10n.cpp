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
#include <cstdlib>

#include <iostream>

#include <cassert>



#include <Pegasus/Common/String.h>

#include <Pegasus/Common/InternalException.h>

#include <Pegasus/Common/LanguageParser.h>

#include <Pegasus/Common/AcceptLanguageElement.h>

#include <Pegasus/Common/ContentLanguageElement.h>

#include <Pegasus/Common/AcceptLanguages.h>

#include <Pegasus/Common/ContentLanguages.h>

#include <Pegasus/Common/MessageLoader.h>

#include <Pegasus/Common/Array.h>



PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;



void drive_LanguageParser(){

	

	LanguageParser lp;

	

	String tag1("en-US-mn-blah-blah");

	

	assert( lp.getLanguage(tag1) == "en" );

	assert( lp.getCountry(tag1) == "US" );

	assert( lp.getVariant(tag1) == "mn-blah-blah" );

	

	String tag2("en-US-mn (should not appear)");

	String tag3("en-US-mn;q=.8");	

	String tag4("en-US-invalid;q=1.1");

	

	assert( lp.parseContentLanguageValue(tag2) == "en-US-mn" );

	

	String accept_language_tag = String::EMPTY;

	Real32 q = lp.parseAcceptLanguageValue(accept_language_tag, tag3);

	Real32 answer = .8;

	assert( q == answer );

	assert( accept_language_tag == "en-US-mn" );

	try{

		lp.parseAcceptLanguageValue(accept_language_tag, tag4);

		assert( false ); //should not get here

	}catch(InvalidAcceptLanguageHeader&){}

		

	String tag5("en-US-mn,fr-wh-imps;q=0"); 

	Array<String> values;

	lp.parseHdr(values, tag5);

	assert( values[0] == "en-US-mn" );

	assert( values[1] == "fr-wh-imps;q=0" );

}



void drive_AcceptLanguageElement(){

	

	String tag1("en-US-mn");

	Real32 quality = 1.0;

	assert( AcceptLanguageElement(tag1).toString() == "en-US-mn" );

	assert( AcceptLanguageElement(tag1).getLanguage() == "en" );

	assert( AcceptLanguageElement(tag1).getCountry() == "US" );

	assert( AcceptLanguageElement(tag1).getVariant() == "mn" );

	assert( AcceptLanguageElement(tag1).getQuality() == quality ); 

	

	AcceptLanguageElement al1("en","US","mn");

	AcceptLanguageElement al2("en","US","mn",0.5);

	AcceptLanguageElement al3(al1);

	AcceptLanguageElement al4("en");



	assert( al1 != al2 ); 

	assert( al1 == al3 ); 

	assert( al2 != al3 ); 

	assert( al1 != al4 ); 

	assert( al1 > al2 );  // only compares quality values

	

}



void drive_ContentLanguageElement(){

	

	String tag1("en-US-mn");

	assert( ContentLanguageElement(tag1).toString() == "en-US-mn" );

	assert( ContentLanguageElement(tag1).getLanguage() == "en" );

	assert( ContentLanguageElement(tag1).getCountry() == "US" );

	assert( ContentLanguageElement(tag1).getVariant() == "mn" );

		

	ContentLanguageElement cl1(tag1);

	ContentLanguageElement cl2(cl1);

	ContentLanguageElement cl3 = cl2;

	ContentLanguageElement cl4(tag1);

	

	assert( cl1 == cl2 ); 

	assert( cl1 == cl3 ); 

	assert( cl2 == cl3 ); 

	assert( cl1 == cl4 );

	

}



void drive_AcceptLanguages(){

		

	AcceptLanguages al("en-US-mn;q=.9,fr-FR;q=.1,en, fr;q=.2,la-SP-bal;q=.7,*");

	AcceptLanguages al1,al2;

	try{

		int i = 0;

		AcceptLanguageElement ale;

		al.itrStart();

		while( (ale = al.itrNext()) != AcceptLanguageElement::EMPTY_REF ){

			switch(i){

				case 0: assert( String::equal(ale.toString(), "en") ); break;

				case 1: assert( String::equal(ale.toString(), "en-US-mn;q=0.900") ); break;	

				case 2: assert( String::equal(ale.toString(), "la-SP-bal;q=0.700") ); break;

				case 3: assert( String::equal(ale.toString(), "fr;q=0.200") ); break;	

				case 4: assert( String::equal(ale.toString(), "fr-FR;q=0.100") ); break;	

				case 5: assert( String::equal(ale.toString(), "*") ); break;	

			}

			i++;

		}

		

		assert( al.size() == 6 );

		

		al.add(AcceptLanguageElement("en-XX-xx"));

		assert( al.size() == 7 );

		assert( al.contains(AcceptLanguageElement("en-XX-xx")) );

		

		al.remove(AcceptLanguageElement("en-XX-xx"));

		assert( !al.contains(AcceptLanguageElement("en-XX-xx")) );

		assert( al.size() == 6 );

		

		al1 = al;

		assert( al1 == al );

		

		al1.clear();

		assert( al1.size() == 0 );

		

		Array<AcceptLanguageElement> al_arr;

		al.getAllLanguageElements(al_arr);

		al2 = AcceptLanguages(al_arr);

		assert( al2 == al );	

			

		AcceptLanguages al3("en-US-mn;;");

		assert(false); //should not get here

	}catch(InvalidAcceptLanguageHeader&){}	

}



void drive_ContentLanguages(){

	ContentLanguages cl("en-US-mn,fr-FR,en, fr(oh you french), la-SP-bal,*");

	

	ContentLanguages cl1,cl2;

	try{

		int i = 0;

		ContentLanguageElement cle;

		cl.itrStart();

		while( (cle = cl.itrNext()) != ContentLanguageElement::EMPTY_REF ){

			if( i == 3 ){

				assert( String::equal(cle.toString(), "fr") );

			}	

			i++;

		}

		

		assert( cl.size() == 6 );

		

		cl.append(ContentLanguageElement("en-XX-xx"));

		assert( cl.size() == 7 );

		assert( cl.contains(ContentLanguageElement("en-XX-xx")) );

		

		cl.remove(ContentLanguageElement("en-XX-xx"));

		assert( !cl.contains(ContentLanguageElement("en-XX-xx")) );

		assert( cl.size() == 6 );

		

		cl1 = cl;

		assert( cl1 == cl );

		

		cl1.clear();

		assert( cl1.size() == 0 );

		

		Array<ContentLanguageElement> cl_arr;

		cl.getAllLanguageElements(cl_arr);

		cl2 = ContentLanguages(cl_arr);

		assert( cl2 == cl );	



		ContentLanguages cl3("en-454545US-mn");  // should throw the below exception

		assert(false); //should not get here

	}catch(InvalidContentLanguageHeader&){}	

		

}

 

void callMessageLoader(MessageLoaderParms mlp_arr[], int size){

	String msg;

	for(int i = 0; i < size; i++){

	 	msg = MessageLoader::getMessage(mlp_arr[i]);

	}

}

 

void drive_MessageLoader(){

	

	String msg;

	int mlp_arr_size = 4;



	MessageLoaderParms mlp("CIMStatusCode.CIM_ERR_SUCCESS","Default CIMStatusCode, $0 $1",

							"rab oof is foo bar backwards",64000);

	mlp.msg_src_path = "test/pegasusTest";

	

	AcceptLanguages al("en_US");

	mlp.acceptlanguages = al;

#ifdef PEGASUS_HAS_ICU

	assert ( MessageLoader::getMessage(mlp) == "CIM_ERR_SUCCESS: SUCCESSFUL en-us rab oof is foo bar backwards, number = 64,000" );

	// test for return content languages

	assert ( mlp.contentlanguages.toString() == "en-US" );

#else

	assert ( MessageLoader::getMessage(mlp) == "Default CIMStatusCode, rab oof is foo bar backwards 64000" );

#endif

	

	

	// should load en-US resource because of single element fallback logic

	mlp.acceptlanguages.clear();

	mlp.acceptlanguages.add(AcceptLanguageElement("en-US-mn"));

#ifdef PEGASUS_HAS_ICU

	assert ( MessageLoader::getMessage(mlp) == "CIM_ERR_SUCCESS: SUCCESSFUL en-us rab oof is foo bar backwards, number = 64,000" );

#else

	assert ( MessageLoader::getMessage(mlp) == "Default CIMStatusCode, rab oof is foo bar backwards 64000" );

#endif

		

	// testing first element fallback after acceptlanguages has been exhausted

	MessageLoaderParms mlp1("CIMStatusCode.CIM_ERR_SUCCESS","Default CIMStatusCode, $0 $1",

							"rab oof is foo bar backwards","fr");

	mlp1.msg_src_path = "test/pegasusTest";

	mlp1.acceptlanguages.clear();	

	mlp1.acceptlanguages.add(AcceptLanguageElement("fr-FR")); 

	mlp1.acceptlanguages.add(AcceptLanguageElement("bl-ow"));

#ifdef PEGASUS_HAS_ICU

	assert ( MessageLoader::getMessage(mlp1) == "CIM_ERR_SUCCESS: SUCCESSFUL fr rab oof is foo bar backwards, number = fr" );

#else

	assert ( MessageLoader::getMessage(mlp) == "Default CIMStatusCode, rab oof is foo bar backwards 64000" );

#endif

	 

	// use gobal default message switch for messageloading

	MessageLoader::_useDefaultMsg = true;

	mlp.acceptlanguages.clear();

	mlp.acceptlanguages.add(AcceptLanguageElement("en-US"));

	assert ( MessageLoader::getMessage(mlp) == "Default CIMStatusCode, rab oof is foo bar backwards 64000" );

	

	// set static AcceptLanguages in message loader

	MessageLoader::_useDefaultMsg = false;

	MessageLoader::_acceptlanguages.add(AcceptLanguageElement("st_at_ic"));

#ifdef PEGASUS_HAS_ICU

	assert ( MessageLoader::getMessage(mlp) == "CIM_ERR_SUCCESS: SUCCESSFUL st_at_ic rab oof is foo bar backwards, number = 64,000" );

#else

	assert ( MessageLoader::getMessage(mlp) == "Default CIMStatusCode, rab oof is foo bar backwards 64000" );

#endif

	

}



int main( int argc, char *argv[] ){

	

	//BEGIN TESTS....

	

		drive_LanguageParser();	

		drive_AcceptLanguageElement();

		drive_ContentLanguageElement();

		drive_AcceptLanguages();

		drive_ContentLanguages();

		drive_MessageLoader();

	

	//END TESTS....



	return 0;

}



