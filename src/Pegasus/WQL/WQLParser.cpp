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
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Destroyer.h>
#include <iostream>
#include "WQLParser.h"
#include "WQLParserState.h"

PEGASUS_USING_STD;

extern int WQL_parse();

PEGASUS_NAMESPACE_BEGIN

WQLParserState* globalParserState = 0;

void WQLParser::parse(
    const char* text,
    WQLSelectStatement& statement)
{
    if (!text)
	throw NullPointer();

    statement.clear();

    globalParserState = new WQLParserState;
    globalParserState->error = false;
    globalParserState->text = text;
    globalParserState->textSize = strlen(text) + 1;
    globalParserState->offset = 0;
    globalParserState->statement = &statement;

    WQL_parse();

    if (globalParserState->error)
    {
	String errorMessage = globalParserState->errorMessage;
	cleanup();
	delete globalParserState;
	throw ParseError(errorMessage);
    }

    cleanup();
    delete globalParserState;
}

void WQLParser::parse(
    const Array<Sint8>& text,
    WQLSelectStatement& statement)
{
    if (text.size() == 0 || text[text.size() - 1] != '\0')
	throw MissingNullTerminator();

    parse(text.getData(), statement);
}

void WQLParser::parse(
    const String& text,
    WQLSelectStatement& statement)
{
    char* tmpText = text.allocateCString();
    ArrayDestroyer<char> destroyer(tmpText);
    parse(tmpText, statement);
}

void WQLParser::cleanup()
{
    Array<char*>& arr = globalParserState->outstandingStrings;

    for (Uint32 i = 0, n = arr.size(); i < n; i++)
	delete [] arr[i];

    arr.clear();
}

PEGASUS_NAMESPACE_END

PEGASUS_USING_PEGASUS;

int WQL_error(char* errorMessage)
{
    globalParserState->error = true;
    globalParserState->errorMessage = errorMessage;
    return -1;
}

int WQLInput(char* buffer, int& numRead, int numRequested)
{
    //
    // Be sure to account for the null terminator (the size of the text will
    // be one or more; this is fixed checked beforehand by WQLParser::parse()).
    //

    int remaining = 
	globalParserState->textSize - globalParserState->offset - 1;

    if (remaining == 0)
    {
	numRead = 0;
	return 0;
    }

    if (remaining < numRequested)
	numRequested = remaining;

    memcpy(buffer, 
	globalParserState->text + globalParserState->offset, 
	numRequested);

    globalParserState->offset += numRequested;
    numRead = numRequested;

    return numRead;
}
