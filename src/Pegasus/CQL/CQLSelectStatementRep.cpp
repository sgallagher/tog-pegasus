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

#include "CQLSelectStatement.h"
#include "CQLSelectStatementRep.h"

#include <iostream>

#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/AutoPtr.h>
#include "CQLValue.h"
#include "CQLIdentifier.h"
#include "CQLChainedIdentifier.h"

// ATTN: TODOs - 
// spec compliance
// assertions
// optimize
// localized CQL exceptions
// documentation
// trace? but this could be used by provider

PEGASUS_NAMESPACE_BEGIN

struct PropertyNode
{
  CIMName name;
  Array<CIMName> scopes;
  Boolean wildcard;
  AutoPtr<PropertyNode> sibling;
  AutoPtr<PropertyNode> firstChild;

  PropertyNode() {/*PEGASUS_STD(cout) << "new " << this << PEGASUS_STD(endl);*/}
  ~PropertyNode() {/*PEGASUS_STD(cout) << "delete " << this << PEGASUS_STD(endl);*/}
};


CQLSelectStatementRep::CQLSelectStatementRep()
  :SelectStatementRep(),
   _hasWhereClause(false),
   _contextApplied(false)
{
}

CQLSelectStatementRep::CQLSelectStatementRep(String& inQlang,
					     String& inQuery,
					     QueryContext& inCtx)
  :SelectStatementRep(inQlang, inQuery, inCtx),
   _hasWhereClause(false),
   _contextApplied(false)
{
}

CQLSelectStatementRep::CQLSelectStatementRep(const CQLSelectStatementRep& rep)
  :SelectStatementRep(rep),
   _selectIdentifiers(rep._selectIdentifiers),
   _whereIdentifiers(rep._whereIdentifiers),
   _hasWhereClause(rep._hasWhereClause),
   _predicate(rep._predicate),
   _contextApplied(rep._contextApplied)
{
}

CQLSelectStatementRep::~CQLSelectStatementRep()
{
}

CQLSelectStatementRep& CQLSelectStatementRep::operator=(const CQLSelectStatementRep& rhs)
{
  if (this ==  &rhs)
    return *this;

  SelectStatementRep::operator=(rhs);

  _whereIdentifiers = rhs._whereIdentifiers;
  _selectIdentifiers = rhs._selectIdentifiers;
  _predicate = rhs._predicate;
  _contextApplied = rhs._contextApplied;
  _hasWhereClause = rhs._hasWhereClause;

  return *this;
}

Boolean CQLSelectStatementRep::evaluate(const CIMInstance& inCI)
{
  if (!hasWhereClause())
  {
    return true;
  }
  else
  {
    if (!_contextApplied)
      applyContext();

    try
    {
      return _predicate.evaluate(inCI, *_ctx);
    }
    catch (UninitializedObjectException& )
    {
      // The null contagion rule.
      // ATTN change this to a specific CQLException
      return false;
    }
  }
}

