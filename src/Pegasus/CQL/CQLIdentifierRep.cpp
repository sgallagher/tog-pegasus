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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
//#include "CQLIdentifier.h"
#include "CQLIdentifierRep.h"
#include <ctype.h>
#include <cstdlib>
PEGASUS_NAMESPACE_BEGIN


Char16 STAR = '*';
Char16 HASH = '#';
Char16 RBRKT = ']';
Char16 LBRKT = '[';
String SCOPE = "::";

CQLIdentifierRep::CQLIdentifierRep(): _isWildcard(false), _isSymbolicConstant(false)
{
	_name = CIMName();
//	printf("CQLIdentifierRep()\n");
}


CQLIdentifierRep::CQLIdentifierRep(String identifier): _isWildcard(false), _isSymbolicConstant(false)
{
	parse(identifier);
}

CQLIdentifierRep::CQLIdentifierRep(const CQLIdentifierRep* rep){
	_symbolicConstant = rep->_symbolicConstant;
        _scope = rep->_scope;
        _indices = rep->_indices;
        _name = rep->_name;
        _isWildcard = rep->_isWildcard;
        _isSymbolicConstant = rep->_isSymbolicConstant;
}

CQLIdentifierRep::~CQLIdentifierRep(){
//	printf("~CQLIdentifierRep()\n");
}

const CIMName& CQLIdentifierRep::getName()const
{
	return _name;
}

const String& CQLIdentifierRep::getSymbolicConstantName()const
{
	return _symbolicConstant;
}

const  Array<SubRange>& CQLIdentifierRep::getSubRanges()const
{
	return _indices;
}

Boolean CQLIdentifierRep::isArray()const
{
	return(_indices.size() > 0);
}

Boolean CQLIdentifierRep::isSymbolicConstant()const
{
	return _isSymbolicConstant;
}

Boolean CQLIdentifierRep::isWildcard()const
{
	return _isWildcard;
}

const String& CQLIdentifierRep::getScope()const
{
	return _scope;
}

Boolean CQLIdentifierRep::isScoped()const
{
	return (_scope != String::EMPTY);
}

void CQLIdentifierRep::applyScope(String scope)
{
        _scope = scope;
}

CQLIdentifierRep& CQLIdentifierRep::operator=(const CQLIdentifierRep& rhs){
	_symbolicConstant = rhs._symbolicConstant;
    	_scope = rhs._scope;
        _indices = rhs._indices;                                                                                                             
    	_name = rhs._name;                                                                                     
    	_isWildcard = rhs._isWildcard;
    	_isSymbolicConstant = rhs._isSymbolicConstant;
	return *this;
}

Boolean CQLIdentifierRep::operator==(const CIMName &rhs)const{
	if(_name == rhs) return true;
	return false;
}

Boolean CQLIdentifierRep::operator!=(const CIMName &rhs)const{
	return(!operator==(rhs));
}

Boolean CQLIdentifierRep::operator==(const CQLIdentifierRep &rhs)const{
	
	if(_isWildcard && rhs.isWildcard()) return true;
	if(getName() == rhs.getName()){
	   if(getScope() == rhs.getScope()){
		if(getSymbolicConstantName() == rhs.getSymbolicConstantName()){
			// compare _indices arrays
			Uint32 size = rhs.getSubRanges().size();
			if(_indices.size() == size){
			Array<SubRange> rhs_SubRanges = rhs.getSubRanges();
				for(Uint32 i = 0; i < size; i++){
					if(_indices[i] != rhs_SubRanges[i])
						return false;
				}
				return true;
			}
		}
	   }
	}
	return false;
}

Boolean CQLIdentifierRep::operator!=(const CQLIdentifierRep &rhs)const{
        return(!operator==(rhs));
}

String CQLIdentifierRep::toString()const{
printf("CQLIdentifierRep::toString()\n");
	if(_isWildcard)
		return "*";
	String s = getScope();
	if(s != String::EMPTY)
		s.append("::");
	s.append(_name.getString());
	if(_isSymbolicConstant){
		s.append("#").append(_symbolicConstant);
		return s;
	}
	if(isArray()){
		s.append("[");
		for(Uint32 i = 0; i < _indices.size(); i++){
			s.append(_indices[i].toString());
			if(i < _indices.size()-1)
				s.append(",");
		}
		s.append("]");
	}
	return s;
}

void CQLIdentifierRep::parse(String identifier){
	/*
	 - Parse for the following:
         1. A::<scoped string>
	 (a)  property name
         (b)  property[3]     e.g. an array index
         (c)  property#'OK'    e.g. a symbolic constant
         (d)  *   (wildcard)
	 (e)  class name
	 (f)  embedded object
	 (g)  namespace
	*/

	Uint32 index;
	if(identifier == String::EMPTY){
		_name = CIMName();
		return;	
	}
	// basic error check
	if((index = identifier.find(HASH)) != PEG_NOT_FOUND){
		if(((index = identifier.find(RBRKT)) != PEG_NOT_FOUND) || 
			((index = identifier.find(LBRKT)) != PEG_NOT_FOUND))	
		{
			//error
			printf("CQLIdentifier::parse(), error\n");
			return;
		}
	}

	if((index = identifier.find(SCOPE)) != PEG_NOT_FOUND){
		printf("index = %d\n" ,index);
		_scope = identifier.subString(0,index);
		identifier = identifier.subString(index+2);
	}

	if((index = identifier.find(RBRKT)) != PEG_NOT_FOUND){
		if((index = identifier.find(LBRKT)) != PEG_NOT_FOUND){
		  // found array index, parse for ','
		  String range = identifier.subString(index);
		  range = range.subString(1,range.size()-2);  // remove left and right bracket
		  while(index != PEG_NOT_FOUND){
			if((index = range.find(',')) != PEG_NOT_FOUND){
				_indices.append(SubRange(range.subString(0,index)));
				range = range.subString(index+1);
			}else{
				_indices.append(SubRange(range));
			}
		  }
		  // remove ranges from identifier
		  identifier = identifier.subString(0,identifier.find(LBRKT));
		}else{
		  // error
			 printf("CQLIdentifier::parse(), error\n");
			return;
		}
	}else if((index = identifier.find(STAR)) != PEG_NOT_FOUND){
		// wildcard
		_isWildcard = true;
	}else if((index = identifier.find(HASH)) != PEG_NOT_FOUND){
		// symbolic constant
		_isSymbolicConstant = true;
		_symbolicConstant = identifier.subString(index+1);
		identifier = identifier.subString(0,index);
	}

	// name
	if(!_isWildcard){
		try{
			_name = CIMName(identifier);
		}catch(Exception e){
			// throw invalid name exception ?
			printf("CQLIdentifier::parse(), error\n");
			return;
		}
	}
	
}

PEGASUS_NAMESPACE_END
