//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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

#ifndef _Service_h
#define _Service_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// REVIEW: there are two implementations of service handling for NT (the
// REVIEW: other is part of cimserver).

/** This class is used to represent a service (a program that runs detached
    from a terimal window). It is registered using the run() method which 
    arranges to call the methods of this class at various times.
*/
class PEGASUS_COMMON_LINKAGE ServiceHandler
{
public:

    virtual ~ServiceHandler();

    /** Called by the Service::run() method. The argc and argv arguments give
	the arguments to the program. In Windows this method is called in
	its own thread. Note that this function should generally not return.
	Instead it should just call exit().
    */
    virtual int main(int argc, char** argv) = 0;

    /** Called when the service is paused by the operating system. In Windows
	this is called when the "pause" button is clicked on this service from
	the Service Manager. In Unix this method is called when the process
	receives a 1 signal (kill -1).
    */
    virtual void pause() = 0;

    /** Called when the service is resumed by the operating system. In Windows
	this is called when the "resume" button is clicked on this service from
	the Service Manager. In Unix this method is called when the process
	receives a 1 signal (kill -1) just after the pause() method is called.
    */
    virtual void resume() = 0;

    /** Called when the service is stopped by the operating system. In Windows
	this is called when the "stop" button is clicked on this service from
	the Service Manager. In Unix, this method is called when a process is
	killed.
    */
    virtual void stop() = 0;
};

/** This class encapsulates the operating specific details of running a
    Service (Windows) or Daemon (Unix). The idea is to allow a service to 
    run detached from a terminal device. The sole run() method starts the
    service.
*/
class PEGASUS_COMMON_LINKAGE Service
{
public:

    /** Runs the service. This will ultimately call the main() method of the
	ServiceHandler object. The argc and argv arguments are passed onto
	the main() routine except when the program is being run as a Windows
	service in which case the arguments originate from the Windows
	Service Manager window. This routine will also arrange to call the
	other methods of the ServiceHandler object under certain circumstances
	(see the ServiceHandler class for more details).
    */
    static Boolean run(
	int argc,
	char** argv,
	const char* serviceName, 
	ServiceHandler* serviceHandler,
	Boolean detach);

private:

    // Private constructor to prevent instantiation of this class.

    Service() { }
};

PEGASUS_NAMESPACE_END

#endif /* _Service_h */
