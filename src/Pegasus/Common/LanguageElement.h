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

#ifndef Pegasus_LanguageElement_h
#define Pegasus_LanguageElement_h

#include <cstdlib>
#include <cctype>

#ifdef PEGASUS_OS_HPUX
# ifdef HPUX_IA64_NATIVE_COMPILER
#  include <iostream>
# else
#  include <iostream.h>
# endif
#else
# include <iostream>
#endif

//#include <ostream>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

class LanguageElementRep;

//////////////////////////////////////////////////////////////
//
// LanguageElement::
//
//////////////////////////////////////////////////////////////
    
/** <I><B>Experimental Interface</B></I><BR>
 *This is a base class for derived LanguageElements
 */ 
class PEGASUS_COMMON_LINKAGE LanguageElement{
	     
public:

	/**	This member is used to represent an empty LanguageElement. Using this 
        member avoids construction of an empty LanguageElement 
        (e.g., LanguageElement()).
    */
	static const LanguageElement EMPTY;
	
	/**
	 * This member is used to notify callers of (Accept/Content)Languages::itrNext() that the end of the container
	 * has been reached and subsequent calls to (Accept/Content)Languages::itrNext() will fail.
	 */
	static LanguageElement EMPTY_REF;

	/** Constructor
	 */
	LanguageElement();
	
	
	/**
	 * Constructor
	 * @param language String - language ex: en
	 * @param country String - country code ex: US
	 * @param variant String - variant ex: rab-oof
	 * @param quality Real32 - quality associated with the language, defaults to 1.0
	 */
	LanguageElement(String language, 
				 String country,
				 String variant,
				 Real32 quality = 1);

	/** 
	 * Constructor
	 * @param language_tag String language value
	 * @param quality Real32 quality associated with the language value, defaults to 1
	 */
	LanguageElement(String language_tag, Real32 quality = 1);

	/** Copy Constructor 
	 */
	LanguageElement(const LanguageElement &rhs);
	
	/**
	 * Destructor
	 */
	virtual ~LanguageElement();

	/**
	 * Assignment operator, deep copy
	 */
	LanguageElement& operator=(const LanguageElement &rhs);
	
	/** Gets the language value
	 * @return String - language
	 */
	String getLanguage() const;

	/**
	 * Gets the country value
	 * @return String - country
	 */
	String getCountry() const;

	/**
	 * Gets the variant value
	 * @return String variant
	 */
	String getVariant() const;
	
	/**
	 * Builds the language tag representation of this object
	 * @return String - language-country-variant 
	 */
	String getTag() const;

	Real32 getQuality() const;
	
	/** String representation of the LanguageElement
	 * @return the String
	 */
	virtual String toString() const;
	
	/**
	 * Equality based on case INSENSITIVE comparison of the language tags
	 */
	Boolean operator==(const LanguageElement &rhs)const;
	
	/**
	 * Inequality based on case INSENSITIVE comparison of the language tags
	 */
	Boolean operator!=(const LanguageElement &rhs)const;
	
	/**
	 * Writes the String representation of this object to the stream
	 */
	PEGASUS_COMMON_LINKAGE friend PEGASUS_STD(ostream)& operator <<(PEGASUS_STD(ostream) &stream, LanguageElement le);

	/**
	 * AcceptLanguages needs direct access to quality, but ContentLanguages does not.
	 */
	friend class AcceptLanguages;	

protected:

	LanguageElementRep *_rep;		
	
}; // end LanguageElement

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T LanguageElement
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif  // PEGASUS_USE_EXPERIMENTAL_INTERFACES

#endif
