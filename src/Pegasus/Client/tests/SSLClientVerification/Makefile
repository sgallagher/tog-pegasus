ROOT = ../../../../..
DIR = Pegasus/Client/tests/SSLClientVerification
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = TestSSLClientVerification

SOURCES = SSLClientVerification.cpp


# for testing purposes, the username can be anything, as long as its a valid username
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
CURRENT_USER=Administrator
else	
CURRENT_USER=`whoami`
endif

include $(ROOT)/mak/program.mak
include $(ROOT)/mak/commands.mak

tests:

clean:

poststarttests:
ifdef PEGASUS_HAS_SSL
	@$(ECHO) ++++SSLClientVerification Test
	$(TOUCH) $(PEGASUS_HOME)/cimserver_current.conf
	$(COPY) $(PEGASUS_HOME)/cimserver_current.conf $(PEGASUS_HOME)/cimserver_current.conf.bkup
	@$(ECHO) Stopping cimserver
	cd $(PEGASUS_HOME)/bin && $(CIMSERVER_STOP_SERVICE)
	@$(ECHO) Stopped.
	@$(ECHO) Creating truststore directory
	$(MKDIRHIER) $(PEGASUS_HOME)/truststore
	$(COPY) client_cert.pem $(PEGASUS_HOME)/truststore/a692aa3b.0
	$(MKDIRHIER) $(PEGASUS_HOME)/clientkeystore
	$(COPY) client_key.pem $(PEGASUS_HOME)/clientkeystore/client_key.pem
	$(COPY) client_cert.pem $(PEGASUS_HOME)/clientkeystore/client_cert.pem
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	$(COPY) cimserver_planned.conf.win32 $(PEGASUS_HOME)/cimserver_planned.conf
else
	$(COPY) cimserver_planned.conf $(PEGASUS_HOME)/cimserver_planned.conf
	@$(ECHO) user is $(CURRENT_USER)
	sed 's/%CURRENT_USER%/'$(CURRENT_USER)'/' $(PEGASUS_HOME)/cimserver_planned.conf > $(PEGASUS_HOME)/cimserver_planned.conf.2
	$(MOVE) $(PEGASUS_HOME)/cimserver_planned.conf.2 $(PEGASUS_HOME)/cimserver_planned.conf
endif
	@$(ECHO) Starting cimserver
	cd $(PEGASUS_HOME)/bin && $(CIMSERVER_START_SERVICE)
	@$(ECHO) Running
	$(PEGASUS_HOME)/bin/cimconfig -g sslTrustStore
	$(PEGASUS_HOME)/bin/cimconfig -g sslTrustStoreUserName
	$(PEGASUS_HOME)/bin/cimconfig -g sslClientVerificationMode
	$(PEGASUS_HOME)/bin/$(PROGRAM)
	@$(ECHO) Stopping cimserver
	cd $(PEGASUS_HOME)/bin && $(CIMSERVER_STOP_SERVICE)
	@$(ECHO) Stopped.
	$(COPY) $(PEGASUS_HOME)/cimserver_current.conf.bkup $(PEGASUS_HOME)/cimserver_planned.conf
	$(RMDIRHIER) $(PEGASUS_HOME)/truststore
	$(RMDIRHIER) $(PEGASUS_HOME)/clientkeystore
	@$(ECHO) Starting cimserver with previous configuration
	cd $(PEGASUS_HOME)/bin && $(CIMSERVER_START_SERVICE)
	@$(ECHO) Running.
	@$(ECHO) ++++SSLClientVerification test completed.
endif
	
install:
