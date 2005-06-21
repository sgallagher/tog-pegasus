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
 
#include <Pegasus/Common/ContentLanguages.h>
#include <Pegasus/Common/LanguageParser.h>
#include <Pegasus/Common/LanguageElementContainerRep.h>
PEGASUS_NAMESPACE_BEGIN
 
//////////////////////////////////////////////////////////////
//
// ContentLanguages::
//
//////////////////////////////////////////////////////////////
 
const ContentLanguages ContentLanguages::EMPTY = ContentLanguages();

ContentLanguages::ContentLanguages(): LanguageElementContainer(){
}

ContentLanguages::ContentLanguages(String hdr): LanguageElementContainer(){
    if(hdr.size() > 0){
        Array<String> values;
        LanguageParser lp;
        lp.parseHdr(values,hdr);
        buildLanguageElements(values);
    }
}

ContentLanguages::ContentLanguages(const Array<LanguageElement> & container):LanguageElementContainer(container){
}

ContentLanguages::ContentLanguages(const Array<ContentLanguageElement> &aContainer){ 
    for(Uint32 i = 0; i < aContainer.size(); i++)
        LanguageElementContainer::append(aContainer[i]);
    
}

ContentLanguages::ContentLanguages(const ContentLanguages &rhs): LanguageElementContainer(rhs){
}

ContentLanguages::~ContentLanguages(){}

void ContentLanguages::append(ContentLanguageElement element){
    _rep->append(element);
}

ContentLanguageElement ContentLanguages::getLanguageElement(Uint32 index) const{
    return ContentLanguageElement(_rep->getLanguageElement(index));
}

void ContentLanguages::getAllLanguageElements(Array<ContentLanguageElement> & elements) const{
    Array<LanguageElement> tmp = _rep->getAllLanguageElements();
    for(Uint32 i = 0; i < tmp.size(); i++)
        elements.append(ContentLanguageElement(tmp[i]));
}    

Array<ContentLanguageElement> ContentLanguages::getAllLanguageElements()const{
    Array<ContentLanguageElement> elements;
    Array<LanguageElement> tmp = _rep->getAllLanguageElements();
        for(Uint32 i = 0; i < tmp.size(); i++)
            elements.append(ContentLanguageElement(tmp[i]));
    return elements;
}

ContentLanguageElement ContentLanguages::itrNext(){
    return ContentLanguageElement(_rep->itrNext());
}

PEGASUS_STD(ostream) & operator<<(PEGASUS_STD(ostream) &stream, const ContentLanguages &cl){
    for(Uint32 i = 0; i < cl.size(); i++){
        stream << cl.getLanguageElement(i);
        if(i != (cl.size()-1)) 
            stream << ", ";
    }
    return stream;
}

ContentLanguages& ContentLanguages::operator=(const ContentLanguages &rhs)
{
    if (&rhs != this)
        LanguageElementContainer::operator=(rhs);
    return *this;    
}

Sint32 ContentLanguages::find(String language_tag)const {
    return _rep->find(language_tag);    
}

void ContentLanguages::buildLanguageElements(Array<String> values){
    for(Uint32 i = 0; i < values.size(); i++){
        String language_tag;
        LanguageParser lp;
        language_tag = lp.parseContentLanguageValue(values[i]);
        append(ContentLanguageElement(language_tag));
    }
}

PEGASUS_NAMESPACE_END
