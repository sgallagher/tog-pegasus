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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Char16_h
#define Pegasus_Char16_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** The Char16 class represents a CIM sixteen bit character (char16).
    This class is a trivial wrapper for a sixteen bit integer. It is used
    as the element type in the String class (used to represent the CIM
    string type). Ordinarily Uint16 could be used; however, a distinguishable
    type was needed for the purposes of function overloaded which occurs in
    the CIMValue class.
*/
class PEGASUS_COMMON_LINKAGE Char16
{
public:

    /// Constructor.
    Char16();

    /// Constructor.
    Char16(Uint16 x);

    /// Constructor.
    Char16(const Char16& x);

    /// Destructor.
    ~Char16();

    /// Assignment operator.
    Char16& operator=(Uint16 x);

    /// Assignment operator.
    Char16& operator=(const Char16& x);

    /// Implicit converter from Char16 to Uint16
    operator Uint16() const;

    // Returns if the character is an ASCII character.
    Boolean isAscii() const throw();

    // Returns if the character is a digit (0-9).
    Boolean isDigit() const throw();

    // Returns if the character is a hex digit (0-0a-fA-F)
    Boolean isXDigit() const throw();

    // Returns if the character is an alphabetic character (a-zA-Z)
    Boolean isAlpha() const throw();

    // Returns if the character is an upper case alphabetic character (A-Z)
    Boolean isUpper() const throw();

    // Returns if the character is a lower case alphabetic character (A-Z)
    Boolean isLower() const throw();

    // Returns if the character is an alphanumeric character (a-zA-Z0-9)
    Boolean isAlnum() const throw();

    // Returns if the character is a whitespace character (' '\b\t\f)
    Boolean isSpace() const throw();

private:
    inline Boolean _isAscii() const throw()
    {
        return(_code < 0x80);
    }

    Uint16 _code;
};

PEGASUS_COMMON_LINKAGE Boolean operator==(const Char16& x, const Char16& y);
PEGASUS_COMMON_LINKAGE Boolean operator==(const Char16& x, char y);
PEGASUS_COMMON_LINKAGE Boolean operator==(char x, const Char16& y);
PEGASUS_COMMON_LINKAGE Boolean operator!=(const Char16& x, const Char16& y);
PEGASUS_COMMON_LINKAGE Boolean operator!=(const Char16& x, char y);
PEGASUS_COMMON_LINKAGE Boolean operator!=(char x, const Char16& y);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Char16_h */
