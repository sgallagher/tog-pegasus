//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: Jenny Yu (jenny_yu@hp.com)
//              Warren Otsuka (warren.otsuka@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Version_h
#define Pegasus_Version_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

const char PEGASUS_NAME[] = "CIM Server ";
const char PEGASUS_VERSION[] = "2.1";
const char PEGAUS_STATUS[] = "Experimental" ;

#if defined(PLATFORM_VERSION_SUPPORTED)
#  if defined(PEGASUS_PLATFORM_HPUX_ACC)
#    include <Pegasus/Common/Version_HPUX_ACC.h>
#  elif defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
#    include <Pegasus/Common/Version_Linux_IA64.h>
#  endif
#endif

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Version_h */
