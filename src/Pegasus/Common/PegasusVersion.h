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
// Modified By: Jenny Yu (jenny_yu@hp.com)
//              Warren Otsuka (warren.otsuka@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Version_h
#define Pegasus_Version_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_OVERRIDE_PRODUCT_ID
# include <Pegasus/Common/ProductVersion.h>
#else
# define PEGASUS_PRODUCT_NAME    "CIM Server"
# define PEGASUS_PRODUCT_VERSION "2.5"
# define PEGASUS_PRODUCT_STATUS  ""
# define PEGASUS_CIMOM_GENERIC_NAME "Pegasus"
// If the following is non-zero length it becomes SLP description.
# define PEGASUS_CIMOM_DESCRIPTION ""
#endif

#ifdef PEGASUS_OS_HPUX
// Define the "what" string for binary files

# ifndef PLATFORM_BUILD_DATE
#  define PLATFORM_BUILD_DATE __DATE__
# endif

# ifndef PLATFORM_COMPONENT_NAME
#  define PLATFORM_COMPONENT_NAME  "Pegasus Component"
# endif

# ifndef PLATFORM_COMPONENT_VERSION 
#  define PLATFORM_COMPONENT_VERSION PEGASUS_PRODUCT_VERSION 
# endif

# ifndef PLATFORM_COMPONENT_STATUS
#  define PLATFORM_COMPONENT_STATUS PEGASUS_PRODUCT_STATUS
# endif

# ifndef COMPONENT_BUILD_DATE
#  define COMPONENT_BUILD_DATE __DATE__
# endif

static const char *PLATFORM_EMBEDDED_IDENTIFICATION_STRING =
    "@(#)" PEGASUS_PRODUCT_NAME " " PEGASUS_PRODUCT_VERSION " "
        PEGASUS_PRODUCT_STATUS "(" PLATFORM_BUILD_DATE ")" "\n@(#)"
        PEGASUS_PRODUCT_NAME "-" PLATFORM_COMPONENT_NAME " "
        PLATFORM_COMPONENT_VERSION " " PLATFORM_COMPONENT_STATUS "("
        COMPONENT_BUILD_DATE ")";

#endif /* PEGASUS_OS_HPUX */

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Version_h */
