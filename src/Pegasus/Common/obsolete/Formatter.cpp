
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include "Formatter.h"
#include "Exception.h"

PEGASUS_NAMESPACE_BEGIN

void ArrIns::insert(Array<Sint8>& out) const
{
    out.append(_arr.getData(), _arr.getSize());
}

void IntIns::insert(Array<Sint8>& out) const
{
    char buffer[32];
    sprintf(buffer, "%d", _x);
    out.append(buffer, strlen(buffer));
}

void StringIns::insert(Array<Sint8>& out) const
{
    CString tmp = _x.getCString();
    out.append(tmp, strlen(tmp));
}

void Formatter::format(
    Array<Sint8>& out,
    const char* format,
    const Inserter& i0,
    const Inserter& i1,
    const Inserter& i2,
    const Inserter& i3,
    const Inserter& i4,
    const Inserter& i5,
    const Inserter& i6,
    const Inserter& i7,
    const Inserter& i8,
    const Inserter& i9)
{
    const Inserter* ins[] =
    {
	&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9
    };

    _format(out, format, ins, sizeof(ins) / sizeof(ins[0]));
}

void Formatter::_format(
    Array<Sint8>& out,
    const char* format,
    const Inserter* ins[],
    Uint32 insSize)
{
    for (const char* p = format; *p; p++)
    {
	if (*p == '$')
	{
	    // Expect '{':

	    if (*++p != '{')
		throw BadFormat();

	    p++;

	    // Expect digits termianted by '}':

	    const char *q = p;

	    for (q = p; *q && *q != '}'; q++)
	    {
		if (!isdigit(*q))
		    throw BadFormat();
	    }

	    if (*q != '}' || p == q)
		throw BadFormat();

	    char buffer[32];
	    *buffer = '\0';
	    strncat(buffer, p, q - p);

	    Uint32 index = atoi(buffer);

	    if (index >= insSize)
		throw BadFormat();

	    ins[index]->insert(out);

	    p = q;

	    continue;
	}

	out.append(*p);
    }
}

PEGASUS_NAMESPACE_END
