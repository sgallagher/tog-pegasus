//%///////////////////////-*-c++-*-/////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Marek Szermutzky (MSzermutzky@de.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////
#ifndef SocketzOS_inline_h
#define SocketzOS_inline_h

// this inline method is needed as zOS does not support an ASCII enabled version
// of inet_addr() at the current time (16th Sep. 2003)
inline in_addr_t inet_addr_ebcdic(char * ip_inptr)
{
    int array_size = 0;
    in_addr_t return_addr;
    while (ip_inptr[array_size] != 0) array_size++;
    char * ip_ptr2 = (char *)malloc(array_size);
    memcpy(ip_ptr2,ip_inptr,array_size);
    __atoe_l(ip_ptr2,array_size);
    return_addr = inet_addr(ip_ptr2);
    free(ip_ptr2);
    return return_addr;
}
#endif

