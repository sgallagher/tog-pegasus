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

#include "QueryContext.h"
#include <Pegasus/Common/System.h>

#include <iostream>

PEGASUS_NAMESPACE_BEGIN
                
QueryContext::QueryContext(CIMNamespaceName& inNS)
  :_NS(inNS)
{
}

QueryContext::QueryContext(const QueryContext& ctx)
  :_NS(ctx._NS),
   _AliasClassTable(ctx._AliasClassTable),
   _fromList(ctx._fromList)
{

}

QueryContext::~QueryContext()
{
}

QueryContext& QueryContext::operator=(const QueryContext& rhs)
{
  if (this == &rhs)
    return *this;

  _NS = rhs._NS;
  _fromList = rhs._fromList;
  _AliasClassTable = rhs._AliasClassTable;

  return *this;
}

String QueryContext::getHost(Boolean fullyQualified)
{
	if(fullyQualified) return System::getFullyQualifiedHostName();
	return System::getHostName();
}

CIMNamespaceName QueryContext::getNamespace() const
{
	return _NS;
}

void QueryContext::insertClassPath(const CQLIdentifier& inIdentifier, String inAlias)
{
  CQLIdentifier _class;  

  if (inIdentifier.getName().getString() == String::EMPTY)
  {
    throw Exception("TEMP MSG QueryContext: not allowed to add empty class name");
  }

  if ((inAlias != String::EMPTY) &&
      (String::equalNoCase(inAlias,inIdentifier.getName().getString())))
  {
    inAlias = String::EMPTY;
  }
 
  if (_AliasClassTable.contains(inIdentifier.getName().getString()))
  {
    throw Exception("TEMP MSG QueryContext: classname is already an alias");
  }

  Boolean found = false;
  for (Uint32 i = 0; i < _fromList.size(); i++)
  {
    if ((inAlias != String::EMPTY) && 
	(String::equalNoCase(inAlias, _fromList[i].getName().getString())))
    {
      throw Exception("TEMP MSG QueryContext: alias is already a classname");
    }

    if (_fromList[i].getName() == inIdentifier.getName())
    {
      found = true;
    }
  }

  if (!found)
  {  
    _fromList.append(inIdentifier);
  }

  if(inAlias != String::EMPTY)
  {
    if (!_AliasClassTable.insert(inAlias, inIdentifier))
    {  
      // Alias already exists
      if (_AliasClassTable.lookup(inAlias, _class))
      {	
	if (!_class.getName().equal(inIdentifier.getName()))
	{
	  throw Exception("TEMP MSG QueryContext: attempted to add conflicting aliases");
	}
      }
    }
  }
}

CQLIdentifier QueryContext::findClass(const String& inAlias)
{
	// look for alias match
	CQLIdentifier _class;
	if(_AliasClassTable.lookup(inAlias, _class)) return _class;

	// look if inAlias is really a class name
	_class = CQLIdentifier(inAlias);
	Array<CQLIdentifier> _identifiers = getFromList();
	for(Uint32 i = 0; i < _identifiers.size(); i++){
		if(_class == _identifiers[i].getName()) return _identifiers[i];
	}	

	// could not find inAlias
	return CQLIdentifier("");
}

Array<CQLIdentifier> QueryContext::getFromList() const 
{
	return _fromList;
}

String QueryContext::getFromString() const
{
  Array<CQLIdentifier> aliasedClasses;
  Array<String> aliases;
  for (HT_Alias_Class::Iterator i = _AliasClassTable.start(); i; i++)
  {  
    aliases.append(i.key());
    aliasedClasses.append(i.value());
  }

  String result("FROM ");
  for (Uint32 i = 0; i < _fromList.size(); i++)
  {
    Boolean hasAlias = false;
    for (Uint32 j = 0; j < aliasedClasses.size(); j++)
    {
      if (_fromList[i].getName() == aliasedClasses[j].getName())
      {
	result.append(_fromList[i].getName().getString());
	result.append(" AS ");
	result.append(aliases[j]);     
	hasAlias = true;
      }
    }

    if (!hasAlias)
    {
      result.append(_fromList[i].getName().getString());      
    }

    if (i < _fromList.size() - 1)
    {
      result.append(" , ");
    }
    else
    {
      result.append(" ");
    }
  }

  return result;
}

void QueryContext::clear(){
  _fromList.clear();
   _AliasClassTable.clear();
}

PEGASUS_NAMESPACE_END

