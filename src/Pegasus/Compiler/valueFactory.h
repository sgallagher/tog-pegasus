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
// Author: Bob Blair (bblair@bmc.com)
//
// $Log: valueFactory.h,v $
// Revision 1.2  2001/04/24 00:00:15  mike
// Ported compiler to use String and Array (rather than STL equivalents)
//
// Revision 1.1  2001/02/16 23:59:09  bob
// Initial checkin
//
//
//
//END_HISTORY
//
// Header for a class to generate CIMValue objects from String values
//

#ifndef _VALUEFACTORY_H_
#define _VALUEFACTORY_H_

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMValue.h>

using namespace Pegasus;
using namespace std;

// The valueFactory builds a CIMValue object given an indication of
// whether this is a Array type, the CIM type to be coerced, and
// a String representation of the value.
//
class PEGASUS_COMPILER_LINKAGE valueFactory  {
 public:
  static unsigned long Stoi(const String &s);
  static CIMValue * createValue(CIMType::Tag type, int arrayDimension,
			     const String *rep);
};

#endif
