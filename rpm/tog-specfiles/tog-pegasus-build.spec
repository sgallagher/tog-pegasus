#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
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
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-build.spec
#
export PEGASUS_ROOT=%PEGASUS_RPM_ROOT
export PEGASUS_HOME=%PEGASUS_RPM_HOME
export PEGASUS_PLATFORM=%PEGASUS_HARDWARE_PLATFORM
export PEGASUS_ARCH_LIB=%PEGASUS_ARCH_LIB
export PEGASUS_ENVVAR_FILE=$PEGASUS_ROOT/env_var_Linux.status

export OPENSSL_HOME=%OPENSSL_HOME
export OPENSSL_BIN=%OPENSSL_BIN
export LD_LIBRARY_PATH=$PEGASUS_HOME/lib
export PATH=$PEGASUS_HOME/bin:$PATH

export PEGASUS_EXTRA_C_FLAGS="$RPM_OPT_FLAGS -g"
export PEGASUS_EXTRA_CXX_FLAGS="$PEGASUS_EXTRA_C_FLAGS"
export PEGASUS_EXTRA_LINK_FLAGS="$RPM_OPT_FLAGS"
#export PEGASUS_EXTRA_LINK_FLAGS="$RPM_OPT_FLAGS -g -pie -Wl,-z,relro,-z,now,-z,nodlopen,-z,noexecstack"

make -f $PEGASUS_ROOT/Makefile.Release create_ProductVersionFile
make -f $PEGASUS_ROOT/Makefile.Release create_CommonProductDirectoriesInclude
make -f $PEGASUS_ROOT/Makefile.Release create_ConfigProductDirectoriesInclude
make -f $PEGASUS_ROOT/Makefile.Release all
make -f $PEGASUS_ROOT/Makefile.Release repository
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-build.spec
