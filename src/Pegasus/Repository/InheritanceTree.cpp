//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Dir.h>
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
    InheritanceTreeNode(const String& className);

    void addSubClass(InheritanceTreeNode* subClass);

    Boolean removeSubClass(InheritanceTreeNode* subClass);

    void getSubClassNames(
	Array<String>& subClassNames, 
	Boolean deepInheritance);

    void getSuperClassNames(Array<String>& superClassNames);

    void print(PEGASUS_STD(ostream)& os) const;

    String className;
    InheritanceTreeNode* superClass;
    InheritanceTreeNode* sibling;
    InheritanceTreeNode* subClasses;
    Boolean provisional;
};

InheritanceTreeNode::InheritanceTreeNode(const String& className) 
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
    Array<String>& subClassNames, 
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

void InheritanceTreeNode::getSuperClassNames(Array<String>& superClassNames)
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
    os << (superClass ? superClass->className : String("<None>"));

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
    delete _rep;
}

void InheritanceTree::insert(
    const String& className, 
    const String& superClassName)
{
    // ATTN: need found flag!

    // -- Insert superclass:

    InheritanceTreeNode* superClassNode = 0;

    if (superClassName.size() &&
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

	if (dot == Uint32(-1))
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
	    throw InvalidInheritanceTree(i.value()->className);
    }
}

Boolean InheritanceTree::getSubClassNames(
    const String& className,
    Boolean deepInheritance,
    Array<String>& subClassNames) const
{
    // -- Case 1: className is empty: get all class names (if deepInheritance)
    // -- or just root class names (if not deepInheritance).

    if (!className.size())
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
	if (String::equalNoCase(className, i.key()))
	{
	    i.value()->getSubClassNames(subClassNames, deepInheritance);
	    return true;
	}
    }

    // Not found!
    return false;
}

Boolean InheritanceTree::getSuperClassNames(
    const String& className,
    Array<String>& superClassNames) const
{
    InheritanceTreeNode* classNode;

    if (_rep->table.lookup(className, classNode))
    {
	classNode->getSuperClassNames(superClassNames);
	return true;
    }

    return false;
}

Boolean InheritanceTree::getSuperClass(
    const String& className,
    String& superClassName) const
{
    InheritanceTreeNode* classNode;

    if (_rep->table.lookup(className, classNode))
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
    const String& className,
    Boolean& hasSubClasses) const
{
    InheritanceTreeNode* node = 0;	

    if (!_rep->table.lookup(className, node))
	return false;

    hasSubClasses = node->subClasses != 0;
    return true;
}

Boolean InheritanceTree::containsClass(const String& className) const
{
    return _rep->table.contains(className);
}

void InheritanceTree::remove(const String& className)
{
    // -- Lookup the node:

    InheritanceTreeNode* node = 0;	

    if (!_rep->table.lookup(className, node))
	throw PEGASUS_CIM_EXCEPTION(INVALID_CLASS, className);

    // -- Disallow if is has any subclasses:

    if (node->subClasses)
	throw PEGASUS_CIM_EXCEPTION(CLASS_HAS_CHILDREN, className);

    // -- Remove as child of superclass:

    InheritanceTreeNode* superClass = node->superClass;

    if (superClass)
    {
	Boolean result = superClass->removeSubClass(node);
	PEGASUS_ASSERT(result);
    }

    // -- Remove from the hash table and delete:

    Boolean result = _rep->table.remove(className);
    PEGASUS_ASSERT(result);
    delete node;
}

void InheritanceTree::print(PEGASUS_STD(ostream)& os) const
{
    for (InheritanceTreeRep::Table::Iterator i = _rep->table.start(); i; i++)
	i.value()->print(os);
}

PEGASUS_NAMESPACE_END
