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
#include <Pegasus/CQL/Cql2Dnf.h>

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
  CIMName name;              // property name
  CIMName scope;             // class the property is on
  Boolean wildcard;          // true if this property is a wildcard 
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

  //
  // Build a tree to represent the projected properties from the select list
  // of chained identifiers.  This is needed because embedded instances below 
  // the FROM class form a tree structure when projected.
  //
  // The design of the tree is to gather all the required properties for 
  // an instance at a node as child nodes.  The root node 
  // of the tree represents the instance passed in to this function.  Below the 
  // root there can be nodes that are required embedded instance properties. 
  // The child nodes of these embedded instance nodes represent the required
  // properties on the embedded instance (which may themselves be embedded instances).
  //
  // Each node has a name, which is in 2 parts -- the property name and the 
  // scope (ie. the class the property is on).  This allows the scoping
  // operator to be handled correctly, so that the parent instance can be
  // checked to see if it is the right class to provide the property.
  // Note that the scoping is a base class; ie. the parent instance of a node
  // may be a subclass of the scope.
  //

  // Set up the root node of the tree.  This represents the instance
  // passed in.
  AutoPtr<PropertyNode> rootNode(new PropertyNode);
  Array<CQLIdentifier> fromList = _ctx->getFromList();
  rootNode->name = fromList[0].getName();  // not doing joins
  rootNode->scope = fromList[0].getName(); // not used on root, just to fill in the var
  rootNode->wildcard = false;
 
  // Build the tree below the root.
  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    // Get the chain elements
    Array<CQLIdentifier> ids = _selectIdentifiers[i].getSubIdentifiers();

    PEGASUS_ASSERT(ids.size() > 1);

    PropertyNode * curNode = rootNode.get();
    PropertyNode * curChild = curNode->firstChild.get();

    // Loop through the identifiers in the chain.
    // NOTE: this starts at the position *after* the FROM class
    // So, the loop index is always one position after the current node,
    // ie. it will become a child node of the current node.
    for (Uint32 j = 1; j < ids.size(); j++)
    {
      // If the child is wildcarded, then every property exposed by the
      // class of the instance at the current node is required.
      // Mark the current node as wildcarded.
      if (ids[j].isWildcard())
      {
        curNode->wildcard = true;
        break;
      }

      // Determine if this identifier is already a child node of
      // the current node.
      Boolean found = false;
      while (curChild != NULL && !found)
      {
        // The scoping class is either the scope of the identifier
        // or the FROM class if the identifier is not scoped.
        String scope = fromList[0].getName().getString();
        if (ids[j].isScoped())
        {
          scope = ids[j].getScope();
        }

        if (curChild->name == ids[j].getName() &&
            String::equalNoCase(curChild->scope.getString(), scope))
        {
          // Name and scope match.  The identifier already has a child node.
          found = true;
        }
        else
        {
          curChild = curChild->sibling.get();
        }
      }

      if (!found)
      {
        // The identifier doesn't already have a child node. 
        // Create a node and add it as a child to the current node.
        curChild = new PropertyNode;
        curChild->sibling = curNode->firstChild;
        curChild->name = ids[j].getName();
        curChild->wildcard = false;
        curNode->firstChild.reset(curChild);  // safer than using the = operator
      }

      // Set the scope for the child node
      if (ids[j].isScoped())
      {
        // Child node has a scoping class
        PEGASUS_ASSERT(ids[j].getScope().size() > 0);
        curChild->scope =  CIMName(ids[j].getScope());
      }
      else
      {
        // Not scoped.  The scope is the FROM class.
        PEGASUS_ASSERT(j == 1);
        PEGASUS_ASSERT(fromList[0].getName().getString().size() > 0);
        curChild->scope = fromList[0].getName();
      }

      curNode = curChild;
      curChild = curNode->firstChild.get();
    }
  }

  //
  // Do the projection.
  //

  Array<CIMName> requiredProps;
  Boolean allPropsRequired = rootNode->wildcard;

  // Loop through the children of the root node.
  // The root node represents the FROM class,
  // and the child nodes are the required properties on the FROM class.
  PropertyNode* childNode = rootNode->firstChild.get();
  while (childNode != NULL)
  {
    // Determine if the instance passed in meets the class scoping
    // rules for the current child node.
    Boolean filterable = isFilterable(inCI, childNode);

    // If the instance is filterable, and the child node has children,
    // or is wildcarded, then the child is assumed to be an embedded instance,
    // and we need to recurse to apply the projection on the embedded instance.
    // (the check for embedded instance is done in the recursive call)
    if (filterable && 
        (childNode->firstChild.get() != NULL || childNode->wildcard))
    {
      // We need to project on an embedded instance property. The steps are to
      // remove the embedded instance property from the instance passed in,
      // project on that embedded instance property, and then add the projected 
      // embedded instance property back to the instance passed in.
      Uint32 index = inCI.findProperty(childNode->name);
      if (index != PEG_NOT_FOUND)
      {
        // The embedded instance has the required embedded instance property.
        CIMProperty childProp = inCI.getProperty(index);
        inCI.removeProperty(index);
        applyProjection(childNode, childProp);
        inCI.addProperty(childProp);
      }
    }

    // If the node is not wildcarded, and the instance passed in
    // is filterable, then add the current child to the list
    // of required properties on the instance passed in.
    if (!allPropsRequired && filterable)
    {
      requiredProps.append(childNode->name);
    }

    childNode = childNode->sibling.get();
  }

  // Remove the properties that are not in the projection.
  // This also checks for missing required properties.
  removeUnneededProperties(inCI,
                           allPropsRequired,
                           fromList[0].getName(),
                           requiredProps);
}

