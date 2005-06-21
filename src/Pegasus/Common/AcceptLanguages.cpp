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
// Modified By: Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3514
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/AcceptLanguages.h>
#include <Pegasus/Common/LanguageParser.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/LanguageElementContainerRep.h>
#ifdef PEGASUS_HAS_ICU
# include <unicode/locid.h>
#endif
#if defined(PEGASUS_OS_OS400)
# include "OS400ConvertChar.h"
#endif

PEGASUS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////
//
// AcceptLanguages
//
//////////////////////////////////////////////////////////////

const AcceptLanguages AcceptLanguages::EMPTY = AcceptLanguages();

AcceptLanguages::AcceptLanguages()
    : LanguageElementContainer()
{
}

AcceptLanguages::AcceptLanguages(const String& hdr)
{
    if (hdr.size() > 0)
    {
        Array<String> values;
        LanguageParser lp;
        lp.parseHdr(values,hdr);
        buildLanguageElements(values);
        prioritize();
    }
}

AcceptLanguages::AcceptLanguages(const Array<LanguageElement>& aContainer)
    : LanguageElementContainer(aContainer)
{
    prioritize();
}

AcceptLanguages::AcceptLanguages(const Array<AcceptLanguageElement>& aContainer)
{
    for (Uint32 i = 0; i < aContainer.size(); i++)
    {
        LanguageElementContainer::append(aContainer[i]);
    }
    prioritize();
}

AcceptLanguages::AcceptLanguages(const AcceptLanguages& rhs)
    : LanguageElementContainer(rhs)
{
}

AcceptLanguages::~AcceptLanguages()
{
}

String AcceptLanguages::toString() const
{
    String s;
    for (Uint32 i = 0; i < _rep->container.size(); i++)
    {
        s.append(_rep->container[i].toString());
        if ((_rep->container[i].getLanguage() != "*") &&
            (_rep->container[i].getQuality() != 1))
        {
            char q[6];
            sprintf(q,"%4.3f", _rep->container[i].getQuality());
            s.append(";q=").append(q);
        }
        if (i < _rep->container.size() - 1)
        {
            s.append(",");
        }
    }
    return s;
}

PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& stream,
    const AcceptLanguages& al)
{
    stream << al.toString();
    return stream;
}

AcceptLanguages& AcceptLanguages::operator=(const AcceptLanguages& rhs)
{
    if (&rhs != this)
    {
        LanguageElementContainer::operator=(rhs);
    }
    return *this;
}

AcceptLanguageElement AcceptLanguages::getLanguageElement(Uint32 index) const
{
    return AcceptLanguageElement(_rep->getLanguageElement(index));
}

void AcceptLanguages::getAllLanguageElements(
    Array<AcceptLanguageElement>& elements) const
{
    Array<LanguageElement> tmp = _rep->getAllLanguageElements();
    for (Uint32 i = 0; i < tmp.size(); i++)
    {
        elements.append(AcceptLanguageElement(tmp[i]));
    }
}

Array<AcceptLanguageElement> AcceptLanguages::getAllLanguageElements() const
{
    Array<AcceptLanguageElement> elements;
    Array<LanguageElement> tmp = _rep->getAllLanguageElements();
    for (Uint32 i = 0; i < tmp.size(); i++)
    {
        elements.append(AcceptLanguageElement(tmp[i]));
    }
    return elements;
}

AcceptLanguageElement AcceptLanguages::itrNext()
{
    return AcceptLanguageElement(_rep->itrNext());
}

void AcceptLanguages::insert(const AcceptLanguageElement& element)
{
    _rep->append(element);
    prioritize();
}

void AcceptLanguages::remove(Uint32 index)
{
    _rep->remove(index);
    prioritize();
}

Uint32 AcceptLanguages::remove(const AcceptLanguageElement& element)
{
    int rc;
    if ((rc = _rep->remove(element)) != -1)
    {
        prioritize();
    }
    return rc;
}

Uint32 AcceptLanguages::find(const AcceptLanguageElement& element) const
{
    for (Uint32 i = 0; i < _rep->container.size(); i++)
    {
        if (element.getTag() == _rep->container[i].getTag())
        {
            if (element.getQuality() == _rep->container[i].getQuality())
            {
                return i;
            }
        }
    }
    return PEG_NOT_FOUND;
}

Uint32 AcceptLanguages::find(const String & language_tag, Real32 quality) const
{
    return find(AcceptLanguageElement(language_tag,quality));
}

void AcceptLanguages::buildLanguageElements(const Array<String>& values)
{
    for (Uint32 i = 0; i < values.size(); i++)
    {
        String language_tag;
        Real32 quality;
        LanguageParser lp;
        quality = lp.parseAcceptLanguageValue(language_tag,values[i]);
        append(AcceptLanguageElement(language_tag,quality));
    }
}

AcceptLanguages AcceptLanguages::getDefaultAcceptLanguages()
{
#if defined(PEGASUS_HAS_MESSAGES) && defined(PEGASUS_HAS_ICU)
    Locale default_loc = Locale::getDefault();

# ifdef PEGASUS_OS_OS400
    char* tmp = (char*)default_loc.getName();
    char tmp_[100];
    EtoA(strcpy(tmp_,tmp));
    try
    {
        return AcceptLanguages(tmp_);
    }
# else
    try
    {
        return AcceptLanguages(default_loc.getName());
    }
# endif
    catch (const InvalidAcceptLanguageHeader& e)
    {
        Logger::put_l(Logger::ERROR_LOG,System::CIMSERVER,Logger::SEVERE,
           "src.Server.cimserver.FAILED_TO_GET_PROCESS_LOCALE",
           "Could not convert the system locale to a valid accept-language "
               "format");
        Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            e.getMessage());
        return AcceptLanguages("*");
    }
#endif

    return AcceptLanguages();
}

void AcceptLanguages::prioritize()
{
    // sort according to quality value; if two qualities are equal then
    // preference is given to the existing order of the objects in the array.

    //array is already sorted
    if (_rep->container.size() <= 1)
    {
        return;
    }

    Boolean changed;
    int n = _rep->container.size();
    do
    {
        n--;               // make loop smaller each time.
        changed = false; // assume this is last pass over array
        for (int i=0; i<n; i++)
        {
            if (_rep->container[i].getQuality() <
                _rep->container[i+1].getQuality())
            {
                LanguageElement temp = _rep->container[i];
                _rep->container[i] = _rep->container[i+1];
                _rep->container[i+1] = temp;
                changed = true;  // after an exchange, must look again
            }
        }
    } while (changed);
}

PEGASUS_NAMESPACE_END
