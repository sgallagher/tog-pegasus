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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_InheritanceTree_h
#define Pegasus_InheritanceTree_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/HashTable.h>

PEGASUS_NAMESPACE_BEGIN

/** The InheritanceTree class tracks inheritance relationships of CIM classes.

    This class is a memory resident version of the repository's persistent 
    inheritance information (represented using file names). The InheritanceTree 
    provides O(1) access (via hashing) to any class in the inheritance tree.

*/
class PEGASUS_REPOSITORY_LINKAGE InheritanceTree
{
public:

    InheritanceTree()
    {

    }

    Boolean insert(const String& className, const String& superClassName)
    {
	// ATTN: need found flag!

	// -- Insert superclass:

	Node* superClassNode = 0;

	if (superClassName.getLength() &&
	    !_table.lookup(superClassName, superClassNode))
	{
	    superClassNode = new Node(superClassName);
	    _table.insert(superClassName, superClassNode);
	}

	// -- Insert class:
	
	Node* classNode = 0;

	if (!_table.lookup(className, classNode))
	{
	    classNode = new Node(className);
	    _table.insert(className, classNode);
	}

	// -- Link the class and superclass nodes:

	if (superClassNode)
	    superClassNode->addSubClass(classNode);

	return true;
    }

    void print() const
    {
	for (Table::Iterator i = _table.start(); i; i++)
	    i.value()->print();
    }

private:

    struct Node
    {
	Node(const String& className) : _className(className), _superClass(0), 
	    _sibling(0), _subClasses(0)
	{

	}

	void addSubClass(Node* child)
	{
	    child->_superClass = this;
	    child->_sibling = _subClasses;
	    _subClasses = child;
	}

	String _className;
	Node* _superClass;
	Node* _sibling;
	Node* _subClasses;

	void print() const
	{
	    std::cout << "*** ClassName: " << _className << std::endl;

	    std::cout << "Subclasses: ";

	    for (Node* p = _subClasses; p; p = p->_sibling)
		std::cout << p->_className << ' ';
	    std::cout << std::endl;

	    std::cout << "Superclass: ";

	    if (_superClass)
		std::cout << _superClass->_className;
	    else
		std::cout << "#";

	    std::cout << std::endl;
	}
    };

    typedef HashTable<String, Node*> Table;
    Table _table;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InheritanceTree_h */
