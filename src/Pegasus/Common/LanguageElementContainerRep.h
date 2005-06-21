//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Humberto Rivero (hurivero@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_LanguageElementContainerRep_h
#define Pegasus_LanguageElementContainerRep_h


#include <cstdlib>
#include <cctype>
#include <iostream>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/LanguageElement.h>
#include <Pegasus/Common/LanguageParser.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////
//
// LanguageElementContainerRep::
//
//////////////////////////////////////////////////////////////

/** This class is a container class for AcceptLanguageElement
 */
class PEGASUS_COMMON_LINKAGE LanguageElementContainerRep{

public:

	/**	This member is used to represent an empty LanguageElementContainer. Using this 
        member avoids construction of an empty LanguageElementContainer 
        (e.g., LanguageElementContainer()).
	*/
//	static const LanguageElementContainer EMPTY;
    
    /**
     * Default Constructor
     */
	LanguageElementContainerRep();    
	
	/**
	 * Constructor
	 * @param container Array<LanguageElement> - contructs the object with the elements in the array
	 */
	LanguageElementContainerRep(const Array<LanguageElement> &container);

	/** Copy Constructor
	 * @param rhs LanguageElementContainer
	 */
	LanguageElementContainerRep(const LanguageElementContainerRep &rhs);
	
	/**
	 * Destructor
	 */
	virtual ~LanguageElementContainerRep();
	
	/**
	 * Deep copy
	 * @param rhs LanguageElementContainer
	 */
	LanguageElementContainerRep& operator=(
	    const LanguageElementContainerRep& rhs);

	/** Gets an AcceptLanguageElement object at position index
	 * @param index int position of AcceptLanguageElement
	 * @return AcceptLanguageElement
	 */
	LanguageElement getLanguageElement(Uint32 index) const;

	/** Gets all AcceptLanguageElement objects in the container
	 * @return Array<AcceptLanguageElement>
	 */
	Array<LanguageElement> getAllLanguageElements() const;

	/** Returns the String representation of this container
	 * @return String
	 */
	virtual String toString() const;

	/** 
	 * Returns the length of the LanguageElementContainer object.
	 * @return Length of the container.
     */
    Uint32 size() const;

	/**
	 * True if the container has element (case INSENSITIVE match of element's language fields
	 * @param LanguageElement element 
	 * @return Boolean
	 */
	Boolean contains(LanguageElement element)const;

	/**
	 * Resets this object's iterator, should be called once before using itrNext()
	 */
	void itrStart();

	/**
	 * Returns the next element in the container
	 * Callers should call itrStart() ONCE before making calls to this function and
	 * anytime callers want the iterator reset to the beginning of the container.
	 * @return LanguageElement - the next element the container or LanguageElement::EMPTY_REF
	 * if the end of the container has been reached.
	 */
	LanguageElement itrNext();

	/**
	 * Removes the element at index from the container.  
	 * @param index Uint32 - position of the element in the container to remove
	 */
	void remove(Uint32 index);
	
	/**
	 * Removes the element matching the parameter, returns -1 if not found
	 * @param element LanguageElement - element to remove
	 * @return int  -1 if element not found, otherwise returns the position of element before the remove.
	 */
	Uint32 remove(const LanguageElement &element);

	/**
	 * Clears the container of all elements
	 */
	void clear();

	void append(LanguageElement element);

	/**
	 * Equality based on language fields only
	 */
	Boolean operator==(const LanguageElementContainerRep &rhs)const;

	/**
	 * Inequality based on language fields only
	 */
	Boolean operator!=(const LanguageElementContainerRep &rhs)const;
	
	/**
	 * Finds the element in the container and returns its position.
	 * @param element LanguageElement - element to find
	 * @return int index of element if found, otherwise -1
	 */
	Sint32 find(LanguageElement element)const;


  	Array<LanguageElement> container;        // holds LanguageElements

private:
	
	void buildLanguageElements(Array<String> values);

	void updateIterator();
	
    Uint32 itr_index;					// current position for iterator type access
	       
}; // end LanguageElementContainerRep

PEGASUS_NAMESPACE_END

#endif  // PEGASUS_USE_EXPERIMENTAL_INTERFACE

#endif
