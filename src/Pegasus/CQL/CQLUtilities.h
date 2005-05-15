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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////
#ifndef Pegasus_CQLUtilities_h
#define Pegasus_CQLUtilities_h


#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

#include <Pegasus/CQL/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// Forward declares
// class PEGASUS_CQL_LINKAGE String;
class String;


class PEGASUS_CQL_LINKAGE CQLUtilities
{
public:
  static Uint64 stringToUint64(const String &stringNum);
  static Sint64 stringToSint64(const String &stringNum);
  static Real64 stringToReal64(const String &stringNum);

  // This will take a formated string representing a real number and if
  // it uses scientific notiation, then it will strip off leading 0's
  // in the exponent.  If the exponent only has 0's, then it will strip
  // the exponent symbol also.  (ie  "2.56E-00" becomes "2.56"; "2.56E-04"
  // becomes "2.56E-4".  If there is no exponent, then nothing is changed.
  // This will also remove a leading '+' sign from the exponent if it is
  // there.
  static String formatRealStringExponent(const String &realString);

private:
  // This is a helper function which checks if a string has a decimal
  // point in it.  If so, it calls it a real.  In the case of a badly
  // formated string, it will fail the format check in StringToReal64.
  static Boolean isReal(const String &numString);
};

PEGASUS_NAMESPACE_END
#endif
#endif
