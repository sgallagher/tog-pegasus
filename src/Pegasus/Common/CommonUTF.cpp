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
// Author: Dave Rosckes   (rosckes@us.ibm.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CommonUTF.h"
#include <cstring>

PEGASUS_NAMESPACE_BEGIN

// Note: Caller must ensure that "src" contains "size" bytes.
int isValid_U8(const Uint8 *src, int size)
{
    Uint8 U8_char;
    const Uint8 *srcptr = src+size;
    switch (size)
    {
	case 4:
	    if ((U8_char = (*--srcptr)) < 0x80 || U8_char > 0xBF)
	    {
		return false;
	    }
	case 3:
	    if ((U8_char = (*--srcptr)) < 0x80 || U8_char > 0xBF)
	    {
		return false;
	    }
	case 2:
	    if ((U8_char = (*--srcptr)) > 0xBF)
	    {
		return false;
	    }
	    switch (*src)
	    {
		case 0xE0:
		    if (U8_char < 0xA0)
		    {
			return false;
		    }
		    break;
		case 0xF0:
		    if (U8_char < 0x90)
		    {
			return false;
		    }
		    break;
		case 0xF4:
		    if (U8_char > 0x8F)
		    {
			return false;
		    }
		    break;
		default:
		    if (U8_char < 0x80)
		    {
			return false;
		    }
	    }
	case 1:
	    if (*src >= 0x80 && *src < 0xC2)
	    {
		return false;
	    }
	    if (*src > 0xF4)
	    {
		return false;
	    }
	    break;
        default:
	    {
		return false;
            }

    }
    return true;
}	

int UTF16toUTF8(const Uint16** srcHead,
		const Uint16* srcEnd, 
		Uint8** tgtHead,
		Uint8* tgtEnd)
{
    int returnCode = 0;
    const Uint16* src = *srcHead;
    Uint8* tgt = *tgtHead;
    while (src < srcEnd)
    {
	Uint32 tempchar;
	Uint16 numberOfBytes = 0;
	const Uint16* oldsrc = src; 
	tempchar = *src++;
	if (tempchar >= FIRST_HIGH_SURROGATE
	    && tempchar <= LAST_HIGH_SURROGATE)
	{
	    if (src < srcEnd)
	    {
		Uint32 tempchar2 = *src;
		if (tempchar2 >= FIRST_LOW_SURROGATE &&
		    tempchar2 <= LAST_LOW_SURROGATE)
		{
		    tempchar = ((tempchar - FIRST_HIGH_SURROGATE) << halfShift)
		      + (tempchar2 - FIRST_LOW_SURROGATE) + halfBase;
		    ++src;
		} 
	    }
	    else
	    { 
		--src;
		returnCode = -1;
		break;
	    }
	}
	if (tempchar < (Uint32)0x80)
	{
	    numberOfBytes = 1;
	}
	else if (tempchar < (Uint32)0x800)
	{
	    numberOfBytes = 2;
	}
	else if (tempchar < (Uint32)0x10000)
	{
	    numberOfBytes = 3;
	}
	else if (tempchar < (Uint32)0x200000)
	{
	    numberOfBytes = 4;
	}
	else
	{
	    numberOfBytes = 2;
	    tempchar = REPLACEMENT_CHARACTER;
	}

	tgt += numberOfBytes;
	if (tgt > tgtEnd)
	{
	    src = oldsrc;
	    tgt -= numberOfBytes;
	    returnCode = -1;
	    break;
	}

	switch (numberOfBytes)
	{ 
	    case 4:
		*--tgt = (Uint8)((tempchar | 0x80) & 0xBF);
		tempchar >>= 6;
	    case 3:
		*--tgt = (Uint8)((tempchar | 0x80) & 0xBF);
		tempchar >>= 6;
	    case 2:
		*--tgt = (Uint8)((tempchar | 0x80) & 0xBF);
		tempchar >>= 6;
	    case 1:
		*--tgt =  (Uint8)(tempchar | firstByteMark[numberOfBytes]);
	}
	tgt += numberOfBytes;
    }
    *srcHead = src;
    *tgtHead = tgt;
    return returnCode;
}

int UTF8toUTF16 (const Uint8** srcHead,
		 const Uint8* srcEnd, 
		 Uint16** tgtHead,
		 Uint16* tgtEnd)
{
    int returnCode = 0;
    const Uint8* src = *srcHead;
    Uint16* tgt = *tgtHead;
    while (src < srcEnd)
    {
	Uint32 tempchar = 0;
	Uint16 moreBytes = trailingBytesForUTF8[*src];
	if (src + moreBytes >= srcEnd)
	{
	    returnCode = -1;
	    break;
	}
	switch (moreBytes)
	{
	    case 3:
		tempchar += *src++;
		tempchar <<= 6;
	    case 2:
		tempchar += *src++;
		tempchar <<= 6;
	    case 1:
		tempchar += *src++;
		tempchar <<= 6;
	    case 0:
		tempchar += *src++;
	}
	tempchar -= offsetsFromUTF8[moreBytes];

	if (tgt >= tgtEnd)
	{
	    src -= (moreBytes+1); 
	    returnCode = -1; break;
	}
	if (tempchar <= MAX_BYTE)
	{	
	    if ((tempchar >= FIRST_HIGH_SURROGATE &&
		 tempchar <= LAST_LOW_SURROGATE) ||
		((tempchar & 0xFFFE) ==	0xFFFE))
	    {
		*tgt++ = REPLACEMENT_CHARACTER;
	    }
	    else
	    {
		*tgt++ = (Uint16)tempchar; 
	    }
	}
	else if (tempchar > MAX_UTF16)
	{
	    *tgt++ = REPLACEMENT_CHARACTER;
	}
	else
	{
	    if (tgt + 1 >= tgtEnd)
	    {
		src -= (moreBytes+1);
		returnCode = -1;
		break;
	    }
	    tempchar -= halfBase;
	    *tgt++ = (Uint16)((tempchar >> halfShift) + FIRST_HIGH_SURROGATE);
	    *tgt++ = (Uint16)((tempchar & halfMask) + FIRST_LOW_SURROGATE);
	}
    }
    *srcHead = src;
    *tgtHead = tgt;
    return returnCode;
}
PEGASUS_NAMESPACE_END
