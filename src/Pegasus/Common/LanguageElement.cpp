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
// Author: Humberto Rivero (hurivero@us.ibm.com)
// 
// Modified By:
// 
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/LanguageElement.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/LanguageParser.h>
 
//PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN
 
#define PEGASUS_ARRAY_T LanguageElement
#include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T
///////////////////////////////////////////////////////////////
//
// LanguageElement::
//  
//////////////////////////////////////////////////////////////


const LanguageElement LanguageElement::EMPTY = LanguageElement();

LanguageElement LanguageElement::EMPTY_REF = LanguageElement();
 
LanguageElement::LanguageElement(){
	language = String::EMPTY;
	country = String::EMPTY;
	variant = String::EMPTY;
}

LanguageElement::LanguageElement(String language, 
								 String country,
								 String variant,
								 Real32 quality){
	this->language = language;
	this->country = country;
	this->variant = variant;
	this->quality = quality;
} 

LanguageElement::LanguageElement(String language_tag, Real32 quality){
	if(language_tag == "*"){ 
		language = language_tag;
		this->quality = 0;	
	}
	else{
		 splitLanguageTag(language_tag);
		this->quality = quality;
	}
}  

LanguageElement::LanguageElement(const LanguageElement &rhs){
	language = rhs.getLanguage();
	country = rhs.getCountry();
	variant = rhs.getVariant();
	quality = rhs.quality;
}

LanguageElement::~LanguageElement(){}

LanguageElement LanguageElement::operator=(LanguageElement rhs){
	language = rhs.getLanguage();
	country = rhs.getCountry();
	variant = rhs.getVariant();
	quality = rhs.quality;
	return *this;	
}

String LanguageElement::getLanguageTag() const {
	return buildLanguageTag();	
}

String LanguageElement::getLanguage() const {
	return language;
}

String LanguageElement::getCountry() const {
	return country;
}

String LanguageElement::getVariant() const {
	return variant;
}

String LanguageElement::getTag() const{
	return buildLanguageTag();	
}

String LanguageElement::toString() const{
	return buildLanguageTag();
}

Boolean LanguageElement::operator==(LanguageElement rhs){
	return String::equalNoCase(getLanguageTag(), rhs.getLanguageTag());		
}

Boolean LanguageElement::operator!=(LanguageElement rhs){
	return (!String::equalNoCase(getLanguageTag(),rhs.getLanguageTag()));
}

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& stream, LanguageElement le){
		stream << le.getLanguageTag();
		return stream;
}


/* %%%%%%%%%%%%%%%%%%%% PRIVATE METHODS and MEMBERS %%%%%%%%%%%%%%%%%%%%%%%% */


String LanguageElement::buildLanguageTag() const {
	String tag = language;
	if(country.size() != 0)
		tag = tag + "-" + country;
	if(variant.size() != 0)
		tag = tag + "-" + variant;
	return tag;	
}

void LanguageElement::splitLanguageTag(String language_tag){
		LanguageParser lp;
		language = lp.getLanguage(language_tag);
		country = lp.getCountry(language_tag);
		variant = lp.getVariant(language_tag);
}


PEGASUS_NAMESPACE_END
