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
 
#include <Pegasus/Common/AcceptLanguages.h>
#include <Pegasus/Common/Tracer.h>

//PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

	 
//////////////////////////////////////////////////////////////
//
// AcceptLanguages::
//
//////////////////////////////////////////////////////////////

const AcceptLanguages AcceptLanguages::EMPTY = AcceptLanguages();

AcceptLanguages::AcceptLanguages(): LanguageElementContainer()
{
}

AcceptLanguages::AcceptLanguages(String hdr): LanguageElementContainer(){
   //TODO: remove temporary fix to constructing with String::EMPTY
	PEG_METHOD_ENTER(TRC_L10N, "AcceptLanguages::AcceptLanguages(String)");
	if(hdr.size() > 0){
		Array<String> values;
		LanguageParser lp;
		lp.parseHdr(values,hdr);
		buildLanguageElements(values);
		prioritize();
	}
	PEG_METHOD_EXIT();
}

AcceptLanguages::AcceptLanguages(Array<AcceptLanguageElement> aContainer) {
	for(int i = 0; i < aContainer.size(); i++)
		this->container.append(dynamic_cast<LanguageElement &>(aContainer[i]));
}

String AcceptLanguages::toString() const{
	String s;
	for(int i = 0; i < container.size(); i++){
		s.append(container[i].toString());
		if(container[i].getLanguage() != "*" && container[i].quality != 1){
			char q[6];
			sprintf(q,"%4.3f", container[i].quality);	
			s.append(";q=").append(q);
		}
		if(i < container.size() - 1)
			s.append(",");	
	}
	return s;
}

PEGASUS_STD(ostream) & operator<<(PEGASUS_STD(ostream) &stream, AcceptLanguages al){
	stream << al.toString();
	return stream;
}

AcceptLanguages AcceptLanguages::operator=(AcceptLanguages rhs){
	LanguageElementContainer::operator=(rhs);
	return *this;	
}

AcceptLanguageElement AcceptLanguages::getLanguageElement(int index) const{
	return AcceptLanguageElement(container[index].getTag(), container[index].quality);
}

void AcceptLanguages::getAllLanguageElements(Array<AcceptLanguageElement> & elements) const{
	for(int i = 0; i < container.size(); i++)
		elements.append(getLanguageElement(i));
}

void AcceptLanguages::buildLanguageElements(Array<String> values){
	PEG_METHOD_ENTER(TRC_L10N, "AcceptLanguages::buildLanguageElements");
	for(int i = 0; i < values.size(); i++){
		String language_tag;
		Real32 quality;
		LanguageParser lp;
		quality = lp.parseAcceptLanguageValue(language_tag,values[i]);
		container.append(AcceptLanguageElement(language_tag,quality));
	} 
	PEG_METHOD_EXIT();
}

AcceptLanguageElement AcceptLanguages::itrNext(){
	LanguageElement le = LanguageElementContainer::itrNext();
	if(le == LanguageElement::EMPTY_REF) return AcceptLanguageElement::EMPTY_REF;
	else return AcceptLanguageElement(le.getTag(),le.quality);	
}

void AcceptLanguages::add(AcceptLanguageElement element){
	container.append(element);
	prioritize();	
}

void AcceptLanguages::remove(Uint32 index){
	LanguageElementContainer::remove(index);
	prioritize();
}

int AcceptLanguages::remove(AcceptLanguageElement element){
	int rc;
	if ((rc = LanguageElementContainer::remove(element)) != -1)
		prioritize();
	return rc;
}

int AcceptLanguages::find(AcceptLanguageElement element) { 
	for(Uint32 i = 0; i < container.size(); i++){
		if(element.getTag() == container[i].getTag()){
			if(element.quality == container[i].quality){
				return i;	 
			}
			else return i;	
		}
	}	 
	return -1;
}

int AcceptLanguages::find(String language_tag, Real32 quality){
	return find(AcceptLanguageElement(language_tag,quality));
}

void AcceptLanguages::prioritize() {
	// sort according to quality value, if two qualities are equal then preference is
    // is given to the existing order of the objects in the array
	PEG_METHOD_ENTER(TRC_L10N, "AcceptLanguages::prioritize");
	
	//array is already sorted
	if(container.size() <= 1)
		return;
	
  	Boolean changed;
  	int n = container.size();
  	do {
    	n--;               // make loop smaller each time.
    	changed = false; // assume this is last pass over array
   		for (int i=0; i<n; i++) {
      		if (container[i].quality < container[i+1].quality) {
         		LanguageElement temp = container[i];  
         		container[i] = container[i+1];  
         		container[i+1] = temp;
         		changed = true;  // after an exchange, must look again 
      		}
    	}
  	} while (changed);
  	PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
