//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies of substantial portions of this software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Logger.h>
#include "BasicAuthenticationHandler.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


BasicAuthenticationHandler::BasicAuthenticationHandler()
{
    // ATTN: Load the local authentication module here
}

BasicAuthenticationHandler::~BasicAuthenticationHandler()
{
    // ATTN: delete the local authentication module here
}

Boolean BasicAuthenticationHandler::authenticate(    
    String authHeader,
    String secretKept)
{
    // ATTN: Add authentication code here
    return (true);
}

String BasicAuthenticationHandler::getAuthResponseHeader(
    String reqHeader,
    String& challenge)
{
    String respHeader = String::EMPTY;

    // actual header "WWW-Authenticate: Basic \"" + realm + "\""
    respHeader.assign("WWW-Authenticate: Basic \"");
    respHeader.append(reqHeader);
    respHeader.append("\"");

    // ATTN: call the appropriate authenticator to obtain the header information.

    return (respHeader);
}

PEGASUS_NAMESPACE_END
