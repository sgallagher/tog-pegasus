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

#include "CQLSelectStatement.h"
#include "CQLSelectStatementRep.h"

#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/InternalException.h>
#include "CQLValue.h"
#include "CQLIdentifier.h"
#include "CQLChainedIdentifier.h"


PEGASUS_NAMESPACE_BEGIN

struct EmbeddedPropertyNode
{
  CIMName name;
  Boolean wildcard;
  EmbeddedPropertyNode * parent;
  EmbeddedPropertyNode * sibling;
  EmbeddedPropertyNode * firstChild;
};


CQLSelectStatementRep::CQLSelectStatementRep()
  :SelectStatementRep()
{
}

CQLSelectStatementRep::CQLSelectStatementRep(String& inQlang, String& inQuery, QueryContext* inCtx)
  :SelectStatementRep(inQlang, inQuery, inCtx)
{
}

CQLSelectStatementRep::CQLSelectStatementRep(const CQLSelectStatementRep& rep)
  :SelectStatementRep(rep),
   _selectIdentifiers(rep._selectIdentifiers),
   _whereIdentifiers(rep._whereIdentifiers),
   _predicate(rep._predicate)
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

  return *this;
}

Boolean CQLSelectStatementRep::evaluate(const CIMInstance& inCI)
{
  if (!hasWhereClause())
    return true;
  else
    return _predicate.evaluate(inCI, *_ctx);
}

void CQLSelectStatementRep::applyProjection(CIMInstance& inCI) throw(Exception)
{
  // assumes that applyContext had been called.

  EmbeddedPropertyNode* rootNode = new EmbeddedPropertyNode;
  Array<CQLIdentifier> fromList = _ctx->getFromList();
  rootNode->name = fromList[0].getName();  // not doing joins
  
  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    CQLValue val(_selectIdentifiers[i]);

    // ATTN: assuming the CQLValue takes care of class aliasing
    // ATTN: assumes that the instance's class name is first subId.
// ATTN: UNCOMMENT when API is available
CQLChainedIdentifier resolvedId; // = val.getResolvedIdentifier(inCI, *_ctx);
    Array<CQLIdentifier> ids = resolvedId.getSubIdentifiers();

    EmbeddedPropertyNode * curNode = rootNode;
    EmbeddedPropertyNode * curChild = curNode->firstChild;

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
	  curChild = curChild->sibling;
	}
      }

      if (!found)
      {
	curChild = new EmbeddedPropertyNode;
	curChild->parent = curNode;
	curChild->sibling = curNode->firstChild;
	curChild->name = ids[j].getName();
	curNode->firstChild = curChild;
	curNode->wildcard = false;
      }

      curNode = curChild;
      curChild = curNode->firstChild;
    }
  }

  Array<CIMName> requiredProps;

  EmbeddedPropertyNode* projNode = rootNode->firstChild;
  while (projNode != NULL)
  {
    if (!projNode->wildcard && !(projNode->firstChild == NULL))
    {
      Uint32 index = inCI.findProperty(projNode->name);
      CIMProperty projProp = inCI.getProperty(index);
      inCI.removeProperty(index);
      applyProjection(projNode, projProp);
      inCI.addProperty(projProp);
    }

    // ATTN: what to do about selecting one element of an array.  Is this allowed
    // in basic, and if so, then it would cause a property type change.  
    // Line 461.  May need to call CQLValue to figure this out.

    if (!projNode->wildcard)
      requiredProps.append(projNode->name);   
 
    projNode = projNode->sibling;
  }

  if (!projNode->wildcard)
    removeUnneededProperties(inCI, requiredProps);

  // ATTN delete the tree  

}

// spec compliance
// assertions
// optimize
// Need a func to check well formed identifiers - ie. all emb props are scoped.  Or does
// applyContext or bison do that?

