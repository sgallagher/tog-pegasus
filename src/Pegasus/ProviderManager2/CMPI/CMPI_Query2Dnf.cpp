//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author:       Konrad Rzeszutek <konradr@us.ibm.com>
//
// Modified By:  
//
//%/////////////////////////////////////////////////////////////////////////////


#include "CMPI_Version.h"
#include "CMPI_Query2Dnf.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

CMPI_QueryOperand::~CMPI_QueryOperand ()
{
  _type = NULL_TYPE;
}

CMPI_QueryOperand::CMPI_QueryOperand (const String & x, Type type)
{
  _type = type;
  _stringValue = String (x);
}


Boolean
CMPI_QueryOperand::operator== (const CMPI_QueryOperand & x) const 
{

  if (x._type == _type)
    {
      return x._stringValue == _stringValue;
    }
  return false;
}

int
CMPI_term_el::toStrings (CMPIType & typ, CMPIPredOp & opr, String & o1,
                         String & o2) const 
{

  opr = op;
  o1 = opn1.getTypeValue ();
  o2 = opn2.getTypeValue ();
  if (opn1.getType () == CMPI_QueryOperand::PROPERTY_TYPE)
    typ = mapType (opn2.getType ());
  else
    typ = mapType (opn1.getType ());
  return 0;
}

Boolean
CMPI_term_el::operator== (const CMPI_term_el & x)
{
  if (x.op == op)
    {
      if (opn1.getType () == opn2.getType ())
        {
          return opn1.getTypeValue () == opn2.getTypeValue ();
        }
    }
  return false;
}

CMPIType
CMPI_term_el::mapType (CMPI_QueryOperand::Type type) const 
{

  switch (type)
    {
    case CMPI_QueryOperand::NULL_TYPE:
      return CMPI_null;
    case CMPI_QueryOperand::SINT64_TYPE:
      return CMPI_sint64;
    case CMPI_QueryOperand::UINT64_TYPE:
      return CMPI_uint64;
    case CMPI_QueryOperand::STRING_TYPE:
      return CMPI_string;
    case CMPI_QueryOperand::DATETIME_TYPE:
      return CMPI_dateTime;
    case CMPI_QueryOperand::REFERENCE_TYPE:
      return CMPI_ref;
    case CMPI_QueryOperand::PROPERTY_TYPE:
      return CMPI_nameString;
    case CMPI_QueryOperand::BOOLEAN_TYPE:
      return CMPI_boolean;
    case CMPI_QueryOperand::OBJECT_TYPE:
      return CMPI_nameString;
    case CMPI_QueryOperand::REAL_TYPE:
      return CMPI_REAL;
    default:
      break;
    }
  return CMPI_null;
}

PEGASUS_NAMESPACE_END
