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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//
//%////////////////////////////////////////////////////////////////////////////
//
// Parent class of classes intended to get or set data on behalf of a
// provider.

#ifndef Pegasus_ValidatedTypes_h
#define Pegasus_ValidatedTypes_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN


/** An exception which is thrown whenever a read is made from an
 *  uninitialized validate<> data type. */
class AccessedInvalidData : public Exception {
public:
  AccessedInvalidData(void) : Exception(String("Attempted to read an invalid data field.")) {}
};


/** A template for validated data types.  The data must be explicitly set
 *  before it can be read.  If the read data was never set, the
 *  AccessedInvalidData exception is thrown. */
template <class DTYPE>
class validated {
private:
  bool valid;
  DTYPE data;
public:
  /// Initialize to invalid state
  validated(void) { valid = false; }
  ~validated(void) {}

  /// Returns true if the data has been set (and so is valid)
  bool is_valid(void) const { return valid; }

  /// Retrieve the value of the data, or throw exception if invalid
  DTYPE getValue(void) const {
    if (!is_valid()) 
      throw AccessedInvalidData(); 
    else 
      return data; 
  }

  /// Set the value of the data, and mark it as valid
  void setValue(DTYPE const &value) { valid = true; data = value; }
};



PEGASUS_NAMESPACE_END

#endif  /* Pegasus_ValidatedTypes_h  */
