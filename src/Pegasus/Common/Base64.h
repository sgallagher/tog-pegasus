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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////


/** This class encodes binary data to base64 Strings and
    decodes Strings coded in base64 into the corresponding binary
    data.
    The base64 data representation is based on a 64-character alphabet:
    <pre>
                       Table 1: The Base64 Alphabet

      Value Encoding  Value Encoding  Value Encoding  Value Encoding
           0 A            17 R            34 i            51 z
           1 B            18 S            35 j            52 0
           2 C            19 T            36 k            53 1
           3 D            20 U            37 l            54 2
           4 E            21 V            38 m            55 3
           5 F            22 W            39 n            56 4
           6 G            23 X            40 o            57 5
           7 H            24 Y            41 p            58 6
           8 I            25 Z            42 q            59 7
           9 J            26 a            43 r            60 8
          10 K            27 b            44 s            61 9
          11 L            28 c            45 t            62 +
          12 M            29 d            46 u            63 /
          13 N            30 e            47 v
          14 O            31 f            48 w         (pad) =
          15 P            32 g            49 x
          16 Q            33 h            50 y
    </pre>
    The input file is encoded 6 bits at a time into a single character
    in the 64-character alphabet. Where padding is required at the end
    of the stream, the padding character is '='.
    Finally, the output stream should also be broken into lines to improve
    human readability.  This class breaks it at 76 characters and insert a
    CR/LF into the stream.  This increases the length by less than 3%.
    Since the decoding ingores characters that are outside the 64 character
    alphabet, the CR, LF and padding character are dropped.
*/

#ifndef Pegasus_Base64_h
#define Pegasus_Base64_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN


class PEGASUS_COMMON_LINKAGE Base64
{
public:
    /**
        Encodes an Array<char> into a base64 array.

        @param vby Array<char> with the data to be encoded.
        @return Array<char> with the encoded data
        @exception bad_alloc Thrown if there is insufficient memory.
    */
    static Array<char> encode(const Array<char>& vby);  

    /**
        Decodes an base64 array into an Array<char>

        @param str Array<char> with the data to be decoded.
        @return Array<char> with the decoded data
        @exception bad_alloc Thrown if there is insufficient memory.
    */
    static Array<char> decode(const Array<char>& str);

private:
    static char _Encode(Uint8 uc);
    static Uint8 _Decode(char c);
    static Boolean _IsBase64(char c);
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_Base64_h */
