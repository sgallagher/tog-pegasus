
#include <iostream>
#include "CQLIdentifier.h"
#include <ctype.h>
#include <cstdlib>
PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLIdentifier
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T


Char16 STAR = '*';
Char16 HASH = '#';
Char16 RBRKT = ']';
Char16 LBRKT = '[';
String SCOPE = "::";

CQLIdentifier::CQLIdentifier(): _isWildcard(false), _isSymbolicConstant(false)
{
	_name = CIMName();
}


CQLIdentifier::CQLIdentifier(String identifier): _isWildcard(false), _isSymbolicConstant(false)
{
	parse(identifier);
}

CQLIdentifier::CQLIdentifier(const CQLIdentifier& id){
	_name = id.getName();
	_indices = id.getSubRanges();
	_symbolicConstant = id.getSymbolicConstantName();
	_scope = id.getScope();
	_isWildcard = id.isWildcard();
}

const CIMName& CQLIdentifier::getName()const
{
	return _name;
}

const String& CQLIdentifier::getSymbolicConstantName()const
{
	return _symbolicConstant;
}

const  Array<SubRange>& CQLIdentifier::getSubRanges()const
{
	return _indices;
}

Boolean CQLIdentifier::isArray()const
{
	return(_indices.size() > 0);
}

Boolean CQLIdentifier::isSymbolicConstant()const
{
	return _isSymbolicConstant;
}

Boolean CQLIdentifier::isWildcard()const
{
	return _isWildcard;
}

const String& CQLIdentifier::getScope()const
{
	return _scope;
}

Boolean CQLIdentifier::isScoped()const
{
	return (_scope != String::EMPTY);
}

void CQLIdentifier::applyScope(String scope)
{
        _scope = scope;
}


Boolean CQLIdentifier::operator==(const CIMName &rhs)const{
	if(_name == rhs) return true;
	return false;
}

Boolean CQLIdentifier::operator!=(const CIMName &rhs)const{
	return(!operator==(rhs));
}

Boolean CQLIdentifier::operator==(const CQLIdentifier &rhs)const{
	
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

Boolean CQLIdentifier::operator!=(const CQLIdentifier &rhs)const{
        return(!operator==(rhs));
}

String CQLIdentifier::toString()const{
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

void CQLIdentifier::parse(String identifier){
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
