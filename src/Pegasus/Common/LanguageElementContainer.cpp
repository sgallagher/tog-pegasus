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
// Author: Humberto Rivero (hurivero@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/LanguageElementContainer.h>
#include <Pegasus/Common/Exception.h>
//#include <Pegasus/Common/LanguageParser.h>

//PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

	
//////////////////////////////////////////////////////////////
//
// LanguageElementContainer::
//
//////////////////////////////////////////////////////////////
		 
const LanguageElementContainer LanguageElementContainer::EMPTY = LanguageElementContainer();
	
LanguageElementContainer::LanguageElementContainer(){}	
		 
LanguageElementContainer::LanguageElementContainer(Array<LanguageElement> aContainer){
	this->container = aContainer;	
}
 
LanguageElementContainer::LanguageElementContainer(const LanguageElementContainer &rhs){
		container = rhs.getAllLanguageElements();	
}

LanguageElementContainer::~LanguageElementContainer(){}

LanguageElementContainer LanguageElementContainer::operator=(LanguageElementContainer rhs){
		container = rhs.getAllLanguageElements();
		return *this;	
}

LanguageElement LanguageElementContainer::getLanguageElement(int index) const{
	if(index >= container.size())
	 	throw IndexOutOfBoundsException();
	return container[index];
} 

void LanguageElementContainer::buildLanguageElements(Array<String> values){}

Array<LanguageElement> LanguageElementContainer::getAllLanguageElements() const{
	return container;	
}  

String LanguageElementContainer::toString() const{
	String s;
	for(int i = 0; i < container.size(); i++){
		s.append(container[i].toString());
		if(i < container.size() - 1)
			s.append(", ");	
	}
	return s;
} 

Uint32 LanguageElementContainer::size() const{
	return container.size();	
}

Boolean LanguageElementContainer::contains(LanguageElement element)const {
	return (find(element) != -1);
}

void LanguageElementContainer::itrStart(){
	itr_index = 0;
}

LanguageElement& LanguageElementContainer::itrNext(){
	if(itr_index >= container.size())
		return LanguageElement::EMPTY_REF;
	return container[itr_index++];
}

//void LanguageElementContainer::append(LanguageElement element){
//	container.append(element);
//}

void LanguageElementContainer::remove(Uint32 index){
	Uint32 size = container.size();
	container.remove(index);
	if(size < container.size()){	
		updateIterator();	
	}
}

int LanguageElementContainer::remove(LanguageElement element){
	int index = find(element);
	if(index != -1){
		container.remove(index);			
		updateIterator();
	}
	return index;	
}

void LanguageElementContainer::clear(){
	container.clear();
	itr_index = 0;	
}

Boolean LanguageElementContainer::operator==(const LanguageElementContainer rhs)const{
	if(container.size() != rhs.size()) return false;
	for(int i = 0; i < container.size(); i++){
		if(container[i] != rhs.getLanguageElement(i)) 
			return false;
	}
	return true;
}

Boolean LanguageElementContainer::operator!=(const LanguageElementContainer rhs)const{
	if(container.size() != rhs.size()) return true;
	for(int i = 0; i < container.size(); i++){
		if(container[i] != rhs.getLanguageElement(i)) 
			return true;
	}
	return false;
}

int LanguageElementContainer::find(LanguageElement element)const { 
	for(Uint32 i = 0; i < container.size(); i++){
		if(element == container[i])
			return i;		
	}	
	return -1;
}

/** 
 * Called when any element is removed from the container
 */
void LanguageElementContainer::updateIterator(){
	if(itr_index > 0)
		itr_index--;	
}

PEGASUS_NAMESPACE_END
