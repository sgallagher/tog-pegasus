//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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

#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/LanguageParser.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/ArrayInternal.h>

PEGASUS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////
//
// AcceptLanguageListRep
//
//////////////////////////////////////////////////////////////

class AcceptLanguageListRep
{
public:
    Array<LanguageTag> languageTags;
    Array<Real32> qualityValues;
};

//////////////////////////////////////////////////////////////
//
// AcceptLanguageList
//
//////////////////////////////////////////////////////////////

AcceptLanguageList::AcceptLanguageList()
{
    _rep = new AcceptLanguageListRep();
}

AcceptLanguageList::AcceptLanguageList(
    const AcceptLanguageList& acceptLanguages)
{
    _rep = new AcceptLanguageListRep();
    AutoPtr<AcceptLanguageListRep> rep(_rep);

    _rep->languageTags = acceptLanguages._rep->languageTags;
    _rep->qualityValues = acceptLanguages._rep->qualityValues;

    rep.release();
}

AcceptLanguageList::~AcceptLanguageList()
{
    delete _rep;
}

AcceptLanguageList& AcceptLanguageList::operator=(
    const AcceptLanguageList& acceptLanguages)
{
    if (&acceptLanguages != this)
    {
        _rep->languageTags = acceptLanguages._rep->languageTags;
        _rep->qualityValues = acceptLanguages._rep->qualityValues;
    }
    return *this;
}

Uint32 AcceptLanguageList::size() const
{
    return _rep->languageTags.size();
}

LanguageTag AcceptLanguageList::getLanguageTag(Uint32 index) const
{
    return _rep->languageTags[index];
}

Real32 AcceptLanguageList::getQualityValue(Uint32 index) const
{
    return _rep->qualityValues[index];
}

void AcceptLanguageList::insert(
    const LanguageTag& languageTag,
    Real32 qualityValue)
{
    LanguageParser::validateQualityValue(qualityValue);

    // Insert in order of descending quality value

    Uint32 index;
    const Uint32 maxIndex = _rep->languageTags.size();

    for (index = 0; index < maxIndex; index++)
    {
        if (_rep->qualityValues[index] < qualityValue)
        {
            // Insert the new element before the element at this index
            break;
        }
    }

    _rep->languageTags.insert(index, languageTag);
    _rep->qualityValues.insert(index, qualityValue);
}

void AcceptLanguageList::remove(Uint32 index)
{
    _rep->languageTags.remove(index);
    _rep->qualityValues.remove(index);
}

Uint32 AcceptLanguageList::find(const LanguageTag& languageTag) const
{
    for (Uint32 i = 0; i < _rep->languageTags.size(); i++)
    {
        if (languageTag == _rep->languageTags[i])
        {
            return i;
        }
    }
    return PEG_NOT_FOUND;
}

void AcceptLanguageList::clear()
{
    _rep->languageTags.clear();
    _rep->qualityValues.clear();
}

Boolean AcceptLanguageList::operator==(
    const AcceptLanguageList& acceptLanguages) const
{
    if (_rep->languageTags.size() != acceptLanguages._rep->languageTags.size())
    {
        return false;
    }

    for (Uint32 i = 0; i < _rep->languageTags.size(); i++)
    {
        if ((_rep->languageTags[i] != acceptLanguages._rep->languageTags[i]) ||
            (_rep->qualityValues[i] != acceptLanguages._rep->qualityValues[i]))
        {
            return false;
        }
    }
    return true;
}

Boolean AcceptLanguageList::operator!=(
    const AcceptLanguageList& acceptLanguages) const
{
    return !(*this == acceptLanguages);
}

PEGASUS_NAMESPACE_END