void CQLSelectStatementRep::applyProjection(CIMInstance& inCI) throw(Exception)
{
  if (!_contextApplied)
    applyContext();

  AutoPtr<PropertyNode> rootNode(new PropertyNode);
  Array<CQLIdentifier> fromList = _ctx->getFromList();
  rootNode->name = fromList[0].getName();  // not doing joins

  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    Array<CQLIdentifier> ids = _selectIdentifiers[i].getSubIdentifiers();

    // ATTN optimize to build the tree once and validate vs schema once

    PropertyNode * curNode = rootNode.get();
    PropertyNode * curChild = curNode->firstChild.get();

    for (Uint32 j = 1; j < ids.size(); j++)
    {
      if (ids[j].isWildcard())
      {
	curNode->wildcard = true;
	break;
      }

      Boolean found = false;
      while (curChild != NULL && !found)
      {
	if (curChild->name == ids[j].getName())
        {
	  found = true;
	}
	else
        {
	  curChild = curChild->sibling.get();
	}
      }

      if (!found)
      {
	curChild = new PropertyNode;
	curChild->sibling = curNode->firstChild;
	curChild->name = ids[j].getName();

	if (ids[j].isScoped())
	{
	  CQLIdentifier matchId  = _ctx->findClass(ids[j].getScope());
	  CIMName scope;
	  if (matchId.getName().getString() != String::EMPTY)
	  {
	    scope = matchId.getName();
	  }
	  else
	  {
	    scope = CIMName(ids[j].getScope());
	  }
	  
	  Uint32 i = 0;
	  for (; i < curChild->scopes.size(); i++)
	  {
	    if (curChild->scopes[i] == scope)
	      break;
	  }
	  if (i == curChild->scopes.size())
	  {
	    curChild->scopes.append(scope);
	  }
 	}

	curChild->wildcard = false;
	curNode->firstChild.reset(curChild);  // safer than using the = operator
      }

      curNode = curChild;
      curChild = curNode->firstChild.get();
    }
  }

  Array<CIMName> requiredProps;
  CIMName requiredScope;
  PropertyNode* childNode = rootNode->firstChild.get();
  while (childNode != NULL)
  {
    if (childNode->firstChild.get() != NULL)
    {
      Uint32 index = inCI.findProperty(childNode->name);
      CIMProperty childProp = inCI.getProperty(index);
      inCI.removeProperty(index);
      applyProjection(childNode, childProp);
      inCI.addProperty(childProp);
    }

    // ATTN: what to do about selecting one element of an array.  Is this allowed
    // in basic, and if so, then it would cause a property type change.  
    // Line 461.  May need to call CQLValue to figure this out.

    // ATTN: Line 636-640 - does this only apply to basic?

    if (!rootNode->wildcard)
      requiredProps.append(childNode->name);   

    for (Uint32 i = 0; i < childNode->scopes.size(); i++)    
    {
      if (childNode->scopes[i] == requiredScope)
      {
	continue;
      }

      if (requiredScope.isNull() || 
	  isSubClass(childNode->scopes[i], requiredScope))
      {
	requiredScope = childNode->scopes[i];
      }
      else if (!isSubClass(requiredScope, childNode->scopes[i]))
      {
	// note: no multiple inheritance in CIM
	throw Exception("TEMP MSG instance cannot be all the required scopes");
      }
    }
 
    childNode = childNode->sibling.get();
  }

  if (requiredScope.getString() == String::EMPTY)
  {
    requiredScope = fromList[0].getName(); 
  }

  if (!inCI.getClassName().equal(requiredScope) &&
      !isSubClass(inCI.getClassName(),requiredScope))
  {
    throw Exception("TEMP MSG - indication is not of the required scope");
  }

  removeUnneededProperties(inCI, requiredProps, requiredScope);
}

void CQLSelectStatementRep::applyProjection(PropertyNode* node,
					    CIMProperty& nodeProp)
{
  PEGASUS_ASSERT(node->firstChild.get() != NULL);

  CIMInstance nodeInst;
  CIMValue nodeVal = nodeProp.getValue();
// ATTN - UNCOMMENT when emb objs are supported
//PEGASUS_ASSERT(nodeVal.getType() == CIMTYPE_INSTANCE);
//nodeVal.get(nodeInst);

  Array<CIMName> requiredProps;
  CIMName requiredScope;

  PropertyNode * curChild = node->firstChild.get();
  while (curChild != NULL)
  {
    if (curChild->firstChild.get() != NULL)
    {
      Uint32 index = nodeInst.findProperty(curChild->name);
      CIMProperty childProp = nodeInst.getProperty(index); 
      nodeInst.removeProperty(index);
      applyProjection(curChild, childProp);
      nodeInst.addProperty(childProp);
    }

    // ATTN: what to do about selecting one element of an array.  Is this allowed
    // in basic, and if so, then it would cause a property type change.  
    // Line 461.  May need to call CQLValue to figure this out.

    if (!node->wildcard)
    {
      requiredProps.append(curChild->name);
    }

    for (Uint32 i = 0; i < curChild->scopes.size(); i++)    
    {
      if (curChild->scopes[i] == requiredScope)
      {
	continue;
      }

      if (requiredScope.isNull() || 
	  isSubClass(curChild->scopes[i], requiredScope))
      {
	requiredScope = curChild->scopes[i];
      }
      else if (!isSubClass(requiredScope, curChild->scopes[i]))
      {
	// note: no multiple inheritance in CIM
	throw Exception("TEMP MSG instance cannot be all the required scopes");
      }
    }

    curChild = curChild->sibling.get();
  }

  PEGASUS_ASSERT(requiredScope.getString() != String::EMPTY);

  if (!nodeInst.getClassName().equal(requiredScope) &&
      !isSubClass(nodeInst.getClassName(),requiredScope))
  {
    throw Exception("TEMP MSG - indication is not of the required scope");
  }

  removeUnneededProperties(nodeInst, requiredProps, requiredScope);

// ATTN - UNCOMMENT when emb objs are supported
//CIMValue newNodeVal(nodeInst);
//nodeProp.setValue(newNodeVal);
} 

