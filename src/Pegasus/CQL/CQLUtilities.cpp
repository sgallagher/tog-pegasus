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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////
#include <Pegasus/CQL/CQLUtilities.h>
#include <Pegasus/Common/Exception.h>
#include <errno.h>

#define PEGASUS_SINT64_MIN (PEGASUS_SINT64_LITERAL(0x8000000000000000))
#define PEGASUS_UINT64_MAX PEGASUS_UINT64_LITERAL(0xFFFFFFFFFFFFFFFF)

#ifndef _MSC_VER
#define _MSC_VER 0
#endif

PEGASUS_NAMESPACE_BEGIN

inline Uint8 _CQLUtilities_hexCharToNumeric(const Char16 c)
{
    Uint8 n;

    if (isdigit(c))
        n = (c - '0');
    else if (isupper(c))
        n = (c - 'A' + 10);
    else // if (islower(c))
        n = (c - 'a' + 10);

    return n;
}

Uint64 CQLUtilities::stringToUint64(const String &stringNum)
{
  Uint64 x = 0;
  const Char16* p = stringNum.getChar16Data();
  const Char16* pStart = p;

  if (!p)
    throw(Exception(String("CQLUtilities::stringToUint64 -- string to convert is NULL")));

  // There cannot be a negative '-' sign
  if (*p == '-')
    throw(Exception(String("CQLUtilities::stringToUint64 -- string to convert is negative")));
  if (*p == '+')
    p++;  // skip over the positive sign

  if (!isdigit(*p))
    throw(Exception(String("CQLUtilities::stringToUint64 -- numeric string must begin with a digit")));

  // if binary
  Uint32 endString = stringNum.size() - 1;
  if ( (pStart[endString] == 'b') || (pStart[endString] == 'B') )
  {
    // Add on each digit, checking for overflow errors
    while ((*p == '0') || (*p == '1'))
    {
      // Make sure we won't overflow when we multiply by 2
      if (x > PEGASUS_UINT64_MAX/2)
        throw(Exception(String("CQLUtilities::stringToUint64 -- overflow error")));

      x = x << 1;

      // We can't overflow when we add the next digit
      Uint64 newDigit = 0;
      if (*p++ == '1')
        newDigit = 1;
      if (PEGASUS_UINT64_MAX - x < newDigit)
        throw(Exception(String("CQLUtilities::stringToUint64 -- overflow error")));

      x = x + newDigit;
    }

    // If we found a non-binary digit, report an error
    if (*p && (*p != 'b') && (*p != 'B'))
      throw(Exception(String("CQLUtilities::stringToUint64 -- non-binary digit")));

    // return value from the binary string
    return x;      
  } // end if binary

  // if hexidecimal
  if ( (*p == '0') && ((p[1] == 'x') || (p[1] == 'X')) )
  {
    // Convert a hexadecimal string

    // Skip over the "0x"
    p+=2;

    // At least one hexadecimal digit is required
    if (!*p)
      throw(Exception(String("CQLUtilities::stringToUint64 -- at least one hexadecimal digit is required")));

    // Add on each digit, checking for overflow errors
    while (isxdigit(*p))
    {
      // Make sure we won't overflow when we multiply by 16
      if (x > PEGASUS_UINT64_MAX/16)
        throw(Exception(String("CQLUtilities::stringToUint64 -- overflow error")));

      x = x << 4;

      // We can't overflow when we add the next digit
      Uint64 newDigit = Uint64(_CQLUtilities_hexCharToNumeric(*p++));
      if (PEGASUS_UINT64_MAX - x < newDigit)
        throw(Exception(String("CQLUtilities::stringToUint64 -- overflow error")));

      x = x + newDigit;
    }

    // If we found a non-hexadecimal digit, report an error
    if (*p)
      throw(Exception(String("CQLUtilities::stringToUint64 -- non-hexadecimal digit")));

    // return value from the hex string
    return x;
  }  // end if hexidecimal     


  // Expect a positive decimal digit:

  // Add on each digit, checking for overflow errors
  while (isdigit(*p))
  {
    // Make sure we won't overflow when we multiply by 10
    if (x > PEGASUS_UINT64_MAX/10)
      throw(Exception(String("CQLUtilities::stringToUint64 -- overflow error")));
    x = 10 * x;

    // Make sure we won't overflow when we add the next digit
    Uint64 newDigit = (*p++ - '0');
    if (PEGASUS_UINT64_MAX - x < newDigit)
      throw(Exception(String("CQLUtilities::stringToUint64 -- overflow error")));

    x = x + newDigit;
  }

  // If we found a non-decimal digit, report an error
  if (*p)
    throw(Exception(String("CQLUtilities::stringToUint64 -- non-decimal digit")));

  // return the value for the decimal string
  return x;
}

