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
// Author: Mike Brasher
//
// $Log: Dir.h,v $
// Revision 1.1  2001/02/11 05:42:33  mike
// new
//
// Revision 1.1.1.1  2001/01/14 19:50:23  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_Dir_h
#define Pegasus_Dir_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

struct DirRep;

/**
    The Dir class provides a platform independent way of iterating the
    files in a directory.
*/
class PEGASUS_COMMON_LINKAGE Dir
{
public:

    /** Start this iterator class on the given path. If the directory 
	cannot be opened, the CannotOpenDirectory exception is thrown.
    */
    Dir(const String& path);

    /// Release any iterator resources.
    ~Dir();

    /// Return true if there are more file names to iterator.
    Boolean more() const { return _more; }

    /// Return the current file name:
    const char* getName() const;

    /// Advance the iterator:
    void next();

private:

    Boolean _more;
    Boolean _isValid;
    DirRep* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Dir_h */
