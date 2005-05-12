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
// Modified By:	Sean Keenan (sean.keenan@hp.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for BUG# 3512, 3513
//
//%/////////////////////////////////////////////////////////////////////////////
 
#include <Pegasus/Common/AcceptLanguageElement.h>
#include <Pegasus/Common/LanguageElementRep.h>

#if defined(PEGASUS_OS_VMS)
#include <stdio.h>
#endif

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T AcceptLanguageElement
#include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

///////////////////////////////////////////////////////////////
//
// AcceptLanguageElement::
//
//////////////////////////////////////////////////////////////

const AcceptLanguageElement AcceptLanguageElement::EMPTY = AcceptLanguageElement();

AcceptLanguageElement AcceptLanguageElement::EMPTY_REF = AcceptLanguageElement();

AcceptLanguageElement::AcceptLanguageElement():LanguageElement(){
}

AcceptLanguageElement::AcceptLanguageElement(const String& language, const String& country, const String& variant, Real32 quality):
	LanguageElement(language, country, variant, quality){
}

AcceptLanguageElement::AcceptLanguageElement(const String& language_tag, Real32 quality):LanguageElement(language_tag, quality){
}

AcceptLanguageElement::AcceptLanguageElement(const LanguageElement &le):LanguageElement(le){}

AcceptLanguageElement::AcceptLanguageElement(const AcceptLanguageElement &rhs):LanguageElement(rhs){
}

AcceptLanguageElement::~AcceptLanguageElement(){
}

Real32 AcceptLanguageElement::getQuality() const {
	return _rep->getQuality();
}

String AcceptLanguageElement::toString() const{
	String s = _rep->toString();
        String quality_prefix = ";q=";
        if(getTag() != "*" && getQuality() != 1.0){
                char q[6];
                sprintf(q,"%4.3f", getQuality());
                s.append(quality_prefix).append(q);
        }
                                                                                                                                                             
        return s;
}

Boolean AcceptLanguageElement::operator>(const AcceptLanguageElement &rhs) const{
	return (_rep->getQuality() > rhs.getQuality());
}

Boolean AcceptLanguageElement::operator<(const AcceptLanguageElement &rhs) const{
	return (_rep->getQuality() < rhs.getQuality());
}

Boolean AcceptLanguageElement::operator==(const AcceptLanguageElement &rhs) const{
	if(*_rep == *rhs._rep)
		if(_rep->getQuality() == rhs._rep->getQuality())
			return true;
	return false;
}

Boolean AcceptLanguageElement::operator!=(const AcceptLanguageElement &rhs) const{
	if(*_rep == *rhs._rep)
                if(_rep->getQuality() == rhs._rep->getQuality())
                        return false;
        return true;
}

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& stream, AcceptLanguageElement ale){
	String s = ale.toString();
	stream << s;
	return stream;
}


PEGASUS_NAMESPACE_END
