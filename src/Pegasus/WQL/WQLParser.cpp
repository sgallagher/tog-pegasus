
//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
// The Open Group, Tivoli Systems
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <iostream>
#include "WQLParser.h"

PEGASUS_USING_STD;

extern int WQL_parse();

PEGASUS_NAMESPACE_BEGIN

static Array<Sint8> _text;
static int _offset;

void WQLParser::parse(const Array<Sint8>& text)
{
    // ATTN: raise error here:

    if (text.size() == 0 || text[text.size() - 1] != '\0')
	return;

    _text = text;
    _offset = 0;
    WQL_parse();
}

PEGASUS_NAMESPACE_END

PEGASUS_USING_PEGASUS;

int WQLInput(char* buffer, int& numRead, int numRequested)
{
    //
    // Be sure to account for the null terminator (the size of the text will
    // be one or more; this is fixed checked beforehand by WQLParser::parse()).
    //

    int remaining = _text.size() - _offset - 1;

    if (remaining == 0)
    {
	numRead = 0;
	return 0;
    }

    if (remaining < numRequested)
	numRequested = remaining;

    memcpy(buffer, _text.getData() + _offset, numRequested);
    _offset += numRequested;
    numRead = numRequested;

    return numRead;
}
