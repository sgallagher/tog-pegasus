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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdarg>
#include <cctype>
#include <Pegasus/Common/System.h>
#include <Pegasus/Repository/SourceTypes.h>
#include "cimmofSourceConsumer.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// Local routines:
//
//==============================================================================

static size_t _indent = 0;

class Str
{
public:
    Str(const String& s) : _cstr(s.getCString()) { }
    Str(const CIMName& n) : _cstr(n.getString().getCString()) { } 
    Str(const CIMNamespaceName& n) : _cstr(n.getString().getCString()) { } 
    Str(const Exception& e) : _cstr(e.getMessage().getCString()) { } 
    Str(const CIMDateTime& x) : _cstr(x.toString().getCString()) { }
    Str(const CIMObjectPath& x) : _cstr(x.toString().getCString()) { } 
    const char* operator*() const { return (const char*)_cstr; }
    operator const char*() const { return (const char*)_cstr; }
private:
    CString _cstr;
};

static void _vout(FILE* os, const char* format, va_list ap)
{
    for (size_t i = 0; i < _indent; i++)
        fprintf(os, "    ");

    vfprintf(os, format, ap);
}

PEGASUS_FORMAT(2, 3)
static void _throw(CIMStatusCode code, const char* format, ...)
{
    char buffer[4096];

    va_list ap;
    va_start(ap, format);
    vsprintf(buffer, format, ap);
    va_end(ap);
    throw CIMException(code, format);
}

static void _line(FILE* os)
{
    fprintf(os, "//");

    for (size_t i = 0; i < 78; i++)
        fputc('=', os);

    fputc('\n', os);
}

PEGASUS_FORMAT(2, 3)
static void _box(FILE* os, const char* format, ...)
{
    _line(os);

    fprintf(os, "//\n");

    fprintf(os, "// ");

    va_list ap;
    va_start(ap, format);
    vfprintf(os, format, ap);
    va_end(ap);

    fputc('\n', os);
    fprintf(os, "//\n");

    _line(os);
}

static void _writeHeaderFile()
{
    const char text[] =
        "#ifndef _repository_h\n"
        "#define _repository_h\n"
        "\n"
        "#include <Pegasus/Repository/SourceTypes.h>\n"
        "\n"
        "PEGASUS_NAMESPACE_BEGIN\n"
        "\n"
        "extern SourceRepository* source_repository;\n"
        "\n"
        "PEGASUS_NAMESPACE_END\n"
        "\n"
        "#endif /* _repository_h */\n"
        ;

    const char PATH[] = "repository.h";
    FILE* os = fopen(PATH, "wb");

    if (!os)
    {
        fprintf(stderr, "cimmofl: failed to open \"%s\" for write\n", PATH);
        exit(1);
    }

    size_t n = sizeof(text) - 1;

    if (fwrite(text, 1, n, os) != n)
    {
        fprintf(stderr, "cimmofl: failed to write\"%s\"\n", PATH);
        exit(1);
    }

    fclose(os);
}

static String _makeIdent(const String& str)
{
    // Build a legal C identifier from str. Translate all illegal characters
    // to underscores.

    String r;

    for (Uint32 i = 0; i < str.size(); i++)
    {
        Uint16 c = str[i];

        if (c < 127 && (isalnum(c) || c == '_'))
            r.append(c);
        else
            r.append('_');
    }

    return r;
}


static const char* _typeNames[] =
{
    "CIMTYPE_BOOLEAN",
    "CIMTYPE_UINT8",
    "CIMTYPE_SINT8",
    "CIMTYPE_UINT16",
    "CIMTYPE_SINT16",
    "CIMTYPE_UINT32",
    "CIMTYPE_SINT32",
    "CIMTYPE_UINT64",
    "CIMTYPE_SINT64",
    "CIMTYPE_REAL32",
    "CIMTYPE_REAL64",
    "CIMTYPE_CHAR16",
    "CIMTYPE_STRING",
    "CIMTYPE_DATETIME",
    "CIMTYPE_REFERENCE",
    "CIMTYPE_OBJECT",
    "CIMTYPE_INSTANCE",
};

//==============================================================================
//
// cimmofSourceConsumer
//
//==============================================================================

cimmofSourceConsumer::cimmofSourceConsumer() : _os(0)
{
}

cimmofSourceConsumer::~cimmofSourceConsumer()
{
}

