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

#include <Pegasus/Common/Config.h>
#include "WbemExecClientHandler.h"
#include "WbemExecClientHandlerFactory.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
  
    WbemExecClientHandlerFactory is a factory class for the 
    WbemExecClientHandler class.  It is based on the ClientHandlerFactory class
    (in Client/CIMClient.cpp).
  
    @author  Hewlett-Packard Company
  
 */
WbemExecClientHandlerFactory::WbemExecClientHandlerFactory (Selector* selector,
   ostream& os, Boolean debugOutput)
    : _selector (selector), _os (os), _debugOutput (debugOutput) 
{
}

WbemExecClientHandlerFactory::~WbemExecClientHandlerFactory () 
{
}

ChannelHandler* WbemExecClientHandlerFactory::create () 
{
    return new WbemExecClientHandler (_selector, _os, _debugOutput);
}

PEGASUS_NAMESPACE_END