void CQLSelectStatementRep::applyProjection(PropertyNode* node,
                                            CIMProperty& nodeProp)
{
  PEGASUS_ASSERT(node->firstChild.get() != NULL);

  //
  // The property passed in must be an embedded instance. It is not
  // allowed to project properties on embedded classes.
  //
  // Get the embedded instance from the property.
  //

  CIMValue nodeVal = nodeProp.getValue();
// ATTN - UNCOMMENT when emb objs are supported 
/*
  if (nodeVal.getType() != CIMTYPE_OBJECT)
  {
    throw Exception("TEMP MSG: applyProjection - property must be emb obj");
  }
*/

  if (nodeVal.isArray() && 
      (node->firstChild.get() != NULL || node->wildcard))
  {
    // NOTE - since we are blocking projection of array elements, we can
    // assume that if we get here we are projecting a whole array.
    throw Exception("TEMP MSG: applyProjection - not allowed to project properties on whole arrays");
  }

  CIMObject nodeObj;
// ATTN - UNCOMMENT when emb objs are supported 
// nodeVal.get(nodeObj);
  if (!nodeObj.isInstance())
  {
    throw Exception("TEMP MSG: applyProjection - not allowed to project properties on classes");
  }

  CIMInstance nodeInst(nodeObj);  
    
  //
  // Do the projection.
  //

  Array<CIMName> requiredProps;
  Boolean allPropsRequired = node->wildcard;

  // Loop through the children of the node.
  // The node represents an embedded instance,
  // and the child nodes are the required properties on the embedded instance.
  PropertyNode * curChild = node->firstChild.get();
  while (curChild != NULL)
  {
    // Determine if the embedded instance meets the class scoping
    // rules for the current child node
    Boolean filterable = isFilterable(nodeInst, curChild);

    // If the embedded instance is filterable, and the child node has children,
    // or is wildcarded, then the child is assumed to be an embedded instance,
    // and we need to recurse to apply the projection on the embedded instance.
    // (the check for embedded instance is done in the recursive call)
    if (filterable && 
        (curChild->firstChild.get() != NULL || curChild->wildcard))
    {
      // We need to project on an embedded instance property. The steps are to
      // remove the embedded instance property from the current instance,
      // project on that embedded instance property, and then add the projected 
      // embedded instance property back to the current instance.
      Uint32 index = nodeInst.findProperty(curChild->name);
      if (index != PEG_NOT_FOUND)
      {
        // The embedded instance has the required embedded instance property.
        CIMProperty childProp = nodeInst.getProperty(index); 
        nodeInst.removeProperty(index);
        applyProjection(curChild, childProp);
        nodeInst.addProperty(childProp);
      }
    }

    // If the node is not wildcarded, and the embedded instance
    // is filterable, then add the current child to the list
    // of required properties on the embedded instance.
    if (!allPropsRequired && filterable)
    {
      // The instance is filterable, add the property to the required list. 
      requiredProps.append(node->name);
    }

    curChild = curChild->sibling.get();
  }

  // Remove the properties that are not in the projection.
  // This also checks for missing required properties.
  removeUnneededProperties(nodeInst, 
                           allPropsRequired,
                           nodeInst.getClassName(),
                           requiredProps);

  // Put the projected instance back into the property.
// ATTN - UNCOMMENT when emb objs are supported
//CIMValue newNodeVal(nodeInst);
//nodeProp.setValue(newNodeVal);
} 

