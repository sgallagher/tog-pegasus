//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L. P.;
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

#include "CQLChainedIdentifier.h"
#include "CQLChainedIdentifierRep.h"
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

CQLChainedIdentifierRep::CQLChainedIdentifierRep(){
//	printf("CQLChainedIdentifier()\n");
}

CQLChainedIdentifierRep::CQLChainedIdentifierRep(String inString)
{
	parse(inString);
}

CQLChainedIdentifierRep::CQLChainedIdentifierRep(CQLIdentifier &id)
{
        _subIdentifiers.append(id);
//	printf("CQLChainedIdentifier(CQLIdentifier &id)\n");
}

CQLChainedIdentifierRep::CQLChainedIdentifierRep(const CQLChainedIdentifierRep* rep){
	_subIdentifiers = rep->_subIdentifiers;
//	printf("CQLChainedIdentifier COPY CONSTR\n");	
}

CQLChainedIdentifierRep::~CQLChainedIdentifierRep(){
	
//	printf("~CQLChainedIdentifierRep()\n");
}

const Array<CQLIdentifier>& CQLChainedIdentifierRep::getSubIdentifiers()const
{
	return _subIdentifiers;
}

CQLIdentifier CQLChainedIdentifierRep::getLastIdentifier(){
	if(_subIdentifiers.size() > 0)
		return _subIdentifiers[_subIdentifiers.size()-1];
	return CQLIdentifier();
}

String CQLChainedIdentifierRep::toString()const{
	String s;
	for(Uint32 i = 0; i < _subIdentifiers.size(); i++){
		s.append(_subIdentifiers[i].toString());
		if(i < _subIdentifiers.size() - 1)
			s.append(".");
	}
	return s;
}

void CQLChainedIdentifierRep::append(CQLIdentifier & id){
	_subIdentifiers.append(id);
}

Boolean CQLChainedIdentifierRep::isSubChain(CQLChainedIdentifier & chain){
	Array<CQLIdentifier> ids = chain.getSubIdentifiers();
	for(Uint32 i = 0; i < ids.size(); i++){
		if(ids[i] != _subIdentifiers[i].getName())
			return false;
	}
	return true;
}

CQLIdentifier& CQLChainedIdentifierRep::operator[](Uint32 index){
	return _subIdentifiers[index];
}

CQLChainedIdentifierRep& CQLChainedIdentifierRep::operator=(const CQLChainedIdentifierRep& rhs){
	if(&rhs != this){
		_subIdentifiers = rhs._subIdentifiers;
//		printf("CQLChainedIdentifierRep::operator=\n");
	}
	return *this;
}

Uint32 CQLChainedIdentifierRep::size(){
	return _subIdentifiers.size();
}

Boolean CQLChainedIdentifierRep::prepend(CQLIdentifier & id){
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

void CQLChainedIdentifierRep::applyContext(QueryContext& inContext)
{
  CQLIdentifier firstId = _subIdentifiers[0];
  
  if (!firstId.isScoped())
  {
    Array<CQLIdentifier> fromList = inContext.getFromList();

    CQLIdentifier matchedId = inContext.findClass(firstId.getName().getString());

    if (firstId.isWildcard())
    {
      // Example:  SELECT * FROM F AS A 
      _subIdentifiers.prepend(fromList[0]);
    }  
    else
    {
      if (matchedId.getName().getString() == String::EMPTY)
      {
	// Could not find the firstId in the FROM list.
	// Assume the firstId is a property, so prepend the FROM class.
	// Examples:  
	// SELECT p FROM F AS A  
	// SELECT p.p1 FROM F AS A (illegal, but caught elsewhere)  
        // SELECT p.* FROM F AS A 
        // SELECT * FROM F AS A WHERE p ISA B
	_subIdentifiers.prepend(fromList[0]);
      }
      else
      {	
	// The firstId was found in the FROM list, but it could have been 
	// an alias
	if (!matchedId.getName().equal(firstId.getName()))
	{
	  // It was an alias.
	  // Replace the alias with the FROM class
	  // Examples:  
	  // SELECT A.p FROM F AS A 
	  // SELECT A FROM F AS A  (illegal, but caught elsewhere) 
	  // SELECT A.* FROM F AS A
	  // SELECT * FROM F AS A WHERE A ISA B 
	  // SELECT * FROM F AS A WHERE A.p ISA B
	  _subIdentifiers[0] = matchedId;
        }
	else 
	{
	  // It was not an alias, and it is the only sub-identifier.
	  // Do nothing.
	  // Examples:
	  // SELECT F.p FROM F AS A
	  // SELECT F FROM F AS A (illegal, but caught elsewhere) 
	  // SELECT F.* FROM F AS A
	  // SELECT * FROM F AS A WHERE F ISA B
	  // SELECT * FROM F AS A WHERE F.p ISA B
	  ;
	}
      }
    }
  }
}

void CQLChainedIdentifierRep::parse(String & string){
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
