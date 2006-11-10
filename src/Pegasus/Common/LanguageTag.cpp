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
    String tag;           // complete language tag
    String language;      // language part of language tag
    String country;       // country code part of the language tag
    String variant;       // language variant part of the language tag
};

// Check for a valid rep pointer.  Throw an UninitializedObjectException if
// the rep is null.
static void _checkRep(LanguageTagRep* rep)
{
    if (!rep)
    {
        throw UninitializedObjectException();
    }
}

LanguageTag::LanguageTag()
{
    _rep = 0;
}

LanguageTag::LanguageTag(const String& languageTagString)
{
    _rep = new LanguageTagRep();
    AutoPtr<LanguageTagRep> rep(_rep);

    LanguageParser::parseLanguageTag(
        languageTagString,
        _rep->language,
        _rep->country,
        _rep->variant);

    _rep->tag = languageTagString;

    rep.release();
}

LanguageTag::LanguageTag(const LanguageTag& languageTag)
{
    if (languageTag._rep)
    {
        _rep = new LanguageTagRep();
        AutoPtr<LanguageTagRep> rep(_rep);

        _rep->tag = languageTag._rep->tag;
        _rep->language = languageTag._rep->language;
        _rep->country = languageTag._rep->country;
        _rep->variant = languageTag._rep->variant;

        rep.release();
    }
    else
    {
        _rep = 0;
    }
}

LanguageTag::~LanguageTag()
{
    delete _rep;
}

LanguageTag& LanguageTag::operator=(const LanguageTag& languageTag)
{
    if (&languageTag != this)
    {
        if (!languageTag._rep)
        {
            delete _rep;
            _rep = 0;
        }
        else
        {
            if (!_rep)
            {
                _rep = new LanguageTagRep();
            }

            AutoPtr<LanguageTagRep> rep(_rep);

            _rep->tag = languageTag._rep->tag;
            _rep->language = languageTag._rep->language;
            _rep->country = languageTag._rep->country;
            _rep->variant = languageTag._rep->variant;

            rep.release();
        }
    }
    return *this;
}

String LanguageTag::getLanguage() const
{
    _checkRep(_rep);
    return _rep->language;
}

String LanguageTag::getCountry() const
{
    _checkRep(_rep);
    return _rep->country;
}

String LanguageTag::getVariant() const
{
    _checkRep(_rep);
    return _rep->variant;
}

String LanguageTag::toString() const
{
    _checkRep(_rep);
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
