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
#include "InheritanceTree.h"

PEGASUS_NAMESPACE_BEGIN

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
// InheritanceTreeNode
//
////////////////////////////////////////////////////////////////////////////////

struct InheritanceTreeNode
{
    InheritanceTreeNode(const String& className);

    void addSubClass(InheritanceTreeNode* child);

    void print(std::ostream& os) const;

    String _className;
    InheritanceTreeNode* _superClass;
    InheritanceTreeNode* _sibling;
    InheritanceTreeNode* _subClasses;
};

InheritanceTreeNode::InheritanceTreeNode(const String& className) 
    : _className(className), _superClass(0), 
    _sibling(0), _subClasses(0)
{

}

void InheritanceTreeNode::addSubClass(InheritanceTreeNode* child)
{
    child->_superClass = this;
    child->_sibling = _subClasses;
    _subClasses = child;
}

void InheritanceTreeNode::print(std::ostream& os) const
{
    os << _className << " : " ;
    os << (_superClass ? _superClass->_className : "<None>");

    os << " { ";

    for (InheritanceTreeNode* p = _subClasses; p; p = p->_sibling)
	os << p->_className << ' ';

    os << "}" << endl;
}

////////////////////////////////////////////////////////////////////////////////
//
// InheritanceTreeRep
//
////////////////////////////////////////////////////////////////////////////////

typedef HashTable<String, InheritanceTreeNode*> InheritanceTable;

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

Boolean InheritanceTree::insert(
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

    // -- Link the class and superclass nodes:

    if (superClassNode)
	superClassNode->addSubClass(classNode);

    return true;
}

void InheritanceTree::print(std::ostream& os) const
{
    for (InheritanceTable::Iterator i = _rep->_table.start(); i; i++)
	i.value()->print(os);
}

PEGASUS_NAMESPACE_END
