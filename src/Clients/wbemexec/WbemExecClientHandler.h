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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//         (carolann_graves@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WbemExecClientHandler_h
#define Pegasus_WbemExecClientHandler_h

#include <Pegasus/Common/TCPChannel.h>
#include <Pegasus/Common/Selector.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Protocol/Handler.h>

PEGASUS_NAMESPACE_BEGIN

/**
  
    WbemExecClientHandler provides a generic handler for the wbemexec command 
    line client.  WbemExecClientHandler is based on the ClientHandler class 
    (in Client/CIMClient.cpp), but is generic, not CIM operation-specific.
  
    @author  Hewlett-Packard Company
  
 */
class WbemExecClientHandler : public Handler 
{
public:

    WbemExecClientHandler (Selector* selector, 
                           ostream& os, 
                           Boolean debugOutput);

    virtual int handleMessage ();

    Boolean waitForResponse (Uint32 timeOutMilliseconds);

    void printContent ();

private:
    Boolean _blocked;
    Selector* _selector;
    ostream& _os;
    Boolean _debugOutput;
};

PEGASUS_NAMESPACE_END

#endif  /*  Pegasus_WbemExecClientHandler_h */
