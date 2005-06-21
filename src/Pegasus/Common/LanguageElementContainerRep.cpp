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

#include <Pegasus/Common/LanguageElementContainerRep.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

	
//////////////////////////////////////////////////////////////
//
// LanguageElementContainer::
//
//////////////////////////////////////////////////////////////
		 
	
LanguageElementContainerRep::LanguageElementContainerRep(){
}	
		 
LanguageElementContainerRep::LanguageElementContainerRep(const Array<LanguageElement> &aContainer){
	this->container = aContainer;	
}
 
LanguageElementContainerRep::LanguageElementContainerRep(const LanguageElementContainerRep &rhs){
		container = rhs.getAllLanguageElements();	
}

LanguageElementContainerRep::~LanguageElementContainerRep(){}

LanguageElementContainerRep& LanguageElementContainerRep::operator=(
    const LanguageElementContainerRep& rhs){
	if(&rhs != this)
		container = rhs.getAllLanguageElements();
	return *this;	
}

LanguageElement LanguageElementContainerRep::getLanguageElement(Uint32 index) const{
	if(index >= container.size())
	 	throw IndexOutOfBoundsException();
	return container[index];
} 

void LanguageElementContainerRep::buildLanguageElements(Array<String> values){
}

Array<LanguageElement> LanguageElementContainerRep::getAllLanguageElements() const{
	return container;	
}  

String LanguageElementContainerRep::toString() const{
	String s;
	for(Uint32 i = 0; i < container.size(); i++){
		s.append(container[i].toString());
		if(i < container.size() - 1)
			s.append(", ");	
	}
	return s;
} 

Uint32 LanguageElementContainerRep::size() const{
	return container.size();	
}

Boolean LanguageElementContainerRep::contains(LanguageElement element)const {
	return (find(element) != -1);
}

void LanguageElementContainerRep::itrStart(){
	itr_index = 0;
}

LanguageElement LanguageElementContainerRep::itrNext(){
	if(itr_index >= container.size()){
		#if defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
                        return LanguageElement(); // To Do on Mac OS X
                #else
                        return LanguageElement::EMPTY_REF;
                #endif
	}
	return container[itr_index++];
}

void LanguageElementContainerRep::append(LanguageElement element){
	container.append(element);
}

void LanguageElementContainerRep::remove(Uint32 index){
	Uint32 size = container.size();
	container.remove(index);
	if(size < container.size()){	
		updateIterator();	
	}
}

Uint32 LanguageElementContainerRep::remove(const LanguageElement &element){
	int index = find(element);
	if(index != -1){
		container.remove(index);			
		updateIterator();
	}
	return index;	
}

void LanguageElementContainerRep::clear(){
	container.clear();
	itr_index = 0;	
}

Boolean LanguageElementContainerRep::operator==(const LanguageElementContainerRep &rhs)const{
	if(container.size() != rhs.size()) return false;
	for(Uint32 i = 0; i < container.size(); i++){
		if(container[i] != rhs.getLanguageElement(i)) 
			return false;
	}
	return true;
}

Boolean LanguageElementContainerRep::operator!=(const LanguageElementContainerRep &rhs)const{
	if(container.size() != rhs.size()) return true;
	for(Uint32 i = 0; i < container.size(); i++){
		if(container[i] != rhs.getLanguageElement(i)) 
			return true;
	}
	return false;
}

Sint32 LanguageElementContainerRep::find(LanguageElement element)const { 
	for(Uint32 i = 0; i < container.size(); i++){
		if(element == container[i])
			return i;		
	}	
	return -1;
}

/** 
 * Called when any element is removed from the container
 */
void LanguageElementContainerRep::updateIterator(){
	if(itr_index > 0)
		itr_index--;	
}

PEGASUS_NAMESPACE_END
