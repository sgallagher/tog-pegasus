//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Dir.h>
#include <Pegasus/Common/XmlWriter.h>
#include "InheritanceTree.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

////////////////////////////////////////////////////////////////////////////////
//
// NoCaseEqualFunc
//
////////////////////////////////////////////////////////////////////////////////

struct NoCaseEqualFunc
{
    static Boolean equal(const String& x, const String& y)
    {
        return String::equalNoCase(x, y);
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// InheritanceTreeNode
//
////////////////////////////////////////////////////////////////////////////////

struct InheritanceTreeNode
{
    InheritanceTreeNode(const CIMName& className);

    void addSubClass(InheritanceTreeNode* subClass);

    Boolean removeSubClass(InheritanceTreeNode* subClass);

    void getSubClassNames(
	Array<CIMName>& subClassNames, 
	Boolean deepInheritance);

    void getSuperClassNames(Array<CIMName>& superClassNames);

    void print(PEGASUS_STD(ostream)& os) const;

    Boolean isSubClass(const CIMName& className) const;

    CIMName className;
    InheritanceTreeNode* superClass;
    InheritanceTreeNode* sibling;
    InheritanceTreeNode* subClasses;
    Boolean provisional;
};

InheritanceTreeNode::InheritanceTreeNode(const CIMName& className) 
    : className(className), superClass(0), 
    sibling(0), subClasses(0), provisional(true)
{

}

void InheritanceTreeNode::addSubClass(InheritanceTreeNode* subClass)
{
    subClass->superClass = this;
    subClass->sibling = subClasses;
    subClasses = subClass;
}

Boolean InheritanceTreeNode::removeSubClass(InheritanceTreeNode* subClass)
{
    InheritanceTreeNode* prev = 0;

    for (InheritanceTreeNode* p = subClasses; p; p = p->sibling)
    {
	if (p == subClass)
	{
	    if (prev)
		prev->sibling = subClass->sibling;
	    else
		subClasses = subClass->sibling;
		
	    return true;
	}
	prev = p;
    }

    return false;
}

void InheritanceTreeNode::getSubClassNames(
    Array<CIMName>& subClassNames, 
    Boolean deepInheritance)
{
    // For each subClass:

    for (InheritanceTreeNode* p = subClasses; p; p = p->sibling)
    {
       
	subClassNames.append(p->className);

	if (deepInheritance)
	{
	    p->getSubClassNames(subClassNames, true);
	}
    }
}

Boolean InheritanceTreeNode::isSubClass(const CIMName& className_) const
{
    if (className.equal (className_))
	return true;

    for (InheritanceTreeNode* p = subClasses; p; p = p->sibling)
    {
	if (p->className.equal (className_))
	    return true;
    }

    return false;
}

void InheritanceTreeNode::getSuperClassNames(Array<CIMName>& superClassNames)
{
    // For each superClass:

    for (InheritanceTreeNode* p = superClass; p; p = p->superClass)
    {
	superClassNames.append(p->className);
	p->getSuperClassNames(superClassNames);
    }
}

void InheritanceTreeNode::print(PEGASUS_STD(ostream)& os) const
{
    os << className << " : " ;
    os << (superClass ? superClass->className : CIMName ());

    os << " { ";

    for (InheritanceTreeNode* p = subClasses; p; p = p->sibling)
	os << p->className << ' ';

    os << "}" << endl;
}

////////////////////////////////////////////////////////////////////////////////
//
// InheritanceTreeRep
//
////////////////////////////////////////////////////////////////////////////////


struct InheritanceTreeRep
{
    typedef HashTable<String, InheritanceTreeNode*, 
          NoCaseEqualFunc, HashFunc<String> > Table;
    Table table;
};

////////////////////////////////////////////////////////////////////////////////
//
// InheritanceTree
//
////////////////////////////////////////////////////////////////////////////////

InheritanceTree::InheritanceTree()
{
    _rep = new InheritanceTreeRep;
}

InheritanceTree::~InheritanceTree()
{
    for (InheritanceTreeRep::Table::Iterator i = _rep->table.start(); i; i++)
        delete i.value();

    delete _rep;
}

void InheritanceTree::insert(
    const String& className, 
    const String& superClassName)
{
    // ATTN: need found flag!

    // -- Insert superclass:

    InheritanceTreeNode* superClassNode = 0;

    if ((superClassName.size()) &&
	!_rep->table.lookup(superClassName, superClassNode))
    {
	superClassNode = new InheritanceTreeNode(superClassName);
	_rep->table.insert(superClassName, superClassNode);
    }

    // -- Insert class:
    
    InheritanceTreeNode* classNode = 0;

    if (!_rep->table.lookup(className, classNode))
    {
	classNode = new InheritanceTreeNode(className);
	_rep->table.insert(className, classNode);
    }

    classNode->provisional = false;

    // -- Link the class and superclass nodes:

    if (superClassNode)
	superClassNode->addSubClass(classNode);
}

void InheritanceTree::insertFromPath(const String& path)
{
    for (Dir dir(path); dir.more(); dir.next())
    {
	String fileName = dir.getName();

	// Ignore the current and parent directories.

	if (fileName == "." || fileName == "..")
	    continue;

	Uint32 dot = fileName.find('.');

	// Ignore files without dots in them:

	if (dot == PEG_NOT_FOUND)
	    continue;

	String className = fileName.subString(0, dot);
	String superClassName = fileName.subString(dot + 1);

	if (superClassName == "#")
	    superClassName.clear();

	insert(className, superClassName);
    }
}

void InheritanceTree::check() const
{
    for (InheritanceTreeRep::Table::Iterator i = _rep->table.start(); i; i++)
    {
	if (i.value()->provisional)
	    throw InvalidInheritanceTree(i.value()->className.getString());
    }
}

Boolean InheritanceTree::getSubClassNames(
    const CIMName& className,
    Boolean deepInheritance,
    Array<CIMName>& subClassNames) const
{

   // -- Case 1: className is empty: get all class names (if deepInheritance)
   // -- or just root class names (if not deepInheritance).
   if (className.isNull())
   {
      for (InheritanceTreeRep::Table::Iterator i = _rep->table.start();i;i++)
      {
	 if (deepInheritance)
	 {
	    // Append all classes:
	    
	    subClassNames.append(i.key());
	 }
	 else if (!i.value()->superClass)
	 {
	    // Just append root classes:
	    
	    subClassNames.append(i.key());
	 }
      }
      return true;
   }
   
   // -- Case 2: className non-empty: get names of classes descendent from
   // -- the given class.
   
   for (InheritanceTreeRep::Table::Iterator i = _rep->table.start(); i; i++)
   {
      if (className.equal (i.key()))
      {
	 i.value()->getSubClassNames(subClassNames, deepInheritance);
	 return true;
      }
   }
   
   // Not found!
   return false;
}

Boolean InheritanceTree::isSubClass(
    const CIMName& class1, 
    const CIMName& class2) const
{
    InheritanceTreeNode* node = 0;	

    if (!_rep->table.lookup(class1.getString(), node))
	return false;

    return node->isSubClass(class2.getString());
}

Boolean InheritanceTree::getSuperClassNames(
    const CIMName& className,
    Array<CIMName>& superClassNames) const
{
    InheritanceTreeNode* classNode;

    if (_rep->table.lookup(className.getString(), classNode))
    {
	classNode->getSuperClassNames(superClassNames);
	return true;
    }

    return false;
}

Boolean InheritanceTree::getSuperClass(
    const CIMName& className,
    CIMName& superClassName) const
{
    InheritanceTreeNode* classNode;

    if (_rep->table.lookup(className.getString(), classNode))
    {
	if (classNode->superClass)
	{
	    superClassName = classNode->superClass->className;
	}
	else
	{
	    superClassName.clear();
	}

	return true;
    }

    return false;
}

Boolean InheritanceTree::hasSubClasses(
    const CIMName& className,
    Boolean& hasSubClasses) const
{
    InheritanceTreeNode* node = 0;	

    if (!_rep->table.lookup(className.getString(), node))
	return false;

    hasSubClasses = node->subClasses != 0;
    return true;
}

Boolean InheritanceTree::containsClass(const CIMName& className) const
{
    return _rep->table.contains(className.getString());
}

void InheritanceTree::remove(const CIMName& className)
{
    // -- Lookup the node:

    InheritanceTreeNode* node = 0;	

    if (!_rep->table.lookup(className.getString(), node))
	throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_CLASS, className.getString());

    // -- Disallow if is has any subclasses:

    if (node->subClasses)
	throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_CLASS_HAS_CHILDREN, className.getString());

    // -- Remove as child of superclass:

    InheritanceTreeNode* superClass = node->superClass;

    if (superClass)
    {
	Boolean result = superClass->removeSubClass(node);
	PEGASUS_ASSERT(result);
    }

    // -- Remove from the hash table and delete:

    Boolean result = _rep->table.remove(className.getString());
    PEGASUS_ASSERT(result);
    delete node;
}

void InheritanceTree::print(PEGASUS_STD(ostream)& os) const
{
    for (InheritanceTreeRep::Table::Iterator i = _rep->table.start(); i; i++)
	i.value()->print(os);
}

PEGASUS_NAMESPACE_END
