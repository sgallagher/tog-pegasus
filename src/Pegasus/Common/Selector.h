//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Selector_h 
#define Pegasus_Selector_h 

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

class SelectorHandler;
struct SelectorRep;

/** The Selector class is used to demultiplex socket descriptor events. 

    The TCPChannel implementations use the Selector class to detect events 
    on socket descriptors. To use the class, one derives from the 
    SelectorHandler class and overrides the handle() method. Then the
    handler is installed into the selector by calling the addHandler() method
    of the Selector class. Here is an example:

    <pre>
	Uint32 sock;
	...
	Selector* selector = new Selector;
	SelectorHandler* handler = new MySelectorHandler;
	selector->addHandler(sock, Selector::READ | Selector::WRITE, handler);
    </pre>

    In this example, an instance of MySelectorHandler is installed to catch
    read and write events on the given socket. The MySelectorHandler::handle()
    method is called when such an event occurs.

    The Selector::select() method must be called to check for events. This
    method sleeps until there is activity on one of the registered descriptors
    and then the associated handle method is called.
*/

class PEGASUS_COMMON_LINKAGE Selector
{
public:

    /** Event types which handlers may be registered to catch.
    */
    enum Reason { READ = 1, WRITE = 2, EXCEPTION = 4 };

    /** Default constructor.
    */
    Selector();

    /** This destruct deletes all handlers which were installed.
    */
    ~Selector();

    /** This waits for the given amount of milliseconds for an event on one
	of the descriptors for which handlers were added. If such an event
	occurs, the correspondintg handle() method is called and the method
	immediately returns (without waiting for the remaining duration
	indicated by the milliseconds argument to elapse. If not such event
	occurs, then the select call will returns after waiting for the
	amount of time indicated by the argument.
    */
    Boolean select(Uint32 milliseconds);

    /** Adds a new handler for the given socket descriptor. The reasons
	are defined by the Reason enumeration above. The handler becomes
	the property of the Selector instance.
    */
    Boolean addHandler(
	Sint32 desc, 
	Uint32 reasons,
	SelectorHandler* handler);

    /** Remove and delete the given handler. 
    */
    Boolean removeHandler(SelectorHandler* handler);

private:

    Uint32 _findEntry(Sint32 desc) const;

    struct Entry
    {
	Sint32 desc;
	SelectorHandler* handler;
    };

    Array<Entry> _entries;
    SelectorRep* _rep;
};

class PEGASUS_COMMON_LINKAGE SelectorHandler
{
public:

    virtual ~SelectorHandler();

    virtual Boolean handle(Sint32 desc, Uint32 reasons) = 0;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Selector_h */