void cimmofSourceConsumer::addClass(
    const CIMNamespaceName& nameSpace,
    CIMClass& cimClass)
{
    if (_findClass(nameSpace, cimClass.getClassName()) != PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_ALREADY_EXISTS, "class already defined: %s:%s", 
            *Str(nameSpace), *Str(cimClass.getClassName()));
    }

    _classes.append(Class(nameSpace, cimClass));
}

void cimmofSourceConsumer::addQualifier(
    const CIMNamespaceName& nameSpace,
    CIMQualifierDecl& cimQualifierDecl)
{
    if (_findQualifier(nameSpace, cimQualifierDecl.getName()) != PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_ALREADY_EXISTS, "qualifier already defined: %s:%s", 
            *Str(nameSpace), *Str(cimQualifierDecl.getName()));
    }

    _qualifiers.append(Qualifier(nameSpace, cimQualifierDecl));
}

void cimmofSourceConsumer::addInstance(
    const CIMNamespaceName& nameSpace,
    CIMInstance& instance)
{
    // Ignore all instances!
}

CIMQualifierDecl cimmofSourceConsumer::getQualifierDecl(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    Uint32 pos = _findQualifier(nameSpace, qualifierName);

    if (pos == PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_NOT_FOUND,
            "undefined qualifier: %s:%s", *Str(nameSpace), *Str(qualifierName));
    }

    return _qualifiers[pos].second;
}

CIMClass cimmofSourceConsumer::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    Uint32 pos = _findClass(nameSpace, className);

    if (pos == PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_NOT_FOUND, 
            "undefined class: %s:%s", *Str(nameSpace), *Str(className));
    }

    return _classes[pos].second;
}

void cimmofSourceConsumer::modifyClass(
    const CIMNamespaceName& nameSpace,
    CIMClass& cimClass)
{
    Uint32 pos = _findClass(nameSpace, cimClass.getClassName());

    if (pos == PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_NOT_FOUND, "undefined class: %s:%s", 
            *Str(nameSpace), *Str(cimClass.getClassName()));
    }

    _classes[pos].second = cimClass;
}

void cimmofSourceConsumer::createNameSpace(
    const CIMNamespaceName& nameSpace)
{
    if (_findNameSpace(nameSpace) != PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_ALREADY_EXISTS, "namespace already exists: %s", 
            *Str(nameSpace));
    }

    _nameSpaces.append(nameSpace.getString());
}

void cimmofSourceConsumer::start()
{
}

void cimmofSourceConsumer::finish()
{
    // Write header file:

    _writeHeaderFile();

    // Open source file:

    const char PATH[] = "repository.cpp";
    _os = fopen(PATH, "wb");

    if (!_os)
    {
        fprintf(stderr, "cimmofl: failed to open \"%s\" for write\n", PATH);
        exit(1);
    }

    // Write prologue:

    _writeSourcePrologue();

    // Write each namespace:

    for (Uint32 i = 0; i < _nameSpaces.size(); i++)
        _writeNameSpace(_nameSpaces[i]);

    // Write epilogue:

    _writeSourceEpilogue();
}

Uint32 cimmofSourceConsumer::_findNameSpace(
    const CIMNamespaceName& nameSpace) const
{
    for (Uint32 i = 0; i < _nameSpaces.size(); i++)
    {
        if (_nameSpaces[i] == nameSpace)
            return i;
    }

    // Not found!
    return PEG_NOT_FOUND;
}

Uint32 cimmofSourceConsumer::_findClass(
    const CIMNamespaceName& nameSpace, 
    const CIMName& className) const
{
    for (Uint32 i = 0; i < _classes.size(); i++)
    {
        const Class& c = _classes[i];

        if (c.first == nameSpace && c.second.getClassName() == className)
            return i;
    }

    // Not found!
    return PEG_NOT_FOUND;
}

Uint32 cimmofSourceConsumer::_findQualifier(
    const CIMNamespaceName& nameSpace, 
    const CIMName& qualifierName) const
{
    for (Uint32 i = 0; i < _qualifiers.size(); i++)
    {
        const Qualifier& q = _qualifiers[i];

        if (q.first == nameSpace && q.second.getName() == qualifierName)
            return i;
    }

    // Not found!
    return PEG_NOT_FOUND;
}

void cimmofSourceConsumer::_writeSourcePrologue()
{
    _outn("#include \"repository.h\"");
    _nl();
    _outn("PEGASUS_NAMESPACE_BEGIN");
    _nl();
}

