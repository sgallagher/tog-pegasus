//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Tony Fiorentino (fiorentino_tony@emc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Attribute.h"

PEGASUS_NAMESPACE_BEGIN

Attribute::Attribute(const String & attrEntry, const Char16 & delimiter) :
  String(attrEntry), _delimit(delimiter)
{
}

Attribute::~Attribute()
{
}

String
Attribute::getValue(const String & defaultValue)
{
  Uint32 idxDelimiter = find(_delimit);
  Uint32 idxValue = idxDelimiter + 1;

  if (idxDelimiter != PEG_NOT_FOUND && idxValue < size())
    {
      return subString(idxValue);
    }
  else
    {
      return defaultValue;
    }
}

Boolean
Attribute::setValue(const String & value)
{
  String newAttrEntry;

  Uint32 idx = find(_delimit);

  if (idx != PEG_NOT_FOUND)
    {
      newAttrEntry = subString(0, idx) + value;
      clear();
      assign(newAttrEntry);
      return true;
    }

  return false;
}

Char16
Attribute::getDelimiter()
{
  return _delimit;
}

#define PEGASUS_ARRAY_T Attribute
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END
