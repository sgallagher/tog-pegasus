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

#include <Pegasus/Common/LanguageElement.h>
#include <Pegasus/Common/LanguageElementRep.h>
 
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
    _rep = new LanguageElementRep();
}

LanguageElement::LanguageElement(String aLanguage, String aCountry, String aVariant, Real32 aQuality){
    _rep = new LanguageElementRep(aLanguage, aCountry, aVariant, aQuality);
} 

LanguageElement::LanguageElement(String language_tag, Real32 aQuality){
    _rep = new LanguageElementRep(language_tag, aQuality);
}  

LanguageElement::LanguageElement(const LanguageElement &rhs){
    _rep = new LanguageElementRep(*rhs._rep);
}

LanguageElement::~LanguageElement(){
    if(_rep) delete _rep;
}

LanguageElement& LanguageElement::operator=(const LanguageElement &rhs)
{
    if (&rhs != this)
    {
        *_rep = *rhs._rep;
    }
    return *this;
}

String LanguageElement::getLanguage() const {
    return _rep->getLanguage();
}

String LanguageElement::getCountry() const {
    return _rep->getCountry();
}

String LanguageElement::getVariant() const {
    return _rep->getVariant();
}

String LanguageElement::getTag() const{
    return _rep->getTag();
}

Real32 LanguageElement::getQuality() const{
    return _rep->getQuality();
}

String LanguageElement::toString() const{
    return _rep->toString();
}

Boolean LanguageElement::operator==(const LanguageElement &rhs)const{
    return _rep->operator==(*rhs._rep);
}

Boolean LanguageElement::operator!=(const LanguageElement &rhs)const{
    return (*_rep != *rhs._rep);
}

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& stream, LanguageElement le){
    stream << le.getTag();
    return stream;
}

PEGASUS_NAMESPACE_END
