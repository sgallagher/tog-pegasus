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

OPENSSL_CNF_SERVER = openssl_server.cnf
OPENSSL_CNF_CLIENT = openssl_client.cnf

ifeq ($(OS_TYPE),windows)
    CURRENT_USER=Administrator
else
    CURRENT_USER=`whoami`
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
	@$(RM) $(PEGASUS_HOME)/testSSLClient_Cert.pem
	@$(RM) $(PEGASUS_HOME)/testSSLClient_Key.pem
	@$(RM) $(PEGASUS_HOME)/ssltest_setup

tests:

create_Client_Certificates: $(OPENSSL_CNF_CLIENT) testsslclient.txt
	@$(MAKE) -i -s cleanup
	@$(TOUCH) $(PEGASUS_HOME)/ssltest_setup

        # Create a self signed Client Certificate
	@$(OPENSSL_COMMAND) genrsa -out $(PEGASUS_HOME)/testSSLClient_Key.pem 1024
	@$(OPENSSL_COMMAND) req -config $(OPENSSL_CNF_CLIENT) -new -key $(PEGASUS_HOME)/testSSLClient_Key.pem -out $(PEGASUS_HOME)/testsslclient.csr < testsslclient.txt
	@$(OPENSSL_COMMAND) x509 -in $(PEGASUS_HOME)/testsslclient.csr -out $(PEGASUS_HOME)/testSSLClient_Cert.pem -req -signkey $(PEGASUS_HOME)/testSSLClient_Key.pem $(SETSERIAL_0) -days 356
	@$(RM) $(PEGASUS_HOME)/testsslclient.csr

create_Server_Certificates: $(OPENSSL_CNF_SERVER) testsslserver.txt
	@$(TOUCH) $(PEGASUS_HOME)/ssltest_setup

        # Create a self Signed Server Certificate
	@$(OPENSSL_COMMAND) genrsa -out $(PEGASUS_HOME)/testsslserver.key 1024
	@$(OPENSSL_COMMAND) req -config $(OPENSSL_CNF_SERVER) -new -key $(PEGASUS_HOME)/testsslserver.key -out $(PEGASUS_HOME)/testsslserver.csr < testsslserver.txt
	@$(OPENSSL_COMMAND) x509 -in $(PEGASUS_HOME)/testsslserver.csr -out $(PEGASUS_HOME)/testsslserver.cert -req -signkey $(PEGASUS_HOME)/testsslserver.key $(SETSERIAL_0) -days 356
	@$(RM) $(PEGASUS_HOME)/testsslserver.csr

ifndef PEGASUS_PAM_AUTHENTICATION
    ifneq ($(OS), zos)
	@cimuser -r -u $(CURRENT_USER)
	@cimuser -a -u $(CURRENT_USER) -w $(CURRENT_USER)
    endif
endif

create_trustStore:
        # Server TrustStore
	cimtrust -a -Ts -U $(CURRENT_USER) -f $(PEGASUS_HOME)/testSSLClient_Cert.pem

        # Client TrustStore
	@$(MOVE) $(PEGASUS_HOME)/client.pem $(PEGASUS_HOME)/client_org.pem
	@$(COPY) $(PEGASUS_HOME)/testsslserver.cert $(PEGASUS_HOME)/client.pem

poststarttests: create_Client_Certificates create_Server_Certificates create_trustStore
	$(TOUCH) $(PEGASUS_HOME)/cimserver_current.conf
	$(COPY) $(PEGASUS_HOME)/cimserver_current.conf $(PEGASUS_HOME)/cimserver_current.conf.bkup
	@$(ECHO) Stopping cimserver
	$(MAKE) --directory=$(PEGASUS_ROOT) -f TestMakefile cimstop
	@$(ECHO) Stopped.

	@$(MOVE) $(PEGASUS_HOME)/server.pem $(PEGASUS_HOME)/server_org.pem
	@$(MOVE) $(PEGASUS_HOME)/file.pem $(PEGASUS_HOME)/file_org.pem
	@$(MOVE) $(PEGASUS_HOME)/testsslserver.key $(PEGASUS_HOME)/file.pem
	@$(MOVE) $(PEGASUS_HOME)/testsslserver.cert $(PEGASUS_HOME)/server.pem

	@chmod 700 $(PEGASUS_HOME)/file.pem
	@chmod 755 $(PEGASUS_HOME)/server.pem
	@chmod 755 $(PEGASUS_HOME)/client.pem

ifeq ($(OS_TYPE),windows)
	$(COPY) cimserver_planned.conf.win32 $(PEGASUS_HOME)/cimserver_planned.conf
else
	$(COPY) cimserver_planned.conf $(PEGASUS_HOME)/cimserver_planned.conf
	@$(ECHO) user is $(CURRENT_USER)
	sed 's/%CURRENT_USER%/'$(CURRENT_USER)'/' $(PEGASUS_HOME)/cimserver_planned.conf > $(PEGASUS_HOME)/cimserver_planned.conf.2
	$(MOVE) $(PEGASUS_HOME)/cimserver_planned.conf.2 $(PEGASUS_HOME)/cimserver_planned.conf
endif

	@$(ECHO) Starting cimserver with new configurations for SSL
	$(MAKE) --directory=$(PEGASUS_ROOT) -f TestMakefile cimstart
	@$(ECHO) Running
	$(PEGASUS_HOME)/bin/cimconfig -g sslTrustStore
	$(PEGASUS_HOME)/bin/cimconfig -g sslTrustStoreUserName
	$(PEGASUS_HOME)/bin/cimconfig -g sslClientVerificationMode

	$(PEGASUS_HOME)/bin/$(PROGRAM) $(PEGASUS_HOME)/testSSLClient_Cert.pem $(PEGASUS_HOME)/testSSLClient_Key.pem $(PEGASUS_HOME)/ssl.rnd

	@$(ECHO) Stopping cimserver
	$(MAKE) --directory=$(PEGASUS_ROOT) -f TestMakefile cimstop
	@$(ECHO) Stopped.
	$(COPY) $(PEGASUS_HOME)/cimserver_current.conf.bkup $(PEGASUS_HOME)/cimserver_planned.conf

	@$(MOVE) $(PEGASUS_HOME)/server_org.pem $(PEGASUS_HOME)/server.pem
	@$(MOVE) $(PEGASUS_HOME)/file_org.pem $(PEGASUS_HOME)/file.pem
	@$(MOVE) $(PEGASUS_HOME)/client_org.pem $(PEGASUS_HOME)/client.pem

	@$(ECHO) Starting cimserver with previous configuration
	$(MAKE) --directory=$(PEGASUS_ROOT) -f TestMakefile cimstart
	@$(ECHO) Running.
	@$(MAKE) -i -s cleanup

