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
// $Log: Selector.cpp,v $
// Revision 1.2  2001/04/08 05:06:06  mike
// New Files for Channel Implementation
//
// Revision 1.1  2001/04/08 04:46:11  mike
// Added new selector class for windows
//
//
//END_HISTORY

#include "Selector.h"

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include "WindowsSelector.h"
#endif /* PEGASUS_OS_TYPE_WINDOWS */

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Selector
//
////////////////////////////////////////////////////////////////////////////////

SelectorHandler::~SelectorHandler()
{

}

Selector::~Selector()
{

}

Selector* Selector::create()
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return new WindowsSelector;
#else
    return 0;
#endif
}

PEGASUS_NAMESPACE_END
