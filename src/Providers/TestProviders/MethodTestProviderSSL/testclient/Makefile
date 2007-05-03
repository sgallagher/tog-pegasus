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
    CURRENT_USER=`whoami`
#    CURRENT_USER=guest
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

tests:

poststarttests:
	@$(ECHO) ++++MethodTestProviderSSL Test
#	@cimtrust -a -U $(CURRENT_USER) -f $(PEGASUS_HOME)/client.pem -Ts
	@cimtrust -a -U $(CURRENT_USER) -f $(PEGASUS_HOME)/server.pem -Ts
	$(TOUCH) $(PEGASUS_HOME)/cimserver_current.conf
	$(COPY) $(PEGASUS_HOME)/cimserver_current.conf $(PEGASUS_HOME)/cimserver_current.conf.bkup
	@$(ECHO) Stopping cimserver
	$(MAKE) --directory=$(PEGASUS_ROOT) -f TestMakefile cimstop
	@$(ECHO) Stopped.

ifeq ($(OS_TYPE),windows)
	$(COPY) cimserver_planned.conf.win32 $(PEGASUS_HOME)/cimserver_planned.conf
else
	$(COPY) cimserver_planned.conf $(PEGASUS_HOME)/cimserver_planned.conf
	@$(ECHO) user is $(CURRENT_USER)
	sed 's/%CURRENT_USER%/'$(CURRENT_USER)'/' $(PEGASUS_HOME)/cimserver_planned.conf > $(PEGASUS_HOME)/cimserver_planned.conf.2
	$(MOVE) $(PEGASUS_HOME)/cimserver_planned.conf.2 $(PEGASUS_HOME)/cimserver_planned.conf
endif

	@$(ECHO) Starting cimserver
	$(MAKE) --directory=$(PEGASUS_ROOT) -f TestMakefile cimstart
	@$(ECHO) Running
	$(PEGASUS_HOME)/bin/cimconfig -g sslTrustStore
	$(PEGASUS_HOME)/bin/cimconfig -g sslTrustStoreUserName
	$(PEGASUS_HOME)/bin/cimconfig -g sslClientVerificationMode
	$(PEGASUS_HOME)/bin/$(PROGRAM) $(PEGASUS_HOME)/client_cert.pem $(PEGASUS_HOME)/client_file.pem $(PEGASUS_HOME)/ssl.rnd
	@$(ECHO) Stopping cimserver
	$(MAKE) --directory=$(PEGASUS_ROOT) -f TestMakefile cimstop
	@$(ECHO) Stopped.
	$(COPY) $(PEGASUS_HOME)/cimserver_current.conf.bkup $(PEGASUS_HOME)/cimserver_planned.conf
	@$(ECHO) Starting cimserver with previous configuration
	$(MAKE) --directory=$(PEGASUS_ROOT) -f TestMakefile cimstart
	@$(ECHO) Running.
	@$(ECHO) ++++MethodTestProviderSSL test completed.

