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
// Author:
//
// $Log: DirWindows.cpp,v $
// Revision 1.2  2001/04/11 19:53:22  mike
// More porting
//
// Revision 1.1  2001/04/11 00:34:03  mike
// more porting
//
// Revision 1.1  2001/02/11 05:42:33  mike
// new
//
// Revision 1.1.1.1  2001/01/14 19:50:23  mike
// Pegasus import
//
//
//END_HISTORY

#include "Destroyer.h"
#include "Dir.h"
#include "Exception.h"

PEGASUS_NAMESPACE_BEGIN

#include <io.h>
#include <direct.h>

struct DirRep
{
    long file;
    struct _finddata_t findData;
};

Dir::Dir(const String& path)
{
    ArrayDestroyer<char> p(strcat(path.allocateCString(2), "/*"));
    _rep = new DirRep;
    _rep->file = _findfirst(p.getPointer(), &_rep->findData);

    if (_rep->file == -1)
    {
	_more = false;
	throw CannotOpenDirectory(path);
    }
    else
	_more = true;
}

Dir::~Dir()
{
    if (_rep->file != -1)
	_findclose(_rep->file);

    delete _rep;
}

const char* Dir::getName() const
{
    return _rep->findData.name;
}

void Dir::next()
{
    if (!_more)
	return;

    _more = _findnext(_rep->file, &_rep->findData) == 0;
}

PEGASUS_NAMESPACE_END
