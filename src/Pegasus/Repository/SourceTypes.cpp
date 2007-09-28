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

#include <Pegasus/Common/System.h>
#include "SourceTypes.h"

PEGASUS_NAMESPACE_BEGIN

const char* _flagNames[] =
{
    "Property", /* 0 */
    "Reference", /* 1 */
    "Method", /* 2 */
    "Class", /* 3 */
    "Abstract", /* 4 */
    "Aggregate", /* 5 */
    "Aggregation", /* 6 */
    "Association", /* 7 */
    "Composition", /* 8 */
    "Counter", /* 9 */
    "Delete", /* 10 */
    "DN", /* 11 */
    "EmbeddedObject", /* 12 */
    "Exception", /* 13 */
    "Expensive", /* 14 */
    "Experimental", /* 15 */
    "Gauge", /* 16 */
    "IfDeleted", /* 17 */
    "In", /* 18 */
    "Indication", /* 19 */
    "Invisible", /* 20 */
    "Key", /* 21 */
    "Large", /* 22 */
    "OctetString", /* 23 */
    "Out", /* 24 */
    "Read", /* 25 */
    "Required", /* 26 */
    "Static", /* 27 */
    "Terminal", /* 28 */
    "Weak", /* 29 */
    "Write", /* 30 */
    "EmbeddedInstance", /* 31 */
};

static const size_t _numFlagNames = sizeof(_flagNames) / sizeof(_flagNames[0]);

size_t FlagNameToIndex(const char* name)
{
    for (size_t i = 0; i < _numFlagNames; i++)
    {
        if (System::strcasecmp(_flagNames[i], name) == 0)
            return i;
    }

    // Not found!
    return size_t(-1);
}

PEGASUS_NAMESPACE_END
