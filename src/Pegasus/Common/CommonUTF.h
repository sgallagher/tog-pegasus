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
// Author: Dave Rosckes   (rosckes@us.ibm.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CommonUTF_h
#define Pegasus_CommonUTF_h
#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

#define FIRST_HIGH_SURROGATE  (Uint32)0xD800
#define LAST_HIGH_SURROGATE   (Uint32)0xDBFF
#define FIRST_LOW_SURROGATE   (Uint32)0xDC00
#define LAST_LOW_SURROGATE    (Uint32)0xDFFF
#define REPLACEMENT_CHARACTER (Uint32)0x0000FFFD
#define MAX_BYTE              (Uint32)0x0000FFFF
#define MAX_UTF16             (Uint32)0x0010FFFF

static const Uint32 halfBase = 0x0010000UL;
static const Uint32 halfMask = 0x3FFUL;
static const int halfShift  = 10;
static const Uint8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static const Uint32 offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
		     0x03C82080UL, 0xFA082080UL, 0x82082080UL };

static const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

#define UTF_8_COUNT_TRAIL_BYTES(leadByte) (trailingBytesForUTF8[(Uint8)leadByte])

#define UTF_8_MASK_LEAD_BYTE(leadByte, countTrailBytes) ((leadByte)&=(1<<(6-(countTrailBytes)))-1)

#define UTF8_NEXT(s, i, c) { \
    (c)=(s)[(i)++]; \
    if((Uint8)((c)-0xc0)<0x35) { \
        Uint8 __count=UTF_8_COUNT_TRAIL_BYTES(c); \
        UTF_8_MASK_LEAD_BYTE(c, __count); \
        switch(__count) { \
        case 3: \
            (c)=((c)<<6)|((s)[(i)++]&0x3f); \
        case 2: \
            (c)=((c)<<6)|((s)[(i)++]&0x3f); \
        case 1: \
            (c)=((c)<<6)|((s)[(i)++]&0x3f); \
            break; \
        } \
    } \
}

int isValid_U8(const Uint8 *src,int size);
int UTF16toUTF8(const Uint16** srcHead,
		const Uint16* srcEnd, 
		Uint8** tgtHead,
		Uint8* tgtEnd);

int UTF8toUTF16 (const Uint8** srcHead,
		 const Uint8* srcEnd, 
		 Uint16** tgtHead,
		 Uint16* tgtEnd);

PEGASUS_NAMESPACE_END
#endif
