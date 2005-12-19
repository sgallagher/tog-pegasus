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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/ContentLanguages.h>
#include <Pegasus/Common/LanguageParser.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/ArrayInternal.h>

PEGASUS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////
//
// ContentLanguagesRep
//
//////////////////////////////////////////////////////////////

class ContentLanguagesRep
{
public:
    Array<LanguageTag> container;
};

//////////////////////////////////////////////////////////////
//
// ContentLanguages
//
//////////////////////////////////////////////////////////////

ContentLanguages::ContentLanguages()
{
    _rep = new ContentLanguagesRep();
}

ContentLanguages::ContentLanguages(const ContentLanguages& contentLanguages)
{
    _rep = new ContentLanguagesRep();
    AutoPtr<ContentLanguagesRep> rep(_rep);

    _rep->container = contentLanguages._rep->container;

    rep.release();
}

ContentLanguages::~ContentLanguages()
{
    delete _rep;
}

ContentLanguages& ContentLanguages::operator=(
    const ContentLanguages& contentLanguages)
{
    if (&contentLanguages != this)
    {
        _rep->container = contentLanguages._rep->container;
    }
    return *this;
}

Uint32 ContentLanguages::size() const
{
    return _rep->container.size();
}

LanguageTag ContentLanguages::getLanguageTag(Uint32 index) const
{
    return LanguageTag(_rep->container[index]);
}

void ContentLanguages::append(const LanguageTag& languageTag)
{
    // Disallow "*" language tag
    if (languageTag.toString() == "*")
    {
        MessageLoaderParms parms(
            "Common.LanguageParser.INVALID_LANGUAGE_TAG",
            "Invalid language tag \"$0\".", languageTag.toString());
        throw InvalidContentLanguageHeader(MessageLoader::getMessage(parms));
    }

    _rep->container.append(languageTag);
}

void ContentLanguages::remove(Uint32 index)
{
    _rep->container.remove(index);
}

Uint32 ContentLanguages::find(const LanguageTag& languageTag) const
{
    for (Uint32 i = 0; i < _rep->container.size(); i++)
    {
        if (languageTag == _rep->container[i])
        {
            return i;
        }
    }
    return PEG_NOT_FOUND;
}

void ContentLanguages::clear()
{
    _rep->container.clear();
}

Boolean ContentLanguages::operator==(
    const ContentLanguages& contentLanguages) const
{
    if (_rep->container.size() != contentLanguages._rep->container.size())
    {
        return false;
    }

    for (Uint32 i = 0; i < _rep->container.size(); i++)
    {
        if (_rep->container[i] != contentLanguages._rep->container[i])
        {
            return false;
        }
    }
    return true;
}

Boolean ContentLanguages::operator!=(
    const ContentLanguages& contentLanguages) const
{
    return !(*this == contentLanguages);
}

PEGASUS_NAMESPACE_END
