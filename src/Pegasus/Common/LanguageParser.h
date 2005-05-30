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
// Modified By: Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3697, 3698, 3699, 3700
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3701, 3702, 3703
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_LanguageParser_h
#define Pegasus_LanguageParser_h

#include <cstdlib>
#include <cctype>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN


/** This class parses and validates AcceptLanguage OR ContentLanguage values
 * from the respective HTTP headers, and provides various utility functions
 * to parse and manipulate language information
 */
class PEGASUS_COMMON_LINKAGE LanguageParser{

public:

/**	This member is used to represent an empty LanguageParser. Using this 
    member avoids construction of an empty LanguageParser
    (e.g., LanguageParser()).
*/
static const LanguageParser EMPTY;

/** Parses the value portion of either an AcceptLanguage HTTP header or a 
 * ContentLanguage HTTP header, extracts the individual fields and stores
 * them in an array.
 * @param values Array<String>& where the individual fields are stored
 * @param hdr String the value portion of the HTTP Accept/ContentLanguage header
 * this parameter is passed as object intentionally
 */
void parseHdr(Array<String> &values, String hdr);

/** Parses an AcceptLanguage or ContentLanguage value from a header.
 * If a valid AcceptLanguage value is determined to contain a quality
 * value, then this quality once validated is returned. Otherwise -1 is returned.
 * It is up to the caller to know in what context this call is being made,
 * that is: it is the callers responsibility to know whether 
 * an AcceptLanguage or a ContentLanguage value is being parsed.
 * @param language String& where the valid language is stored
 * @param hdr String the value being parsed and validated
 * @return Real32 if a valid quality found then 1 <=> 0, otherwise 1
 * @exception throws ???
 */
Real32 parseAcceptLanguageValue(String &language, String hdr);

String parseContentLanguageValue(const String& hdr);

/** 
 * Given a language ex: en-US-xx, 
 * returns the base of the language "en".
 * @param language String
 * @param separator Char16 the character that separates the various subtags
 * @return String base of the language parameter
 */
String getLanguage(const String & language_tag);

String getCountry(const String & language_tag);

String getVariant(const String & language_tag);

private:

/**
 * Fills the first position in the array with the most general part of
 * the language String, each subsequent postion is the array gets more 
 * and more detailed.  For example: en-us-fooblah would be returned as
 * ["en"]["us"]["fooblah"]
 * @param subtags Array<String>& array filled in with the root and subtags
 * @param language String the language to parse
 * @param separator Char16 the character that separates the various subtags
 * @return String the root of the language tag
 */
void parseLanguageSubtags(Array<String> &subtags, String language_tag);

/** Breaks the language into its parts(subtags), then validates each part
 * according to the RFCs.
 * @param s String language to be validated
 * @return Boolean
 */
Boolean isValid(const String& language_tag, Boolean length_check = true);

/** Checks if the String has all characters in A-Z OR a-z range.
 * @param s String
 * @return Boolean
 */
Boolean checkAlpha(const CString & _str);

CString replaceSeparator(const CString & _s, char new_sep = '-');

char findSeparator(const CString & _str);

String convertPrivateLanguageTag(const String & language_tag);

}; //end LanguageParser

PEGASUS_NAMESPACE_END

#endif  // PEGASUS_USE_EXPERIMENTAL_INTERFACES

#endif
