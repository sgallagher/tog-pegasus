//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Tony Fiorentino (fiorentino_tony@emc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Attribute_h
#define Pegasus_Attribute_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** Defines a single SLP attribute that will be provided to the CIMServerDiscovery Class
    as part of the execution of an SLP discovery.
    ATTN: KS Add more material here.
*/
class PEGASUS_CLIENT_LINKAGE Attribute : public String
{
public:

  /** Constructor for a Attribute object.
      ATTN: TBD
  */
  Attribute(const String & attrEntry = String::EMPTY, const Char16 & delimiter = '=');

  /** Destructor for a Attribute object.
  */
  ~Attribute();

  /** Returns this attributes value.
      @param   defaultValue Default attribute value to return if empty or mal-formed.
      @return  String that contains an attribute value.
  */
  String getValue(const String & defaultValue);

  /** Sets this attributes value.
      @param   defaultValue Default attribute value to return if empty or mal-formed.
      @return  Boolean value that is false if empty or mal-formed.
  */
  Boolean setValue(const String & value);

  /** Returns this attributes delimiter.
      @return  Char16 that contains the delimiter for this attribute.
  */
  Char16 getDelimiter();

private:
  Char16 _delimit;
};

#define PEGASUS_ARRAY_T Attribute
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Attribute_h */
