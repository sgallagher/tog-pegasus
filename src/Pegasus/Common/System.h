//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author: Michael E. Brasher
//
// $Log: System.h,v $
// Revision 1.1  2001/04/11 00:23:44  mike
// new files
//
//
//END_HISTORY

#ifndef Pegasus_System_h
#define Pegasus_System_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/** The System class defines wrappers for operating system related calls.
    These are only placed here if they are extremely light. These are
    usually just direct wrappers which map more or less one to one to the
    underlying function.
*/
class PEGASUS_COMMON_LINKAGE System
{
public:

    static void getCurrentTime(Uint32& seconds, Uint32& milliseconds);

    static void sleep(Uint32 seconds);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_System_h */
