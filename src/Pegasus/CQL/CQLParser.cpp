//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Query/QueryCommon/QueryException.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/IPC.h>
#include <iostream>
#include "CQLParser.h"
#include "CQLParserState.h"

PEGASUS_USING_STD;

extern int CQL_parse();
extern void CQL_restart (FILE *input_file);

PEGASUS_NAMESPACE_BEGIN

CQLParserState* globalParserState = 0;
static Mutex CQL_mutex;

void CQLParser::parse(
    const char* text,
    CQLSelectStatement& statement)
{
    PEG_METHOD_ENTER(TRC_CQL,"CQLParser::parse");
	 
	 AutoMutex mtx(CQL_mutex);
	 
    if (!text)
    {
        PEG_METHOD_EXIT();
	throw NullPointer();
    }

    statement.clear();
    CQL_restart (0);

    globalParserState = new CQLParserState;
    globalParserState->error = false;
    globalParserState->text = text;
    globalParserState->textSize = strlen(text) + 1;
    globalParserState->offset = 0;
    globalParserState->currentTokenPos = 0;
    globalParserState->tokenCount = 0;
    globalParserState->currentRule = String::EMPTY;
    globalParserState->statement = &statement;

    CQL_parse();

    if (globalParserState->error)
    {
	String errorMessage = globalParserState->errorMessage;
	cleanup();
	Uint32 position = globalParserState->currentTokenPos;
	Uint32 token = globalParserState->tokenCount;
	String rule = globalParserState->currentRule;
	delete globalParserState;
        PEG_METHOD_EXIT();
	throw CQLSyntaxErrorException(errorMessage,token,position,rule);
    }

    cleanup();
    delete globalParserState;
    PEG_METHOD_EXIT();
}

void CQLParser::parse(
    const Array<Sint8>& text,
    CQLSelectStatement& statement)
{
    PEG_METHOD_ENTER(TRC_CQL,"CQLParser::parse");

    if (text.size() == 0 || text[text.size() - 1] != '\0')
    {
        PEG_METHOD_EXIT();
	throw MissingNullTerminator();
    }

    parse(text.getData(), statement);
    PEG_METHOD_EXIT();
}

void CQLParser::parse(
    const String& text,
    CQLSelectStatement& statement)
{
    PEG_METHOD_ENTER(TRC_CQL,"CQLParser::parse");

    parse(text.getCString(), statement);

    PEG_METHOD_EXIT();
}

void CQLParser::cleanup()
{
    PEG_METHOD_ENTER(TRC_CQL,"CQLParser::cleanup");


    Array<char*>& arr = globalParserState->outstandingStrings;

    for (Uint32 i = 0, n = arr.size(); i < n; i++)
	delete [] arr[i];

    arr.clear();

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END

PEGASUS_USING_PEGASUS;

int CQL_error(const char* errorMessage)
{
    PEG_METHOD_ENTER(TRC_CQL,"CQL_error");
    globalParserState->error = true;
    globalParserState->errorMessage = errorMessage;

    //
    //  flex does not automatically flush the input buffer in case of error
    //
    CQL_restart (0);

    PEG_METHOD_EXIT();
    return -1;
}

int CQLInput(char* buffer, int& numRead, int numRequested)
{
    PEG_METHOD_ENTER(TRC_CQL,"CQLInput");
    //
    // Be sure to account for the null terminator (the size of the text will
    // be one or more; this is fixed checked beforehand by CQLParser::parse()).
    //
    int remaining = 
	globalParserState->textSize - globalParserState->offset - 1;

    if (remaining == 0)
    {
	numRead = 0;
        PEG_METHOD_EXIT();
	return 0;
    }

    if (remaining < numRequested)
	numRequested = remaining;

    memcpy(buffer, 
	globalParserState->text + globalParserState->offset, 
	numRequested);

    globalParserState->offset += numRequested;
    numRead = numRequested;


    PEG_METHOD_EXIT();
    return numRead;
}
