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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AcceptLanguages_h
#define Pegasus_AcceptLanguages_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/LanguageElementContainer.h>
#include <Pegasus/Common/AcceptLanguageElement.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

class AcceptLanguagesRep;

//////////////////////////////////////////////////////////////
//
// AcceptLanguages
//
//////////////////////////////////////////////////////////////

/** <I><B>Experimental Interface</B></I><BR>
    This class is a container class for AcceptLanguageElement
 */
class PEGASUS_COMMON_LINKAGE AcceptLanguages : public LanguageElementContainer
{
public:

    /** This member is used to represent an empty AcceptLanguages. Using this
        member avoids construction of an empty AcceptLanguages
        (e.g., AcceptLanguages()).
    */
    static const AcceptLanguages EMPTY;

    /**
     * Constructor
     */
    AcceptLanguages();

    /** Constructor
        @param hdr String complete value portion of AcceptLanguage header
     */
    AcceptLanguages(const String& hdr);

    /**
        Constructor
        @param container An Array of LanguageElement objects from which to
        construct an AcceptLanguages object.
     */
    AcceptLanguages(const Array<LanguageElement>& container);

    /**
        Constructor
        @param container An Array of AcceptLanguageElement objects from which
        to construct an AcceptLanguages object.
     */
    AcceptLanguages(const Array<AcceptLanguageElement>& container);

    AcceptLanguages(const AcceptLanguages& rhs);

    /**
        Destrctor
     */
    ~AcceptLanguages();


    /**
        @return A String representation of this object in AcceptLanguage
        header format according to the HTTP specification.
     */
    String toString() const;

    /**
        @return ostream - Returns a representation of this object in AcceptLanguage header format
        according to the RFC
     */
    PEGASUS_COMMON_LINKAGE friend PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& stream, const AcceptLanguages& al);

    /**
        Assignment
        @param rhs AcceptLanguages
     */
    AcceptLanguages& operator=(const AcceptLanguages& rhs);

    /**
        Accesses an AcceptLanguageElement at a specified index.
        @param index Integer index into the container.
        @return The AcceptLanguageElement corresponding to the specified index.
        @throw IndexOutOfBoundsException If the specified index is out of
        range.
     */
    AcceptLanguageElement getLanguageElement(Uint32 index) const;

    /**
        Fills in the array with all the language elements in the container
        @param elements An Array of AcceptLanguageElement objects to be
        populated.
     */
    void getAllLanguageElements(Array<AcceptLanguageElement> & elements) const;

    Array<AcceptLanguageElement> getAllLanguageElements() const;

    /**
        Returns the next element in the container.  The behavior of this
        method is undefined before itrStart() is called.
        @return The next AcceptLanguageElement in the container or
        AcceptLanguageElement::EMPTY if the end of the container has been
        reached.
     */
    AcceptLanguageElement itrNext();

    /**
        Inserts an element into this container.  The element is inserted
        at a position determined by the quality value associated with the
        language_tag.
        @param element The AcceptLanguageElement to insert
     */
    void insert(const AcceptLanguageElement& element);

    /**
        Removes the specified element from the container.
        @param index Integer index of the element to remove
     */
    void remove(Uint32 index);

    /**
        Removes the element matching the parameter
        @param element The AcceptLanguageElement to remove
        @return int  -1 if element not found, otherwise returns the position
        of element before the remove.
     */
    Uint32 remove(const AcceptLanguageElement& element);

    /**
        Finds the element in the container and returns its position.
        @param element AcceptLanguageElement - element to find
        @return int index of element if found, otherwise PEG_NOT_FOUND
     */
    Uint32 find(const AcceptLanguageElement& element) const;

    /**
        Finds the element in the container that matches the language_tag and
        quality.
        @param language_tag The language tag string of the element to find
        (based on case-insensitive comparison).
        @param quality Real32 language_tag quality value of the element to find
        @return int index of element if found, otherwise PEG_NOT_FOUND
     */
    Uint32 find(const String & language_tag, Real32 quality) const;

    static AcceptLanguages getDefaultAcceptLanguages();

    void prioritize();

    void buildLanguageElements(const Array<String>& values);

}; // end AcceptLanguages

PEGASUS_NAMESPACE_END

#endif  // PEGASUS_USE_EXPERIMENTAL_INTERFACES

#endif
