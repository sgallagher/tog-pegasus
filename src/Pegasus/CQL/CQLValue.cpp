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
// Author: Dave Rosckes (rosckes@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>

#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/Repository/NameSpaceManager.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/CQLFactory.h>



PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

#define PEGASUS_ARRAY_T CQLValue
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T


#define CIMTYPE_EMBEDDED 15  //temporary
CQLValue::CQLValue()
{
	_rep = new CQLValueRep();
}

CQLValue::~CQLValue()
{
   if(_rep)
		delete _rep;
}

CQLValue::CQLValue(const CQLValue& val)
{
   _rep = new CQLValueRep(val._rep);
}

CQLValue::CQLValue(String inString, NumericType inValueType, Boolean inSign)
{
   _rep = new CQLValueRep(inString, inValueType, inSign);
}


CQLValue::CQLValue(CQLChainedIdentifier inCQLIdent)
{
   _rep = new CQLValueRep(inCQLIdent);
}


CQLValue::CQLValue(String inString)
{
   _rep = new CQLValueRep(inString);
}

CQLValue::CQLValue(CIMInstance inInstance)
{
   _rep = new CQLValueRep(inInstance);
}

CQLValue::CQLValue(CIMClass inClass)
{
   _rep = new CQLValueRep(inClass);
}

CQLValue::CQLValue(CIMObjectPath inObjPath)
{  
   _rep = new CQLValueRep(inObjPath);
}

CQLValue::CQLValue(CIMDateTime inDateTime)
{
   _rep = new CQLValueRep(inDateTime);
}

CQLValue::CQLValue(Uint64 inUint)
{
   _rep = new CQLValueRep(inUint);
}

CQLValue::CQLValue(Boolean inBool)
{
   _rep = new CQLValueRep(inBool);
}

CQLValue::CQLValue(Sint64 inSint)
{
   _rep = new CQLValueRep(inSint);
}

CQLValue::CQLValue(Real64 inReal)
{
   _rep = new CQLValueRep(inReal);
}

CQLValue::CQLValue(CQLValueRep rhs)
{
   _rep = new CQLValueRep(&rhs);
}

void CQLValue::resolve(const CIMInstance& CI,const QueryContext& inQueryCtx)
{   
   _rep->resolve(CI, inQueryCtx);
}

CQLValue& CQLValue::operator=(const CQLValue& rhs)
{
	if(&rhs != this)
   {
		if(_rep) delete _rep;
		_rep = NULL;
		_rep = new CQLValueRep(rhs._rep);
	}
	return *this;
}
Boolean CQLValue::operator==(const CQLValue& x)
{
   return _rep->operator==(x._rep);
}

//##ModelId=40FBFF9502BB
Boolean CQLValue::operator!=(const CQLValue& x)
{
   return _rep->operator!=(x._rep);
}


Boolean CQLValue::operator<=(const CQLValue& x)
{
   return _rep->operator<=(x._rep);
}


Boolean CQLValue::operator>=(const CQLValue& x)
{
   return _rep->operator>=(x._rep);
}


Boolean CQLValue::operator<(const CQLValue& x)
{
   return _rep->operator<(x._rep);
}


Boolean CQLValue::operator>(const CQLValue& x)
{
   return _rep->operator>(x._rep);
}


CQLValue CQLValue::operator+(const CQLValue& x)
{
   return CQLValue(_rep->operator+(x._rep));
}

/*
CQLValue CQLValue::operator-(const CQLValue& x)
{
   return CQLValue(_rep->operator-(x._rep));
}

CQLValue CQLValue::operator*(const CQLValue& x)
{
   return CQLValue(_rep->operator*(x._rep));
}


CQLValue CQLValue::operator/(const CQLValue& x)
{
   return CQLValue(_rep->operator/(x._rep));
}
*/
//##ModelId=40FC3F6F0302
CQLValue::CQLValueType CQLValue::getValueType()
{
   return _rep->getValueType();
}


void CQLValue::setNull()
{
   _rep->setNull();
}


Boolean CQLValue::isResolved()
{
   return _rep->isResolved();
}


Boolean CQLValue::isNull()
{
   return _rep->isNull();
}


Boolean CQLValue::isa(const CQLChainedIdentifier& cid, QueryContext& QueryCtx)
{
   return _rep->isa(cid, QueryCtx);
}


Boolean CQLValue::like(const CQLValue& inVal)
{
   return _rep->like(inVal._rep);   
}

/*
void CQLValue::invert()
{
   _rep->invert();
}
*/

CQLChainedIdentifier CQLValue::getChainedIdentifier()const
{
   return _rep->getChainedIdentifier();
}

Uint64 CQLValue::getUint()const
{
   return _rep->getUint();
}

Boolean CQLValue::getBool()const
{
   return _rep->getBool();
}

Sint64 CQLValue::getSint()const
{
   return _rep->getSint();
}

Real64 CQLValue::getReal()const
{
   return _rep->getReal();
}

String CQLValue::getString()const
{
   return _rep->getString();
}

CIMDateTime CQLValue::getDateTime()const
{
   return _rep->getDateTime();
}

CIMObjectPath CQLValue::getReference()const
{
   return _rep->getReference();
}

CIMInstance CQLValue::getInstance()const
{
   return _rep->getInstance();
}

CIMClass CQLValue::getClass()const
{
   return _rep->getClass();
}

String CQLValue::toString()const
{
   return _rep->toString();
}

void CQLValue::applyContext(Pegasus::QueryContext& _ctx,
                           CQLChainedIdentifier& inCid)
{
	_rep->applyContext(_ctx,inCid);
}



PEGASUS_NAMESPACE_END
