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

#include <Pegasus/Common/LanguageElementContainer.h>
#include <Pegasus/Common/LanguageElementContainerRep.h>

PEGASUS_NAMESPACE_BEGIN


//////////////////////////////////////////////////////////////
//
// LanguageElementContainer::
//
//////////////////////////////////////////////////////////////
 
const LanguageElementContainer LanguageElementContainer::EMPTY = LanguageElementContainer();

LanguageElementContainer::LanguageElementContainer(){
    _rep = new LanguageElementContainerRep();
}
 
LanguageElementContainer::LanguageElementContainer(const Array<LanguageElement> &aContainer){
    _rep = new LanguageElementContainerRep(aContainer);
}
 
LanguageElementContainer::LanguageElementContainer(const LanguageElementContainer &rhs){
    _rep = new LanguageElementContainerRep(*rhs._rep);
}

LanguageElementContainer::~LanguageElementContainer(){
    if(_rep) delete _rep;
}

LanguageElementContainer& LanguageElementContainer::operator=(
    const LanguageElementContainer &rhs)
{
    if (&rhs != this)
        *_rep = *rhs._rep;
    return *this;
}

LanguageElement LanguageElementContainer::getLanguageElement(Uint32 index) const{
    return _rep->getLanguageElement(index);
} 

Array<LanguageElement> LanguageElementContainer::getAllLanguageElements() const{
    return _rep->getAllLanguageElements();
}  

String LanguageElementContainer::toString() const{
    return _rep->toString();
} 

Uint32 LanguageElementContainer::size() const{
    return _rep->size();
}

Boolean LanguageElementContainer::contains(LanguageElement element)const {
    return _rep->contains(element);
}

void LanguageElementContainer::itrStart(){
    _rep->itrStart();
}

LanguageElement LanguageElementContainer::itrNext(){
    return _rep->itrNext();
}

void LanguageElementContainer::append(LanguageElement element){
    _rep->append(element);
}

void LanguageElementContainer::remove(Uint32 index){
    _rep->remove(index);
}

Uint32 LanguageElementContainer::remove(const LanguageElement &element){
    return _rep->remove(element);
}

void LanguageElementContainer::clear(){
    _rep->clear();
}

Boolean LanguageElementContainer::operator==(const LanguageElementContainer &rhs)const{
    return (*_rep == *rhs._rep);
}

Boolean LanguageElementContainer::operator!=(const LanguageElementContainer &rhs)const{
    return (*_rep != *rhs._rep);
}

PEGASUS_NAMESPACE_END
