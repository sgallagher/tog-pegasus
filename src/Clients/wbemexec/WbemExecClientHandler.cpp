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

#include <Pegasus/Common/TCPChannel.h>
#include <Pegasus/Common/Selector.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Protocol/Handler.h>
#include "WbemExecClientHandler.h"

PEGASUS_NAMESPACE_BEGIN

/**
  
    Constructs a WbemExecClientHandler and initializes instance variables.
  
 */
WbemExecClientHandler::WbemExecClientHandler (Selector* selector, 
                                              ostream& os,
                                              Boolean debugOutput) 
    : _blocked (false), _selector (selector), _os (os), 
      _debugOutput (debugOutput)
{
}


/**
  
    Handles a message containing a response from the CIM Server.  The response
    is printed to the output stream.  This function is called by select.
  
 */
int WbemExecClientHandler::handleMessage ()
{
    Handler::handleMessage ();

    printContent ();

    _blocked = false;

    return 0;
}


/**
  
    Waits for a response.  If no response is received and the timeout expires,
    returns false.  Otherwise, returns true.
  
 */
Boolean WbemExecClientHandler::waitForResponse (Uint32 timeOutMilliseconds)
{
    _blocked = true;
    long rem = long (timeOutMilliseconds);

    while (_blocked)
    {
	TimeValue start = TimeValue::getCurrentTime ();

	TimeValue tv;
	tv.fromMilliseconds (rem);

	_selector->select (tv.toMilliseconds ());

	TimeValue stop = TimeValue::getCurrentTime ();

	long diff = stop.toMilliseconds () - start.toMilliseconds ();

	if (diff >= rem)
	    break;

	rem -= diff;
    }

    Boolean gotResponse = !_blocked;
    _blocked = false;
    return gotResponse;
}

/**
  
    Prints the response to the specified output stream.
  
 */

void WbemExecClientHandler::printContent ()
{
    //
    //  Get HTTP header
    //
    const char* message = _message.getData ();

    if (_debugOutput)
    {
        //
        //  Print HTTP header to the ostream
        //
        for (Uint32 i = 0; i < _lines.size (); i++)
        {
            _os << &message [_lines [i]] << "\r\n";
        }

        _os << "\r\n";
    }

    //
    //  Print XML response to the ostream
    //
    const char* content = _message.getData () + _contentOffset;
    ((Array <Sint8>&) _message).append ('\0');
    XmlWriter::indentedPrint (_os, content, 0);
    ((Array <Sint8>&) _message).remove (_message.size () - 1);
}

PEGASUS_NAMESPACE_END
