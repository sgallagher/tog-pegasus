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
#include "WQLParserState.h"

PEGASUS_USING_STD;

extern int WQL_parse();

PEGASUS_NAMESPACE_BEGIN

WQLParserState* globalParserState = 0;

void WQLParser::parse(
    const Array<Sint8>& text,
    WQLSelectStatement& statement)
{
    // ATTN: raise error here:

    if (text.size() == 0 || text[text.size() - 1] != '\0')
	return;

    globalParserState = new WQLParserState;
    globalParserState->error = false;
    globalParserState->text = text;
    globalParserState->offset = 0;
    globalParserState->statement = &statement;

    WQL_parse();

    delete globalParserState;
}

PEGASUS_NAMESPACE_END

PEGASUS_USING_PEGASUS;

int WQLInput(char* buffer, int& numRead, int numRequested)
{
    //
    // Be sure to account for the null terminator (the size of the text will
    // be one or more; this is fixed checked beforehand by WQLParser::parse()).
    //

    int remaining = 
	globalParserState->text.size() - globalParserState->offset - 1;

    if (remaining == 0)
    {
	numRead = 0;
	return 0;
    }

    if (remaining < numRequested)
	numRequested = remaining;

    memcpy(buffer, 
	globalParserState->text.getData() + globalParserState->offset, 
	numRequested);

    globalParserState->offset += numRequested;
    numRead = numRequested;

    return numRead;
}