Boolean CQLSelectStatementRep::isFilterable(const  CIMInstance& inst,
                                            PropertyNode* node)
{
  //
  // Determine if an instance is filterable for a scoped property (ie. its
  // type is the scoping class or a subclass of the scoping class where the
  // property exists)
  //
  // Note that an instance that is unfilterable is not considered
  // an error.  In CQL, an instance that is not of the required scope
  // would cause a NULL to be placed in the result set column for the 
  // property. However since we are not implementing result set in stage1,
  // just skip the property.  This can lead to an instance having
  // NO required properties even though it is derived from the FROM class.
  // This can easily happen if the scoping operator is used.
  //

  Boolean filterable = false;
  if (inst.getClassName() == node->scope)
  {
    // The instance's class is the same as the required scope
    filterable = true;
  }
  else
  {
    try 
    {
      if (_ctx->isSubClass(node->scope, inst.getClassName()))
      {
        // The instance's class is a subclass of the required scope.
        filterable = true;
      }
    }
    catch (CIMException& ce)
    {
      if (ce.getCode() == CIM_ERR_INVALID_CLASS || 
          ce.getCode() == CIM_ERR_NOT_FOUND)
      {
        // The scoping class was not found in the schema.
        // Just swallow this error because according to the
        // spec we should be putting NULL in the result column,
        // which means skipping the property on the instance.
        ;
      }
    }
  }
  
  return filterable;
} 

void CQLSelectStatementRep::removeUnneededProperties(CIMInstance& inst, 
                                                     Boolean& allPropsRequired,
                                                     const CIMName& allPropsClass,
                                                     Array<CIMName>& requiredProps)
{
  // Implementation note:
  // Scoping operator before a wildcard is not allowed:
  // Example:
  // SELECT fromclass.embobj1.scope1::* FROM fromclass
  //
  // However, the following are allowed:
  // SELECT fromclass.embobj1.* FROM fromclass
  // (this means that all the properties on the class of instance embobj1
  //  are required)
  //
  // SELECT fromclass.* FROM fromclass
  // (this means that all the properties on class fromclass are required
  //  to be on the instance being projected, not including any
  //  properties on a subclass of fromclass)

  // If all properties are required (ie. wildcarded), then rebuild the 
  // required property list from all the properties on the classname passed in  
  // This is either the FROM class or the class of an embedded instance.
  if (allPropsRequired)
  {
    requiredProps.clear();
    CIMClass cls = _ctx->getClass(allPropsClass);
    Array<CIMName> clsProps;
    for (Uint32 i = 0; i < cls.getPropertyCount(); i++)
    {
      requiredProps.append(cls.getProperty(i).getName());
    }
  }

  // Find out what properties are on the instance.
  Array<CIMName> supportedProps;
  for (Uint32 i = 0; i < inst.getPropertyCount(); i++)
  {
    supportedProps.append(inst.getProperty(i).getName());
  }
  
  // Check that all required properties are on the instance.
  for (Uint32 i = 0; i < requiredProps.size(); i++)
  {
    if (!containsProperty(requiredProps[i], supportedProps))
    {
      throw Exception("TEMP MSG - instance missing required property: " + requiredProps[i].getString());
    }
  }

  // Remove the properties on the instance that are not required.
  for (Uint32 i = 0; i < supportedProps.size(); i++)
  {
    if (!containsProperty(supportedProps[i], requiredProps))
    {
      Uint32 index = inst.findProperty(supportedProps[i]);
      PEGASUS_ASSERT(index != PEG_NOT_FOUND);
      inst.removeProperty(index);
    }
  }
}