Sint64 CQLUtilities::stringToSint64(const String &stringNum)
{
  Sint64 x = 0;
  Boolean invert = false;
  const Char16* p = stringNum.getChar16Data();
  const Char16* pStart = p;

  if (!p)
    throw(Exception(String("CQLUtilities::stringToSint64 -- string to convert is NULL")));

  // skip over the sign if there is one
  if (*p == '-')
  {
    invert = true;
    p++;
  }
  if (*p == '+')
    p++;

  if (!isdigit(*p))
    throw(Exception(String("CQLUtilities::stringToSint64 -- numeric string must begin with a digit")));

  // ********************
  // Build the Sint64 as a negative number, regardless of the
  // eventual sign (negative numbers can be bigger than positive ones)
  // ********************
  
  // if binary
  Uint32 endString = stringNum.size() - 1;
  if ( (pStart[endString] == 'b') || (pStart[endString] == 'B') )
  {
    // Add on each digit, checking for overflow errors
    while ((*p == '0') || (*p == '1'))
    {
      // Make sure we won't overflow when we multiply by 2
      if (x < PEGASUS_SINT64_MIN/2)
        throw(Exception(String("CQLUtilities::stringToSint64 -- overflow error")));

      x = x << 1;

      // We can't overflow when we add the next digit
      Sint64 newDigit = 0;
      if (*p++ == '1')
        newDigit = 1;
      if (PEGASUS_SINT64_MIN - x > -newDigit)
        throw(Exception(String("CQLUtilities::stringToSint64 -- overflow error")));

      x = x - newDigit;
    }

    // If we found a non-binary digit, report an error
    if (*p && (*p != 'b') && (*p != 'B'))
      throw(Exception(String("CQLUtilities::stringToSint64 -- non-binary digit")));

    // Return the integer to positive, if necessary, checking for an
    // overflow error
    if (!invert)
    {
      if (x == PEGASUS_SINT64_MIN)
        throw(Exception(String("CQLUtilities::stringToSint64 -- overflow error")));
      x = -x;
    }
    
    // return value from the binary string
    return x;      
  }  // end if binary

  // if hexidecimal
  if ( (*p == '0') && ((p[1] == 'x') || (p[1] == 'X')) )
  {
    // Convert a hexadecimal string

    // Skip over the "0x"
    p+=2;

    // At least one hexadecimal digit is required
    if (!*p)
      throw(Exception(String("CQLUtilities::stringToSint64 -- at least one hexadecimal digit is required")));

    // Add on each digit, checking for overflow errors
    while (isxdigit(*p))
    {
      // Make sure we won't overflow when we multiply by 16
      if (x < PEGASUS_SINT64_MIN/16)
        throw(Exception(String("CQLUtilities::stringToSint64 -- overflow error")));

      x = x << 4;

      // We can't overflow when we add the next digit
      Sint64 newDigit = Sint64(_CQLUtilities_hexCharToNumeric(*p++));
      if (PEGASUS_SINT64_MIN - x > -newDigit)
        throw(Exception(String("CQLUtilities::stringToSint64 -- overflow error")));

      x = x - newDigit;
    }

    // If we found a non-hexadecimal digit, report an error
    if (*p)
      throw(Exception(String("CQLUtilities::stringToSint64 -- non-hexadecimal digit")));

    // Return the integer to positive, if necessary, checking for an
    // overflow error
    if (!invert)
    {
      if (x == PEGASUS_SINT64_MIN)
        throw(Exception(String("CQLUtilities::stringToSint64 -- overflow error")));
      x = -x;
    }
    
    // return value from the hex string
    return x;
  }  // end if hexidecimal     


  // Expect a positive decimal digit:

  // Add on each digit, checking for overflow errors
  while (isdigit(*p))
  {
    // Make sure we won't overflow when we multiply by 10
    if (x < PEGASUS_SINT64_MIN/10)
      throw(Exception(String("CQLUtilities::stringToSint64 -- overflow error")));
    x = 10 * x;

    // Make sure we won't overflow when we add the next digit
    Sint64 newDigit = (*p++ - '0');
    if (PEGASUS_SINT64_MIN - x > -newDigit)
      throw(Exception(String("CQLUtilities::stringToSint64 -- overflow error")));

    x = x - newDigit;
  }

  // If we found a non-decimal digit, report an error
  if (*p)
    throw(Exception(String("CQLUtilities::stringToSint64 -- non-decimal digit")));

  // Return the integer to positive, if necessary, checking for an
  // overflow error
  if (!invert)
  {
    if (x == PEGASUS_SINT64_MIN)
      throw(Exception(String("CQLUtilities::stringToSint64 -- overflow error")));
    x = -x;
  }

  // return the value for the decimal string
  return x;  
}

