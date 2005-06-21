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

#ifndef Pegasus_AcceptLanguageElement_h
#define Pegasus_AcceptLanguageElement_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/LanguageElement.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

class AcceptLanguageElementRep;

//////////////////////////////////////////////////////////////
//
// AcceptLanguageElement::
//
//////////////////////////////////////////////////////////////
    
/** <I><B>Experimental Interface</B></I><BR>
 *  This class is used to hold language information for a given AcceptLanguage
 *  header value.
 */ 
class PEGASUS_COMMON_LINKAGE AcceptLanguageElement : public LanguageElement{
	     
public:

	/**	This member is used to represent an empty AcceptLanguageElement. Using this 
        member avoids construction of an empty AcceptLanguageElement 
        (e.g., AcceptLanguageElement()).
    */
	static const AcceptLanguageElement EMPTY;

	/**
	 * This member is used to notify callers of AcceptLanguages::itrNext() that the end of the container
	 * has been reached and subsequent calls to AcceptLanguages::itrNext() will fail.
	 */
	static AcceptLanguageElement EMPTY_REF;

	/** Constructor
	 */
	AcceptLanguageElement();
	
	/**
	 * Constructor
	 * @param language String - language ex: en
	 * @param country String - country code ex: US
	 * @param variant String - variant ex: rab-oof
	 * @param quality Real32 - quality associated with the language, defaults to 1.0
	 */
	AcceptLanguageElement(const String& language, 
				     const String& country,
				     const String& variant,
				     Real32 quality = 1.0);
	/** Constructor
	 * @param language_tag String IANA language value
	 * @param q Real32 quality value for language, 0 < q < 1, defaults to 1
	 */
	AcceptLanguageElement(const String& language_tag, 
						  Real32 quality = 1.0);

	AcceptLanguageElement(const LanguageElement &le);

	/** Copy Constructor 
	 */
	AcceptLanguageElement(const AcceptLanguageElement &rhs);

	~AcceptLanguageElement();

	/** Gets the quality value associated with the language
	 * @return Real32 quality value
	 */
	Real32 getQuality() const;

	/** String representation of the AcceptLanguageElement
	 * @return the String
	 */
	String toString() const;
	
	/**
	 * Assignment operator, deep copy
	 * @param rhs AcceptLanguageElement
	 */
	//AcceptLanguageElement& operator=(const AcceptLanguageElement& rhs);
	
	/**
	 * Comparison based on the quality values ONLY, that is, the language tags of the elements are NOT 
	 * taken into account.
	 */
	Boolean operator>(const AcceptLanguageElement &rhs) const;
	
	/**
	 * Comparison based on the quality values ONLY, that is, the language tags of the elements are NOT 
	 * taken into account.
	 */
	Boolean operator<(const AcceptLanguageElement &rhs) const;
	
	/**
	 * True if the langauge tag portion equals that of rhs (case INSENSITIVE) AND
	 * if the quality values are equal 
	 * @param rhs AcceptLanguageElement
	 */
	Boolean operator==(const AcceptLanguageElement &rhs) const;
	
	/**
	 * True if the langauge tag portion does NOT equal that of rhs (case INSENSITIVE) OR
	 * if the quality values are NOT equal
	 * @param rhs AcceptLanguageElement 
	 */
	Boolean operator!=(const AcceptLanguageElement &rhs) const;

	/**
	 * Writes the string representation of this object to the stream
	 */
	PEGASUS_COMMON_LINKAGE friend PEGASUS_STD(ostream) &operator<<(PEGASUS_STD(ostream) &stream, AcceptLanguageElement ale);

			
}; // end AcceptLanguageElement

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T AcceptLanguageElement
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif  // PEGASUS_USE_EXPERIMENTAL_INTERFACES

#endif