void CQLSelectStatementRep::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
  Array<CQLIdentifier> fromList = _ctx->getFromList();
  PEGASUS_ASSERT(fromList.size() == 1);  // no joins yet

  // Check if the classname passed in is the FROM class or a subclass
  // of the FROM class.
  if (!inClassName.getClassName().equal(fromList[0].getName()))
  {
    // Not the FROM class, check if it is a subclass of the FROM
    QueryContext::ClassRelation rel;
    try
    {
      rel = _ctx->getClassRelation(fromList[0].getName(),
                                   inClassName.getClassName());
    }
    catch (CIMException& ce)
    {
      if (ce.getCode() == CIM_ERR_INVALID_CLASS || 
          ce.getCode() == CIM_ERR_NOT_FOUND)
      {
        // Either the FROM or the class passed in does not exist
        // ATTN may just want to throw the CIMException rather than
        // CQL exception
        throw Exception("TEMP MSG - class does not exist: " + ce.getMessage());
      }

      throw;
    }    
  
    if (rel != QueryContext::SUBCLASS)
    {
      throw Exception("TEMP MSG: class is not the FROM class and not a subclass of FROM class");
    }
  }
  else
  {
    // Class passed in is the FROM class.  Check if it exists.
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
        throw Exception("TEMP MSG - class does not exist " + ce.getMessage());
      }
      
      throw;
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

  Array<CQLChainedIdentifier> _whereIdentifiers = _ctx->getWhereList();
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
  // Note: applyContext has been called beforehand
  
  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  Uint32 startingPos = 0;
  CIMName curContext;
  for (Uint32 pos = startingPos; pos < ids.size(); pos++)
  {
    // Determine the current class context
    if (ids[pos].isScoped())
    {
      // The chain element is scoped.  Use the scoping
      // class as the current context.  Note: this depends 
      // on applyContext resolving the class aliases before we get here.
      curContext = CIMName(ids[pos].getScope());
    }
    else
    {
      // The chain element is not scoped.  Assume that we are
      // before a position that is required to be scoped.
      // (applyContext validates that the chained identifier
      // has scoped identifiers in the required positions).
      // The current context is the name at the first position,
      // which must be a classname (ie. right side of ISA where
      // the only element in the chain is a classname, or
      // cases where the FROM class is the first, and maybe only,
      // element of the chain).
      PEGASUS_ASSERT((pos < startingPos + 2) || ids[pos].isWildcard());
      curContext = ids[0].getName();
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
        throw Exception("TEMP_MSG: class context does not exist: " + curContext.getString());
      }
    }

    // Now do the checks for properties existing on the current class context
    // and the class relationship rules in section 5.4.1.
    // Only do these checks if the chain id has a property.
    if (pos > startingPos)
    {
      if (ids[pos].isWildcard())
      {
        // The wildcard is at the end (verified by applyContext), so
        // no checking is required at this position.
        continue;
      }

      // Determine if the property name at the current position
      // exists on the current class context.
      Uint32 propertyIndex = classDef.findProperty(ids[pos].getName());
      if (propertyIndex == PEG_NOT_FOUND)
      {
        throw Exception("TEMP MSG: prop not on class context: " + ids[pos].getName().getString());
      }

      // Checking class relationship rules in section 5.4.1.
      // For validateProperties, this only applies to the first
      // property in the chain.  This is because once we get into
      // embedded properties we don't know what the class will be
      // until we have an instance.
      if ((pos == (startingPos+1)) && !curContext.equal(ids[0].getName()))
      {
        // Its the first property, and the class context is not the FROM class.
        // Check the class relationship between the scoping class and the FROM class.
        if (_ctx->getClassRelation(ids[0].getName(), curContext) == QueryContext::NOTRELATED)
        {
          throw Exception("TEMP MSG: section 5.4.1 violation!");
        }
      }

      // If the current position implies an embedded object, then
      // verify that the property is an embedded object
      if ((pos > startingPos) && (pos < (ids.size() - 1))) 
      {
        CIMProperty embObj = classDef.getProperty(propertyIndex);
        CIMName qual("EmbeddedObject");
        if (embObj.findQualifier(qual) == PEG_NOT_FOUND)
        {
          throw Exception("TEMP MSG: property is not an embedded object: " + embObj.getName().getString());
        }
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
  return getPropertyListInternal(inClassName, true, true);
}

CIMPropertyList CQLSelectStatementRep::getSelectPropertyList(const CIMObjectPath& inClassName)
{
  return getPropertyListInternal(inClassName, true, false);
}

CIMPropertyList CQLSelectStatementRep::getWherePropertyList(const CIMObjectPath& inClassName)
{
  return getPropertyListInternal(inClassName, false, true);
}

CIMPropertyList CQLSelectStatementRep::getPropertyListInternal(const CIMObjectPath& inClassName,
                                                               Boolean includeSelect,
                                                               Boolean includeWhere)
{
  if (!_contextApplied)
    applyContext();

  // Get the classname.  Note: since wbem-uri is not supported yet,
  // only use the classname part of the path. 
  CIMName className = inClassName.getClassName();
  if (className.isNull())
  {
    // If the caller passed in an empty className, then the 
    // FROM class is to be used.
    className = _ctx->getFromList()[0].getName();
  }
  
  Boolean isWildcard;
  Array<CIMName> reqProps;
  Array<CIMName> matchedScopes;
  Array<CIMName> unmatchedScopes;

  // Add required properties from the select list.
  if (includeSelect)
  {
    for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
    {
      isWildcard = addRequiredProperty(reqProps, 
                                       className,
                                       _selectIdentifiers[i],
                                       matchedScopes,
                                       unmatchedScopes);
    
      if (isWildcard)
      {
        // If any wildcard is found then all properties are required.
        // Return null property list to indicate all properties required.
        return CIMPropertyList();
      }
    }
  }

  // Add required properties from the WHERE clause.
  if (includeWhere)
  {
    Array<CQLChainedIdentifier> _whereIdentifiers = _ctx->getWhereList();
    for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
    {
      isWildcard = addRequiredProperty(reqProps,
                                       className,
                                       _whereIdentifiers[i],
                                       matchedScopes,
                                       unmatchedScopes);                                     

      // Wildcards are not allowed in the WHERE clause
      PEGASUS_ASSERT(!isWildcard);
    }
  }
    
  // Check if every property on the class is required.
  CIMClass theClass = _ctx->getClass(className);
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
    // Return null property list to indicate all properties are required.
    return CIMPropertyList();
  }
  else
  {
    // Return the required property list.  Note that it is possible to return
    // an empty list in the case of no required properties for the classname
    // passed in.  This can happen when the scoping operator is used.
    return CIMPropertyList(reqProps);
  }
}

Boolean CQLSelectStatementRep::addRequiredProperty(Array<CIMName>& reqProps,
                                                   CIMName& className,
                                                   CQLChainedIdentifier& chainId,
                                                   Array<CIMName>& matchedScopes,
                                                   Array<CIMName>& unmatchedScopes)
{
  //
  // Implementation notes:  
  // This function does not look for required properties on embedded objects.
  // This function assumes that applyContext has been called.
  //

  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  // After applyContext has been called, a single element
  // chained identifier refers to either an instance of the
  // FROM class, or is the classname on the right side of ISA.
  if (ids.size() == 1)
  {
    // This identifier is not a property name
    return false;
  }

  if (ids[1].isSymbolicConstant())
  {
    // Non-embedded symbolic constants are not properties
    // Note that an embedded symbolic constant like this:
    // fromclass.embobj.scope::someprop#'ok'
    // implies that embobj is a required property, because 
    // embobj could be null, and that affects how the
    // identifier is evaluated.
    return false;
  }

  // Since applyContext has been called, the first chain element
  // will be the FROM class, so go to the 2nd chain element.
  if (ids[1].isScoped())
  {
    // The 2nd chain element is a scoped property.
    // Eg. fromclass.someclass::someprop

    // Determine the class that the property is being scoped to.
    // This could be the FROM class, or some other class not in the FROM list
    CIMName scopingClass = CIMName(ids[1].getScope());

    // Check if the scoping class is the same as the class passed in.
    if (scopingClass == className)
    {
      // The scoping class is the same as the class passed, 
      // add the property if not already added
      if (!containsProperty(ids[1].getName(), reqProps))
      {  
        reqProps.append(ids[1].getName());
      }
    }
    else
    {
      // The scoping class is not the same as the class passed. 
      // Check if we already know that the scoping class is a subclass
      // of the class passed in
      if (containsProperty(scopingClass, unmatchedScopes))
      {
        // Scoping class is a subclass.
        return false;
      }
        
      // Check if we already know that the scoping class is a superclass
      // of the class passed in
      Boolean isSuper = false;
      if (containsProperty(scopingClass, matchedScopes))
      {
        // Scoping class is a superclass.
        isSuper = true;
      }

      // Check if the scoping class is a superclass of the class passed in
      if (isSuper || _ctx->isSubClass(scopingClass, className))
      {
        // Scoping class is a superclass of the class passed in.
        if (!isSuper)
        {
          // Save this information
          matchedScopes.append(scopingClass);
        }

        // Add to the required property list if not already there.
        if (!containsProperty(ids[1].getName(), reqProps))
        {  
          reqProps.append(ids[1].getName());
        }
      }
      else
      {
        // Scoping class is not superclass of class passed in.
        // Save this information.
        unmatchedScopes.append(scopingClass);
      }
    }  // end else scoping class not == class passed in
  }  // end if first id is scoped
  else
  {
    // The 2nd chain element is an unscoped property
    // Check if it is wildcarded
    if (ids[1].isWildcard())
    {
      // Wildcard.
      // If the class passed in is the FROM class, then
      // all properties are required on the class passed in.
      CIMName fromClassName = _ctx->getFromList()[0].getName();
      if (fromClassName == className)
      {
        return true;
      }

      // Add all the properties on the FROM class to
      // the required property list.
      CIMClass fromClass = _ctx->getClass(fromClassName);
      for (Uint32 n = 0; n < fromClass.getPropertyCount(); n++)
      {
        // Add to the required property list if not already there.
        if (!containsProperty(fromClass.getProperty(n).getName(), reqProps))
        {  
          reqProps.append(fromClass.getProperty(n).getName());
        }        
      }

      return false;
    }

    // Implementation note:
    // Since this API assumes that the class passed in
    // is the FROM class or a subclass of the FROM class,
    // AND validateProperties can be called to check if
    // unscoped properties are on the FROM class, 
    // we can just add the required property because
    // it is assumed to be on the FROM class.

    // Add to the required property list if not already there.
    if (!containsProperty(ids[1].getName(), reqProps))
    {  
      reqProps.append(ids[1].getName());
    }
  }
  
  // Indicate the required property is not a wildcard
  return false;
}

