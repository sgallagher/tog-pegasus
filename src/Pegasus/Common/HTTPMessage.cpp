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

#include <iostream>
#include "HTTPMessage.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static char* _FindSeparator(const char* data, Uint32 size)
{
    const char* p = data;
    const char* end = p + size;

    while (p != end)
    {
	if (*p == '\r')
	{
	    Uint32 n = end - p;

	    if (n >= 2 && p[1] == '\n')
		return (char*)p;
	}
	else if (*p == '\n')
	    return (char*)p;

	p++;
    }

    return 0;
}

HTTPMessage::HTTPMessage(
    const Array<Sint8>& message_, 
    Uint32 returnQueueId_)
    :
    Message(HTTP_MESSAGE), 
    message(message_), 
    returnQueueId(returnQueueId_)
{

}

void HTTPMessage::parse(
    String& startLine,
    Array<HTTPHeader>& headers,
    Sint8*& content,
    Uint32& contentLength) const
{
    startLine.clear();
    headers.clear();
    content = "";
    contentLength = 0;

    char* data = (char*)message.getData();
    Uint32 size = message.size();
    char* line = (char*)data;
    char* sep;
    Boolean firstTime = true;

    while ((sep = _FindSeparator(line, size - (line - data))))
    {
	// Look for double separator which terminates the header?

	if (line == sep)
	{
	    // Establish pointer to content (account for "\n" and "\r\n").

	    content = line + ((*sep == '\r') ? 2 : 1);

	    // Determine length of content:

	    contentLength = message.size() - (content - data);
	    break;
	}

	Uint32 lineLength = sep - line;

	if (firstTime)
	    startLine.assign(line, lineLength);
	else
	{
	    // Find the colon:

	    Sint8* colon = 0;

	    for (Uint32 i = 0; i < lineLength; i++)
	    {
		if (line[i] == ':')
		{
		    colon = &line[i];
		    break;
		}
	    }

	    // This should always be true:

	    if (colon)
	    {
		// Get the name part:

		Sint8* end;

		for (end = colon - 1; end > line && isspace(*end); end--)
		    ;

		end++;

		String name(line, end - line);

		// Get the value part:

		Sint8* start;

		for (start = colon + 1; start < sep && isspace(*start); start++)
		    ;

		String value(start, sep - start);

		headers.append(HTTPHeader(name, value));
	    }
	}

	line = sep + ((*sep == '\r') ? 2 : 1);
	firstTime = false;
    }
}

void HTTPMessage::print(ostream& os) const
{
    Message::print(os);

    String startLine;
    Array<HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;
    parse(startLine, headers, content, contentLength);

    // Print the first line:

    cout << startLine << endl;

    // Print the headers:

    Boolean image = false;

    for (Uint32 i = 0; i < headers.size(); i++)
    {
	cout << headers[i].first << ": " << headers[i].second << endl;

	if (String::equalNoCase(headers[i].first, "content-type"))
	{
	    if (headers[i].second.find("image/") == 0)
		image = true;
	}
    }

    cout << endl;

    // Print the content:

    for (Uint32 i = 0; i < contentLength; i++)
    {
	Sint8 c = content[i];

	if (image)
	{
	    if ((i % 60) == 0)
		cout << endl;

	    Sint8 c = content[i];

	    if (c >= ' ' && c < '~')
		cout << c;
	    else
		cout << '.';
	}
	else
	    cout << c;
    }

    cout << endl;
}

PEGASUS_NAMESPACE_END
