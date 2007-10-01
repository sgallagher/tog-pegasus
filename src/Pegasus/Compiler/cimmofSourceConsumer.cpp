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

#define PEGASUS_LLD "%" PEGASUS_64BIT_CONVERSION_WIDTH "d"
#define PEGASUS_LLU "%" PEGASUS_64BIT_CONVERSION_WIDTH "u"

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

static void _writeHeaderFile(const String& ns)
{
    const char format[] =
        "#ifndef _%s_namespace_h\n"
        "#define _%s_namespace_h\n"
        "\n"
        "#include <Pegasus/Repository/SourceTypes.h>\n"
        "\n"
        "PEGASUS_NAMESPACE_BEGIN\n"
        "\n"
        "extern const SourceNameSpace %s_namespace;\n"
        "\n"
        "PEGASUS_NAMESPACE_END\n"
        "\n"
        "#endif /* _%s_namespace_h */\n"
        ;

    String path = ns + "_namespace.h";
    FILE* os = fopen(*Str(path), "wb");

    if (!os)
    {
        fprintf(stderr, "cimmofl: failed to open \"%s\" for write\n", 
            *Str(path));
        exit(1);
    }

    fprintf(os, format, *Str(ns), *Str(ns), *Str(ns), *Str(ns));

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

static bool _is_printable(const char* s)
{
    for (; *s; s++)
    {
        if (!isprint(*s))
            return false;
    }

    return true;
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
        else if (System::strcasecmp(*Str(qn), "IN") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_IN;
            else
                flags &= ~PEGASUS_FLAG_IN;
        }
        else if (System::strcasecmp(*Str(qn), "OUT") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_OUT;
            else
                flags &= ~PEGASUS_FLAG_OUT;
        }
        else if (System::strcasecmp(*Str(qn), "ABSTRACT") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_ABSTRACT;
            else
                flags &= ~PEGASUS_FLAG_ABSTRACT;
        }
        else if (System::strcasecmp(*Str(qn), "AGGREGATE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_AGGREGATE;
            else
                flags &= ~PEGASUS_FLAG_AGGREGATE;
        }
        else if (System::strcasecmp(*Str(qn), "AGGREGATION") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_AGGREGATION;
            else
                flags &= ~PEGASUS_FLAG_AGGREGATION;
        }
        else if (System::strcasecmp(*Str(qn), "COUNTER") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_COUNTER;
            else
                flags &= ~PEGASUS_FLAG_COUNTER;
        }
        else if (System::strcasecmp(*Str(qn), "DELETE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_DELETE;
            else
                flags &= ~PEGASUS_FLAG_DELETE;
        }
        else if (System::strcasecmp(*Str(qn), "DN") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_DN;
            else
                flags &= ~PEGASUS_FLAG_DN;
        }
        else if (System::strcasecmp(*Str(qn), "EMBEDDEDOBJECT") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_EMBEDDEDOBJECT;
            else
                flags &= ~PEGASUS_FLAG_EMBEDDEDOBJECT;
        }
        else if (System::strcasecmp(*Str(qn), "EXPENSIVE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_EXPENSIVE;
            else
                flags &= ~PEGASUS_FLAG_EXPENSIVE;
        }
        else if (System::strcasecmp(*Str(qn), "EXPERIMENTAL") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_EXPERIMENTAL;
            else
                flags &= ~PEGASUS_FLAG_EXPERIMENTAL;
        }
        else if (System::strcasecmp(*Str(qn), "GAUGE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_GAUGE;
            else
                flags &= ~PEGASUS_FLAG_GAUGE;
        }
        else if (System::strcasecmp(*Str(qn), "IFDELETED") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_IFDELETED;
            else
                flags &= ~PEGASUS_FLAG_IFDELETED;
        }
        else if (System::strcasecmp(*Str(qn), "INVISIBLE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_INVISIBLE;
            else
                flags &= ~PEGASUS_FLAG_INVISIBLE;
        }
        else if (System::strcasecmp(*Str(qn), "LARGE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_LARGE;
            else
                flags &= ~PEGASUS_FLAG_LARGE;
        }
        else if (System::strcasecmp(*Str(qn), "OCTETSTRING") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_OCTETSTRING;
            else
                flags &= ~PEGASUS_FLAG_OCTETSTRING;
        }
        else if (System::strcasecmp(*Str(qn), "READ") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_READ;
            else
                flags &= ~PEGASUS_FLAG_READ;
        }
        else if (System::strcasecmp(*Str(qn), "REQUIRED") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_REQUIRED;
            else
                flags &= ~PEGASUS_FLAG_REQUIRED;
        }
        else if (System::strcasecmp(*Str(qn), "STATIC") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_STATIC;
            else
                flags &= ~PEGASUS_FLAG_STATIC;
        }
        else if (System::strcasecmp(*Str(qn), "TERMINAL") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_TERMINAL;
            else
                flags &= ~PEGASUS_FLAG_TERMINAL;
        }
        else if (System::strcasecmp(*Str(qn), "WEAK") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_WEAK;
            else
                flags &= ~PEGASUS_FLAG_WEAK;
        }
        else if (System::strcasecmp(*Str(qn), "WRITE") == 0)
        {
            if (x)
                flags |= PEGASUS_FLAG_WRITE;
            else
                flags &= ~PEGASUS_FLAG_WRITE;
        }
        else
        {
            // ATTN: Composition qualifier not handled (no more room in mask).
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

static void _writeBoolean(FILE* os, Boolean x)
{
    fprintf(os, "\\%03o", (int)x);
}

static void _writeUint8(FILE* os, Uint8 x)
{
    fprintf(os, "\\%03o", (int)x);
}

static void _writeSint8(FILE* os, Sint8 x)
{
    _writeUint8(os, Uint8(x));
}

static void _writeUint16(FILE* os, Uint16 x)
{
    Uint16 x0 = (x >> 8) & 0x00FF;
    Uint16 x1 = (x >> 0) & 0x00FF;
    fprintf(os, "\\%03o", (int)x0);
    fprintf(os, "\\%03o", (int)x1);
}

static void _writeSint16(FILE* os, Sint16 x)
{
    _writeUint16(os, Uint16(x));
}

static void _writeUint32(FILE* os, Uint32 x)
{
    Uint32 x0 = (x >> 24) & 0x000000FF;
    Uint32 x1 = (x >> 16) & 0x000000FF;
    Uint32 x2 = (x >>  8) & 0x000000FF;
    Uint32 x3 = (x >>  0) & 0x000000FF;
    fprintf(os, "\\%03o", (int)x0);
    fprintf(os, "\\%03o", (int)x1);
    fprintf(os, "\\%03o", (int)x2);
    fprintf(os, "\\%03o", (int)x3);
}

static void _writeSint32(FILE* os, Sint32 x)
{
    _writeUint32(os, Uint32(x));
}

static void _writeUint64(FILE* os, Uint64 x)
{
    Uint64 x0 = (x >> 56) & 0x000000FF;
    Uint64 x1 = (x >> 48) & 0x000000FF;
    Uint64 x2 = (x >> 40) & 0x000000FF;
    Uint64 x3 = (x >> 32) & 0x000000FF;
    Uint64 x4 = (x >> 24) & 0x000000FF;
    Uint64 x5 = (x >> 16) & 0x000000FF;
    Uint64 x6 = (x >>  8) & 0x000000FF;
    Uint64 x7 = (x >>  0) & 0x000000FF;
    fprintf(os, "\\%03o", (int)x0);
    fprintf(os, "\\%03o", (int)x1);
    fprintf(os, "\\%03o", (int)x2);
    fprintf(os, "\\%03o", (int)x3);
    fprintf(os, "\\%03o", (int)x4);
    fprintf(os, "\\%03o", (int)x5);
    fprintf(os, "\\%03o", (int)x6);
    fprintf(os, "\\%03o", (int)x7);
}

static void _writeSint64(FILE* os, Sint64 x)
{
    _writeUint64(os, Uint64(x));
}

static void _writeReal32(FILE* os, Real32 x)
{
    _writeUint32(os, *((Uint32*)&x));
}

static void _writeReal64(FILE* os, Real64 x)
{
    _writeUint64(os, *((Uint64*)&x));
}

static void _writeChar16(FILE* os, const Char16& x)
{
    _writeUint16(os, x);
}

static void _writeString(FILE* os, const char* s)
{
    size_t n = strlen(s);

    for (size_t i = 0; i < n; i++)
    {
        char c = s[i];

        if (isprint(c) && c != '"')
            fprintf(os, "%c", c);
        else
            fprintf(os, "\\%03o", c);
    }
}

static void _writeString(FILE* os, const String& x)
{
    _writeString(os, *Str(x));
}

static void _writeDateTime(FILE* os, const CIMDateTime& x)
{
    _writeString(os, x.toString());
}

static int _writeValue(FILE* os, const CIMValue& cv, bool quote)
{
    if (cv.isNull())
    {
        fprintf(os, "0");
        return 0;
    }

    if (quote)
        fputc('"', os);

    if (cv.isArray())
    {
        switch (cv.getType())
        {
            case CIMTYPE_BOOLEAN:
            {
                Array<Boolean> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeBoolean(os, x[i]);
                break;
            }

            case CIMTYPE_UINT8:
            {
                Array<Uint8> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeUint8(os, x[i]);
                break;
            }

            case CIMTYPE_SINT8:
            {
                Array<Sint8> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeSint8(os, x[i]);
                break;
            }

            case CIMTYPE_UINT16:
            {
                Array<Uint16> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeUint16(os, x[i]);
                break;
            }

            case CIMTYPE_SINT16:
            {
                Array<Sint16> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeSint16(os, x[i]);
                break;
            }

            case CIMTYPE_UINT32:
            {
                Array<Uint32> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeUint32(os, x[i]);
                break;
            }

            case CIMTYPE_SINT32:
            {
                Array<Sint32> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeSint32(os, x[i]);
                break;
            }

            case CIMTYPE_UINT64:
            {
                Array<Uint64> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeUint64(os, x[i]);
                break;
            }

            case CIMTYPE_SINT64:
            {
                Array<Sint64> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeSint64(os, x[i]);
                break;
            }

            case CIMTYPE_REAL32:
            {
                Array<Real32> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeReal32(os, x[i]);
                break;
            }

            case CIMTYPE_REAL64:
            {
                Array<Real64> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeReal64(os, x[i]);
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Array<Char16> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeChar16(os, x[i]);
                break;
            }

            case CIMTYPE_STRING:
            {
                Array<String> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                {
                    _writeString(os, x[i]);
                    _writeUint8(os, 0);
                }
                break;
            }

            case CIMTYPE_DATETIME:
            {
                Array<CIMDateTime> x;
                cv.get(x);

                _writeUint16(os, x.size());

                for (Uint32 i = 0; i < x.size(); i++)
                    _writeDateTime(os, x[i]);
                break;
            }

            default:
                return -1;
        }
    }
    else
    {
        switch (cv.getType())
        {
            case CIMTYPE_BOOLEAN:
            {
                Boolean x;
                cv.get(x);
                _writeBoolean(os, x);
                break;
            }

            case CIMTYPE_UINT8:
            {
                Uint8 x;
                cv.get(x);
                _writeUint8(os, x);
                break;
            }

            case CIMTYPE_SINT8:
            {
                Sint8 x;
                cv.get(x);
                _writeSint8(os, x);
                break;
            }

            case CIMTYPE_UINT16:
            {
                Uint16 x;
                cv.get(x);
                _writeUint16(os, x);
                break;
            }

            case CIMTYPE_SINT16:
            {
                Sint16 x;
                cv.get(x);
                _writeSint16(os, x);
                break;
            }

            case CIMTYPE_UINT32:
            {
                Uint32 x;
                cv.get(x);
                _writeUint32(os, x);
                break;
            }

            case CIMTYPE_SINT32:
            {
                Sint32 x;
                cv.get(x);
                _writeSint32(os, x);
                break;
            }

            case CIMTYPE_UINT64:
            {
                Uint64 x;
                cv.get(x);
                _writeUint64(os, x);
                break;
            }

            case CIMTYPE_SINT64:
            {
                Sint64 x;
                cv.get(x);
                _writeSint64(os, x);
                break;
            }

            case CIMTYPE_REAL32:
            {
                Real32 x;
                cv.get(x);
                _writeReal32(os, x);
                break;
            }

            case CIMTYPE_REAL64:
            {
                Real64 x;
                cv.get(x);
                _writeReal64(os, x);
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Char16 x;
                cv.get(x);
                _writeChar16(os, x);
                break;
            }

            case CIMTYPE_STRING:
            {
                String x;
                cv.get(x);
                _writeString(os, x);
                break;
            }

            case CIMTYPE_DATETIME:
            {
                CIMDateTime x;
                cv.get(x);
                _writeDateTime(os, x);
                break;
            }

            default:
                return -1;
        }
    }

    if (quote)
        fputc('"', os);

    return 0;
}

//==============================================================================
//
// cimmofSourceConsumer
//
//==============================================================================

cimmofSourceConsumer::cimmofSourceConsumer(bool discard) : 
    _discard(discard), _os(0)
{
}

cimmofSourceConsumer::~cimmofSourceConsumer()
{
}

void cimmofSourceConsumer::addClass(
    const CIMNamespaceName& nameSpace,
    CIMClass& cimClass)
{
    if (_findClass(cimClass.getClassName()) != PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_ALREADY_EXISTS, "class already defined: %s:%s", 
            *Str(nameSpace), *Str(cimClass.getClassName()));
    }

    _classes.append(cimClass);
}

void cimmofSourceConsumer::addQualifier(
    const CIMNamespaceName& nameSpace,
    CIMQualifierDecl& cimQualifierDecl)
{
    if (_findQualifier(cimQualifierDecl.getName()) != PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_ALREADY_EXISTS, "qualifier already defined: %s:%s", 
            *Str(nameSpace), *Str(cimQualifierDecl.getName()));
    }

    _qualifiers.append(cimQualifierDecl);
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
    Uint32 pos = _findQualifier(qualifierName);

    if (pos == PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_NOT_FOUND,
            "undefined qualifier: %s:%s", *Str(nameSpace), *Str(qualifierName));
    }

    return _qualifiers[pos];
}

CIMClass cimmofSourceConsumer::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    Uint32 pos = _findClass(className);

    if (pos == PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_NOT_FOUND, 
            "undefined class: %s:%s", *Str(nameSpace), *Str(className));
    }

    return _classes[pos];
}

void cimmofSourceConsumer::modifyClass(
    const CIMNamespaceName& nameSpace,
    CIMClass& cimClass)
{
    Uint32 pos = _findClass(cimClass.getClassName());

    if (pos == PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_NOT_FOUND, "undefined class: %s:%s", 
            *Str(nameSpace), *Str(cimClass.getClassName()));
    }

    _classes[pos] = cimClass;
}

void cimmofSourceConsumer::createNameSpace(
    const CIMNamespaceName& nameSpace)
{
    if (_nameSpace == nameSpace)
    {
        _throw(CIM_ERR_ALREADY_EXISTS, "namespace already exists: %s", 
            *Str(nameSpace));
    }

    if (!_nameSpace.isNull())
    {
        _throw(CIM_ERR_FAILED, "cannot create more than one namespace");
    }

    _nameSpace = nameSpace;
}

void cimmofSourceConsumer::start()
{
}

void cimmofSourceConsumer::finish()
{
    String ns = _makeIdent(_nameSpace.getString());

    // Write header file:

    _writeHeaderFile(ns);

    // Open source file:

    String path = ns + "_namespace.cpp";
    _os = fopen(*Str(path), "wb");

    if (!_os)
    {
        fprintf(stderr, "cimmofl: failed to open \"%s\" for write\n", 
            *Str(path));
        exit(1);
    }

    // Write prologue:

    _writeSourcePrologue();

    // Write namespace:

    _writeNameSpace(_nameSpace);

    // Write epilogue:

    _writeSourceEpilogue();

    // Close file:

    fclose(_os);

    // Write messages:

    printf("Created %s_namespace.h\n", *Str(ns));
    printf("Created %s_namespace.cpp\n", *Str(ns));
    printf("\n");
}

Uint32 cimmofSourceConsumer::_findClass(const CIMName& className) const
{
    for (Uint32 i = 0; i < _classes.size(); i++)
    {
        if (_classes[i].getClassName() == className)
            return i;
    }

    // Not found!
    return PEG_NOT_FOUND;
}

Uint32 cimmofSourceConsumer::_findQualifier(const CIMName& qualifierName) const
{
    for (Uint32 i = 0; i < _qualifiers.size(); i++)
    {
        if (_qualifiers[i].getName() == qualifierName)
            return i;
    }

    // Not found!
    return PEG_NOT_FOUND;
}

void cimmofSourceConsumer::_writeSourcePrologue()
{
    String ns = _makeIdent(_nameSpace.getString());
    String path = ns + "_namespace.h";

    _outn("#include \"%s\"", *Str(path));
    _nl();
    _outn("/*NOCHKSRC*/");
    _nl();
    _outn("PEGASUS_NAMESPACE_BEGIN");
    _nl();
}

void cimmofSourceConsumer::_writeSourceEpilogue()
{
    _outn("PEGASUS_NAMESPACE_END");
}

void cimmofSourceConsumer::_writeQualifier(
    const Array<CIMQualifierDecl>& qualifierDecls,
    const CIMConstQualifier& cq)
{
    CIMName qn = cq.getName();
    CIMType qt = cq.getType();
    CIMValue qv = cq.getValue();

    Uint32 pos = _findQualifier(qn);

    if (pos == PEG_NOT_FOUND)
        _throw(CIM_ERR_FAILED, "undefined qualifier: %s", *Str(qn));

    // Write the qualifier string literal:

    _outn("    /* %s */", *Str(qn));
    _out("    \"");
    _writeUint8(_os, pos);
    _writeValue(_os, qv, false);
    _outn("\",");
}

void cimmofSourceConsumer::_writeQualifierDecl(const CIMConstQualifierDecl& cq)
{
    CIMName qn = cq.getName();
    CIMType qt = cq.getType();
    const CIMValue& cv = cq.getValue();

    // Write value definition (if any).

    String path = "_" + qn.getString() + "_qualifier_decl";

    // Write SourceQualifierDecl header:

    _outn("static SourceQualifierDecl");
    _outn("%s =", *Str(path));
    _outn("{");

    // SourceQualifierDecl.name:

    _outn("    /* name */");
    _outn("    \"%s\",", *Str(qn));

    // SourceQualifierDecl.type:

    _outn("    /* type */");
    _outn("    %s,", _typeNames[qt]);

    // SourceQualifierDecl.subscript:

    _outn("    /* subscript */");

    if (cq.isArray())
    {
        Uint32 n = cq.getArraySize();
        _outn("    %u,", n);
    }
    else
    {
        _outn("    -1,");
    }

    // SourceQualifierDecl.scope:
    {
        _outn("    /* scope */");

        CIMScope scope = cq.getScope();
        Array<String> scopes;

        if (scope.hasScope(CIMScope::ANY))
            scopes.append("PEGASUS_SCOPE_ANY");
        else
        {
            if (scope.hasScope(CIMScope::CLASS))
                scopes.append("PEGASUS_SCOPE_CLASS");
            if (scope.hasScope(CIMScope::ASSOCIATION))
                scopes.append("PEGASUS_SCOPE_ASSOCIATION");
            if (scope.hasScope(CIMScope::INDICATION))
                scopes.append("PEGASUS_SCOPE_INDICATION");
            if (scope.hasScope(CIMScope::PROPERTY))
                scopes.append("PEGASUS_SCOPE_PROPERTY");
            if (scope.hasScope(CIMScope::REFERENCE))
                scopes.append("PEGASUS_SCOPE_REFERENCE");
            if (scope.hasScope(CIMScope::METHOD))
                scopes.append("PEGASUS_SCOPE_METHOD");
            if (scope.hasScope(CIMScope::PARAMETER))
                scopes.append("PEGASUS_SCOPE_PARAMETER");
        }

        _out("    ");

        for (Uint32 i = 0; i < scopes.size(); i++)
        {
            _out("%s", *Str(scopes[i]));

            if (i + 1 != scopes.size())
                _out("|");
        }

        _outn(",");
    }

    // SourceQualifierDecl.flavor:
    {
        _outn("    /* flavor */");

        CIMFlavor flavor = cq.getFlavor();
        Array<String> flavors;

        if (flavor.hasFlavor(CIMFlavor::OVERRIDABLE))
            flavors.append("PEGASUS_FLAVOR_OVERRIDABLE");
        if (flavor.hasFlavor(CIMFlavor::TOSUBCLASS))
            flavors.append("PEGASUS_FLAVOR_TOSUBCLASS");
        if (flavor.hasFlavor(CIMFlavor::TOINSTANCE))
            flavors.append("PEGASUS_FLAVOR_TOINSTANCE");
        if (flavor.hasFlavor(CIMFlavor::TRANSLATABLE))
            flavors.append("PEGASUS_FLAVOR_TRANSLATABLE");
        if (flavor.hasFlavor(CIMFlavor::DISABLEOVERRIDE))
            flavors.append("PEGASUS_FLAVOR_DISABLEOVERRIDE");
        if (flavor.hasFlavor(CIMFlavor::RESTRICTED))
            flavors.append("PEGASUS_FLAVOR_RESTRICTED");

        _out("    ");

        for (Uint32 i = 0; i < flavors.size(); i++)
        {
            _out("%s", *Str(flavors[i]));

            if (i + 1 != flavors.size())
                _out("|");
        }

        _outn(",");
    }

    // SourceQualifierDecl.value:

    _outn("    /* value */");
    _out("    ");
    _writeValue(_os, cv, true);
    _outn(",");

    _outn("};");
    _nl();
}

template<class C>
Array<CIMConstQualifier> _Qualifiers(const C& c)
{
    Array<CIMConstQualifier> tmp;

    for (Uint32 i = 0; i < c.getQualifierCount(); i++)
        tmp.append(c.getQualifier(i));

    return tmp;
}

void cimmofSourceConsumer::_writeQualifierArray(
    const String& root,
    const Array<CIMConstQualifier>& qualifiers)
{
    _outn("static const char*");
    _outn("%s_qualifiers[] =", *Str(root));
    _outn("{");

    for (Uint32 i = 0; i < qualifiers.size(); i++)
    {
        CIMConstQualifier cq = qualifiers[i];
        CIMName qn = cq.getName();
        CIMType qt = cq.getType();

        if (_discard && qn == "Description")
            continue;

        if (qt == CIMTYPE_BOOLEAN && !cq.isArray())
            continue;

        _writeQualifier(_qualifiers, cq);
    }

    // Write terminator:
    _outn("    0,");

    _outn("};");
    _nl();
}

void cimmofSourceConsumer::_writeProperty(
    const CIMNamespaceName& nameSpace,
    const CIMName& cn,
    const CIMConstProperty& cp)
{
    String ns = _makeIdent(nameSpace.getString());
    CIMName pn = cp.getName();
    CIMType ct = cp.getType();
    CIMValue cv = cp.getValue();

    String path = "_" + cn.getString() + "_" + pn.getString();

    // Write qualifiers:

    _writeQualifierArray(path, _Qualifiers(cp));

    // Header:

    if (ct == CIMTYPE_REFERENCE)
        _outn("static SourceReference");
    else
        _outn("static SourceProperty");

    _outn("%s =", *Str(path));
    _outn("{");

    // SourceProperty.flags:

    _outn("    /* flags */");

    if (ct == CIMTYPE_REFERENCE)
        _out("    PEGASUS_FLAG_PROPERTY");
    else
        _out("    PEGASUS_FLAG_REFERENCE");

    _writeFlags(_os, cp, true, false);
    fprintf(_os, ",\n");

    // SourceProperty.name:

    _outn("    /* name */");
    _outn("    \"%s\",", *Str(pn));

    // SourceProperty.qualifiers:

    _outn("    /* qualifiers */");
    _outn("    %s_qualifiers,", *Str(path));

    // SourceProperty.type:

    if (ct != CIMTYPE_REFERENCE)
    {
        _outn("    /* type */");
        _outn("    %s,", _typeNames[ct]);
    }

    // SourceProperty.subscript:

    _outn("    /* subscript */");

    if (cp.isArray())
    {
        Uint32 n = cp.getArraySize();
        _outn("    %u,", n);
    }
    else
    {
        _outn("    -1,");
    }

    // SourceReference.ref:

    if (ct == CIMTYPE_REFERENCE)
    {
        const CIMName& rcn = cp.getReferenceClassName();
        _outn("    /* refId */");
        _outn("    &__%s_%s,", *Str(ns), *Str(rcn));
    }

    // SourceQualifierDecl.value:

    if (ct != CIMTYPE_REFERENCE)
    {
        _outn("    /* value */");
        _out("    ");
        _writeValue(_os, cv, true);
        _outn(",");
    }

    _outn("};");
    _nl();
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

    String path = 
        "_" + cn.getString() + "_" + mn.getString() + "_" + pn.getString();

    _writeQualifierArray(path, _Qualifiers(cp));

    if (ct == CIMTYPE_REFERENCE)
        _outn("static SourceReference");
    else
        _outn("static SourceProperty");

    _outn("%s =", *Str(path));
    _outn("{");

    // SourceProperty.flags:

    _outn("    /* flags */");

    if (ct == CIMTYPE_REFERENCE)
        _out("    PEGASUS_FLAG_PROPERTY");
    else
        _out("    PEGASUS_FLAG_REFERENCE");

    _writeFlags(_os, cp, false, true);
    fprintf(_os, ",\n");

    // SourceProperty.name:

    _outn("    /* name */");
    _outn("    \"%s\",", *Str(pn));

    // SourceProperty.qualifiers:

    _outn("    /* qualifiers */");
    _outn("    %s_qualifiers,", *Str(path));

    // SourceProperty.type:

    if (ct != CIMTYPE_REFERENCE)
    {
        _outn("    /* type */");
        _outn("    %s,", _typeNames[ct]);
    }

    // SourceProperty.subscript:

    _outn("    /* subscript */");

    if (cp.isArray())
    {
        Uint32 n = cp.getArraySize();
        _outn("    %u,", n);
    }
    else
    {
        _outn("    -1,");
    }

    // SourceProperty.ref:

    if (ct == CIMTYPE_REFERENCE)
    {
        const CIMName& rcn = cp.getReferenceClassName();
        _outn("    /* ref */");
        _outn("    &__%s_%s,", *Str(ns), *Str(rcn));
    }

    // SourceQualifierDecl.value:

    if (ct != CIMTYPE_REFERENCE)
    {
        _outn("    /* value */");
        _outn("    0,");
    }

    _outn("};");
    _nl();
}

void cimmofSourceConsumer::_writeMethod(
    const CIMNamespaceName& nameSpace,
    const CIMName& cn,
    const CIMConstMethod& cm)
{
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

    _outn("static SourceFeature*");
    _outn("_%s_%s_parameters[] =", *Str(cn), *Str(mn));
    _outn("{");

    for (Uint32 i = 0; i < parameterNames.size(); i++)
    {
        const CIMName& pn = parameterNames[i];
        _outn("    (SourceFeature*)&_%s_%s_%s,", *Str(cn), *Str(mn), *Str(pn));
    }

    _outn("    0,");
    _outn("};");
    _nl();

    // Method header:

    String path = "_" + cn.getString() + "_" + mn.getString();

    _writeQualifierArray(path, _Qualifiers(cm));

    _outn("static SourceMethod");
    _outn("%s =", *Str(path));
    _outn("{");

    // SourceMethod.flags:

    _outn("    /* flags */");
    _out("    PEGASUS_FLAG_METHOD");
    _writeFlags(_os, cm, false, false);
    fprintf(_os, ",\n");

    // SourceMethod.name:

    _outn("    /* name */");
    _outn("    \"%s\",", *Str(cn));

    // SourceMethod.qualifiers:

    _outn("    /* qualifiers */");
    _outn("    %s_qualifiers,", *Str(path));

    // SourceProperty.type:

    _outn("    /* type */");
    _outn("    %s,", _typeNames[cm.getType()]);

    // SourceMethod.parameters:

    _outn("    /* parameters */");
    _outn("    _%s_%s_parameters,", *Str(cn), *Str(mn));

    // Method footer:

    _outn("};");
    _nl();
}

void cimmofSourceConsumer::_writeClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& cc)
{
    String ns = _makeIdent(nameSpace.getString());
    CIMName cn = cc.getClassName();

    // Write comment:

    _box(_os, "Class: %s", *Str(cn));
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

    _outn("static SourceFeature*");
    _outn("_%s_features[] =", *Str(cn));
    _outn("{");

    for (Uint32 i = 0; i < featureNames.size(); i++)
    {
        const CIMName& fn = featureNames[i];
        _outn("    (SourceFeature*)&_%s_%s,", *Str(cn), *Str(fn));
    }

    _outn("    0,");
    _outn("};");
    _nl();

    // Class header:

    String path = "__" + ns + "_" + cn.getString();

    _writeQualifierArray(path, _Qualifiers(cc));

    _outn("SourceClass");
    _outn("%s =", *Str(path));
    _outn("{");

    // SourceClass.flags:

    _outn("    /* flags */");

    if (_testBooleanQualifier(cc, "Association"))
        _out("    PEGASUS_FLAG_ASSOCIATION");
    else if (_testBooleanQualifier(cc, "Indication"))
        _out("    PEGASUS_FLAG_INDICATION");
    else
        _out("    PEGASUS_FLAG_CLASS");

    _writeFlags(_os, cc, false, false);
    fprintf(_os, ",\n");

    // SourceClass.name:

    _outn("    /* name */");
    _outn("    \"%s\",", *Str(cn));

    // SourceClass.qualifiers:

    _outn("    /* qualifiers */");
    _outn("    %s_qualifiers,", *Str(path));

    // SourceClass.super:

    const CIMName& scn = cc.getSuperClassName();
    _outn("    /* super */");

    if (scn.isNull())
        _outn("    0,");
    else
        _outn("    &__%s_%s,", *Str(ns), *Str(scn));

    // SourceClass.features:

    _outn("    /* features */");
    _outn("    _%s_features,", *Str(cn));

    // Class footer:

    _outn("};");
    _nl();
}

void cimmofSourceConsumer::_writeNameSpace(const CIMNamespaceName& nameSpace)
{
    String ns = _makeIdent(nameSpace.getString());

    // Write qualifiers:

    _box(_os, "Qualifiers");
    _nl();

    for (Uint32 i = 0; i < _qualifiers.size(); i++)
    {
        _writeQualifierDecl(_qualifiers[i]);
    }

    // Forward declare all classes:

    _box(_os, "Forward class declarations");

    _nl();

    for (Uint32 i = 0; i < _classes.size(); i++)
    {
        CIMName cn = _classes[i].getClassName();
        _outn("extern SourceClass __%s_%s;", *Str(ns), *Str(cn));
    }

    _nl();

    // Write classes:

    for (Uint32 i = 0; i < _classes.size(); i++)
        _writeClass(nameSpace, _classes[i]);

    // Write qualifiers list:

    _box(_os, "Qualifier array");
    _nl();

    _outn("static SourceQualifierDecl*");
    _outn("_qualifiers[] =");
    _outn("{");
    _indent++;

    for (Uint32 i = 0; i < _qualifiers.size(); i++)
    {
        _outn("&_%s_qualifier_decl,", *Str(_qualifiers[i].getName()));
    }

    _outn("0,");

    _indent--;
    _outn("};");
    _nl();

    // Write classes list:

    _box(_os, "Class array");
    _nl();

    _outn("static SourceClass*");
    _outn("_classes[] =");
    _outn("{");
    _indent++;

    for (Uint32 i = 0; i < _classes.size(); i++)
    {
        CIMName cn = _classes[i].getClassName();
        _outn("&__%s_%s,", *Str(ns), *Str(cn));
    }

    _outn("0,");

    _indent--;
    _outn("};");
    _nl();

    // Write SourceNameSpace structure:

    _outn("const SourceNameSpace %s_namespace =", *Str(ns));
    _outn("{");
    _outn("    \"%s\",", *Str(nameSpace));
    _outn("    _qualifiers,");
    _outn("    _classes,");
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
