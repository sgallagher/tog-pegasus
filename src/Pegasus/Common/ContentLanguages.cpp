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
 
#include <Pegasus/Common/ContentLanguages.h>
#include <Pegasus/Common/Tracer.h>

//PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN
 
//////////////////////////////////////////////////////////////
//
// ContentLanguages::
//
//////////////////////////////////////////////////////////////
 
	const ContentLanguages ContentLanguages::EMPTY = ContentLanguages();

	ContentLanguages::ContentLanguages(String hdr): LanguageElementContainer(){
		PEG_METHOD_ENTER(TRC_L10N, "ContentLanguages::ContentLanguages(String)");
		Array<String> values;
		LanguageParser lp;
		lp.parseHdr(values,hdr);
		buildLanguageElements(values);
		PEG_METHOD_EXIT();
	}

	ContentLanguages::ContentLanguages(Array<LanguageElement> container):LanguageElementContainer(container){}
	
	ContentLanguages::ContentLanguages(Array<ContentLanguageElement> container){ 
		for(int i = 0; i < container.size(); i++)
			this->container.append(dynamic_cast<LanguageElement &>(container[i]));
	}

	ContentLanguages::ContentLanguages(const ContentLanguages &rhs): LanguageElementContainer(rhs){}
	
	ContentLanguages::~ContentLanguages(){}
	
	void ContentLanguages::append(ContentLanguageElement element){
		container.append(element);
	}

	void ContentLanguages::buildLanguageElements(Array<String> values){
		PEG_METHOD_ENTER(TRC_L10N, "ContentLanguages::buildLanguageElements");
		for(int i = 0; i < values.size(); i++){
			String language_tag;
			LanguageParser lp;
			language_tag = lp.parseContentLanguageValue(values[i]);
			container.append(ContentLanguageElement(language_tag));
		}
		PEG_METHOD_EXIT();
	}
	
	ContentLanguageElement ContentLanguages::getLanguageElement(int index) const{
		return ContentLanguageElement(container[index].getTag());
	}

	void ContentLanguages::getAllLanguageElements(Array<ContentLanguageElement> & elements) const{
		for(int i = 0; i < container.size(); i++)
			elements.append(getLanguageElement(i));
	}	
	
	ContentLanguageElement ContentLanguages::itrNext(){
		LanguageElement le = LanguageElementContainer::itrNext();
		if(le == LanguageElement::EMPTY_REF) return ContentLanguageElement::EMPTY_REF;
		else return ContentLanguageElement(le.getTag());	
	}
	
	PEGASUS_STD(ostream) & operator<<(PEGASUS_STD(ostream) &stream, ContentLanguages cl){
		for(int i = 0; i < cl.size(); i++){
			stream << cl.getLanguageElement(i);
			if(i != (cl.size()-1)) 
				stream << ", ";
		}
		return stream;
	}
	
ContentLanguages ContentLanguages::operator=(ContentLanguages rhs){
	LanguageElementContainer::operator=(rhs);
	return *this;	
}

	int ContentLanguages::find(String language_tag){
		return LanguageElementContainer::find(ContentLanguageElement(language_tag));
	}
	
PEGASUS_NAMESPACE_END
