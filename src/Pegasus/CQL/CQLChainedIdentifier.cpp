#include "CQLChainedIdentifier.h"
//#include <Pegasus/CQL/CQLScope.h>
PEGASUS_NAMESPACE_BEGIN

CQLChainedIdentifier::CQLChainedIdentifier(String inString)
{
	parse(inString);
}

CQLChainedIdentifier::CQLChainedIdentifier(CQLIdentifier &id)
{
        _subIdentifiers.append(id);
}

CQLChainedIdentifier::CQLChainedIdentifier(const CQLChainedIdentifier& cid){
	_subIdentifiers = cid.getSubIdentifiers();	
}

const Array<CQLIdentifier>& CQLChainedIdentifier::getSubIdentifiers()const
{
	return _subIdentifiers;
}

CQLIdentifier CQLChainedIdentifier::getLastIdentifier(){
	if(_subIdentifiers.size() > 0)
		return _subIdentifiers[_subIdentifiers.size()-1];
	return CQLIdentifier();
}

String CQLChainedIdentifier::toString()const{
	String s;
	for(Uint32 i = 0; i < _subIdentifiers.size(); i++){
		s.append(_subIdentifiers[i].toString());
		if(i < _subIdentifiers.size() - 1)
			s.append(".");
	}
	return s;
}

void CQLChainedIdentifier::append(CQLIdentifier & id){
	_subIdentifiers.append(id);
}

Boolean CQLChainedIdentifier::isSubChain(CQLChainedIdentifier & chain){
	Array<CQLIdentifier> ids = chain.getSubIdentifiers();
	for(Uint32 i = 0; i < ids.size(); i++){
		if(ids[i] != _subIdentifiers[i].getName())
			return false;
	}
	return true;
}

CQLIdentifier& CQLChainedIdentifier::operator[](Uint32 index){
	return _subIdentifiers[index];
}

Uint32 CQLChainedIdentifier::size(){
	return _subIdentifiers.size();
}

Boolean CQLChainedIdentifier::prepend(CQLIdentifier & id){
	/*
	   Compare id against the first element in _subIdentifiers, 
	   if not an exact match, then prepend.  This is used to fully
	   qualify the chained identifier.
	*/
	if(id != _subIdentifiers[0]){
		_subIdentifiers.prepend(id);
		return true;
	}
	return false;
}

void CQLChainedIdentifier::applyScopes(Array<CQLScope>& scopes){

}

void CQLChainedIdentifier::parse(String & string){
	/* 
	  - parse string on "."
	  - start from the end of string
	  - if more than one substring found, 
		-- store first found string then
		-- prepend remaining substrings 
	*/
	Char16 delim('.');
	Uint32 index;
	String range;

	/* remove any array ranges so we dont parse a false . */
	if((index = string.find("[")) != PEG_NOT_FOUND){
		range = string.subString(index);
		string.remove(index);
	}

	index = string.reverseFind(delim);
	if(index == PEG_NOT_FOUND){
		/* append the range we may have removed */
		string.append(range);
		_subIdentifiers.append(CQLIdentifier(string));
	}else{
		/* append the range we may have removed */
		String tmp = string.subString(index+1);
		tmp.append(range);
		PEGASUS_STD(cout) << "tmp = " << tmp << PEGASUS_STD(endl);
		_subIdentifiers.append(CQLIdentifier(tmp));

		while(index != PEG_NOT_FOUND){
			tmp = string.subString(0,index);
			index = tmp.reverseFind(delim);
			if(index == PEG_NOT_FOUND){
				_subIdentifiers.prepend(CQLIdentifier(tmp));
			}
			else{
				_subIdentifiers.prepend(CQLIdentifier(tmp.subString(index+1)));
			}
		}
	}
}

PEGASUS_NAMESPACE_END
