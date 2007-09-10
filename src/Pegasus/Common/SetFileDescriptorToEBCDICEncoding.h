//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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

#ifndef SetFileDescriptorToEBCDICEncoding_h
#define SetFileDescriptorToEBCDICEncoding_h

#include <sys/stat.h>

PEGASUS_NAMESPACE_BEGIN

#define __CCSID_ZOS 1047

inline int setEBCDICEncoding(int fileDescriptor)
{
    struct stat entryInfo;
    attrib_t repFileAttr;
    int repFileAttr_len = sizeof(repFileAttr);
    
    /* Generate the file tag attributes */
    /* Clear the attribute structure   */
    memset(&repFileAttr, 0, repFileAttr_len);
    /* Update attribute to request file tag change */
    repFileAttr.att_filetagchg = 1; 
    /* Set the file descriptor CCSID */
    repFileAttr.att_filetag.ft_ccsid = __CCSID_ZOS; 
    /* Set the text file conversion on */
    repFileAttr.att_filetag.ft_txtflag = 1;

    return __fchattr(fileDescriptor, &repFileAttr, repFileAttr_len);
} 

PEGASUS_NAMESPACE_END

#endif // SetFileDescriptorToEBCDICEncoding_h 