void CQLSelectStatementRep::removeUnneededProperties(CIMInstance& inst, 
						     Array<CIMName>& requiredProps,
						     CIMName requiredScope)
{
  if (requiredProps.size() == 0)
  {
    CIMClass cls = _ctx->getClass(requiredScope);
    Array<CIMName> clsProps;
    for (Uint32 i = 0; i < cls.getPropertyCount(); i++)
    {
      requiredProps.append(cls.getProperty(i).getName());
    }
  }

  Array<CIMName> supportedProps;
  for (Uint32 i = 0; i < inst.getPropertyCount(); i++)
  {
    supportedProps.append(inst.getProperty(i).getName());
  }
  
  for (Uint32 i = 0; i < requiredProps.size(); i++)
  {
    if (!containsProperty(requiredProps[i], supportedProps))
    {
      throw Exception("TEMP MSG - instance missing required property");
    }
  }

  for (Uint32 i = 0; i < supportedProps.size(); i++)
  {
    if (!containsProperty(supportedProps[i], requiredProps))
    {
      inst.removeProperty(i);
    }
  }
}

void CQLSelectStatementRep::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
  Array<CQLIdentifier> fromList = _ctx->getFromList();
  PEGASUS_ASSERT(fromList.size() == 1);  // no joins yet

  if (!inClassName.getClassName().equal(fromList[0].getName()))
  {
    throw Exception("TEMP MSG:  not in the FROM list ");
  }

  try
  {
    _ctx->getClass(inClassName.getClassName());
  }
  catch (CIMException& ce)
  {
    if (ce.getCode() == CIM_ERR_INVALID_CLASS || 
	ce.getCode() == CIM_ERR_NOT_FOUND)
    {
      // ATTN may just want to throw the CIMException rather than
      // CQL exception
      throw Exception("TEMP MSG - class does not exist");
    }
  }
}

//
// Validates that all the chained identifiers in the statement meet
// the rules in the CQL spec vs.the class definitions in the repository
//
void CQLSelectStatementRep::validateProperties() throw(Exception)
{
  if (!_contextApplied)
    applyContext();

  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    validateProperty(_selectIdentifiers[i]);
  }

  for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
  {
    validateProperty(_whereIdentifiers[i]);
  }
}

//
// Validates that the chained identifier meets all the rules in the CQL
// spec vs.the class definitions in the repository
//
void CQLSelectStatementRep::validateProperty(CQLChainedIdentifier& chainId)
{
  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  // Determine the starting class context.
  // See CQLIdentifier::applyContext for a description
  // of the form of the identifier after applyContext.
  CIMName startingContext;
  Uint32 startingPos = 0;
  if (ids[0].isScoped())
  { 
    // The first element is scoped, therefore the FROM class
    // was not prepended.  Get the starting context from the FROM
    Array<CQLIdentifier> fromList = _ctx->getFromList();
    PEGASUS_ASSERT(fromList.size() == 1);   // no joins yet
    startingContext = fromList[0].getName();
  }
  else
  {
    // First element was not scoped, therefore the FROM class
    // was prepended.  Use the first element as the starting context.
    startingContext = ids[0].getName();
    startingPos = 1;
  }

  CIMName curContext;
  for (Uint32 pos = startingPos; pos < ids.size(); pos++)
  {
    // Determine the current class context
    if (ids[pos].isScoped())
    {
      // The chain element is scoped.  Lookup the scope
      // name in the FROM list, taking into account
      // aliases.
      curContext = lookupFromClass(ids[pos].getScope());
      if (curContext.getString() == String::EMPTY)
      {
	// It was not in the FROM list, just use the scope name.
	curContext = CIMName(ids[pos].getScope());
      }      
    }
    else
    {
      PEGASUS_ASSERT(pos == startingPos || ids[pos].isWildcard());
      curContext = startingContext;
    }

    // Get the class definition of the current class context
    CIMClass classDef;
    try 
    {
      classDef = _ctx->getClass(curContext);
    }
    catch (CIMException& ce)
    {
      if (ce.getCode() == CIM_ERR_NOT_FOUND ||
	  ce.getCode() == CIM_ERR_INVALID_CLASS)
      {
	// ATTN: better to just throw the CIMException rather
	// than the CQL exception?
	throw Exception("TEMP_MSG: class context does not exist");
      }
    }

    // Determine if the property name at the current position
    // exists on the current class context.
    if (classDef.findProperty(ids[pos].getName()) == PEG_NOT_FOUND)
    {
      throw Exception("TEMP MSG: prop not on scoped class");
    }

    // Checking class relationship rules in section 5.4.1.
    // For validateProperties, this only applies to the first
    // property in the chain,
    if ((pos == startingPos) && !curContext.equal(startingContext))
    {
      if (!isSubClass(curContext, startingContext) &&
	  !isSubClass(startingContext, curContext))
      {
	throw Exception("TEMP MSG: section 5.4.1 violation!");
      }
    }
  }
}

