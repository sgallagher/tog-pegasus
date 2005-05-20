#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
# Name for the Pegasus compiler.

CIMMOFCLI = cimmofl

# The following variables determine the version of the Schema to
# be loaded into Namespaces.
# Update the following environment variables to change the version.

PG_INTEROP_SCHEMA_VER=20
PG_INTEROP_SCHEMA_DIR=VER$(PG_INTEROP_SCHEMA_VER)
PG_INTEROP_MOF_PATH=$(ROOT)/Schemas/Pegasus/InterOp/$(PG_INTEROP_SCHEMA_DIR)

# The INTEROPNS variable defines the absolute name of the
# Pegasus InterOp Namespace.

INTEROPNS=root/PG_InterOp
INTEROPNSDIRECTORY = $(REPOSITORY_ROOT)/root\#PG_InterOp

# The INTERNALNS variable defines the absolute name of the
# Pegasus INTERNALNS Namespace.

INTERNALNS = root/PG_Internal
INTERNALNSDIRECTORY = $(REPOSITORY_ROOT)/root\#PG_Internal

# The MANAGEDSYSTEMNS variable defines the absolute name of the
# namespace for the managed system running the CIMOM.  The content and
# the name of this namespace is expected to be platform specific.

MANAGEDSYSTEMNS = NOTDEFINED

ifeq ($(OS), HPUX)
#   MANAGEDSYSTEMNS = root/HPV2
#   MANAGEDSYSTEMNSDIRECTORY = $(REPOSITORY_ROOT)/root\#HPV2
endif

ifeq ($(MANAGEDSYSTEMNS),NOTDEFINED) 
   MANAGEDSYSTEMNS = root/cimv2
   MANAGEDSYSTEMNSDIRECTORY = $(REPOSITORY_ROOT)/root\#cimv2
endif
