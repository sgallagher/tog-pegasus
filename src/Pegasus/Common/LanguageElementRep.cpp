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
// Author: Humberto Rivero (hurivero@us.ibm.com)
// 
// Modified By:
// 
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/LanguageElementRep.h>
#include <Pegasus/Common/LanguageParser.h>
 
PEGASUS_NAMESPACE_BEGIN
 
//#define PEGASUS_ARRAY_T LanguageElement
//#include "ArrayImpl.h"
//#undef PEGASUS_ARRAY_T
///////////////////////////////////////////////////////////////
//
// LanguageElement::
//  
//////////////////////////////////////////////////////////////


LanguageElementRep::LanguageElementRep(){
	language = String();
	country = String();
	variant = String();
	quality = 0;
}

LanguageElementRep::LanguageElementRep(String aLanguage, 
								 String aCountry,
								 String aVariant,
								 Real32 aQuality){
	this->language = aLanguage;
	this->country = aCountry;
	this->variant = aVariant;
	this->quality = aQuality;
} 

LanguageElementRep::LanguageElementRep(String language_tag, Real32 aQuality){
	if(language_tag == "*"){ 
		language = language_tag;
		country = String();
		variant = String();
		this->quality = 0;	
	}
	else{
		 splitLanguageTag(language_tag);
		this->quality = aQuality;
	}
}  

LanguageElementRep::LanguageElementRep(const LanguageElementRep &rhs){
	language = rhs.getLanguage();
	country = rhs.getCountry();
	variant = rhs.getVariant();
	quality = rhs.getQuality();
}

LanguageElementRep::~LanguageElementRep(){}

LanguageElementRep& LanguageElementRep::operator=(const LanguageElementRep &rhs)
{
	language = rhs.getLanguage();
	country = rhs.getCountry();
	variant = rhs.getVariant();
	quality = rhs.getQuality();
	return *this;	
}

String LanguageElementRep::getLanguage() const {
	return language;
}

String LanguageElementRep::getCountry() const {
	return country;
}

String LanguageElementRep::getVariant() const {
	return variant;
}

String LanguageElementRep::getTag() const{
	return buildLanguageTag();	
}

Real32 LanguageElementRep::getQuality() const{
	return quality;
}

String LanguageElementRep::toString() const{
	return buildLanguageTag();
}

Boolean LanguageElementRep::operator==(const LanguageElementRep &rhs)const{
	return String::equalNoCase(getTag(), rhs.getTag());		
}

Boolean LanguageElementRep::operator!=(const LanguageElementRep &rhs)const{
	return (!String::equalNoCase(getTag(),rhs.getTag()));
}


/* %%%%%%%%%%%%%%%%%%%% PRIVATE METHODS and MEMBERS %%%%%%%%%%%%%%%%%%%%%%%% */


String LanguageElementRep::buildLanguageTag() const {
	String tag = language;
	if(country.size() != 0)
		tag = tag + "-" + country;
	if(variant.size() != 0)
		tag = tag + "-" + variant;
	return tag;	
}

void LanguageElementRep::splitLanguageTag(String language_tag){
	LanguageParser lp;
	language = lp.getLanguage(language_tag);
	country = lp.getCountry(language_tag);
	variant = lp.getVariant(language_tag);
}

PEGASUS_NAMESPACE_END