Array<CQLChainedIdentifier> CQLSelectStatementRep::getSelectChainedIdentifiers()
{
  if (!_contextApplied)
    applyContext();

  return _selectIdentifiers;
}

Array<CQLChainedIdentifier> CQLSelectStatementRep::getWhereChainedIdentifiers()
{
  if (!_contextApplied)
    applyContext();

  return _ctx->getWhereList();
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

void CQLSelectStatementRep::appendClassPath(const CQLIdentifier& inIdentifier)
{
  _ctx->insertClassPath(inIdentifier);
}

void CQLSelectStatementRep::setPredicate(const CQLPredicate& inPredicate)
{
  _predicate = inPredicate;
}

CQLPredicate CQLSelectStatementRep::getPredicate() const
{
  return _predicate;
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

void CQLSelectStatementRep::applyContext()
{
  for (Uint32 i = 0; i < _selectIdentifiers.size(); i++)
  {
    _selectIdentifiers[i].applyContext(*_ctx);
    checkWellFormedIdentifier(_selectIdentifiers[i], true);
  }

  if (hasWhereClause())
  {
    _predicate.applyContext(*_ctx);

    // Note: must be after call to predicate's applyContext
    Array<CQLChainedIdentifier> _whereIdentifiers = _ctx->getWhereList();
    for (Uint32 i = 0; i < _whereIdentifiers.size(); i++)
    {
      checkWellFormedIdentifier(_whereIdentifiers[i], false);
    }
  }
}

void CQLSelectStatementRep::checkWellFormedIdentifier(const CQLChainedIdentifier& chainId,
                                                      Boolean isSelectListId)
{
  // This function assumes that applyContext has been called.
  Array<CQLIdentifier> ids = chainId.getSubIdentifiers();

  if (ids.size() == 0)
  {
    throw Exception("TEMP MSG: empty chained identifier");
  }

  if (ids.size() == 1 && isSelectListId)
  {
    // Single element chain ids are not allow in the select list.
    // The select list can only have properties.
    throw Exception("TEMP MSG: need to select a property on the FROM class");
  }

  if (ids[0].isScoped()
      || ids[0].isWildcard()
      || ids[0].isSymbolicConstant() 
      || ids[0].isArray())
  {
    // The first identifier must be a classname (it could be the FROM class, or
    // some other class for the right side of ISA)
    throw Exception("TEMP MSG: first identifier is illegal after applyContext");
  }
   
  Uint32 startingPos = 1;
  for (Uint32 pos = startingPos; pos < ids.size(); pos++)
  {  
    if (ids[pos].isArray() && isSelectListId)
    {
      throw Exception("TEMP MSG: array indexing not allowed in basic select");
    }

    if (ids[pos].isSymbolicConstant() && isSelectListId)
    {
      throw Exception("TEMP MSG: symbolic constant not allowed in basic select");
    }

    if (ids[pos].isSymbolicConstant() && pos != (ids.size() -1))
    {
      throw Exception("TEMP MSG: symbolic constant must be last chain element");
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
  if (!_contextApplied)
    applyContext();

  // ATTN - add normalize code.
   if(_hasWhereClause){
   	Cql2Dnf DNFer(_predicate);
   	_predicate = DNFer.getDnfPredicate(); 
   }
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
}

PEGASUS_NAMESPACE_END