CIMName CQLSelectStatementRep::lookupFromClass(const String&  lookup)
{
  CQLIdentifier id = _ctx->findClass(lookup);

  return id.getName();
}

Array<CIMObjectPath> CQLSelectStatementRep::getClassPathList()
{
  Array<CQLIdentifier> ids = _ctx->getFromList();
  PEGASUS_ASSERT(ids.size() == 1);  // no joins yet

  // No wbem-uri support yet.
  CIMObjectPath path(String::EMPTY, _ctx->getNamespace(), ids[0].getName());

  Array<CIMObjectPath> paths;
  paths.append(path);

  return paths;
}

CIMPropertyList CQLSelectStatementRep::getPropertyList(const CIMObjectPath& inClassName)
{
  if (!_contextApplied)
    applyContext();

  // check if namespace matches default namespace?
  
  // No wbem-uri support yet
  CIMName className = inClassName.getClassName();
  CIMClass theClass = _ctx->getClass(className);
 
  Boolean isWildcard;
  Array<CIMName> reqProps;
  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    isWildcard = addRequiredProperty(reqProps, theClass, _selectIdentifiers[i]);
    
    if (isWildcard)
    {
      return CIMPropertyList();
    }
  }

  for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
  {
    isWildcard = addRequiredProperty(reqProps, theClass, _whereIdentifiers[i]);

    PEGASUS_ASSERT(!isWildcard);
  }

  Uint32 propCnt = theClass.getPropertyCount();
  Boolean allProps = true;
  for (Uint32 i = 0; i < propCnt; i++)
  {
    if (!containsProperty(theClass.getProperty(i).getName(), reqProps))
    {
      allProps = false;
      break;
    }
  }

  if (allProps)
  {
    return CIMPropertyList();
  }
  else
  {
    return CIMPropertyList(reqProps);
  }
}

Boolean CQLSelectStatementRep::addRequiredProperty(Array<CIMName>& reqProps,
						     CIMClass& theClass,
						     CQLChainedIdentifier& chainId)
{
  // Does not look at properties on embedded objects

  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  // see the description of CQLIdentifier::applyContext

  if (ids[0].isScoped())
  {
    PEGASUS_ASSERT(!ids[0].isWildcard());

    // Check if the scoped property is exposed by the class passed in.
    if (theClass.findProperty(ids[0].getName()) != PEG_NOT_FOUND) 
    {
      CIMName scopingClass = lookupFromClass(ids[0].getScope());
      if (scopingClass.getString() == String::EMPTY)
      {
	scopingClass = CIMName(ids[0].getScope());
      }    

      // Check if the scoping class is a subclass of the class passed in
      Boolean sub  = isSubClass(scopingClass, theClass.getClassName());
      
      // Add to the required properties only if the scoping class 
      // is not a subclass of the class passed in
      if (!sub)
      {
	if (!containsProperty(ids[0].getName(), reqProps))
	{  
	  reqProps.append(ids[0].getName());
	}
      }
    }
  }
  else
  {
    if (ids[1].isWildcard())
    {
      return true;
    }

    if (theClass.findProperty(ids[1].getName()) != PEG_NOT_FOUND) 
    {
      if (!containsProperty(ids[1].getName(), reqProps))
      {  
	reqProps.append(ids[1].getName());
      }
    }
  }
  
  return false;
}

