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
// $Log: WindowsSelector.h,v $
// Revision 1.1  2001/04/08 04:46:11  mike
// Added new selector class for windows
//
//
//END_HISTORY

#ifndef Pegasus_CLASSNAME_h
#define Pegasus_CLASSNAME_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Selector.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

struct WindowsSelectorRep;

/** WindowsSelector class.
*/
class WindowsSelector : public Selector
{
public:

    WindowsSelector();

    virtual ~WindowsSelector();

    virtual Boolean select(Uint32 milliseconds);

    virtual Boolean addHandler(
	Uint32 desc, 
	Uint32 reasons,
	Handler* handler);

    virtual Boolean removeHandler(Handler* handler);

private:

    Uint32 _findEntry(Uint32 desc) const;

    struct Entry
    {
	Uint32 desc;
	Handler* handler;
    };

    Array<Entry> _entries;
    WindowsSelectorRep* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CLASSNAME_h */