Real64 CQLUtilities::stringToReal64(const String &stringNum)
{
  Real64 x = 0;
  const Char16* p = stringNum.getChar16Data();
  Boolean neg = false;
  const Char16* pStart = p;

  if (!*p)
    throw(Exception(String("CQLUtilities::stringToReal64 -- string to convert is empty")));

  
  // Skip optional sign:

  if (*p == '+')
    p++;
  
  if (*p  == '-')
  {
    neg = true;
    p++;
  };
  
  // Check if it it is a binary or hex integer
  Uint32 endString = stringNum.size() - 1;
  if ((*p == '0' && (p[1] == 'x' || p[1] == 'X')) ||  // hex OR
      pStart[endString] == 'b' || pStart[endString] == 'B')  // binary
  {
    if (neg)
      x = stringToSint64(stringNum);
    else

// Check if the complier is MSVC 6, which does not support the conversion operator from Uint64 to Real64      
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC) && (_MSC_VER < 1300)
    {
      Uint64 num = stringToUint64(stringNum);
      Sint64 half = num / 2;
      x = half;
      x == half;
      if (num % 2)  // if odd, then add the lost remainder
        x += 1;
    }
#else
      x = stringToUint64(stringNum);
#endif    
    return x;
  }  
  
  // Skip optional first set of digits:

  while (isdigit(*p))
    p++;

  // Test if optional dot is there
  if (*p++ == '.')
  {
    // One or more digits required:
    if (!isdigit(*p++))
      throw(Exception(String("CQLUtilities::stringToReal64 -- required digit missing after the decimal")));

    while (isdigit(*p))
      p++;

    // If there is an exponent now:

    if (*p)
    {
      // Test exponent:

      if (*p != 'e' && *p != 'E')
        throw(Exception(String("CQLUtilities::stringToReal64 -- malformed string - digit or exponent symbol expected")));

      p++;

      // Skip optional sign:

      if (*p == '+' || *p  == '-')
        p++;

      // One or more digits required:

      if (!isdigit(*p++))
        throw(Exception(String("CQLUtilities::stringToReal64 -- required digit missing after the exponent symbol")));

      while (isdigit(*p))
        p++;
    }
  }
  if (*p)
    throw(Exception(String("CQLUtilities::stringToReal64 -- non-decimal digit")));
  //
  // Do the conversion
  //
  char* end;
  errno = 0;
  CString temp = stringNum.getCString();
  x = strtod((const char *) temp, &end);
  if (*end || (errno == ERANGE))
  {
    throw(Exception(String("CQLUtilities::stringToReal64 -- conversion error - string may be out of range")));
  }
  return x;
}

PEGASUS_NAMESPACE_END
