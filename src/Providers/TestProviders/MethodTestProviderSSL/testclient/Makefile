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
	@$(RM) $(PEGASUS_HOME)/testca1.cert
	@$(RM) $(PEGASUS_HOME)/testca1.key
	@$(RM) $(PEGASUS_HOME)/ssltest_setup

tests:

poststarttests: $(OPENSSL_CNF) testca1.txt
	@$(MAKE) -i -s cleanup
	@$(TOUCH) $(PEGASUS_HOME)/ssltest_setup

	@# Create a self-signed CA
	@$(OPENSSL_COMMAND) genrsa -out $(PEGASUS_HOME)/testca1.key 1024
	@$(OPENSSL_COMMAND) req -new -key $(PEGASUS_HOME)/testca1.key -x509 -config $(OPENSSL_CNF) $(SETSERIAL_0) -days 365 -out $(PEGASUS_HOME)/testca1.cert < testca1.txt

	@$(ECHO) "Add the CA certificate to the truststore."
	@cimtrust -a -T a -U $(CURRENT_USER) -f $(PEGASUS_HOME)/testca1.cert

	$(PROGRAM) $(PEGASUS_HOME)/testca1.cert $(PEGASUS_HOME)/testca1.key

