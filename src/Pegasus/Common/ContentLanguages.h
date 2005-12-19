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

#ifndef Pegasus_ContentLanguages_h
#define Pegasus_ContentLanguages_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/LanguageTag.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

class ContentLanguagesRep;

//////////////////////////////////////////////////////////////
//
// ContentLanguages
//
//////////////////////////////////////////////////////////////

/** <I><B>Experimental Interface</B></I><BR>
    This class represents a list of content languages (such as may appear
    in an HTTP Content-Language header value).  It is managed as a list of
    LanguageTag objects.
 */
class PEGASUS_COMMON_LINKAGE ContentLanguages
{
public:

    /**
        Constructs an empty ContentLanguages object.
     */
    ContentLanguages();

    /**
        Copy constructor.
        @param contentLanguages The ContentLanguages object to copy.
     */
    ContentLanguages(const ContentLanguages& contentLanguages);

    /**
        Destructor.
     */
    ~ContentLanguages();

    /**
        Assignment operator.
        @param contentLanguages The ContentLanguages object to copy.
     */
    ContentLanguages& operator=(const ContentLanguages& contentLanguages);

    /**
        Returns the number of LanguageTags in the ContentLanguages object.
        @return Integer size of the ContentLanguages list.
     */
    Uint32 size() const;

    /**
        Accesses a LanguageTag at a specified index.
        @param index Integer index of the LanguageTag to access.
        Valid indices range from 0 to size()-1.
        @return The LanguageTag corresponding to the specified index.
        @exception IndexOutOfBoundsException If the specified index is out of
        range.
     */
    LanguageTag getLanguageTag(Uint32 index) const;

    /**
        Appends a LanguageTag to the ContentLanguages object.
        @param languageTag The LanguageTag to append.
     */
    void append(const LanguageTag& languageTag);

    /**
        Removes the specified LanguageTag from the ContentLanguages object.
        @param index Integer index of the LanguageTag to remove.
        @exception IndexOutOfBoundsException If the specified index is out of
        range.
     */
    void remove(Uint32 index);

    /**
        Finds the specified LanguageTag in the ContentLanguages object and
        returns its index.
        @param languageTag The LanguageTag to find.
        @return Integer index of the LanguageTag, if found; otherwise
        PEG_NOT_FOUND.
     */
    Uint32 find(const LanguageTag& languageTag) const;

    /**
        Removes all the LanguageTags from the ContentLanguages object.
     */
    void clear();

    /**
        Tests ContentLanguages objects for equality.
        @param contentLanguages A ContentLanguages object to be compared.
        @return True if the ContentLanguages objects contain the same
        ContentLanguagesElements in the same order, false otherwise.
     */
    Boolean operator==(const ContentLanguages& contentLanguages) const;

    /**
        Tests ContentLanguages objects for inequality.
        @param contentLanguages A ContentLanguages object to be compared.
        @return False if the ContentLanguages objects contain the same
        ContentLanguagesElements in the same order, true otherwise.
     */
    Boolean operator!=(const ContentLanguages& contentLanguages) const;

private:
    ContentLanguagesRep *_rep;
};

PEGASUS_NAMESPACE_END

#endif  // PEGASUS_USE_EXPERIMENTAL_INTERFACES

#endif