Boolean CQLSelectStatementRep::containsProperty(const CIMName& name,
						const Array<CIMName>& props) 
{
  for (Uint32 i = 0; i < props.size(); i++)
  {
    if (props[i] == name)
    {
      return true;
    }
  }

  return false;
}

Boolean CQLSelectStatementRep::isSubClass(const CIMName& derived,
					  const CIMName& base)
{
  Array<CIMName> subClasses = _ctx->enumerateClassNames(base);
  for (Uint32 i = 0; i < subClasses.size(); i++)
  {
    if (subClasses[i] == derived)
    {
      return true;
    }	
  }
  
  return false;
}

void CQLSelectStatementRep::appendClassPath(const CQLIdentifier& inIdentifier)
{
  _ctx->insertClassPath(inIdentifier);
}

void CQLSelectStatementRep::setPredicate(CQLPredicate inPredicate)
{
  _predicate = inPredicate;
}

void CQLSelectStatementRep::insertClassPathAlias(const CQLIdentifier& inIdentifier,
						 String inAlias)
{
  _ctx->insertClassPath(inIdentifier,inAlias);
}

void CQLSelectStatementRep::appendSelectIdentifier(const CQLChainedIdentifier& x)
{
  _selectIdentifiers.append(x);
}

Boolean CQLSelectStatementRep::appendWhereIdentifier(const CQLChainedIdentifier& x)
{
  _whereIdentifiers.append(x);
  return true;
}

void CQLSelectStatementRep::applyContext()
{
  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    _selectIdentifiers[i].applyContext(*_ctx);
    checkWellFormedIdentifier(_selectIdentifiers[i], true);
  }

  for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
  {
    _whereIdentifiers[i].applyContext(*_ctx);
    checkWellFormedIdentifier(_whereIdentifiers[i], false);
  }

  if (hasWhereClause())
  {
    _predicate.applyContext(*_ctx);
  }

}

void CQLSelectStatementRep::checkWellFormedIdentifier(const CQLChainedIdentifier& chainId,
						      Boolean isSelectListId)
{
  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  Uint32 startingPos = 0;

  if (!ids[0].isScoped())
  {
    if (ids.size() == 1 && isSelectListId)
    {
      throw Exception("TEMP MSG: need to select a property on the FROM class");
    }
    
    startingPos = 1;
  }

  for (Uint32 pos = startingPos; pos < ids.size(); pos++)
  {  
    if (ids[pos].isArray() && isSelectListId)
    {
      throw Exception("TEMP MSG: array indexing not allowed in basic select");
    }

    if (ids[pos].isSymbolicConstant() && isSelectListId)
    {
      throw Exception("TEMP MSG: array indexing not allowed in basic select");
    }

    if (ids[pos].isWildcard())
    {
      if ( !isSelectListId)
      {
	throw Exception("TEMP MSG: wildcard not allowed in WHERE clause");
      }

      if ( pos != ids.size() - 1)
      {
	throw Exception("TEMP MSG: wildcard must be at the end of select-list property");
      }
    }

    if (pos > startingPos && !ids[pos].isWildcard())
    {
      if (!ids[pos].isScoped())
      {
	throw Exception("TEMP MSG: property on embedded object must be scoped");
      }
    }
  }
}

void CQLSelectStatementRep::normalizeToDOC()
{

}

String CQLSelectStatementRep::toString()
{
	String s("SELECT ");
	for(Uint32 i = 0; i < _selectIdentifiers.size(); i++){
		if((i > 0) && (i < _selectIdentifiers.size())){
			s.append(",");
		}
		s.append(_selectIdentifiers[i].toString());
	}	

	s.append(" ");
	s.append(_ctx->getFromString());

	if(_hasWhereClause){
		s.append(" WHERE ");
		s.append(_predicate.toString());
	}
	return s;
}

void CQLSelectStatementRep::setHasWhereClause()
{
        _hasWhereClause = true;
}

Boolean CQLSelectStatementRep::hasWhereClause()
{
        return _hasWhereClause;
}

void  CQLSelectStatementRep::clear()
{
	_ctx->clear();
	_hasWhereClause = false;
	_contextApplied = false;
	_predicate = CQLPredicate();
	_selectIdentifiers.clear();
	_whereIdentifiers.clear();
}

PEGASUS_NAMESPACE_END
