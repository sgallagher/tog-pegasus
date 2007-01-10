/*
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
//%/////////////////////////////////////////////////////////////////////////////
*/

#include <string.h>
#include "Defines.h"
#include "Macro.h"
#include "Strlcpy.h"
#include "Strlcat.h"
#include "Log.h"
#include <assert.h>

/*
**==============================================================================
**
** Macro
**
**     This structure defines a macro (a name-value pair).
**
**==============================================================================
*/

struct Macro
{
    char* name;
    char* value;
    struct Macro* next;
};

/*
**==============================================================================
**
** _macros
**
**     The list of process macros.
**
**==============================================================================
*/

static struct Macro* _macros = 0;

/*
**==============================================================================
**
** FindMacro()
**
**     Find the value of the named macro. Return pointer to value or NULL if
**     not found.
**
**==============================================================================
*/

const char* FindMacro(const char* name)
{
    const struct Macro* p;

    for (p = _macros; p; p = p->next)
    {
        if (strcmp(p->name, name) == 0)
            return p->value;
    }

    /* Not found. */
    return NULL;
}

/*
**==============================================================================
**
** DefineMacro()
**
**     Add a new macro to the macro list.
**
**==============================================================================
*/

int DefineMacro(const char* name, const char* value)
{
    struct Macro* macro;

    /* Reject if the macro is already defined. */

    if (FindMacro(name) != NULL)
        return -1;

    /* Create new macro. */

    macro = (struct Macro*)malloc(sizeof(struct Macro));
    macro->name = strdup(name);
    macro->value = strdup(value);

    /* Add to front of list. */

    macro->next = _macros;
    _macros = macro;

    return 0;
}

/*
**==============================================================================
**
** ExpandMacros()
**
**     Expand all macros in *input*. Leave result in *output*.
**
**==============================================================================
*/

int ExpandMacros(const char* input, char output[EXECUTOR_BUFFER_SIZE])
{
    char buffer[EXECUTOR_BUFFER_SIZE];
    char* p;
    size_t n = 0;

    /* Make copy of input since we'll need to destroy it. */

    Strlcpy(buffer, input, sizeof(buffer));
    output[0] = '\0';

    /* Traverse buffer, copying characters and expanding macros as we go. */

    for (p = buffer; *p; )
    {
        /* Look for start of macro. */

        if (p[0] == '$' && p[1] == '{')
        {
            char* q;

            /* Set p and q as follows.
             *
             *       p       q
             *       |       |
             *       v       v
             * "...${MY_MACRO}..."
             */

            p += 2;

            /* Find end of macro. */

            for (q = p; *q && *q != '}'; q++)
                ;

            if (*q != '}')
            {
                Log(LL_SEVERE, "ExpandMacros(): corrupt input: %s", input);
                return -1;
            }

            /* Replace '}' with null-terminator. */

            *q = '\0';

            /* Lookup macro and append value. */

            {
                const char* value;
                value = FindMacro(p);

                if (!value)
                {
                    Log(LL_SEVERE, "ExpandMacros(): Undefined macro: %s", p);
                    return -1;
                }

                Strlcat(output, value, EXECUTOR_BUFFER_SIZE);
            }

            p = q + 1;
        }
        else
        {
            /* Append current character to output. */

            char two[2];
            two[0] = *p++;
            two[1] = '\0';

            Strlcat(output, two, EXECUTOR_BUFFER_SIZE);
        }
    }

    return 0;
}
