//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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

#ifndef Pegasus_Dir_h
#define Pegasus_Dir_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

struct DirRep;

/** The Dir class provides a platform independent way of iterating the
    files in a directory.
*/
class PEGASUS_COMMON_LINKAGE Dir
{
public:

    /** Starts this iterator class on the given path.
	@param String path is the path to the target directory 
	@return
	@exception throws CannotOpenDirectory if invalid directory.

	<pre>
	char* path = "."
	try
	{ 
	   for (Dir dir(path); dir.more(); dir.next())
	   {
	       cout << "name: " << dir.getName() << endl;
	   }
	}
	catch(CannotOpenDirectory&)
	{
	   // Error!
	}
	</pre>
    */
    Dir(const String& path);

    /** Release any iterator resources. */
    ~Dir();

    /** Return true if there are more file names to iterator. */
    Boolean more() const { return _more; }

    /** Returns the current file name. */
    const char* getName() const;

    /** Advance to next file in directory. */
    void next();

private:

    Boolean _more;
    Boolean _isValid;
    DirRep* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Dir_h */