void cimmofSourceConsumer::_writeSourceEpilogue()
{
    _outn("PEGASUS_NAMESPACE_END");
}

template<class C>
static void _writeFlags(
    FILE* os, 
    const C& c, 
    bool isProperty,
    bool isParameter)
{
    // Build up flags mask:

    Uint32 flags = 0;

    if (isProperty)
        flags |= PEGASUS_FLAG_READ;

    if (isParameter)
        flags |= PEGASUS_FLAG_IN;

    for (Uint32 i = 0; i < c.getQualifierCount(); i++)
    {
        CIMConstQualifier cq = c.getQualifier(i);
        const CIMName& qn = cq.getName();

        if (cq.getType() != CIMTYPE_BOOLEAN || cq.isArray())
            continue;

        Boolean x;
        cq.getValue().get(x);


        if (System::strcasecmp(*Str(qn), "KEY") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_KEY;
            else
                flags &= ~PEGASUS_FLAG_KEY;
        }
        if (System::strcasecmp(*Str(qn), "IN") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_IN;
            else
                flags &= ~PEGASUS_FLAG_IN;
        }
        if (System::strcasecmp(*Str(qn), "OUT") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_OUT;
            else
                flags &= ~PEGASUS_FLAG_OUT;
        }
        if (System::strcasecmp(*Str(qn), "ABSTRACT") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_ABSTRACT;
            else
                flags &= ~PEGASUS_FLAG_ABSTRACT;
        }
        if (System::strcasecmp(*Str(qn), "AGGREGATE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_AGGREGATE;
            else
                flags &= ~PEGASUS_FLAG_AGGREGATE;
        }
        if (System::strcasecmp(*Str(qn), "AGGREGATION") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_AGGREGATION;
            else
                flags &= ~PEGASUS_FLAG_AGGREGATION;
        }
        if (System::strcasecmp(*Str(qn), "COUNTER") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_COUNTER;
            else
                flags &= ~PEGASUS_FLAG_COUNTER;
        }
        if (System::strcasecmp(*Str(qn), "DELETE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_DELETE;
            else
                flags &= ~PEGASUS_FLAG_DELETE;
        }
        if (System::strcasecmp(*Str(qn), "DN") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_DN;
            else
                flags &= ~PEGASUS_FLAG_DN;
        }
        if (System::strcasecmp(*Str(qn), "EMBEDDEDOBJECT") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_EMBEDDEDOBJECT;
            else
                flags &= ~PEGASUS_FLAG_EMBEDDEDOBJECT;
        }
        if (System::strcasecmp(*Str(qn), "EXPENSIVE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_EXPENSIVE;
            else
                flags &= ~PEGASUS_FLAG_EXPENSIVE;
        }
        if (System::strcasecmp(*Str(qn), "EXPERIMENTAL") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_EXPERIMENTAL;
            else
                flags &= ~PEGASUS_FLAG_EXPERIMENTAL;
        }
        if (System::strcasecmp(*Str(qn), "GAUGE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_GAUGE;
            else
                flags &= ~PEGASUS_FLAG_GAUGE;
        }
        if (System::strcasecmp(*Str(qn), "IFDELETED") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_IFDELETED;
            else
                flags &= ~PEGASUS_FLAG_IFDELETED;
        }
        if (System::strcasecmp(*Str(qn), "INVISIBLE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_INVISIBLE;
            else
                flags &= ~PEGASUS_FLAG_INVISIBLE;
        }
        if (System::strcasecmp(*Str(qn), "LARGE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_LARGE;
            else
                flags &= ~PEGASUS_FLAG_LARGE;
        }
        if (System::strcasecmp(*Str(qn), "OCTETSTRING") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_OCTETSTRING;
            else
                flags &= ~PEGASUS_FLAG_OCTETSTRING;
        }
        if (System::strcasecmp(*Str(qn), "READ") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_READ;
            else
                flags &= ~PEGASUS_FLAG_READ;
        }
        if (System::strcasecmp(*Str(qn), "REQUIRED") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_REQUIRED;
            else
                flags &= ~PEGASUS_FLAG_REQUIRED;
        }
        if (System::strcasecmp(*Str(qn), "STATIC") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_STATIC;
            else
                flags &= ~PEGASUS_FLAG_STATIC;
        }
        if (System::strcasecmp(*Str(qn), "TERMINAL") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_TERMINAL;
            else
                flags &= ~PEGASUS_FLAG_TERMINAL;
        }
        if (System::strcasecmp(*Str(qn), "WEAK") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_WEAK;
            else
                flags &= ~PEGASUS_FLAG_WEAK;
        }
        if (System::strcasecmp(*Str(qn), "WRITE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_WRITE;
            else
                flags &= ~PEGASUS_FLAG_WRITE;
        }
    }

    // Write flags mask:

    if (flags & PEGASUS_FLAG_KEY)
        fprintf(os, "|PEGASUS_FLAG_KEY");
    if (flags && (flags & PEGASUS_FLAG_IN))
        fprintf(os, "|PEGASUS_FLAG_IN");
    if (flags && (flags & PEGASUS_FLAG_OUT))
        fprintf(os, "|PEGASUS_FLAG_OUT");
    if (flags & PEGASUS_FLAG_ABSTRACT)
        fprintf(os, "|PEGASUS_FLAG_ABSTRACT");
    if (flags & PEGASUS_FLAG_AGGREGATE)
        fprintf(os, "|PEGASUS_FLAG_AGGREGATE");
    if (flags & PEGASUS_FLAG_AGGREGATION)
        fprintf(os, "|PEGASUS_FLAG_AGGREGATION");
    if (flags & PEGASUS_FLAG_COUNTER)
        fprintf(os, "|PEGASUS_FLAG_COUNTER");
    if (flags & PEGASUS_FLAG_DELETE)
        fprintf(os, "|PEGASUS_FLAG_DELETE");
    if (flags & PEGASUS_FLAG_DN)
        fprintf(os, "|PEGASUS_FLAG_DN");
    if (flags & PEGASUS_FLAG_EMBEDDEDOBJECT)
        fprintf(os, "|PEGASUS_FLAG_EMBEDDEDOBJECT");
    if (flags & PEGASUS_FLAG_EXPENSIVE)
        fprintf(os, "|PEGASUS_FLAG_EXPENSIVE");
    if (flags & PEGASUS_FLAG_EXPERIMENTAL)
        fprintf(os, "|PEGASUS_FLAG_EXPERIMENTAL");
    if (flags & PEGASUS_FLAG_GAUGE)
        fprintf(os, "|PEGASUS_FLAG_GAUGE");
    if (flags & PEGASUS_FLAG_IFDELETED)
        fprintf(os, "|PEGASUS_FLAG_IFDELETED");
    if (flags & PEGASUS_FLAG_INVISIBLE)
        fprintf(os, "|PEGASUS_FLAG_INVISIBLE");
    if (flags & PEGASUS_FLAG_LARGE)
        fprintf(os, "|PEGASUS_FLAG_LARGE");
    if (flags & PEGASUS_FLAG_OCTETSTRING)
        fprintf(os, "|PEGASUS_FLAG_OCTETSTRING");
    if (flags & PEGASUS_FLAG_READ)
        fprintf(os, "|PEGASUS_FLAG_READ");
    if (flags & PEGASUS_FLAG_REQUIRED)
        fprintf(os, "|PEGASUS_FLAG_REQUIRED");
    if (flags & PEGASUS_FLAG_STATIC)
        fprintf(os, "|PEGASUS_FLAG_STATIC");
    if (flags & PEGASUS_FLAG_TERMINAL)
        fprintf(os, "|PEGASUS_FLAG_TERMINAL");
    if (flags & PEGASUS_FLAG_WEAK)
        fprintf(os, "|PEGASUS_FLAG_WEAK");
    if (flags & PEGASUS_FLAG_WRITE)
        fprintf(os, "|PEGASUS_FLAG_WRITE");
}

void cimmofSourceConsumer::_writeProperty(
    const CIMNamespaceName& nameSpace,
    const CIMName& cn,
    const CIMConstProperty& cp)
{
    String ns = _makeIdent(nameSpace.getString());
    CIMName pn = cp.getName();
    CIMType ct = cp.getType();

    _outn("static SourceProperty _%s__%s_%s =", *Str(ns), *Str(cn), *Str(pn));
    _outn("{");
    _indent++;

    // SourceProperty.flags:

    _out("PEGASUS_FLAG_PROPERTY");
    _writeFlags(_os, cp, true, false);
    fprintf(_os, ",\n");

    // SourceProperty.name:

    _outn("\"%s\", /* name */", *Str(pn));

    // SourceProperty.type:

    _outn("%s, /* type */", _typeNames[ct]);

    // SourceProperty.subscript:

    if (cp.isArray())
    {
        Uint32 n = cp.getArraySize();
        _outn("%u, /* subscript */", n);
    }
    else
    {
        _outn("-1, /* subscript */");
    }

    // SourceProperty.refClass:

    if (ct == CIMTYPE_REFERENCE)
    {
        const CIMName& rcn = cp.getReferenceClassName();
        _outn("&_%s__%s, /* refClass */\n", *Str(ns), *Str(rcn));
    }
    else
    {
        _outn("0, /* refClass */");
    }

    _indent--;
    _outn("};");
    _nl();

    // ATTN: define qualifiers:
}

void cimmofSourceConsumer::_writeParameter(
    const CIMNamespaceName& nameSpace,
    const CIMName& cn,
    const CIMName& mn,
    const CIMConstParameter& cp)
{
    String ns = _makeIdent(nameSpace.getString());
    CIMName pn = cp.getName();
    CIMType ct = cp.getType();

    _outn("static SourceProperty _%s__%s_%s_%s =", 
        *Str(ns), *Str(cn), *Str(mn), *Str(pn));
    _outn("{");
    _indent++;

    // SourceProperty.flags:

    _out("PEGASUS_FLAG_PROPERTY");
    _writeFlags(_os, cp, false, true);
    fprintf(_os, ",\n");

    // SourceProperty.name:

    _outn("\"%s\", /* name */", *Str(pn));

    // SourceProperty.type:

    _outn("%s, /* type */", _typeNames[ct]);

    // SourceProperty.subscript:

    if (cp.isArray())
    {
        Uint32 n = cp.getArraySize();
        _outn("%u, /* subscript */", n);
    }
    else
    {
        _outn("-1, /* subscript */");
    }

    // SourceProperty.refClass:

    if (ct == CIMTYPE_REFERENCE)
    {
        const CIMName& rcn = cp.getReferenceClassName();
        _outn("&_%s__%s, /* refClass */\n", *Str(ns), *Str(rcn));
    }
    else
    {
        _outn("0, /* refClass */");
    }

    _indent--;
    _outn("};");
    _nl();

    // ATTN: define qualifiers:
}

void cimmofSourceConsumer::_writeMethod(
    const CIMNamespaceName& nameSpace,
    const CIMName& cn,
    const CIMConstMethod& cm)
{
    String ns = _makeIdent(nameSpace.getString());
    CIMName mn = cm.getName();

    // Write parameter definitions:

    Array<CIMName> parameterNames;

    for (Uint32 i = 0; i < cm.getParameterCount(); i++)
    {
        CIMConstParameter cp = cm.getParameter(i);
        _writeParameter(nameSpace, cn, mn, cp);
        parameterNames.append(cp.getName());
    }

    // Write parameters array:

    _outn("static SourceProperty* _%s__%s_%s_parameters[] =", 
        *Str(ns), *Str(cn), *Str(mn));
    _outn("{");
    _indent++;

    for (Uint32 i = 0; i < parameterNames.size(); i++)
    {
        const CIMName& pn = parameterNames[i];
        _outn("&_%s__%s_%s_%s,", 
            *Str(ns), *Str(cn), *Str(mn), *Str(pn));
    }

    _outn("0,");
    _indent--;
    _outn("};");
    _nl();

    // Method header:

    _outn("static SourceMethod _%s__%s_%s =", *Str(ns), *Str(cn), *Str(mn));
    _outn("{");
    _indent++;

    // SourceMethod.flags:

    _out("PEGASUS_FLAG_METHOD");
    _writeFlags(_os, cm, false, false);
    fprintf(_os, ",\n");

    // SourceMethod.name:

    _outn("\"%s\", /* name */", *Str(cn));

    // SourceMethod.type:

    // SourceProperty.type:

    _outn("%s, /* type */", _typeNames[cm.getType()]);

    // SourceMethod.parameter:

    _outn("_%s__%s_%s_parameters,", *Str(ns), *Str(cn), *Str(mn));

    // Method footer:

    _indent--;
    _outn("};");
    _nl();
}

static bool _testBooleanQualifier(const CIMClass& cc, const CIMName& name)
{
    Uint32 pos = cc.findQualifier(name);

    if (pos == PEG_NOT_FOUND)
        return false;

    CIMConstQualifier cq = cc.getQualifier(pos);

    if (cq.getType() != CIMTYPE_BOOLEAN || cq.isArray())
        return false;

    Boolean x;
    cq.getValue().get(x);
    return x;
}

void cimmofSourceConsumer::_writeClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& cc)
{
    String ns = _makeIdent(nameSpace.getString());
    CIMName cn = cc.getClassName();

    // Write comment:

    _box(_os, "Class: %s:%s", *Str(nameSpace), *Str(cn));
    _nl();

    // Write property definitions:

    Array<CIMName> featureNames;

    for (Uint32 i = 0; i < cc.getPropertyCount(); i++)
    {
        CIMConstProperty cp = cc.getProperty(i);
        _writeProperty(nameSpace, cc.getClassName(), cp);
        featureNames.append(cp.getName());
    }

    // Write method definitions:

    for (Uint32 i = 0; i < cc.getMethodCount(); i++)
    {
        CIMConstMethod cm = cc.getMethod(i);
        _writeMethod(nameSpace, cc.getClassName(), cm);
        featureNames.append(cm.getName());
    }

    // Write feature array:

    _outn("static SourceFeature* _%s__%s_features[] =", *Str(ns), *Str(cn));
    _outn("{");
    _indent++;

    for (Uint32 i = 0; i < featureNames.size(); i++)
    {
        const CIMName& fn = featureNames[i];
        _outn("(SourceFeature*)&_%s__%s_%s,", *Str(ns), *Str(cn), *Str(fn));
    }

    _outn("0,");
    _indent--;
    _outn("};");
    _nl();

    // Class header:

    _outn("static SourceClass _%s__%s =", *Str(ns), *Str(cn));
    _outn("{");
    _indent++;

    // SourceClass.flags:

    if (_testBooleanQualifier(cc, "Association"))
        _out("PEGASUS_FLAG_ASSOCIATION");
    else if (_testBooleanQualifier(cc, "Indication"))
        _out("PEGASUS_FLAG_INDICATION");
    else
        _out("PEGASUS_FLAG_CLASS");

    _writeFlags(_os, cc, false, false);
    fprintf(_os, ",\n");

    // SourceClass.name:

    _outn("\"%s\", /* name */", *Str(cn));

    // SourceClass.super:

    const CIMName& scn = cc.getSuperClassName();

    if (scn.isNull())
        _outn("0, /* super */");
    else
        _outn("&_%s__%s, /* super */", *Str(ns), *Str(scn));

    // SourceClass.features:

    _outn("_%s__%s_features,", *Str(ns), *Str(cn));

    // Class footer:

    _indent--;
    _outn("};");
    _nl();
}

void cimmofSourceConsumer::_writeNameSpace(const CIMNamespaceName& nameSpace)
{
    String ns = _makeIdent(nameSpace.getString());

    // Write classes:

    for (Uint32 i = 0; i < _classes.size(); i++)
    {
        const Class& c = _classes[i];

        if (c.first != nameSpace)
            continue;

        _writeClass(c.first, c.second);
    }

    // Write classes list:

    _outn("static SourceClass* _%s__classes[] =", *Str(ns));
    _outn("{");
    _indent++;

    for (Uint32 i = 0; i < _classes.size(); i++)
    {
        const Class& c = _classes[i];

        if (c.first != nameSpace)
            continue;

        _outn("&_%s__%s,", *Str(ns), *Str(c.second.getClassName()));
    }

    _outn("0,");

    _indent--;
    _outn("};");
    _nl();

    // Write SourceNameSpace structure:

    _outn("SourceNameSpace %s_namespace =", *Str(ns));
    _outn("{");
    _outn("    \"%s\",", *Str(nameSpace));
    _outn("    _%s__classes,", *Str(ns));
    _outn("};");
    _nl();
}

PEGASUS_FORMAT(2, 3)
void cimmofSourceConsumer::_out(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    _vout(_os, format, ap);
    va_end(ap);
}

PEGASUS_FORMAT(2, 3)
void cimmofSourceConsumer::_outn(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    _vout(_os, format, ap);
    va_end(ap);
    fputc('\n', _os);
}

void cimmofSourceConsumer::_nl()
{
    _out("\n");
}

PEGASUS_NAMESPACE_END
