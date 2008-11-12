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

#include <Pegasus/Common/LanguageTag.h>
#include <Pegasus/Common/LanguageParser.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/InternalException.h>

PEGASUS_NAMESPACE_BEGIN

class LanguageTagRep
{
public:
    LanguageTagRep() : refs(1) { }
    AtomicInt refs;
    String tag;           // complete language tag
    String language;      // language part of language tag
    String country;       // country code part of the language tag
    String variant;       // language variant part of the language tag
};

static inline void _ref(const LanguageTagRep* rep)
{
    if (rep)
        ((LanguageTagRep*)rep)->refs++;
}

static inline void _unref(const LanguageTagRep* rep)
{
    if (rep && ((LanguageTagRep*)rep)->refs.decAndTestIfZero())
        delete (LanguageTagRep*)rep;
}

LanguageTag::LanguageTag()
{
    _rep = 0;
}

LanguageTag::LanguageTag(const String& languageTagString)
{
    _rep = new LanguageTagRep();

    try
    {
        LanguageParser::parseLanguageTag(
            languageTagString,
            _rep->language,
            _rep->country,
            _rep->variant);
    }
    catch (...)
    {
        _unref(_rep);
        throw;
    }

    _rep->tag = languageTagString;
}

LanguageTag::LanguageTag(const LanguageTag& x)
{
    _ref(_rep = x._rep);
}

LanguageTag::~LanguageTag()
{
    _unref(_rep);
}

LanguageTag& LanguageTag::operator=(const LanguageTag& x)
{
    if (_rep != x._rep)
    {
        _unref(_rep);
        _ref(_rep = x._rep);
    }

    return *this;
}

String LanguageTag::getLanguage() const
{
    CheckRep(_rep);
    return _rep->language;
}

String LanguageTag::getCountry() const
{
    CheckRep(_rep);
    return _rep->country;
}

String LanguageTag::getVariant() const
{
    CheckRep(_rep);
    return _rep->variant;
}

String LanguageTag::toString() const
{
    CheckRep(_rep);
    return _rep->tag;
}

Boolean LanguageTag::operator==(const LanguageTag& languageTag) const
{
    return String::equalNoCase(toString(), languageTag.toString());
}

Boolean LanguageTag::operator!=(const LanguageTag& languageTag) const
{
    return !String::equalNoCase(toString(), languageTag.toString());
}

PEGASUS_NAMESPACE_END
