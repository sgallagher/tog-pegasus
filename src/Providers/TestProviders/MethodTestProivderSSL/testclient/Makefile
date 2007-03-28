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
ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/TestProviders/MethodTestProviderSSL/testclient

include $(ROOT)/mak/config.mak

OPENSSL_CNF = openssl.cnf

ifeq ($(OS_TYPE),windows)
    CURRENT_USER=Administrator
else
    CURRENT_USER=root
endif

ifeq ($(OPENSSL_SET_SERIAL_SUPPORTED),true)
    SETSERIAL_0 = -set_serial 0
    SETSERIAL_1 = -set_serial 1
    SETSERIAL_2 = -set_serial 2
    SETSERIAL_3 = -set_serial 3
else
    SETSERIAL_0 =
    SETSERIAL_1 =
    SETSERIAL_2 =
    SETSERIAL_3 =
endif

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegclient \
    pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = TestMethodTestProviderSSL

SOURCES = TestMethodTestProviderSSL.cpp

include $(ROOT)/mak/program.mak

cleanup:
	@$(RM) $(PEGASUS_HOME)/testchca.cert
	@$(RM) $(PEGASUS_HOME)/testchca.key
	@$(RM) $(PEGASUS_HOME)/testchca.srl
	@$(RM) $(PEGASUS_HOME)/testinter.cert
	@$(RM) $(PEGASUS_HOME)/testinter.key
	@$(RM) $(PEGASUS_HOME)/testinter.srl
	@$(RM) $(PEGASUS_HOME)/testleaf.cert
	@$(RM) $(PEGASUS_HOME)/testleaf.key

tests:

poststarttests: $(OPENSSL_CNF) testchca.cert testchca.key testinter.cert testinter.key testleaf.cert testleaf.key
	@$(MAKE) -i -s cleanup
	@$(TOUCH) ssltest_setup

        # Copy the test chain root certificate to the staging area
	@$(COPY) testchca.cert $(PEGASUS_HOME)/testchca.cert
	@$(COPY) testchca.key $(PEGASUS_HOME)/testchca.key
        # Copy the test chain intermediate certificate to the staging area
	@$(COPY) testinter.cert $(PEGASUS_HOME)/testinter.cert
	@$(COPY) testinter.key $(PEGASUS_HOME)/testinter.key
        # Copy the leaf certificate to the staging area
	@$(COPY) testleaf.cert $(PEGASUS_HOME)/testleaf.cert
	@$(COPY) testleaf.key $(PEGASUS_HOME)/testleaf.key

	@$(ECHO) "Add the authority certificate to the truststore."
	@cimtrust -a -Ta -U $(CURRENT_USER) -f $(PEGASUS_HOME)/testchca.cert

	@$(ECHO) "Add the intermediate CA signed certificate to the truststore."
	@cimtrust -a -T a -U $(CURRENT_USER) -f $(PEGASUS_HOME)/testinter.cert

	$(PROGRAM) $(PEGASUS_HOME)/testinter.cert $(PEGASUS_HOME)/testinter.key