void CQLSelectStatementRep::applyProjection(EmbeddedPropertyNode* node,
					    CIMProperty& nodeProp)
{
  if (node->wildcard)
    return;

  PEGASUS_ASSERT(node->firstChild != NULL);

  CIMInstance nodeInst;
  CIMValue nodeVal = nodeProp.getValue();
// ATTN - UNCOMMENT when emb objs are supported
//PEGASUS_ASSERT(nodeVal.getType() == CIMTYPE_INSTANCE);
//nodeVal.get(nodeInst);

  Array<CIMName> requiredProps;

  EmbeddedPropertyNode * curChild = node->firstChild;
  while (curChild != NULL)
  {
    if (curChild->firstChild != NULL)
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

    requiredProps.append(curChild->name);

    curChild = curChild->sibling;
  }

  removeUnneededProperties(nodeInst, requiredProps);

// ATTN - UNCOMMENT when emb objs are supported
//CIMValue newNodeVal(nodeInst);
//nodeProp.setValue(newNodeVal);
} 

void CQLSelectStatementRep::removeUnneededProperties(CIMInstance& inst, 
						     Array<CIMName>& requiredProps)
{
  for (Uint32 i = 0; i < inst.getPropertyCount(); i++)
  {
    Boolean found = false;
    for (Uint32 j = 0; j < requiredProps.size(); j++)
    {
      if (inst.getProperty(i).getName() == requiredProps[j])
      {
	found = true;
	break;
      }  
    }

    if (!found)
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

void CQLSelectStatementRep::validateProperties() throw(Exception)
{
  // assumes applyContext has been called

  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    validateProperty(_selectIdentifiers[i]);
  }

  for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
  {
    validateProperty(_whereIdentifiers[i]);
  }
}

void CQLSelectStatementRep::validateProperty(CQLChainedIdentifier& chainId)
{
  // assumes that applyContext has been called and all identfiers are well-formed

  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  // Normalize to prepend the FROM class
  // see the description of CQLIdentifier::applyContext
  CIMName prevContext;
  Uint32 startingPos = 0;
  if (ids[0].isScoped())
  {
    Array<CQLIdentifier> fromList = _ctx->getFromList();
    PEGASUS_ASSERT(fromList.size() == 1);   // no joins yet
    prevContext = fromList[0].getName();
  }
  else
  {
    prevContext = ids[0].getName();
    startingPos = 1;
  }
  
  for (Uint32 pos = startingPos; pos < ids.size(); pos++)
  {
    CIMName classContext;
    if (ids[pos].isScoped())
    {
      classContext = lookupFromClass(ids[pos].getScope());
      if (classContext.getString() == String::EMPTY)
      {
	classContext = CIMName(ids[pos].getScope());
      }      
    }
    else
    {
      PEGASUS_ASSERT(pos == 1);
      classContext = prevContext;
    }

    CIMClass classDef = _ctx->getClass(classContext);
    if (classDef.findProperty(ids[pos].getName()) == PEG_NOT_FOUND)
    {
      throw Exception("TEMP MSG: prop not on scoped class");
    }

    if (!classContext.equal(prevContext))
    {
      if (!isSubClass(classContext, prevContext) &&
	  !isSubClass(prevContext, classContext))
      {
	throw Exception("TEMP MSG: section 5.4.1 violation!");
      }
    }

    prevContext = classContext;
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
  // assumes that applyContext had been called.

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
  // Assumes that applyContext had been called
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

void CQLSelectStatementRep::insertClassPathAlias(const CQLIdentifier& inIdentifier, String inAlias)
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
//   ATTN - wait for applyContext API
  /*
  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    _selectIdentifiers[i].applyContext(*_ctx);
  }

  for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
  {
    _whereIdentifiers[i].applyContext(*_ctx);
  }

  if (hasWhereClause())
    _predicate.applyContext(*_ctx);
  */
}

void CQLSelectStatementRep::normalizeToDOC()
{

}

String CQLSelectStatementRep::toString()
{
    printf("CQLSelectStatementRep::toString()\n");
	String s("SELECT ");
	for(Uint32 i = 0; i < _selectIdentifiers.size(); i++){
		s.append(_selectIdentifiers[i].toString());
	}	
	s.append(" FROM ");
	Array<CQLIdentifier> _ids = _ctx->getFromList();
	for(Uint32 i = 0; i < _ids.size(); i++){
		s.append(_ids[i].toString());
	}
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
}

PEGASUS_NAMESPACE_END
