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
// LIMITED TO THEa
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
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

#ifndef Pegasus_LanguageElement_h
#define Pegasus_LanguageElement_h

#include <cstdlib>
#include <cctype>
#include <iostream>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>


PEGASUS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////
//
// LanguageElement::
//
//////////////////////////////////////////////////////////////
    
/** This is a base class for derived LanguageElements
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
	LanguageElement::LanguageElement(String language, 
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
	virtual LanguageElement operator=(LanguageElement rhs);
	
	/**
	 * Builds the language tag representation of this object
	 * @return String - language-country-variant 
	 */
	virtual String getLanguageTag() const;

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
	
	/** String representation of the LanguageElement
	 * @return the String
	 */
	virtual String toString() const;
	
	/**
	 * Equality based on case INSENSITIVE comparison of the language tags
	 */
	virtual Boolean operator==(LanguageElement rhs);
	
	/**
	 * Inequality based on case INSENSITIVE comparison of the language tags
	 */
	virtual Boolean operator!=(LanguageElement rhs);
	
	/**
	 * Writes the String representation of this object to the stream
	 */
	friend ostream & operator<<(ostream &stream, LanguageElement le);

	/**
	 * AcceptLanguages needs direct access to quality, but ContentLanguages does not.
	 */
	friend class AcceptLanguages;	

protected:
	Real32 quality;			// quality value of the languge tag
	
private:
	
	String buildLanguageTag() const;
	
	void splitLanguageTag(String language_tag);
	
	String language;      // language part of language tag
	String country;       // country code part of the language tag
	String variant;       // language variant part of the language tag
			
}; // end LanguageElement

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T LanguageElement
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif
