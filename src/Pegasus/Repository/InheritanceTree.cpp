//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author:
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Dir.h>
#include "InheritanceTree.h"

PEGASUS_NAMESPACE_BEGIN

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
// IgnoreCaseEqualFunc
//
////////////////////////////////////////////////////////////////////////////////

struct IgnoreCaseEqualFunc
{
    static Boolean equal(const String& x, const String& y)
    {
        return String::equalIgnoreCase(x, y);
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

    void addSubClass(InheritanceTreeNode* child);

    void getSubClassNames(
	Array<String>& subClassNames, 
	Boolean deepInheritance);

    void print(std::ostream& os) const;

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

void InheritanceTreeNode::addSubClass(InheritanceTreeNode* child)
{
    child->superClass = this;
    child->sibling = subClasses;
    subClasses = child;
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
	    p->getSubClassNames(subClassNames, true);
    }
}

void InheritanceTreeNode::print(std::ostream& os) const
{
    os << className << " : " ;
    os << (superClass ? superClass->className : "<None>");

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

typedef HashTable<String, InheritanceTreeNode*, IgnoreCaseEqualFunc> 
    InheritanceTable;

struct InheritanceTreeRep
{
    InheritanceTable _table;
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

    if (superClassName.getLength() &&
	!_rep->_table.lookup(superClassName, superClassNode))
    {
	superClassNode = new InheritanceTreeNode(superClassName);
	_rep->_table.insert(superClassName, superClassNode);
    }

    // -- Insert class:
    
    InheritanceTreeNode* classNode = 0;

    if (!_rep->_table.lookup(className, classNode))
    {
	classNode = new InheritanceTreeNode(className);
	_rep->_table.insert(className, classNode);
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
    for (InheritanceTable::Iterator i = _rep->_table.start(); i; i++)
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

    if (!className.getLength())
    {
	for (InheritanceTable::Iterator i = _rep->_table.start(); i; i++)
	{
	    if (deepInheritance)
	    {
		// Append all classes:

		subClassNames.append(i.key());
		return true;
	    }
	    else if (!i.value()->superClass)
	    {
		// Just append root classes:

		subClassNames.append(i.key());
		return true;
	    }
	}
    }

    // -- Case 2: className non-empty: get names of classes descendent from
    // -- the given class.

    for (InheritanceTable::Iterator i = _rep->_table.start(); i; i++)
    {
	if (String::equalIgnoreCase(className, i.key()))
	{
	    i.value()->getSubClassNames(subClassNames, deepInheritance);
	    return true;
	}
    }

    // Not found!
    return false;
}

void InheritanceTree::print(std::ostream& os) const
{
    for (InheritanceTable::Iterator i = _rep->_table.start(); i; i++)
	i.value()->print(os);
}

PEGASUS_NAMESPACE_END
